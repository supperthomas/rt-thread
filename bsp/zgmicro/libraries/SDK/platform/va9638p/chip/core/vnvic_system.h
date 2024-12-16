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
	NVIC SYSTEM

Description:
 	NVIC system control functions.

Revision History:
 	Date                  Author                   Description
 	2009.06.01           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef _VNVIC_SYSTEM_H_
#define _VNVIC_SYSTEM_H_

#include "vstdlib.h"
	
#ifdef __cplusplus 
extern "C" {
#endif
	
#define NVIC_RESET_SYSTEM 			1
#define NVIC_RESET_VECT 			2

#define NVIC_SYSTICK_SRC_STCLK 		0
#define NVIC_SYSTICK_SRC_FCLK 		1

/* Bit definitions of System Control Register in NVIC */
#define RBIT_NVIC_SCR_SEVONPEND 			4
#define RBIT_NVIC_SCR_SLEEPDEEP 			2
#define RBIT_NVIC_SCR_SLEEPONEXIT 			1

/* Bit definitions of Application Interrupt and Reset Control Register in NVIC */
#define RBIT_NVIC_AIRCR_SYSRESETREQ 		2
#define RBIT_NVIC_AIRCR_VECTRESET 			0

/* Bit definitions of SysTick Control and Status Register */
#define RBIT_NVIC_ST_CTRL_CLKSOURCE 		2
#define RBIT_NVIC_ST_CTRL_TICKINT 			1
#define RBIT_NVIC_ST_CTRL_ENABLE 			0


/* bit assignments of Configurable Fault Status Registers */
// [7:0], Memory Manage Fault Status Register
#define RBIT_NVIC_FSR_MMARVALID 	7
// [6:5], reserved
#define RBIT_NVIC_FSR_MSTKERR 		4
#define RBIT_NVIC_FSR_MUNSTKERR 	3
// [2], reserved
#define RBIT_NVIC_FSR_DACCVIOL 		1
#define RBIT_NVIC_FSR_IACCVIOL 		0

// [15:8], Bus Fault Status Register
#define RBIT_NVIC_FSR_BFARVALID 	15
// [14:13], reserved
#define RBIT_NVIC_FSR_STKERR 		12
#define RBIT_NVIC_FSR_UNSTKERR 		11
#define RBIT_NVIC_FSR_IMPRECISERR 	10
#define RBIT_NVIC_FSR_PRECISERR 	9
#define RBIT_NVIC_FSR_IBUSERR 		8

// [31:16], Usage Fault Status Register
// [31:26], reserved
#define RBIT_NVIC_FSR_DIVBYZERO 	25
#define RBIT_NVIC_FSR_UNALIGNED 	24
// [23:20], reserved
#define RBIT_NVIC_FSR_NOCP 			19
#define RBIT_NVIC_FSR_INVPC 		18
#define RBIT_NVIC_FSR_INVSTATE 		17
#define RBIT_NVIC_FSR_UNDEFINSTR 	16

/* bit assignments of Hard Fault Status Registers */
#define RBIT_NVIC_HFSR_DEBUGEVT 	31
#define RBIT_NVIC_HFSR_FORCED 		30
// [29:2], reserved
#define RBIT_NVIC_HFSR_VECTTBL 		1
// [0], reserved

/* bit assignments of Debug Fault Status Registers */
// [31:5], reserved
#define RBIT_NVIC_DFSR_EXTERNAL 	4
#define RBIT_NVIC_DFSR_VCATCH 		3
#define RBIT_NVIC_DFSR_DWTTRAP 		2
#define RBIT_NVIC_DFSR_BKPT 		1
#define RBIT_NVIC_DFSR_HALTED 		0


void VNVIC_SYS_SleepNow(UINT8 Deep, UINT8 SleepOnExit);
void VNVIC_SYS_Reset(UINT8 RstType);
void VNVIC_SYS_ConfigSysTick(UINT8 ClkSrc, UINT32 Reload);
void VNVIC_SYS_EnableSysTick(void);
void VNVIC_SYS_DisableSysTick(void);


#ifdef __cplusplus
}
#endif 

#endif  /* _VNVIC_SYSTEM_H_ */

