/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-11     supperthomas first version
 *
 */

#include "drv_gpio.h"
#include <stdbool.h>
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_gpio.h"

#ifdef RT_USING_PIN

#define DBG_LEVEL   DBG_LOG
#include <rtdbg.h>
#define LOG_TAG                "drv.gpio"





static void mcu_pin_write(struct rt_device *device, rt_base_t pin, rt_uint8_t value)
{
	    uint8_t gpio_port;
      uint64_t gpio_pin;
			gpio_port = PIN_PORT(pin);
			gpio_pin = PIN_NO(pin);
	    if(gpio_port == GPIO_X)
			{
				  gpio_pin = VSYS_GPIO_PIN_X(gpio_pin);
			}
			else
			{
				  gpio_pin = VSYS_GPIO_L_PIN_X(gpio_pin);
			}
	    VSYS_GPIO_SetPinOutput(gpio_pin,value);
}

rt_ssize_t mcu_pin_read(struct rt_device *device, rt_base_t pin)
{
    int value;
	   
      /*TODO:set gpio out put mode */
    return value;
}

static void mcu_pin_mode(struct rt_device *device, rt_base_t pin, rt_uint8_t mode)
{
				/*TODO:set gpio out put mode */
				uint8_t gpio_port;
				uint64_t gpio_pin;
				gpio_port = PIN_PORT(pin);
				gpio_pin = PIN_NO(pin);
				VDRVGpioMode_t gpio_mode;
				if(gpio_port == GPIO_X)
				{
					gpio_pin = VSYS_GPIO_PIN_X(gpio_pin);
				}
				else
				{
					gpio_pin = VSYS_GPIO_L_PIN_X(gpio_pin);
				}
				if (mode == PIN_MODE_OUTPUT)
				{
				gpio_mode = VSYS_GPIO_MODE_OUTPUT;
				}
				else if (mode == PIN_MODE_INPUT)
				{
				gpio_mode = VSYS_GPIO_MODE_INPUT;
				}
				else if (mode == PIN_MODE_INPUT_PULLUP)
				{
				 
				}
				else if (mode == PIN_MODE_INPUT_PULLDOWN)
				{
				}
				else if (mode == PIN_MODE_OUTPUT_OD)
				{
				}
    VSYS_GPIO_Init(gpio_pin, gpio_mode,VSYS_GPIO_OUTPUT_LOW_STRENGTH);
}

static rt_err_t mcu_pin_attach_irq(struct rt_device *device, rt_base_t pin,
            rt_uint8_t mode, void (*hdr)(void *args), void *args)
{
    /*TODO: start irq handle */
    return RT_EOK;
}

static rt_err_t mcu_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
    /*TODO:disable gpio irq handle */
    return RT_EOK;
}

static rt_err_t mcu_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint8_t enabled)
{
     /*TODO:start irq handle */
    return RT_EOK;
}

const static struct rt_pin_ops _mcu_pin_ops =
{
    mcu_pin_mode,
    mcu_pin_write,
    mcu_pin_read,
    mcu_pin_attach_irq,
    mcu_pin_dettach_irq,
    mcu_pin_irq_enable,
    NULL,
};

int rt_hw_pin_init(void)
{
    /*TODO: INIT THE GPIO CLOCK AND OTHER */
    return rt_device_pin_register("pin", &_mcu_pin_ops, RT_NULL);
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

//irq handle
void GPIO0_IRQHandler(void)
{
   
}
#endif /* RT_USING_PIN */
