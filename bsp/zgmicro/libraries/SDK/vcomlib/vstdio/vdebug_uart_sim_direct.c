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
        DEBUG

Description:
        Debug relative function definitions.

Revision History:
        Date                  Author                   Description
        2009.04.15           chenjingen              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vstdlib.h"
#include "vstdcore.h"
#include "vpla_uart.h"
#include "vstdos.h"
#include "vtypes.h"
#if (VDBG_EN > 0)

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  1000000
#endif

#if (VDBG_DIRECT_TO_UART_SIM)

#include "vdrv_ckm.h"
#include "vdrv_gpio.h"
#include "vreg_def.h"
#include "vpla_gpio.h"

#ifndef UART_SIM_TX_PIN
#define UART_SIM_TX_PIN 1
#endif

#if (UART_SIM_TX_PIN >= 32)
#define UART_TX_PIN (UART_SIM_TX_PIN >> 32)
#define GPIO_SET_BIT() WRITE_REG(BB_GPIOL_BASE+(UART_TX_PIN)*4,0x1);
#define GPIO_CLR_BIT() WRITE_REG(BB_GPIOL_BASE+(UART_TX_PIN)*4,0x0);
#else
#define UART_TX_PIN UART_SIM_TX_PIN
#define GPIO_SET_BIT() WRITE_REG(BB_GPIO_BASE+(UART_TX_PIN)*4,0x1);
#define GPIO_CLR_BIT() WRITE_REG(BB_GPIO_BASE+(UART_TX_PIN)*4,0x0);
#endif

#if (CONFIG_SOC == SOC_WS9652)
#define COMMON_CNT  16
#define WHILE_DELAY 6
#elif (CONFIG_SOC == SOC_WS9648)
#define COMMON_CNT 16
#define WHILE_DELAY 6
#elif (CONFIG_SOC == SOC_WS9654)
#define COMMON_CNT 16
#define WHILE_DELAY 6
#else
#error "SOC not defined"
#endif

static int bit_delay;

#pragma push
#pragma O2 //强制指定O2优化等级

__attribute__((section("RamCode"),noinline)) static void UART_SimSendChar(UINT8 ch)
{
    UINT16 bit10 = ch;
    UINT32 count = bit_delay;
    
    bit10 = (bit10 << 1) | 0x200;
    bit10 = bit10 & 0x3FE;
    VOS_CRITICAL_ENTER();
    for(int i = 0; i < 10; i++)
    {
        if(bit10 & 0x01)
        {
            GPIO_SET_BIT();
        }
        else
        {
            GPIO_CLR_BIT();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
        }

        while (--count);   //2条指令
        bit10 >>= 1;  //1条指令
        count = bit_delay;
    }
    GPIO_SET_BIT();
    VOS_CRITICAL_EXIT();
}

#pragma pop

void vdirect_uartinit(void)
{
#if (VDBG_IO_RETARGET != IO_UART_SIM)
    static UINT32 VUART_INIT_Flag = 0;
	if (VUART_INIT_Flag != VCKM_GetSysClockFreq())
    {
        UINT32 freq = VCKM_GetSysClockFreq();
        UINT32 bit_clk = (freq + VDBG_UART_BAUDRATE / 2) / VDBG_UART_BAUDRATE;
        
        bit_delay = (bit_clk - COMMON_CNT) / WHILE_DELAY;

        VSYS_GPIO_Init(1<<UART_SIM_TX_PIN, VSYS_GPIO_MODE_OUTPUT, VSYS_GPIO_OUTPUT_LOW_STRENGTH);

        GPIO_SET_BIT();
        VUART_INIT_Flag = VCKM_GetSysClockFreq();
    }
#endif
}


/* stdio retarget to UART */
int vdirect_vputc(int ch)
{
#if (VDBG_IO_RETARGET != IO_UART_SIM)
    vdirect_uartinit();
    UART_SimSendChar((UINT8)ch);
#else
    extern int vfputc(int ch);
    vfputc(ch);
#endif
    return (ch);
}

void vdirect_vputs(char *str)
{
#if (VDBG_IO_RETARGET != IO_UART_SIM)
    vdirect_uartinit();
    while (*str != '\0')
    {
        UART_SimSendChar((UINT8)*str++);
    }
#else
    extern void vputs(char *str);
    vputs(str);
#endif
}


#endif

#endif /* VDBG_EN */
