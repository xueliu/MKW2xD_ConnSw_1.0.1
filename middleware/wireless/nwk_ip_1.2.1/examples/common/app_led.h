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

#ifndef _APP_LED_H             
#define _APP_LED_H

/*!=================================================================================================
\file       app_led.h
\brief      This is the header file for the application led configuration
==================================================================================================*/
   
/*==================================================================================================
Include Files
==================================================================================================*/
#include "app_init.h"
#include "LED.h"

/*==================================================================================================
Public macros
==================================================================================================*/
#if gLedRgbEnabled_d
    #define gConfigLedsOnTargetBoard_c          gLEDsOnTargetBoardDefault_c - 2 
#else
    #define gConfigLedsOnTargetBoard_c          gLEDsOnTargetBoardDefault_c
#endif

#define gLedId_Undefined_c                      THR_ALL_FFs8
#define gLedState_Undefined_c                   THR_ALL_FFs8
#define gLedSetup_DefaultFlashPeriod_c          mLEDInterval_c

/* LED setup bitmap */
#define gLedSetup_On_c                          (0x01<<7)
#define gLedSetup_Flashing_c                    (0x01<<6)
#define gLedSetup_Red_c                         (0x01<<5)
#define gLedSetup_Green_c                       (0x01<<4)
#define gLedSetup_Blue_c                        (0x01<<3)
#define gLedSetup_Chasing_c                     (0x01<<2)
#define gLedSetup_Rgb_c                         (0x01<<1)
#define gLedSetup_FlashPeriod_c                 (0x01<<0)

/* RGB led - color wheel*/
#define gLedSetupColorWheel_c                   (gLedSetup_Flashing_c | gLedSetup_Chasing_c)

/*==================================================================================================
Public type definitions
==================================================================================================*/

/*==================================================================================================
Public global variables declarations
==================================================================================================*/


/*==================================================================================================
Public function prototypes
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

void Led_SetState(appDeviceMode_t devModeId, appDeviceState_t devStateId);
#if gLedRgbEnabled_d
void Led_UpdateRgbState(uint8_t redValue, uint8_t greenValue, uint8_t blueValue);
#endif

/* 802.15.4 - LED tx activity functions:*/
void Led_MacTxOn(void);
void Led_MacTxOff(void);

#ifdef __cplusplus
}
#endif

/*================================================================================================*/
#endif  /*  _APP_LED_H */
