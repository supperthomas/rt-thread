/*------------------------------------------------------------------------------
					COPYRIGHT (C) 2018, Zgmicro Corporation
							  ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
		ckm

Description:
		ckm Driver
Revision History:
		Date                  Author                   Description
	2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_ckm.h"
#include "vdrv_ckm.h"

extern VCKM_CONFIG g_VCKM_Config;

VDRVCKMConfig_t g_VSYS_VCKM_Config = {
	CKM_DEFAULT_SYSCLK_FREQ_HIGH,
	CKM_DEFAULT_APB_DIV,
	0,
	0,
	0, // CKM_LPO_CLK,//not use in 9635
	/* high system clock frequency */
	CKM_DEFAULT_SYSCLK_FREQ_HIGH,
	/* crystal frequency */
	CKM_DEFAULT_XCLK_FREQ,
	/* DPLL config */
	CKM_DEFAULT_DPLL_R11,
	CKM_DEFAULT_DPLL_R15,
	CKM_DEFAULT_DPLL_R16,
	CKM_DEFAULT_DPLL_REF_SEL,
	/* clock inverter selections */
	CKM_DEFAULT_48M_INV_SEL,
	CKM_DEFAULT_24M_INV_SEL,
	/* apb divider for high frequecy */
	CKM_DEFAULT_APB_DIV_HIGH};

VRET_VALUE VSYS_CKM_Init(void)
{
	vmemcpy(&g_VCKM_Config,&g_VSYS_VCKM_Config,sizeof(g_VSYS_VCKM_Config));
	VCKM_SetSysClockFreq(0, (VCKM_CONFIG *)&g_VSYS_VCKM_Config);
	return VRET_NO_ERROR;
}

VRET_VALUE VSYS_CKM_Uninit(void)
{
	VCKM_Init(CKM_DEFAULT_XCLK_FREQ);
	return VRET_NO_ERROR;
}

UINT32 VSYS_CKM_GetParam(VDRVCKMInfo_t info)
{
	UINT32 ret = 0;

	switch (info)
	{
	case VSYS_SysClockFreq:
		ret = VCKM_GetSysClockFreq();
		break;
	case VSYS_ApbClockFreq:
		ret = VCKM_GetAPBClockFreq();
		break;
	case VSYS_I2cClockFreq:
		ret = VCKM_GetI2CClockFreq();
		break;
	case VSYS_TimerClockFreq:
		ret = VCKM_GetTimerClockFreq();
		break;
	case VSYS_UartClockFreq:
		ret = VCKM_GetUartClockFreq();
		break;
	default:
		break;
	}
	return ret;
}

VRET_VALUE VSYS_CKM_SetParam(VDRVCKMInfo_t info, /*VDRVCKMParam_t*/UINT32 val)
{
	VRET_VALUE ret = VRET_NO_ERROR;
	switch (info)
	{
	case VSYS_SysClockFreq:
		g_VCKM_Config.sys_clock_freq = val;
		VCKM_SetSysClockFreq(0, &g_VCKM_Config);
		break;
	case VSYS_ApbClockFreq_div:
		g_VCKM_Config.apb_divider = val;
		VCKM_SetSysClockFreq(0, &g_VCKM_Config);
		break;
	case VSYS_SwitchClock:
		switch ((VDRVCKMSwitch_t)val)
		{
		case VSYS_SwitchNormalClock:
			if (g_VCKM_Config.sys_clock_freq_high > g_VCKM_Config.sys_clock_freq)
			{
				VCKM_SetSysClockFreq(0, &g_VCKM_Config);
			}
			break;
		case VSYS_SwitchHighClock:
			if (g_VCKM_Config.sys_clock_freq_high > g_VCKM_Config.sys_clock_freq)
			{
				VCKM_SetSysClockFreq(1, &g_VCKM_Config);
			}
			break;
		case VSYS_SwitchSleepClock:
			VCKM_SetSysClockForSleepDeep();
			break;
		default:
			ret = VRET_ERROR;
			break;
		}
		break;
	case VSYS_TopClockEnable:
		VCKM_SetCorprocessor(CKM_SYSCTRL_TOP, val);
		break;
	case VSYS_BTClockEnable:
		VCKM_SetCorprocessor(CKM_SYSCTRL_BT, val);
		break;
	case VSYS_DSPClockEnable:
		VCKM_SetCorprocessor(CKM_SYSCTRL_DSP, val);
		break;
	case VSYS_CKMSetProtect:
		VCKM_SetProtect((UINT16)val);
		break;
	case VSYS_DspClockFreq:
		VCKM_DSPClockSelect(val);
		break;
	default:
		ret = VRET_ERROR;
		break;
	}

	return ret;
}
