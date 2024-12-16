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
    OTG

Description:
    OTG Driver

Revision History:
    Date                  Author                   Description
    2014.05.04            yinxingjie                   initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vstdio.h"
#include "vdrv_dma.h"
#include "vdrv_gpio.h"
#include "vudc.h"
#include "vdrv_otg.h"
#include "vudc_serial.h"
#include "vreg_def.h"

VOTG_MSG_INFO   g_otgMsgInfo;

// 因为EXI USB速度慢，针对EXI需要做特殊处理，
// 该变量为1，表征OTG发送数据给PC的IN Packet为最后一包数据，此时产生的DMA写完成中断，不需要做任何处理，接直退出
volatile UINT32 is_otg_tx_data_end = 0;

/***************************** TYPES ******************************/
static UOTGC_Callback g_uotg_isr_callback[UOTGC_INT_NUMBER] = {NULL, NULL, NULL, NULL, NULL, NULL};

//VSYS_TMR_DATA *pUSBReConnectTimer;

UINT8 __attribute__((aligned(64))) dmabuf[ALIGN_UP(VOTG_EP0_RXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;
UINT8 __attribute__((aligned(64))) dmatxbuf[ALIGN_UP(VOTG_EP0_TXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;
UINT8 __attribute__((aligned(64))) dmabuf1[ALIGN_UP(VOTG_EP1_RXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;
UINT8 __attribute__((aligned(64))) dmatxbuf1[ALIGN_UP(VOTG_EP1_TXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;
UINT8 __attribute__((aligned(64))) dmabuf2[ALIGN_UP(VOTG_EP2_RXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;
UINT8 __attribute__((aligned(64))) dmatxbuf2[ALIGN_UP(VOTG_EP2_TXBUF_SIZE, DCACHE_OP_PER_SIZE)] __attribute__((section(".vdev_ram")));;

static BOOL USB_ID_SENSE;
//static BOOL USB_device=0;
volatile UINT8 VUHC_EP0_rst_cnt;

//BOOL USB_audioDev;
extern const USB_DEVICE_HDL Usb_device_hdl;
//extern const USB_HOST_HDL Usb_host_hdl;

static UOTGC_MODE_ID  Usb_Mode_Id;

void VOTG_SetUsbMode ( UINT8 mode )
{
    Usb_Mode_Id = ( UOTGC_MODE_ID ) mode;
}

UINT8 VOTG_GetUsbMode ( void )
{
    return Usb_Mode_Id;
}

/******************************************************************************
Description:
    To operate USB write path of DMA. Disable the specified path(endpoint)
Parameters:
    channel - write path (VDMA_USB_SRAMx),  x respect to the endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

static void VOTG_DMA_DisableWr ( VDMA_USB_PATH chanel )
{
    VDMA_MaskInt ( VDMA_INT_MC_SRAM_USB_DONE_MSK0 << chanel );
    VDMA_USB_WriteAction ( chanel, VDMA_ACTION_ABORT );
    VDMA_USB_WriteEnable ( chanel, FALSE );
}

/******************************************************************************
Description:
    To operate USB read path of DMA. Disable the specified path(endpoint)
Parameters:
    channel - read path (VDMA_USB_SRAMx),  x respect to the endpoint number
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

static void VOTG_DMA_DisableRd ( VDMA_USB_PATH chanel )
{
    VDMA_MaskInt ( VDMA_INT_MC_USB_SRAM_DONE_MSK0 << chanel );
    VDMA_USB_ReadAction ( chanel, VDMA_ACTION_ABORT );
    VDMA_USB_ReadEnable ( chanel, FALSE );
}

/******************************************************************************
Description:
    To get USB ep0 received byte count upon interrupt
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

UINT32 VOTG_GetEp0RxByteCnt ( void )
{
    return READ_REG ( REG_UOTG_EP0_COUNT );
}

/******************************************************************************
Description:
    To get USB epX received byte count upon interrupt
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

UINT32 VOTG_GetEpxRxByteCnt ( UINT32 ep )
{
    WRITE_REG ( REG_UOTG_INDEX, ep );
    return READ_REG ( REG_UOTG_EP_RXCOUNT );
}

/******************************************************************************
Description:
    EP0 DMA读完成(USB->SRAM)中断回调
Parameters: ErrorCode
Return Value: void
*****************************************************************************/
static void VOTG_DMA_EP0_ReadCompleteCallBack ( VDMA_ERR ErrorCode )
{
    if (ErrorCode != VDMA_ERR_OCCUR)
    {
        VOTG_DMA_StartRead ( VDMA_USB_SRAM0, ( UINT32 ) dmabuf, 64 );
        VUDC_EP0_Isr();
    }
}

/******************************************************************************
Description:
    EP0 DMA写完成(SRAM->USB)中断回调
Parameters: ErrorCode
Return Value: void
*****************************************************************************/
static void VOTG_DMA_EP0_WriteCompleteCallBack ( VDMA_ERR ErrorCode )
{
    if (ErrorCode != VDMA_ERR_OCCUR)
    {
        if (is_otg_tx_data_end == 1)
        {
            is_otg_tx_data_end = 0;
            return;
        }
        else
        {
            VOTG_DMA_StartRead(VDMA_USB_SRAM0, (UINT32)dmabuf, 64);
            VUDC_EP0_Isr();
        }
    }
}

/******************************************************************************
Description:
    EP2 DMA读完成(USB->SRAM)中断回调, EP2一般作为buckout端口，即PC发送数据给device
Parameters: ErrorCode
Return Value: void
*****************************************************************************/
extern volatile UINT32 remainlen;
static void VOTG_DMA_EP2_ReadCompleteCallBack ( VDMA_ERR ErrorCode )
{
    if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) 
    {
        if ( Usb_device_hdl.device_serial->serial_data_out ) 
        {
            Usb_device_hdl.device_serial->serial_data_out();
            remainlen = VDMA_USB_GetReadCount ( VDMA_USB_SRAM2 );
        }
    }
}

/******************************************************************************
Description:
    EP1 DMA写完成(SRAM->USB)中断回调, EP1一般作为buckin端口，即device发送数据给PC
Parameters: ErrorCode
Return Value: void
*****************************************************************************/
static void VOTG_DMA_EP1_WriteCompleteCallBack ( VDMA_ERR ErrorCode )
{
    if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) 
    {
        if ( Usb_device_hdl.device_serial->serial_data_in ) 
        {
            Usb_device_hdl.device_serial->serial_data_in();
        }
    }
}

/******************************************************************************
Description:
    config the related information and enable DMA read for the corresponding endpoint
Parameters:
    channel - USB DMA sram path, corresponding to ep
    Addr - xfer buffer address/pointer, needs to be in SRAM
    len - xfer length
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

void    VOTG_DMA_StartRead ( VDMA_USB_PATH channel, UINT32 Addr, UINT32 Len )
{
    VOTG_DMA_DisableRd ( channel ); // Disable both read   and write   channels before a   new start.
    if ( VOTG_GetUsbMode() != UOTGC_MODE_DEVICE_AUDIO ) {
        VDMA_UnmaskInt ( VDMA_INT_MC_USB_SRAM_DONE_MSK0 << channel );
    } else {
        VDMA_MaskInt ( VDMA_INT_MC_USB_SRAM_DONE_MSK0 << channel );
    }
    VDMA_USB_ReadEnable ( channel, TRUE );
    VDMA_USB_ConfigReadPath ( channel, Addr, Len );
    VDMA_USB_ReadAction ( channel, VDMA_ACTION_START );
}

/******************************************************************************
Description:
    config the related information and enable DMA write for the corresponding endpoint
Parameters:
    channel - USB DMA sram path, corresponding to ep
    Addr - xfer buffer address/pointer, needs to be in SRAM
    len - xfer length
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

void    VOTG_DMA_StartWrite ( VDMA_USB_PATH channel, UINT32   Addr,   UINT32 Len )
{
    VOTG_DMA_DisableWr ( channel ); // Disable both read   and write   channels before a   new start.
    if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) {
        VDMA_UnmaskInt ( VDMA_INT_MC_SRAM_USB_DONE_MSK0 << channel );
    }
    VDMA_USB_WriteEnable ( channel, TRUE );
    VDMA_USB_ConfigWritePath ( channel, Addr, Len );
    VDMA_USB_WriteAction ( channel, VDMA_ACTION_START );
}

/********************************************************************************************
 * Description :
 *      Register callback function to the UOTG level2 ISR, and ummask it.
 * Parameters :
 *      id :    Interrupt ID.
 *      pcall : Point to the callback function.
 * Return :
 *      void.
 * Note :
 *******************************************************************************************/
void VOTG_RegisterLLevel2Isr ( UOTGC_INT_ID id, UOTGC_Callback pcall )
{
    UINT32 mask = 0;

    g_uotg_isr_callback[id] = pcall;

    mask = 1 << id;
    __UOTGC_ClearSrcPending ( mask );
    __UOTGC_IntUnMask ( mask );
}

/********************************************************************************************
 * Description :
 *      UnRegister the IPP level2 ISR, and mask it.
 * Parameters :
 *      id :    Interrupt ID.

 * Return :
 *      void.
 * Note :
 *******************************************************************************************/
void VOTG_UnRegisterLevel2Isr ( UOTGC_INT_ID id )
{
    UINT32 mask = 0;

    mask = 1 << id;

    __UOTGC_IntSetMask ( mask );
    __UOTGC_ClearSrcPending ( mask );

    g_uotg_isr_callback[id] = NULL;
}

/******************************************************************************
Description:
    clear interrupt of OTG controller
Parameters:
    BitMasks - int mask
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

void VOTG_ClearInt ( UINT32 BitMasks )
{
    UINT32 reg;

    reg =   READ_REG ( REG_UOTG_UOTGC_SRC_PND );
    WRITE_REG ( REG_UOTG_UOTGC_SRC_PND,   ( reg | ( BitMasks & UOTGC_INT_MASK_ALL ) ) );
}

void VOTG_EnableInt ( void )
{
    VCPU_INT_SrcEnable ( IRQ_UOTG_INT );
}

void VOTG_DisableInt ( void )
{
    VCPU_INT_SrcDisable ( IRQ_UOTG_INT );
}

/******************************************************************************
Description:
    enable MC interrupt of OTG controller
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

void VOTGC_EnControllerInt ( void )
{
    WRITE_REG ( REG_UOTG_INTRUSBE,    0xF7 );
    WRITE_REG ( REG_UOTG_INTRTXE, 0x7 );
    WRITE_REG ( REG_UOTG_INTRRXE, 0x6 );
}

/******************************************************************************
Description:
    Disable ISO Interrupt
Parameters:
    None.
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTGC_DisISOInt ( void )
{
    UINT32 temp;

    temp = READ_REG ( REG_UOTG_INTRRXE );
    WRITE_REG ( REG_UOTG_INTRRXE,        temp & ( ~0x02 ) );
}

/******************************************************************************
Description:
    Enable ISO Interrupt
Parameters:
    None.
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTGC_EnISOInt ( void )
{
    UINT32 temp;

    temp = READ_REG ( REG_UOTG_INTRRXE );
    WRITE_REG ( REG_UOTG_INTRRXE,        temp | 0x02 );
}

/******************************************************************************
Description:
    config MC interrupt of OTG controller
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

void VOTGC_CfgEpxMaxPktSize ( void )
{
    WRITE_REG ( REG_UOTG_INDEX, VOTG_EP1 );
    WRITE_REG ( REG_UOTG_EP_TXMAXP, VUDC_EP1_MAX_TX_SIZE );
    WRITE_REG ( REG_UOTG_EP_RXMAXP, VUDC_EP1_MAX_RX_SIZE );

    WRITE_REG ( REG_UOTG_INDEX, VOTG_EP2 );
    WRITE_REG ( REG_UOTG_EP_TXMAXP, VUDC_EP2_MAX_TX_SIZE );
    WRITE_REG ( REG_UOTG_EP_RXMAXP, VUDC_EP2_MAX_RX_SIZE );

}

/******************************************************************************
Description:
    config OTG contorller to work in HS mode
Parameters:
    highspeed - true for HS mode
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTGC_CfgHighSpeed ( BOOL highspeed )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_POWER );
    if ( highspeed ) {
        reg = reg | PWR_HSEN;
    } else {
        reg = reg & ~PWR_HSEN;
    }
    WRITE_REG ( REG_UOTG_POWER, reg );
}

/******************************************************************************
Description:
    config OTG contorller to connect to the USB host
Parameters:
    connect - true for connect, false for disconnect
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTGC_CfgConnect ( BOOL connect )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_POWER );
    if ( connect ) {
        reg = reg | PWR_SFTCON;
    } else {
        reg = reg & ~PWR_SFTCON;
    }
    WRITE_REG ( REG_UOTG_POWER, reg );
}

/******************************************************************************
Description:
    used to enable host/device detect
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
void VOTGC_CfgDect ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_UOTGC_CLK_GATE );
    if ( setval ) {
        reg = reg | UOTGC_GATE_PHY_DETEN;
    } else {
        reg = reg & ~UOTGC_GATE_PHY_DETEN;
    }
    WRITE_REG ( REG_UOTG_UOTGC_CLK_GATE, reg );
}

/******************************************************************************
Description:
    used to enable clock gating of uotgc controller
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
void VOTGC_CfgCG ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_UOTGC_CLK_GATE );
    if ( setval ) {
        reg = reg | UOTGC_GATE_UOTGEN;
    } else {
        reg = reg & ~UOTGC_GATE_UOTGEN;
    }
    WRITE_REG ( REG_UOTG_UOTGC_CLK_GATE, reg );
}

/******************************************************************************
Description:
    used to reset phy
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
void VOTGC_PhyRst ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_UOTGC_CLK_GATE );
    if ( setval ) {
        reg = reg | UOTGC_GATE_PHY_RST;
    } else {
        reg = reg & ~UOTGC_GATE_PHY_RST;
    }
    WRITE_REG ( REG_UOTG_UOTGC_CLK_GATE, reg );
}

BOOL VOTG_IntPending ( void )
{
    return VCPU_INT_SrcIsPending ( IRQ_UOTG_INT );
}

void VOTG_ChargerIsr ( void )
{
    VOTG_NPRINTF ( "There is a UOTG Charger Interupt!\r\n" );
}

void VOTG_AvldIsr ( void )
{
    VOTG_NPRINTF ( "There is a UOTG AVLD Interupt!\r\n" );
}

static void VOTG_DriveBusIsr ( void )
{

    VOTG_NPRINTF ( "There is a UOTG DriveBus Interupt!\r\n" );
    VUHC_EP0_rst_cnt = 0;
    //VSOL_UsbSendMsg(VMMI_MSG_USB_HOST_PLUG_IN,0);
}

static void VOTG_UnDriveBusIsr ( void )
{
    UINT32 reg;
    ( void ) reg;
    USB_ID_SENSE = FALSE;
    VUHC_EP0_rst_cnt = 0;
    reg = READ_REG ( REG_UOTG_DEVCTL );
    VOTG_NPRINTF ( "There is a UOTG UnDriveBus Interupt! DEVCTL %x \r\n", reg );
}

/******************************************************************************
Description:
    OTG bus reset (device) isr
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
static void VOTG_BusRstIsr ( void )
{
    VOTG_NPRINTF ( "******* UOTG Dev Bus reset Interupt! ******\r\n" );
    //if(GET_REG_BIT(REG_UOTG_DEVCTL, 7)) {
    if ( READ_REG ( REG_UOTG_DEVCTL ) & UOTGC_DVC_CID ) {
        //VOTG_NPRINTF("There is a B device!\r\n");
    } else {
        VOTG_NPRINTF ( "ERROR This is HOST!\r\n" );
    }
    //VUDC_CfgDesc();
    //VOTGC_CfgHighSpeed(TRUE);
    //VOTGC_CfgConnect(TRUE);
}

/******************************************************************************
Description:
    used to enable uotgphy_idpullup
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
void VOTGC_CfgPhyIDPullUp ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_UOTGPHY_STRAP );
    if ( setval ) {
        reg = reg | UOTGC_PHYSTRAP_IDPULLUP;
    } else {
        reg = reg & ~UOTGC_PHYSTRAP_IDPULLUP;
    }
    WRITE_REG ( REG_UOTG_UOTGPHY_STRAP, reg );
}

/******************************************************************************
Description:
    used to enable power reg suspendM
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
void VOTGC_EnSpdm ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_POWER );
    if ( setval ) {
        reg = reg | PWR_SUSE;
    } else {
        reg = reg & ~PWR_SUSE;
    }
    WRITE_REG ( REG_UOTG_POWER, reg );
}

/******************************************************************************
Description:
    used to config suspendM bit of power reg
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
void VOTGC_CfgSpd ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_POWER );
    if ( setval ) {
        reg = reg | PWR_SUSM;
    } else {
        reg = reg & ~PWR_SUSM;
    }
    WRITE_REG ( REG_UOTG_POWER, reg );
}

/******************************************************************************
Description:
    used to config resume bit of power reg
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
void VOTGC_CfgRsm ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_POWER );
    if ( setval ) {
        reg = reg | PWR_RSM;
    } else {
        reg = reg & ~PWR_RSM;
    }
    WRITE_REG ( REG_UOTG_POWER, reg );
}

/******************************************************************************
Description:
    used to enable uotgphy_suspendm
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
void VOTGC_CfgPhySpdm ( BOOL setval )
{
    UINT32 reg;
    reg = READ_REG ( REG_UOTG_UOTGPHY_STRAP );
    if ( setval ) {
        reg = reg | UOTGC_PHYSTRAP_SUSPEM;
    } else {
        reg = reg & ~UOTGC_PHYSTRAP_SUSPEM;
    }
    WRITE_REG ( REG_UOTG_UOTGPHY_STRAP, reg );
}


/******************************************************************************
Description:
    OTG PMU plug in isr
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
void VOTG_PmuPulgInIsr ( void )
{
    VOTG_NPRINTF ( "******* PMU PLUG IN\r\n" );
}

/******************************************************************************
Description:
    OTG vbus valid in isr
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
void VOTG_VbusVldIsr ( void )
{
    VOTG_NPRINTF ( "******* VBUS VLD ISR\r\n" );
}

/******************************************************************************
Description:
    OTG host disconnect isr
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
void VOTG_HstDisconIsr ( void )
{
    VOTG_NPRINTF ( "******* Hst Discon ISR\r\n" );
}

/******************************************************************************
Description:
    OTG host suspend
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
void VOTG_CfgHstSpd ( BOOL setval )
{
    if ( setval ) {
        VOTG_NPRINTF ( "Hst suspend enable\r\n" );
        VOTGC_EnSpdm ( 1 );
        VOTGC_CfgSpd ( 1 );
        VOTGC_CfgPhySpdm ( 0 );
    } else {
        VOTGC_EnSpdm ( 0 );
        VOTGC_CfgPhySpdm ( 1 );
        VOTGC_CfgRsm ( 1 );
        VOTG_NPRINTF ( "Hst suspend disable\r\n" );
        VCOMMON_Delay ( 30000 );
        VOTGC_CfgRsm ( 0 );
    }
}

/******************************************************************************
Description:
    OTG as device, when switch function between audio and msc device, reconnect needs
    to be a timed task to avoid signal integrity issue after disconnect
Parameters:
    p_Tmr - timer pointer
    p_Arg - argument for funtions, not used here
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

void VOTGC_ReConnect ( void *p_Tmr, void *p_Arg )
{
    //VSYS_TMR_Stop(p_Tmr);
    //VSYS_TMR_Delete(p_Tmr);
    VOTGC_CfgConnect ( TRUE );
}

/******************************************************************************
Description:
    used to help determine if the OTG  is working as a device or host
    a timed task
Parameters:
    p_Tmr - timer pointer
    p_Arg - argument for funtions, not used here
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTGC_GetID ( void *p_Tmr, void *p_Arg )
{
    if ( USB_ID_SENSE == 0 ) {
        WRITE_REG ( REG_UOTG_DEVCTL, 0 ) ;
        VCOMMON_Delay ( 1 );
        WRITE_REG ( REG_UOTG_DEVCTL, UOTGC_DVC_SESS ) ;
    } else {
        //VSYS_TMR_Stop(p_Tmr);
        //VSYS_TMR_Delete(p_Tmr);
    }
}

/******************************************************************************
Description:
    OTG dev initial function.
    will init the OTG controller based on the device config
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

void VOTGC_DevInit ( void )
{
    //UINT8 i;
    //for(i=0;1<UOTGC_INT_NUMBER;i++)
    //  VOTG_UnRegisterLevel2Isr(i);
    //USB_device = 1;
    USB_ID_SENSE = TRUE;

    VOTG_UnRegisterLevel2Isr ( UOTGC_INT_ID_MC ); //unreg device isr
    VOTG_RegisterLLevel2Isr ( UOTGC_INT_ID_MC,    VOTG_Dev_Isr );
    VOTG_EnableInt();

    VOTGC_CfgDect ( 1 ); //enable phy to detect host/device, clock gating part1
    VOTGC_CfgCG ( 1 ); //enable clock gating part2
    VOTGC_CfgPhySpdm ( 1 );

    //*********************device init *****************
    //get ready to be device at first, because device needs to response PC host very fast
    //if later host is identified, then host related init config can be done by then
    if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_AUDIO ) {
        VOTGC_CfgHighSpeed ( FALSE );
    } else if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_STORE ) {
        VOTGC_CfgHighSpeed ( TRUE );
        //DMA related irq callback reg, for endpoint 2
        // VDMA_RegisterISRCallback ( VDMA_DEV_USB2SRAM2, TRUE, VOTG_DMA_Rd2IntCallBack );
    } else if ( VOTG_GetUsbMode() == UOTGC_MODE_DEVICE_SERIAL ) {
#if (USB_SERIAL_HS_EN)
        VOTGC_CfgHighSpeed ( TRUE );
#else
        VOTGC_CfgHighSpeed ( FALSE );
#endif
        // 注册 EP2, USB to SRAM, 即DMA读中断处理函数， EP2作为buckout传输
        VDMA_RegisterISRCallback ( VDMA_DEV_USB2SRAM2, TRUE, VOTG_DMA_EP2_ReadCompleteCallBack );

        // 注册 EP1, SRAM to USB, 即DMA写中断处理函数， EP1作为buckin传输
       VDMA_RegisterISRCallback ( VDMA_DEV_SRAM2USB1, TRUE, VOTG_DMA_EP1_WriteCompleteCallBack );
        
        VUDC_Serial_Init();
    }
    VOTGC_CfgEpxMaxPktSize();
    //VOTG_RegisterLLevel2Isr(UOTGC_INT_ID_MC,  VOTG_Dev_Isr);
    VDMA_EnableInt();
    //enable DMA for ep0 setup token
    VOTG_DMA_StartRead ( VDMA_USB_SRAM0, ( UINT32 ) dmabuf, 64 );
    //*********************device init *****************

    VOTGC_EnControllerInt();
    VOTG_NPRINTF ( "USB device connect\r\n" );
    VOTGC_CfgConnect ( TRUE );

}

/******************************************************************************
Description:
    OTG host initial function.
    will init the OTG controller based on the host config
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



/******************************************************************************
Description:
    OTG initial function.
    will init the OTG controller based on the device config
    and start task to tell if it is working as  host or device
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

void VOTGC_Init ( void )
{
    VOTGC_CfgDect ( 1 ); //enable phy to detect host/device, clock gating part1
    VOTGC_CfgCG ( 1 ); //enable clock gating part2
    VOTGC_CfgPhySpdm ( 0 ); //normal mode

    VOTG_DisableInt();

#if (VDEV_TYPE == TYPE_FPGA )
    WRITE_REG ( REG_UOTG_UOTGC_SIM_MODE, UOTGC_SIMMODE_NEG_SEL );
    //WRITE_REG(REG_UOTG_UOTGC_SIM_MODE,UOTGC_SIMMODE_NEG_SEL);
#endif

#if (VDEV_TYPE == TYPE_ASIC)
#if (VOTG_HOST_SUPPORTED)
    VGPIO_SetLMuxFunction ( GPIO_MUX_FUNC_USB_VBUS, GPIO_MUX_FUNC_USB_VBUS );

    //VGPIO_SetMode(GPIO_MUX_FUNC_USB_VBUS, GPIO_MODE_OUTPUT, GPIO_INPUT_NORMAL);
    VGPIO_SetLMode ( GPIO_MUX_FUNC_USB_VBUS, GPIO_MODE_OUTPUT, GPIO_INPUT_NORMAL );
#endif
#endif
    __UOTGC_ClearSrcPending ( UOTGC_INT_MASK_ALL );
    __UOTGC_IntSetMask ( UOTGC_INT_MASK_ALL );

    //VOTG_RegisterLLevel2Isr(UOTGC_INT_ID_CHARGER, VOTG_ChargerIsr);
    VOTG_RegisterLLevel2Isr ( UOTGC_INT_ID_DRVVBUS,   VOTG_DriveBusIsr );
    VOTG_RegisterLLevel2Isr ( UOTGC_INT_ID_UNDRVVBUS, VOTG_UnDriveBusIsr );
    VOTG_RegisterLLevel2Isr ( UOTGC_INT_ID_BUSRESET,  VOTG_BusRstIsr );
}

/******************************************************************************
Description:
    OTG message queue initial function
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

void VOTG_MSG_Init ( void )
{
    //g_VOTG_MSGQueueID = VSYS_MSG_CreateQueue(VOTG_MSG_QUEUE_DEPTH,"Otg Queue");

}

/******************************************************************************
Description:
    OTG message send function
Parameters:
    MsgID - pre-defined message ID for USB, usually the action for the message
    p_Msg - pointer to the message structure, actual information
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/

VRET_VALUE VOTG_MSG_Send ( UINT32 MsgID, void *p_Msg )
{
    VRET_VALUE ret = 0;

    while ( g_otgMsgInfo.MsgInd ) {
        VOTG_EPRINTF ( "message processing.\r\n" );
    }

    if ( p_Msg ) {
        VCOMMON_MemCpy ( & ( g_otgMsgInfo.msgPkt ), p_Msg, sizeof ( VOTG_MSG ) );
    } else {
        VCOMMON_MemSet ( & ( g_otgMsgInfo.msgPkt ), 0, sizeof ( VOTG_MSG ) );
    }
    g_otgMsgInfo.msgId = MsgID;
    g_otgMsgInfo.MsgInd = 1;
    return ret;
}

/******************************************************************************
Description:
    OTG message get function
Parameters:
    p_MsgId - pre-defined message ID for USB, usually the action for the message
    pp_Msg - pointer to the message structure, actual information
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VOTG_MSG_Get ( UINT32 *p_MsgId, void **pp_Msg )
{
    VRET_VALUE ret = VRET_ERROR;

    if ( g_otgMsgInfo.MsgInd ) {
        *p_MsgId =  g_otgMsgInfo.msgId;
        *pp_Msg = ( void * ) ( & ( g_otgMsgInfo.msgPkt ) );
        ret = VRET_NO_ERROR;
    }
    return ret;
}

#pragma arm section code="VUSB_TASK"
/******************************************************************************
Description:
    OTG isr
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
void _VOTG_Interrupt_Routine_Service ( void )
{
    UINT32 i;
    UINT32 irq_id;
    UINT32 fnfh_reg =   READ_REG ( REG_UOTG_UOTGC_SRC_PND );

    for ( i   =   UOTGC_INT_NUMBER; i > 0; i-- ) {
        irq_id = i - 1;
        if ( fnfh_reg & ( 1 << irq_id ) ) {
            VOTG_ClearInt ( 1 << irq_id );
            if ( g_uotg_isr_callback[irq_id] !=    NULL ) {
                ( g_uotg_isr_callback[irq_id] ) ();
            }
        }
    }
}

/******************************************************************************
Description:
    OTG BSR
Parameters:
    p_Arg - argument for this BSR
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VOTG_Task ( void )
{
    VRET_VALUE ret;
    UINT32 msg_id;
    void *p_msg;

    {
        ret = VOTG_MSG_Get ( &msg_id, ( void ** ) &p_msg );
        if ( ret == VRET_NO_ERROR ) {
            VOTG_Dev_MSGHandler ( msg_id, p_msg );
            g_otgMsgInfo.MsgInd = 0;
        }
    }
}


#pragma arm section

/******************************************************************************
Description:
    used to set USB device function (audio/MSC device)
Parameters:
    funsel - 1 for audio, 0 for MSC device
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
BOOL VOTG_Function_Set ( BOOL funsel )
{
    VOTGC_CfgConnect ( FALSE ); //disconnect

    VCOMMON_Delay ( 0x4ff );

    if ( funsel == UOTGC_MODE_DEVICE_SERIAL_COM_FIX ) {
        VOTG_SetUsbMode ( UOTGC_MODE_DEVICE_SERIAL );
        VUDC_CfgDescSerialFixCom();
    } else {
        VOTG_SetUsbMode ( funsel );

        VUDC_CfgDesc();
    }

    VOTGC_Init();

    VOTGC_CfgConnect ( FALSE ); //disconnect
    //USB_audioDev = funsel;
    //get ready to re-connect
    //pUSBReConnectTimer = VSYS_TMR_Create(890, 1000, VOTGC_DevInit, NULL);
    //VSYS_TMR_Start(pUSBReConnectTimer); remove this timer task
    VCOMMON_Delay ( 0x4ff );
    VOTGC_DevInit();

    VCOMMON_MemSet ( &g_otgMsgInfo, 0, sizeof ( VOTG_MSG_INFO ) );

    return 0;
}

/******************************************************************************
Description:
    disconnect USB before system software reset.
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
void VOTG_SoftReset ( void )
{
    g_USBSerialReady = 0;
    VCOMMON_Delay ( 60000 );
    VOTGC_CfgConnect ( FALSE );
    VOTG_DMA_DisableWr ( VDMA_USB_SRAM1 );
}


/*--------------------------test code----------------------------------*/
void VUSB_PrintTest ( void );
void VOTG_Function_Test ( UINT32 mode )
{
#if 0
    extern void VSOL_USBModeSel ( UINT32 mode );
    VSOL_USBModeSel ( mode );

#endif

    switch ( mode ) {
#if (VAPP_TASK_OTG_EN > 0)
        case 0:
            VOTGC_HstInit();
            break;
        case 1: //usb devie store
            VOTG_Function_Set ( 1 );
            break;
        case 2: //usb device audio
            VOTG_Function_Set ( 2 );
            break;
        case 3: //usb device serial
            VOTG_Function_Set ( 3 );
            break;
        case 4:
            //VUDC_Serial_Data_In();
            //          VUSB_PrintTest();
            break;
#endif
    }

}


