/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    main

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vpla_ckm.h"
#include "vpla_uart.h"
#include "vdrv_timer.h"
#include "vdrv_gpio.h"
#include "vdrv_uart.h"
#include "vreg_def.h"

__IRQ void VISR_Uart(void)
{
	extern void VUART_Interrupt_Routine_Service(void);
	VUART_Interrupt_Routine_Service();
}

#define UART_RX_BUF_SIZE    1024    //2k=2048, 1.5K=1536
char uart_buf[UART_RX_BUF_SIZE];        // old=5k
volatile int uart_recv_len = 0;

static int uart_stream_read ( char *buf, int size)
{
    char c;
    int res;
    int i;

    for ( i = 0; i < size; i++ ) {
        if ( tstc () ) {
            VUART_CharRx ( ( UINT8 * ) &c );
            res = 1;
        } else {
            res = 0;
        }

        if ( res ) {
            buf[i] = c;
        } else {
            return i;
        }
    }

    return i;
}

void VUART_Interrupt_Routine_Service()
{
    UINT32 brxfifoInt = 0;
    int uart_rxlen = 0, rlen = 1024;
 
	
	brxfifoInt = VUART_GetFifoAndLineStatus()&(BIT_MASK(RBIT_UART_UFLSR_RX)|BIT_MASK(RBIT_UART_UFLSR_RFTO));

    if ( brxfifoInt && (uart_recv_len < UART_RX_BUF_SIZE - 1)) {
		rlen = UART_RX_BUF_SIZE - uart_recv_len - 1;
		uart_rxlen = uart_stream_read ( uart_buf + uart_recv_len, rlen);
		uart_recv_len += uart_rxlen;
		if ( uart_recv_len >= UART_RX_BUF_SIZE - 1 ) {
			VUART_DisableInt();
		}
    }
    if ( brxfifoInt ) {
        VUART_ClearInt(BIT_MASK(RBIT_UART_UFLSR_RX));
    }
    if ( VUART_GetFifoAndLineStatus()&BIT_MASK(RBIT_UART_UFLSR_ERR)) {
        VUART_ClearInt(BIT_MASK(RBIT_UART_UFLSR_OE) | BIT_MASK(RBIT_UART_UFLSR_PE) | BIT_MASK(RBIT_UART_UFLSR_FE) | BIT_MASK(RBIT_UART_UFLSR_BI));
    }
}

UINT32 uart_err = 0,uart_timerout = 0;
UINT32 VPORT_PutString ( UINT8 *p_Str, UINT32 Num)
{
    UINT32 i = 0, timer,ret;
    while ( i < Num ) {
        timer = 0;
        while ( VUART_GetTxCounter() == 32 && timer < 550000 ) {
            VCOMMON_Delay ( 100 );
            timer += 100;
        }
        if ( timer >= 550000 ) {
            uart_timerout = 0xFF;
            return VRET_ERROR;
        }
        ret = VUART_CharTx ( p_Str[i++] );
        if(ret != VRET_NO_ERROR) uart_err++;
    }
    return VRET_NO_ERROR;
}

void uart_test_task()
{

	int i;
	uint8_t len;
	static uint8_t buff[500], fifo_len_front[500], fifo_len_back[500];

	for(i = 0; i < sizeof(buff); i++)
	{
		buff[i] = i;
	}

	while (1) {
		for(i = 0; i < sizeof(buff); )
		{
			len = (UINT8)((READ_REG(REG_UART_UFCT) & 0x003F0000) >> 16);
			if( len < 32)
			{
				//fifo_len_front[i] = len;
				WRITE_REG ( REG_UART_UTDR, buff[i] );
				//fifo_len_back[i] = (UINT8)((READ_REG(REG_UART_UFCT) & 0x003F0000) >> 16);
			}
			else {
				continue;
			}

			i++;
		}
		vdelayus(200*1000);
	}
}

#define UART_SUPPORT 1
int main ( void )
{
    UINT32 i  = 0 , j = 0, k = 0;
	SINT8 str_buff[100] = "";

    VCPU_CoreInit(); 
    VCPU_INT_Enable();
    VSYS_CKM_Init();
	
	VDRVUartConfig_t uartconfig = {0};
	
	uartconfig.Baudrate = 921600;
	uartconfig.port_sel = VSYS_UART_PORT_SEL_PRIMARY;
	uartconfig.Parity = VSYS_UART_PARITY_NONE;
	uartconfig.StopBit = VSYS_UART_STOP_BIT1;
	uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
	uartconfig.mode = VSYS_UART_INT_MODE;
	uartconfig.RxFifolevel = 16;
	VSYS_UART_Init(&uartconfig);
	
	uart_test_task();

	
}
