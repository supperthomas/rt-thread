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
    SYSTMR

Description:
     System timer.

Revision History:
     Date                  Author                   Description
     2009.07.21           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSYS_TIMER_H_
#define _VSYS_TIMER_H_

#ifdef __cplusplus 
extern "C" {
#endif

#include "vstdlib.h"

#define VSYS_TMR_PROFILE_EN     0 /* enable for system timer profiling */

#define VSYS_TMR_MAGIC_ID         (0x53544D52UL)     /* 'S', 'T', 'M', 'R' */

#define VSYS_TMR_MAX_TIME         (200000 * 100)   /* max. time is 20000s */  

#define VSYS_TMR_STATE_STOPPED 0
#define VSYS_TMR_STATE_RUNNING 1

typedef  void (*VSYS_TMR_CALLBACK)(void *p_Tmr, void *p_Arg);

typedef struct tag_VSYS_TMR_DATA {
    UINT8 state;
    UINT32 magic_id;
    VSYS_TMR_CALLBACK pfn_callback;
    void *p_callback_args;
    void *p_next;
    void *p_prev;
    UINT32 time_out;
    UINT32 initial;
    UINT32 period;
#if (VSYS_TMR_PROFILE_EN > 0)
    SINT32 creator;
    UINT32 expired_count;
#endif
}VSYS_TMR_DATA;

void VSYS_TMR_HwTmrSel( UINT8 sel);
void VSYS_TMR_Init(void);
VSYS_TMR_DATA *VSYS_TMR_Create(UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs);
void VSYS_TMR_Delete(VSYS_TMR_DATA *p_SysTmr);
UINT32 VSYS_TMR_Create_Static(VSYS_TMR_DATA *p_SysTmr, UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs);
void VSYS_TMR_Delete_Static(VSYS_TMR_DATA *p_SysTmr);
void VSYS_TMR_ChangeSetting(VSYS_TMR_DATA *p_SysTmr, UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs);
void VSYS_TMR_Start(VSYS_TMR_DATA *p_SysTmr);
void VSYS_TMR_Stop(VSYS_TMR_DATA *p_SysTmr);
void VSYS_TMR_SendExpireSignal(void);
void VSYS_TMR_Check(void);
void VSYS_TMR_ShortenRunningTimers(UINT32 MicroSeconds, UINT16 StartPos);
BOOL VSYS_TMR_IsListHeader(VSYS_TMR_DATA* p_Tmr); 
UINT32 VSYS_TMR_GetElapsedTime(void);
UINT32 VSYS_TMR_GetRemainderTime(VSYS_TMR_DATA* p_Tmr);
void VSYS_TMR_Synchronization(UINT32 MicroSeconds);


#ifdef FPGA_QA_BUILD
#if (VSYS_TMR_PROFILE_EN > 0)
#error "vsys_timer.h, VSYS_TMR_PROFILE_EN should be set to 0 for FPGA QA Build!"
#endif
#endif

#ifdef RELEASE_BUILD
#if (VSYS_TMR_PROFILE_EN > 0)
#error "vsys_timer.h, VSYS_TMR_PROFILE_EN should be set to 0 for Release Build!"
#endif
#endif

#ifdef __cplusplus
}
#endif 

#endif  /* _VSYS_TIMER_H_ */

