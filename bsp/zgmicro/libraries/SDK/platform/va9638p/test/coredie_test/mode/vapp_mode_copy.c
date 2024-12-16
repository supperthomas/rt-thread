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
	slave die copy

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_copy.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_COPY]", __VA_ARGS__)

#define VModeSelfArg	((VMODECopyConfig_t*)self->arg)
#define VModeSelfHandle	((VMODECopyHandle_t*)self->handle)

static VMODECopyConfig_t mode_config = {
	.pack_len = AUDIO_PACK_LEN,
	.backup = 2};
VMODECorHandle vmode_copy_handle = {.arg = &mode_config};

static void Copy_Task(VMODECorHandle *self)
{
	UINT8 id, *buff;
	UINT32 ret = 0;

	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		VCORQueReceive(self->receive, &id, &buff, VCORMAX_DELAY);

		for (UINT8 i = 1; i < VModeSelfArg->backup; i++)
		{
			ret = vcorquesend(self->send, i, buff, VModeSelfArg->pack_len);
			if (ret != 0)
			{
				VMODE_DEBUG("%s send error\r\n", __FUNC__);
			}
		}
		VCORQueSend(self->send, 0, buff, 0, ret);
		if (ret != 0)
		{
			VMODE_DEBUG("%s send error\r\n", __FUNC__);
			vfree(buff);
		}
	}
	VCOREND();
	VMODE_Clean_Handle(self);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

UINT8 VMODE_Copy_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	self->tcb = VCORTaskCreate((VCORTaskFunc_t)Copy_Task, self, 7);
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

UINT8 VMODE_Copy_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

UINT8 VMODE_Copy_Operate(VMODECorHandle *self, UINT8 cmd, void *arg)
{
	switch (cmd)
	{

	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(copy, vmode_copy_handle, NULL, VMODE_Copy_Mount, VMODE_Copy_Unmount, VMODE_Copy_Operate);
