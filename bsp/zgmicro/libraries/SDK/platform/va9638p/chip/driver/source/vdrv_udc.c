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
    OTG device Driver

Revision History:
    Date                  Author                   Description
    2014.05.04            yinxingjie                   initial version
    2014.05.28          zhangqichen         add device ep0 states
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vdrv_dma.h"
#include "vudc.h"
#include "vudc_serial.h"
#include "vreg_def.h"
extern VOTG_MSG g_OtgMsgPkt;
extern UINT8    g_OtgMsgInd;


volatile UINT8 USBD_ST;
volatile UINT8 USBD_CurrIntf;
volatile UINT8 USBD_CurrIntfSetting;


volatile static UINT8 FAddr;//usb device address
volatile static VUDC_EP0 dev_ep0 = {0, 0, 0, 0, NULL, VUDC_EP0_ST_IDLE};
volatile static UINT8 TEST_SEL = 0; //usb test mode selector

extern volatile UINT32 is_otg_tx_data_end;

extern const UINT8 VUDC_MSD_desc_dev[];
extern const UINT8 VUDC_MSD_desc_StringLangID[];
extern const UINT8 VUDC_MSD_desc_StringVendor[];
extern const UINT8 VUDC_MSD_desc_StringProduct[];
extern const UINT8 VUDC_MSD_desc_StringSerial[];
extern const UINT8 VUDC_MSD_desc_cfg[];
extern const UINT8 VUDC_MSD_Desc_HighSpeedData[];
extern const UINT8 VUDC_MSD_desc_low_speed_cfg[];

VUDC_Descriptor VUDC_aDescriptorData_dev;
VUDC_Descriptor VUDC_aDescriptorData_string_langID;
VUDC_Descriptor VUDC_aDescriptorData_string_vendor;
VUDC_Descriptor VUDC_aDescriptorData_string_product;
VUDC_Descriptor VUDC_aDescriptorData_string_serial;
VUDC_Descriptor VUDC_aDescriptorData_cfg;
VUDC_Descriptor VUDC_aDescriptorData_qual;
VUDC_Descriptor VUDC_aDescriptorData_oth_speed_cfg;
VUDC_Descriptor VUDC_aDescriptorData_rpt;

extern const USB_DEVICE_HDL Usb_device_hdl;

extern void VOTGC_CfgCG ( BOOL setval );
extern void VOTGC_PhyRst ( BOOL setval );
extern void VOTGC_CfgDect ( BOOL setval );
extern void VOTGC_CfgConnect ( BOOL connect );


extern void VOTGC_CfgCG ( BOOL setval );
extern void VOTGC_PhyRst ( BOOL setval );
extern void VOTGC_CfgDect ( BOOL setval );
extern void VOTGC_CfgConnect ( BOOL connect );

extern UINT8 g_usb_serial_detect_flag;

