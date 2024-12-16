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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VGPIO_H_
#define _VGPIO_H_

#include "vstdlib.h"
#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_OUT_ADDR_OPERATION 	0x00
#define GPIO_OUT_BIT_OPERATION 		0x01
#define GPIO_BIT_MODE  						GPIO_OUT_BIT_OPERATION
	
#define GPIO_PIN_0                 ((UINT16)0x0001)  /* PIN 0 selected */
#define GPIO_PIN_1                 ((UINT16)0x0002)  /* PIN 1 selected */
#define GPIO_PIN_2                 ((UINT16)0x0004)  /* PIN 2 selected */
#define GPIO_PIN_3                 ((UINT16)0x0008)  /* PIN 3 selected */
#define GPIO_PIN_4                 ((UINT16)0x0010)  /* PIN 4 selected */
#define GPIO_PIN_5                 ((UINT16)0x0020)  /* PIN 5 selected */
#define GPIO_PIN_6                 ((UINT16)0x0040)  /* PIN 6 selected */
#define GPIO_PIN_7                 ((UINT16)0x0080)  /* PIN 7 selected */
#define GPIO_PIN_8                 ((UINT16)0x0100)  /* PIN 8 selected */
#define GPIO_PIN_9                 ((UINT16)0x0200)  /* PIN 9 selected */
#define GPIO_PIN_10                ((UINT16)0x0400)  /* PIN 10 selected */
#define GPIO_PIN_11                ((UINT16)0x0800)  /* PIN 11 selected */
#define GPIO_PIN_12                ((UINT16)0x1000)  /* PIN 12 selected */
#define GPIO_PIN_13                ((UINT32)(1<<13))     /* PIN 13 selected */
#define GPIO_PIN_14                ((UINT32)(1<<14))     /* PIN 14 selected */
#define GPIO_PIN_15                ((UINT32)(1<<15))     /* PIN 14 selected */
#define GPIO_PIN_16                ((UINT32)(1<<16))     /* PIN 14 selected */
#define GPIO_PIN_17                ((UINT32)(1<<17))     /* PIN 14 selected */
#define GPIO_PIN_18                ((UINT32)(1<<18))     /* PIN 14 selected */
#define GPIO_PIN_19                ((UINT32)(1<<19))     /* PIN 14 selected */
#define GPIO_PIN_20                ((UINT32)(1<<20))     /* PIN 14 selected */
#define GPIO_PIN_21                ((UINT32)(1<<21))     /* PIN 14 selected */
#define GPIO_PIN_22                ((UINT32)(1<<22))     /* PIN 14 selected */

#define GPIO_PIN_ALL               ((UINT32)0xFFFFFFFF)  /* All pins selected */

#define GPIO_L_PIN_0                 ((UINT16)0x0001)  /* PIN 0 selected */
#define GPIO_L_PIN_1                 ((UINT16)0x0002)  /* PIN 1 selected */
#define GPIO_L_PIN_2                 ((UINT16)0x0004)  /* PIN 2 selected */
#define GPIO_L_PIN_3                 ((UINT16)0x0008)  /* PIN 3 selected */
#define GPIO_L_PIN_4                 ((UINT16)0x0010)  /* PIN 4 selected */
#define GPIO_L_PIN_5                 ((UINT16)0x0020)  /* PIN 5 selected */
#define GPIO_L_PIN_6                 ((UINT16)0x0040)  /* PIN 6 selected */
#define GPIO_L_PIN_7                 ((UINT16)0x0080)  /* PIN 7 selected */
#define GPIO_L_PIN_8                 ((UINT16)0x0100)  /* PIN 8 selected */
#define GPIO_L_PIN_9                 ((UINT16)0x0200)  /* PIN 9 selected */
#define GPIO_L_PIN_10                ((UINT16)0x0400)  /* PIN 10 selected */	
#define GPIO_L_PIN_11				   ((UINT16)0x0800)	 /* PIN 11 selected */
#define GPIO_L_PIN_12				   ((UINT16)0x1000)	 /* PIN 12 selected */
#define GPIO_L_PIN_13				   ((UINT32)(1<<13))	 /* PIN 13 selected */
#define GPIO_L_PIN_14				   ((UINT32)(1<<14))	 /* PIN 14 selected */
#define GPIO_L_PIN_15				   ((UINT32)(1<<15))	 /* PIN 15 selected */
#define GPIO_L_PIN_16				   ((UINT32)(1<<16))	 /* PIN 16 selected */

