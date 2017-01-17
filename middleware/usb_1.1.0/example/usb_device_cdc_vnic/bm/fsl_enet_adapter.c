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
#include "fsl_enet_driver.h"
#include "fsl_enet_hal.h"
#include "fsl_phy_driver.h"
#include "fsl_enet_adapter.h"
#include "clock_config.h"
#include "fsl_interrupt_manager.h"
#include "fsl_os_abstraction.h"
#include <string.h>

//#include "bsp.h"
/*******************************************************************************
* Variables
******************************************************************************/
static enet_dev_if_t enetDevIf[ENET_INSTANCE_COUNT];
static enet_config_rmii_t rmiiCfg = {kEnetCfgRmii, kEnetCfgSpeed100M, kEnetCfgFullDuplex, false, false};
static enet_mac_config_t g_enetMacCfg[ENET_INSTANCE_COUNT] = {
    /* ENET0 */
    {
        kEnetMacNormalMode, /*!< ENET normal mode*/
        NULL,               /*!< ENET mac address*/
        &rmiiCfg,           /*!< ENET rmii interface*/
                            /*!< enet mac control flag recommended to use enet_mac_control_flag_t
                              it is special control for loop mode, sleep mode, crc forward/terminate etc*/
        kEnetRxCrcFwdEnable | kEnetTxCrcBdEnable | kEnetMacEnhancedEnable
#if BSPCFG_ENET_HW_TX_IP_CHECKSUM || BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
            | kEnetTxAccelEnable
#endif
#if BSPCFG_ENET_HW_RX_IP_CHECKSUM || BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM || BSPCFG_ENET_HW_RX_MAC_ERR
            | kEnetRxAccelEnable
#endif
        ,
        NULL, /*!< ENET Rx FIFO threshold*/
        NULL, /*!< ENET Tx FIFO threshold*/
        0     /*!< ENET rxaccelerator config*/
#if BSPCFG_ENET_HW_RX_IP_CHECKSUM
            | kEnetRxAccelIpCheckEnabled /* Discard with wrong IP header checksum */
#endif
#if BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM
            | kEnetRxAccelProtoCheckEnabled /* Discard with wrong protocol checksum*/
#endif
#if BSPCFG_ENET_HW_RX_MAC_ERR
            | kEnetRxAccelMacCheckEnabled /* Discard with Mac layer errors*/
#endif
        ,
        0 /*!< ENET txaccelerator config*/
#if BSPCFG_ENET_HW_TX_IP_CHECKSUM
            | kEnetTxAccelIpCheckEnabled /* Insert IP header checksum */
#endif
#if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
            | kEnetTxAccelProtoCheckEnabled /* Insert protocol checksum*/
#endif
        ,
        0,    /*!< ENET Pause duration*/
        NULL, /*!< ENET Special configure for MAC*/
#if FSL_FEATURE_ENET_SUPPORT_PTP
        false,
#endif
    },
};

#define BSP_ENET0_PHY_ADDR (0)
/* Configuration for PHY.*/
static const enet_phy_config_t g_enetPhyCfg[ENET_INSTANCE_COUNT] = {{false /* PHY address auto discover*/
                                                                     ,
                                                                     BSP_ENET0_PHY_ADDR /* PHY address*/
                                                                     ,
                                                                     false}}; /* Switcher to enable the PHY loop mode*/

/* ENET error describe list*/
static const char *ENETErrlist[kStatus_ENET_AlreadyAddedMulticast - kStatus_ENET_InvalidInput + 1] = {
    "Invaild ENET input parameter"           /* kStatus_ENET_InvalidInput*/
    "Invalid ENET device",                   /* kStatus_ENET_InvalidDevice */
    "Memory allocate failure",               /* kStatus_ENET_MemoryAllocateFail */
    "Get clock frequency failure",           /* kStatus_ENET_GetClockFreqFail */
    "ENET device already initialized",       /* kStatus_ENET_Initialized */
    "Open ENET device",                      /* kStatus_ENET_Open */
    "Close ENET device failure",             /* kStatus_ENET_Close*/
    "NULL L2 PTP buffer queue pointer",      /* kStatus_ENET_Layer2QueueNull*/
    "Layer2 packet length over large",       /* kStatus_ENET_Layer2OverLarge*/
    "Layer2 packet buffer full",             /* kStatus_ENET_Layer2BufferFull*/
    "Layer2 packet error type",              /* kStatus_ENET_Layer2TypeError */
    "PTP ring buffer full",                  /* kStatus_ENET_PtpringBufferFull*/
    "PTP ring buffer empty",                 /* kStatus_ENET_PtpringBufferEmpty */
    "MII uninitialized",                     /* kStatus_ENET_Miiuninitialized*/
    "Receive buffer descriptor invalid",     /* kStatus_ENET_RxbdInvalid */
    "Receive buffer descriptor empty",       /* kStatus_ENET_RxbdEmpty */
    "Receive buffer descriptor truncate",    /* kStatus_ENET_RxbdTrunc */
    "Receive buffer descriptor error",       /* kStatus_ENET_RxbdError  */
    "Receive buffer descriptor full",        /* kStatus_ENET_RxBdFull  */
    "Small receive buffer size",             /* kStatus_ENET_SmallBdSize*/
    "Receive large buffer full",             /* kStatus_ENET_LargeBufferFull */
    "Transmit large packet",                 /* kStatus_ENET_TxLarge */
    "Transmit buffer descriptor full",       /* kStatus_ENET_TxbdFull*/
    "Transmit buffer descriptor Null",       /* kStatus_ENET_TxbdNull*/
    "Transmit data buffer Null",             /* kStatus_ENET_TxBufferNull*/
    "No more receive buffer left",           /* kStatus_ENET_NoRxBufferLeft */
    "Invalid ENET PTP IOCTL command",        /* kStatus_ENET_UnknownCommand*/
    "ENET Timeout",                          /* kStatus_ENET_TimeOut*/
    "Null multicast group pointer",          /* kStatus_ENET_MulticastPointerNull */
    "No multicast group address",            /* kStatus_ENET_NoMulticastAdd */
    "Have Already added to multicast group", /* kStatus_ENET_AlreadyAddedMulticast */
};

static uint8_t *txBd[ENET_INSTANCE_COUNT];
static uint8_t *rxBd[ENET_INSTANCE_COUNT];
static uint8_t *txBuffer[ENET_INSTANCE_COUNT];
static uint8_t *rxBuffer[ENET_INSTANCE_COUNT];
static uint8_t *rxRtcsBuffer[ENET_INSTANCE_COUNT];
static pcbQueue packbuffer[ENET_INSTANCE_COUNT];
static PCB2 *pcb[ENET_INSTANCE_COUNT];
static uint8_t *dataBuffQue;

