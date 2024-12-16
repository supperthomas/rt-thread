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
	slave die dac

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_dac.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_DAC]", __VA_ARGS__)

#define VModeSelfArg	((VMODEDACConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEDACHandle_t*)self->handle)

extern void VMODE_DAC_Isr(void);
static VMODEDACConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.dwidth = VSYS_CDC_WIDTH_BIT24,
	.SR = VSYS_CDC_SR_48000,
	.callback = (VDRVCDC_ISR_CALLBACK)VMODE_DAC_Isr,
	.sync_en = FALSE};
VMODECorHandle vmode_dac_handle = {.arg = &mode_config};

static void VMODE_DAC_Isr(void)
{
	VCORISRCoRSche(FALSE);
	((VMODEDACHandle_t*)vmode_dac_handle.handle)->updata_diff++;
}

static void DAC_Task(VMODECorHandle *self)
{
	size_t len = 0;
	UINT8 id = 0;

	VCORSTART();
	VSYS_CDC_Init(VSYS_CDC_CH0OUT, &VModeSelfHandle->config);
	VSYS_CDC_SetParam(VSYS_CDC_CH0OUT,VSYS_CDC_Gain,11);
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	if (VModeSelfArg->sync_en)
	{
		VSYS_CDC_SetParam(VSYS_CDC_CH0OUT, VSYS_CDCSyncSet, TRUE);
		VSYS_CDC_Enable(VSYS_CDC_CH0OUT, TRUE);
		VCORWaitContent(VSYS_CDC_GetParam(VSYS_CDC_CH0OUT, VSYS_CDCState), VCORMAX_DELAY);
	}
	else
	{
		VCORWaitContent(self->receive->numb > 1, VCORMAX_DELAY);
		VSYS_CDC_Enable(VSYS_CDC_CH0OUT, TRUE);
	}
	VMODE_DEBUG("%s start\r\n", __FUNC__);
	while (1)
	{
		VCORQueReceive(self->receive, &id, &VModeSelfHandle->cache, VCORMAX_DELAY);
		if (id == 0)
		{
			VCORWaitContent((len = VSYS_CDC_Update(VSYS_CDC_CH0OUT, VModeSelfHandle->cache, VModeSelfHandle->config.len / 2)) > 0, VCORMAX_DELAY);
			VCORCRITICAL_ENTER();
			VModeSelfHandle->updata_diff--;
			VCORCRITICAL_EXIT();
			
			if (len != VModeSelfHandle->config.len / 2)
				VMODE_DEBUG("VSYS_CDC_Update:ret%d != len%d\r\n", len, VModeSelfHandle->config.len / 2);
			if (VModeSelfHandle->updata_diff < -1 || VModeSelfHandle->updata_diff > 1)
			{
				VMODE_DEBUG("%s Update %d error\r\n", __FUNC__, VModeSelfHandle->updata_diff);
				VModeSelfHandle->updata_diff = 0;
			}
		}
		vfree(VModeSelfHandle->cache);
	}
	VCOREND();
	VSYS_CDC_Enable(VSYS_CDC_CH0OUT, FALSE);
	VSYS_CDC_Uninit(VSYS_CDC_CH0OUT);
	if (VModeSelfHandle->config.buff)
		vfree(VModeSelfHandle->config.buff);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

UINT8 VMODE_DAC_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEDACHandle_t *handle = vcalloc(1, sizeof(VMODEDACHandle_t));
	handle->config.dwidth = VModeSelfArg->dwidth;
	handle->config.SR = VModeSelfArg->SR;
	handle->config.channel = VSYS_CDC_BOTH_CHANNEL_ENABLE;
	handle->config.len = VModeSelfArg->pack_len * 2;
	handle->config.buff = vmalloc(handle->config.len);
	handle->config.data_mode = VSYS_CDC_DataMode_CROSS;
	handle->config.callback = VModeSelfArg->callback;
	if (handle->config.buff == NULL)
		goto vmode_init_err;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)DAC_Task, self, 5);
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
		vfree(handle);
	}
	if (self->tcb)
		vcortaskdel(self->tcb);
	return 1;
}

UINT8 VMODE_DAC_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_DAC_Operate(VMODECorHandle *self, VMODEDACOutCMD_e cmd, void *arg)
{
	switch(cmd)
	{
		case VMODEDacCMD_Enable:
			VSYS_CDC_SetParam(VSYS_CDC_CH0OUT, VSYS_CDCSyncEn, 0);
			break;
		case VMODEDacCMD_ArgSet:
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

INIT_MODE_MOUNT(dac, vmode_dac_handle, NULL, VMODE_DAC_Mount, VMODE_DAC_Unmount, VMODE_DAC_Operate);
