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
    SYS_TMR

Description:
     System timer.

Revision History:
     Date                  Author                   Description
     2009.07.21           chenjingen              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vdrv_ckm.h"
#include "vdrv_timer.h"
#include "vsys_timer.h"


VSYS_TMR_DATA *gp_VSYS_TMR_TimerList;
UINT32 g_VSYS_TMR_CountDownTicks;

#if (VSYS_TMR_PROFILE_EN > 0)
UINT32 g_VSYS_TMR_TotalTimers;
UINT32 g_VSYS_TMR_TotalExpCnt;
extern INT8U OSIntNesting;
extern INT8U OSPrioCur;
#endif

UINT8 g_VSYS_TMR_HwTmrSel;
static UINT8 g_VSYS_TMR_ExpFlag;
static VOSSemap_t g_VSYS_TMR_Lock;
static VOSSemap_t g_VSYS_TMR_ExpireSignal;

#define _VSYS_TMR_Lock()     { VOSSemapTake(g_VSYS_TMR_Lock,VOSMAX_DELAY); }
#define _VSYS_TMR_Unlock()     { VOSSemapGive(g_VSYS_TMR_Lock); }

//#define _VSYS_TMR_GetTickNum(Ms)     ((VCKM_GetTimerClockFreq() / 1000) * Ms)
//#define _VSYS_TMR_GetCurTicks()      (VTMR_GetCntValue(TMR_FCLKS))     

#define _VSYS_TMR_WaitForExpiration() { VOSSemapTake(g_VSYS_TMR_ExpireSignal, VOSMAX_DELAY); }

#if ((CKM_SLOW_CLK_EN > 0) || (CKM_RFLPO_SUPPORTED > 0) || (CKM_9611LPO_SUPPORTED > 0))
extern UINT32 SYSpwr_LPO_GetTickNum(UINT32 Ms);
extern UINT32 SYSpwr_LPO_GetTickNum_By_us(UINT32 MicroSeconds);
#endif

static void _VSYS_TMR_StartHwTimer(UINT32 NumOfTicks);
static void _VSYS_TMR_StopHwTimer(void);
static void _VSYS_TMR_Link(VSYS_TMR_DATA *p_SysTmr);
static void _VSYS_TMR_Unlink(VSYS_TMR_DATA * p_SysTmr);
static void _VSYS_TMR_Expiration(void);
static BOOL _VSYS_TMR_IsTimerEnable(void);

static UINT32 _VSYS_TMR_Tick2Ms(UINT32 tick)
{
    if (g_VSYS_TMR_HwTmrSel == TMR_FCLKS) {
        return (tick / (VCKM_GetTimerClockFreq() / 1000));
    } else {/* LPO is selected */
        return (tick/(4000 / 1000));
    }
}

static UINT32 _VSYS_TMR_GetTickNum(UINT32 Ms)
{
    if (g_VSYS_TMR_HwTmrSel == TMR_FCLKS) {
        return ((VCKM_GetTimerClockFreq() / 1000) * Ms);
    } else {/* LPO is selected */
#if SYS_LPO_SUPPOR
		extern UINT32 SYSpwr_LPO_GetTickNum(UINT32 Ms);
        return SYSpwr_LPO_GetTickNum(Ms);
#else
        return ((4000 / 1000) * Ms);
#endif
    }
}

static UINT32 _VSYS_TMR_GetCurTicks(void)
{
#if (VDEV_TYPE == TYPE_FPGA)
    // zljun_20150623:
    // From CHIP_9613IDC on, SCLK will be reset after timer is stoped and the current count can't be read.
    if((VTMR_SEL)g_VSYS_TMR_HwTmrSel == TMR_SCLK ) {
        if(!_VSYS_TMR_IsTimerEnable()) {
            WARNING_PRINTF("From CHIP_9613IDC on, SCLK current count shall be read when it is running.\r\n");
            while(1);
        }
    }
#endif    
    return VTMR_GetCntValue((VTMR_SEL)g_VSYS_TMR_HwTmrSel);
}

static BOOL _VSYS_TMR_IsTimerEnable(void)
{
    return VTMR_IsRunning((VTMR_SEL)g_VSYS_TMR_HwTmrSel);
}

static BOOL _VSYS_TMR_GetWorkStatus(void)
{
    return VTMR_GetWorkStatus((VTMR_SEL)g_VSYS_TMR_HwTmrSel);

}

