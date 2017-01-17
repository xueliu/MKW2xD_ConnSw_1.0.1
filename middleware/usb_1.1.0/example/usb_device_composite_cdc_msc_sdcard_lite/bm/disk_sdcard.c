/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
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
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_msc.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "disk_sdcard.h"

#include "fsl_device_registers.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#include <stdio.h>
#include <stdlib.h>
#if (defined(FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT > 0U))
#include "fsl_mpu.h"
#endif /* FSL_FEATURE_SOC_MPU_COUNT */
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#include "usb_phy.h"
#endif

#include "composite.h"
#include "fsl_port.h"
#include "fsl_sdhc.h"
#include "fsl_card.h"
#include "fsl_gpio.h"
#include "event.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The maximum timeout time for the transfer complete event */
#define EVENT_TIMEOUT_TRANSFER_COMPLETE (1000U)
/*******************************************************************************
  * Prototypes
  ******************************************************************************/
void BOARD_InitHardware(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Data structure of msc device, store the information ,such as class handle */
usb_device_msc_struct_t *g_mscHandle;
static usb_device_composite_struct_t *g_deviceComposite;
uint32_t g_mscReadRequestBuffer[USB_DEVICE_MSC_READ_BUFF_SIZE >> 2];

uint32_t g_mscWriteRequestBuffer[USB_DEVICE_MSC_WRITE_BUFF_SIZE >> 2];

/* Card detect flag. */
volatile uint32_t g_sdInsertedFlag;

/* State in SDHC driver. */
uint32_t g_sdhcAdmaTable[USB_DEVICE_MSC_ADMA_TABLE_WORDS];
sdhc_handle_t g_sdhcHandle;
/* State in Card driver. */
sd_card_t g_sd;
volatile uint32_t g_sdhcTransferFailedFlag; /* SDHC transfer status. 0 means success, 1 means failed. */
sd_card_t *usbDeviceMscCard;
#if ((defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
     (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0)))
usb_msc_buffer_struct_t dataBuffer[USB_DEVICE_MSC_BUFFER_NUMBER];
usb_msc_buffer_struct_t *currentTrasfer;
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/

/* SDHC interrupt handler. */
void SDHC_IRQHandler(void)
{
    SDHC_TransferHandleIRQ(BOARD_SDHC_BASEADDR, &g_sdhcHandle);
}
/* Transfer complete callback function. */
void SDHC_TransferCompleteCallback(SDHC_Type *base, sdhc_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        g_sdhcTransferFailedFlag = 0;
    }
    else
    {
        g_sdhcTransferFailedFlag = 1;
    }

    EVENT_Notify(kEVENT_TransferComplete);
}
/* User defined transfer function. */
status_t USB_DeviceMscSdhcTransfer(SDHC_Type *base, sdhc_transfer_t *content)
{
    status_t error = kStatus_Success;

    do
    {
        error =
            SDHC_TransferNonBlocking(base, &g_sdhcHandle, g_sdhcAdmaTable, USB_DEVICE_MSC_ADMA_TABLE_WORDS, content);
    } while (error == kStatus_SDHC_BusyTransferring);

    if ((error != kStatus_Success) || (false == EVENT_Wait(kEVENT_TransferComplete, EVENT_TIMEOUT_TRANSFER_COMPLETE)) ||
        (g_sdhcTransferFailedFlag))
    {
        error = kStatus_Fail;
    }

    return error;
}
/*!
 * @brief device msc sdhc card detect function.
 *
 * This function check the card detect pin value.
 * @return g_sdInsertedFlag value.
 */
uint32_t USB_DeviceMscSdhcCardDetect(void)
{
    if (GPIO_ReadPinInput(BOARD_SDHC_CD_GPIO_BASE, BOARD_SDHC_CD_GPIO_PIN))
#if defined BOARD_SDHC_CD_LOGIC_RISING
    {
        g_sdInsertedFlag = 1U;
    }
    else
    {
        g_sdInsertedFlag = 0U;
    }
#else
    {
        g_sdInsertedFlag = 0U;
    }
    else
    {
        g_sdInsertedFlag = 1U;
    }
#endif
    return g_sdInsertedFlag;
}
/*!
 * @brief device msc sdhc init function.
 *
 * This function initialize the SDHC module and the card on a specific SDHC.
 * @return kStatus_USB_Success or error.
 */
