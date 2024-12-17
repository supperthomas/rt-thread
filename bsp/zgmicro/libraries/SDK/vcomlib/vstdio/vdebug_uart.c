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
#if (VDBG_EN > 0)

#ifndef VDBG_UART_BAUDRATE
#if defined FPGA_QA_BUILD
#define VDBG_UART_BAUDRATE 512000
#else
#define VDBG_UART_BAUDRATE 921600
#endif
#endif

#ifndef VDBG_UART_PORT_SEL
#define VDBG_UART_PORT_SEL VSYS_UART_PORT_SEL_AUX
#endif

#if (VDBG_IO_RETARGET == IO_UART)

#include "vdrv_ckm.h"
#include "vdrv_gpio.h"
#include "vdrv_uart.h"
#include "vreg_def.h"



#define VDBG_UART_RX_FIFO_SIZE 128  //必须为2的整次幂

extern void VDBG_DrvInit(void);
static UINT32 VUART_INIT_Flag = 0;

#if (VDBG_UART_WORKMODE == 1)
UINT8 g_VDBG_UART_RX_Buffer[VDBG_UART_RX_FIFO_SIZE];
UINT16 g_VDBG_UART_Rx_Rd_Idx = 0;
UINT16 g_VDBG_UART_Rx_Wr_Idx = 0;

VOSSemap_t g_VDBG_UART_RX_Sem = NULL;

static inline  UINT16 VDBG_UART_Rx_GetCount(void)
{
    return (g_VDBG_UART_Rx_Wr_Idx - g_VDBG_UART_Rx_Rd_Idx) & (VDBG_UART_RX_FIFO_SIZE - 1);
}

static inline int VDBG_UART_Rx_GetBuf(UINT8 *buf, UINT16 len)
{
    UINT16 i;
    for (i = 0; i < len; i++)
    {
        if (VDBG_UART_Rx_GetCount() == 0)
        {
            break;
        }

        buf[i] = g_VDBG_UART_RX_Buffer[g_VDBG_UART_Rx_Rd_Idx&(VDBG_UART_RX_FIFO_SIZE - 1)];
        g_VDBG_UART_Rx_Rd_Idx++;
    }
    return i;
}

static inline int VDBG_UART_Rx_PutBuf(UINT8 *buf, UINT16 len)
{
    UINT16 i;
    for (i = 0; i < len; i++)
    {
        if (VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1))
        {
            break;
        }
        g_VDBG_UART_RX_Buffer[g_VDBG_UART_Rx_Wr_Idx&(VDBG_UART_RX_FIFO_SIZE - 1)] = buf[i];
        g_VDBG_UART_Rx_Wr_Idx++;
    }

    return i;
}


void VDBG_UART_Rx_ISR(void)
{
	UINT8 count = VSYS_UART_GetStatus(VSYS_UART_ValidRxCount_Status);
    UINT8 tmp;
    // v_printf("uart rx c=%d\r\n",count);
    while (count > 0) {
        VUART_CharRx(&(tmp));
        VDBG_UART_Rx_PutBuf(&tmp, 1);
        count--;
    }
    
    if(g_VDBG_UART_RX_Sem != NULL) //防止OS 调度器未启动时执行信号量操作
    {
        VOSSemapGive(g_VDBG_UART_RX_Sem);
    }
}

void VDBG_Uart_ISR(void)
{
    UINT32 status;

    status = VSYS_UART_GetStatus(VSYS_UART_Interrupt_Status);

    if (status & VSYS_UART_RX_INT) /* Uart rxFifo reach Trigger Level or rxfifo timeout*/
    {
        VDBG_UART_Rx_ISR();
    }

    if (status & VSYS_UART_TX_INT) /* Uart txFifo empty */
    {
        //VDBG_UART_Tx_ISR();
    }

	/* Uart Error Interrupt handler */
	{
		if(status & VSYS_UART_CTS_INT) {
			UINT8 cts;
			cts = VSYS_UART_GetStatus(VSYS_UART_CTS_Status);	
		} else if (status & VSYS_UART_ERR_INT) {
			//  OE should not be triggered, so I think no need to process it
		}
	}

    VSYS_UART_ClearInt();
}
#endif


/* stdio retarget to UART */
int vfputc(int ch)
{
    VDBG_DrvInit();
    VUART_CharTxWithBlocking((UINT8)ch);
    return (ch);
}


int vfgetc(void)
{
    int ch = 0;
    VDBG_DrvInit();
#if (VDBG_UART_WORKMODE == 1)
    uint32_t get_len;
    do 
    {
        get_len = VDBG_UART_Rx_GetBuf((UINT8 *)&ch, 1);
        if((get_len != 1) && (g_VDBG_UART_RX_Sem))
        {
            VOSSemapTake(g_VDBG_UART_RX_Sem, VOSMAX_DELAY);     //wait for data
        }
    }while(get_len != 1);
#else
    while (VUART_CharRxWithBlocking((UINT8 *)&ch));
#endif
    return (ch);
}


