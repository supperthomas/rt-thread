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
	slave die delay

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_delay.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_DELAY]", __VA_ARGS__)

#define VModeSelfArg ((VMODEDelayConfig_t *)self->arg)
#define VModeSelfHandle ((VMODEDelayHandle_t *)self->handle)

static VMODEDelayConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.dwidth = 32,
	.channels = 2,
	.delay_sample = 2};
VMODECorHandle vmode_delay_handle = {.arg = &mode_config};

UINT8 *delay_handle(VMODECorHandle *self, UINT8 *buff)
{
	UINT8 *cache = NULL;
	UINT32 sample_len, delay_len, offset = 0, copylen;
	UINT8 dir = VModeSelfArg->delay_sample > VModeSelfHandle->delay_sample ? 1 : 0;

	sample_len = ((VModeSelfArg->dwidth + 8) / 16) * 2 * VModeSelfArg->channels;
	if (dir == 1)
		delay_len = (VModeSelfArg->delay_sample - VModeSelfHandle->delay_sample) * sample_len;
	else
		delay_len = (VModeSelfHandle->delay_sample - VModeSelfArg->delay_sample) * sample_len;

	if (vcorquesend(VModeSelfHandle->delay_list, 0, buff, 0) != VRET_NO_ERROR)
	{
		VMODE_DEBUG("%s delay cache error\r\n", __FUNC__);
		vfree(buff);
	}
	if (dir == 0 && delay_len == VModeSelfHandle->pack_len - VModeSelfHandle->offset)
	{
		if (VModeSelfHandle->cache)
			vfree(VModeSelfHandle->cache);
		vcorquereceive(VModeSelfHandle->delay_list, NULL, &VModeSelfHandle->cache);
		cache = VModeSelfHandle->cache;
		VModeSelfHandle->cache = NULL;
		VModeSelfHandle->offset = VModeSelfHandle->pack_len;
	}
	else
	{
		cache = vmalloc(VModeSelfHandle->pack_len);
		if (cache == NULL)
			return NULL;
		if (dir == 1)
		{
			if(VModeSelfHandle->pack_len >= delay_len)
			{
				vmemset(cache, 0, delay_len);
				offset = delay_len;
				VModeSelfHandle->delay_sample = VModeSelfArg->delay_sample;
			}
			else
			{
				vmemset(cache, 0, VModeSelfHandle->pack_len);
				offset = VModeSelfHandle->pack_len;
				VModeSelfHandle->delay_sample += VModeSelfHandle->pack_len/sample_len;
			}
		}
		while (offset != VModeSelfHandle->pack_len)
		{
			if (VModeSelfHandle->pack_len == VModeSelfHandle->offset)
			{
				if (VModeSelfHandle->cache)
					vfree(VModeSelfHandle->cache);
				vcorquereceive(VModeSelfHandle->delay_list, NULL, &VModeSelfHandle->cache);
				if (VModeSelfHandle->cache != NULL)
					VModeSelfHandle->offset = 0;
				else
				{
					VModeSelfHandle->delay_sample = VModeSelfArg->delay_sample;
					break;
				}
			}
			copylen = VModeSelfHandle->pack_len - MAX(VModeSelfHandle->offset, offset);
			if (dir == 0 && delay_len > 0)
			{
				if (copylen >= delay_len)
				{
					copylen -= delay_len;
					VModeSelfHandle->offset += delay_len;
					delay_len = 0;
					VModeSelfHandle->delay_sample = VModeSelfArg->delay_sample;
				}
				else
				{
					delay_len -= copylen;
					VModeSelfHandle->offset += copylen;
					copylen = 0;
					VModeSelfHandle->delay_sample -= copylen/sample_len;
				}
			}
			vmemcpy(cache + offset, VModeSelfHandle->cache + VModeSelfHandle->offset, copylen);
			VModeSelfHandle->offset += copylen;
			offset += copylen;
		}
	}
	
	return cache;
}

static void Delay_Task(VMODECorHandle *self)
{
	UINT8 id, *buff;
	UINT32 ret = 0;

	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		VCORQueReceive(self->receive, &id, &buff, VCORMAX_DELAY);
		buff = delay_handle(self, buff);
		VCORQueSend(self->send, id, buff, 0, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
			vfree(buff);
		}
	}
	VCOREND();
	if (VModeSelfHandle->cache)
		vfree(VModeSelfHandle->cache);
	while (vcorquereceive(VModeSelfHandle->delay_list, &id, &buff) == VRET_NO_ERROR)
	{
		if (vcorquesend(self->send, id, buff, 0) != VRET_NO_ERROR)
			vfree(buff);
	}
	if (VModeSelfHandle->delay_list)
		VCORQueDel(VModeSelfHandle->delay_list);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

UINT8 VMODE_Delay_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEDelayHandle_t *handle = vcalloc(1, sizeof(VMODEDelayHandle_t));
	if (handle == NULL)
		goto vmode_init_err;

	handle->cache = NULL;
	handle->pack_len = VModeSelfArg->pack_len;
	handle->offset = handle->pack_len;
	handle->delay_sample = 0;
	handle->delay_list = VCORQueCreate(20);
	if (handle->delay_list == NULL)
		goto vmode_init_err;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)Delay_Task, self, 7);
	if (self->tcb == NULL)
		goto vmode_init_err;

	self->state = 1;
	VMODE_DEBUG("%s Mount\r\n", __FUNC__);
	return 0;
vmode_init_err:
	if (handle != NULL)
	{
		if (handle->delay_list)
			VCORQueDel(handle->delay_list);
		vfree(handle);
	}
	if (self->tcb)
		vcortaskdel(self->tcb);
	return 1;
}

UINT8 VMODE_Delay_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_Delay_Operate(VMODECorHandle *self, UINT8 cmd, void *arg)
{
	switch (cmd)
	{

	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(delay, vmode_delay_handle, NULL, VMODE_Delay_Mount, VMODE_Delay_Unmount, VMODE_Delay_Operate);