uint8_t USB_DeviceMscSdhcInit(void)
{
    sdhc_transfer_callback_t g_sdhcCallback;
    sdhc_config_t *g_sdhcConfig;
    SDHC_Type *base;
    uint8_t *temptptr;
    int i;

    g_sdhcTransferFailedFlag = 0U;
    usb_status_t error = kStatus_USB_Success;
    base = BOARD_SDHC_BASEADDR;
    g_sdhcConfig = &(usbDeviceMscCard->host.config);

    NVIC_SetPriority(SDHC_IRQn, (USB_DEVICE_INTERRUPT_PRIORITY - 1U));
    /* Opens SDHC clock gate/NVIC. */
    /* Initializes SDHC. */

    temptptr = (uint8_t *)g_sdhcConfig;
    for (i = 0; i < sizeof(sdhc_config_t); i++)
    {
        temptptr[i] = 0;
    }
    g_sdhcConfig->cardDetectDat3 = false;
    g_sdhcConfig->endianMode = kSDHC_EndianModeLittle;
    g_sdhcConfig->dmaMode = kSDHC_DmaModeAdma2;
    g_sdhcConfig->readWatermarkLevel = 0x80U;
    g_sdhcConfig->writeWatermarkLevel = 0x80U;
    SDHC_Init(base, g_sdhcConfig);

    /* Sets callback in SDHC driver. */
    temptptr = (uint8_t *)&g_sdhcCallback;
    for (i = 0; i < sizeof(g_sdhcCallback); i++)
    {
        temptptr[i] = 0;
    }
    g_sdhcCallback.TransferComplete = SDHC_TransferCompleteCallback;
    /* Creates handle for SDHC driver */
    SDHC_TransferCreateHandle(BOARD_SDHC_BASEADDR, &g_sdhcHandle, &g_sdhcCallback, (void *)&g_sdhcTransferFailedFlag);

    /* Create transfer complete event. */
    if (false == EVENT_Create(kEVENT_TransferComplete))
    {
        return kStatus_USB_Error;
    }
    /* Fills state in card driver. */
    usbDeviceMscCard->host.base = base;
    usbDeviceMscCard->host.sourceClock_Hz = CLOCK_GetFreq(BOARD_SDHC_CLKSRC);
    usbDeviceMscCard->host.transfer = USB_DeviceMscSdhcTransfer;
    /* Init card. */
    if (SD_Init(usbDeviceMscCard))
    {
        PRINTF("\n SD card init failed \n");
        error = kStatus_USB_Error;
    }

    return error;
}
void USB_DeviceMscApp(void)
{
    /*TO DO*/
    /*add user code*/
    return;
}
#if (defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
    (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
/*!
 * @brief msc enter critical.
 *
 * This function is used to enter critical disable interrupt .
 *
 */
static void USB_BmEnterCritical(uint8_t *sr)
{
    *sr = __get_PRIMASK();
    __ASM("CPSID I");
}
/*!
 * @brief msc exit critical.
 *
 * This function is used to exit critical ,enable interrupt .
 *
 */
static void USB_BmExitCritical(uint8_t sr)
{
    __set_PRIMASK(sr);
}
/*!
 * @brief device msc add a buffer to the tail queue function.
 *
 * This function add a buffer which contains data from the host pc to the head queue when receive write response
 * kUSB_DeviceMscEventWriteResponse
 */
void USB_DeviceMscAddBufferToTail(usb_msc_buffer_struct_t *bufferinfo)
{
    usb_msc_buffer_struct_t *temp;
    usb_msc_buffer_struct_t *pre;

    temp = g_deviceComposite->mscDisk.taillist;
    pre = g_deviceComposite->mscDisk.taillist;
    while (temp)
    {
        pre = temp;
        if (temp == bufferinfo)
        {
            return;
        }
        temp = temp->next;
    }
    if (pre)
    {
        pre->next = bufferinfo;
        bufferinfo->next = NULL;
    }
    else
    {
        g_deviceComposite->mscDisk.taillist = bufferinfo;
        bufferinfo->next = NULL;
    }
}
/*!
 * @brief device msc get buffer from tail queue function.
 *
 * This function get a buffer which contains data from the host pc . The write task need get those data in order from
 * the tail queue, and then write those data to sdcard.
 */

void USB_DeviceMscGetBufferFromTail(usb_msc_buffer_struct_t **bufferinfo)
{
    if (g_deviceComposite->mscDisk.taillist != NULL)
    {
        *bufferinfo = g_deviceComposite->mscDisk.taillist;
        g_deviceComposite->mscDisk.taillist = g_deviceComposite->mscDisk.taillist->next;
    }
    else
    {
        *bufferinfo = NULL;
    }
}
/*!
 * @brief device msc add a buffer to the head function.
 *
 * This function add a empty buffer to the head queue, head queue maintain the eampty buffer, when usb stack will add
 * the
 *  buffer to head queue when the data in this buffer has already write into the sdcard.
 */
void USB_DeviceMscAddBufferToHead(usb_msc_buffer_struct_t *bufferinfo)
{
    usb_msc_buffer_struct_t *p;

    p = g_deviceComposite->mscDisk.headlist;
    while (p)
    {
        if (p == bufferinfo)
        {
            return;
        }
        p = p->next;
    }
    if (g_deviceComposite->mscDisk.headlist == NULL)
    {
        g_deviceComposite->mscDisk.headlist = bufferinfo;
    }
    else
    {
        bufferinfo->next = g_deviceComposite->mscDisk.headlist;
        g_deviceComposite->mscDisk.headlist = bufferinfo;
    }
}
/*!
 * @brief device msc get buffer from head function.
 *
 * This function get a empty buffer from the head queue. The stack need get a
 * empty buffer from head queue when receive a write request
 */
void USB_DeviceMscGetBufferFromHead(usb_msc_buffer_struct_t **bufferinfo)
{
    if (g_deviceComposite->mscDisk.headlist != NULL)
    {
        *bufferinfo = g_deviceComposite->mscDisk.headlist;
        g_deviceComposite->mscDisk.headlist = g_deviceComposite->mscDisk.headlist->next;
    }
    else
    {
        *bufferinfo = NULL;
    }
}
/*!
 * @brief device msc write task function.
 *
 * This function wrtie data to the sdcard.
 */
void USB_DeviceMscWriteTask(void)
{
    usb_msc_buffer_struct_t *temp;
    temp = g_deviceComposite->mscDisk.taillist;
    status_t errorCode;
    usb_status_t error = kStatus_USB_Success;
    uint8_t usbOsaCurrentSr;
    USB_BmEnterCritical(&usbOsaCurrentSr);
    USB_DeviceMscGetBufferFromTail(&temp);
    USB_BmExitCritical(usbOsaCurrentSr);
    if (NULL != temp)
    {
        errorCode = SD_WriteBlocks(usbDeviceMscCard, temp->buffer, temp->offset,
                                   temp->size >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER);
        USB_BmEnterCritical(&usbOsaCurrentSr);
        USB_DeviceMscAddBufferToHead(temp);
        USB_BmExitCritical(usbOsaCurrentSr);
        if (kStatus_Success != errorCode)
        {
            g_deviceComposite->mscDisk.readWriteError = 1;
            usb_echo(
                "Write error, error = 0xx%x \t Please check write request buffer size(must be less than 128 "
                "sectors)\r\n",
                error);
            error = kStatus_USB_Error;
        }
    }
}
void USB_DeviceMscInitQueue(void)
{
    uint8_t i;
    g_deviceComposite->mscDisk.headlist = dataBuffer;
    usb_msc_buffer_struct_t *pre;
    usb_msc_buffer_struct_t *temp;
    pre = temp = g_deviceComposite->mscDisk.headlist;
    for (i = 1; i < USB_DEVICE_MSC_BUFFER_NUMBER; i++)
    {
        temp++;
        pre->next = temp;
        pre++;
    }
    pre->next = NULL;
    g_deviceComposite->mscDisk.taillist = NULL;
    g_deviceComposite->mscDisk.transferlist = NULL;
}
#endif

/*!
 * @brief Send data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls USB_DeviceSendRequest internally.
 *
 * @param handle The msc class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the sending request is successful or not; the transfer done is notified by
 * USB_DeviceMscBulkIn.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceMscSend(usb_device_msc_struct_t *mscHandle)
{
    usb_status_t error = kStatus_USB_Success;
    status_t errorCode = kStatus_Success;
    usb_device_lba_app_struct_t lba;

    lba.offset = mscHandle->currentOffset;
    lba.size = (mscHandle->bulkInBufferSize > USB_DEVICE_MSC_MAX_SEND_TRANSFER_LENGTH) ?
                   USB_DEVICE_MSC_MAX_SEND_TRANSFER_LENGTH :
                   mscHandle->bulkInBufferSize;
    lba.size =
        (mscHandle->transferRemaining > lba.size) ? lba.size : mscHandle->transferRemaining; /* whichever is smaller */

    lba.buffer = (uint8_t *)&g_mscReadRequestBuffer[0];

    errorCode = SD_ReadBlocks(usbDeviceMscCard, lba.buffer, lba.offset, lba.size >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER);

    if (kStatus_Success != errorCode)
    {
        g_deviceComposite->mscDisk.readWriteError = 1;
        usb_echo(
            "Read error, error = 0xx%x \t Please check read request buffer size(must be less than 128 sectors)\r\n",
            error);
        error = kStatus_USB_Error;
    }

    if (mscHandle->currentOffset < (mscHandle->totalLogicalBlockNumber))
    {
        error = USB_DeviceSendRequest(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint, lba.buffer, lba.size);
    }
    else
    {
        mscHandle->needInStallFlag = 0;
        mscHandle->inEndpointStallFlag = 1;
        mscHandle->dataInFlag = 0;
        mscHandle->stallStatus = (uint8_t)USB_DEVICE_MSC_STALL_IN_DATA;
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint);
    }
    return error;
}
/*!
 * @brief Receive data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls USB_DeviceRecvRequest internally.
 *
 * @param handle The msc class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the receiving request is successful or not; the transfer done is notified by
 * USB_DeviceMscBulkOut.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceMscRecv(usb_device_msc_struct_t *mscHandle)
{
    usb_status_t error = kStatus_USB_Success;
    usb_device_lba_app_struct_t lba;

    lba.offset = mscHandle->currentOffset;
    lba.size = (mscHandle->bulkOutBufferSize > USB_DEVICE_MSC_MAX_RECV_TRANSFER_LENGTH) ?
                   USB_DEVICE_MSC_MAX_RECV_TRANSFER_LENGTH :
                   mscHandle->bulkOutBufferSize;
    lba.size =
        (mscHandle->transferRemaining > lba.size) ? lba.size : mscHandle->transferRemaining; /* whichever is smaller */

