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

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_hid.h"

#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "composite.h"

#include "hid_generic.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_device_hid_generic_struct_t s_UsbDeviceHidGeneric;

/*******************************************************************************
 * Code
 ******************************************************************************/

usb_status_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            break;
        case kUSB_DeviceHidEventRecvResponse:
            if (s_UsbDeviceComposite->attach)
            {
                USB_DeviceHidSend(s_UsbDeviceComposite->hidGenericHandle, USB_HID_GENERIC_ENDPOINT_IN,
                                  (uint8_t *)s_UsbDeviceHidGeneric.buffer[s_UsbDeviceHidGeneric.bufferIndex],
                                  USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                s_UsbDeviceHidGeneric.bufferIndex ^= 1U;
                return USB_DeviceHidRecv(s_UsbDeviceComposite->hidGenericHandle, USB_HID_GENERIC_ENDPOINT_OUT,
                                         (uint8_t *)s_UsbDeviceHidGeneric.buffer[s_UsbDeviceHidGeneric.bufferIndex],
                                         USB_HID_GENERIC_OUT_BUFFER_LENGTH);
            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
    }

    return error;
}

usb_status_t USB_DeviceHidGenericSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        return USB_DeviceHidRecv(handle, USB_HID_GENERIC_ENDPOINT_OUT,
                                 (uint8_t *)s_UsbDeviceHidGeneric.buffer[s_UsbDeviceHidGeneric.bufferIndex],
                                 USB_HID_GENERIC_OUT_BUFFER_LENGTH);
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidGenericSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_GENERIC_INTERFACE_INDEX == interface)
    {
        return USB_DeviceHidRecv(handle, USB_HID_GENERIC_ENDPOINT_OUT,
                                 (uint8_t *)s_UsbDeviceHidGeneric.buffer[s_UsbDeviceHidGeneric.bufferIndex],
                                 USB_HID_GENERIC_OUT_BUFFER_LENGTH);
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidGenericInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbDeviceHidGeneric.bufferIndex = 0U;
    s_UsbDeviceHidGeneric.idleRate = 125U;
    return kStatus_USB_Success;
}
