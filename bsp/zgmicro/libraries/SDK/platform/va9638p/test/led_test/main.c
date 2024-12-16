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

int main ( void )
{
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
	VSYS_CKM_Init();

    Dbg_PRINT("LED test...\r\n");
	

    VDRVLEDConfig_t led_config = {0};
    led_config.on_length =  125;
    led_config.off_length =  250;
    led_config.onoff_times =  3;
    led_config.onoff_level =  TRUE;
    led_config.intvl_length =  250;
    led_config.repeat_times =  5;
    led_config.intvl_state =  FALSE;

    VSYS_LED_Init(VSYS_LED_INDEX_0);
	VSYS_LED_Init(VSYS_LED_INDEX_1);
	VSYS_LED_Init(VSYS_LED_INDEX_2);
	VSYS_LED_Init(VSYS_LED_INDEX_3);

    VSYS_LED_SetMode(VSYS_LED_INDEX_0, &led_config);
	VSYS_LED_SetMode(VSYS_LED_INDEX_1, &led_config);
	VSYS_LED_SetMode(VSYS_LED_INDEX_2, &led_config);
	VSYS_LED_SetMode(VSYS_LED_INDEX_3, &led_config);
    while(1);
}