#if (defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
    (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
    usb_msc_buffer_struct_t *tempbuffer;
    USB_DeviceMscGetBufferFromHead(&tempbuffer);
    while (NULL == tempbuffer)
    {
        usb_echo("No buffer available");
        USB_DeviceMscWriteTask();
        USB_DeviceMscGetBufferFromHead(&tempbuffer);
    }
    lba.buffer = tempbuffer->buffer;
    currentTrasfer = tempbuffer;
#else
    lba.buffer = (uint8_t *)&g_mscWriteRequestBuffer[0];
#endif
    if (NULL == lba.buffer)
    {
        usb_echo("No buffer available");
    }
    if (mscHandle->currentOffset < (mscHandle->totalLogicalBlockNumber))
    {
        error =
            USB_DeviceRecvRequest(g_deviceComposite->deviceHandle, mscHandle->bulkOutEndpoint, lba.buffer, lba.size);
    }
    else
    {
        mscHandle->needOutStallFlag = 0;
        mscHandle->outEndpointStallFlag = 1;
        mscHandle->dataOutFlag = 0;
        mscHandle->stallStatus = (uint8_t)USB_DEVICE_MSC_STALL_IN_DATA;
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkOutEndpoint);
    }
    return error;
}
/*!
 * @brief Recv Send data through a specified endpoint.
 *
 * The function is used when ufi process read/write command .
 * The function calls USB_DeviceMscRecv or  usb_deviceSendRecv as the direction internally.
 *
 * @param handle The msc class handle got from usb_device_class_config_struct_t::classHandle.
 * @param direction     Data direction: 0 = Data-Out from host to the device, 1 = Data-In from the device to the host.
 * @param buffer The memory address to hold the data need to be sent.
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the sending or reciving request is successful or not.
 */
