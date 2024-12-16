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
	slave die dsp

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
#include "vapp_mode_dsp.h"
#include "vpla_box.h"
#include "vpla_fmw.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_DSP]", __VA_ARGS__)

#undef VModeSelfArg
#undef VModeSelfHandle
#define VModeSelfArg	((VMODEDSPCfg_t*)self->arg)
#define VModeSelfHandle	((VMODEDSPHandle_t*)self->handle)

static VMODEDSPCfg_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.freq = VSYS_CKM_DSPCLK_192MHZ,
	.dwidth = 24,
	.volume_val = 16,
	.path[0].in_len = AUDIO_PACK_LEN,
};
VMODECorHandle vmode_dsp_handle = {.arg = &mode_config};

__IRQ void VISR_Mbox(void)
{
	VMBOX_DSP_INT_CLR();
	VCORISRCoRSche(TRUE);
	if (VBOX_GetTxReq())
	{
		((VMODEDSPHandle_t*)vmode_dsp_handle.handle)->MsgBox_TxReq = TRUE;
	}
}

static void MsgBox_Task(VMODECorHandle *self)
{
	VRET_VALUE ret = VRET_NO_ERROR;
	UINT8 *buff = NULL;
	UINT16 id = 0;
	UINT8 path = 0;
	VCORSTART();
	while (1)
	{
		VCORWaitContent((buff = (UINT8 *)vbox_queue_recv(&id)) != NULL || VModeSelfHandle->MsgBox_TxReq, VCORMAX_DELAY);
		if (VModeSelfHandle->MsgBox_TxReq)
		{
			VBOX_TxCacheUpdate();
			VBOX_HW_Signal();
			VModeSelfHandle->MsgBox_TxReq = FALSE;
		}
		if (NULL != buff)
		{	
			if (id == VDSP_ID_BOX_PRINT_BASE)
			{
				VMODE_DEBUG("BspLog>> %s", buff);
				vfree(buff);
			}
			else if (id >= VDSP_ID_BOX_PATH_DATA_BASE && (id - VDSP_ID_BOX_PATH_DATA_BASE) < VDSP_PATH_MAX_SUPPORT)
			{VSYS_SYSVIEW_Mark(3);
				path = id - VDSP_ID_BOX_PATH_DATA_BASE;
				if(VModeSelfHandle->path[path].dsppack_num == 0)
				{
					vfree(buff);
					VMODE_DEBUG("dsp legacy data\r\n");
					continue;
				}
				VModeSelfHandle->path[path].dsppack_num--;
				VCORQueSend(self->send, path, buff, 0, ret);
				if (ret != 0)
				{
					vfree(buff);
					VMODE_DEBUG("dsp send error\r\n");
				}
			}
			else
			{VSYS_SYSVIEW_Mark(3);
				VMODE_DEBUG("%s id error\r\n", __FUNC__);
				vfree(buff);
			}
		}
	}
	VCOREND();
}

static UINT8 Vapp_Dsp_SetClk(UINT32 clk)
{
	VDSPQue_COMM_t cmd_format;
	cmd_format.cmd = VDSPQue_COMM_SetCycl;
	cmd_format.buff[0] = clk;
	vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, &cmd_format, sizeof(VDSPQue_COMM_t));
	return 0;
}

static UINT8 Vapp_Dsp_InitPath(VMODECorHandle *self, UINT8 path)
{
	UINT16 malloc_len = 0;
	VDSPQue_COMM_t *cmd_format = NULL;
	UINT16 cache_len = VModeSelfArg->pack_len;

	malloc_len = sizeof(VDSPQue_COMM_t) + 4;
	cmd_format = vmalloc(malloc_len);

	if (cmd_format != NULL)
	{
		cmd_format->cmd = VDSPQue_COMM_OpenPath;
		cmd_format->buff[0] = path;
		cmd_format->buff[1] = cache_len | (cache_len << 16);
		vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, cmd_format, malloc_len);
		vfree(cmd_format);
		VModeSelfHandle->path[path].state = 1;
		return 0;
	}
	return 1;
}

static UINT8 Vapp_Dsp_UninitPath(VMODECorHandle *self, UINT8 path)
{
	VDSPQue_COMM_t cmd_format;
	cmd_format.cmd = VDSPQue_COMM_ClosePath;
	cmd_format.buff[0] = path;
	vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, &cmd_format, sizeof(cmd_format));
	VModeSelfHandle->path[path].state = 0;
	return 0;
}

