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
#define BLINK_GPIO 8
    rt_pin_mode(BLINK_GPIO, PIN_MODE_OUTPUT);
    while (1)
    {
        rt_pin_write(BLINK_GPIO, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(BLINK_GPIO, PIN_LOW);
        rt_thread_mdelay(1000);
    }
}