usb_status_t USB_DeviceMscLbaTransfer(usb_device_msc_struct_t *mscHandle,
                                      uint8_t direction,
                                      usb_lba_transfer_information_struct_t *lbaInformation)
{
    usb_status_t error = kStatus_USB_Success;

    mscHandle->transferRemaining = lbaInformation->transferNumber * mscHandle->lengthOfEachLba;
    mscHandle->currentOffset = lbaInformation->startingLogicalBlockAddress;

    if (direction == USB_IN)
    {
        error = USB_DeviceMscSend(mscHandle);
    }
    else
    {
        error = USB_DeviceMscRecv(mscHandle);
    }
    return error;
}

/*!
 * @brief Process usb msc ufi command.
 *
 * This function analyse the cbw , get the command code.
 *
 * @param handle          The device msc class hanlde.
 *
 * @retval kStatus_USB_Success              Free device msc class hanlde successfully.
 */
usb_status_t USB_DeviceMscProcessUfiCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_msc_ufi_struct_t *ufi = NULL;
    ufi = &mscHandle->g_mscUfi;

    ufi->requestSense.senseKey = USB_DEVICE_MSC_UFI_NO_SENSE;
    ufi->requestSense.additionalSenseCode = USB_DEVICE_MSC_UFI_NO_SENSE;
    ufi->requestSense.additionalSenseQualifer = USB_DEVICE_MSC_UFI_NO_SENSE;
    ufi->thirteenCase.hostExpectedDataLength = mscHandle->g_mscCbw.dataTransferLength;
    ufi->thirteenCase.hostExpectedDirection =
        (uint8_t)(mscHandle->g_mscCbw.flags >> USB_DEVICE_MSC_CBW_DIRECTION_SHIFT);
    /*The first byte of all ufi command blocks shall contain an Operation Code, refer to ufi spec*/
    switch (mscHandle->g_mscCbw.cbwcb[0])
    {
        /* ufi commmand operation code*/
        case USB_DEVICE_MSC_INQUIRY_COMMAND: /*operation code : 0x12*/
            error = USB_DeviceMscUfiInquiryCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_READ_10_COMMAND: /*operation code : 0x28 */
        case USB_DEVICE_MSC_READ_12_COMMAND: /*operation code : 0xA8 */
            error = USB_DeviceMscUfiReadCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_REQUEST_SENSE_COMMAND: /*operation code : 0x03*/
            error = USB_DeviceMscUfiRequestSenseCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_TEST_UNIT_READY_COMMAND: /*operation code : 0x00 */
            error = USB_DeviceMscUfiTestUnitReadyCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_WRITE_10_COMMAND: /*operation code : 0x2A */
        case USB_DEVICE_MSC_WRITE_12_COMMAND: /*operation code : 0xAA */
            error = USB_DeviceMscUfiWriteCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_PREVENT_ALLOW_MEDIUM_REM_COMMAND: /*operation code :0x1E */
            error = USB_DeviceMscUfiPreventAllowMediumCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_FORMAT_UNIT_COMMAND: /*operation code : 0x04*/
            error = USB_DeviceMscUfiFormatUnitCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_READ_CAPACITY_10_COMMAND: /*operation code : 0x25*/
        case USB_DEVICE_MSC_READ_CAPACITY_16_COMMAND: /*operation code : 0x9E*/
            error = USB_DeviceMscUfiReadCapacityCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_MODE_SENSE_10_COMMAND: /* operation code :0x5A*/
        case USB_DEVICE_MSC_MODE_SENSE_6_COMMAND:  /* operation code : 0x1A */
            error = USB_DeviceMscUfiModeSenseCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_MODE_SELECT_10_COMMAND: /*operation code : 0x55 */
        case USB_DEVICE_MSC_MODE_SELECT_6_COMMAND:  /*operation code : 0x15 */
            error = USB_DeviceMscUfiModeSelectCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_READ_FORMAT_CAPACITIES_COMMAND: /*operation code : 0x23 */
            error = USB_DeviceMscUfiReadFormatCapacityCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_SEND_DIAGNOSTIC_COMMAND: /*operation code : 0x1D*/
            error = USB_DeviceMscUfiSendDiagnosticCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_VERIFY_COMMAND: /*operation code : 0x2F*/
            error = USB_DeviceMscUfiVerifyCommand(mscHandle);
            break;
        case USB_DEVICE_MSC_START_STOP_UNIT_COMMAND: /*operation code : 0x1B*/
            error = USB_DeviceMscUfiStartStopUnitCommand(mscHandle);
            break;
        default:
            error = USB_DeviceMscUfiUnsupportCommand(mscHandle);
            mscHandle->dataOutFlag = 0;
            mscHandle->dataInFlag = 0;
            mscHandle->outEndpointStallFlag = 0;
            mscHandle->inEndpointStallFlag = 0;
            mscHandle->needOutStallFlag = 0;
            mscHandle->needInStallFlag = 0;
            break;
    }
    return error;
}
/*!
 * @brief Bulk IN endpoint callback function.
 *
 * This callback function is used to notify uplayer the tranfser result of a transfer.
 * This callback pointer is passed when the Bulk IN pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param event          The result of the Bulk IN pipe transfer.
 * @param arg             The paramter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the MSC class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscBulkIn(usb_device_handle deviceHandle,
                                 usb_device_endpoint_callback_message_struct_t *event,
                                 void *arg)
{
    usb_device_msc_csw_t *csw;
    usb_status_t error = kStatus_USB_Error;
    usb_device_msc_struct_t *mscHandle = (usb_device_msc_struct_t *)arg;

    if (event->length == USB_UNINITIALIZED_VAL_32)
    {
        return error;
    }
    if (mscHandle->transferRemaining >= event->length)
    {
        mscHandle->transferRemaining -= event->length;
    }

    if (mscHandle->needInStallFlag == 1)
    {
        mscHandle->needInStallFlag = 0;
        mscHandle->inEndpointStallFlag = 1;
        mscHandle->dataInFlag = 0;
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint);
        return error;
    }
    if ((!mscHandle->dataInFlag) && (event->length == USB_DEVICE_MSC_CSW_LENGTH))
    {
        csw = (usb_device_msc_csw_t *)(event->buffer);
    }

    if (mscHandle->dataInFlag)
    {
        if (mscHandle->transferRemaining)
        {
            mscHandle->currentOffset += (event->length / mscHandle->lengthOfEachLba);
            error = USB_DeviceMscSend(mscHandle);
        }
        if (!mscHandle->transferRemaining)
        {
            mscHandle->dataInFlag = 0;

            /* Send the command status information */
            mscHandle->cswPrimeFlag = 1;
            USB_DeviceSendRequest(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint,
                                  (uint8_t *)&mscHandle->g_mscCsw, USB_DEVICE_MSC_CSW_LENGTH);
        }
    }
    else if ((event->length == USB_DEVICE_MSC_CSW_LENGTH) && (csw->signature == USB_DEVICE_MSC_DCSWSIGNATURE))
    {
        mscHandle->cbwValidFlag = 1;
        mscHandle->cswPrimeFlag = 0;
        (void)USB_DeviceRecvRequest(g_deviceComposite->deviceHandle, mscHandle->bulkOutEndpoint,
                                    (uint8_t *)&mscHandle->g_mscCbw, USB_DEVICE_MSC_CBW_LENGTH);
        mscHandle->cbwPrimeFlag = 1;
    }
    else
    {
    }
    return error;
}
/*!
 * @brief Bulk OUT endpoint callback function.
 *
 * This callback function is used to notify uplayer the tranfser result of a transfer.
 * This callback pointer is passed when the Bulk OUT pipe initialized.
 *
 * @param handle          The device handle. It equals the value returned from USB_DeviceInit.
 * @param message         The result of the Bulk OUT pipe transfer.
 * @param callbackParam  The paramter for this callback. It is same with
 * usb_device_endpoint_callback_struct_t::callbackParam. In the class, the value is the MSC class handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscBulkOut(usb_device_handle deviceHandle,
                                  usb_device_endpoint_callback_message_struct_t *event,
                                  void *arg)
{
    usb_status_t error = kStatus_USB_Success;
    usb_device_msc_struct_t *mscHandle = (usb_device_msc_struct_t *)arg;

    if (event->length == USB_UNINITIALIZED_VAL_32)
    {
#if (defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
    (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
        if (NULL != currentTrasfer)
        {
            USB_DeviceMscAddBufferToHead(currentTrasfer);
        }

#endif
        return error;
    }

    if (mscHandle->transferRemaining >= event->length)
    {
        mscHandle->transferRemaining -= event->length;
    }

    if (mscHandle->needOutStallFlag == 1)
    {
        mscHandle->needOutStallFlag = 0;
        mscHandle->outEndpointStallFlag = 1;
        mscHandle->dataOutFlag = 0;
        mscHandle->cbwPrimeFlag = 0;
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkOutEndpoint);
        return error;
    }

    if (mscHandle->dataOutFlag)
    {
#if (defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
    (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
        if (NULL != currentTrasfer)
        {
            currentTrasfer->offset = mscHandle->currentOffset;
            currentTrasfer->size = event->length;
            if (0 == currentTrasfer->size)
            {
                USB_DeviceMscAddBufferToHead(currentTrasfer);
            }
            else
            {
                USB_DeviceMscAddBufferToTail(currentTrasfer);
            }
        }

#else
        /*write the data to sd card*/
        if (0 != event->length)
        {
            if (kStatus_Success != SD_WriteBlocks(usbDeviceMscCard, event->buffer, mscHandle->currentOffset,
                                                  event->length >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER))
            {
                g_deviceComposite->mscDisk.readWriteError = 1;
                usb_echo(
                    "Write error, error = 0xx%x \t Please check write request buffer size(must be less than 128 "
                    "sectors)\r\n",
                    error);
                error = kStatus_USB_Error;
            }
        }
