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
#include "vdrv_gpio.h"

LINECODING g_stLineCoding = {DEFAULT_BITRATE, DEFAULT_FORMAT, DEFAULT_PARITYTYPE, DEFAULT_DATATYPE};

//static  OS_SEM  *g_Sem_UsbSerial_DataIn = NULL;
//static  OS_SEM  *g_Sem_UsbSerial_DataOut = NULL;
//extern  OS_SEM  *g_Sem_UsbSerial_Ready ;
extern void VOTGC_CfgDect ( BOOL setval );

volatile UINT8 g_UsbSerial_DataIn_flag = 0;
volatile UINT8 g_UsbSerial_DataOut_flag = 0;

UINT32 g_USBPrintDebug[10];

UINT8 g_fl_rx_data_ready = 0;
UINT8 g_USBSerialReady = 0;
//OS_SEM  *g_Sem_UsbSerial_Ready = NULL;
UINT32 g_usb_rx_len = 0;
UINT32 g_usb_rx_left_len = 0;
UINT8 g_rx_pckt_cnt = 0;
UINT8 g_next_pckt_flag = 0;

UINT16 g_rx_data_available_index = 0;
UINT16 g_rx_data_available_size = 0;
UINT8 g_read_deal_flag = 0;
UINT8 g_usb_serial_err = SERIAL_STATUS_OK;
UINT8 g_usb_serial_detect_flag = 0;

//struct ListStru g_udc_serial_list;
//UINT8 g_rx_payloadbuf[MAX_BULK_PACKET_SIZE];

void VUDC_RxNonblock_Reset ( void );