/******************************************************************************
Description:
    descriptor initial for USB device.
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_CfgDesc ( void )
{
    if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) {
        VUDC_aDescriptorData_dev.pDesc = VUDC_Serial_desc_dev;
        VUDC_aDescriptorData_string_langID.pDesc = VUDC_Serial_desc_StringLangID;
        VUDC_aDescriptorData_string_vendor.pDesc = VUDC_Serial_desc_StringVendor;
        VUDC_aDescriptorData_string_product.pDesc = VUDC_Serial_desc_StringProduct;
        VUDC_aDescriptorData_string_serial.pDesc = VUDC_Serial_desc_StringSerial;
        VUDC_aDescriptorData_cfg.pDesc = VUDC_Serial_desc_cfg;
        VUDC_aDescriptorData_qual.pDesc = VUDC_Serial_Desc_HighSpeedData;
        VUDC_aDescriptorData_oth_speed_cfg.pDesc = VUDC_Serial_desc_low_speed_cfg;

        //length
        VUDC_aDescriptorData_dev.bLength = VUDC_Serial_desc_dev[0];
        VUDC_aDescriptorData_string_langID.bLength = VUDC_Serial_desc_StringLangID[0];
        VUDC_aDescriptorData_string_vendor.bLength = VUDC_Serial_desc_StringVendor[0];
        VUDC_aDescriptorData_string_product.bLength = VUDC_Serial_desc_StringProduct[0];
        VUDC_aDescriptorData_string_serial.bLength = VUDC_Serial_desc_StringSerial[0];
        VUDC_aDescriptorData_cfg.bLength = VUDC_Serial_desc_cfg[2];
        VUDC_aDescriptorData_qual.bLength = VUDC_Serial_Desc_HighSpeedData[0];
        VUDC_aDescriptorData_oth_speed_cfg.bLength = VUDC_Serial_desc_low_speed_cfg[2];
    }

}
void VUDC_CfgDescSerialFixCom ( void )
{
    VUDC_aDescriptorData_dev.pDesc = VUDC_Serial_desc_dev_fix_com;
    VUDC_aDescriptorData_string_langID.pDesc = VUDC_Serial_desc_StringLangID;
    VUDC_aDescriptorData_string_vendor.pDesc = VUDC_Serial_desc_StringVendor;
    VUDC_aDescriptorData_string_product.pDesc = VUDC_Serial_desc_StringProduct;
    VUDC_aDescriptorData_string_serial.pDesc = VUDC_Serial_desc_StringSerial;
    VUDC_aDescriptorData_cfg.pDesc = VUDC_Serial_desc_cfg;
    VUDC_aDescriptorData_qual.pDesc = VUDC_Serial_Desc_HighSpeedData;
    VUDC_aDescriptorData_oth_speed_cfg.pDesc = VUDC_Serial_desc_low_speed_cfg;

    //length
    VUDC_aDescriptorData_dev.bLength = VUDC_Serial_desc_dev[0];
    VUDC_aDescriptorData_string_langID.bLength = VUDC_Serial_desc_StringLangID[0];
    VUDC_aDescriptorData_string_vendor.bLength = VUDC_Serial_desc_StringVendor[0];
    VUDC_aDescriptorData_string_product.bLength = VUDC_Serial_desc_StringProduct[0];
    VUDC_aDescriptorData_string_serial.bLength = VUDC_Serial_desc_StringSerial[0];
    VUDC_aDescriptorData_cfg.bLength = VUDC_Serial_desc_cfg[2];
    VUDC_aDescriptorData_qual.bLength = VUDC_Serial_Desc_HighSpeedData[0];
    VUDC_aDescriptorData_oth_speed_cfg.bLength = VUDC_Serial_desc_low_speed_cfg[2];
}
/******************************************************************************
Description:
    get the min value
Parameters:
    lenA - value A
    lenB - value B
Return Value:
    the min value of A and B
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

UINT32 VUDC_GetMinLen ( UINT32 lenA, UINT32 lenB )
{
    if ( lenA > lenB ) {
        return lenB;
    } else {
        return lenA;
    }
}

/******************************************************************************
Description:
    clear ep0 tx and rx length upon start of a setup read/write command or  Error
Parameters:
    void
Return Value:
    the min value of A and B
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EP0_LenClear ( void )
{
    dev_ep0.actRxLen = 0;
    dev_ep0.reqRxLen = 0;
    dev_ep0.actTxLen = 0;
    dev_ep0.reqTxLen = 0;
}

/******************************************************************************
Description:
    used to prepare and send the message for control xfer
Parameters:
    msg_id - message ID, usually action
    message - pointer to the message structure
Return Value:
    void
Remarks:
    this function is employed make sure every Malloc'ed message is sent,  to avoid mem leak
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_CtrlMsg_Send ( UINT32 msg_id, VOTG_MSG *message )
{

    VOTG_MSG msg;
    UINT8 ep0 = TRUE;

    switch ( msg_id ) {
        case VOTG_MSG_DEV_EP0_NODATA:
            VUDC_EP0_RxServiced_NoData();
            break;
        case VOTG_MSG_DEV_EP0_RX:
            VUDC_RX_EP0 ( message->len, ( UINT32 ) message->p_msg, message->offset, message->dend );
            break;
        case VOTG_MSG_DEV_EP0_TX:
            VUDC_TX_EP0 ( message->len, ( UINT32 ) message->p_msg, message->offset, message->dend );
            break;
        case VOTG_MSG_DEV_EP0_STALL:
            VUDC_EP0_RxServiced_STALL();
            break;
        default:
            ep0 = FALSE;
            break;
    }

    if ( !ep0 ) {
        VCOMMON_MemSet ( &msg, 0, sizeof ( VOTG_MSG ) );
        //copy message info
        msg.dend = message->dend;
        msg.idx = message->idx;
        msg.len = message->len;
        msg.offset = message->offset;
        msg.p_msg = message->p_msg;

        //VOTG_NPRINTF("id=%d,dend=%d,idx=%d,len=%d,offset=%d\r\n",msg_id,msg.dend,msg.idx,msg.len,msg.offset);
        //send to queue
        VOTG_MSG_Send ( msg_id, &msg );

        //VOTG_Dev_MSGHandler(msg_id, &msg);

    }
}

/******************************************************************************
Description:
    used to prepare the controller to ack the command setup stage for a setup command
    and send the status stage
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EP0_RxServiced_NoData ( void )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_EP0_CSR );
    reg = reg | UOTGC_D_CSR0_SVDOPR | UOTGC_D_CSR0_DE;
    WRITE_REG ( REG_UOTG_EP0_CSR, reg );
}

/******************************************************************************
Description:
    used to prepare the controller to ack the command setup stage for a setup command
    and send stall
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EP0_RxServiced_STALL ( void )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_EP0_CSR );
    reg = reg | UOTGC_D_CSR0_SS | UOTGC_D_CSR0_SVDOPR;
    WRITE_REG ( REG_UOTG_EP0_CSR, reg );
}

/******************************************************************************
Description:
    used to prepare the controller to send data for the data stage for a setup read command
    if dataend is set, then status stage will also be send after data stage
Parameters:
    len - xfer data length
    addr - data buffer base adderss (must be word align )
    offset - offset of data buffer (must be word align )
    dataend - TRUE, status stage will also be send
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_TX_EP0 ( UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend )
{
    //VOTG_NPRINTF("len :%d,addr:%08x,offset:%08x,dataend:%02x\r\n",len,addr,offset,dataend);
    VOTG_DMA_StartWrite ( VDMA_USB_SRAM0, addr + offset, len );
    WRITE_REG ( REG_UOTG_EP0_TXCNT, len );
    
    if ( dataend ) {
        WRITE_REG ( REG_UOTG_EP0_CSR, UOTGC_D_CSR0_SVDOPR | UOTGC_D_CSR0_IPR | UOTGC_D_CSR0_DE );
        is_otg_tx_data_end = 1;
    } else {
        WRITE_REG ( REG_UOTG_EP0_CSR, UOTGC_D_CSR0_SVDOPR | UOTGC_D_CSR0_IPR );
    }

}

/******************************************************************************
Description:
    used to prepare the controller to receive data for the data stage for a setup write command
    if dataend is set, then status stage will also be send after data stage
Parameters:
    len - xfer data length
    addr - data buffer base adderss (must be word align )
    offset - offset of data buffer (must be word align )
    dataend - TRUE, status stage will also be send
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_RX_EP0 ( UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend )
{

    VOTG_DMA_StartRead ( VDMA_USB_SRAM0, addr + offset, len );
    if ( dataend ) {
        WRITE_REG ( REG_UOTG_EP0_CSR, UOTGC_D_CSR0_SVDOPR | UOTGC_D_CSR0_DE );
    } else {
        WRITE_REG ( REG_UOTG_EP0_CSR, UOTGC_D_CSR0_SVDOPR );
    }

}

/******************************************************************************
Description:
    used to send rx stall for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_EPxRxSTALL ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_RXCSR );
    reg = reg | UOTGC_RXCSR_SDS;
    WRITE_REG ( REG_UOTG_EP_RXCSR, reg );

    //VOTG_WPRINTF("UOTG: VUDC_EPxRxSTALL ep %d stall \r\n", ep);
}

/******************************************************************************
Description:
    used to clear rx stall status for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPxClrRxSTALL ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_RXCSR );
    reg = reg & ~ ( UOTGC_RXCSR_SDS );
    reg = reg | UOTGC_RXCSR_CDT;
    WRITE_REG ( REG_UOTG_EP_RXCSR, reg );
}

/******************************************************************************
Description:
    used to send tx stall for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_EPxTxSTALL ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_TXCSR );
    reg = reg & ~ ( UOTGC_TXCSR_UR | UOTGC_TXCSR_ITX );
    reg = reg | UOTGC_TXCSR_SDS;
    WRITE_REG ( REG_UOTG_EP_TXCSR, reg );

    //VOTG_WPRINTF("UOTG: VUDC_EPxTxSTALL ep %d stall \r\n", ep);
}

/******************************************************************************
Description:
    used to clear tx stall for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPxClrTxSTALL ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_TXCSR );
    reg = reg & ~ ( UOTGC_TXCSR_SDS | UOTGC_TXCSR_UR | UOTGC_TXCSR_ITX );
    reg = reg | UOTGC_TXCSR_CDT;
    WRITE_REG ( REG_UOTG_EP_TXCSR, reg );
    //VOTG_WPRINTF("UOTG: VUDC_EPxClrTxSTALL ep %d clear stall \r\n", ep);
}

/******************************************************************************
Description:
    used to clear rx data toggle for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPxClrRxDTog ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_RXCSR );
    reg = reg | UOTGC_RXCSR_CDT;
    WRITE_REG ( REG_UOTG_EP_RXCSR, reg );
}

/******************************************************************************
Description:
    used to clear tx data toggle for epX
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPxClrTxDTog ( UINT8 ep )
{
    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_TXCSR );
    reg = reg | UOTGC_TXCSR_CDT;
    WRITE_REG ( REG_UOTG_EP_TXCSR, reg );
}

/******************************************************************************
Description:
    used to config epX as ISO xfer type RX ep
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_EPx_CfgRxISO ( UINT8 ep )
{

    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_RXTYPE );
    reg = ep | UOTGC_TFTYPE_ISOC;
    WRITE_REG ( REG_UOTG_EP_RXTYPE, reg );
    reg = READ_REG ( REG_UOTG_EP_RXCSR );
    reg = reg | UOTGC_RXCSR_ISO;
    WRITE_REG ( REG_UOTG_EP_RXCSR, reg );
}

/******************************************************************************
Description:
    used to config epX as INT xfer type TX ep
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_EPx_CfgTxINT ( UINT8 ep )
{

    UINT32 reg;
    WRITE_REG ( REG_UOTG_INDEX, ep );
    reg = READ_REG ( REG_UOTG_EP_TXTYPE );
    reg = ep | UOTGC_TFTYPE_INTR;
    WRITE_REG ( REG_UOTG_EP_TXTYPE, reg );
    /*reg = READ_REG(REG_UOTG_EP_TXCSR);
    reg = reg | UOTGC_TXCSR_CDT;
    WRITE_REG(REG_UOTG_EP_TXCSR, reg);*/
}

