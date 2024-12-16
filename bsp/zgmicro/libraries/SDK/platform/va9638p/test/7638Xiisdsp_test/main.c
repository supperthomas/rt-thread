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
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstddrv.h"
#include "vcoroutine.h"
#include "vpla_box.h"
#include "vpla_fmw.h"

#define Dbg_PRINT vcom_printf

#define IISCacheLEN (512)

UINT8 cache_buff[IISCacheLEN*2] = {0};
VCORQue_t DebugList = NULL;
VCORQue_t IISOutList = NULL;
VCORQue_t ConfigList = NULL;

BOOL g_MsgBox_TxReq = FALSE;
__IRQ void VISR_Mbox(void)
{
	VMBOX_DSP_INT_CLR();
	VCORISRCoRSche(TRUE);
	if (VBOX_GetTxReq())
	{
		g_MsgBox_TxReq = TRUE;
	}
}

extern void TRA_UART_Rx_Char(UINT8 ch);
__IRQ void VISR_Uart(void)
{
	UINT32 state;
	UINT8 ch;
    state = VSYS_UART_GetStatus(VSYS_UART_Interrupt_Status);

    if (state & VSYS_UART_RX_INT) {
	
            while(VSYS_UART_CharRx(&ch,1)){
			TRA_UART_Rx_Char(ch);
		}
	}
    VSYS_UART_ClearInt();
}

static UINT8 uart_buf[32];
#define TRA_UART_CMD_ORD 0//无请求内容，ACK应答即可
#define TRA_UART_CMD_REQ 1//有请求内容，REP回应数据
#define TRA_UART_CMD_ACK 2
#define TRA_UART_CMD_REP 3
static UINT8 tra_uart_tx_data(UINT8 cmd, UINT8* buff, UINT8 len)
{
	if(len > 28 || cmd == TRA_UART_CMD_ORD || cmd == TRA_UART_CMD_REQ)
		return 0;
	uart_buf[0] = 0x5A;
	uart_buf[1] = (~len&0x3)<<6|(len&0x3)<<4|(cmd&0xF);
	uart_buf[2] = ((~len>>2)&0xF)<<4|((len>>2)&0xF);
	for(UINT8 i = 0;i < len; i++)
		uart_buf[3+i] = buff[i];
	uart_buf[3+len] = VCOMMON_CRC7(uart_buf+3,len);
	VSYS_UART_CharTx(uart_buf,4+len);
	return len;
}

void TRA_UART_Rx_Char(UINT8 ch)
{
	VRET_VALUE ret = VRET_NO_ERROR;
	static UINT8 tra_uart_rx_state = 0;
	static UINT8 tra_uart_rx_cmd;
	static UINT8 tra_uart_rx_len;
	static UINT8 tra_uart_rx_cnt;
	switch(tra_uart_rx_state)    
    {
        case 0://magic
			tra_uart_rx_state_err:
			if(ch == 0x5A)
				tra_uart_rx_state++;
			else
				tra_uart_rx_state = 0;
			break;
		case 1://cmd/len
			if(~(ch>>6&0x3) != (ch>>4&0x3))
				goto tra_uart_rx_state_err;
			tra_uart_rx_cmd = ch&0x0F;
			tra_uart_rx_len = ch>>4&0x3;
			tra_uart_rx_state++;
			break;
		case 2://len
			if(~(ch>>4&0xF) != (ch&0xF))
				goto tra_uart_rx_state_err;	
			tra_uart_rx_cnt = 0;
			tra_uart_rx_len |= (ch&0xF)<<2;
			tra_uart_rx_state++;
			if(tra_uart_rx_len > 0) break;
		case 3://data
			if(tra_uart_rx_cnt < tra_uart_rx_len)
				uart_buf[tra_uart_rx_cnt++] = ch;
			if(tra_uart_rx_cnt == tra_uart_rx_len)
				tra_uart_rx_state++;
			break;
		case 4://crc
			if(VCOMMON_CRC7(uart_buf,tra_uart_rx_len) != ch)
				goto tra_uart_rx_state_err;
			tra_uart_rx_state++;
		case 5://submit
			ret = vcorquesend(ConfigList, 0, uart_buf, tra_uart_rx_len);
			if (ret != VRET_NO_ERROR)
			{
				Dbg_PRINT("%d send error\r\n", __LINE__);
			}
			if(tra_uart_rx_cmd == TRA_UART_CMD_ORD)
				tra_uart_tx_data(TRA_UART_CMD_ACK,NULL,0);
			tra_uart_rx_state = 0;
			break;
		default:
			goto tra_uart_rx_state_err;
	}
}

