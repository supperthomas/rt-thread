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
	errno

Description:
	errno.

Revision History:
	Date                  Author                   Description
	2024.02.23             cxt                  initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDEBUG_ERRNO_
#define _VDEBUG_ERRNO_
#ifdef __cplusplus
extern "C"
{
#endif

	extern UINT32 *__verrinfo_location(void);
	extern UINT32 __verrinfo_store(UINT16 verrno, UINT16 line);

#define __VERRNO (*__verrinfo_location()&0xFFFF)
#define __VERRLINE (*__verrinfo_location()>>0x10)

#define __VERRNO_STORE(verno) __verrinfo_store(verno, __LINE__)



#define VERRNO_NONE 0x00

#define VERRNO_INVALID 0x01		 // 错误码非法
#define VERRNO_PARA_NULL 0x02	 // 参数为NULL
#define VERRNO_PARA_INVALID 0x03 // 参数无效

#define VERRNO_MEM_BASE 0x100
#define VERRNO_MEM_CRASH (VERRNO_MEM_BASE + 0x00) // 内存越界崩溃
#define VERRNO_MEM_NULL (VERRNO_MEM_BASE + 0x01)  // 内存耗尽

#define VERRNO_OS_BASE 0x200
#define VERRNO_OS_STATE (VERRNO_OS_BASE + 0x00)
#define VERRNO_OS_MULTI (VERRNO_OS_BASE + 0x01)
#define VERRNO_OS_IPC (VERRNO_OS_BASE + 0x02)

#define VERRNO_GPIO_BASE 0x300
#define VERRNO_GPIO_INVALID (VERRNO_GPIO_BASE + 0x00)

#define VERRNO_CKM_BASE 0x400

#define VERRNO_UART_BASE 0x500

#define VERRNO_TIMER_BASE 0x600
#define VERRNO_TIMER_INVALID (VERRNO_TIMER_BASE + 0x00)

#define VERRNO_I2C_BASE 0x700
#define VERRNO_I2C_INVALID (VERRNO_I2C_BASE + 0x00)
#define VERRNO_I2C_SWITCH (VERRNO_I2C_BASE + 0x01)
#define VERRNO_I2C_WR (VERRNO_I2C_BASE + 0x02)
#define VERRNO_I2C_RD (VERRNO_I2C_BASE + 0x03)
#define VERRNO_I2C_BIT (VERRNO_I2C_BASE + 0x04)

#define VERRNO_LED_BASE 0x800
#define VERRNO_LED_INVALID (VERRNO_LED_BASE + 0x00)

#define VERRNO_FLASH_BASE 0x900
#define VERRNO_FLASH_INVALID (VERRNO_FLASH_BASE + 0x00)
#define VERRNO_FLASH_ERASE (VERRNO_FLASH_BASE + 0x01)
#define VERRNO_FLASH_WR (VERRNO_FLASH_BASE + 0x02)
#define VERRNO_FLASH_RD (VERRNO_FLASH_BASE + 0x03)
#define VERRNO_FLASH_LIMITS (VERRNO_FLASH_BASE + 0x04)

#define VERRNO_DSP_BASE 0xA00
#define VERRNO_DSP_INVALID (VERRNO_DSP_BASE + 0x00)

#define VERRNO_WDT_BASE 0xB00
#define VERRNO_WDT_INVALID (VERRNO_WDT_BASE + 0x00)

#ifdef __cplusplus
}
#endif

#endif /* _VDEBUG_ERRNO_ */