#define GPIO_L_PIN_ALL                 (0x0001FFFF)

#define GPIO_MODE_OUTPUT            0
#define GPIO_MODE_INPUT             1
#define GPIO_MODE_Z                 2

#define GPIO_INPUT_NORMAL           0x0
#define GPIO_INPUT_INVERT           0x1
#define GPIO_INPUT_DISPULL          0x2

#define GPIO_OUTPUT_LOW_STRENGTH    0
#define GPIO_OUTPUT_HIGH_STRENGTH   1

#define GPIO_Z_NORMAL               0

#define GPIO_INTTYPE_HIGH_LEVEL     0
#define GPIO_INTTYPE_LOW_LEVEL  1


#define GPIO_PWRON_SHFT                     (0)
#define GPIO_PWRON_PIN                     (1<<GPIO_PWRON_SHFT)

#define GPIO_SPI_CSN_SHFT                32

#define GPIO_UMEN_MASK                  ((UINT32)0xFFFFFFFE) // PIN 31:1
#define GPIO_UMEN_SHFT                  1

#define GPIO_UMMODSEL_MASK              ((UINT32)0x1F)
#define GPIO_UMMODSEL_SHFT              8

#define GPIO_UMSIGSEL_MASK              ((UINT32)0x3F)
#define GPIO_UMSIGSEL_SHFT              0

#define GPIO_UMSEL_TOP                  0
#define GPIO_UMSEL_CODEC                1
#define GPIO_UMSEL_USB                  2
#define GPIO_UMSEL_TMR                  3
#define GPIO_UMSEL_I2C                  4
#define GPIO_UMSEL_UART                 5
#define GPIO_UMSEL_DSP                  6
#define GPIO_UMSEL_MODEM                7
#define GPIO_UMSEL_RFC                  8
#define GPIO_UMSEL_RFFE                 9
#define GPIO_UMSEL_BB                   10
#define GPIO_UMSEL_DMA                  11
#define GPIO_UMSEL_CM3                  12
#define GPIO_UMSEL_SPI                  13
#define GPIO_UMSEL_BBLE                 14
#define GPIO_UMSEL_CACHE                15
#define GPIO_UMSEL_SD                   16
#define GPIO_UMSEL_LCDC                 17
#define GPIO_UMSEL_CKM                  18
#define GPIO_UMSEL_HDCPORT              19

#define GPIO_MUX_FUNC_NONE          ((UINT32)0x00000000)

#define GPIO_MUX_FUNC_UART_TX       (GPIO_PIN_1)
#define GPIO_MUX_FUNC_UART_RTS      (GPIO_PIN_2)
#define GPIO_MUX_FUNC_UART_RX       (GPIO_PIN_3)
#define GPIO_MUX_FUNC_UART_CTS      (GPIO_PIN_4)
#define GPIO_MUX_FUNC_UART_TX_AUX   (GPIO_PIN_13)
#define GPIO_MUX_FUNC_UART_RX_AUX   (GPIO_PIN_14)

#define GPIO_MUX_FUNC_UART          (GPIO_MUX_FUNC_UART_TX | GPIO_MUX_FUNC_UART_RX )
#define GPIO_MUX_FUNC_UART_TRX      (GPIO_MUX_FUNC_UART_TX | GPIO_MUX_FUNC_UART_RX)
#define GPIO_MUX_FUNC_UART_TRX_FLOW (GPIO_MUX_FUNC_UART_TX | GPIO_MUX_FUNC_UART_RX|GPIO_MUX_FUNC_UART_RTS|GPIO_MUX_FUNC_UART_CTS)
#define GPIO_MUX_FUNC_UART_TRX_AUX  (GPIO_MUX_FUNC_UART_TX_AUX | GPIO_MUX_FUNC_UART_RX_AUX)

