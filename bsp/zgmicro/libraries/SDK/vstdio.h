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
#ifndef _VSTDIO_H_
#define _VSTDIO_H_
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDIO_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES */
#include "vstdcfg.h"
#include "vstdlib.h"
#include "vpla_trace.h"
#include "vdebug_errno.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "vdebug.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */ 

 /**
 * @addtogroup Std_IO
 * 
 * @{
 */

 /// @brief 初始化调试功能
 /// @note 该函数必须在vkernel初始化之前调用
 extern void VDBG_Init(void);

 /// @brief 提供字符输出功能
 /// @param ch 要输出的字符
 /// @return 返回输出的字符
 /// @note 该函数会根据vdbg_output_mode的值来决定是否输出字符
 extern int vfputc(int ch);

 /// @brief 提供字符串输出功能
 /// @param str 要输出的字符串
 /// @note 该函数会根据vdbg_output_mode的值来决定是否输出字符串
 extern void vputs(char *str);

 /// @brief 提供字符输入功能
 /// @return 返回输入的字符
 /// @note 该函数会根据vdbg_input_mode的值来决定是否输入字符
 extern int vfgetc(void);

 /// @brief 格式化输出函数
 /// @param fmt 格式化字符串
 /// @note 该函数不支持格式化字符串中的 
 extern void vcom_printf(const char *fmt, ...);

 /// @brief 小型格式化输出函数
 /// @param fmt 格式化字符串
 /// @note 该函数不支持格式化字符串中的%p和%n格式化符号
 extern void v_printf(const char *fmt, ...);
 
 /// @brief 直接字符串输出功能
 /// @param str 要输出的字符串\
 /// @note 该函数会直接输出字符串，不受vdbg_output_mode的值影响
 extern void vdirect_vputs(char *str);
 /// @brief 直接字符输出功能
 /// @param ch 要输出的字符
 /// @return 返回输出的字符
 /// @note 该函数会直接输出字符，不受vdbg_output_mode的值影响
 extern int vdirect_vputc(int ch);

 /// @brief 直接格式化输出函数
 /// @param fmt 格式化字符串
 /// @note 该函数不支持格式化字符串中的%p和%n格式化符号
 extern void vdirect_printf(const char *fmt, ...);

/**@}*/

#ifdef __cplusplus
}
#endif

#ifdef ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDIO_H_
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDIO_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDIO_H_ */
#endif /* _VSTDIO_H_ */
