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
        vpla led

Description:
        led Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_LED_H_
#define _VPLA_LED_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */


/// @brief 定义 LED 的索引枚举
typedef enum {
    /// @brief LED 索引 0
    VSYS_LED_INDEX_0 = 0,
    /// @brief LED 索引 1
    VSYS_LED_INDEX_1 = 1,
    /// @brief LED 索引 2
    VSYS_LED_INDEX_2 = 2,
    /// @brief LED 索引 3
    VSYS_LED_INDEX_3 = 3,
    /// @brief LED 总数
    VSYS_LED_TOTAL_NUM,
} VDRVLEDSel_t;

/// @brief LED模式枚举类型
typedef enum {
    /// @brief LED常灭模式
    VSYS_LED_ALWAYS_QUENCH_MODE = 0,
    /// @brief LED常亮模式
    VSYS_LED_ALWAYS_LIGHT_MODE,
    /// @brief LED闪烁模式
    VSYS_LED_FLASH_MODE,
} VDRVLEDMode_t;

/**
 * @brief LED配置结构体
 */
typedef struct {
    /// @brief LED亮的时间长度,单位时间是8ms，精度也是8ms
    UINT16 on_length;
    /// @brief LED灭的时间长度,单位时间是8ms，精度也是8ms
    UINT16 off_length;
    /// @brief LED亮灭电平,TRUE表示off为高电平，FALSE表示off为低电平，
    BOOL onoff_level;
    /// @brief LED亮灭次数
    UINT8 onoff_times;
    /// @brief 间隔状态,TRUE表示为on，FALSE表示为off，
    BOOL intvl_state;
    /// @brief 间隔长度,单位时间是8ms，精度也是8ms
    UINT32 intvl_length;
    /// @brief 重复次数
    UINT8 repeat_times;
} VDRVLEDConfig_t;


 /**
 * @addtogroup Vpla_LED
 * 
 * @{
 */
/// @brief 硬件LED灯初始化
/// @param ledx led灯的序号
/// @return 0，初始化成功；其它 初始化失败
extern VRET_VALUE VSYS_LED_Init(VDRVLEDSel_t ledx);

/// @brief 硬件LED灯反初始化
/// @param ledx led灯的序号
/// @return 0，反初始化成功；其它 反初始化失败
extern VRET_VALUE VSYS_LED_Uninit(VDRVLEDSel_t ledx);

/// @brief 硬件LED灯闪亮模式配置和选择
/// @param ledx led灯的序号
/// @param led_config led亮灭时隙配置信息
/// @return 0，成功；其它 失败
extern VRET_VALUE VSYS_LED_SetMode(VDRVLEDSel_t ledx, VDRVLEDConfig_t *led_config);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_LED_H_ */
