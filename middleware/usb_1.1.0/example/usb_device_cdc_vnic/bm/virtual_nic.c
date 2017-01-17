/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
#include "fsl_device_registers.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#include <stdio.h>
#include <stdlib.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_cdc_rndis.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "fsl_enet_adapter.h"
#include "virtual_nic.h"
#include "virtual_nic_enet.h"
#include "usb_khci.h"
#include "usb_device_dci.h"

#include "khci_device.h"
#if (defined(FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT > 0U))
#include "fsl_mpu.h"
#endif /* FSL_FEATURE_SOC_MPU_COUNT */
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#include "usb_phy.h"
#endif

/*******************************************************************************
* Definitions
******************************************************************************/
/* Base unit for ENIT layer is 1Mbps while for RNDIS its 100bps*/
#define ENET_CONVERT_FACTOR (10000)

/* Instance number of ethernet module. */
#define ENET_INSTANCE (0)

void BOARD_InitHardware(void);

/*******************************************************************************
* Variables
******************************************************************************/
extern usb_device_endpoint_struct_t g_cdcVnicDicEp[];
extern usb_device_class_struct_t g_cdcVnicClass;
extern EnetHandle g_enetHandle;
extern enet_address_t g_nicMacAddr;
extern queue_struct_t g_enetServiceQue;
extern usb_device_khci_state_struct_t s_UsbDeviceKhciState[USB_DEVICE_CONFIG_KHCI];
/* Data structure of virtual nic device. */
usb_cdc_vnic_struct_t g_cdcVnic;
/* Buffer to receive data. */
static uint8_t s_currRecvBuf[DATA_BUFF_SIZE];
/* Pointer of part 1 of usb transmit buffer. */
static uint8_t *s_usbTxPart1Buffer;
/* Pointer of usb receive buffer to store the rndis packet. */
static uint8_t *s_usbRxRndisPacketBuffer;
/* Append byte for zero length packet. */
static uint8_t g_zeroSend = 0x00;

static usb_device_class_config_struct_t s_cdcAcmConfig[1] = {{
    USB_DeviceCdcVnicCallback, 0, &g_cdcVnicClass,
}};

static usb_device_class_config_list_struct_t s_cdcAcmConfigList = {
    s_cdcAcmConfig, usb_deviceCallback, 1,
};

/*******************************************************************************
* Code
******************************************************************************/

/*!
 * @brief Set the state of the usb transmit direction
 *
 * @param state The state of the usb transmit direction.
 * @return A USB error code or kStatus_USB_Success.
 */
