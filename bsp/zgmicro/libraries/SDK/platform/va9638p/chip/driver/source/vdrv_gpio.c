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
    GPIO

Description:
    GPIO driver.

Revision History:
    Date                  Author                   Description
    2010.03.05           chenjingen              initial version
    2022.05.30            cxt                       9650
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vdrv_gpio.h"
#include "vstdcore.h"
#include "vreg_def.h"

/******************************************************************************
Description:
    Configures GPIO mode.

Parameters:
    Pins - which pins to configure, can be any combination of valid GPIO pin
           definitions by performing bit-wise OR operation.
    Mode - 1 for input, 0 for output.
    Param - for input, Param used as input invertion setting,
            0 for non-invertion, 1 for invertion;
            for output, Param used as output drive strength setting,
            0 for low strength, 1 for high strength

Return Value:
    None.

Notes:
    None.
*****************************************************************************/
void VGPIO_SetMode ( UINT32 Pins, UINT8 Mode, UINT8 Param )
{
    if ( Mode == GPIO_MODE_INPUT ) {
        /* input setting. */
        WRITE_REG ( REG_GPIO_GDOEN, READ_REG ( REG_GPIO_GDOEN ) | Pins );
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        WRITE_REG ( REG_GPIO_GDOUT, READ_REG ( REG_GPIO_GDOUT ) | Pins );
#else
			BB_GPIO_GDOUT_SET(Pins);
#endif
        if ( Param & 0x02) {
            WRITE_REG ( REG_GPIO_GDDRIVE, READ_REG ( REG_GPIO_GDDRIVE ) & ~Pins ); 
        } else {
            WRITE_REG ( REG_GPIO_GDDRIVE, READ_REG ( REG_GPIO_GDDRIVE ) | Pins ); // pull up/down enable.
        }
        /* input invert setting */
        if ( Param & 0x01) {
            WRITE_REG ( REG_GPIO_GDINVEN, READ_REG ( REG_GPIO_GDINVEN ) | Pins );
        } else {
            WRITE_REG ( REG_GPIO_GDINVEN, READ_REG ( REG_GPIO_GDINVEN ) & ~ ( Pins ) );
        }

        /* at least delay 10us for state transition */
        //VCOMMON_Delay ( 10 );
    } else if ( Mode == GPIO_MODE_Z ) {
        /* Z */
        WRITE_REG ( REG_GPIO_GDOEN, READ_REG ( REG_GPIO_GDOEN ) | Pins );
        WRITE_REG ( REG_GPIO_GDDRIVE, READ_REG ( REG_GPIO_GDDRIVE ) & ~ ( Pins ) );
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        WRITE_REG ( REG_GPIO_GDOUT, READ_REG ( REG_GPIO_GDOUT ) & ~ ( Pins ) );
#else
			BB_GPIO_GDOUT_CLR(Pins);
#endif
    } else {
        /* output setting */
        WRITE_REG ( REG_GPIO_GDOEN, READ_REG ( REG_GPIO_GDOEN ) & ~ ( Pins ) );
        /* output strength setting */
        if ( Param & 0x01) {
            WRITE_REG ( REG_GPIO_GDDRIVE, READ_REG ( REG_GPIO_GDDRIVE ) | Pins );
        } else {
            WRITE_REG ( REG_GPIO_GDDRIVE, READ_REG ( REG_GPIO_GDDRIVE ) & ~ ( Pins ) );
        }
    }
}
/******************************************************************************
Description:
    Configures GPIOL mode.

Parameters:
    Pins - which pins to configure, can be any combination of valid GPIOL pin
           definitions by performing bit-wise OR operation.
    Mode - 1 for input, 0 for output.
    Param - for input, Param used as input invertion setting,
            0 for non-invertion, 1 for invertion;
            for output, Param used as output drive strength setting,
            0 for low strength, 1 for high strength

Return Value:
    None.

Notes:
    None.
*****************************************************************************/
void VGPIO_SetLMode ( UINT32 Pins, UINT8 Mode, UINT8 Param )
{
    if ( Mode == GPIO_MODE_INPUT ) {
        /* input setting. */
        WRITE_REG ( REG_GPIO_GLDOEN, READ_REG ( REG_GPIO_GLDOEN ) | Pins );
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        WRITE_REG ( REG_GPIO_GLDOUT, READ_REG ( REG_GPIO_GLDOUT ) | Pins );
#else
		BB_GPIOL_GDOUT_SET(Pins);
#endif
        if ( Param & 0x02) {
            WRITE_REG ( REG_GPIO_GLDDRIVE, READ_REG ( REG_GPIO_GLDDRIVE ) & ~Pins );
        } else {
            WRITE_REG ( REG_GPIO_GLDDRIVE, READ_REG ( REG_GPIO_GLDDRIVE ) | Pins ); // pull up/down enable.
        }
        /* input invert setting */
        if ( Param & 0x01 ) {
            WRITE_REG ( REG_GPIO_GLDINVEN, READ_REG ( REG_GPIO_GLDINVEN ) | Pins );
        } else {
            WRITE_REG ( REG_GPIO_GLDINVEN,    READ_REG ( REG_GPIO_GLDINVEN ) & ~ ( Pins ) );
        }

        /* at least delay 10us for state transition */
        //VCOMMON_Delay ( 10 );
    } else if ( Mode == GPIO_MODE_Z ) {
        /* Z */
        WRITE_REG ( REG_GPIO_GLDOEN, READ_REG ( REG_GPIO_GLDOEN ) | Pins );
        WRITE_REG ( REG_GPIO_GLDDRIVE,    READ_REG ( REG_GPIO_GLDDRIVE ) & ~ ( Pins ) );
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        WRITE_REG ( REG_GPIO_GLDOUT, READ_REG ( REG_GPIO_GLDOUT ) & ~ ( Pins ) );
#else
			BB_GPIOL_GDOUT_CLR(Pins);
#endif
    } else {
        /* output setting */
        WRITE_REG ( REG_GPIO_GLDOEN, READ_REG ( REG_GPIO_GLDOEN ) & ~ ( Pins ) );
        /* output strength setting */
        if ( Param & 0x01 ) {
            WRITE_REG ( REG_GPIO_GLDDRIVE, READ_REG ( REG_GPIO_GLDDRIVE ) | Pins );
        } else {
            WRITE_REG ( REG_GPIO_GLDDRIVE, READ_REG ( REG_GPIO_GLDDRIVE ) & ~ ( Pins ) );
        }
    }
}



