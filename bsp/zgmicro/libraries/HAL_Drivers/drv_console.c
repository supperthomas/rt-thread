/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <board.h>
#include <rtthread.h>

void rt_hw_console_init(void)
{
 
}

void rt_hw_console_output(const char *str)
{
        //HAL_UART_Transmit(&console_uart, (uint8_t *)(str + i), 1, 1);
}

char rt_hw_console_getchar(void)
{
#if 0
    int ch = -1;

    if (HAL_UART_Receive(&console_uart, (uint8_t *)&ch, 1, 0) != HAL_OK)
    {
        ch = -1;
        rt_thread_mdelay(10);
    }
    return ch;
#else 
	return 0;
#endif

}
