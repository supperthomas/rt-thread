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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vcortex_m3.h"

/******************************************************************************
Description:
 	Puts the processor into sleep mode or deep-sleep mode.

Parameters:
 	Deep - Enable deep-sleep mode or not. (0: Disable, Others: Enable)
 	SleepOnExit - Enable sleep-on-exit or not.(0: Disable, Others: Enable)
	
Return Value:
	None.

Remarks:
	Can only be waked up by the interrupt with higher priority than BASEPRI.

Example:
 	None.

See Also:
	None.
*****************************************************************************/
void VNVIC_SYS_SleepNow(UINT8 Deep, UINT8 SleepOnExit)
{
	UINT32 val;

	val = READ_REG(REG_NVIC_SCR);
	if (Deep)
		val |= REG_BIT_MASK(RBIT_NVIC_SCR_SLEEPDEEP);
	else
		val &= ~(REG_BIT_MASK(RBIT_NVIC_SCR_SLEEPDEEP));
	
	
	if (SleepOnExit)
		val |= REG_BIT_MASK(RBIT_NVIC_SCR_SLEEPONEXIT);
	else
		val &= ~(REG_BIT_MASK(RBIT_NVIC_SCR_SLEEPONEXIT));
	
	WRITE_REG(REG_NVIC_SCR, val);

	// Wait for an interrupt.
    __WFI();
}

/******************************************************************************
Description:
 	Initiate a specified reset request to reset the MCU.

Parameters:
	RstType - Specified reset request type, can be one of following constants:
	          RBIT_NVIC_AIRCR_SYSRESETREQ, RBIT_NVIC_AIRCR_VECTRESET
	
Return Value:
	None.
*****************************************************************************/
void VNVIC_SYS_Reset(UINT8 RstType)
{
	UINT32 reg_value;

	reg_value  = READ_REG(REG_NVIC_AIRCR);	/* read old register configuration */
	reg_value &= ((UINT32)0x7 << 8); 		/* keep prigroup setting */
	VCOMMON_Delay ( 1000 );
	if (RstType == NVIC_RESET_SYSTEM /*chip reset*/) {	
		WRITE_REG(REG_NVIC_AIRCR, (reg_value | NVIC_AIRCR_VECTKEY | REG_BIT_MASK(RBIT_NVIC_AIRCR_SYSRESETREQ)));
	} else if (RstType == NVIC_RESET_VECT/*core reset*/) {
		WRITE_REG(REG_NVIC_AIRCR, (reg_value | NVIC_AIRCR_VECTKEY | REG_BIT_MASK(RBIT_NVIC_AIRCR_VECTRESET)));
		VCOMMON_Delay ( 10 );
	}

	/* insert data memory barrier */
	__DMB();
	
	while (1);
}

/******************************************************************************
Description:
 	Configures SYSTICK.

Parameters:
	ClkSrc - Specifies the clock source of SYSTICK, can be one of following constants:
	          NVIC_SYSTICK_SRC_STCLK, NVIC_SYSTICK_SRC_FCLK
	Reload - Specifies the reload value if counter enalbed
	
Return Value:
	None.
*****************************************************************************/
void VNVIC_SYS_ConfigSysTick(UINT8 ClkSrc, UINT32 Reload)
{
	UINT32 reg_value;

	reg_value  = READ_REG(REG_NVIC_ST_CTRL);

	/* clear related bits first */
	if (ClkSrc == NVIC_SYSTICK_SRC_STCLK) {
		reg_value &= ~ REG_BIT_MASK(RBIT_NVIC_ST_CTRL_CLKSOURCE);
	} else {
		reg_value |= REG_BIT_MASK(RBIT_NVIC_ST_CTRL_CLKSOURCE);
	}

	WRITE_REG(REG_NVIC_ST_CTRL, reg_value);
	
	WRITE_REG(REG_NVIC_ST_RELOAD, Reload);
}

