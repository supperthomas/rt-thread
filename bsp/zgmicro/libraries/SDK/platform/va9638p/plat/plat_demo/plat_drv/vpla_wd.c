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
        wd

Description:
        wd Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_wd.h"
#include "vdrv_wd.h"
#include "vdrv_ipc.h"
#include "vreg_def.h"
#include "vdebug_errno.h"

#define VDRV_WDT_MAGIC_CODE (0x00AA6688)
static BOOL VWDT_SW_CheckTimeout(void);

static struct{
    UINT8 init;
    VDRVWDTConfig_t cfg;
    wd_info_t *wd;
}g_wdt_info[VDRVWDT_LIMIT] = {0};
static BOOL VWDT_SW_IsTimeout(void);

VRET_VALUE VSYS_WDT_Init(VDRVWDTSel_t wdt, VDRVWDTConfig_t *wdt_config)
{
    if(wdt_config == NULL || wdt >= VDRVWDT_LIMIT)
    {
        __VERRNO_STORE(VERRNO_WDT_INVALID);
        return VRET_ERROR; 
    }
    vmemcpy(&g_wdt_info[wdt].cfg, wdt_config, sizeof(VDRVWDTConfig_t));
    switch (wdt)
    {
    case VDRVWDT_HW:
        g_wdt_info[wdt].init = 1;
        break;
    case VDRVWDT_SW:
        g_wdt_info[wdt].wd = VIPC_Get_WD();
        if (g_wdt_info[wdt].wd != NULL)
        {
            g_wdt_info[wdt].init = 1;
            g_wdt_info[wdt].wd->magic = VDRV_WDT_MAGIC_CODE|(wdt_config->Mode<<24);
            g_wdt_info[wdt].wd->period = g_wdt_info[wdt].cfg.timeout_ms;
            g_wdt_info[wdt].wd->wd_count = 0;
            g_wdt_info[wdt].wd->is_enable = FALSE;
        }
        break;
    default:
        break;
    }

    return VRET_NO_ERROR;
}


VRET_VALUE VSYS_WDT_Uninit(VDRVWDTSel_t wdt)
{
    if(wdt >= VDRVWDT_LIMIT)
    {
        __VERRNO_STORE(VERRNO_WDT_INVALID);
        return VRET_ERROR; 
    }
    g_wdt_info[wdt].init = 0;
    return VRET_NO_ERROR;
}


VRET_VALUE VSYS_WDT_Enable(VDRVWDTSel_t wdt)
{
    if(wdt >= VDRVWDT_LIMIT)
    {
        __VERRNO_STORE(VERRNO_WDT_INVALID);
        return VRET_ERROR; 
    }
    if (g_wdt_info[wdt].init == 1)
    {
        switch (wdt)
        {
        case VDRVWDT_HW:
            VWD_Enable(g_wdt_info[wdt].cfg.Mode, g_wdt_info[wdt].cfg.timeout_ms);
            break;
        case VDRVWDT_SW:
            g_wdt_info[wdt].wd->is_enable = TRUE;   
            g_wdt_info[wdt].wd->wd_count = vtimeusget();
            break;
        default:
            break;
        }
    }

    return VRET_NO_ERROR;
}


VRET_VALUE VSYS_WDT_Disable(VDRVWDTSel_t wdt)
{
    if(wdt >= VDRVWDT_LIMIT)
    {
        __VERRNO_STORE(VERRNO_WDT_INVALID);
        return VRET_ERROR; 
    }
    if (g_wdt_info[wdt].init == 1)
    {
        switch (wdt)
        {
        case VDRVWDT_HW:
            VWD_Disable();
            break;
        case VDRVWDT_SW:
            g_wdt_info[wdt].wd->is_enable = FALSE;       
            break;
        default:
            break;
        }
    }
    return VRET_NO_ERROR;
}


VRET_VALUE VSYS_WDT_Feed(VDRVWDTSel_t wdt)
{
    if(wdt >= VDRVWDT_LIMIT)
    {
        __VERRNO_STORE(VERRNO_WDT_INVALID);
        return VRET_ERROR; 
    }
    VWDT_SW_CheckTimeout();
    if (g_wdt_info[wdt].init == 1)
    {
        switch (wdt)
        {
        case VDRVWDT_HW:
            VWD_Feed();
            break;
        case VDRVWDT_SW:
            g_wdt_info[wdt].wd->wd_count = vtimeusget();    
            break;
        default:
            break;
        }
    }

    return VRET_NO_ERROR;
}


UINT32 VSYS_WDT_GetStatus(VDRVWDTSel_t wdt,VDRVWDTStatus_t status)
{
    UINT32 ret = 0;

    switch (wdt)
    {
    case VDRVWDT_HW:
        if (status == VSYS_WDT_CntValue_Status)
            ret = VWD_GetCount()/(VWD_CLK_IN / 1000);
        else if (status == VSYS_WDT_IsRunning_Status)
            ret = VWD_GetStatus();
        else if (status == VSYS_WDT_Timeout_Status)
            ret = GET_REG_BIT2 ( REG_TIMER_TSTA, REG_TIMER_TSTA_WD_STA_BIT );
        break;
    case VDRVWDT_SW:
        if (status == VSYS_WDT_CntValue_Status)
            ret = g_wdt_info[wdt].wd->wd_count;
        else if (status == VSYS_WDT_IsRunning_Status)
            ret = g_wdt_info[wdt].wd->is_enable;
        else if (status == VSYS_WDT_Timeout_Status)
            ret = g_wdt_info[wdt].wd->wd_count >= g_wdt_info[wdt].wd->period;
        break;
    default:
        break;
    }

    return ret;
}

static BOOL VWDT_SW_CheckTimeout(void)
{
    if (g_wdt_info[VDRVWDT_SW].wd->magic&0x00FFFFFF == VDRV_WDT_MAGIC_CODE)
    {
        if (vtimeusinterval(g_wdt_info[VDRVWDT_SW].wd->wd_count, vtimeusget())/1000 >= g_wdt_info[VDRVWDT_SW].wd->period)    
        {
            if (g_wdt_info[VDRVWDT_HW].init == 1)
            {
                VWD_Disable();
                vdelayus(800);
                VWD_Enable(g_wdt_info[VDRVWDT_SW].wd->magic>>24, 0);
                while(1);
            }
            else
            {
                if (g_wdt_info[VDRVWDT_SW].cfg.Mode == VDRVWDT_MODE_RESET)
                    VCPU_Reset(CORE_RESET_SYSTEM);
                else if (g_wdt_info[VDRVWDT_SW].cfg.WDTCallback != NULL)
                    g_wdt_info[VDRVWDT_SW].cfg.WDTCallback(VDRVWDT_SW);
                while(1);
            }
        }
    }
    return FALSE;
}
