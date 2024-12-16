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
    LCDC

Description:
     LCDC driver.

Revision History:
     Date                  Author                   Description
     2023.10.13            cxt                       initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vdrv_lcd.h"
#include "vdrv_gpio.h"
#include "vdrv_dma.h"
#include "vreg_def.h"

#undef ASSERT
#define ASSERT(expr) if (!(expr)) {while(1);}

static UINT8 vlcd_WRform = 0; //[0] CSN_FORM_SEL,[1:2] WRN_FORM_SEL, [3:4] RDN_FORM_SEL
static UINT8 vlcd_RDform = 0; //[0] CSN_FORM_SEL,[1:2] WRN_FORM_SEL, [3:4] RDN_FORM_SEL
static void VLCDC_Interface(UINT8 inf_select) {
    UINT8 inf_mode = 0;
    switch(inf_select) {
    case VLCD_INFMODE_6800_F1: // 6800 form1
        vlcd_WRform = VLCD_FORM_RDN_F|VLCD_FORM_WRN_L|VLCD_FORM_CSN_L;
        vlcd_RDform = VLCD_FORM_RDN_F|VLCD_FORM_WRN_H|VLCD_FORM_CSN_L;
        inf_mode = 3;
        break;
    case VLCD_INFMODE_6800_F2:  // 6800 form2
        vlcd_WRform = VLCD_FORM_RDN_H|VLCD_FORM_WRN_R|VLCD_FORM_CSN_R;
        vlcd_RDform = VLCD_FORM_RDN_R|VLCD_FORM_WRN_H|VLCD_FORM_CSN_R;
        inf_mode = 3;
        break;
    case VLCD_INFMODE_8080_F1:  // 8080 form1
        vlcd_WRform = VLCD_FORM_RDN_H|VLCD_FORM_WRN_R|VLCD_FORM_CSN_L;
        vlcd_RDform = VLCD_FORM_RDN_R|VLCD_FORM_WRN_H|VLCD_FORM_CSN_L;
        inf_mode = 2;
        break;
    case VLCD_INFMODE_8080_F2: // 8080 form2
        vlcd_WRform = VLCD_FORM_RDN_H|VLCD_FORM_WRN_R|VLCD_FORM_CSN_R;
        vlcd_RDform = VLCD_FORM_RDN_R|VLCD_FORM_WRN_H|VLCD_FORM_CSN_R;
        inf_mode = 2;
        break;
    case VLCD_INFMODE_3WIRE: // spi 3wire
        inf_mode = 1;
        break;
    case VLCD_INFMODE_4WIRE: // spi 4wire
        inf_mode = 0;
        break;
    }
    REG_SET_MEMBER(REG_LCD_LCDC_CTRL_INF_MODE,inf_mode);
}

void VLCD_Init(UINT8 inf_mode, UINT8 dma_order, UINT8 sdata_oreder, UINT8 sclk_pol, UINT8 clk_div)
{
	VGPIO_SetLMuxFunction ( GPIO_MUX_FUNC_LCDALL, GPIO_MUX_FUNC_LCDALL );
	VGPIO_SetLMux2Function ( GPIO_MUX_FUNC_LCDALL, 0 );
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CLKGAT,1);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_INF_MODE,inf_mode);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_DMA_ORDER,dma_order);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_SDATA_ORDER,sdata_oreder);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_SCLK_POL,sclk_pol);
	if(clk_div == 0){
		REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CLK96M,1);
	}else {
		REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CLK96M,0);
		REG_SET_MEMBER(REG_LCD_LCDC_CLK_DIV_CLK_DIV,clk_div);
	}
    VLCDC_Interface(inf_mode);
    REG_SET_MEMBER(REG_LCD_LCDC_CTRL_LCDC_ENABLE,1);
}

void VLCD_Uninit(void)
{
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_LCDC_ENABLE,0);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CLKGAT,0);
}

