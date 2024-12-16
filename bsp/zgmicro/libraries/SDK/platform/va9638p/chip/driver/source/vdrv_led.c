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
    LED

Description:
    LED driver.

Revision History:
    Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vdrv_gpio.h"
#include "vdrv_led.h"

#define VLED_REG_LTEN_LTX_EN_SHFT(X) (X << 1)
#define VLED_REG_LTEN_LTX_EN_MSK(X) (0x00000003 << VLED_REG_LTEN_LTX_EN_SHFT(X))

#define VLED_REG_LTX_CTRL1_ONOFF_TIMES_SHFT 28
#define VLED_REG_LTX_CTRL1_ONOFF_TIMES_MSK 0xF0000000
#define VLED_REG_LTX_CTRL1_OFF_LEN_SHFT 14
#define VLED_REG_LTX_CTRL1_OFF_LEN_MSK 0x0FFFC000
#define VLED_REG_LTX_CTRL1_ON_LEN_SHFT 0
#define VLED_REG_LTX_CTRL1_ON_LEN_MSK 0x00003FFF

#define VLED_REG_LTX_CTRL2_RPT_TIMES_SHFT 24
#define VLED_REG_LTX_CTRL2_RPT_TIMES_MSK 0x0F000000
#define VLED_REG_LTX_CTRL2_LEVEL_INV_SHFT 21
#define VLED_REG_LTX_CTRL2_LEVEL_INV_MSK 0x00200000
#define VLED_REG_LTX_CTRL2_ONOFF_INTVL_SHFT 20
#define VLED_REG_LTX_CTRL2_ONOFF_INTVL_MSK 0x00100000
#define VLED_REG_LTX_CTRL2_INTVL_LEN_SHFT 0
#define VLED_REG_LTX_CTRL2_INTVL_LEN_MSK 0x000FFFFF

static UINT32 VLED_IsInverterEnabled(UINT8 LED_Index);
/******************************************************************************
Description:
        Set the work mode of LEDx.

Parameters:
        leds: a bit map, the n-th bit indicates the GPIO number which can be muxed as LED[n].
        e.g. GPIO_MUX_FUNC_LED_ALL.
Return Value:
Notes:
*****************************************************************************/
void VLED_MuxGPIO(UINT32 led_pins)
{
    VGPIO_SetMuxFunction(led_pins, led_pins);
}

void VLED_Mux2GPIO(UINT32 led_pins)
{
    VGPIO_SetMux2Function(led_pins, led_pins);
}

/******************************************************************************
Description:
        Set the work mode of LEDx.

Parameters:
        WorkMode =
          2'b00, always low;
          2'b01, always high;
          2'b1x, flash in a certain pattern.
Return Value:
Notes:
        The flash pattern is: on -> off -> repeat the previous "on/off" pattern for some times -> interval ->
 repeat the previous "on/off/repeat/interval" for some times.
*****************************************************************************/
void VLED_Set_WorkMode(UINT8 LED_Index, UINT8  workmode)
{
    UINT32 reg;
    reg = READ_REG(REG_TIMER_LTEN);
    reg &= ~(VLED_REG_LTEN_LTX_EN_MSK(LED_Index));
    reg |= (VLED_WORK_MODE_ALWAYS_LOW << VLED_REG_LTEN_LTX_EN_SHFT(LED_Index)); 
    WRITE_REG(REG_TIMER_LTEN, reg);

    reg &= ~(VLED_REG_LTEN_LTX_EN_MSK(LED_Index));
    if(workmode != VLED_WORK_MODE_FLASH && VLED_IsInverterEnabled(LED_Index))
    {
        reg |= ((workmode == VLED_WORK_MODE_ALWAYS_LOW?VLED_WORK_MODE_ALWAYS_HIGH:VLED_WORK_MODE_ALWAYS_LOW) << VLED_REG_LTEN_LTX_EN_SHFT(LED_Index));
    }else{
        reg |= (workmode << VLED_REG_LTEN_LTX_EN_SHFT(LED_Index));
    }

    WRITE_REG(REG_TIMER_LTEN, reg);
}

