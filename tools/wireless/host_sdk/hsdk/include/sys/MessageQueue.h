/*! *********************************************************************************
* \file MessageQueue.h
* This is the header file for the MessageQueue module.
*
* Copyright 2013-2016 Freescale Semiconductor, Inc.
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
********************************************************************************* */
#ifndef __QUEUE_H__
#define __QUEUE_H__

/*! *********************************************************************************
 ************************************************************************************
 * Include
 ************************************************************************************
 ********************************************************************************* */
#include "hsdkOSCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! *********************************************************************************
 ************************************************************************************
 * Public type definitions
 ************************************************************************************
 ********************************************************************************* */
/**
 * @brief Node structure for the MessageQueue structural elements.
 */
typedef struct _list {
    void *data;         /**< Generic pointer to data stored into the node. */
    struct _list *next; /**< Pointer to the next element in the list. */
} Node;

/**
 * @brief Queue structure for passing messages between components of HSDK different layers.
 */
typedef struct {
    Node *head;                 /**< Head of the queue. */
    Node *tail;                 /**< End of the queue. */
    uint32_t cAvailableData;    /**< A generic count of the data stored. Can be used to store number of messages of total size of messages. */
    Semaphore sAnnounceData;    /**< A semaphore to indicate availability of  messages to be processed. */
    Lock lock;                  /**< A lock to synchronize access to the message queue. */
} MessageQueue;


/*! *********************************************************************************
 ************************************************************************************
 * Public memory declarations
 ************************************************************************************
 ********************************************************************************* */

/*! *********************************************************************************
 ************************************************************************************
 * Public macros
 ************************************************************************************
 ********************************************************************************* */

/*! *********************************************************************************
 ************************************************************************************
 * Public prototypes
 ************************************************************************************
 ********************************************************************************* */
MessageQueue *InitializeMessageQueue(uint32_t maxMessageFlow);
int DestroyMessageQueue(MessageQueue *pMessageQueue);
int ClearMessageQueue(MessageQueue *pMessageQueue);
void MessageQueuePut(MessageQueue *pMessageQueue, void *pData);
void *MessageQueueGet(MessageQueue *pMessageQueue);
void MessageQueuePutWithSize(MessageQueue *pMessageQueue, void *pData, uint32_t);
uint32_t MessageQueueDecrementSize(MessageQueue *pMessageQueue, uint32_t);
uint32_t MessageQueueGetContentSize(MessageQueue *pMessageQueue);
uint8_t IsEmpty(MessageQueue *pMessageQueue, uint8_t synchronized);
void *PeekFront(MessageQueue *pMessageQueue);
void PushFront(MessageQueue *pMessageQueue, void *pData);

#ifdef DEBUG
void InspectQueue(MessageQueue *pMessageQueue);
#endif

#ifdef __cplusplus
}
#endif

#endif
