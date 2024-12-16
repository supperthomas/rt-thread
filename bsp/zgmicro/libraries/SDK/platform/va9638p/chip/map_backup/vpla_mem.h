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
     2022.11.24            cxt                  
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_MEM_H_
#define _VPLA_MEM_H_
	
//Reassignment is required
#define VDEV_YRAM_BASE                        (0x20000000)    
#define VDEV_YRAM_SIZE                       (128 * 1024)  
#define VDEV_DSP_YRAM_BASE                  (0x20000000)
#define VDEV_DSP_YRAM_SIZE                  (128 * 1024)

//Reassignment is required
#define VDEV_XRAM_BASE                      (0x30000000)      
#define VDEV_XRAM_SIZE                       (256 * 1024)  
#define VDEV_DSP_XRAM_BASE                  (0x30000000)
#define VDEV_DSP_XRAM_SIZE                     (256 * 1024)

#define VDEV_CODERAM_BASE                   (0x30040000)        
#define VDEV_CODERAM_SIZE                   (64 * 1024) 

#define VDEV_ROM_BASE                       (0x40000000)        
#define VDEV_ROM_SIZE                       (48 * 1024)  

#define VDEV_FLASH_BASE                     (0x40400000)        
#define VDEV_FLASH_SIZE                     (4 * 1024 * 1024) 

#define VDEV_FLASHEX_BASE                    (0x40800000)        
#define VDEV_FLASHEx_SIZE                      (8 * 1024 * 1024) 


#endif //#ifndef _VPLA_MEM_H_
