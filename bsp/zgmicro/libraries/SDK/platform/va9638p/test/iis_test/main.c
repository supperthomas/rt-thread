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

#define Dbg_PRINT vcom_printf

UINT8 cache_buff[1024] = {0};
volatile UINT32 g_VSYS_Test_INFO [12] __attribute__((at(0x2000E000 + 0),zero_init));
int main(void)
{
	size_t len = 0,ret_len = 0;
	
	VCPU_CoreInit();
	VSYS_CKM_Init();
	VSYS_FLASH_Init();
	VCPU_INT_Enable();
	Dbg_PRINT(" iis test\r\n");
	
	VDRVIISConfig_t config = {0};
	config.mode = VSYS_IIS_SLAVE_MODE;//VSYS_IIS_MASTER_MODE;//VSYS_IIS_SLAVE_MODE;
	config.dwidth = VSYS_IIS_WIDTH_BIT32;
	config.SR = VSYS_IIS_SR_48000;
	config.format = VSYS_IIS_FORMAT_STANDARD;
	config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	config.buff = vmalloc(1024);
	config.len = 1024;
	config.data_mode = VSYS_IIS_DataMode_CROSS;
	VSYS_IIS_Init(VSYS_IIS_CH1OUT,&config);

	config.mode = VSYS_IIS_MASTER_MODE;
	config.dwidth = VSYS_IIS_WIDTH_BIT32;
	config.SR = VSYS_IIS_SR_48000;
	config.format = VSYS_IIS_FORMAT_STANDARD;
	config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	config.buff = vmalloc(1024);
	config.len = 1024;
	config.data_mode = VSYS_IIS_DataMode_CROSS;
	VSYS_IIS_Init(VSYS_IIS_CH0IN,&config);

	VSYS_IIS_Enable(VSYS_IIS_CH0IN,TRUE);
	VSYS_IIS_Enable(VSYS_IIS_CH1OUT,TRUE);
	
	#if 0
	while(1)
	{
		len = VSYS_IIS_Update(VSYS_IIS_CH1IN,cache_buff,sizeof(cache_buff));

		ret_len = VSYS_IIS_Update(VSYS_IIS_CH0OUT,cache_buff,len);
		if(ret_len != len)
			Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH0OUT:ret%d != len%d\r\n",ret_len,len);
	}
	
	#elif 0
	UINT8 index = 0;
	UINT8 RX_buff[512] = {0};
	while(1){
		index++;
		for(UINT32 i = 0;i < sizeof(cache_buff);i++)
		{
			cache_buff[i] = i + index;
		}
		ret_len = VSYS_IIS_Update(VSYS_IIS_CH0OUT,cache_buff,sizeof(cache_buff));
		len = VSYS_IIS_Update(VSYS_IIS_CH1IN,RX_buff,sizeof(RX_buff));
		if(ret_len > 0 && ret_len != sizeof(cache_buff)/2)
			Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH0OUT:ret%d != len%d\r\n",ret_len,sizeof(cache_buff)/2);
		if(len > 0 && len != sizeof(RX_buff))
			Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH1IN:ret%d != len%d\r\n",len,sizeof(RX_buff));
	}
	#else
	UINT32 index = 0,updata_cnt = 0,offset = 0xFFFFFFFF,err_index = 0xFFFFFFFF;
	UINT32 flag_form = 0, valid_len = 0,err_check = 0,err_flag = 0,err_cnt = 0,data_cnt = 0,dpll_s = 0;
	UINT32 TX_buff[512/4] = {0}, RX_buff[512/4];
	VSYS_GPIO_Init(VSYS_GPIO_PIN_21|VSYS_GPIOL_PIN_5, VSYS_GPIO_MODE_OUTPUT, VSYS_GPIO_OUTPUT_HIGH_STRENGTH);
	VSYS_GPIO_Init(VSYS_GPIOL_PIN_4, VSYS_GPIO_MODE_INPUT, VSYS_GPIO_INPUT_NORMAL);
    VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_21|VSYS_GPIOL_PIN_5, FALSE);
	for(UINT32 i = 0;i < ARRAY_SIZE(TX_buff);i++)
	{
		TX_buff[i] = i|(i%4<<29);
	}
	while(1)
	{
		ret_len = VSYS_IIS_Update(VSYS_IIS_CH1OUT,(UINT8*)TX_buff,sizeof(TX_buff));
		if(ret_len > 0 && ret_len != sizeof(TX_buff))
			Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH0OUT:ret%d != len%d\r\n",ret_len,sizeof(TX_buff));
		#if 1 
		if (VSYS_GPIO_GetPinInput(VSYS_GPIOL_PIN_4))
		{
			UINT32 div = 0;
			void VCKM_Set_DPLL_Adjust_Enable(BOOL enable);
			UINT32 VCKM_Speed_Up_DPLL(UINT8 step);
			UINT32  VCKM_Slow_Down_DPLL(UINT8 step);
			VCKM_Set_DPLL_Adjust_Enable(TRUE);
			if (updata_cnt%g_VSYS_Test_INFO[7] == 0)
			{
				if (dpll_s == 0) 
					div = VCKM_Speed_Up_DPLL(g_VSYS_Test_INFO[8]);
				else
					div = VCKM_Slow_Down_DPLL(g_VSYS_Test_INFO[8]);
				if (div != 0)
				{
					if (div >= g_VSYS_Test_INFO[9])
						dpll_s = 1;
					else if (div <= g_VSYS_Test_INFO[10])
						dpll_s = 0;
					g_VSYS_Test_INFO[11] = div;
				}
				else{
					dpll_s = dpll_s?0:1;
				}
			}
		}
		if (g_VSYS_Test_INFO[6] != 0)
		{
			err_cnt = 0;
			data_cnt = 0;
			g_VSYS_Test_INFO[6] = 0;
		}
		len = VSYS_IIS_Update(VSYS_IIS_CH0IN,cache_buff,sizeof(cache_buff));
		if(len > 0)
		{
			updata_cnt++;
			if (updata_cnt < 10)
				continue;
			for(UINT32 i = 0;i < ARRAY_SIZE(RX_buff);i++)
			{
				RX_buff[i] = ((UINT32*)cache_buff)[i]>>1;
			}
			valid_len = 0;
			err_check = 0;
			for(UINT32 i = 0;i < ARRAY_SIZE(RX_buff);i++)
			{
				UINT32 tmp = RX_buff[i]&0xFFFFFF, flag = (RX_buff[i]>>29)&0x03;
				if (tmp != offset)
				{
					offset = RX_buff[i]&0xFFFFFF;
					err_index = i;
					err_check++;
					VSYS_GPIO_SetPinOutput(VSYS_GPIOL_PIN_5, TRUE);
					//Dbg_PRINT("err: %d\r\n",err_index);
					VSYS_GPIO_SetPinOutput(VSYS_GPIOL_PIN_5, FALSE);
				}
				offset = (offset + 1)%ARRAY_SIZE(RX_buff);
				if ((flag&0x2) == (flag_form&0x2))
				{
					*((UINT32*)cache_buff + valid_len) = RX_buff[i]&0xFFFFFF;
					flag_form = (flag_form + 1)%4;
					valid_len++;
				}		
			}
			if(len != sizeof(RX_buff))
				Dbg_PRINT("VSYS_IIS_Update VSYS_IIS_CH1IN:ret%d != len%d\r\n",len,sizeof(RX_buff));
			
//			for(UINT32 i = 1;i < valid_len;i++)
//			{
//				if (*((UINT32*)cache_buff + i) != (*((UINT32*)cache_buff) + i))
//				{
//					Dbg_PRINT("cache_buff error\r\n");
//				}		
//			}
			
			err_cnt += ARRAY_SIZE(RX_buff) - valid_len;
			data_cnt += ARRAY_SIZE(RX_buff);
			g_VSYS_Test_INFO[0] = MAKE_U32("DAUP");
			g_VSYS_Test_INFO[1] = (2<<16)|2;   //x为主坐标轴个数;   y为从坐标轴个数;   
			g_VSYS_Test_INFO[1+1] = ARRAY_SIZE(RX_buff) - valid_len;
			g_VSYS_Test_INFO[1+2] = err_check;
			g_VSYS_Test_INFO[1+3] = 10000*err_cnt/data_cnt;
			g_VSYS_Test_INFO[1+4] = 10000*err_cnt/data_cnt;
		}
		#endif
		if (index++%100000 == 0)
		{
			VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_21, !VSYS_GPIO_GetPinInput(VSYS_GPIO_PIN_21));
		}
	}
	
	#endif 
}
