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

void rt_hw_console_init(void)
{
    VDBG_Init();
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
			vdirect_vputc(ch);
		}
		
    return ch;
}