#define GPIO_MUX_FUNC_SW_CLK        (GPIO_PIN_5)
#define GPIO_MUX_FUNC_SW_DIO        (GPIO_PIN_6)
#define GPIO_MUX_FUNC_SW        (GPIO_MUX_FUNC_SW_CLK | GPIO_MUX_FUNC_SW_DIO)

#define GPIO_MUX_FUNC_I2C_SCL       (GPIO_PIN_7)
#define GPIO_MUX_FUNC_I2C_SDA       (GPIO_PIN_8)
#define GPIO_MUX_FUNC_I2C        (GPIO_MUX_FUNC_I2C_SCL | GPIO_MUX_FUNC_I2C_SDA)



#define GPIO_MUX_FUNC_LED0      (GPIO_PIN_19)
#define GPIO_MUX_FUNC_LED1      (GPIO_PIN_20)
#define GPIO_MUX_FUNC_LED2      (GPIO_PIN_21)
#define GPIO_MUX_FUNC_LED3      (GPIO_PIN_22)
#define GPIO_MUX_FUNC_LED_ALL   (GPIO_MUX_FUNC_LED0 | GPIO_MUX_FUNC_LED1 | GPIO_MUX_FUNC_LED2 | GPIO_MUX_FUNC_LED3)

#define GPIO_MUX_FUNC_SD_LOCK 		(GPIO_L_PIN_16)
#define GPIO_MUX_FUNC_SD_DET 		(GPIO_L_PIN_2)
#define GPIO_MUX_FUNC_SD 	(GPIO_MUX_FUNC_SD_LOCK | GPIO_MUX_FUNC_SD_DET)

#define GPIO_MUX_FUNC_JTAG 		(0x00000F80)

#define GPIO_MUX_FUNC_USB_VBUS      (GPIO_L_PIN_3)

#define GPIO_MUX_FUNC_LCD_CSB 		(GPIO_L_PIN_4)
#define GPIO_MUX_FUNC_LCD_DO 		(GPIO_L_PIN_5)
#define GPIO_MUX_FUNC_LCD_A0 		(GPIO_L_PIN_6)
#define GPIO_MUX_FUNC_LCD_CLK 		(GPIO_L_PIN_7)
#define GPIO_MUX_FUNC_LCD_WRN 		(GPIO_L_PIN_8)
#define GPIO_MUX_FUNC_LCD_RDN 		(GPIO_L_PIN_9)
#define GPIO_MUX_FUNC_LCD_D2 		(GPIO_L_PIN_10)
#define GPIO_MUX_FUNC_LCD_D3 		(GPIO_L_PIN_11)
#define GPIO_MUX_FUNC_LCD_D4 		(GPIO_L_PIN_12)
#define GPIO_MUX_FUNC_LCD_D5 		(GPIO_L_PIN_13)
#define GPIO_MUX_FUNC_LCD_D6 		(GPIO_L_PIN_14)
#define GPIO_MUX_FUNC_LCD_D7 		(GPIO_L_PIN_15)
#define GPIO_MUX_FUNC_SPILCD 		(GPIO_MUX_FUNC_LCD_CSB | GPIO_MUX_FUNC_LCD_DO | GPIO_MUX_FUNC_LCD_A0 | GPIO_MUX_FUNC_LCD_CLK)
#define GPIO_MUX_FUNC_LCDALL 		(GPIO_MUX_FUNC_SPILCD | GPIO_MUX_FUNC_LCD_WRN | GPIO_MUX_FUNC_LCD_RDN | GPIO_MUX_FUNC_LCD_D2|\
                                        GPIO_MUX_FUNC_LCD_D3|GPIO_MUX_FUNC_LCD_D4|GPIO_MUX_FUNC_LCD_D5|GPIO_MUX_FUNC_LCD_D6|GPIO_MUX_FUNC_LCD_D7)
#define GPIO_MUX_FUNC_PARALCD 		(0x0FFF0000)

#define GPIO_MUX_FUNC_I2S0_SCLK 		(GPIO_PIN_15)
#define GPIO_MUX_FUNC_I2S0_LR 		(GPIO_PIN_16)
#define GPIO_MUX_FUNC_I2S0_DI 		(GPIO_PIN_17)
#define GPIO_MUX_FUNC_I2S0_DO 		(GPIO_PIN_18)