/******************************************************************************
Description:
 	Enables SYSTICK counter and interrupt.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VNVIC_SYS_EnableSysTick(void)
{
	UINT32 reg_value;

	reg_value  = READ_REG(REG_NVIC_ST_CTRL);

	reg_value |= (REG_BIT_MASK(RBIT_NVIC_ST_CTRL_TICKINT) | REG_BIT_MASK(RBIT_NVIC_ST_CTRL_ENABLE));
	
	WRITE_REG(REG_NVIC_ST_CTRL, reg_value);
}

/******************************************************************************
Description:
 	Disables SYSTICK counter and interrupt.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VNVIC_SYS_DisableSysTick(void)
{
	UINT32 reg_value;

	reg_value  = READ_REG(REG_NVIC_ST_CTRL);

	reg_value &= ~ (REG_BIT_MASK(RBIT_NVIC_ST_CTRL_TICKINT) | REG_BIT_MASK(RBIT_NVIC_ST_CTRL_ENABLE));
	
	WRITE_REG(REG_NVIC_ST_CTRL, reg_value);

}


void VNVIC_SYS_PrintFaultStatus(void)
{
	UINT32 reg_val = READ_REG(REG_NVIC_FSR);
	VSYS_SOC_NPRINTF("\r\n\r\n -- Memory Manage Fault Status --");
	VSYS_SOC_NPRINTF("\r\n    Fault Flags: ");
	VSYS_SOC_NPRINTF("\r\n\t (%c) MMARVALID \t (%c) MSTKERR \t (%c) MUNSTKERR", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_MMARVALID)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_MSTKERR)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_MUNSTKERR)) ? '*' : ' ');
	VSYS_SOC_NPRINTF("\r\n\t (%c) DACCVIOL \t (%c) IACCVIOL", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_DACCVIOL)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_IACCVIOL)) ? '*' : ' ');

	VSYS_SOC_NPRINTF("\r\n    Fault Address: 0x%08X", READ_REG(REG_NVIC_MMAR));
	

	VSYS_SOC_NPRINTF("\r\n\r\n -- Bus Fault Status --");
	VSYS_SOC_NPRINTF("\r\n    Fault Flags: ");
	VSYS_SOC_NPRINTF("\r\n\t (%c) BFARVALID \t (%c) STKERR \t (%c) UNSTKERR", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_BFARVALID)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_STKERR)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_UNSTKERR)) ? '*' : ' ');
	VSYS_SOC_NPRINTF("\r\n\t (%c) IBUSERR \t (%c) PRECISERR \t (%c) IMPRECISERR", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_IBUSERR)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_PRECISERR)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_IMPRECISERR)) ? '*' : ' ');
	VSYS_SOC_NPRINTF("\r\n    Fault Address: 0x%08X", READ_REG(REG_NVIC_BFAR));
	
	VSYS_SOC_NPRINTF("\r\n\r\n -- Usage Fault Status --");
	VSYS_SOC_NPRINTF("\r\n    Fault Flags: ");
	VSYS_SOC_NPRINTF("\r\n\t (%c) DIVBYZERO \t (%c) UNALIGNED \t (%c) NOCP", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_DIVBYZERO)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_UNALIGNED)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_NOCP)) ? '*' : ' ');
	VSYS_SOC_NPRINTF("\r\n\t (%c) INVPC \t (%c) INVSTATE \t (%c) UNDEFINSTR", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_INVPC)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_INVSTATE)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_FSR_UNDEFINSTR)) ? '*' : ' ');


	reg_val = READ_REG(REG_NVIC_HFSR);
	VSYS_SOC_NPRINTF("\r\n\r\n -- Hard Fault Status --");
	VSYS_SOC_NPRINTF("\r\n    Fault Flags: ");
	VSYS_SOC_NPRINTF("\r\n\t (%c) DEBUGEVT \t (%c) FORCED \t (%c) VECTTBL", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_HFSR_DEBUGEVT)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_HFSR_FORCED)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_HFSR_VECTTBL)) ? '*' : ' ');


	reg_val = READ_REG(REG_NVIC_DFSR);
	VSYS_SOC_NPRINTF("\r\n\r\n -- Debug Fault Status --");
	VSYS_SOC_NPRINTF("\r\n    Fault Flags: ");
	VSYS_SOC_NPRINTF("\r\n\t (%c) EXTERNAL \t (%c) VCATCH \t (%c) DWTTRAP", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_DFSR_EXTERNAL)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_DFSR_VCATCH)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_DFSR_DWTTRAP)) ? '*' : ' ');
	VSYS_SOC_NPRINTF("\r\n\t (%c) BKPT \t (%c) HALTED", \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_DFSR_BKPT)) ? '*' : ' ', \
						(reg_val & REG_BIT_MASK(RBIT_NVIC_DFSR_HALTED)) ? '*' : ' ');
}

void VSYS_PrintFaultStatus_Direct(UINT32 return_addr, UINT32 *p_StkPtr)
{
	vdirect_printf ( "Fault return[0x%08X]\r\n", return_addr );
	vdirect_printf ( "p_StkPtr:0x%08X StkPtr_return:0x%08X\r\n",p_StkPtr,*((UINT32 *)p_StkPtr+5));
	UINT32 reg_val = READ_REG(REG_NVIC_FSR);
	vdirect_printf ( "REG_NVIC_FSR:0x%08X\r\n",reg_val);
	reg_val = READ_REG(REG_NVIC_HFSR);
	vdirect_printf ( "REG_NVIC_HFSR:0x%08X\r\n",reg_val);
	reg_val = READ_REG(REG_NVIC_DFSR);
	vdirect_printf ( "REG_NVIC_DFSR:0x%08X\r\n",reg_val);
	VNVIC_SYS_PrintFaultStatus();
}

