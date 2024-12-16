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
        vpla wd

Description:
        wd Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_WD_H_
#define _VPLA_WD_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "vstdlib.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */ 

/// @brief 定义了一个枚举类型 VDRVWDTSel_t，用于选择不同的看门狗
typedef enum {
    /// @brief 硬件看门狗
    VDRVWDT_HW = 0,
    /// @brief 软件看门狗，需要配合硬件看门狗使用
    VDRVWDT_SW = 1,
    /// @brief 看门狗总数
    VDRVWDT_LIMIT,
} VDRVWDTSel_t;

/// @brief 看门狗回调函数
typedef void (* VDRVWDT_Callback)(VDRVWDTSel_t);

/// @brief 看门狗模式
typedef enum {
	VDRVWDT_MODE_RESET = 0,
	VDRVWDT_MODE_NMI = 1
}VDRVWDTMode_e;

/// @brief 看门狗配置信息
typedef struct{
    /// @brief 模式选择
    VDRVWDTMode_e Mode;
    /// @brief 超时时间
    UINT32  timeout_ms;
    /// @brief 回调函数
    VDRVWDT_Callback WDTCallback;
}VDRVWDTConfig_t;

/// @brief 定义一个枚举类型 VDRVWDTStatus_t，用于表示看门狗的各种状态
typedef enum {
    /// @brief 看门狗计数值状态
    VSYS_WDT_CntValue_Status = 0,
    /// @brief 看门狗是否运行状态
    VSYS_WDT_IsRunning_Status,
    /// @brief 看门狗超时状态
    VSYS_WDT_Timeout_Status,
} VDRVWDTStatus_t;


 /**
 * @addtogroup Vpla_WDT
 * 
 * @{
 */

/// @brief 看门狗初始化函数，用于设置和配置看门狗定时器。
/// @param wdt 看门狗选择，指定使用的看门狗类型。
/// @param wdt_config 配置信息，包含初始化所需的各种参数。
/// @return 0，成功；其他 失败，返回错误码。
/// @note 该函数需要在系统初始化之前调用，且只能调用一次。
extern VRET_VALUE VSYS_WDT_Init(VDRVWDTSel_t wdt, VDRVWDTConfig_t *wdt_config);

/// @brief 看门狗反初始化
/// @param wdt 看门狗选择
/// @return 0，成功；其他 失败
/// @note 该函数需要在系统退出之前调用，且只能调用一次。
extern VRET_VALUE VSYS_WDT_Uninit(VDRVWDTSel_t wdt);

/// @brief 看门狗使能
/// @param wdt 看门狗选择
/// @return 0，成功；其他 失败
/// @note 该函数用于使能看门狗，使其正常工作。
extern VRET_VALUE VSYS_WDT_Enable(VDRVWDTSel_t wdt);

/// @brief 看门狗禁止
/// @param wdt 看门狗选择
/// @return 0，成功；其他 失败
/// @note 该函数用于禁止看门狗，使其停止工作。
extern VRET_VALUE VSYS_WDT_Disable(VDRVWDTSel_t wdt);

/// @brief 看门狗喂狗
/// @param wdt 看门狗选择
/// @return 0，成功；其他 失败
/// @note 该函数用于喂狗，使看门狗重新开始计时。
extern VRET_VALUE VSYS_WDT_Feed(VDRVWDTSel_t wdt);

/// @brief 获得看门狗各种状态
/// @param wdt 看门狗选择
/// @param status 看门狗的状态
/// @return 获得看门狗各种状态，具体参考结构体信息
/// @note 该函数用于获得看门狗各种状态，包括计数值、是否运行、超时状态等。
extern UINT32 VSYS_WDT_GetStatus(VDRVWDTSel_t wdt,VDRVWDTStatus_t status);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_WD_H_ */
