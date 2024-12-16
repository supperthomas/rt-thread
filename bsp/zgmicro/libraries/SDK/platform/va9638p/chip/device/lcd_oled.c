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
    LCD

Description:
     LCD driver.

Revision History:
     Date                  Author                   Description
     2023.10.13            cxt                       initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vdrv_lcd.h"


void oled_Init(void)
{
//     VLCD_TSC_PARM(15, 10);
//     VLCD_TSD_PARM(2, 2);
//     VLCD_TCS_PARM(0, 2);
//     VLCD_TWR_PARM(2, 2, 30);
//     VLCD_TRD_PARM(2, 2, 30);
	VLCD_Init(VLCD_INFMODE_8080_F1, VLCD_DMAORDER_LSB, VLCD_DATAORDER_LSB, VLCD_SCLK_POL_P, 0);
}

void oled_demo(void)
{
     UINT32 buff[3] = {0x56789012,0x12345678};
	oled_Init();
     while(1){
          VLCD_Byte_Write(0x12,TRUE);
          VLCD_Byte_Write(0x34,FALSE);
          VLCD_Byte_Read(FALSE);
          VLCD_DMA_Write(buff, 6, FALSE);
     }
}