#if FSL_FEATURE_ENET_SUPPORT_PTP
static enet_mac_ptp_ts_data_t ptpTsRxData[ENET_PTP_RXTS_RING_LEN];
static enet_mac_ptp_ts_data_t ptpTsTxData[ENET_PTP_TXTS_RING_LEN];
#endif

#if !ENET_RECEIVE_ALL_INTERRUPT
static bool frameIsCollected = true;
#else
static bool frameIsCollected = false;
#endif
#if BSPCFG_ENABLE_ENET_STATS
static ENET_STATS enetStats;
#endif
#if !ENET_RECEIVE_ALL_INTERRUPT
static uint8_t *rxExtBuffer[ENET_INSTANCE_COUNT];
static OSA_TASK_DEFINE(ENETReceive, ENET_TASK_STACK_SIZE);
#endif
/*******************************************************************************
* Code
******************************************************************************/
/*
 * There are global variables in this file, that are shared between ENET Rx isr (ENETFindReceiver) or
 * ENETReceiveTask
 * and TCP/IP task (ENETFree).
 * Namely: packbuffer, dataBuffQue, frameIsCollected
 *
 * Therefore they need to be protected against simultaneous modifications. (race condition).
*/
static void enetCriticalBegin(uint8_t hwEnetInstance)
{
#if ENET_RECEIVE_ALL_INTERRUPT
    // IntDisable();
    ENET_HAL_SetIntMode(g_enetBase[hwEnetInstance], kEnetRxFrameInterrupt, false);
#else
    TaskStopPreemption();
#endif
}
static void enetCriticalEnd(uint8_t hwEnetInstance)
{
#if ENET_RECEIVE_ALL_INTERRUPT
    // IntEnable();
    ENET_HAL_SetIntMode(g_enetBase[hwEnetInstance], kEnetRxFrameInterrupt, true);
#else
    TaskStartPreemption();
#endif
}

/*******************************************************************************
 * Code
 ******************************************************************************/
extern void IPERecv_IP(PCB_PTR pcb, void *handle);
extern void IPERecv_ARP(PCB_PTR pcb, void *handle);
#if RTCSCFG_ENABLE_IP6
extern void IP6ERecv_IP(PCB_PTR pcb, void *handle);
#endif

/*!
 * @brief Initialize enet mac buffer.
 *
 * @return The execution status.
 */
uint32_t ENETBufferInit(enet_dev_if_t *enetIf, enet_buff_config_t *buffCfg)
{
    uint32_t rxBufferSizeAlign, txBufferSizeAlign, rxLargeBufferSizeAlign;
    uint8_t count, *txBufferAlign, *rxBufferAlign, *rxLargeBufferAlign;
    volatile enet_bd_struct_t *txBdPtrAlign, *rxBdPtrAlign;
    PCB2 *pcbbuffer;

    /* Check input parameter*/
    if ((!enetIf) || (!buffCfg))
    {
        return kStatus_ENET_InvalidInput;
    }

    /* Allocate ENET receive buffer descriptors*/
    txBd[enetIf->deviceNumber] =
        (uint8_t *)OSA_MemAllocZero(ENET_TXBD_NUM * sizeof(enet_bd_struct_t) + ENET_BD_ALIGNMENT);
    if (!txBd[enetIf->deviceNumber])
    {
        return kStatus_ENET_MemoryAllocateFail;
    }
    txBdPtrAlign = (volatile enet_bd_struct_t *)ENET_ALIGN((uint32_t)txBd[enetIf->deviceNumber], ENET_BD_ALIGNMENT);

    rxBd[enetIf->deviceNumber] =
        (uint8_t *)OSA_MemAllocZero(ENET_RXBD_NUM * sizeof(enet_bd_struct_t) + ENET_BD_ALIGNMENT);
    if (!rxBd[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
        return kStatus_ENET_MemoryAllocateFail;
    }
    rxBdPtrAlign = (volatile enet_bd_struct_t *)ENET_ALIGN((uint32_t)rxBd[enetIf->deviceNumber], ENET_BD_ALIGNMENT);

    /* Allocate the transmit and receive date buffers*/
    rxBufferSizeAlign = ENET_RXBuffSizeAlign(ENET_RXBuff_SIZE);
    rxBuffer[enetIf->deviceNumber] =
        (uint8_t *)OSA_MemAllocZero(ENET_RXBD_NUM * rxBufferSizeAlign + ENET_RX_BUFFER_ALIGNMENT);
    if (!rxBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBd[enetIf->deviceNumber]);
        return kStatus_ENET_MemoryAllocateFail;
    }
    rxBufferAlign = (uint8_t *)ENET_ALIGN((uint32_t)rxBuffer[enetIf->deviceNumber], ENET_RX_BUFFER_ALIGNMENT);

    txBufferSizeAlign = ENET_RXBuffSizeAlign(ENET_TXBuff_SIZE);
    txBuffer[enetIf->deviceNumber] = OSA_MemAllocZero(ENET_TXBD_NUM * txBufferSizeAlign + ENET_TX_BUFFER_ALIGNMENT);
    if (!txBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBuffer[enetIf->deviceNumber]);
        return kStatus_ENET_MemoryAllocateFail;
    }
    txBufferAlign = (uint8_t *)ENET_ALIGN((uint32_t)txBuffer[enetIf->deviceNumber], ENET_TX_BUFFER_ALIGNMENT);

    /*Initialize the data buffer queue for rtcs */
    rxLargeBufferSizeAlign = ENET_ALIGN(kEnetMaxFrameVlanSize, ENET_RX_BUFFER_ALIGNMENT);
    rxRtcsBuffer[enetIf->deviceNumber] =
        OSA_MemAllocZero(ENET_RXRTCSBUFF_NUM * rxLargeBufferSizeAlign + ENET_RX_BUFFER_ALIGNMENT);
    if (!rxRtcsBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBuffer[enetIf->deviceNumber]);
        OSA_MemFree(txBuffer[enetIf->deviceNumber]);
        return kStatus_ENET_MemoryAllocateFail;
    }
    /* Store for buffer free*/
    rxLargeBufferAlign = (uint8_t *)ENET_ALIGN((uint32_t)rxRtcsBuffer[enetIf->deviceNumber], ENET_RX_BUFFER_ALIGNMENT);
    dataBuffQue = NULL;
    for (count = 0; count < ENET_RXRTCSBUFF_NUM; count++)
    {
        enetMacEnqueueBuffer((void **)&dataBuffQue, rxLargeBufferAlign);
        rxLargeBufferAlign += rxLargeBufferSizeAlign;
    }

    /* Initialize PCB buffer*/
    pcb[enetIf->deviceNumber] = (PCB2 *)OSA_MemAllocZero(PCB_MINIMUM_SIZE * ENET_PCB_NUM);
    pcbbuffer = pcb[enetIf->deviceNumber];
    for (count = 0; count < ENET_PCB_NUM; count++)
    {
        QUEUEADD(packbuffer[enetIf->deviceNumber].pcbHead, packbuffer[enetIf->deviceNumber].pcbTail, (PCB *)pcbbuffer);
        pcbbuffer->FRAG[1].LENGTH = 0;
        pcbbuffer->FRAG[1].FRAGMENT = NULL;
        pcbbuffer->FREE = ENETFree;
        pcbbuffer++;
    }

