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
        vpla i2c

Description:
        i2c Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_I2C_H_
#define _VPLA_I2C_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include "vstdlib.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */


    /// @brief VDRVI2CSel_t 枚举，定义了I2C从设备类型
    typedef enum
    {
        /// @brief PROM类型的I2C从设备
        VSYS_I2C_SLAVE_PROM = 0,
        /// @brief 模拟类型的I2C从设备
        VSYS_I2C_SLAVE_ANA = 1,
        /// @brief 软件类型的I2C从设备
        VSYS_I2C_SLAVE_SOFTWARE = 2,
        /// @brief 第二种软件类型的I2C从设备
        VSYS_I2C_SLAVE_SOFTWARE2 = 3,
        /// @brief 第三种软件类型的I2C从设备
        VSYS_I2C_SLAVE_SOFTWARE3 = 4,
        /// @brief 第四种软件类型的I2C从设备
        VSYS_I2C_SLAVE_SOFTWARE4 = 5,
        /// @brief 第五种软件类型的I2C从设备
        VSYS_I2C_SLAVE_SOFTWARE5 = 6,
        /// @brief I2C从设备类型数量限制
        VSYS_I2C_CH_LIMIT
    } VDRVI2CSel_t;

    /// @brief 定义I2C操作模式的枚举类型
    typedef enum
    {
        /// @brief 标准模式
        VSYS_I2C_MODE_STANDARD = 0,
        /// @brief 快速模式
        VSYS_I2C_MODE_FAST = 1,
        /// @brief 慢速模式
        VSYS_I2C_MODE_SLOW = 2,
        /// @brief 高数据速率模式
        VSYS_I2C_MODE_HDS = 3,
    } VDRVI2CMode_t;


    /// @brief VDRVI2C配置结构体，用于配置I2C的工作模式和针脚
    typedef struct
    {
        /// @brief 工作模式
        VDRVI2CMode_t mode;
        /// @brief SCL针脚
        uint64_t scl_pin;
        /// @brief SDA针脚
        uint64_t sda_pin;
        /// @brief 连续读写模式使能
        BOOL    contmode_en;
    } VDRVI2CConfig_t;
    
    /// @brief I2C位操作枚举类型定义文件
    typedef enum
    {
        /// @brief 设置位
        VSYS_I2C_Bit_SET = 0,  
        /// @brief 清除位
        VSYS_I2C_Bit_CLR = 1,  
    } VDRVI2CBitOperat_t;

    /// @brief 定义I2C控制类型的枚举
    typedef enum
    {
        /// @brief 设置时钟模式
        VSYS_I2C_Set_Clkmode = 0,
        /// @brief 设置连续读写模式
        VSYS_I2C_Set_Contmode = 1,
    } VDRVI2CControl_t;

    #define I2C_WR                0x0000    /*写操作的标志位*/
    #define I2C_RD               (1u << 0)  /*读操作的标志位*/
    #define I2C_ADDR_10BIT       (1u << 2)  /*十位地址模式的标志位 */
    #define I2C_NO_START         (1u << 4)  /*无START条件的标志位*/
    #define I2C_IGNORE_NACK      (1u << 5)  /*忽略非应答的标志位*/
    #define I2C_NO_READ_ACK      (1u << 6)  /* 读操作时不要应答的标志位 */
    #define I2C_NO_STOP          (1u << 7)  /*无STOP条件的标志位*/
    #define I2C_WR_DMA           (1u << 8)  /* 使用DMA进行写操作的标志位 */
    #define I2C_RD_DMA           (1u << 9)	/* 使用DMA进行读操作的标志位 */
    
    /// @brief 定义I2C消息的数据结构
    struct i2c_msg
    {
        /// @brief 设备地址
        uint16_t addr;
        /// @brief 标志位，用于指定读/写操作等
        uint16_t flags;
        /// @brief 缓冲区长度
        uint16_t len;
        /// @brief 指向数据缓冲区的指针
        uint8_t  *buf;
    };


 /**
 * @addtogroup Vpla_I2C
 * 
 * @{
 */
    /// @brief I2C 初始化
    /// @param i2c_sel 使用哪组I2C 注意：PROM 和ANN 不能同时使用
    /// @param i2c_config 配置I2C速度模式, 软I2C需要传scl, sda 引脚号
    /// @return 0 初始化成功，其他值 初始化失败
    /// @note 注意: scl, sda 引脚号定义为GPIOLX=32+X e.g. 3 表示 GPIO3, 34 表示 GPIOL2
    extern VRET_VALUE VSYS_I2C_Init(VDRVI2CSel_t i2c_sel, VDRVI2CConfig_t *i2c_config);

    /// @brief I2C 反初始化
    /// @param i2c_sel 使用哪组I2C 注意：PROM 和ANN 不能同时使用
    /// @return 0 反初始化成功，其他值 初始化失败
    extern VRET_VALUE VSYS_I2C_Uninit(VDRVI2CSel_t i2c_sel);

    /// @brief I2C 写
    /// @param i2c_sel 使用哪组I2C 注意：PROM 和ANN 不能同时使用
    /// @param dev_addr 设备地址
    /// @param Reg 需要写的寄存器地址
    /// @param buf 需要写的寄存器内容值
    /// @param siz 需要写的寄存器内容值长度
    /// @return 0 写成功，其他 写失败
    extern VRET_VALUE VSYS_I2C_Write(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, const UINT8 *buf, size_t siz);

    /// @brief I2C 读
    /// @param i2c_sel 使用哪组I2C 注意：PROM 和ANN 不能同时使用
    /// @param dev_addr 设备地址
    /// @param Reg 需要读的寄存器地址
    /// @param buf 需要读的寄存器内容值
    /// @param siz 需要读的寄存器内容值长度
    /// @return 0 读成功，其他 写失败
    extern VRET_VALUE VSYS_I2C_Read(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, UINT8 *buf, size_t siz);

    /// @brief I2C 操作I2C 寄存器某一比特，读改写操作
    /// @param i2c_sel 使用哪组I2C 注意：PROM 和ANN 不能同时使用
    /// @param dev_addr 设备地址
    /// @param Reg 寄存器地址
    /// @param operat 设置 和清除 比特位操作
    /// @param Value 设置的值
    /// @return 0 成功，其他 失败
    extern VRET_VALUE VSYS_I2C_BitOperat(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, VDRVI2CBitOperat_t operat, UINT8 Value);

    /// @brief I2C 传输数据
    /// @param i2c_sel 使用哪组I2C 注意：暂不支持ANN
    /// @param msgs 传输数据结构体数组
    /// @param num 传输数据结构体数组长度
    /// @return 0 成功，其他 失败
    VRET_VALUE VSYS_I2C_Transfer(VDRVI2CSel_t i2c_sel, struct i2c_msg msgs[], uint32_t num);

    /// @brief I2C 控制接口
    /// @param i2c_sel 使用哪组I2C
    /// @param control I2C控制类型的枚举
    /// @param Value 控制值
    /// @return 0 成功，其他 失败
    VRET_VALUE VSYS_I2C_Control(VDRVI2CSel_t i2c_sel, VDRVI2CControl_t control, UINT32 Value);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_I2C_H_ */
