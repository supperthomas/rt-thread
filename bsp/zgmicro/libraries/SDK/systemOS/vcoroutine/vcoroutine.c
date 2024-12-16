/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    croutine

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vstdcore.h"
#include "vcoroutine.h"

typedef struct
{
    VCORTcb_t pxNext;
    UINT32 cnt;
} VCORTcbList_t;

typedef struct
{
    VCORTmr_t pxNext;
    VCORTmr_t pxCurrent;
    UINT32 cnt;
} VCORTmrList_t;

static VCORTcbList_t pxCoRoutineLists = {0};
static VCORTmrList_t pxCoRTimerLists = {0};
static VCORTcb_t pxPrioCoRTCB = NULL;
VCORTcb_t pxCurrentCoRTCB = NULL;
BOOL pxCurrentCoRSche = FALSE;
static BOOL pxISRCoRSche = FALSE;
static BOOL pxISRCoRRun = FALSE;
static UINT32 VCOR_SYS_AllTime = 0;
static UINT32 VCOR_SYS_CYCLES = 0;
static UINT32 VCOR_SYS_TMEMAX = 0;
static UINT32 VCorCriticalNesting = 0;
static UINT32 VCorCpuSrValue = 0;


void vcorCriticalEnter(void)
{
    if (VCorCriticalNesting == 0)
    {
        VCorCpuSrValue = VCPU_SR_Save();
    }
    VCorCriticalNesting++;
    if (VCorCriticalNesting == (UINT32)-1)
    {
        while (1)
        {
            ASSERT(0);
        };
    }
}

void vcorCriticalExit(void)
{
    VCorCriticalNesting--;
    if (VCorCriticalNesting == 0)
    {
        VCPU_SR_Restore(VCorCpuSrValue);
    }
    if (VCorCriticalNesting == (UINT32)-1)
    {
        while (1)
        {
            ASSERT(0);
        };
    }
}

UINT32 vcorListInsert(VCORTcb_t new_tcb)
{
    VCORTcb_t p_cortcb, p_cortcbprev = (VCORTcb_t)&pxCoRoutineLists;

    for (p_cortcb = pxCoRoutineLists.pxNext; p_cortcb != NULL && p_cortcb->prio <= new_tcb->prio; p_cortcb = p_cortcb->pxNext)
    {
        p_cortcbprev = p_cortcb;
    }
    new_tcb->pxNext = p_cortcb;
    p_cortcbprev->pxNext = new_tcb;

    pxCoRoutineLists.cnt++;
    return pxCoRoutineLists.cnt;
}

UINT32 vcorListRemove(VCORTcb_t remove_tcb)
{
    VCORTcb_t p_cortcb, p_cortcbprev = (VCORTcb_t)&pxCoRoutineLists;

    for (p_cortcb = pxCoRoutineLists.pxNext; p_cortcb != NULL && p_cortcb != remove_tcb; p_cortcb = p_cortcb->pxNext)
    {
        p_cortcbprev = p_cortcb;
    }
    if (p_cortcb != NULL)
    {
        p_cortcbprev->pxNext = p_cortcb->pxNext;
        --pxCoRoutineLists.cnt;
    }
    remove_tcb->pxNext = NULL;
    return pxCoRoutineLists.cnt;
}

UINT8 vcorListPrioSched(VCORTcb_t sched_tcb)
{
    VCORTcb_t p_front, p_back;
    if (pxPrioCoRTCB != NULL && sched_tcb->pxNext != NULL && sched_tcb->pxNext->prio == sched_tcb->prio)
    {
        p_front = pxPrioCoRTCB->pxNext;
        for (p_back = sched_tcb; p_back->pxNext != NULL && p_back->pxNext->prio == p_back->prio; p_back = p_back->pxNext)
        {
            ;
        }
        pxPrioCoRTCB->pxNext = sched_tcb->pxNext;
        sched_tcb->pxNext = p_back->pxNext;
		p_back->pxNext = p_front;
        
    }
    return sched_tcb->prio;
}

