/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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

#ifndef _FSL_ENET_RTCS_ADAPTOR_H_
#define _FSL_ENET_RTCS_ADAPTOR_H_

#include "fsl_enet_hal.h"
#include "fsl_enet_driver.h"
#include "pcb.h"

/*******************************************************************************
* Definitions
******************************************************************************/
/*!
 * @addtogroup enet_rtcs_adaptor
 * @{
 */

#define ENET_PCB_NUM (16)

/*! @brief Definitions of the configuration parameter*/
#define ENET_RXBD_NUM (6) // 8
#define ENET_TXBD_NUM (3) // 4
#define ENET_RXRTCSBUFF_NUM (ENET_RXBD_NUM)

#if !ENET_RECEIVE_ALL_INTERRUPT
#define ENET_EXTRXBD_NUM (4)
#define ENET_RECEIVE_TASK_PRIO (1)
#define ENET_TASK_STACK_SIZE (800)

#endif

#define ENET_RXBuff_SIZE (kEnetMaxFrameSize)
#define ENET_TXBuff_SIZE (kEnetMaxFrameSize)

#define ENET_RX_BUFFER_ALIGNMENT (16)
#define ENET_TX_BUFFER_ALIGNMENT (16)
#define ENET_BD_ALIGNMENT (16)
#define ENET_RXBuffSizeAlign(n) ENET_ALIGN(n, ENET_RX_BUFFER_ALIGNMENT)
#define ENET_TXBuffSizeAlign(n) ENET_ALIGN(n, ENET_TX_BUFFER_ALIGNMENT)

#if FSL_FEATURE_ENET_SUPPORT_PTP
#define ENET_PTP_TXTS_RING_LEN (25)
#define ENET_PTP_RXTS_RING_LEN (25)

#endif

/*! @brief Definitions of the error codes */
#define ENET_OK (0)
#define ENET_ERROR (0xFF) /* General ENET error */

#define ENETERR_INVALID_DEVICE (kStatus_ENET_InvalidDevice) /* Device number out of range  */
#define ENETERR_INIT_DEVICE (kStatus_ENET_Initialized)      /* Device already initialized  */

#define ENET_FRAMESIZE_MAXDATA (1500)

/*! @brief Definitions of the ENET protocol parameter*/
#define ENETPROT_IP 0x0800
#define ENETPROT_ARP 0x0806
#define ENETPROT_8021Q 0x8100
#define ENETPROT_IP6 0x86DD
#define ENETPROT_ETHERNET 0x88F7

#define ENET_OPT_8023 0x0001
#define ENET_OPT_8021QTAG 0x0002 /* http://en.wikipedia.org/wiki/IEEE_802.1Q */
#define ENET_SETOPT_8021QPRIO(priority) (((uint32_t)(priority)&0x7U) << 2U)
#define ENET_GETOPT_8021QPRIO(flags) (((flags) >> 2U) & 0x7U)
#define ENET_SETOPT_8021QVID(vlanId) (((uint32_t)(vlanId)&0xFFFU) << 5U)
#define ENET_GETOPT_8021QVID(flags) (((flags) >> 5U) & 0xFFFU)

/*! @brief Definitions of the ENET option macro*/
#define ENET_OPTION_HW_TX_IP_CHECKSUM 0x00001000
#define ENET_OPTION_HW_TX_PROTOCOL_CHECKSUM 0x00002000
#define ENET_OPTION_HW_RX_IP_CHECKSUM 0x00004000
#define ENET_OPTION_HW_RX_PROTOCOL_CHECKSUM 0x00008000
#define ENET_OPTION_HW_RX_MAC_ERR 0x00010000

/*! @brief Definitions of the ENET default Mac*/
#define ENET_DEFAULT_MAC_ADD      \
    {                             \
        0x00, 0x00, 0x5E, 0, 0, 0 \
    }
#define PCB_MINIMUM_SIZE (sizeof(PCB2))
#define PCBFree(pcb) ((pcb)->FREE(pcb))

