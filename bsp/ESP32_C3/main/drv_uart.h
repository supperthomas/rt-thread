/*
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-02     supperthomas first version
 */
 

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <rtthread.h>
#include "rtdevice.h"


/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BSP_USING_UART1)
#ifndef UART1_CONFIG
#define UART1_CONFIG                                                \
    {                                                               \
        .name = "uart1"                                            \
    }
#endif /* UART1_CONFIG */
#endif /* BSP_USING_UART1 */


int mcu_hw_uart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_UART_H__ */

