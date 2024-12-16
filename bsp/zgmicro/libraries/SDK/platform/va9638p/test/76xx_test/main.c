/*------------------------------------------------------------------------------
					COPYRIGHT (C) 2017, Vimicro Corporation
							  ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
	main

Description:
	test

Revision History:
	Date                  Author                   Description
	2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstddrv.h"

#define Dbg_PRINT vcom_printf

volatile BOOL g_VA76xx_Flag = FALSE;
volatile UINT8 g_VA76xx_Reg = 0;
__IRQ void VISR_VA76xx(void)
{
	UINT8 reg = 0;
	
	g_VA76xx_Flag = TRUE;
	UINT32 tmr_cnt = VSYS_TIMER_GetStatus(VSYS_TMR_FCLKL_us,VSYS_TMR_CntValue_Status);
	VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0xC0,&reg,1);
	VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,0x34,0xC1,&reg,1);
	tmr_cnt -= VSYS_TIMER_GetStatus(VSYS_TMR_FCLKL_us,VSYS_TMR_CntValue_Status);
	g_VA76xx_Reg = reg;
	Dbg_PRINT("VISR 76xx (%02x) time:%dus\r\n",g_VA76xx_Reg, tmr_cnt);
}

volatile BOOL g_check_Flag = FALSE;
void cyclic_check(VDRVTMRSel_t sel)
{
	g_check_Flag = TRUE;
}

int main(void)
{
	UINT8 reg_buf[3] = {0,0,0xFB};
	VCPU_CoreInit();
	VSYS_CKM_Init();
	VCPU_INT_Enable();
	Dbg_PRINT(" 76xx INT test\r\n");
	
	VDRVTMRConfig_t tmr_cfg = {0};
	tmr_cfg.AutoReload = TRUE;
	tmr_cfg.EnableInt = TRUE;
	tmr_cfg.Period = 1000*1000*3;
	tmr_cfg.TMRCallback = cyclic_check;
	VSYS_TIMER_Init(VSYS_TMR_FCLKL_us,&tmr_cfg);
	VSYS_TIMER_Start(VSYS_TMR_FCLKL_us);
	
	VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
	VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);
	VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,0x34,0xC0,reg_buf,3);
	
	VCPU_INT_SrcEnable(IRQ_VA76XX_INT1);

	while (1)
	{
		if (g_VA76xx_Flag)
		{
			g_VA76xx_Flag = FALSE;
			Dbg_PRINT("VISR 76xx reg:\r\n\tCDC_LINE_DET (%d)\ttermb_inv (%d)\tStat (%d)\r\n\tPg (%d)\t\tBAT_Low (%d)\r\n",
					g_VA76xx_Reg&0x01,g_VA76xx_Reg>>1&0x01,g_VA76xx_Reg>>2&0x01,g_VA76xx_Reg>>3&0x01,g_VA76xx_Reg>>4&0x01);
		}
		if (g_check_Flag)
		{
			g_check_Flag = FALSE;
			VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0xC0,reg_buf,sizeof(reg_buf));
			Dbg_PRINT("timer 76xx C0(%02x) C1(%02x) C2(%02x)\r\n",reg_buf[0],reg_buf[1],reg_buf[2]);
		}
	}
}