#define htone(p, x) \
    ((p)[0] = (x)[0], (p)[1] = (x)[1], (p)[2] = (x)[2], (p)[3] = (x)[3], (p)[4] = (x)[4], (p)[5] = (x)[5])

#define ntohe(p, x)                                                                                      \
    ((x)[0] = (p)[0] & 0xFFU, (x)[1] = (p)[1] & 0xFFU, (x)[2] = (p)[2] & 0xFFU, (x)[3] = (p)[3] & 0xFFU, \
     (x)[4] = (p)[4] & 0xFFU, (x)[5] = (p)[5] & 0xFFU)

/*! @brief Definitions of the add to queue*/
#define QUEUEADD(head, tail, pcb) \
    if ((head) == NULL)           \
    {                             \
        (head) = (pcb);           \
    }                             \
    else                          \
    {                             \
        (tail)->PRIVATE = (pcb);  \
    }                             \
    (tail) = (pcb);               \
    (pcb)->PRIVATE = NULL

/*! @brief Definitions of the get from queue*/
#define QUEUEGET(head, tail, pcb) \
    (pcb) = (head);               \
    if (head)                     \
    {                             \
        (head) = (head)->PRIVATE; \
        if ((head) == NULL)       \
        {                         \
            (tail) = NULL;        \
        }                         \
    }

/*! @brief Definition for ENET six-byte Mac type*/
typedef unsigned char EnetAddress[6];

/*! @brief Definition of the IPCFG structure*/
typedef void *EnetHandle;

/*! @brief Definition of the Ethernet packet header structure*/
typedef struct enetHeader
{
    EnetAddress DEST;      /*!< destination Mac address*/
    EnetAddress SOURCE;    /*!< source Mac address*/
    unsigned char TYPE[2]; /*!< protocol type*/
} ENET_HEADER, *ENET_HEADER_PTR;

/*! @brief Definition of the 8021 tag header*/
typedef struct enet_8021qtagHeader
{
    unsigned char TAG[2];  /*!< ENET 8021tag header tag region*/
    unsigned char TYPE[2]; /*!< ENET 8021tag header type region*/
} ENET_8021QTAG_HEADER, *ENET_8021QTAG_HEADER_PTR;

/*! @brief Definition of the 8022 header*/
typedef struct enet_8022Header
{
    uint8_t dsap[1];    /*!< DSAP region*/
    uint8_t ssap[1];    /*!< SSAP region*/
    uint8_t command[1]; /*!< Command region*/
    uint8_t oui[3];     /*!< OUI region*/
    uint16_t type;      /*!< type region*/
} ENET_8022_HEADER, *ENET_8022_HEADER_PTR;

/*! @brief Definition of the two fragment PCB structure*/
typedef struct pcbQueue
{
    PCB *pcbHead; /*!< PCB buffer head*/
    PCB *pcbTail; /*!< PCB buffer tail*/
} pcbQueue;

/*! @brief Definition of the ECB structure, which contains the protocol type and it's related service function*/
typedef struct ENETEcbStruct
{
    uint16_t TYPE;
    void (*SERVICE)(PCB_PTR, void *);
    void *PRIVATE;
    struct ENETEcbStruct *NEXT;
} enet_ecb_struct_t;

/*! @brief Definition of the  common status structure*/
typedef struct enetCommomStatsStruct
{
    uint32_t ST_RX_TOTAL;     /*!< Total number of received packets*/
    uint32_t ST_RX_MISSED;    /*!<  Number of missed packets*/
    uint32_t ST_RX_DISCARDED; /*!< Discarded a protocol that was not recognized*/
    uint32_t ST_RX_ERRORS;    /*!< Discarded error during reception*/
    uint32_t ST_TX_TOTAL;     /*!< Total number of transmitted packets*/
    uint32_t ST_TX_MISSED;    /*!< Discarded transmit ring full*/
    uint32_t ST_TX_DISCARDED; /*!< Discarded bad packet*/
    uint32_t ST_TX_ERRORS;    /*!< Error during transmission*/
} ENET_COMMON_STATS_STRUCT, *ENET_COMMON_STATS_STRUCT_PTR;

