/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
* \file
* FSCI Bootloader Interface file
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

#ifndef __FSCIBOOTLOADER_H__
#define __FSCIBOOTLOADER_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "EmbeddedTypes.h"

/******************************************************************************
 *******************************************************************************
 * Public macros
 *******************************************************************************
 ******************************************************************************/
/*
 * FSCI Configuration
 */
#ifndef gFsciIncluded_c
#define gFsciIncluded_c           0 /* Enable/Disable FSCI module */
#endif

#ifndef gFsciUseCRC_c
#define gFsciUseCRC_c             0 /* Enable/Disable FSCI CRC check */
#endif
#ifndef gFsciMaxPayloadLen_c
#define gFsciMaxPayloadLen_c    2060 /* bytes */
#endif

#ifndef gFsciLenHas2Bytes_c
#define gFsciLenHas2Bytes_c       1 /* boolean */
#endif

#ifndef gFsciBootUseSeqNo_c
#define gFsciBootUseSeqNo_c       1
#endif

#ifndef gFsciTxAck_c
#define gFsciTxAck_c              0 /* boolean */
#endif

#ifndef gFsciRxAck_c
#define gFsciRxAck_c              0 /* boolean */
#endif

#define mFsciTxRetryCnt_c       4
#define mFsciRxAckTimeoutMs_c   100

#define gFSCI_StartMarker_c     0x02
#define gFSCI_EndMarker_c       0x03
#define gFSCI_EscapeChar_c      0x7F

/* FSCI OpGroup */
#define gFSCI_ReqOpcodeGroup_c             0xA3    /* FSCI utility Requests         */
#define gFSCI_CnfOpcodeGroup_c             0xA4    /* FSCI utility Confirmations/Indications    */
#define gFSCI_ReservedOpGroup_c            0x52

/* FSCI OpCodes */
#define mFsciMsgResetCPUReq_c              0x08 /* Fsci-CPU_Reset.Request.               */
#define mFsciLowLevelPing_c                0x38 /* Fsci-Ping.Request                    */
#define mFsciBootSetModeReq_c              0x28
#define mFsciBootStartImageReq_c           0x29
#define mFsciBootPushImageChunkReq_c       0x2A
#define mFsciBootCommitImageReq_c          0x2B
#define mFsciBootCancelImageReq_c          0x2C
#define mFsciBootSetFileVerPoliciesReq_c   0x2D
#define mFsciBootAbortUpgradeReq_c         0x2E
#define mFsciBootImageChunkReq_c           0x2F
#define mFsciBootQueryImageReq_c           0xC2
#define mFsciBootQueryImageRsp_c           0xC3
#define mFsciBootImageNotifyReq_c          0xC4
#define mFsciMsgError_c                    0xFE /* Fsci internal error.                 */

/* FSCI ACK OpCode */
#define mFsciMsgAck_c                      0xFD /* Fsci acknowledgment.                 */

/******************************************************************************
 *******************************************************************************
 * Public type definitions
 *******************************************************************************
 ******************************************************************************/
typedef uint8_t clientPacketStatus_t;
typedef uint8_t opGroup_t;
typedef uint8_t opCode_t;
#if gFsciLenHas2Bytes_c
typedef uint16_t fsciLen_t;
#else
typedef uint8_t fsciLen_t;
#endif

/* FSCI status codes */
typedef enum{
    gFsciSuccess_c                 = 0x00,
    gFsciSAPHook_c                 = 0xEF,
    gFsciSAPDisabled_c             = 0xF0,
    gFsciSAPInfoNotFound_c         = 0xF1,
    gFsciUnknownPIB_c              = 0xF2,
    gFsciAppMsgTooBig_c            = 0xF3,
    gFsciOutOfMessages_c           = 0xF4,
    gFsciEndPointTableIsFull_c     = 0xF5,
    gFsciEndPointNotFound_c        = 0xF6,
    gFsciUnknownOpcodeGroup_c      = 0xF7,
    gFsciOpcodeGroupIsDisabled_c   = 0xF8,
    gFsciDebugPrintFailed_c        = 0xF9,
    gFsciReadOnly_c                = 0xFA,
    gFsciUnknownIBIdentifier_c     = 0xFB,
    gFsciRequestIsDisabled_c       = 0xFC,
    gFsciUnknownOpcode_c           = 0xFD,
    gFsciTooBig_c                  = 0xFE,
    gFsciError_c                   = 0xFF    /* General catchall error. */
} gFsciStatus_t;

typedef enum {
  PACKET_IS_VALID,
  PACKET_IS_TO_SHORT,
  FRAMING_ERROR,
  INTERNAL_ERROR
} fsci_packetStatus_t;

/* Format of packets exchanged between the external client and Fsci. */
typedef PACKED_STRUCT clientPacketHdr_tag
{
    uint8_t    startMarker;
    opGroup_t  opGroup;
    opCode_t   opCode;
    fsciLen_t  len;      /* Actual length of payload[] */
} clientPacketHdr_t;

/* The terminal checksum is actually stored at payload[len]. The checksum */
/* field insures that there is always space for it, even if the payload */
/* is full. */
typedef PACKED_STRUCT clientPacketStructured_tag
{
    clientPacketHdr_t header;
    uint8_t payload[gFsciMaxPayloadLen_c];
    uint8_t checksum;
} clientPacketStructured_t;

#if gFsciTxAck_c
typedef PACKED_STRUCT gFsciAckMsg_tag{
    clientPacketHdr_t header;
    uint8_t checksumPacketReceived;
    uint8_t checksum;
}gFsciAckMsg_t;
#endif

/* defines the working packet type */
typedef PACKED_UNION clientPacket_tag
{
    /* The entire packet as unformatted data. */
    uint8_t raw[sizeof(clientPacketStructured_t)];
    /* The packet as header + payload. */
    clientPacketStructured_t structured;
    /* A minimal packet with only a status value as a payload. */
    PACKED_STRUCT
    {                              /* The packet as header + payload. */
        clientPacketHdr_t header;
        clientPacketStatus_t status;
    } headerAndStatus;
} clientPacket_t;

/******************************************************************************
 *******************************************************************************
 * Public memory declarations
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************
 *******************************************************************************
 * Public function prototypes
 *******************************************************************************
 ******************************************************************************/
#if gFsciIncluded_c
void FSCI_BOOT_main(void);
void FSCI_BOOT_SerialRxCallback(uint32_t param);
void FSCI_BOOT_transmitFormatedPacket(clientPacket_t *pPacket);

#else
#define FSCI_BOOT_main()
#define FSCI_BOOT_SerialRxCallback(inteface)
#define FSCI_BOOT_transmitFormatedPacket(pPacket)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FSCIBOOTLOADER_H__ */
