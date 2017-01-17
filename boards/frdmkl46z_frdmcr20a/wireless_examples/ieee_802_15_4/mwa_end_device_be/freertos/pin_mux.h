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
 *
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

/* PORTC12 (number 88), SW3 */
#define BOARD_INITBUTTONS_SW3_GPIO                                         GPIOC   /*!< GPIO device name: GPIOC */
#define BOARD_INITBUTTONS_SW3_PORT                                         PORTC   /*!< PORT device name: PORTC */
#define BOARD_INITBUTTONS_SW3_GPIO_PIN                                       12U   /*!< PORTC pin index: 12 */
#define BOARD_INITBUTTONS_SW3_PIN_NAME                                     PTC12   /*!< Pin name */
#define BOARD_INITBUTTONS_SW3_LABEL                                        "SW3"   /*!< Label */
#define BOARD_INITBUTTONS_SW3_NAME                                         "SW3"   /*!< Identifier name */
#define BOARD_INITBUTTONS_SW3_DIRECTION                  kPIN_MUX_DirectionInput   /*!< Direction */

/* PORTC3 (number 73), SW1 */
#define BOARD_INITBUTTONS_SW1_GPIO                                         GPIOC   /*!< GPIO device name: GPIOC */
#define BOARD_INITBUTTONS_SW1_PORT                                         PORTC   /*!< PORT device name: PORTC */
#define BOARD_INITBUTTONS_SW1_GPIO_PIN                                        3U   /*!< PORTC pin index: 3 */
#define BOARD_INITBUTTONS_SW1_PIN_NAME                                      PTC3   /*!< Pin name */
#define BOARD_INITBUTTONS_SW1_LABEL                                        "SW1"   /*!< Label */
#define BOARD_INITBUTTONS_SW1_NAME                                         "SW1"   /*!< Identifier name */
#define BOARD_INITBUTTONS_SW1_DIRECTION                  kPIN_MUX_DirectionInput   /*!< Direction */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitButtons(void);

/* PORTE29 (number 26), LED_RED */
#define BOARD_INITLEDS_LED_RED_GPIO                                        GPIOE   /*!< GPIO device name: GPIOE */
#define BOARD_INITLEDS_LED_RED_PORT                                        PORTE   /*!< PORT device name: PORTE */
#define BOARD_INITLEDS_LED_RED_GPIO_PIN                                      29U   /*!< PORTE pin index: 29 */
#define BOARD_INITLEDS_LED_RED_PIN_NAME                                    PTE29   /*!< Pin name */
#define BOARD_INITLEDS_LED_RED_LABEL                                   "LED_RED"   /*!< Label */
#define BOARD_INITLEDS_LED_RED_NAME                                    "LED_RED"   /*!< Identifier name */
#define BOARD_INITLEDS_LED_RED_DIRECTION                kPIN_MUX_DirectionOutput   /*!< Direction */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDs(void);

/* PORTD5 (number 98), J2[12]/D13/LED_GREEN */
#define BOARD_INITXCVR_LED_GREEN_PERIPHERAL                                 SPI1   /*!< Device name: SPI1 */
#define BOARD_INITXCVR_LED_GREEN_SIGNAL                                      SCK   /*!< SPI1 signal: SCK */
#define BOARD_INITXCVR_LED_GREEN_PIN_NAME                               SPI1_SCK   /*!< Pin name */
#define BOARD_INITXCVR_LED_GREEN_LABEL                    "J2[12]/D13/LED_GREEN"   /*!< Label */
#define BOARD_INITXCVR_LED_GREEN_NAME                                "LED_GREEN"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitXCVR(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitI2C(void);

/* PORTA1 (number 35), J1[2]/D0/UART1_RX_TGTMCU */
#define BOARD_INITLPSCI_DEBUG_UART_RX_PERIPHERAL                           UART0   /*!< Device name: UART0 */
#define BOARD_INITLPSCI_DEBUG_UART_RX_SIGNAL                                  RX   /*!< UART0 signal: RX */
#define BOARD_INITLPSCI_DEBUG_UART_RX_PIN_NAME                          UART0_RX   /*!< Pin name */
#define BOARD_INITLPSCI_DEBUG_UART_RX_LABEL           "J1[2]/D0/UART1_RX_TGTMCU"   /*!< Label */
#define BOARD_INITLPSCI_DEBUG_UART_RX_NAME                       "DEBUG_UART_RX"   /*!< Identifier name */

/* PORTA2 (number 36), J1[4]/D1/UART1_TX_TGTMCU */
#define BOARD_INITLPSCI_DEBUG_UART_TX_PERIPHERAL                           UART0   /*!< Device name: UART0 */
#define BOARD_INITLPSCI_DEBUG_UART_TX_SIGNAL                                  TX   /*!< UART0 signal: TX */
#define BOARD_INITLPSCI_DEBUG_UART_TX_PIN_NAME                          UART0_TX   /*!< Pin name */
#define BOARD_INITLPSCI_DEBUG_UART_TX_LABEL           "J1[4]/D1/UART1_TX_TGTMCU"   /*!< Label */
#define BOARD_INITLPSCI_DEBUG_UART_TX_NAME                       "DEBUG_UART_TX"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLPSCI(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSPI(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitRGB(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUART(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
