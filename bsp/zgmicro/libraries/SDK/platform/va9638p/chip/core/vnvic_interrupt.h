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
	NVIC INTERRUPT

Description:
 	NVIC interrupts configure and status functions.

Revision History:
 	Date                  Author                   Description
 	2009.04.23           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef _VNVIC_INTERRUPT_H_
#define _VNVIC_INTERRUPT_H_
	
#ifdef __cplusplus 
extern "C" {
#endif

#define NVIC_PRIO_BITS 					4 				/* NVIC Priority Bits */
#define NVIC_INTERRUPT_NUM				28
#define NVIC_VECT_TBL_BOUNDARY 			(32 * 4) 		/* Byte boundary of vector table, 
                                                         * should be next power of 2 larger 
                                                         * than number of interrupts  
                                                         */

/* Exception Numbers of Cortex-M3 System Fault */
#define FAULT_NMI               		2           	// Non Maskable Interrupt
#define FAULT_HARD              		3           	// Hard fault
#define FAULT_MEM               		4           	// Memory Management fault
#define FAULT_BUS               		5           	// Bus fault
#define FAULT_USAGE             		6           	// Usage fault
#define FAULT_SVCALL            		11          	// SVCall
#define FAULT_DEBUG             		12          	// Debug monitor
#define FAULT_PENDSV            		14          	// PendSV
#define FAULT_SYSTICK           		15          	// System Tick

/* Exception Numbers of IRQ */
/*#define IRQ_DUAL_BB						16
#define IRQ_MODEM						17
#define IRQ_UART						18
#define IRQ_I2C							19
#define IRQ_TIMER						20
#define IRQ_GPIO_IN						21
#define IRQ_DMA							22
#define IRQ_VA7633						23*/

#define NVIC_AIRCR_VECTKEY 				(0x5FA << 16) 	/* AIRCR Key for write access */

#define RBIT_NVIC_SHCSR_USGFAULTENA 		18
#define RBIT_NVIC_SHCSR_BUSFAULTENA 		17
#define RBIT_NVIC_SHCSR_MEMFAULTENA 		16
#define RBIT_NVIC_SHCSR_SYCALLPENDED 		15
#define RBIT_NVIC_SHCSR_BUSFAULTPENDED 		14
#define RBIT_NVIC_SHCSR_MEMFAULTPENDED 		13
#define RBIT_NVIC_SHCSR_USGFAULTPENDED 		12
#define RBIT_NVIC_SHCSR_SYSTICKACT 			11
#define RBIT_NVIC_SHCSR_PENDSVACT 			10
#define RBIT_NVIC_SHCSR_MONITORACT 			8
#define RBIT_NVIC_SHCSR_SVCALLACT 			7
#define RBIT_NVIC_SHCSR_USGFAULTACT 		3
#define RBIT_NVIC_SHCSR_BUSFAULTACT 		1
#define RBIT_NVIC_SHCSR_MEMFAULTACT 		0

#define RBIT_NVIC_ICSR_NMIPENDSET 			31
#define RBIT_NVIC_ICSR_PENDSVSET 			28
#define RBIT_NVIC_ICSR_PENDSVCLR 			27
#define RBIT_NVIC_ICSR_PENDSTSET 			26
#define RBIT_NVIC_ICSR_PENDSTCLR 			25
#define RBIT_NVIC_ICSR_ISRPREEMPT 			23
#define RBIT_NVIC_ICSR_ISRPENDING 			22
/* 21:12 VECTPENDING */
#define RBIT_NVIC_ICSR_RETTOBASE 			11
/* 9:0 VECTACTIVE */

/* bit definitions of register REG_NVIC_VTOR */
#define RBIT_NVIC_VTOR_TBLBASE 				29

void VNVIC_INT_SetPriorityGroup(UINT8 SplitBit);
UINT8 VNVIC_INT_GetPriorityGroup(void);
void VNVIC_INT_SetPriority(UINT16 Interrupt, UINT8 PreemptPrio, UINT8 SubPrio);
void VNVIC_INT_GetPriority(UINT16 Interrupt, UINT8 *p_PreemptPrio, UINT8 * p_SubPrio);
void VNVIC_INT_Enable(UINT16 Interrupt);
void VNVIC_INT_Disable(UINT16 Interrupt);
void VNVIC_INT_SetPending(UINT16 Interrupt);
void VNVIC_INT_ClearPending(UINT16 Interrupt);
BOOL VNVIC_INT_IsPending(UINT16 Interrupt);
VRET_VALUE VNVIC_INT_SetVectorTbl(UINT32 Addr);


#ifdef __cplusplus
}
#endif 

#endif  /* _VNVIC_INTERRUPT_H_ */

