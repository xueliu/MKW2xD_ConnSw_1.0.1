/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
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

#ifndef __gpio_pins_H
#define __gpio_pins_H

/* MODULE gpio_pins. */

/* Include inherited beans */
#include "fsl_gpio.h"

/* SPI Master Data Available Pin */
#define gSpiMasterDap_Port_d  gpioPort_C_c
#define gSpiMasterDap_Pin_d   2
/* SPI Slave Data Available Pin */
#define gSpiSlaveDap_Port_d   gpioPort_C_c
#define gSpiSlaveDap_Pin_d    2
/* I2C Master Data Available Pin */
#define gI2cMasterDap_Port_d  gpioPort_C_c
#define gI2cMasterDap_Pin_d   2
/* I2C Slave Data Available Pin */
#define gI2cSlaveDap_Port_d   gpioPort_C_c
#define gI2cSlaveDap_Pin_d    2

/* switch pins to be initialized by the low power component if the keyboard isn't enabled*/
/* 1<<n for sitchPins[n] */   
#define gLowPower_switchPinsToInitBitmap_d  ( 1<<1 | 1<<0 )

/*! @brief Configuration structure 0 for input pins */
extern gpioInputPinConfig_t switchPins[];

/*! @brief Configuration structure 0 for output pins */
extern gpioOutputPinConfig_t ledPins[];

/*! @brief Configuration structure for XCVR pins */
extern const gpioOutputPinConfig_t mXcvrSpiCsCfg;
extern const gpioOutputPinConfig_t mXcvrResetPinCfg;
extern const gpioInputPinConfig_t mXcvrIrqPinCfg;
  
#endif
