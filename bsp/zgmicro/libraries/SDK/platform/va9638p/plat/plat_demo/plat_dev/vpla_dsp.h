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
    croutine
Description:
     test

Revision History:
     Date                  Author                   Description
    2023.04.3            cxt   						create
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif

#ifndef _VPLA_DSP_H_
#define _VPLA_DSP_H_

#include "vstdlib.h"
#include "vpla_ckm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */

#define VDRV_DSP_DEBUG(...) // DEBUG_PRINTF("VDRV_DSP", __VA_ARGS__)

/// @brief 定义常量 VSYS_DSP_PMEM_CACHE_SIZE，表示 DSP（数字信号处理器）内存缓存的大小为 96 KB
#define VSYS_DSP_PMEM_CACHE_SIZE (96 * 1024)

    /// @brief VDRVDSPCfg_t 结构体定义，包含了与 DSP 配置相关的参数。
    typedef struct
    {
        /// @brief DSP 的起始地址
        UINT32 start_addr;
        /// @brief PMEM（程序存储器）的地址
        UINT32 pmem_addr;
        /// @brief PMEM 的大小
        UINT32 pmem_size;
        /// @brief XMEM 的地址（数据存储器）
        UINT32 xmem_addr;
        /// @brief XMEM 的大小
        UINT32 xmem_size;
        /// @brief YMEM 的地址（另一个数据存储器）
        UINT32 ymem_addr;
        /// @brief YMEM 的大小
        UINT32 ymem_size;
        /// @brief DSP 的频率时钟
        VDRVCKMDspClk_t DSPFreqClk;
        /// @brief DSP 密钥
        UINT16 dspkey;
        /// @brief 数据密钥
        UINT16 datakey;
    } VDRVDSPCfg_t;

    /// @brief 定义了一个枚举类型 VDRVDSPControl_t，用于表示不同的 DSP 控制命令
    typedef enum
    {
        /// @brief 暂停 DSP 操作 
        VSYS_DSP_HOLD = 0,
        /// @brief 重置 DSP
        VSYS_DSP_RESET,
        /// @brief 设置 DSP 中断
        VSYS_DSP_SETINT,
    } VDRVDSPControl_t;

    /// @brief 定义了VDRVDSPStatus_t枚举类型，用于表示DSP的不同状态
    typedef enum
    {
        /// @brief IO Hold User状态
        VSYS_DSP_IOHOLDUSER = 0,
        /// @brief Halt Acknowledge状态
        VSYS_DSP_HALTACK,
        /// @brief Self Interrupt Status状态
        VSYS_DSP_SELFINTSTAT,
    } VDRVDSPStatus_t;

    /// @brief 定义一个枚举类型 VDRVDSPHold_t，用于表示DSP的不同保持状态
    typedef enum
    {
        /// @brief 无保持状态
        VSYS_DSP_HOLD_NONE = 0,
        /// @brief IO保持状态
        VSYS_DSP_HOLD_IO = (0x01 << 0),
        /// @brief 核心保持状态
        VSYS_DSP_HOLD_CORE = (0x01 << 1),
        /// @brief DMS保持状态
        VSYS_DSP_HOLD_DMS = (0x01 << 2),
    } VDRVDSPHold_t;

    // VDRVDSPReset_t 枚举类型定义，表示DSP重置标志
    typedef enum
    {
        // DSS重置
        VSYS_DSP_RESET_DSS = (0x01 << 0),
        // DSP全局重置
        VSYS_DSP_RESET_DSP = (0x01 << 1),
        // DSP重启重置
        VSYS_DSP_RESET_RESTART = (0x01 << 5),
    } VDRVDSPReset_t;

    /// @brief 定义一个联合体类型，用于封装 DSP 的 Hold 和 Reset 参数
    typedef union
    {
        /// @brief DSP Hold 参数
        VDRVDSPHold_t hold;
        /// @brief DSP Reset 参数
        VDRVDSPReset_t reset;
    } VDRVDSPParam_t;

 /**
 * @addtogroup Vpla_DSP
 * 
 * @{
 */

    /// @brief DSP 初始化
    /// @param cfg 配置信息结构体
    /// @return 0，成功；其它，失败
    extern VRET_VALUE VSYS_DSP_Init(VDRVDSPCfg_t *cfg);

    /// @brief DSP 反初始化
    /// @return 0，成功；其它，失败
    extern VRET_VALUE VSYS_DSP_Uninit(void);

    /// @brief DSP 运行
    /// @return 0，成功；其它，失败
    extern VRET_VALUE VSYS_DSP_Run(void);

    /// @brief 获得DSP的状态信息
    /// @param status 选择需要的状态
    /// @return 返回获得的状态信息
    extern UINT32 VSYS_DSP_GetStatus(VDRVDSPStatus_t status);

    /// @brief 控制设置DSP的配置
    /// @param control 选择需要配置的DSP设置
    /// @param val 配置值
    /// @return 0，成功；其它，失败
    extern VRET_VALUE VSYS_DSP_Control(VDRVDSPControl_t control, /*VDRVDSPParam_t*/ UINT32 val);

    /// @brief DSP 写操作
    /// @param XYMEM 目的地址的配置信息
    /// @param source 传输的数据地址
    /// @param dest 需要写的目标path
    /// @param length 传输数据长度，单位字节
    void VSYS_DSP_WriteDM(UINT8 XYMEM, UINT32 *source, UINT32 *dest, UINT32 length);

    /// @brief DSP 读操作
    /// @param XYMEM 目的地址的配置信息
    /// @param source 传输的数据地址
    /// @param dest 需要写的目标path
    /// @param length 传输数据长度，单位字节
    void VSYS_DSP_ReadDM(UINT8 XYMEM, UINT32 *source, UINT32 *dest, UINT32 length);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_DSP_H_ */
