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
	slave die mix

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_mix.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_MIX]", __VA_ARGS__)

#define VModeSelfArg	((VMODEMixConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEMixHandle_t*)self->handle)

static VMODEMixConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.dwidth = 32,
	.channels = 2,
	.ratio = {1,1,1}};
VMODECorHandle vmode_mix_handle = {.arg = &mode_config};

UINT8 *mix_handle(VMODECorHandle *self, UINT8 *buff[], UINT8 len)
{
	UINT8 valid_num = 0;
	UINT8  ratio_tmp[VMODE_MIX_MAX_SUPPORT];
	UINT16 ratio_all = 0;
	SINT16 *buff16_tmp[VMODE_MIX_MAX_SUPPORT];
	SINT32 *buff32_tmp[VMODE_MIX_MAX_SUPPORT];
	UINT8 *ratio = VModeSelfArg->ratio;

	if (len > VMODE_MIX_MAX_SUPPORT)
	{
		VMODE_DEBUG("too many mixed paths\r\n");
		return NULL;
	}
	for (UINT8 i = 0; i < len; i++)
	{
		if (buff[i] != NULL)
		{
			buff16_tmp[valid_num] = (SINT16 *)buff[i];
			buff32_tmp[valid_num] = (SINT32 *)buff[i];
			ratio_tmp[valid_num++] = ratio[i];
			ratio_all += ratio[i];
		}
	}
	if (valid_num > 0 && ratio_all > 0)
	{
		if (VModeSelfArg->dwidth == 16)
		{
			for (UINT32 j = 0; j < VModeSelfArg->pack_len / 2; j++)
			{
				buff16_tmp[0][j] = buff16_tmp[0][j] / ratio_all * ratio_tmp[0];
				for (UINT8 k = 1; k < valid_num; k++)
				{
					buff16_tmp[0][j] += buff16_tmp[k][j] / ratio_all * ratio_tmp[k];
				}
			}
		}
		else
		{
			for (UINT32 j = 0; j < VModeSelfArg->pack_len / 4; j++)
			{
				buff32_tmp[0][j] = buff32_tmp[0][j] / ratio_all * ratio_tmp[0];
				for (UINT8 k = 1; k < valid_num; k++)
				{
					buff32_tmp[0][j] += buff32_tmp[k][j] / ratio_all * ratio_tmp[k];
				}
			}
		}
		for (UINT8 k = 1; k < valid_num; k++)
		{
			vfree(buff16_tmp[k]);
		}
		return (UINT8 *)buff16_tmp[0];
	}
	return NULL;
}

static void Mix_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	UINT8 *buff[VMODE_MIX_MAX_SUPPORT] = {0}, *mix_buff;

	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		VMODE_QueReceive_Merge(self->receive, buff, ARRAY_SIZE(buff), VCORMAX_DELAY);

		mix_buff = mix_handle(self, buff, ARRAY_SIZE(buff));
		VCORQueSend(self->send, 0, mix_buff, 0, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
			vfree(mix_buff);
		}
	}
	VCOREND();
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

UINT8 VMODE_Mix_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	self->tcb = VCORTaskCreate((VCORTaskFunc_t)Mix_Task, self, 7);
	if (self->tcb == NULL)
		goto vmode_init_err;

	self->state = 1;
	VMODE_DEBUG("%s Mount\r\n", __FUNC__);
	return 0;
vmode_init_err:
	if (self->tcb)
		vcortaskdel(self->tcb);
	return 1;
}

UINT8 VMODE_Mix_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_Mix_Operate(VMODECorHandle *self, VMODEMixCMD_e cmd, void *arg)
{
	switch (cmd)
	{
		case VMODEMixCMD_ArgSet:
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

INIT_MODE_MOUNT(mix, vmode_mix_handle, NULL, VMODE_Mix_Mount, VMODE_Mix_Unmount, VMODE_Mix_Operate);
