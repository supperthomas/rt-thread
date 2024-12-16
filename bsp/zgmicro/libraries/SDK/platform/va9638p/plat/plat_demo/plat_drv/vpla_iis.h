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
		vpla aif

Description:
		irq Driver
Revision History:
		Date                  Author                   Description
	2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_IIS_H_
#define _VPLA_IIS_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

	/// @brief IIS输入输出通道选择
	typedef enum
	{
		VSYS_IIS_CH0IN = 0,
		VSYS_IIS_CH0OUT = 1,
		VSYS_IIS_CH1IN = 2,
		VSYS_IIS_CH1OUT = 3,
		VSYS_IIS_CH_LIMIT,
	} VDRVIISSel_t;

	/// @brief IIS 主从选择
	typedef enum
	{
		VSYS_IIS_SLAVE_MODE = 0,
		VSYS_IIS_MASTER_MODE = 1,
	} VDRVIISMode_t;

	/// @brief 位宽选择
	typedef enum
	{
		VSYS_IIS_WIDTH_BIT24 = 0,
		VSYS_IIS_WIDTH_BIT16 = 1,
		VSYS_IIS_WIDTH_BIT32 = 2,
	} VDRVIISDwidth_t;

	/// @brief 采样率选择
	typedef enum
	{
		VSYS_IIS_SR_8000 = 8000,
		VSYS_IIS_SR_11025 = 11025,
		VSYS_IIS_SR_12000 = 12000,
		VSYS_IIS_SR_16000 = 16000,
		VSYS_IIS_SR_22050 = 22050,
		VSYS_IIS_SR_24000 = 24000,
		VSYS_IIS_SR_44100 = 44100,
		VSYS_IIS_SR_48000 = 48000,
		VSYS_IIS_SR_88200 = 88200,
		VSYS_IIS_SR_96000 = 96000,
		VSYS_IIS_SR_192000 = 192000,
		VSYS_IIS_SR_384000 = 384000,
	} VDRVIISSampleRate_t;

	/// @brief IIS 格式配置
	typedef enum
	{
		VSYS_IIS_FORMAT_STANDARD = 0,
		VSYS_IIS_FORMAT_LEFTJUSTIFIED = 2,
		VSYS_IIS_FORMAT_RIGHTJUSTIFIED = 3,
	} VDRVIISFormat_t;

	/// @brief IIS 回调函数
	typedef void (*VDRVIIS_ISR_CALLBACK)(UINT32 ErrorCode);

	/// @brief IIS 通道使能
	typedef enum
	{
		VSYS_IIS_BOTH_CHANNEL_DISABLE = 0,
		VSYS_IIS_LEFT_CHANNEL_ENABLE = 1,
		VSYS_IIS_RIGHT_CHANNEL_ENABLE = 2,
		VSYS_IIS_BOTH_CHANNEL_ENABLE = 3,
	} VDRVIISChannel_t;

	/// @brief IIS 数据模式
	typedef enum
	{
		VSYS_IIS_DataMode_CROSS = 0,
		VSYS_IIS_DataMode_HEADTAIL = 1,
	} VDRVIISDataMode_t;

	/// @brief IIS配置
	typedef struct
	{
		/// @brief 长度
		size_t len;
		/// @brief 缓存
		UINT8* buff;
		/// @brief 模式
		VDRVIISMode_t mode;
		/// @brief 位宽
		VDRVIISDwidth_t dwidth;
		/// @brief 采样率
		VDRVIISSampleRate_t SR;
		/// @brief 格式
		VDRVIISFormat_t format;
		/// @brief 通道
		VDRVIISChannel_t channel;
		/// @brief 回调
		VDRVIIS_ISR_CALLBACK callback;
		/// @brief 数据模式
		VDRVIISDataMode_t data_mode;
	} VDRVIISConfig_t;

 /// @brief 定义了枚举类型 VDRVIISInfo_t，用于描述IIS的各种状态和控制属性
  typedef enum
  {
   /// @brief 用于表示IIS的音频源
   VSYS_IISASRC = 0,
   /// @brief 用于表示IIS的状态
   VSYS_IISState,
   /// @brief 用于表示IIS的时钟保持
   VSYS_IISKeepClk,
   /// @brief 用于表示IIS的同步设置
   VSYS_IISSyncSet,
   /// @brief 用于表示IIS的同步使能
   VSYS_IISSyncEn,
   /// @brief 用于获取IIS的空闲缓存
   VSYS_IISFreeCache,
  } VDRVIISInfo_t;


 /**
 * @addtogroup Vpla_IIS
 * 
 * @{
 */

	/// @brief IIS 接口初始化
	/// @param iis_sel 选择输入输出通道
	/// @param iis_config 配置IIS 驱动结构体信息
	/// @return 0，初始化成功；其它值 初始化失败
	extern VRET_VALUE VSYS_IIS_Init(VDRVIISSel_t iis_sel, VDRVIISConfig_t *iis_config);

	/// @brief IIS 接口反初始化
	/// @param iis_sel 选择输入输出通道
	/// @return 0，反初始化成功；其它值反初始化失败
	extern void* VSYS_IIS_Uninit(VDRVIISSel_t iis_sel);

	/// @brief 使能IIS 接口功能
	/// @param iis_sel 选择输入输出通道
	/// @param en 1，使能；0 ，禁能
	extern void VSYS_IIS_Enable(VDRVIISSel_t iis_sel, BOOL en);

	/// @brief 更新IIS 读写DMA buffer
	/// @param iis_sel 选择输入输出通道
	/// @param data 读取和写入的数据缓存
	/// @param size 数据长度大小
	/// @return 0，更新成功；其它 更新过程异常
	extern size_t VSYS_IIS_Update(VDRVIISSel_t iis_sel, UINT8 *data, size_t size);

	/// @brief 获得使能的IIS 通道信息
	/// @param iis_sel 选择输入输出通道
	/// @param info IIS 的状态信息等
	/// @return 获得的某个IIS通道使能信息
	extern UINT32 VSYS_IIS_GetParam(VDRVIISSel_t iis_sel, VDRVIISInfo_t info);

	/// @brief 设置使能的IIS 通道信息
	/// @param iis_sel 选择输入输出通道
	/// @param info IIS 的状态信息等
	/// @param val 设置IIS的状态值
	/// @return 0，设置成功；其它值 设置过程异常
	extern VRET_VALUE VSYS_IIS_SetParam(VDRVIISSel_t iis_sel, VDRVIISInfo_t info, UINT32 val);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_IIS_H_ */