#endif

        if (mscHandle->transferRemaining)
        {
            mscHandle->currentOffset += (event->length / mscHandle->lengthOfEachLba);
            error = USB_DeviceMscRecv(mscHandle);
        }

        if (!mscHandle->transferRemaining)
        {
            mscHandle->dataOutFlag = 0;
            {
                USB_DeviceSendRequest(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint,
                                      (uint8_t *)&mscHandle->g_mscCsw, USB_DEVICE_MSC_CSW_LENGTH);
                mscHandle->cswPrimeFlag = 1;
            }
        }
    }
    else if ((mscHandle->cbwValidFlag) && (event->length == USB_DEVICE_MSC_CBW_LENGTH) &&
             (mscHandle->g_mscCbw.signature == USB_DEVICE_MSC_DCBWSIGNATURE) &&
             (!((mscHandle->g_mscCbw.logicalUnitNumber & 0xF0) || (mscHandle->g_mscCbw.cbLength & 0xE0))) &&
             (mscHandle->g_mscCbw.logicalUnitNumber < (mscHandle->logicalUnitNumber + 1)) &&
             ((mscHandle->g_mscCbw.cbLength >= 0x01) && (mscHandle->g_mscCbw.cbLength <= 0x10)))
    {
        mscHandle->cbwPrimeFlag = 0;
        mscHandle->transferRemaining = 0;

        mscHandle->g_mscCsw.signature = USB_DEVICE_MSC_DCSWSIGNATURE;
        mscHandle->g_mscCsw.dataResidue = 0;
        mscHandle->g_mscCsw.tag = mscHandle->g_mscCbw.tag;

        mscHandle->cbwValidFlag = 0;

        mscHandle->g_mscCbw.dataTransferLength = USB_LONG_TO_LITTLE_ENDIAN(mscHandle->g_mscCbw.dataTransferLength);

        mscHandle->dataOutFlag = (uint8_t)(((!(mscHandle->g_mscCbw.flags & USB_DEVICE_MSC_CBW_DIRECTION_BIT)) &&
                                            (mscHandle->g_mscCbw.dataTransferLength)) ?
                                               1 :
                                               0);

        mscHandle->dataInFlag = (uint8_t)(((mscHandle->g_mscCbw.flags & USB_DEVICE_MSC_CBW_DIRECTION_BIT) &&
                                           (mscHandle->g_mscCbw.dataTransferLength)) ?
                                              1 :
                                              0);

        if ((0 != mscHandle->dataInFlag) && (0 != mscHandle->inEndpointStallFlag))
        {
            error = kStatus_USB_Error;
            return error;
        }
        error = USB_DeviceMscProcessUfiCommand(mscHandle);
        if (error == kStatus_USB_InvalidRequest)
        {
            if (mscHandle->dataOutFlag == 1)
            {
                if (mscHandle->outEndpointStallFlag == 0)
                {
                    mscHandle->needOutStallFlag = 1;
                }
                mscHandle->dataOutFlag = 0;
            }
            else if (mscHandle->dataInFlag == 1)
            {
                if (mscHandle->inEndpointStallFlag == 0)
                {
                    mscHandle->needInStallFlag = 1;
                }
                mscHandle->dataInFlag = 0;
            }
            else
            {
            }
            mscHandle->stallStatus = (uint8_t)USB_DEVICE_MSC_STALL_IN_DATA;
        }

        if (!((mscHandle->dataOutFlag) || ((mscHandle->dataInFlag) || (mscHandle->needInStallFlag))))
        {
            USB_DeviceSendRequest(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint,
                                  (uint8_t *)&mscHandle->g_mscCsw, USB_DEVICE_MSC_CSW_LENGTH);
            mscHandle->cswPrimeFlag = 1;
        }
    }
    else
    {
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkOutEndpoint);
        USB_DeviceStallEndpoint(g_deviceComposite->deviceHandle, mscHandle->bulkInEndpoint);
        mscHandle->cbwValidFlag = 0;
        mscHandle->outEndpointStallFlag = 1;
        mscHandle->inEndpointStallFlag = 1;
        mscHandle->stallStatus = (uint8_t)USB_DEVICE_MSC_STALL_IN_CBW;
        mscHandle->performResetRecover = 1;
    }
    return error;
}

