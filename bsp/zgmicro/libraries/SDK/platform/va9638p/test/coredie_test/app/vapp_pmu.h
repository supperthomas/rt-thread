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
	slave die

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#ifndef _VAPP_PMU_H_
#define _VAPP_PMU_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

	VRET_VALUE VPMU_WriteReg ( UINT8 Reg, UINT8 Value );
	VRET_VALUE VPMU_ReadReg ( UINT8 Reg, UINT8 *p_Value );
	void VPMU_SetLDO5En ( BOOL IsEnable );
	void VPMU_SetDCDCMode ( UINT8 Mode );
	void VPMU_SetDCOutputVoltage ( UINT8 Voltage );
	void VPMU_SetMCP ( UINT8 Mcp );
	void VSYS_PWR_Set(void);

	

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_PMU_H_ */