void VSYS_TMR_HwTmrSel( UINT8 sel)
{
        g_VSYS_TMR_HwTmrSel = sel;
}

void VSYS_TMR_Init(void)
{
    if (g_VSYS_TMR_HwTmrSel != TMR_FCLKS)
        g_VSYS_TMR_HwTmrSel = TMR_SCLK;

    _VSYS_TMR_StopHwTimer();

    /* initial global variables */
    gp_VSYS_TMR_TimerList = (VSYS_TMR_DATA *)NULL;    
    g_VSYS_TMR_Lock = VOSSemapCreate("Tmr Lock", 1);
    g_VSYS_TMR_ExpireSignal = VOSSemapCreate("Tmr Signal",0);
    g_VSYS_TMR_CountDownTicks = 0;
    g_VSYS_TMR_ExpFlag = 0;
#if (VSYS_TMR_PROFILE_EN > 0)
    g_VSYS_TMR_TotalTimers = 0;
    g_VSYS_TMR_TotalExpCnt = 0;
#endif

}

VSYS_TMR_DATA *VSYS_TMR_Create(UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs)
{
    VSYS_TMR_DATA *p_sys_tmr;

    ASSERT((Initial <= VSYS_TMR_MAX_TIME) && (Period <= VSYS_TMR_MAX_TIME));

    p_sys_tmr = (VSYS_TMR_DATA *)VSYS_MEM_Malloc(sizeof(VSYS_TMR_DATA));
    if (p_sys_tmr != (VSYS_TMR_DATA *)NULL) {        
        p_sys_tmr->state = VSYS_TMR_STATE_STOPPED;
        p_sys_tmr->magic_id = VSYS_TMR_MAGIC_ID;
        p_sys_tmr->initial = Initial ? Initial : 1; //VA9621_20131011_CJ:  at least 1ms
        p_sys_tmr->period = Period;
        p_sys_tmr->pfn_callback = pfn_Callback;
        p_sys_tmr->p_callback_args = p_CallbackArgs;
#if (VSYS_TMR_PROFILE_EN > 0)
        p_sys_tmr->creator = (OSIntNesting ? (0 - OSIntNesting) : OSPrioCur);
        p_sys_tmr->expired_count = 0;
        g_VSYS_TMR_TotalTimers++;
#endif


    }

    return (p_sys_tmr);
}

void VSYS_TMR_Delete(VSYS_TMR_DATA *p_SysTmr)
{
    _VSYS_TMR_Lock();

    if ((p_SysTmr != NULL) && (p_SysTmr->magic_id == VSYS_TMR_MAGIC_ID)) {
        if (p_SysTmr->state != VSYS_TMR_STATE_STOPPED) {
            _VSYS_TMR_Unlink(p_SysTmr);
            p_SysTmr->state = VSYS_TMR_STATE_STOPPED;
        }
        p_SysTmr->magic_id = 0;
        VSYS_MEM_Free((void *)p_SysTmr);
#if (VSYS_TMR_PROFILE_EN > 0)        
        g_VSYS_TMR_TotalTimers--;
#endif

    } else {
        VSYS_TMR_EPRINTF("%s: Invalid timer! p_SysTmr: 0x%08X, Magic ID: 0x%08X, Return Addr.: 0x%08X.\r\n", \
                            FUNCTION_NAME, p_SysTmr, p_SysTmr->magic_id, __GET_RETURN_ADDR());
    }
    
    _VSYS_TMR_Unlock();
}

UINT32 VSYS_TMR_Create_Static(VSYS_TMR_DATA *p_SysTmr, UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs)
{
    ASSERT((Initial <= VSYS_TMR_MAX_TIME) && (Period <= VSYS_TMR_MAX_TIME));

    if (p_SysTmr != (VSYS_TMR_DATA *)NULL) {        
        p_SysTmr->state = VSYS_TMR_STATE_STOPPED;
        p_SysTmr->magic_id = VSYS_TMR_MAGIC_ID;
        p_SysTmr->initial = Initial ? Initial : 1; //VA9621_20131011_CJ:  at least 1ms
        p_SysTmr->period = Period;
        p_SysTmr->pfn_callback = pfn_Callback;
        p_SysTmr->p_callback_args = p_CallbackArgs;
#if (VSYS_TMR_PROFILE_EN > 0)
        p_SysTmr->creator = (OSIntNesting ? (0 - OSIntNesting) : OSPrioCur);
        p_SysTmr->expired_count = 0;
        g_VSYS_TMR_TotalTimers++;
#endif
        return 0;
    }

    return 1;
}