/******************************************************************************
Description:
    Reads the values present of GPIO port.

Parameters:
    GPIO register addr.

Return Value:
    Returns input data value of GPIO port.
*****************************************************************************/

UINT32 VGPIO_GetInput ( void/*UINT32 addr*/ )
{
    UINT32 invert;
    UINT32 pin_states;
    //VCPU_SR_ALLOC();
    //VCPU_CRITICAL_ENTER();
    invert = READ_REG ( REG_GPIO_GDINVEN );
    pin_states = ( ( UINT32 ) READ_REG ( REG_GPIO_GDIN/*addr*/ ) ) ^ invert;
    //pin_states = ((UINT32)READ_REG(addr));
    //VCPU_CRITICAL_EXIT();
    //printf("%s,pin_states=%x\r\n",FUNCTION_NAME,pin_states);
    return pin_states;
}

/******************************************************************************
Description:
    Reads the values present of GPIOL port.

Parameters:
    GPIO register addr.

Return Value:
    Returns input data value of GPIO port.
*****************************************************************************/

UINT32 VGPIO_GetLInput ( void/*UINT32 addr*/ )
{
    UINT32 invert;
    UINT32 pin_states;
    //VCPU_SR_ALLOC();
    //VCPU_CRITICAL_ENTER();
    invert = READ_REG ( REG_GPIO_GLDINVEN );

    pin_states = ( ( UINT32 ) READ_REG ( REG_GPIO_GLDIN/*addr*/ ) ) ^ invert;
    //pin_states = ((UINT32)READ_REG(addr));
    //VCPU_CRITICAL_EXIT();
    //printf("%s,pin_states=%x\r\n",FUNCTION_NAME,pin_states);
    return pin_states;
}


