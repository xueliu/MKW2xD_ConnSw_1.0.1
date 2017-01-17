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

/*
 * How to setup clock using clock driver functions:
 *
 * 1. CLOCK_SetSimSafeDivs, to make sure core clock, bus clock, flexbus clock
 *    and flash clock are in allowed range during clock mode switch.
 *
 * 2. Call CLOCK_Osc0Init to setup OSC clock, if it is used in target mode.
 *
 * 3. Set MCG configuration, MCG includes three parts: FLL clock, PLL clock and
 *    internal reference clock(MCGIRCLK). Follow the steps to setup:
 *
 *    1). Call CLOCK_BootToXxxMode to set MCG to target mode.
 *
 *    2). If target mode is FBI/BLPI/PBI mode, the MCGIRCLK has been configured
 *        correctly. For other modes, need to call CLOCK_SetInternalRefClkConfig
 *        explicitly to setup MCGIRCLK.
 *
 *    3). Don't need to configure FLL explicitly, because if target mode is FLL
 *        mode, then FLL has been configured by the function CLOCK_BootToXxxMode,
 *        if the target mode is not FLL mode, the FLL is disabled.
 *
 *    4). If target mode is PEE/PBE/PEI/PBI mode, then the related PLL has been
 *        setup by CLOCK_BootToXxxMode. In FBE/FBI/FEE/FBE mode, the PLL could
 *        be enabled independently, call CLOCK_EnablePll0 explicitly in this case.
 *
 * 4. Call CLOCK_SetSimConfig to set the clock configuration in SIM.
 */

/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!ClocksProfile
product: Clocks v1.0
processor: MKW24D512xxx5
package_id: MKW24D512VHA5
mcu_data: ksdk2_0
processor_version: 1.1.0
board: FRDM-KW24
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