/******************************************************************************
Description:
        Set the work mode of LEDx.

Parameters:
        WorkMode =
          2'b00, always low;
          2'b01, always high;
          2'b1x, flash in a certain pattern.
Return Value:
Notes:
        The flash pattern is: on -> off -> repeat the previous "on/off" pattern for some times -> interval ->
 repeat the previous "on/off/repeat/interval" for some times.
*****************************************************************************/
void VLED_Config_Scheme(UINT8 LED_Index, VLED_SCHEME *p_Scheme)
{
    UINT32 reg_ctrl1;
    UINT32 reg_ctrl2;
    UINT32 value;

    switch (LED_Index)
    {
    case VLED_INDEX_0:
        reg_ctrl1 = REG_TIMER_LT1_CTRL1;
        reg_ctrl2 = REG_TIMER_LT1_CTRL2;
        break;
    case VLED_INDEX_1:
        reg_ctrl1 = REG_TIMER_LT2_CTRL1;
        reg_ctrl2 = REG_TIMER_LT2_CTRL2;
        break;
    case VLED_INDEX_2:
        reg_ctrl1 = REG_TIMER_LT3_CTRL1;
        reg_ctrl2 = REG_TIMER_LT3_CTRL2;
        break;
    case VLED_INDEX_3:
        reg_ctrl1 = REG_TIMER_LT4_CTRL1;
        reg_ctrl2 = REG_TIMER_LT4_CTRL2;
        break;
    default:
        return;
    }

    if (p_Scheme != NULL)
    {
        value = (((UINT32)p_Scheme->on_length) << VLED_REG_LTX_CTRL1_ON_LEN_SHFT) |
                (((UINT32)p_Scheme->off_length) << VLED_REG_LTX_CTRL1_OFF_LEN_SHFT) |
                (((UINT32)p_Scheme->onoff_times) << VLED_REG_LTX_CTRL1_ONOFF_TIMES_SHFT);
        WRITE_REG(reg_ctrl1, value);
        value = (((UINT32)p_Scheme->intvl_length) << VLED_REG_LTX_CTRL2_INTVL_LEN_SHFT) |
                (((p_Scheme->intvl_state == p_Scheme->onoff_level) ? 0 : 1) << VLED_REG_LTX_CTRL2_ONOFF_INTVL_SHFT) |
                ((p_Scheme->onoff_level ? 1 : 0) << VLED_REG_LTX_CTRL2_LEVEL_INV_SHFT) |
                (((UINT32)p_Scheme->repeat_times) << VLED_REG_LTX_CTRL2_RPT_TIMES_SHFT);
        WRITE_REG(reg_ctrl2, value);
    }
}

//
// When inverter is enabled, the flash pattern will generate 0->1->0 signals, otherwise is 1->0->1;
// However, no matter whether inverter is enabled, the ON/OFF pattern will generate 1/0 signals.
//
UINT32 VLED_IsInverterEnabled(UINT8 LED_Index)
{
    UINT32 reg_ctrl2;
    UINT32 value;

    switch (LED_Index)
    {
    case VLED_INDEX_0:
        reg_ctrl2 = REG_TIMER_LT1_CTRL2;
        break;
    case VLED_INDEX_1:
        reg_ctrl2 = REG_TIMER_LT2_CTRL2;
        break;
    case VLED_INDEX_2:
        reg_ctrl2 = REG_TIMER_LT3_CTRL2;
        break;
    case VLED_INDEX_3:
        reg_ctrl2 = REG_TIMER_LT4_CTRL2;
        break;
    default:
        return 0;
    }
    value = READ_REG(reg_ctrl2);
    return (value & (1 << VLED_REG_LTX_CTRL2_LEVEL_INV_SHFT)) ? 1 : 0;
}

 
