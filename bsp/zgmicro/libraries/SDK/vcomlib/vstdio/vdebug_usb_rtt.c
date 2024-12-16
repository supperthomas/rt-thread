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
 	2019.02.11           zhangcheng             	initial version
 	2019.10.09           miao.mingming              TP+BT print
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"     
#include "vstdio.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_USB_RTT)

#include "SEGGER_RTT.h"
#include "vstdos.h"
#include "vstddrv.h"
#include "usb.h"
#include "usb_osal.h"

#define UsbGetRttBuff_EN					1     //cxt 2020.08.20  0 - Disable USB to print BT log , 1 - Enable USB to print BT log
#define LOG_BUFF_LEN							128     //cxt 2021.07.02  USB send log cache size
#define LOG_TOP_BT_STYLE					1			//cxt 2021.07.02  0-No style, 1-color style, 2-label style
#define TOP_LABEL                           "[TOP] "
#define BT_LABEL                            "[BT] "

VOSTcb_t AppTopTaskOTGPrintTCB = NULL;

void VOTG_Print_Task(void *p_Arg);
int VDBG_IsOutputEnable(void);
#define RTT_HIGH_SPEED_SYNC_DBG_EN          (VDBG_EN && 0)
#define VDBG_UART_RX_FIFO_SIZE 128  //必须为2的整次幂
static usb_osal_sem_t cdc_rx_sem;


UINT8 g_VDBG_USB_RX_Buffer[VDBG_UART_RX_FIFO_SIZE];
UINT16 g_VDBG_USB_Rx_Rd_Idx = 0;
UINT16 g_VDBG_USB_Rx_Wr_Idx = 0;

VOSSemap_t g_VDBG_USB_RX_Sem = NULL;

static inline  UINT16 VDBG_USB_Rx_GetCount(void)
{
    return (g_VDBG_USB_Rx_Wr_Idx - g_VDBG_USB_Rx_Rd_Idx) & (VDBG_UART_RX_FIFO_SIZE - 1);
}

static inline int VDBG_USB_Rx_GetBuf(UINT8 *buf, UINT16 len)
{
    UINT16 i;
    for (i = 0; i < len; i++)
    {
        if (VDBG_USB_Rx_GetCount() == 0)
        {
            break;
        }

        buf[i] = g_VDBG_USB_RX_Buffer[g_VDBG_USB_Rx_Rd_Idx&(VDBG_UART_RX_FIFO_SIZE - 1)];
        g_VDBG_USB_Rx_Rd_Idx++;
    }
    return i;
}

static inline int VVDBG_USB_Rx_PutBuf(UINT8 *buf, UINT16 len)
{
    UINT16 i;
    for (i = 0; i < len; i++)
    {
        if (VDBG_USB_Rx_GetCount() == (VDBG_UART_RX_FIFO_SIZE - 1))
        {
            break;
        }
        g_VDBG_USB_RX_Buffer[g_VDBG_USB_Rx_Wr_Idx&(VDBG_UART_RX_FIFO_SIZE - 1)] = buf[i];
        g_VDBG_USB_Rx_Wr_Idx++;
    }

    return i;
}

static void _cdc_rx_cb(uint8_t *buff, uint16_t size)
{
	VVDBG_USB_Rx_PutBuf(buff, size);
    usb_osal_sem_give(cdc_rx_sem);
}



void VDBG_DrvInit(void) 
{
	static UINT8 workon = 0;

	if(workon==0)
	{
		workon = 1;
        SEGGER_RTT_Init();
#if (RTT_HIGH_SPEED_SYNC_DBG_EN > 0)
        // set the buffer run in block mode
        SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#else
        // set the buffer run in nonblock mode
        SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif //#if (RTT_HIGH_SPEED_SYNC_DBG_EN >0)

        VIPC_SetRTTInfoValue_TOP(&_SEGGER_RTT) ;

		if (NULL == cdc_rx_sem) {
			cdc_rx_sem = usb_osal_sem_create(0);
			usbd_cdc_set_rx_indicate(_cdc_rx_cb);
		}


		AppTopTaskOTGPrintTCB = VOSTaskCreate("App Task Otg Print",(VOSTaskFunc_t)VOTG_Print_Task,NULL,13,50*4);
	}
}

void VDBG_DrvDeInit(void){
	/* wait until usb  not in use */
	if(AppTopTaskOTGPrintTCB){
		VOSTaskSuspend(AppTopTaskOTGPrintTCB);
	}
}

void vputs(char *str) {
  VDBG_DrvInit();
  SEGGER_RTT_WriteString(0, str);
}

void vputs_rtt(char *buf,unsigned int len) {
  SEGGER_RTT_Write(0, buf,len);
}
 
int vfputc(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
	return(ch);
}

int vfgetc(void) 
{
	int ch = 0;
	int ret = VDBG_USB_Rx_GetBuf((UINT8 *)&ch, 1);
	if(ret != 1)
	{
		usb_osal_sem_take(cdc_rx_sem, VOSMAX_DELAY);
	}

	return (ch);
}

void _ttywrch(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
}



void usb_vputs(char *str, unsigned int strlength) {
	if(strlength == 0)
    {
        strlength = vstrlen((SINT8*)str);
    }
	usbd_cdc_write((const uint8_t *)str, strlength);
}


