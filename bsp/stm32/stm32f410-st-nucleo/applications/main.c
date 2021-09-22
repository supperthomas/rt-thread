/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-07     tanglj86   first version
 */

#include <rtthread.h>
#include <stm32f4xx.h>
#include <board.h>

/* defined the LED0 pin: PA5 */
#define LED0_PIN               GET_PIN(A, 5)

int main(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* set LED0 pin mode to output */
    //rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    GPIOA->OSPEEDR = 0x0C000800;
    GPIOA->MODER = 0xA8000400;
    while (1)
    {
        GPIOA->BSRR = 1<<5;
        rt_thread_mdelay(500);
        GPIOA->BSRR = (1<<5)<<16;
        rt_thread_mdelay(500);
    }
}
