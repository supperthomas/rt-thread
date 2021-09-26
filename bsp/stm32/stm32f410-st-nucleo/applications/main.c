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

void main_thread_entry(void *parameter)
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
/**
 * @brief  This function will create and start the main thread, but this thread
 *         will not run until the scheduler starts.
 */
static struct rt_thread _main_thread;
static rt_uint8_t _main_thread_stack[512];
void rt_application_init(void)
{
    rt_thread_t tid;

//    tid = rt_thread_init("main", main_thread_entry, RT_NULL,
//                           RT_MAIN_THREAD_STACK_SIZE, RT_MAIN_THREAD_PRIORITY, 20);
//    
       rt_thread_init(&_main_thread,
                   "main",
                   main_thread_entry,
                   RT_NULL,
                   &_main_thread_stack[0],
                   sizeof(_main_thread_stack),
                   2,
                   10);

    rt_thread_startup(&_main_thread);
}

void rtthread_startup(void)
{
    /* init board */
    rt_hw_board_init();

    /* init scheduler system */
    rt_system_scheduler_init();
        /* init application */
    rt_application_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}

int main(void)
{
    rt_hw_interrupt_disable();

    /* startup RT-Thread RTOS */
    rtthread_startup();
  
}
