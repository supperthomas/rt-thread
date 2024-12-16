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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vreg_def.h"
#include "vstdlib.h"
#include "vcortex_m3.h"

/******************************************************************************
Description:
 	Sets the Priority Grouping in NVIC Interrupt Controller.

Parameters:
 	SplitBit - Specified the bit (0 ~ 7) splits the priority.
	
Return Value:
	None.

Remarks:
	None.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_SetPriorityGroup(UINT8 SplitBit)
{
	UINT32 reg_value = 0;

	SplitBit &= 0x7;

	reg_value  = READ_REG(REG_NVIC_AIRCR); 		/* read old register configuration */
	reg_value &= ~((0xFFFFU << 16) | (0x0F << 8)); 	/* clear bits to change */
	reg_value  = ((reg_value | NVIC_AIRCR_VECTKEY | (SplitBit << 8))); /* Insert write key and priorty group */
	WRITE_REG(REG_NVIC_AIRCR, reg_value);
}

/******************************************************************************
Description:
 	Get the Priority Grouping from NVIC Interrupt Controller

Parameters:
 	None.
	
Return Value:
	Priority grouping field.

Remarks:
	None.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
UINT8 VNVIC_INT_GetPriorityGroup(void)
{
	return (UINT8)((READ_REG(REG_NVIC_AIRCR) >> 8) & (UINT32)0x07);
}


/******************************************************************************
Description:
 	Sets the priority for an interrupt

Parameters:
 	Interrupt - The number of interrupt.
 	PreemptPrio - The preemption priority for the interrupt.
 	SubPrio - The sub-priority for the interrupt.
	
Return Value:
	None.

Remarks:
	None.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_SetPriority(UINT16 Interrupt, UINT8 PreemptPrio, UINT8 SubPrio)
{
	UINT8 split_bit, preempt_nbits, sub_nbits, priority;
	UINT32 val;

	ASSERT((Interrupt < NVIC_INTERRUPT_NUM));
		
	split_bit = VNVIC_INT_GetPriorityGroup();

	preempt_nbits = (8 - (split_bit + 1));
	if (preempt_nbits > NVIC_PRIO_BITS)
		preempt_nbits = NVIC_PRIO_BITS;

	sub_nbits = NVIC_PRIO_BITS - preempt_nbits;

	if (PreemptPrio > ((1 << preempt_nbits) - 1)) 
		PreemptPrio = ((1 << preempt_nbits) - 1);
	
	if (SubPrio > ((1 << sub_nbits) - 1))
		SubPrio = ((1 << sub_nbits) - 1);

	priority = (PreemptPrio << (8 - preempt_nbits)) | (SubPrio << (8 - NVIC_PRIO_BITS));	
	
	switch (Interrupt) {
	case FAULT_MEM:
	case FAULT_BUS:
	case FAULT_USAGE:
	case FAULT_SVCALL:
	case FAULT_DEBUG:
	case FAULT_PENDSV:
	case FAULT_SYSTICK:
		val = READ_REG(REG_NVIC_SYS_PRI1 + ((Interrupt >> 2) << 2) - 4);
    	val &= ~((UINT32)0xFF << (8 * (Interrupt & 0x3)));
		val |= (priority << (8 * (Interrupt & 0x3)));
		WRITE_REG(REG_NVIC_SYS_PRI1 + ((Interrupt >> 2) << 2) - 4, val); /* system exception */
		break;
	default:
		val = READ_REG(REG_NVIC_PRI0 + ((Interrupt >> 2) << 2) - 16);
		val &= ~((UINT32)0xFF << (8 * (Interrupt & 0x3)));
		val |= (priority << (8 * (Interrupt & 0x3)));
		WRITE_REG(REG_NVIC_PRI0 + ((Interrupt >> 2) << 2) - 16, val); /* IRQ */
		break;
	}
}

