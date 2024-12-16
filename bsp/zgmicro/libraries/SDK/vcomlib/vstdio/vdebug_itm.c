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
#include "vpla_trace.h"
#include "vdrv_gpio.h"
#include "vdrv_ckm.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_ITM)

#define ITM_BASE            (0xE0000000UL)

#define ITM_Port8(n)    (*((volatile unsigned char *)(ITM_BASE+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(ITM_BASE+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(ITM_BASE+4*n)))
	
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

#define ITM_RXBUFFER_EMPTY 	0x5AA55AA5 /* value identifying ITM_RxBuffer is ready for next character */

/* stdio retarget to ITM */
volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY; /* variable to receive characters */

void SWO_Init(UINT8 select,UINT32 portBits, UINT32 cpuCoreFreqHz) {
	UINT32 reg_val;
	UINT32 SWOSpeed = 24000000; /* default 64k baud rate */
	UINT32 SWOPrescaler = (cpuCoreFreqHz/SWOSpeed) - 1 ;

	*((volatile unsigned long *)(0xE000EDFC)) =  (0X01 << 24);
	*((volatile unsigned *)(ITM_BASE + 0x400F0)) = 0x00000002;
	*((volatile unsigned *)(ITM_BASE + 0x40010)) = SWOPrescaler;          
	*((volatile unsigned *)(ITM_BASE + 0x00FB0)) = 0xC5ACCE55;
	if(select == VDBG_SWO_BT){
		*((volatile unsigned *)(ITM_BASE + 0x00E80)) = 0x0001000F; 
		*((volatile unsigned *)(ITM_BASE + 0x00E40)) = 0x00000000;
		*((volatile unsigned *)(ITM_BASE + 0x00E00)) = portBits;
		*((volatile unsigned *)(ITM_BASE + 0x01000)) = 0x40000001; 
		*((volatile unsigned *)(ITM_BASE + 0x40304)) = 0x00000100;
	}else if (select == VDBG_SWO_TOP){
		*((volatile unsigned *)(ITM_BASE + 0x00E80)) = 0x0001000D;
		*((volatile unsigned *)(ITM_BASE + 0x00E40)) = 0x00000000;
		*((volatile unsigned *)(ITM_BASE + 0x00E00)) = portBits;
		*((volatile unsigned *)(ITM_BASE + 0x01000)) = 0x400003FE;
		*((volatile unsigned *)(ITM_BASE + 0x40304)) = 0x00000100;
	}
	

}

__INLINE int ITM_ReceiveChar (void)
{
	int ch = -1;                               /* no character available */

	if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY) {
		ch = ITM_RxBuffer;
		ITM_RxBuffer = ITM_RXBUFFER_EMPTY;       /* ready for next character */
	}

	return (ch); 
}

__INLINE int ITM_CheckChar (void) 
{
	if (ITM_RxBuffer == ITM_RXBUFFER_EMPTY) {
		return (0);                                 /* no character available */
	} else {
		return (1);                                 /*    character available */
	}
}


int vfputc(int ch) 
{
	if (DEMCR & TRCENA) {
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
	return(ch);
}

int vfgetc(void) 
{
	int ch = -1;

	while (!ITM_CheckChar());
	ch = ITM_ReceiveChar();
	if (DEMCR & TRCENA) {
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
	return ch;
}

void _ttywrch(int ch) 
{
	if (DEMCR & TRCENA) {
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
}

void vputs(char *str) {
    while(*str != '\0')
    {
        vfputc(*str++);
    }
}

void VDBG_DrvInit(void) 
{

	#ifdef CORE_SEL_MCUTOP
	VGPIO_SetMuxFunction(GPIO_PIN_8, GPIO_PIN_8);
    VGPIO_SetMux2Function(GPIO_PIN_8, GPIO_PIN_8);
	SWO_Init(VDBG_SWO_TOP,0xFFFFFFFF, CKM_XCLK_48000000HZ);
	#else
	VSYS_DebugPort_Enable(TRUE);
	SWO_Init(VDBG_SWO_BT,0xFFFFFFFF, CKM_XCLK_48000000HZ);
	#endif

}
#endif /* VDBG_EN */

