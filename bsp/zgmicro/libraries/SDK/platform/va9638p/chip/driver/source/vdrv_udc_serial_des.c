/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2009, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    OTG device

Description:
    OTG device Driver USB2SERIAL descritors

Revision History:
    Date                  Author                   Description
    2017.10.30          chanjianglin              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"

#include "vdrv_dma.h"
#include "vudc.h"
#include "vdrv_otg.h"
#include "vudc_serial.h"


/* USB Standard Device Descriptor */
/* USB Standard Device Descriptor */
const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_dev[] = {
    USB_DEVICE_DESC_SIZE,          /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
#if (USB_SERIAL_HS_EN)
    WBVAL ( 0x0200 ), /* 1.00 */          /* bcdUSB */
#else
    WBVAL ( 0x0200 ), /* 1.00 */          /* bcdUSB */
#endif
    0x02,      /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
    VUDC_EP0_MAX_TX_SIZE,                      /* bMaxPacketSize0 */
    WBVAL ( 0x0ac8 ),                      /* idVendor */
    WBVAL ( 0x38b9 ),                    /* idProduct */
    WBVAL ( 0x0200 ),                    /* bcdDevice */
    0x01,                                 /* iManufacturer */
    0x02,                                 /* iProduct */
    0x00, //0x03,                         /* iSerialNumber */
    0x01                                  /* bNumConfigurations */
};
const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_dev_fix_com[] = {
    USB_DEVICE_DESC_SIZE,          /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
#if (USB_SERIAL_HS_EN)
    WBVAL ( 0x0200 ), /* 1.00 */          /* bcdUSB */
#else
    WBVAL ( 0x0200 ), /* 1.00 */          /* bcdUSB */
#endif
    0x02,      /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
    VUDC_EP0_MAX_TX_SIZE,                      /* bMaxPacketSize0 */
    WBVAL ( 0x0ac8 ),                      /* idVendor */
    WBVAL ( 0x38b9 ),                    /* idProduct */
    WBVAL ( 0x0200 ),                    /* bcdDevice */
    0x01,                                 /* iManufacturer */
    0x02,                                 /* iProduct */
    0x00,                                 /* iSerialNumber */
    0x01                                  /* bNumConfigurations */
};


