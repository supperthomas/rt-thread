/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    main

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vpla_ckm.h"
#include "vpla_gpio.h"
#include "vdrv_timer.h"
#include "vdrv_gpio.h"
#include "vreg_def.h"
#include "rtthread.h"
#include "rthw.h"

#define Dbg_PRINT        vcom_printf

#define GPIO_LED  VSYS_GPIOL_PIN_4

void main_thread_entry(void *parameter)
{
	int i  = 0 , j = 0, k = 0,m = 0;
	static int flag = 0;
	UINT32 start_time = 0,end_time = 0; 
	VSYS_GPIO_Init(GPIO_LED, VSYS_GPIO_MODE_OUTPUT,VSYS_GPIO_OUTPUT_LOW_STRENGTH);
	while(1)
	{
		VSYS_GPIO_SetPinOutput(GPIO_LED,TRUE);
		rt_thread_mdelay(1000);
    VSYS_GPIO_SetPinOutput(GPIO_LED,FALSE);
    rt_thread_mdelay(1000);
	}
}

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
uint8_t heap_size[10240*5];

void rt_hw_board_init(void)
{
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

int rtthread_startup(void)
{
    rt_hw_interrupt_disable();
	
    /* board level initalization
     * NOTE: please initialize heap inside board initialization.
     */
    rt_hw_board_init();

    /* show RT-Thread version */
    rt_show_version();

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();
	
    /* create init_thread */
    rt_application_init();

    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return 0;
}
int main ( void )
{
	int i  = 0 , j = 0, k = 0,m = 0;
	static int flag = 0;
	VCPU_CoreInit();	
	VCPU_INT_Enable();
	VSYS_CKM_Init();
	
	VCPU_INT_Enable();
	VCPU_INT_SrcEnable(IRQ_MCUIPC_INT);
	
	Dbg_PRINT("gpio test...\r\n");
	VCPU_SysTick_Config(1);
	VNVIC_SYS_EnableSysTick();
	rtthread_startup();
	
	UINT32 start_time = 0,end_time = 0; 
	VSYS_GPIO_Init(GPIO_LED, VSYS_GPIO_MODE_OUTPUT,VSYS_GPIO_OUTPUT_LOW_STRENGTH);
	
}