void vputs(char *str)
{
    VDBG_DrvInit();

    while (*str != '\0')
    {
        VUART_CharTxWithBlocking((UINT8)*str++);
    }
}
#if 0 //THOMAS
void _ttywrch(int ch) 
{
    VUART_CharTxWithBlocking((UINT8)ch);
}
#endif

void VDBG_DrvInit(void)
{
    if (VUART_INIT_Flag != VCKM_GetUartClockFreq())
    {

#if (VDBG_UART_WORKMODE == 1)
        g_VDBG_UART_RX_Sem = VOSSemapCreate("VDBG_UART_RX_Sem", 0);
#endif

		VDRVUartConfig_t uartconfig = {0};
		uartconfig.Baudrate = VDBG_UART_BAUDRATE;
		uartconfig.port_sel = VDBG_UART_PORT_SEL;
		uartconfig.Parity = VSYS_UART_PARITY_NONE;
		uartconfig.StopBit = VSYS_UART_STOP_BIT1;
		uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
#if (VDBG_UART_WORKMODE == 1)
		uartconfig.mode = VSYS_UART_INT_MODE;
		uartconfig.TxFifolevel = 0;
        uartconfig.RxFifolevel = 20;
#else
		uartconfig.mode = VSYS_UART_FIFO_MODE;
#endif
		VSYS_UART_Init(&uartconfig);
        VUART_INIT_Flag = VCKM_GetUartClockFreq();
		vfputc('\0');
    }
}

#if 0

// 测试函数
void test_VDBG_UART_Rx_PutBuf_GetBuf(void)
{
    static UINT8 test_buf_put[200];
    static UINT8 test_buf_get[200];
    int result;
    
    // 初始化测试数据
    for (int i = 0; i < 200; i++) {
        test_buf_put[i] = i;
    }

    // 测试空缓冲区读取
    result = VDBG_UART_Rx_GetBuf(test_buf_get, 100);
    ASSERT(result == 0);

    // 测试正常放入数据
    result = VDBG_UART_Rx_PutBuf(test_buf_put, 50);
    ASSERT(result == 50);
    ASSERT(VDBG_UART_Rx_GetCount() == 50);

    // 测试从缓冲区中读取数据
    result = VDBG_UART_Rx_GetBuf(test_buf_get, 50);
    ASSERT(result == 50);
    ASSERT(VDBG_UART_Rx_GetCount() == 0);
    for (int i = 0; i < 50; i++) {
        ASSERT(test_buf_get[i] == i);
    }

    // 测试缓冲区满
    result = VDBG_UART_Rx_PutBuf(test_buf_put, VDBG_UART_RX_FIFO_SIZE);
    ASSERT(result == (VDBG_UART_RX_FIFO_SIZE - 1));
    ASSERT(VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1));

    // 测试缓冲区满时继续放入数据
    result = VDBG_UART_Rx_PutBuf(test_buf_put, 10);
    ASSERT(result == 0);
    ASSERT(VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1));

    // 测试从满缓冲区读取部分数据
    result = VDBG_UART_Rx_GetBuf(test_buf_get, 100);
    ASSERT(result == 100);
    ASSERT(VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1) - 100);

    // 测试循环缓冲区
    result = VDBG_UART_Rx_PutBuf(test_buf_put, 100);
    ASSERT(result == 100);
    ASSERT(VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1));

    // 清空缓冲区
    g_VDBG_UART_Rx_Rd_Idx = 0;
    g_VDBG_UART_Rx_Wr_Idx = 0;

    // 测试循环边界条件
    for (int i = 0; i < (VDBG_UART_RX_FIFO_SIZE - 1); i++) {
        result = VDBG_UART_Rx_PutBuf(&test_buf_put[i], 1);
        ASSERT(result == 1);
    }
    ASSERT(VDBG_UART_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1));

    for (int i = 0; i < (VDBG_UART_RX_FIFO_SIZE - 1); i++) {
        result = VDBG_UART_Rx_GetBuf(test_buf_get, 1);
        ASSERT(result == 1);
//        TOP_DEBUG("i = %d,test_buf_get=%d\r\n", i, test_buf_get[0]);
        ASSERT(test_buf_get[0] == i);
    }
    ASSERT(VDBG_UART_Rx_GetCount() == 0);

    //大量数据写入读取测试
    for (int i = 0; i < 1000; i++) {
        result = VDBG_UART_Rx_PutBuf(test_buf_put, 100);
        ASSERT(result == 100);
        result = VDBG_UART_Rx_GetBuf(test_buf_get, 100);
        ASSERT(result == 100);
        for (int j = 0; j < 100; j++) {
            ASSERT(test_buf_get[j] == j);
        }
    }
}




#endif



#endif /* VDBG_IO_RETARGET == IO_UART */
#endif /* VDBG_EN */
