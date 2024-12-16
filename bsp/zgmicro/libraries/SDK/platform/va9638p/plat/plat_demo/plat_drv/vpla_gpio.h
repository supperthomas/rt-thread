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
        vpla gpio

Description:
        gpio Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_GPIO_H_
#define _VPLA_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "vstdlib.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

#define VSYS_GPIO_PIN_0                 		(1<<0)
#define VSYS_GPIO_PIN_1                 		(1<<1)
#define VSYS_GPIO_PIN_2                 		(1<<2)
#define VSYS_GPIO_PIN_3                 		(1<<3)
#define VSYS_GPIO_PIN_4                 		(1<<4)
#define VSYS_GPIO_PIN_5                 		(1<<5)
#define VSYS_GPIO_PIN_6                 		(1<<6)
#define VSYS_GPIO_PIN_7                 		(1<<7)
#define VSYS_GPIO_PIN_8                 		(1<<8)
#define VSYS_GPIO_PIN_9                 		(1<<9)
#define VSYS_GPIO_PIN_10                        (1<<10)
#define VSYS_GPIO_PIN_11                        (1<<11)
#define VSYS_GPIO_PIN_12                        (1<<12)
#define VSYS_GPIO_PIN_13                        (1<<13)
#define VSYS_GPIO_PIN_14                        (1<<14)
#define VSYS_GPIO_PIN_15                        (1<<15)
#define VSYS_GPIO_PIN_16                        (1<<16)
#define VSYS_GPIO_PIN_17                        (1<<17)
#define VSYS_GPIO_PIN_18                        (1<<18)
#define VSYS_GPIO_PIN_19                        (1<<19)
#define VSYS_GPIO_PIN_20                        (1<<20)
#define VSYS_GPIO_PIN_21                        (1<<21)
#define VSYS_GPIO_PIN_22                        (1<<22)
#define VSYS_GPIOL_PIN_0                        ((UINT64)(1<<0)<<32)
#define VSYS_GPIOL_PIN_1                        ((UINT64)(1<<1)<<32)
#define VSYS_GPIOL_PIN_2                        ((UINT64)(1<<2)<<32)
#define VSYS_GPIOL_PIN_3                        ((UINT64)(1<<3)<<32)
#define VSYS_GPIOL_PIN_4                        ((UINT64)(1<<4)<<32)
#define VSYS_GPIOL_PIN_5                        ((UINT64)(1<<5)<<32)
#define VSYS_GPIOL_PIN_6                        ((UINT64)(1<<6)<<32)
#define VSYS_GPIOL_PIN_7                        ((UINT64)(1<<7)<<32)
#define VSYS_GPIOL_PIN_8                        ((UINT64)(1<<8)<<32)
#define VSYS_GPIOL_PIN_9                        ((UINT64)(1<<9)<<32)
#define VSYS_GPIOL_PIN_10                       ((UINT64)(1<<10)<<32)
#define VSYS_GPIOL_PIN_11                       ((UINT64)(1<<11)<<32)
#define VSYS_GPIOL_PIN_12                       ((UINT64)(1<<12)<<32)
#define VSYS_GPIOL_PIN_13                       ((UINT64)(1<<13)<<32)
#define VSYS_GPIOL_PIN_14                       ((UINT64)(1<<14)<<32)
#define VSYS_GPIOL_PIN_15                       ((UINT64)(1<<15)<<32)
#define VSYS_GPIOL_PIN_16                       ((UINT64)(1<<16)<<32)


#define VSYS_GPIO_PIN_X(x)          ((UINT64)0x01<<(x))
#define VSYS_GPIO_L_PIN_X(x)        ((UINT64)0x01<<((x)+32))
#define VSYS_GPIO_L_PIN_S(mask)        ((UINT64)mask<<32)
extern SINT8 VSYS_GPIO_Get_Index(UINT64 Pins);
#define VSYS_GPIO_GET_INDEX(pin)       VSYS_GPIO_Get_Index(pin)

#define VSYS_GPIOX_PIN_ALL           ((UINT64)((VSYS_GPIO_PIN_22<<1) -1) | ((UINT64)((VSYS_GPIOL_PIN_16<<1) -1))<<32)  /* All pins selected */