/******************************************************************************
Description:
    used to prepare epX for ISO RX
Parameters:
    ep - endpoint number
    len - rx length
    addr - rx data buffer addr base
    offset - offset of data buffer addr base
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_RX_ISO_EPx ( UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset )
{
    //UINT32 reg;

    //VOTG_NPRINTF("UOTG: -> Rx ep %d len %d \r\n", ep, len);

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "UOTG: ISO RxEp0 when RxEpx %d len %d \r\n", ep, len );
    } else if ( ep == 1 ) {
        VOTG_DMA_StartRead ( VDMA_USB_SRAM1, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 1 );
        if ( len > VUDC_EP1_MAX_RX_SIZE ) {
            VOTG_EPRINTF ( "UOTG: ISO when RxEpx %d len %d > MaxPkt\r\n", ep, len );
        }
        WRITE_REG ( REG_UOTG_EP_PKTNUM, 1 );
    } else if ( ep == 2 ) {
        VOTG_DMA_StartRead ( VDMA_USB_SRAM2, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 2 );
        if ( len > VUDC_EP2_MAX_RX_SIZE ) {
            VOTG_EPRINTF ( "UOTG: ISO when RxEpx %d len %d > MaxPkt \r\n", ep, len );
        }
        WRITE_REG ( REG_UOTG_EP_PKTNUM, 1 );
    }

}

/******************************************************************************
Description:
    used to prepare the tx buffer for test packet sending
Parameters:
    none
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_SetTestPkt ( void )
{
    UINT8 i ;

    for ( i = 0; i < 9; i++ ) {
        dmatxbuf[i] = 0x0;
    }
    for ( i = 9; i < 17; i++ ) {
        dmatxbuf[i] = 0xAA;
    }
    for ( i = 17; i < 25; i++ ) {
        dmatxbuf[i] = 0xEE;
    }
    dmatxbuf[25] = 0xFE;
    for ( i = 26; i < 37; i++ ) {
        dmatxbuf[i] = 0xFF;
    }
    dmatxbuf[37] = 0x7F;
    dmatxbuf[38] = 0xBF;
    dmatxbuf[39] = 0xDF;
    dmatxbuf[40] = 0xEF;
    dmatxbuf[41] = 0xF7;
    dmatxbuf[42] = 0xFB;
    dmatxbuf[43] = 0xFD;
    dmatxbuf[44] = 0xFC;
    dmatxbuf[45] = 0x7E;
    dmatxbuf[46] = 0xBF;
    dmatxbuf[47] = 0xDF;
    dmatxbuf[48] = 0xEF;
    dmatxbuf[49] = 0xF7;
    dmatxbuf[50] = 0xFB;
    dmatxbuf[51] = 0xFD;
    dmatxbuf[52] = 0x7E;
}

/******************************************************************************
Description:
    used to prepare epX for INT/BULK TX
Parameters:
    ep - endpoint number
    len - tx length
    addr - tx data buffer addr base
    offset - offset of data buffer addr base
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_TX_EPx ( UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset )
{

    UINT32 reg;

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "UOTG: TxEp0 when TxEpx %d len %d ------------------>\r\n", ep, len );
    } else if ( ep == 1 ) {
        WRITE_REG ( REG_UOTG_TXBYTECNT1, len );
        VOTG_DMA_StartWrite ( VDMA_USB_SRAM1, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 1 );

        reg = READ_REG ( REG_UOTG_EP_TXCSR );
        reg = reg | UOTGC_TXCSR_TPR;
        WRITE_REG ( REG_UOTG_EP_TXCSR, reg );
    } else if ( ep == 2 ) {
        WRITE_REG ( REG_UOTG_TXBYTECNT2, len );
        VOTG_DMA_StartWrite ( VDMA_USB_SRAM2, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 2 );

        reg = READ_REG ( REG_UOTG_EP_TXCSR );
        reg = reg | UOTGC_TXCSR_TPR;
        WRITE_REG ( REG_UOTG_EP_TXCSR, reg );
    }

    //VOTG_NPRINTF("UOTG: Tx ep %d len %d --------->\r\n", ep, len);

}

/******************************************************************************
Description:
    used to prepare epX for INT/BULK RX
Parameters:
    ep - endpoint number
    len - rx length
    addr - rx data buffer addr base
    offset - offset of data buffer addr base
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_RX_EPx ( UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset )
{
    UINT32 pktnum;
    UINT32 reg;

    //VOTG_NPRINTF("UOTG: -> Rx ep %d len %d \r\n", ep, len);

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "UOTG: RxEp0 when RxEpx %d len %d \r\n", ep, len );
    } else if ( ep == 1 ) {
        VOTG_DMA_StartRead ( VDMA_USB_SRAM1, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 1 );
        reg = READ_REG ( REG_UOTG_EP_RXCSR );
        reg = reg | UOTGC_RXCSR_AUTO;
        WRITE_REG ( REG_UOTG_EP_RXCSR, reg );
        pktnum = ( len % VUDC_EP1_MAX_RX_SIZE ) ? ( len / VUDC_EP1_MAX_RX_SIZE + 1 ) : ( len / VUDC_EP1_MAX_RX_SIZE );
        WRITE_REG ( REG_UOTG_EP_PKTNUM, pktnum );
    } else if ( ep == 2 ) {
        VOTG_DMA_StartRead ( VDMA_USB_SRAM2, addr + offset, len );
        WRITE_REG ( REG_UOTG_INDEX, 2 );
        reg = READ_REG ( REG_UOTG_EP_RXCSR );
        reg = reg | UOTGC_RXCSR_AUTO;
        WRITE_REG ( REG_UOTG_EP_RXCSR, reg );
        pktnum = ( len % VUDC_EP2_MAX_RX_SIZE ) ? ( len / VUDC_EP2_MAX_RX_SIZE + 1 ) : ( len / VUDC_EP2_MAX_RX_SIZE );
        WRITE_REG ( REG_UOTG_EP_PKTNUM, pktnum );
    }

}

/******************************************************************************
Description:
    used to parse the setup command to get if it is a read or write command
    to tell the IN/OUT direction of the setup command and then setup the
    next state for ep0
Parameters:
    dmabuf - setup command data buffer address
Return Value:
    next state for ep0 state machine
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static DEV_EP0_STATES VUDC_EP0_GetInOut ( UINT8 *dmabuf )
{

    volatile UINT32 reg, i;

    DEV_EP0_STATES ret = VUDC_EP0_ST_IDLE; //default go2 status stage

    UOTGC_DEV_REQ *setup_req;

    //extract request from setup pkt
    setup_req = ( UOTGC_DEV_REQ * ) dmabuf;

    if ( VSYS_OTG_DBG_EN & 0 ) {
        reg = VDMA_USB_GetReadCount ( VDMA_USB_SRAM0 );
        VOTG_NPRINTF ( "->>received len %d buf(8):", reg );
        for ( i = 0; i < 8; i++ ) {
            VOTG_NPRINTF ( " %x ", dmabuf[i] );
        }
        VOTG_NPRINTF ( "\r\n" );
    }

    //decide the xfer direction and ep0 state
    if ( setup_req->bmRequestType & USB_CMD_DIRMASK ) { //IN data
        if ( setup_req->wLength ) {
            ret = VUDC_EP0_ST_TX;
        } else {
            ret = VUDC_EP0_ST_IDLE;
        }
    } else { //OUT data
        if ( setup_req->wLength ) {
            ret = VUDC_EP0_ST_RX;
        } else {
            ret = VUDC_EP0_ST_IDLE;
        }
    }

    return ret;

}

/******************************************************************************
Description:
    used to get if the high speed handshake result
Parameters:
    void
Return Value:
    TRUE for HighSpeed mode, FALSE for fullspeed/low speed mode
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
BOOL VUDC_GetHSMode ( void )
{
    BOOL ret;
    ret = ( READ_REG ( REG_UOTG_POWER ) & PWR_HSMOD ) ? TRUE : FALSE;
    return ret;
}

/******************************************************************************
Description:
    used to get the stall state of epX
Parameters:
    ep - endpoint number
    tx - TRUE for tx, FALSE for RX
Return Value:
    TRUE for stalled, FALSE otherwise
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
BOOL VUDC_GetEpState ( UINT8 ep, BOOL tx )
{

    UINT8 tmp = TRUE;
    BOOL ret;

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "VUDC_ClrEpState: ep %d tx %x ------------------>\r\n", ep, tx );
    } else if ( ep == 1 ) {
        tmp = tx ? ( USBD_ST & EP1_TSTALL ) : ( USBD_ST & EP1_RSTALL ) ;
    } else if ( ep == 2 ) {
        tmp = tx ? ( USBD_ST & EP2_TSTALL ) : ( USBD_ST & EP2_RSTALL );
    }

    ret = tmp ? TRUE : FALSE;
    return ret;

}

/******************************************************************************
Description:
    used to set the stall state of epX
Parameters:
    ep - endpoint number
    tx - TRUE for tx, FALSE for RX
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_SetEpState ( UINT8 ep, BOOL tx )
{

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "VUDC_ClrEpState: ep %d tx %x ------------------>\r\n", ep, tx );
    } else if ( ep == 1 ) {
        USBD_ST = tx ? ( USBD_ST | EP1_TSTALL ) : ( USBD_ST | EP1_RSTALL ) ;
    } else if ( ep == 2 ) {
        USBD_ST = tx ? ( USBD_ST | EP2_TSTALL ) : ( USBD_ST | EP2_RSTALL );
    }

}

/******************************************************************************
Description:
    used to handle set feature command
Parameters:
    setup_req - setup command  buffer pointer
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_SetFeatReqHdlr ( UOTGC_DEV_REQ *setup_req )
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT8 bIdxH;
    UINT8 bIdxL;
    ( void ) bIdxL;
    bIdxH = setup_req->wIndex >> 8; //
    bIdxL = setup_req->wIndex; //

    switch ( setup_req->bmRequestType & USB_CMD_RECIPMASK ) {
        case USB_CMD_RECIP_DEV: {
            //TODO
            switch ( setup_req->wValue ) {
                case B_HNP_ENABLE: {
                    VOTG_NPRINTF ( "UOTG: SET_FEATURE (B_HNP_REQUEST) received\r\n" );
                    ret = VRET_NO_ERROR;
                    break;
                }
                case USB_FEATURE_TESTMODE: {
                    VOTG_NPRINTF ( "UOTG: SET_FEATURE (TESTMODE) received\r\n" );
                    ret = VRET_NO_ERROR;
                    switch ( bIdxH ) { //test selector
                        case VUDC_TESTMODE_TEST_J: {
                            TEST_SEL = VUDC_TESTMODE_TEST_J;
                            VOTG_NPRINTF ( "UOTG: VUDC_TESTMODE_TEST_J received\r\n" );
                            break;
                        }
                        case VUDC_TESTMODE_TEST_K: {
                            TEST_SEL = VUDC_TESTMODE_TEST_K;
                            VOTG_NPRINTF ( "UOTG: VUDC_TESTMODE_TEST_K received\r\n" );
                            break;
                        }
                        case VUDC_TESTMODE_TEST_SE0_NAK: {
                            TEST_SEL = VUDC_TESTMODE_TEST_SE0_NAK;
                            VOTG_NPRINTF ( "UOTG: VUDC_TESTMODE_TEST_SE0_NAK received\r\n" );
                            break;
                        }
                        case VUDC_TESTMODE_TEST_PACKET: {
                            TEST_SEL = VUDC_TESTMODE_TEST_PACKET;
                            VUDC_SetTestPkt();//config the test related pkt in the buffer
                            VOTG_NPRINTF ( "UOTG: VUDC_TESTMODE_TEST_PACKET received\r\n" );
                            break;
                        }
                        default: {
                            TEST_SEL = 0;
                            VOTG_EPRINTF ( "UOTG: unknown test selector received\r\n" );
                            ret = VRET_DRV_USB_UNKCMD;
                            break;
                        }
                    }
                    break;
                }
                default: {
                    VOTG_NPRINTF ( "UOTG: Unknown SET_FEATURE received\r\n" );
                    ret = VRET_DRV_USB_UNKCMD;
                    break;
                }
            }
            break;
        }
        case USB_CMD_RECIP_IF: {
            //TODO
            ret = VRET_DRV_USB_UNKCMD;
            break;
        }
        case USB_CMD_RECIP_EP: {
            if ( ( setup_req->wValue == USB_FEATURE_ENDPOINT_STALL )
                    && ( ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK ) != 0 ) ) {
                if ( setup_req->wIndex & USB_ENDPOINT_DIR_MASK ) {
                    VUDC_SetEpState ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK, TRUE );
                    VUDC_EPxTxSTALL ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK );
                } else {
                    VUDC_SetEpState ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK, FALSE );
                    VUDC_EPxRxSTALL ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK );
                }
                ret = VRET_NO_ERROR;
            } else {
                ret = VRET_DRV_USB_UNKCMD;
            }
            break;
        }
    }

    return ret;
}

/******************************************************************************
Description:
    used to clear the stall state of epX
Parameters:
    ep - endpoint number
    tx - TRUE for tx, FALSE for RX
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VUDC_ClrEpState ( UINT8 ep, BOOL tx )
{

    if ( ep == 0 ) {
        VOTG_EPRINTF ( "VUDC_ClrEpState: ep %d tx %x ------------------>\r\n", ep, tx );
    } else if ( ep == 1 ) {
        USBD_ST = tx ? ( USBD_ST & ~EP1_TSTALL ) : ( USBD_ST & ~EP1_RSTALL ) ;
    } else if ( ep == 2 ) {
        USBD_ST = tx ? ( USBD_ST & ~EP2_TSTALL ) : ( USBD_ST & ~EP2_RSTALL );
    }

}

/******************************************************************************
Description:
    used to handle clear feature command
Parameters:
    setup_req - setup command  buffer pointer
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_ClrFeatReqHdlr ( UOTGC_DEV_REQ *setup_req )
{
    VRET_VALUE ret = VRET_NO_ERROR;

    switch ( setup_req->bmRequestType & USB_CMD_RECIPMASK ) {
        case USB_CMD_RECIP_DEV: {
            //TODO
            ret = VRET_DRV_USB_UNKCMD;
            break;
        }
        case USB_CMD_RECIP_IF: {
            //TODO
            ret = VRET_DRV_USB_UNKCMD;
            break;
        }
        case USB_CMD_RECIP_EP: {
            if ( ( setup_req->wValue == USB_FEATURE_ENDPOINT_STALL )
                    && ( ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK ) != 0 ) ) {
                if ( setup_req->wIndex & USB_ENDPOINT_DIR_MASK ) {
                    VUDC_ClrEpState ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK, TRUE );
                    VUDC_EPxClrTxSTALL ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK );
                } else {
                    VUDC_ClrEpState ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK, FALSE );
                    VUDC_EPxClrRxSTALL ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK );
                }
                ret = VRET_NO_ERROR;
                break;
            } else {
                ret = VRET_DRV_USB_UNKCMD;
            }
        }
    }

    return ret;
}

/******************************************************************************
Description:
    used to handle get status command
Parameters:
    setup_req - setup command  buffer pointer
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_GetStatusReqHdlr ( UOTGC_DEV_REQ *setup_req )
{
    VRET_VALUE ret = VRET_NO_ERROR;

    switch ( setup_req->bmRequestType & USB_CMD_RECIPMASK ) {
        case USB_CMD_RECIP_DEV: {
            if ( setup_req->wIndex == 0x0 ) { //for device this must be 0
                dmatxbuf[0] = 0x00; //BUS powered and no remote wakeup
                dmatxbuf[1] = 0x00; //
            } else {
                ret = VRET_DRV_USB_UNKCMD;
            }
            break;
        }
        case USB_CMD_RECIP_IF: {
            //TODO
            ret = VRET_DRV_USB_UNKCMD;
            break;
        }
        case USB_CMD_RECIP_EP: {
            if ( ( setup_req->wValue == USB_FEATURE_ENDPOINT_STALL )
                    && ( ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK ) != 0 ) ) {
                dmatxbuf[0] = VUDC_GetEpState ( setup_req->wIndex & USB_ENDPOINT_NUMBER_MASK, setup_req->wIndex & USB_ENDPOINT_DIR_MASK );
                dmatxbuf[1] = 0x00;
                ret = VRET_NO_ERROR;
                break;
            } else {
                ret = VRET_DRV_USB_UNKCMD;
            }
        }
    }

    return ret;
}

/******************************************************************************
Description:
    used to handle get string descriptor command
Parameters:
    setup_req - setup command  buffer pointer
    msg -pointer to the message to be send to host, used to store info for the string
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_StringReqHdlr ( UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg )
{
    VRET_VALUE ret = VRET_NO_ERROR;

    UINT8 bDescType;
    UINT8 bIndex;
    ( void ) bDescType;
    bDescType = setup_req->wValue >> 8; //USB_CMDOFF_VALUEHI;
    bIndex = setup_req->wValue; //USB_CMDOFF_VALUELO;

    if ( bIndex == 0x0 ) { //send language ID
        msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_string_langID.bLength, setup_req->wLength ); //6
        VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_string_langID.pDesc, msg->len );
        msg->p_msg = ( UINT32 * ) dmatxbuf;
        //msg->p_msg = (UINT32 *)VUDC_aDescriptorData_string_langID;
    } else {//should send string
        if ( setup_req->wIndex != 0x0409 ) { //language ID not match English US
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_WPRINTF ( "UOTG: GET_DESCRIPTOR (STRING) unknown wIndex(languageID) %x\r\n", setup_req->wIndex );
        } else { //language ID good
            if ( bIndex == 0x3 ) {
                msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_string_serial.bLength, setup_req->wLength );
                VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_string_serial.pDesc, msg->len );
                msg->p_msg = ( UINT32 * ) dmatxbuf;
                //msg->p_msg = (UINT32 *)(VUDC_aDescriptorData_string_lang1+32);
            } else if ( bIndex == 0x2 ) {
                msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_string_product.bLength, setup_req->wLength );
                VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_string_product.pDesc, msg->len );
                msg->p_msg = ( UINT32 * ) dmatxbuf;
                //msg->p_msg = (UINT32 *)(VUDC_aDescriptorData_string_lang1+16);
            } else if ( bIndex == 0x1 ) {
                msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_string_vendor.bLength, setup_req->wLength );;
                VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_string_vendor.pDesc, msg->len );
                msg->p_msg = ( UINT32 * ) dmatxbuf;
                //msg->p_msg = (UINT32 *)(VUDC_aDescriptorData_string_lang1);
            } else {
                ret = VRET_DRV_USB_UNKCMD;
                VOTG_WPRINTF ( "UOTG: GET_DESCRIPTOR (STRING) unknown idx %d\r\n", bIndex );
            }
        }//language id GOOD
    }//send string
    return ret;
}

/******************************************************************************
Description:
    used to parse the setup write command info. Get the write data length, prepare for RX
    the same length.
Parameters:
    dmabuf - setup command  buffer pointer
    msg -pointer to the message to be send to host, used to store info for the command
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static VRET_VALUE VUDC_EP0_CmdRxSnoop ( UINT8 *dmabuf, VOTG_MSG *msg )
{

    VRET_VALUE ret = VRET_NO_ERROR; //default go2 status stage

    UOTGC_DEV_REQ *setup_req;

    //extract request from setup pkt
    setup_req = ( UOTGC_DEV_REQ * ) dmabuf;

    msg->len = setup_req->wLength;

    return ret;

}

/******************************************************************************
Description:
    used to parse the setup command. main paser for setup command, then call the related
    command handler
Parameters:
    dmabuf - setup command  buffer pointer
    msg -pointer to the message to be send to host, used to store info for the command
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static VRET_VALUE VUDC_EP0_CmdParser ( UINT8 *dmabuf, VOTG_MSG *msg )
{

    VRET_VALUE ret = VRET_NO_ERROR; //default go2 status stage

    UOTGC_DEV_REQ *setup_req;

    UINT8 bDescType;
    UINT8 bIndex;

    //extract request from setup pkt
    setup_req = ( UOTGC_DEV_REQ * ) dmabuf;
    bDescType = setup_req->wValue >> 8; //USB_CMDOFF_VALUEHI;
    bIndex = setup_req->wValue; //USB_CMDOFF_VALUELO;

    switch ( setup_req->bmRequestType & USB_CMD_TYPEMASK ) {
        case USB_CMD_STDREQ: {
            switch ( setup_req->bRequest ) {
                case USB_REQ_GET_STATUS: {
                    VOTG_NPRINTF ( "UOTG:  GET_STATUS received\r\n" );
                    if ( ( setup_req->bmRequestType & USB_CMD_DIRMASK ) != USB_CMD_D2H_IN
                            || setup_req->wValue != 0x0 ) {
                        //command check
                        ret = VRET_DRV_USB_UNKCMD;
                        VOTG_EPRINTF ( "UOTG: GET_STATUS Req Error Req %x \r\n", setup_req->bmRequestType );
                        return ret;
                    }
                    msg->len = 2;
                    msg->p_msg = ( UINT32 * ) dmatxbuf;
                    ret = VUDC_GetStatusReqHdlr ( setup_req );
                    break;
                }
                case USB_REQ_CLEAR_FEATURE: {
                    VOTG_NPRINTF ( "UOTG:  CLEAR_FEATURE received\r\n" );
                    if ( ( setup_req->bmRequestType & USB_CMD_DIRMASK ) != USB_CMD_H2D_OUT ) {
                        ret = VRET_DRV_USB_UNKCMD;
                        VOTG_EPRINTF ( "UOTG: CLEAR_FEATURE Req Error Req %x \r\n", setup_req->bmRequestType );
                        return ret;
                    }
                    ret = VUDC_ClrFeatReqHdlr ( setup_req );
                    break;
                }
                case USB_REQ_SET_FEATURE: {
                    if ( ( setup_req->bmRequestType & USB_CMD_DIRMASK ) != USB_CMD_H2D_OUT ) {
                        ret = VRET_DRV_USB_UNKCMD;
                        VOTG_EPRINTF ( "UOTG: USB_REQ_SET_FEATURE Req Error Req %x \r\n", setup_req->bmRequestType );
                        return ret;
                    }
                    ret = VUDC_SetFeatReqHdlr ( setup_req );
                    break;
                }
                case USB_REQ_SET_ADDRESS: {
                    if ( ( setup_req->bmRequestType & USB_CMD_DIRMASK ) != USB_CMD_H2D_OUT ) {
                        ret = VRET_DRV_USB_UNKCMD;
                        VOTG_EPRINTF ( "UOTG: USB_REQ_SET_ADDRESS Req Error Req %x \r\n", setup_req->bmRequestType );
                    }
                    FAddr = setup_req->wValue;
                    USBD_ST = USBD_ST | USBD_ADDRED;
                    VOTG_NPRINTF ( "UOTG: SET_ADDRESS received Addr %x \r\n", FAddr );
                    break;
                }
                case USB_REQ_GET_DESCRIPTOR: {
                    //VOTG_NPRINTF("UOTG: GET_DESCRIPTOR\r\n");
                    // Decode required descriptor from the command
                    if ( ( setup_req->bmRequestType & USB_CMD_DIRMASK ) != USB_CMD_D2H_IN ) {
                        ret = VRET_DRV_USB_UNKCMD;
                        VOTG_EPRINTF ( "UOTG: GET_DESCRIPTOR Req Error Req %x \r\n", setup_req->bmRequestType );
                        return ret;
                    }
                    switch ( bDescType ) {
                        case USB_DEVICE_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (DEVICE) received\r\n" );
                            msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_dev.bLength, setup_req->wLength ); //setup_req->wLength
                            VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_dev.pDesc, msg->len );
                            msg->p_msg = ( UINT32 * ) dmatxbuf;
                            //msg->p_msg = (UINT32 *)VUDC_aDescriptorData_dev;
                            break;
                        }
                        case USB_CONFIGURATION_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (CONFIG) received\r\n" );
                            VOTG_NPRINTF ( "des_len:%02x,setup_len:%04x\r\n", VUDC_aDescriptorData_cfg.bLength, setup_req->wLength );
                            msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_cfg.bLength, setup_req->wLength );
                            VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_cfg.pDesc, msg->len );
                            msg->p_msg = ( UINT32 * ) dmatxbuf;
                            //msg->p_msg = (UINT32 *)VUDC_aDescriptorData_cfg;
                            break;
                        }
                        case USB_STRING_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (STRING) received\r\n" );
                            msg->len = setup_req->wLength;
                            ret = VUDC_StringReqHdlr ( setup_req, msg );
                            break;
                        }
                        case USB_INTERFACE_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (interface) received\r\n" );
                            ret = VRET_DRV_USB_UNKCMD;
                            break;
                        }
                        case USB_OTG_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (OTG) received\r\n" );
                            ret = VRET_DRV_USB_UNKCMD;
                            break;
                        }
                        case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (DEVQUAL) received\r\n" );
                            if ( ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_AUDIO ) ||
                                    ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) ) {
                                ret = VRET_DRV_USB_UNKCMD; //audio device does not support other speed, should STALL
                                break;
                            }
                            msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_qual.bLength, setup_req->wLength );
                            VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_qual.pDesc, msg->len );
                            msg->p_msg = ( UINT32 * ) dmatxbuf;
                            break;
                        }
                        case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (OTHER_SPEED_CONFIG) received\r\n" );
                            if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_AUDIO ) {
                                ret = VRET_DRV_USB_UNKCMD; //audio device does not support other speed, should STALL
                                break;
                            }
                            msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_oth_speed_cfg.bLength, setup_req->wLength );
                            VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_oth_speed_cfg.pDesc, msg->len );
                            msg->p_msg = ( UINT32 * ) dmatxbuf;
                            break;
                        }
                        case USB_RPT_DESCRIPTOR_TYPE: {
                            VOTG_NPRINTF ( "UOTG: GET_DESCRIPTOR (Report) received\r\n" );
                            if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_STORE ) {
                                ret = VRET_DRV_USB_UNKCMD; //MSD does not support HID report, should STALL
                                break;
                            }
                            msg->len = VUDC_GetMinLen ( VUDC_aDescriptorData_rpt.bLength, setup_req->wLength );
                            VCOMMON_MemCpy ( dmatxbuf, VUDC_aDescriptorData_rpt.pDesc, msg->len );
                            msg->p_msg = ( UINT32 * ) dmatxbuf;
                            break;
                        }
                        default: {
                            VOTG_NPRINTF ( "UOTG: Unknown GET_DESCRIPTOR received %x\r\n", bDescType );
                            ret = VRET_DRV_USB_UNKCMD;
                        }
                    }//switch bDescType
                    break;
                }//case USB_REQ_GET_DESCRIPTOR
                case USB_REQ_SET_DESCRIPTOR: {
                    VOTG_NPRINTF ( "UOTG: USB_REQ_SET_DESCRIPTOR received\r\n" );
                    ret = VRET_DRV_USB_UNKCMD;
                    break;
                }
                case USB_REQ_GET_CONFIGURATION: {
                    msg->len = 1; //length fixed 1
                    dmatxbuf[0] = ( USBD_ST & USBD_CFGED ) ? 0x1 : 0x0;
                    msg->p_msg = ( UINT32 * ) dmatxbuf;
                    VOTG_NPRINTF ( "UOTG: USB_REQ_GET_CONFIGURATION received\r\n" );
                    break;
                }
                case USB_REQ_SET_CONFIGURATION: {
                    VOTG_NPRINTF ( "UOTG: SET_CONFIGURATION received\r\n" );
                    VUDC_EPxClrTxDTog ( VOTG_EP1 );
                    VUDC_EPxClrRxDTog ( VOTG_EP1 );
                    VUDC_EPxClrTxDTog ( VOTG_EP2 );
                    VUDC_EPxClrRxDTog ( VOTG_EP2 );

                    if ( bDescType == 00 && bIndex == 0x01 ) {
                        USBD_ST = USBD_ST | USBD_CFGED;
                    } else if ( bDescType == 00 && bIndex == 0x00 ) {
                        USBD_ST = USBD_ST & ~USBD_CFGED;
                    } else {
                        VOTG_NPRINTF ( "UOTG: Unknown SET_CONFIGURATION received %x\r\n", bIndex );
                        ret = VRET_DRV_USB_UNKCMD;
                    }
                    break;
                }
                case USB_REQ_GET_INTERFACE: {
                    VOTG_NPRINTF ( "UOTG: USB_REQ_GET_INTERFACE received\r\n" );
                    dmatxbuf[0] = USBD_CurrIntfSetting;
                    msg->len = 1; //length fixed 1
                    msg->p_msg = ( UINT32 * ) dmatxbuf;
                    break;
                }
                case USB_REQ_SET_INTERFACE: {
                    VOTG_NPRINTF ( "UOTG: USB_REQ_SET_INTERFACE received\r\n" );
                    USBD_CurrIntf = setup_req->wIndex;
                    USBD_CurrIntfSetting = setup_req->wValue;
                    VUDC_EPxClrTxDTog ( VOTG_EP1 );
                    VUDC_EPxClrRxDTog ( VOTG_EP1 );
                    VUDC_EPxClrTxDTog ( VOTG_EP2 );
                    VUDC_EPxClrRxDTog ( VOTG_EP2 );
                    break;
                }
                case USB_REQ_SYNCH_FRAME: {
                    VOTG_NPRINTF ( "UOTG: USB_REQ_SYNCH_FRAME received\r\n" );
                    ret = VRET_DRV_USB_UNKCMD;
                    break;
                }
                default: {
                    VOTG_NPRINTF ( "UOTG: Unknown STD_REQUEST received REQ %x\r\n", setup_req->bRequest );
                    ret = VRET_DRV_USB_UNKCMD;
                }
            }
            break;
        }
        case USB_CMD_CLASSREQ: {
            VOTG_NPRINTF ( "UOTG: Class Request received\r\n" );
            if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) {
                ret = VUDC_Serial_ClassReqHdlr ( setup_req, msg );
            }
            break;
        }
        case USB_CMD_VENDREQ: {
            VOTG_EPRINTF ( "UOTG: Vendor Request received\r\n" );
            ret = VRET_DRV_USB_UNKCMD;
            break;
        }
        default: {
            VOTG_EPRINTF ( "UOTG: Unknown Request received\r\n" );
            ret = VRET_DRV_USB_UNKCMD;
        }
    } //switch
    return ret;
}

/******************************************************************************
Description:
    epX error handler when RX interrupt happened
    will clear the related error bit
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPx_RxErrHdler ( UINT32 ep )
{
    UINT32 reg, wval;

    switch ( ep ) {
        case VOTG_EP1:
            WRITE_REG ( REG_UOTG_INDEX, VOTG_EP1 );
            break;
        case VOTG_EP2:
            WRITE_REG ( REG_UOTG_INDEX, VOTG_EP2 );
            break;
        default:
            VOTG_WPRINTF ( "VUDC_EPx_RxErrHdler: wrong ep num %d \r\n", ep );
            break;
    }

    reg = READ_REG ( REG_UOTG_EP_RXCSR );
    wval = reg;

    //VOTG_NPRINTF("VUDC: ep %d REG_UOTG_EP_RXCSR %x \r\n",ep, reg);

    if ( reg & UOTGC_RXCSR_STS ) {
        wval = wval & ~UOTGC_RXCSR_STS;
        VOTG_WPRINTF ( "VUDC: ep %d sent stall clear \r\n", ep );
    }//clrear sent stall after stall is send

    if ( reg & UOTGC_RXCSR_OR ) {
        wval = wval & ~UOTGC_RXCSR_OR;
        VOTG_WPRINTF ( "VUDC: ep %d overrun clear \r\n", ep );
    }//clrear overrun, only for ISO

    if ( reg & UOTGC_RXCSR_DE ) {
        wval = wval & ~UOTGC_RXCSR_DE;
        VOTG_WPRINTF ( "VUDC: ep %d dataerr clear \r\n", ep );
    }//clrear data error, only for ISO

    if ( reg & UOTGC_RXCSR_RPR ) {
        wval = wval & ~UOTGC_RXCSR_RPR;
    }//handle pkt send

    WRITE_REG ( REG_UOTG_EP_RXCSR, wval );

}

/******************************************************************************
Description:
    epX error handler when TX interrupt happened
    will clear the related error bit
Parameters:
    ep - endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
static void VUDC_EPx_TxErrHdler ( UINT32 ep )
{
    UINT32 reg, wval;

    switch ( ep ) {
        case VOTG_EP1:
            WRITE_REG ( REG_UOTG_INDEX, VOTG_EP1 );
            break;
        case VOTG_EP2:
            WRITE_REG ( REG_UOTG_INDEX, VOTG_EP2 );
            break;
        default:
            VOTG_WPRINTF ( "VUDC_EPx_RxErrHdler: wrong ep num %d \r\n", ep );
            break;
    }

    reg = READ_REG ( REG_UOTG_EP_TXCSR );
    wval = reg;

    if ( reg & UOTGC_TXCSR_STS ) {
        wval = wval & ~UOTGC_TXCSR_STS;
        VOTG_WPRINTF ( "VUDC: ep %d sent stall clear \r\n", ep );
    }//clrear sent stall after stall is send

    if ( reg & UOTGC_TXCSR_UR ) {
        wval = wval & ~UOTGC_TXCSR_UR;
        //VOTG_WPRINTF("VUDC: ep %d underrun clear \r\n",ep);
    }//clrear underrun

    WRITE_REG ( REG_UOTG_EP_TXCSR, wval );
}


/******************************************************************************
Description:
    ep0 isr.  handle ep0 interrupt handler
    call the related function to tx or rx more data if the command have data stage
    call the setup command parser to handler each command
    update ep0 state machine
Parameters:
    void
Return Value:
    ep0 state
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
#include <string.h>

UINT8 g_set_line_coding[8] = {0x21, 0x20, 0, 0, 0, 0, 0x07, 0x00};
UINT8 g_ep0_recv_buffer[30][10] = {0};
UINT8 g_ep0_recv_time = 0;
UINT8 g_udc_log_buffer[10] = {0};
DEV_EP0_STATES VUDC_EP0_Isr ( void )
{
    UINT32 i, reg, tmp;
    UINT32 msg_id;
    VOTG_MSG msg;
    DEV_EP0_STATES ret = VUDC_EP0_ST_IDLE;
    VRET_VALUE parse_result;
    VCOMMON_MemSet ( &msg, 0, sizeof ( VOTG_MSG ) );
    reg = READ_REG ( REG_UOTG_EP0_CSR );
    if ( reg & UOTGC_D_CSR0_SNTS ) {
        reg = reg & ~UOTGC_D_CSR0_SNTS;
        WRITE_REG ( REG_UOTG_EP0_CSR, reg );
        dev_ep0.st = VUDC_EP0_ST_IDLE;
        VOTG_WPRINTF ( "VUDC: ep0 sent stall clear \r\n" );
    }//clrear sent stall after stall is send

    if ( reg & UOTGC_D_CSR0_DE ) {
        reg = reg | UOTGC_D_CSR0_SVDSE;
        WRITE_REG ( REG_UOTG_EP0_CSR, reg );
        dev_ep0.st = VUDC_EP0_ST_IDLE;
        ret = VUDC_EP0_ST_ERR;
        VOTG_WPRINTF ( "VUDC: ep0 setup end serviced \r\n" );
    }//handle setup end

    if ( dev_ep0.st == VUDC_EP0_ST_IDLE ) {
        if ( ( reg & UOTGC_D_CSR0_OPR ) == 0 ) {
            //VOTG_NPRINTF("UOTG: NO pkt received, Status Stage\r\n");
             if ( FAddr != 0xFF ) {
                 //have to set address to the register, during the status statge
                 VOTG_NPRINTF ( "UOTG: USB device SET_ADDRESS addr %x \r\n", FAddr );
                 WRITE_REG ( REG_UOTG_FADDR, FAddr );
                 FAddr = 0xFF;

                 //colin mark
                 if ( Usb_device_hdl.device_sol->vsol_sendmsg ) {
                     //Usb_device_hdl.device_sol->vsol_sendmsg(VMMI_MSG_USB_DEVICE_PLUG_IN,0);
                 }
             }

            //test mode related begin
            if ( TEST_SEL == VUDC_TESTMODE_TEST_J ) {
                WRITE_REG ( REG_UOTG_TESTMODE, 1 << 1 );
            } else if ( TEST_SEL == VUDC_TESTMODE_TEST_K ) {
                WRITE_REG ( REG_UOTG_TESTMODE, 1 << 2 );
            } else if ( TEST_SEL == VUDC_TESTMODE_TEST_SE0_NAK ) {
                WRITE_REG ( REG_UOTG_TESTMODE, 1 );
            } else if ( TEST_SEL == VUDC_TESTMODE_TEST_PACKET ) {
                VOTG_DMA_StartWrite ( VDMA_USB_SRAM0, ( UINT32 ) dmatxbuf, 53 );
                WRITE_REG ( REG_UOTG_EP0_TXCNT, 53 );
               
                WRITE_REG ( REG_UOTG_EP0_CSR, UOTGC_D_CSR0_IPR );
                if ( ! ( READ_REG ( REG_UOTG_TESTMODE ) & ( 1 << 3 ) ) ) {
                    WRITE_REG ( REG_UOTG_TESTMODE, 1 << 3 );
                }
            } else if ( TEST_SEL != 0 ) {
                VOTG_EPRINTF ( "UOTG: USB device unknown test mode request %x\r\n", TEST_SEL );
            }
            //test mode related end

            if ( VSYS_OTG_DBG_EN & 0 ) {
                for ( i = 0; i < 8; i++ ) {
                    dmabuf[i] = 0xE;
                }
                //write 0xe, for debug purpose,restore the dmabuf to a invalid state after parsing
            }//VSYS_OTG_DBG_EN
        } else {
            VUDC_EP0_LenClear();//clear all rx tx length before each setup cmd
            //memcpy(g_ep0_recv_buffer[g_ep0_recv_time], dmabuf, 8);
            //if (0 == memcmp(g_ep0_recv_buffer[g_ep0_recv_time], g_set_line_coding, 8))
            //{
            //    g_udc_log_buffer[0]++;
            //}
            //g_ep0_recv_time++;
            
            
            //VOTG_NPRINTF("UOTG: NO pkt received, Setup Stage\r\n");
            dev_ep0.st = VUDC_EP0_GetInOut ( dmabuf );
            if ( dev_ep0.st == VUDC_EP0_ST_IDLE ) {
                //no data stage, parse now
                parse_result = VUDC_EP0_CmdParser ( dmabuf, &msg );
                if ( parse_result ) {
                    VOTG_EPRINTF ( "VUDC_EP0_CmdParser nodata failed id %x\r\n", parse_result );
                    //VUDC_EP0_RxServiced_STALL();
                    dev_ep0.st = VUDC_EP0_ST_IDLE;
                    msg_id = VOTG_MSG_DEV_EP0_STALL;
                    VUDC_CtrlMsg_Send ( msg_id, &msg );
                    ret = VUDC_EP0_ST_ERR;
                    return ret;
                }
                msg_id = VOTG_MSG_DEV_EP0_NODATA;
                VUDC_CtrlMsg_Send ( msg_id, &msg );
            } else if ( dev_ep0.st == VUDC_EP0_ST_TX ) {
                //for IN xaction, needs parse the cmd now
                parse_result = VUDC_EP0_CmdParser ( dmabuf, &msg );
                if ( parse_result ) {
                    VOTG_EPRINTF ( "VUDC_EP0_CmdParser in failed id %x\r\n", parse_result );
                    //VUDC_EP0_RxServiced_STALL();
                    dev_ep0.st = VUDC_EP0_ST_IDLE;
                    msg_id = VOTG_MSG_DEV_EP0_STALL;
                    VUDC_CtrlMsg_Send ( msg_id, &msg );
                    ret = VUDC_EP0_ST_ERR;
                    return ret;
                }
                dev_ep0.reqTxLen = msg.len;
            } else if ( dev_ep0.st == VUDC_EP0_ST_RX ) {
                parse_result = VUDC_EP0_CmdRxSnoop ( dmabuf, &msg );
                dev_ep0.reqRxLen = msg.len;
            } else {
                VOTG_EPRINTF ( "UOTG: USB device dev_ep0 state error %x\r\n", dev_ep0.st );
            }
        }//end if rxpktrdy
    }

    if ( dev_ep0.st == VUDC_EP0_ST_TX ) {
        msg_id = VOTG_MSG_DEV_EP0_TX;
        msg.offset = dev_ep0.actTxLen;

        msg.len = VUDC_GetMinLen ( ( dev_ep0.reqTxLen - dev_ep0.actTxLen ), VUDC_EP0_MAX_TX_SIZE );
        dev_ep0.actTxLen += msg.len;
        //while(delay--);
        //VOTG_NPRINTF("UOTG: USB dev_ep0 st TX len %d dataend %x ->>->->->\r\n", msg.len, msg.dend);
        if ( dev_ep0.actTxLen >= dev_ep0.reqTxLen ) {
            msg.dend = TRUE;
            if ( dev_ep0.cacheAddr != NULL ) {
                msg.p_msg = dev_ep0.cacheAddr;
                dev_ep0.cacheAddr = NULL;   //?? ==
            }
            dev_ep0.st = VUDC_EP0_ST_IDLE;

            //VOTG_NPRINTF("UOTG: USB dev_ep0 st TX exp len %d actLen %d dataend %x\r\n",
            //dev_ep0.reqTxLen, dev_ep0.actTxLen, msg.dend);
            //VOTG_NPRINTF("UOTG: USB dev_ep0 st TX exp len  actLen  dataend \r\n");
        } else {
            if ( dev_ep0.cacheAddr == NULL ) {
                dev_ep0.cacheAddr = msg.p_msg;
            } else {
                msg.p_msg = dev_ep0.cacheAddr;
            }
            msg.dend = FALSE;
        }
        VUDC_CtrlMsg_Send ( msg_id, &msg );
    }

    if ( dev_ep0.st == VUDC_EP0_ST_RX ) {
        reg = READ_REG ( REG_UOTG_EP0_CSR );
        if ( ( reg & UOTGC_D_CSR0_OPR ) == 0 ) {
            VOTG_WPRINTF ( "UOTG: USB device dev_ep0 state RX NODATA\r\n" );
        } else {//received pkt ready
            tmp = VOTG_GetEp0RxByteCnt();
            dev_ep0.actRxLen += tmp;
            //VOTG_NPRINTF("UOTG: USB dev_ep0 st RX rxLen %d actLen %d reqLen 8+ %d <-\r\n", tmp, dev_ep0.actRxLen, dev_ep0.reqRxLen);
            msg.offset = dev_ep0.actRxLen;
            msg.len = VUDC_GetMinLen ( ( dev_ep0.reqRxLen - ( dev_ep0.actRxLen - 8 ) ), VUDC_EP0_MAX_RX_SIZE );
            if ( dev_ep0.actRxLen - 8 >= dev_ep0.reqRxLen ) { //8 is the setup cmd length
                msg.dend = TRUE; //got enough data
                dev_ep0.st = VUDC_EP0_ST_IDLE;
                /*VOTG_NPRINTF("UOTG: USB dev_ep0 st RX exp len %d actLen %d dataend %x <-<<-<\r\n",
                    dev_ep0.reqRxLen, dev_ep0.actRxLen, msg.dend);*/
                parse_result = VUDC_EP0_CmdParser ( dmabuf, &msg ); //parse OUT with data stage here, U have all the data
                if ( parse_result ) {
                    VOTG_EPRINTF ( "VUDC_EP0_CmdParser out failed id %x\r\n", parse_result );
                    //VUDC_EP0_RxServiced_STALL();
                    dev_ep0.st = VUDC_EP0_ST_IDLE;
                    msg_id = VOTG_MSG_DEV_EP0_STALL;
                    VUDC_CtrlMsg_Send ( msg_id, &msg );
                    ret = VUDC_EP0_ST_ERR;
                    return ret;
                } else {//parse result good
                    msg_id = VOTG_MSG_DEV_EP0_NODATA;
                    VUDC_CtrlMsg_Send ( msg_id, &msg );
                }
            } else {
                msg.dend = FALSE;
                msg.p_msg = dmabuf;
                msg_id = VOTG_MSG_DEV_EP0_RX; //needs to receive more data
                VUDC_CtrlMsg_Send ( msg_id, &msg );
            }
        }
    }

    return ret;
}

