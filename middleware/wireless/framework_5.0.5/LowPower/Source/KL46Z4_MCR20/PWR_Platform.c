/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file PWR_Platform.c
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

/*****************************************************************************
*                               INCLUDED HEADERS                            *
*---------------------------------------------------------------------------*
* Add to this section all the headers that this module needs to include.    *
*---------------------------------------------------------------------------*
*****************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_common.h"

#include "PWRLib.h"
#include "PWR_Configuration.h"
#include "PWR_Interface.h"
#include "PWR_Platform.h"

#include "TimersManager.h"
#include "MCR20Drv.h"
#include "AspInterface.h"
#include "Keyboard.h"
#include "SerialManager.h"

#include "fsl_smc.h"
#include "fsl_clock.h" 
#include "fsl_llwu.h"
#include "fsl_lptmr.h"

#include "TMR_Adapter.h"

#include "PhyTypes.h"
#include "Phy.h"

#include "board.h"


/*****************************************************************************
*                             PUBLIC MACROS                                *
*---------------------------------------------------------------------------*
* Add to this section all the access macros, registers mappings, bit access *
* macros, masks, flags etc ...
*---------------------------------------------------------------------------*
*****************************************************************************/

/*****************************************************************************
*                        PRIVATE TYPE DEFINITIONS                           *
*---------------------------------------------------------------------------*
* Add to this section all the data types definitions: stuctures, unions,    *
* enumerations, typedefs ...                                                *
*---------------------------------------------------------------------------*
*****************************************************************************/
typedef void (*xcvr_ptr)(void);

typedef enum power_mode_name_tag
{
    kPowerModeRun = 0,
    kPowerModeWait,
    kPowerModeStop,
    kPowerModeVlpr,
    kPowerModeVlpw,
    kPowerModeVlps,
    kPowerModeLls,
    kPowerModeVlls
}power_mode_name_t;

typedef struct power_modes_config_tag
{
    power_mode_name_t powerModeName;
    uint8_t stopSubMode;
}power_modes_config_t;

/*****************************************************************************
*                               PUBLIC VARIABLES                            *
*---------------------------------------------------------------------------*
* Add to this section all the variables and constants that have global      *
* (project) scope.                                                          *
* These variables / constants can be accessed outside this module.          *
* These variables / constants shall be preceded by the 'extern' keyword in  *
* the interface header.                                                     *
*---------------------------------------------------------------------------*
*****************************************************************************/

/*****************************************************************************
*                               PRIVATE VARIABLES                           *
*---------------------------------------------------------------------------*
* Add to this section all the variables and constants that have local       *
* (file) scope.                                                             *
* Each of this declarations shall be preceded by the 'static' keyword.      *
* These variables / constants cannot be accessed outside this module.       *
*---------------------------------------------------------------------------*
*****************************************************************************/
static uint8_t gWakeUpSource = 0;
static power_modes_config_t mPWRP_Config;
static uint16_t gu16WakeUpTime = 0;
static bool_t bRadioIRQWakeUp = FALSE;
static uint32_t sim_outdiv4 = 0xFFFFFFFF;
/*****************************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES                    *
*---------------------------------------------------------------------------*
* Add to this section all the functions prototypes that have local (file)   *
* scope.                                                                    *
* These functions cannot be accessed outside this module.                   *
* These declarations shall be preceded by the 'static' keyword.             *
*---------------------------------------------------------------------------*
*****************************************************************************/
static xcvr_ptr pSendRadioToMode = NULL;
static void Disable_WakeUp(bool_t);
static void Enable_WakeUp(bool_t);
static void PWRP_SetMode(SMC_Type * base, power_modes_config_t* config);
static void Enter_LPMode(power_mode_name_t mode, uint8_t submode);
static void PWRP_UnregisterLLWUPin(uint32_t pinBitmap);
static void PWRP_RegisterLLWUPin(uint32_t pinBitmap);
static void PWRP_UnregisterLLWUModule(uint8_t modPos);
static void PWRP_RegisterLLWUModule(uint8_t modPos);

