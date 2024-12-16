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
        led

Description:
        led Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_led.h"
#include "vdrv_gpio.h"
#include "vdrv_led.h"
#include "vdebug_errno.h"
   
VRET_VALUE VSYS_LED_Init(VDRVLEDSel_t ledx)
{   
    switch (ledx)
    {
        case VSYS_LED_INDEX_0:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED0,GPIO_MUX_FUNC_LED0); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED0,GPIO_MUX_FUNC_LED0);
            break;
        case VSYS_LED_INDEX_1:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED1,GPIO_MUX_FUNC_LED1); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED1,GPIO_MUX_FUNC_LED1);
            break;
        case VSYS_LED_INDEX_2:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED2,GPIO_MUX_FUNC_LED2); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED2,GPIO_MUX_FUNC_LED2);
            break;
        case VSYS_LED_INDEX_3:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED3,GPIO_MUX_FUNC_LED3); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED3,GPIO_MUX_FUNC_LED3);
            break;
        default:
            __VERRNO_STORE(VERRNO_LED_INVALID);
            return VRET_ERROR;
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_LED_Uninit(VDRVLEDSel_t ledx)
{
    switch (ledx)
    {
        case VSYS_LED_INDEX_0:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED0,GPIO_MUX_FUNC_NONE); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED0,GPIO_MUX_FUNC_NONE);
            break;
        case VSYS_LED_INDEX_1:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED1,GPIO_MUX_FUNC_NONE); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED1,GPIO_MUX_FUNC_NONE);
            break;
        case VSYS_LED_INDEX_2:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED2,GPIO_MUX_FUNC_NONE); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED2,GPIO_MUX_FUNC_NONE);
            break;
        case VSYS_LED_INDEX_3:
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_LED3,GPIO_MUX_FUNC_NONE); 
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_LED3,GPIO_MUX_FUNC_NONE);
            break;
        default:
            __VERRNO_STORE(VERRNO_LED_INVALID);
            return VRET_ERROR;
    }
    VLED_Set_WorkMode(ledx,VLED_WORK_MODE_ALWAYS_LOW);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_LED_SetMode(VDRVLEDSel_t ledx, VDRVLEDConfig_t *led_config)
{
    VLED_Config_Scheme(ledx,(VLED_SCHEME*)led_config);
    if(led_config->on_length != 0 && led_config->off_length == 0){
        VLED_Set_WorkMode(ledx,VLED_WORK_MODE_ALWAYS_HIGH);
    }else if(led_config->on_length == 0 && led_config->off_length != 0){
        VLED_Set_WorkMode(ledx,VLED_WORK_MODE_ALWAYS_LOW);
    }else {
        VLED_Set_WorkMode(ledx,VLED_WORK_MODE_FLASH);
    }
    return VRET_NO_ERROR;
}
