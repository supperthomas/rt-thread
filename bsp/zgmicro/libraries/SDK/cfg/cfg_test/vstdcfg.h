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

// The files are contained in vstdio.h, vstdlib.h, vstdcore.h,v stddrv.h, vstdos.h
#ifdef __cplusplus
extern "C"
{
#endif


#define VSYS_IO_UART                 2
#define VSYS_IO_RTT                  8
#define VSYS_IO_USB_RTT              9
#define VSYS_IO_UART_RTT             10
#define VSYS_IO_UART_SIM             11
#define VSYS_IO_UART_SIM_RTT         12

#define VOS_PLATFORM_NONE (0)
#define VOS_PLATFORM_UCOSIII (1)
#define VOS_PLATFORM_FREERTOS (2)
#define VOS_PLATFORM_COROUTINE (3)

#include "vdrv_cfg.h"
#include "vdev_cfg.h"

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  921600
#endif

#ifndef VDEV_TYPE
#define TYPE_FPGA 0
#define TYPE_ASIC 1
#ifdef FPGA_QA_BUILD
#define VDEV_TYPE TYPE_FPGA /* FPGA or ASIC */
#else
#define VDEV_TYPE TYPE_ASIC /* FPGA or ASIC */
#endif
#endif

#ifndef VDBG_IO_RETARGET
#define VDBG_IO_RETARGET        VSYS_IO_UART
#endif

#ifndef VDBG_UART_WORKMODE
#define VDBG_UART_WORKMODE  0
#endif

#define SYS_LPO_SUPPOR (0)

#define SYS_DMA_MulticoreState_MODE (0)


#ifndef VSYS_MEM_POOL_SIZE
#define VSYS_MEM_POOL_SIZE					((40) * 1024)
#define VSYS_MEM_POOL_SIZE_NEW 				((0) * 1024 + 384)
#endif

#ifndef VSYS_MEM_POOL_SIZE_TOP_NEW
#define VSYS_MEM_POOL_SIZE_TOP_NEW 			(18 * 1024)
#endif

/*The static memory pool occupies MEM_POOL*/
#ifndef VSYS_MEM_BLOCK_SUPPORT
#define VSYS_MEM_BLOCK_SUPPORT (1)
#define VSYS_STATIC_4BLOCK_NUMBER (30)
#define VSYS_STATIC_8BLOCK_NUMBER (50)
#define VSYS_STATIC_12BLOCK_NUMBER (100)
#define VSYS_STATIC_16BLOCK_NUMBER (100)
#endif

#define VDEV_EXT_FLASH_SUPPORTED (1)

#define VDEV_24BIT_DAC_SUPPORTED (1)
#define VDEV_24BIT_ADC_SUPPORTED (0)

#define VDEV_BSP32_SUPPORTED (1)

#define VDEV_IPC_SUPPORTED (1)

#define VDEV_9900_SUPPORTED (1)

#ifndef VOS_PLATFORM_SELECT_TYPE
#ifdef VOS_TEST
#define VOS_PLATFORM_SELECT_TYPE VOS_PLATFORM_FREERTOS
#elif defined COROUTINE_SUPPORTED
#define VOS_PLATFORM_SELECT_TYPE VOS_PLATFORM_COROUTINE
#else
#define VOS_PLATFORM_SELECT_TYPE VOS_PLATFORM_NONE  //THOMAS
#endif
#endif

#ifndef VSYS_SYSVIEW_SUPPORTED
#define VSYS_SYSVIEW_SUPPORTED (0)
#endif

#ifndef VSYS_OS_MIPS_SUPPORTED
#define VSYS_OS_MIPS_SUPPORTED (1 & !RELEASE_BUILD)
#endif

#define VOSIPCModule_SUPPORT (1)
#define VOSPEND_MULTI_EN_SUPPORT (1)

#define __FAST_ACCESS_VARIABLE

#define __SYNC_ACCESS_VARIABLE

#define __NEW_MEM_AREA_ATTRIBUTES

#define __VERRNO_SUPPORTED (1)

#ifdef __cplusplus
}
#endif

#include "vpla_mem.h"

#endif /* _VSTDCFG_H_ */