const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_cfg[] = {
    /* configuration */
    USB_CONFIGUARTION_DESC_SIZE,            /* bLength              */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* CONFIGURATION        */
    WBVAL (
                    USB_CONFIGUARTION_DESC_SIZE +
                    USB_CDC_INTERFACE_DESC_SIZE +
                    USB_INTERFACE_DESC_SIZE  +
                    USB_CDC_HEADER_FUNC_DESC_SIZE +
                    USB_CDC_CALL_MANAGE_DESC_SIZE +
                    USB_CDC_ACM_FUNC_DESC_SIZE +
                    USB_CDC_UNION_FUNC_DESC_SIZE +
                    USB_ENDPOINT_DESC_SIZE +

                    USB_INTERFACE_DESC_SIZE  +
                    USB_ENDPOINT_DESC_SIZE +
                    USB_ENDPOINT_DESC_SIZE
    ),                                          /* length               */
    SERIAL_NUM_OF_INTF,                        /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    USB_CONFIG_BUS_POWERED,                             /* bmAttributes (required) */
    USB_CONFIG_POWER_MA ( 100 ),       /* power                */

    /*ASSOCIATION INTERFACE*/
    USB_CDC_INTERFACE_DESC_SIZE,
    0X0B,
    0, //fist interface
    2, //interfacecount
    USB_DEVICE_CLASS_COMMUNICATIONS,        /* bInterfaceClass      */
    0x02,                                   /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x01, //0x01, /*Common AT commands*/ //0x00,                                   /* bInterfaceProtocol (BOT) */
    0x02,


    /* control interface */
    USB_INTERFACE_DESC_SIZE,                /* bLength              */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* INTERFACE            */
    0x0,                                    /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    USB_DEVICE_CLASS_COMMUNICATIONS,        /* bInterfaceClass      */
    0x02,                                   /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x01, /*Common AT commands*/ //0x00,                                   /* bInterfaceProtocol (BOT) */
    0x00,                                   /* iInterface           */

    /*header functional desc*/
    USB_CDC_HEADER_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X00,
    WBVAL ( 0x0110 ), //WBVAL(0x0100),


    /*call management*/
    USB_CDC_CALL_MANAGE_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X01,
    0x03,    /* bmCapabilities: D0+D1 */ //0X01,
    0X01,  //DATA INTERFACE

    /*ACM FUNCTION*/
    USB_CDC_ACM_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X02,
    0X02,

    /*UNION FUNCTION*/
    USB_CDC_UNION_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X06,
    0X00, //command interface
    0X01, //data interface

    /* Endpoint Descriptor  : INT-In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* ENDPOINT             */
    USB_ENDPOINT_IN ( CDC_INT_IN_EP ),      /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes         */
    WBVAL ( 10 ),                            /* wMaxPacketSize       */
    1, //0x10, //0x0A,  /* bInterval            */

    /* interface */
    USB_INTERFACE_DESC_SIZE,                /* bLength              */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* INTERFACE            */
    0x01,                                   /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x02,                                   /* bNumEndpoints        */
    USB_DEVICE_CLASS_DATA,                  /* bInterfaceClass      */
    0x0,                                    /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x0,                                    /* bInterfaceProtocol (BOT) */
    0x0,                                    /* iInterface           */


    /* Endpoint Descriptor  : Bulk-In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* ENDPOINT             */
    USB_ENDPOINT_IN ( CDC_BULK_IN_EP ),     /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_BULK,                 /* bmAttributes         */
#if (USB_SERIAL_HS_EN)
    WBVAL ( 512 ),                          /* wMaxPacketSize       */
#else
    WBVAL ( 64 ),
#endif
    0x00,                                   /* bInterval            */

    /* Endpoint Descriptor  : Bulk-Out */
    USB_ENDPOINT_DESC_SIZE,                /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,          /* ENDPOINT             */
    USB_ENDPOINT_OUT ( CDC_BULK_OUT_EP ),  /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_BULK,                 /* bmAttributes         */
#if (USB_SERIAL_HS_EN)
    WBVAL ( 512 ),                        /* wMaxPacketSize       */
#else
    WBVAL ( 64 ),
#endif
    0x00                                    /* bInterval            */

};


const UINT8 __attribute__((aligned(4))) VUDC_Serial_Desc_HighSpeedData [] = {
    /* device qualifier */
    USB_DEVQUAL_DESC_SIZE,                 /* bLength              */
    USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,  /* DEVICE Qualifier     */
    WBVAL ( 0x0200 ),              /* USB 2.0              */
    0,                         /* CLASS                */
    0,                         /* Subclass             */
    0x00,                      /* Protocol             */
    VUDC_EP0_MAX_TX_SIZE,      /* bMaxPacketSize0      */
    0x01,                      /* number of other speed config configuration    */
    0x00,                      /* bReserved            */
};

