/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file MKL46Z256.h
*
* This file contains configuration options for the frdmkl46z board.
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

/**************************************************/
/** USER SETTINGS OF KINETIS MCU */
/**  Kinetis ARM Cortex-M0+ model */
#ifndef _FRDMKL46Z_CFG_H
#define _FRDMKL46Z_CFG_H


#define KINETIS_MODEL KL46_50MHz

/*  in the case of using USB VIRTUAL SERIAL LINK you must activate No break TRIM CHECKBOX in the master AN2295 PC Application  */
/*  the break impulse is replaced by using only 9 bits zero impulse  */
//  BREAK IMPULSE       |START| |0| |0| |0| |0| |0| |0| |0| |0| |0| |0| STOP|
//  ZERO IMPULSE        |START| |0| |0| |0| |0| |0| |0| |0| |0| |0| |STOP|
#define BOOTLOADER_SHORT_TRIM  1

/** Kinetis RAM */
#define KINETIS_RAM_START (0x1FFFE000)
#define KINETIS_RAM_END   (0x20005FFF)


/**************************************************/
/* Actual used UART module */
/**************************************************/
#define BOOT_UART_IRQ          UART0_IRQn
#define BOOT_UART_BASE         UART0
#define BOOT_UART_SIM_SCG_REG  SIM->SCGC4
#define BOOT_UART_SIM_SCG_MASK SIM_SCGC4_UART0_MASK
#define BOOT_UART_CLOCK        (32768 * 640)
/* Actual used UART module */
/* A range of UART baudrates is (9600 - 115200) */
//#define BOOT_UART_BAUD_RATE  9600 
//#define BOOT_UART_BAUD_RATE    14400
//#define BOOT_UART_BAUD_RATE    19200 
//#define BOOT_UART_BAUD_RATE  38400 
#define BOOT_UART_BAUD_RATE  57600 
//#define BOOT_UART_BAUD_RATE  76800 
//#define BOOT_UART_BAUD_RATE  115200 
//#define BOOT_UART_BAUD_RATE  128000 
/** GPIO & UART pins initialization */


#define BOOT_UART_GPIO_PORT_RX   PORTE
#define BOOT_UART_GPIO_PORT_TX   PORTE
#define BOOT_UART_GPIO_PORT_SIM_SCG_REG  SIM->SCGC5
#define BOOT_UART_GPIO_PORT_SIM_SCG_MASK SIM_SCGC5_PORTE_MASK 
/*  setting of multiplexer for UART alternative of pin */
#define BOOT_PIN_UART_ALTERNATIVE 3

/*  setting of multiplexer for GPIO alternative of pin */
#define BOOT_PIN_GPIO_ALTERNATIVE 1

#define BOOT_UART_GPIO_PIN_RX   0  

#define BOOT_UART_GPIO_PIN_TX   1  

/**************************************************/
/* SPI */
/**************************************************/

/* SPI Master */
#define BOOT_SPI_BasePtr           SPI1
#define BOOT_SPI_PORT_SIM_SCG      SIM->SCGC5
#define cSPI_PORT_SIM_SCG_Config_c SIM_SCGC5_PORTC_MASK
#define BOOT_SPI_SIM_SCG           SIM->SCGC4
#define cSPI_SIM_SCG_Config_c      SIM_SCGC4_SPI1_MASK

#define BOOT_SPI_ALTERNATIVE       2
#define BOOT_SPI_SCLK_PCR          PORTD->PCR[5]
#define BOOT_SPI_MOSI_PCR          PORTD->PCR[6]
#define BOOT_SPI_MISO_PCR          PORTD->PCR[7]
#define BOOT_SPI_SSEL_PCR          PORTD->PCR[4]

#define BOOT_SPI_SSEL_PCOR         GPIOD->PCOR
#define BOOT_SPI_SSEL_PSOR         GPIOD->PSOR
#define BOOT_SPI_SSEL_PDDR         GPIOD->PDDR
#define BOOT_SPI_SSEL_PIN_Number_c 4

/* SPI Slave */
#define BOOT_SPI_Slave_IRQ               SPI1_IRQn
#define BOOT_SPI_Slave_BasePtr           SPI1
#define BOOT_SPI_Slave_PORT_SIM_SCG      SIM->SCGC5
#define cSPI_Slave_PORT_SIM_SCG_Config_c SIM_SCGC5_PORTC_MASK
#define BOOT_SPI_Slave_SIM_SCG           SIM->SCGC4
#define cSPI_Slave_SIM_SCG_Config_c      SIM_SCGC4_SPI1_MASK

