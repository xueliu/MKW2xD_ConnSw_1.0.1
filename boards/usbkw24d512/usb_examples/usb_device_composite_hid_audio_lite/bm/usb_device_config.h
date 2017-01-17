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

#ifndef _USB_DEVICE_CONFIG_H_
#define _USB_DEVICE_CONFIG_H_

/*******************************************************************************
* Definitions
******************************************************************************/
/*!
 * @addtogroup usb_device_configuration
 * @{
 */

/*!
 * @name Hardware instance define
 * @{
 */

/*! @brief KHCI instance count */
#define USB_DEVICE_CONFIG_KHCI (1U)

/*! @brief EHCI instance count */
#define USB_DEVICE_CONFIG_EHCI (0U)

/*! @brief Device instance count, the sum of KHCI and EHCI instance counts*/
#define USB_DEVICE_CONFIG_NUM (USB_DEVICE_CONFIG_KHCI + USB_DEVICE_CONFIG_EHCI)

/* @} */

/*!
 * @name class instance define
 * @{
 */

/*! @brief HID instance count */
#define USB_DEVICE_CONFIG_HID (1U)

/*! @brief CDC ACM instance count */
#define USB_DEVICE_CONFIG_CDC_ACM (0U)

/*! @brief MSC instance count */
#define USB_DEVICE_CONFIG_MSC (0U)

/*! @brief Audio instance count */
#define USB_DEVICE_CONFIG_AUDIO (1U)

/*! @brief PHDC instance count */
#define USB_DEVICE_CONFIG_PHDC (0U)

/*! @brief Video instance count */
#define USB_DEVICE_CONFIG_VIDEO (0U)

/*! @brief CCID instance count */
#define USB_DEVICE_CONFIG_CCID (0U)

/*! @brief Printer instance count */
#define USB_DEVICE_CONFIG_PRINTER (0U)

/*! @brief DFU instance count */
#define USB_DEVICE_CONFIG_DFU (0U)

/* @} */

/*! @brief Whether device is self power. 1U supported, 0U not supported */
#define USB_DEVICE_CONFIG_SELF_POWER (1U)

/*! @brief Whether device remote wakeup supported. 1U supported, 0U not supported */
#define USB_DEVICE_CONFIG_REMOTE_WAKEUP (0U)

/*! @brief How many endpoints are supported in the stack. */
#define USB_DEVICE_CONFIG_ENDPOINTS (6U)

/*! @brief Whether the device task is enabled. */
#define USB_DEVICE_CONFIG_USE_TASK (0U)

/*! @brief How many the notification message are supported when the device task is enabled. */
#define USB_DEVICE_CONFIG_MAX_MESSAGES (8U)

#if ((defined(USB_DEVICE_CONFIG_KHCI)) && (USB_DEVICE_CONFIG_KHCI > 0U))

/*! @brief The MAX buffer length for the KHCI DMA workaround.*/
#define USB_DEVICE_CONFIG_KHCI_DMA_ALIGN_BUFFER_LENGTH (64U)
/*! @brief Whether handle the USB KHCI bus error. */
#define USB_DEVICE_CONFIG_KHCI_ERROR_HANDLING (0U)
#endif

#if ((defined(USB_DEVICE_CONFIG_EHCI)) && (USB_DEVICE_CONFIG_EHCI > 0U))
/*! @brief How many the DTD are supported. */
#define USB_DEVICE_CONFIG_EHCI_MAX_DTD (16U)
/*! @brief Whether handle the USB EHCI bus error. */
#define USB_DEVICE_CONFIG_EHCI_ERROR_HANDLING (0U)
/*! @brief Whether test mode enabled. */
#define USB_DEVICE_CONFIG_EHCI_TEST_MODE (0U)
/*! @brief Whether the EHCI ID pin detect feature enabled. */
#define USB_DEVICE_CONFIG_EHCI_ID_PIN_DETECT (0U)
#endif

/*! @brief Whether the keep alive feature enabled. */
#define USB_DEVICE_CONFIG_KEEP_ALIVE_MODE (0U)

/*! @brief Whether the transfer buffer is cache-enabled or not. */
#define USB_DEVICE_CONFIG_BUFFER_PROPERTY_CACHEABLE (0U)

/*! @brief Whether the low power mode is enabled or not. */
#define USB_DEVICE_CONFIG_LOW_POWER_MODE (0U)

/*! @brief Whether the device detached feature is enabled or not. */
#define USB_DEVICE_CONFIG_DETACH_ENABLE (0U)

/* @} */

#endif /* _USB_DEVICE_CONFIG_H_ */