/******************************************************************************
Description:
    Reads the value present of the specified pin in GPIO port.

Parameters:
    PinNum - the numeric value of GPIO pin, e.g. 0 for GPIO_PIN_0,
             5 for GPIO_PIN_5
    range   GPIO0 ~ GPIO22(PinNum is 0~22)

Return Value:
    Returns input data value of the specified GPIO pin.
*****************************************************************************/
UINT8 VGPIO_GetPinInput ( UINT8 PinNum )
{
    UINT8 invert;
    UINT8 pin_states;

    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    invert = VGPIO_GetInvert ( PinNum );

    pin_states = ( ( UINT8 ) GET_REG_BIT2 ( REG_GPIO_GDIN, PinNum ) ) ^ invert;
    VCPU_CRITICAL_EXIT();

    return pin_states;
}


/******************************************************************************
Description:
    Reads the value present of the specified pin in GPIOL port.

Parameters:
    PinNum - the numeric value of GPIOL pin, e.g. 0 for GPIOL_PIN_0,
             5 for GPIOL_PIN_5.
    range   GPIO0 ~ GPIO16(PinNum is 0~16)
Return Value:
    Returns input data value of the specified GPIO pin.
*****************************************************************************/
UINT8 VGPIO_GetLPinInput ( UINT8 PinNum )
{
    UINT8 invert;
    UINT8 pin_states;
    //UINT32 addr_offset = 0;
    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    invert = VGPIO_GetLInvert ( PinNum );
    pin_states = ( ( UINT8 ) GET_REG_BIT2 ( REG_GPIO_GLDIN, PinNum ) ) ^ invert;
    VCPU_CRITICAL_EXIT();

    return pin_states;
}

/******************************************************************************
Description:
    read the value to the GPIO port.

Parameters:
    OutputVal - the value to write to the port.

Return Value:
    None.

Remarks:
    Only the output-enabled pins will be effected.
*****************************************************************************/
UINT32 VGPIO_GetOutput ( void)
{
    return READ_REG (REG_GPIO_GDOUT);
}

UINT32 VGPIOL_GetOutput ( void)
{
    return READ_REG (REG_GPIO_GLDOUT);
}

UINT32 VGPIO_GetOutEn ( void)
{
    return ~READ_REG (REG_GPIO_GDOEN);
}

UINT32 VGPIOL_GetOutEn ( void)
{
    return (~READ_REG (REG_GPIO_GLDOEN)&0xFFFFFF);
}

