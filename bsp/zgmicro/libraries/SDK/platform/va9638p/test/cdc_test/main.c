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
#include "pcm_generate.h"


#define Dbg_PRINT vcom_printf

#define CDCCacheLEN (512)

UINT8 cache_buff[CDCCacheLEN] = {0};
#if 0
int main(void)
{
	VRET_VALUE ret;
	size_t len = 0,ret_len = 0;
//	extern void VI2C_EnableI2CClk(void);
//	VI2C_EnableI2CClk();
	
	VCPU_CoreInit();
	VSYS_CKM_Init();
	VSYS_FLASH_Init();
	VCPU_INT_Enable();
	Dbg_PRINT(" cdc test\r\n");

	VDRVCDCConfig_t cdc_config = {0};
	
	cdc_config.dwidth = VSYS_CDC_WIDTH_BIT24;
	cdc_config.SR = VSYS_CDC_SR_48000;
	cdc_config.channel = VSYS_CDC_BOTH_CHANNEL_ENABLE;
	cdc_config.len = CDCCacheLEN*2;
	cdc_config.buff = vmalloc(cdc_config.len);
	cdc_config.data_mode = VSYS_CDC_DataMode_CROSS;
	//cdc_config.ctrl_mode = 0x01; set ctrl_mode 0 to direct open or close PA
	VSYS_CDC_Init(VSYS_CDC_CH0OUT,&cdc_config);
	VSYS_CDC_SetParam(VSYS_CDC_CH0OUT,VSYS_CDC_Gain,13);
	
	
	UINT16 sin_len = vsine_wave_generate_len(3000,48000,24,2);
	UINT8 *sin_pcm = vmalloc(sin_len);
	vsine_wave_generate(0,3000,48000,24,2, 5, (UINT8*)sin_pcm, sin_len);
	
	VSYS_CDC_Enable(VSYS_CDC_CH0OUT,TRUE);
	
	UINT16 sin_pcm_offset = 0;
	while(1)
	{
		for(len = 0;len < sizeof(cache_buff);len++,sin_pcm_offset++)
		{
			if(sin_pcm_offset >= sin_len)
				sin_pcm_offset = 0;
			cache_buff[len] = sin_pcm[sin_pcm_offset];
		}
		do{
			ret_len = VSYS_CDC_Update(VSYS_CDC_CH0OUT,cache_buff,len);
		}while(ret_len != len);
	}
	
	return 0;
}
#else
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
	Dbg_PRINT(" cdc test\r\n");
	
	VDRVCDCConfig_t cdc_config = {0};
	VDRVIISConfig_t iis_config = {0};

	cdc_config.dwidth = VSYS_CDC_WIDTH_BIT16;
	cdc_config.SR = VSYS_CDC_SR_48000;
	cdc_config.channel = VSYS_CDC_BOTH_CHANNEL_ENABLE;
	cdc_config.len = CDCCacheLEN;
	cdc_config.buff = vmalloc(cdc_config.len);
	cdc_config.data_mode = VSYS_CDC_DataMode_CROSS;
	VSYS_CDC_Init(VSYS_CDC_CH0IN,&cdc_config);
	//VSYS_CDC_SetParam(VSYS_CDC_CH0IN,VSYS_CDC_Gain,10);
	
	cdc_config.dwidth = VSYS_CDC_WIDTH_BIT24;
	cdc_config.SR = VSYS_CDC_SR_48000;
	cdc_config.channel = VSYS_CDC_BOTH_CHANNEL_ENABLE;
	cdc_config.len = CDCCacheLEN*2;
	cdc_config.buff = vmalloc(cdc_config.len);
	cdc_config.data_mode = VSYS_CDC_DataMode_CROSS;
	//cdc_config.ctrl_mode = 0x01; set ctrl_mode 0 to direct open or close PA
	VSYS_CDC_Init(VSYS_CDC_CH0OUT,&cdc_config);
	VSYS_CDC_SetParam(VSYS_CDC_CH0OUT,VSYS_CDC_Gain,13);
	
	iis_config.dwidth = VSYS_IIS_WIDTH_BIT24;
	iis_config.SR = VSYS_IIS_SR_48000;
	iis_config.format = VSYS_IIS_FORMAT_STANDARD;
	iis_config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	iis_config.data_mode = VSYS_IIS_DataMode_CROSS;
	iis_config.mode = VSYS_IIS_MASTER_MODE;
	iis_config.len = CDCCacheLEN*2;
	iis_config.buff = vmalloc(iis_config.len);
	VSYS_IIS_Init(VSYS_IIS_CH0OUT,&iis_config);
	
	iis_config.dwidth = VSYS_IIS_WIDTH_BIT24;
	iis_config.SR = VSYS_IIS_SR_48000;
	iis_config.format = VSYS_IIS_FORMAT_STANDARD;
	iis_config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	iis_config.data_mode = VSYS_IIS_DataMode_CROSS;
	iis_config.mode = VSYS_IIS_MASTER_MODE;
	iis_config.len = CDCCacheLEN*2;
	iis_config.buff = vmalloc(iis_config.len);
	VSYS_IIS_Init(VSYS_IIS_CH1IN,&iis_config);
	
	VSYS_CDC_Enable(VSYS_CDC_CH0IN,TRUE);
	VSYS_CDC_Enable(VSYS_CDC_CH0OUT,TRUE);
//	VSYS_IIS_Enable(VSYS_IIS_CH1IN,TRUE);
	VSYS_IIS_Enable(VSYS_IIS_CH0OUT,TRUE);
	while(1)
	{
		len = VSYS_CDC_Update(VSYS_CDC_CH0IN,cache_buff,sizeof(cache_buff));
		for(UINT32 i = len/2;i > 0;i -= 2)
		{
			((SINT32*)cache_buff)[i-2] = (SINT32)((SINT16*)cache_buff)[i-2] << 8;
			//((SINT32*)cache_buff)[i-1] = (SINT32)((SINT16*)cache_buff)[i-1] << 8;
			((SINT32*)cache_buff)[i-1] = ((SINT32*)cache_buff)[i-2];
		}
		len *= 2;
		
		
		VSYS_IIS_Update(VSYS_IIS_CH0OUT,cache_buff,len);
		len = VSYS_IIS_Update(VSYS_IIS_CH1IN,cache_buff,sizeof(cache_buff));
		for(UINT32 i = 0;i < len/4;i += 1)
		{
			((SINT32*)cache_buff)[i] = ((SINT32*)cache_buff)[i] >> 1;
		}
		
		
		ret_len = VSYS_CDC_Update(VSYS_CDC_CH0OUT,cache_buff,len);
		if(ret_len != len)
			Dbg_PRINT("VSYS_Audio_Update%d:ret%d != len%d\r\n",__LINE__,ret_len,len);
		
		static UINT32 cnt_test = 0;
		if((cnt_test++&0xFFFFFF) == 0)
		{
			UINT8 reg_val[128] = {0};
			VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0x40,reg_val,sizeof(reg_val));
			Dbg_PRINT("\n\n----------------------------\n");
			for(UINT8 i = 0;i < sizeof(reg_val);i++){
				Dbg_PRINT("%02x ",reg_val[i]);
				if(i%16 == 15) Dbg_PRINT("\n");
			}
		}
	}
	
	return 0;
}
#endif