/*****************************************************************************
*                           PUBLIC FUNCTIONS PROTOTYPES                     *
*---------------------------------------------------------------------------*
* Add to this section all the functions prototypes that have local (file)   *
* scope.                                                                    *
* These functions cannot be accessed outside this module.                   *
* These declarations shall be preceded by the 'static' keyword.             *
*---------------------------------------------------------------------------*
*****************************************************************************/
/*use interrupt handlers from PWRLib*/
extern void PWRLib_LLWU_Isr(void);
extern void PWRLib_LPTMR_Isr(void);

/*---------------------------------------------------------------------------
* Name: PWRP_ConfigureXCVRMode
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void PWRP_ConfigureXCVRMode(pwrp_xcvr_modes_t mode)
{
    switch(mode)
    {
    case pwrp_xcvr_rx:
        pSendRadioToMode = XCVR_Enter_RX;
        break;
    case pwrp_xcvr_tx:
        pSendRadioToMode = XCVR_Enter_TX;
        break;
    case pwrp_xcvr_hibernate:
        pSendRadioToMode = PWRLib_Radio_Enter_Hibernate;
        break;
    case pwrp_xcvr_doze:
        pSendRadioToMode = PWRLib_Radio_Enter_Doze;
        break;
    case pwrp_xcvr_autoDoze:
        pSendRadioToMode = PWRLib_Radio_Enter_AutoDoze;
        break;
    case pwrp_xcvr_idle:
        pSendRadioToMode = PWRLib_Radio_Enter_Idle;
        break;
    default:
        pSendRadioToMode = NULL;
        break;
    }
}

/*---------------------------------------------------------------------------
* Name: PWRP_Init
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void PWRP_Init(void)
{
    /*Init LPTMR*/
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;/*Un-gate clock for lptrm*/
    
    LPTMR0->CSR = (LPTMR_CSR_TCF_MASK | LPTMR_CSR_TPS(0x00)); /* Clear control register */
    LPTMR0->CMR = LPTMR_CMR_COMPARE(0x02); /* Set up compare register */
    
    /*enable interrupt*/
    OSA_InstallIntHandler(LPTMR0_IRQn, PWRLib_LPTMR_Isr);
    NVIC_SetPriority(LPTMR0_IRQn, 0x80 >> (8 - __NVIC_PRIO_BITS));
    NVIC_EnableIRQ(LPTMR0_IRQn);
    
    LPTMR0->CSR = (LPTMR_CSR_TIE_MASK | LPTMR_CSR_TPS(0x00)); /* Set up control register */
    LPTMR0->PSR = (LPTMR_PSR_PBYP(1)) | (LPTMR_PSR_PCS(1)); /*use LPO and bypass prescaler*/
    
    /*Un-register modules from LLWU*/
    PWRP_UnregisterLLWUPin(BOARD_LLWU_PIN_ENABLE_BITMAP);
    
    /*LPTMR module connected to LLWU*/
    PWRP_UnregisterLLWUModule(gPWRLib_LLWU_WakeupModuleEnable_LPTMR_c);
    
    /* Clear external pins wakeup interrupts */
    LLWU->F1 = LLWU->F1; 
    LLWU->F2 = LLWU->F2; 
    
    OSA_InstallIntHandler(LLWU_IRQn, PWRLib_LLWU_Isr);
    NVIC_SetPriority(LLWU_IRQn, 0x80 >> (8 - __NVIC_PRIO_BITS));
    NVIC_EnableIRQ(LLWU_IRQn);
}

