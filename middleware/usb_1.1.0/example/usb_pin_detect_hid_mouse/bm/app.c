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
#include "usb_host_config.h"
#include "usb_host.h"
#include "fsl_port.h"
#include "fsl_device_registers.h"
#include "usb_host_hid.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "host_mouse.h"
#include "fsl_common.h"
#if (defined(FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT > 0U))
#include "fsl_mpu.h"
#endif /* FSL_FEATURE_SOC_MPU_COUNT */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
#include "usb_phy.h"
#endif /* USB_HOST_CONFIG_EHCI */

#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI))
#error Please enable USB_HOST_CONFIG_KHCI or USB_HOST_CONFIG_EHCI in file usb_host_config.
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
#define CONTROLLER_ID kUSB_ControllerKhci0
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
#define CONTROLLER_ID kUSB_ControllerEhci0
#endif /* USB_HOST_CONFIG_EHCI */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
extern void BOARD_UsbVbusOn(uint8_t on);
extern void Device_AppInit(void);
extern void Device_AppDeinit(void);
extern void Device_AppTaskFunction(void);
extern void Host_AppInit(void);
extern void Host_AppDeinit(void);
extern void Host_AppTaskFunction(void);
extern void USB_DeviceKhciIsr(void);
extern void USB_DeviceEhciIsr(void);
extern void USB_HostKhciIsr(void);
extern void USB_HostEhciIsr(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_idPinStatus = 0;
volatile uint32_t g_idPinStatusChange = 0;
volatile uint32_t g_deviceMode = 0;
volatile USBHS_Type *ehciRegisterBase;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief get USB id pin status
 */
uint8_t USB_GetIdPinStatus(void)
{
    return ((ehciRegisterBase->OTGSC & USBHS_OTGSC_ID_MASK) >> USBHS_OTGSC_ID_SHIFT);
}
/*!
 * @brief USB isr function.
 */
#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
void USB0_IRQHandler(void)
{
    if ((g_deviceMode == 0))
    {
        USB_HostKhciIsr();
    }
    else if ((g_deviceMode == 1))
    {
        USB_DeviceKhciIsr();
    }
}
#endif /* USB_HOST_CONFIG_KHCI */

#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
void USBHS_IRQHandler(void)
{
    if ((ehciRegisterBase->OTGSC & USBHS_OTGSC_IDIS_MASK) && (ehciRegisterBase->OTGSC & USBHS_OTGSC_IDIE_MASK))
    {
        ehciRegisterBase->OTGSC |= USBHS_OTGSC_IDIS_MASK;
        if (USB_GetIdPinStatus())
        {
            g_idPinStatus = 1;
        }
        else
        {
            g_idPinStatus = 0;
        }
        g_idPinStatusChange = 1;
    }
    else
    {
        if ((g_deviceMode == 0))
        {
            USB_HostEhciIsr();
        }
        else if ((g_deviceMode == 1))
        {
            USB_DeviceEhciIsr();
        }
        else
        {
        }
    }
}
#endif /* USB_HOST_CONFIG_EHCI */

/*!
 * @brief app initialization.
 */
void APP_init(void)
{
    uint32_t usbhsBaseAddrs[] = USBHS_BASE_ADDRS;
    if (CONTROLLER_ID - kUSB_ControllerEhci0 >= (sizeof(usbhsBaseAddrs) / sizeof(usbhsBaseAddrs[0])))
    {
        usb_echo("Pin detect:controller is not found!\r\n");
        return;
    }
    ehciRegisterBase = (USBHS_Type *)usbhsBaseAddrs[CONTROLLER_ID - kUSB_ControllerEhci0];
#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
    #if (defined(FSL_FEATURE_SOC_SCG_COUNT) && (FSL_FEATURE_SOC_SCG_COUNT > 0U))
        CLOCK_EnableUsbfs0Clock(kCLOCK_IpSrcFircAsync, CLOCK_GetFreq(kCLOCK_ScgFircAsyncDiv1Clk));
    #else
        CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
    #endif
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
    CLOCK_EnableUsbhs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ);
#endif /* USB_HOST_CONFIG_EHCI */
#if ((defined FSL_FEATURE_SOC_MPU_COUNT) && (FSL_FEATURE_SOC_MPU_COUNT))
    MPU_Enable(MPU, 0);
#endif /* FSL_FEATURE_SOC_MPU_COUNT */

    if (USB_GetIdPinStatus())
    {
        g_idPinStatus = 1;
        g_deviceMode = 1;
        BOARD_UsbVbusOn(0);
        Device_AppInit();
    }
    else
    {
        g_idPinStatus = 0;
        g_deviceMode = 0;
        BOARD_UsbVbusOn(1);
        Host_AppInit();
    }
    ehciRegisterBase->OTGSC |= USBHS_OTGSC_IDIE_MASK;
}

/*!
 * @brief pin detect task function.
 */
void Pin_DetectTaskFunction(void)
{
    if (g_idPinStatusChange == 1)
    {
        if (g_idPinStatus == 0)
        {
            Device_AppDeinit();
            g_deviceMode = 0;
            BOARD_UsbVbusOn(1);
            Host_AppInit();
        }
        else
        {
            Host_AppDeinit();
            g_deviceMode = 1;
            BOARD_UsbVbusOn(0);
            Device_AppInit();
        }
        g_idPinStatusChange = 0;
    }
}

#if defined(__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    BOARD_InitHardware();

    APP_init();
    Pin_DetectTaskFunction();

    while (1)
    {
        Pin_DetectTaskFunction();
        if (g_deviceMode == 0)
        {
            Host_AppTaskFunction();
        }
        else if (g_deviceMode == 1)
        {
            Device_AppTaskFunction();
        }
        else
        {
        }
    }
}
