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
	slave die merge

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_merge.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_MERGE]", __VA_ARGS__)

#define VModeSelfArg	((VMODEMergeConfig_t*)self->arg)
#define VModeSelfHandle	((VMODEMergeHandle_t*)self->handle)

static VMODEMergeConfig_t mode_config = {
	.sync_en = 1,
};
VMODECorHandle vmode_merge_handle[3] = {{.arg = &mode_config},};

static void Merge_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	UINT8 *buff[3] = {0};

	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		if (VModeSelfArg->sync_en)
		{
			VCORQueReceive(self[0].receive, NULL, &buff[0], VCORMAX_DELAY);
			VCORQueReceive(self[1].receive, NULL, &buff[1], 0);
			VCORQueReceive(self[2].receive, NULL, &buff[2], 0);
		}
		else
		{
			VCORWaitContent(vcorquereceive(vmode_merge_handle[0].receive, NULL, &buff[0]) == VRET_NO_ERROR ||
								vcorquereceive(vmode_merge_handle[1].receive, NULL, &buff[1]) == VRET_NO_ERROR ||
								vcorquereceive(vmode_merge_handle[2].receive, NULL, &buff[2]) == VRET_NO_ERROR,
							VCORMAX_DELAY);
		}

		for (UINT8 i = ARRAY_SIZE(buff); i > 0; i--)
		{
			if (buff[i - 1] != NULL)
			{
				ret = vcorquesend(self[0].send, i - 1, buff[i - 1], 0);
				if (ret != 0)
				{
					VMODE_DEBUG("%s send error\r\n", __FUNC__);
					vfree(buff[i - 1]);
				}
			}
		}
	}
	VCOREND();
	VMODE_Clean_Handle(self);
	VMODE_Clean_Handle(&self[1]);
	VMODE_Clean_Handle(&self[2]);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_Merge_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	self->tcb = VCORTaskCreate((VCORTaskFunc_t)Merge_Task, self, 6);
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

static UINT8 VMODE_Merge_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

static UINT8 VMODE_Merge_Operate(VMODECorHandle *self, UINT8 cmd, void *arg)
{
	switch ((VMODEMergeCMD_e)cmd)
	{
	case VMODEMergeCMD_Mount1:
		VMODE_Mount_AddMode(&self[1], ((VMODE_AddMountFormat_t *)arg)->p_name, ((VMODE_AddMountFormat_t *)arg)->depth);
		VMODE_DEBUG("%s Mount %s\r\n", __FUNC__, ((VMODE_AddMountFormat_t *)arg)->p_name);
		break;
	case VMODEMergeCMD_Mount2:
		break;
	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(merge, vmode_merge_handle, NULL, VMODE_Merge_Mount, VMODE_Merge_Unmount, VMODE_Merge_Operate);
