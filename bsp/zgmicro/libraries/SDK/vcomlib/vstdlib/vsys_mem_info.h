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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSYS_MEM_INFO_H_
#define _VSYS_MEM_INFO_H_
#include "vstdlib.h"
#include "vstdio.h"
#ifdef __cplusplus 
extern "C" {
#endif


#define VSYS_MEM_CHK_OWNER_LOG_EN        1
#define VSYS_MEM_CHK_OVERFLOW_EN		(1)

#define VSYS_MEM_OK           0
#define VSYS_MEM_HEADBREAK    -1
#define VSYS_MEM_TAILBREAK    -2
#define VSYS_MEM_ERR          -3


#define VSYS_MEM_CHK_OVERFLOW_MAGICCODE 0x5A5A

/** Memory color
 *
 *   the memory map
 *
 *       block head ---> | 0x55AA0000|size
 *   mem   addr ---> | data 1
 *        size = n   | data 2
 *                   | data n
 *       block tail ---> | head addr
 *
 */
typedef struct mem_blk_info_head_s
{
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	UINT32 owner; // the PC Addr, who malloc the block
#endif			  // #if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	UINT16 sz;	  // the payload size of the mem block
	UINT16 magic; // magic code
} mem_blk_info_head_t __attribute__((aligned(1)));

typedef struct mem_blk_info_tail_s
{
	UINT16 head;
#if (VSYS_MEM_CHK_OWNER_LOG_EN > 0)
	UINT8 arg;
	UINT8 type;
#endif
} mem_blk_info_tail_t __attribute__((aligned(1)));

UINT32 VSYS_MEM_RecalcSizeWithMagic(UINT32 sz);
void VSYS_MEM_SetOverwriteMagic(void *blk, UINT32 datasize, UINT32 LR);
void *VSYS_MEM_UserDataPtr(void *blk);
void *VSYS_MEM_BlockPtr(void *p);
int VSYS_MEM_CheckOverwrite(void *p);
void VSYS_MEM_SetType(void *p_mem, UINT8 type);
UINT32 VSYS_MEM_AllSize(UINT8 *buff);
void VSYS_MEM_CheckMalloc_ERR(UINT8 *buff);
void VSYS_MEM_DataUpload(UINT8 *buff);

#ifdef __cplusplus
}
#endif 

#endif  /* _VSYS_MEM_INFO_H_ */