#if (LOG_BUFF_LEN > 0)
char top_log_buff[LOG_BUFF_LEN] = {0};
char bt_log_buff[LOG_BUFF_LEN] = {0};
#endif
void VOTG_Print_Task(void *p_Arg)
{
	UINT32 read_buf_size = 0;
	static UINT32 top_log_buff_size = 0;
	static UINT32 bt_log_buff_size = 0;
	char *rcvbuf,*rcbuf;
	VDBG_DrvInit();
	/* 初始化cdc设备 */
	usbd_cdc_init();
  while(1)
	{
		//read_buf_size = SEGGER_RTT_LowLevel_ReadNoLock(VIPC_GetRTTInfoValue_TOP(),0,&rcvbuf);	
		SEGGER_RTT_ReadNoLock_Protect(VIPC_GetRTTInfoValue_TOP(),0,&rcvbuf,read_buf_size);
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
					usb_vputs(NORMAL_LOG_FORMAT_TOP,0);
				#elif (LOG_TOP_BT_STYLE == 2)
					usb_vputs("\r\n"TOP_LABEL,0);
				#endif
					if(top_log_buff_size > 0)
						usb_vputs(top_log_buff,top_log_buff_size);
					usb_vputs(rcvbuf,read_buf_size);
					usb_vputs("\r\n",0);
					top_log_buff_size = 0;
				}else{
					VCOMMON_MemCpy(top_log_buff+top_log_buff_size,rcvbuf,read_buf_size);
					top_log_buff_size += read_buf_size;
                    if (!VDBG_IsOutputEnable()) {   // DEBUG LOG输出关闭(通常意味着MSH开启)
                        if(top_log_buff_size > 0) {
                            usb_vputs(top_log_buff,top_log_buff_size);
                            top_log_buff_size = 0;
                        }
                    }
				}		
			}else {
				#if (LOG_TOP_BT_STYLE == 1)
					usb_vputs(NORMAL_LOG_FORMAT_TOP,0);
				#elif (LOG_TOP_BT_STYLE == 2)
					usb_vputs("\r\n"TOP_LABEL,0);
				#endif
				if(top_log_buff_size > 0)
					usb_vputs(top_log_buff,top_log_buff_size);
				top_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
				if(top_log_buff_size > 0 && top_log_buff_size <= LOG_BUFF_LEN){
					read_buf_size = rcbuf+1-rcvbuf;
					VCOMMON_MemCpy(top_log_buff,rcvbuf+read_buf_size,top_log_buff_size);
				}
				usb_vputs(rcvbuf,read_buf_size);
				if(top_log_buff_size > LOG_BUFF_LEN){
					usb_vputs("\r\n",0);
					top_log_buff_size = 0;
				}
			}
			#else
				#if (LOG_TOP_BT_STYLE == 1)
					usb_vputs(NORMAL_LOG_FORMAT_TOP,0);
				#elif (LOG_TOP_BT_STYLE == 2)
					usb_vputs("\r\n"TOP_LABEL,0);
				#endif
				usb_vputs(rcvbuf,read_buf_size);
			#endif
		}
		SEGGER_RTT_RdOffUpDataNoLock_Protect(VIPC_GetRTTInfoValue_TOP(),0);
#if (UsbGetRttBuff_EN)
		//read_buf_size = SEGGER_RTT_LowLevel_ReadNoLock(VIPC_GetRTTInfoValue_BT(),0,&rcvbuf);
		SEGGER_RTT_ReadNoLock_Protect(VIPC_GetRTTInfoValue_BT(),0,&rcvbuf,read_buf_size);

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
						usb_vputs(NORMAL_LOG_FORMAT_BT,0);;
					#elif (LOG_TOP_BT_STYLE == 2)
						usb_vputs("\r\n"BT_LABEL,0);
					#endif
					if(bt_log_buff_size > 0)
						usb_vputs(bt_log_buff,bt_log_buff_size);
					usb_vputs(rcvbuf,read_buf_size);
					usb_vputs("\r\n",0);
					bt_log_buff_size = 0;
				}else{
					VCOMMON_MemCpy(bt_log_buff+bt_log_buff_size,rcvbuf,read_buf_size);
					bt_log_buff_size += read_buf_size;
				}		
			}else {
				#if (LOG_TOP_BT_STYLE == 1)
					usb_vputs(NORMAL_LOG_FORMAT_BT,0);;
				#elif (LOG_TOP_BT_STYLE == 2)
					usb_vputs("\r\n"BT_LABEL,0);
				#endif
				if(bt_log_buff_size > 0)
					usb_vputs(bt_log_buff,bt_log_buff_size);
				bt_log_buff_size = read_buf_size - (rcbuf+1-rcvbuf);
				if(bt_log_buff_size > 0 && bt_log_buff_size <= LOG_BUFF_LEN){
					read_buf_size = rcbuf+1-rcvbuf;
					VCOMMON_MemCpy(bt_log_buff,rcvbuf+read_buf_size,bt_log_buff_size);
				}
				usb_vputs(rcvbuf,read_buf_size);
				if(bt_log_buff_size > LOG_BUFF_LEN){
					usb_vputs("\r\n",0);
					bt_log_buff_size = 0;
				}
			}
			#else
				#if (LOG_TOP_BT_STYLE == 1)
					usb_vputs(NORMAL_LOG_FORMAT_BT,0);;
				#elif (LOG_TOP_BT_STYLE == 2)
					usb_vputs("\r\n"BT_LABEL,0);
				#endif
				usb_vputs(rcvbuf,read_buf_size);
			#endif
		}	
		SEGGER_RTT_RdOffUpDataNoLock_Protect(VIPC_GetRTTInfoValue_BT(),0);
#endif	
        #if SYSTEM_DEEPSLEEP_ENABLE
        VOSTimeDly(1);
        #endif	
	}
}






#endif

