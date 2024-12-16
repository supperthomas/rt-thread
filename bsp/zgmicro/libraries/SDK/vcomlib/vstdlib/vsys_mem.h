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
 	Memory management fuctions.

Revision History:
 	Date                  Author                   Description
 	2009.05.08           chenjingen              initial version
	2013.01.23			 XiongYan			  	 change for EQ(and ANC&AEC)
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSYS_MEM_H_
#define _VSYS_MEM_H_

#include "vstdlib.h"
#include "vstdio.h"
#ifdef __cplusplus 
extern "C" {
#endif

#define VSYS_MEM_NUM_OF_PARTITION		0 		/* number of memory partitions */


typedef struct {
#if (VSYS_MEM_NUM_OF_PARTITION > 0)
	UINT16 mem_blk_num[VSYS_MEM_NUM_OF_PARTITION];
#endif
	UINT32 ext_pool_siz;
}VSYS_MEM_CONFIG;

#ifdef __cplusplus
}
#endif 

#endif  /* _VSYS_MEM_H_ */