void VLCD_TSC_PARM(UINT8 tscyc, UINT8 tclw)
{
	REG_SET_MEMBER(REG_LCD_LCDC_TSCYC_TSCYC_PARM,tscyc);
	REG_SET_MEMBER(REG_LCD_LCDC_TSLW_TSLW_PARM,tclw);
}

void VLCD_TSD_PARM(UINT8 tsds, UINT8 tsdh)
{
	REG_SET_MEMBER(REG_LCD_LCDC_TSDS_TSDS_PARM,tsds);
	REG_SET_MEMBER(REG_LCD_LCDC_TSDH_TSDH_PARM,tsdh);
}

void VLCD_TCS_PARM(UINT8 tcss, UINT8 hold)
{
	REG_SET_MEMBER(REG_LCD_LCDC_TCSS_TCSS_PARM,tcss);
	REG_SET_MEMBER(REG_LCD_LCDC_HOLD_TIME_HOLD_TIME,hold);
}

void VLCD_TWR_PARM(UINT8 twrs, UINT8 twrlw, UINT8 twrcyc)
{
	REG_SET_MEMBER(REG_LCD_LCDC_TWRS_TWRS_PARM,twrs);
	REG_SET_MEMBER(REG_LCD_LCDC_TWRLW_TWRLW_PARM,twrlw);
	REG_SET_MEMBER(REG_LCD_LCDC_TWRCYC_TWRCYC_PARM,twrcyc);
}

void VLCD_TRD_PARM(UINT8 trds, UINT8 trdlw, UINT8 trdcyc)
{
	REG_SET_MEMBER(REG_LCD_LCDC_TRDS_TRDS_PARM,trds);
	REG_SET_MEMBER(REG_LCD_LCDC_TRDLW_TRDLW_PARM,trdlw);
	REG_SET_MEMBER(REG_LCD_LCDC_TRDCYC_TRDCYC_PARM,trdcyc);
}

void VLCD_Byte_Write(UINT8 data, BOOL iscmd)
{
	REG_SET_MEMBER(REG_LCD_LCDC_CMD_DATA_CMD_DATA,data);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_DMA_MODE,0);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_WRITE,1);
    SET_REG_BITS(REG_LCD_LCDC_CTRL,10,14,vlcd_WRform);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CMD_DAT_SEL,iscmd);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,1);
	while(REG_GET_MEMBER(REG_LCD_LCDC_STATUS_TRANS_DONE) == 0);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,0);
}

UINT8 VLCD_Byte_Read(BOOL iscmd)
{
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_DMA_MODE,0);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_WRITE,0);
	SET_REG_BITS(REG_LCD_LCDC_CTRL,10,14,vlcd_RDform);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CMD_DAT_SEL,iscmd);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,1);
	while(REG_GET_MEMBER(REG_LCD_LCDC_STATUS_TRANS_DONE) == 0);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,0);
	return REG_GET_MEMBER(REG_LCD_LCDC_STATUS_RD_DAT_STAT);
}

void VLCD_DMA_Write(UINT32 *data, UINT32 len, BOOL iscmd)
{
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_DMA_MODE,1);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_WRITE,1);
	SET_REG_BITS(REG_LCD_LCDC_CTRL,10,14,vlcd_WRform);
	REG_SET_MEMBER(REG_LCD_LCDC_CTRL_CMD_DAT_SEL,iscmd);
	REG_SET_MEMBER(REG_LCD_LCDC_LENGTH_TFCLKS_CINI,len);
	ASSERT(((UINT32)data&0x03) == 0);
	VDMA_LCD_ConfigWritePath((UINT32)data, len);
	VDMA_LCD_WriteEnable(TRUE);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,1);
	VDMA_LCD_WriteAction(VDMA_ACTION_START);
	while(REG_GET_MEMBER(REG_LCD_LCDC_STATUS_TRANS_DONE) == 0);
	REG_SET_MEMBER(REG_LCD_LCDC_TRANS_START_TRANS_START,0);
	VDMA_LCD_WriteEnable(FALSE);
}


