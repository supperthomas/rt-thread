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
#ifndef _VPLA_CKM_H_
#define _VPLA_CKM_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "vstdlib.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

#define MCUBT_RST_MAGIC_NUM     (0x96381234)
	
/// @brief BT核系统时钟结构
typedef enum
{
	/// @brief 24MHz
	VSYS_CKM_SYSCLK_24MHZ = (24000000L),
	/// @brief 39MHz
	VSYS_CKM_SYSCLK_39MHZ = (39075000L),
	/// @brief 48MHz
	VSYS_CKM_SYSCLK_48MHZ = (48000000L),
	/// @brief 64MHz
	VSYS_CKM_SYSCLK_64MHZ = (64000000L),
	/// @brief 78MHz
	VSYS_CKM_SYSCLK_78MHZ = (78150000L),
	/// @brief 96MHz
	VSYS_CKM_SYSCLK_96MHZ = (96000000L),
	/// @brief 128MHz
	VSYS_CKM_SYSCLK_128MHZ = (128000000L),
	/// @brief 156MHz
	VSYS_CKM_SYSCLK_156MHZ = (156300000L),
	/// @brief 192MHz
	VSYS_CKM_SYSCLK_192MHZ = (192000000L)
} VDRVCKMSysClk_t;

/// @brief Apb 分频参数
typedef enum
{
	/// @brief 	APB 分频 2
	VSYS_CKM_APB_DIV_2 = 2,
	/// @brief APB 分频 4
	VSYS_CKM_APB_DIV_4 = 4
} VDRVCKMApbDiv_t;

/// @brief DSP的系统时钟选择
typedef enum
{
	/// @brief 26Mhz
	VSYS_CKM_DSPCLK_26MHZ = (26000000L),
	/// @brief 128MHz
	VSYS_CKM_DSPCLK_128MHZ = (128000000L),
	/// @brief 153.6MHz
	VSYS_CKM_DSPCLK_153_6MHZ = (153600000L),
	/// @brief 192MHz
	VSYS_CKM_DSPCLK_192MHZ = (192000000L),
	/// @brief 256MHz
	VSYS_CKM_DSPCLK_256MHZ = (256000000L),
	/// @brief 384MHz
	VSYS_CKM_DSPCLK_384MHZ = (384000000L)
} VDRVCKMDspClk_t;

/// @brief 系统时钟切换开关
typedef enum
{
	/// @brief 开关高时钟
	VSYS_SwitchHighClock = 0,
	/// @brief 开关普通时钟
	VSYS_SwitchNormalClock,
	/// @brief 开关睡眠时钟
	VSYS_SwitchSleepClock,
} VDRVCKMSwitch_t;

/// @brief DPLL 参数配置
typedef struct
{
	/// @brief r11
	UINT32 r11;
	/// @brief r15
	UINT32 r15;
	/// @brief r16
	UINT32 r16;
	/// @brief 参考选择
	UINT8 ref_sel;
} VDRVCKMDPLLConfig_t;

/// @brief 时钟配置结构体，包括系统时钟，串口等。
typedef struct
{
	/// @brief 系统时钟频率
	UINT32 sys_clock_freq;
	/// @brief apb 分频
	UINT8 apb_divider;
	/// @brief 定时器分频
	UINT8 timer_divider;
	/// @brief 串口分频
	UINT8 uart_divider;
	/// @brief 32k src 
	UINT8 clock_32k_src;
	/// @brief 系统高频时钟
	UINT32 sys_clock_freq_high;
	/// @brief 晶振频率
	UINT32 xtal_freq;
	/// @brief 定义dpll 配置
	VDRVCKMDPLLConfig_t dpll_cfg;
	/// @brief 48M
	UINT8 clk48m_inv_sel;
	/// @brief 24M
	UINT8 clk24m_inv_sel;
	/// @brief apb 分频高
	UINT8 apb_divider_high;
} VDRVCKMConfig_t;

/// @brief 时钟信息结构体
typedef enum
{
	/// @brief 系统时钟频率
	VSYS_SysClockFreq = 0,
	/// @brief APB时钟频率
	VSYS_ApbClockFreq,
	/// @brief I2c 时钟频率
	VSYS_I2cClockFreq,
	/// @brief 定时器 时钟频率 
	VSYS_TimerClockFreq,
	/// @brief 串口 时钟频率
	VSYS_UartClockFreq,
	/// @brief 系统开关时钟
	VSYS_SwitchClock,
	/// @brief APB 时钟分频
	VSYS_ApbClockFreq_div,
	/// @brief top 核时钟使能
	VSYS_TopClockEnable,
	/// @brief BT 核时钟使能
	VSYS_BTClockEnable,
	/// @brief DSP 核时钟使能
	VSYS_DSPClockEnable,
	/// @brief 设置时钟保护
	VSYS_CKMSetProtect,
	/// @brief DSP 时钟频率
	VSYS_DspClockFreq,
} VDRVCKMInfo_t;

/// @brief 时钟频率结构体
typedef union
{
	/// @brief BT系统频率
	VDRVCKMSysClk_t sys_freq;
	/// @brief dsp时钟频率
	VDRVCKMDspClk_t dsp_freq;
	/// @brief apb 分频
	VDRVCKMApbDiv_t apb_div;
	/// @brief 开关
	VDRVCKMSwitch_t ckm_switch;
	/// @brief 使能
	BOOL en;
	/// @brief block
	UINT16 block_mask;
} VDRVCKMParam_t;

extern VDRVCKMConfig_t g_VSYS_VCKM_Config;

 /**
 * @addtogroup Vpla_CKM
 * 
 * @{
 */

///@brief 芯片时钟已初始化
///参数为默认值g_VSYS_VCKM_Config
///@return VRET_VALUE 错误代码。
extern VRET_VALUE VSYS_CKM_Init(void);

///@brief 芯片时钟被取消初始化，恢复为晶体时钟 
///@return VRET_VALUE 错误代码。
extern VRET_VALUE VSYS_CKM_Uninit(void);

///@brief 获取有关时钟的信息
///@param info 获取时钟的哪一部分信息
///@return 有关时钟的信息
extern UINT32 VSYS_CKM_GetParam(VDRVCKMInfo_t info);

///@brief 设置时钟信息参数
///@param info 要设置时钟的哪一部分信息
///@param val 设置时钟信息值
///@return VRET_VALUE 错误代码。
extern VRET_VALUE VSYS_CKM_SetParam(VDRVCKMInfo_t info, /*VDRVCKMParam_t*/UINT32 val);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_CKM_H_ */
