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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif

#ifndef _VLCDC_H_
#define _VLCDC_H_

#include "vstdlib.h"

#ifdef __cplusplus 
extern "C" {
#endif


#define VLCD_INFMODE_4WIRE    0
#define VLCD_INFMODE_3WIRE    1
#define VLCD_INFMODE_8080_F1     2
#define VLCD_INFMODE_8080_F2     3
#define VLCD_INFMODE_6800_F1     4
#define VLCD_INFMODE_6800_F2     5

#define VLCD_DMAORDER_LSB     0
#define VLCD_DMAORDER_MSB     1

#define VLCD_DATAORDER_LSB     0
#define VLCD_DATAORDER_MSB     1

#define VLCD_FORM_RDN_L     (0<<3)
#define VLCD_FORM_RDN_R     (1<<3)
#define VLCD_FORM_RDN_F     (2<<3)
#define VLCD_FORM_RDN_H     (3<<3)
#define VLCD_FORM_WRN_L     (0<<1)
#define VLCD_FORM_WRN_R     (1<<1)
#define VLCD_FORM_WRN_F     (2<<1)
#define VLCD_FORM_WRN_H     (3<<1)
#define VLCD_FORM_CSN_L     (0<<0)
#define VLCD_FORM_CSN_R     (1<<0)

#define VLCD_SCLK_POL_P     (0)
#define VLCD_SCLK_POL_N     (1)

void VLCD_Init(UINT8 inf_mode, UINT8 dma_order, UINT8 sdata_oreder, UINT8 sclk_pol, UINT8 clk_div);
void VLCD_Uninit(void);
void VLCD_TSC_PARM(UINT8 tscyc, UINT8 tclw);
void VLCD_TSD_PARM(UINT8 tsds, UINT8 tsdh);
void VLCD_TCS_PARM(UINT8 tcss, UINT8 hold);
void VLCD_TWR_PARM(UINT8 twrs, UINT8 twrlw, UINT8 twrcyc);
void VLCD_TRD_PARM(UINT8 trds, UINT8 trdlw, UINT8 trdcyc);
void VLCD_Byte_Write(UINT8 data, BOOL iscmd);
UINT8 VLCD_Byte_Read(BOOL iscmd);
void VLCD_DMA_Write(UINT32 *data, UINT32 len, BOOL iscmd);

#ifdef __cplusplus
}
#endif 

#endif  /* _VLCDC_H_ */


