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
    2019.02.11           zhangcheng                 initial version
    2019.10.09           miao.mingming              TP+BT print
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"     
#include "vstdio.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_UART_RTT)

#include "SEGGER_RTT.h"
#include "vstdos.h"
#include "vpla_ckm.h"
#include "vpla_uart.h"
#include "vdrv_uart.h"
#include "vdrv_ipc.h" 

#define UartGetRttBuff_EN                   1       //cxt 2020.08.20  0 - Disable uart to print BT log , 1 - Enable uart to print BT log
#define LOG_BUFF_LEN                        128     //cxt 2021.07.02  uart send log cache size
#define LOG_TOP_BT_STYLE                    1       //cxt 2021.07.02  0-No style, 1-color style, 2-label style
#define TOP_LABEL                           "[TOP] "
#define BT_LABEL                            "[BT] "

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  921600
#endif

static VOSTcb_t  AppTopTaskUartPrintTCB = NULL;

void VUart_Print_Task(void *p_Arg);
int VDBG_IsOutputEnable(void);


#define VDBG_UART_RX_FIFO_SIZE 128  //必须为2的整次幂

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



void VDBG_DrvInit(void)
{
    static UINT32 VUART_INIT_Flag = 0;
	if (VUART_INIT_Flag != VSYS_CKM_GetParam(VSYS_UartClockFreq))
    {
        SEGGER_RTT_Init();
#if (RTT_HIGH_SPEED_SYNC_DBG_EN > 0)
        // set the buffer run in block mode
        SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#else
        // set the buffer run in nonblock mode
        SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif //#if (RTT_HIGH_SPEED_SYNC_DBG_EN >0)
        VIPC_SetRTTInfoValue_TOP(&_SEGGER_RTT) ;
#if (VDBG_UART_WORKMODE == 1)
        g_VDBG_UART_RX_Sem = VOSSemapCreate("VDBG_UART_RX_Sem", 0);
#endif

        VDRVUartConfig_t uart_config = {0};
        uart_config.Baudrate = VDBG_UART_BAUDRATE;
        uart_config.port_sel = VSYS_UART_PORT_SEL_AUX;
        uart_config.Parity = VSYS_UART_PARITY_NONE;
        uart_config.StopBit = VSYS_UART_STOP_BIT1;
        uart_config.BitWidth = VSYS_UART_CHAR_BIT8;
#if (VDBG_UART_WORKMODE == 1)
		uart_config.mode = VSYS_UART_INT_MODE;
		uart_config.TxFifolevel = 0;
        uart_config.RxFifolevel = 20;
#else
		uart_config.mode = VSYS_UART_FIFO_MODE;
#endif
        VSYS_UART_Init(&uart_config);

        AppTopTaskUartPrintTCB = VOSTaskCreate("App Task Uart Print",(VOSTaskFunc_t)VUart_Print_Task,NULL,13,64*4);
        VUART_INIT_Flag = VSYS_CKM_GetParam(VSYS_UartClockFreq);
    }
}


void VDBG_DrvDeInit(void){
    /* wait until uart rtt  not in use */
    if(AppTopTaskUartPrintTCB){
        VOSTaskSuspend(AppTopTaskUartPrintTCB);
    }
}
void vputs(char *str)
{
	VDBG_DrvInit();
    SEGGER_RTT_WriteString(0, str);
}

