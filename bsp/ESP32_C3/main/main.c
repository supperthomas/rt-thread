/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-02     supperthomas first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

int rtt_main(void)
{

#define SAMPLE_UART_NAME       "uart1"    /* 串口设备名称 */
static rt_device_t serial;                /* 串口设备句柄 */
/* 查找串口设备 */
    char str[] = "hello RT-Thread!\r\n";
    serial = rt_device_find(SAMPLE_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

#define BLINK_GPIO 12
    rt_pin_mode(BLINK_GPIO, PIN_MODE_OUTPUT);
    while (1)
    {
        rt_pin_write(BLINK_GPIO, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(BLINK_GPIO, PIN_LOW);
        rt_thread_mdelay(1000);
        rt_device_write(serial, 0, str, (sizeof(str) - 1));
    }
}
