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
    DEBUG

Description:
     Debug relative function definitions.

Revision History:
     Date                  Author                   Description
     2022.11.16             cxt              initial version
------------------------------------------------------------------------------*/
#ifndef _VSTDCFG_H_
#define _VSTDCFG_H_

//The files are contained in vstdio.h, vstdlib.h, vstdcore.h,v stddrv.h, vstdos.h
#ifdef __cplusplus 
extern "C" { 
#endif


#define VSYS_IO_UART                 2
#define VSYS_IO_RTT                  8
#define VSYS_IO_USB_RTT              9
#define VSYS_IO_UART_RTT             10
#define VSYS_IO_UART_SIM             11
#define VSYS_IO_UART_SIM_RTT         12

#include "vdrv_cfg.h"
#include "vdev_cfg.h"

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  921600
#endif

#ifndef VDBG_IO_RETARGET
#define VDBG_IO_RETARGET        VSYS_IO_UART
#endif

#ifndef VDBG_UART_WORKMODE
#define VDBG_UART_WORKMODE  0
#endif

#define SYS_LPO_SUPPOR			(1)

#define SYS_DMA_MulticoreState_MODE			(0)

#ifndef VSYS_MEM_POOL_SIZE
#define VSYS_MEM_POOL_SIZE					((128) * 1024)
#define VSYS_MEM_POOL_SIZE_NEW 				((27) * 1024 + 384)
#endif


/*The static memory pool occupies MEM_POOL*/
#ifndef VSYS_MEM_BLOCK_SUPPORT
#define VSYS_MEM_BLOCK_SUPPORT (1)
#define VSYS_STATIC_4BLOCK_NUMBER (30)
#define VSYS_STATIC_8BLOCK_NUMBER (50)
#define VSYS_STATIC_12BLOCK_NUMBER (100)
#define VSYS_STATIC_16BLOCK_NUMBER (100)
#endif

#define VOS_PLATFORM_NONE (0)
#define VOS_PLATFORM_UCOSIII (1)
#define VOS_PLATFORM_FREERTOS (2)
#define VOS_PLATFORM_COROUTINE (3)

#define VOS_PLATFORM_SELECT_TYPE VOS_PLATFORM_UCOSIII

#ifndef VOSIPCModule_SUPPORT
#define VOSIPCModule_SUPPORT (0)
#endif
#define VOSPEND_MULTI_EN_SUPPORT (0)

#ifndef VSYS_OS_MIPS_SUPPORTED
#define VSYS_OS_MIPS_SUPPORTED (1 & !RELEASE_BUILD)
#endif

#ifndef VSYS_SYSVIEW_SUPPORTED
#define VSYS_SYSVIEW_SUPPORTED (0)
#endif


#define __FAST_ACCESS_VARIABLE 

#define __SYNC_ACCESS_VARIABLE 

#define __RTT_AREA_ATTRIBUTES  //__attribute__ ((section("rtt_area_section")))

#define __NEW_MEM_AREA_ATTRIBUTES  //__attribute__ ((at(VDEV_BT_NEW_RAM_BASE)))

#define __VERRNO_SUPPORTED (1)

#ifdef __cplusplus
}
#endif

#include "vpla_mem.h"

#endif  /* _VSTDCFG_H_ */