UINT8 TRA_UART_Tx_Send(UINT8* buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_ORD,buff,len);
}

UINT8 TRA_UART_Tx_Request(UINT8* buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_REQ,buff,len);
}

UINT8 TRA_UART_Tx_Response(UINT8* buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_REP,buff,len);
}


static void MsgBox_Task(void *p_arg)
{
	VRET_VALUE ret = VRET_NO_ERROR;
	UINT8 *buff = NULL;
	UINT16 id = 0;
	VCORSTART();
	while (1)
	{
		VCORWaitContent((buff = (UINT8 *)vbox_queue_recv(&id)) != NULL || g_MsgBox_TxReq,VCORMAX_DELAY);
		if (g_MsgBox_TxReq)
		{
			VBOX_TxCacheUpdate();
			g_MsgBox_TxReq = FALSE;
		}
		if (NULL != buff)
		{
			if (id == 0xFFFF)
			{
				VCORQueSend(DebugList, 0, buff, 0, ret);
				if (ret != VRET_NO_ERROR)
				{
					vfree(buff);
					Dbg_PRINT("%d send error\r\n", __LINE__);
				}
			}
			else if (id == 0x0001)
			{
				VCORQueSend(IISOutList, IISCacheLEN/4, buff, 0, ret);
				if (ret != VRET_NO_ERROR)
				{
					vfree(buff);
					Dbg_PRINT("%d send error\r\n", __LINE__);
				}
			}
			else
			{
				Dbg_PRINT("%d id error\r\n", __LINE__);
				vfree(buff);
			}
		}
	}
	VCOREND();
}

static void Debug_Task(void *p_arg)
{
	UINT8 qid = 0;
	UINT8 *buff = NULL;
	VCORSTART();
	while (1)
	{
		VCORQueReceive(DebugList, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			Dbg_PRINT("BspLog>>[%d]%s", qid, buff);
			vfree(buff);
		}
	}
	VCOREND();
}

static void IISin_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 *buff = NULL;
	size_t len = 0;

	VCORSTART();
	VSYS_IIS_Enable(VSYS_IIS_CH1IN,TRUE);
	while (1)
	{
		VCORWaitContent((len = VSYS_IIS_Update(VSYS_IIS_CH1IN,cache_buff,sizeof(cache_buff))) != 0,VCORMAX_DELAY);
		//VCORQueSend(IISOutList, len/4, cache_buff, len, ret);
		ret = vbox_queue_send_static(0x0001, cache_buff, len);
		if (ret != VRET_NO_ERROR)
		{
			Dbg_PRINT("%d IISinsend error\r\n", __LINE__);
		}
		VCORSched();
	}
	VCOREND();
	VSYS_IIS_Enable(VSYS_IIS_CH1IN,FALSE);
}

static void IISout_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 qid = 0;
	size_t ret_len = 0;
	static UINT8 *buff = NULL;
	static size_t len = 0;

	VCORSTART();
	VSYS_IIS_Enable(VSYS_IIS_CH0OUT,TRUE);
	while (1)
	{
		if(NULL == buff)
		{
			VCORQueReceive(IISOutList, &qid, &buff, 0);
			len = qid*4;
		}
		if (NULL != buff)
		{
			VCORWaitContent((ret_len = VSYS_IIS_Update(VSYS_IIS_CH0OUT,buff,len)) > 0,VCORMAX_DELAY);
			vfree(buff);
			buff = NULL;
			if(ret_len != len)
				Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH0OUT:ret%d != len%d\r\n",ret_len,len);
		}	
	}
	VCOREND();
	VSYS_IIS_Enable(VSYS_IIS_CH0OUT,FALSE);
}

static void cmd_handle(UINT8 *buff)
{
	static UINT8 volume_val = 4;
	UINT8 config_buff[2] = {0};
	switch(buff[0]){
		case 0x11://volume +
			if(volume_val < 8) volume_val++;
			config_buff[0] = 0x11;
			config_buff[1] = volume_val;
			break;
		case 0x12://volume -
			if(volume_val > 0) volume_val--;
			config_buff[0] = 0x11;
			config_buff[1] = volume_val;	
			break;
		default:
			return;
	}
	vbox_queue_send_static(0x0002, config_buff, 2);
}

