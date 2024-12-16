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
		Memory Management

Description:
		Memory management functions.

Revision History:
		Date                  Author                   Description
		2009.05.08           chenjingen              initial version
		2015.09.29                       fangzhenwei                     update os  API
		2022.11.17                              cxt                                             Rearrange
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vsys_mem.h"
#include "vsys_mem_tlsf.h"
#include "vsys_mem_info.h"
#include "vstdcore.h"
#include "vsys_mem_blk.h"
#include "SEGGER_SYSVIEW_Wrap.h"

VSYS_MEM_CONFIG g_VSYS_MEM_Config;
static UINT32 g_VSYS_MEM_Pool[VSYS_MEM_POOL_SIZE / 4];
#if VSYS_MEM_POOL_SIZE_NEW > 4
//static UINT32 g_VSYS_MEM_Pool_NEW[VSYS_MEM_POOL_SIZE_NEW / 4] __attribute__((at(0x20013A1c), zero_init));;
static UINT32 *g_VSYS_MEM_Pool_NEW = NULL;
#endif

UINT8 *VSYS_MEM_Init(void)
{
	static UINT8 *p_mem_pool = NULL;
	int ret = 0;
	if (p_mem_pool == NULL)
	{
		p_mem_pool = (UINT8 *)&g_VSYS_MEM_Pool[0];
		destroy_memory_pool(p_mem_pool);
		ret = tlsf_init(sizeof(g_VSYS_MEM_Pool), p_mem_pool);
		if (ret < 0)
		{
			VSYS_MEM_CPRINTF("Init fail,Addr:0x%08X;size=%d\r\n", p_mem_pool, sizeof(g_VSYS_MEM_Pool));
			return NULL;
		}
#if (VSYS_MEM_POOL_SIZE_NEW > 0)
		if (g_VSYS_MEM_Pool_NEW == NULL)
			g_VSYS_MEM_Pool_NEW = VShareMemInit((SINT8*)"memp",VSYS_MEM_POOL_SIZE_NEW,SHAREMEM_PRIVATE);
		if (g_VSYS_MEM_Pool_NEW != NULL)
			add_new_area(g_VSYS_MEM_Pool_NEW, VSYS_MEM_POOL_SIZE_NEW, p_mem_pool);
#endif
#if (VSYS_MEM_BLOCK_SUPPORT > 0)
		UINT32 blk_4 = VMemBlkPoolSizeGet(VSYS_STATIC_4BLOCK_NUMBER, 4);
		UINT32 blk_8 = VMemBlkPoolSizeGet(VSYS_STATIC_8BLOCK_NUMBER, 8);
		UINT32 blk_12 = VMemBlkPoolSizeGet(VSYS_STATIC_12BLOCK_NUMBER, 12);
		UINT32 blk_16 = VMemBlkPoolSizeGet(VSYS_STATIC_16BLOCK_NUMBER, 16);
		UINT8 *blk_addr = vmalloc(blk_4+blk_8+blk_12+blk_16);
		if (blk_addr != NULL){
			VSYS_MEM_SetType(blk_addr,1);
			VMemBlkCreate(blk_addr, blk_4, 4);
			blk_addr += blk_4;
			VMemBlkCreate(blk_addr, blk_8, 8);
			blk_addr += blk_8;
			VMemBlkCreate(blk_addr, blk_12, 12);
			blk_addr += blk_12;
			VMemBlkCreate(blk_addr, blk_16, 16);
		}
#endif
		SYSVIEW_HeapDefine(p_mem_pool, p_mem_pool, sizeof(g_VSYS_MEM_Pool) + VSYS_MEM_POOL_SIZE_NEW, 20);
	}
	return p_mem_pool;
}

SINT32 VSYS_MEM_CheckAddr(void *p_Addr)
{
	UINT8 *p_mem_pool = VSYS_MEM_Init();
	if (tlsf_is_addr_valid(p_mem_pool, p_Addr))
	{
		return VSYS_MEM_OK;
	}

	return VSYS_MEM_ERR;
}

void *vmalloc(UINT32 Size)
{
	void *p_addr = NULL;
	UINT32 alloc_size = Size;
	UINT8 *p_mem_pool = VSYS_MEM_Init();
	if (alloc_size == 0){
		return p_addr;
	}
#if (VSYS_MEM_BLOCK_SUPPORT > 0)
	if (alloc_size <= 16)
	{
		p_addr = VMemBlkMalloc(alloc_size);
		vmemsetowner(p_addr, __GET_RETURN_ADDR());
	}
	if (p_addr == NULL)
#endif
	{
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
		alloc_size = VSYS_MEM_RecalcSizeWithMagic(Size);
#endif
		p_addr = tlsf_malloc(p_mem_pool, alloc_size);
		if (p_addr == NULL)
		{
			VSYS_MEM_CPRINTF("SYS Malloc fail,Addr: x%08X;Return:0x%08X;size=%d\r\n", p_addr, __GET_RETURN_ADDR(), Size);
			VSYS_MEM_CheckMalloc_ERR(p_mem_pool);
			__VERRNO_STORE(VERRNO_MEM_NULL);
		}
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
		VSYS_MEM_SetOverwriteMagic(p_addr, Size, __GET_RETURN_ADDR());
		p_addr = VSYS_MEM_UserDataPtr(p_addr);
#endif
		VSYS_ResInfo_Update((UINT32 *)p_mem_pool, FALSE);
		SYSVIEW_HeapAllocEx(p_mem_pool, p_addr, Size, __GET_RETURN_ADDR());
	}
	return p_addr;
}

