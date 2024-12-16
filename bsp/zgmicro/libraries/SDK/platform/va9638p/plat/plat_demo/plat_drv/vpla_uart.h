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
        vpla uart

Description:
        uart Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_UART_H_
#define _VPLA_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "vstdlib.h"

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */ 

    /// @brief UART端口选择枚举类型
    typedef enum
    {
        /// @brief 主端口
        VSYS_UART_PORT_SEL_PRIMARY = 0,
        /// @brief 辅助端口
        VSYS_UART_PORT_SEL_AUX = 1,
    } VDRVUartSely_t;

    /// @brief UART奇偶校验枚举类型
    typedef enum
    {
        /// @brief 无校验
        VSYS_UART_PARITY_NONE = 0,
        /// @brief 奇校验
        VSYS_UART_PARITY_ODD = 1,
        /// @brief 偶校验
        VSYS_UART_PARITY_EVEN = 2,
    } VDRVUartParity_t;

    /// @brief UART停止位枚举类型
    typedef enum
    {
        /// @brief 1个停止位
        VSYS_UART_STOP_BIT1 = 0,
        /// @brief 2个停止位
        VSYS_UART_STOP_BIT2 = 1,
    } VDRVUartStopBit_t;

    /// @brief UART数据位枚举类型
    typedef enum
    {
        /// @brief 7个数据位
        VSYS_UART_CHAR_BIT7 = 0,
        /// @brief 8个数据位
        VSYS_UART_CHAR_BIT8 = 1,
    } VDRVUartCharBit_t;

    /// @brief UART模式枚举类型
    typedef enum
    {
        /// @brief FIFO模式
        VSYS_UART_FIFO_MODE = 0,
        /// @brief 中断模式
        VSYS_UART_INT_MODE = 1,
        // VSYS_UART_BUFF_MODE = 2,
    } VDRVUartMode_t;

    /// @brief 串口配置信息
    typedef struct
    {
        /// @brief 波特率
        UINT32 Baudrate;
        /// @brief 端口选择
        VDRVUartSely_t port_sel;
        /// @brief 奇偶校验
        VDRVUartParity_t Parity;
        /// @brief 停止位
        VDRVUartStopBit_t StopBit;
        /// @brief 位宽
        VDRVUartCharBit_t BitWidth;
        /// @brief 模式
        VDRVUartMode_t mode;
        /// @brief RTS 使能
        BOOL Rtsen;
        /// @brief CTS 使能
        BOOL Ctsen;
        /// @brief 接收 FIFO 级别
        UINT8 RxFifolevel;
        /// @brief 发送 FIFO 级别
        UINT8 TxFifolevel;
        /// @brief 发送 Buff 使能
        BOOL TxBuffen;
        /// @brief 接收 Buff 使能
        BOOL RxBuffen;
        /// @brief Buff 深度
        UINT32 BuffDepth;
    } VDRVUartConfig_t;

    /// @brief 定义UART中断类型的枚举
    typedef enum
    {
        /// @brief UART接收中断
        VSYS_UART_RX_INT = (0x01 << 0),
        /// @brief UART发送中断
        VSYS_UART_TX_INT = (0x01 << 1),
        /// @brief UART CTS中断
        VSYS_UART_CTS_INT = (0x01 << 2),
        /// @brief UART错误中断
        VSYS_UART_ERR_INT = (0x01 << 3),
        /// @brief 包含所有UART中断
        VSYS_UART_ALL_INT = (0xFF),
    } VDRVUartInt_t;
 
    /// @brief 定义UART状态类型的枚举
    typedef enum
    {
        /// @brief 空闲传输计数状态
        VSYS_UART_FreeTxCount_Status = 0,
        /// @brief 有效接收计数状态
        VSYS_UART_ValidRxCount_Status,
        /// @brief 中断状态
        VSYS_UART_Interrupt_Status,
        /// @brief CTS状态
        VSYS_UART_CTS_Status,
        /// @brief 接收传送状态
        VSYS_UART_RecTra_Status,
    } VDRVUartStatus_t;
 
    /// @brief 定义UART控制类型的枚举
    typedef enum
    {
        /// @brief 重置发送FIFO
        VSYS_UART_ResetTxFifo = 0,
        /// @brief 重置接收FIFO
        VSYS_UART_ResetRxFifo,
        /// @brief UART流量控制启动
        VSYS_UART_FLOW_GO,
        /// @brief UART流量控制停止
        VSYS_UART_FLOW_STOP,
    } VDRVUartControl_t;

 /**
 * @addtogroup Vpla_UART
 * 
 * @{
 */

    ///@brief 初始化uart接口
    ///@param uart_config 串口参数配置
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_UART_Init(VDRVUartConfig_t *uart_config);

    ///@brief 取消初始化 uart 接口
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_UART_Uninit(void);

    ///@brief 设置UART中断函数
    ///@param Int 中断模式
    ///@param en 是否启用
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_UART_EnableInt(VDRVUartInt_t Int, BOOL en);

    ///@brief 清除UART中断
    ///@return VRET_VALUE 错误代码。  
    extern VRET_VALUE VSYS_UART_ClearInt(void);

    ///@brief uart发送数据
    ///@param Chs 需要发送的数据
    ///@param size 待发送数据的大小
    ///@return 发送的数据的实际大小
    extern size_t VSYS_UART_CharTx(const UINT8 *const Chs, size_t size);

    ///@brief uart接收数据
    ///@param Chs 需要接收的数据
    ///@param size 要接收的数据大小
    ///@return 接收到的数据的实际大小
    extern size_t VSYS_UART_CharRx(UINT8 *const Chs, size_t size);

    ///@brief 获取串口状态
    ///@param status 获取串口的哪个状态
    ///@return 串口的状态值
    extern UINT32 VSYS_UART_GetStatus(VDRVUartStatus_t status);

    ///@brief 设置uart的参数
    ///@param control 指定uart参数
    ///@return VRET_VALUE 错误代码。
    extern VRET_VALUE VSYS_UART_Control(VDRVUartControl_t control);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_UART_H_ */
