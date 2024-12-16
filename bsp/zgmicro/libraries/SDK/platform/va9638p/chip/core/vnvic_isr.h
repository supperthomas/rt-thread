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
	ISR

Description:
 	Handlers of CM3 exceptions and peripheral interrupts.

Revision History:
 	Date                  Author                   Description
 	2009.10.22           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef _VISR_H_
#define _VISR_H_

#ifdef __cplusplus 
extern "C" {
#endif


typedef void(* const VISR_FUNCPTR)(void) __irq;

__irq void VISR_NMI(void);
__irq void VISR_Fault(void);
__irq void VISR_HardFault(void);
__irq void VISR_MMUFault(void);
__irq void VISR_BusFault(void);
__irq void VISR_UsageFault(void);
__irq void VISR_SVCall(void);
__irq void VISR_DebugMon(void);
__irq void VISR_BB(void);
__irq void VISR_Modem(void);
__irq void VISR_Uart(void);
__irq void VISR_I2c(void);
__irq void VISR_Timer(void);
__irq void VISR_GPIO(void);
__irq void VISR_DMA(void);
__irq void VISR_VA76XX(void);
__irq void VISR_IPC(void);
__irq void VISR_Mbox(void);
__irq void VISR_Usb(void);
__irq void VISR_SDIO(void);



#ifdef __cplusplus
}
#endif 

#endif  /* _VISR_H_ */