static void vcortcbclear(VCORTcb_t remove_tcb)
{
    vcorListRemove(remove_tcb);
    if (remove_tcb->StaticAlloc == FALSE)
    {
        VCORFree(remove_tcb);
    }
}

BOOL vcortmrhandle(void)
{
    BOOL state = FALSE;
    UINT32 cnt = 0;
    while (pxCoRTimerLists.pxCurrent != NULL &&
           (cnt = vcortimeinterval(pxCoRTimerLists.pxCurrent->timeout_va, vcortimegetms())) != 0 && cnt < 0x7FFFFFFF)
    {
        VCORTmr_t pxCurrent = pxCoRTimerLists.pxCurrent;
        pxCurrent->uxState = VCORTMREXE_STATE;
        if (pxCurrent->pxCorTimerFunction != NULL)
        {
            pxCurrent->pxCorTimerFunction(pxCurrent->p_arg);
        }
        if (VCORTMREXE_STATE == VCORTmrStop(pxCurrent))
        {
            if (pxCurrent->period_ms != 0)
            {
                pxCurrent->uxState = VCORTMRRELOAD_STATE;
                VCORTmrStart(pxCurrent);
            }
            else
            {
                pxCurrent->uxState = VCORTMRSTOP_STATE;
            }
        }
        state = TRUE;
    }
    return state;
}

void VCORScheduler(void)
{
    BOOL prioSche = FALSE;
    VCORTcb_t p_cortcb, remove_tcb = NULL;
    UINT32 run_cycle = 0;
#if VCORPRIO_SUPPORT
	pxPrioCoRTCB = (VCORTcb_t)&pxCoRoutineLists;
#endif
    for (p_cortcb = pxCoRoutineLists.pxNext; p_cortcb != NULL; p_cortcb = p_cortcb->pxNext)
    {
        if (remove_tcb != NULL)
        {
            vcortcbclear(remove_tcb);
            remove_tcb = NULL;
        }
        if (p_cortcb->uxState == vcorDELETE_STATE)
        {
            remove_tcb = p_cortcb;
            continue;
        }
        if (pxISRCoRSche == TRUE)
        {
            pxISRCoRSche = FALSE;
			pxISRCoRRun = FALSE;
            break;
        }
        if (pxCoRTimerLists.pxCurrent != NULL && vcortmrhandle())
        {
#if VCORPRIO_SUPPORT
            break;
        }
        if (p_cortcb->pxNext != NULL)
        {
            if (p_cortcb->pxNext->prio != p_cortcb->prio)
            {
                pxPrioCoRTCB = p_cortcb;
                prioSche = FALSE;
            }
            else
            {
                prioSche = TRUE;
            }
        }
        else
        {
            prioSche = FALSE;
#endif
        }
        pxCurrentCoRTCB = p_cortcb;
        p_cortcb->runcnt++;
        run_cycle = VCORCPUCycleGet();
        p_cortcb->pxCoRoutineFunction(p_cortcb->p_arg);
        run_cycle = VCORCPUCycleGet() - run_cycle;
        p_cortcb->runtime_max = p_cortcb->runtime_max < run_cycle ? run_cycle : p_cortcb->runtime_max;
#if VCORPRIO_SUPPORT
        if (pxCurrentCoRSche == TRUE)
        {
            pxCurrentCoRSche = FALSE;
            if (prioSche == TRUE)
            {
                vcorListPrioSched(p_cortcb);
            }
            break;
        }
#endif
    }
    pxCurrentCoRTCB = NULL;
    if (remove_tcb != NULL)
    {
        vcortcbclear(remove_tcb);
        remove_tcb = NULL;
    }
    if (pxCoRTimerLists.pxCurrent != NULL)
    {
        vcortmrhandle();
    }
}

