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
#include "vpla_box.h"
#include "vcoroutine.h"
#include "vpla_fmw.h"

#define Dbg_PRINT vcom_printf

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

VCORQue_t DebugList = NULL;
VCORQue_t XX1List = NULL;
VCORQue_t XX_List = NULL;

UINT32 flag_num = 0;

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
				VCORQueSend(XX1List, 1, buff, 0, ret);
				if (ret != VRET_NO_ERROR)
				{
					vfree(buff);
					Dbg_PRINT("%d send error\r\n", __LINE__);
				}
			}
			else
			{
				VCORQueSend(XX_List, id, buff, 0, ret);
				if (ret != VRET_NO_ERROR)
				{
					vfree(buff);
					Dbg_PRINT("%d send error\r\n", __LINE__);
				}
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

static void XX1_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 qid = 0;
	UINT8 *buff = NULL;
	SINT8 str[15] = "test..cc";

	VCORSTART();

	for (UINT32 i = 0; i < 10; i++)
	{
		buff = vmalloc(15);
		vcom_sprintf((SINT8 *)buff, "test..cc%d", flag_num++);
		ret = vbox_queue_send(0x01, buff, vstrlen((SINT8 *)buff) + 1);
		if (ret)
		{
			vfree(buff);
		}
	}

	while (1)
	{
		VCORQueReceive(XX1List, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			Dbg_PRINT("XX1>>[%d]%s", qid, buff);
			if (vstrncmp((SINT8 *)buff, str, vstrlen(str)))
			{
				Dbg_PRINT("err................\r\n");
			}
			vfree(buff);
		}
		// if(flag_num > 15) flag_num = 0;
		// VBOX_EvtSet(1<<flag_num);
		buff = vmalloc(15);
		vcom_sprintf((SINT8 *)buff, "test..cc%d", flag_num++);
		ret = vbox_queue_send(56, buff, vstrlen((SINT8 *)buff) + 1);
		if (ret)
		{
			vfree(buff);
		}
	}
	VCOREND();
}

static void XX__Task(void *p_arg)
{
	UINT8 qid = 0;
	UINT8 *buff = NULL;
	VCORSTART();
	while (1)
	{
		VCORQueReceive(XX_List, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			Dbg_PRINT("XX_>>[%d]%s", qid, buff);
			vfree(buff);
		}
	}
	VCOREND();
}

static void Timer_Task(void *p_arg)
{
	UINT8 qid = 0;
	UINT8 *buff = NULL;
	static UINT8 index = 0;
	VCORSTART();
	while (1)
	{
		VCORTaskDelay(1000);
		UINT8 reg_buff[6] = {0};
		reg_buff[0] = index++;
		VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
		VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);
		VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,PMU_REG763X_I2C_TEST,reg_buff,1);
		reg_buff[0] = 0;
		VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,0,reg_buff,1);
		Dbg_PRINT("I2C_0:%x\r\n",reg_buff[0]);
		VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,PMU_REG763X_I2C_TEST,reg_buff,6);
		Dbg_PRINT("I2C_TEST:%x %x %x %x %x %x\r\n",reg_buff[0],reg_buff[1],reg_buff[2],reg_buff[3],reg_buff[4],reg_buff[5]);
		
		reg_buff[0] = 0x05;
		VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,0,reg_buff,1);
	}
	VCOREND();
}

UINT32 DSP_BUFF[2][1024] = {0};
volatile UINT32 debug_flag = 1;
int main(void)
{
	VRET_VALUE ret;
	UINT16 flag = 0;
	SINT8 *buff = NULL;
	SINT8 str[15] = "test..cc";

	VCPU_CoreInit();
	VSYS_CKM_Init();
	VSYS_FLASH_Init();
	VCPU_INT_Enable();
	Dbg_PRINT(" dsp test\r\n");
	
#if 1//master
	VDRVGpioUmonitor_t config = {0};
	config.module = VSYS_GPIO_UMSEL_TOP;
	config.pins = 0x10;
	config.sig = 10;
	VSYS_GPIO_SetUmonitor(&config);
	vdelayus(10);
	VSYS_GPIO_Init(VSYS_GPIOL_PIN_12,VSYS_GPIO_MODE_OUTPUT,VSYS_GPIO_OUTPUT_HIGH_STRENGTH);
	VSYS_GPIO_SetPinOutput(VSYS_GPIOL_PIN_12,TRUE);
	VCORTimeCycles(VSYS_CKM_GetParam(VSYS_SysClockFreq),0xFFFFFFFF);
	VCORTaskCreate(Timer_Task, NULL, 4);
#else//slave
	extern void VI2C_EnableI2CClk(void);
	VI2C_EnableI2CClk();
	
	VBOX_StateSet(0x56478980);

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
	VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
	VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);
	dsp_cfg.DSPFreqClk = VSYS_CKM_DSPCLK_192MHZ;
	if(dsp_cfg.DSPFreqClk == VSYS_CKM_DSPCLK_384MHZ || 1)
	{
		
		UINT8 reg_buff[1] = {(DIG_DCDC_1_3V&0x0F)};
		VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,PMU_REG763X_MVDC,reg_buff,1);
	}
	dsp_cfg.dspkey = 0;
	dsp_cfg.datakey = 0;
	VSYS_DSP_Init(&dsp_cfg);
	VSYS_DSP_Run();
	
	VBOX_Init();

	UINT32 ckm_dsp_clk = VSYS_CKM_DSPCLK_384MHZ;
	vbox_queue_send_static(0xFFFF, &ckm_dsp_clk, 4);

	DebugList = VCORQueCreate(30);
	XX1List = VCORQueCreate(10);
	XX_List = VCORQueCreate(10);

	VCORTimeCycles(VSYS_CKM_GetParam(VSYS_SysClockFreq),0xFFFFFFFF);
	VCORTaskCreate(MsgBox_Task, NULL, 0);
	VCORTaskCreate(Debug_Task, NULL, 1);
	VCORTaskCreate(XX1_Task, NULL, 2);
	VCORTaskCreate(XX__Task, NULL, 3);
#endif
	for (;;)
	{
		VCORScheduler();
	}
#if 0
	while (0)
	{
		VBOX_EvtSet(flag);
		buff = vmalloc(15);
		vcom_sprintf(buff, "test..cc%d", flag);
		ret = vbox_queue_send(56, buff, vstrlen((SINT8 *)buff) + 1);
		if (ret)
		{
			vfree(buff);
			buff = NULL;
		}
		if (flag == 170)
		{
			Dbg_PRINT("ok\r\n");
		}
		VDSP_ReadDM(CODEC_DSP_XMEM, (UINT32 *)&DSP_BUFF, (UINT32 *)((UINT32)VBOX_SLAVE_BUFF_BASE >> 2), sizeof(DSP_BUFF));
		vbox_queue_recv(NULL);
		if (NULL != buff)
		{
			Dbg_PRINT("%s\r\n", buff);
			if (vstrncmp(buff, str, vstrlen((SINT8 *)str)))
			{
				Dbg_PRINT("err................\r\n");
			}
			vfree(buff);
		}
		if (VBOX_EvtGet(0xFFFF) != flag)
		{
			Dbg_PRINT("evt err............\r\n");
		}
		flag++;
	}
	while (1)
	{
		UINT8 str_buff[512] = "";
		VBOX_EvtSet(flag++);
		buff = (SINT8 *)vbox_queue_recv_static(NULL, str_buff, sizeof(str_buff));
		if (NULL != buff)
		{
			Dbg_PRINT("BspLog>>%s", buff);
		}
	}
#endif
}
