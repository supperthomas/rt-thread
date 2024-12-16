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
#include "vstdos.h"
#include "vstddrv.h"
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_wd.h"

#define Dbg_PRINT        vcom_printf

VDRVWDTConfig_t wdt_config = {VDRVWDT_MODE_RESET,1000, NULL};

int main ( void )
{
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
	VSYS_CKM_Init();

    Dbg_PRINT("WDT test...\r\n");
	
	VSYS_WDT_Init(VDRVWDT_HW,&wdt_config);
	VSYS_WDT_Enable(VDRVWDT_HW);
	
	while(1)
	{
		vdelayus(500*1000);
		//VSYS_WDT_Feed(VDRVWDT_HW);
		Dbg_PRINT("feed...\r\n");
	}
	
    while(1);
}
