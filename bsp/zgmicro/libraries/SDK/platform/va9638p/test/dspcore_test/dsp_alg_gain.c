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
	SCO Process

Description:
	SCO Process main functions

Revision History:
	Date                  Author                   Description
	2023.07.14             cxt                   initial version
------------------------------------------------------------------------------*/ \
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "dsp_path.h"
#include "dsp_alg_gain.h"
#include "alg_gain.h"

#undef VDSP_DEBUG
#define VDSP_DEBUG(...) vcom_printf("[VDSP_ALG_GAIN]"__VA_ARGS__)

UINT32 VDsp_Alg_Gain_Init(void *arg)
{
	UINT32 ret = 1;
	VDSP_ALG_GAIN_Handle *handle = (VDSP_ALG_GAIN_Handle *)arg;
	UINT32 packin_len = VDsp_Path_Alg_GetPackIn((VDSP_ALG_HANDLE_t *)handle);
	UINT8 path = VDsp_Path_Alg_GetIndex((VDSP_ALG_HANDLE_t *)handle);
	Alg_Gain_Handle *gain_hd = Alg_Gain_Init(packin_len, handle->data_dwidth, handle->volume_val);
	if (gain_hd != NULL)
	{
		handle->hd = gain_hd;
		ret = 0;
	}
	VDSP_DEBUG("path%d %s %d\r\n", path, __FUNC__, ret);
	return ret;
}

UINT32 VDsp_Alg_Gain_Uninit(void *arg)
{
	UINT32 ret = 1;
	VDSP_ALG_GAIN_Handle *handle = (VDSP_ALG_GAIN_Handle *)arg;
	UINT8 path = VDsp_Path_Alg_GetIndex((VDSP_ALG_HANDLE_t *)handle);
	ret = Alg_Gain_Uninit(handle->hd);
	VDsp_Path_Gain_Delete(handle);
	VDSP_DEBUG("path%d %s %d\r\n", path, __FUNC__, ret);
	return ret;
}

UINT32 VDsp_Alg_Gain_Handling(void *arg, void *in_buff, UINT8 **out_buff)
{
	UINT32 ret;
	VDSP_ALG_GAIN_Handle *handle = (VDSP_ALG_GAIN_Handle *)arg;
	UINT32 packin_len = VDsp_Path_Alg_GetPackIn((VDSP_ALG_HANDLE_t *)handle);
	UINT32 packout_len = VDsp_Path_Alg_GetPackOut((VDSP_ALG_HANDLE_t *)handle);
	UINT8 *alg_cache = vmalloc(packout_len);

	if (alg_cache == NULL)
		return 0;

	ret = Alg_Gain_Exec(handle->hd, in_buff, alg_cache);
	vfree(in_buff);
	*out_buff = alg_cache;
	return ret;
}

UINT32 VDsp_Alg_Gain_Oper(void *arg, void *buff)
{
	UINT32 ret;
	VDSP_ALG_GAIN_Handle *handle = (VDSP_ALG_GAIN_Handle *)arg;
	UINT8 path = VDsp_Path_Alg_GetIndex((VDSP_ALG_HANDLE_t *)handle);
	VDSP_ALG_GAIN_CFG_t *buf_cfg = (VDSP_ALG_GAIN_CFG_t *)buff;

	switch (buf_cfg->cmd)
	{
	case VDSP_ALG_GAINcfg_SetConfig:
		break;
	case VDSP_ALG_GAINcfg_Setdatalen:
		// handle->packin_len = buf_cfg->data;
		// handle->packout_len = buf_cfg->data;
		// VDSP_DEBUG("Alg_Gain data_len = %d\r\n", handle->packin_len);
		break;
	case VDSP_ALG_GAINcfg_Setdwidth:
		// handle->data_dwidth = (UINT8)buf_cfg->data;
		// VDSP_DEBUG("Alg_Gain data_dwidth = %d\r\n", handle->data_dwidth);
		break;
	case VDSP_ALG_GAINcfg_Setvolume:
		ret = Alg_Gain_SetVolume(handle->hd, buf_cfg->data[1]);
		VDSP_DEBUG("path%d Alg_Gain volume_val = %d, ret = %d\r\n", buf_cfg->data[1], ret);
		break;
	default:
		break;
	}
	return 0;
}

VDSP_ALG_GAIN_Handle *VDsp_ALG_Gain_Create(UINT8 data_dwidth, UINT8 volume_val)
{
	VDSP_ALG_GAIN_Handle *handle = vmalloc(sizeof(VDSP_ALG_GAIN_Handle));
	if (handle == NULL)
	{
		return NULL;
	}
	vstrncpy(handle->handle.name, (SINT8 *)"Gain", sizeof(handle->handle.name));
	handle->handle.init = VDsp_Alg_Gain_Init;
	handle->handle.uninit = VDsp_Alg_Gain_Uninit;
	handle->handle.handling = VDsp_Alg_Gain_Handling;
	handle->handle.oper = VDsp_Alg_Gain_Oper;
	handle->handle.pre = NULL;
	handle->handle.next = NULL;
	handle->handle.path = NULL;
	handle->hd = NULL;
	handle->data_dwidth = data_dwidth;
	handle->volume_val = volume_val;

	return handle;
}

UINT32 VDsp_Path_Gain_Delete(VDSP_ALG_GAIN_Handle *handle)
{
	if (handle == NULL)
		return 1;
	vfree(handle);
	return 0;
}
