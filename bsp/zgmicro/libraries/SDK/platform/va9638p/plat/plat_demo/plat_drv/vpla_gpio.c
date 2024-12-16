/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
        gpio

Description:
        gpio Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_gpio.h"
#include "vdrv_gpio.h"
#include "vdebug_errno.h"
#include "vreg_def.h"

VDRVGpioUmonitor_t g_VSYS_GPIO_UMonitor_Cfg;
UINT32 g_VSYS_GPIO_ODMode_Pins = 0, g_VSYS_GPIO_ODMode_LPins = 0;

VRET_VALUE VSYS_GPIO_Init(UINT64 Pins, VDRVGpioMode_t Mode, VDRVGpioModeParam_t Param)
{
    UINT32 gpio_pins = 0 , gpiol_pins = 0;
    if((Pins & VSYS_GPIOX_PIN_ALL) != Pins)
    {
        __VERRNO_STORE(VERRNO_GPIO_INVALID);
        return VRET_ERROR;
    }

    gpio_pins = Pins&0xFFFFFFFF;
    gpiol_pins = (Pins>>32)&0xFFFFFFFF;
    if(gpio_pins){
        VGPIO_DisableInt(gpio_pins);
        VGPIO_SetUmonitorEn(VGPIO_GetUmonitorEn()&~gpio_pins);
        VGPIO_SetMuxFunction(gpio_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMux2Function(gpio_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMode(gpio_pins, Mode, Param);
        if (Mode == VSYS_GPIO_MODE_OUTPUT)
        {
            if(Param == VSYS_GPIO_OUTPUT_OD_MODE)
                g_VSYS_GPIO_ODMode_Pins |= gpio_pins;
            else 
                g_VSYS_GPIO_ODMode_Pins &= ~gpio_pins;
        }
    }
    if(gpiol_pins){
        VGPIO_L_DisableInt(gpiol_pins);
        VGPIO_SetLMuxFunction(gpiol_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMux2Function(gpiol_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMode(gpiol_pins, Mode, Param);
        if (Mode == VSYS_GPIO_MODE_OUTPUT)
        {
            if(Param == VSYS_GPIO_OUTPUT_OD_MODE)
                g_VSYS_GPIO_ODMode_LPins |= gpiol_pins;
            else
                g_VSYS_GPIO_ODMode_LPins &= ~gpiol_pins;
        }
    }
    
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_GPIO_Uninit(UINT64 Pins)
{
    UINT32 gpio_pins = 0 , gpiol_pins = 0;

    if((Pins & VSYS_GPIOX_PIN_ALL) != Pins)
    {
        __VERRNO_STORE(VERRNO_GPIO_INVALID);
        return VRET_ERROR;
    }

    gpio_pins = Pins&0xFFFFFFFF;
    gpiol_pins = (Pins>>32)&0xFFFFFFFF;
    if(gpio_pins){
        VGPIO_DisableInt(gpio_pins);
        VGPIO_SetUmonitorEn(VGPIO_GetUmonitorEn()&~gpio_pins);
        VGPIO_SetMuxFunction(gpio_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMux2Function(gpio_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMode(gpio_pins, GPIO_MODE_Z, 0);
        g_VSYS_GPIO_ODMode_Pins &= ~gpio_pins;
    }
    if(gpiol_pins){
        VGPIO_L_DisableInt(gpiol_pins);
        VGPIO_SetLMuxFunction(gpiol_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMux2Function(gpiol_pins, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMode(gpiol_pins, GPIO_MODE_Z, 0);
        g_VSYS_GPIO_ODMode_LPins &= ~gpiol_pins;
    }
    
    return VRET_NO_ERROR;
}

UINT64 VSYS_GPIO_GetPinInput(UINT64 Pins)
{
    UINT64 Pins_ret = 0;
    UINT32 gpio_pins = Pins & 0xFFFFFFFF, gpiol_pins = (Pins >> 32) & 0xFFFFFFFF;
    UINT32 pins_t = gpio_pins & g_VSYS_GPIO_ODMode_Pins, pinsl_t = gpiol_pins & g_VSYS_GPIO_ODMode_LPins;

    if (pins_t) {
        Pins_ret |= READ_REG(REG_GPIO_GDIN)&pins_t;
        gpio_pins &= ~pins_t;
    }
    if (pinsl_t) {
        Pins_ret |= ((UINT64)(READ_REG(REG_GPIO_GLDIN)&pinsl_t)) << 32;
        gpiol_pins &= ~pinsl_t;
    }
    if (gpio_pins) {
        pins_t = VGPIO_GetInput();
        //Support output fetch
        UINT32 gpio_outpins, gpio_outen;
        gpio_outpins = VGPIO_GetOutput();
        gpio_outen = VGPIO_GetOutEn();
        pins_t &= ~gpio_outen;
        pins_t |= gpio_outpins & gpio_outen;
        //-------
        Pins_ret |= pins_t&gpio_pins;
    }
    if (gpiol_pins) {
        pinsl_t = VGPIO_GetLInput();
        //Support output fetch
        UINT32 gpiol_outpins, gpiol_outen;
        gpiol_outpins = VGPIOL_GetOutput();
        gpiol_outen = VGPIOL_GetOutEn();
        pinsl_t &= ~gpiol_outen;
        pinsl_t |= gpiol_outpins & gpiol_outen;
        //-------
        Pins_ret |= ((UINT64)(gpiol_pins&pinsl_t)) << 32;
    }

    return Pins_ret;
}

VRET_VALUE VSYS_GPIO_SetPinOutput(UINT64 Pins, BOOL BitVal)
{
    UINT32 gpio_pins = Pins & 0xFFFFFFFF, gpiol_pins = (Pins >> 32) & 0xFFFFFFFF;
    UINT32 pins_t = gpio_pins & g_VSYS_GPIO_ODMode_Pins, pinsl_t = gpiol_pins & g_VSYS_GPIO_ODMode_LPins;
    if (pins_t) {
        if (BitVal) {
            WRITE_REG(REG_GPIO_GDOEN, READ_REG(REG_GPIO_GDOEN) | pins_t);
            WRITE_REG(REG_GPIO_GDOUT, READ_REG(REG_GPIO_GDOUT) | pins_t);
        } else {
            WRITE_REG(REG_GPIO_GDOUT, READ_REG(REG_GPIO_GDOUT) & ~pins_t);
            WRITE_REG(REG_GPIO_GDOEN, READ_REG(REG_GPIO_GDOEN) & ~pins_t);
        }
        gpio_pins &= ~pins_t;
    }
    if (pinsl_t) {
        if (BitVal) {
            WRITE_REG(REG_GPIO_GLDOEN, READ_REG(REG_GPIO_GLDOEN) | pinsl_t);
            WRITE_REG(REG_GPIO_GLDOUT, READ_REG(REG_GPIO_GLDOUT) | pinsl_t);
        } else {
            WRITE_REG(REG_GPIO_GLDOUT, READ_REG(REG_GPIO_GLDOUT) & ~pinsl_t);
            WRITE_REG(REG_GPIO_GLDOEN, READ_REG(REG_GPIO_GLDOEN) & ~pinsl_t);
        }
        gpiol_pins &= ~pinsl_t;
    }
    if (gpio_pins) {
        VGPIO_SetPinsOutput(gpio_pins, BitVal);
    }
    if (gpiol_pins) {
        VGPIO_SetLPinsOutput(gpiol_pins, BitVal);
    }

    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_GPIO_EnableInt(UINT64 Pins, VDRVGpioIntMode_t type)
{
    UINT32 gpio_pins = 0 , gpiol_pins = 0;
    gpio_pins = Pins&0xFFFFFFFF;
    gpiol_pins = (Pins>>32)&0xFFFFFFFF;
    if(gpio_pins){
        switch(type){
            case VSYS_GPIO_INTTYPE_INVERT_MODE:
                GPIO_Set_Invert(gpio_pins);
            case VSYS_GPIO_INTTYPE_NORMAL_MODE:
                VGPIO_EnableInt(gpio_pins);
                break;
            case VSYS_GPIO_INTTYPE_DISABLE:
                VGPIO_DisableInt(gpio_pins);
        }
    }
    if(gpiol_pins){
        switch(type){
            case VSYS_GPIO_INTTYPE_INVERT_MODE:
                GPIO_Set_L_Invert(gpiol_pins);
            case VSYS_GPIO_INTTYPE_NORMAL_MODE:
                VGPIO_L_EnableInt(gpiol_pins);
                break;
            case VSYS_GPIO_INTTYPE_DISABLE:
                VGPIO_L_DisableInt(gpiol_pins);
        }
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_GPIO_SetUmonitor(VDRVGpioUmonitor_t *config)
{
    UINT32 gpio_pins = 0;
    if(config == NULL){
        config = &g_VSYS_GPIO_UMonitor_Cfg;
    }      
    gpio_pins = config->pins&0xFFFFFFFF;
    VGPIO_SetMuxFunction(gpio_pins, 0); 
    VGPIO_SetMux2Function(gpio_pins, 0);        
    VGPIO_SetUmonitorEn(gpio_pins);
    VGPIO_ConfigUmonitor(config->module, config->sig);
    return VRET_NO_ERROR;
}

SINT8 VSYS_GPIO_Get_Index(UINT64 Pins)
{
	SINT8 index = VCOMMON_LsBit((UINT32)Pins);
	
	if (index == -1){
		index = VCOMMON_LsBit((UINT32)(Pins>>32));
		if (index != -1){
			index += 32;
		}
	}
	return index;
}






