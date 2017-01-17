#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"

#include "usb_device_descriptor.h"

/*******************************************************************************
* Variables
******************************************************************************/
/* Define endpoint for communication class */
usb_device_endpoint_struct_t g_cdcVnicCicEp[USB_CDC_VNIC_ENDPOINT_CIC_COUNT] = {
    {
        USB_CDC_VNIC_INTERRUPT_IN_ENDPOINT | (USB_IN << 7U), USB_ENDPOINT_INTERRUPT,
        FS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE,
    },
};

/* Define endpoint for data class */
usb_device_endpoint_struct_t g_cdcVnicDicEp[USB_CDC_VNIC_ENDPOINT_DIC_COUNT] = {
    {
        USB_CDC_VNIC_BULK_IN_ENDPOINT | (USB_IN << 7U), USB_ENDPOINT_BULK, FS_CDC_VNIC_BULK_IN_PACKET_SIZE,
    },
    {
        USB_CDC_VNIC_BULK_OUT_ENDPOINT | (USB_OUT << 7U), USB_ENDPOINT_BULK, FS_CDC_VNIC_BULK_OUT_PACKET_SIZE,
    }};

/* Define interface for communication class */
usb_device_interface_struct_t g_cdcVnicCommunicationInterface[] = {{
    0,
    {
        USB_CDC_VNIC_ENDPOINT_CIC_COUNT, g_cdcVnicCicEp,
    },
}};

/* Define interface for data class */
usb_device_interface_struct_t g_cdcVnicDataInterface[] = {{
    0,
    {
        USB_CDC_VNIC_ENDPOINT_DIC_COUNT, g_cdcVnicDicEp,
    },
}};

/* Define interfaces for virtual nic */
usb_device_interfaces_struct_t g_cdcVnicIfs[USB_CDC_VNIC_INTERFACE_COUNT] = {
    {USB_CDC_VNIC_CIC_CLASS, USB_CDC_VNIC_CIC_SUBCLASS, USB_CDC_VNIC_CIC_PROTOCOL, USB_CDC_VNIC_COMM_INTERFACE_INDEX,
     g_cdcVnicCommunicationInterface, sizeof(g_cdcVnicCommunicationInterface) / sizeof(usb_device_interfaces_struct_t)},
    {USB_CDC_VNIC_DIC_CLASS, USB_CDC_VNIC_DIC_SUBCLASS, USB_CDC_VNIC_DIC_PROTOCOL, USB_CDC_VNIC_DATA_INTERFACE_INDEX,
     g_cdcVnicDataInterface, sizeof(g_cdcVnicDataInterface) / sizeof(usb_device_interfaces_struct_t)},
};

/* Define configurations for virtual nic */
usb_device_interface_list_t g_cdcVnicInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_CDC_VNIC_INTERFACE_COUNT, g_cdcVnicIfs,
    },
};

/* Define class information for virtual nic */
usb_device_class_struct_t g_cdcVnicClass = {
    g_cdcVnicInterfaceList, kUSB_DeviceClassTypeCdc, USB_DEVICE_CONFIGURATION_COUNT,
};

/* Define device descriptor */
uint8_t g_UsbDeviceDescriptor[USB_DESCRIPTOR_LENGTH_DEVICE] = {
    /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_LENGTH_DEVICE,
    /* DEVICE Descriptor Type */
    USB_DESCRIPTOR_TYPE_DEVICE,
    /* USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION), USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION),
    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_CLASS,
    /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,
    /* Protocol code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,
    /* Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid) */
    USB_CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID (assigned by the USB-IF) */
    0xC9U, 0x1FU,
    /* Product ID (assigned by the manufacturer) */
    0x01, 0x03,
    /* Device release number in binary-coded decimal */
    USB_SHORT_GET_LOW(USB_DEVICE_DEMO_BCD_VERSION), USB_SHORT_GET_HIGH(USB_DEVICE_DEMO_BCD_VERSION),
    /* Index of string descriptor describing manufacturer */
    0x01,
    /* Index of string descriptor describing product */
    0x02,
    /* Index of string descriptor describing the device's serial number */
    0x00,
    /* Number of possible configurations */
    USB_DEVICE_CONFIGURATION_COUNT,
};