void VSYS_TMR_Delete_Static(VSYS_TMR_DATA *p_SysTmr)
{
    _VSYS_TMR_Lock();

    if ((p_SysTmr != NULL) && (p_SysTmr->magic_id == VSYS_TMR_MAGIC_ID)) {
        if (p_SysTmr->state != VSYS_TMR_STATE_STOPPED) {
            _VSYS_TMR_Unlink(p_SysTmr);
            p_SysTmr->state = VSYS_TMR_STATE_STOPPED;
        }
        p_SysTmr->magic_id = 0;
#if (VSYS_TMR_PROFILE_EN > 0)        
        g_VSYS_TMR_TotalTimers--;
#endif
    } else {
        VSYS_TMR_EPRINTF("%s: Invalid timer! p_SysTmr: 0x%08X, Magic ID: 0x%08X, Return Addr.: 0x%08X.\r\n", \
                            FUNCTION_NAME, p_SysTmr, p_SysTmr->magic_id, __GET_RETURN_ADDR());
    }
    
    _VSYS_TMR_Unlock();
}

void VSYS_TMR_ChangeSetting(VSYS_TMR_DATA *p_SysTmr, UINT32 Initial, UINT32 Period, VSYS_TMR_CALLBACK pfn_Callback, void *p_CallbackArgs)
{
    _VSYS_TMR_Lock();

    ASSERT((Initial <= VSYS_TMR_MAX_TIME) && (Period <= VSYS_TMR_MAX_TIME));
    
    if ((p_SysTmr != NULL) && (p_SysTmr->magic_id == VSYS_TMR_MAGIC_ID)) {        
        if (p_SysTmr->state != VSYS_TMR_STATE_STOPPED) {
            _VSYS_TMR_Unlink(p_SysTmr);
            p_SysTmr->state = VSYS_TMR_STATE_STOPPED;
        }
        p_SysTmr->initial = Initial;
        p_SysTmr->period = Period;
        p_SysTmr->pfn_callback = pfn_Callback;
        p_SysTmr->p_callback_args = p_CallbackArgs;
    } else {
        VSYS_TMR_EPRINTF("%s: Invalid timer! p_SysTmr: 0x%08X, Magic ID: 0x%08X, Return Addr.: 0x%08X.\r\n", \
                            FUNCTION_NAME, p_SysTmr, p_SysTmr->magic_id, __GET_RETURN_ADDR());
    }
    
    _VSYS_TMR_Unlock();
}

void VSYS_TMR_Start(VSYS_TMR_DATA *p_SysTmr)
{
    _VSYS_TMR_Lock();

    if ((p_SysTmr != NULL) && (p_SysTmr->magic_id == VSYS_TMR_MAGIC_ID)) {
        if (p_SysTmr->state != VSYS_TMR_STATE_RUNNING) {
            p_SysTmr->state = VSYS_TMR_STATE_RUNNING;
            _VSYS_TMR_Link(p_SysTmr);            
        }
    } else {
        VSYS_TMR_EPRINTF("%s: Invalid timer! p_SysTmr: 0x%08X, Magic ID: 0x%08X, Return Addr.: 0x%08X.\r\n", \
                            FUNCTION_NAME, p_SysTmr, p_SysTmr->magic_id, __GET_RETURN_ADDR());
    }
    
    _VSYS_TMR_Unlock();
}

void VSYS_TMR_Stop(VSYS_TMR_DATA *p_SysTmr)
{
    _VSYS_TMR_Lock();

    if ((p_SysTmr != NULL) && (p_SysTmr->magic_id == VSYS_TMR_MAGIC_ID)) {
        if (p_SysTmr->state != VSYS_TMR_STATE_STOPPED) {
            _VSYS_TMR_Unlink(p_SysTmr);
            p_SysTmr->state = VSYS_TMR_STATE_STOPPED;
        }
    } else {
        VSYS_TMR_EPRINTF("%s: Invalid timer! p_SysTmr: 0x%08X, Magic ID: 0x%08X, Return Addr.: 0x%08X.\r\n", \
                            FUNCTION_NAME, p_SysTmr, p_SysTmr->magic_id, __GET_RETURN_ADDR());
    }

    _VSYS_TMR_Unlock();
}

