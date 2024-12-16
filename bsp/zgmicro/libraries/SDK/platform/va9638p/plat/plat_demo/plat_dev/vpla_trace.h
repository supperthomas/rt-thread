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
        vpla trace

Description:
        trace Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_TRACE_H_
#define _VPLA_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "vstdlib.h"

 /**
 * @addtogroup Vpla_TRACE
 * 
 * @{
 */

/// @brief 使能SW口，使用JLINK进行调试:GPIO_L_PIN_10 GPIO_L_PIN_11
/// @param en 真，使能；假，禁能
/// @return 0，使能成功；其它，异常
extern VRET_VALUE VSYS_DebugPort_Enable(BOOL en);

/// @brief 使能 trace接口：GPIO_L_PIN_0 /GPIO_L_PIN_1 /GPIO_PIN_9 /GPIO_PIN_10
/// @param en 真，使能；假，禁能
/// @return 0，使能成功；其它，异常
extern VRET_VALUE VSYS_TracePort_Enable(BOOL en);

/// @brief 实时的记录目标的执行情况功能sysview 初始化 通过jilnk链接进行调试
/// @param id User defined ID for the marker
/// @param name Pointer to the marker name. (Max. SEGGER_SYSVIEW_MAX_STRING_LEN Bytes)
/// @return 0，成功；其它，异常
extern VRET_VALUE VSYS_SYSVIEW_MarkInit(UINT32 id, char *name);

/// @brief Record a Performance Marker Start event to start measuring runtime
/// @param id User defined ID for the marker
/// @return 0，成功；其它，异常
extern VRET_VALUE VSYS_SYSVIEW_MarkStart(UINT32 id);

/// @brief Record a Performance Marker Stop event to stop measuring runtime.
/// @param id User defined ID for the marker
/// @return 成功；其它，异常
extern VRET_VALUE VSYS_SYSVIEW_MarkStop(UINT32 id);

/// @brief Record a Performance Marker intermediate event.
/// @param id User defined ID for the marker
/// @return 成功；其它，异常
extern VRET_VALUE VSYS_SYSVIEW_Mark(UINT32 id);

/// @brief 更新系统资源信息.
/// @param p_mem 内存信息头
/// @param usage_updata 是否更新cpu使用率
/// @return 成功；其它，异常
extern VRET_VALUE VSYS_ResInfo_Update(UINT32 *p_mem, BOOL usage_updata);

/**@}*/

#ifdef __cplusplus
}
#endif 


#endif  /* _VPLA_TRACE_H_ */
