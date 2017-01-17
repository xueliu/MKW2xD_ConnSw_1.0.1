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

import sys

from com.nxp.wireless_connectivity.commands.thread.enums import THR_GetThreadIpAddrRequestAddressType
from com.nxp.wireless_connectivity.commands.thread.sync_requests import *
from com.nxp.wireless_connectivity.hsdk.utils import list_to_int
from time import sleep


if __name__ == '__main__':

    if len(sys.argv) < 2:
        print 'Usage: # %s DEVICE_PATH' % sys.argv[0]
        sys.exit(1)

    leader = sys.argv[1]
    subscribe_to_async_thread_events_from(leader)

    THR_FactoryReset(leader)

    THR_CreateNwk(leader, InstanceID=0)
    sleep(7)

    response = NWKU_IfconfigAll(leader)
    for interface in response.InterfaceList:
        print interface.__dict__

    response = THR_GetThreadIpAddr(
        leader,
        InstanceId=0,
        AddressType=THR_GetThreadIpAddrRequestAddressType.Link_Local_64
    )
    try:
        print 'LL64 -> %x' % list_to_int(response.AddressList, False)
    except ValueError:
        print 'LL64 -> None'

    response = THR_GetThreadIpAddr(
        leader,
        InstanceId=0,
        AddressType=THR_GetThreadIpAddrRequestAddressType.MLEID
    )
    try:
        print 'MLEID -> %x' % list_to_int(response.AddressList, False)
    except ValueError:
        print 'MLEID -> None'

    response = THR_GetThreadIpAddr(
        leader,
        InstanceId=0,
        AddressType=THR_GetThreadIpAddrRequestAddressType.RLOC
    )
    try:
        print 'RLOC -> %x' % list_to_int(response.AddressList, False)
    except ValueError:
        print 'RLOC -> None'

    response = THR_GetThreadIpAddr(
        leader,
        InstanceId=0,
        AddressType=THR_GetThreadIpAddrRequestAddressType.Global,
        StartIndex=0,
        NoOfElements=1
    )
    try:
        print 'Global -> %x' % list_to_int(response.AddressList, False)
    except ValueError:
        print 'Global -> None'
