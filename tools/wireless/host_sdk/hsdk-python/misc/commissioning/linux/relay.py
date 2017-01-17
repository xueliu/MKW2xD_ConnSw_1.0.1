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

from binascii import hexlify
from com.nxp.wireless_connectivity.commands.thread.sync_requests import THR_FactoryReset
import commands
import socket
from threading import Event, Thread
from time import sleep

from CC_Helper import ThreadDevice
from VTun import VTunInterface


SO_BINDTODEVICE = 25

LINUX_PORT = None  # TBD in prepare()
THREAD_IP = 'fd01::1'
THREAD_PORT = 49191  # Port abbreviation :MC
COMM_IP = None  # TBD in proxy_from_commissioner()
COMM_PORT = None  # TBD in proxy_from_commissioner()

RUN_EVENT = Event()
RUN_EVENT.set()


def prepare(port, netif):
    '''
    Configures the mDNS service to be advertised. The Network Name and XPANID
    (Extended PANID) of the network MUST be provided as a name/value pair in
    the TXT record so the PSKc can be correctly derived by the Commissioner.
    @param port: the port from which to retrieve the network name and
    extended PAN ID
    '''
    global LINUX_PORT, args

    router = ThreadDevice(port)

    with open('thread.service', 'r') as service_file:
        lines = service_file.readlines()

    for i in range(len(lines)):
        if lines[i].lstrip().startswith('<txt-record>'):
            if 'nn=' in lines[i]:
                net_name = router.getNetworkName()
                lines[i] = lines[i].replace('nn=', 'nn=' + net_name)
            elif 'xp=' in lines[i]:
                xpanid = router.getXPanId()
                lines[i] = lines[i].replace('xp=', 'xp=' + xpanid.upper())
            elif 'vn=' in lines[i]:
                vendor_name = router.getVendorName()
                lines[i] = lines[i].replace('vn=', 'vn=' + vendor_name)
            elif 'mn=' in lines[i]:
                vendor_model = router.getVendorModel()
                lines[i] = lines[i].replace('mn=', 'mn=' + vendor_model)
            elif 'br_ipv6=' in lines[i]:
                output = commands.getoutput('ip address show dev ' + netif).split()
                try:
                    br_ipv6 = output[output.index('inet6') + 1].split('/')[0]
                    lines[i] = lines[i].replace('br_ipv6=', 'br_ipv6=' + br_ipv6)
                except ValueError:
                    print '[WARNING] No IPv6 addresses detected on interface %s.' % netif
                    if args.ipv6:
                        print '[WARNING] Argument -6/--ipv6 is discarded. Falling back to IPv4.'
                        args.ipv6 = False

        if lines[i].lstrip().startswith('<port>'):
            LINUX_PORT = int(lines[i].replace('port', '').replace('<', '').replace('>', '').replace('/', ''))

    with open('/etc/avahi/services/thread.service', 'w+') as avahi_file:
        for line in lines:
            avahi_file.write(line)

    print commands.getoutput('ip link delete dev fslthr0')
    print commands.getoutput('bash make_tun.sh')
    print commands.getoutput('/etc/init.d/avahi-daemon restart')


def start_router(port, channel):
    '''
    Configures and starts the Thread stack on the specified tty port.
    @param port: the Linux character device assigned to the Thread device
    e.g /dev/ttyACM0
    '''
    router = ThreadDevice(port)
    router.subscribe_to_async_thread_events()

    # Configure
    router.setChannel(channel)
    router.setExtendedAddress(0x146E0A000000000B)
    router.setDevIsPreconfigured(True)

    # Start
    router.start(wait_time=5)


def start_vtun(port):
    '''
    Starts a thread responsible for sending/receiving data from/to the TUN
    interface.
    @param port: the Linux character device assigned to the Thread device
    e.g /dev/ttyACM0
    '''
    vtun_thread = VTunInterface(port, debug=False)  # set debug=True to see IPv6 packets over TUN/TAP
    vtun_thread.daemon = True
    vtun_thread.start()


def proxy_from_commissioner(src_sock, dst_sock):
    '''
    Method responsible for receiving data from the commissioner and forwarding
    it to the Thread device.
    @param src_sock: receive socket
    @param dst_sock: send socket
    '''
    global COMM_IP, COMM_PORT

    while RUN_EVENT.is_set():
        data, addr = src_sock.recvfrom(1024)
        COMM_IP, COMM_PORT = addr[:2]
        print '[Commissioner -> BR][UDP]', hexlify(data), '\n'
        dst_sock.sendto(data, (THREAD_IP, THREAD_PORT))


def proxy_to_commissioner(src_sock, dst_sock):
    '''
    Method responsible for receiving data from the Thread dongle and forwarding
    it to the commissioner.
    @param src_sock: receive socket
    @param dst_sock: send socket
    '''
    while RUN_EVENT.is_set():
        data, _ = src_sock.recvfrom(1024)
        print '[BR -> Commissioner][UDP]', hexlify(data), '\n'
        dst_sock.sendto(data, (COMM_IP, COMM_PORT))


def usage():
    import argparse

    parser = argparse.ArgumentParser(
        description='Starts a Thread Border Router with Serial TUN capabilities, capable of allowing external commissioners.'
    )
    parser.add_argument(
        'dev',
        metavar='serial_port',
        type=str,
        help='Kinetis-W system device node.',
        nargs='?',
        default='/dev/ttymxc0')
    parser.add_argument(
        'netif',
        metavar='nwk_interface',
        type=str,
        help='The network interface used to communicate with the Commissioner.',
        nargs='?',
        default='eth0')
    parser.add_argument(
        '-r', '--reset',
        help='The device will suffer a factory reset at the beginning of the script. If not, please make sure the device is in a factory reset state.',
        action='store_true')
    parser.add_argument(
        '-6', '--ipv6',
        help='The communication between the Commissioner and the BR is IPv6.',
        action='store_true')
    parser.add_argument(
        '-c', '--channel',
        metavar='802.15.4 RF channel',
        type=int,
        choices=range(11, 26),
        help='RF channel for the Thread network',
        default=11
    )

    args = parser.parse_args()
    print args

    return args

if __name__ == '__main__':

    args = usage()

    # factory reset? configure and start network
    if args.reset:
        confirm = THR_FactoryReset(args.dev)
        if confirm.Status == 'Success':
            print '[THR] Factory Reset            OK!'
        else:
            print '[THR] Factory Reset          FAIL!'

    start_router(args.dev, args.channel)
    prepare(args.dev, args.netif)
    start_vtun(args.dev)

    # create channel to Commissioner
    if args.ipv6:
        external_channel = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    else:
        external_channel = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    external_channel.setsockopt(socket.SOL_SOCKET, SO_BINDTODEVICE, args.netif)
    external_channel.bind(('', LINUX_PORT))

    # create channel to Thread network via Serial TUN
    internal_channel = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    internal_channel.setsockopt(socket.SOL_SOCKET, SO_BINDTODEVICE, 'fslthr0')
    internal_channel.bind(('', LINUX_PORT))

    # start threads for each channel
    from_comm = Thread(target=proxy_from_commissioner, args=(external_channel, internal_channel))
    to_comm = Thread(target=proxy_to_commissioner, args=(internal_channel, external_channel))
    from_comm.daemon = True
    to_comm.daemon = True
    from_comm.start()
    to_comm.start()

    # let the threads do their jobs while main sleeps
    try:
        while 1:
            sleep(.5)
    except KeyboardInterrupt:
        RUN_EVENT.clear()
        external_channel.close()
        internal_channel.close()
