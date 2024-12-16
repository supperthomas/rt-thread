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

#define Dbg_PRINT        vcom_printf



#include "SEGGER_RTT.h"


void VRTT_Init(void) 
{	
	static UINT8 workon = 0;
	if(workon==0)
	{
	  SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);


		SEGGER_RTT_TerminalOut(0,RTT_CTRL_RESET);
		SEGGER_RTT_TerminalOut(0,RTT_CTRL_CLEAR);
		SEGGER_RTT_TerminalOut(0,RTT_CTRL_TEXT_WHITE);

		vputs("vdbg init\r\n");
		workon = 1;
	}
}


int main ( void )
{
	char string[1024] = "",time = 0;
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
	VSYS_CKM_Init();
	
    Dbg_PRINT("RTT test...\r\n");
	
    VRTT_Init();
	while('q' != SEGGER_RTT_GetKey()){
		vcom_sprintf((char*)string,"rtt time:%d\r\n",time++);
		SEGGER_RTT_WriteString(0,string);
		vdelayus(1000*1000);
	}
	Dbg_PRINT("RTT quit...\r\n");
	while(1);
}
