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

#include "usb_device_class.h"
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

#include "composite.h"

#include "fsl_sdhc.h"
#include "fsl_card.h"
#include "fsl_gpio.h"
#include "event.h"
#if (USB_DEVICE_CONFIG_USE_TASK < 1)
#error This application requires USB_DEVICE_CONFIG_USE_TASK value defined > 0 in usb_device_config.h. Please recompile with this option.
#endif
/*******************************************************************************
* Definitions
******************************************************************************/
/*! @brief The maximum timeout time for the transfer complete event */
#define EVENT_TIMEOUT_TRANSFER_COMPLETE (1000U)

/*******************************************************************************
* Variables
******************************************************************************/
static usb_device_composite_struct_t *g_deviceComposite;

uint32_t g_mscReadRequestBuffer[USB_DEVICE_MSC_READ_BUFF_SIZE >> 2];
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))

uint32_t g_mscWriteRequestBuffer[USB_DEVICE_MSC_WRITE_BUFF_NUM][USB_DEVICE_MSC_WRITE_BUFF_SIZE >> 2];
#else

uint32_t g_mscWriteRequestBuffer[USB_DEVICE_MSC_WRITE_BUFF_SIZE >> 2];

#endif

/* Card detect flag. */
volatile uint32_t g_sdInsertedFlag;

/* State in SDHC driver. */
uint32_t g_sdhcAdmaTable[USB_DEVICE_MSC_ADMA_TABLE_WORDS];
sdhc_handle_t g_sdhcHandle;
/* State in Card driver. */
sd_card_t g_sd;
volatile uint8_t g_sdhcTransferFailedFlag; /* SDHC transfer status. 0 means success, 1 means failed. */
sd_card_t *usbDeviceMscCard;

#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
void *g_writeBufferHandle;
void *g_writeTaskHandle;
xTaskHandle g_usbWriteTaskHandle;
SemaphoreHandle_t g_xMutex;
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
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
/*!
 * @brief device msc write task function.
 *
 * This function wrtie data to the sdcard.
 * @param handle          The write task parameter.
 */
void USB_DeviceMscWriteTask(void *Handle)
{
    uint32_t writeInformationa[3];
    status_t errorCode;
    while (1)
    {
        xQueueReceive(g_writeTaskHandle, &writeInformationa, portMAX_DELAY);
        xSemaphoreTake(g_xMutex, portMAX_DELAY);
        errorCode = SD_WriteBlocks(usbDeviceMscCard, (uint8_t *)writeInformationa[0], writeInformationa[1],
                                   writeInformationa[2] >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER);
        xSemaphoreGive(g_xMutex);
        if (kStatus_Success != errorCode)
        {
            g_deviceComposite->mscDisk.readWriteError = 1;
            usb_echo(
                "Write error, error = 0xx%x \t Please check write request buffer size(must be less than 128 "
                "sectors)\r\n",
                errorCode);
        }
        xQueueSend(g_writeBufferHandle, &writeInformationa[0], 0U);
    }
}

#endif
/*!
 * @brief device msc callback function.
 *
 * This function handle the disk class specified event.
 * @param handle          The USB class  handle.
 * @param event           The USB device event type.
 * @param param           The parameter of the class specific event.
 * @return kStatus_USB_Success or error.
 */
usb_status_t USB_DeviceMscCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Success;
    status_t errorCode = kStatus_Success;
    usb_device_lba_information_struct_t *lbaInformation;
    usb_device_lba_app_struct_t *lba;
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
    uint32_t writeInformation[3];
    uint32_t tempbuffer;
