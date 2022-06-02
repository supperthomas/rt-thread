/*
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-02     supperthomas          first version
 */
#include <rtdevice.h>
#include <rthw.h>

#include <stdio.h>

#include "drv_uart.h"

#ifdef RT_USING_SERIAL

#if !defined(BSP_USING_UART1) && !defined(BSP_USING_UART2) && !defined(BSP_USING_UART3) && \
    !defined(BSP_USING_UART4) && !defined(BSP_USING_UART5) && !defined(BSP_USING_UART6) && \
    !defined(BSP_USING_UART7) && !defined(BSP_USING_UART8) && !defined(BSP_USING_UART9) && \
    !defined(BSP_USING_UART10)
#error "Please define at least one BSP_USING_UARTx"
/* UART instance can be selected at menuconfig -> Hardware Drivers Config -> On-chip Peripheral Drivers -> Enable UART */
#endif



static rt_err_t mcu_configure(struct rt_serial_device *serial,
                                struct serial_configure *cfg)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return RT_EOK;
}

static rt_err_t mcu_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return RT_EOK;
}

static int mcu_putc(struct rt_serial_device *serial, char c)
{

    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return 1;
}

static int mcu_getc(struct rt_serial_device *serial)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    int ch= -1;
    return ch;
}



// static const struct rt_uart_ops mcu_uart_ops =
// {
//     .configure = mcu_configure,
//     .control = mcu_control,
//     .putc = mcu_putc,
//     .getc = mcu_getc,
//     .dma_transmit = RT_NULL
// };

int mcu_hw_uart_init(void)
{
    rt_err_t result = 0;
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return result;
}

INIT_BOARD_EXPORT(mcu_hw_uart_init);

#endif /* RT_USING_SERIAL */