/* Define configuration descriptor */
uint8_t g_UsbDeviceConfigurationDescriptor[USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL] = {
    /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_LENGTH_CONFIGURE,
    /* CONFIGURATION Descriptor Type */
    USB_DESCRIPTOR_TYPE_CONFIGURE,
    /* Total length of data returned for this configuration. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL),
    /* Number of interfaces supported by this configuration */
    USB_CDC_VNIC_INTERFACE_COUNT,
    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
    USB_CDC_VNIC_CONFIGURE_INDEX,
    /* Index of string descriptor describing this configuration */
    0,
    /* Configuration characteristics D7: Reserved (set to one) D6: Self-powered D5: Remote Wakeup D4...0: Reserved
       (reset to zero) */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK) |
        (USB_DEVICE_CONFIG_SELF_POWER << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT) |
        (USB_DEVICE_CONFIG_REMOTE_WAKEUP << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT),
    /* Maximum power consumption of the USB * device from the bus in this specific * configuration when the device is
       fully * operational. Expressed in 2 mA units *  (i.e., 50 = 100 mA).  */
    USB_DEVICE_MAX_POWER,

    /* Communication Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE, USB_DESCRIPTOR_TYPE_INTERFACE, USB_CDC_VNIC_COMM_INTERFACE_INDEX, 0x00,
    USB_CDC_VNIC_ENDPOINT_CIC_COUNT, USB_CDC_VNIC_CIC_CLASS, USB_CDC_VNIC_CIC_SUBCLASS, 0xFF,
    0x00, /* Interface Description String Index*/

    /* CDC Class-Specific descriptor */
    USB_DESCRIPTOR_LENGTH_CDC_HEADER_FUNC, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE,  /* CS_INTERFACE Descriptor Type */
    USB_CDC_HEADER_FUNC_DESC, 0x10,
    0x01, /* USB Class Definitions for Communications the Communication specification version 1.10 */

    USB_DESCRIPTOR_LENGTH_CDC_CALL_MANAG, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE, /* CS_INTERFACE Descriptor Type */
    USB_CDC_CALL_MANAGEMENT_FUNC_DESC,
    0x01, /*Bit 0: Whether device handle call management itself 1, Bit 1: Whether device can send/receive call
             management information over a Data Class Interface 0 */
    0x01, /* Indicates multiplexed commands are handled via data interface */

    USB_DESCRIPTOR_LENGTH_CDC_ABSTRACT,       /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE,     /* CS_INTERFACE Descriptor Type */
    USB_CDC_ABSTRACT_CONTROL_FUNC_DESC, 0x00, /* No ACM capability */

    USB_DESCRIPTOR_LENGTH_CDC_UNION_FUNC, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE, /* CS_INTERFACE Descriptor Type */
    USB_CDC_UNION_FUNC_DESC, 0x00,        /* The interface number of the Communications or Data Class interface  */
    0x01,                                 /* Interface number of subordinate interface in the Union  */

    /*Notification Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT, USB_CDC_VNIC_INTERRUPT_IN_ENDPOINT | (USB_IN << 7U),
    USB_ENDPOINT_INTERRUPT, USB_SHORT_GET_LOW(FS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE), FS_CDC_VNIC_INTERRUPT_IN_INTERVAL,

    /* Data Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE, USB_DESCRIPTOR_TYPE_INTERFACE, USB_CDC_VNIC_DATA_INTERFACE_INDEX, 0x00,
    USB_CDC_VNIC_ENDPOINT_DIC_COUNT, USB_CDC_VNIC_DIC_CLASS, USB_CDC_VNIC_DIC_SUBCLASS, USB_CDC_VNIC_DIC_PROTOCOL,
    0x00, /* Interface Description String Index*/

    /*Bulk IN Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT, USB_CDC_VNIC_BULK_IN_ENDPOINT | (USB_IN << 7U),
    USB_ENDPOINT_BULK, USB_SHORT_GET_LOW(FS_CDC_VNIC_BULK_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_CDC_VNIC_BULK_IN_PACKET_SIZE), 0x00, /* The polling interval value is every 0 Frames */

    /*Bulk OUT Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT, USB_CDC_VNIC_BULK_OUT_ENDPOINT | (USB_OUT << 7U),
    USB_ENDPOINT_BULK, USB_SHORT_GET_LOW(FS_CDC_VNIC_BULK_OUT_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_CDC_VNIC_BULK_OUT_PACKET_SIZE), 0x00, /* The polling interval value is every 0 Frames */
};

/* Define string descriptor */
uint8_t g_UsbDeviceString0[USB_DESCRIPTOR_LENGTH_STRING0] = {sizeof(g_UsbDeviceString0), USB_DESCRIPTOR_TYPE_STRING,
                                                             0x09, 0x04};

uint8_t g_UsbDeviceString1[USB_DESCRIPTOR_LENGTH_STRING1] = {
    sizeof(g_UsbDeviceString1),
    USB_DESCRIPTOR_TYPE_STRING,
    'N',
    0x00U,
    'X',
    0x00U,
    'P',
    0x00U,
    ' ',
    0x00U,
    'S',
    0x00U,
    'E',
    0x00U,
    'M',
    0x00U,
    'I',
    0x00U,
    'C',
    0x00U,
    'O',
    0x00U,
    'N',
    0x00U,
    'D',
    0x00U,
    'U',
    0x00U,
    'C',
    0x00U,
    'T',
    0x00U,
    'O',
    0x00U,
    'R',
    0x00U,
    'S',
    0x00U,
};

