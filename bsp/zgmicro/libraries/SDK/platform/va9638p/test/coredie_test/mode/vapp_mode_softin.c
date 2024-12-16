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
	slave die softin

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_softin.h"
#include "pcm_generate.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_SOFTIN]", __VA_ARGS__)

#define VModeSelfArg	((VMODESOFTinConfig_t*)self->arg)
#define VModeSelfHandle	((VMODESOFTinHandle_t*)self->handle)

static VMODESOFTinConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.chan = 2,
	.dwidth = 24,
	.divisor = 10,
	.SR = 48000,
	.freq = 1000};
VMODECorHandle vmode_softin_handle = {.arg = &mode_config};

static void SOFTin_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	size_t len = 0;
	extern UINT32 iisout_isr_cnt;
	
	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		if (VModeSelfHandle->init_cnt != 0)
		{
			VCORWaitContent(VModeSelfHandle->en, VCORMAX_DELAY);
			VModeSelfHandle->init_cnt--;
		}
		else
			VCORWaitContent(VModeSelfHandle->syn_val != iisout_isr_cnt, VCORMAX_DELAY);
		VModeSelfHandle->syn_val = iisout_isr_cnt;
		
		for(len = 0; len < VModeSelfArg->pack_len; len++, VModeSelfHandle->pcm_offset++)
		{
			if(VModeSelfHandle->pcm_offset >= VModeSelfHandle->pcm_len)
				VModeSelfHandle->pcm_offset = 0;
			VModeSelfHandle->cache[len] = VModeSelfHandle->pcm_buff[VModeSelfHandle->pcm_offset];
		}
		
		VCORQueSend(self->send, 0, VModeSelfHandle->cache, len, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
		}	
	}
	VCOREND();
	if (VModeSelfHandle->cache)
		vfree(VModeSelfHandle->cache);
	if (VModeSelfHandle->pcm_buff)
		vfree(VModeSelfHandle->pcm_buff);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_SoftIn_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;
	
	VMODESOFTinHandle_t *handle = vcalloc(1, sizeof(VMODESOFTinHandle_t));
	if (handle == NULL)
		goto vmode_init_err;

	handle->cache = vmalloc(VModeSelfArg->pack_len);
	if (handle->cache == NULL)
		goto vmode_init_err;

	handle->pcm_len = vsine_wave_generate_len(VModeSelfArg->freq, VModeSelfArg->SR, VModeSelfArg->dwidth, VModeSelfArg->chan);
	handle->pcm_buff = vmalloc(handle->pcm_len);
	if (handle->pcm_buff == NULL)
		goto vmode_init_err;
	
	vsine_wave_generate(0, VModeSelfArg->freq, VModeSelfArg->SR, VModeSelfArg->dwidth, VModeSelfArg->chan, VModeSelfArg->divisor, handle->pcm_buff, handle->pcm_len);
	handle->pcm_offset = 0;
	handle->init_cnt = 2;
	handle->syn_val = 0;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)SOFTin_Task, self, 9);
	if (self->tcb == NULL)
		goto vmode_init_err;

	self->state = 1;
	VMODE_DEBUG("%s Mount\r\n", __FUNC__);
	return 0;
vmode_init_err:
	if (handle != NULL)
	{
		if (handle->cache)
			vfree(handle->cache);
		if (handle->pcm_buff)
			vfree(handle->pcm_buff);
		vfree(handle);
	}
	if (self->tcb)
		vcortaskdel(self->tcb);
	return 1;
}

static UINT8 VMODE_SoftIn_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

static UINT8 VMODE_SoftIn_Operate(VMODECorHandle *self, UINT8 cmd, void* arg)
{
	switch ((VMODESOFTinCMD_e)cmd)
	{
	case VMODESOFTinCMD_Enable:
		if (self->state)
		{
			VModeSelfHandle->en = *(UINT8*)arg;
		}
		break;
	case VMODESOFTinCMD_SetFreq:
		if (self->state)
		{
			VModeSelfArg->freq = *(UINT32*)arg;
			if (VModeSelfHandle->pcm_buff)
				vfree(VModeSelfHandle->pcm_buff);
			VModeSelfHandle->pcm_len = vsine_wave_generate_len(VModeSelfArg->freq, VModeSelfArg->SR, VModeSelfArg->dwidth, VModeSelfArg->chan);
			VModeSelfHandle->pcm_buff = vmalloc(VModeSelfHandle->pcm_len);
			if (VModeSelfHandle->pcm_buff != NULL)
			{
				vsine_wave_generate(0, VModeSelfArg->freq, VModeSelfArg->SR, VModeSelfArg->dwidth, VModeSelfArg->chan, VModeSelfArg->divisor, VModeSelfHandle->pcm_buff, VModeSelfHandle->pcm_len);
				VModeSelfHandle->pcm_offset = 0;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(softin, vmode_softin_handle, NULL, VMODE_SoftIn_Mount, VMODE_SoftIn_Unmount,VMODE_SoftIn_Operate);
