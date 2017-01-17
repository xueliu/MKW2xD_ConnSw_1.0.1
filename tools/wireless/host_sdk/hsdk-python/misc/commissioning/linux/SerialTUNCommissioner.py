#!/usr/bin/env python
'''
* Copyright 2015-2016 Freescale Semiconductor, Inc.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'''

from com.nxp.wireless_connectivity.commands.comm import Comm
from com.nxp.wireless_connectivity.commands.thread.sync_requests import *
from com.nxp.wireless_connectivity.hsdk.CUartLibrary import Baudrate
from com.nxp.wireless_connectivity.hsdk.device.device_manager import DeviceManager
from com.nxp.wireless_connectivity.hsdk.framing.fsci_framer import FsciFramer

from binascii import hexlify
import commands
from Queue import Empty
import struct
import threading

from pytun import TunTapDevice, IFF_TUN, IFF_NO_PI


class VTunInterface(threading.Thread):

    '''
    Handles the sending/receiving of data over the fslthr0 interface.
    '''

    def __init__(self, port, debug=False):
        threading.Thread.__init__(self)

        self.port = port
        self.debug = debug

        # get an object pointing to the TUN device
        self.tun = TunTapDevice(name='fslthr0', flags=IFF_TUN | IFF_NO_PI)
        self.tun.up()

        # open FSL device
        self.comm = Comm(self.port)

        observer = SerialTun_IPPacketReceivedConfirmObserver('RX')
        self.comm.fsciFramer.addObserver(observer, self.event_callback)

    def event_callback(self, devName, event):
        '''
        Gets executed by libframer.so whenever the board sends VTun packets back.
        @param event: VTUNIPPacketReceivedConfirm object, IPpayload of interest
        '''
        # concatenate bytes into a string
        response = ''.join([chr(x) for x in event.IPpayload])
        # send the response on the tun interface
        if self.debug:
            print 'RX: ' + hexlify(response) + '\n'
        self.tun.write(response)

    def run(self):

        while 1:
            try:
                # read data from the TUN interface
                buf = self.tun.read(self.tun.mtu)
                if self.debug:
                    print 'TX: ' + hexlify(buf)

                # send it to the Kinetis-W device, FSCI encapsulated
                buf_fsci = SerialTun_IPPacketSendRequest(
                    Size=bytearray([len(buf) & 0x00FF, (len(buf) & 0xFF00) >> 8]),
                    Data=bytearray(buf)
                )
                self.comm.send(Spec.SerialTun_IPPacketSendRequestFrame, buf_fsci)

            except KeyboardInterrupt:
                self.tun.close()
                break


