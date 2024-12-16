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
	slave die host

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstddrv.h"
#include "vstdcore.h"
#include "vcoroutine.h"
#include "vapp_host.h"
#include "vapp_die.h"
#include "vmode_common.h"

#undef VAPP_HOST_DEBUG
#define VAPP_HOST_DEBUG(...) DEBUG_PRINTF("[VAPP_HOST]",__VA_ARGS__)

extern TAR_DIE_CALLBACK_TYPE tar_die_callback_handle[VAPP_Die_CMDREQ_LIST_LIMIT];

VCORQue_t ConfigList = NULL;

UINT8 VAPP_Mode_Mount(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret, ret_len = 0;
	VAPPDie_ModeMount_t *config = (VAPPDie_ModeMount_t *)arg;

	ret = VMODE_Mount_Mode(config->m_name, config->p_name, config->depth);
	if (len > 0)
	{
		buff[0] = ret;
		ret_len = 1;
	}
	return ret_len;
}

UINT8 VAPP_Mode_Unmount(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret, ret_len = 0;
	VAPPDie_ModeUnmount_t *config = (VAPPDie_ModeUnmount_t *)arg;

	ret = VMODE_Unmount_Mode(config->m_name);
	if (len > 0)
	{
		buff[0] = ret;
		ret_len = 1;
	}
	return ret_len;
}

UINT8 VAPP_Mode_Oper(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret, ret_len = 0;
	VAPPDie_ModeOper_t *config = (VAPPDie_ModeOper_t *)arg;

	ret = VMODE_Mode_Operate(config->m_name, config->oper, &config->arg);
	if (len > 0)
	{
		buff[0] = ret;
		ret_len = 1;
	}
	return ret_len;
}

UINT8 VAPP_Mode_Display(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret_len = 0;
	VMODE_Display_Mode();
	if (len > 0)
	{
		buff[0] = 1;
		ret_len = 1;
	}
	return ret_len;
}

UINT8 VAPP_Mode_AdjustDPLL(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret_len = 0;
	SINT8 step = *(SINT8*)arg;
	UINT32 ret;
	
	void VCKM_Set_DPLL_Adjust_Enable(BOOL enable);
	UINT32  VCKM_Slow_Adjust_DPLL(SINT8 step);
	
	VCKM_Set_DPLL_Adjust_Enable(TRUE);
	ret = VCKM_Slow_Adjust_DPLL(step);
	//VAPP_HOST_DEBUG("Set_DPLL%d,value:0x%x\r\n",ret?step:0,ret);
	if (len >= 4)
	{
		*(UINT32*)buff = ret;
		ret_len = 4;
	}
	return ret_len;
}

UINT8 VAPP_Mode_StartAudio(UINT8 *arg, UINT8 *buff, UINT8 len)
{
	UINT8 ret_len = 0;
	
	VAPP_HOST_DEBUG("StartAudio init\r\n",__FUNC__);
	VSYS_GPIO_Init(VSYS_GPIOL_PIN_16, VSYS_GPIO_MODE_INPUT, VSYS_GPIO_INPUT_NORMAL);
	VSYS_GPIO_EnableInt(VSYS_GPIOL_PIN_16, VSYS_GPIO_INTTYPE_NORMAL_MODE);
	
	if (len > 0)
	{
		buff[0] = 0;
		ret_len = 1;
	}
	return ret_len;
}

static TAR_DIE_CALLBACK_TYPE tar_die_callback_handle[VAPP_Die_CMDREQ_LIST_LIMIT] = {
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_Mount,
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_Unmount,
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_Oper,
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_Display,
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_AdjustDPLL,
	(TAR_DIE_CALLBACK_TYPE)VAPP_Mode_StartAudio,
};

