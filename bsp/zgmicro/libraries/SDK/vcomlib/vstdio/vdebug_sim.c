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

#if (VDBG_EN > 0) &&  (VDBG_IO_RETARGET == IO_SIM)
#include <stdio.h>

#define PRNT_SCRN_FMT_C     0x41001000
#define PRNT_SCRN_FMT_FLUSH 0x41001014 

#define SETMEM32(reg,val)   (*((volatile int *) (reg)) = ((int) (val))) 


/* stdio retarget to simulation port*/
int fputc(int ch, FILE *f) 
{ 
    char tmp_ch;

	SETMEM32(PRNT_SCRN_FMT_C, ch); 

    tmp_ch = ch & 0xff;
    if (tmp_ch == '\n')  
        SETMEM32(PRNT_SCRN_FMT_FLUSH, 1);      

	return(ch);
}

int fgetc(FILE *f) 
{
	int ch = 0;
	
	return (ch);
}

void _ttywrch(int ch) 
{ 
    char tmp_ch;

    tmp_ch = ch & 0xff;
    if (tmp_ch == '\n')  
        SETMEM32(PRNT_SCRN_FMT_FLUSH, 1);
    else 
        SETMEM32(PRNT_SCRN_FMT_C, ch); 
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

