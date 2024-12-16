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
	slave die slave

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstddrv.h"
#include "vstdcore.h"
#include "vapp_die.h"
#include "vapp_pmu.h"
#include "vapp_mode_iisin.h"
#include "vapp_mode_iisout.h"
#include "vapp_mode_dsp.h"
#include "vapp_mode_adc.h"
#include "vapp_mode_dac.h"
#include "vapp_mode_mix.h"
#include "vapp_mode_merge.h"
#include "vapp_mode_split.h"
#include "vapp_mode_softin.h"
#include "vapp_mode_copy.h"
#include "vapp_mode_delay.h"

#undef VAPP_SLAVE_DEBUG
#define VAPP_SLAVE_DEBUG(...) DEBUG_PRINTF("[VAPP_SLAVE]", __VA_ARGS__)

__irq void VISR_GPIO(void)
{
	UINT64 gpio_state = VSYS_GPIO_GetPinInput(VSYS_GPIOL_PIN_16);
	if (gpio_state)
	{
		UINT8 en = 1;
		VSYS_GPIO_EnableInt(VSYS_GPIOL_PIN_16,VSYS_GPIO_INTTYPE_DISABLE);
		VMODE_Mode_Operate(VMODE_ADC_MODE, VMODEAdcCMD_Enable, &en);
		VMODE_Mode_Operate(VMODE_IISIN_MODE, VMODEIISinCMD_Enable, &en);
		VAPP_SLAVE_DEBUG("%s audio start\r\n",__FUNC__);
	}
}

void Vapp_Host_Demo_Tmr(void *arg)
{
	//Vapp_SlaveDie_OperMode(VMODE_DSP_MODE, VMODEDSPCMD_DebugCheck, NULL, 0);
	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_DisplayMode,NULL,0);
//	UINT16 num = 0;
//	VMODE_Mode_Operate(VMODE_IISOUT_MODE, VMODEIISOutCMD_ListNum, &num);
//	VAPP_SLAVE_DEBUG("%s num=%d\r\n",VMODE_IISOUT_MODE,num);
}

UINT8 Vapp_Host_9647X(void)
{
	UINT8 en;
	VMODE_AddMountFormat_t add_mount = {0};

	vdelayus(1000*1000);
	
	// * -> iisin
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN1_MODE, 0);
	
	//iisin -> iisout
	Vapp_SlaveDie_MountMode(VMODE_IISIN1_MODE, VMODE_IISOUT_MODE, 6);

	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT_MODE, VMODEIISOutCMD_Enable, (UINT8*)&en, sizeof(en));
	
	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);

	return 0;
}

UINT8 Vapp_Host_9647X_Demo(void)
{
	UINT8 argset[3], en;
	VMODE_AddMountFormat_t add_mount = {0};
	
	vdelayus(1000*1000);

	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISIN1_MODE, VMODEIISinCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN1_MODE, 0);
	
	Vapp_SlaveDie_MountMode(VMODE_IISIN1_MODE, VMODE_DSP_MODE, 6);

	Vapp_SlaveDie_MountMode(VMODE_DSP_MODE, VMODE_SPLIT_MODE, 6);
	
	Vapp_SlaveDie_MountMode(VMODE_SPLIT_MODE, VMODE_DELAY_MODE, 6);
	
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(VMODE_DELAY_MODE, VMODE_IISOUT_MODE, 6);
	
	
	argset[0] = 5;argset[1] = 1;argset[2] = 0;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT1_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT1_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	add_mount.depth = 6;
	vstrncpy(add_mount.p_name, VMODE_IISOUT1_MODE, sizeof(add_mount.p_name));
	Vapp_SlaveDie_OperMode(VMODE_SPLIT_MODE, VMODESplitCMD_INVMount2, (UINT8*)&add_mount, sizeof(add_mount));
	
	
#if 1	
	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISIN_MODE, VMODEIISinCMD_Enable, (UINT8*)&en, sizeof(en));
#else 
	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_StartAudio, NULL, 0);
#endif
	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);

	return 0;
}

UINT8 Vapp_Host_9648_Demo(void)
{
	UINT8 argset[3], en;
	VMODE_AddMountFormat_t add_mount = {0};

	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_ADC_MODE, VMODEAdcCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(NULL, VMODE_ADC_MODE, 0);
	
	Vapp_SlaveDie_MountMode(VMODE_ADC_MODE, VMODE_COPY_MODE, 6);

	Vapp_SlaveDie_MountMode(VMODE_COPY_MODE, VMODE_SPLIT_MODE, 6);
	
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(VMODE_SPLIT_MODE, VMODE_IISOUT_MODE, 6);
	
	argset[0] = 5;argset[1] = 1;argset[2] = 0;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT1_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISOUT1_MODE, VMODEIISOutCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	add_mount.depth = 6;
	vstrncpy(add_mount.p_name, VMODE_IISOUT1_MODE, sizeof(add_mount.p_name));
	Vapp_SlaveDie_OperMode(VMODE_SPLIT_MODE, VMODESplitCMD_INVMount1, (UINT8*)&add_mount, sizeof(add_mount));
	
	
	
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISIN1_MODE, VMODEIISinCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN1_MODE, 0);
	
	Vapp_SlaveDie_MountMode(VMODE_IISIN1_MODE, VMODE_DELAY_MODE, 6);
	
	argset[0] = 16;argset[1] = 1;argset[2] = 1;
	Vapp_SlaveDie_OperMode(VMODE_DAC_MODE, VMODEDacCMD_ArgSet, (UINT8*)&argset, sizeof(argset));
	Vapp_SlaveDie_MountMode(VMODE_DELAY_MODE, VMODE_DAC_MODE, 6);
	
	