static void cmd_handle(UINT8 id, UINT8 *buff)
{
	UINT8 send_buff[16] = {0}, ret_len = 0;
	VAPPDieCmd_AllFormat_t *die_cmd = (VAPPDieCmd_AllFormat_t *)buff;

	if ((UINT8)(die_cmd->cmd) < ARRAY_SIZE(tar_die_callback_handle))
	{
		ret_len = tar_die_callback_handle[(UINT8)(die_cmd->cmd)](die_cmd->data, send_buff, sizeof(send_buff));
	}
	if(id == TRA_UART_CMD_REQ)
	{
		TRA_UART_Response(send_buff, ret_len);
	}
}

static void Config_Task(void *p_arg)
{
	UINT32 ret = 0;
	UINT8 qid = 0;
	size_t ret_len = 0;
	static UINT8 *buff = NULL;
	static size_t len = 0;

	VCORSTART();
	while (1)
	{
		VCORQueReceive(ConfigList, &qid, &buff, VCORMAX_DELAY);
		if (NULL != buff)
		{
			cmd_handle(qid, buff);
			vfree(buff);
		}
		VCORSched();
	}
	VCOREND();
}

static void Idle_Task(void *p_arg)
{
	VCORSTART();
	while (1)
	{
		VCORCRITICAL_ENTER();
		VSYS_SYSVIEW_MarkStart(0);
		if(!VCORISRCoRGetRun())
		{
			//VCPU_SleepNow(0,0);
		}
		VCORISRCoRGetRun();
		VSYS_SYSVIEW_MarkStop(0);
		VCORCRITICAL_EXIT();
		VCORSched();
	}
	VCOREND();
}