#include "fsl_smc.h"
#include "fsl_rtc.h"
#include "fsl_port.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MCG_PLL_DISABLE                                   0U  /*!< MCGPLLCLK disabled */
#define MODEM_CLKOUT_DIV_8                                3U  /*!< Modem CLK_OUT divider: divided by 8 */
#define MODEM_CLKOUT_ENABLED                              1U  /*!< Modem CLK_OUT clock: enabled */
#define OSC_CAP0P                                         0U  /*!< Oscillator 0pF capacitor load */
#define OSC_ER_CLK_DISABLE                                0U  /*!< Disable external reference clock */
#define RTC_OSC_CAP_LOAD_0PF                            0x0U  /*!< RTC oscillator capacity load: 0pF */
#define RTC_RTC32KCLK_PERIPHERALS_ENABLED                 1U  /*!< RTC32KCLK to other peripherals: enabled */
#define SIM_OSC32KSEL_RTC32KCLK_CLK                       2U  /*!< OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
#define SIM_PLLFLLSEL_MCGFLLCLK_CLK                       0U  /*!< PLLFLL select: MCGFLLCLK clock */
#define SIM_PLLFLLSEL_MCGPLLCLK_CLK                       1U  /*!< PLLFLL select: MCGPLLCLK clock */
#define SIM_RTC_CLKOUT_SEL_RTC1HZCLK_CLK                  0U  /*!< RTC clock output select: RTC1HzCLK clock */
#define SIM_TRACE_CLK_SEL_CORE_SYSTEM_CLK                 1U  /*!< Trace clock select: Core/system clock */
#define SIM_USB_CLK_48000000HZ                     48000000U  /*!< Input SIM frequency for USB: 48000000Hz */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetRtcClock
 * Description   : This function is used to configuring RTC clock including 
 * enabling RTC oscillator.
 * Param capLoad : RTC oscillator capacity load
 * Param enableOutPeriph : Enable (1U)/Disable (0U) clock to peripherals
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetRtcClock(uint32_t capLoad, uint8_t enableOutPeriph)
{
    /* RTC clock gate enable */
    CLOCK_EnableClock(kCLOCK_Rtc0);
    if ((RTC->CR & RTC_CR_OSCE_MASK) == 0u) { /* Only if the Rtc oscillator is not already enabled */
        /* Set the specified capacitor configuration for the RTC oscillator */
        RTC_SetOscCapLoad(RTC, capLoad);
        /* Enable the RTC 32KHz oscillator */
        RTC->CR |= RTC_CR_OSCE_MASK;
    }
    /* Output to other peripherals */
    if (enableOutPeriph) {
        RTC->CR &= ~RTC_CR_CLKO_MASK;
    }
    else {
        RTC->CR |= RTC_CR_CLKO_MASK;
    }
    /* Set the XTAL32/RTC_CLKIN frequency based on board setting. */
    CLOCK_SetXtal32Freq(BOARD_XTAL32K_CLK_HZ);
    /* Set RTC_TSR if there is fault value in RTC */
    if (RTC->SR & RTC_SR_TIF_MASK) {
        RTC -> TSR = RTC -> TSR;
    }
    /* RTC clock gate disable */
    CLOCK_DisableClock(kCLOCK_Rtc0);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetFllExtRefDiv
 * Description   : Configure FLL external reference divider (FRDIV).
 * Param frdiv   : The value to set FRDIV.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetFllExtRefDiv(uint8_t frdiv)
{
    MCG->C1 = ((MCG->C1 & ~MCG_C1_FRDIV_MASK) | MCG_C1_FRDIV(frdiv));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetModemClock
 * Description   : Set Modem clock source.
 * Param enableClkout : Enable (1U)/Disable (0U) Modem CLK_OUT
 * Param div     : The value to set Modem CLK_OUT divider.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetModemClock(uint8_t enableClkout, uint8_t div)
{
    /* Modem CLK_OUT control value */
    uint8_t ctrl_value = 0x80U;
    if (enableClkout) {
        ctrl_value |= (0x08U | div);
    }
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Modem IRQ pin initialization (MCU PTB3) */
    /* Clear PTB3 interrupt status flag */
    PORTB->ISFR = PORT_ISFR_ISF(0x08);
    /* Set PTB3 as GPIO input with rising edge detection */
    PORTB->PCR[3] = (PORT_PCR_IRQC(0x09) | PORT_PCR_MUX(0x01));
    /* Modem RESET pin initialization (MCU PTB19) */
    /* Set PTB19 as GPIO */
    PORTB->PCR[19] = (PORT_PCR_IRQC(0x00) | PORT_PCR_MUX(0x01));
    /* Set PTB19 as output */
    GPIOB->PDDR |= GPIO_PDDR_PDD(0x00080000U);
    /* Set PTB19 output to 1 */
    GPIOB->PSOR = GPIO_PSOR_PTSO(0x00080000U);
    /* Wait for modem wake-up */
    while((PORTB->PCR[3] & PORT_PCR_ISF_MASK) == 0U){}
    /* Clear IRQ detection flag */
    PORTB->PCR[3] |= PORT_PCR_ISF_MASK;

    /* ZigBee modem CLK_OUT initialization */
    /* Initialize SPI1 interface for communication with modem */
    /* SPI1 clock gate enable */
    CLOCK_EnableClock(kCLOCK_Spi1);
    /* Pin routing of modem interface */
    /* Modem MOSI pin routing */
    PORT_SetPinMux(PORTB, 16U, kPORT_MuxAlt2);
    /* Modem MISO pin routing */
    PORT_SetPinMux(PORTB, 17U, kPORT_MuxAlt2);
    /* Modem SCLK pin routing */
    PORT_SetPinMux(PORTB, 11U, kPORT_MuxAlt2);
    /* Modem CS0 pin routing */
    PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt2);
    /* Configure SPI clock polarity and phase; MSB data transferred first; frame size 8 bits; baud rate set to always fulfill modem SPI limitations */
    SPI1->CTAR[0] = SPI_CTAR_FMSZ(0x07) |
                   SPI_CTAR_PCSSCK(0x00) |
                   SPI_CTAR_PASC(0x00) |
                   SPI_CTAR_PDT(0x00) |
                   SPI_CTAR_PBR(0x00) |
                   SPI_CTAR_CSSCK(0x00) |
                   SPI_CTAR_ASC(0x00) |
                   SPI_CTAR_DT(0x00) |
                   SPI_CTAR_BR(0x01);
    /* Configure SPI as master with inactive state of PCS0 in high */
    SPI1->MCR = SPI_MCR_MSTR_MASK |
               SPI_MCR_DCONF(0x00) |
               SPI_MCR_PCSIS(0x01) |
               SPI_MCR_SMPL_PT(0x00);
    /* Send CLK_OUT_CTRL register address */
    SPI1->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(0x01) | 0x3CU;
    /* Wait until address is send */
    while ((SPI1->SR & (SPI_SR_TFFF_MASK | SPI_SR_RFDF_MASK)) == 0U) {}
    /* Clear Tx and Rx status flags */
    SPI1->SR |= SPI_SR_TFFF_MASK | SPI_SR_RFDF_MASK;
     /* Discard received modem status */
    (void)SPI1->POPR;
    /* Send CLK_OUT_CTRL register value */
    SPI1->PUSHR = SPI_PUSHR_PCS(0x01) | ctrl_value;
    /* Wait until value is send */
    while ((SPI1->SR & (SPI_SR_TFFF_MASK | SPI_SR_RFDF_MASK)) == 0U) {}
    /* Clear Tx and Rx status flags */
    SPI1->SR |= SPI_SR_TFFF_MASK | SPI_SR_RFDF_MASK;
    /* Discard received modem status */
    (void)SPI1->POPR;
    /* SPI is returned back to its initialization state */
    /* Stop the SPI */
    SPI1->MCR |= SPI_MCR_HALT_MASK;
    /* Wait until SPI is in stop mode */
    while ((SPI1->SR & SPI_SR_TXRXS_MASK) != 0U) {}
    /* Disable the SPI */
    SPI1->MCR |= SPI_MCR_MDIS_MASK;
    /* Reset value of MCR */
    SPI1->MCR = SPI_MCR_DCONF(0x00) |
               SPI_MCR_PCSIS(0x00) |
               SPI_MCR_MDIS_MASK |
               SPI_MCR_SMPL_PT(0x00) |
               SPI_MCR_HALT_MASK;
    /* Reset value of CTAR0 */
    SPI1->CTAR[0] = SPI_CTAR_FMSZ(0x0F) |
                   SPI_CTAR_PCSSCK(0x00) |
                   SPI_CTAR_PASC(0x00) |
                   SPI_CTAR_PDT(0x00) |
                   SPI_CTAR_PBR(0x00) |
                   SPI_CTAR_CSSCK(0x00) |
                   SPI_CTAR_ASC(0x00) |
                   SPI_CTAR_DT(0x00) |
                   SPI_CTAR_BR(0x00);
    /* SPI1 clock gate disable */
    CLOCK_DisableClock(kCLOCK_Spi1);
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!Configuration
name: BOARD_BootClockRUN
outputs:
- {id: Bus_clock.outFreq, value: 48 MHz}
- {id: Core_clock.outFreq, value: 48 MHz}
- {id: ERCLK32K.outFreq, value: 32.768 kHz}
- {id: Flash_clock.outFreq, value: 24 MHz}
- {id: LPO_clock.outFreq, value: 1 kHz}
- {id: MCGIRCLK.outFreq, value: 4 MHz}
- {id: OSCERCLK.outFreq, value: 4 MHz}
- {id: PLLFLLCLK.outFreq, value: 48 MHz}
- {id: RTC_CLKOUT.outFreq, value: 1 Hz}
- {id: System_clock.outFreq, value: 48 MHz}
- {id: TRACECLKIN.outFreq, value: 48 MHz}
- {id: USB48MCLK.outFreq, value: 48 MHz}
settings:
- {id: MCGMode, value: PEE}
- {id: ModemInitConfig, value: 'yes'}
- {id: CLKOUTConfig, value: 'yes'}
- {id: MCG.FCRDIV.scale, value: '1', locked: true}
- {id: MCG.FRDIV.scale, value: '32'}
- {id: MCG.IRCS.sel, value: MCG.FCRDIV}
- {id: MCG.IREFS.sel, value: MCG.FRDIV}
- {id: MCG.PLLS.sel, value: MCG.PLL}
- {id: MCG.PRDIV.scale, value: '2'}
- {id: MCG_C1_IRCLKEN_CFG, value: Enabled}
- {id: MCG_C2_RANGE0_CFG, value: High}
- {id: MCG_C2_RANGE0_FRDIV_CFG, value: High}
- {id: MCRFFCLKAllowConfig, value: 'no'}
- {id: OSC_CR_ERCLKEN_CFG, value: Enabled}
- {id: RTCCLKOUTConfig, value: 'yes'}
- {id: RTC_CR_OSCE_CFG, value: Enabled}
- {id: SIM.OSC32KSEL.sel, value: RTC.RTC32KCLK}
- {id: SIM.OUTDIV4.scale, value: '2', locked: true}
- {id: SIM.PLLFLLSEL.sel, value: MCG.MCGPLLCLK}
- {id: TraceClkConfig, value: 'yes'}
- {id: USBClkConfig, value: 'yes'}
sources:
- {id: OSC.OSC.outFreq, value: 4 MHz, enabled: true}
- {id: RTC.RTC32kHz.outFreq, value: 32.768 kHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const mcg_config_t mcgConfig_BOARD_BootClockRUN =
    {
        .mcgMode = kMCG_ModePEE,                  /* PEE - PLL Engaged External */
        .irclkEnableMode = kMCG_IrclkEnable,      /* MCGIRCLK enabled, MCGIRCLK disabled in STOP mode */
        .ircs = kMCG_IrcFast,                     /* Fast internal reference clock selected */
        .fcrdiv = 0x0U,                           /* Fast IRC divider: divided by 1 */
        .frdiv = 0x0U,                            /* FLL reference clock divider: divided by 32 */
        .drs = kMCG_DrsLow,                       /* Low frequency range */
        .dmx32 = kMCG_Dmx32Default,               /* DCO has a default range of 25% */
        .oscsel = kMCG_OscselOsc,                 /* Selects System Oscillator (OSCCLK) */
        .pll0Config =
            {
                .enableMode = MCG_PLL_DISABLE,    /* MCGPLLCLK disabled */
                .prdiv = 0x1U,                    /* PLL Reference divider: divided by 2 */
                .vdiv = 0x0U,                     /* VCO divider: multiplied by 24 */
            },
    };
const sim_clock_config_t simConfig_BOARD_BootClockRUN =
    {
        .pllFllSel = SIM_PLLFLLSEL_MCGPLLCLK_CLK, /* PLLFLL select: MCGPLLCLK clock */
        .er32kSrc = SIM_OSC32KSEL_RTC32KCLK_CLK,  /* OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
        .clkdiv1 = 0x10000U,                      /* SIM_CLKDIV1 - OUTDIV1: /1, OUTDIV2: /1, OUTDIV4: /2 */
    };
const osc_config_t oscConfig_BOARD_BootClockRUN =
    {
        .freq = 4000000U,                         /* Oscillator frequency: 4000000Hz */
        .capLoad = (OSC_CAP0P),                   /* Oscillator capacity load: 0pF */
        .workMode = kOSC_ModeExt,                 /* Use external clock */
        .oscerConfig =
            {
                .enableMode = kOSC_ErClkEnable,   /* Enable external reference clock, disable external reference clock in STOP mode */
            }
    };

/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    /* Set the system clock dividers in SIM to safe value. */
    CLOCK_SetSimSafeDivs();
    /* Configure RTC clock including enabling RTC oscillator. */
    CLOCK_CONFIG_SetRtcClock(RTC_OSC_CAP_LOAD_0PF, RTC_RTC32KCLK_PERIPHERALS_ENABLED);
    /* Configure Modem CLK_OUT clock. */
    CLOCK_CONFIG_SetModemClock(MODEM_CLKOUT_ENABLED, MODEM_CLKOUT_DIV_8);
    /* Initializes OSC0 according to board configuration. */
    CLOCK_InitOsc0(&oscConfig_BOARD_BootClockRUN);
    CLOCK_SetXtal0Freq(oscConfig_BOARD_BootClockRUN.freq);
    /* Configure FLL external reference divider (FRDIV). */
    CLOCK_CONFIG_SetFllExtRefDiv(mcgConfig_BOARD_BootClockRUN.frdiv);
    /* Set MCG to PEE mode. */
    CLOCK_BootToPeeMode(mcgConfig_BOARD_BootClockRUN.oscsel,
                        kMCG_PllClkSelPll0,
                        &mcgConfig_BOARD_BootClockRUN.pll0Config);
    /* Configure the Internal Reference clock (MCGIRCLK). */
    CLOCK_SetInternalRefClkConfig(mcgConfig_BOARD_BootClockRUN.irclkEnableMode,
                                  mcgConfig_BOARD_BootClockRUN.ircs, 
                                  mcgConfig_BOARD_BootClockRUN.fcrdiv);
    /* Set the clock configuration in SIM module. */
    CLOCK_SetSimConfig(&simConfig_BOARD_BootClockRUN);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
    /* Set RTC_CLKOUT source. */
    CLOCK_SetRtcClkOutClock(SIM_RTC_CLKOUT_SEL_RTC1HZCLK_CLK);
    /* Enable USB FS clock. */
    CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcPll0, SIM_USB_CLK_48000000HZ);
    /* Set debug trace clock source. */
    CLOCK_SetTraceClock(SIM_TRACE_CLK_SEL_CORE_SYSTEM_CLK);
}

/*******************************************************************************
 ********************* Configuration BOARD_BootClockVLPR ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR THE CLOCKS TOOL *****************************
!!Configuration
name: BOARD_BootClockVLPR
outputs:
- {id: Bus_clock.outFreq, value: 4 MHz}
- {id: Core_clock.outFreq, value: 4 MHz}
- {id: Flash_clock.outFreq, value: 800 kHz}
- {id: LPO_clock.outFreq, value: 1 kHz}
- {id: MCGIRCLK.outFreq, value: 4 MHz}
- {id: System_clock.outFreq, value: 4 MHz}
settings:
- {id: MCGMode, value: BLPI}
- {id: powerMode, value: VLPR}
- {id: MCG.CLKS.sel, value: MCG.IRCS}
- {id: MCG.FCRDIV.scale, value: '1'}
- {id: MCG.IRCS.sel, value: MCG.FCRDIV}
- {id: MCG_C1_IRCLKEN_CFG, value: Enabled}
- {id: SIM.OSC32KSEL.sel, value: RTC.RTC32KCLK}
- {id: SIM.OUTDIV4.scale, value: '5'}
sources:
- {id: OSC.OSC.outFreq, value: 4 MHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE CLOCKS TOOL **/

/*******************************************************************************
 * Variables for BOARD_BootClockVLPR configuration
 ******************************************************************************/
const mcg_config_t mcgConfig_BOARD_BootClockVLPR =
    {
        .mcgMode = kMCG_ModeBLPI,                 /* BLPI - Bypassed Low Power Internal */
        .irclkEnableMode = kMCG_IrclkEnable,      /* MCGIRCLK enabled, MCGIRCLK disabled in STOP mode */
        .ircs = kMCG_IrcFast,                     /* Fast internal reference clock selected */
        .fcrdiv = 0x0U,                           /* Fast IRC divider: divided by 1 */
        .frdiv = 0x0U,                            /* FLL reference clock divider: divided by 1 */
        .drs = kMCG_DrsLow,                       /* Low frequency range */
        .dmx32 = kMCG_Dmx32Default,               /* DCO has a default range of 25% */
        .oscsel = kMCG_OscselOsc,                 /* Selects System Oscillator (OSCCLK) */
        .pll0Config =
            {
                .enableMode = MCG_PLL_DISABLE,    /* MCGPLLCLK disabled */
                .prdiv = 0x0U,                    /* PLL Reference divider: divided by 1 */
                .vdiv = 0x0U,                     /* VCO divider: multiplied by 24 */
            },
    };
const sim_clock_config_t simConfig_BOARD_BootClockVLPR =
    {
        .pllFllSel = SIM_PLLFLLSEL_MCGFLLCLK_CLK, /* PLLFLL select: MCGFLLCLK clock */
        .er32kSrc = SIM_OSC32KSEL_RTC32KCLK_CLK,  /* OSC32KSEL select: RTC32KCLK clock (32.768kHz) */
        .clkdiv1 = 0x40000U,                      /* SIM_CLKDIV1 - OUTDIV1: /1, OUTDIV2: /1, OUTDIV4: /5 */
    };
const osc_config_t oscConfig_BOARD_BootClockVLPR =
    {
        .freq = 0U,                               /* Oscillator frequency: 0Hz */
        .capLoad = (OSC_CAP0P),                   /* Oscillator capacity load: 0pF */
        .workMode = kOSC_ModeExt,                 /* Use external clock */
        .oscerConfig =
            {
                .enableMode = OSC_ER_CLK_DISABLE, /* Disable external reference clock */
            }
    };

/*******************************************************************************
 * Code for BOARD_BootClockVLPR configuration
 ******************************************************************************/
void BOARD_BootClockVLPR(void)
{
    /* Set the system clock dividers in SIM to safe value. */
    CLOCK_SetSimSafeDivs();
    /* Set MCG to BLPI mode. */
    CLOCK_BootToBlpiMode(mcgConfig_BOARD_BootClockVLPR.fcrdiv,
                         mcgConfig_BOARD_BootClockVLPR.ircs,
                         mcgConfig_BOARD_BootClockVLPR.irclkEnableMode);
    /* Set the clock configuration in SIM module. */
    CLOCK_SetSimConfig(&simConfig_BOARD_BootClockVLPR);
    /* Set VLPR power mode. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
#if (defined(FSL_FEATURE_SMC_HAS_LPWUI) && FSL_FEATURE_SMC_HAS_LPWUI)
    SMC_SetPowerModeVlpr(SMC, false);
#else
    SMC_SetPowerModeVlpr(SMC);
#endif
    while (SMC_GetPowerModeState(SMC) != kSMC_PowerStateVlpr)
    {
    }
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKVLPR_CORE_CLOCK;
}

