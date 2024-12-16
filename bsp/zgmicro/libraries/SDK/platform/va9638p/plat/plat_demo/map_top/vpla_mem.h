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
	REGISTER DEFINITION

Description:
 	

Revision History:
 	Date                  Author                   Description
 	2020.12.8             ming.m        
	2022.11.24            cxt                            
------------------------------------------------------------------------------*/
#ifndef _VPLA_MEM_H_
#define _VPLA_MEM_H_


    /************************* BT **************************  
    
     0x2005 0000  |----------------------| ---     
                  |   data RAM Block 4   |  |
     0x2004 0000  |----------------------|  |
                  |   data RAM Block 3   |  |
     0x2003 0000  |----------------------|  B
                  |   data RAM Block 2   |  T
     0x2002 0000  |----------------------|  |   ---
                  |   data RAM Block 1   |  |    T
     0x2001 0000  |----------------------| ---   O
                  |   data RAM Block 0   |       P
     0x2000 0000  |----------------------|      ---
    
    
     0x0041 0000  |----------------------| 
                  |  64K Code RAM BT/TOP | share with TOP/BT
     0x0040 0000  |----------------------|
     
     0x0010 0000  |----------------------| 
                  |1MB CodeSpace OnFlash | through cache
     0x0000 0000  |----------------------|
     
     *******************************************************/
     
    /************************* TOP *************************
     
     0x2002 0000  |----------------------| ---
                  |   data RAM Block 1   |  T
     0x2001 0000  |----------------------|  O
                  |   data RAM Block 0   |  P
     0x2000 0000  |----------------------| ---
    
    
     0x0041 0000  |----------------------| 
                  |  64K Code RAM BT/TOP | share with TOP/BT
     0x0040 0000  |----------------------|
 
 
     0x0008 0000  |----------------------| 
                  |256K CodeSpaceOnFlash | through cache
     0x0004 0000  |----------------------|
     
                  |----------------------| 
                  |32K ROM (bootloader)  | 
     0x0000 0000  |----------------------|
     
    *********************************************************/

/* RAM Usage */
#define VDEV_RAM_BASE                       (0x20000000)       
#define VDEV_RAM_SIZE                       (64 * 1024)       

#define VDEV_ROM_BASE                       (0x00000000)        
#define VDEV_ROM_SIZE                       (32 * 1024)  

#define VDEV_SHARERAM_BASE                  (0x20010000)
#define VDEV_SHARERAM_SIZE                  (64 * 1024)

#define VDEV_CODERAM_BASE                   (0x00400000)        
#define VDEV_CODERAM_SIZE                   (64 * 1024) 

#define VDEV_STACK_SIZE                     (1 * 1024)          /* stack size in byte for C library */
#define VDEV_STACK_BASE                     (VDEV_RAM_BASE + VDEV_RAM_SIZE) /* top of stack memory */

#define VDEV_HEAP_SIZE                      (0 * 1024)          /* heap size in byte for C library */
#define VDEV_HEAP_BASE                      (VDEV_STACK_BASE - VDEV_STACK_SIZE - VDEV_HEAP_SIZE)     /* heap memory base addr */

#define VDEV_BT_RAM_BASE					(0x20020000)
#define VDEV_BT_RAM_SIZE					(0x30000)
#define VDEV_TOP_RAM_BASE					(0x20000000)
#define VDEV_TOP_RAM_SIZE					(0x10000)

#define VDEV_BT_FLASH_BASE					(0x00000000)
#define VDEV_BT_FLASH_SIZE					(0x100000)
#define VDEV_TOP_FLASH_BASE					(0x00040000)
#define VDEV_TOP_FLASH_SIZE					(0x40000)

#define VDEV_LOG_TRACE_VIRTUAL_BASE         (0x00800000)
#define VDEV_LOG_TRACE_VIRTUAL_SIZE         (0x00080000)

#endif //#ifndef _VPLA_MEM_H_