void VSYS_TMR_SendExpireSignal(void)
{
        //wls: VTMR int may occures earlier than initialize of g_VSYS_TMR_ExpireSignal
        if(g_VSYS_TMR_ExpireSignal){
            VOSSemapGive(g_VSYS_TMR_ExpireSignal);
        }else{
            VSYS_TMR_WPRINTF("WARNING: g_VSYS_TMR_ExpireSignal not initialized\n");
        }
}

void VSYS_TMR_Check(void)
{
    for (;;) {
        _VSYS_TMR_WaitForExpiration();
        _VSYS_TMR_Expiration();
    }    
}

/* Shorten timeout value of running timers. It could be used when timeout is not accurate enough
 * bceause of such as waken up from deep sleep. Caller can select position from which to be adjusted.
 * MicorSeconds: us to be shorten
 * StartPos: from which timer to be shorten
 */
void VSYS_TMR_ShortenRunningTimers(UINT32 MicroSeconds, UINT16 StartPos)
{
    VSYS_TMR_DATA* p_tmr = gp_VSYS_TMR_TimerList;
    UINT32 ticks = 0;

    while (p_tmr != NULL && StartPos--) {
        p_tmr = p_tmr->p_next;
    }

    if (p_tmr != NULL) {
        if (g_VSYS_TMR_HwTmrSel == TMR_FCLKS) {
            ticks = (VCKM_GetTimerClockFreq() / 1000) * MicroSeconds / 1000;
        } else {/* LPO is selected */
#if SYS_LPO_SUPPOR
			extern UINT32 SYSpwr_LPO_GetTickNum_By_us(UINT32 MicroSeconds);
            ticks = SYSpwr_LPO_GetTickNum_By_us(MicroSeconds);
#else
            ticks = (4000 / 1000) * MicroSeconds / 1000;
#endif
        }
    }
    
    _VSYS_TMR_Lock();
    while (p_tmr != NULL && ticks > 0) {
        if (p_tmr->time_out >= ticks) {
            p_tmr->time_out -= ticks;
            ticks = 0;
        } else {
            ticks -=p_tmr->time_out;
            p_tmr->time_out = 0;
        }
        p_tmr = p_tmr->p_next;
    }
    _VSYS_TMR_Unlock();
}

static void _VSYS_TMR_StopHwTimer(void)
{
    VTMR_DisableInt((VTMR_SEL)g_VSYS_TMR_HwTmrSel);    
    VTMR_Stop((VTMR_SEL)g_VSYS_TMR_HwTmrSel);
}

static void _VSYS_TMR_StartHwTimer(UINT32 NumOfTicks)
{
    VTMR_Config((VTMR_SEL)g_VSYS_TMR_HwTmrSel, NumOfTicks, FALSE);
    VTMR_RegisterISRCallback((VTMR_SEL)g_VSYS_TMR_HwTmrSel,(VDRV_TMR_CALLBACK)VSYS_TMR_SendExpireSignal);
    VTMR_EnableInt((VTMR_SEL)g_VSYS_TMR_HwTmrSel);    
    VTMR_Start((VTMR_SEL)g_VSYS_TMR_HwTmrSel);
}