static void VUDC_EP0_CSR_Handle(void)
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_EP0_CSR );
    
    // clrear sent stall after stall is send
     if ( reg & UOTGC_D_CSR0_SNTS ) 
     {
         reg = reg & ~UOTGC_D_CSR0_SNTS;
         WRITE_REG ( REG_UOTG_EP0_CSR, reg );
         dev_ep0.st = VUDC_EP0_ST_IDLE;
         VOTG_WPRINTF ( "VUDC: ep0 sent stall clear \r\n" );
     }

     // handle setup end
     if ( reg & UOTGC_D_CSR0_DE ) {
         reg = reg | UOTGC_D_CSR0_SVDSE;
         WRITE_REG ( REG_UOTG_EP0_CSR, reg );
         dev_ep0.st = VUDC_EP0_ST_IDLE;
         //ret = VUDC_EP0_ST_ERR;
         VOTG_WPRINTF ( "VUDC: ep0 setup end serviced \r\n" );
     }

    // 没有数据发送完成，一般是Device发送给HOST数据长度为0
    if ( ( reg & UOTGC_D_CSR0_OPR ) == 0 ) 
    {
        //VOTG_NPRINTF("UOTG: NO pkt received, Status Stage\r\n");
        if ( FAddr != 0xFF ) 
        {
            //have to set address to the register, during the status statge
            VOTG_NPRINTF ( "UOTG: USB device SET_ADDRESS addr %x \r\n", FAddr );
            WRITE_REG ( REG_UOTG_FADDR, FAddr );
            FAddr = 0xFF;
        }
    }
}
/******************************************************************************
Description:
    USB Device isr.  handle ep0, epX and other bus event interrupt
    call related isr for each endpoint
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTG_Dev_Isr ( void )
{
    UINT32 rdv;
    UINT32 bIntrUsbValue;
    UINT32 wIntrTxValue;
    UINT32 wIntrRxValue;

    bIntrUsbValue = READ_REG ( REG_UOTG_INTRUSB );
    wIntrTxValue = READ_REG ( REG_UOTG_INTRTX );    // OTG EPx发送数据完成.
    wIntrRxValue = READ_REG ( REG_UOTG_INTRRX );

    //VOTG_NPRINTF("bIntrUsbValue is 0x%x\r\n", bIntrUsbValue);
    //VOTG_NPRINTF("wIntrTxValue is 0x%x\r\n", wIntrTxValue);
    //VOTG_NPRINTF("wIntrRxValue is 0x%x\r\n", wIntrRxValue);

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_RST ) ) {
        //deal with reset interrupt
        FAddr = 0xFF;//invalid the address each time
        dev_ep0.st = VUDC_EP0_ST_IDLE;
        USBD_ST = 0;
        VOTG_DMA_StartRead ( VDMA_USB_SRAM0, ( UINT32 ) dmabuf, 64 );
        //VOTG_NPRINTF("HS MODE is %x\r\n", VUDC_GetHSMode());
        VOTGC_CfgEpxMaxPktSize();
        VOTGC_CfgPhySpdm ( 1 ); //enable cg after suspend, if
        VOTGC_EnControllerInt();
    }

    if ( wIntrTxValue & ( 0x01 << UOTGC_EPINTR_0 ) ) {
        /**
         * 因为OTG中断和DMA中断不匹配，即OTG中断产生时间点，DMA读数据有可能没有完成，因此需
         * 有如下策略：
         * 将EP0 CSR相关处理还要放在OTG中断中处理， 其余数据处理，放到DMA EP0读完成中断中处理
         */
        // g_otg_int_count++;
            VOTG_DMA_StartRead ( VDMA_USB_SRAM0, ( UINT32 ) dmabuf, 64 );
            //rdv = VOTG_GetEp0RxByteCnt();
            //VOTG_NPRINTF("ep0 interrupt rx cnt %d\r\n", rdv);
            VUDC_EP0_Isr();
        
    }//end ep0 irq

