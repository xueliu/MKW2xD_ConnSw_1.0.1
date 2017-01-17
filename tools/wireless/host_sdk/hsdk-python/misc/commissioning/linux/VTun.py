#!/usr/bin/env python
'''
* Copyright 2014-2016 Freescale Semiconductor, Inc.
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
from com.nxp.wireless_connectivity.commands.comm import Comm
from com.nxp.wireless_connectivity.commands.thread.events import SerialTun_IPPacketReceivedConfirmObserver
from com.nxp.wireless_connectivity.commands.thread.frames import SerialTun_IPPacketSendRequest
from com.nxp.wireless_connectivity.commands.thread.spec import Spec
from com.nxp.wireless_connectivity.hsdk.device.device_manager import DeviceManager
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
                self.comm.send(Spec().SerialTun_IPPacketSendRequestFrame, buf_fsci)

            except KeyboardInterrupt:
                self.tun.close()
                break

if __name__ == '__main__':
    vtun_thread = VTunInterface('/dev/ttyACM0')
    vtun_thread.start()
