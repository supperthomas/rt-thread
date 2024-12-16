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
	FPB

Description:
 	Flash patching relative function definitions.

Revision History:
 	Date                  Author                   Description
 	2009.10.21           chenjingen              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vreg_def.h"
#include "vcortex_m3.h"

void VFPB_EnableFlashPatch(void)
{
	UINT32 reg_val;

	reg_val = READ_REG(REG_FPB_CTRL);
	reg_val |= (FPB_CTRL_WRITE_KEY | 0x1);
	WRITE_REG(REG_FPB_CTRL, reg_val);
	/* insert data and instruction barrier */	
	__DMB();
	__ISB();
}

void VFPB_DisableFlashPatch(void)
{
	UINT32 reg_val;

	reg_val = READ_REG(REG_FPB_CTRL);
	reg_val |= FPB_CTRL_WRITE_KEY;
	reg_val &= ~ REG_BIT_MASK(0x0);
	WRITE_REG(REG_FPB_CTRL, reg_val);
	/* insert data and instruction barrier */	
	__DMB();
	__ISB();
}


void VFPB_SetRemapBase(UINT32 BaseAddr)
{
	ASSERT(IS_VALID_REMAP_ADDR(BaseAddr));
#if (FPB_SLOTS_EXTENED == 24)
	WRITE_REG(REG_FPB_REMAP, ((BaseAddr >> 7) << 7));
#else
	WRITE_REG(REG_FPB_REMAP, ((BaseAddr >> 5) << 5));
#endif
}

void VFPB_EnableComparator(UINT32 CompAddr, VFPB_COMP CompX)
{
	UINT32 reg_val;

	ASSERT(IS_VALID_COMP_ADDR(CompAddr));
	reg_val = ((CompAddr << 2) >> 2);
	reg_val |= 0x1; /* enable the comparator */
	WRITE_REG((REG_FPB_COMP0 + CompX * 4), reg_val);
	/* insert data and instruction barrier */	
	__DMB();
	__ISB();
}

void VFPB_DisableComparator(VFPB_COMP CompX)
{
	WRITE_REG((REG_FPB_COMP0 + CompX * 4), READ_REG(REG_FPB_COMP0 + CompX * 4) & ~(UINT32)0x1);
	/* insert data and instruction barrier */	
	__DMB();
	__ISB();
}