/*---------------------------------------------------------------------------
* Name: PWRP_UpdateLPTMRCount
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void PWRP_UpdateLPTMRCount(uint16_t u16TimeInMs)
{
    gu16WakeUpTime = u16TimeInMs;
}

/*---------------------------------------------------------------------------
* Name: PWRP_SetWakeUpSource
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t PWRP_SetWakeUpSource(uint8_t u8WUSource)
{
    if ( u8WUSource != LPTMR_WAKE_UP && u8WUSource != UART_WAKE_UP && u8WUSource != GPIO_WAKE_UP
        && u8WUSource != XCVR_IRQ_WAKE_UP )
    {
        return PWRP_ERR;
    }
    gWakeUpSource = u8WUSource;
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: PWRP_GetWakeUpReason
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
uint8_t PWRP_GetWakeUpReason(void)
{
    if(PWRLib_MCU_WakeupReason.Bits.FromKeyBoard && gWakeUpSource == GPIO_WAKE_UP)
    {
        return GPIO_WAKE_UP;
    }
    if(PWRLib_MCU_WakeupReason.Bits.FromLPTMR && gWakeUpSource == LPTMR_WAKE_UP)
    {
        return LPTMR_WAKE_UP;
    }
    if(PWRLib_MCU_WakeupReason.Bits.FromUART && gWakeUpSource == UART_WAKE_UP)
    {
        return UART_WAKE_UP;
    }
    if(bRadioIRQWakeUp && gWakeUpSource == XCVR_IRQ_WAKE_UP)
    {
        return XCVR_IRQ_WAKE_UP;
    }
    
    return UNKNOWN_WAKE_UP;
}

/*---------------------------------------------------------------------------
* Name: PWRP_UnregisterLLWUPin
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/ 
static void PWRP_UnregisterLLWUPin(uint32_t pinBitmap)
{
    if(pinBitmap == 0)
    {
        return;
    }
    uint16_t pinEn16 = pinBitmap;
    uint32_t PinEn32 = 0;
    uint32_t i;
    for(i=0; pinEn16 ; i++)
    {
        if(pinEn16 & 0x1)
        {
            PinEn32 |= 0x3<<(i<<1);
        }
        pinEn16 >>= 1;
    }
    LLWU->PE1 &= ~(PinEn32&0xff);
    LLWU->PE2 &= ~((PinEn32>>8)&0xff);
    LLWU->PE3 &= ~((PinEn32>>16)&0xff);
    LLWU->PE4 &= ~((PinEn32>>24)&0xff);
}

/*---------------------------------------------------------------------------
* Name: PWRP_RegisterLLWUPin
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/ 
static void PWRP_RegisterLLWUPin(uint32_t pinBitmap)
{
    if(pinBitmap == 0)
    {
        return;
    }
    uint16_t pinEn16 = pinBitmap;
    uint32_t PinEn32 = 0;
    uint32_t i;
    for(i=0; pinEn16 ; i++)
    {
        if(pinEn16 & 0x1)
        {
            PinEn32 |= 0x3<<(i<<1);
        }
        pinEn16 >>= 1;
    }
    LLWU->PE1 |= PinEn32&0xff;
    LLWU->PE2 |= (PinEn32>>8)&0xff;
    LLWU->PE3 |= (PinEn32>>16)&0xff;
    LLWU->PE4 |= (PinEn32>>24)&0xff;
}

/*---------------------------------------------------------------------------
* Name: PWRP_RegisterLLWUModule
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/  
static void PWRP_RegisterLLWUModule(uint8_t modPos)
{
    if(modPos > 128)
    {
        return;
    }
    LLWU->ME |= (modPos);
}

/*---------------------------------------------------------------------------
* Name: PWRP_UnregisterLLWUModule
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
static void PWRP_UnregisterLLWUModule(uint8_t modPos)
{
    if(modPos > 127)
    {
        return;
    }
    LLWU->ME &= ~(modPos);
}
/*---------------------------------------------------------------------------
* Name: Enter_LPMode
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
static void Enter_LPMode(power_mode_name_t mode, uint8_t submode)
{
    bool_t bLLWUWakeUp = FALSE;
    uint32_t temp = 0;
    
    OSA_InterruptDisable();
    
    /*disable systick*/
    temp = SysTick->CTRL & (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    
    mPWRP_Config.powerModeName = mode;
    if(mode == kPowerModeLls || mode == kPowerModeVlls)
    {
        mPWRP_Config.stopSubMode = submode;
        bLLWUWakeUp = TRUE;
    }
    else
    {
        (void)submode;
        bLLWUWakeUp = FALSE;
    }
    bRadioIRQWakeUp = FALSE;
    PWRLib_MCU_WakeupReason.AllBits = 0;
    /*enables wake up source based on previously applied configuration*/
    Enable_WakeUp(bLLWUWakeUp);
    /* configure MCG in BLPI mode */      
    BOARD_EnterLowPowerCb();
    
    if(mode == kPowerModeVlps || mode == kPowerModeVlpw)
    {
        /* divide flash clock by 5 */
        if(sim_outdiv4 == 0xFFFFFFFF)
        {
            sim_outdiv4  = (SIM->CLKDIV1 & 0xF0000);
            SIM->CLKDIV1 = (SIM->CLKDIV1 & 0xFFF0FFFF) | (4 << 16);
        }
    }
    /* send radio in configured mode*/
    if(pSendRadioToMode != NULL)
    {
        pSendRadioToMode();
    }
    
    /*enter configure low power mode*/
    PWRP_SetMode(SMC, &mPWRP_Config);
    
    /* checks source of wakeup and disables it */
    Disable_WakeUp(bLLWUWakeUp);
 
    /* configure Radio in auto-doze mode if was changed */
    if(pSendRadioToMode != NULL)
    {
        PWRLib_Radio_Enter_AutoDoze();
        /*force idle in case cont tx or rx was previously used*/
        ASP_TelecTest(gTestForceIdle_c);
    }
    
    BOARD_ExitLowPowerCb();
    
    if(mode == kPowerModeVlps || mode == kPowerModeVlpw)
    {
        /* restore flash clock divider */
        if(sim_outdiv4 != 0xFFFFFFFF)
        {
            SIM->CLKDIV1 = (SIM->CLKDIV1 & 0xFFF0FFFF) | sim_outdiv4;
            sim_outdiv4  = 0xFFFFFFFF;
        }
    }
    /* restore the state of SysTick */
    SysTick->CTRL |= temp;
    
    OSA_InterruptEnable();
}