#if 1
    if ( wIntrTxValue & ( 0x01 << UOTGC_EPINTR_2 ) ) {
        VUDC_EPx_TxErrHdler ( VOTG_EP2 );
        //VOTG_NPRINTF("ep2 TX interrupt \r\n");
    }

    if ( wIntrRxValue & ( 0x01 << UOTGC_EPINTR_1 ) ) {
        VUDC_EPx_RxErrHdler ( VOTG_EP1 );
        //reg = VOTG_GetEpxRxByteCnt(AUDIO_ID_EP_OUT_ENDP);     //for debug
        //if(reg == 192) VOTG_NPRINTF("0x%x %x\r\n", dmabuf1[0], dmabuf1[1]);
        if ( Usb_device_hdl.device_audio->audio_data_in ) {
            Usb_device_hdl.device_audio->audio_data_in();
        }
    }

    if ( wIntrRxValue & ( 0x01 << UOTGC_EPINTR_2 ) ) {
        VUDC_EPx_RxErrHdler ( VOTG_EP2 );
        //Mass_Storage_Out();
        //VOTG_NPRINTF("ep2 RX interrupt \r\n");
    }
#endif
    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_CONN ) ) {
        //check for ID pin
        rdv = READ_REG ( REG_UOTG_DEVCTL ) & UOTGC_DVC_CID;
        if ( rdv == 0 ) {
            VOTG_EPRINTF ( "USBINTR: connect irq for host in Dev mode!\r\n" );
        } else {
            VOTG_EPRINTF ( "USBINTR: connect irq for host found in Dev mode while ID wrong!\r\n" );
        }
    }

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_SUS ) ) {
        VOTG_NPRINTF ( "USBINTR: Dev Suspend\r\n" );

        if ( ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) ) {
            g_usb_serial_detect_flag = 1;
            VOTG_MSG_Send ( VOTG_MSG_DEV_SUSPEND, NULL );
        } else {
            VOTGC_CfgPhySpdm ( 0 );
        }
    }

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_RES ) ) {
        VOTG_NPRINTF ( "USBINTR: Dev Reusme\r\n" );
        VOTGC_CfgPhySpdm ( 1 );
    }

    //if(bIntrUsbValue& (0x01<<UOTGC_USBINTR_SOF)) {
    //  VOTG_NPRINTF("USBINTR: SOF\r\n");
    //}

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_DISCON ) ) {
        VOTG_NPRINTF ( "USBINTR: Dev Disconnected\r\n" );
    }

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_SREQ ) ) {
        VOTG_NPRINTF ( "USBINTR: Dev SREQ\r\n" );
    }

    if ( bIntrUsbValue & ( 0x01 << UOTGC_USBINTR_VBERR ) ) {
        VOTG_EPRINTF ( "USBINTR: Vbus Error while in Dev Mode\r\n" );
    }
    //usb intr

    //VOTG_NPRINTF("bIntrUsbValue is 0x%x\r\n", bIntrUsbValue);
    //VOTG_NPRINTF("wIntrTxValue is 0x%x\r\n", wIntrTxValue);
    //VOTG_NPRINTF("wIntrRxValue is 0x%x\r\n", wIntrRxValue);

}

