/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
    MEM

Description:
     MEM Driver
Revision History:
     Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_mem.h"

#if (VDEV_MEM_POOL_SIZE > VDEV_MEM_POOL_UNIT)
UINT32 VDEV_MEM_POOL_peak_size;
UINT32 VDEV_MEM_POOL_used_size;
UINT8 VDEV_MEM_POOL_Manage[VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT]__attribute__((section(".vdev_ram")));
__attribute__((aligned(64))) UINT8 VDEV_MEM_POOL[VDEV_MEM_POOL_SIZE]  __attribute__((section(".vdev_ram")));


void *VDEV_MEM_Malloc(UINT32 size)
{
   UINT16 block = 0;
   UINT8  i = 0,hsize = 0;
   UINT8 *manage = NULL;
   void *addr = NULL;
   static UINT8 init_flag = 0;
  
  VCPU_SR_ALLOC();
  VCPU_CRITICAL_ENTER();
   manage = VDEV_MEM_POOL_Manage;
   if(!init_flag){//init
      VCOMMON_MemSet(manage,0,VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT);
      manage[0] = VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT; 
      manage[VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT -1] = VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT;
      VDEV_MEM_POOL_peak_size = 0;
      VDEV_MEM_POOL_used_size = 0;
      init_flag = 1;
   }
   block = (size + VDEV_MEM_POOL_UNIT - 1)/VDEV_MEM_POOL_UNIT;
   while(i < (VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT)){
       hsize = manage[i]&VDEV_MEM_UNIT_SIZE;
       if((block == 0) || (hsize == 0) || ((i+hsize) > (VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT))\
          || (manage[i] != manage[i+hsize-1])){
          break;
       }
       if(!(manage[i]&VDEV_MEM_UNIT_USE) && (block <= hsize)){
          manage[i] = (block|VDEV_MEM_UNIT_USE);
          manage[i+block-1] = (block|VDEV_MEM_UNIT_USE);
          if(block < hsize){
            manage[i+block] = (hsize-block);
            manage[i+hsize-1] = (hsize-block);
          }
          addr = i*VDEV_MEM_POOL_UNIT+VDEV_MEM_POOL;
          break;
       }
       i += hsize;
   }
   if(addr != NULL){
        VDEV_MEM_POOL_used_size += block*VDEV_MEM_POOL_UNIT;
        if(VDEV_MEM_POOL_peak_size < VDEV_MEM_POOL_used_size){
            VDEV_MEM_POOL_peak_size = VDEV_MEM_POOL_used_size;
        }
   }
   VCPU_CRITICAL_EXIT();
   DEVMEM_ASSERT(addr == NULL);
   return addr;
}

void VDEV_MEM_Free(void *addr)
{
   UINT32 pool_addr = 0;
   UINT8  i = 0,hsize = 0;
   UINT8 *manage = NULL;
  
   VCPU_SR_ALLOC();
   VCPU_CRITICAL_ENTER();
   pool_addr = (UINT32)addr;
   manage = VDEV_MEM_POOL_Manage;
   if((pool_addr >= (UINT32)VDEV_MEM_POOL) && ((pool_addr-(UINT32)VDEV_MEM_POOL)%VDEV_MEM_POOL_UNIT == 0)){
     i = (pool_addr - (UINT32)VDEV_MEM_POOL)/VDEV_MEM_POOL_UNIT;
     hsize = manage[i]&VDEV_MEM_UNIT_SIZE;
     if(hsize && ((i+hsize) <= (VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT)) && (manage[i]&VDEV_MEM_UNIT_USE)\
        && (manage[i] == manage[i+hsize-1])){
       VDEV_MEM_POOL_used_size -= hsize*VDEV_MEM_POOL_UNIT;
       manage[i] = hsize;      
       manage[i+hsize-1] = hsize;
       if((i+hsize < (VDEV_MEM_POOL_SIZE/VDEV_MEM_POOL_UNIT)) && (!(manage[i+hsize]&VDEV_MEM_UNIT_USE))){
          manage[i+hsize-1] = 0;
          hsize += manage[i+hsize]&VDEV_MEM_UNIT_SIZE;
          manage[i+hsize-manage[i+hsize-1]] = 0;
          manage[i] = hsize;      
          manage[i+hsize-1] = hsize;
       }
       if((i > 0) && (!(manage[i-1]&VDEV_MEM_UNIT_USE))){
          manage[i] = 0;
          hsize += manage[i-1]&VDEV_MEM_UNIT_SIZE;
          i -= manage[i-1]&VDEV_MEM_UNIT_SIZE;  
          manage[i+manage[i]-1] = 0;
          manage[i] = hsize;
          manage[i+hsize-1] = hsize;
       }
     }else{
       DEVMEM_ASSERT(1);
     }
   }else{
    DEVMEM_ASSERT(1);
   }
   VCPU_CRITICAL_EXIT();
   DEVMEM_ASSERT(addr == NULL);
   return;
}
#endif