#if 1	
	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_ADC_MODE, VMODEAdcCMD_Enable, (UINT8*)&en, sizeof(en));
	
	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_IISIN_MODE, VMODEIISinCMD_Enable, (UINT8*)&en, sizeof(en));
#else 
	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_StartAudio, NULL, 0);
#endif
	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);

	return 0;
}

UINT8 Vapp_Host_softin_Demo(void)
{
	UINT8 en;
	VMODE_AddMountFormat_t add_mount = {0};

	// * -> softin
	Vapp_SlaveDie_MountMode(NULL, VMODE_SOFTIN_MODE, 0);
	
	// softin -> dsp	
	Vapp_SlaveDie_MountMode(VMODE_SOFTIN_MODE, VMODE_DSP_MODE, 6);
	
	//dsp -> iisout
	Vapp_SlaveDie_MountMode(VMODE_DSP_MODE, VMODE_IISOUT_MODE, 6);
	
	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);
	
	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_SOFTIN_MODE, VMODESOFTinCMD_Enable, (UINT8*)&en, sizeof(en));
	Vapp_SlaveDie_OperMode(VMODE_IISOUT_MODE, VMODEIISOutCMD_Enable, (UINT8*)&en, sizeof(en));
	
	return 0;
}

UINT8 Vapp_Host_9647Y_Demo(void)
{
	VMODE_AddMountFormat_t add_mount = {0};
	
	VSYS_PWR_Set();

	// * -> iisin
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN1_MODE, 0);
	
	// iisin -> dsp	
	Vapp_SlaveDie_MountMode(VMODE_IISIN1_MODE, VMODE_DSP_MODE, 6);
	
	//dsp -> iisout
	Vapp_SlaveDie_MountMode(VMODE_DSP_MODE, VMODE_IISOUT_MODE, 6);
	
	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);
	
	return 0;
}

UINT8 Vapp_Host_9657_Demo(void)
{
	UINT8 path, en, vol[2];
	VDRVIISMode_t mode;
	VMODE_AddMountFormat_t add_mount = {0};
	
#if 0
	// * -> iisin
	mode = VSYS_IIS_MASTER_MODE;
	Vapp_SlaveDie_OperMode(VMODE_IISIN_MODE, VMODEIISinCMD_SetCfgMode, (UINT8*)&mode, sizeof(mode));
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN_MODE, 0);
	
	// * -> iis1in
	mode = VSYS_IIS_MASTER_MODE;
	Vapp_SlaveDie_OperMode(VMODE_IISIN1_MODE, VMODEIISinCMD_SetCfgMode, (UINT8*)&mode, sizeof(mode));
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN1_MODE, 0);
	
	// iisin -> merge
	Vapp_SlaveDie_MountMode(VMODE_IISIN_MODE, VMODE_MERGE_MODE, 6);
	
	// iis1in -> merge(1)
	add_mount.depth = 6;
	vstrncpy(add_mount.p_name, VMODE_IISIN1_MODE, sizeof(add_mount.p_name));
	Vapp_SlaveDie_OperMode(VMODE_MERGE_MODE, VMODEMergeCMD_Mount1, (UINT8*)&add_mount, sizeof(add_mount));
	
	// merge -> mix
	Vapp_SlaveDie_MountMode(VMODE_MERGE_MODE, VMODE_MIX_MODE, 6);

	
	// iis1in -> iis0
	Vapp_SlaveDie_MountMode(VMODE_MIX_MODE, VMODE_IISOUT_MODE, 6);

#elif 1
	// * -> iisin
	mode = VSYS_IIS_MASTER_MODE;
	Vapp_SlaveDie_OperMode(VMODE_IISIN_MODE, VMODEIISinCMD_SetCfgMode, (UINT8*)&mode, sizeof(mode));
	Vapp_SlaveDie_MountMode(NULL, VMODE_IISIN_MODE, 0);
	
	// iisin -> copy
	Vapp_SlaveDie_MountMode(VMODE_IISIN_MODE, VMODE_COPY_MODE, 6);
	
	// copy -> split
	Vapp_SlaveDie_MountMode(VMODE_COPY_MODE, VMODE_SPLIT_MODE, 6);
	
	// split -> iis0	
	Vapp_SlaveDie_MountMode(VMODE_SPLIT_MODE, VMODE_IISOUT_MODE, 6);
	
	// split(1) -> iis1
	add_mount.depth = 6;
	vstrncpy(add_mount.p_name, VMODE_IISOUT1_MODE, sizeof(add_mount.p_name));
	Vapp_SlaveDie_OperMode(VMODE_SPLIT_MODE, VMODESplitCMD_INVMount1, (UINT8*)&add_mount, sizeof(add_mount));
