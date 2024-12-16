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
	slave die iisin

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_iisin.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_IISIN]", __VA_ARGS__)

#define VModeSelfArg	((VMODEIISinConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEIISinHandle_t*)self->handle)

extern void VMODE_I2sIn0_Isr(void);
static VMODEIISinConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.iis_sel = VSYS_IIS_CH0IN,
	.mode = VSYS_IIS_SLAVE_MODE,
	.dwidth = VSYS_IIS_WIDTH_BIT24,
	.SR = VSYS_IIS_SR_48000,
	.callback = (VDRVIIS_ISR_CALLBACK)VMODE_I2sIn0_Isr,
	.sync_en = FALSE};
VMODECorHandle vmode_iisin_handle = {.arg = &mode_config};

extern void VMODE_I2sIn1_Isr(void);
static VMODEIISinConfig_t mode1_config = {
	.pack_len = AUDIO_PACK_LEN,
	.iis_sel = VSYS_IIS_CH1IN,
	.mode = VSYS_IIS_SLAVE_MODE,
	.dwidth = VSYS_IIS_WIDTH_BIT24,
	.SR = VSYS_IIS_SR_48000,
	.callback = (VDRVIIS_ISR_CALLBACK)VMODE_I2sIn1_Isr,
	.sync_en = TRUE};
VMODECorHandle vmode_iisin1_handle = {.arg = &mode1_config};

static void VMODE_I2sIn0_Isr(void)
{
	VCORISRCoRSche(FALSE);
	((VMODEIISinHandle_t*)vmode_iisin_handle.handle)->updata_diff++;
}

UINT32 iisin_isr_falg , iisin_isr_cnt, iisin_isr_start, iisin_isr_index, iisin_isr_aggre[5];
static void VMODE_I2sIn1_Isr(void)
{
	UINT32 cpucycle = __CPUCycleGet();
	if(iisin_isr_falg == 0)
	{
		iisin_isr_start = cpucycle;
		iisin_isr_falg = 1;
	}
	if(++iisin_isr_index == ARRAY_SIZE(iisin_isr_aggre))
		iisin_isr_index = 0;
	iisin_isr_aggre[iisin_isr_index] = cpucycle - iisin_isr_start;
	iisin_isr_cnt++;


	VCORISRCoRSche(FALSE);
	((VMODEIISinHandle_t*)vmode_iisin1_handle.handle)->updata_diff++;
	VSYS_SYSVIEW_Mark(2);
}

static void IISin_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	size_t len = 0;

	VCORSTART();
	VSYS_IIS_Init(VModeSelfHandle->iis_sel, &VModeSelfHandle->config);
	VMODE_DEBUG("%s%d init\r\n", __FUNC__,VModeSelfHandle->iis_sel>>1);
	if (VModeSelfArg->sync_en)
	{
		VSYS_IIS_SetParam(VModeSelfHandle->iis_sel, VSYS_IISSyncSet, TRUE);
		VSYS_IIS_Enable(VModeSelfHandle->iis_sel, TRUE);
		VCORWaitContent(VSYS_IIS_GetParam(VModeSelfHandle->iis_sel, VSYS_IISState), VCORMAX_DELAY);
	}
	else
	{
		VSYS_IIS_Enable(VModeSelfHandle->iis_sel, TRUE);
	}
	VMODE_DEBUG("%s%d start\r\n", __FUNC__,VModeSelfHandle->iis_sel>>1);
	while (1)
	{
		VCORWaitContent((len = VSYS_IIS_Update(VModeSelfHandle->iis_sel, VModeSelfHandle->cache, VModeSelfHandle->config.len/2)) != 0, VCORMAX_DELAY);
		VCORCRITICAL_ENTER();
		VModeSelfHandle->updata_diff--;
		VCORCRITICAL_EXIT();
		VCORQueSend(self->send, 0, VModeSelfHandle->cache, len, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
		}
		if (VModeSelfHandle->updata_diff < -1 || VModeSelfHandle->updata_diff > 1)
		{
			VMODE_DEBUG("%s Update %d error\r\n", __FUNC__,VModeSelfHandle->updata_diff);
			VModeSelfHandle->updata_diff = 0;
		}
	}
	VCOREND();
	VSYS_IIS_Enable(VModeSelfHandle->iis_sel, FALSE);
	VSYS_IIS_Uninit(VModeSelfHandle->iis_sel);
	if (VModeSelfHandle->cache)
		vfree(VModeSelfHandle->cache);
	if (VModeSelfHandle->config.buff)
		vfree(VModeSelfHandle->config.buff);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_I2sIn_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEIISinHandle_t *handle = vcalloc(1, sizeof(VMODEIISinHandle_t));
	if (handle == NULL)
		goto vmode_init_err;

	handle->iis_sel = VModeSelfArg->iis_sel;
	handle->config.mode = VModeSelfArg->mode; // in slave mode, the clock is supplied externally, which may cause rate inconsistency
	handle->config.dwidth = VModeSelfArg->dwidth;
	handle->config.SR = VModeSelfArg->SR;
	handle->config.format = VSYS_IIS_FORMAT_STANDARD;
	handle->config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	handle->config.len = VModeSelfArg->pack_len * 2;
	handle->config.buff = vmalloc(handle->config.len);
	handle->config.data_mode = VSYS_IIS_DataMode_CROSS;
	handle->config.callback = VModeSelfArg->callback;
	if (handle->config.buff == NULL)
		goto vmode_init_err;

	handle->cache = vmalloc(handle->config.len/2);
	if (handle->cache == NULL)
		goto vmode_init_err;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)IISin_Task, self, 4);
	if (self->tcb == NULL)
		goto vmode_init_err;

	self->state = 1;
	VMODE_DEBUG("%s Mount\r\n", __FUNC__);
	return 0;

vmode_init_err:
	if (handle != NULL)
	{
		if (handle->config.buff)
			vfree(handle->config.buff);
		if (handle->cache)
			vfree(handle->cache);
		vfree(handle);
	}
	if (self->tcb)
		vcortaskdel(self->tcb);
	return 1;
}

static UINT8 VMODE_I2sIn_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_I2sIn_Operate(VMODECorHandle *self, UINT8 cmd, void* arg)
{
	switch ((VMODEIISinCMD_e)cmd)
	{
	case VMODEIISinCMD_SetCfgMode:
		VModeSelfArg->mode =  *(VDRVIISMode_t*)arg;
		break;
	case VMODEIISinCMD_Enable:
		VSYS_IIS_SetParam(VSYS_IIS_CH0IN, VSYS_IISSyncEn, 0);
		break;
	case VMODEIISinCMD_ArgSet:
		{
			UINT8 offset = *((UINT8*)arg);
			UINT8 size = *((UINT8*)arg + 1);
			if(offset + size <= (UINT32)(VModeSelfArg + 1) - (UINT32)VModeSelfArg)
				vmemcpy((UINT8*)VModeSelfArg + offset,(UINT8*)arg + 2,size);
		}
	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(iisin, vmode_iisin_handle, NULL, VMODE_I2sIn_Mount, VMODE_I2sIn_Unmount,VMODE_I2sIn_Operate);
INIT_MODE_MOUNT(iisin1, vmode_iisin1_handle, NULL, VMODE_I2sIn_Mount, VMODE_I2sIn_Unmount,VMODE_I2sIn_Operate);