uint8_t g_UsbDeviceString2[USB_DESCRIPTOR_LENGTH_STRING2] = {sizeof(g_UsbDeviceString2),
                                                             USB_DESCRIPTOR_TYPE_STRING,
                                                             'M',
                                                             0,
                                                             'C',
                                                             0,
                                                             'U',
                                                             0,
                                                             ' ',
                                                             0,
                                                             'V',
                                                             0,
                                                             'I',
                                                             0,
                                                             'R',
                                                             0,
                                                             'T',
                                                             0,
                                                             'U',
                                                             0,
                                                             'A',
                                                             0,
                                                             'L',
                                                             0,
                                                             ' ',
                                                             0,
                                                             'N',
                                                             0,
                                                             'I',
                                                             0,
                                                             'C',
                                                             0,
                                                             ' ',
                                                             0,
                                                             'D',
                                                             0,
                                                             'E',
                                                             0,
                                                             'M',
                                                             0,
                                                             'O',
                                                             0};

uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT] = {g_UsbDeviceString0, g_UsbDeviceString1,
                                                                      g_UsbDeviceString2};

/* Define string descriptor size */
uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0), sizeof(g_UsbDeviceString1), sizeof(g_UsbDeviceString2)};
usb_language_t g_UsbDeviceLanguage[USB_DEVICE_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray, g_UsbDeviceStringDescriptorLength, (uint16_t)0x0409,
}};

usb_language_list_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0, sizeof(g_UsbDeviceString0), g_UsbDeviceLanguage, USB_DEVICE_LANGUAGE_COUNT,
};

/*******************************************************************************
* Code
******************************************************************************/
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor)
{
    deviceDescriptor->buffer = g_UsbDeviceDescriptor;
    deviceDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE;
    return kStatus_USB_Success;
}

usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor)
{
    if (USB_CDC_VNIC_CONFIGURE_INDEX > configurationDescriptor->configuration)
    {
        configurationDescriptor->buffer = g_UsbDeviceConfigurationDescriptor;
        configurationDescriptor->length = USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        return kStatus_USB_Success;
    }
    return kStatus_USB_InvalidRequest;
}

usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed)
{
    usb_descriptor_union_t *ptr1;
    usb_descriptor_union_t *ptr2;

    ptr1 = (usb_descriptor_union_t *)g_UsbDeviceConfigurationDescriptor;
    ptr2 = (usb_descriptor_union_t *)(&g_UsbDeviceConfigurationDescriptor[USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1]);

    while (ptr1 < ptr2)
    {
        if (ptr1->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            if (USB_SPEED_HIGH == speed)
            {
                if (USB_CDC_VNIC_INTERRUPT_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    ptr1->endpoint.bInterval = HS_CDC_VNIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE,
                                                       ptr1->endpoint.wMaxPacketSize);
                }
                else if (USB_CDC_VNIC_BULK_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VNIC_BULK_IN_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else if (USB_CDC_VNIC_BULK_OUT_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VNIC_BULK_OUT_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
            }
            else
            {
                if (USB_CDC_VNIC_INTERRUPT_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    ptr1->endpoint.bInterval = FS_CDC_VNIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE,
                                                       ptr1->endpoint.wMaxPacketSize);
                }
                else if (USB_CDC_VNIC_BULK_IN_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VNIC_BULK_IN_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
                else if (USB_CDC_VNIC_BULK_OUT_ENDPOINT == (ptr1->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VNIC_BULK_OUT_PACKET_SIZE, ptr1->endpoint.wMaxPacketSize);
                }
            }
        }
        ptr1 = (usb_descriptor_union_t *)((uint8_t *)ptr1 + ptr1->common.bLength);
    }

    for (int i = 0; i < USB_CDC_VNIC_ENDPOINT_CIC_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            g_cdcVnicCicEp[i].maxPacketSize = HS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE;
        }
        else
        {
            g_cdcVnicCicEp[i].maxPacketSize = FS_CDC_VNIC_INTERRUPT_IN_PACKET_SIZE;
        }
    }
    for (int i = 0; i < USB_CDC_VNIC_ENDPOINT_DIC_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            g_cdcVnicDicEp[i].maxPacketSize = HS_CDC_VNIC_BULK_OUT_PACKET_SIZE;
        }
        else
        {
            g_cdcVnicDicEp[i].maxPacketSize = FS_CDC_VNIC_BULK_OUT_PACKET_SIZE;
        }
    }

    return kStatus_USB_Success;
}
