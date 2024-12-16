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
    MEM

Description:
    MEM driver.

Revision History:
    Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDRV_MEM_H_
#define _VDRV_MEM_H_

#include "vstdlib.h"
#ifdef __cplusplus
extern "C" {
#endif

#define DEVMEM_ASSERT(expr)  do{if(expr)while(1);}while(0)

#define VDEV_MEM_POOL_SIZE  (43*1024)

#define VDEV_MEM_UNIT_USE    (0x80)
#define VDEV_MEM_UNIT_SIZE   (0x7F)
#define VDEV_MEM_POOL_UNIT   (VDEV_MEM_POOL_SIZE/VDEV_MEM_UNIT_SIZE>512?1024:VDEV_MEM_POOL_SIZE/VDEV_MEM_UNIT_SIZE>256?512:256)

#if (VDEV_MEM_POOL_SIZE/VDEV_MEM_UNIT_SIZE>1024)
    #error "VDEV_MEM_POOL_SIZE too big!"
#endif

void *VDEV_MEM_Malloc(UINT32 size);
void VDEV_MEM_Free(void *addr);

#ifdef __cplusplus
}
#endif

#endif  /* _VDRV_MEM_H_ */

