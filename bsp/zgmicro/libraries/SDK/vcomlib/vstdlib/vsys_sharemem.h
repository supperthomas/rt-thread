/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    share mem

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSYS_SHAREMEM_H_
#define _VSYS_SHAREMEM_H_

#include "vstdlib.h"

#define SHAREMEM_PRIVATE		(0)
#define SHAREMEM_TOP_BIT		(BIT_MASK(CPU_ID_TOP))
#define SHAREMEM_BT_BIT			(BIT_MASK(CPU_ID_BT))
#define SHAREMEM_DSP_BIT		(BIT_MASK(CPU_ID_DSP))

void *VShareMemInit(SINT8 *name, UINT32 Size, UINT8 mode);
void *VShareMemGet(SINT8 *name, UINT32 *Size, UINT8 *mode);
const SINT8 *VShareMemGetName(void * addr);
UINT32 VShareMemGetSize(void * addr);
UINT32 VShareMemGetlimits(void * addr);
UINT32 VShareMemGetUse(void * addr);
#endif /* _VSYS_SHAREMEM_H_ */