void vfree(void *p_Addr)
{
	UINT8 *p = (UINT8 *)p_Addr;
	if (p == NULL)
	{
		VSYS_MEM_EPRINTF("SYS vfree fail,Return: 0x%08X[addr=0x%08X]\r\n", __GET_RETURN_ADDR(), (UINT32)p);
		return;
	}
#if (VSYS_MEM_BLOCK_SUPPORT > 0)
	if (VMemBlkValidAddr(VSYS_MEM_BlockPtr(p)))
	{
		VMemBlkFree(p);
	}
	else
#endif
	{
		UINT8 *p_mem_pool = VSYS_MEM_Init();
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
		if (VSYS_MEM_CheckOverwrite(p) != VSYS_MEM_OK)
		{
			VSYS_MEM_WPRINTF("SYS free Return: 0x%08X[addr=0x%08X] had Mem OverFlow!\r\n", __GET_RETURN_ADDR(), (UINT32)p);
			VSYS_MEM_CheckMalloc_ERR(p_mem_pool);
			__VERRNO_STORE(VERRNO_MEM_CRASH);
		}
		p = (void *)VSYS_MEM_BlockPtr(p);
#endif
		tlsf_free(p_mem_pool, p);
		VSYS_ResInfo_Update((UINT32 *)p_mem_pool, FALSE);
		SYSVIEW_HeapFree(p_mem_pool, p);
	}
	return;
}

void *vrealloc(void *p_Addr, UINT32 Size)
{
	UINT32 alloc_size = Size;
#if (VSYS_MEM_BLOCK_SUPPORT > 0)
	if (VMemBlkValidAddr(p_Addr))
	{
		void *p_addr = vmalloc(Size);
		if (p_addr)
		{
			vmemcpy(p_addr, p_Addr, Size);
		}
		VMemBlkFree(p_Addr);
	}
	else
#endif
	{
		UINT8 *p_mem_pool = VSYS_MEM_Init();
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
		if (VSYS_MEM_CheckOverwrite(p_Addr) != VSYS_MEM_OK)
		{
			VSYS_MEM_WPRINTF("realloc Return: 0x%08X[addr=0x%08X] had Mem OverFlow!\r\n", __GET_RETURN_ADDR(), (UINT32)p_Addr);
		}
		p_Addr = (void *)VSYS_MEM_BlockPtr(p_Addr);
		alloc_size = VSYS_MEM_RecalcSizeWithMagic(Size);
#endif

		p_Addr = tlsf_realloc(p_mem_pool, p_Addr, alloc_size);
		if (p_Addr == NULL)
		{
			VSYS_MEM_CPRINTF("realloc fail,Addr: x%08X;Return:0x%08X;size=%d\r\n", p_Addr, __GET_RETURN_ADDR(), Size);
			VSYS_MEM_CheckMalloc_ERR(p_mem_pool);
			__VERRNO_STORE(VERRNO_MEM_NULL);
		}
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
		VSYS_MEM_SetOverwriteMagic(p_Addr, Size, __GET_RETURN_ADDR());
		p_Addr = VSYS_MEM_UserDataPtr(p_Addr);
#endif
		VSYS_ResInfo_Update((UINT32 *)p_mem_pool, FALSE);
		SYSVIEW_HeapAllocEx(p_mem_pool, p_Addr, Size, __GET_RETURN_ADDR());
	}
	return p_Addr;
}

void *vcalloc(size_t nelem, size_t Size)
{
	void *p_addr = NULL;
	UINT8 *p_mem_pool = VSYS_MEM_Init();

	p_addr = vmalloc(nelem * Size);
	vmemsetowner(p_addr, __GET_RETURN_ADDR());

	if (p_addr != NULL)
	{
		vmemset(p_addr, 0, nelem * Size);
	}
	SYSVIEW_HeapAllocEx(p_mem_pool, p_addr, Size, __GET_RETURN_ADDR());
	return p_addr;
}

void VSYS_SysMem_Crash()
{
	#if (VDBG_EN > 0 && VDBG_IO_RETARGET == IO_UART)
	UINT8 *p_mem = VSYS_MEM_Init();
	VSYS_MEM_CheckMalloc_ERR((UINT8*)p_mem);
	VSYS_MEM_EPRINTF("---------------------------------------\r\n");
	VSYS_MEM_DataUpload((UINT8*)p_mem);
	VSYS_MEM_EPRINTF("---------------------------------------\r\n");
	#endif
}
