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
 	Debug relative function definitions.

Revision History:
 	Date                  Author                   Description
 	2009.04.15           chenjingen              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_MEM)
#include <stdio.h>

static unsigned int vdbg_trace_mem[4*1024];
static unsigned char *vdbg_p_trace = (unsigned char *)vdbg_trace_mem; 
/* stdio retarget to simulation port*/
int fputc(int ch, FILE *f) 
{ 
	if(vdbg_p_trace >= (unsigned char *)vdbg_trace_mem + sizeof(vdbg_trace_mem)) {
		vdbg_p_trace = (unsigned char *)vdbg_trace_mem;
		//return;
	}
	if((ch == '\r') || (ch == '\n')) {
		vdbg_p_trace += (16- ((unsigned int)vdbg_p_trace & 0xF));
	}
	else {
	    *vdbg_p_trace = ch&0xFF;
		 vdbg_p_trace ++;
	}

	return(ch);
}

int fgetc(FILE *f) 
{
	int ch = 0;
	
	return (ch);
}

void _ttywrch(int ch) 
{ 
    if((ch == '\r') || (ch == '\n')) {
		vdbg_p_trace += (16- ((unsigned int)vdbg_p_trace & 0xF));
	}
	else {
	    *vdbg_p_trace = ch&0xFF;
		 vdbg_p_trace ++;
	}
}

void vputs(char *str) {
	
    while(*str != '\0')
	{
		fputc(*str++, stdout);
	}
}

void VDBG_DrvInit(void) {
}
#endif /* VDBG_EN */

