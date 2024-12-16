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
    SYS_trace

Description:
     System trace.

Revision History:
     Date                  Author                   Description
    2022.12.20             cxt                  initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_trace.h"
#include "vstdos.h"
#include "vpla_gpio.h"
#include "vdrv_gpio.h"
#include "vsys_mem_info.h"
#include "SEGGER_SYSVIEW_Wrap.h"

#define TRACE_GPIO(x) (x)
#define TRACE_GPIOL(x) (x+32)
static UINT8 trace_gpio[] = {TRACE_GPIO(1),TRACE_GPIO(2),TRACE_GPIO(3),TRACE_GPIO(4),TRACE_GPIO(7),TRACE_GPIO(8)};

VRET_VALUE VSYS_DebugPort_Enable(BOOL en)
{
    if (en)
    {
        VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_BT_SW, GPIO_MUX_FUNC_BT_SW);
        VGPIO_SetLMux2Function(GPIO_MUX_FUNC_BT_SW, GPIO_MUX_FUNC_BT_SW);
    }
    else
    {
        VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_BT_SW, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMux2Function(GPIO_MUX_FUNC_BT_SW, GPIO_MUX_FUNC_NONE);
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_TracePort_Enable(BOOL en)
{
    if (en)
    {
        VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_BT_TRACEDATDACLK, GPIO_MUX_FUNC_BT_TRACEDATDACLK);
        VGPIO_SetLMux2Function(GPIO_MUX_FUNC_BT_TRACEDATDACLK, GPIO_MUX_FUNC_BT_TRACEDATDACLK);
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_BT_TRACEDATA, GPIO_MUX_FUNC_BT_TRACEDATA);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_BT_TRACEDATA, GPIO_MUX_FUNC_BT_TRACEDATA);
    }
    else
    {
        VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_BT_TRACEDATDACLK, GPIO_MUX_FUNC_NONE);
        VGPIO_SetLMux2Function(GPIO_MUX_FUNC_BT_TRACEDATDACLK, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_BT_TRACEDATA, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_BT_TRACEDATA, GPIO_MUX_FUNC_NONE);
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_SYSVIEW_MarkInit(UINT32 id, char *name)
{
#if VPLA_TRACE_MARK_EN
#if SYSVIEW_CFG_TRACE_MARK_EN
    SEGGER_SYSVIEW_NameMarker(id, name);
#else
	if(sizeof(trace_gpio) > id)
		VSYS_GPIO_Init(VSYS_GPIO_PIN_X(trace_gpio[id]), VSYS_GPIO_MODE_OUTPUT, VSYS_GPIO_OUTPUT_LOW_STRENGTH);
    else
        return VRET_ERROR;
#endif
#endif
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_SYSVIEW_MarkStart(UINT32 id)
{
#if VPLA_TRACE_MARK_EN
#if SYSVIEW_CFG_TRACE_MARK_EN
    SEGGER_SYSVIEW_MarkStart(id);
#else
	if(sizeof(trace_gpio) > id)
		VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_X(trace_gpio[id]), TRUE);
    else
        return VRET_ERROR;
#endif
#endif
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_SYSVIEW_MarkStop(UINT32 id)
{
#if VPLA_TRACE_MARK_EN
#if SYSVIEW_CFG_TRACE_MARK_EN
    SEGGER_SYSVIEW_MarkStop(id);
#else
	if(sizeof(trace_gpio) > id)
		VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_X(trace_gpio[id]), FALSE);
    else
        return VRET_ERROR;
#endif
#endif
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_SYSVIEW_Mark(UINT32 id)
{
#if VPLA_TRACE_MARK_EN
#if SYSVIEW_CFG_TRACE_MARK_EN
    SEGGER_SYSVIEW_Mark(id);
#else
	if(sizeof(trace_gpio) > id)
	{
		if (VSYS_GPIO_GetPinInput(VSYS_GPIO_PIN_X(trace_gpio[id])))
		{
			VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_X(trace_gpio[id]), FALSE);
		}
		else
		{
			VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_X(trace_gpio[id]), TRUE);
		}
	}
    else
        return VRET_ERROR;
#endif
#endif
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_ResInfo_Update(UINT32 *p_mem, BOOL usage_updata)
{
	static UINT32 *g_VSYS_Pool_INFO = NULL;
	
	if (g_VSYS_Pool_INFO == NULL)
	{
		#ifdef CORE_SEL_MCUTOP
		g_VSYS_Pool_INFO = VShareMemInit((SINT8*)"TPif", 32*4, SHAREMEM_TOP_BIT);
		#else
		g_VSYS_Pool_INFO = VShareMemInit((SINT8*)"BTif", 32*4, SHAREMEM_BT_BIT);
		#endif
		if (g_VSYS_Pool_INFO == NULL) return VRET_ERROR;
	}
	if (p_mem != NULL)
	{
		if (g_VSYS_Pool_INFO[4] != MAKE_U32("INFO"))
		{
			g_VSYS_Pool_INFO[4] = MAKE_U32("INFO");
			g_VSYS_Pool_INFO[1] = VSYS_MEM_AllSize((UINT8*)p_mem);
		}
		g_VSYS_Pool_INFO[0] = p_mem[0];
		g_VSYS_Pool_INFO[2] = p_mem[2];
		if(g_VSYS_Pool_INFO[3] < p_mem[2])
			g_VSYS_Pool_INFO[3] = p_mem[2];
		vmemcpy(&g_VSYS_Pool_INFO[5],p_mem+5,14*4);
	}
	#if VSYS_OS_MIPS_SUPPORTED
	if (usage_updata)
	{
		g_VSYS_Pool_INFO[19] = MAKE_U32("MIPS"); //CPUUsage
		g_VSYS_Pool_INFO[20] = VOSGetCPUUsage(); // CPUUsage
		if(g_VSYS_Pool_INFO[21] < g_VSYS_Pool_INFO[20])
			g_VSYS_Pool_INFO[21] = g_VSYS_Pool_INFO[20];
	}
	#endif
    return VRET_NO_ERROR;
}
	