static void _VSYS_TMR_Link(VSYS_TMR_DATA *p_SysTmr)
{
    UINT32 timer_ticks = _VSYS_TMR_GetTickNum(p_SysTmr->initial);
    UINT32 expend_ticks = 0;
    
    if (gp_VSYS_TMR_TimerList == NULL) {
        /* place the timer on an empty list.  */
        p_SysTmr->p_prev = NULL;
        p_SysTmr->p_next = NULL;
        
        /* link the timer to the list head.  */
        gp_VSYS_TMR_TimerList = p_SysTmr;

        /* setup count-down timer ticks */
        p_SysTmr->time_out = timer_ticks;
        if (!g_VSYS_TMR_ExpFlag) {
            g_VSYS_TMR_CountDownTicks =  p_SysTmr->time_out;

            if (g_VSYS_TMR_CountDownTicks != 0) {
                _VSYS_TMR_StartHwTimer(g_VSYS_TMR_CountDownTicks);
            } else {
                /* ensure the hardware timer stopped */
                _VSYS_TMR_StopHwTimer();
                /* just send expiration signal */
                VSYS_TMR_SendExpireSignal();
            }
        }
    } else {
        VSYS_TMR_DATA *p_tmr = NULL;        
        UINT32 elapsed_ticks = 0;
        BOOL done = FALSE;
        
        /* place the new timer into the list. */
        p_tmr = gp_VSYS_TMR_TimerList;

        /* adjust the actual ticks to time out of the first entry in list */
        if (!g_VSYS_TMR_ExpFlag) {
            UINT32 ticks = _VSYS_TMR_GetCurTicks();       
            expend_ticks = VTMR_CommTimeget();
            elapsed_ticks = g_VSYS_TMR_CountDownTicks - ticks;
            g_VSYS_TMR_CountDownTicks -= elapsed_ticks;
            if (p_tmr->time_out > elapsed_ticks) {
                p_tmr->time_out -= elapsed_ticks;
            } else {
                p_tmr->time_out = 0;
            }
        }

        /* find the appropriate place on the timer list for the new timer */
        done = FALSE;
        do {
            if (timer_ticks < p_tmr->time_out) {
                /* update the time out value of the next timer.  */
                p_tmr->time_out -= timer_ticks;

                if (p_tmr == gp_VSYS_TMR_TimerList) {
                    gp_VSYS_TMR_TimerList = p_SysTmr;
                }
                p_SysTmr->p_next = p_tmr;
                p_SysTmr->p_prev = p_tmr->p_prev;
                if (p_tmr->p_prev != NULL) {
                    ((VSYS_TMR_DATA *)(p_tmr->p_prev))->p_next = p_SysTmr;                    
                }
                p_tmr->p_prev = p_SysTmr;
                done = TRUE;
            } else {
                timer_ticks -= p_tmr->time_out;

                if (p_tmr->p_next == NULL) {
                    /* end of timer list */
                    p_tmr->p_next = p_SysTmr;
                    p_SysTmr->p_prev = p_tmr;
                    p_SysTmr->p_next = NULL;
                    done = TRUE;
                } else {
                    p_tmr =  p_tmr->p_next;
                }
            }
        } while (!done);

        /* update the actual number of ticks to time out */
        p_SysTmr->time_out = timer_ticks;

        /*
         * if expiration processing, 
         *    hardware timer will start in expiration routine
         * else
         *    start hardware timer here if neccessary
         */
        if (!g_VSYS_TMR_ExpFlag) {
            timer_ticks = gp_VSYS_TMR_TimerList->time_out;

            // VA9621_20131011_CJ: modify <= to <, and restrict that the new added timer initial time
            // shall be at least 1ms, so if timer_ticks equals 0, the head of timer list must not be the new
            // added timer, but some old one. Then let the timer go until the interrupt comes. If <= kept,
            // there's probability timer_ticks equal 0, then hw timer would be stopped and a signal sent,
            // but the same signal might had been sent already by interrupt...
            // The reason that do not remove VSYS_TMR_SendExpireSignal and keep <=, is that timer_ticks
            // equals 0 can not represent interrupt has came, stop the hw timer might lead to interrupt not
            // coming, then the result was none signal sent out.
            if (timer_ticks < g_VSYS_TMR_CountDownTicks) {
                /* stop hardware timer and restart it with a new count-down ticks */
                _VSYS_TMR_StopHwTimer();
                /* setup for a minimum of timer expiration ticks */
                g_VSYS_TMR_CountDownTicks = timer_ticks;
                expend_ticks -= VTMR_CommTimeget();
                expend_ticks = _VSYS_TMR_GetTickNum(expend_ticks)/1000;
                if(g_VSYS_TMR_CountDownTicks > expend_ticks){
                    _VSYS_TMR_StartHwTimer(g_VSYS_TMR_CountDownTicks - expend_ticks);
                }else {
                    // VA9621_20131011_CJ: actually meaningless now
                    /* just send expiration signal */
                    VSYS_TMR_SendExpireSignal();
                }
            }
        }
    }    
}

