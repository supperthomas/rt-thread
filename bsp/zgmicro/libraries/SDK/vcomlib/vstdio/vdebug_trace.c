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
	DEBUG

Description:
 	Trace system.
 	Only support Segger RTT based System View logger tool chain

Revision History:
 	Date                  Author                   Description
 	2021.3.9              ming.m                   initial version 
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vstdlib.h"

#if (VDBG_TRACE_EN)
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_Wrap.h"


void VDBG_TraceInit(void) {
    static int init_done = 0;

    if(init_done > 0) return;

    VSYSVIEW_Conf();
    VSYSVIEW_Start();
    VSYSVIEW_MarkStart(0);
    VSYSVIEW_MarkStart(1);
    init_done ++;
}

void VDBG_TraceLog(const char *fmt, ...) {
	SEGGER_SYSVIEW_Print(fmt);
}

#else // #if (VDBG_TRACE_EN)
// Empty functions
void VDBG_TraceInit(void) {}
void VDBG_TraceLog(const char *fmt, ...) {}

#endif


