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
#include "vpla_ckm.h"
#include "vdrv_timer.h"
#include "vdrv_gpio.h"
#include "vreg_def.h"

#define Dbg_PRINT        vcom_printf

void VTMR_IntHandlertest ( VTMR_SEL timex )
{
    static UINT32 i = 0;

    if ( timex == TMR_FCLKL) {
        Dbg_PRINT("TMR_FCLKL:%d\r\n",i++);
    }
    if ( timex == TMR_FCLKS) {
        Dbg_PRINT("TMR_FCLKS:%d\r\n",i++);
    }
    if ( timex == TMR_SCLK) {
        Dbg_PRINT("TMR_SCLK:%d\r\n",i++);
    }
    if ( timex == TMR_FCLKH) {
        Dbg_PRINT("TMR_FCLKH:%d\r\n",i++);
    }
}

VTMR_SEL _TMR_TEST_SEL = TMR_SCLK;//TMR_FCLKL
int main ( void )
{
    int i  = 0 , j = 0, k = 0;
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
    VSYS_CKM_Init();

    Dbg_PRINT("timer test...\r\n");

    VTMR_Stop(_TMR_TEST_SEL);
    if(_TMR_TEST_SEL == TMR_SCLK){
        VTMR_Config ( _TMR_TEST_SEL, 4*1000, TRUE );
    }else{
        VTMR_Config ( _TMR_TEST_SEL, 1000*1000, TRUE );
    }
    VTMR_DisableInt ( _TMR_TEST_SEL );
    VTMR_EnableInt(_TMR_TEST_SEL);
    VTMR_RegisterISRCallback(_TMR_TEST_SEL,VTMR_IntHandlertest);
    VTMR_Start ( _TMR_TEST_SEL );
	
    
    while ( 1 ){
      vdelayus(1000*1000);
      Dbg_PRINT("mian:%d\r\n",i++);
    }
}
