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
                        zuo.longjun                        initial version
    2022.0620           cao.xuetao             
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VLED_H_
#define _VLED_H_

#include "vstdlib.h"
#ifdef __cplusplus
extern "C" {
#endif

#define VLED_WORK_MODE_ALWAYS_LOW               0x0
#define VLED_WORK_MODE_ALWAYS_HIGH              0x1
#define VLED_WORK_MODE_FLASH                    0x2

#define VLED_INDEX_0                                    0
#define VLED_INDEX_1                                    1
#define VLED_INDEX_2                                    2
#define VLED_INDEX_3                                    3
#define VLED_TOTAL_NUM                                  4

#define VLED_ONOFF_LENGTH_MIN                   0x0001
#define VLED_ONOFF_LENGTH_MAX                   0x3FFF
#define VLED_ONOFF_TIMES_MIN                    0x00
#define VLED_ONOFF_TIMES_MAX                    0x0F
#define VLED_INTVL_LENGTH_MIN                   0x00000001
#define VLED_INTVL_LENGTH_MAX                   0x000FFFFF
#define VLED_REPEAT_TIMES_MIN                   0x00
#define VLED_REPEAT_TIMES_MAX                   0x0F
#define VLED_INFINIT_REPEAT                          0x0

/*              |-on-|-off-|......|-on-|-off-|-----interval------|
*               |<-----onoff_times------->|
*               |<--------------repeat_times = 1----------->|
*/
//Time unit: 8ms
typedef struct VLED_SCHEME_TAG {
        UINT16 on_length; /* unit is millisecond, 0<on_length<=0x3FFF */
        UINT16 off_length; /* unit is millisecond, 0<off_length<=0x3FFF */
        BOOL onoff_level; /* TRUE means on level is high and off level is low, FALSE means invert the level */
        UINT8 onoff_times; /* 0<onoff_times<0xF */
        BOOL intvl_state; /* indicate led on or off during Interval, TRUE=on, FALSE=off */
        UINT32 intvl_length; /* interval lenght, unit is millisecond, 0<intvl_length<=0x000FFFFF */
        UINT8 repeat_times; /* 0<=repeat_times<0xF, 0 means repeat infinite */
} VLED_SCHEME;


void VLED_MuxGPIO(UINT32 led_pins);
void VLED_Mux2GPIO(UINT32 led_pins);
void VLED_Set_WorkMode(UINT8 LED_Index, UINT8  workmode);
UINT8 VLED_Get_WorkMode(UINT8 LED_Index);
void VLED_Config_Scheme(UINT8 LED_Index, VLED_SCHEME* p_Scheme);

#ifdef __cplusplus
}
#endif
#endif