#if FSL_FEATURE_ENET_SUPPORT_PTP
    buffCfg->ptpTsRxData = &ptpTsRxData[0];
    buffCfg->ptpTsRxBuffNum = ENET_PTP_RXTS_RING_LEN;
    buffCfg->ptpTsTxData = &ptpTsTxData[0];
    buffCfg->ptpTsTxBuffNum = ENET_PTP_TXTS_RING_LEN;
#endif
#if !ENET_RECEIVE_ALL_INTERRUPT
    uint8_t *rxExtBufferAlign;
    rxExtBuffer[enetIf->deviceNumber] =
        (uint8_t *)OSA_MemAllocZero(ENET_EXTRXBD_NUM * rxBufferSizeAlign + ENET_RX_BUFFER_ALIGNMENT);
    if (!rxExtBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBd[enetIf->deviceNumber]);
        OSA_MemFree(rxBuffer[enetIf->deviceNumber]);
        OSA_MemFree(txBuffer[enetIf->deviceNumber]);
        OSA_MemFree(rxRtcsBuffer[enetIf->deviceNumber]);
        return kStatus_ENET_MemoryAllocateFail;
    }
    rxExtBufferAlign = (uint8_t *)ENET_ALIGN((uint32_t)rxExtBuffer[enetIf->deviceNumber], ENET_RX_BUFFER_ALIGNMENT);
    buffCfg->extRxBuffQue = rxExtBufferAlign;
    buffCfg->extRxBuffNum = ENET_EXTRXBD_NUM;
#else
    buffCfg->extRxBuffQue = NULL;
    buffCfg->extRxBuffNum = 0;
#endif

    buffCfg->rxBdNumber = ENET_RXBD_NUM;
    buffCfg->rxBdPtrAlign = rxBdPtrAlign;
    buffCfg->rxBufferAlign = rxBufferAlign;
    buffCfg->rxBuffSizeAlign = rxBufferSizeAlign;
    buffCfg->txBdNumber = ENET_TXBD_NUM;
    buffCfg->txBdPtrAlign = txBdPtrAlign;
    buffCfg->txBufferAlign = txBufferAlign;
    buffCfg->txBuffSizeAlign = txBufferSizeAlign;

    return kStatus_ENET_Success;
}

/*!
 * @brief Initialize enet mac buffer.
 *
 * @return The execution status.
 */
uint32_t ENETBufferDeinit(enet_dev_if_t *enetIf)
{
    /* Check input parameter*/
    if (!enetIf)
    {
        return kStatus_ENET_InvalidInput;
    }

    /* Free allocated memory*/
    if (txBd[enetIf->deviceNumber])
    {
        OSA_MemFree(txBd[enetIf->deviceNumber]);
    }
    if (rxBd[enetIf->deviceNumber])
    {
        OSA_MemFree(rxBd[enetIf->deviceNumber]);
    }
    if (txBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(txBuffer[enetIf->deviceNumber]);
    }
    if (rxBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(rxBuffer[enetIf->deviceNumber]);
    }
    if (rxRtcsBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(rxRtcsBuffer[enetIf->deviceNumber]);
    }
#if !ENET_RECEIVE_ALL_INTERRUPT
    if (rxExtBuffer[enetIf->deviceNumber])
    {
        OSA_MemFree(rxExtBuffer[enetIf->deviceNumber]);
    }
#endif

    if (pcb[enetIf->deviceNumber])
    {
        OSA_MemFree(pcb[enetIf->deviceNumber]);
    }
    return kStatus_ENET_Success;
}

/*!
 * @brief ENET packet buffer free.
 *
 * @return None.
 */
void ENETFree(PCB_PTR packet)
{
    enet_dev_if_t *param;
    uint8_t count;

    /* Check input Parameter*/
    if (packet == NULL)
    {
        return;
    }

    param = (enet_dev_if_t *)packet->PRIVATE;
    count = param->deviceNumber;

    enetCriticalBegin(count);
    if ((packet->FRAG[0].FRAGMENT != NULL))
    {
        *(uint32_t *)packet->FRAG[0].FRAGMENT = 0;
        enetMacEnqueueBuffer((void **)&dataBuffQue, packet->FRAG[0].FRAGMENT);

        /* Clear fragment in the packet*/
        packet->FRAG[0].FRAGMENT = NULL;
    }

    /* Add the used PCB buffer into the PCB buffer queue.
     */
    QUEUEADD(packbuffer[count].pcbHead, packbuffer[count].pcbTail, packet);
    enetCriticalEnd(count);
}

#if ENET_RECEIVE_ALL_INTERRUPT

/*!
 * @brief Find ENET tcp/ip upper layer functions.
 *
 * Called from ENET Rx isr.
 *
 * @return The execution status.
 */
uint32_t ENETFindReceiver(void *enet, enet_mac_packet_buffer_t *packetBuffer)
{
    uint8_t *packet, counter;
    uint16_t type, length = 0;
    PCB *pcb;
    uint16_t *type;
    enet_ecb_struct_t *ecb;
    bool packetInvalid = ENET_ERROR;
    enet_dev_if_t *enetDevif = (enet_dev_if_t *)enet;

    /* Dequeue a large  buffer */
    packet = enetMacDequeueBuffer((void **)&dataBuffQue);
    if (packet != NULL)
    {
        for (counter = 0; ((0 == counter) || (packetBuffer[counter].next != NULL)); counter++)
        {
            memcpy(packet + length, packetBuffer[counter].data, packetBuffer[counter].length);
            length += packetBuffer[counter].length;
        }
    }
    else
    {
#if ENET_ENABLE_DETAIL_STATS
        enetDevif->stats.statsRxDiscard++;
#endif
        return kStatus_ENET_LargeBufferFull;
    }

    /* Process the received frame*/
    type = &((enet_ethernet_header_t *)packet)->type;
    type = ENET_NTOHS((*type));
    if (type == ENETPROT_8021Q)
    {
        type = &((enet_8021vlan_header_t *)packet)->type;
        type = ENET_NTOHS((*type));
    }
    if (type <= kEnetMaxFrameDateSize)
    {
        ENET_8022_HEADER_PTR llc = (ENET_8022_HEADER_PTR)(type + 2);
        type = ENET_NTOHS(llc->type);
    }
#if FSL_FEATURE_ENET_SUPPORT_PTP
    if (type == ENETPROT_ETHERNET)
    {
        ENET_DRV_ServiceL2packet(enetDevif, &packetBuffer[0]);
    }
#endif
    for (ecb = (enet_ecb_struct_t *)enetDevif->netIfPrivate; ecb; ecb = ecb->NEXT)
    {
        if (ecb->TYPE == type)
        {
            packetInvalid = ENET_OK;
            /* Collect frame to PCB structure for upper layer process*/
            QUEUEGET(packbuffer[enetDevif->deviceNumber].pcbHead, packbuffer[enetDevif->deviceNumber].pcbTail, pcb);
            if (pcb)
            {
                pcb->FRAG[0].LENGTH = length;
                pcb->FRAG[0].FRAGMENT = packet;
                pcb->FRAG[1].LENGTH = 0;
                pcb->PRIVATE = enetDevif;
                ecb->SERVICE(pcb, ecb->PRIVATE);
            }
        }
    }
    if (ENET_OK != packetInvalid)
    {
        enetMacEnqueueBuffer((void **)&dataBuffQue, packet);
    }
    return packetInvalid;
}
#endif

