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
        vpla tmier

Description:
        tmier Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_TIMER_H_
#define _VPLA_TIMER_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

typedef UINT32      VDRVTMR_US;

/// @brief 定义了一个枚举类型 VDRVTMRSel_t，用于选择不同的定时器模式。
typedef enum {
    /// @brief 微秒级的低速FCLKL定时器模式
    VSYS_TMR_FCLKL_us = 0,
    /// @brief 微秒级的普通FCLKS定时器模式
    VSYS_TMR_FCLKS_us = 1,
    /// @brief 四分之一秒级的SCLK定时器模式
    VSYS_TMR_SCLK_quarus = 2,
    /// @brief 微秒级的高速FCLKH定时器模式
    VSYS_TMR_FCLKH_us = 4,
} VDRVTMRSel_t;

/// @brief 定时器回调函数
typedef void (* VDRVTMR_Callback)(VDRVTMRSel_t);

/// @brief 定时器配置信息
typedef struct{
    /// @brief 周期
    UINT32  Period;
    /// @brief 自动重载
    BOOL    AutoReload;
    /// @brief 使能中断
    BOOL    EnableInt;
    /// @brief 回调函数
    VDRVTMR_Callback TMRCallback;
}VDRVTMRConfig_t;

/// @brief 定义一个枚举类型 VDRVTMRStatus_t，用于表示定时器的各种状态
typedef enum {
    /// @brief 定时器计数值状态
    VSYS_TMR_CntValue_Status = 0,
    /// @brief 定时器是否运行状态
    VSYS_TMR_IsRunning_Status,
    /// @brief 定时器中断状态
    VSYS_TMR_Interrupt_Status,
} VDRVTMRStatus_t;


 /**
 * @addtogroup Vpla_TIMER
 * 
 * @{
 */

/// @brief 定时器初始化
/// @param timer 时钟选择
/// @param tmr_config 配置信息
/// @return 0，成功；其他 失败
extern VRET_VALUE VSYS_TIMER_Init(VDRVTMRSel_t timer, VDRVTMRConfig_t *tmr_config);

/// @brief 定时器反初始化
/// @param timer 时钟选择
/// @return 0，成功；其他 失败
extern VRET_VALUE VSYS_TIMER_Uninit(VDRVTMRSel_t timer);

/// @brief 定时器开始
/// @param timer 时钟选择
/// @return 0，成功；其他 失败
extern VRET_VALUE VSYS_TIMER_Start(VDRVTMRSel_t timer);

/// @brief 定时器停止
/// @param timer 时钟选择
/// @return 0，成功；其他 失败
extern VRET_VALUE VSYS_TIMER_Stop(VDRVTMRSel_t timer);

/// @brief 获得定时器各种状态
/// @param timer 时钟选择
/// @param status 定时的状态
/// @return 获得定时器各种状态，具体参考结构体信息
extern UINT32 VSYS_TIMER_GetStatus(VDRVTMRSel_t timer,VDRVTMRStatus_t status);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_TIMER_H_ */
