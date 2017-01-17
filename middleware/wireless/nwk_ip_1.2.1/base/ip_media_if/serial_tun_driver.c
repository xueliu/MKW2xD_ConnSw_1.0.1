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
/*!=================================================================================================
\file       serial_tun_driver.c
\brief      This is a private source file for the Serial Tunnel driver (UDP over THCI over UART)
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "EmbeddedTypes.h"
#include "FunctionLib.h"

/*==================================================================================================
Private macros
==================================================================================================*/

/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private prototypes
==================================================================================================*/

/*==================================================================================================
Private global variables declarations
==================================================================================================*/
static uint8_t macAddress[6] = {0x00, 0x04, 0x9F, 0x00, 0xFA, 0x5D};

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public functions
==================================================================================================*/

/*!*************************************************************************************************
\fn uint32_t VIRTUAL_TUN_initialize(uint8_t* address)
\brief  Initializes the chip.
        
\param [in]   pAddr       the local Ethernet address
       
\retval       uint32_t     ENET_OK
                           error code 
***************************************************************************************************/
uint32_t VIRTUAL_TUN_initialize
(
      uint8_t* pAddr
)
{
    uint32_t addrSize;
    uint8_t* pAddrPtr = (uint8_t*)&addrSize;

    if(pAddr == NULL)
    {
        addrSize = 1U;
    }
    else
    {
        addrSize = 6U;
        pAddrPtr = pAddr;
    }

    (void)pAddrPtr;
    FLib_MemCpy(macAddress, pAddr, addrSize);

    /* return status received from SLAVE */
    return 0;
}


/*!*************************************************************************************************
\fn uint32_t VIRTUAL_TUN_get_address(uint8_t* address)
\brief  Retrieves the TUN address of a device.

\param [out]  address      mac address

\retval       uint32_t     ENET_OK
                           error code
***************************************************************************************************/
uint32_t VIRTUAL_TUN_get_address
(
      uint8_t*        address
)
{
    FLib_MemCpy(address, macAddress, 6);

    return 0;
}

/*==================================================================================================
Private functions
==================================================================================================*/


/* Delimiters */
/*================================================================================================*/


/*==================================================================================================
Private debug functions
==================================================================================================*/