VCORTcb_t VCORTaskCreateStatic(VCORTaskFunc_t pxCoRoutineCode, void *p_arg, UINT8 uxPriority, VCORTcb_Static *handle)
{
    VCORTcb_t p_cortcb = handle;
    if (p_cortcb != NULL)
    {
        p_cortcb->uxState = vcorINITIAL_STATE;
        p_cortcb->prio = uxPriority;
        p_cortcb->pxNext = NULL;
        p_cortcb->p_arg = p_arg;
        p_cortcb->pxCoRoutineFunction = pxCoRoutineCode;
        p_cortcb->timedelay = 0;
        p_cortcb->StaticAlloc = TRUE;
        p_cortcb->runcnt = 0;
        p_cortcb->runtime_max = 0;
        vcorListInsert(p_cortcb);
    }

    return p_cortcb;
}

VCORTcb_t VCORTaskCreate(VCORTaskFunc_t pxCoRoutineCode, void *p_arg, UINT8 uxPriority)
{
    VCORTcb_t p_cortcb = (VCORTcb_t)VCORMalloc(sizeof(VCORTcb_Static));

    if (p_cortcb != NULL)
    {
        VCORTaskCreateStatic(pxCoRoutineCode, p_arg, uxPriority, p_cortcb);
        p_cortcb->StaticAlloc = FALSE;
    }

    return p_cortcb;
}

void vcortaskdel(VCORTcb_t p_cortcb)
{
    if (p_cortcb == NULL)
    {
        p_cortcb = pxCurrentCoRTCB;
    }
    if (p_cortcb != NULL)
    {
        if (p_cortcb->uxState == vcorINITIAL_STATE)
        {
            vcortcbclear(p_cortcb);
        }
        else
        {
            p_cortcb->uxState = vcorDELETE_STATE - 1;
        }
    }
}

void VCORISRCoRSche(BOOL en)
{
    if (en == TRUE)
    {
        pxISRCoRSche = TRUE;
    }
	pxISRCoRRun = TRUE;
}

BOOL VCORISRCoRGetRun(void)
{
	BOOL state = pxISRCoRRun;
	pxISRCoRRun = FALSE;
    return state;
}

void vcorFunction_demo(void *p_arg)
{
    VCORSTART();
    (void)p_arg;
    while (1)
    {
        VCORWaitContent(TRUE, VCORMAX_DELAY);
        VCORSched();
    }
    VCOREND();
}

void VCORTimeCycles(UINT32 cycle, UINT32 maxval)
{
    VCOR_SYS_CYCLES = cycle / 1000;
    VCOR_SYS_TMEMAX = maxval / VCOR_SYS_CYCLES;
}

UINT32 vcortimegetms(void)
{
	static UINT32 last_time = 0;
    UINT32 time = 0;
    if (VCOR_SYS_CYCLES)
    {
		time = VCORCPUCycleGet();
		if(last_time > time)
		{
			VCOR_SYS_AllTime += VCOR_SYS_TMEMAX;
		}
		last_time = time;
        time = time / VCOR_SYS_CYCLES % VCOR_SYS_TMEMAX;
    }
    return (VCOR_SYS_AllTime + time);
}

UINT32 vcortimeinterval(UINT32 ms_start, UINT32 ms_end)
{
        return ms_end - ms_start;
}

VCORQue_t VCORQueCreateStatic(VCORQue_Static *handle, UINT16 numb)
{
    VCORQue_t p_corque = handle;
    if (numb == 0)
    {
        return NULL;
    }

    if (p_corque != NULL)
    {
        p_corque->length = numb;
        p_corque->numb = 0;
        p_corque->StaticAlloc = TRUE;
        for (UINT16 i = 0; i < numb; i++)
        {
            p_corque->queList[i] = 0;
        }
        p_corque->rd_p = 0;
        p_corque->wr_p = 0;
    }

    return p_corque;
}

VCORQue_t VCORQueCreate(UINT16 numb)
{
    VCORQue_t p_corque = NULL;
    if (numb > 0)
    {
        p_corque = (VCORQue_t)VCORMalloc(sizeof(VCORQue_Static) + sizeof(UINT32) * (numb - 1));

        if (p_corque != NULL)
        {
            VCORQueCreateStatic(p_corque, numb);
            p_corque->StaticAlloc = FALSE;
        }
    }

    return p_corque;
}

