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
#ifndef _APP_ETHERNET_USB_H
#define _APP_ETHERNET_USB_H


/*!=================================================================================================
\file       app_ethernet_usb.h
\brief      This is a header file for the ENET Media interface configuration module.
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include"network_utils.h"

/*==================================================================================================
Public macros
==================================================================================================*/

/*==================================================================================================
Public type definitions
==================================================================================================*/

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

extern taskMsgQueue_t *gpUSBEthTaskMsgQueue;

/*==================================================================================================
Public function prototypes
==================================================================================================*/
/*!*************************************************************************************************
\fn     IFUSBENET_Start
\brief  This is a public function used to start the USB Ethernet IP Media Interface.

\param  [in] pTaskMsgQueue  task message queue
\param  [in] thrInstanceID  Thread Instance ID

\return void
***************************************************************************************************/
void IFUSBENET_Start(taskMsgQueue_t* pTaskMsgQueue, instanceId_t thrInstanceID);

/*!*************************************************************************************************
\fn     IFUSBENET_ThreadStarted
\brief  This is a public function which handles the steps that should be done after a device with
        USB Ethernet functionality transitions to Thread Leader.

\param  [in] thrInstanceId  instance ID of the Thread interface

\return void
***************************************************************************************************/
void IFUSBENET_ThreadStarted(uint32_t thrInstanceId);
/*!*************************************************************************************************
\fn    void NET_RaReceived()
\brief  This function is used to handle Ra received.

\param  [in] pEvent      pointer to the event

\return void
***************************************************************************************************/
void NET_RaReceived(void *pEvent);
/*!*************************************************************************************************
\fn    void NET_RaRouteInfoReceived()
\brief  This function is used to handle Ra route info event received.

\param  [in] pEvent      pointer to the event

\return void
***************************************************************************************************/
void NET_RaRouteInfoReceived(void *pEvent);
/*!*************************************************************************************************
\fn    void NET_RaDnsInfoReceived()
\brief  This function is used to handle Ra dns info event received.

\param  [in] pEvent      pointer to the event

\return void
***************************************************************************************************/
#if ND_OPT_DNS_RCV_ENABLED
    void NET_RaDnsInfoReceived(void *pEvent);
#endif
#endif /* _APP_ETHERNET_USB_H */