#ifndef GPIO_MUX_FUNC_I2S0
#define GPIO_MUX_FUNC_I2S0 		(GPIO_MUX_FUNC_I2S0_SCLK | GPIO_MUX_FUNC_I2S0_LR | GPIO_MUX_FUNC_I2S0_DI | GPIO_MUX_FUNC_I2S0_DO)
#endif

#define GPIO_MUX_FUNC_I2S1_SCLK 		(GPIO_PIN_19)
#define GPIO_MUX_FUNC_I2S1_LR 		(GPIO_PIN_20)
#define GPIO_MUX_FUNC_I2S1_DI 		(GPIO_PIN_21)
#define GPIO_MUX_FUNC_I2S1_DO 		(GPIO_PIN_22)

#ifndef GPIO_MUX_FUNC_I2S1
#define GPIO_MUX_FUNC_I2S1 		(GPIO_MUX_FUNC_I2S1_SCLK | GPIO_MUX_FUNC_I2S1_LR | GPIO_MUX_FUNC_I2S1_DI | GPIO_MUX_FUNC_I2S1_DO)
#endif

#define GPIO_MUX_FUNC_I2S1_4LED 		(GPIO_MUX_FUNC_I2S1_DO)

#define GPIO_MUX_FUNC_SPI_CSN       (GPIO_PIN_9) // These GPIOs shall muxed to SPI when an external flash is attatched.
#define GPIO_MUX_FUNC_SPI_DI        (GPIO_PIN_10)
#define GPIO_MUX_FUNC_SPI_DO        (GPIO_PIN_11)
#define GPIO_MUX_FUNC_SPI_SCK       (GPIO_PIN_12)
#define GPIO_MUX_FUNC_SPI_HOLD 		(GPIO_L_PIN_0)
#define GPIO_MUX_FUNC_SPI_WP 		(GPIO_L_PIN_1)
#define GPIO_MUX_FUNC_SPI 		(GPIO_MUX_FUNC_SPI_CSN | GPIO_MUX_FUNC_SPI_SCK | GPIO_MUX_FUNC_SPI_DI | GPIO_MUX_FUNC_SPI_DO)
#define GPIO_MUX_FUNC_SPI_L 	(GPIO_MUX_FUNC_SPI_WP | GPIO_MUX_FUNC_SPI_HOLD)

#define GPIO_MUX_FUNC_BT_SW_CLK			(GPIO_L_PIN_10)
#define GPIO_MUX_FUNC_BT_SW_DIO			(GPIO_L_PIN_11)
#define GPIO_MUX_FUNC_BT_SW             (GPIO_MUX_FUNC_BT_SW_CLK | GPIO_MUX_FUNC_BT_SW_DIO)

#define GPIO_MUX_FUNC_BT_TRACECLK			(GPIO_L_PIN_0)
#define GPIO_MUX_FUNC_BT_SWO					(GPIO_L_PIN_1)
#define GPIO_MUX_FUNC_BT_TRACEDATA0		(GPIO_PIN_9)
#define GPIO_MUX_FUNC_BT_TRACEDATA1		(GPIO_PIN_10)
#define GPIO_MUX_FUNC_BT_TRACEDATA2		(GPIO_PIN_11)
#define GPIO_MUX_FUNC_BT_TRACEDATA3		(GPIO_PIN_12)
#define GPIO_MUX_FUNC_BT_TRACEDATDACLK	(GPIO_MUX_FUNC_BT_TRACECLK | GPIO_MUX_FUNC_BT_SWO)
#define GPIO_MUX_FUNC_BT_TRACEDATA			(GPIO_MUX_FUNC_BT_TRACEDATA0 | GPIO_MUX_FUNC_BT_TRACEDATA1 \
																				| GPIO_MUX_FUNC_BT_TRACEDATA2 | GPIO_MUX_FUNC_BT_TRACEDATA3)

typedef struct VGPIO_UMONITOR_CONFIG_TAG {
    UINT32      pins;
    UINT8       module;
    UINT8       sig;
} VGPIO_UMONITOR_CONFIG;

