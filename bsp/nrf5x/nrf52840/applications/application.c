/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-29     supperthomas first version
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#if 0
#define LED_DEVICE_NAME  "pin"
#define DK_BOARD_LED_1  13
int main(void)
{
    int count = 1;
    struct rt_device *led_drv = rt_device_find(LED_DEVICE_NAME); 
    if(led_drv == NULL)
    {
        rt_kprintf("error!!!");
    }
      rt_pin_mode(DK_BOARD_LED_1, PIN_MODE_OUTPUT);
    rt_device_open(led_drv,RT_DEVICE_FLAG_RDWR);
    struct rt_device_pin_status status;
    uint8_t open_flag = 0;
    status.pin = DK_BOARD_LED_1;
    while (count++)
    {    
        status.status = PIN_HIGH;
        rt_device_write(led_drv,0,&status,sizeof(struct rt_device_pin_status));
        rt_thread_mdelay(1000);
         status.status = PIN_LOW;
        rt_device_write(led_drv,0,&status,sizeof(struct rt_device_pin_status));
        rt_thread_mdelay(1000);                    
    }
    return RT_EOK;
}
#else
#define LED_DEVICE_NAME  "led1"
int main(void)
{
    int count = 1;
    uint8_t led_status = 0;
    struct rt_device *led_drv = rt_device_find(LED_DEVICE_NAME); 
    if(led_drv == NULL)
    {
        rt_kprintf("error!!!");
    }
    rt_device_open(led_drv,RT_DEVICE_FLAG_RDWR);
    while (count++)
    {    
        led_status = PIN_HIGH;
        rt_device_write(led_drv,0,&led_status,sizeof(uint8_t));
        rt_thread_mdelay(1000);
        led_status= PIN_LOW;
        rt_device_write(led_drv,0,&led_status,sizeof(uint8_t));
        rt_thread_mdelay(1000);                    
    }
    return RT_EOK;
}
#endif
