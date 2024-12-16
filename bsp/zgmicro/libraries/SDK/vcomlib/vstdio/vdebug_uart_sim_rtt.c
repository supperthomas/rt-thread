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
#include "vstdlib.h"     
#include "vstdio.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_UART_SIM_RTT)
#define ALLOW_VKERNEL_INTERNAL_INCLUDES

#include "SEGGER_RTT.h"
#include "vstdos.h"
#include "vpla_ckm.h"
#include "vdrv_ipc.h" 
#include "vreg_def.h"
#include "vpla_gpio.h"

#define UartGetRttBuff_EN                   1       //cxt 2020.08.20  0 - Disable uart to print BT log , 1 - Enable uart to print BT log
#define LOG_BUFF_LEN                        128     //cxt 2021.07.02  uart send log cache size
#define LOG_TOP_BT_STYLE                    1       //cxt 2021.07.02  0-No style, 1-color style, 2-label style
#define TOP_LABEL                           "[TOP] "
#define BT_LABEL                            "[BT] "

#ifndef VDBG_UART_BAUDRATE
#define VDBG_UART_BAUDRATE                  1000000
#endif


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
static VOSTcb_t  AppTopTaskUartPrintTCB = NULL;

void VUart_Sim_Print_Task(void *p_Arg);



#pragma push
#pragma O2 //强制指定O2优化等级

__attribute__((section("RamCode"), noinline)) static void UART_SimSendChar(UINT8 ch)
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




void VDBG_DrvInit(void)
{
    static UINT32 VUART_INIT_Flag = 0;
	if (VUART_INIT_Flag != VSYS_CKM_GetParam(VSYS_SysClockFreq))
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

        UINT32 freq = VSYS_CKM_GetParam(VSYS_SysClockFreq);
        UINT32 bit_clk = (freq + VDBG_UART_BAUDRATE / 2) / VDBG_UART_BAUDRATE;
        bit_delay = (bit_clk - COMMON_CNT) / WHILE_DELAY;

        VSYS_GPIO_Init(1<<UART_SIM_TX_PIN, VSYS_GPIO_MODE_OUTPUT, VSYS_GPIO_OUTPUT_LOW_STRENGTH);

        GPIO_SET_BIT();

        AppTopTaskUartPrintTCB = VOSTaskCreate("App Task Uart SIM Print",(VOSTaskFunc_t)VUart_Sim_Print_Task,NULL,13,64*4);
        VUART_INIT_Flag = VSYS_CKM_GetParam(VSYS_SysClockFreq);
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
    // while (*str != '\0')
    // {
    //     UART_SimSendChar((UINT8)*str++);
    // }
    SEGGER_RTT_WriteString(0, str);
}

int vfputc(int ch) 
{
    //UART_SimSendChar((UINT8)ch);
	SEGGER_RTT_Write(0,&ch,1);
    return(ch);
}


int vfgetc(void)
{
    int ch = 0;

    return (ch);
}



void _ttywrch(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
}

void uart_sim_vputs(char *str, unsigned int strlength)
{
    if(strlength == 0)
    {
        strlength = vstrlen((const SINT8 *)str);
    }
	for (size_t offset = 0;offset < strlength;offset++) {
        UART_SimSendChar((UINT8)str[offset]);
	}
}