/******************************************************************************
Description:
    Writes the value to the specified pin in GPIO port.

Parameters:
    PinNum - the numeric value of GPIO pin, e.g. 0 for GPIO0,
             37 for GPIO37.
    BitVal - the value to write to the specified pin.

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetPinOutput ( UINT8 PinNum, UINT8 BitVal )
{
    if ( BitVal ) {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        SET_REG_BIT2 ( REG_GPIO_GDOUT, PinNum );
#else
		WRITE_REG(BB_GPIO_BASE+PinNum*4,0x1);
#endif
    } else {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        CLR_REG_BIT2 ( REG_GPIO_GDOUT, PinNum );
#else
		WRITE_REG(BB_GPIO_BASE+PinNum*4,0x0);
#endif
    }
}

void VGPIO_SetPinsOutput ( UINT32 Pins, UINT8 BitVal )
{
	UINT32 val = READ_REG (REG_GPIO_GDOUT);
    if ( BitVal ) {
		val |= Pins;
    } else {
		val &= ~Pins;
    }
	WRITE_REG(REG_GPIO_GDOUT,val);
}

void VGPIO_SetPinsOutput_OD (UINT32 Pins, UINT8 BitVal )
{
	UINT32 val = READ_REG (REG_GPIO_GDOUT);
    if ( BitVal ) {
        WRITE_REG(REG_GPIO_GDOEN, val | Pins);
        WRITE_REG(REG_GPIO_GDOUT, val | Pins);
    } else {
		WRITE_REG(REG_GPIO_GDOUT, val &~Pins);
        WRITE_REG(REG_GPIO_GDOEN, val &~Pins);
    }
}

/******************************************************************************
Description:
    Writes the value to the specified pin in GPIO L port.

Parameters:
    PinNum - the numeric value of GPIO pin, e.g. 0 for GPIO0,
             37 for GPIO37.
    BitVal - the value to write to the specified pin.

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetLPinOutput ( UINT8 PinNum, UINT8 BitVal )
{

    if ( BitVal ) {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        SET_REG_BIT2 ( REG_GPIO_GLDOUT, PinNum );
#else
		WRITE_REG(BB_GPIOL_BASE+PinNum*4,0x1);
#endif
    } else {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
        CLR_REG_BIT2 ( REG_GPIO_GLDOUT, PinNum );
#else
		WRITE_REG(BB_GPIOL_BASE+PinNum*4,0x0);
#endif
	}
}

void VGPIO_SetLPinsOutput ( UINT32 Pins, UINT8 BitVal )
{
	UINT32 val = READ_REG (REG_GPIO_GLDOUT);
    if ( BitVal ) {
		val |= Pins;
    } else {
		val &= ~Pins;
    }
	WRITE_REG(REG_GPIO_GLDOUT,val);
}

void VGPIO_SetLPinsOutput_OD (UINT32 Pins, UINT8 BitVal )
{
	UINT32 val = READ_REG (REG_GPIO_GLDOUT);
    if ( BitVal ) {
        WRITE_REG(REG_GPIO_GLDOEN, val | Pins);
        WRITE_REG(REG_GPIO_GLDOUT, val | Pins);
    } else {
		WRITE_REG(REG_GPIO_GLDOUT, val &~Pins);
        WRITE_REG(REG_GPIO_GLDOEN, val &~Pins);
    }
}

/******************************************************************************
Description:
    Change the value and wirte to the specified pin in GPIO port.

Parameters:
    io_mask:GPIO_PIN_0->pin num 0;
            GPIO_PIN_1->pin mun 1;
            GPIO_PIN_2->pin num 2;
            ......
            GPIO_PIN_22->pin num 22;
Return Value:
    None.
*****************************************************************************/

void VGPIO_ChangePinOutput ( UINT32 io_mask )
{
    UINT8 PinNum;

    for ( PinNum = 0; PinNum < 32; PinNum++ ) {

        if ( io_mask & 0x1 ) {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
            if ( GET_REG_BIT2 ( REG_GPIO_GDOUT, PinNum ) ) {
                CLR_REG_BIT2 ( REG_GPIO_GDOUT, PinNum );
            } else {
                SET_REG_BIT2 ( REG_GPIO_GDOUT, PinNum );
            }
#else
		if (GET_BB_GPIO_GDOUT(PinNum)) {
				SET_BB_GPIO_GDOUT(PinNum,0x0);
		} else {			
				SET_BB_GPIO_GDOUT(PinNum,0x1);
        }
#endif
		}
        io_mask = io_mask >> 1;

    }
    return;
}

