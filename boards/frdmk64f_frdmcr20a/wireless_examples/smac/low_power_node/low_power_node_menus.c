/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file low_power_node_menus.c
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

#include "low_power_node_menus.h"

char * const cu8Logo[]={  
  "\f\r\n",
  "\n\n\r\n",
  " ####         ######      ##### ##########\n\r",
  " ######      # ######    ##### #############\n\r", 
  " #######     ## ######  ##### ###############\n\r",  
  " ########    ### ########### ####       #####\n\r",  
  " #### #####  #### ######### #####       #####\n\r",   
  " ####  ##### #### ######### #################\n\r",  
  " ####   ######## ########### ###############\n\r", 
  " ####     ##### ######  ##### ############\n\r",
  " ####      ### ######    ##### ##\n\r",
  " ####       # ######      ##### #\n\r\n\r",
  "\r             Low Power Demo\n\n"           ,
  "\r\n -Press enter to start",
  NULL
};

char * const cu8MainMenu[]={  
  "\f\r Low Power Demo Interface short cuts    \n",
  "\r------------------------------------------\n",
  "\r -Press [q] for channel up\n",
  "\r -Press [w] for channel down\n",
  "\r -Press [a] for Power up\n",
  "\r -Press [s] for Power down\n",
  "\r -Press [r] for RX Mode\n",
  "\r -Press [t] for TX Mode\n",
  "\r Use these keys in the main menu before running any scenario \n",
  "\r  ______________________________________\n",
  "\r |                                      |\n",
  "\r |     Select Low Power Demo Option     |\n",
  "\r |______________________________________|\n",
  "\r -Press [1] Manual Power Modes Configuration\n",
  "\r -Press [2] Low Power Preamble Search Scenario  \n",
  "\r -Press [!] Reset MCU\n\r\n",
  NULL
};


char * const cu8ShortCutsBar[]={ 
  "\f\r\n",
  "\r------------------------------------------\n",
  "\r  [q] Ch+     [a] Pw+     [r] RX Mode     \n",  
  "\r  [w] Ch-     [s] Pw-     [t] TX Mode     \n",  
  "\r------------------------------------------\n\r",
  NULL
};

char * const cu8XCVRModes[]={ 
  "\r\n", 
  "\rSelect Radio operation mode: \n",
  "\r-Press [1] Idle mode \n",
  "\r-Press [2] Hibernate mode   \n",                                              //@CMA, Low Power Demo. TO DO!! (Rx-Idle-Rx-> To Do- Need to figure out)
  "\r-Press [3] Doze mode  \n",
  "\r-Press [4] Auto Doze mode \n",
  "\r-Press [5] RX mode  \n",
  "\r-Press [6] TX mode  \r\n",                                                   //@CMA, Low Power Demo. Refer to enabled PowerOutput and PABoost selection in ApplicationConf.h
  NULL
};

char * const cu8LPCSelectMCUPowerModeMenuString[]={ 
  "\r\n", 
  "\rSelect MCU Power mode: \n",
  "\r-Press [0] Wait mode \n",
  "\r-Press [1] Stop mode \n",
  "\r-Press [2] VLPR (Very Low Power Run) mode  \n",
  "\r-Press [3] VLPW (Very Low Power Wait) mode \n",
  "\r-Press [4] VLPS (Very Low Power Stop) mode  \n",
  "\r-Press [5] LLS  (Low Leakage Stop) mode  \n",
  "\r-Press [6] VLLS3 (Very Low Leakage Stop 3) mode  \n",
  "\r-Press [7] VLLS2 (Very Low Leakage Stop 2) mode \n",
  "\r-Press [8] VLLS1 (Very Low Leakage Stop 1) mode \n",
  "\r-Press [9] VLLS0 (Very Low Leakage Stop 0) mode  \n",
  "\r-Press [p] Return to previous menu  \r\n",
  NULL
};

char * const cu8WakeUpNonLLWU[]={ 
  "\r\n", 
  "\rSelect Wake Up Source: \n",
  "\r-Press [1] UART (any key) \n",
  "\r-Press [2] GPIO (any)   \n",                                             
  "\r-Press [3] LPTMR (5s)  \r\n",                                                 
  NULL
};

char * const cu8WakeUpLLWU[]={ 
  "\r\n", 
  "\rSelect Wake Up Source: \n",
  "\r-Press [1] GPIO (SW2 - K64F) \n",                                             
  "\r-Press [2] LPTMR (5s)  \r\n",                                                 
  NULL
};

char * const cu8LPPSMenu[]={ 
  "\f\r\n",
  "\r------------------------------------------------------\n",
  "\r  *LPPS: TX device goes in VLPS for 2s and wakes up   \n",
  "\r  to send wake up packet. RX device goes in VLPS      \n"
  "\r  and xcvr in reception with lpps on.                 \n",  
  "\r  * TX exits after 5 transmissions.                   \n",
  "\r  * Exit RX (LPPS) by pressing any key                \n", 
  "\r  or by receiving a wake up packet from the TX device.\n",
  "\r------------------------------------------------------\n\r",
  NULL
};
/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Interface functions
*************************************************************************************
************************************************************************************/
/***********************************************************************************
*
* PrintMenu
*
************************************************************************************/
void PrintMenu(char * const pu8Menu[], uint8_t port)
{
  uint8_t u8Index = 0;
  (void)port;
  while(pu8Menu[u8Index]){
    Serial_Print(port, pu8Menu[u8Index], gAllowToBlock_d);
    u8Index++;
  }
}
/************************************************************************************
*************************************************************************************
* private functions
*************************************************************************************
************************************************************************************/