/******************************************************************************
Description:
 	Reads the priority for an interrupt.

Parameters:
 	Interrupt - The number of interrupt.
	p_PreemptPrio - Where to store preemption priority.
	p_SubPrio - Where to store sub-priority.
	
Return Value:
	None.

Remarks:
	None.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_GetPriority(UINT16 Interrupt, UINT8 *p_PreemptPrio, UINT8 * p_SubPrio)
{
	UINT8 split_bit, preempt_nbits;
	UINT32 priority;

	ASSERT((Interrupt < NVIC_INTERRUPT_NUM));
	
	split_bit = VNVIC_INT_GetPriorityGroup();

	preempt_nbits = (8 - (split_bit + 1));
	if (preempt_nbits > NVIC_PRIO_BITS)
		preempt_nbits = NVIC_PRIO_BITS;
	
	switch (Interrupt) {
	case FAULT_MEM:
	case FAULT_BUS:
	case FAULT_USAGE:
	case FAULT_SVCALL:
	case FAULT_DEBUG:
	case FAULT_PENDSV:
	case FAULT_SYSTICK:  		
		//priority = READ_REG8(REG_NVIC_SYS_PRI1 + (Interrupt - 4));
		priority = (READ_REG(REG_NVIC_SYS_PRI1 + ((Interrupt >> 2) << 2) - 4) >> (8 * (Interrupt & 0x03))) & 0xFF;
		break;
	default:
		//priority = READ_REG8(REG_NVIC_PRI0 + (Interrupt - 16));
		priority = (READ_REG(REG_NVIC_PRI0 + ((Interrupt >> 2) << 2) - 16) >> (8 * (Interrupt & 0x03))) & 0xFF;
		break;
	}

	*p_PreemptPrio = priority >> (8 - preempt_nbits);
	*p_SubPrio = (priority & ((1 << (8 - preempt_nbits)) - 1)) >> (8 - NVIC_PRIO_BITS);
}

/******************************************************************************
Description:
 	Enables interrupt in NVIC interrupt controller.

Parameters:
 	Interrupt - The number of interrupt.
	
Return Value:
	None.

Remarks:
	NMI, Hard, SVCall, Debug Monitor, PendSV, SysTick can not be enabled 
	by this routine.
	
Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_Enable(UINT16 Interrupt)
{
	switch (Interrupt) {
	case FAULT_MEM:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_MEMFAULTENA));
		break;
	case FAULT_BUS:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_BUSFAULTENA));
		break;
	case FAULT_USAGE:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_USGFAULTENA));
		break;	
	case FAULT_SYSTICK:
		VNVIC_SYS_EnableSysTick();
		break;	
	default:
		if (Interrupt >= 16 && Interrupt < NVIC_INTERRUPT_NUM)
			WRITE_REG(REG_NVIC_SETENA0 + ((UINT32)(Interrupt - 16) >> 5), (UINT32)1 << ((Interrupt - 16) & 0x1F));
		break;
	}
}

/******************************************************************************
Description:
 	Disables interrupt in NVIC interrupt controller.

Parameters:
 	Interrupt - The number of interrupt.
	
Return Value:
	None.

Remarks:
	NMI, Hard, SVCall, PendSV, SysTick can not be disabled, 
	Debug Monitor should be controlled by debugger. 
	
Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_Disable(UINT16 Interrupt)
{
	switch (Interrupt) {
	case FAULT_MEM:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~(REG_BIT_MASK(RBIT_NVIC_SHCSR_MEMFAULTENA))));
		break;
	case FAULT_BUS:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~(REG_BIT_MASK(RBIT_NVIC_SHCSR_BUSFAULTENA))));
		break;
	case FAULT_USAGE:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~(REG_BIT_MASK(RBIT_NVIC_SHCSR_USGFAULTENA))));
		break;
	case FAULT_SYSTICK:
		VNVIC_SYS_DisableSysTick();
		break;	
	default:
		if (Interrupt >= 16 && Interrupt < NVIC_INTERRUPT_NUM)
			WRITE_REG(REG_NVIC_CLRENA0 + ((UINT32)(Interrupt - 16) >> 5), (UINT32)1 << ((Interrupt - 16) & 0x1F));
		break;
	}
}

/******************************************************************************
Description:
 	Sets the pending bit for an interrupt.

Parameters:
 	Interrupt - The number of interrupt.
	
Return Value:
	None.

Remarks:
	Only NMI, PendSV, SysTick, MemFault, BusFault, SVCall and IRQs can be pended.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_SetPending(UINT16 Interrupt)
{
	switch (Interrupt) {
	case FAULT_NMI:
		WRITE_REG(REG_NVIC_ICSR, READ_REG(REG_NVIC_ICSR) | REG_BIT_MASK(RBIT_NVIC_ICSR_NMIPENDSET));
		break;
	case FAULT_PENDSV:
		WRITE_REG(REG_NVIC_ICSR, READ_REG(REG_NVIC_ICSR) | REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSVSET));
		break;
	case FAULT_SYSTICK:
		WRITE_REG(REG_NVIC_ICSR, READ_REG(REG_NVIC_ICSR) | REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSTSET));
		break;
	case FAULT_MEM:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_MEMFAULTPENDED));
		break;
	case FAULT_BUS:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_BUSFAULTPENDED));
		break;
	case FAULT_SVCALL:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) | REG_BIT_MASK(RBIT_NVIC_SHCSR_SYCALLPENDED));
		break;
	default:
		/* IRQ only */
		if (Interrupt >= 16 && Interrupt < NVIC_INTERRUPT_NUM)
			WRITE_REG(REG_NVIC_SETPEND0 + ((UINT32)(Interrupt - 16) >> 5), (UINT32)1 << ((Interrupt - 16) & 0x1f));
		break;		
	}
	
}

