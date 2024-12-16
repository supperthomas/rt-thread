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
#include "vstdos.h"
#include "vsys_mem.h"
#include "vsys_mem_tlsf.h"
#include "vsys_mem_info.h"
#include "vstdcore.h"

/// Set Info to the alloc block
/// @blk block address
/// @datasize  user data size
/// @LR        return address of the alloc calling
void VSYS_MEM_SetOverwriteMagic(void *blk, UINT32 datasize, UINT32 LR)
{
	mem_blk_info_head_t *hdr = (mem_blk_info_head_t *)blk;
	mem_blk_info_tail_t *tail;
	if (hdr != NULL)
	{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
		hdr->owner = LR;
#endif
		hdr->magic = VSYS_MEM_CHK_OVERFLOW_MAGICCODE;
		hdr->sz = (UINT16)(datasize & 0x0000FFFF);
		tail = (mem_blk_info_tail_t *)((UINT32)hdr + datasize + sizeof(mem_blk_info_head_t));
		SPLIT_U16(&tail->head, *(UINT16 *)&hdr);
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
		tail->arg = 0;
		tail->type = ((VCORE_GetID()&0x0F)<<4)|0;
#endif
	}
}

/// calc mem alloc size with infomation data
UINT32 VSYS_MEM_RecalcSizeWithMagic(UINT32 sz)
{
	if (sz)
		sz += sizeof(mem_blk_info_head_t) + sizeof(mem_blk_info_tail_t);
	return sz;
}

void *VSYS_MEM_UserDataPtr(void *blk)
{
	UINT32 *p_Addr = blk;
	if (p_Addr != NULL)
	{
		p_Addr = (void *)((UINT32)p_Addr + sizeof(mem_blk_info_head_t));
	}
	return p_Addr;
}

void *VSYS_MEM_BlockPtr(void *p)
{
	UINT32 *p_Addr = p;

	if (p_Addr != NULL)
	{
		p_Addr = (void *)((UINT32)p_Addr - sizeof(mem_blk_info_head_t));
	}
	return p_Addr;
}

int VSYS_MEM_CheckOverwrite(void *p)
{
	mem_blk_info_head_t *hdr;
	mem_blk_info_tail_t *tail;

	if ((UINT32)p < sizeof(mem_blk_info_head_t))
		return VSYS_MEM_ERR;

	// move backward, find the alloc block head
	hdr = (mem_blk_info_head_t *)VSYS_MEM_BlockPtr(p);

	if (hdr->magic != VSYS_MEM_CHK_OVERFLOW_MAGICCODE)
	{
		return VSYS_MEM_HEADBREAK;
	}

	// check tail
	tail = (mem_blk_info_tail_t *)((UINT32)hdr + sizeof(mem_blk_info_head_t) + hdr->sz);

	// check the head and tail
	if (*(UINT16 *)&hdr != MAKE_U16(&tail->head))
	{
		return VSYS_MEM_TAILBREAK;
	}

	return VSYS_MEM_OK;
}

void vmemsetowner(void *p_mem, UINT32 LR)
{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	mem_blk_info_head_t *hdr = VSYS_MEM_BlockPtr(p_mem);
	if (hdr != NULL && hdr->magic == VSYS_MEM_CHK_OVERFLOW_MAGICCODE)
	{
		hdr->owner = LR;
	}
#endif
}

void vmemsetarg(void *p_mem, UINT8 arg)
{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	mem_blk_info_head_t *hdr = VSYS_MEM_BlockPtr(p_mem);
	if(VSYS_MEM_OK == VSYS_MEM_CheckAddr(hdr)){
		if (hdr != NULL && hdr->magic == VSYS_MEM_CHK_OVERFLOW_MAGICCODE)
		{
			mem_blk_info_tail_t *tail = (mem_blk_info_tail_t *)((UINT32)hdr + sizeof(mem_blk_info_head_t) + hdr->sz);
			tail->arg = arg;
		}
	}
#endif
}