void VCORQueDel(VCORQue_t p_corque)
{
    if (p_corque != NULL)
    {
        if (p_corque->StaticAlloc == FALSE)
        {
            VCORFree(p_corque);
        }
    }
}

VRET_VALUE vcorquereceive(VCORQue_t p_corque, UINT8 *id, UINT8 **p_void)
{
    UINT32 buf = 0;
    VRET_VALUE ret = VRET_ERROR;
    if (p_corque != NULL)
    {
		VCORCRITICAL_ENTER();
        if (p_corque->numb > 0)
        {
            buf = p_corque->queList[p_corque->rd_p++];
            if (p_corque->rd_p == p_corque->length)
            {
                p_corque->rd_p = 0;
            }
            p_corque->numb--;
            ret = VRET_NO_ERROR;
        }
		VCORCRITICAL_EXIT();
    }
    if (id != NULL)
    {
        *id = (UINT8)(buf >> 24);
    }
    if (p_void != NULL)
    {
        *p_void = (UINT8 *)(buf ? ((buf & 0xFFFFFF) | VCORPRIO_MEM_BASE) : 0);
    }
    return ret;
}

VRET_VALUE vcorquepeek(VCORQue_t p_corque, UINT8 *id, UINT8 **p_void)
{
    UINT32 buf = 0;
    VRET_VALUE ret = VRET_ERROR;
    if (p_corque != NULL)
    {
		VCORCRITICAL_ENTER();
        if (p_corque->numb > 0)
        {
            buf = p_corque->queList[p_corque->rd_p];
            ret = VRET_NO_ERROR;
        }
		VCORCRITICAL_EXIT();
    }
    if (id != NULL)
    {
        *id = (UINT8)(buf >> 24);
    }
    if (p_void != NULL)
    {
        *p_void = (UINT8 *)(buf ? ((buf & 0xFFFFFF) | VCORPRIO_MEM_BASE) : 0);
    }
    return ret;
}

VRET_VALUE vcorquesend(VCORQue_t p_corque, UINT8 id, UINT8 *p_void, size_t size)
{
	VRET_VALUE ret = VRET_ERROR;
    void *buf = p_void;
    if (p_corque != NULL && buf != NULL)
    {
        if (size != 0)
        {
            buf = VCORMalloc(size);
			vmemsetowner(buf,__GET_RETURN_ADDR());
            if (buf == NULL)
            {
                return VRET_ERROR;
            }
            VCORMemcpy(buf, p_void, size);
        }
		VCORCRITICAL_ENTER();
        if (p_corque->numb < p_corque->length)
        {
            p_corque->queList[p_corque->wr_p++] = ((UINT32)buf & 0xFFFFFF) | (id << 24);
            if (p_corque->wr_p == p_corque->length)
            {
                p_corque->wr_p = 0;
            }
            p_corque->numb++;
            ret = VRET_NO_ERROR;
        }
		VCORCRITICAL_EXIT();
        if (ret != VRET_NO_ERROR && size != 0)
        {
            VCORFree(buf);
        }
    }
    return ret;
}

VCORTmr_t VCORTmrCreateStatic(UINT32 init_ms, UINT32 period_ms, VCORTmrFunc_t fun, void *p_arg, VCORTmr_Static *handle)
{
    VCORTmr_t p_cortmr = handle;
    if (p_cortmr != NULL && init_ms <= 0x7FFFFFFF && period_ms <= 0x7FFFFFFF)
    {
        p_cortmr->pxNext = NULL;
        p_cortmr->init_ms = init_ms;
        p_cortmr->period_ms = period_ms;
        p_cortmr->pxCorTimerFunction = fun;
        p_cortmr->p_arg = p_arg;
        p_cortmr->timeout_va = 0;
        p_cortmr->uxState = VCORTMRSTOP_STATE;
        p_cortmr->StaticAlloc = TRUE;
    }

    return p_cortmr;
}