/*!
 * @brief Initialize the ENET device.
 *
 * @return The execution status.
 */
uint32_t ENETInitialize(uint32_t device, EnetAddress address, uint32_t flag, EnetHandle *handle)
{
    enet_dev_if_t *enetIf;
    uint32_t result;
    const enet_mac_config_t *macCfg;
    enet_buff_config_t buffCfg;
    enet_user_config_t enetUserConfig;

    if (device > ENET_INSTANCE_COUNT)
    {
        return kStatus_ENET_InvalidDevice;
    }

    /* Check the device status*/
    if (enetDevIf[device].isInitialized)
    {
        return kStatus_ENET_Initialized;
    }

    /* Initialize device*/
    enetIf = (enet_dev_if_t *)&enetDevIf[device];
    if (ENET_INSTANCE_COUNT == device)
    {
        enetIf->next = NULL;
    }

    /*     Configure structure*/
    enetIf->deviceNumber = device;
    g_enetMacCfg[device].macAddr = address;
    macCfg = &g_enetMacCfg[device];

#if ENET_RECEIVE_ALL_INTERRUPT
    enetIf->enetNetifcall = ENETFindReceiver;
#endif

    /* Create sync signal*/
    OSA_MutexCreate(&enetIf->enetContextSync);

    /* Initialize enet buffers*/
    result = ENETBufferInit(enetIf, &buffCfg);
    if (result != kStatus_ENET_Success)
    {
        return result;
    }

/* Initialize ENET  pins and install interrupts.
*  Before it was done in BSP Init. */

/* ENET module */
#ifdef CPU_MK65FN2M0VMI18
    /* Set clock source for Ethernet */
    if (0 == strcmp("TWR-K65F180M", BOARD_NAME))
    {
        CLOCK_SYS_SetEnetRmiiSrc(ENET_IDX, kClockRmiiSrcExt);
    }
#endif
    configureEnetPins(device);

    /* Select the ptp timer outclk */
    CLOCK_SYS_SetEnetTimeStampSrc(ENET_IDX, kClockTimeSrcOsc0erClk);

#define BSP_MACNET0_INT_TX_LEVEL (4)
#define BSP_MACNET0_INT_RX_LEVEL (4)
    NVIC_SetPriority(ENET_Transmit_IRQn, BSP_MACNET0_INT_TX_LEVEL);
    NVIC_EnableIRQ(ENET_Transmit_IRQn);

    NVIC_SetPriority(ENET_Receive_IRQn, BSP_MACNET0_INT_RX_LEVEL);
    NVIC_EnableIRQ(ENET_Receive_IRQn);

#if FSL_FEATURE_ENET_SUPPORT_PTP
    NVIC_SetPriority(ENET_1588_Timer_IRQn, BSP_MACNET0_INT_RX_LEVEL);
    NVIC_EnableIRQ(ENET_1588_Timer_IRQn);
#endif

    /* Initialize ENET device*/
    enetUserConfig.macCfg = macCfg;
    enetUserConfig.buffCfg = &buffCfg;
    result = ENET_DRV_Init(enetIf, &enetUserConfig);
    if (result == kStatus_ENET_Success)
    {
        /* Initialize PHY*/
        if (g_enetPhyCfg[device].isAutodiscoverEnabled)
        {
            uint32_t phyAddr;
            result = PHY_DRV_Autodiscover(device, &phyAddr);
            if (result != kStatus_ENET_Success)
            {
                return result;
            }
            enetIf->phyAddr = phyAddr;
        }
        else
        {
            enetIf->phyAddr = g_enetPhyCfg[device].phyAddr;
        }

        PHY_DRV_Init(device, enetIf->phyAddr, g_enetPhyCfg[device].isLoopEnabled);

        *handle = enetIf;
        enetIf->isInitialized = true;

#if !ENET_RECEIVE_ALL_INTERRUPT
        osa_status_t osaFlag;
        osaFlag = OSA_EventCreate(&enetIf->enetReceiveSync, kEventAutoClear);
        if (osaFlag != kStatus_OSA_Success)
        {
            return osaFlag;
        }
        /* Create receive task*/
        osaFlag = OSA_TaskCreate(ENETReceive, "receive", ENET_TASK_STACK_SIZE, ENETReceiveStack, ENET_RECEIVE_TASK_PRIO,
                                 (task_param_t)enetIf, false, &ENETReceiveTaskHandler);
        if (osaFlag != kStatus_OSA_Success)
        {
            return osaFlag;
        }
#endif

        return ENET_OK;
    }
    else
    {
        ENET_DRV_Deinit(enetIf);
        ENETBufferDeinit(enetIf);
        OSA_MutexDestroy(&enetIf->enetContextSync);
#if !ENET_RECEIVE_ALL_INTERRUPT
        OSA_TaskDestroy(ENETReceiveTaskHandler);
        OSA_EventDestroy(&enetIf->enetReceiveSync);
#endif
        *handle = NULL;
        return ENET_ERROR;
    }
}
#if !ENET_RECEIVE_ALL_INTERRUPT

/*!
 * @brief Open the ENET device.
 *
 * Open the ENET device, This interface is used to add the private
 * address to the ENET device structure.
 *
 * @return The execution status.
 */
