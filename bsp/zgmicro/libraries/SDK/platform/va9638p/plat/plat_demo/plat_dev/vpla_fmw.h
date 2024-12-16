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
        vpla firmware

Description:
        firmware
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_FMW_H_
#define _VPLA_FMW_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif
#pragma push
#pragma pack(1)
#pragma anon_unions

#define VSYS_FMW_PARTITION_MAX 10
#define VSYS_FMW_DATE_TIME_MAX 6
#define VSYS_FMW_FILE_NAME_LENGTH 76
#define VSYS_FMW_DSP_MEM_NUM 3

    /// @brief 定义一个结构体 VSYS_ECK_FLASH_INFO ，用于存储闪存的信息
    typedef struct
    {
        /// @brief 存储 magic_num
        UINT32 magic_num;
        /// @brief 总大小，以K字节为单位
        UINT16 total_size; /*Unit:Kbytes,flash's total size*/
        /// @brief 扇区大小，以K字节为单位，通常为4KB
        UINT8 sector_size; /*Unit:Kbytes,sector size, typical 4KB(0x4)*/
        /// @brief 页大小，以2的对数计算，通常为256字节
        UINT8 page_size;   /*Log2(FLASH PAGESIZE), typical 256bytes(0x8)*/
        /// @brief SPI模式，通常为0x0
        UINT8 spi_mode;    /*typical : 0x0*/
        /// @brief 双模式，通常为0x0
        UINT8 dual_mode;   /*typical : 0x0*/
        /// @brief CRC校验码
        UINT16 crc;
    } VSYS_ECK_FLASH_INFO;

    /// @brief 固件头结构定义
    typedef struct
    {
        /// @brief 项目名称，最大长度为16
        UINT8 name[16]; /* project name */
        /// @brief 固件版本号
        UINT32 version_num;
        /// @brief 固件日期，最大长度由VSYS_FMW_DATE_TIME_MAX定义
        UINT8 date[VSYS_FMW_DATE_TIME_MAX];
        /// @brief 固件二进制文件的CRC32校验值
        UINT32 bin_crc32;
        /// @brief 工具版本，即固件包版本
        UINT32 tool_version; /* package firmware version */
        /// @brief 保留字段，长度为16
        UINT8 res[16];
        /// @brief 结构体的CRC校验值
        UINT16 crc;
    } VSYS_FMW_HEADER;

    /// @brief VSYS_FMW_PARTITION_INFO 结构体定义，包含固件分区的相关信息
    typedef struct
    {
        /// @brief 分区地址
        UINT32 address;
        /// @brief 分区长度
        UINT32 length;
        /// @brief 分区版本
        UINT32 version;
        /// @brief 分区的 git 哈希值
        UINT32 hash; /* for git hash value */
        /// @brief 分区日期时间，数组长度由宏 VSYS_FMW_DATE_TIME_MAX 定义
        UINT8 date[VSYS_FMW_DATE_TIME_MAX];
        /// @brief 分区二进制数据的 CRC 校验值
        UINT32 bin_crc;
        /// @brief 标识位
        UINT16 flag;
        /// @brief 分区扇区数量
        UINT16 sector_num;
        /// @brief 结构体的 CRC 校验值
        UINT16 crc;
    } VSYS_FMW_PARTITION_INFO;

    /// @brief 定义一个结构体用于存储文件信息，包括文件名、长度和CRC校验码
    typedef struct
    {
        /// @brief 文件名，长度为VSYS_FMW_FILE_NAME_LENGTH
        UINT8 name[VSYS_FMW_FILE_NAME_LENGTH];
        /// @brief 文件长度
        UINT32 length;
        /// @brief 文件的CRC校验码
        UINT16 crc;
    } VSYS_FMW_FILE_INFO;

    /// @brief VSYS固件全局信息的结构体
    typedef struct
    {
        /// @brief VSYS ECK闪存信息
        VSYS_ECK_FLASH_INFO flash_info;
        /// @brief 固件头部信息
        VSYS_FMW_HEADER header;
        /// @brief 分区信息
        union
        {
            /// @brief 分区结构体
            struct
            {
                /// @brief TOP分区信息
                VSYS_FMW_PARTITION_INFO top;
                /// @brief BT分区信息
                VSYS_FMW_PARTITION_INFO bt;
                /// @brief DSP分区信息
                VSYS_FMW_PARTITION_INFO dsp;
                /// @brief ECK分区信息
                VSYS_FMW_PARTITION_INFO eck;
                /// @brief TOP备份分区信息
                VSYS_FMW_PARTITION_INFO top_bk;
                /// @brief BT备份分区信息
                VSYS_FMW_PARTITION_INFO bt_bk;
                /// @brief DSP备份分区信息
                VSYS_FMW_PARTITION_INFO dsp_bk;
                /// @brief TUD分区信息
                VSYS_FMW_PARTITION_INFO tud;
                /// @brief Second Boot分区信息
                VSYS_FMW_PARTITION_INFO second_boot;
                /// @brief MCUBT MP分区信息
                VSYS_FMW_PARTITION_INFO mcubt_mp;
            };
            /// @brief 分区数组信息
            VSYS_FMW_PARTITION_INFO partition[VSYS_FMW_PARTITION_MAX];
        };
        /// @brief 文件信息数组
        VSYS_FMW_FILE_INFO file_info[VSYS_FMW_PARTITION_MAX];
    } VSYS_FMW_GLOBAL_INFO;

    /// @brief VSYS_FMW_DSP_HEADER 结构体用于存储DSP固件的头部信息，包括魔术数、版本、日期、哈希值和CRC值。
    typedef struct
    {
        /// @brief 数据魔术数，用于标识数据的合法性。
        UINT32 magic_num;
        /// @brief 固件版本号。
        UINT32 version;
        /// @brief 固件生成日期，最大长度由VSYS_FMW_DATE_TIME_MAX定义。
        UINT8 date[VSYS_FMW_DATE_TIME_MAX];
        /// @brief 固件内容的哈希值，用于校验数据完整性。
        UINT32 hash;
        /// @brief 校验和，用于保证数据的完整性。
        UINT16 crc;
    } VSYS_FMW_DSP_HEADER;

    /// @brief VSYS_FMW_DSP_MEM结构体，表示DSP内存相关信息
    typedef struct
    {
        /// @brief 内存地址
        UINT32 address;
        /// @brief 内存长度
        UINT32 length;
        /// @brief 保留字节数组
        UINT8 res[6];
        /// @brief 内存数据的CRC32校验值
        UINT32 bin_crc32;
        /// @brief 结构体数据的CRC校验值
        UINT16 crc;
    } VSYS_FMW_DSP_MEM;

    /// @brief VSYS_FMW_DSP_INFO 结构体，包含DSP的相关信息
    typedef struct
    {
        /// @brief DSP头信息
        VSYS_FMW_DSP_HEADER header;
        /// @brief DSP存储信息的联合体
        union
        {
            /// @brief 分别存储x, y, p内存信息的结构体
            struct
            {
                /// @brief x内存信息
                VSYS_FMW_DSP_MEM x;
                /// @brief y内存信息
                VSYS_FMW_DSP_MEM y;
                /// @brief p内存信息
                VSYS_FMW_DSP_MEM p;
            };
            /// @brief 存储所有类型内存信息的数组
            VSYS_FMW_DSP_MEM mem[VSYS_FMW_DSP_MEM_NUM];
        };
        /// @brief 存储文件信息的数组，包含DSP存储信息
        VSYS_FMW_FILE_INFO file_info[VSYS_FMW_DSP_MEM_NUM];
    } VSYS_FMW_DSP_INFO;

 /**
 * @addtogroup Vpla_FMW
 * 
 * @{
 */
    /// @brief 初始化FMW模块
    /// @param info 包含FMW全局信息的结构体
    /// @return 返回初始化状态码
    UINT8 VPLA_FMW_Init(VSYS_FMW_GLOBAL_INFO *info);

    /// @brief 配置DSP参数
    /// @param dsp_cfg DSP配置结构体
    /// @return 返回配置状态码
    UINT8 VPLA_FMW_DspCfg(VDRVDSPCfg_t *dsp_cfg);
/**@}*/

#pragma pop

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_FMW_H_ */