VCORTmr_t VCORTmrCreate(UINT32 init_ms, UINT32 period_ms, VCORTmrFunc_t fun, void *p_arg)
{
    VCORTmr_t p_cortmr = NULL;
    if (init_ms <= 0x7FFFFFFF && period_ms <= 0x7FFFFFFF)
    {
        p_cortmr = (VCORTmr_t)VCORMalloc(sizeof(VCORTmr_Static));
        if (p_cortmr != NULL)
        {
            VCORTmrCreateStatic(init_ms, period_ms, fun, p_arg, p_cortmr);
            p_cortmr->StaticAlloc = FALSE;
        }
    }

    return p_cortmr;
}

void VCORTmrDel(VCORTmr_t p_cortmr)
{
    if (p_cortmr != NULL)
    {
        if (p_cortmr->StaticAlloc == FALSE)
        {
            VCORFree(p_cortmr);
        }
    }
}

UINT32 VCORTmrStart(VCORTmr_t p_cortmr)
{
    UINT32 time = vcortimegetms();
    VCORTmr_t p_cortmrt, p_cortmrprev = (VCORTmr_t)&pxCoRTimerLists;
    if (p_cortmr->uxState == VCORTMRSTOP_STATE)
    {
        p_cortmr->timeout_va = (time + p_cortmr->init_ms);
    }
    else if (p_cortmr->uxState == VCORTMRRELOAD_STATE)
    {
        p_cortmr->timeout_va = (time + p_cortmr->period_ms);
    }
    else
    {
        return (UINT32)-1;
    }
    VCORCRITICAL_ENTER();
    for (p_cortmrt = pxCoRTimerLists.pxNext; p_cortmrt != NULL && p_cortmrt->timeout_va <= p_cortmr->timeout_va; p_cortmrt = p_cortmrt->pxNext)
    {
        p_cortmrprev = p_cortmrt;
    }
    p_cortmr->pxNext = p_cortmrt;
    p_cortmrprev->pxNext = p_cortmr;
    pxCoRTimerLists.cnt++;
    if (pxCoRTimerLists.pxCurrent == NULL ||
        (pxCoRTimerLists.pxCurrent->timeout_va > time && p_cortmr->timeout_va < pxCoRTimerLists.pxCurrent->timeout_va))
    {
        pxCoRTimerLists.pxCurrent = p_cortmr;
    }
    VCORCRITICAL_EXIT();
    p_cortmrt->uxState = VCORTMRRUN_STATE;

    return p_cortmr->timeout_va;
}

UINT8 VCORTmrStop(VCORTmr_t p_cortmr)
{
    UINT8 uxState = VCORTMRDEL_STATE;
    VCORTmr_t p_cortmrt, p_cortmrprev = (VCORTmr_t)&pxCoRTimerLists;
    VCORCRITICAL_ENTER();
    for (p_cortmrt = pxCoRTimerLists.pxNext; p_cortmrt != NULL && p_cortmrt != p_cortmr; p_cortmrt = p_cortmrt->pxNext)
    {
        p_cortmrprev = p_cortmrt;
    }
    if (p_cortmrt != NULL)
    {
        p_cortmrprev->pxNext = p_cortmrt->pxNext;
        --pxCoRTimerLists.cnt;
        if (pxCoRTimerLists.pxCurrent == p_cortmrt)
        {
            if (p_cortmrt->pxNext != NULL)
            {
                pxCoRTimerLists.pxCurrent = p_cortmrt->pxNext;
            }
            else
            {
                pxCoRTimerLists.pxCurrent = pxCoRTimerLists.pxNext;
            }
        }
        p_cortmrt->pxNext = NULL;
        uxState = p_cortmrt->uxState;
        p_cortmrt->uxState = VCORTMRSTOP_STATE;
    }
    VCORCRITICAL_EXIT();

    return uxState;
}