void VSYS_MEM_SetType(void *p_mem, UINT8 type)
{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	mem_blk_info_head_t *hdr = VSYS_MEM_BlockPtr(p_mem);
	if(VSYS_MEM_OK == VSYS_MEM_CheckAddr(hdr)){
		if (hdr != NULL && hdr->magic == VSYS_MEM_CHK_OVERFLOW_MAGICCODE)
		{
			mem_blk_info_tail_t *tail = (mem_blk_info_tail_t *)((UINT32)hdr + sizeof(mem_blk_info_head_t) + hdr->sz);
			tail->type = (tail->type&0xF0)|type;
		}
	}
#endif
}


UINT32 VSYS_MEM_AllSize(UINT8 *buff)
{
	tlsf_t *tlsf = (tlsf_t *)buff;
	UINT8 *ib, *lb, *lbn;
	UINT32 size = 0;

	TLSF_ACQUIRE_LOCK(&tlsf->lock);
	size += ROUNDUP_SIZE(sizeof(tlsf_t));
	area_info_t *ai = tlsf->area_head;
	while (ai)
	{
		ib = (UINT8 *)((char *)ai - BHDR_OVERHEAD);
		lb = (UINT8 *)(ai->end);
		lbn = lb + BHDR_OVERHEAD;
		size += (UINT32)(lbn - ib);
		ai = ai->next;
	}
	TLSF_RELEASE_LOCK(&tlsf->lock);
	return size;
}

#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
#if (VDBG_EN > 0)
#define MALLOC_BLK_SIZE 20
#if (VDBG_IO_RETARGET != IO_FLASH)
#define VSYS_MEM_Check_EPRINTF VSYS_MEM_EPRINTF
#define VSYS_MEM_Check_NPRINTF VSYS_MEM_NPRINTF
#else
#define VSYS_MEM_Check_EPRINTF FLASH_LOG_Record
#define VSYS_MEM_Check_NPRINTF FLASH_LOG_Record
#endif


static void mem_data_upload(UINT8 index, UINT8* buff, UINT16 sz)
{
	UINT32 addr = (UINT32)buff;

	vdirect_vputc('t');vdirect_vputc('l');vdirect_vputc('s');vdirect_vputc('f');
	vdirect_vputc(addr&0xFF);vdirect_vputc((addr>>8)&0xFF);
	vdirect_vputc((addr>>16)&0xFF);vdirect_vputc((addr>>24)&0xFF);
	vdirect_vputc(0x5A);vdirect_vputc(index);vdirect_vputc(sz&0xFF);vdirect_vputc((sz>>8)&0xFF);
	for(UINT16 j = 0;j < sz;j++){
		vdirect_vputc(*buff++);
	}
	vdirect_vputc(0xA5);vdirect_vputc(index);vdirect_vputc(sz&0xFF);vdirect_vputc((sz>>8)&0xFF);
}

void VSYS_MEM_DataUpload(UINT8 *buff)
{
	tlsf_t *tlsf = (tlsf_t *)buff;
	UINT8 *ib, *lb, *lbn, *tmp_b;
	UINT8 index = 1;
	#define MEM_PDU_LEN 1024

	TLSF_ACQUIRE_LOCK(&tlsf->lock);
	area_info_t *ai = tlsf->area_head;
	mem_data_upload(0,(UINT8 *)tlsf,sizeof(tlsf_t));
	while (ai)
	{
		ib = (UINT8 *)((char *)ai - BHDR_OVERHEAD);
		lb = (UINT8 *)(ai->end);
		tmp_b = ib;
		lbn = lb + BHDR_OVERHEAD;
		
		while(tmp_b + MEM_PDU_LEN <= lbn)
		{
			mem_data_upload(index,tmp_b,MEM_PDU_LEN);
			tmp_b += MEM_PDU_LEN;
			index++;	
		}
		if(tmp_b < lbn)
		{
			mem_data_upload(index,tmp_b,lbn - tmp_b);
			index++;
		}
		ai = ai->next;
	}
	TLSF_RELEASE_LOCK(&tlsf->lock);
	return;
}

