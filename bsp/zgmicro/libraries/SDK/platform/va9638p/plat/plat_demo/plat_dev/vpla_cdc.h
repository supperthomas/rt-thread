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
        vpla cdc

Description:
        cdc Device
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_CDC_H_
#define _VPLA_CDC_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

 /// @brief VDRVCDCSel_t枚举，表示V系统CDC通道选择
 typedef enum
 {
  /// @brief CDC通道0输入
  VSYS_CDC_CH0IN = 0,
  /// @brief CDC通道0输出
  VSYS_CDC_CH0OUT = 1,
  /// @brief CDC通道选择限制
  VSYS_CDC_CH_LIMIT,
 } VDRVCDCSel_t;

 /// @brief 定义 VSYS_CDC_WIDTH_BIT 的枚举类型，用于表示不同位宽的常量
 typedef enum
 {
  /// @brief 24 位宽
  VSYS_CDC_WIDTH_BIT24 = 0,
  /// @brief 16 位宽
  VSYS_CDC_WIDTH_BIT16 = 1,
  /// @brief 32 位宽
  VSYS_CDC_WIDTH_BIT32 = 1,
 } VDRVCDCDwidth_t;

 /// @brief 定义音频采样率的枚举类型
 typedef enum
 {
  /// @brief 8000 Hz 采样率
  VSYS_CDC_SR_8000 = 8000,
  /// @brief 16000 Hz 采样率
  VSYS_CDC_SR_16000 = 16000,
  /// @brief 24000 Hz 采样率
  VSYS_CDC_SR_24000 = 24000,
  /// @brief 48000 Hz 采样率
  VSYS_CDC_SR_48000 = 48000,
  /// @brief 96000 Hz 采样率
  VSYS_CDC_SR_96000 = 96000,
 } VDRVCDCSampleRate_t;

 /// @brief 定义了一个函数指针类型 VDRVCDC_ISR_CALLBACK，用于指向带有一个 UINT32 参数的函数，这个参数通常代表错误码
 typedef void (*VDRVCDC_ISR_CALLBACK)(UINT32 ErrorCode);

 /// @brief 定义用于表示声道选择的枚举类型
 typedef enum
 {
  /// @brief 禁用左右声道
  VSYS_CDC_BOTH_CHANNEL_DISABLE = 0,
  /// @brief 启用左声道
  VSYS_CDC_LEFT_CHANNEL_ENABLE = 1,
  /// @brief 启用右声道
  VSYS_CDC_RIGHT_CHANNEL_ENABLE = 2,
  /// @brief 启用左右声道
  VSYS_CDC_BOTH_CHANNEL_ENABLE = 3,
 } VDRVCDCChannel_t;

  /// @brief VDRVCDCDataMode_t 枚举类型定义了两个 CDC 数据模式
  typedef enum
  {
   /// @brief CROSS 数据模式，值为 0
   VSYS_CDC_DataMode_CROSS = 0,
   /// @brief HEADTAIL 数据模式，值为 1
   VSYS_CDC_DataMode_HEADTAIL = 1,
  } VDRVCDCDataMode_t;

/// @brief 音频驱动器配置结构体，该结构体用于配置音频驱动器的信息，包括缓存大小、数据模式、回调函数等。
 typedef struct {
     /// @brief 缓冲区长度
     size_t len;
     /// @brief 缓冲区指针
     UINT8* buff;
     /// @brief 数据宽度
     VDRVCDCDwidth_t dwidth;
     /// @brief 采样率
     VDRVCDCSampleRate_t SR;
     /// @brief 声道数
     VDRVCDCChannel_t channel;
     /// @brief 中断服务回调函数
     VDRVCDC_ISR_CALLBACK callback;
     /// @brief 数据模式
     VDRVCDCDataMode_t data_mode;
     /// @brief 控制模式，bit[0] 控制PA
     UINT8 ctrl_mode; // bit[0]:control PA, 0 means it will control pa register in interface; 1 means it doesn't control but fade pop will control pa register
 } VDRVCDCConfig_t;

 /// @brief 定义一个枚举类型 VDRVCDCInfo_t ，用于表示不同的 CDC 增益模式
 typedef enum
 {
  /// @brief CDC 增益模式 0
  VSYS_CDC_Gain = 0,
 } VDRVCDCInfo_t;


 /**
 * @addtogroup Vpla_CDC
 * 
 * @{
 */
	/// @brief Codec 初始化配置信息
	/// @param cdc_sel 输入输出通道选择
	/// @param CDC_config 配置信息结构体
	/// @return 0,成功；其它失败
	extern VRET_VALUE VSYS_CDC_Init(VDRVCDCSel_t cdc_sel, VDRVCDCConfig_t *CDC_config);

	/// @brief Codec 反初始化配置信息
	/// @param cdc_sel 输入输出通道选择
	/// @return NULL
	extern void* VSYS_CDC_Uninit(VDRVCDCSel_t cdc_sel);

	/// @brief 使能Codec 输入输出设备
	/// @param cdc_sel 输入输出通道选择
	/// @param en 1，使能；0，禁能
	extern void VSYS_CDC_Enable(VDRVCDCSel_t cdc_sel, BOOL en);

	/// @brief 更新输入输出数据从active buffer
	/// @param cdc_sel 输入输出通道选择
	/// @param data 数据缓存
	/// @param size 数据大小
	/// @return 返回实际数据大小
	extern size_t VSYS_CDC_Update(VDRVCDCSel_t cdc_sel, UINT8 *data, size_t size);

	/// @brief 获得输入输出使能信息，判断codec 是否再工作
	/// @param cdc_sel 输入输出通道选择
	/// @param info 增益信息
	/// @return 0 返回成功；其他失败
	extern UINT32 VSYS_CDC_GetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info);

	/// @brief 配置codec输入输出信息
	/// @param cdc_sel 输入输出通道选择
	/// @param info 增益信息
	/// @param val 配置数值
	/// @return 0 返回成功；其他失败
	extern VRET_VALUE VSYS_CDC_SetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info, UINT32 val);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_CDC_H_ */