const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_low_speed_cfg[] = {

    /* configuration */
    USB_CONFIGUARTION_DESC_SIZE,            /* bLength              */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* CONFIGURATION        */
    WBVAL (
                    USB_CONFIGUARTION_DESC_SIZE +
                    USB_INTERFACE_DESC_SIZE  +
                    USB_CDC_HEADER_FUNC_DESC_SIZE +
                    USB_CDC_CALL_MANAGE_DESC_SIZE +
                    USB_CDC_ACM_FUNC_DESC_SIZE +
                    USB_CDC_UNION_FUNC_DESC_SIZE +
                    USB_ENDPOINT_DESC_SIZE +

                    USB_INTERFACE_DESC_SIZE  +
                    USB_ENDPOINT_DESC_SIZE +
                    USB_ENDPOINT_DESC_SIZE
    ),                                          /* length               */
    SERIAL_NUM_OF_INTF,                        /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    0xc0,                             /* bmAttributes (required) */
    USB_CONFIG_POWER_MA ( 100 ),       /* power                */

    /* control interface */
    USB_INTERFACE_DESC_SIZE,                /* bLength              */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* INTERFACE            */
    0x0,                                    /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x01,                                   /* bNumEndpoints        */
    USB_DEVICE_CLASS_COMMUNICATIONS,        /* bInterfaceClass      */
    0x02,                                   /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x01, /*Common AT commands*/ //0x00,                                   /* bInterfaceProtocol (BOT) */
    0x00,                                   /* iInterface           */

    /*header functional desc*/
    USB_CDC_HEADER_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X00,
    WBVAL ( 0x0101 ), //WBVAL(0x0100),


    /*call management*/
    USB_CDC_CALL_MANAGE_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X01,
    0x00,    /* bmCapabilities: D0+D1 */ //0X01,
    0X01,

    /*ACM FUNCTION*/
    USB_CDC_ACM_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X02,
    0X02,

    /*UNION FUNCTION*/
    USB_CDC_UNION_FUNC_DESC_SIZE,
    CDC_CS_INTERFACE,
    0X06,
    0X00,
    0X01,

    /* Endpoint Descriptor  : INT-In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* ENDPOINT             */
    USB_ENDPOINT_IN ( CDC_INT_IN_EP ),      /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes         */
    WBVAL ( 8 ),                            /* wMaxPacketSize       */
    0xff, //0x0A,   /* bInterval            */


    /* interface */
    USB_INTERFACE_DESC_SIZE,                /* bLength              */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* INTERFACE            */
    0x01,                                   /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x02,                                   /* bNumEndpoints        */
    USB_DEVICE_CLASS_DATA,                  /* bInterfaceClass      */
    0x0,                                    /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x0,                                    /* bInterfaceProtocol (BOT) */
    0x0,                                    /* iInterface           */

    /* Endpoint Descriptor  : Bulk-In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* ENDPOINT             */
    USB_ENDPOINT_IN ( CDC_BULK_IN_EP ),     /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_BULK,                 /* bmAttributes         */
    WBVAL ( 64 ),                          /* wMaxPacketSize       */
    0x00,                                   /* bInterval            */

    /* Endpoint Descriptor  : Bulk-Out */
    USB_ENDPOINT_DESC_SIZE,                /* bLength              */
    USB_ENDPOINT_DESCRIPTOR_TYPE,          /* ENDPOINT             */
    USB_ENDPOINT_OUT ( CDC_BULK_OUT_EP ),  /* bEndpointAddress     */
    USB_ENDPOINT_TYPE_BULK,                 /* bmAttributes         */
    WBVAL ( 64 ),                        /* wMaxPacketSize       */
    0x00                                    /* bInterval            */
};


const UINT8 __attribute__((aligned(4))) VUDC_Serial_desc_StringLangID[] = {
    0x04,
    0x03,
    0x09,
    0x04
}
; /* LangID = 0x0409: U.S. English */

const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_StringVendor[] = {
    /* TODO: make tool to generate strings and eventually whole descriptor! */
    /* English (U.S.) strings */
    2 + 14,         /* Manufacturer: Vimicro */
    USB_STRING_DESCRIPTOR_TYPE,
    'Z', 0x0, 'G', 0x0, 'M', 0x0, 'i', 0x0, 'c', 0x0, 'r', 0x0, 'o', 0x0,

};

const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_StringProduct[] = {
    2 + 14,         /* Product ID: Storage */
    USB_STRING_DESCRIPTOR_TYPE,
    'U', 0x0, 'S', 0x0, 'B', 0x0, '2', 0x0, 'S', 0x0, 'e', 0x0, 'r', 0x0,
};

const UINT8  __attribute__((aligned(4))) VUDC_Serial_desc_StringSerial[] = {
    2 + 26,         /* Serial #: 123412341234 */
    USB_STRING_DESCRIPTOR_TYPE,
    '9', 0, '6', 0, '8', 0, '8', 0, 'B', 0, 'S', 0, 'E', 0, 'R', 0, '1', 0, '0', 0, '0', 0, '0', 0, '0', 0
};