void VSYS_TesInfo_Update(UINT32 arg1, UINT32 arg2)
{
	static UINT32 *g_VSYS_Test_INFO = NULL;
	if (g_VSYS_Test_INFO == NULL)
	{
		g_VSYS_Test_INFO = VShareMemInit((SINT8*)"info", 32*4, SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (g_VSYS_Test_INFO == NULL) return;
	}
	g_VSYS_Test_INFO[0] = MAKE_U32("DAUP");
	g_VSYS_Test_INFO[1] = (1<<16)|1;
	g_VSYS_Test_INFO[2] = arg1;
	g_VSYS_Test_INFO[3] = arg2;
}
void Vapp_Host_DPLL_Tmr(void *arg)
{
	extern UINT32 iisin_isr_falg, iisin_isr_cnt, iisin_isr_start, iisin_isr_index, iisin_isr_aggre[5]; 
	extern UINT32 iisout_isr_falg, iisout_isr_cnt, iisout_isr_start, iisout_isr_index, iisout_isr_aggre[5];
	extern UINT32  VCKM_Slow_Adjust_DPLL(SINT8 step);
	
	static SINT32 total_step = 0;
	static UINT32 iisin_cnt = 0, iisout_cnt = 0;
	static UINT8 diff_cycle_index = 0, diff_average_index = 0;
	static SINT32 diff_cycle_array[5] = {0}, diff_average_array[5] = {0};
	static SINT32 cycle_list[9] =      {-500,   -200  , -50 ,   -20 ,   0  ,  20  ,  50   ,  200 ,  500};
	static SINT8 step_list[2][10] = {{+60 ,  +35,   +20 ,    +5,    +2,    -2 ,  -5 ,   -20 ,    -35 ,  -60},
									{ -30,   -20,   -15  ,   -3,    -1,    +1,   +3 ,   +15 ,    +20 ,  +30}};
	SINT8 step = 0, slope = 0;
	SINT32 index, diff_cycle = 0, diff_cnt = 0, max_slope = 0;
	SINT64 diff_aggre;
	UINT32 iisin_cycle_average, iisin_falg, iisin_start, iisin_aggre[2];
	UINT32 iisout_cycle_average, iisout_falg, iisout_start, iisout_aggre[2];
	
	VCORCRITICAL_ENTER();
	if (iisin_cnt == iisin_isr_cnt || iisout_cnt == iisout_isr_cnt)
	{
		VCORCRITICAL_EXIT();
		return;
	}
	iisin_cnt = iisin_isr_cnt;
	iisout_cnt = iisout_isr_cnt;
	diff_cnt = iisin_cnt - iisout_cnt;
	if (diff_cnt >= 0 && diff_cnt < ARRAY_SIZE(iisin_isr_aggre))
	{
		index = iisin_isr_index - diff_cnt;
		if (index < 0)
			index += ARRAY_SIZE(iisin_isr_aggre);
		diff_cycle = iisin_isr_aggre[index] - iisout_isr_aggre[iisout_isr_index];
	}
	else if (-diff_cnt < ARRAY_SIZE(iisin_isr_aggre))
	{
		index = iisout_isr_index + diff_cnt;
		if (index < 0)
			index += ARRAY_SIZE(iisout_isr_aggre);
		diff_cycle = iisin_isr_aggre[iisin_isr_index] - iisout_isr_aggre[index];
	}
	else if (diff_cnt > 0)
	{
		diff_cycle = -1000;
	}
	else
	{
		diff_cycle = 1000;
	}
	VCORCRITICAL_EXIT();
	if (diff_cycle >= 500 || diff_cycle <= -500)
		max_slope = 100;
	
	if(++diff_cycle_index == ARRAY_SIZE(diff_cycle_array))
		diff_cycle_index = 0;
	diff_cycle_array[diff_cycle_index] = diff_cycle;
	if(++diff_average_index == ARRAY_SIZE(diff_average_array))
		diff_average_index = 0;
	diff_aggre = diff_cycle_array[0] + diff_cycle_array[1] + diff_cycle_array[2] + diff_cycle_array[3] + diff_cycle_array[4];
	diff_average_array[diff_average_index] = diff_aggre/5;
	index = diff_average_index - 1;
	if(index < 0)
		index += ARRAY_SIZE(diff_average_array);
	if (diff_average_array[diff_average_index] > diff_average_array[index] + max_slope)
		slope = +2;
	else if (diff_average_array[diff_average_index] < diff_average_array[index] - max_slope)
		slope = -2;
	else if (diff_average_array[diff_average_index] > diff_average_array[index])
		slope = +1;
	else if (diff_average_array[diff_average_index] < diff_average_array[index])
		slope = -1;
	
	for(UINT8 i = 0;i < ARRAY_SIZE(cycle_list); i++)
	{
		if((diff_cycle > cycle_list[i]) && (i + 1 != ARRAY_SIZE(cycle_list)))
			continue;
		if((i + 1 == ARRAY_SIZE(cycle_list)) && (diff_cycle > cycle_list[i]))
			i++;
		
		if(step_list[0][i] > 0)
		{
			if (slope <= 0)
				step = step_list[0][i];
			else if (slope >= +2)
				step = step_list[1][i];
		}
		else if(step_list[0][i] < 0)
		{
			if (slope >= 0)
				step = step_list[0][i];
			else if (slope <= -2)
				step = step_list[1][i];
		}
		break;
	}

	total_step += step;
	VCKM_Slow_Adjust_DPLL(step);
	VSYS_TesInfo_Update(diff_cycle,total_step);
	return ;
}

UINT8 Vapp_Host_Init(void)
{
	VCORTmr_t DPLL_tmr = NULL;
	ConfigList = VCORQueCreate(20);
	VCORTaskCreate(Config_Task, NULL, 0xFE);
	VCORTaskCreate(Idle_Task, NULL, 0xFF);
	
	VMODE_Mount_Init();

//	DPLL_tmr = VCORTmrCreate(1*1000, 5, Vapp_Host_DPLL_Tmr, NULL);
//	VCORTmrStart(DPLL_tmr);
//	void VCKM_Set_DPLL_Adjust_Enable(BOOL enable);
//	VCKM_Set_DPLL_Adjust_Enable(TRUE);
	
	return 0;
}