uint32_t ENETOpen(EnetHandle handle, uint16_t type, void (*service)(PCB_PTR, void *), void *private)
{
    enet_dev_if_t *enetIf;

    /* Check input parameter*/
    if ((!handle) || (!private))
    {
        return kStatus_ENET_InvalidInput;
    }

    enetIf = (enet_dev_if_t *)handle;
    OSA_MutexLock(&enetIf->enetContextSync, USB_OSA_WAIT_FOREVER);

    /*add the upper layer netiIF structure into the device structure*/
    enetIf->netIfPrivate = private;
    OSA_MutexUnlock(&enetIf->enetContextSync);

    return ENET_OK;
}
#else

/*!
 * @brief Open the ENET device.
 *
 * Open the ENET device, This interface is used to add the private
 * address to the enet device structure.
 *
 * @return The execution status.
 */
uint32_t ENETOpen(EnetHandle handle, uint16_t type, void (*service)(PCB_PTR, void *), void *private)
{
    enet_dev_if_t *enetIf;
    enet_ecb_struct_t *ecb, **search;

    /* Check input parameter*/
    if ((!handle) || (!private))
    {
        return kStatus_ENET_InvalidInput;
    }

    enetIf = (enet_dev_if_t *)handle;
    OSA_MutexLock(&enetIf->enetContextSync, USB_OSA_WAIT_FOREVER);
    for (search = (enet_ecb_struct_t **)(&enetIf->netIfPrivate); *search; search = &(*search)->NEXT)
    {
        if ((*search)->TYPE == type)
        {
            OSA_MutexUnlock(&enetIf->enetContextSync);
            return kStatus_ENET_Open;
        }
    }

    ecb = (enet_ecb_struct_t *)OSA_MemAllocZero(sizeof(enet_ecb_struct_t));
    if (!ecb)
    {
        OSA_MutexUnlock(&enetIf->enetContextSync);
        return kStatus_ENET_MemoryAllocateFail;
    }
    ecb->TYPE = type;
    ecb->SERVICE = service;
    ecb->PRIVATE = private;
    ecb->NEXT = NULL;
    *search = ecb;

    OSA_MutexUnlock(&enetIf->enetContextSync);

    return ENET_OK;
}
#endif

/*!
 * @brief Stop the ENET device.
 *
 * @return The execution status.
 */
uint32_t ENETShutdown(EnetHandle handle)
{
    enet_dev_if_t *enetIf;
    uint32_t result;

    /* Check the input parameter*/
    if (!handle)
    {
        return ENETERR_INVALID_DEVICE;
    }

    enetIf = (enet_dev_if_t *)handle;

#if ENET_RECEIVE_ALL_INTERRUPT
    /* Make sure upper layers have closed the device*/
    if (enetIf->netIfPrivate)
    {
        return ENETERR_INVALID_DEVICE;
    }
#endif
    /* Close the ENET device*/
    result = ENET_DRV_Deinit(enetIf);
    if (result == kStatus_ENET_Success)
    {
        ENETBufferDeinit(enetIf);
        OSA_MutexDestroy(&enetIf->enetContextSync);

#if !ENET_RECEIVE_ALL_INTERRUPT
        OSA_TaskDestroy(ENETReceiveTaskHandler);
        OSA_EventDestroy(&enetIf->enetReceiveSync);
#endif
        enetIf->isInitialized = false;

        return ENET_OK;
    }

    return ENET_ERROR;
}

#if !ENET_RECEIVE_ALL_INTERRUPT

/*!
 * @brief Net Receive interface.
 *
 * @return None.
 */
static void ENETReceive(task_param_t param)
{
    uint8_t *packet;
    uint16_t length = 0, type, counter = 0;
    uint32_t result;
    PCB *pcb;
    uint16_t *type;
    event_flags_t flag = 0x1, flagCheck;
    enet_mac_packet_buffer_t packetBuffer[kEnetMaxFrameBdNumbers];

    /* Check input parameter*/
    if (!param)
    {
        return;
    }
    enet_dev_if_t *enetIf = (enet_dev_if_t *)param;

    /* Init the packet buffer*/
    memset(&packetBuffer[0], 0, kEnetMaxFrameBdNumbers * sizeof(enet_mac_packet_buffer_t));
    for (counter = 0; counter < kEnetMaxFrameBdNumbers - 1; counter++)
    {
        packetBuffer[counter].next = &packetBuffer[counter + 1];
    }
    packetBuffer[kEnetMaxFrameBdNumbers - 1].next = NULL;
    counter = 0;

    while (1)
    {
        /* Init the packet buffer*/
        for (counter = 0; counter < kEnetMaxFrameBdNumbers; counter++)
        {
            packetBuffer[counter].data = NULL;
            packetBuffer[counter].length = 0;
        }

        /* Receive frame*/
        result = ENET_DRV_ReceiveData(enetIf, &packetBuffer[0]);
        if ((result == kStatus_ENET_RxbdEmpty) || (result == kStatus_ENET_InvalidInput))
        {
            OSA_EventWait(&enetIf->enetReceiveSync, flag, false, USB_OSA_WAIT_FOREVER, &flagCheck);
        }

        /* Process with the right packets*/
        if (packetBuffer[0].data != NULL)
        {
            /* Collect the frame first*/
            enetCriticalBegin(enetIf->deviceNumber);
            packet = enetMacDequeueBuffer((void **)&dataBuffQue);
            enetCriticalEnd(enetIf->deviceNumber);
            if (packet != NULL)
            {
                length = 0;
                for (counter = 0; packetBuffer[counter].length != 0; counter++)
                {
                    memcpy(packet + length, packetBuffer[counter].data, packetBuffer[counter].length);
                    length += packetBuffer[counter].length;
                    *(uint32_t *)(packetBuffer[counter].data) = 0;
                    enetMacEnqueueBuffer((void **)&enetIf->bdContext.extRxBuffQue, packetBuffer[counter].data);
                    packetBuffer[counter].length = 0;
                }
            }
            else
            {
#if ENET_ENABLE_DETAIL_STATS
                enetIf->stats.statsRxMissed++;
#endif
            }

            /* Process the received frame*/
            type = &((enet_ethernet_header_t *)packet)->type;
            type = RTCS_NTOHS((*type));
            if (type == ENETPROT_8021Q)
            {
                type = &((enet_8021vlan_header_t *)packet)->type;
                type = RTCS_NTOHS((*type));
            }
            if (type <= kEnetMaxFrameDateSize)
            {
                enet_8022Header llc = (enet_8022Header)(type + 2);
                type = RTCS_NTOHS(llc->type);
            }

#if FSL_FEATURE_ENET_SUPPORT_PTP
            if (type == ENETPROT_ETHERNET)
            {
                ENET_DRV_ServiceL2packet(enetIf, &packetBuffer[0]);
            }
#endif
            /* Collect frame to PCB structure for upper layer process*/
            enetCriticalBegin(enetIf->deviceNumber);
            QUEUEGET(packbuffer[enetIf->deviceNumber].pcbHead, packbuffer[enetIf->deviceNumber].pcbTail, pcb);
            enetCriticalEnd(enetIf->deviceNumber);
            if (pcb)
            {
                pcb->FRAG[0].LENGTH = length;
                pcb->FRAG[0].FRAGMENT = packet;
                pcb->FRAG[1].LENGTH = 0;
                pcb->PRIVATE = (void *)enetIf;

                switch (type)
                {
                    case ENETPROT_IP:
                        IPERecv_IP((PCB *)pcb, enetIf->netIfPrivate);
                        break;
                    case ENETPROT_ARP:
                        IPERecv_ARP((PCB *)pcb, enetIf->netIfPrivate);
                        break;
#if RTCSCFG_ENABLE_IP6
                    case ENETPROT_IP6:
                        IP6ERecv_IP((PCB *)pcb, enetIf->netIfPrivate);
                        break;
#endif
                    default:
                        PCBFree((PCB *)pcb);
                        break;
                }
            }
            else
            {
#if ENET_ENABLE_DETAIL_STATS
                enetIf->stats.statsRxMissed++;
#endif
            }
        }
    }
}
#endif

