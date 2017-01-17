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

#ifndef _VIRTUAL_NIC_ENET_H
#define _VIRTUAL_NIC_ENET_H 1

#include "virtual_nic.h"
/*******************************************************************************
* Definitions
******************************************************************************/
typedef uint32_t IpAddress;
typedef uint8_t enet_address_t[6];

#define BOARD_DEBUG_ENET_INSTANCE (0)
#define ENET_DEVICE (BOARD_DEBUG_ENET_INSTANCE)
/* USB header size of RNDIS packet in bytes. */
#define RNDIS_USB_OVERHEAD_SIZE (44)
/* Specifies the offset in bytes from the start of the DataOffset field of rndis_packet_msg_struct_t to the start of the
 * data. */
#define RNDIS_DATA_OFFSET (36)
/* MAC address size in bytes. */
#define MAC_ADDR_SIZE (6)

/* Helpful macros to convert ip address. */
#define IPBYTES(a) ((a >> 24U) & 0xFFU), ((a >> 16U) & 0xFFU), ((a >> 8U) & 0xFFU), (a & 0xFFU)
#define IPADDR(a, b, c, d)                                                                            \
    ((uint32_t)(a & 0xFFU) << 24U) | (uint32_t)((b & 0xFFU) << 16U) | (uint32_t)((c & 0xFFU) << 8U) | \
        (uint32_t)(d & 0xFFU)

#ifndef ENET_IPADDR
#define ENET_IPADDR IPADDR(192, 168, 1, 109) /* Default IP Address */
#endif

#ifndef ENET_IPMASK
#define ENET_IPMASK IPADDR(255, 255, 255, 0)
#endif

/* Disable interrupt to enter critical section. */
#define USB_DEVICE_VNIC_ENTER_CRITICAL() \
    USB_OSA_SR_ALLOC();                  \
    USB_OSA_ENTER_CRITICAL()

/* Enable interrupt to exit critical section. */
#define USB_DEVICE_VNIC_EXIT_CRITICAL() USB_OSA_EXIT_CRITICAL()

/* ENET queue size. */
#define ENET_QUEUE_MAX (16)
/* Queue specific implementation */
#define VNIC_ENET_QUEUE_MQX (16)

/* Define VNIC_ENET transfer struct */
typedef struct VnicEnetTransferStruct
{
    uint8_t *buffer;
    uint32_t length;
} vnic1enet1transfer1struct1t;

/* Define VNIC_ENET queue struct */
typedef struct QueueStruct
{
    uint32_t head;
    uint32_t tail;
    uint32_t maxSize;
    uint32_t curSize;
    usb_osa_mutex_handle mutex;
    vnic_enet_transfer_struct_t *qArray;
} queue_struct_t;

/*******************************************************************************
* API
******************************************************************************/

/*!
 * @brief Initialize the queue.
 *
 * @return Error code.
 */
static inline usb_status_t vnicEnetQueueInit(queue_struct_t *q, uint32_t maxSize)
{
    usb_status_t error = kStatus_USB_Error;
    USB_DEVICE_VNIC_ENTER_CRITICAL();
    (q)->head = 0;
    (q)->tail = 0;
    (q)->maxSize = maxSize;
    (q)->curSize = 0;
    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&((q)->mutex)))
    {
        usb_echo("queue mutex create error!");
    }
    error = kStatus_USB_Success;
    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return error;
}

/*!
 * @brief Put element into the queue.
 *
 * @return Error code.
 */
static inline usb_status_t vnicEnetQueuePut(queue_struct_t *q, vnic_enet_transfer_struct_t *e)
{
    usb_status_t error = kStatus_USB_Error;
    USB_DEVICE_VNIC_ENTER_CRITICAL();
    if ((q)->curSize < (q)->maxSize)
    {
        (q)->qArray[(q)->head++] = *(e);
        if ((q)->head == (q)->maxSize)
        {
            (q)->head = 0;
        }
        (q)->curSize++;
        error = kStatus_USB_Success;
    }
    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return error;
}

/*!
 * @brief Get element from the queue.
 *
 * @return Error code.
 */
static inline usb_status_t vnicEnetQueueGet(queue_struct_t *q, vnic_enet_transfer_struct_t *e)
{
    usb_status_t error = kStatus_USB_Error;
    USB_DEVICE_VNIC_ENTER_CRITICAL();
    if ((q)->curSize)
    {
        *(e) = (q)->qArray[(q)->tail++];
        if ((q)->tail == (q)->maxSize)
        {
            (q)->tail = 0;
        }
        (q)->curSize--;
        error = kStatus_USB_Success;
    }
    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return error;
}

/*!
 * @brief Delete the queue.
 *
 * @return Error code.
 */
static inline usb_status_t vnicEnetQueueDelete(queue_struct_t *q)
{
    usb_status_t error = kStatus_USB_Error;
    USB_DEVICE_VNIC_ENTER_CRITICAL();
    (q)->head = 0;
    (q)->tail = 0;
    (q)->maxSize = 0;
    (q)->curSize = 0;
    error = kStatus_USB_Success;
    USB_DEVICE_VNIC_EXIT_CRITICAL();
    return error;
}

/*!
 * @brief Check if the queue is empty.
 *
 * @return 1: queue is empty, 0: not empty.
 */
static inline uint8_t vnicEnetQueueIsEmpty(queue_struct_t *q)
{
    return ((q)->curSize == 0) ? 1 : 0;
}

/*!
 * @brief Check if the queue is full.
 *
 * @return 1: queue is full, 0: not full.
 */
static inline uint8_t vnicEnetQueueIsFull(queue_struct_t *q)
{
    return ((q)->curSize >= (q)->maxSize) ? 1 : 0;
}

/*!
 * @brief Get the size of the queue.
 *
 * @return Size of the quue.
 */
static inline uint32_t vnicEnetQueueSize(queue_struct_t *q)
{
    return (q)->curSize;
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

/*!
 * @brief Initialize the ethernet module.
 *
 * @return Error code.
 */
extern uint32_t vnicEnetInit(void);

/*!
 * @brief Clear the transfer requests in enet queue.
 *
 * @return Error code.
 */
extern usb_status_t vnicEnetClearEnetQueue();

/*!
 * @brief Send packets to Ethernet module.
 *
 * @return Error code.
 */
extern enet_status_t vnicEnetSend(void *arg, uint8_t *ethernetFrame, uint32_t payloadSize);
#endif /* _VIRTUAL_NIC_ENET_H */