#else	
	// * -> softin
	Vapp_SlaveDie_MountMode(NULL, VMODE_SOFTIN_MODE, 0);
	
	// softin -> copy
	Vapp_SlaveDie_MountMode(VMODE_SOFTIN_MODE, VMODE_COPY_MODE, 6);
	
	// copy -> dsp
	Vapp_SlaveDie_MountMode(VMODE_COPY_MODE, VMODE_DSP_MODE, 6);
	
	path = 1;
	Vapp_SlaveDie_OperMode(VMODE_DSP_MODE, VMODEDSPCMD_InitPath, (UINT8*)&path, sizeof(path));
	vol[0] = 0;vol[1] = 24;
	Vapp_SlaveDie_OperMode(VMODE_DSP_MODE, VMODEDSPCMD_SetVolume, vol, sizeof(vol));
	
	// dsp -> split
	Vapp_SlaveDie_MountMode(VMODE_DSP_MODE, VMODE_SPLIT_MODE, 6);
	
	// split -> iis0	
	Vapp_SlaveDie_MountMode(VMODE_SPLIT_MODE, VMODE_IISOUT_MODE, 6);
	
	// split(1) -> iis1
	add_mount.depth = 6;
	vstrncpy(add_mount.p_name, VMODE_IISOUT1_MODE, sizeof(add_mount.p_name));
	Vapp_SlaveDie_OperMode(VMODE_SPLIT_MODE, VMODESplitCMD_INVMount1, (UINT8*)&add_mount, sizeof(add_mount));
	
	en = 1;
	Vapp_SlaveDie_OperMode(VMODE_SOFTIN_MODE, VMODESOFTinCMD_Enable, (UINT8*)&en, sizeof(en));
#endif

	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);


	return 0;
}

#include <string.h>
#define TEST_MEMCPY_LEN 200
UINT8 Vapp_Host_test_Demo(void)
{
	UINT8 path, en, vol[2];
	VDRVIISMode_t mode;
	VMODE_AddMountFormat_t add_mount = {0};
	
	Vapp_SlaveDie_MountMode(NULL, VMODE_DSP_MODE, 0);

	VCORTmr_t tmr = VCORTmrCreate(2000, 0, Vapp_Host_Demo_Tmr, NULL);
	VCORTmrStart(tmr);
	
	UINT32 start_time = 0,end_time = 0; 
	UINT8 src_buff[1010] = {0};
	UINT8 des_buff[1010] = {0};
	UINT32 ret = 0;
	for(UINT16 i = 0;i < sizeof(src_buff); i++)
	{
		src_buff[i] = i;
	}
	while(1)
	{
		vmemset(src_buff ,0x5A ,sizeof(des_buff));
		vmemset(des_buff ,0 ,sizeof(des_buff));
		start_time = __CPUCycleGet();
		vmemset(des_buff,0x5A,TEST_MEMCPY_LEN);
		end_time = __CPUCycleGet();
		ret = vmemcmp(des_buff,src_buff,TEST_MEMCPY_LEN);
		VAPP_SLAVE_DEBUG("vmemset align:%d (%d) ret:%d\r\n", (end_time - start_time)/128, end_time - start_time, ret);
		
		vmemset(des_buff ,0 ,sizeof(des_buff));
		start_time = __CPUCycleGet();
		memset(des_buff,0x5A,TEST_MEMCPY_LEN);
		end_time = __CPUCycleGet();
		ret = vmemcmp(des_buff,src_buff,TEST_MEMCPY_LEN);
		VAPP_SLAVE_DEBUG("memset align:%d (%d) ret:%d\r\n", (end_time - start_time)/128, end_time - start_time, ret);
		
		vmemset(des_buff ,0 ,sizeof(des_buff));
		start_time = __CPUCycleGet();
		vmemset(des_buff+1,0x5A,TEST_MEMCPY_LEN);
		end_time = __CPUCycleGet();
		ret = vmemcmp(des_buff+1,src_buff,TEST_MEMCPY_LEN);
		VAPP_SLAVE_DEBUG("vmemset unalign:%d (%d) ret:%d\r\n", (end_time - start_time)/128, end_time - start_time, ret);
		
		vmemset(des_buff ,0 ,sizeof(des_buff));
		start_time = __CPUCycleGet();
		memset(des_buff+1,0x5A,TEST_MEMCPY_LEN);
		end_time = __CPUCycleGet();
		ret = vmemcmp(des_buff+1,src_buff,TEST_MEMCPY_LEN);
		VAPP_SLAVE_DEBUG("memset unalign:%d (%d) ret:%d\r\n", (end_time - start_time)/128, end_time - start_time, ret);
		
		vdelayus(1000*100);
		
	}


	return 0;
}
