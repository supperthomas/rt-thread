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

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VUDC_DEVICE_SERIAL_H__
#define __VUDC_DEVICE_SERIAL_H__

#include "vdrv_otg.h"

#define USB_SERIAL_HS_EN   0
#if (USB_SERIAL_HS_EN)
    #define MAX_BULK_PACKET_SIZE  508
#else
    #define MAX_BULK_PACKET_SIZE  64    //60
#endif

#define VUSB_PRINT_BUFFER_MAX_SIZE   512 //1024
#define SERIAL_DATA_HEADER_SIZE      4
#define SERIAL_DATA_HEADER_FLAG  0xF00F
#define SER_CRC_ERR 0x1F
#define SERIAL_PACKET_COUNT   8
#define VOTG_EP1_TXBUF_SIZE_WITH_CRC  (VOTG_EP1_TXBUF_SIZE - 6)

#define SERIAL_STATUS_OK   0
#define SERIAL_PLUGOUT_ERROR   1
#define SERIAL_NOT_READY    2

#define CDC_INT_IN_EP    2
#define CDC_BULK_IN_EP   1
#define CDC_BULK_OUT_EP  2//1//2

#define SERIAL_NUM_OF_INTF   2

#define CDC_CS_INTERFACE   0X24
#define CDC_CS_ENDPOINT    0X25

#define  USB_CDC_INTERFACE_DESC_SIZE     8
#define  USB_CDC_HEADER_FUNC_DESC_SIZE   5
#define  USB_CDC_CALL_MANAGE_DESC_SIZE   5
#define  USB_CDC_ACM_FUNC_DESC_SIZE      4
#define  USB_CDC_UNION_FUNC_DESC_SIZE    5
#define  USB_CDC_NOTIFY_EP_DESC_SIZE     7

/* CDC Request Codes */
#define CDC_SET_COMM_FEATURE           0X02
#define CDC_GET_COMM_FEATURE           0X03
#define CDC_SET_LINE_CODING            0X20
#define CDC_GET_LINE_CODING            0X21
#define CDC_SET_CONTROL_LINE_STATE     0X22


#define DEFAULT_BITRATE       115200 //921600  //115200   
#define DEFAULT_FORMAT        0
#define DEFAULT_PARITYTYPE    0
#define DEFAULT_DATATYPE      8

#define RX_PACKET_HEADER_SIZE  4

#define SELECT_OS_SEM    0
#define GPIO_DEBUG       0

typedef struct _LINECODING {
    UINT32 u32Bitrate;
    UINT8  u8Format;
    UINT8  u8ParityType;
    UINT8  u8DataType;
} LINECODING;

typedef struct {
    UINT16 rx_size;
    UINT8 p_rx_data[1];
} udc_serial_rx_info;

typedef struct {
    UINT16 magiccode;
    UINT16 len;
    UINT8 payload[58];
    UINT16 crc;
} USB_frame_s;


/* Exported functions ------------------------------------------------------- */
extern const UINT8 VUDC_Serial_desc_dev[];
extern const UINT8 VUDC_Serial_desc_dev_fix_com[];
extern const UINT8 VUDC_Serial_desc_cfg[];
extern const UINT8 VUDC_Serial_Desc_HighSpeedData[];
extern const UINT8 VUDC_Serial_desc_low_speed_cfg[];
extern const UINT8 VUDC_Serial_desc_StringVendor[];
extern const UINT8 VUDC_Serial_desc_StringProduct[];
extern const UINT8 VUDC_Serial_desc_StringSerial[];
extern const UINT8 VUDC_Serial_desc_StringLangID[] ;

VRET_VALUE VUDC_Serial_ClassReqHdlr ( UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg );

void VUDC_Serial_DataIn ( void );
void VUDC_Serial_DataIn_Done ( void );
void VUDC_Serial_DataOut ( void );
void VUDC_Serial_DataOut_Done ( void );
void VUDC_Serial_Init ( void );

extern int g_USBPrintBufferPtr ;
extern int g_USBPrintBufferSize ;
extern UINT8 g_USBSerialReady ;
static void VUDC_TX_EP0 ( UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend );
static void VUDC_EP0_RxServiced_STALL ( void );
static void VUDC_EP0_RxServiced_NoData ( void );
static void VUDC_RX_EP0 ( UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend );
UINT16 VUDC_Serial_Cal_Crc ( const unsigned char *ptr, int count );
UINT8 VUDC_Serial_WriteBytes_With_CRC ( const unsigned char *ptr, UINT16 count );

UINT16 VUDC_Serial_ReadBytes ( UINT8 *pbuf, UINT16 len );
UINT16 VUDC_Serial_Exit ( UINT8 errcode );
void VUDC_Serial_Suspend_Proc ( void );

#endif


