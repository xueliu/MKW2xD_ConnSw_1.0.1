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

/******************************************************************************
 * Includes
 *****************************************************************************/
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
#include "virtual_nic_enet.h"
#if (defined(FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT > 0U))
#include "fsl_mpu.h"
#endif /* FSL_FEATURE_SOC_MPU_COUNT */
#if (!FSL_FEATURE_SOC_ENET_COUNT)
#error This application requires FSL_FEATURE_SOC_ENET_COUNT defined non-zero.
#endif
/*******************************************************************************
* Variables
******************************************************************************/
extern usb_cdc_vnic_struct_t g_cdcVnic;
/* MAC address of nic device. */
enet_address_t g_nicMacAddr;
/* Ethernet device handle. */
EnetHandle g_enetHandle;
/* Ethernet queue for nic device. */
queue_struct_t g_enetServiceQue;

/* Buffer queue of the ethernet queue. */
static vnic_enet_transfer_struct_t s_enetQBuf[ENET_QUEUE_MAX];
/* Pointer of the packet control block used by nic device. */
static uint8_t *s_nicPcb = NULL;
void USB_DeviceVnicSendPcbFree(PCB_PTR nicPcb);

/*******************************************************************************
* Code
******************************************************************************/

/*!
 * @brief Services an IP packet.
 *
 * @return ENET or error code.
 */
static void vnicEnetCallback(PCB_PTR enetPcb,
                             /* [IN] the received packet */
                             void *handle
                             /* [IN] the IP interface structure */
                             )
{
    uint32_t event;
    usb_status_t error = kStatus_USB_Error;
    vnic_enet_transfer_struct_t cdcAcmTransfer;
    cdcAcmTransfer.buffer = (uint8_t *)enetPcb;
    cdcAcmTransfer.length = 0;

    error = vnicEnetQueuePut(&g_enetServiceQue, &cdcAcmTransfer);
    if (kStatus_USB_Success != error)
    {
        error = kStatus_USB_Busy;
        PCBFree(enetPcb);
    }
    else
    {
        g_cdcVnic.nicTrafficInfo.enetRxEnet2usb++;
    }
    return;
} /* Endbody */

/*!
 * @brief Initialize the ethernet module.
 *
 * @return Error code.
 */
uint32_t vnicEnetInit(void)
{
    uint32_t error;
    bool flag = false;
    g_enetServiceQue.qArray = s_enetQBuf;
    IpAddress nicIpAddr = (IpAddress)ENET_IPADDR;

    vnicEnetQueueInit(&g_enetServiceQue, ENET_QUEUE_MAX);
    ENETGetMacAddress((uint32_t)ENET_DEVICE, nicIpAddr, g_nicMacAddr);
#if ((defined FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT))
    MPU_Enable(MPU, 0);
#endif /* FSL_FEATURE_SOC_MPU_COUNT */

    error = ENETInitialize(ENET_DEVICE, g_nicMacAddr, flag, &g_enetHandle);
    if (error != ENET_OK)
    {
        usb_echo("ENET_Initialization Failed\n");
        return ENET_ERROR;
    }

    error = ENETOpen(g_enetHandle, ENETPROT_IP, vnicEnetCallback, (void *)&g_cdcVnic);
    error |= ENETOpen(g_enetHandle, ENETPROT_ARP, vnicEnetCallback, (void *)&g_cdcVnic);
    if (error != ENET_OK)
    {
        usb_echo("ENETOpen Failed.\n");
        return ENET_ERROR;
    }
    s_nicPcb = USB_OsaMemoryAllocate(sizeof(PCB) + sizeof(PCB_FRAGMENT));
    if (NULL == s_nicPcb)
    {
        usb_echo("s_nicPcb alloc fail\r\n");
    }

    return error;
}

/*!
 * @brief Send packets to Ethernet module.
 *
 * @return Error code.
 */
enet_status_t vnicEnetSend(void *arg, uint8_t *ethernetFrame, uint32_t payloadSize)
{
    uint16_t protocol;
    uint8_t *nicPcb = NULL;
    uint32_t error;
    PCB_FRAGMENT_PTR frag = NULL;
    vnic_enet_transfer_struct_t nicPcbParam;

    protocol = *((uint16_t *)(((ENET_HEADER_PTR)ethernetFrame)->TYPE));
    protocol = USB_SHORT_FROM_BIG_ENDIAN(protocol);

    nicPcb = s_nicPcb;

    frag = ((PCB_PTR)nicPcb)->FRAG;
    ((PCB_PTR)nicPcb)->FREE = (void (*)(PCB_PTR))USB_DeviceVnicSendPcbFree;
    ((PCB_PTR)nicPcb)->PRIVATE = NULL;
    frag[0].LENGTH = payloadSize;

    frag[0].FRAGMENT = ethernetFrame;

    frag[1].LENGTH = 0;
    frag[1].FRAGMENT = NULL;

    error = ENETSend(g_enetHandle, (PCB_PTR)nicPcb, protocol, ((ENET_HEADER_PTR)ethernetFrame)->DEST,
                     0); /* flag passed as Zero */

    return (enet_status_t)error;
}

/*!
 * @brief Clear the transfer requests in enet queue.
 *
 * @return Error code.
 */
usb_status_t vnicEnetClearEnetQueue()
{
    usb_status_t error = kStatus_USB_Success;
    vnic_enet_transfer_struct_t cdcAcmTransfer;
    PCB_PTR enetPcb;
    while (kStatus_USB_Success == error)
    {
        error = vnicEnetQueueGet(&g_enetServiceQue, &cdcAcmTransfer);
        if (kStatus_USB_Success == error)
        {
            enetPcb = (PCB_PTR)(cdcAcmTransfer.buffer);
            PCBFree(enetPcb);
            g_cdcVnic.nicTrafficInfo.enetRxUsb2hostCleared++;
        }
    }
    return error;
}