/*---------------------------------------------------------------------------
* Name: Enable_WakeUp
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
static void Enable_WakeUp(bool_t bWakeFromLLWU)
{
    if(bWakeFromLLWU)
    {
        if(gWakeUpSource == LPTMR_WAKE_UP)
        {
            /* enable LPTMR as wakeup source for LLWU module */
            PWRP_RegisterLLWUModule(gPWRLib_LLWU_WakeupModuleEnable_LPTMR_c);
            LPTMR0->CMR = gu16WakeUpTime;
            LPTMR0->CNR = 0;
            LPTMR0->CSR |= LPTMR_CSR_TEN(1);
        }
        else if(gWakeUpSource == GPIO_WAKE_UP)
        {
            PWRP_RegisterLLWUPin(BOARD_LLWU_PIN_ENABLE_BITMAP);
        }
    }
    else
    {
        if(gWakeUpSource == UART_WAKE_UP)
        {
              Serial_EnableLowPowerWakeup(gSerialMgrLpsci_c);
        }
        else if(gWakeUpSource == LPTMR_WAKE_UP)
        {
            LPTMR0->CMR = gu16WakeUpTime;
            LPTMR0->CNR = 0;
            LPTMR0->CSR |= LPTMR_CSR_TEN(1);
        }
        else if(gWakeUpSource == GPIO_WAKE_UP)
        {
            
        }
        else if(gWakeUpSource == XCVR_IRQ_WAKE_UP)
        {
        }
    }
}

