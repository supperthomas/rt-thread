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
#include "vcoroutine.h"
#include "vapp_die.h"
#include "vapp_host.h"
#include "vapp_slave.h"

#undef VAPP_MAIN_DEBUG
#define VAPP_MAIN_DEBUG(...) DEBUG_PRINTF("[VAPP_MAIN]", __VA_ARGS__)

int main(void)
{
	UINT8 mode = 0;

	extern void VI2C_EnableI2CClk(void);
	VI2C_EnableI2CClk();
	
	VSYS_SYSVIEW_MarkInit(0,"sys");
	VSYS_SYSVIEW_MarkInit(1,"dspin");
	VSYS_SYSVIEW_MarkInit(2,"iisin_isr");
	VSYS_SYSVIEW_MarkInit(3,"iisout_updata");
	VSYS_SYSVIEW_MarkInit(4,"iisout_isr");

	VSYS_SYSVIEW_MarkStart(0);
	
	VCPU_CoreInit();
	VSYS_CKM_Init();
	VSYS_FLASH_Init();
	VCPU_INT_Enable();
	VAPP_MAIN_DEBUG(" 7638Xiisdsp SDK\r\n");

	Vapp_SlaveDie_Init();
	Vapp_SlaveDie_Set_state(0x01);
	
	//mode = Vapp_SlaveDie_Get_mode();
	mode = 2;
	VAPP_MAIN_DEBUG(" Vapp_SlaveDie mode %d\r\n", mode);

	Vapp_Host_Init();
	VAPP_MAIN_DEBUG(" 7638Xiisdsp SDK start...\r\n");
	
	switch(mode)
	{
		case 1:
			Vapp_Host_9647X_Demo();
			break;
		case 2:
			Vapp_Host_softin_Demo();
			break;
		case 3:
			Vapp_Host_9647X();
			break;
		case 4:
			Vapp_Host_9647Y_Demo();
			break;
		case 5:
			Vapp_Host_9647Y_Demo();
			break;
		case 6:
			Vapp_Host_9657_Demo();
			break;
		case 7:
			Vapp_Host_9648_Demo();
			break;
		case 8:
			Vapp_Host_test_Demo();
			break;
		default:
			break;
	}

	VSYS_SYSVIEW_MarkStop(0);
	
	for (VCORTimeCycles(VSYS_CKM_GetParam(VSYS_SysClockFreq), 0xFFFFFFFF);;)
	{
		VCORScheduler();
	}
	return 0;
}