#define BOOT_SPI_Slave_SCLK_PCR          PORTC->PCR[5]
#define BOOT_SPI_Slave_MOSI_PCR          PORTC->PCR[6]
#define BOOT_SPI_Slave_MISO_PCR          PORTC->PCR[7]
#define BOOT_SPI_Slave_SSEL_PCR          PORTC->PCR[4]
#define BOOT_SPI_Slave_ALTERNATIVE       2


#define BOOT_SPI_Slave_DAP_SCG_REG       SIM->SCGC5
#define BOOT_SPI_Slave_DAP_SCG_MASK      SIM_SCGC5_PORTD_MASK
#define BOOT_SPI_Slave_DAP_PCR           PORTD->PCR[1]
#define BOOT_SPI_Slave_DAP_PDDR          GPIOD->PDDR
#define BOOT_SPI_Slave_DAP_PSOR          GPIOD->PSOR
#define BOOT_SPI_Slave_DAP_PCOR          GPIOD->PCOR
#define BOOT_SPI_Slave_DAP_pin           1

/**************************************************/
/* Actual used PIN reset setting */
/**************************************************/
#define BOOT_PIN_ENABLE_SIM_SCG_REG  SIM->SCGC5
#define BOOT_PIN_ENABLE_SIM_SCG_MASK SIM_SCGC5_PORTC_MASK
#define BOOT_PIN_ENABLE_PORT_BASE  PORTC   

#define BOOT_PIN_ENABLE_GPIO_BASE  GPIOC   

#define BOOT_PIN_ENABLE_NUM        4          

/**************************************************/
/* CRC module clock gating info */
/**************************************************/
#define BOOT_CRC_SIM_SCG_REG  SIM_SCGC6
#define BOOT_CRC_SIM_SCG_MASK SIM_SCGC6_CRC_MASK 
/**************************************************/
/** BOOTLOADER FEATURES SETTINGS */
/**************************************************/
#define BOOTLOADER_ENABLE_READ_CMD  1

#define BOOTLOADER_INT_WATCHDOG     0

#define BOOTLOADER_ENABLE_VERIFY    1

#define BOOTLOADER_CRC_ENABLE       1

#define BOOTLOADER_AUTO_TRIMMING    1 

#define BOOTLOADER_PIN_ENABLE       0
/**************************************************/
/** CALIBRATION OF BOOTLOADER TRIM SETTINGS */
#define BOOT_CALIBRATION_GPIO_BASE  PTE_BASE_PTR
/******************************************************************************
*
*
*     Advanced detail settings for non standard settings
*
*
******************************************************************************/

#define VECTOR_TABLE_SIZE (48)

/* Flash block count of this MCU */
#define FLASH_BLOCK_CNT 1

/* Start address of interrupt vector table */ 
#define INTERRUPT_VECTORS 0x0000

/* Start address of relocated interrutp vector table */
#define RELOCATED_VECTORS 0x4000 

/* Flash start address */
#define USER_FLASH_START RELOCATED_VECTORS

/* Flash end address */
//#define USER_FLASH_END 0x0003FFFF

/* Flash2 start address */
//#define USER_FLASH_START_2 0x00040000

/* Flash2 end address */
//#define USER_FLASH_END_2 0x0005FFFF

/* Size of write block */
#define FLASH_WRITE_PAGE 128

/* Size of erase block */
#define FLASH_ERASE_PAGE 1024

/* Maximal length of ID_STRING */
//#define ID_STRING_MAX 5

/* Description string */
#define KINETIS_MODEL_STR "KL46"
#define FLASH_INIT_FLASH_CLOCK { SIM->SCGC6 |= SIM_SCGC6_FTF_MASK; }
#define FLASH_BASE_PTR                FTFA
#define FLASH_FSTAT                   FTFA->FSTAT                                  
#define FLASH_FSTAT_CCIF_MASK         FTFA_FSTAT_CCIF_MASK
#define FLASH_FSTAT_ACCERR_MASK       FTFA_FSTAT_ACCERR_MASK
#define FLASH_FSTAT_FPVIOL_MASK       FTFA_FSTAT_FPVIOL_MASK
#define FLASH_FSTAT_RDCOLERR_MASK     FTFA_FSTAT_RDCOLERR_MASK
#define FLASH_FSTAT_MGSTAT0_MASK      FTFA_FSTAT_MGSTAT0_MASK 

#endif //_FRDMKL46Z_CFG_H