/*---------------------------------------------------------------------------
* Name: Disable_WakeUp
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
static void Disable_WakeUp(bool_t bWakeFromLLWU)
{
    if(bWakeFromLLWU)
    {
        PWRLib_LLWU_UpdateWakeupReason();
        
        if(PWRLib_MCU_WakeupReason.Bits.FromKeyBoard)
        {
            PWRP_UnregisterLLWUPin(BOARD_LLWU_PIN_ENABLE_BITMAP);
        }
        else if(PWRLib_MCU_WakeupReason.Bits.FromLPTMR)
        {
            LPTMR0->CSR |= (LPTMR_CSR_TCF_MASK);
            LPTMR0->CSR &= ~(LPTMR_CSR_TEN_MASK);
            PWRP_UnregisterLLWUPin(gPWRLib_LLWU_WakeupModuleEnable_LPTMR_c);
        }
    }
    else
    {
        if(KBD_IsWakeUpSource())
        {
            PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = TRUE;
        }
    if(Serial_IsWakeUpSource(gSerialMgrLpsci_c))
        {
            PWRLib_MCU_WakeupReason.Bits.FromUART = TRUE;
            Serial_DisableLowPowerWakeup(gSerialMgrLpsci_c);
        }
        else if (gWakeUpSource == UART_WAKE_UP)
        {
            Serial_DisableLowPowerWakeup(gSerialMgrLpsci_c);
        }
        if(LPTMR0->CSR & LPTMR_CSR_TCF_MASK)
        {
            LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
            LPTMR0->CSR &= ~(LPTMR_CSR_TEN_MASK);
            PWRLib_MCU_WakeupReason.Bits.FromLPTMR = TRUE;
        }
        else if(gWakeUpSource == LPTMR_WAKE_UP)
        {
            LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
            LPTMR0->CSR &= ~(LPTMR_CSR_TEN_MASK);
        }
        
        /*check for radio wake-up*/
        if(MCR20Drv_IsIrqPending())
        {
            bRadioIRQWakeUp = TRUE;
        }
    }
}

