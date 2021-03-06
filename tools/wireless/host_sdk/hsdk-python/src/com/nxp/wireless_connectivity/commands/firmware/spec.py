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

from com.nxp.wireless_connectivity.commands.fsci_frame_description import FsciFrameDescription
from com.nxp.wireless_connectivity.commands.fsci_parameter import FsciParameter
from com.nxp.wireless_connectivity.hsdk.singleton import singleton


@singleton
class Spec(object):

    def __init__(self):
        self.FSCIEnterBootloaderRequestFrame = self.InitFSCIEnterBootloaderRequest()
        self.FSCICPUResetRequestFrame = self.InitFSCICPUResetRequest()
        self.FSCIFirmware_StartImageRequestFrame = self.InitFSCIFirmware_StartImageRequest()
        self.FSCIFirmware_SetOperationModeRequestFrame = self.InitFSCIFirmware_SetOperationModeRequest()
        self.FSCIFirmware_QueryImageRspRequestFrame = self.InitFSCIFirmware_QueryImageRspRequest()
        self.FSCIFirmware_PushImageChunkRequestFrame = self.InitFSCIFirmware_PushImageChunkRequest()
        self.FSCIFirmware_CancelProcessRequestFrame = self.InitFSCIFirmware_CancelProcessRequest()
        self.FSCIFirmware_CommitImageRequestFrame = self.InitFSCIFirmware_CommitImageRequest()
        self.FSCIFirmware_ImageNotifyRequestFrame = self.InitFSCIFirmware_ImageNotifyRequest()
        self.FSCIFirmware_SetFileVersionPolicyRequestFrame = self.InitFSCIFirmware_SetFileVersionPolicyRequest()
        self.FSCIFirmware_AbortRequestFrame = self.InitFSCIFirmware_AbortRequest()
        self.FSCIFirmware_QueryImageIndicationFrame = self.InitFSCIFirmware_QueryImageIndication()
        self.FSCIFirmware_ChunkIndicationFrame = self.InitFSCIFirmware_ChunkIndication()
        self.FSCIFirmware_StartImageConfirmFrame = self.InitFSCIFirmware_StartImageConfirm()
        self.FSCIFirmware_SetOperationModeConfirmFrame = self.InitFSCIFirmware_SetOperationModeConfirm()
        self.FSCIFirmware_QueryImageRspConfirmFrame = self.InitFSCIFirmware_QueryImageRspConfirm()
        self.FSCIFirmware_PushImageChunkConfirmFrame = self.InitFSCIFirmware_PushImageChunkConfirm()
        self.FSCIFirmware_CancelProcessConfirmFrame = self.InitFSCIFirmware_CancelProcessConfirm()
        self.FSCIFirmware_CommitImageConfirmFrame = self.InitFSCIFirmware_CommitImageConfirm()
        self.FSCIFirmware_ImageNotifyConfirmFrame = self.InitFSCIFirmware_ImageNotifyConfirm()
        self.FSCIFirmware_SetFileVersionPolicyConfirmFrame = self.InitFSCIFirmware_SetFileVersionPolicyConfirm()

    def InitFSCIEnterBootloaderRequest(self):
        cmdParams = []
        return FsciFrameDescription(0xA3, 0xCF, cmdParams)

    def InitFSCICPUResetRequest(self):
        cmdParams = []
        return FsciFrameDescription(0xA3, 0x08, cmdParams)

    def InitFSCIFirmware_StartImageRequest(self):
        cmdParams = []
        ImageSize = FsciParameter("ImageSize", 4)
        cmdParams.append(ImageSize)
        return FsciFrameDescription(0xA3, 0x29, cmdParams)

    def InitFSCIFirmware_SetOperationModeRequest(self):
        cmdParams = []
        OperationMode = FsciParameter("OperationMode", 1)
        cmdParams.append(OperationMode)
        return FsciFrameDescription(0xA3, 0x28, cmdParams)

    def InitFSCIFirmware_QueryImageRspRequest(self):
        cmdParams = []
        OtaImageStatus = FsciParameter("OtaImageStatus", 1)
        cmdParams.append(OtaImageStatus)
        DeviceId = FsciParameter("DeviceId", 2)
        cmdParams.append(DeviceId)
        ManufacturerCode = FsciParameter("ManufacturerCode", 2)
        cmdParams.append(ManufacturerCode)
        ImageType = FsciParameter("ImageType", 2)
        cmdParams.append(ImageType)
        FileVersion = FsciParameter("FileVersion", 4)
        cmdParams.append(FileVersion)
        ImageSize = FsciParameter("ImageSize", 4)
        cmdParams.append(ImageSize)
        return FsciFrameDescription(0xA3, 0xC3, cmdParams)

    def InitFSCIFirmware_PushImageChunkRequest(self):
        cmdParams = []
        DataImageBlock = FsciParameter("DataImageBlock", 150)
        cmdParams.append(DataImageBlock)
        return FsciFrameDescription(0xA3, 0x2A, cmdParams)

    def InitFSCIFirmware_CancelProcessRequest(self):
        cmdParams = []
        return FsciFrameDescription(0xA3, 0x2C, cmdParams)

    def InitFSCIFirmware_CommitImageRequest(self):
        cmdParams = []
        CRCval = FsciParameter("CRCval", 2)
        cmdParams.append(CRCval)
        return FsciFrameDescription(0xA3, 0x2B, cmdParams)

    def InitFSCIFirmware_ImageNotifyRequest(self):
        cmdParams = []
        DeviceId = FsciParameter("DeviceId", 2)
        cmdParams.append(DeviceId)
        ManufacturerCode = FsciParameter("ManufacturerCode", 2)
        cmdParams.append(ManufacturerCode)
        ImageType = FsciParameter("ImageType", 2)
        cmdParams.append(ImageType)
        ImageSize = FsciParameter("ImageSize", 4)
        cmdParams.append(ImageSize)
        FileSize = FsciParameter("FileSize", 4)
        cmdParams.append(FileSize)
        FileVersion = FsciParameter("FileVersion", 4)
        cmdParams.append(FileVersion)
        return FsciFrameDescription(0xA3, 0xC4, cmdParams)

    def InitFSCIFirmware_SetFileVersionPolicyRequest(self):
        cmdParams = []
        FileVersionPolicy = FsciParameter("FileVersionPolicy", 1)
        cmdParams.append(FileVersionPolicy)
        return FsciFrameDescription(0xA3, 0x2D, cmdParams)

    def InitFSCIFirmware_AbortRequest(self):
        cmdParams = []
        DeviceId = FsciParameter("DeviceId", 2)
        cmdParams.append(DeviceId)
        return FsciFrameDescription(0xA3, 0x2E, cmdParams)

    def InitFSCIFirmware_QueryImageIndication(self):
        cmdParams = []
        DeviceId = FsciParameter("DeviceId", 2)
        cmdParams.append(DeviceId)
        ManufacturerCode = FsciParameter("ManufacturerCode", 2)
        cmdParams.append(ManufacturerCode)
        ImageType = FsciParameter("ImageType", 2)
        cmdParams.append(ImageType)
        FileVersion = FsciParameter("FileVersion", 4)
        cmdParams.append(FileVersion)
        return FsciFrameDescription(0xA3, 0xC2, cmdParams)

    def InitFSCIFirmware_ChunkIndication(self):
        cmdParams = []
        DeviceId = FsciParameter("DeviceId", 2)
        cmdParams.append(DeviceId)
        ImageOffset = FsciParameter("ImageOffset", 4)
        cmdParams.append(ImageOffset)
        BlockLength = FsciParameter("BlockLength", 1)
        cmdParams.append(BlockLength)
        return FsciFrameDescription(0xA3, 0x2F, cmdParams)

    def InitFSCIFirmware_StartImageConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        ProtocolVersion = FsciParameter("ProtocolVersion", 1)
        cmdParams.append(ProtocolVersion)
        ExternalMemorySupported = FsciParameter("ExternalMemorySupported", 1)
        cmdParams.append(ExternalMemorySupported)
        return FsciFrameDescription(0xA4, 0x29, cmdParams)

    def InitFSCIFirmware_SetOperationModeConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0x28, cmdParams)

    def InitFSCIFirmware_QueryImageRspConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0xC3, cmdParams)

    def InitFSCIFirmware_PushImageChunkConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0x2A, cmdParams)

    def InitFSCIFirmware_CancelProcessConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0x2C, cmdParams)

    def InitFSCIFirmware_CommitImageConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0x2B, cmdParams)

    def InitFSCIFirmware_ImageNotifyConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0xC4, cmdParams)

    def InitFSCIFirmware_SetFileVersionPolicyConfirm(self):
        cmdParams = []
        Status = FsciParameter("Status", 1)
        cmdParams.append(Status)
        return FsciFrameDescription(0xA4, 0x2D, cmdParams)