int vfputc(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
    return(ch);
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

//int fputc(int ch, FILE *f) 
//{
//    VSYS_UART_CharTx((UINT8*)&ch,1);
//    return(ch);
//}

//int fgetc(FILE *f) 
//{
//	int ch = 0;
//	ch = SEGGER_RTT_WaitKey();
//	SEGGER_RTT_Write(0,&ch,1);
//	return (ch);
//}

void _ttywrch(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
}

void uart_vputs(char *str, unsigned int strlength)
{
    if(strlength == 0)
    {
        strlength = vstrlen((const SINT8 *)str);
    }
	for (size_t offset = 0;offset < strlength;) {
		offset += VSYS_UART_CharTx((const uint8_t *)str+offset,strlength-offset);
	}
}


#if (LOG_BUFF_LEN > 0)
char top_log_buff[LOG_BUFF_LEN] = {0};
char bt_log_buff[LOG_BUFF_LEN] = {0};
#endif
void VUart_Print_Task(void *p_Arg)
{
    UINT32 read_buf_size = 0;
    static UINT32 top_log_buff_size = 0;
    static UINT32 bt_log_buff_size = 0;
    char *rcvbuf,*rcbuf;
    SEGGER_RTT_CB *SEGGER_RTT_indicator;
	VDBG_DrvInit();
    while(1)
    {
        //read_buf_size = SEGGER_RTT_LowLevel_ReadNoLock(VIPC_GetRTTInfoValue_TOP(),0,&rcvbuf);
        SEGGER_RTT_indicator = VIPC_GetRTTInfoValue_TOP();
        if (SEGGER_RTT_indicator != NULL) {
            SEGGER_RTT_ReadNoLock_Protect(SEGGER_RTT_indicator,0,&rcvbuf,read_buf_size);
            if(read_buf_size > 0 && rcvbuf != NULL){
                #if (LOG_BUFF_LEN > 0)
                rcbuf = rcvbuf + read_buf_size -1;
                while(*rcbuf != '\n'){
                    if (!VDBG_IsOutputEnable()) {   // DEBUG LOG输出关闭(通常意味着MSH开启)
                        break;
                    }
                    if(--rcbuf < rcvbuf)
                        break;
                }
                if(rcbuf < rcvbuf){
                    if(top_log_buff_size + read_buf_size >= LOG_BUFF_LEN){
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_vputs("\r\n"TOP_LABEL,0);
                    #endif
                        if(top_log_buff_size > 0)
                            uart_vputs(top_log_buff,top_log_buff_size);
                        uart_vputs(rcvbuf,read_buf_size);
                        uart_vputs("\r\n",0);
                        top_log_buff_size = 0;
                    }else{
                        VCOMMON_MemCpy(top_log_buff+top_log_buff_size,rcvbuf,read_buf_size);
                        top_log_buff_size += read_buf_size;
                        if (!VDBG_IsOutputEnable()) {   // DEBUG LOG输出关闭(通常意味着MSH开启)
                            if(top_log_buff_size > 0) {
                                uart_vputs(top_log_buff,top_log_buff_size);
                                top_log_buff_size = 0;
                            }
                        }
                    }
                }else {
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_vputs("\r\n"TOP_LABEL,0);
                    #endif
                    if(top_log_buff_size > 0)
                        uart_vputs(top_log_buff,top_log_buff_size);
                    top_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
                    if(top_log_buff_size > 0 && top_log_buff_size <= LOG_BUFF_LEN){
                        read_buf_size = rcbuf+1-rcvbuf;
                        VCOMMON_MemCpy(top_log_buff,rcvbuf+read_buf_size,top_log_buff_size);
                    }
                    uart_vputs(rcvbuf,read_buf_size);
                    if(top_log_buff_size > LOG_BUFF_LEN){
                        uart_vputs("\r\n",0);
                        top_log_buff_size = 0;
                    }
                }
                #else
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_vputs("\r\n"TOP_LABEL,0);
                    #endif
                    uart_vputs(rcvbuf,read_buf_size);
                #endif
            }
            SEGGER_RTT_RdOffUpDataNoLock_Protect(SEGGER_RTT_indicator,0);
        }
#if (UartGetRttBuff_EN)
        //read_buf_size = SEGGER_RTT_LowLevel_ReadNoLock(VIPC_GetRTTInfoValue_BT(),0,&rcvbuf);
        SEGGER_RTT_indicator = VIPC_GetRTTInfoValue_BT();
        if (SEGGER_RTT_indicator != NULL) {
            SEGGER_RTT_ReadNoLock_Protect(SEGGER_RTT_indicator,0,&rcvbuf,read_buf_size);
            
            // TODO: BT核的打印, 暂时简单操作, 关闭, 后续需要MSH支持BT核再调整
            if(read_buf_size > 0 && rcvbuf != NULL && VDBG_IsOutputEnable()){
                #if (LOG_BUFF_LEN > 0)
                rcbuf = rcvbuf + read_buf_size -1;
                while(*rcbuf != '\n'){
                    if(--rcbuf < rcvbuf)
                        break;
                }
                if(rcbuf < rcvbuf){
                    if(bt_log_buff_size + read_buf_size >= LOG_BUFF_LEN){
                        #if (LOG_TOP_BT_STYLE == 1)
                            uart_vputs(NORMAL_LOG_FORMAT_BT,0);;
                        #elif (LOG_TOP_BT_STYLE == 2)
                            uart_vputs("\r\n"BT_LABEL,0);
                        #endif
                        if(bt_log_buff_size > 0)
                            uart_vputs(bt_log_buff,bt_log_buff_size);
                        uart_vputs(rcvbuf,read_buf_size);
                        uart_vputs("\r\n",0);
                        bt_log_buff_size = 0;
                    }else{
                        VCOMMON_MemCpy(bt_log_buff+bt_log_buff_size,rcvbuf,read_buf_size);
                        bt_log_buff_size += read_buf_size;
                    }
                }else {
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_vputs(NORMAL_LOG_FORMAT_BT,0);;
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_vputs("\r\n"BT_LABEL,0);
                    #endif
                    if(bt_log_buff_size > 0)
                        uart_vputs(bt_log_buff,bt_log_buff_size);
                    bt_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
                    if(bt_log_buff_size > 0 && bt_log_buff_size <= LOG_BUFF_LEN){
                        read_buf_size = rcbuf+1-rcvbuf;
                        VCOMMON_MemCpy(bt_log_buff,rcvbuf+read_buf_size,bt_log_buff_size);
                    }
                    uart_vputs(rcvbuf,read_buf_size);
                    if(bt_log_buff_size > LOG_BUFF_LEN){
                        uart_vputs("\r\n",0);
                        bt_log_buff_size = 0;
                    }
                }
                #else
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_vputs(NORMAL_LOG_FORMAT_BT,0);;
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_vputs("\r\n"BT_LABEL,0);
                    #endif
                    uart_vputs(rcvbuf,read_buf_size);
                #endif
            }
            SEGGER_RTT_RdOffUpDataNoLock_Protect(SEGGER_RTT_indicator,0);
        }
#endif
    }
}

#endif /*VDBG_EN*/
