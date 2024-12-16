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
        irq

Description:
        irq Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vdrv_spiflash.h"
#include "vdrv_gpio.h"



void VDRV_EnableDebugPort ( void )
{
    VGPIO_SetLMuxFunction ( GPIO_L_PIN_11, GPIO_L_PIN_11 );
    VGPIO_SetLMux2Function ( GPIO_L_PIN_11, GPIO_L_PIN_11 );
    VGPIO_SetLMuxFunction ( GPIO_L_PIN_10, GPIO_L_PIN_10 );
    VGPIO_SetLMux2Function ( GPIO_L_PIN_10, GPIO_L_PIN_10 );
}
