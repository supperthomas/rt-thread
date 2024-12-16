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
    TMR

Description:
    Timer driver.

Revision History:
    Date                  Author                   Description
    2009.07.08           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif

#ifndef _VTIMER_H_
#define _VTIMER_H_
	
#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TMR_VALUE_MAX                   0xFFFFFFFA  // value larger than it will be recognized as invalid

typedef enum tag_VTMR_SEL {
    TMR_FCLKL = 0,//1M
    TMR_FCLKS = 1,
    TMR_SCLK  = 2,//4k
    TMR_WDCLK = 3,
    TMR_FCLKH = 4,
    TMR_END
} VTMR_SEL;

typedef  void (*VDRV_TMR_CALLBACK)(VTMR_SEL );

void VTMR_Config ( VTMR_SEL TimerX, UINT32 Value, BOOL AutoReload );
void VTMR_Start ( VTMR_SEL TimerX );
void VTMR_Stop ( VTMR_SEL TimerX );
void VTMR_EnableInt ( VTMR_SEL TimerX );
void VTMR_DisableInt ( VTMR_SEL TimerX );
void VTMR_ClearInt ( VTMR_SEL TimerX );
UINT32 VTMR_GetCntValue ( VTMR_SEL TimerX );
BOOL VTMR_IsRunning ( VTMR_SEL TimerX );
BOOL VTMR_GetWorkStatus ( VTMR_SEL TimerX );
BOOL VTMR_GetReloadStatus ( VTMR_SEL TimerX );
void VTMR_IntHandler ( void );
UINT8 VTMR_GetInterruptStatus ( VTMR_SEL TimerX );
void VTMR_RegisterISRCallback(VTMR_SEL sel, VDRV_TMR_CALLBACK p_Callback);
void VTMR_CommDelay(UINT32 us);
UINT32 VTMR_CommTimeget(void);
void VTMR_BBTimeStart(void);
UINT64 VTMR_BBTimeget(void);//It can be used only after BB is enabled
UINT64 VTMR_BBTimediff(UINT64 start_us, UINT64 end_us);
void VTMR_BBDelay(UINT32 us);
#ifdef __cplusplus
}
#endif

#endif  /* _VTIMER_H_ */


