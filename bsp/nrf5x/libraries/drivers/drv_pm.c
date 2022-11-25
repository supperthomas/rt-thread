/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-06     Zero-Free    first version
 */

#include <board.h>
//#include <drv_lptim.h>
#include <drivers/pm.h>
#include <nrfx_rtc.h>
#include <nrfx_clock.h>
#define SYSTICK_CLOCK_HZ_PM    (32768UL)
#define RTC_PRESCALER_PM      ((uint32_t) (NRFX_ROUNDED_DIV(SYSTICK_CLOCK_HZ_PM, RT_TICK_PER_SECOND) - 1))
 const nrfx_rtc_t rtc_instance_pm = NRFX_RTC_INSTANCE(1);
static void rtc_callback(nrfx_rtc_int_type_t int_type)
{
    static uint32_t count = 0;
	count++;

    if (int_type == NRFX_RTC_INT_TICK)
    {
      
    }
}

/**
 * This function will put STM32L4xx into sleep mode.
 *
 * @param pm pointer to power manage structure
 */
static void sleep(struct rt_pm *pm, uint8_t mode)
{
    switch (mode)
    {
    case PM_SLEEP_MODE_NONE:
        break;

    case PM_SLEEP_MODE_IDLE:
        // __WFI();
        break;

    case PM_SLEEP_MODE_LIGHT:
     
        break;

    case PM_SLEEP_MODE_DEEP:
                __SEV();
                __WFE();
                __WFE();
        break;

    case PM_SLEEP_MODE_STANDBY:
        /* Enter STANDBY mode */
      
        break;

    case PM_SLEEP_MODE_SHUTDOWN:
        /* Enter SHUTDOWNN mode */
       
        break;

    default:
        RT_ASSERT(0);
        break;
    }
}



static void run(struct rt_pm *pm, uint8_t mode)
{

}

/**
 * This function caculate the PM tick from OS tick
 *
 * @param tick OS tick
 *
 * @return the PM tick
 */
static rt_tick_t nrf5x_pm_tick_from_os_tick(rt_tick_t tick)
{
  //  rt_uint32_t freq = stm32l4_lptim_get_countfreq();
	rt_uint32_t freq;
    freq=RT_TICK_PER_SECOND;
    return (freq * tick / RT_TICK_PER_SECOND);
}

/**
 * This function caculate the OS tick from PM tick
 *
 * @param tick PM tick
 *
 * @return the OS tick
 */
static rt_tick_t nrf5x_os_tick_from_pm_tick(rt_uint32_t tick)
{
    static rt_uint32_t os_tick_remain = 0;
    rt_uint32_t ret, freq;

    freq = RT_TICK_PER_SECOND;
    ret = (tick * RT_TICK_PER_SECOND ) / freq;

    

    return ret;
}

/**
 * This function start the timer of pm
 *
 * @param pm Pointer to power manage structure
 * @param timeout How many OS Ticks that MCU can sleep
 */
static void pm_timer_start(struct rt_pm *pm, rt_uint32_t timeout)
{
    RT_ASSERT(pm != RT_NULL);
    RT_ASSERT(timeout > 0);
	    
    timeout = nrf5x_pm_tick_from_os_tick(timeout);
    

	nrfx_rtc_cc_set(&rtc_instance_pm,0,timeout,true);



    //Power on RTC instance
    nrfx_rtc_enable(&rtc_instance_pm);
  
}

/**
 * This function stop the timer of pm
 *
 * @param pm Pointer to power manage structure
 */
static void pm_timer_stop(struct rt_pm *pm)
{
    RT_ASSERT(pm != RT_NULL);
	nrfx_rtc_disable(&rtc_instance_pm);

  
}

/**
 * This function calculate how many OS Ticks that MCU have suspended
 *
 * @param pm Pointer to power manage structure
 *
 * @return OS Ticks
 */
static rt_tick_t pm_timer_get_tick(struct rt_pm *pm)
{
    rt_uint32_t timer_tick;

    RT_ASSERT(pm != RT_NULL);

    timer_tick = nrfx_rtc_counter_get(&rtc_instance_pm);

    return nrf5x_os_tick_from_pm_tick(timer_tick);
 
}

/**
 * This function initialize the power manager
 */
int drv_pm_hw_init(void)
{
    static const struct rt_pm_ops _ops =
    {
        sleep,
        run,
        pm_timer_start,
        pm_timer_stop,
        pm_timer_get_tick
    };

    rt_uint8_t timer_mask = 0;
    nrf_clock_lf_src_set(NRF_CLOCK, (nrf_clock_lfclk_t)NRFX_CLOCK_CONFIG_LF_SRC);
    nrfx_clock_lfclk_start();

    //Initialize RTC instance
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    config.prescaler = RTC_PRESCALER_PM;

    nrfx_rtc_init(&rtc_instance_pm, &config, rtc_callback);
	nrfx_rtc_tick_enable(&rtc_instance_pm, true);

	//nrfx_rtc_enable(&rtc_instance_pm);


    /* initialize timer mask */
    timer_mask = 1UL << PM_SLEEP_MODE_DEEP;

    /* initialize system pm module */
    rt_system_pm_init(&_ops, timer_mask, RT_NULL);
	rt_kprintf("drvpm init success");

    return 0;
}

INIT_BOARD_EXPORT(drv_pm_hw_init);