/*!
 * @brief Net send interface. this is called by tcp/ip stack.
 *
 * @return The execution status.
 */
uint32_t ENETSend(EnetHandle handle, PCB_PTR packet, uint32_t type, EnetAddress dest, uint32_t flags)
{
    uint8_t headerLen, *frame;
    PCB_FRAGMENT *frag;
    uint16_t size = 0, lenTemp = 0, bdNumUsed = 0;
    enet_dev_if_t *enetIf;
    enet_ethernet_header_t *packet;
    volatile enet_bd_struct_t *curBd;
    uint32_t result, lenoffset = 0;
    /*Check out*/
    if ((!handle) || (!packet))
    {
        return kStatus_ENET_InvalidInput;
    }

    enetIf = (enet_dev_if_t *)handle;
    /* Default frame header size*/
    headerLen = kEnetEthernetHeadLen;

    /* Check the frame length*/
    for (frag = packet->FRAG; frag->LENGTH; frag++)
    {
        size += frag->LENGTH;
    }
    if (size > enetIf->maxFrameSize)
    {
#if ENET_ENABLE_DETAIL_STATS
        enetIf->stats.statsTxLarge++;
#endif
        return kStatus_ENET_TxLarge;
    }

    /*Add MAC hardware address*/
    packet = (enet_ethernet_header_t *)packet->FRAG[0].FRAGMENT;
    htone(packet->destAddr, dest);
    htone(packet->sourceAddr, enetIf->macAddr);
    packet->type = ENET_HTONS(type);
    if (flags & ENET_OPT_8021QTAG)
    {
        enet_8021vlan_header_t *vlanHead = (enet_8021vlan_header_t *)packet;
        vlanHead->tpidtag = ENET_HTONS(ENETPROT_8021Q);
        vlanHead->othertag = ENET_HTONS((ENET_GETOPT_8021QPRIO(flags) << 13U));
        vlanHead->type = ENET_HTONS(type);
        headerLen = sizeof(enet_8021vlan_header_t);
        packet->FRAG[0].LENGTH = headerLen;
    }

    if (flags & ENET_OPT_8023)
    {
        ENET_8022_HEADER_PTR lc = (ENET_8022_HEADER_PTR)(packet->type + 2);
        packet->type = ENET_HTONS(size - headerLen);
        lc->dsap[0] = 0xAA;
        lc->ssap[0] = 0xAA;
        lc->command[0] = 0x03;
        lc->oui[0] = 0x00;
        lc->oui[1] = 0x00;
        lc->oui[2] = 0x00;
        lc->type = ENET_HTONS(type);
        packet->FRAG[0].LENGTH = packet->FRAG[0].LENGTH + sizeof(ENET_8022_HEADER);
    }

    /* Get the current transmit data buffer in buffer descriptor */
    curBd = enetIf->bdContext.txBdCur;
    frame = ENET_HAL_GetBuffDescripData(curBd);

    /* Send a whole frame with a signal buffer*/
    if (size <= enetIf->bdContext.txBuffSizeAlign)
    {
        bdNumUsed = 1;
        for (frag = packet->FRAG; frag->LENGTH; frag++)
        {
            memcpy(frame + lenTemp, frag->FRAGMENT, frag->LENGTH);
            lenTemp += frag->LENGTH;
        }

        /* Send packet to the device*/
        while (kStatus_ENET_TxbdFull == enetSend(enetIf, size, bdNumUsed, &result))
        {
            // SchedYield();
        }

        /* Free the PCB buffer*/
        PCBFree(packet);
        return result;
    }

    /* Copy the Ethernet header first*/
    memcpy(frame, packet->FRAG[0].FRAGMENT, packet->FRAG[0].LENGTH);

    /* Send a whole frame with multiple buffer descriptors*/
    while ((size - bdNumUsed * enetIf->bdContext.txBuffSizeAlign) > enetIf->bdContext.txBuffSizeAlign)
    {
        if (bdNumUsed == 0)
        {
            memcpy((void *)(frame + packet->FRAG[0].LENGTH), (void *)(packet->FRAG[1].FRAGMENT),
                   enetIf->bdContext.txBuffSizeAlign - packet->FRAG[0].LENGTH);
            lenoffset += (enetIf->bdContext.txBuffSizeAlign - packet->FRAG[0].LENGTH);
        }
        else
        {
            memcpy((void *)frame, (void *)(packet->FRAG[1].FRAGMENT + lenoffset), enetIf->bdContext.txBuffSizeAlign);
            lenoffset += enetIf->bdContext.txBuffSizeAlign;
        }

        /* Incremenet the buffer descriptor*/
        curBd = ENET_DRV_IncrTxBuffDescripIndex(enetIf, curBd);
        frame = ENET_HAL_GetBuffDescripData(curBd);
        /* Increment the index and parameters*/
        bdNumUsed++;
    }
    memcpy((void *)frame, (void *)(packet->FRAG[1].FRAGMENT + lenoffset),
           size - bdNumUsed * enetIf->bdContext.txBuffSizeAlign);
    bdNumUsed++;
    /* Send packet to the device*/
    while (kStatus_ENET_TxbdFull == enetSend(enetIf, size, bdNumUsed, &result))
    {
        // SchedYield();
    }

    /* Free the PCB buffer*/
    PCBFree(packet);

    return result;
}

/*!
 * @brief Get the ENET address of an initialized device.
 *
 * @return The execution status.
 */
uint32_t ENETGetAddress(EnetHandle handle, EnetAddress address)
{
    /* Check input param*/
    if (!handle)
    {
        return kStatus_ENET_InvalidInput;
    }

    enet_dev_if_t *enetIf = (enet_dev_if_t *)handle;

    memcpy(address, enetIf->macAddr, kEnetMacAddrLen);

    return ENET_OK;
}

