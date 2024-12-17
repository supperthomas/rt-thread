/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-08     obito0       first version
 * 2023-12-03     Meco Man     support nano version
 */

#include <board.h>

#include "rtthread.h"
#include "rthw.h"
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_gpio.h"

/* defined the LED0 pin: PC13 */
#define LED0_PIN    GET_PIN(C, 13)
#define GPIO_LED  VSYS_GPIOL_PIN_4
#if 0
void main_thread_entry(void *parameter)
{
	VSYS_GPIO_Init(GPIO_LED, VSYS_GPIO_MODE_OUTPUT,VSYS_GPIO_OUTPUT_LOW_STRENGTH);
	while(1)
	{
		VSYS_GPIO_SetPinOutput(GPIO_LED,TRUE);
		rt_thread_mdelay(1000);
    VSYS_GPIO_SetPinOutput(GPIO_LED,FALSE);
    rt_thread_mdelay(1000);
	}
}
#endif

#if 0
void rt_application_init(void)
{
    rt_thread_t tid;
	
#ifndef RT_MAIN_THREAD_STACK_SIZE
#define RT_MAIN_THREAD_STACK_SIZE     2048
#endif
#ifndef RT_MAIN_THREAD_PRIORITY
#define RT_MAIN_THREAD_PRIORITY       (RT_THREAD_PRIORITY_MAX / 3)
#endif
	
    tid = rt_thread_create("main", main_thread_entry, RT_NULL,
                           RT_MAIN_THREAD_STACK_SIZE, RT_MAIN_THREAD_PRIORITY, 20);
    RT_ASSERT(tid != RT_NULL);

    rt_thread_startup(tid);
}
#endif

int main ( void )
{

//	rtthread_startup();

	VSYS_GPIO_Init(GPIO_LED, VSYS_GPIO_MODE_OUTPUT,VSYS_GPIO_OUTPUT_LOW_STRENGTH);
	while(1)
	{
		VSYS_GPIO_SetPinOutput(GPIO_LED,TRUE);
		rt_thread_mdelay(1000);
    VSYS_GPIO_SetPinOutput(GPIO_LED,FALSE);
    rt_thread_mdelay(1000);
	}
}