static UINT8 Vapp_Dsp_SetVolume(UINT8 path, UINT8 volume)
{
	UINT16 malloc_len = 0;
	VDSPQue_PathCfg_t *cmd_format = NULL;
	VDSP_ALG_GAIN_CFG_t *gain_cfg = NULL;

	malloc_len = sizeof(VDSPQue_PathCfg_t) - membersizeof(VDSPQue_PathCfg_t, buff) + sizeof(VDSP_ALG_GAIN_CFG_t);
	cmd_format = vmalloc(malloc_len);

	if (cmd_format != NULL)
	{
		gain_cfg = (VDSP_ALG_GAIN_CFG_t *)cmd_format->buff;
		cmd_format->path = path;
		vstrncpy(cmd_format->name, (SINT8 *)"Gain", sizeof(cmd_format->name));
		gain_cfg->cmd = VDSP_ALG_GAINcfg_Setvolume;
		gain_cfg->data[0] = volume / 4;
		vbox_queue_send_static(VDSP_ID_BOX_PATH_CONFIG_BASE, cmd_format, malloc_len);
		vfree(cmd_format);
		return 0;
	}

	return 1;
}

static UINT8 Vapp_Dsp_Init(VMODECorHandle *self)
{
	VSYS_CKM_SetParam(VSYS_DspClockFreq, VModeSelfArg->freq);
	VSYS_CKM_SetParam(VSYS_DSPClockEnable, TRUE);
	VMBOX_DSP_ClkEnable(TRUE);
	VSYS_DSP_Run();
	VBOX_Uninit();
	VBOX_Init();
	Vapp_Dsp_SetClk(VModeSelfArg->freq); // config dsp sysclk
	Vapp_Dsp_UninitPath(self, 0);
	Vapp_Dsp_InitPath(self, 0);
	Vapp_Dsp_SetVolume(0, VModeSelfArg->volume_val);
	VModeSelfHandle->data_len = VModeSelfArg->pack_len;
	return 0;
}

static UINT8 Vapp_Dsp_Uninit(VMODECorHandle *self)
{
	VBOX_Disable();
	VModeSelfHandle->cache = vcalloc(1, VModeSelfHandle->data_len);
	if(VModeSelfHandle->cache != NULL)
	{
		for(UINT8 path = 0; path < ARRAY_SIZE(VModeSelfHandle->path); path++)
		{
			VMODE_DEBUG("dsppack_num:%d %d\r\n",path,VModeSelfHandle->path[path].dsppack_num);
			while(VModeSelfHandle->path[path].dsppack_num)
			{
				VModeSelfHandle->path[path].dsppack_num--;
				vcorquesend(vmode_dsp_handle.send, path, VModeSelfHandle->cache, VModeSelfHandle->data_len);
			}
		}
		vfree(VModeSelfHandle->cache);
	}
	VSYS_DSP_Uninit();
	return 0;
}

static void DSP_Task(VMODECorHandle *self)
{
	VRET_VALUE ret = VRET_NO_ERROR;
	
	VCORSTART();
	Vapp_Dsp_Init(self);
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		VCORQueReceive(self->receive, &VModeSelfHandle->cache_id, &VModeSelfHandle->cache, VCORMAX_DELAY);
		if (VModeSelfHandle->bypass == 0)
		{
			if (VModeSelfHandle->path[VModeSelfHandle->cache_id].state && VModeSelfArg->pack_len == VModeSelfHandle->data_len)
			{
				VCORWaitContent(vbox_queue_send_static(VModeSelfHandle->cache_id + VDSP_ID_BOX_PATH_DATA_BASE, VModeSelfHandle->cache, VModeSelfHandle->data_len) == 0, VCORMAX_DELAY);
				vfree(VModeSelfHandle->cache);
				VSYS_SYSVIEW_Mark(1);
				VModeSelfHandle->path[VModeSelfHandle->cache_id].dsppack_num++;
				if(VModeSelfHandle->cache_id == 0)
				{// dsp path 0 data algorithm copy to path 2
					VModeSelfHandle->path[2].dsppack_num++;
				}
			}
			else
			{
				VMODE_DEBUG("dsp%d send err, state%d len%d receiv len%d\r\n", VModeSelfHandle->cache_id, 
				VModeSelfHandle->path[VModeSelfHandle->cache_id].state,VModeSelfHandle->data_len, VModeSelfArg->pack_len);
				vfree(VModeSelfHandle->cache);
			}
		}
		else
		{
			if(VModeSelfHandle->cache_id == 0)
			{// dsp path 0 data algorithm copy to path 2
				VCORQueSend(self->send, 2, VModeSelfHandle->cache, VModeSelfArg->pack_len, ret);
				if(ret != VRET_NO_ERROR)
				{
					VMODE_DEBUG("dsp%d straight send err\r\n", 2);
				}
			}
			VCORQueSend(self->send, VModeSelfHandle->cache_id, VModeSelfHandle->cache, 0, ret);
			if(ret != VRET_NO_ERROR)
			{
				VMODE_DEBUG("dsp%d straight send err\r\n", VModeSelfHandle->cache_id);
				vfree(VModeSelfHandle->cache);
			}
		}
	}
	VCOREND();
	if (VModeSelfHandle->cache)
		vfree(VModeSelfHandle->cache);
	Vapp_Dsp_Uninit(self);
	if (VModeSelfHandle)
		vfree(VModeSelfHandle);
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_DSP_Init(void)
{
	VDRVDSPCfg_t dsp_cfg;
	if (0 != VPLA_FMW_DspCfg(&dsp_cfg))
	{
		return 1;
	}
	dsp_cfg.DSPFreqClk = VSYS_CKM_DSPCLK_192MHZ;
	VSYS_DSP_Init(&dsp_cfg);
	return 0;
}