class ThreadBorderRouter():

    JOINER_EUI = 0xFFFFFFFFFFFFFFFF  # Allow all joiners for now
    JOINER_PSKD = 'kinetis'

    def __init__(self, tty_port):
        self.tty = tty_port
        self.subscribe_to_async_events()

    def factory_reset(self):
        confirm = None
        confirm = THR_FactoryReset(self.tty)
        if confirm.Status != 'Success':
            print '[FAIL] Factory Reset'
            return False
        return True

    def is_connected(self):
        confirm = THR_GetAttr(
            self.tty,
            AttributeId=THR_GetAttrRequestAttributeId.IsDevConnected,
        )
        if confirm.Status != 'Success':
            print '[FAIL] THR_GetAttr IsDevConnected'
            return False

        return True if bool(confirm.AttributeValue) else False

    def get_channel(self):
        confirm = THR_GetAttr(
            self.tty,
            AttributeId=THR_GetAttrRequestAttributeId.Channel,
        )
        if confirm.Status != 'Success':
            print '[FAIL] THR_GetAttr Channel'
            return 0

        return confirm.AttributeValue


    def set_channel(self, channel):
        confirm = THR_SetAttr(
            self.tty,
            AttributeId=THR_SetAttrRequestAttributeId.Channel,
            AttrSize=1,
            AttributeValue=int(channel)
        )
        if confirm.Status != 'Success':
            print '[FAIL] THR_SetAttr Channel'
            return False
        return True

    def set_eui_addr(self, eui):
        confirm = THR_SetAttr(
            self.tty,
            AttributeId=THR_SetAttrRequestAttributeId.IeeeExtendedAddr,
            AttrSize=8,
            AttributeValue=eui
        )
        if confirm.Status != 'Success':
            print '[FAIL] THR_SetAttr IeeeExtendedAddr'
            return False
        return True

    def subscribe_to_async_events(self):
        subscribe_to_async_thread_events_from(self.tty)

    def configure(self, eui_addr, channel, reset):
        if reset:
            assert self.factory_reset(), 'Factory Reset failed'
        assert self.set_eui_addr(eui_addr), 'Set Extended Address failed'
        assert self.set_channel(channel), 'Set Channel failed'

    def start_commissioner(self):
        confirm = THR_CreateNwk(self.tty, InstanceID=0)
        if confirm.Status != 'OK':
            return False

        # expect async events: watch console
        sleep(7)

        confirm = MESHCOP_StartCommissioner(self.tty, InstanceId=0)
        if confirm.Status != 'Success':
            return False

        # removing all the obsolete joiners, if any
        confirm = MESHCOP_RemoveAllExpectedJoiners(self.tty, InstanceId=0)
        if confirm.Status != 'Success':
            return False

        return True

    def allow_all_joiners(self, joiner_pskd):
        confirm = MESHCOP_AddExpectedJoiner(
            self.tty,
            Selected=True,
            EuiType=MESHCOP_AddExpectedJoinerRequestEuiType.LongEUI,
            EUI=ThreadBorderRouter.JOINER_EUI,
            PSKdSize=len(joiner_pskd),
            PSKd=joiner_pskd
        )
        if confirm.Status != 'Success':
            return False

        confirm = MESHCOP_SyncSteeringData(
            self.tty,
            InstanceId=0,
            EuiMask=MESHCOP_SyncSteeringDataRequestEuiMask.AllFFs
        )
        if confirm.Status != 'Success':
            return False

        return True

    def validate_joiner_accepted(self):
        while True:
            # we keep consuming async events until the Joiner confirmation occurs
            try:
                event = FsciFramer(self.tty, ack_policy=FsciAckPolicy.GLOBAL, protocol=Protocol.Thread, baudrate=Baudrate.BR115200).async_queue.get(block=True, timeout=1)
            except Empty:
                continue
            except KeyboardInterrupt:
                print 'Exiting at user request..'
                sys.exit()
            if isinstance(event, THR_EventNwkCommissioningIndication) and event.EventStatus == 'CommissionerJoinerAccepted':
                break
        # If the number of active Routers on the Thread Network partition, as reported in the
        # Route64 TLV of neighboring nodes, is less than ROUTER_UPGRADE_THRESHOLD, a REED MUST wait
        # a random period between 0 and ROUTER_SELECTION_JITTER (120 seconds) [...] and then [...]
        # MUST attempt to become an active Router by requesting a Router ID from the Leader.
        print '\nWaiting 120 seconds for the Joiner to promote to Router for it to be available in our neighbor table..\n'
        sleep(120)

    def add_prefix(self):
        confirm = THR_BrPrefixAddEntry(
            self.tty,
            InstanceId=0,
            prefixLength=64,
            PrefixValue=[0xFD, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xAD] + 8 * [0x00],
            PrefixFlagsReserved=0x00,
            PrefixFlags=0x32,
            prefixLifetime=0xFFFFFFFF,
            prefixAdvertised=True,
            ExternalRouteFlags=0x00,
            ExternalRouteLifetime=0xFFFFFFFF,
            ExternalRouteAdvertised=True
        )
        if confirm.Status != 'Success':
            return False

        confirm = THR_BrPrefixSync(self.tty, InstanceId=0)
        if confirm.Status != 'Success':
            return False

        return True

    def expect_joiners(self, validate=False):

        ULAs = []
        if validate:
            self.validate_joiner_accepted()

        table = THR_GetNeighborTable(self.tty, NoOfElements=10)
        for entry in table.NeighborEntries:
            raw_ext_addr = struct.unpack('8B', struct.pack('>Q', entry.ExtendedAddress))
            dest_ll = [0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00] + [raw_ext_addr[0] ^ 0x02] + list(raw_ext_addr[1:])

            diag_response = THR_MgmtDiagnosticGet(
                self.tty,
                DestIpAddr=dest_ll,
                NumberOfTlvIds=1,
                TlvIds=[THR_MgmtDiagnosticGetRequestTlvId.Ip6AddrList],
            )

            try:
                Ip6AddrList = diag_response.TLVs[0].raw_value
                ULAs.append(':'.join([('%02x' % Ip6AddrList[i] + '%02x' % Ip6AddrList[i + 1]) for i in range(48, 64, 2)]))
            except AttributeError:
                print '\nNeighbor', ':'.join([('%02x' % dest_ll[i] + '%02x' % dest_ll[i + 1]) for i in range(0, 16, 2)]), 'expired!\n'

        print
        for i in range(len(ULAs)):
            print 'ULA NEIGHBOR' + str(i), '->', ULAs[i]

        self.expect_joiners(validate=True)


def usage():
    import argparse

    parser = argparse.ArgumentParser(
        description='Starts a Thread Border Router with Serial TUN capabilities.')
    parser.add_argument('dev', metavar='serial port', type=str, nargs='?',
                        help='Kinetis-W system device node', default='/dev/ttymxc1')
    parser.add_argument('channel', metavar='802.15.4 RF channel', type=int, choices=range(
        11, 26), nargs='?', help='RF channel for the Thread network', default=25)
    parser.add_argument(
        '-r', '--reset', help='Perform a factory reset of the device at start', action='store_true')

    args = parser.parse_args()
    print args

    return args


if __name__ == '__main__':

    args = usage()

    print commands.getoutput('bash make_tun.sh')

    leader = ThreadBorderRouter(args.dev)

    # don't allow channel change if not --reset and device is already connected
    if not args.reset:
        # retrieve current device state
        current_channel = leader.get_channel()
        dev_connected = leader.is_connected()
        # exit on bad condition
        if dev_connected and (current_channel != args.channel):
            print '%s: error: Changing the channel while the Thread network is created is forbidden.' % sys.argv[0]
            print '%s: error: The Thread network is currently running on channel %d.' % (sys.argv[0], current_channel)
            print '%s: error: Please use -r/--reset to switch to the desired channel.' % sys.argv[0]
            sys.exit(1)

    # configure device if user requests reset or if the device is not connected
    if args.reset or not dev_connected:
        leader.configure(eui_addr=0x146E0A0000000001, channel=args.channel, reset=args.reset)
        assert leader.start_commissioner(), 'Start Commissioner failed'
        assert leader.allow_all_joiners(joiner_pskd='kinetis'), 'Allow Joiners into Network failed'
        assert leader.add_prefix(), 'Border Router Prefix Add failed'

    vtun_thread = VTunInterface(args.dev, debug=True)
    vtun_thread.daemon = True
    vtun_thread.start()

    print 'Everything is set up. Expecting Joiners now.'
    # if reset is False, we do not validate the already connected Joiners
    leader.expect_joiners(args.reset)
