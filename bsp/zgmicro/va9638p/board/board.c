/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-08     obito0   first version
 */

#include <board.h>
#include <stdint.h>
#include "rtthread.h"
#include "rthw.h"
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vpla_ckm.h"
#include "vpla_gpio.h"
#include "vdrv_timer.h"
#include "vdrv_gpio.h"
#include "vreg_def.h"
#include "vnvic_system.h"

void SystemClock_Config(void)
{
}

uint8_t heap_size[10240*5];

void rt_hw_board_init(void)
{
		VCPU_CoreInit();	
		VCPU_INT_Enable();
		VSYS_CKM_Init();

		VCPU_SysTick_Config(1);
		VNVIC_SYS_EnableSysTick();
	
#ifdef RT_USING_HEAP
    rt_system_heap_init((void *)&heap_size[0], (void *)&heap_size[10240*5-1]);
#endif
	
#if defined(RT_USING_CONSOLE) && defined(RT_USING_NANO)
    extern void rt_hw_console_init(void);
    rt_hw_console_init();
#endif
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
	
    rt_tick_increase();
    /* leave interrupt */
    rt_interrupt_leave();
}