/******************************************************************************
Description:
    USB CDC Class Request handler
    parse the request and then call Endpoint or FeatureUnit handler to carry out the request
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_Serial_ClassReqHdlr ( UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg )
{
    VRET_VALUE ret = VRET_NO_ERROR;

    switch ( setup_req->bRequest ) {
        case CDC_SET_LINE_CODING:
            //VCOMMON_MemCpy(&g_stLineCoding,dmabuf, 7);
            //printf("set line coding\r\n");
            break;

        case CDC_GET_LINE_CODING:
            VCOMMON_MemCpy ( dmatxbuf, &g_stLineCoding, 7 );
            msg->p_msg = ( UINT32 * ) dmatxbuf;
            msg->len = 7;
            break;

        case CDC_SET_CONTROL_LINE_STATE:
            g_USBSerialReady = 1;
            g_usb_serial_err = SERIAL_STATUS_OK;
            g_UsbSerial_DataIn_flag = 0;
            g_UsbSerial_DataOut_flag = 0;
            VUDC_RxNonblock_Reset();
            break;

        default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF ( "AUD class req Get unknown receip!!!\r\n" );
            break;
    }

    return ret;
}


void VUDC_Serial_Init ( void )
{
    g_rx_pckt_cnt = 0;
    g_rx_data_available_index = 0;
    g_rx_data_available_size = 0;

    g_USBSerialReady = 0;
    g_UsbSerial_DataIn_flag = 0;
    g_UsbSerial_DataOut_flag = 0;
    g_usb_serial_detect_flag = 0;

#if (GPIO_DEBUG == 1)
    VGPIO_SetLMode ( GPIO_L_PIN_13, GPIO_MODE_OUTPUT, GPIO_INPUT_NORMAL );
    VGPIO_SetPinOutput ( 13 + 32, 0 );
#endif
}

void VUDC_Serial_DataIn ( void )
{
    g_USBPrintDebug[0]++;
    g_UsbSerial_DataIn_flag = 1;
}

void VUDC_Serial_DataIn_Done ( void )
{
    g_USBPrintDebug[1]++;
    while ( 1 ) {

        if ( g_usb_serial_detect_flag == 1 ) {
            VUDC_Serial_Suspend_Proc();
            g_UsbSerial_DataIn_flag = 1;
        }

        if ( g_UsbSerial_DataIn_flag ) {
            g_UsbSerial_DataIn_flag = 0;
            break;
        }
    }
}

void VUDC_Serial_DataOut ( void )
{
    g_UsbSerial_DataOut_flag = 1;
}

void VUDC_Serial_DataOut_Done ( void )
{
    while ( 1 ) {

        if ( g_usb_serial_detect_flag == 1 ) {
            VUDC_Serial_Suspend_Proc();
            g_UsbSerial_DataOut_flag = 1;
        }

        if ( g_UsbSerial_DataOut_flag ) {
            g_UsbSerial_DataOut_flag = 0;
            break;
        }
    }
}


UINT16 VUDC_Serial_Exit ( UINT8 errcode )
{
    g_usb_serial_err = errcode;
    g_USBSerialReady = 0;

    return VRET_NO_ERROR;
}


void VUDC_Serial_Suspend_Proc ( void )
{
    static BOOL plugin_last = FALSE;
    if ( plugin_last == FALSE ) {
        plugin_last = TRUE;
    } else if ( g_usb_serial_detect_flag ) {
        //colin mark
        VOTGC_CfgDect ( 0 );
        //VOSTimeGet(2000);
        VCOMMON_Delay ( 40000 );
        VOTGC_CfgDect ( 1 );
        VOTG_NPRINTF ( "USB serial detect discon!!!\r\n" );
        VUDC_Serial_Exit ( SERIAL_PLUGOUT_ERROR );
    }
    g_usb_serial_detect_flag = 0;
}

#if 0
UINT16 VUDC_GetDataFromHost ( UINT8 *buffer, UINT16 size )
{
    UINT16 ret = VRET_NO_ERROR;
    UINT16 totsize;
    UINT16 offset, len;
    UINT8 *p_buf;

    totsize = size;
    offset = 0;

    while (  totsize > 0 ) {

        if ( g_USBSerialReady ) {

            p_buf = buffer + offset;
            len = ( totsize > MAX_BULK_PACKET_SIZE ) ? MAX_BULK_PACKET_SIZE : totsize;
            totsize -= len;
            offset += len;

            VUDC_RX_EPx ( VOTG_EP2, len, ( UINT32 ) dmabuf2, 0 );
            //wait tx done
            VUDC_Serial_DataOut_Done();
            VCOMMON_MemCpy ( p_buf, dmabuf2, len );
        } else {
            ret = VRET_ERROR;
            break;
        }
    }

    return ret;
}
#endif

#if 0
UINT16 VUDC_GetDataFromHost_Rep ( UINT8 *buffer, UINT16 *psize )
{
    UINT16 ret = VRET_NO_ERROR;
    UINT16 totsize = 0;
    UINT16 offset = 0, len;
    UINT8 *p_buf;
    int mask = 0;

    while ( totsize > 0 || mask == 0 ) {
        if ( g_USBSerialReady && mask == 0 ) {
            mask = 1;
            p_buf = buffer;
            offset += 6;

            VCOMMON_MemSet ( dmabuf2, 0, VOTG_EP2_RXBUF_SIZE );
            VUDC_RX_EPx ( VOTG_EP2, 6, ( UINT32 ) dmabuf2, 0 );
            //wait tx done
            VUDC_Serial_DataOut_Done();
            VCOMMON_MemCpy ( p_buf, dmabuf2, 6 );


            totsize = p_buf[2] | ( p_buf[3] << 8 );

            if ( totsize > 1088 ) { //防止数据长度太大，导致程序一直处在接收数据，表现程序假死状态
                totsize = 1088;
            }
            *psize = totsize + 6;
        } else if ( g_USBSerialReady ) {
            p_buf = buffer + offset;
            len = ( totsize > MAX_BULK_PACKET_SIZE ) ? MAX_BULK_PACKET_SIZE : totsize;
            totsize -= len;
            offset += len;

            VCOMMON_MemSet ( dmabuf2, 0, VOTG_EP2_RXBUF_SIZE );
            VUDC_RX_EPx ( VOTG_EP2, len, ( UINT32 ) dmabuf2, 0 );
            //wait tx done
            VUDC_Serial_DataOut_Done();
            VCOMMON_MemCpy ( p_buf, dmabuf2, len );
        } else {
            ret = VRET_ERROR;
            break;
        }
    }

    return ret;
}
#endif

UINT16 VUDC_SetDataToHost ( UINT8 *buffer, UINT16 size )
{
    UINT16 ret = VRET_NO_ERROR;
    UINT16 totsize;
    UINT16 offset, len;
    UINT8 *p_buf;

    totsize = size;
    offset = 0;
    while (  totsize > 0 ) {
        if ( g_USBSerialReady ) {

            p_buf = buffer + offset;
            len = ( totsize > MAX_BULK_PACKET_SIZE ) ? MAX_BULK_PACKET_SIZE : totsize;
            totsize -= len;
            offset += len;

            VCOMMON_MemCpy ( dmatxbuf1, p_buf, len );
            VUDC_TX_EPx ( VOTG_EP1, len, ( UINT32 ) dmatxbuf1, 0 );

            //wait rx done
            VUDC_Serial_DataIn_Done();

        } else {
            ret = VRET_ERROR;
            break;
        }
    }
    return ret;
}

UINT8 VUDC_GetReadyState ( void )
{
    return g_USBSerialReady;
}

void VUDC_SetReadyState ( void )
{
    g_USBSerialReady = 1;
}


/* USB非阻塞式接收数据 */
typedef struct _USB_RX_NOBLOCK_STU {
    UINT8  readStep;        //接收数据状态
} USB_Recv_Noblock_t;