/*!
 * @brief Get the ENET address of an uninitialized device.
 *
 * @return The execution status.
 */
uint32_t ENETGetMacAddress(uint32_t device, uint32_t value, EnetAddress address)
{
    uint8_t g_enetAddress[kEnetMacAddrLen] = ENET_DEFAULT_MAC_ADD;

    address[0] = g_enetAddress[0];
    address[1] = g_enetAddress[1];
    address[2] = g_enetAddress[2];
    address[3] = (value & 0xFF00000U) >> 16U;
    address[4] = (value & 0xFF00U) >> 8U;
    address[5] = (value & 0xFFU);

    return ENET_OK;
}

/*!
 * @brief Join a multicast group.
 *
 * @return The execution status.
 */
uint32_t ENETJoin(EnetHandle handle, uint16_t type, EnetAddress address)
{
    enet_dev_if_t *enetIf = (enet_dev_if_t *)handle;
    enet_multicast_group_t *enetMultiGroup;
    uint32_t hash;

    /* Make sure it's a multicast group*/
    if (!(address[0] & 1U))
    {
        return kStatus_ENET_NoMulticastAddr;
    }

    OSA_MutexLock(&enetIf->enetContextSync, USB_OSA_WAIT_FOREVER);

    if (!enetIf->multiGroup)
    {
        enetIf->multiGroup = OSA_MemAlloc(sizeof(enet_multicast_group_t));
        if (enetIf->multiGroup == NULL)
        {
            OSA_MutexUnlock(&enetIf->enetContextSync);
            return kStatus_ENET_MemoryAllocateFail;
        }
        memcpy(enetIf->multiGroup->groupAdddr, address, kEnetMacAddrLen);
        ENET_DRV_AddMulticastGroup(enetIf->deviceNumber, address, &hash);
        enetIf->multiGroup->hash = hash;
        enetIf->multiGroup->next = NULL;
        enetIf->multiGroup->prv = NULL;
    }
    else
    {
        /* Check if we had add the multicast group*/
        enetMultiGroup = enetIf->multiGroup;
        while (enetMultiGroup != NULL)
        {
            if (!memcmp(enetMultiGroup->groupAdddr, address, kEnetMacAddrLen))
            {
                OSA_MutexUnlock(&enetIf->enetContextSync);
                return kStatus_ENET_AlreadyAddedMulticast;
            }
            if (enetMultiGroup->next == NULL)
            {
                break;
            }
            enetMultiGroup = enetMultiGroup->next;
        }

        /* Add this multicast group*/
        enetMultiGroup->next = OSA_MemAllocZero(sizeof(enet_multicast_group_t));
        if (enetMultiGroup->next == NULL)
        {
            OSA_MutexUnlock(&enetIf->enetContextSync);
            return kStatus_ENET_MemoryAllocateFail;
        }
        memcpy(enetMultiGroup->next->groupAdddr, address, kEnetMacAddrLen);
        ENET_DRV_AddMulticastGroup(enetIf->deviceNumber, address, &hash);
        enetMultiGroup->next->hash = hash;
        enetMultiGroup->next->next = NULL;
        enetMultiGroup->next->prv = enetMultiGroup;
    }

    OSA_MutexUnlock(&enetIf->enetContextSync);
    return ENET_OK;
}

/*!
 * @brief Leave a multicast group.
 *
 * @return The execution status.
 */
uint32_t ENETLeave(EnetHandle handle, uint16_t type, EnetAddress address)
{
    enet_dev_if_t *enetIf = (enet_dev_if_t *)handle;
    enet_multicast_group_t *enetMultiGroup, *enetTemp;

    /* Make sure it's a multicast group*/
    if (!(address[0] & 1U))
    {
        return kStatus_ENET_NoMulticastAddr;
    }

    OSA_MutexLock(&enetIf->enetContextSync, USB_OSA_WAIT_FOREVER);

    if (!enetIf->multiGroup)
    {
        OSA_MutexUnlock(&enetIf->enetContextSync);
        return kStatus_ENET_MulticastPointerNull;
    }

    /* Check if we had add the multicast group*/
    for (enetMultiGroup = enetIf->multiGroup; enetMultiGroup != NULL; enetMultiGroup = enetMultiGroup->next)
    {
        if (!memcmp(enetMultiGroup->groupAdddr, address, kEnetMacAddrLen))
        {
            ENET_DRV_LeaveMulticastGroup(enetIf->deviceNumber, address);
            memset(enetMultiGroup->groupAdddr, 0, kEnetMacAddrLen);
            enetTemp = enetMultiGroup->prv;
            if (enetTemp != NULL)
            {
                enetTemp->next = enetMultiGroup->next;
            }
            if (enetMultiGroup->next != NULL)
            {
                enetMultiGroup->next->prv = enetTemp;
            }

            /* Last entry.*/
            if (enetMultiGroup == enetIf->multiGroup)
            {
                enetIf->multiGroup = NULL;
            }

            OSA_MemFree((void *)enetMultiGroup);
            break;
        }
    }

    OSA_MutexUnlock(&enetIf->enetContextSync);

    return ENET_OK;
}

#if BSPCFG_ENABLE_ENET_STATS

/*!
 * @brief Get ENET packets statistic.
 *
 * @return The execution status.
 */
ENET_STATS_PTR ENETGetStats(EnetHandle handle)
{
#if ENET_ENABLE_DETAIL_STATS
    /* Common stats*/
    memcpy(&enetStats.COMMON, &((enet_dev_if_t *)handle)->stats, sizeof(enetStats.COMMON));
    /* Detail stats*/
    enetStats.ST_RX_ALIGN = ((enet_dev_if_t *)handle)->stats.statsRxAlign;
    enetStats.ST_RX_FCS = ((enet_dev_if_t *)handle)->stats.statsRxFcs;
    enetStats.ST_RX_GIANT = ((enet_dev_if_t *)handle)->stats.statsRxLengthGreater;
    enetStats.ST_RX_LATECOLL = ((enet_dev_if_t *)handle)->stats.statsRxCollision;
    enetStats.ST_RX_OVERRUN = ((enet_dev_if_t *)handle)->stats.statsRxOverRun;
    enetStats.ST_RX_RUNT = ((enet_dev_if_t *)handle)->stats.statsRxTruncate;
    enetStats.ST_TX_EXCESSCOLL = ((enet_dev_if_t *)handle)->stats.statsTxExcessCollision;
    enetStats.ST_TX_UNDERRUN = ((enet_dev_if_t *)handle)->stats.statsTxUnderFlow;
    enetStats.ST_TX_LATECOLL = ((enet_dev_if_t *)handle)->stats.statsTxLateCollision;

    enetStats.ST_TX_COPY_LARGE = ((enet_dev_if_t *)handle)->stats.statsTxLarge;
#endif
    return (ENET_STATS_PTR)&enetStats;
}
#endif