static inline usb_status_t USB_DeviceVnicTransmitSetState(usb_cdc_vnic_tx_state_t state)
{
    USB_DEVICE_VNIC_ENTER_CRITICAL();

    g_cdcVnic.nicTrafficInfo.usbTxState = state;

    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

/*!
 * @brief Set the state of the usb receive direction.
 *
 * @param state The state of the usb receive direction.
 * @return A USB error code or kStatus_USB_Success.
 */
static inline usb_status_t USB_DeviceVnicReceiveSetState(usb_cdc_vnic_rx_state_t state)
{
    USB_DEVICE_VNIC_ENTER_CRITICAL();

    g_cdcVnic.nicTrafficInfo.usbRxState = state;

    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return kStatus_USB_Success;
}

/*!
 * @brief Free nicPcb.
 *
 * @param nicPcb The pointer to the packet control block of the NIC.
 * @return None.
 */
void USB_DeviceVnicSendPcbFree(PCB_PTR nicPcb)
{
    if (nicPcb != NULL)
    {
        g_cdcVnic.nicTrafficInfo.enetTxUsb2enet++;
        USB_DeviceVnicReceiveSetState(RX_USB2ENET_DONE);
    }
}

/*!
 * @brief State process for usb transmit direction.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceVnicTransmit()
{
    usb_status_t error = kStatus_USB_Error;
    vnic_enet_transfer_struct_t cdcAcmTransfer;
    PCB_PTR enetPcb;
    uint32_t usbTxLen;
    uint32_t usbTxPart_1Len;
    uint8_t *nicData;
    uint8_t status;
    uint32_t length;
    uint8_t *firstSendBuff = s_usbTxPart1Buffer;
    switch (g_cdcVnic.nicTrafficInfo.usbTxState)
    {
        case TX_IDLE:
        {
            /* Initialize the tx variables */
            g_cdcVnic.nicTrafficInfo.usbTxNicData = NULL;
            g_cdcVnic.nicTrafficInfo.usbTxEnetPcb = NULL;
            g_cdcVnic.nicTrafficInfo.usbTxRndisPkgLength = 0;
            g_cdcVnic.nicTrafficInfo.usbTxPart_1Len = 0;

            /* Get a transfer request from the enet queue */
            error = vnicEnetQueueGet(&g_enetServiceQue, &cdcAcmTransfer);
            if (kStatus_USB_Success == error)
            {
                enetPcb = (PCB_PTR)(cdcAcmTransfer.buffer);
                length = enetPcb->FRAG[0].LENGTH;
                nicData = enetPcb->FRAG[0].FRAGMENT;

                usbTxLen = length + RNDIS_USB_OVERHEAD_SIZE;
                usbTxPart_1Len = 0;
                g_cdcVnic.nicTrafficInfo.enetRxUsb2hostSent++;
                g_cdcVnic.nicTrafficInfo.usbTxNicData = nicData;
                g_cdcVnic.nicTrafficInfo.usbTxRndisPkgLength = usbTxLen;
                g_cdcVnic.nicTrafficInfo.usbTxEnetPcb = enetPcb;

                /* RNDIS Protocol defines 1 byte call of 0x00, if transfer size is multiple of endpoint packet size */
                g_cdcVnic.nicTrafficInfo.usbTxZeroSendFlag =
                    (uint8_t)((usbTxLen % g_cdcVnicDicEp[0].maxPacketSize) ? 0 : 1);

                /* Whichever is smaller but not less than RNDIS_USB_OVERHEAD_SIZE */
                usbTxPart_1Len =
                    usbTxLen > g_cdcVnicDicEp[0].maxPacketSize ? g_cdcVnicDicEp[0].maxPacketSize : usbTxLen;

                if (usbTxPart_1Len < RNDIS_USB_OVERHEAD_SIZE)
                {
                    /* For g_cdcVnicDicEp[0].maxPacketSize as 8, 16 or 32 minimum  usbTxPart_1Len has to be
                     either usbTxLen
                     (which is definitely greater than RNDIS_USB_OVERHEAD_SIZE) or at least 64 which is the next allowed
                     multiple of
                     g_cdcVnicDicEp[0].maxPacketSize */
                    usbTxPart_1Len = usbTxLen > 64 ? 64 : usbTxLen;
                }
                g_cdcVnic.nicTrafficInfo.usbTxPart_1Len = usbTxPart_1Len;

                /* Prepare USB Header */
                ((rndis_packet_msg_format_t *)firstSendBuff)->messageType = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_PACKET_MSG);
                ((rndis_packet_msg_format_t *)firstSendBuff)->messageLen = USB_LONG_TO_LITTLE_ENDIAN(usbTxLen);
                ((rndis_packet_msg_format_t *)firstSendBuff)->dataOffset = USB_LONG_TO_LITTLE_ENDIAN(RNDIS_DATA_OFFSET);
                ((rndis_packet_msg_format_t *)firstSendBuff)->dataLen = USB_LONG_TO_LITTLE_ENDIAN(length);

                /* Fill rest of firstSendBuff buffers with payload as much as possible */
                memcpy(firstSendBuff + RNDIS_USB_OVERHEAD_SIZE, nicData, usbTxPart_1Len - RNDIS_USB_OVERHEAD_SIZE);

                USB_DEVICE_VNIC_ENTER_CRITICAL();
                error = USB_DeviceCdcAcmSend(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_IN_ENDPOINT, firstSendBuff,
                                             usbTxPart_1Len);
                if (kStatus_USB_Error != error)
                {
                    USB_DeviceVnicTransmitSetState(TX_PART_ONE_PROCESS);
                }
                else
                {
                    usb_echo("Part 1 of RNDIS packet send failed, 0x%x\n", error);
                    PCBFree(enetPcb);
                }
                USB_DEVICE_VNIC_EXIT_CRITICAL();
            }
        }
        break;
        case TX_PART_ONE_PROCESS:
            break;
        case TX_PART_ONE_DONE:
        {
            usbTxLen = g_cdcVnic.nicTrafficInfo.usbTxRndisPkgLength;
            usbTxPart_1Len = g_cdcVnic.nicTrafficInfo.usbTxPart_1Len;
            nicData = g_cdcVnic.nicTrafficInfo.usbTxNicData;
            enetPcb = g_cdcVnic.nicTrafficInfo.usbTxEnetPcb;
            uint8_t returnStatus = kStatus_USB_Success;

            if (usbTxLen > usbTxPart_1Len)
            {
                /* Send the part 2 of the RNDIS packet */
                USB_DEVICE_VNIC_ENTER_CRITICAL();
                error = USB_DeviceCdcAcmSend(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_IN_ENDPOINT,
                                             nicData + (usbTxPart_1Len - RNDIS_USB_OVERHEAD_SIZE),
                                             usbTxLen - usbTxPart_1Len);
                if (kStatus_USB_Error != error)
                {
                    USB_DeviceVnicTransmitSetState(TX_PART_TWO_PROCESS);
                }
                else
                {
                    usb_echo("Part 2 of RNDIS packet send failed, 0x%x\n", returnStatus);
                    PCBFree(enetPcb);
                }
                USB_DEVICE_VNIC_EXIT_CRITICAL();
            }
            else
            {
                USB_DeviceVnicTransmitSetState(TX_PART_TWO_DONE);
            }
        }
        break;
        case TX_PART_TWO_PROCESS:
            break;
        case TX_PART_TWO_DONE:
        {
            enetPcb = g_cdcVnic.nicTrafficInfo.usbTxEnetPcb;
            if (g_cdcVnic.nicTrafficInfo.usbTxZeroSendFlag == 1)
            {
                /* Send a zero length packet */
                USB_DEVICE_VNIC_ENTER_CRITICAL();
                error = USB_DeviceCdcAcmSend(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_IN_ENDPOINT, &g_zeroSend,
                                             sizeof(uint8_t));
                if (kStatus_USB_Error != error)
                {
                    USB_DeviceVnicTransmitSetState(TX_ZLP_PROCESS);
                }
                else
                {
                    usb_echo("Zero length packet send failed, 0x%x\n", error);
                }
                USB_DEVICE_VNIC_EXIT_CRITICAL();
            }
            else
            {
                PCBFree(enetPcb);
                g_cdcVnic.nicTrafficInfo.enetRxUsb2host++;
                USB_DeviceVnicTransmitSetState(TX_IDLE);
            }
        }
        break;
        case TX_ZLP_PROCESS:
            break;
        case TX_ZLP_DONE:
        {
            enetPcb = g_cdcVnic.nicTrafficInfo.usbTxEnetPcb;
            PCBFree(enetPcb);
            g_cdcVnic.nicTrafficInfo.enetRxUsb2host++;
            USB_DeviceVnicTransmitSetState(TX_IDLE);
        }
        break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief State process for usb receive direction.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceVnicReceive()
{
    usb_status_t error = kStatus_USB_Error;
    enet_status_t enetErr = kStatus_ENET_Success;
    uint8_t *rndisPktMsgData = NULL;
    uint32_t frameRemainingLen = 0;
    uint32_t messageLen = 0;
    uint8_t *buffer;
    uint32_t len;
    switch (g_cdcVnic.nicTrafficInfo.usbRxState)
    {
        case RX_IDLE:
        {
            /* Prime for next receive */
            USB_DEVICE_VNIC_ENTER_CRITICAL();
            error = USB_DeviceCdcAcmRecv(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_OUT_ENDPOINT, s_currRecvBuf,
                                         g_cdcVnicDicEp[1].maxPacketSize);
            if (kStatus_USB_Error != error)
            {
                USB_DeviceVnicReceiveSetState(RX_PART_ONE_PROCESS);
            }
            else
            {
                usb_echo("Part 1 of RNDIS packet recv failed\r\n");
            }
            USB_DEVICE_VNIC_EXIT_CRITICAL();
        }
        break;
        case RX_PART_ONE_PROCESS:
            break;
        case RX_PART_ONE_DONE:
        {
            buffer = (uint8_t *)g_cdcVnic.nicTrafficInfo.usbRxPart1Buf;
            len = g_cdcVnic.nicTrafficInfo.usbRxPart1Len;
            if (0xFFFFFFFF == len)
            {
                USB_DeviceVnicReceiveSetState(RX_IDLE);
                break;
            }
            messageLen = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)(buffer) + 1));
            ;

            if (!(messageLen % g_cdcVnicDicEp[1].maxPacketSize))
            {
                /* RNDIS Protocol defines 1 byte call of 0x00, if transfer size is multiple of endpoint packet size */
                messageLen++;
            }
            rndisPktMsgData = s_usbRxRndisPacketBuffer;
            memcpy(rndisPktMsgData, buffer, len);

            frameRemainingLen = messageLen - len;

            /* Receive the second part of RNDIS packet from host */
            if (frameRemainingLen)
            {
                /* Required when ethernet packet + usb header is larger than maxPacketSize */
                USB_DEVICE_VNIC_ENTER_CRITICAL();
                error = USB_DeviceCdcAcmRecv(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_OUT_ENDPOINT,
                                             rndisPktMsgData + g_cdcVnicDicEp[1].maxPacketSize, frameRemainingLen);
                if (kStatus_USB_Error != error)
                {
                    USB_DeviceVnicReceiveSetState(RX_PART_TWO_PROCESS);
                }
                else
                {
                    usb_echo("Part 2 of RNDIS packet recv failed\r\n");
                }
                USB_DEVICE_VNIC_EXIT_CRITICAL();
            }
            else
            {
                /* Send the ethernet packet */
                USB_DEVICE_VNIC_ENTER_CRITICAL();
                USB_DeviceVnicReceiveSetState(RX_USB2ENET_PROCESS);
                enetErr = vnicEnetSend(NULL, (uint8_t *)(rndisPktMsgData + RNDIS_USB_OVERHEAD_SIZE),
                                       messageLen - RNDIS_USB_OVERHEAD_SIZE);

                if (kStatus_ENET_Success == enetErr)
                {
                    g_cdcVnic.nicTrafficInfo.enetTxHost2usb++;
                }
                else
                {
                    usb_echo("RX_PART_ONE_DONE, vnicEnetSend failed\n");
                }
                USB_DEVICE_VNIC_EXIT_CRITICAL();
            }
        }
        break;
        case RX_PART_TWO_PROCESS:
            break;
        case RX_PART_TWO_DONE:
        {
            buffer = (uint8_t *)g_cdcVnic.nicTrafficInfo.usbRxPart2Buf;
            len = g_cdcVnic.nicTrafficInfo.usbRxPart2Len;
            if (0xFFFFFFFF == len)
            {
                USB_DeviceVnicReceiveSetState(RX_IDLE);
                break;
            }
            /* Entire ethernet packet with USB header was not received as one transaction */
            rndisPktMsgData = buffer - g_cdcVnicDicEp[1].maxPacketSize;

            /* Re-calculate messageLen as it might have changed because of 1 byte of zero recv */
            messageLen = USB_LONG_TO_LITTLE_ENDIAN(*((uint32_t *)rndisPktMsgData + 1));

            /* Send the ethernet packet */
            USB_DEVICE_VNIC_ENTER_CRITICAL();
            USB_DeviceVnicReceiveSetState(RX_USB2ENET_PROCESS);
            enetErr = vnicEnetSend(NULL, (uint8_t *)(rndisPktMsgData + RNDIS_USB_OVERHEAD_SIZE),
                                   messageLen - RNDIS_USB_OVERHEAD_SIZE);

            if (kStatus_ENET_Success == enetErr)
            {
                g_cdcVnic.nicTrafficInfo.enetTxHost2usb++;
            }
            else
            {
                usb_echo("RX_PART_TWO_DONE, vnicEnetSend failed\n");
            }
            USB_DEVICE_VNIC_EXIT_CRITICAL();
        }
        break;
        case RX_USB2ENET_PROCESS:
            break;
        case RX_USB2ENET_DONE:
        {
            USB_DeviceVnicReceiveSetState(RX_IDLE);
        }
        break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief Callback for RNDIS specific requests.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param event The event for the RNDIS device.
 * @param param The pointer to parameter of the callback.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcRndisCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Success;
    usb_device_cdc_rndis_request_param_struct_t *rndisParam;
    rndisParam = (usb_device_cdc_rndis_request_param_struct_t *)param;

    switch (event)
    {
        case kUSB_DeviceCdcEventAppGetLinkSpeed:
            if (1 == g_cdcVnic.attach)
            {
                *((uint32_t *)rndisParam->buffer) = ENETGetSpeed(g_enetHandle);
                *((uint32_t *)rndisParam->buffer) *= ENET_CONVERT_FACTOR;
            }
            break;
        case kUSB_DeviceCdcEventAppGetSendPacketSize:
            *((uint32_t *)rndisParam->buffer) = USB_LONG_TO_LITTLE_ENDIAN(g_cdcVnicDicEp[0].maxPacketSize);
            break;
        case kUSB_DeviceCdcEventAppGetRecvPacketSize:
            *((uint32_t *)rndisParam->buffer) = USB_LONG_TO_LITTLE_ENDIAN(g_cdcVnicDicEp[1].maxPacketSize);
            break;
        case kUSB_DeviceCdcEventAppGetMacAddress:
            memcpy(rndisParam->buffer, g_nicMacAddr, MAC_ADDR_SIZE);
            break;
        case kUSB_DeviceCdcEventAppGetMaxFrameSize:
            *((uint32_t *)rndisParam->buffer) = (ENET_FRAMESIZE_MAXDATA + sizeof(ENET_HEADER));
            break;
        case kUSB_DeviceCdcEventAppGetLinkStatus:
            if (1 == g_cdcVnic.attach)
            {
                *((uint32_t *)rndisParam->buffer) = ENETLinkStatus(g_enetHandle);
            }
            break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief Callback for CDC RNDIS class specific requests.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param event The event for the RNDIS device.
 * @param param The pointer to parameter of the callback.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcVnicCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    uint8_t count;
    uint32_t len;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;
    vnic_enet_transfer_struct_t firstSendBuffParam;
    vnic_enet_transfer_struct_t rndisPktMsgDataParam;
    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam = (usb_device_endpoint_callback_message_struct_t *)param;
    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
        {
            if (1 == g_cdcVnic.attach)
            {
                if (NULL == epCbParam->buffer)
                {
                    return error;
                }

                switch (g_cdcVnic.nicTrafficInfo.usbTxState)
                {
                    case TX_PART_ONE_PROCESS:
                        USB_DeviceVnicTransmitSetState(TX_PART_ONE_DONE);
                        break;
                    case TX_PART_TWO_PROCESS:
                        USB_DeviceVnicTransmitSetState(TX_PART_TWO_DONE);
                        break;
                    case TX_ZLP_PROCESS:
                        USB_DeviceVnicTransmitSetState(TX_ZLP_DONE);
                        break;
                    default:
                        break;
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventRecvResponse:
        {
            if (1 == g_cdcVnic.attach)
            {
                switch (g_cdcVnic.nicTrafficInfo.usbRxState)
                {
                    case RX_PART_ONE_PROCESS:
                        g_cdcVnic.nicTrafficInfo.usbRxPart1Buf = epCbParam->buffer;
                        g_cdcVnic.nicTrafficInfo.usbRxPart1Len = epCbParam->length;
                        USB_DeviceVnicReceiveSetState(RX_PART_ONE_DONE);
                        break;
                    case RX_PART_TWO_PROCESS:
                        g_cdcVnic.nicTrafficInfo.usbRxPart2Buf = epCbParam->buffer;
                        g_cdcVnic.nicTrafficInfo.usbRxPart2Len = epCbParam->length;
                        USB_DeviceVnicReceiveSetState(RX_PART_TWO_DONE);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                usb_echo("Discard the received data\r\n");
            }
        }
        break;
        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0;
            break;
        case kUSB_DeviceCdcEventSendEncapsulatedCommand:
            if (1 == acmReqParam->isSetup)
            {
                *(acmReqParam->buffer) = (g_cdcVnic.rndisHandle)->rndisCommand;
            }
            else
            {
                USB_DeviceCdcRndisMessageSet(g_cdcVnic.rndisHandle, acmReqParam->buffer, acmReqParam->length);
                *(acmReqParam->length) = 0;
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventGetEncapsulatedResponse:
            USB_DeviceCdcRndisMessageGet(g_cdcVnic.rndisHandle, acmReqParam->buffer, acmReqParam->length);
            error = kStatus_USB_Success;
            break;
        default:
            break;
    }

    return error;
}

/*!
 * @brief USB device callback function.
 *
 * This function handles the usb device specific requests.
 *
 * @param handle          The USB device handle.
 * @param event           The USB device event type.
 * @param param           The parameter of the device specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_control_request_struct_t *requestStruct;
    uint16_t *temp16 = (uint16_t *)param;
    uint8_t *temp8 = (uint8_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            uint8_t *message;
            uint32_t len;
            g_cdcVnic.attach = 0;
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
            if (kStatus_USB_Success == USB_DeviceClassGetSpeed(CONTROLLER_ID, &g_cdcVnic.speed))
            {
                USB_DeviceSetSpeed(handle, g_cdcVnic.speed);
            }
#endif
            USB_DeviceCdcRndisHaltCommand(g_cdcVnic.rndisHandle);
            USB_DeviceCdcRndisResetCommand(g_cdcVnic.rndisHandle, &message, &len);
            vnicEnetClearEnetQueue();
            if ((g_cdcVnic.nicTrafficInfo.usbTxEnetPcb != NULL) &&
                (((PCB_PTR)(g_cdcVnic.nicTrafficInfo.usbTxEnetPcb))->PRIVATE != NULL))
            {
                PCBFree(g_cdcVnic.nicTrafficInfo.usbTxEnetPcb);
            }
            USB_DeviceVnicTransmitSetState(TX_IDLE);
            USB_DeviceVnicReceiveSetState(RX_PART_ONE_PROCESS);
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
            if (param)
            {
                g_cdcVnic.attach = 1;
                g_cdcVnic.currentConfiguration = *temp8;
                if (USB_CDC_VNIC_CONFIGURE_INDEX == (*temp8))
                {
                    /* Schedule buffer for receive */
                    error = USB_DeviceCdcAcmRecv(g_cdcVnic.cdcAcmHandle, USB_CDC_VNIC_BULK_OUT_ENDPOINT, s_currRecvBuf,
                                                 g_cdcVnicDicEp[0].maxPacketSize);
                    if (kStatus_USB_Error == error)
                    {
                        usb_echo("kUSB_DeviceEventSetConfiguration, USB_DeviceCdcAcmRecv failed.\r\n");
                    }
                }
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if (g_cdcVnic.attach)
            {
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < USB_CDC_VNIC_INTERFACE_COUNT)
                {
                    g_cdcVnic.currentInterfaceAlternateSetting[interface] = alternateSetting;
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            break;
        case kUSB_DeviceEventGetInterface:
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param)
            {
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                error = USB_DeviceGetConfigurationDescriptor(handle,
                                                             (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
    }

    return error;
}

/*!
 * @brief USB Interrupt service routine.
 *
 * This function serves as the USB interrupt service routine.
 *
 * @return None.
 */
void USB0_IRQHandler(void)
{
    USB_DeviceKhciIsrFunction(g_cdcVnic.deviceHandle);
}

/*!
 * @brief ENET_Transmit Interrupt service routine.
 *
 * This function serves as the ENET_Transmit interrupt service routine.
 *
 * @return None.
 */
void ENET_Transmit_IRQHandler(void)
{
    ENET_DRV_TxIRQHandler(ENET_INSTANCE);
}

/*!
 * @brief ENET_Receive Interrupt service routine.
 *
 * This function serves as the ENET_Receive interrupt service routine.
 *
 * @return None.
 */
void ENET_Receive_IRQHandler(void)
{
    ENET_DRV_RxIRQHandler(ENET_INSTANCE);
}

/*!
 * @brief ENET_1588_Timer Interrupt service routine.
 *
 * This function serves as the ENET_1588_Timer interrupt service routine.
 *
 * @return None.
 */
void ENET_1588_Timer_IRQHandler(void)
{
    ENET_DRV_TsIRQHandler(ENET_INSTANCE);
}

/*!
 * @brief Application initialization function.
 *
 * This function initializes the application.
 *
 * @return None.
 */
void APPInit(void)
{
    usb_device_cdc_rndis_config_struct_t rndisConfig;
    uint8_t irqNo;
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
    uint8_t ehciIrq[] = USBHS_IRQS;
    irqNo = ehciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

    CLOCK_EnableUsbhs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ);
#endif
    uint8_t khciIrq[] = USB_IRQS;
    irqNo = khciIrq[CONTROLLER_ID - kUSB_ControllerKhci0];

    SystemCoreClockUpdate();

#if ((defined FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED) && (FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED))
    CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcIrc48M, 48000000U);
#else
    #if (defined(FSL_FEATURE_SOC_SCG_COUNT) && (FSL_FEATURE_SOC_SCG_COUNT > 0U))
        CLOCK_EnableUsbfs0Clock(kCLOCK_IpSrcFircAsync, CLOCK_GetFreq(kCLOCK_ScgFircAsyncDiv1Clk));
    #else
        CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
    #endif
#endif /* FSL_FEATURE_USB_KHCI_IRC48M_MODULE_CLOCK_ENABLED */
#endif
#if (defined(FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT > 0U))
    MPU_Enable(MPU, 0);
#endif /* FSL_FEATURE_SOC_MPU_COUNT */

/*
 * If the SOC has USB KHCI dedicated RAM, the RAM memory needs to be clear after
 * the KHCI clock is enabled. When the demo uses USB EHCI IP, the USB KHCI dedicated
 * RAM can not be used and the memory can't be accessed.
 */
#if (defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U))
#if (defined(FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS) && (FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS > 0U))
    for (int i = 0; i < FSL_FEATURE_USB_KHCI_USB_RAM; i++)
    {
        ((uint8_t *)FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS)[i] = 0x00U;
    }
#endif /* FSL_FEATURE_USB_KHCI_USB_RAM_BASE_ADDRESS */
#endif /* FSL_FEATURE_USB_KHCI_USB_RAM */

    g_cdcVnic.speed = USB_SPEED_FULL;
    g_cdcVnic.attach = 0;
    g_cdcVnic.cdcAcmHandle = (class_handle_t)NULL;
    g_cdcVnic.deviceHandle = NULL;

    USB_DeviceVnicReceiveSetState(RX_PART_ONE_PROCESS);
    /* Initialize the FEC interface */
    if (ENET_OK != vnicEnetInit())
    {
        usb_echo("vnicEnetInit failed\r\n");
    }

    if (kStatus_USB_Success != USB_DeviceClassInit(CONTROLLER_ID, &s_cdcAcmConfigList, &g_cdcVnic.deviceHandle))
    {
        usb_echo("USB device init failed\r\n");
    }
    else
    {
        usb_echo("USB device CDC virtual nic demo\r\n");
        g_cdcVnic.cdcAcmHandle = s_cdcAcmConfigList.config->classHandle;
    }

    rndisConfig.devMaxTxSize = ENET_FRAMESIZE_MAXDATA + sizeof(ENET_HEADER) + RNDIS_USB_HEADER_SIZE;
    rndisConfig.rndisCallback = USB_DeviceCdcRndisCallback;
    if (kStatus_USB_Success != USB_DeviceCdcRndisInit(g_cdcVnic.cdcAcmHandle, &rndisConfig, &(g_cdcVnic.rndisHandle)))
    {
        usb_echo("USB_DeviceCdcRndisInit failed\r\n");
    }

    s_usbRxRndisPacketBuffer = USB_OsaMemoryAllocate(rndisConfig.devMaxTxSize + 1);
    if (NULL == s_usbRxRndisPacketBuffer)
    {
        usb_echo("s_usbRxRndisPacketBuffer alloc failed\r\n");
    }
    s_usbTxPart1Buffer = USB_OsaMemoryAllocate(64);
    if (NULL == s_usbTxPart1Buffer)
    {
        usb_echo("s_usbTxPart1Buffer alloc failed\r\n");
    }

    NVIC_SetPriority((IRQn_Type)irqNo, USB_DEVICE_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ((IRQn_Type)irqNo);

    USB_DeviceRun(g_cdcVnic.deviceHandle);
}

/*!
 * @brief Application task function.
 *
 * This function runs the task for application.
 *
 * @return None.
 */
void APPTask(void)
{
    usb_status_t error = kStatus_USB_Error;
    if ((1 == g_cdcVnic.attach))
    {
        /* User Code */
        USB_DeviceVnicTransmit();
        USB_DeviceVnicReceive();
    }
#if USB_DEVICE_CONFIG_USE_TASK
    USB_DeviceKhciTaskFunction(g_cdcVnic.deviceHandle);
#endif
}

#if defined(__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    BOARD_InitHardware();

    APPInit();

    while (1)
    {
        APPTask();

#if USB_DEVICE_CONFIG_USE_TASK
        USB_DeviceKhciTaskFunction(g_cdcVnic.deviceHandle);
#endif
    }
}