static UINT8 VMODE_DSP_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	VMODEDSPHandle_t *handle = vcalloc(1, sizeof(VMODEDSPHandle_t));
	if (handle == NULL)
		goto vmode_init_err;

	self->handle = handle;
	VModeSelfHandle->msgBox_tcb = VCORTaskCreate((VCORTaskFunc_t)MsgBox_Task, self, 2);
	self->tcb = VCORTaskCreate((VCORTaskFunc_t)DSP_Task, self, 3);
	if (self->tcb == NULL || VModeSelfHandle->msgBox_tcb == NULL)
		goto vmode_init_err;

	self->state = 1;
	VMODE_DEBUG("%s Mount\r\n", __FUNC__);
	return 0;
vmode_init_err:
	if (handle != NULL)
	{
		if (VModeSelfHandle->msgBox_tcb)
			vcortaskdel(VModeSelfHandle->msgBox_tcb);
		vfree(handle);
	}
	if (self->tcb)
		vcortaskdel(self->tcb);

	return 1;
}

static UINT8 VMODE_DSP_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(VModeSelfHandle->msgBox_tcb);
	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

static UINT8 VMODE_DSP_Operate(VMODECorHandle *self, UINT8 cmd, void *arg)
{
	UINT8 *Debug_Mem = NULL;
	switch ((VMODEDSPCMD_e)cmd)
	{
	case VMODEDSPCMD_SetVolume:
		if (self->state)
			Vapp_Dsp_SetVolume(*(UINT8 *)arg,*((UINT8 *)arg + 1));
		VModeSelfArg->volume_val = *((UINT8 *)arg + 1);
		break;
	case VMODEDSPCMD_InitPath:
		if (self->state)
			Vapp_Dsp_InitPath(self, *(UINT8 *)arg);
		break;
	case VMODEDSPCMD_UninitPath:
		if (self->state)
			Vapp_Dsp_UninitPath(self, *(UINT8 *)arg);
		break;
	case VMODEDSPCMD_ByPass:
		if (self->state){
			VModeSelfHandle->bypass = *(UINT8 *)arg;
			VMODE_DEBUG("%s dsp bypass mode%d\r\n", __FUNC__, VModeSelfHandle->bypass);
		}
		break;
	case VMODEDSPCMD_MemDisplay:
		if (self->state)
		{
			VDSPQue_COMM_t cmd_format;
			cmd_format.cmd = VDSPQue_COMM_MemDisplay;
			vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, &cmd_format, sizeof(cmd_format));
		}
		break;
	case VMODEDSPCMD_DebugCheck:
		if (self->state)
		{
			Debug_Mem = vmalloc(1 * 1024);
			if (Debug_Mem != NULL)
			{	
				VBOX_KeepRun(TRUE);
				VSYS_DSP_ReadDM(CODEC_DSP_XMEM, (UINT32 *)Debug_Mem, (UINT32 *)(*(UINT32 *)arg), 1 * 1024);
				vfree(Debug_Mem);
			}
		}
		break;
	case VMODEDSPCMD_Test1:
		if (self->state)
		{
			VDSPQue_COMM_t cmd_format;
			cmd_format.cmd = VDSPQue_COMM_Test1;
			cmd_format.buff[0] = *(UINT32 *)arg;
			vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, &cmd_format, sizeof(cmd_format));
		}
		break;
	case VMODEDSPCMD_Test2:
		if (self->state)
		{
			VDSPQue_COMM_t cmd_format;
			cmd_format.cmd = VDSPQue_COMM_Test2;
			cmd_format.buff[0] = *(UINT32 *)arg;
			vbox_queue_send_static(VDSP_ID_BOX_COMM_BASE, &cmd_format, sizeof(cmd_format));
		}
		break;
	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(dsp, vmode_dsp_handle, VMODE_DSP_Init, VMODE_DSP_Mount, VMODE_DSP_Unmount, VMODE_DSP_Operate);