/*!
 * @brief Get ENET link status.
 *
 * @return True if link is up else false.
 */
bool ENETLinkStatus(EnetHandle handle)
{
    enet_dev_if_t *enetIf;
    bool status = false;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }
    enetIf = (enet_dev_if_t *)handle;

    PHY_DRV_GetLinkStatus(enetIf->deviceNumber, enetIf->phyAddr, &status);
    return status;
}

/*!
 * @brief The link speed.
 *
 * @return The link speed.
 */
uint32_t ENETGetSpeed(EnetHandle handle)
{
    enet_dev_if_t *enetIf;
    enet_phy_speed_t speed;
    uint32_t result;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }

    enetIf = (enet_dev_if_t *)handle;
    result = PHY_DRV_GetLinkSpeed(enetIf->deviceNumber, enetIf->phyAddr, &speed);
    if (result != kStatus_ENET_Success)
    {
        return result;
    }
    else
    {
        if (speed == (uint32_t)kEnetSpeed100M)
        {
            return 100;
        }
        else if (speed == (uint32_t)kEnetSpeed10M)
        {
            return 10;
        }
    }

    return ENET_ERROR;
}

/*!
 * @brief Get ENET MTU.
 *
 * @return The value of MTU.
 */
uint32_t ENETGet_MTU(EnetHandle handle)
{
    enet_dev_if_t *enetIf;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }

    enetIf = (enet_dev_if_t *)handle;
    if (!enetIf->maxFrameSize)
    {
        return kEnetMaxFrameDateSize;
    }

    if (enetIf->isVlanTagEnabled)
    {
        return enetIf->maxFrameSize - sizeof(enet_8021vlan_header_t) - kEnetFrameFcsLen;
    }
    else
    {
        return enetIf->maxFrameSize - kEnetEthernetHeadLen - kEnetFrameFcsLen;
    }
}

/*!
 * @brief Read required ENET PHY registers.
 *
 * @return The number of registers.
 */
bool ENETPhyRegisters(EnetHandle handle, uint32_t numRegs, uint32_t *reg)
{
    uint32_t counter;
    enet_dev_if_t *enetIf;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }

    enetIf = (enet_dev_if_t *)handle;
    if (!enetIf->maxFrameSize)
    {
        return kEnetMaxFrameDateSize;
    }

    for (counter = 0; counter < numRegs; counter++)
    {
        *reg = 0;
        if (PHY_DRV_Read(enetIf->deviceNumber, enetIf->phyAddr, counter, reg) != kStatus_ENET_Success)
        {
            return false;
        }
        reg++;
    }
    return true;
}

/*!
 * @brief Get the next device structure address.
 *
 * @return The device structure address .
 */
EnetHandle ENETGetNextDeviceHandle(EnetHandle handle)
{
    enet_dev_if_t *enetIf;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return NULL;
    }

    enetIf = (enet_dev_if_t *)handle;

    return (void *)enetIf->next;
}

/*!
 * @brief Get device option.
 *
 * @return The device structure address.
 */
uint32_t ENETGetOptions(EnetHandle handle)
{
    enet_dev_if_t *enetIf;
    uint32_t option = 0;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }

    enetIf = (enet_dev_if_t *)handle;

    if (g_enetMacCfg[enetIf->deviceNumber].macCtlConfigure & kEnetRxAccelEnable)
    {
        if (g_enetMacCfg[enetIf->deviceNumber].rxAccelerCfg & kEnetRxAccelIpCheckEnabled)
        {
            option |= ENET_OPTION_HW_RX_IP_CHECKSUM;
        }
        if (g_enetMacCfg[enetIf->deviceNumber].rxAccelerCfg & kEnetRxAccelProtoCheckEnabled)
        {
            option |= ENET_OPTION_HW_RX_PROTOCOL_CHECKSUM;
        }
        if (g_enetMacCfg[enetIf->deviceNumber].rxAccelerCfg & kEnetRxAccelMacCheckEnabled)
        {
            option |= ENET_OPTION_HW_RX_MAC_ERR;
        }
    }
    if (g_enetMacCfg[enetIf->deviceNumber].macCtlConfigure & kEnetTxAccelEnable)
    {
        if (g_enetMacCfg[enetIf->deviceNumber].txAccelerCfg & kEnetTxAccelIpCheckEnabled)
        {
            option |= ENET_OPTION_HW_TX_IP_CHECKSUM;
        }
        if (g_enetMacCfg[enetIf->deviceNumber].txAccelerCfg & kEnetTxAccelProtoCheckEnabled)
        {
            option |= ENET_OPTION_HW_TX_PROTOCOL_CHECKSUM;
        }
    }

    return option;
}

/*!
 * @brief Unregisters a protocol type on an Ethernet channel.
 *
 * @return The execution status.
 */
uint32_t ENETClose(EnetHandle handle, uint16_t type)
{
#if !ENET_RECEIVE_ALL_INTERRUPT

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }
#else
    enet_dev_if_t *enetIf;
    enet_ecb_struct_t *ecb, **search;

    /* Check input parameter*/
    if (handle == NULL)
    {
        return kStatus_ENET_InvalidDevice;
    }
    enetIf = (enet_dev_if_t *)handle;
    OSA_MutexLock(&enetIf->enetContextSync, USB_OSA_WAIT_FOREVER);

    for (search = (enet_ecb_struct_t **)&enetIf->netIfPrivate; *search; search = &(*search)->NEXT)
    {
        if ((*search)->TYPE == type)
        {
            break;
        }
    }

    if (!*search)
    {
        OSA_MutexUnlock(&enetIf->enetContextSync);
        return kStatus_ENET_Close;
    }

    ecb = *search;
    *search = ecb->NEXT;

    OSA_MutexUnlock(&enetIf->enetContextSync);
    OSA_MemFree(ecb);

#endif

    return ENET_OK;
}

/*!
 * @brief ENET mediactl interface.
 *
 * @return The execution status.
 */
uint32_t ENETMediactl(EnetHandle handle, uint32_t commandId, void *inOutParam)
{
    return ENET_OK;
}

/*!
 * @brief Describe an ENET error code.
 *
 * @return Pointer to error string.
 */
const char *ENETStrerror(uint32_t error)
{
    if (error == ENET_OK)
    {
        return "OK";
    }
    if ((error < kStatus_ENET_InvalidInput) || (error > kStatus_ENET_AlreadyAddedMulticast))
    {
        return "Unknown error";
    }
    return ENETErrlist[error - kStatus_ENET_InvalidInput];
}
