/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <board.h>
#include <rtthread.h>
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vdrv_uart.h"
#include "vdebug.h"
#include "vstdio.h"
#include "vdrv_ckm.h"
#include "vpla_uart.h"


static UINT32 VUART_INIT_Flag = 0;

void rt_hw_console_init(void)
{
    if (VUART_INIT_Flag != VCKM_GetUartClockFreq())
    {
#define VDBG_UART_PORT_SEL VSYS_UART_PORT_SEL_AUX
				VDRVUartConfig_t uartconfig = {0};
				uartconfig.Baudrate = VDBG_UART_BAUDRATE;
				uartconfig.port_sel = VDBG_UART_PORT_SEL;
				uartconfig.Parity = VSYS_UART_PARITY_NONE;
				uartconfig.StopBit = VSYS_UART_STOP_BIT1;
				uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
				uartconfig.mode = VSYS_UART_FIFO_MODE;
				uartconfig.TxFifolevel = 0;
				uartconfig.RxFifolevel = 20;
				VSYS_UART_Init(&uartconfig);
				VUART_INIT_Flag = VCKM_GetUartClockFreq();
    }
}

void rt_hw_console_output(const char *str)
{
   // vputs((char *)str);
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            vdirect_vputc((int)a);
        }
        vdirect_vputc(*(uint8_t *)(str + i));
    }
	
}

char rt_hw_console_getchar(void)
{
    int ch = -1;
	  if(VUART_CharRx((UINT8 *)&ch) != VRET_NO_ERROR)
		{
        ch = -1;
        rt_thread_mdelay(10);
		}
		else
		{
			//vdirect_vputc(ch);
		}
		
    return ch;
}