/******************************************************************************
Description:
    USB Device message handler.
    process each message and call the related function for related action
Parameters:
    Msg_Id - the message ID, action
    p_Msg - pointer to the message structure
Return Value:
    Error code or No error
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VOTG_Dev_MSGHandler ( UINT32 Msg_Id, void *p_Msg )
{
    VOTG_MSG *msg = ( VOTG_MSG * ) p_Msg;

    switch ( Msg_Id ) {
        case VOTG_MSG_DEV_EP0_NODATA:
            VUDC_EP0_RxServiced_NoData();
            break;
        case VOTG_MSG_DEV_EP0_RX:
            VUDC_RX_EP0 ( msg->len, ( UINT32 ) msg->p_msg, msg->offset, msg->dend );

            break;
        case VOTG_MSG_DEV_EP0_TX:
            VUDC_TX_EP0 ( msg->len, ( UINT32 ) msg->p_msg, msg->offset, msg->dend );

            break;
        case VOTG_MSG_DEV_EP0_STALL:
            VUDC_EP0_RxServiced_STALL();
            break;

        case VOTG_MSG_DEV_EP1_TX:
            VUDC_TX_EPx ( VOTG_EP1, msg->len, ( UINT32 ) dmatxbuf1, msg->offset );

            break;
        case VOTG_MSG_DEV_EP1_RX:
            VUDC_RX_EPx ( VOTG_EP1, msg->len, ( UINT32 ) dmabuf1, msg->offset );

            break;
        case VOTG_MSG_DEV_EP2_TX:
            VUDC_TX_EPx ( VOTG_EP2, msg->len, ( UINT32 ) dmatxbuf2, msg->offset );

            break;
        case VOTG_MSG_DEV_EP2_RX:
            VUDC_RX_EPx ( VOTG_EP2, msg->len, ( UINT32 ) dmabuf2, msg->offset );
            break;

        case VOTG_MSG_DEV_READ_MEM:
            //Read_Memory(msg->idx, msg->offset, msg->len);

            break;

        case VOTG_MSG_DEV_WRITE_MEM:
            //Write_Memory(msg->idx, msg->offset, msg->len);

            break;


        case VOTG_MSG_DEV_SUSPEND:
            if ( ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) ) {
                VUDC_Serial_Suspend_Proc();
            }
            break;

        case VOTG_MSG_DEV_START_UAC:
            //VUDC_AUD_Start();
            break;

        case VOTG_MSG_DEV_STOP_UAC:
            //VUDC_AUD_Stop();
            break;


        case VOTG_MSG_DEV_DSP_DBG:
#if (VDBG_EN == 1 && VDBG_IO_RETARGET == IO_USB_SERIAL)
            VDSPDBG_MessageHandle ( msg->len );
#endif
            break;

        case VOTG_MSG_DEV_EP1_TSTALL:
            VUDC_SetEpState ( VOTG_EP1, TRUE );
            VUDC_EPxTxSTALL ( VOTG_EP1 );

            break;

        case VOTG_MSG_DEV_EP1_RSTALL:
            VUDC_SetEpState ( VOTG_EP1, FALSE );
            VUDC_EPxRxSTALL ( VOTG_EP1 );

            break;

        case VOTG_MSG_DEV_EP2_TSTALL:
            VUDC_SetEpState ( VOTG_EP2, TRUE );
            VUDC_EPxTxSTALL ( VOTG_EP2 );

            break;

        case VOTG_MSG_DEV_EP2_RSTALL:
            VUDC_SetEpState ( VOTG_EP2, FALSE );
            VUDC_EPxRxSTALL ( VOTG_EP2 );

            break;

        case VOTG_MSG_DEV_BASE:
            VOTG_EPRINTF ( "VOTG_Dev_MSGHandle: VOTG_MSG_DEV_BASE Nothing Done.\r\n" );
            break;
    }

    return VRET_NO_ERROR;
}