static void Config_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 qid = 0;
	size_t ret_len = 0;
	static UINT8 *buff = NULL;
	static size_t len = 0;
	UINT8 config_buff[2] = {0x11,4};
	
	VCORSTART();

	vbox_queue_send_static(0x0002, config_buff, 2);
	while (1)
	{
		VCORQueReceive(ConfigList, &qid, &buff, 0);
		if (NULL != buff)
		{
			cmd_handle(buff);
			vfree(buff);
		}	
	}
	VCOREND();
}

int main(void)
{
	VRET_VALUE ret;
	size_t len = 0,ret_len = 0;
	
	extern void VI2C_EnableI2CClk(void);
	VI2C_EnableI2CClk();
	
	VCPU_CoreInit();
	VSYS_CKM_Init();
	VSYS_FLASH_Init();
	VCPU_INT_Enable();
	Dbg_PRINT(" 7638Xiisdsp test\r\n");
	#if 1
	VDRVDSPCfg_t dsp_cfg;
	dsp_cfg.start_addr = 0x30000;
	VSYS_FMW_DSP_INFO vfmw_dsp_info;
	ret = VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, dsp_cfg.start_addr, (UINT8 *)&vfmw_dsp_info, sizeof(VSYS_FMW_DSP_INFO));
	if (ret == VRET_NO_ERROR)
	{
		dsp_cfg.xmem_addr = vfmw_dsp_info.x.address + dsp_cfg.start_addr;
		dsp_cfg.xmem_size = vfmw_dsp_info.x.length;
		dsp_cfg.ymem_addr = vfmw_dsp_info.y.address + dsp_cfg.start_addr;
		dsp_cfg.ymem_size = vfmw_dsp_info.y.length;
		dsp_cfg.pmem_addr = vfmw_dsp_info.p.address + dsp_cfg.start_addr;
		dsp_cfg.pmem_size = vfmw_dsp_info.p.length;
	}
	else
	{
		while (1)
			;
	}
	dsp_cfg.DSPFreqClk = VSYS_CKM_DSPCLK_192MHZ;
	dsp_cfg.dspkey = 0;
	dsp_cfg.datakey = 0;
	VSYS_DSP_Init(&dsp_cfg);
	VSYS_DSP_Run();
	
	VBOX_Init();
	vbox_queue_send_static(0xFFFF, &dsp_cfg.DSPFreqClk, 4);//config dsp sysclk
	UINT8 config_buff[2] = {1,IISCacheLEN/4};
	vbox_queue_send_static(0x0002, config_buff, 2);//config dsp cache len
	config_buff[0] = 2;
	config_buff[1] = 24;
	vbox_queue_send_static(0x0002, config_buff, 2);//config dsp cache len
	
#endif

	DebugList = VCORQueCreate(30);
	IISOutList = VCORQueCreate(10);
	ConfigList = VCORQueCreate(10);

	VCORTimeCycles(VSYS_CKM_GetParam(VSYS_SysClockFreq),0xFFFFFFFF);
	VCORTaskCreate(MsgBox_Task, NULL, 0);
	VCORTaskCreate(Debug_Task, NULL, 1);
	VCORTaskCreate(IISout_Task, NULL, 2);
	VCORTaskCreate(IISin_Task, NULL, 3);
	VCORTaskCreate(Config_Task, NULL, 4);
	
	VDRVUartConfig_t uartconfig = {0};
	uartconfig.Baudrate = 512000;
	uartconfig.port_sel = VSYS_UART_PORT_SEL_AUX;
	uartconfig.Parity = VSYS_UART_PARITY_NONE;
	uartconfig.StopBit = VSYS_UART_STOP_BIT1;
	uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
	uartconfig.mode = VSYS_UART_INT_MODE;
	uartconfig.RxFifolevel = 16;
	VSYS_UART_Init(&uartconfig);
	
	
	
	VDRVIISConfig_t config = {0};
	config.mode = VSYS_IIS_SLAVE_MODE;
	config.dwidth = VSYS_IIS_WIDTH_BIT24;
	config.SR = VSYS_IIS_SR_48000;
	config.format = VSYS_IIS_FORMAT_STANDARD;
	config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	config.len = IISCacheLEN*2;
	config.buff = vmalloc(config.len);
	config.data_mode = VSYS_IIS_DataMode_CROSS;
	VSYS_IIS_Init(VSYS_IIS_CH1IN,&config);

	config.mode = VSYS_IIS_MASTER_MODE;
	config.len = IISCacheLEN*2;
	config.buff = vmalloc(config.len);
	VSYS_IIS_Init(VSYS_IIS_CH0OUT,&config);
	
	for (;;)
	{
		VCORScheduler();
	}
	return 0;
}
