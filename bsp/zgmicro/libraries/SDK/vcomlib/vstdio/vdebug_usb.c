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

#if (VDBG_EN > 0) 

#if (VDBG_IO_RETARGET == 0)

/**
 * @brief 在使用新的Log模块时，VDBG_IO_RETARGET 可能为 0 需要声明 _ttywrch 函数
 * 
 * @param ch 
 */
void _ttywrch(int ch) 
{
//    VUART_CharTxWithBlocking((UINT8)ch);
}
#endif

#if (VDBG_IO_RETARGET == IO_USB_SERIAL)

#include "vstdos.h"
#include "vstddrv.h"
#include "usb.h"
#include "usb_osal.h"


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

void usb_vputs(char *str, unsigned int strlength) {
	if(strlength == 0)
    {
        strlength = vstrlen((SINT8*)str);
    }
	usbd_cdc_write((const uint8_t *)str, strlength);
}

void VDBG_DrvInit(void) 
{
	static UINT8 workon = 0;

	if(workon==0)
	{
		workon = 1;

		if (NULL == cdc_rx_sem) {
			cdc_rx_sem = usb_osal_sem_create(0);
			usbd_cdc_set_rx_indicate(_cdc_rx_cb);
		}

       /* 初始化cdc设备 */
    	usbd_cdc_init();
	}
}

void VDBG_DrvDeInit(void){
	/* wait until usb  not in use */
}

void vputs(char *str) {
	usb_vputs(str,0);
}
 
int vfputc(int ch) 
{
	char c = ch;
	usb_vputs(&c,1);

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
    char c = (char)ch;
    usb_vputs(&c,1);
}


#endif
#endif