/******************************************************************************
Description:
    Change the value and wirte to the specified pin in GPIO L port.

Parameters:
    io_mask:GPIO_L_PIN_0->pin L num 0;
            GPIO_L_PIN_1->pin L mun 1;
            GPIO_L_PIN_2->pin Lnum 2;
            ......
            GPIO_L_PIN_16->pin L num 16;
Return Value:
    None.
*****************************************************************************/
void VGPIO_ChangeLPinOutput ( UINT32 io_mask )
{
    UINT8 PinNum;

    for ( PinNum = 0; PinNum < 32; PinNum++ ) {

        if ( io_mask & 0x1 ) {
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
            if ( GET_REG_BIT2 ( REG_GPIO_GLDOUT, PinNum ) ) {
                CLR_REG_BIT2 ( REG_GPIO_GLDOUT, PinNum );
            } else {
                SET_REG_BIT2 ( REG_GPIO_GLDOUT, PinNum );
            }
#else
			if (GET_BB_GPIOL_GDOUT(PinNum)) {
					SET_BB_GPIOL_GDOUT(PinNum,0x0);
			} else {			
					SET_BB_GPIOL_GDOUT(PinNum,0x1);
			}	
#endif
		}
		io_mask = io_mask >> 1;	
	}		
	return;
}

/******************************************************************************
Description:
    Enables interrupts for the specified pin(s).

Parameters:
    Pins - which pins to enable interrupt, can be any combination of valid GPIO
           pin definitions by performing bit-wise OR operation.

Return Value:
    None.
*****************************************************************************/
void VGPIO_EnableInt ( UINT32 Pins )
{
    WRITE_REG ( REG_GPIO_GINTMASK, ( READ_REG ( REG_GPIO_GINTMASK ) & ~ ( Pins ) ) );
    VCPU_INT_SrcEnable ( IRQ_GPIO_INT );
}


/******************************************************************************
Description:
    Enables interrupts for the specified pin(s).

Parameters:
    Pins - which pins to enable interrupt, can be any combination of valid GPIO L
           pin definitions by performing bit-wise OR operation.

Return Value:
    None.
*****************************************************************************/
void VGPIO_L_EnableInt ( UINT32 Pins )
{
    WRITE_REG ( REG_GPIO_GLINTMASK, ( READ_REG ( REG_GPIO_GLINTMASK ) & ~ ( Pins ) ) );
    VCPU_INT_SrcEnable ( IRQ_GPIO_INT );
}



/******************************************************************************
Description:
    Disables interrupts for the specified pin(s).

Parameters:
    Pins - which pins to disable interrupt, can be any combination of valid
           GPIO pin definitions by performing bit-wise OR operation.

Return Value:
    None.
*****************************************************************************/
void VGPIO_DisableInt ( UINT32 Pins )
{
    // mask
    WRITE_REG ( REG_GPIO_GINTMASK, ( READ_REG ( REG_GPIO_GINTMASK ) | Pins ) );
    if ( ( READ_REG ( REG_GPIO_GINTMASK ) & GPIO_PIN_ALL ) == GPIO_PIN_ALL ) {
        /* disable the NVIC irq source if all GPIO interrupts masked */
        VCPU_INT_SrcDisable ( IRQ_GPIO_INT );
    }
}

/******************************************************************************
Description:
    Disables interrupts for the specified L pin(s).

Parameters:
    Pins - which pins to disable interrupt, can be any combination of valid
           GPIO pin definitions by performing bit-wise OR operation.

Return Value:
    None.
*****************************************************************************/
void VGPIO_L_DisableInt ( UINT32 Pins )
{
    // mask
    WRITE_REG ( REG_GPIO_GLINTMASK, ( READ_REG ( REG_GPIO_GLINTMASK ) | Pins ) );
    if ( ( READ_REG ( REG_GPIO_GLINTMASK ) & GPIO_PIN_ALL ) == GPIO_PIN_ALL ) {
        /* disable the NVIC irq source if all GPIO interrupts masked */
        VCPU_INT_SrcDisable ( IRQ_GPIO_INT );
    }
}


