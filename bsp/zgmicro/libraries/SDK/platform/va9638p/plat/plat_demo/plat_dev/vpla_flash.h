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
        vpla FLASH

Description:
        FLASH Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_FLASH_H_
#define _VPLA_FLASH_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VSYS_FLASH_PAGE_SIZE        (256)
#define VSYS_FLASH_SECTOR_SIZE      (4*1024)
#define VSYS_FLASH_ALL_SIZE         (0xFFFFFFFF)

#define VSYS_FLASH_OTP_SIZE                        (1024)
#define VSYS_FLASH_OTP1_ADDR_BASE                  (0x01000)
#define VSYS_FLASH_OTP2_ADDR_BASE                  (0x02000)
#define VSYS_FLASH_OTP3_ADDR_BASE                  (0x03000)


/// @brief 闪存选择类型枚举定义
typedef enum  {
    /// @brief 无闪存
    VSYS_FLASH_NONE = 0,
    /// @brief 外部闪存
    VSYS_FLASH_EXTERNAL = 1,
    /// @brief 封装内闪存
    VSYS_FLASH_INPACKAGE = 2,
    /// @brief 同时使用外部闪存和封装内闪存
    VSYS_FLASH_BOTH = 3,
    /// @brief 外部闪存（OTP）
    VSYS_FLASH_EXTERNAL_OTP = 4,
    /// @brief 封装内闪存（OTP）
    VSYS_FLASH_INPACKAGE_OTP = 5,
}VDRVFLASHSel_t;

/// @brief 定义一个枚举类型 VDRVFLASHVoltage_t，用于表示不同的电压值
typedef enum  {
    /// @brief 无闪存电压
    VSYS_FLASH_NONE_VOL = 0,
    /// @brief 1.8V 闪存电压
    VSYS_FLASH_1_8v_VOL = 1,
    /// @brief 3.3V 闪存电压
    VSYS_FLASH_3_3v_VOL = 2,
}VDRVFLASHVoltage_t;

/// @brief 定义了一个枚举类型 VDRVFLASHInfo_t ，用于存储有关闪存的信息类型。
typedef enum  {
    /// @brief 闪存制造商ID
    VSYS_FLASH_MID = 0,
    /// @brief 闪存用户ID
    VSYS_FLASH_UID,
    /// @brief 闪存总大小
    VSYS_FLASH_TOTALSIZE,
    /// @brief 闪存电压
    VSYS_FLASH_VOLTAGE,
    /// @brief 闪存双模
    VSYS_FLASH_DUALMODE,
    /// @brief 闪存顶层基地址
    VSYS_FLASH_TOPBASE,
    /// @brief 闪存蓝牙基地址
    VSYS_FLASH_BTBASE,
    /// @brief 闪存DSP基地址
    VSYS_FLASH_DSPBASE,
    /// @brief 闪存加密密钥
    VSYS_FLASH_ENCRYPTKEY,
    /// @brief 闪存加密密钥2
    VSYS_FLASH_ENCRYPTKEY2,
    /// @brief 闪存时钟分频
    VSYS_FLASH_CLKDIV,
}VDRVFLASHInfo_t;

 /**
 * @addtogroup Vpla_FLASH
 * 
 * @{
 */

/// @brief SPI Flash 初始化
/// @return 0,成功；其它，失败
extern VRET_VALUE VSYS_FLASH_Init(void);

/// @brief SPI Flash 反初始化
/// @return 0,成功；其它，失败 
extern VRET_VALUE VSYS_FLASH_Uninit(void);

/// @brief SPI Flash 擦除
/// @param flash_sel flash 选择
/// @param addr 操作的起始地址
/// @param siz 擦除长度大小
/// @return 0,成功；其它，失败 
extern VRET_VALUE VSYS_FLASH_Erase(VDRVFLASHSel_t flash_sel, UINT32 addr, size_t siz);

/// @brief SPI Flash 将缓存数据写到某个地址中
/// @param flash_sel flash 选择
/// @param addr 操作的起始地址
/// @param buf 需要烧写的data 缓存起始地址
/// @param siz 烧写数据的长度
/// @return 0,成功；其它，失败
extern VRET_VALUE VSYS_FLASH_Program (VDRVFLASHSel_t flash_sel, UINT32 addr, const UINT8 *buf, size_t siz );

/// @brief SPI Flash 读取某个地址的内容
/// @param flash_sel flash 选择
/// @param addr 操作的起始地址
/// @param buf 缓存起始地址
/// @param siz 数据的长度
/// @return 0,成功；其它，失败
extern VRET_VALUE VSYS_FLASH_Read (VDRVFLASHSel_t flash_sel, UINT32 addr, UINT8 *buf, size_t siz );

/// @brief SPI Flash 获取配置信息
/// @param flash_sel flash 选择
/// @param info 需要获取的flash信息
/// @param buff 缓存数据的地址
/// @param size 数据长度
/// @return 0,成功；其它，失败
extern VRET_VALUE VSYS_FLASH_GetParam(VDRVFLASHSel_t flash_sel, VDRVFLASHInfo_t info, UINT8 *buff, size_t size);

/// @brief SPI Flash 设置配置信息
/// @param flash_sel  flash 选择
/// @param info 需要设置的flash信息
/// @param buff 缓存数据的地址
/// @param size 数据长度
/// @return 0,成功；其它，失败
extern VRET_VALUE VSYS_FLASH_SetParam(VDRVFLASHSel_t flash_sel, VDRVFLASHInfo_t info, UINT8 *buff, size_t size);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_FLASH_H_ */