/*!
 * @brief Initialize the endpoints of the msc class.
 *
 * This callback function is used to initialize the endpoints of the msc class.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscEndpointsInit(void)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_endpoint_init_struct_t epInitStruct;
    usb_device_endpoint_callback_struct_t endpointCallback;
    usb_device_msc_struct_t *mscHandle;

    mscHandle = &(g_deviceComposite->mscDisk.mscStruct);
    endpointCallback.callbackFn = USB_DeviceMscBulkIn;
    endpointCallback.callbackParam = (void *)mscHandle;

    epInitStruct.zlt = 0;
    epInitStruct.transferType = USB_ENDPOINT_BULK;
    epInitStruct.endpointAddress =
        USB_MSC_DISK_BULK_IN_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
    g_mscHandle->bulkInEndpoint = epInitStruct.endpointAddress;
    if (USB_SPEED_HIGH == g_deviceComposite->speed)
    {
        epInitStruct.maxPacketSize = HS_MSC_DISK_BULK_IN_PACKET_SIZE;
    }
    else
    {
        epInitStruct.maxPacketSize = FS_MSC_DISK_BULK_IN_PACKET_SIZE;
    }

    USB_DeviceInitEndpoint(g_deviceComposite->deviceHandle, &epInitStruct, &endpointCallback);

    endpointCallback.callbackFn = USB_DeviceMscBulkOut;
    endpointCallback.callbackParam = (void *)mscHandle;

    epInitStruct.zlt = 0;
    epInitStruct.transferType = USB_ENDPOINT_BULK;
    epInitStruct.endpointAddress =
        USB_MSC_DISK_BULK_OUT_ENDPOINT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);
    g_mscHandle->bulkOutEndpoint = epInitStruct.endpointAddress;
    if (USB_SPEED_HIGH == g_deviceComposite->speed)
    {
        epInitStruct.maxPacketSize = HS_MSC_DISK_BULK_OUT_PACKET_SIZE;
    }
    else
    {
        epInitStruct.maxPacketSize = FS_MSC_DISK_BULK_OUT_PACKET_SIZE;
    }

    USB_DeviceInitEndpoint(g_deviceComposite->deviceHandle, &epInitStruct, &endpointCallback);
    g_mscHandle->dataOutFlag = 0;
    g_mscHandle->dataInFlag = 0;
    g_mscHandle->outEndpointStallFlag = 0;
    g_mscHandle->inEndpointStallFlag = 0;
    g_mscHandle->needOutStallFlag = 0;
    g_mscHandle->needInStallFlag = 0;
    g_mscHandle->cbwValidFlag = 1;
    g_mscHandle->transferRemaining = 0;
    g_mscHandle->performResetRecover = 0;
    g_mscHandle->performResetDoneFlag = 0;
    g_mscHandle->stallStatus = 0;

    if (g_mscHandle->cbwPrimeFlag == 1)
    {
        USB_DeviceCancel(g_deviceComposite->deviceHandle, g_mscHandle->bulkOutEndpoint);
    }
    USB_DeviceRecvRequest(g_deviceComposite->deviceHandle, g_mscHandle->bulkOutEndpoint,
                          (uint8_t *)&g_mscHandle->g_mscCbw, USB_DEVICE_MSC_CBW_LENGTH);
    g_mscHandle->cbwPrimeFlag = 1;

    return error;
}
/*!
 * @brief De-initialize the endpoints of the msc class.
 *
 * This callback function is used to de-initialize the endpoints of the msc class.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscEndpointsDeinit(void)
{
    usb_status_t error = kStatus_USB_Error;

    error = USB_DeviceDeinitEndpoint(g_deviceComposite->deviceHandle, g_mscHandle->bulkInEndpoint);
    error = USB_DeviceDeinitEndpoint(g_deviceComposite->deviceHandle, g_mscHandle->bulkOutEndpoint);
    return error;
}

usb_status_t USB_DeviceMscDiskConfigureEndpointStatus(usb_device_handle handle, uint8_t ep, uint8_t status)
{
    usb_status_t error = kStatus_USB_Error;
    if (status)
    {
        if ((USB_MSC_DISK_BULK_IN_ENDPOINT == (ep & USB_ENDPOINT_NUMBER_MASK)) &&
            (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK))
        {
            if (g_mscHandle->inEndpointStallFlag == 0)
            {
                g_mscHandle->inEndpointStallFlag = 1;
                error = USB_DeviceStallEndpoint(handle, ep);
            }
        }
        else if ((USB_MSC_DISK_BULK_OUT_ENDPOINT == (ep & USB_ENDPOINT_NUMBER_MASK)) &&
                 (!(ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)))
        {
            if (g_mscHandle->outEndpointStallFlag == 0)
            {
                g_mscHandle->outEndpointStallFlag = 1;
                error = USB_DeviceStallEndpoint(handle, ep);
            }
        }
        else
        {
        }
    }
    else
    {
        if ((USB_MSC_DISK_BULK_IN_ENDPOINT == (ep & USB_ENDPOINT_NUMBER_MASK)) &&
            (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK))
        {
            if (g_mscHandle->inEndpointStallFlag == 1)
            {
                g_mscHandle->inEndpointStallFlag = 0;
                error = USB_DeviceUnstallEndpoint(handle, ep);
            }
        }
        else if ((USB_MSC_DISK_BULK_OUT_ENDPOINT == (ep & USB_ENDPOINT_NUMBER_MASK)) &&
                 (!(ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)))
        {
            if (g_mscHandle->outEndpointStallFlag == 1)
            {
                g_mscHandle->outEndpointStallFlag = 0;
                error = USB_DeviceUnstallEndpoint(handle, ep);
            }
        }
        else
        {
        }
    }
    if (((g_mscHandle->stallStatus == USB_DEVICE_MSC_STALL_IN_CSW) ||
         (g_mscHandle->stallStatus == USB_DEVICE_MSC_STALL_IN_DATA)) &&
        (g_mscHandle->performResetDoneFlag != 1))
    {
        if (g_mscHandle->cswPrimeFlag == 1)
        {
            USB_DeviceCancel(handle, g_mscHandle->bulkInEndpoint);
        }
        USB_DeviceSendRequest(handle, g_mscHandle->bulkInEndpoint, (uint8_t *)&g_mscHandle->g_mscCsw,
                              USB_DEVICE_MSC_CSW_LENGTH);
        g_mscHandle->cswPrimeFlag = 1;
        g_mscHandle->stallStatus = 0;
    }
    if ((g_mscHandle->performResetDoneFlag == 1) && (g_mscHandle->inEndpointStallFlag == 0) &&
        (g_mscHandle->outEndpointStallFlag == 0))
    {
        g_mscHandle->performResetDoneFlag = 0;
        if (g_mscHandle->cbwPrimeFlag == 1)
        {
            USB_DeviceCancel(handle, g_mscHandle->bulkOutEndpoint);
        }
        USB_DeviceRecvRequest(handle, g_mscHandle->bulkOutEndpoint, (uint8_t *)&g_mscHandle->g_mscCbw,
                              USB_DEVICE_MSC_CBW_LENGTH);
        g_mscHandle->cbwPrimeFlag = 1;
        g_mscHandle->stallStatus = 0;
    }
    return error;
}

usb_status_t USB_DeviceMscDiskClassRequest(usb_device_handle handle,
                                           usb_setup_struct_t *setup,
                                           uint32_t *length,
                                           uint8_t **buffer)
{
    usb_status_t error = kStatus_USB_Error;

    if (setup->wIndex != USB_MSC_DISK_INTERFACE_INDEX)
    {
        return error;
    }

    switch (setup->bRequest)
    {
        case USB_DEVICE_MSC_GET_MAX_LUN:
            if ((setup->wIndex == USB_MSC_DISK_INTERFACE_INDEX) && (!setup->wValue) && (setup->wLength <= 0x0001) &&
                ((setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN))
            {
                *buffer = (uint8_t *)&g_mscHandle->logicalUnitNumber;
                *length = setup->wLength;
            }
            else
            {
                error = kStatus_USB_InvalidRequest;
            }
            break;
        case USB_DEVICE_MSC_BULK_ONLY_MASS_STORAGE_RESET:
            if ((setup->wIndex == g_mscHandle->interfaceNumber) && (!setup->wValue) && (!setup->wLength) &&
                ((setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT))
            {
                error = USB_DeviceMscEndpointsDeinit();
                error = USB_DeviceMscEndpointsInit();
                g_mscHandle->performResetRecover = 0;
                g_mscHandle->performResetDoneFlag = 1;
            }
            else
            {
                error = kStatus_USB_InvalidRequest;
            }
            break;
        default:
            break;
    }
    return error;
}

usb_status_t USB_DeviceMscDiskSetConfigure(usb_device_handle handle, uint8_t configure)
{
    usb_status_t error = kStatus_USB_Error;

    g_deviceComposite->mscDisk.attach = 1;
    error = USB_DeviceMscEndpointsInit();
    return error;
}

void USB_DeviceMscAppTask(void)
{
    if (g_deviceComposite->mscDisk.readWriteError)
    {
        return;
    }
    USB_DeviceMscApp();
}
/*!
 * @brief device application init function.
 *
 * This function init the usb stack and sdhc driver.
 *
 * @return None.
 */

