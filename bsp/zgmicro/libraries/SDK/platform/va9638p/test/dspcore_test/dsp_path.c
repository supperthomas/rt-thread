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
#include "dsp_host.h"

#undef VDSP_DEBUG
#define VDSP_DEBUG(...) vcom_printf("[VDSP_PATH]"__VA_ARGS__)

extern VCORQue_t hostList;

VDSP_PATH_Algorit_t vdsp_path_alg[DSP_PATH_MAX_SUPPORT] = {0};

static void VDspProcessing_Task(UINT8 index)
{
	UINT32 ret = 0;
	UINT8 qid = 0;
	UINT8 *buff = NULL;
	VDSP_ALG_HANDLE_t *handle;

	if (index >= DSP_PATH_MAX_SUPPORT)
	{
		VDSP_DEBUG("index%d beyond range\r\n", index);
		return;
	}

	VCORSTART();
	for (handle = vdsp_path_alg[index].head; handle != NULL; handle = handle->next)
	{
		if (handle->init != NULL)
			handle->init(handle);
	}
	while (1)
	{
		VCORQueReceive(vdsp_path_alg[index].p_corque, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			for (handle = vdsp_path_alg[index].head; handle != NULL; handle = handle->next)
			{
				if (handle->handling != NULL)
					ret = handle->handling(handle, buff, &buff);
				if (ret != 0)
				{
					VDSP_DEBUG("path%d alg error\r\n", index);
					break;
				}
			}
			VCORQueSend(hostList, VDSP_ID_PATH_RETDATA_BASE - index, buff, 0, ret);
			if (ret != 0)
			{
				vfree(buff);
				VDSP_DEBUG("path%d send error\r\n", index);
			}
		}
	}
	VCOREND();
	for (handle = vdsp_path_alg[index].head; handle != NULL; handle = handle->next)
	{
		if (handle->uninit != NULL)
			handle->uninit(handle);
	}
}

UINT32 VDsp_Path_Init(UINT8 path, UINT8 depth, UINT32 packin_len, UINT32 packout_len)
{
	VCORTcb_t tcb;
	VCORQue_t p_corque = NULL;
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb != NULL)
	{
		return 1;
	}

	if (depth > 0)
	{
		p_corque = VCORQueCreate(depth);
		if (p_corque == NULL)
		{
			return 2;
		}
	}
	tcb = VCORTaskCreate((VCORTaskFunc_t)VDspProcessing_Task, (void *)path, path);
	if (tcb == NULL)
	{
		VCORQueDel(p_corque);
		return 3;
	}
	vdsp_path_alg[path].tcb = tcb;
	vdsp_path_alg[path].p_corque = p_corque;
	vdsp_path_alg[path].packin_len = packin_len;
	vdsp_path_alg[path].packout_len = packout_len;
	vdsp_path_alg[path].head = NULL;
	vdsp_path_alg[path].tail = NULL;

	return 0;
}

UINT32 VDsp_Path_Uninit(UINT8 path)
{
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb == NULL)
	{
		return 1;
	}
	if (vdsp_path_alg[path].p_corque != NULL)
	{
		VCORQueDel(vdsp_path_alg[path].p_corque);
		vdsp_path_alg[path].p_corque = NULL;
	}
	vcortaskdel(vdsp_path_alg[path].tcb);
	vdsp_path_alg[path].tcb = NULL;

	return 0;
}

UINT32 VDsp_Path_Oper(UINT8 path, SINT8 *alg_name, void *arg)
{
	UINT32 ret = 0;
	VDSP_ALG_HANDLE_t *handle;
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb == NULL)
	{
		return 1;
	}
	for (handle = vdsp_path_alg[path].head; handle != NULL; handle = handle->next)
	{
		if (vstrncmp(handle->name, alg_name, sizeof(handle->name)) == 0)
		{
			if (handle->oper != NULL)
				ret = handle->oper(handle, arg);
			break;
		}
	}
	return ret;
}

UINT32 VDsp_Path_Get_PackInLen(UINT8 path)
{
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb == NULL)
	{
		return 1;
	}
	return vdsp_path_alg[path].packin_len;
}

UINT32 VDsp_Path_Get_PackOutLen(UINT8 path)
{
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb == NULL)
	{
		return 1;
	}
	return vdsp_path_alg[path].packout_len;
}

UINT32 VDsp_Path_Alg_Add(UINT8 path, VDSP_ALG_HANDLE_t *handle)
{
	VCORTcb_t tcb;
	if (path >= ARRAY_SIZE(vdsp_path_alg) || vdsp_path_alg[path].tcb == NULL || handle == NULL)
	{
		return 1;
	}

	handle->next = NULL;
	handle->pre = vdsp_path_alg[path].tail;
	if (vdsp_path_alg[path].tail != NULL)
		vdsp_path_alg[path].tail->next = handle;
	vdsp_path_alg[path].tail = handle;
	if (vdsp_path_alg[path].head == NULL)
		vdsp_path_alg[path].head = handle;

	handle->path = &vdsp_path_alg[path];

	if (vdsp_path_alg[path].tcb->uxState != vcorINITIAL_STATE)
	{
		if (handle->init != NULL)
			handle->init(handle);
	}

	return 0;
}

UINT32 VDsp_Path_Alg_Minus(VDSP_ALG_HANDLE_t *handle)
{
	if(handle == NULL || handle->path == NULL){
		return 1;
	}

	if (handle->path->head == handle)
		handle->path->head = handle->next;
	if (handle->path->tail == handle)
		handle->path->tail = handle->pre;
	if (handle->pre != NULL)
	{
		handle->pre->next = handle->next;
	}
	if (handle->next != NULL)
	{
		handle->next->pre = handle->pre;
	}
	handle->pre = NULL;
	handle->next = NULL;
	
	if (handle->path != NULL && handle->path->tcb != NULL && handle->path->tcb->uxState != vcorDELETE_STATE)
	{
		if (handle->uninit != NULL)
			handle->uninit(handle);
	}

	return 0;
}

UINT32 VDsp_Path_Alg_GetPackIn(VDSP_ALG_HANDLE_t *handle)
{
	if (handle != NULL && handle->path != NULL)
	{
		return handle->path->packin_len;
	}

	return 0;
}

UINT32 VDsp_Path_Alg_GetPackOut(VDSP_ALG_HANDLE_t *handle)
{
	if (handle != NULL && handle->path != NULL)
	{
		return handle->path->packout_len;
	}

	return 0;
}

UINT8 VDsp_Path_Alg_GetIndex(VDSP_ALG_HANDLE_t *handle)
{
	UINT8 ret = 0xFF;
	if (handle != NULL && handle->path != NULL)
	{
		for (UINT8 index = 0; index < ARRAY_SIZE(vdsp_path_alg); index++)
		{
			if (vdsp_path_alg + index == handle->path)
			{
				ret = index;
				break;
			}
		}
	}
	return ret;
}
