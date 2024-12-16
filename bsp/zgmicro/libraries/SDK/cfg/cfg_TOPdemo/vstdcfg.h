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
#define VSYS_IO_USB_SERIAL           7
#define VSYS_IO_RTT                  8
#define VSYS_IO_USB_RTT              9
#define VSYS_IO_UART_RTT 		  10
#define VSYS_IO_UART_SIM             11
#define VSYS_IO_UART_SIM_RTT         12

#include "vdrv_cfg.h"
#include "vdev_cfg.h"
#include "stdbool.h"
#include "stdint.h"

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  921600
#endif

#ifndef VDBG_IO_RETARGET
#define VDBG_IO_RETARGET        VSYS_IO_UART_RTT
#endif

#ifndef VDBG_UART_WORKMODE
#define VDBG_UART_WORKMODE  0
#endif

#define SYS_LPO_SUPPOR			(0)

#define SYS_DMA_MulticoreState_MODE			(0)

#ifndef VSYS_MEM_POOL_SIZE
#define VSYS_MEM_POOL_SIZE					((48) * 1024)
#define VSYS_MEM_POOL_SIZE_NEW 				((18) * 1024 + 384)
#endif

#define VOS_PLATFORM_NONE (0)
#define VOS_PLATFORM_UCOSIII (1)
#define VOS_PLATFORM_FREERTOS (2)
#define VOS_PLATFORM_COROUTINE (3)

#define VOS_PLATFORM_SELECT_TYPE VOS_PLATFORM_FREERTOS	

#ifndef VSYS_SYSVIEW_SUPPORTED
#define VSYS_SYSVIEW_SUPPORTED (0)
#endif

#ifndef VOSIPCModule_SUPPORT
#define VOSIPCModule_SUPPORT (0)
#endif
#define VOSPEND_MULTI_EN_SUPPORT (0)

#define VPLA_TRACE_MARK_EN   0

#ifndef VDEV_DMA_DCACHE_SYNC
#define VDEV_DMA_DCACHE_SYNC
#endif

#ifndef CONFIG_VKERNEL_I2C_INTERRUPT_ENABLE
#define CONFIG_VKERNEL_I2C_INTERRUPT_ENABLE             (1) /* 1: vkernel 硬件I2C支持中断方式; 0: vkernel 硬件I2C不支持中断方式 */
#endif

#ifndef VSYS_OS_MIPS_SUPPORTED
#define VSYS_OS_MIPS_SUPPORTED (1 & !RELEASE_BUILD)
#endif

#define __FAST_ACCESS_VARIABLE 

#define __SYNC_ACCESS_VARIABLE 

#define __RTT_AREA_ATTRIBUTES

#define __NEW_MEM_AREA_ATTRIBUTES  //__attribute__ ((section("new_mem_area_section")))

#define __VERRNO_SUPPORTED (1)

#ifdef __cplusplus
}
#endif

#include "vpla_mem.h"

#endif  /* _VSTDCFG_H_ */