/******************************************************************************
Description:
    Sets the interrupt type for the specified pin(s).

Parameters:
    Pins - which pins to set, can be any combination of valid GPIO pin
           definitions by performing bit-wise OR operation.
    Type - the type of interrupt, can be one of the following constants:
           GPIO_INTTYPE_HIGH_LEVEL, GPIO_INTTYPE_LOW_LEVEL and

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetIntType ( UINT32 Pins, UINT8 Type )
{
    if ( Type ) {
        WRITE_REG ( REG_GPIO_GDINVEN, READ_REG ( REG_GPIO_GDINVEN ) | Pins );
    } else {
        WRITE_REG ( REG_GPIO_GDINVEN, READ_REG ( REG_GPIO_GDINVEN ) & ~ ( Pins ) );
    }
}

void VGPIO_SetLIntType ( UINT32 Pins, UINT8 Type )
{
    if ( Type ) {
        WRITE_REG ( REG_GPIO_GLDINVEN, READ_REG ( REG_GPIO_GLDINVEN ) | Pins );
    } else {
        WRITE_REG ( REG_GPIO_GLDINVEN, READ_REG ( REG_GPIO_GLDINVEN ) & ~ ( Pins ) );
    }
}


/******************************************************************************
Description:
    Sets the interrupt type for the specified pin(s).

Parameters:
    Pins - which pins to set, can be any combination of valid GPIO pin
           definitions by performing bit-wise OR operation.
    Type - the type of interrupt, can be one of the following constants:
           GPIO_INTTYPE_HIGH_LEVEL, GPIO_INTTYPE_LOW_LEVEL and

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetInvert ( UINT8 PinNum, UINT8 BitVal )
{
    if ( BitVal ) {
        SET_REG_BIT2 ( REG_GPIO_GDINVEN, PinNum );
        // for test
        //VIO_TIMING_Set(16, 1);
    } else {
        CLR_REG_BIT2 ( REG_GPIO_GDINVEN, PinNum );
        //VIO_TIMING_Set(16, 0);
    }
}

void GPIO_Set_Invert(UINT32 Pins)
{
    UINT8 i;

    for (i = 0; i < sizeof(UINT32)*8; i++) {
        if (Pins & ((UINT32)1 << i)) {
            if(VGPIO_GetPinInput(i) == 1) {
                VGPIO_SetInvert(i, 1);
            }
            else {
                VGPIO_SetInvert(i, 0);
            }
        }
    }
}

void VGPIO_SetLInvert ( UINT8 PinNum, UINT8 BitVal )
{
    if ( BitVal ) {
        SET_REG_BIT2 ( REG_GPIO_GLDINVEN, PinNum );
        // for test
        //VIO_TIMING_Set(16, 1);
    } else {
        CLR_REG_BIT2 ( REG_GPIO_GLDINVEN, PinNum );
        //VIO_TIMING_Set(16, 0);
    }
}

void GPIO_Set_L_Invert(UINT32 Pins)
{
    UINT8 i;

    for (i = 0; i < sizeof(UINT32)*8; i++) {
        if (Pins & ((UINT32)1 << i)) {
            if(VGPIO_GetLPinInput(i) == 1) {
                VGPIO_SetLInvert(i, 1);
            }
            else {
                VGPIO_SetLInvert(i, 0);
            }
        }
    }  
}


UINT8 VGPIO_GetInvert ( UINT32 PinNum )
{
    return ( UINT8 ) GET_REG_BIT2 ( REG_GPIO_GDINVEN, PinNum );
    //UINT32 addr_offset = 0;
    //if(PinNum >= 32) {
    //  PinNum -= 32;
    //  addr_offset = 0x40;
    //}
    //return (UINT8)GET_REG_BIT2(REG_GPIO_GDINVEN + addr_offset, PinNum);
}

UINT8 VGPIO_GetLInvert ( UINT32 PinNum )
{
    return ( UINT8 ) GET_REG_BIT2 ( REG_GPIO_GLDINVEN, PinNum );
}


/******************************************************************************
Description:
    Sets the bit of UM_EN to enable or disable Umonitor.

Parameters:
    Pins - which pins to enable as UMONITOR signals output, can be any combination of
           valid GPIO pin definitions by performing bit-wise OR operation.

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetUmonitorEn ( UINT32 Pins )
{
    WRITE_REG ( REG_GPIO_UMEN, Pins );
}

UINT32 VGPIO_GetUmonitorEn (void )
{
    return READ_REG (REG_GPIO_UMEN);
}

/******************************************************************************
Description:
    Configs Umonitor to select specified output signals.

Parameters:
    Module - specifies the module, can be one of the following constants:
             GPIO_UMSEL_BB, GPIO_UMSEL_CODEC, GPIO_UMSEL_AIF, GPIO_UMSEL_TMR,
             GPIO_UMSEL_I2C, GPIO_UMSEL_UART, GPIO_UMSEL_CKM.
    Group - specifies the signals group of selected module

Return Value:
    None.
*****************************************************************************/
void VGPIO_ConfigUmonitor ( UINT8 Module, UINT8 sig )
{
    UINT32 reg_value;

    /* set the module to UMSEL, and Group to UMGRP */
    reg_value = ( ( UINT32 ) ( Module & GPIO_UMMODSEL_MASK ) << GPIO_UMMODSEL_SHFT ) \
                | ( ( UINT32 ) ( sig & GPIO_UMSIGSEL_MASK ) << GPIO_UMSIGSEL_SHFT );
    WRITE_REG ( REG_GPIO_UMCTRL, reg_value );
}