USB_Recv_Noblock_t g_st_UsbRx = {0};
volatile UINT8  g_bUsbRecvFlag = 0; //接收完成标记， 0-未接收到数据，1-接收到一帧数据
int g_usb_recv_len = 0;

volatile UINT32 remainlen = 0;
UINT16 VUDC_GetDataFromHost_nonblock ( UINT8 *buffer, UINT16 *psize, UINT16 buffSize )
{
    // UINT32 remainlen = 0;

    if ( ( g_bUsbRecvFlag == 0 ) && g_USBSerialReady ) {
        if ( g_usb_serial_detect_flag == 1 ) {
            VUDC_Serial_Suspend_Proc();
            VUDC_RxNonblock_Reset();
            return VRET_ERROR;
        }

        switch ( g_st_UsbRx.readStep ) {
            case 0: { //cofig DMA To read the first Frame,in order to get totallen
                /* 1. */
                g_st_UsbRx.readStep = 1;
                g_usb_recv_len = 0;

                VCOMMON_MemSet ( dmabuf2, 0, VOTG_EP2_RXBUF_SIZE );
                VUDC_RX_EPx ( VOTG_EP2, MAX_BULK_PACKET_SIZE, ( UINT32 ) dmabuf2, 0 );
                break;
            }
            case 1: {
                /*2. 读数据 */
                if ( g_UsbSerial_DataOut_flag ) {
                    g_UsbSerial_DataOut_flag = 0;

                    // remainlen = VDMA_USB_GetReadCount ( VDMA_USB_SRAM2 );
                    if ( ( g_usb_recv_len + remainlen ) > buffSize ) {
                        remainlen = buffSize - g_usb_recv_len;
                    }
                    VCOMMON_MemCpy ( buffer + g_usb_recv_len, dmabuf2, remainlen );

                    g_usb_recv_len += remainlen;
                    if ( remainlen > 0 ) {
                        g_bUsbRecvFlag = 1;
                    }

                    VCOMMON_MemSet ( dmabuf2, 0, VOTG_EP2_RXBUF_SIZE );
                    VUDC_RX_EPx ( VOTG_EP2, MAX_BULK_PACKET_SIZE, ( UINT32 ) dmabuf2, 0 );
                }
                break;
            }
            default: {
                g_st_UsbRx.readStep = 0;
                g_bUsbRecvFlag = 0;
                return VRET_ERROR;
            }
        }//end of switch()
    } else if ( !g_USBSerialReady ) {
        g_st_UsbRx.readStep = 0;
        g_bUsbRecvFlag = 0;
        return VRET_ERROR;
    }

    return VRET_NO_ERROR;
}


//USB断开连接后，需要调用此函数。
void VUDC_RxNonblock_Reset ( void )
{
    g_st_UsbRx.readStep = 0;
    g_bUsbRecvFlag = 0;
    g_usb_recv_len = 0;
}

