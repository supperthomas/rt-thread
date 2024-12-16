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
#include "vpla_box.h"
#include "vcoroutine.h"
#include "dsp_host.h"
#include "dsp_path.h"
#include "dsp_alg_gain.h"

#undef VDSP_DEBUG
#define VDSP_DEBUG(...) vcom_printf("[VDSP_HOST]"__VA_ARGS__)

VCORQue_t hostList = NULL;

static void MsgBox_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 *buff = NULL;
	UINT16 event = 0;
	UINT16 id = 0;
	VCORSTART();
	while (1)
	{
		VCORWaitContent((buff = (UINT8 *)vbox_queue_recv(&id)) != NULL || (event = VBOX_EvtGet(0x8000)) != 0, VCORMAX_DELAY);
		{
			if (event != 0)
			{
				VBOX_Uninit();
				VBOX_Init();
				continue;
			}
			if (id == 0xFFFF)
			{
				VCORQueSend(hostList, VDSPQue_ID_COMM, buff, 0, ret);
				if (ret != 0)
				{
					vfree(buff);
					VDSP_DEBUG("hostList send error\r\n");
				}
			}
			else if (id >= VDSP_ID_PATH_DATA_BASE && (id - VDSP_ID_PATH_DATA_BASE) < DSP_PATH_MAX_SUPPORT) // path data
			{
				VCORQueSend(vdsp_path_alg[id].p_corque, 0, buff, 0, ret);
				if (ret != 0)
				{
					vfree(buff);
					VDSP_DEBUG("path%d send error\r\n", id);
				}
			}
			else if (id >= VDSP_ID_PATH_CONFIG_BASE && (id - VDSP_ID_PATH_CONFIG_BASE) < DSP_PATH_MAX_SUPPORT) // path cfg
			{
				VCORQueSend(hostList, VDSPQue_ID_PathCfg, buff, 0, ret);
				if (ret != 0)
				{
					vfree(buff);
					VDSP_DEBUG("path%d send error\r\n", id);
				}
			}
			else
			{
				vfree(buff);
				VDSP_DEBUG("id %d unused error\r\n", id);
			}
		}
	}
	VCOREND();
}

static void Host_Task(void *p_arg)
{
	SINT32 box_ret;
	UINT8 qid = 0, path;
	UINT8 *buff = NULL;
	VDSPCmd_Format_t *cmd_data;
	VCORSTART();
	while (1)
	{
		VCORQueReceive(hostList, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			if (VDSP_ID_PATH_RETDATA_BASE - qid < DSP_PATH_MAX_SUPPORT)
			{
				path = VDSP_ID_PATH_RETDATA_BASE - qid;

				box_ret = vbox_queue_send(path + VDSP_ID_PATH_DATA_BASE, buff, vdsp_path_alg[path].packout_len);
				if (box_ret != 0)
				{
					vfree(buff);
					VDSP_DEBUG("vbox_queue_send fail\r\n");
				}
				continue;
			}
			cmd_data = (VDSPCmd_Format_t *)buff;
			switch ((VDSPQue_ID_e)qid)
			{
			case VDSPQue_ID_COMM:
				switch (cmd_data->u.comm.cmd)
				{
				case VDSPQue_COMM_SetCycl:
					VCORTimeCycles(cmd_data->u.comm.buff[0], 0xFFFFFFFF);
					VDSP_DEBUG("vcortimecycles: %d\r\n", cmd_data->u.comm.buff[0]);
					break;
				default:
					break;
				}
				break;
			case VDSPQue_ID_PathCfg:
				break;
			default:
				break;
			}
			vfree(buff);
		}
	}
	VCOREND();
}

UINT8 VDsp_Host_Init(void)
{
	hostList = VCORQueCreate(10);
	VCORTaskCreate(Host_Task, NULL, 0);
	return 0;
}

/*----------------------------------demo-------------------------*/
VDSP_ALG_GAIN_Handle *gain;
VDSP_ALG_GAIN_Handle *gain1;
void VDsp_Host_Demo_Timer(void *arg)
{
	UINT32 ret;
	static UINT8 cnt = 0;
	UINT8 buff[16] = {0x20, 0x40, 0x60, 0x80, 0xA0};
	UINT8 *p_data = vmalloc(vdsp_path_alg[1].packin_len);
	vmemcpy(p_data, buff, sizeof(buff));
	ret = vcorquesend(vdsp_path_alg[1].p_corque, 0, p_data, 0);
	if (ret != 0)
	{
		vfree(p_data);
		VDSP_DEBUG("VDsp_Host_Demo_Timer send error\r\n");
	}
	if(cnt++ == 5)
	{
		VDsp_Path_Alg_Minus((VDSP_ALG_HANDLE_t*)gain);
	}
	if(cnt == 10)
	{
		VDsp_Path_Uninit(1);
	}
}
void VDsp_Host_Demo(void)
{
	VCORTmr_t demo_timer;
	VDSPQue_COMM_t cmd_data;
	
	VDsp_Path_Init(1, 4, 512, 512);
	gain = VDsp_ALG_Gain_Create(24, 4);
	gain1 = VDsp_ALG_Gain_Create(16, 2);
	VDsp_Path_Alg_Add(1, (VDSP_ALG_HANDLE_t *)gain);
	VDsp_Path_Alg_Add(1, (VDSP_ALG_HANDLE_t *)gain1);

	cmd_data.cmd = VDSPQue_COMM_SetCycl;
	cmd_data.buff[0] = 128000000;
	vcorquesend(hostList, VDSPQue_ID_COMM, (UINT8*)&cmd_data, sizeof(cmd_data));
	VCORTimeCycles(128000000, 0xFFFFFFFF);
	demo_timer = VCORTmrCreate(2000, 2000, VDsp_Host_Demo_Timer, NULL);
	VCORTmrStart(demo_timer);
}