/*---------------------------------------------------------------------------
* Name: Disable_WakeUp
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
static void PWRP_SetMode(SMC_Type * base, power_modes_config_t* config)
{
    //  smc_power_mode_lls_config_t config_lls;
    smc_power_mode_vlls_config_t config_vlls;
    
    switch (config->powerModeName)
    {
    case kPowerModeRun:
        SMC_SetPowerModeRun(base);
        break;
    case kPowerModeWait:
        SMC_SetPowerModeWait(base);
        break;
    case kPowerModeStop:
        SMC_SetPowerModeStop(base, (smc_partial_stop_option_t)0);
        break;
    case kPowerModeVlpr:
        SMC_SetPowerModeVlpr(base);
        break;
    case kPowerModeVlpw:
//        SMC_SetPowerModeVlpr(base);
//        while(kSMC_PowerStateVlpr != SMC_GetPowerModeState(base));
        SMC_SetPowerModeVlpw(base);
        break;
    case kPowerModeVlps:
        SMC_SetPowerModeVlps(base);
        break;
    case kPowerModeLls:
        SMC_SetPowerModeLls(base);
        break;
    case kPowerModeVlls:
        config_vlls.enablePorDetectInVlls0 = false;
        //    config_vlls.enableRam2InVlls2 = false;
        config_vlls.subMode = (smc_stop_submode_t)config->stopSubMode;
        SMC_SetPowerModeVlls(base, &config_vlls);
        break;
    }
}
/*---------------------------------------------------------------------------
* Name: Enter_RUN
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_RUN (void)
{
    OSA_InterruptDisable();
    
    mPWRP_Config.powerModeName = kPowerModeRun;
    PWRP_SetMode(SMC, &mPWRP_Config);
    
    if(pSendRadioToMode)
    {
        pSendRadioToMode();
    }
    
    BOARD_ExitLowPowerCb();
    
    /* restore flash clock divider */
    if(sim_outdiv4 != 0xFFFFFFFF)
    {
        SIM->CLKDIV1 = (SIM->CLKDIV1 & 0xFFF0FFFF) | sim_outdiv4;
        sim_outdiv4  = 0xFFFFFFFF;
    }
    OSA_InterruptEnable();
    
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_VLPR
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_VLPR ( void )
{
    OSA_InterruptDisable();
    
    BOARD_EnterLowPowerCb();
    
    /* divide flash clock by 5 */
    if(sim_outdiv4 == 0xFFFFFFFF)
    {
        sim_outdiv4  = (SIM->CLKDIV1 & 0xF0000);
        SIM->CLKDIV1 = (SIM->CLKDIV1 & 0xFFF0FFFF) | (4 << 16);
    }
    if(pSendRadioToMode)
    {
        pSendRadioToMode();
    }
    
    mPWRP_Config.powerModeName = kPowerModeVlpr;
    PWRP_SetMode(SMC, &mPWRP_Config);
    
    OSA_InterruptEnable();
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_Wait
* Description: -
* Parameters:
* switch2InternalClock : if TRUE, 
*                        before entering wait switch to internal clock
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_Wait ( void )
{
    PWRLib_MCU_WakeupReason.AllBits = 0;
    
    if(gWakeUpSource == NO_WAKE_UP)
    {
        return PWRP_ERR;
    }
    
    StackTimer_Disable();
    
    Enter_LPMode(kPowerModeWait, 0);
    
    StackTimer_Enable();
    
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_Stop
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_Stop ( void )
{
    if(gWakeUpSource == NO_WAKE_UP)
    {
        return PWRP_ERR;
    }
    StackTimer_Disable();
    
    Enter_LPMode(kPowerModeStop, (smc_stop_submode_t)0);
    
    StackTimer_Enable();
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_VLPW
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_VLPW ( void )
{
    if(gWakeUpSource == NO_WAKE_UP)
    {
        return PWRP_ERR;
    }
    
    StackTimer_Disable();
    
    Enter_LPMode(kPowerModeVlpw, 0);
    
    StackTimer_Enable();
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_VLPS
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_VLPS ( void )
{
    if(gWakeUpSource == NO_WAKE_UP)
    {
        return PWRP_ERR;
    }
    
    StackTimer_Disable();
    
    Enter_LPMode(kPowerModeVlps, 0);
    
    StackTimer_Enable();
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_LLS
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_LLS  ( void )
{
    PWRLib_MCU_WakeupReason.AllBits = 0;
    
    if(gWakeUpSource == NO_WAKE_UP || gWakeUpSource == UART_WAKE_UP)
    {
        return PWRP_ERR;
    }
    
    Enter_LPMode(kPowerModeLls, 3);
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: Enter_VLLS
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
pwrp_status_t Enter_VLLS (uint8_t u8SubMode)
{
    PWRLib_MCU_WakeupReason.AllBits = 0;
    
    if(gWakeUpSource == NO_WAKE_UP || gWakeUpSource == UART_WAKE_UP)
    {
        return PWRP_ERR;
    }
    Enter_LPMode(kPowerModeVlls, (smc_stop_submode_t)u8SubMode);
    return PWRP_OK;
}

/*---------------------------------------------------------------------------
* Name: XCVR_Enter_RX
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void XCVR_Enter_RX ( void )
{
    ASP_TelecTest(gTestContinuousRx_c);
}

/*---------------------------------------------------------------------------
* Name: XCVR_Enter_TX
* Description: -
* Parameters: -
* Return: -
*---------------------------------------------------------------------------*/
void XCVR_Enter_TX ( void )
{
    ASP_TelecTest(gTestPulseTxPrbs9_c);
}