usb_status_t USB_DeviceMscDiskInit(usb_device_composite_struct_t *deviceComposite)
{
    g_deviceComposite = deviceComposite;
#if (defined(USB_DEVICE_CONFIG_USE_TASK) && (USB_DEVICE_CONFIG_USE_TASK > 0)) && \
    (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
    USB_DeviceMscInitQueue();
#endif
    usb_device_msc_ufi_struct_t *ufi = NULL;

    g_mscHandle = &deviceComposite->mscDisk.mscStruct;

    g_mscHandle->handle = deviceComposite->deviceHandle;

    ufi = &g_mscHandle->g_mscUfi;
    g_mscHandle->totalLogicalBlockNumber = usbDeviceMscCard->blockCount;
    g_mscHandle->lengthOfEachLba = usbDeviceMscCard->blockSize;
    g_mscHandle->logicalUnitNumber = LOGICAL_UNIT_SUPPORTED - 1;
    g_mscHandle->bulkInBufferSize = USB_DEVICE_MSC_READ_BUFF_SIZE;
    g_mscHandle->bulkOutBufferSize = USB_DEVICE_MSC_WRITE_BUFF_SIZE;
    g_mscHandle->implementingDiskDrive = USB_DEVICE_CONFIG_MSC_IMPLEMENTING_DISK_DRIVE;

    if (g_mscHandle->lengthOfEachLba * g_mscHandle->totalLogicalBlockNumber == 0)
    {
        return kStatus_USB_Error;
    }
    ufi->requestSense.validErrorCode = USB_DEVICE_MSC_UFI_REQ_SENSE_VALID_ERROR_CODE;
    ufi->requestSense.additionalSenseLength = USB_DEVICE_MSC_UFI_REQ_SENSE_ADDITIONAL_SENSE_LEN;
    ufi->requestSense.senseKey = USB_DEVICE_MSC_UFI_NO_SENSE;
    ufi->requestSense.additionalSenseCode = USB_DEVICE_MSC_UFI_NO_SENSE;
    ufi->requestSense.additionalSenseQualifer = USB_DEVICE_MSC_UFI_NO_SENSE;

    ufi->readCapacity.lastLogicalBlockAddress = USB_LONG_TO_BIG_ENDIAN(g_mscHandle->totalLogicalBlockNumber - 1);
    ufi->readCapacity.blockSize = USB_LONG_TO_BIG_ENDIAN((uint32_t)g_mscHandle->lengthOfEachLba);
    ufi->readCapacity16.lastLogicalBlockAddress1 = USB_LONG_TO_BIG_ENDIAN(g_mscHandle->totalLogicalBlockNumber - 1);
    ufi->readCapacity16.blockSize = USB_LONG_TO_BIG_ENDIAN((uint32_t)g_mscHandle->lengthOfEachLba);

    g_mscHandle->cbwPrimeFlag = 0;
    g_mscHandle->cswPrimeFlag = 0;

    return kStatus_USB_Success;
}
