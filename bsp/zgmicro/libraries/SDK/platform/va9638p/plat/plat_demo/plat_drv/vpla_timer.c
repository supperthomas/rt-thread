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
        timer

Description:
        tmier Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_timer.h"
#include "vdrv_ckm.h"
#include "vdrv_timer.h"
#include "vdebug_errno.h"
   
VRET_VALUE VSYS_TIMER_Init(VDRVTMRSel_t timer, VDRVTMRConfig_t *tmr_config)
{   
    UINT32 value = 0;
    if(tmr_config == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR; 
    } 
    value = tmr_config->Period;
    if(timer == VSYS_TMR_SCLK_quarus){
            value /= 250;
            value = value?value:1;
    }
    VTMR_Config ((VTMR_SEL)timer, value, tmr_config->AutoReload);
    if(tmr_config->EnableInt){
        VTMR_RegisterISRCallback((VTMR_SEL)timer, (VDRV_TMR_CALLBACK)tmr_config->TMRCallback);
        VTMR_EnableInt ((VTMR_SEL)timer);
    }else{
        VTMR_DisableInt ((VTMR_SEL)timer);
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_TIMER_Uninit(VDRVTMRSel_t timer)
{
    VTMR_DisableInt ((VTMR_SEL)timer);
    VTMR_Stop ((VTMR_SEL)timer);
    VTMR_RegisterISRCallback((VTMR_SEL)timer, NULL);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_TIMER_Start(VDRVTMRSel_t timer)
{
    VTMR_Start ((VTMR_SEL)timer);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_TIMER_Stop(VDRVTMRSel_t timer)
{
    VTMR_Stop ((VTMR_SEL)timer);
    return VRET_NO_ERROR;
}

UINT32 VSYS_TIMER_GetStatus(VDRVTMRSel_t timer,VDRVTMRStatus_t status)
{
    UINT32 ret = 0;
    switch (status)
    {
        case VSYS_TMR_CntValue_Status:
            ret = VTMR_GetCntValue((VTMR_SEL)timer);
            break;
        case VSYS_TMR_IsRunning_Status:
            ret = VTMR_IsRunning((VTMR_SEL)timer);
            break;
        case VSYS_TMR_Interrupt_Status:
            ret = VTMR_GetInterruptStatus((VTMR_SEL)timer);
            break;        
        default:
            __VERRNO_STORE(VERRNO_TIMER_INVALID);
            break;  
    }
    return ret;
}