#define VSYS_GPIO_PWRON             (VSYS_GPIO_PIN_0)
#define VSYS_GPIO_SWO               (VSYS_GPIO_PIN_5|VSYS_GPIO_PIN_6)
#define VSYS_GPIO_BOOTSEL           ((UINT64)1<<31<<32)


    /// @brief 定义一个枚举类型VDRVGpioMode_t，用于表示GPIO的工作模式。
    typedef enum
    {
        /// @brief 输出模式
        VSYS_GPIO_MODE_OUTPUT = 0,
        /// @brief 输入模式
        VSYS_GPIO_MODE_INPUT = 1,
        /// @brief 高阻抗模式
        VSYS_GPIO_MODE_Z = 2,
    } VDRVGpioMode_t;

    /// @brief 定义GPIO输入输出上拉下拉反转等配置模式选择的枚举类型
    typedef enum
    {
        /// @brief 模式0：为正常模式，并且使能上下拉功能
        VSYS_GPIO_INPUT_NORMAL = 0,
        /// @brief 模式1：为引脚翻转模式，并且使能上下拉功能
        VSYS_GPIO_INPUT_INVERT = 1,
        /// @brief 模式2：为正常模式，并且禁用上下拉功能
        VSYS_GPIO_INPUT_DISPULL = 2,
        /// @brief 模式3：为引脚翻转模式，并且禁用上下拉功能；
        VSYS_GPIO_INPUT_INVERT_DISPULL = 3,
        /// @brief 模式4：低输出强度
        VSYS_GPIO_OUTPUT_LOW_STRENGTH = 0,
        /// @brief 模式5：高输出强度
        VSYS_GPIO_OUTPUT_HIGH_STRENGTH = 1,
        /// @brief 模式：开漏输出模式
		VSYS_GPIO_OUTPUT_OD_MODE = 2,
        /// @brief 模式6：正常高阻态
        VSYS_GPIO_Z_NORMAL = 0,
    } VDRVGpioModeParam_t;

    /// @brief 中断类型
    typedef enum
    {
        /// @brief 中断类型 禁能
        VSYS_GPIO_INTTYPE_DISABLE = 0,
        /// @brief 中断类型 普通模式
        VSYS_GPIO_INTTYPE_NORMAL_MODE = 1,
        /// @brief 中断类型 翻转模式
        VSYS_GPIO_INTTYPE_INVERT_MODE = 2,
    } VDRVGpioIntMode_t;

    /// @brief 定义GPIO的结构体和枚举类型
    typedef struct
    {
        /// @brief GPIO引脚
        UINT32 pins;
        /// @brief GPIO方向类型的枚举
        enum
        {
            /// @brief 顶部选择
            VSYS_GPIO_UMSEL_TOP = 0,
            /// @brief 编解码选择
            VSYS_GPIO_UMSEL_CODEC,
            /// @brief USB选择
            VSYS_GPIO_UMSEL_USB,
            /// @brief 计时器选择
            VSYS_GPIO_UMSEL_TMR,
            /// @brief I2C选择
            VSYS_GPIO_UMSEL_I2C,
            /// @brief UART选择
            VSYS_GPIO_UMSEL_UART,
            /// @brief DSP选择
            VSYS_GPIO_UMSEL_DSP,
            /// @brief 调制解调器选择
            VSYS_GPIO_UMSEL_MODEM,
            /// @brief RFC选择
            VSYS_GPIO_UMSEL_RFC,
            /// @brief RFFE选择
            VSYS_GPIO_UMSEL_RFFE,
            /// @brief BB选择
            VSYS_GPIO_UMSEL_BB = 10,
            /// @brief DMA选择
            VSYS_GPIO_UMSEL_DMA,
            /// @brief CM3选择
            VSYS_GPIO_UMSEL_CM3,
            /// @brief SPI选择
            VSYS_GPIO_UMSEL_SPI,
            /// @brief BBLE选择
            VSYS_GPIO_UMSEL_BBLE,
            /// @brief 缓存选择
            VSYS_GPIO_UMSEL_CACHE,
            /// @brief SD选择
            VSYS_GPIO_UMSEL_SD,
            /// @brief LCD选择
            VSYS_GPIO_UMSEL_LCDC,
            /// @brief CKM选择
            VSYS_GPIO_UMSEL_CKM,
            /// @brief HDCPORT选择
            VSYS_GPIO_UMSEL_HDCPORT,
        } module;
        /// @brief 用于标识GPIO监控器的信号
        UINT8 sig;
    } VDRVGpioUmonitor_t;


 /**
 * @addtogroup Vpla_GPIO
 * 
 * @{
 */
    
    ///@brief 将芯片的引脚初始化为GPIO功能
    ///@param Pins 需要初始化、支持或操作的引脚
    ///@param Mode GPIO 模式
    ///@param Param gpio 参数
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_GPIO_Init(UINT64 Pins, VDRVGpioMode_t Mode, VDRVGpioModeParam_t Param);

    ///@brief gpio 去初始化
    ///@param Pins 需要去初始化、支持或操作的引脚
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_GPIO_Uninit(UINT64 Pins);

    ///@brief 获取gpio的输入状态
    ///@param Pins 需要操作的gpio，支持还是操作
    ///@return GPIO的输入状态 
    extern UINT64 VSYS_GPIO_GetPinInput(UINT64 Pins);

    ///@brief 设置GPIO的输出状态
    ///@param Pins 需要操作的gpio，支持还是操作
    ///@param BitVal 要设置的状态
    ///@return VRET_VALUE 错误代码。 
    extern VRET_VALUE VSYS_GPIO_SetPinOutput(UINT64 Pins, BOOL BitVal);

    ///@brief 设置GPIO中断函数
    ///@param Pins 需要操作的gpio，支持还是操作
    ///@param type 中断模式
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_GPIO_EnableInt(UINT64 Pins, VDRVGpioIntMode_t type);

    ///@brief 将 GPIO 设置为监控模式
    ///@param config 配置参数
    ///@return VRET_VALUE 错误代码。 
    extern VRET_VALUE VSYS_GPIO_SetUmonitor(VDRVGpioUmonitor_t *config);
/**@}*/

#ifdef __cplusplus
}
#endif 


#endif  /* _VPLA_GPIO_H_ */
