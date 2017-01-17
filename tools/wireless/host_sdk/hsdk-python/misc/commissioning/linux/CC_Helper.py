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

from com.nxp.wireless_connectivity.commands.thread.sync_requests import *  # @UnusedWildImport


class ThreadDevice(object):

    def __init__(self, com_port):
        self.com_port = com_port
        self.PSKc = None
        self.PSKd = None
        sleep(0.5)

    def factoryReset(self):
        confirm = THR_FactoryReset(self.com_port)
        assert confirm.Status == 'Success'

    def subscribe_to_async_thread_events(self):
        subscribe_to_async_thread_events_from(self.com_port)

    def setChannel(self, channel):
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.Channel,
            AttrSize=1,
            AttributeValue=channel,
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def setExtendedAddress(self, addr):
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.IeeeExtendedAddr,
            AttrSize=8,
            AttributeValue=addr
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def getExtendedAddress(self):
        confirm = THR_GetAttr(
            self.com_port,
            InstanceId=0,
            AttributeId=THR_GetAttrRequestAttributeId.IeeeExtendedAddr,
        )
        print ''.join(format(byte, '02x') for byte in confirm.AttributeValue[::-1])

    def setDevIsPreconfigured(self, enable):
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.IsDevCommissioned,
            AttrSize=1,
            AttributeValue=enable
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def start(self, wait_time=0):
        confirm = THR_CreateNwk(self.com_port, InstanceID=0)
        sleep(wait_time)
        assert confirm.Status == 'OK'

    def setPSKd(self, PSKd):
        self.PSKd = PSKd
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.Security_PSKd,
            AttrSize=len(PSKd),
            AttributeValue=bytearray(PSKd)
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def setPSKc(self, PSKc):
        self.PSKd = PSKc
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.Security_PSKc,
            AttrSize=len(PSKc),
            AttributeValue=bytearray(PSKc)
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def verifyPSKd(self):
        confirm = THR_GetAttr(
            self.com_port,
            InstanceId=0,
            AttributeId=THR_GetAttrRequestAttributeId.Security_PSKd,
        )
        assert ''.join([chr(byte) for byte in confirm.AttributeValue][::-1]) == self.PSKd

    def getNetworkName(self):
        response = THR_GetAttr(
            self.com_port,
            AttributeId=THR_GetAttrRequestAttributeId.NwkName
        )
        sleep(0.5)
        return ''.join([chr(x) for x in response.AttributeValue[::-1]])

    def setXPanId(self, xpanid):
        confirm = THR_SetAttr(
            self.com_port,
            AttributeId=THR_SetAttrRequestAttributeId.ExtendedPanId,
            AttrSize=8,
            AttributeValue=xpanid
        )
        sleep(0.5)
        assert confirm.Status == 'Success'

    def getXPanId(self):
        response = THR_GetAttr(
            self.com_port,
            AttributeId=THR_GetAttrRequestAttributeId.ExtendedPanId
        )
        sleep(0.5)
        return ''.join(['%02x' % x for x in response.AttributeValue[::-1]])

    def getVendorName(self):
        response = THR_GetAttr(
            self.com_port,
            AttributeId=THR_GetAttrRequestAttributeId.VendorName
        )
        sleep(0.5)
        return ''.join([chr(x) for x in response.AttributeValue[::-1]])

    def getVendorModel(self):
        response = THR_GetAttr(
            self.com_port,
            AttributeId=THR_GetAttrRequestAttributeId.ModelName
        )
        sleep(0.5)
        return ''.join([chr(x) for x in response.AttributeValue[::-1]])