typedef struct enetStats
{
    ENET_COMMON_STATS_STRUCT COMMON; /*!< Common status structure*/
    uint32_t ST_RX_ALIGN;            /*!< Frame Alignment error*/
    uint32_t ST_RX_FCS;              /*!< CRC error  */
    uint32_t ST_RX_RUNT;             /*!< Runt packet received */
    uint32_t ST_RX_GIANT;            /*!< Giant packet received*/
    uint32_t ST_RX_LATECOLL;         /*!< Late collision */
    uint32_t ST_RX_OVERRUN;          /*!< DMA overrun*/
    uint32_t ST_TX_SQE;              /*!< Heartbeat lost*/
    uint32_t ST_TX_DEFERRED;         /*!< Transmission deferred*/
    uint32_t ST_TX_LATECOLL;         /*!< Late collision*/
    uint32_t ST_TX_EXCESSCOLL;       /*!< Excessive collisions*/
    uint32_t ST_TX_CARRIER;          /*!< Carrier sense lost*/
    uint32_t ST_TX_UNDERRUN;         /*!< DMA underrun*/
    /* Following stats are collected by the Ethernet driver  */
    uint32_t ST_RX_COPY_SMALL; /*!< Driver had to copy packet */
    uint32_t ST_RX_COPY_LARGE; /*!< Driver had to copy packet */
    uint32_t ST_TX_COPY_SMALL; /*!< Driver had to copy packet */
    uint32_t ST_TX_COPY_LARGE; /*!< Driver had to copy packet */
    uint32_t RX_FRAGS_EXCEEDED;
    uint32_t RX_PCBS_EXHAUSTED;
    uint32_t RX_LARGE_BUFFERS_EXHAUSTED;
    uint32_t TX_ALIGNED;
    uint32_t TX_ALL_ALIGNED;
#if BSPCFG_ENABLE_ENET_HISTOGRAM
    uint32_t RX_HISTOGRAM[ENET_HISTOGRAM_ENTRIES];
    uint32_t TX_HISTOGRAM[ENET_HISTOGRAM_ENTRIES];
#endif

} ENET_STATS, *ENET_STATS_PTR;

/*******************************************************************************
* API
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
  * @name ENET RTCS ADAPTOR
  * @{
  */

/*!
* @brief Initializes the ENET device.
*
* @param device The ENET device number.
* @param address The hardware address.
* @param flag The flag for upper layer.
* @param handle The address pointer for ENET device structure.
* @return The execution status.
*/
extern uint32_t ENETInitialize(uint32_t device, EnetAddress address, uint32_t flag, EnetHandle *handle);

/*!
 * @brief Opens the ENET device.
 *
 * @param handle The address pointer for ENET device structure.
 * @param type The ENET protocol type.
 * @param service The service function for type.
 * @param private The private data for ENET device.
 * @return The execution status.
 */
extern uint32_t ENETOpen(EnetHandle handle, uint16_t type, void (*service)(PCB_PTR, void *), void *private);

/*!
 * @brief Shuts down the ENET device.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The execution status.
 */
extern uint32_t ENETShutdown(EnetHandle handle);
#if !ENET_RECEIVE_ALL_INTERRUPT
/*!
 * @brief ENET frame receive.
 *
 * @param enetIf The address pointer for ENET device structure.
 */
static void ENETReceive(task_param_t param);
#endif
/*!
 * @brief ENET frame transmit.
 *
 * @param handle The address pointer for ENET device structure.
 * @param packet The ENET packet buffer.
 * @param type The ENET protocol type.
 * @param dest The destination hardware address.
 * @param flag The flag for upper layer.
 * @return The execution status.
 */
uint32_t ENETSend(EnetHandle handle, PCB_PTR packet, uint32_t type, EnetAddress dest, uint32_t flags);