#if (LOG_BUFF_LEN > 0)
char top_log_buff[LOG_BUFF_LEN] = {0};
char bt_log_buff[LOG_BUFF_LEN] = {0};
#endif
void VUart_Sim_Print_Task(void *p_Arg)
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
                    if(--rcbuf < rcvbuf)
                        break;
                }
                if(rcbuf < rcvbuf){
                    if(top_log_buff_size + read_buf_size >= LOG_BUFF_LEN){
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_sim_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_sim_vputs("\r\n"TOP_LABEL,0);
                    #endif
                        if(top_log_buff_size > 0)
                            uart_sim_vputs(top_log_buff,top_log_buff_size);
                        uart_sim_vputs(rcvbuf,read_buf_size);
                        uart_sim_vputs("\r\n",0);
                        top_log_buff_size = 0;
                    }else{
                        VCOMMON_MemCpy(top_log_buff+top_log_buff_size,rcvbuf,read_buf_size);
                        top_log_buff_size += read_buf_size;
                    }
                }else {
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_sim_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_sim_vputs("\r\n"TOP_LABEL,0);
                    #endif
                    if(top_log_buff_size > 0)
                        uart_sim_vputs(top_log_buff,top_log_buff_size);
                    top_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
                    if(top_log_buff_size > 0 && top_log_buff_size <= LOG_BUFF_LEN){
                        read_buf_size = rcbuf+1-rcvbuf;
                        VCOMMON_MemCpy(top_log_buff,rcvbuf+read_buf_size,top_log_buff_size);
                    }
                    uart_sim_vputs(rcvbuf,read_buf_size);
                    if(top_log_buff_size > LOG_BUFF_LEN){
                        uart_sim_vputs("\r\n",0);
                        top_log_buff_size = 0;
                    }
                }
                #else
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_sim_vputs(NORMAL_LOG_FORMAT_TOP,0);
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_sim_vputs("\r\n"TOP_LABEL,0);
                    #endif
                    uart_sim_vputs(rcvbuf,read_buf_size);
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
            if(read_buf_size > 0 && rcvbuf != NULL){
                #if (LOG_BUFF_LEN > 0)
                rcbuf = rcvbuf + read_buf_size -1;
                while(*rcbuf != '\n'){
                    if(--rcbuf < rcvbuf)
                        break;
                }
                if(rcbuf < rcvbuf){
                    if(bt_log_buff_size + read_buf_size >= LOG_BUFF_LEN){
                        #if (LOG_TOP_BT_STYLE == 1)
                            uart_sim_vputs(NORMAL_LOG_FORMAT_BT,0);;
                        #elif (LOG_TOP_BT_STYLE == 2)
                            uart_sim_vputs("\r\n"BT_LABEL,0);
                        #endif
                        if(bt_log_buff_size > 0)
                            uart_sim_vputs(bt_log_buff,bt_log_buff_size);
                        uart_sim_vputs(rcvbuf,read_buf_size);
                        uart_sim_vputs("\r\n",0);
                        bt_log_buff_size = 0;
                    }else{
                        VCOMMON_MemCpy(bt_log_buff+bt_log_buff_size,rcvbuf,read_buf_size);
                        bt_log_buff_size += read_buf_size;
                    }
                }else {
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_sim_vputs(NORMAL_LOG_FORMAT_BT,0);;
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_sim_vputs("\r\n"BT_LABEL,0);
                    #endif
                    if(bt_log_buff_size > 0)
                        uart_sim_vputs(bt_log_buff,bt_log_buff_size);
                    bt_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
                    if(bt_log_buff_size > 0 && bt_log_buff_size <= LOG_BUFF_LEN){
                        read_buf_size = rcbuf+1-rcvbuf;
                        VCOMMON_MemCpy(bt_log_buff,rcvbuf+read_buf_size,bt_log_buff_size);
                    }
                    uart_sim_vputs(rcvbuf,read_buf_size);
                    if(bt_log_buff_size > LOG_BUFF_LEN){
                        uart_sim_vputs("\r\n",0);
                        bt_log_buff_size = 0;
                    }
                }
                #else
                    #if (LOG_TOP_BT_STYLE == 1)
                        uart_sim_vputs(NORMAL_LOG_FORMAT_BT,0);;
                    #elif (LOG_TOP_BT_STYLE == 2)
                        uart_sim_vputs("\r\n"BT_LABEL,0);
                    #endif
                    uart_sim_vputs(rcvbuf,read_buf_size);
                #endif
            }
            SEGGER_RTT_RdOffUpDataNoLock_Protect(SEGGER_RTT_indicator,0);
        }
#endif
    }
}

#endif /*VDBG_EN*/
