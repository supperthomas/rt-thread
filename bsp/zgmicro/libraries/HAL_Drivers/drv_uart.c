/*
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-05-18     xph          first version
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <rtdevice.h>
#include <rthw.h>

#include "drv_uart.h"
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vdrv_uart.h"
#include "vdebug.h"
#include "vstdio.h"
#include "vdrv_ckm.h"
#include "vpla_uart.h"


static UINT32 VUART_INIT_Flag = 0;

#ifdef RT_USING_SERIAL

static rt_err_t mcu_configure(struct rt_serial_device *serial,
                                struct serial_configure *cfg)
{
    return RT_EOK;
}

static rt_err_t mcu_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct mcu_uart *uart;

    return RT_EOK;
}

static int mcu_putc(struct rt_serial_device *serial, char c)
{
    vdirect_vputc((int)c);
    return 1;
}

static int mcu_getc(struct rt_serial_device *serial)
{
    int ch= -1;
    return ch;
}

static const struct rt_uart_ops mcu_uart_ops =
{
    .configure = mcu_configure,
    .control = mcu_control,
    .putc = mcu_putc,
    .getc = mcu_getc,
    .dma_transmit = RT_NULL
};

struct rt_serial_device serial;
int rt_hw_uart_init(void)
{
    rt_err_t result = RT_EOK;
			if (VUART_INIT_Flag != VCKM_GetUartClockFreq())
		{
			VDRVUartConfig_t uartconfig = {0};
			uartconfig.Baudrate = VDBG_UART_BAUDRATE;
			uartconfig.port_sel = VSYS_UART_PORT_SEL_AUX;
			uartconfig.Parity = VSYS_UART_PARITY_NONE;
			uartconfig.StopBit = VSYS_UART_STOP_BIT1;
			uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
			uartconfig.mode = VSYS_UART_FIFO_MODE;
			uartconfig.TxFifolevel = 0;
			uartconfig.RxFifolevel = 20;
			VSYS_UART_Init(&uartconfig);
			VUART_INIT_Flag = VCKM_GetUartClockFreq();
		}
		serial.ops = &mcu_uart_ops;
		
		rt_hw_serial_register(&serial,"uart1",
														 RT_DEVICE_FLAG_RDWR
												 | RT_DEVICE_FLAG_INT_RX
												 | RT_DEVICE_FLAG_INT_TX,
													NULL);
    return result;
}

INIT_BOARD_EXPORT(rt_hw_uart_init);

#endif /* RT_USING_SERIAL */