#endif
    switch (event)
    {
        case kUSB_DeviceMscEventReadResponse:
            lba = (usb_device_lba_app_struct_t *)param;
            break;
        case kUSB_DeviceMscEventWriteResponse:
            lba = (usb_device_lba_app_struct_t *)param;
/*write the data to sd card*/
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
            writeInformation[0] = (uint32_t)lba->buffer;
            writeInformation[1] = lba->offset;
            writeInformation[2] = lba->size;
            if (0 == lba->size)
            {
                xQueueSend(g_writeBufferHandle, &writeInformation[0], 0U);
            }
            else
            {
                xQueueSend(g_writeTaskHandle, &writeInformation, 0U);
            }
#else
            /*write the data to sd card*/
            if (0 != lba->size)
            {
                errorCode = SD_WriteBlocks(usbDeviceMscCard, lba->buffer, lba->offset,
                                           lba->size >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER);
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
#endif
            break;
        case kUSB_DeviceMscEventWriteRequest:
            lba = (usb_device_lba_app_struct_t *)param;
/*get a buffer to store the data from host*/
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))

            xQueueReceive(g_writeBufferHandle, (void *)&tempbuffer, portMAX_DELAY);
            lba->buffer = (uint8_t *)tempbuffer;
#else
            lba->buffer = (uint8_t *)&g_mscWriteRequestBuffer[0];
#endif
            break;
        case kUSB_DeviceMscEventReadRequest:
            lba = (usb_device_lba_app_struct_t *)param;
            lba->buffer = (uint8_t *)&g_mscReadRequestBuffer[0];

/*read the data from sd card, then store these data to the read buffer*/
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
            xSemaphoreTake(g_xMutex, portMAX_DELAY);
#endif
            errorCode = SD_ReadBlocks(usbDeviceMscCard, lba->buffer, lba->offset,
                                      lba->size >> USB_DEVICE_SDCARD_BLOCK_SIZE_POWER);
#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))
            xSemaphoreGive(g_xMutex);
#endif
            if (kStatus_Success != errorCode)
            {
                g_deviceComposite->mscDisk.readWriteError = 1;
                usb_echo(
                    "Read error, error = 0xx%x \t Please check read request buffer size(must be less than 128 "
                    "sectors)\r\n",
                    error);
                error = kStatus_USB_Error;
            }
            break;
        case kUSB_DeviceMscEventGetLbaInformation:
            lbaInformation = (usb_device_lba_information_struct_t *)param;
            lbaInformation->lengthOfEachLba = usbDeviceMscCard->blockSize;
            lbaInformation->totalLbaNumberSupports = usbDeviceMscCard->blockCount;
            lbaInformation->logicalUnitNumberSupported = LOGICAL_UNIT_SUPPORTED;
            lbaInformation->bulkInBufferSize = USB_DEVICE_MSC_READ_BUFF_SIZE;
            lbaInformation->bulkOutBufferSize = USB_DEVICE_MSC_WRITE_BUFF_SIZE;
            break;
        case kUSB_DeviceMscEventFormatComplete:
            break;
        case kUSB_DeviceMscEventRemovalRequest:
            break;
        default:
            break;
    }
    return error;
}
/*!
 * @brief msc device set configuration function.
 *
 * This function sets configuration for msc class.
 *
 * @param handle The msc class handle.
 * @param configure The msc class configure index.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscDiskSetConfigure(class_handle_t handle, uint8_t configure)
{
    return kStatus_USB_Error;
}
/*!
 * @brief device msc init function.
 *
 * This function initializes the device with the composite device class information.
 *
 * @param deviceComposite          The pointer to the composite device structure.
 * @return kStatus_USB_Success .
 */
usb_status_t USB_DeviceMscDiskInit(usb_device_composite_struct_t *deviceComposite)
{
    g_deviceComposite = deviceComposite;

#if (defined(USB_DEVICE_MSC_USE_WRITE_TASK) && (USB_DEVICE_MSC_USE_WRITE_TASK > 0))

    g_xMutex = xSemaphoreCreateMutex();
    g_writeBufferHandle = xQueueCreate(USB_DEVICE_MSC_WRITE_BUFF_NUM, sizeof(uint32_t *));
    for (int i = 0; i < USB_DEVICE_MSC_WRITE_BUFF_NUM; i++)
    {
        uint8_t *bufferAddress = (uint8_t *)&g_mscWriteRequestBuffer[i][0];
        xQueueSend(g_writeBufferHandle, &bufferAddress, 0);
    }
    g_writeTaskHandle = xQueueCreate(USB_DEVICE_MSC_WRITE_BUFF_NUM, sizeof(usb_device_lba_app_struct_t));
#endif

    return kStatus_USB_Success;
}