/******************************************************************************
Description:
    Sets the alternate functions of GPIO.

Parameters:
    Func - the alternate function to set, can be one of the following constants:
           GPIO_MUX_FUNC_NORMAL, GPIO_MUX_FUNC_UART.

Return Value:
    None.
*****************************************************************************/
void VGPIO_SetMuxFunction ( UINT32 mask, UINT32 Func )
{
    UINT32 reg_val;
    reg_val = ( READ_REG ( REG_GPIO_GDMUX1 ) & ( ~mask ) ) | Func;
    WRITE_REG ( REG_GPIO_GDMUX1, reg_val );
    return;
}

void VGPIO_SetMux2Function ( UINT32 mask, UINT32 Func )
{
    UINT32 reg_val;
    reg_val = ( READ_REG ( REG_GPIO_GDMUX2 ) & ( ~mask ) ) | Func;
    WRITE_REG ( REG_GPIO_GDMUX2, reg_val );
    return;
}

void VGPIO_SetLMuxFunction ( UINT32 mask, UINT32 Func )
{
    UINT32 reg_val;
    reg_val = ( READ_REG ( REG_GPIO_GLDMUX1 ) & ( ~mask ) ) | Func;
    WRITE_REG ( REG_GPIO_GLDMUX1, reg_val );
    return;
}

void VGPIO_SetLMux2Function ( UINT32 mask, UINT32 Func )
{
    UINT32 reg_val;
    reg_val = READ_REG ( REG_GPIO_GLDMUX2 );
	
	if ((reg_val&mask) != (Func&mask)){
		reg_val = (reg_val & ( ~mask ) ) | Func;
		WRITE_REG ( REG_GPIO_GLDMUX2, reg_val );
	}
    return;
}

/*
  Enable JLINK port, otherwise, JLINK will not be recognized.
  in common target_debug is enable, target_release is disable.
*/
void VGPIO_EnableDebugPort ( void )
{
	VGPIO_SetLMuxFunction(GPIO_L_PIN_11, GPIO_L_PIN_11);
	VGPIO_SetLMux2Function(GPIO_L_PIN_11, GPIO_L_PIN_11);     
	VGPIO_SetLMuxFunction(GPIO_L_PIN_10, GPIO_L_PIN_10);
	VGPIO_SetLMux2Function(GPIO_L_PIN_10, GPIO_L_PIN_10);
}

// is come from rom boot loader
UINT32 VGPIO_GetBootSel ( void )
{
    return 0x01 & ( READ_REG ( REG_GPIO_GLDIN ) >> 31 );
}