static void _VSYS_TMR_Unlink(VSYS_TMR_DATA * p_SysTmr)
{
    /* adjust the time out ticks of next timer in the list */
    if (p_SysTmr->p_next != NULL) {
        ((VSYS_TMR_DATA *)(p_SysTmr->p_next))->time_out += p_SysTmr->time_out;
    }

    /* unlink the timer from the list */
    if (p_SysTmr->p_next == NULL && p_SysTmr->p_prev == NULL) {
        /* only one timer on the list.    */
        gp_VSYS_TMR_TimerList = NULL;
        _VSYS_TMR_StopHwTimer();
    } else {
        /* more than one timer on the list.  */
        if (p_SysTmr->p_prev != NULL) {
            ((VSYS_TMR_DATA *)(p_SysTmr->p_prev))->p_next = p_SysTmr->p_next;
        }

        if (p_SysTmr->p_next != NULL) {
            ((VSYS_TMR_DATA *)(p_SysTmr->p_next))->p_prev = p_SysTmr->p_prev;
        }        

        /* move the head pointer to next time if the timer is at the head of the list */
        if (gp_VSYS_TMR_TimerList == p_SysTmr) {
            gp_VSYS_TMR_TimerList =  p_SysTmr->p_next;
        }
        
        p_SysTmr->p_next = NULL;
        p_SysTmr->p_prev = NULL;
    }
}

static void _VSYS_TMR_Expiration(void)
{
    VSYS_TMR_CALLBACK pfn_callback = NULL;
    void *p_callback_args = NULL;
    VSYS_TMR_DATA *p_tmr = NULL;
    BOOL done = FALSE;
    UINT32 expend_ticks = VTMR_CommTimeget();

    _VSYS_TMR_Lock();

    /* ensure the hardware timer stopped during expiration */
    _VSYS_TMR_StopHwTimer();

    /* set the flag to indicate expiration processing */
    g_VSYS_TMR_ExpFlag = 1;

#if (VSYS_TMR_PROFILE_EN > 0)
    g_VSYS_TMR_TotalExpCnt++;
#endif

    p_tmr = gp_VSYS_TMR_TimerList;
    if (p_tmr) {
        /* adjust the remain ticks of the first entry on the list */
        if (p_tmr->time_out > g_VSYS_TMR_CountDownTicks) {
            p_tmr->time_out -= g_VSYS_TMR_CountDownTicks;
        } else {
            p_tmr->time_out = 0;
        }
    }

    /* 
     * release timer list protection, but keep the expiration flag set
     * to prevent activating new timers
     */
    _VSYS_TMR_Unlock();


    /* search expired timers, the value of time_out is 0 indicate the timer expired  */
    done = FALSE;
    do {   
        _VSYS_TMR_Lock();

        /* 
         * list head will be updated in link & unlink operations, 
         * so always use the list head in the while loop to search expired timer
         */
        p_tmr = gp_VSYS_TMR_TimerList;
        pfn_callback = NULL;
        p_callback_args = NULL;
        
        if ((p_tmr) && (p_tmr->time_out == 0)) {
#if (VSYS_TMR_PROFILE_EN > 0)
            p_tmr->expired_count++;
#endif
            /* an expired timer found */
            pfn_callback = p_tmr->pfn_callback;
            p_callback_args = p_tmr->p_callback_args;

            /* remove the timer from the list and set state to stopped */            
            _VSYS_TMR_Unlink(p_tmr);
            p_tmr->state = VSYS_TMR_STATE_STOPPED;

            /* restart the timer if it is a periodic timer */
            if (p_tmr->period) {
                /* set new initial value to period */
                p_tmr->initial = p_tmr->period;

                /* set running state and put it on the list again */
                p_tmr->state = VSYS_TMR_STATE_RUNNING;
                _VSYS_TMR_Link(p_tmr);                
            }
        } else {
            /* no more expired timers on the list */
            done = TRUE;
        }
        
        /* release the lock before callback routine */
        _VSYS_TMR_Unlock();

        if (pfn_callback != NULL) {
            /* call timer's expiration callback function */
            (*pfn_callback)(((VOSTimer_t*)p_tmr - 1), p_callback_args);
        }
    } while (!done);

    _VSYS_TMR_Lock();

    /* start hareware timer if necessary */
    if (gp_VSYS_TMR_TimerList) {
        g_VSYS_TMR_CountDownTicks = gp_VSYS_TMR_TimerList->time_out;
        expend_ticks -= VTMR_CommTimeget();
        expend_ticks = _VSYS_TMR_GetTickNum(expend_ticks + 50)/1000;
        if(g_VSYS_TMR_CountDownTicks > expend_ticks){
            expend_ticks = g_VSYS_TMR_CountDownTicks - expend_ticks;
        }else {
            expend_ticks = 1;
        }
        _VSYS_TMR_StartHwTimer(expend_ticks);
    }

    /* clear the expiration flag to indicate expiration processing is complete. */
    g_VSYS_TMR_ExpFlag = 0;

    _VSYS_TMR_Unlock();
}

