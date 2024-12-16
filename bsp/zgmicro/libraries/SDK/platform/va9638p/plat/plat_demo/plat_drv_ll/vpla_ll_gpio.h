/*------------------------------------------------------------------------------
					COPYRIGHT (C) 2009, Vimicro Corporation
							  ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
	special interface file

Description:
	 A special interface file is used to support special scenarios.

Revision History:
	 Date                  Author                   Description
	2023.04.3            cxt   						create
------------------------------------------------------------------------------*/

#ifndef _VPLA_DIRECT_GPIO_H_
#define _VPLA_DIRECT_GPIO_H_

#include "vstdlib.h"
#include "vstdcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "vreg_operation.h"

#define GPIO_BASE       (0x40014000)
#define REG_GPIO_GDOUT  (GPIO_BASE + 0x0)
#define REG_GPIO_GDOEN  (GPIO_BASE + 0x4)
#define REG_GPIO_GDIN   (GPIO_BASE + 0x10)
#define REG_GPIO_GLDOUT (GPIO_BASE + 0x40)
#define REG_GPIO_GLDOEN (GPIO_BASE + 0x44)
#define REG_GPIO_GLDIN  (GPIO_BASE + 0x50)

__attribute__((always_inline)) static void gpio_od_pin_set(uint8_t pinnum, uint8_t level)
{
    uint32_t primask;
    uint32_t reg_gpio_gdoen;
    uint32_t reg_gpio_gdout;
    uint32_t reg_gpio_gdoen_value;
    uint32_t reg_gpio_gdout_value;
    // 保存当前的PRIMASK状态并禁用中断
    __asm {
        MRS primask, PRIMASK   // 将PRIMASK寄存器的值移动到C变量中
        CPSID I                // 禁用中断
    }
    if (pinnum < 32) {
        reg_gpio_gdoen = REG_GPIO_GDOEN;
        reg_gpio_gdout = REG_GPIO_GDOUT;
    } else {
        pinnum -= 32;
        reg_gpio_gdoen = REG_GPIO_GLDOEN;
        reg_gpio_gdout = REG_GPIO_GLDOUT;
    }
    reg_gpio_gdoen_value = READ_REG(reg_gpio_gdoen);
    reg_gpio_gdout_value = READ_REG(reg_gpio_gdout);
    if(level) {
        /* output disable */
        WRITE_REG(reg_gpio_gdoen, reg_gpio_gdoen_value | (1 << pinnum));
        /* input enable when output disable */
        WRITE_REG(reg_gpio_gdout, reg_gpio_gdout_value | (1 << pinnum));
    } else {
        /* input disable when output disable, output 0 when output enable */
        WRITE_REG(reg_gpio_gdout, reg_gpio_gdout_value & ~(1 << pinnum));
        /* output enable */
        WRITE_REG(reg_gpio_gdoen, reg_gpio_gdoen_value & ~(1 << pinnum));
    }
    // 恢复原来的PRIMASK状态
    __asm {
        MSR PRIMASK, primask   // 将C变量的值恢复到PRIMASK寄存器中
    }
}

__attribute__((always_inline)) static int gpio_od_pin_get(uint8_t pinnum)
{
    uint32_t reg_gpio_gdin;
    uint32_t level;

    if (pinnum < 32) {
        reg_gpio_gdin = REG_GPIO_GDIN;
    } else {
        pinnum -= 32;
        reg_gpio_gdin = REG_GPIO_GLDIN;
    }

    level = (READ_REG(reg_gpio_gdin) >> pinnum) & 0x01;

    return level;
}


#ifdef __cplusplus
}
#endif

#endif /* _VPLA_DIRECT_GPIO_H_ */