/******************************************************************************
Description:
 	Set GPIO GDOUT to high  through bitband.

Parameters:
 	Pins	-	set gpio output to high, can be any combination of 
 	       valid GPIO pin definitions by performing bit-wise OR operation.
	
Return Value:
	None.
*****************************************************************************/
void BB_GPIO_GDOUT_SET(UINT32 Pins)
{
	UINT8 i = 0;
	while (Pins) {
		if(Pins & 0x1)
			WRITE_REG(BB_GPIO_BASE+i*4,0x1);
		Pins >>= 1;
		i++;
	};
}
/******************************************************************************
Description:
 	Set GPIO GDOUT to low  through bitband.

Parameters:
 	Pins	-	set gpio output to low, can be any combination of 
 	       valid GPIO pin definitions by performing bit-wise OR operation.
	
Return Value:
	None.
*****************************************************************************/
void BB_GPIO_GDOUT_CLR(UINT32 Pins)
{
	UINT8 i = 0;
	while (Pins) {
		if(Pins & 0x1)
			WRITE_REG(BB_GPIO_BASE+i*4,0x0);
		Pins >>= 1;
		i++;
	};
}
/******************************************************************************
Description:
 	Reads the value present of the specified pin in GPIO port through bitband.
Parameters:
 	PinNum - the numeric value of GPIO pin,
Return Value:
	Returns input data value of the specified GPIO pin.
*****************************************************************************/
UINT8 GET_BB_GPIO_GDOUT(UINT8 PinNum)
{
	return (READ_REG(BB_GPIO_BASE + PinNum*4));
}

/******************************************************************************
Description:
 	Set GPIO GDOUT value through bitband.

Parameters:
 	PinNum - the numeric value of GPIO pin, e.g. 0 for GPIO0, 
 	         37 for GPIO37.
 	BitVal - the value to set to the specified pin.
	
Return Value:
	None.
*****************************************************************************/
void SET_BB_GPIO_GDOUT(UINT8 PinNum,UINT8 val)
{
	WRITE_REG(BB_GPIO_BASE+PinNum*4,val);
}
/******************************************************************************
Description:
 	Set GPIOL GDOUT to high  through bitband.

Parameters:
 	Pins	-	set gpio output to high, can be any combination of 
 	       valid GPIO pin definitions by performing bit-wise OR operation.
	
Return Value:
	None.
*****************************************************************************/
void BB_GPIOL_GDOUT_SET(UINT32 Pins)
{
	UINT8 i = 0;
	while (Pins) {
		if(Pins & 0x1)
			WRITE_REG(BB_GPIOL_BASE+i*4,0x1);
		Pins >>= 1;
		i++;
	};
}
/******************************************************************************
Description:
 	Set GPIOL GDOUT to low  through bitband.

Parameters:
 	Pins	-	set gpio output to low, can be any combination of 
 	       valid GPIO pin definitions by performing bit-wise OR operation.
	
Return Value:
	None.
*****************************************************************************/
void BB_GPIOL_GDOUT_CLR(UINT32 Pins)
{
	UINT8 i = 0;
	while (Pins) {
		if(Pins & 0x1)
			WRITE_REG(BB_GPIOL_BASE+i*4,0x0);
		Pins >>= 1;
		i++;
	};
}
/******************************************************************************
Description:
 	Reads the value present of the specified pin in GPIOL port through bitband.
Parameters:
 	PinNum - the numeric value of GPIOL pin,
Return Value:
	Returns input data value of the specified GPIOL pin.
*****************************************************************************/
UINT8 GET_BB_GPIOL_GDOUT(UINT8 PinNum)
{
	return (READ_REG(BB_GPIOL_BASE + PinNum*4));
}
/******************************************************************************
Description:
 	Set GPIOL GDOUT value through bitband.

Parameters:
 	PinNum - the numeric value of GPIO pin, e.g. 0 for GPIOL0, 
 	         16 for GPIOL16.
 	BitVal - the value to set to the specified pin.
	
Return Value:
	None.
*****************************************************************************/
void SET_BB_GPIOL_GDOUT(UINT8 PinNum,UINT8 val)
{
	WRITE_REG(BB_GPIOL_BASE+PinNum*4,val);
}

