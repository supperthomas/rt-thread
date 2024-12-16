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
	slave die iis

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstddrv.h"
#include "vcoroutine.h"
#include "vapp_mode_iisout.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_IISOUT]", __VA_ARGS__)

#define VModeSelfArg	((VMODEIISoutConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEIISoutHandle_t*)self->handle)

extern void VMODE_I2sOut0_Isr(void);
static VMODEIISoutConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.iis_sel = VSYS_IIS_CH0OUT,
	.mode = VSYS_IIS_MASTER_MODE,
	.dwidth = VSYS_IIS_WIDTH_BIT24,
	.SR = VSYS_IIS_SR_48000,
	.callback = (VDRVIIS_ISR_CALLBACK)VMODE_I2sOut0_Isr,
	.sync_en = TRUE};
VMODECorHandle vmode_iisout_handle = {.arg = &mode_config};

extern void VMODE_I2sOut1_Isr(void);
static VMODEIISoutConfig_t mode1_config = {
	.pack_len = AUDIO_PACK_LEN,
	.iis_sel = VSYS_IIS_CH1OUT,
	.mode = VSYS_IIS_MASTER_MODE,
	.dwidth = VSYS_IIS_WIDTH_BIT24,
	.SR = VSYS_IIS_SR_48000,
	.callback = (VDRVIIS_ISR_CALLBACK)VMODE_I2sOut1_Isr,
	.sync_en = FALSE};
VMODECorHandle vmode_iisout1_handle = {.arg = &mode1_config};

UINT32 iisout_isr_falg = 0, iisout_isr_cnt = 0, iisout_isr_start = 0, iisout_isr_index = 0, iisout_isr_aggre[5] = {0};
static void VMODE_I2sOut0_Isr(void)
{
	UINT32 cpucycle = __CPUCycleGet();
	UINT32 iisout_isr_aggret;
	if(iisout_isr_falg == 0)
	{
		iisout_isr_start = cpucycle + 10000;
		iisout_isr_falg = 1;
	}
	if(++iisout_isr_index == ARRAY_SIZE(iisout_isr_aggre))
		iisout_isr_index = 0;
	iisout_isr_aggre[iisout_isr_index] = cpucycle - iisout_isr_start;
	iisout_isr_cnt++;

	VCORISRCoRSche(FALSE);
	((VMODEIISoutHandle_t*)vmode_iisout_handle.handle)->updata_diff++;
	VSYS_SYSVIEW_Mark(4);
}

static void VMODE_I2sOut1_Isr(void)
{
	VCORISRCoRSche(FALSE);
	((VMODEIISoutHandle_t*)vmode_iisout1_handle.handle)->updata_diff++;
	//VSYS_SYSVIEW_Mark(3);
}

static void IISout_Task(VMODECorHandle *self)
{
	size_t ret_len = 0;
	UINT8 id = 0;

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
		VCORWaitContent(self->receive->numb > 1, VCORMAX_DELAY);
		VSYS_IIS_Enable(VModeSelfHandle->iis_sel, TRUE);
	}
	VMODE_DEBUG("%s%d start\r\n", __FUNC__,VModeSelfHandle->iis_sel>>1);
	while (1)
	{
		VCORQueReceive(self->receive, &id, &VModeSelfHandle->cache, VCORMAX_DELAY);
		if (id == 0)
		{
			VCORWaitContent((ret_len = VSYS_IIS_Update(VModeSelfHandle->iis_sel, VModeSelfHandle->cache, VModeSelfHandle->config.len / 2)) > 0, VCORMAX_DELAY);
			VCORCRITICAL_ENTER();
			VModeSelfHandle->updata_diff--;
			VCORCRITICAL_EXIT();
			
			if (ret_len != VModeSelfHandle->config.len / 2)
				VMODE_DEBUG("VSYS_IIS_Update%d:ret%d != len%d\r\n",VModeSelfHandle->iis_sel>>1, ret_len, VModeSelfHandle->config.len / 2);
			if (VModeSelfHandle->updata_diff < -1 || VModeSelfHandle->updata_diff > 1)
			{
				VMODE_DEBUG("%s%d Update %d error\r\n", __FUNC__,VModeSelfHandle->iis_sel>>1, VModeSelfHandle->updata_diff);
				VModeSelfHandle->updata_diff = 0;
			}
		}
		vfree(VModeSelfHandle->cache);
	}
	VCOREND();
	VSYS_IIS_Enable(VModeSelfHandle->iis_sel, FALSE);
	VSYS_IIS_Uninit(VModeSelfHandle->iis_sel);
	if (VModeSelfHandle->config.buff)
		vfree(VModeSelfHandle->config.buff);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_I2sOut_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEIISoutHandle_t *handle = vcalloc(1, sizeof(VMODEIISoutHandle_t));
	if (handle == NULL)
		goto vmode_init_err;

	handle->iis_sel = VModeSelfArg->iis_sel;
	handle->config.mode = VModeSelfArg->mode; // in slave mode, the clock is supplied externally, which may cause rate inconsistency
	handle->config.dwidth = VModeSelfArg->dwidth;
	handle->config.SR = VModeSelfArg->SR;
	handle->config.format = VSYS_IIS_FORMAT_STANDARD;
	handle->config.channel = VSYS_IIS_BOTH_CHANNEL_ENABLE;
	handle->config.len = VModeSelfArg->pack_len * 2;
	handle->config.buff = vcalloc(handle->config.len, 1);
	handle->config.data_mode = VSYS_IIS_DataMode_CROSS;
	handle->config.callback = VModeSelfArg->callback;
	if (handle->config.buff == NULL)
		goto vmode_init_err;

	self->handle = handle;
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)IISout_Task, self, 1);
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

static UINT8 VMODE_I2sOut_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

static UINT8 VMODE_I2sOut_Operate(VMODECorHandle *self, VMODEIISOutCMD_e cmd, void *arg)
{
	switch(cmd)
	{
		case VMODEIISOutCMD_Enable:
			VSYS_IIS_SetParam(VSYS_IIS_CH0IN, VSYS_IISSyncEn, 0);
			break;
		case VMODEIISOutCMD_ListNum:
			if(self->receive)
				*((UINT16*)arg) = self->receive->numb;
		case VMODEIISOutCMD_ArgSet:
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

INIT_MODE_MOUNT(iisout, vmode_iisout_handle, NULL, VMODE_I2sOut_Mount, VMODE_I2sOut_Unmount, VMODE_I2sOut_Operate);
INIT_MODE_MOUNT(iisout1, vmode_iisout1_handle, NULL, VMODE_I2sOut_Mount, VMODE_I2sOut_Unmount, VMODE_I2sOut_Operate);
