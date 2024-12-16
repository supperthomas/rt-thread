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
	slave die adc

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_adc.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_ADC]", __VA_ARGS__)

#define VModeSelfArg	((VMODEADCConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEADCHandle_t*)self->handle)

extern void VMODE_ADC_Isr(void);
static VMODEADCConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN / 2,
	.dwidth = VSYS_CDC_WIDTH_BIT16,
	.SR = VSYS_CDC_SR_48000,
	.callback = (VDRVCDC_ISR_CALLBACK)VMODE_ADC_Isr,
	.sync_en = FALSE};
VMODECorHandle vmode_adc_handle = {.arg = &mode_config};

static void VMODE_ADC_Isr(void)
{
	VCORISRCoRSche(FALSE);
	((VMODEADCHandle_t*)vmode_adc_handle.handle)->updata_diff++;
}

static void ADC_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	size_t len = 0;
	SINT16 *cache;

	VCORSTART();
	VSYS_CDC_Init(VSYS_CDC_CH0IN, &VModeSelfHandle->config);
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	if (VModeSelfArg->sync_en)
	{
		VSYS_CDC_SetParam(VSYS_CDC_CH0IN, VSYS_CDCSyncSet, TRUE);
		VSYS_CDC_Enable(VSYS_CDC_CH0IN, TRUE);
		VCORWaitContent(VSYS_CDC_GetParam(VSYS_CDC_CH0IN, VSYS_CDCState), VCORMAX_DELAY);
	}
	else
	{
		VSYS_CDC_Enable(VSYS_CDC_CH0IN, TRUE);
	}
	VMODE_DEBUG("%s start\r\n", __FUNC__);
	while (1)
	{
		VCORWaitContent((len = VSYS_CDC_Update(VSYS_CDC_CH0IN, VModeSelfHandle->cache, VModeSelfHandle->config.len/2)) != 0, VCORMAX_DELAY);
		VCORCRITICAL_ENTER();
		VModeSelfHandle->updata_diff--;
		VCORCRITICAL_EXIT();
		cache = (SINT16 *)VModeSelfHandle->cache;
		for (UINT32 i = len / 2; i > 0; i -= 2)
		{
			((SINT32 *)cache)[i - 1] = (SINT32)cache[i - 1] << 8;
			((SINT32 *)cache)[i - 2] = (SINT32)cache[i - 2] << 8;
		}
		VCORQueSend(self->send, 0, VModeSelfHandle->cache, len * 2, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
		}
	}
	VCOREND();
	VSYS_CDC_Enable(VSYS_CDC_CH0IN, FALSE);
	VSYS_CDC_Uninit(VSYS_CDC_CH0IN);
	if (VModeSelfHandle->cache)
		vfree(VModeSelfHandle->cache);
	if (VModeSelfHandle->config.buff)
		vfree(VModeSelfHandle->config.buff);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

UINT8 VMODE_ADC_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEADCHandle_t *handle = vcalloc(1, sizeof(VMODEADCHandle_t));
	handle->config.dwidth = VModeSelfArg->dwidth;
	handle->config.SR = VModeSelfArg->SR;
	handle->config.channel = VSYS_CDC_BOTH_CHANNEL_ENABLE;
	handle->config.len = VModeSelfArg->pack_len * 2;
	handle->config.buff = vmalloc(handle->config.len);
	handle->config.data_mode = VSYS_CDC_DataMode_CROSS;
	handle->config.callback = VModeSelfArg->callback;
	if (handle->config.buff == NULL)
		goto vmode_init_err;

	handle->cache = vmalloc(handle->config.len);
	if (handle->cache == NULL)
		goto vmode_init_err;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)ADC_Task, self, 5);
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

UINT8 VMODE_ADC_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_ADC_Operate(VMODECorHandle *self, VMODEADCOutCMD_e cmd, void *arg)
{
	switch(cmd)
	{
		case VMODEAdcCMD_Enable:
			VSYS_CDC_SetParam(VSYS_CDC_CH0IN, VSYS_CDCSyncEn, 0);
			break;
		case VMODEAdcCMD_ArgSet:
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

INIT_MODE_MOUNT(adc, vmode_adc_handle, NULL, VMODE_ADC_Mount, VMODE_ADC_Unmount, VMODE_ADC_Operate);
