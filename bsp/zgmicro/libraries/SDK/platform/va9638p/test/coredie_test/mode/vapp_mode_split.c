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
	slave die split

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vcoroutine.h"
#include "vapp_mode_split.h"

#undef VMODE_DEBUG
#define VMODE_DEBUG(...) DEBUG_PRINTF("[VMODE_SPLIT]", __VA_ARGS__)

#define VModeSelfArg	((VMODESplitConfig_t*)self->arg)
#define VModeSelfHandle	((VMODESplitHandle_t*)self->handle)

static VMODESplitConfig_t mode_config = {
	.sync_en = 1,
};
VMODECorHandle vmode_split_handle[3] = {{.arg = &mode_config},};

static void Split_Task(VMODECorHandle *self)
{
	UINT32 ret = 0;
	UINT8 *split_buff, id;

	VCORSTART();
	VMODE_DEBUG("%s init\r\n", __FUNC__);
	while (1)
	{
		VCORQueReceive(self->receive, &id, &split_buff, VCORMAX_DELAY);

		if (id < 3)
		{
			ret = vcorquesend(self[id].send, 0, split_buff, 0);
			if (ret != 0)
			{
				if(self[id].send != NULL)
					VMODE_DEBUG("%s%d send error\r\n", __FUNC__,id);
				vfree(split_buff);
			}
		}
		VCORSched();
	}
	VCOREND();
	VMODE_Clean_Handle(self);
	VMODE_Clean_Handle(&self[1]);
	VMODE_Clean_Handle(&self[2]);
	VMODE_DEBUG("%s uninit\r\n", __FUNC__);
}

static UINT8 VMODE_Split_Mount(VMODECorHandle *self)
{
	if (self->state != 0)
		return 1;

	self->tcb = VCORTaskCreate((VCORTaskFunc_t)Split_Task, self, 8);
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

static UINT8 VMODE_Split_Unmount(VMODECorHandle *self)
{
	if (self->state == 0)
		return 1;

	vcortaskdel(self->tcb);
	self->state = 0;
	VMODE_DEBUG("%s Unmount\r\n", __FUNC__);
	return 0;
}

static UINT8 VMODE_Split_Operate(VMODECorHandle *self, UINT8 cmd, void *arg)
{
	switch ((VMODESplitCMD_e)cmd)
	{
	case VMODESplitCMD_INVMount1:
		VMODE_Mount_INVMode(((VMODE_AddMountFormat_t *)arg)->p_name, &self[1], ((VMODE_AddMountFormat_t *)arg)->depth);
		break;
	case VMODESplitCMD_INVMount2:
		VMODE_Mount_INVMode(((VMODE_AddMountFormat_t *)arg)->p_name, &self[2], ((VMODE_AddMountFormat_t *)arg)->depth);
		break;
	default:
		break;
	}
	return 0;
}

INIT_MODE_MOUNT(split, vmode_split_handle, NULL, VMODE_Split_Mount, VMODE_Split_Unmount, VMODE_Split_Operate);