/******************************************************************************
Description:
 	Clears the pending bit for an interrupt.

Parameters:
 	Interrupt - The number of interrupt.
	
Return Value:
	None.

Remarks:
	Only PendSV, SysTick, MemFault, BusFault, SVCall and IRQs can be unpended.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_INT_ClearPending(UINT16 Interrupt)
{
	switch (Interrupt) {
	case FAULT_PENDSV:
		WRITE_REG(REG_NVIC_ICSR, READ_REG(REG_NVIC_ICSR) | REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSVCLR));
		break;
	case FAULT_SYSTICK:
		WRITE_REG(REG_NVIC_ICSR, READ_REG(REG_NVIC_ICSR) | REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSTCLR));
		break;
	case FAULT_MEM:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~ REG_BIT_MASK(RBIT_NVIC_SHCSR_MEMFAULTPENDED)));
		break;
	case FAULT_BUS:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~ REG_BIT_MASK(RBIT_NVIC_SHCSR_BUSFAULTPENDED)));
		break;
	case FAULT_SVCALL:
		WRITE_REG(REG_NVIC_SHCSR, READ_REG(REG_NVIC_SHCSR) & (~ REG_BIT_MASK(RBIT_NVIC_SHCSR_SYCALLPENDED)));
		break;	
	default:
		/* IRQ only */
		if (Interrupt >= 16 && Interrupt < NVIC_INTERRUPT_NUM)
			WRITE_REG(REG_NVIC_CLRPEND0 + ((UINT32)(Interrupt - 16) >> 5), (UINT32)1 << ((Interrupt - 16) & 0x1f));
		break;
	}
}

/******************************************************************************
Description:
 	Checks specified interrupt pending status.

Parameters:
 	Interrupt - The number of interrupt.
	
Return Value:
	Returns TRUE if interrupt is pending, else returns FALSE.

Remarks:
	Can only read pending status of PendSV, SysTick, MemFault, BusFault, 
	SVCall and IRQs.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
BOOL VNVIC_INT_IsPending(UINT16 Interrupt)
{
	BOOL is_pending = FALSE;
	
	switch (Interrupt) {
	case FAULT_NMI:
		is_pending = (READ_REG(REG_NVIC_ICSR) & REG_BIT_MASK(RBIT_NVIC_ICSR_NMIPENDSET)) ? TRUE : FALSE;
		break;
	case FAULT_PENDSV:
		is_pending = (READ_REG(REG_NVIC_ICSR) & REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSVSET)) ? TRUE : FALSE;
		break;
	case FAULT_SYSTICK:
		is_pending = (READ_REG(REG_NVIC_ICSR) & REG_BIT_MASK(RBIT_NVIC_ICSR_PENDSTSET)) ? TRUE : FALSE;
		break;
	case FAULT_MEM:
		is_pending = (READ_REG(REG_NVIC_SHCSR) & REG_BIT_MASK(RBIT_NVIC_SHCSR_MEMFAULTPENDED)) ? TRUE : FALSE;
		break;
	case FAULT_BUS:
		is_pending = (READ_REG(REG_NVIC_SHCSR) & REG_BIT_MASK(RBIT_NVIC_SHCSR_BUSFAULTPENDED)) ? TRUE : FALSE;
		break;
	case FAULT_SVCALL:
		is_pending = (READ_REG(REG_NVIC_SHCSR) & REG_BIT_MASK(RBIT_NVIC_SHCSR_SYCALLPENDED)) ? TRUE : FALSE;
		break;
	default:
		/* IRQ only */
		if (Interrupt >= 16 && Interrupt < NVIC_INTERRUPT_NUM)
			is_pending = (READ_REG(REG_NVIC_SETPEND0 + ((UINT32)(Interrupt - 16) >> 5)) & (UINT32)1 << ((Interrupt - 16) & 0x1f)) ? TRUE : FALSE;
		break;		
	}
	return is_pending;
}


/******************************************************************************
Description:
 	Redirects exception vector to specified address in SRAM.

Parameters:
 	Addr - New address of vector table in SRAM.
	
Return Value:
	Returns VRET_NO_ERROR if succeed, else returns error code.

Remarks:
	Byte boundary of new vector table should be next power of 2 larger than 
	number of interrupts. e.g. if interrupts number is 29, the address should 
	be aligned with (2^5 * 4).

Example:
 	None.

See Also:
	None.
*****************************************************************************/
VRET_VALUE VNVIC_INT_SetVectorTbl(UINT32 Addr)
{
	UINT32 value; 

	if (Addr & ((UINT32)NVIC_VECT_TBL_BOUNDARY - 1)) {
		/* mis-aligned */
		return VRET_ERROR;
	}

	if (Addr >= VDEV_RAM_BASE) {
		value = (Addr) | REG_BIT_MASK(RBIT_NVIC_VTOR_TBLBASE);		
	} else {
 		value = (Addr) & (~ REG_BIT_MASK(RBIT_NVIC_VTOR_TBLBASE));
	}

	WRITE_REG(REG_NVIC_VTOR, value);

	return VRET_NO_ERROR;
}