/******************************************************************************
Description:
     check the input timer is the header of timer list.

Parameters:
    p_Tmr - pointer to a timer. 

Return Value:
    TRUE  - header timer.
    FALSE - not header timer.
*****************************************************************************/
BOOL VSYS_TMR_IsListHeader(VSYS_TMR_DATA* p_Tmr)
{
    return (gp_VSYS_TMR_TimerList == p_Tmr);
} 

/******************************************************************************
Description:
    get elapsed time in us.

Parameters:
    none. 

Return Value:
    elapsed time in us.
*****************************************************************************/
UINT32 VSYS_TMR_GetElapsedTime(void)
{
    UINT32 elapsed_ticks;

    elapsed_ticks = g_VSYS_TMR_CountDownTicks - _VSYS_TMR_GetCurTicks(); 

    /* the sclk period is 31.25us */ 
    return (elapsed_ticks * 125) >> 2 ;
}

/******************************************************************************
Description:
    get Remainder time in ms.

Parameters:
    none. 

Return Value:
    Remainder time in ms.
*****************************************************************************/
UINT32 VSYS_TMR_GetRemainderTime(VSYS_TMR_DATA* p_Tmr)
{
    UINT32 Remainder_tick = 0;
    VSYS_TMR_DATA *p_tmr = NULL;

    _VSYS_TMR_Lock();
    if ((p_Tmr != NULL) && (p_Tmr->magic_id == VSYS_TMR_MAGIC_ID)) {
        if (p_Tmr->state == VSYS_TMR_STATE_RUNNING) {
            p_tmr = gp_VSYS_TMR_TimerList;
            if(p_tmr != NULL){
                Remainder_tick += _VSYS_TMR_GetCurTicks();
                if(p_tmr != p_Tmr){
                    p_tmr = p_tmr->p_next;
                    while (p_tmr != NULL ) {
                        Remainder_tick += p_tmr->time_out;
                        if(p_tmr == p_Tmr) break;
                        p_tmr = p_tmr->p_next;
                    } 
                } 
            }    
        }
    }
    _VSYS_TMR_Unlock();

    return _VSYS_TMR_Tick2Ms(Remainder_tick);
}

/******************************************************************************
Description:
    Time synchronization.

Parameters:
    none. 

Return Value:
    
*****************************************************************************/
void VSYS_TMR_Synchronization(UINT32 Ms)
{
    UINT32 ticks;
    UINT32 remainder_ticks;
    VSYS_TMR_DATA *p_tmr = NULL;

    _VSYS_TMR_Lock();
    remainder_ticks = _VSYS_TMR_GetCurTicks();
    ticks = _VSYS_TMR_GetTickNum(Ms);

        _VSYS_TMR_StopHwTimer();
 
        p_tmr = gp_VSYS_TMR_TimerList;
    if (p_tmr) {
                if(remainder_ticks >= ticks){
                        p_tmr->time_out = remainder_ticks - ticks;
                }else{
                        p_tmr->time_out = 0;
                        ticks -= remainder_ticks;
                        p_tmr = p_tmr->p_next;
                        while (p_tmr != NULL && ticks > 0) {
                                if (p_tmr->time_out >= ticks) {
                                        p_tmr->time_out -= ticks;
                                        ticks = 0;
                                } else {
                                        ticks -=p_tmr->time_out;
                                        p_tmr->time_out = 0;
                                }
                                p_tmr = p_tmr->p_next;
                        }            
                }
                /* start hareware timer if necessary */
                if (gp_VSYS_TMR_TimerList) {
                        g_VSYS_TMR_CountDownTicks = gp_VSYS_TMR_TimerList->time_out;
                        _VSYS_TMR_StartHwTimer(g_VSYS_TMR_CountDownTicks);
                }
        }
    _VSYS_TMR_Unlock();
}