void VSYS_MEM_CheckMalloc_ERR(UINT8 *buff)
{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	tlsf_t *tlsf = (tlsf_t *)buff;
	bhdr_t *ib, *lb, *ibn, *lbn, *tmp_b, *tmp_bn;
	area_info_t *ai, *ain;
	mem_blk_info_head_t *hadr = NULL;
	UINT8 i = 0, j = 0, mem_err = 0;
	UINT32 owner_p, total_size, used_num;
	UINT32 total_used = 0, total_free = 0;
	UINT32 *addr = 0;
	SINT32 ret_err = 0;
	UINT32 malloc_blk_info[MALLOC_BLK_SIZE][2] = {
		0,
	};

	vmemset((char *)malloc_blk_info, 0, sizeof(malloc_blk_info));
	TLSF_ACQUIRE_LOCK(&tlsf->lock);
	ai = tlsf->area_head;
	while (ai)
	{
		ib = (bhdr_t *)((char *)ai - BHDR_OVERHEAD);
		lb = (bhdr_t *)(ai->end);
		tmp_b = GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
		while (tmp_b && (tmp_b != lb))
		{
			if (tmp_b <= ib || tmp_b >= lb)
			{
				mem_err |= 0x01;
				break;
			}
			if ((tmp_b->size & BLOCK_STATE) == USED_BLOCK)
			{
				hadr = (mem_blk_info_head_t *)tmp_b->ptr.buffer;
				if (!(hadr->owner & 0x80000000UL))
				{ //(0x1<<31)
					owner_p = hadr->owner;
					hadr->owner |= (0x80000000UL); //~(0x1<<31)
					total_size = hadr->sz;
					used_num = 1;
					lbn = lb;
					ain = ai;
					tmp_bn = GET_NEXT_BLOCK(tmp_b->ptr.buffer, tmp_b->size & BLOCK_SIZE);
					do
					{
						while (tmp_bn && (tmp_bn != lbn))
						{
							if (tmp_bn <= (bhdr_t *)ain || tmp_bn >= lbn)
							{
								mem_err |= 0x02;
								break;
							}
							if ((tmp_bn->size & BLOCK_STATE) == USED_BLOCK)
							{
								hadr = (mem_blk_info_head_t *)tmp_bn->ptr.buffer;
								if (hadr->owner == owner_p)
								{
									hadr->owner |= (0x80000000UL); //~(0x1<<31);
									total_size += hadr->sz;
									used_num++;
								}
							}
							tmp_bn = GET_NEXT_BLOCK(tmp_bn->ptr.buffer, tmp_bn->size & BLOCK_SIZE);
						}
						ain = ain->next;
						if (ain)
						{
							ibn = (bhdr_t *)((char *)ain - BHDR_OVERHEAD);
							lbn = (bhdr_t *)(ain->end);
							tmp_bn = GET_NEXT_BLOCK(ibn->ptr.buffer, ibn->size & BLOCK_SIZE);
						}
					} while (ain);
					// owner_p &= ~(0x80000000UL); //~(0x1<<31);
					// if((owner_from <= owner_p) && (owner_to >= owner_p))
					{
						UINT32 total_sizemin = malloc_blk_info[0][1] & 0x3FFFF;
						j = 0;
						for (i = 1; i < MALLOC_BLK_SIZE; i++)
						{
							UINT32 tmp_size = malloc_blk_info[i][1] & 0x3FFFF;
							if (total_sizemin > tmp_size)
							{
								total_sizemin = tmp_size;
								j = i;
							}
						}
						if (total_sizemin < total_size)
						{
							malloc_blk_info[j][0] = owner_p;
							malloc_blk_info[j][1] = ((used_num << 18) | (total_size & 0x3FFFF));
						}
					}
				}
			}
			tmp_b = GET_NEXT_BLOCK(tmp_b->ptr.buffer, tmp_b->size & BLOCK_SIZE);
		}
		ai = ai->next;
	}

	ai = tlsf->area_head;
	while (ai)
	{
		ib = (bhdr_t *)((char *)ai - BHDR_OVERHEAD);
		lb = (bhdr_t *)(ai->end);
		tmp_b = GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
		while (tmp_b && (tmp_b != lb))
		{
			if (tmp_b <= ib || tmp_b >= lb)
			{
				mem_err |= 0x10;
				break;
			}
			if ((tmp_b->size & BLOCK_STATE) == USED_BLOCK)
			{
				hadr = (mem_blk_info_head_t *)tmp_b->ptr.buffer;
				hadr->owner &= (~0x80000000UL); // 0x01<<31;
				addr = VSYS_MEM_UserDataPtr(hadr);
				ret_err = VSYS_MEM_CheckOverwrite(addr);
				if (ret_err == VSYS_MEM_HEADBREAK)
				{
					VSYS_MEM_Check_EPRINTF("[0x%06x] 0x%08x Head overflow, tail unknown\r\n", hadr->owner & 0x7FFFFFFF, addr);
				}
				else if (ret_err == VSYS_MEM_TAILBREAK)
				{
					VSYS_MEM_Check_EPRINTF("[0x%06x] 0x%08x Tail overflow, size = 0x%04x.\r\n", hadr->owner & 0x7FFFFFFF, addr, hadr->sz);
				}
				else if (ret_err != VSYS_MEM_OK)
				{
					VSYS_MEM_Check_EPRINTF("mem err %d unknown\r\n", ret_err);
				}
				total_used++;
			}
			else
			{
				total_free++;
			}
			tmp_b = GET_NEXT_BLOCK(tmp_b->ptr.buffer, tmp_b->size & BLOCK_SIZE);
		}
		ai = ai->next;
	}

	TLSF_RELEASE_LOCK(&tlsf->lock);
	VSYS_MEM_Check_NPRINTF("\r\n");
	VSYS_MEM_Check_NPRINTF(" tlsf_p    0x%08x\ttlsf_size    %6d\r\n", (UINT32)tlsf, ROUNDUP_SIZE(sizeof(tlsf_t)));
	ai = tlsf->area_head;
	while (ai)
	{
		VSYS_MEM_Check_NPRINTF(" area_p    0x%08x\tarea_size    %6d\r\n", ((UINT32)ai - BHDR_OVERHEAD), (UINT32)ai->end - (UINT32)ai + BHDR_OVERHEAD * 2);
		ai = ai->next;
	}
	VSYS_MEM_Check_NPRINTF(" used_size     %6d\tpeak_size    %6d\r\n", tlsf->used_size, tlsf->peak_size);
	VSYS_MEM_Check_NPRINTF(" total_used    %6d\ttotal_free   %6d\r\n", total_used, total_free);

	if (mem_err)
		VSYS_MEM_Check_EPRINTF("\tTLSF memory management system crash%d!!!!\r\n", mem_err);

	VSYS_MEM_Check_NPRINTF("-----------malloc_blk_info-------------\r\n");
	if (malloc_blk_info[0][0] != 0)
	{
		for (i = 0; i < MALLOC_BLK_SIZE - 1; i++)
		{
			for (j = 0; j < MALLOC_BLK_SIZE - 1 - i; j++)
			{
				UINT32 malloc_blk_tmp[2] = {0, 0};
				if ((malloc_blk_info[j][1] & 0x3FFFF) < (malloc_blk_info[j + 1][1] & 0x3FFFF))
				{
					malloc_blk_tmp[0] = malloc_blk_info[j][0];
					malloc_blk_tmp[1] = malloc_blk_info[j][1];
					malloc_blk_info[j][0] = malloc_blk_info[j + 1][0];
					malloc_blk_info[j][1] = malloc_blk_info[j + 1][1];
					malloc_blk_info[j + 1][0] = malloc_blk_tmp[0];
					malloc_blk_info[j + 1][1] = malloc_blk_tmp[1];
				}
			}
		}
		for (i = 0; i < MALLOC_BLK_SIZE; i++)
		{
			owner_p = malloc_blk_info[i][0] & 0x7FFFFFFF;
			total_size = malloc_blk_info[i][1] & 0x3FFFF;
			used_num = (malloc_blk_info[i][1] >> 18) & 0x3FFF;
			VSYS_MEM_Check_NPRINTF("   [0x%06x] %6db %3d\r\n", owner_p, total_size, used_num);
		}
	}
	else
	{
		VSYS_MEM_Check_NPRINTF("             NULL\r\n");
	}
#endif
	return;
}

#else
void VSYS_MEM_CheckMalloc_ERR(UINT8 *buff)
{
}
#endif
#endif


