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
		Memory Block

Description:
		Memory Block.

Revision History:
		Date                  Author                   Description
		2023.10.23            cxt                       initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSYS_MEMBLK_H_
#define _VSYS_MEMBLK_H_
#include "vstdlib.h"
#include "vstdio.h"
#include "vsys_mem_info.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define VSYS_MEM_BLOCK_DEBUG_EN VSYS_MEM_CHK_OVERFLOW_EN

#define VSYSMEMBLK_DEBUG VCOMMON_EPRINTF
#define STAMEM_ALIGN (sizeof(void *) - 1)
#define STAMEM_ROUNDUP_SIZE(_r) (((_r) + STAMEM_ALIGN) & ~STAMEM_ALIGN)
	typedef struct vmemblk_info
	{
		struct vmemblk_info *BlkNext;
		UINT16 Freeoffset;
		UINT16 BlkSize;
		UINT16 NbrMax;
		UINT16 NbrMax_inv;
	} VMemBlk_Info;

	typedef struct
	{
		UINT16 offset;
		UINT16 off_inv;
	} VMemBlk_B;

	VMemBlk_Info *VMemBlkCreate(UINT8 *p_addr, UINT16 size, UINT16 blk_size);
	void *VMemBlkMalloc(UINT32 Size);
	void VMemBlkFree(void *p_Addr);
	VMemBlk_Info *VMemBlkValidAddr(void *p_Addr);
	void *VMemBlkOverMagic(void *p_blk, UINT16 size, UINT16 blk_size, UINT32 LR);
	SINT32 VMemBlkOverCheck(void *p_blk, UINT16 blk_size);
	UINT32 VMemBlkPoolSizeGet(UINT16 blk_num, UINT16 blk_size);

#ifdef __cplusplus
}
#endif

#endif /* _VSYS_MEMBLK_H_ */
