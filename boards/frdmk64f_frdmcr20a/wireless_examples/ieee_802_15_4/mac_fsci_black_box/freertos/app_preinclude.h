/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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
#ifndef __APP_PREINCLUDE_H__
#define __APP_PREINCLUDE_H__

/* 
 * KSDK configuration 
 */
#define CPU_MK64FN1M0VMD12              1
#define FSL_RTOS_FREE_RTOS              1
#define gTotalHeapSize_c                12000
#define USB_STACK_FREERTOS              1

/* 
 * 802.15.4 PHY configuration 
 */
#define gPhyDefaultTxPowerLevel_d       23
#define gAspCapability_d 1


/* 
 * 802.15.4 MAC configuration 
 */
#define gMacFeatureSet_d gMacFeatureSet_06BEGTSM4_d


/* 
 * Connectivity Framework configuration 
 */
#define gFsciIncluded_c                 1
#define gFSCI_IncludeMacCommands_c      1
#define gFSCI_IncludeLpmCommands_c      0
#define gUpgradeImageOnCurrentDevice_d  0
#define gSerialMgrUseUart_c             1
#define gSerialMgrUseUSB_c              0
#define gKBD_KeysCount_c                4
#define gLEDsOnTargetBoardCnt_c         3
#define gEepromParams_WriteAlignment_c  8
#define gMCR20_ClkOutFreq_d             gCLK_OUT_FREQ_DISABLE
#define cPWR_UsePowerDownMode           1

#define PoolsDetails_c \
         _block_size_  64  _number_of_blocks_    16 _pool_id_(0) _eol_  \
         _block_size_ 128  _number_of_blocks_    8 _pool_id_(0) _eol_  \
         _block_size_ 256  _number_of_blocks_    16 _pool_id_(0) _eol_

#endif /* __APP_PREINCLUDE_H__ */