void VGPIO_SetMode ( UINT32 Pins, UINT8 Mode, UINT8 Param );
void VGPIO_SetLMode ( UINT32 Pins, UINT8 Mode, UINT8 Param );

UINT32 VGPIO_GetInput ( void/*UINT32 addr*/ );
UINT32 VGPIO_GetLInput ( void/*UINT32 addr*/ );

UINT8 VGPIO_GetPinInput ( UINT8 PinNum );
UINT8 VGPIO_GetLPinInput ( UINT8 PinNum );

UINT32 VGPIO_GetOutput (void);
UINT32 VGPIOL_GetOutput ( void);

UINT32 VGPIO_GetOutEn ( void);
UINT32 VGPIOL_GetOutEn ( void);

void VGPIO_SetPinOutput ( UINT8 PinNum, UINT8 BitVal );
void VGPIO_SetLPinOutput ( UINT8 PinNum, UINT8 BitVal );

void VGPIO_SetPinsOutput ( UINT32 Pins, UINT8 BitVal );
void VGPIO_SetLPinsOutput ( UINT32 Pins, UINT8 BitVal );
void VGPIO_SetPinsOutput_OD (UINT32 Pins, UINT8 BitVal );
void VGPIO_SetLPinsOutput_OD (UINT32 Pins, UINT8 BitVal );

void VGPIO_ChangePinOutput ( UINT32 io_mask );
void VGPIO_ChangeLPinOutput ( UINT32 io_mask );
void VGPIO_EnableInt ( UINT32 Pins );
void VGPIO_L_EnableInt ( UINT32 Pins );

void VGPIO_DisableInt ( UINT32 Pins );
void VGPIO_L_DisableInt ( UINT32 Pins );

void VGPIO_SetIntType ( UINT32 Pins, UINT8 Type );
void VGPIO_SetLIntType ( UINT32 Pins, UINT8 Type );

void VGPIO_SetInvert ( UINT8 PinNum, UINT8 BitVal );
void VGPIO_SetLInvert ( UINT8 PinNum, UINT8 BitVal );
void GPIO_Set_Invert(UINT32 Pins);
void GPIO_Set_L_Invert(UINT32 Pins);

UINT8 VGPIO_GetInvert ( UINT32 PinNum );
UINT8 VGPIO_GetLInvert ( UINT32 PinNum );

void VGPIO_SetMuxFunction ( UINT32 mask, UINT32 Func );
void VGPIO_SetMux2Function ( UINT32 mask, UINT32 Func );
void VGPIO_SetLMuxFunction ( UINT32 mask, UINT32 Func );
void VGPIO_SetLMux2Function ( UINT32 mask, UINT32 Func );
void VGPIO_SetUmonitorEn ( UINT32 Pins );
UINT32 VGPIO_GetUmonitorEn (void);
void VGPIO_ConfigUmonitor ( UINT8 Module, UINT8 sig );
void VGPIO_EnableDebugPort ( void );

UINT32 VGPIO_GetBootSel ( void );

void BB_GPIO_GDOUT_SET(UINT32 Pins);
void BB_GPIO_GDOUT_CLR(UINT32 Pins);
void SET_BB_GPIO_GDOUT(UINT8 PinNum,UINT8 val);
UINT8 GET_BB_GPIO_GDOUT(UINT8 PinNum);

void BB_GPIOL_GDOUT_SET(UINT32 Pins);
void BB_GPIOL_GDOUT_CLR(UINT32 Pins);
void SET_BB_GPIOL_GDOUT(UINT8 PinNum,UINT8 val);
UINT8 GET_BB_GPIOL_GDOUT(UINT8 PinNum);

#define _GPIO_OUT_HIGH(gpionum) VGPIO_SetPinOutput(gpionum, 1)
#define _GPIO_OUT_LOW(gpionum)  VGPIO_SetPinOutput(gpionum, 0)
#define _GPIO_OUT_Init(gpionum) VGPIO_SetMode(1UL<<gpionum, GPIO_MODE_OUTPUT, 1)



#ifdef __cplusplus
}
#endif

#endif  /* _VGPIO_H_ */