/*!
 * @brief The ENET gets the address with the initialized device.
 *
 * @param handle The address pointer for ENET device structure.
 * @param address The destination hardware address.
 * @return The execution status.
 */
extern uint32_t ENETGetAddress(EnetHandle handle, EnetAddress address);

/*!
 * @brief The ENET gets the address with an uninitialized device.
 *
 * @param handle The address pointer for ENET device structure.
 * @param value The value to change the last three bytes of hardware.
 * @param address The destination hardware address.
 * @return True if the execution status is success else false.
 */
extern uint32_t ENETGetMacAddress(uint32_t device, uint32_t value, EnetAddress address);
/*!
 * @brief The ENET joins a multicast group address.
 *
 * @param handle The address pointer for ENET device structure.
 * @param type The ENET protocol type.
 * @param address The destination hardware address.
 * @return The execution status.
 */
extern uint32_t ENETJoin(EnetHandle handle, uint16_t type, EnetAddress address);

/*!
 * @brief The ENET leaves a multicast group address.
 *
 * @param handle The address pointer for ENET device structure.
 * @param type The ENET protocol type.
 * @param address The destination hardware address.
 * @return The execution status.
 */
extern uint32_t ENETLeave(EnetHandle handle, uint16_t type, EnetAddress address);
#if BSPCFG_ENABLE_ENET_STATS
/*!
 * @brief The ENET gets the packet statistic.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The statistic.
 */
extern ENET_STATS_PTR ENETGetStats(EnetHandle handle);
#endif
/*!
 * @brief The ENET gets the link status.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The link status.
 */
extern bool ENETLinkStatus(EnetHandle handle);

/*!
 * @brief The ENET gets the link speed.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The link speed.
 */
extern uint32_t ENETGetSpeed(EnetHandle handle);

/*!
 * @brief The ENET gets the MTU.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The link MTU
 */
extern uint32_t ENETGet_MTU(EnetHandle handle);

/*!
 * @brief Gets the ENET PHY registers.
 *
 * @param handle The address pointer for ENET device structure.
 * @param numRegs The number of registers.
 * @param reg The buffer for data read from PHY registers.
 * @return True if all numRegs registers are read succeed else false.
 */
extern bool ENETPhyRegisters(EnetHandle handle, uint32_t numRegs, uint32_t *reg);

/*!
 * @brief Gets ENET options.
 *
 * @param handle The address pointer for ENET device structure.
 * @return ENET options.
 */
extern uint32_t ENETGetOptions(EnetHandle handle);

/*!
 * @brief Unregisters a protocol type on an Ethernet channel.
 *
 * @param handle The address pointer for ENET device structure.
 * @return ENET options.
 */
extern uint32_t ENETClose(EnetHandle handle, uint16_t type);

/*!
 * @brief ENET mediactl.
 *
 * @param handle The address pointer for ENET device structure.
 * @param The command ID.
 * @param The buffer for input or output parameters.
 * @return ENET options.
 */
extern uint32_t ENETMediactl(EnetHandle handle, uint32_t commandId, void *inOutParam);

/*!
 * @brief Gets the next ENET device handle address.
 *
 * @param handle The address pointer for ENET device structure.
 * @return The address of next ENET device handle.
 */
extern EnetHandle ENETGetNextDeviceHandle(EnetHandle handle);

/*!
 * @brief ENET free.
 *
 * @param packet The buffer address.
 */
extern void ENETFree(PCB_PTR packet);

/*!
 * @brief ENET error description.
 *
 * @param error The ENET error code.
 * @return The error string.
 */
extern const char *ENETStrerror(uint32_t error);

/*!
 * @brief Static inline function to send out data and return error code.
 *
 * @param
 * @return Error code.
 */
static inline uint32_t enetSend(enet_dev_if_t *enetIf, uint32_t dataLen, uint32_t bdNumUsed, uint32_t *error)
{
    *error = ENET_DRV_SendData(enetIf, dataLen, bdNumUsed);
    return *error;
}

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_ENET_RTCS_ADAPTOR_H_ */
