#!/usr/bin/env python
'''
* Copyright 2016 Freescale Semiconductor, Inc.
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

import sys
sys.path.append('../../src/')

from com.nxp.wireless_connectivity.hsdk.sniffer import Sniffer


def usage():
    import argparse

    parser = argparse.ArgumentParser(
        description='Starts an 802.15.4 sniffer instance on TAP interface sniffer802154.'
    )
    parser.add_argument(
        'dev',
        metavar='serial port',
        type=str,
        nargs='?',
        help='Kinetis-W system device node',
        default='/dev/ttyACM0'
    )
    parser.add_argument(
        'channel',
        metavar='802.15.4 RF channel',
        type=int,
        choices=range(11, 26),
        nargs='?',
        help='RF channel for the Thread network',
        default=25
    )

    args = parser.parse_args()
    print args

    return args

if __name__ == '__main__':

    args = usage()

    s = Sniffer(args.dev, args.channel, use_tap=True)
    s.start()
