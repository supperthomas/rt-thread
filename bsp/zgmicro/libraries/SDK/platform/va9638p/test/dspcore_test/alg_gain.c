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
#include "alg_gain.h"

#undef VDSP_DEBUG
#define VDSP_DEBUG(...) vcom_printf("[VALG_GAIN]"__VA_ARGS__)


Alg_Gain_Handle *Alg_Gain_Init(UINT32 pack_len, UINT8 data_dwidth, UINT8 volume_val)
{
	Alg_Gain_Handle *handle = vmalloc(sizeof(Alg_Gain_Handle));
	if (handle == NULL)
	{
		return NULL;
	}
	handle->data_dwidth = data_dwidth;
	handle->volume_val = volume_val;
	handle->pack_len = pack_len;
	return handle;
}

UINT32 Alg_Gain_Uninit(Alg_Gain_Handle *handle)
{
	if (handle == NULL)
	{
		return 1;
	}
	vfree(handle);
	return 0;
}

UINT32 Alg_Gain_Exec(Alg_Gain_Handle *handle, void *in_buff, UINT8 *out_buff)
{
	if (handle == NULL || in_buff == NULL || out_buff == NULL)
	{
		return 1;
	}

	if (handle->data_dwidth > 16)
	{
		UINT32 *in_cache = (UINT32 *)in_buff;
		UINT32 *out_cache = (UINT32 *)out_buff;
		for (UINT32 i = 0; i < handle->pack_len / 4; i++)
		{
			out_cache[i] = in_cache[i] >> (8 - handle->volume_val);
		}
	}
	else
	{
		UINT16 *in_cache = (UINT16 *)in_buff;
		UINT16 *out_cache = (UINT16 *)out_buff;
		for (UINT32 i = 0; i < handle->pack_len / 2; i++)
		{
			out_cache[i] = in_cache[i] >> (8 - handle->volume_val);
		}
	}
	return 0;
}

UINT32 Alg_Gain_SetVolume(Alg_Gain_Handle *handle, UINT8 volume)
{
	if (handle == NULL)
	{
		return 1;
	}

	handle->volume_val = volume;

	return 0;
}
