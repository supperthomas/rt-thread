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
		io

Description:


Revision History:
		Date                  Author                   Description
		2023.03.13           cxt                      initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vstdlib.h"

#ifdef __VERRNO_SUPPORTED
#include "vstdcore.h"
#include "vstdos.h"

UINT32 isr_errinfo[IRQ_ALL_LIMIT] = {0};

UINT32 *__verrinfo_location(void)
{
	UINT32 *errinfo_p = &isr_errinfo[1];
	UINT32 isr_n = VCPU_INT_IsExec();
#if VOS_PLATFORM_SELECT_TYPE != VOS_PLATFORM_NONE
	if (isr_n == 0)
	{
		VOSTcb_t tcb = VOSTaskGetCur();
		if (tcb != NULL)
			errinfo_p = &tcb->tkerrinfo;
	}
	else
#endif
	{
		errinfo_p = &isr_errinfo[isr_n];
	}
	return errinfo_p;
}

#else

UINT32 __verrinfo = 0;
UINT32 *__verrinfo_location(void)
{
	return &__verrinfo;
}
#endif

UINT32 __verrinfo_store(UINT16 verrno, UINT16 line)
{
	return ((*__verrinfo_location()) = verrno | (line<<0x10));
}
