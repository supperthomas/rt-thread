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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vsys_mem_blk.h"
#include "vstdcore.h"
#include "vstdio.h"
#include "vstdlib.h"
#include "vsys_mem_info.h"

const char *os_mem__c = "$Id: $";

#if VSYS_MEM_BLOCK_SUPPORT

#undef ASSERT
#define ASSERT(expr)                                                               \
	if (!(expr))                                                                   \
	{                                                                              \
		VSYSMEMBLK_DEBUG("[BLKMEM]", "(%d)%s %s", __LINE__, #expr, "failed!\r\n"); \
		while (1)                                                                  \
			;                                                                      \
	}

VMemBlk_Info *VMemBlk_Head = NULL;

// Formula for how many memory blocks to generate: blk_num = (size - sizeof(VMemBlk_Info))/(blk_size + sizeof(mem_blk_info_head_t))
VMemBlk_Info *VMemBlkCreate(UINT8 *p_addr, UINT16 size, UINT16 blk_size)
{
	VMemBlk_Info *p_blk = VMemBlk_Head, *p_front = NULL, *p_mem = (VMemBlk_Info *)p_addr;
	UINT8 *p_node;

	if (p_mem == NULL || blk_size == 0 || size <= sizeof(VMemBlk_Info))
	{
		return NULL;
	}
	if ((UINT32)p_mem & (sizeof(void *) - 1) || blk_size & (sizeof(void *) - 1))
	{
		return NULL;
	}
	p_mem->Freeoffset = 1;
#if VSYS_MEM_BLOCK_DEBUG_EN
	blk_size += sizeof(mem_blk_info_head_t);
#endif
	p_mem->BlkSize = blk_size;
	p_mem->NbrMax = (size - sizeof(VMemBlk_Info)) / blk_size;
	p_mem->NbrMax_inv = ~p_mem->NbrMax;
	p_node = p_addr + sizeof(VMemBlk_Info);
	for (UINT16 i = 0; i < p_mem->NbrMax - 1; i++)
	{
		((VMemBlk_B *)(p_node + i * blk_size))->offset = i + 2;
		((VMemBlk_B *)(p_node + i * blk_size))->off_inv = ~(i + 2);
	}
	((VMemBlk_B *)(p_node + (p_mem->NbrMax - 1) * blk_size))->offset = 0;
	((VMemBlk_B *)(p_node + (p_mem->NbrMax - 1) * blk_size))->off_inv = ~(0);

	for (; p_blk != NULL; p_blk = p_blk->BlkNext)
	{
		if (p_blk->BlkSize > p_mem->BlkSize)
		{
			p_mem->BlkNext = p_blk;
			if (p_front)
			{
				p_front->BlkNext = p_mem;
			}
			else
			{
				VMemBlk_Head = p_mem;
			}
			break;
		}
		p_front = p_blk;
	}
	if (p_blk == NULL)
	{
		p_mem->BlkNext = NULL;
		if (p_front)
		{
			p_front->BlkNext = p_mem;
		}
		else
		{
			VMemBlk_Head = p_mem;
		}
	}

	return VRET_NO_ERROR;
}

void *VMemBlkMalloc(UINT32 Size)
{
	UINT32 malloc_size = Size;
	VMemBlk_Info *p_mem = NULL;
	void *p_addr = NULL;
	VCPU_SR_ALLOC();
#if VSYS_MEM_BLOCK_DEBUG_EN
	malloc_size += sizeof(mem_blk_info_head_t);
#endif
	for (VMemBlk_Info *p_blk = VMemBlk_Head; p_blk != NULL; p_blk = p_blk->BlkNext)
	{
		if (p_blk->BlkSize >= malloc_size)
		{
			p_mem = p_blk;
			break;
		}
	}

	if (p_mem != NULL)
	{
		VCPU_CRITICAL_ENTER();
		if (p_mem->Freeoffset != 0)
		{
			p_addr = (p_mem->Freeoffset - 1) * p_mem->BlkSize + (UINT8 *)p_mem + sizeof(VMemBlk_Info);
			if(((VMemBlk_B *)p_addr)->offset > p_mem->NbrMax)
				p_addr = NULL;
			else
				p_mem->Freeoffset = ((VMemBlk_B *)p_addr)->offset;
		}
		VCPU_CRITICAL_EXIT();
#if VSYS_MEM_BLOCK_DEBUG_EN
		p_addr = VMemBlkOverMagic(p_addr, Size, p_mem->BlkSize, __GET_RETURN_ADDR());
#endif
	}
	return p_addr;
}

void VMemBlkFree(void *p_addr)
{
	VMemBlk_Info *p_mem;
	UINT8 *start_addr = NULL;
	VCPU_SR_ALLOC();

#if VSYS_MEM_BLOCK_DEBUG_EN
	p_addr = VSYS_MEM_BlockPtr(p_addr);
#endif
	p_mem = VMemBlkValidAddr(p_addr);
	ASSERT(p_addr && p_mem);
	if (p_addr == NULL || p_mem == NULL)
		return;
	start_addr = (UINT8 *)p_mem + sizeof(VMemBlk_Info);
	if (((UINT8 *)p_addr - start_addr) % p_mem->BlkSize)
		return;
#if VSYS_MEM_BLOCK_DEBUG_EN
	SINT32 ret = VMemBlkOverCheck(p_addr, p_mem->BlkSize);
	if (ret == VSYS_MEM_HEADBREAK)
	{
		VSYSMEMBLK_DEBUG("BlkMem0x%08X Head overflow!lr:0x%08X\r\n", p_addr, __GET_RETURN_ADDR());
		__VERRNO_STORE(VERRNO_MEM_CRASH);
	}
	else if (ret == VSYS_MEM_TAILBREAK)
	{
		VSYSMEMBLK_DEBUG("BlkMem0x%08X Tail overflow!lr:0x%08X\r\n", p_addr, __GET_RETURN_ADDR());
		__VERRNO_STORE(VERRNO_MEM_CRASH);
	}
#endif

	VCPU_CRITICAL_ENTER();
	((VMemBlk_B *)p_addr)->offset = p_mem->Freeoffset;
	((VMemBlk_B *)p_addr)->off_inv = ~p_mem->Freeoffset;
	p_mem->Freeoffset = ((UINT8 *)p_addr - start_addr) / p_mem->BlkSize + 1;
	VCPU_CRITICAL_EXIT();
}

VMemBlk_Info *VMemBlkValidAddr(void *p_Addr)
{
	VMemBlk_Info *p_blk = VMemBlk_Head, *p_mem = NULL;
	if (p_Addr == NULL)
		return NULL;
	for (; p_blk != NULL; p_blk = p_blk->BlkNext)
	{
		ASSERT(p_blk->NbrMax == (UINT16)~p_blk->NbrMax_inv);
		UINT8 *AddrPtr = (UINT8 *)p_blk + sizeof(VMemBlk_Info);
		if (AddrPtr <= (UINT8 *)p_Addr && ((UINT8 *)p_Addr - AddrPtr) < p_blk->NbrMax * p_blk->BlkSize)
		{
			p_mem = p_blk;
			break;
		}
	}
	return p_mem;
}

#if VSYS_MEM_BLOCK_DEBUG_EN
void *VMemBlkOverMagic(void *p_blk, UINT16 size, UINT16 blk_size, UINT32 LR)
{
	if (p_blk != NULL)
	{
		mem_blk_info_head_t *p_d = (mem_blk_info_head_t *)p_blk;
		#if VSYS_MEM_CHK_OWNER_LOG_EN
		p_d->owner = LR;
		#endif
		p_d->sz = size;
		p_d->magic = VSYS_MEM_CHK_OVERFLOW_MAGICCODE;
		p_blk = (UINT8 *)p_blk + sizeof(mem_blk_info_head_t);
		if (blk_size > (size + sizeof(mem_blk_info_head_t)))
		{
			UINT8 *data = (UINT8 *)p_blk + size;
			*data = 0xA5;
		}
	}
	return p_blk;
}

SINT32 VMemBlkOverCheck(void *p_blk, UINT16 blk_size)
{
	if (p_blk != NULL)
	{
		blk_size -= sizeof(mem_blk_info_head_t);
		mem_blk_info_head_t *p_d = (mem_blk_info_head_t *)p_blk;
		if (p_d->magic != VSYS_MEM_CHK_OVERFLOW_MAGICCODE || p_d->sz > blk_size)
		{
			return VSYS_MEM_HEADBREAK;
		}
		else if ((p_d->sz + sizeof(mem_blk_info_head_t)) < blk_size && *((UINT8 *)p_blk + sizeof(mem_blk_info_head_t) + p_d->sz) != 0xA5)
		{
			return VSYS_MEM_TAILBREAK;
		}
		else
		{
			return VSYS_MEM_OK;
		}
	}
	return VSYS_MEM_ERR;
}
#endif

UINT32 VMemBlkPoolSizeGet(UINT16 blk_num, UINT16 blk_size)
{
	UINT32 size = 0;
	if (blk_num && blk_size)
#if VSYS_MEM_BLOCK_DEBUG_EN
		size = blk_num * (STAMEM_ROUNDUP_SIZE(blk_size) + sizeof(mem_blk_info_head_t)) + sizeof(VMemBlk_Info);
#else
		size = blk_num * STAMEM_ROUNDUP_SIZE(blk_size) + sizeof(VMemBlk_Info);
#endif
	return size;
}

#endif
