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
    os

Description:
     os.

Revision History:
     Date                  Author                   Description
     2022.11.22           cxt                  initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstdos.h"
#include "SEGGER_SYSVIEW_Wrap.h"
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
#include "os.h"
#include "vsys_timer.h"
static void VOS_TimerTask(void);
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
static UINT32 VOSYieldRequired = 0;
static UINT32 VOSIntNestingCtr = 0;
#if configGENERATE_RUN_TIME_STATS
static uint32_t OSStatCPUUsage = 0;
static void CPUUsage_Callback(void *p_Tmr, void *arg);
#endif
#endif
#if VOSIPCModule_SUPPORT
#include "vos_ipc.h"
#endif

#undef ASSERT
#define ASSERT(en)                                                           \
    do                                                                       \
    {                                                                        \
        if (!(en))                                                           \
        {                                                                    \
            DEBUG_PRINTF("[OS]", "(%d)%s %s", __LINE__, #en, "failed!\r\n"); \
        }                                                                    \
    } while (0)

#define GET_HIDDEN_MEMBER(p_addr) (((p_addr) == NULL) ? NULL : ((void *)(p_addr + 1)))

sglist_t g_QueueRequestList = {0};
typedef struct
{
    void *msg_id;
    VOSQueue_t p_retmsg;
} VOSRequest;
static VOSMutex_t VOSReqMutex = NULL;

/* ================================================================================================================== */
/*                                                    EVENT FLAGS                                                     */
/* ================================================================================================================== */

/// @brief This function is called to create an event flag group. (Support internuclear communication)
/// @param p_name name of the event flag group
/// @param flags contains the initial value to store in the event flag group (typically 0).
/// @return event flag group handle (need to check whether it is NULL)
VOSEvent_t VOSFlagCreate(VOSName_t p_name, VOSEventFlags_t flags)
{
    VOSEvent_t p_osevent = NULL;
#if VOSIPCModule_SUPPORT
    if (p_name != NULL && p_name[0] == '2')
    {
        p_osevent = vmalloc(sizeof(struct vosevent_handle) + sizeof(void *));
        ipc_val_t *flags_handle = vipc_flags_create((SINT8 *)&p_name[1]);
        if (p_osevent == NULL || flags_handle == NULL)
        {
            if (p_osevent)
            {
                vfree(p_osevent);
            }
            if (flags_handle)
            {
                vipc_flags_del(flags_handle);
            }
            return NULL;
        }
        vipc_flags_set(flags_handle, flags);
        *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent) = flags_handle;
        p_osevent->transm = IPC_TRANSMISSION;
    }
    else
#endif
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        p_osevent = vmalloc(sizeof(struct vosevent_handle) + sizeof(OS_FLAG_GRP));
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        OSFlagCreate(p_grp, (CPU_CHAR *)p_name, (OS_FLAGS)flags, &err);
        if (err != OS_ERR_NONE || p_osevent == NULL || p_grp == NULL)
        {
            if (p_osevent)
            {
                vfree(p_osevent);
            }
            return NULL;
        }
        p_osevent->transm = NORMAL_TRANSMISSION;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        ASSERT((flags & 0xff000000UL) == 0);
        p_osevent = vmalloc(sizeof(struct vosevent_handle) + sizeof(StaticEventGroup_t));
        StaticEventGroup_t *pxEventGroupBuffer = GET_HIDDEN_MEMBER(p_osevent);
        EventGroupHandle_t event_handle = xEventGroupCreateStatic(pxEventGroupBuffer);
        if (event_handle == NULL || p_osevent == NULL || pxEventGroupBuffer == NULL)
        {
            if (p_osevent)
            {
                vfree(p_osevent);
            }
            return NULL;
        }
        p_osevent->transm = NORMAL_TRANSMISSION;
        if (flags != 0)
        {
            VOSFlagSet(p_osevent, flags);
        }
#endif
    }

    if (p_osevent != NULL)
    {
#if VOSPEND_MULTI_EN_SUPPORT
        p_osevent->pend_multi = NULL;
        p_osevent->bitmask = 0;
#endif
        p_osevent->name = p_name;
        vmemsetowner(p_osevent, __GET_RETURN_ADDR());
    }
    return p_osevent;
}

/// @brief This function deletes an event flag group and readies all tasks pending on the event flag group. (Support internuclear communication)
/// @param p_osevent event flag group handle
/// @return VRET_VALUE error code
VRET_VALUE VOSFlagDel(VOSEvent_t p_osevent)
{
    if (p_osevent == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    if (p_osevent->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_DEL_ALWAYS;
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        OSFlagDel(p_grp, opt, &err);
        if (err == OS_ERR_NONE)
        {
            __VERRNO_STORE(VERRNO_OS_STATE);
            vfree(p_osevent);
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        EventGroupHandle_t xEventGroup = GET_HIDDEN_MEMBER(p_osevent);
        vEventGroupDelete(xEventGroup);
        vfree(p_osevent);
        return VRET_NO_ERROR;
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osevent->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *flags_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent);
        if (flags_handle != NULL)
        {
            vipc_flags_del(flags_handle);
            vfree(p_osevent);
            return VRET_NO_ERROR;
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function is called to wait for a combination of bits to be set in an event flag group. (Support internuclear communication)
/// @param p_osevent event flag group handle
/// @param flags Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
/// @param osopt specifies whether you want ALL bits to be set or ANY of the bits to be set. view the features supported by enumerations
/// @param timeout Timeout duration of wait (ms),  set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return The flags in the event flag group that made the task ready or, 0 if a timeout or an error occurred.
VOSEventFlags_t VOSFlagWait(VOSEvent_t p_osevent, VOSEventFlags_t flags, VOSFlagOpt_t osopt, VOSTimeMs_t timeout)
{
    VOSEventFlags_t flag = 0;
    if (p_osevent == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return (VOSEventFlags_t)0;
    }
    if (p_osevent->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = (timeout == 0) ? OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING;
        OS_TICK time = (timeout == VOSMAX_DELAY) ? 0 : VOSTimeToTick(timeout);
        switch (osopt)
        {
        case VOSFLAG_ANY_CONSUME:
            opt |= OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME;
            break;
        case VOSFLAG_ANY_PEEK:
            opt |= OS_OPT_PEND_FLAG_SET_ANY;
            break;
        case VOSFLAG_ALL_CONSUME:
            opt |= OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME;
            break;
        case VOSFLAG_ALL_PEEK:
            opt |= OS_OPT_PEND_FLAG_SET_ALL;
            break;
        default:
            return (VOSEventFlags_t)0;
        }
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        flag = (VOSEventFlags_t)OSFlagPend(p_grp, (OS_FLAGS)flags, time, opt, (CPU_TS *)NULL, &err);
        if (err != OS_ERR_NONE)
        {
            __VERRNO_STORE(VERRNO_OS_STATE);
            return (VOSEventFlags_t)0;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        ASSERT((flags & 0xff000000UL) == 0);
        EventGroupHandle_t xEventGroup = GET_HIDDEN_MEMBER(p_osevent);
        BaseType_t xClearOnExit, xWaitForAllBits;
        switch (osopt)
        {
        case VOSFLAG_ANY_CONSUME:
            xClearOnExit = pdTRUE;
            xWaitForAllBits = pdFALSE;
            break;
        case VOSFLAG_ANY_PEEK:
            xClearOnExit = pdFALSE;
            xWaitForAllBits = pdFALSE;
            break;
        case VOSFLAG_ALL_CONSUME:
            xClearOnExit = pdTRUE;
            xWaitForAllBits = pdTRUE;
            break;
        case VOSFLAG_ALL_PEEK:
            xClearOnExit = pdFALSE;
            xWaitForAllBits = pdTRUE;
            break;
        default:
            return (VOSEventFlags_t)0;
        }
        flag = (VOSEventFlags_t)xEventGroupWaitBits(xEventGroup, (EventBits_t)flags, xClearOnExit, xWaitForAllBits, (TickType_t)VOSTimeToTick(timeout));
        flag &= flags;
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osevent->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *flags_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent);
        if (flags_handle != NULL)
        {
            ipc_valtype_t type;
            switch (osopt)
            {
            case VOSFLAG_ANY_CONSUME:
                type = VALUE_TYPE_FLAGS_CLR;
                break;
            case VOSFLAG_ANY_PEEK:
                type = VALUE_TYPE_FLAGS_SET;
                break;
            case VOSFLAG_ALL_CONSUME:
                type = VALUE_TYPE_FLAGS_CLRALL;
                break;
            case VOSFLAG_ALL_PEEK:
                type = VALUE_TYPE_FLAGS_SETALL;
                break;
            default:
                return (VOSEventFlags_t)0;
            }
            flag = (VOSEventFlags_t)vipc_flags_wait(flags_handle, flags, type, timeout);
        }
    }
#endif
#if VOSPEND_MULTI_EN_SUPPORT
    if (flag != 0 && p_osevent->pend_multi != NULL)
    {
        VRET_VALUE ret = VOSFlagSet(p_osevent->pend_multi, BIT_MASK(p_osevent->bitmask));
        if(ret != VRET_NO_ERROR) __VERRNO_STORE(VERRNO_OS_MULTI);
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return flag;
}

/// @brief This function is called to set some bits in an event flag group. (Support internuclear communication)
/// @param p_osevent event flag group handle
/// @param flags Is a bit pattern indicating which bit(s) (i.e. flags) you wish to set for.
/// @return VRET_VALUE error code
VRET_VALUE VOSFlagSet(VOSEvent_t p_osevent, VOSEventFlags_t flags)
{
    if (p_osevent == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    if (p_osevent->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_POST_FLAG_SET;
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        OSFlagPost(p_grp, (OS_FLAGS)flags, opt, &err);
        if (err != OS_ERR_NONE)
        {
            __VERRNO_STORE(VERRNO_OS_STATE);
            return VRET_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        ASSERT((flags & 0xff000000UL) == 0);
        EventGroupHandle_t xEventGroup = GET_HIDDEN_MEMBER(p_osevent);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;
            if (xEventGroupSetBitsFromISR(xEventGroup, (EventBits_t)flags, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                __VERRNO_STORE(VERRNO_OS_STATE);
                return VRET_ERROR;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            xEventGroupSetBits(xEventGroup, (EventBits_t)flags);
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osevent->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *flags_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent);
        if (flags_handle == NULL || 0 != vipc_flags_set(flags_handle, flags))
        {
            __VERRNO_STORE(VERRNO_OS_IPC);
            return VRET_ERROR;
        }
    }
#endif
    else
    {
        return VRET_ERROR;
    }
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osevent->pend_multi != NULL)
    {
        VRET_VALUE ret = VOSFlagSet(p_osevent->pend_multi, BIT_MASK(p_osevent->bitmask));
        if(ret != VRET_NO_ERROR) __VERRNO_STORE(VERRNO_OS_MULTI);
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is called to clear some bits in an event flag group. (Support internuclear communication)
/// @param p_osevent event flag group handle
/// @param flags Is a bit pattern indicating which bit(s) (i.e. flags) you wish to clear for.
/// @return VRET_VALUE error code
VRET_VALUE VOSFlagClr(VOSEvent_t p_osevent, VOSEventFlags_t flags)
{
    if (p_osevent == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    if (p_osevent->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_POST_FLAG_CLR;
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        OSFlagPost(p_grp, (OS_FLAGS)flags, opt, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        ASSERT((flags & 0xff000000UL) == 0);
        EventGroupHandle_t xEventGroup = GET_HIDDEN_MEMBER(p_osevent);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xResult;
            xResult = xEventGroupClearBitsFromISR(xEventGroup, (EventBits_t)flags);
            if (xResult == pdTRUE)
            {
                __VERRNO_STORE(VERRNO_OS_STATE);
                return VRET_ERROR;
            }
        }
        else
        {
            xEventGroupClearBits(xEventGroup, (EventBits_t)flags);
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osevent->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *flags_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent);
        if (flags_handle != NULL && 0 == vipc_flags_clr(flags_handle, flags))
        {
            return VRET_NO_ERROR;
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function aborts & readies any tasks currently waiting on an event flag group.  (Supports only intercore communication to unsuspend itself)
///        This function should be used to fault-abort the wait on the event flag group.
/// @param p_osevent event flag group handle
/// @return VRET_VALUE error code
VRET_VALUE VOSFlagPendAbort(VOSEvent_t p_osevent)
{
    if (p_osevent == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    if (p_osevent->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_PEND_ABORT_ALL;
        OS_FLAG_GRP *p_grp = GET_HIDDEN_MEMBER(p_osevent);
        OSFlagPendAbort(p_grp, opt, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        StaticEventGroup_t *pxEventGroupBuffer = GET_HIDDEN_MEMBER(p_osevent);
        ASSERT(pxEventGroupBuffer);
        if (pxEventGroupBuffer == NULL) return VRET_ERROR;
        while (pxEventGroupBuffer->xDummy2.uxDummy2 != 0)
        {
            TaskHandle_t UnblockedTCB = (TaskHandle_t)((UINT32 *)pxEventGroupBuffer->xDummy2.xDummy4.pvDummy3[0])[3];
            if (UnblockedTCB != NULL)
            {
                xTaskAbortDelay(UnblockedTCB);
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osevent->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *flags_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_osevent);
        if (flags_handle != NULL)
        {
            return VOSFlagPendAbort(flags_handle->p_osevent);
        }
    }
#endif
    return VRET_ERROR;
}

/* ================================================================================================================== */
/*                                             MUTUAL EXCLUSION SEMAPHORES                                            */
/* ================================================================================================================== */

/// @brief This function creates a mutex. It is used for resource protection to prevent priority reversal
/// @param p_name name of the mutex
/// @return mutex handle (need to check whether it is NULL)
VOSMutex_t VOSMutexCreate(VOSName_t p_name)
{
    VOSMutex_t p_osmutex = NULL;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
#if VOSIPCModule_SUPPORT
    if (p_name != NULL && p_name[0] == '2')
    {
        p_osmutex = vmalloc(sizeof(struct vosmutex_handle) + sizeof(OS_MUTEX) + sizeof(void *));
        OS_MUTEX *p_mutex = GET_HIDDEN_MEMBER(p_osmutex);
        OSMutexCreate(p_mutex, (CPU_CHAR *)p_name, &err);
        if (err != OS_ERR_NONE || p_osmutex == NULL || p_mutex == NULL)
        {
            if (p_osmutex)
            {
                vfree(p_osmutex);
            }
            return NULL;
        }
        ipc_val_t *mutex_handle = vipc_mutex_create((SINT8 *)&p_name[1]);
        if (mutex_handle == NULL)
        {
            OSMutexDel(p_mutex, OS_OPT_DEL_ALWAYS, &err);
            if (err == OS_ERR_NONE)
            {
                vfree(p_osmutex);
            }
            return NULL;
        }
        *(ipc_val_t **)GET_HIDDEN_MEMBER(p_mutex) = mutex_handle;
        p_osmutex->transm = IPC_TRANSMISSION;
    }
    else
#endif
    {
        p_osmutex = vmalloc(sizeof(struct vosmutex_handle) + sizeof(OS_MUTEX));
        OS_MUTEX *p_mutex = GET_HIDDEN_MEMBER(p_osmutex);
        OSMutexCreate(p_mutex, (CPU_CHAR *)p_name, &err);
        if (err != OS_ERR_NONE || p_osmutex == NULL || p_mutex == NULL)
        {
            if (p_osmutex)
            {
                vfree(p_osmutex);
            }
            return NULL;
        }
        p_osmutex->transm = NORMAL_TRANSMISSION;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#if VOSIPCModule_SUPPORT
    if (p_name != NULL && p_name[0] == '2')
    {
        p_osmutex = vmalloc(sizeof(struct vosmutex_handle) + sizeof(StaticSemaphore_t) + sizeof(void *));
        StaticSemaphore_t *pxMutexBuffer = GET_HIDDEN_MEMBER(p_osmutex);
        SemaphoreHandle_t xSemaphore = xSemaphoreCreateRecursiveMutexStatic(pxMutexBuffer);
        if (xSemaphore == NULL || p_osmutex == NULL || pxMutexBuffer == NULL)
        {
            if (p_osmutex)
            {
                vfree(p_osmutex);
            }
            return NULL;
        }
        ipc_val_t *mutex_handle = vipc_mutex_create((SINT8 *)&p_name[1]);
        if (mutex_handle == NULL)
        {
            vSemaphoreDelete(xSemaphore);
            vfree(p_osmutex);
            return NULL;
        }
        *(ipc_val_t **)GET_HIDDEN_MEMBER(pxMutexBuffer) = mutex_handle;
        p_osmutex->transm = IPC_TRANSMISSION;
    }
    else
#endif
    {
        p_osmutex = vmalloc(sizeof(struct vosmutex_handle) + sizeof(StaticSemaphore_t));
        StaticSemaphore_t *pxMutexBuffer = GET_HIDDEN_MEMBER(p_osmutex);
        SemaphoreHandle_t xSemaphore = xSemaphoreCreateRecursiveMutexStatic(pxMutexBuffer);
        if (xSemaphore == NULL || p_osmutex == NULL || pxMutexBuffer == NULL)
        {
            if (p_osmutex)
            {
                vfree(p_osmutex);
            }
            return NULL;
        }
        p_osmutex->transm = NORMAL_TRANSMISSION;
    }
#endif
    if (p_osmutex != NULL)
    {
        p_osmutex->name = p_name;
        vmemsetowner(p_osmutex, __GET_RETURN_ADDR());
    }
    return p_osmutex;
}

/// @brief This function deletes a mutex and readies all tasks pending on the mutex.
/// @param p_osmutex mutex handle
/// @return VRET_VALUE error code
VRET_VALUE VOSMutexDel(VOSMutex_t p_osmutex)
{
    if (p_osmutex == NULL)
    {
        return VRET_ERROR;
    }
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OS_OPT opt = OS_OPT_DEL_ALWAYS;
    OS_MUTEX *p_mutex = GET_HIDDEN_MEMBER(p_osmutex);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        OSMutexDel(p_mutex, opt, &err);
        if (err == OS_ERR_NONE)
        {
            vfree(p_osmutex);
            return VRET_NO_ERROR;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        OSMutexDel(p_mutex, opt, &err);
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_mutex);
        if (mutex_handle != NULL && err == OS_ERR_NONE && 0 == vipc_mutex_del(mutex_handle))
        {
            vfree(p_osmutex);
            return VRET_NO_ERROR;
        }
    }
#endif
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_osmutex);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        vSemaphoreDelete(xSemaphore);
        vfree(p_osmutex);
        return VRET_NO_ERROR;
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        vSemaphoreDelete(xSemaphore);
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER((StaticSemaphore_t *)xSemaphore);
        if (mutex_handle != NULL && 0 == vipc_mutex_del(mutex_handle))
        {
            vfree(p_osmutex);
            return VRET_NO_ERROR;
        }
    }
#endif
#endif
    return VRET_ERROR;
}

/// @brief This function lock for a mutex.(May need to wait)
/// @param p_osmutex mutex handle
/// @param timeout Timeout duration of wait (ms),  set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return VRET_VALUE error code
VRET_VALUE VOSMutexLock(VOSMutex_t p_osmutex, VOSTimeMs_t timeout)
{
    if (p_osmutex == NULL)
    {
        return VRET_ERROR;
    }
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OS_OPT opt = (timeout == 0) ? OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING;
    OS_TICK time = (timeout == VOSMAX_DELAY) ? 0 : VOSTimeToTick(timeout);
    OS_MUTEX *p_mutex = GET_HIDDEN_MEMBER(p_osmutex);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        OSMutexPend(p_mutex, time, opt, (CPU_TS *)NULL, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_mutex);
        if (mutex_handle != NULL)
        {
            OS_TICK starttick = OSTimeGet(&err);
            OSMutexPend(p_mutex, time, opt, (CPU_TS *)NULL, &err);
            if (err == OS_ERR_NONE && p_mutex->OwnerNestingCtr == 1)
            {
                if (timeout != VOSMAX_DELAY)
                {
                    time -= MIN(VOSTimeInterval(starttick, OSTimeGet(&err)), time);
                    timeout = VOSTickToTime(time);
                }
                if (0 != vipc_mutex_lock(mutex_handle, timeout))
                {
                    return VRET_NO_ERROR;
                }
                else
                {
                    OSMutexPost(p_mutex, OS_OPT_POST_NONE, &err);
                }
            }
        }
    }
#endif
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_osmutex);
    TickType_t xTicksToWait = VOSTimeToTick(timeout);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        if (xSemaphoreTakeRecursive(xSemaphore, xTicksToWait) == pdTRUE)
        {
            return VRET_NO_ERROR;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER((StaticSemaphore_t *)xSemaphore);
        if (mutex_handle != NULL)
        {
            TickType_t starttick = xTaskGetTickCount();
            if (xSemaphoreTakeRecursive(xSemaphore, xTicksToWait) == pdTRUE)
            {
                if (timeout != VOSMAX_DELAY)
                {
                    xTicksToWait -= MIN(VOSTimeInterval(starttick, xTaskGetTickCount()), xTicksToWait);
                    timeout = VOSTickToTime(xTicksToWait);
                }
                if (0 != vipc_mutex_lock(mutex_handle, timeout))
                {
                    return VRET_NO_ERROR;
                }
                else
                {
                    xSemaphoreGiveRecursive(xSemaphore);
                }
            }
        }
    }
#endif
#endif
    return VRET_ERROR;
}

/// @brief This function unlock for a mutex.
/// @param p_osmutex mutex handle
/// @return VRET_VALUE error code
VRET_VALUE VOSMutexUnlock(VOSMutex_t p_osmutex)
{
    if (p_osmutex == NULL)
    {
        return VRET_ERROR;
    }
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OS_OPT opt = OS_OPT_POST_NONE;
    OS_MUTEX *p_mutex = GET_HIDDEN_MEMBER(p_osmutex);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        OSMutexPost(p_mutex, opt, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_mutex);
        if (mutex_handle != NULL)
        {
            if (p_mutex->OwnerNestingCtr == 1)
            {
                if (0 == vipc_mutex_unlock(mutex_handle))
                {
                    OSMutexPost(p_mutex, opt, &err);
                    if (err == OS_ERR_NONE)
                    {
                        return VRET_NO_ERROR;
                    }
                }
            }
            else
            {
                OSMutexPost(p_mutex, opt, &err);
                if (err == OS_ERR_NONE)
                {
                    return VRET_NO_ERROR;
                }
            }
        }
    }
#endif
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_osmutex);
    if (p_osmutex->transm == NORMAL_TRANSMISSION)
    {
        if (xSemaphoreGiveRecursive(xSemaphore) == pdTRUE)
        {
            return VRET_NO_ERROR;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmutex->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *mutex_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER((StaticSemaphore_t *)xSemaphore);
        if (mutex_handle != NULL)
        {
            if (0 == vipc_mutex_unlock(mutex_handle))
            {
                if (xSemaphoreGiveRecursive(xSemaphore) == pdTRUE)
                {
                    return VRET_NO_ERROR;
                }
            }
        }
    }
#endif
#endif
    return VRET_ERROR;
}

/* ================================================================================================================== */
/*                                                     SEMAPHORES                                                     */
/* ================================================================================================================== */

/// @brief This function creates a semaphore. (Support internuclear communication)
/// @param p_name name of the semaphore
/// @param cnt is the initial value for the semaphore. If used to share resources, you should initialize to the number of resources available.
///             If used to signal the occurrence of event(s) then you should initialize to 0.
/// @return semaphore handle (need to check whether it is NULL)
VOSSemap_t VOSSemapCreate(VOSName_t p_name, VOSSemapCtr_t cnt)
{
    VOSSemap_t p_ossem = NULL;
#if VOSIPCModule_SUPPORT
    if (p_name != NULL && p_name[0] == '2')
    {
        p_ossem = vmalloc(sizeof(struct vossemap_handle) + sizeof(void *));
        ipc_val_t *sem_handle = vipc_semaphore_create((SINT8 *)&p_name[1]);
        if (p_ossem == NULL || sem_handle == NULL)
        {
            if (p_ossem)
            {
                vfree(p_ossem);
            }
            if (sem_handle)
            {
                vipc_semaphore_del(sem_handle);
            }
            return NULL;
        }
        *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem) = sem_handle;
        p_ossem->transm = IPC_TRANSMISSION;
    }
    else
#endif
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        p_ossem = vmalloc(sizeof(struct vossemap_handle) + sizeof(OS_SEM));
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        OSSemCreate(p_sem, (CPU_CHAR *)p_name, (OS_SEM_CTR)cnt, &err);
        if (err != OS_ERR_NONE || p_ossem == NULL || p_sem == NULL)
        {
            if (p_ossem)
            {
                vfree(p_ossem);
            }
            return NULL;
        }
        p_ossem->transm = NORMAL_TRANSMISSION;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        p_ossem = vmalloc(sizeof(struct vossemap_handle) + sizeof(StaticSemaphore_t));
        StaticSemaphore_t *pxSemaphoreBuffer = GET_HIDDEN_MEMBER(p_ossem);
        SemaphoreHandle_t xSemaphore = xSemaphoreCreateCountingStatic(0xFFFFFFFF, (UBaseType_t)cnt, pxSemaphoreBuffer);
        if (xSemaphore == NULL || p_ossem == NULL)
        {
            if (p_ossem)
            {
                vfree(p_ossem);
            }
            return NULL;
        }
        p_ossem->transm = NORMAL_TRANSMISSION;
#endif
    }
    if (p_ossem != NULL)
    {
#if VOSPEND_MULTI_EN_SUPPORT
        p_ossem->pend_multi = NULL;
        p_ossem->bitmask = 0;
#endif
        p_ossem->name = p_name;
        vmemsetowner(p_ossem, __GET_RETURN_ADDR());
    }
    return p_ossem;
}

/// @brief This function deletes a semaphore. (Support internuclear communication)
/// @param p_ossem semaphore handle
/// @return VRET_VALUE error code
VRET_VALUE VOSSemapDel(VOSSemap_t p_ossem)
{
    if (p_ossem == NULL)
    {
        return VRET_ERROR;
    }
    if (p_ossem->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_DEL_ALWAYS;
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        OSSemDel(p_sem, opt, &err);
        if (err == OS_ERR_NONE)
        {
            vfree(p_ossem);
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_ossem);
        vSemaphoreDelete(xSemaphore);
        vfree(p_ossem);
        return VRET_NO_ERROR;
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_ossem->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *sem_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem);
        if (sem_handle != NULL)
        {
            vipc_semaphore_del(sem_handle);
            vfree(p_ossem);
            return VRET_NO_ERROR;
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function take for a semaphore. (May need to wait) (Support internuclear communication)
/// @param p_ossem semaphore handle
/// @param timeout Timeout duration of wait (ms),  set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return VRET_VALUE error code
VRET_VALUE VOSSemapTake(VOSSemap_t p_ossem, VOSTimeMs_t timeout)
{
    if (p_ossem == NULL)
    {
        return VRET_ERROR;
    }
    if (p_ossem->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = (timeout == 0) ? OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING;
        OS_TICK time = (timeout == VOSMAX_DELAY) ? 0 : VOSTimeToTick(timeout);
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        OSSemPend(p_sem, time, opt, (CPU_TS *)NULL, &err);
        if (err != OS_ERR_NONE)
        {
            return VRET_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_ossem);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xSemaphoreTakeFromISR(xSemaphore, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                return VRET_ERROR;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            if (xSemaphoreTake(xSemaphore, (TickType_t)VOSTimeToTick(timeout)) != pdTRUE)
            {
                return VRET_ERROR;
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_ossem->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *sem_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem);
        if (sem_handle == NULL || 0 == vipc_semaphore_take(sem_handle, timeout))
        {
            return VRET_ERROR;
        }
    }
#endif
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_ossem->pend_multi != NULL && VOSSemapCtr(p_ossem) > 0)
    {
        VRET_VALUE ret = VOSFlagSet(p_ossem->pend_multi, BIT_MASK(p_ossem->bitmask));
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function give a semaphore. (Support internuclear communication)
/// @param p_ossem semaphore handle
/// @return VRET_VALUE error code
VRET_VALUE VOSSemapGive(VOSSemap_t p_ossem)
{
    if (p_ossem == NULL)
    {
        return VRET_ERROR;
    }
    if (p_ossem->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_POST_1;
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        OSSemPost(p_sem, opt, &err);
        if (err != OS_ERR_NONE)
        {
            return VRET_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_ossem);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                return VRET_ERROR;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            if (xSemaphoreGive(xSemaphore) != pdTRUE)
            {
                return VRET_ERROR;
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_ossem->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *sem_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem);
        if (sem_handle == NULL || 0 != vipc_semaphore_give(sem_handle))
        {
            return VRET_ERROR;
        }
    }
#endif
    else
    {
        return VRET_ERROR;
    }
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_ossem->pend_multi != NULL)
    {
        VRET_VALUE ret = VOSFlagSet(p_ossem->pend_multi, BIT_MASK(p_ossem->bitmask));
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function aborts & readies any tasks currently waiting on a semaphore. (Supports only intercore communication to unsuspend itself)
///         This function should be used to fault-abort the wait on the semaphore.
/// @param p_ossem semaphore handle
/// @return VRET_VALUE error code
VRET_VALUE VOSSemapPendAbort(VOSSemap_t p_ossem)
{
    if (p_ossem == NULL)
    {
        return VRET_ERROR;
    }
    if (p_ossem->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_PEND_ABORT_ALL;
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        OSSemPendAbort(p_sem, opt, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        StaticSemaphore_t *pxSemaphoreBuffer = GET_HIDDEN_MEMBER(p_ossem);
        ASSERT(pxSemaphoreBuffer);
        if (pxSemaphoreBuffer == NULL) return VRET_ERROR;
        while (pxSemaphoreBuffer->xDummy3[1].uxDummy2 != 0)
        {
            TaskHandle_t UnblockedTCB = (TaskHandle_t)((UINT32 *)pxSemaphoreBuffer->xDummy3[1].xDummy4.pvDummy3[0])[3];
            if (UnblockedTCB != NULL)
            {
                xTaskAbortDelay(UnblockedTCB);
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_ossem->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *sem_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem);
        if (sem_handle != NULL)
        {
            return VOSFlagPendAbort(sem_handle->p_osevent);
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function get the number of semaphore. (Support internuclear communication)
/// @param p_ossem semaphore handle
/// @return The current number of semaphore
VOSSemapCtr_t VOSSemapCtr(VOSSemap_t p_ossem)
{
    VOSSemapCtr_t ctr = 0;
    if (p_ossem == NULL)
    {
        return 0;
    }
    if (p_ossem->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_SEM *p_sem = GET_HIDDEN_MEMBER(p_ossem);
        ASSERT(p_sem);
        if (p_sem == NULL) return 0;
        ctr = p_sem->Ctr;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        SemaphoreHandle_t xSemaphore = GET_HIDDEN_MEMBER(p_ossem);
        ctr = uxSemaphoreGetCount(xSemaphore);
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_ossem->transm == IPC_TRANSMISSION)
    {
        ipc_val_t *sem_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(p_ossem);
        if (sem_handle != NULL)
        {
            ctr = sem_handle->handle_ctr->val;
        }
    }
#endif
    return ctr;
}

/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

/// @brief This function create a message queue. (Support internuclear communication)
/// @param p_name name of the message queue
/// @param max_qty indicates the maximum size of the message queue (must be non-zero).
///                 Note that it's also not possible to have a size higher than the maximum number of msg available.
/// @return message queue handle (need to check whether it is NULL)
VOSQueue_t VOSQueueCreate(VOSName_t p_name, VOSQueueNbr_t max_qty)
{
    VOSQueue_t p_osmsg = NULL;
#if VOSIPCModule_SUPPORT
    if (p_name != NULL && p_name[0] == '2')
    {
        p_osmsg = vmalloc(sizeof(struct vosqueue_handle) + sizeof(void *));
        ipc_que_t *que_handle = vipc_queue_create((SINT8 *)&p_name[1], max_qty);
        if (p_osmsg == NULL || que_handle == NULL)
        {
            if (p_osmsg)
            {
                vfree(p_osmsg);
            }
            if (que_handle)
            {
                vipc_queue_del(que_handle);
            }
            return NULL;
        }
        *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg) = que_handle;
        p_osmsg->transm = IPC_TRANSMISSION;
    }
    else
#endif
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        p_osmsg = vmalloc(sizeof(struct vosqueue_handle) + sizeof(OS_Q));
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        OSQCreate(p_q, (CPU_CHAR *)p_name, (OS_MSG_QTY)max_qty, &err);
        if (err != OS_ERR_NONE || p_osmsg == NULL || p_q == NULL)
        {
            if (p_osmsg)
            {
                vfree(p_osmsg);
            }
            return NULL;
        }
        p_osmsg->transm = NORMAL_TRANSMISSION;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        p_osmsg = vmalloc(sizeof(struct vosqueue_handle) + sizeof(StaticQueue_t) + max_qty * sizeof(void *));
        StaticQueue_t *pxQueueBuffer = GET_HIDDEN_MEMBER(p_osmsg);
        uint8_t *pucQueueStorageBuffer = GET_HIDDEN_MEMBER(pxQueueBuffer);
        QueueHandle_t xQueue = xQueueCreateStatic((UBaseType_t)max_qty, sizeof(void *), pucQueueStorageBuffer, pxQueueBuffer);
        if (xQueue == NULL || p_osmsg == NULL || pxQueueBuffer == NULL || pucQueueStorageBuffer == NULL)
        {
            if (p_osmsg)
            {
                vfree(p_osmsg);
            }
            return NULL;
        }
        p_osmsg->transm = NORMAL_TRANSMISSION;
#endif
    }
    if (p_osmsg != NULL)
    {
#if VOSPEND_MULTI_EN_SUPPORT
        p_osmsg->pend_multi = NULL;
        p_osmsg->bitmask = 0;
#endif
        p_osmsg->name = p_name;
        vmemsetowner(p_osmsg, __GET_RETURN_ADDR());
    }
    return p_osmsg;
}

/// @brief This function deletes a message queue and readies all tasks pending on the queue. (Support internuclear communication)
/// @param p_osmsg message queue handle
/// @return VRET_VALUE error code
VRET_VALUE VOSQueueDel(VOSQueue_t p_osmsg)
{
    if (p_osmsg == NULL)
    {
        return VRET_ERROR;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_DEL_ALWAYS;
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        OSQDel(p_q, opt, &err);
        if (err == OS_ERR_NONE)
        {
            vfree(p_osmsg);
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        vQueueDelete(xQueue);
        vfree(p_osmsg);
        return VRET_NO_ERROR;
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL)
        {
            vipc_queue_del(que_handle);
            vfree(p_osmsg);
            return VRET_NO_ERROR;
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function receive for a message to be sent to a queue. (May need to wait) (Support internuclear communication)
/// @param p_osmsg message queue handle
/// @param timeout Timeout duration of wait (ms), set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return is a pointer to the message received (need to check whether it is NULL)
void *VOSQueueReceive(VOSQueue_t p_osmsg, VOSTimeMs_t timeout)
{
    return VOSQueueReplyReceive(p_osmsg, NULL, timeout);
}

/// @brief This function sends a message to a queue. (put the end of the queue). (Support internuclear communication)
///         With the 'size' argument, you can do address send or copy data send.
/// @param p_osmsg message queue handle
/// @param p_void is a pointer to the message to send.
/// @param size specifies the size of the message (in bytes)
///             'size' == 0: do address send (nonSupport internuclear communication)
///             'size' != 0: copy data send (Support internuclear communication)
/// @return VRET_VALUE error code
VRET_VALUE VOSQueueSend(VOSQueue_t p_osmsg, void *p_void, size_t size)
{

    if (p_osmsg == NULL || p_void == NULL)
    {
        return VRET_ERROR;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        void *buff = p_void;
        OS_OPT opt = OS_OPT_POST_FIFO;
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        if (size > 0)
        {
            buff = vmalloc(size);
            if (NULL == buff)
            {
                return VRET_ERROR;
            }
            vmemcpy(buff, p_void, size);
        }
        OSQPost(p_q, buff, (OS_MSG_SIZE)size, opt, &err);
        if (err != OS_ERR_NONE)
        {
            return VRET_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        void *buff = p_void;
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        if (size > 0)
        {
            buff = vmalloc(size);
            if (NULL == buff)
            {
                return VRET_ERROR;
            }
            vmemcpy(buff, p_void, size);
        }
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xQueueSendFromISR(xQueue, &buff, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                return VRET_ERROR;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            if (xQueueSend(xQueue, &buff, 0) != pdTRUE)
            {
                return VRET_ERROR;
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        if (size == 0)
        {
            while (1)
            { // forbidden use
                ASSERT(0);
            }
        }
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle == NULL || 0 != vipc_queue_send(que_handle, p_void, size))
        {
            return VRET_ERROR;
        }
    }
#endif
    else
    {
        return VRET_ERROR;
    }
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osmsg->pend_multi != NULL && VOSQueueGetNbr(p_osmsg,VOSQUEUE_NBR_USED) > 0)
    {
        VRET_VALUE ret = VOSFlagSet(p_osmsg->pend_multi, BIT_MASK(p_osmsg->bitmask));
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function sends a message to a queue. (put the front of the queue). (Support internuclear communication)
///         With the 'size' argument, you can do address send or copy data send.
/// @param p_osmsg message queue handle
/// @param p_void is a pointer to the message to send.
/// @param size specifies the size of the message (in bytes)
///             'size' == 0: do address send (nonSupport internuclear communication)
///             'size' != 0: copy data send (Support internuclear communication)
/// @return VRET_VALUE error code
VRET_VALUE VOSQueueSendToFront(VOSQueue_t p_osmsg, void *p_void, size_t size)
{
    if (p_osmsg == NULL || p_void == NULL)
    {
        return VRET_ERROR;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        void *buff = p_void;
        OS_OPT opt = OS_OPT_POST_LIFO;
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        if (size > 0)
        {
            buff = vmalloc(size);
            if (NULL == buff)
            {
                return VRET_ERROR;
            }
            vmemcpy(buff, p_void, size);
        }
        OSQPost(p_q, buff, (OS_MSG_SIZE)size, opt, &err);
        if (err != OS_ERR_NONE)
        {
            return VRET_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        void *buff = p_void;
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        if (size > 0)
        {
            buff = vmalloc(size);
            if (NULL == buff)
            {
                return VRET_ERROR;
            }
            vmemcpy(buff, p_void, size);
        }
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xQueueSendToFrontFromISR(xQueue, &buff, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                return VRET_ERROR;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            if (xQueueSendToFront(xQueue, &buff, 0) != pdTRUE)
            {
                return VRET_ERROR;
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        if (size == 0)
        {
            while (1)
            { // forbidden use
                ASSERT(0);
            }
        }
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle == NULL || 0 != vipc_queue_sendtofront(que_handle, p_void, size))
        {
            return VRET_ERROR;
        }
    }
#endif
    else
    {
        return VRET_ERROR;
    }
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osmsg->pend_multi != NULL)
    {
        VRET_VALUE ret = VOSFlagSet(p_osmsg->pend_multi, BIT_MASK(p_osmsg->bitmask));
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function aborts & readies any tasks currently waiting on a queue.  (Supports only intercore communication to unsuspend itself)
///         This function should be used to fault-abort the wait on the queue.
/// @param p_osmsg message queue handle
/// @return VRET_VALUE error code
VRET_VALUE VOSQueuePendAbort(VOSQueue_t p_osmsg)
{
    if (p_osmsg == NULL)
    {
        return VRET_ERROR;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_OPT opt = OS_OPT_PEND_ABORT_ALL;
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        OSQPendAbort(p_q, opt, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        StaticQueue_t *pxQueueBuffer = GET_HIDDEN_MEMBER(p_osmsg);
        ASSERT(pxQueueBuffer);
        if (pxQueueBuffer == NULL) return VRET_ERROR;
        while (pxQueueBuffer->xDummy3[1].uxDummy2 != 0)
        {
            TaskHandle_t UnblockedTCB = (TaskHandle_t)((UINT32 *)pxQueueBuffer->xDummy3[1].xDummy4.pvDummy3[0])[3];
            if (UnblockedTCB != NULL)
            {
                xTaskAbortDelay(UnblockedTCB);
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL)
        {
            return VOSFlagPendAbort(que_handle->p_osevent);
        }
    }
#endif
    return VRET_ERROR;
}

/// @brief This function peek for a message to be sent to a queue. (Secretly view, does not affect the data in the queue) (nonSupport internuclear communication)
/// @param p_osmsg message queue handle
/// @return is a pointer to the message received (need to check whether it is NULL)
void *VOSQueuePeek(VOSQueue_t p_osmsg)
{
    void *p_msg = NULL;
    if (p_osmsg == NULL)
    {
        return NULL;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        ASSERT(p_q);
        if (p_q == NULL) return NULL;
        if (p_q->MsgQ.NbrEntries != 0u)
        {
            p_msg = p_q->MsgQ.OutPtr->MsgPtr;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        if (VOSIntNestingCtr > 0)
        {
            if (xQueuePeekFromISR(xQueue, &p_msg) != pdTRUE)
            {
                return NULL;
            }
        }
        else
        {
            if (xQueuePeek(xQueue, &p_msg, 0) != pdTRUE)
            {
                return NULL;
            }
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL && que_handle->handle_ctr->que.cnt != 0u)
        {
            p_msg = que_handle->handle_ctr->que.head;
        }
    }
#endif
    return p_msg;
}

/// @brief This function get number of messages. With the 'opt' argument, you can get quantities of different types.
/// @param p_osmsg message queue handle
/// @param opt determines the type of get number of messages.
/// @return  number of messages
VOSQueueNbr_t VOSQueueGetNbr(VOSQueue_t p_osmsg, VOSQueueOpt_t opt)
{
    VOSQueueNbr_t nbr = 0;
    if (p_osmsg == NULL)
    {
        return 0;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        switch (opt)
        {
        case VOSQUEUE_NBR_USED:
            nbr = p_q->MsgQ.NbrEntries;
            break;
        case VOSQUEUE_NBR_FREE:
            nbr = p_q->MsgQ.NbrEntriesSize - p_q->MsgQ.NbrEntries;
            break;
        case VOSQUEUE_NBR_SIZE:
            nbr = p_q->MsgQ.NbrEntriesSize;
            break;
        default:
            break;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        switch (opt)
        {
        case VOSQUEUE_NBR_USED:
            if (VOSIntNestingCtr > 0)
            {
                nbr = uxQueueMessagesWaitingFromISR(xQueue);
            }
            else
            {
                nbr = uxQueueMessagesWaiting(xQueue);
            }
            break;
        case VOSQUEUE_NBR_FREE:
            nbr = uxQueueSpacesAvailable(xQueue);
            break;
        case VOSQUEUE_NBR_SIZE:
            nbr = uxQueueAllLength(xQueue);
            break;
        default:
            break;
        }
#endif
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL)
        {
            switch (opt)
            {
            case VOSQUEUE_NBR_USED:
                nbr = que_handle->handle_ctr->que.cnt;
                break;
            case VOSQUEUE_NBR_FREE:
                nbr = que_handle->handle_ctr->msg_max - que_handle->handle_ctr->que.cnt;
                break;
            case VOSQUEUE_NBR_SIZE:
                nbr = que_handle->handle_ctr->msg_max;
                break;
            default:
                break;
            }
        }
    }
#endif
    return nbr;
}

/// @brief This function receive for a message to be sent to a queue. (May need to wait) (Support internuclear communication)
/// @param p_osmsg message queue handle
/// @param ret_handle message queue reply handle
/// @param timeout Timeout duration of wait (ms), set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return is a pointer to the message received (need to check whether it is NULL)
void *VOSQueueReplyReceive(VOSQueue_t p_osmsg, UINT32 *ret_handle, VOSTimeMs_t timeout)
{
    void *msg = NULL;
    if (p_osmsg == NULL)
    {
        return NULL;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_MSG_SIZE msg_size = 0;
        OS_OPT opt = (timeout == 0) ? OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING;
        OS_TICK time = (timeout == VOSMAX_DELAY) ? 0 : VOSTimeToTick(timeout);
        OS_Q *p_q = GET_HIDDEN_MEMBER(p_osmsg);
        msg = OSQPend(p_q, time, opt, &msg_size, (CPU_TS *)NULL, &err);
        if (err != OS_ERR_NONE)
        {
            return NULL;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        QueueHandle_t xQueue = GET_HIDDEN_MEMBER(p_osmsg);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xQueueReceiveFromISR(xQueue, &msg, &xHigherPriorityTaskWoken) != pdTRUE)
            {
                return NULL;
            }
            VOSYieldRequired += xHigherPriorityTaskWoken;
        }
        else
        {
            if (xQueueReceive(xQueue, &msg, (TickType_t)VOSTimeToTick(timeout)) != pdTRUE)
            {
                return NULL;
            }
        }
#endif
        if (ret_handle != NULL)
        {
            *ret_handle = (UINT32)msg;
        }
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL)
        {
            msg = vipc_return_recv(que_handle, ret_handle, timeout);
        }
    }
#endif
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osmsg->pend_multi != NULL && msg != NULL)
    {
        VRET_VALUE ret = VOSFlagSet(p_osmsg->pend_multi, BIT_MASK(p_osmsg->bitmask));
        ASSERT(ret == VRET_NO_ERROR);
    }
#endif
    return msg;
}

/// @brief This function sends a message to a queue and wait answer. (Support internuclear communication)
/// @param p_osmsg message queue handle
/// @param p_void is a pointer to the message to send.
/// @param size specifies the size of the message (in bytes) (It has to be non-zero)
/// @param timeout Timeout duration of wait (ms), set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return VRET_VALUE error code
void *VOSQueueRequest(VOSQueue_t p_osmsg, void *p_void, size_t size, VOSTimeMs_t timeout)
{
    void *msg = NULL;
    if (p_osmsg == NULL || p_void == NULL || size == 0)
    {
        return NULL;
    }
    if (p_osmsg->transm == NORMAL_TRANSMISSION)
    {
        VOSRequest *req = (VOSRequest *)sglist_data_new(sizeof(VOSRequest));
        VOSQueue_t p_ret = VOSQueueCreate("return", 1);
        if (req == NULL || p_ret == NULL)
        {
            if (req)
            {
                sglist_data_del((list_data_t *)req);
            }
            if (p_ret)
            {
                VOSQueueDel(p_ret);
            }
            return NULL;
        }
        req->msg_id = p_void;
        req->p_retmsg = p_ret;
        sglist_append_data(&g_QueueRequestList, (list_data_t *)req);
        if (VOSQueueSend(p_osmsg, p_void, size) == VRET_NO_ERROR)
        {
#if VOSPEND_MULTI_EN_SUPPORT
            if (p_osmsg->pend_multi != NULL)
            {
                VRET_VALUE ret = VOSFlagSet(p_osmsg->pend_multi, BIT_MASK(p_osmsg->bitmask));
                ASSERT(ret == VRET_NO_ERROR);
            }
#endif
            msg = VOSQueueReceive(p_ret, timeout);
            VOSMutexLock(VOSReqMutex, VOSMAX_DELAY);
            if (sglist_remove_data_at(&g_QueueRequestList, (list_data_t *)req) != NULL && msg == NULL)
            {
                msg = VOSQueueReceive(p_ret, 0);
            }
            VOSMutexUnlock(VOSReqMutex);
        }
        else
        {
            sglist_remove_data_at(&g_QueueRequestList, (list_data_t *)req);
            VOSQueueDel(p_ret);
        }
        sglist_data_del((list_data_t *)req);
    }
#if VOSIPCModule_SUPPORT
    else if (p_osmsg->transm == IPC_TRANSMISSION)
    {
        ipc_que_t *que_handle = *(ipc_que_t **)GET_HIDDEN_MEMBER(p_osmsg);
        if (que_handle != NULL)
        {
            msg = ipc_execute_callback(que_handle, p_void, size, timeout);
        }
    }
#endif
    return msg;
}

/// @brief This function sends a reply message to a queue. (Support internuclear communication)
/// @param msg_id message id. (The address of the msg received is used as the id to reply)
/// @param p_void is a pointer to the message to send.
/// @param size specifies the size of the message (in bytes)
///             'size' == 0: do address send (nonSupport internuclear communication)
///             'size' != 0: copy data send (Support internuclear communication)
/// @return VRET_VALUE error code
VRET_VALUE VOSQueueResponse(UINT32 msg_id, void *p_void, size_t size)
{
    VRET_VALUE ret = VRET_ERROR;
    if (p_void == NULL || msg_id == 0)
    {
        return VRET_ERROR;
    }
#if VOSIPCModule_SUPPORT
    if ((msg_id & BIT0) == 1)
    {
        if (size == 0)
        {
            while (1)
            { // forbidden use
                ASSERT(0);
            }
        }
        ret = ipc_send_return(msg_id, p_void, size);
    }
#endif
    else
    {
        VOSMutexLock(VOSReqMutex, VOSMAX_DELAY);
        VOSRequest *req = (VOSRequest *)sglist_remove_data_check(&g_QueueRequestList, (UINT32)msg_id);
        if (req != NULL)
        {
            ret = VOSQueueSend(req->p_retmsg, p_void, size);
        }
        VOSMutexUnlock(VOSReqMutex);
    }
    return ret;
}

/* ================================================================================================================== */
/*                                                    MULTI PEND                                                      */
/* ================================================================================================================== */

/// @brief This function creates multiple object wait handles.
///         (This function only allows you to pend on semaphores and/or message queues and/or event flag group).
/// @param p_name name of the multiple_objects.
/// @param multi_size is the number of waiting object. (Corresponds to the number of subsequent arguments passed)
/// @param obj the handle to the wait object. (Variable parameter operation is supported)
/// @param ... variable parameter.
/// @return multiple_objects handle (need to check whether it is NULL)
VOSMulti_t VOSMultiCreate(VOSName_t p_name, VOSMultiNbr_t multi_size, void *obj, ...)
{
    VOSMulti_t p_osmulti = NULL;
#if VOSPEND_MULTI_EN_SUPPORT
    VOSQueue_t *obj_list = NULL;
    VOSEvent_t p_grp = NULL;
    va_list args;

    if (sizeof(struct vosqueue_handle) != sizeof(struct vossemap_handle) || sizeof(struct vosqueue_handle) != sizeof(struct vosevent_handle))
    {
        while (1)
        { // data structure is inconsistent!!
            ASSERT(0);
        }
    }
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    if (multi_size == 0 || multi_size > 32 || obj == NULL)
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    if (multi_size == 0 || multi_size > 24 || obj == NULL)
#else
	if (multi_size == 0 || obj == NULL)
#endif
    {
        return NULL;
    }

    p_osmulti = vmalloc(sizeof(struct vosmulti_handle) + sizeof(VOSQueue_t) * multi_size);
    p_grp = VOSFlagCreate(p_name, BIT_MASK(multi_size) - 1);
    if (p_grp == NULL || p_osmulti == NULL)
    {
        if (p_osmulti)
        {
            vfree(p_osmulti);
        }
        if (p_grp)
        {
            VOSFlagDel(p_grp);
        }
        return NULL;
    }
    p_osmulti->pend_event = p_grp;
    obj_list = GET_HIDDEN_MEMBER(p_osmulti);

    va_start(args, obj);
    for (VOSMultiNbr_t i = 0; i < multi_size; i++)
    {
        if (i == 0)
        {
            obj_list[i] = (VOSQueue_t)obj;
        }
        else
        {
            obj_list[i] = (VOSQueue_t)va_arg(args, void *);
        }
        obj_list[i]->pend_multi = p_grp;
        obj_list[i]->bitmask = i;
#if VOSIPCModule_SUPPORT
        if (obj_list[i]->transm == IPC_TRANSMISSION)
        {
            ipc_val_t *que_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(obj_list[i]);
            if (que_handle != NULL)
            {
                que_handle->p_osevent->pend_multi = p_grp;
                que_handle->p_osevent->bitmask = i;
            }
        }
#endif
    }
    va_end(args);

    p_osmulti->multi_size = multi_size;
    p_osmulti->flag = 0;
    p_osmulti->name = p_name;
    vmemsetowner(p_osmulti, __GET_RETURN_ADDR());
#endif
    return p_osmulti;
}

/// @brief This function deletes a multiple_objects.
/// @param p_osmulti multiple_objects handle.
/// @return VRET_VALUE error code
VRET_VALUE VOSMultiDel(VOSMulti_t p_osmulti)
{
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osmulti != NULL)
    {
        VOSQueue_t *obj_list = GET_HIDDEN_MEMBER(p_osmulti);
        for (VOSMultiNbr_t i = 0; i < p_osmulti->multi_size; i++)
        {
            obj_list[i]->pend_multi = NULL;
            obj_list[i]->bitmask = 0;
#if VOSIPCModule_SUPPORT
            if (obj_list[i]->transm == IPC_TRANSMISSION)
            {
                ipc_val_t *que_handle = *(ipc_val_t **)GET_HIDDEN_MEMBER(obj_list[i]);
                if (que_handle != NULL)
                {
                    que_handle->p_osevent->pend_multi = NULL;
                    que_handle->p_osevent->bitmask = 0;
                }
            }
#endif
        }
        if (VRET_NO_ERROR != VOSFlagDel(p_osmulti->pend_event))
        {
            return VRET_ERROR;
        }
        vfree(p_osmulti);
        return VRET_NO_ERROR;
    }
#endif
    return VRET_ERROR;
}

/// @brief This function wait on multiple objects. If the object is found ready, then all available objects that are ready will be indicated to the caller.
/// @param p_osmulti multiple_objects handle.
/// @param timeout Timeout duration of wait (ms),  set VOSMAX_DELAY to permanent wait ,set 0 to no wait
/// @return is the number of the ready objec. (May not be ready)
VOSMultiNbr_t VOSMultiWait(VOSMulti_t p_osmulti, VOSTimeMs_t timeout)
{
    VOSMultiNbr_t nbr = (VOSMultiNbr_t)-1;
#if VOSPEND_MULTI_EN_SUPPORT
    if (p_osmulti != NULL)
    {
        if (p_osmulti->flag == 0)
        {
            p_osmulti->flag = VOSFlagWait(p_osmulti->pend_event, (VOSEventFlags_t)(-1), VOSFLAG_ANY_CONSUME, timeout);
            if (p_osmulti->flag == 0)
            {
                return (VOSMultiNbr_t)(-1);
            }
        }
        nbr = VCOMMON_LsBit(p_osmulti->flag);
        p_osmulti->flag &= ~BIT_MASK(nbr);
    }
#endif
    return nbr;
}

/// @brief This function filters ready items in multiple_objects.
/// @param p_osmulti multiple_objects handle.
/// @param obj object handle.
/// @return the queued message pointer or get semaphore
void *VOSMultiParse(VOSMulti_t p_osmulti, void *obj)
{
    //    if(p_osmulti != NULL){
    //        OS_PEND_DATA  *p_pend_data_tbl = GET_HIDDEN_MEMBER(p_osmulti);
    //        OS_OBJ_QTY     tbl_size = p_osmulti->multi_size;
    //        for(OS_OBJ_QTY i = 0;i < tbl_size;i++){
    //            if(p_pend_data_tbl[i].RdyObjPtr == GET_HIDDEN_MEMBER((struct vosqueue_handle*)obj)){
    //                switch(((OS_Q *)GET_HIDDEN_MEMBER((struct vosqueue_handle*)obj))->Type){
    //                    case OS_OBJ_TYPE_Q:
    //                        return p_pend_data_tbl[i].RdyMsgPtr;
    //                    case OS_OBJ_TYPE_SEM:
    //                        return (void*)TRUE;
    //                }
    //            }
    //        }
    //    }
    return NULL;
}

/* ================================================================================================================== */
/*                                                 TASK MANAGEMENT                                                    */
/* ================================================================================================================== */

/// @brief This function creates a task. Tasks can either be created prior to the start of multitasking or by a running task. A task cannot be created by an ISR.
/// @param p_name name of the task.
/// @param p_task is a pointer to the task's code.
/// @param p_arg task parameter pointer.
/// @param prio is the task's priority. A priority must be assigned to each task and the lower the number, the higher the priority.
/// @param stk_depth the size of the task stack. (in bytes)
/// @return task handle.
VOSTcb_t VOSTaskCreate(VOSName_t p_name, VOSTaskFunc_t p_task, void *p_arg, VOS_Prio_t prio, VOSStkDepth_t stk_depth)
{
    VOSTcb_t p_ostcb = NULL;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OS_OPT opt = OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR;
    CPU_STK_SIZE stk_size = stk_depth / sizeof(CPU_STK);
    p_ostcb = vmalloc(sizeof(struct vostcb_handle) + sizeof(OS_TCB) + stk_size * sizeof(CPU_STK));
    OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
    CPU_STK *p_stk = GET_HIDDEN_MEMBER(p_tcb);
    OSTaskCreate(p_tcb, (CPU_CHAR *)p_name, (OS_TASK_PTR)p_task, p_arg, (OS_PRIO)prio, p_stk, stk_size / 10, stk_size, 0, 0, NULL, opt, &err);
    if (err != OS_ERR_NONE || p_ostcb == NULL || p_tcb == NULL || p_stk == NULL)
    {
        if (p_ostcb)
        {
            vfree(p_ostcb);
        }
        return NULL;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    uint16_t usStackDepth = stk_depth / sizeof(StackType_t);
    if (prio < configMAX_PRIORITIES)
    {
        p_ostcb = vmalloc(sizeof(struct vostcb_handle) + sizeof(StaticTask_t) + usStackDepth * sizeof(StackType_t));
        StaticTask_t *pxTaskBuffer = GET_HIDDEN_MEMBER(p_ostcb);
        StackType_t *puxStackBuffer = GET_HIDDEN_MEMBER(pxTaskBuffer);
        TaskHandle_t task_handle = xTaskCreateStatic((TaskFunction_t)p_task, p_name, usStackDepth, p_arg, configMAX_PRIORITIES - prio - 1, puxStackBuffer, pxTaskBuffer);
        if (task_handle == NULL || p_ostcb == NULL || pxTaskBuffer == NULL || puxStackBuffer == NULL)
        {
            if (p_ostcb)
            {
                vfree(p_ostcb);
            }
            return NULL;
        }
    }
#endif
	if (p_ostcb != NULL)
    {
		p_ostcb->name = p_name;
		vmemsetowner(p_ostcb, __GET_RETURN_ADDR());
		VSYS_MEM_SetType(p_ostcb, 2);
    }
    return p_ostcb;
}

/// @brief This function delete a task.
/// @param p_ostcb task handle. can pass a NULL parameter to delete yourself.
/// @return VRET_VALUE error code
VRET_VALUE VOSTaskDel(VOSTcb_t p_ostcb)
{
    if (p_ostcb != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
        OSTaskDel(p_tcb, &err);
        if (err != OS_ERR_NONE && p_tcb->TaskState != (OS_STATE)OS_TASK_STATE_DEL)
        {
            return VRET_ERROR;
        }
        vfree(p_ostcb);
        return VRET_NO_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
        vTaskDelete(task_handle);
        vfree(p_ostcb);
        return VRET_NO_ERROR;
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called to resume a previously suspended task.
/// @param p_ostcb task handle.
/// @return VRET_VALUE error code
VRET_VALUE VOSTaskResume(VOSTcb_t p_ostcb)
{
    if (p_ostcb != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
        OSTaskResume(p_tcb, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
        if (VOSIntNestingCtr > 0)
        {
            VOSYieldRequired += xTaskResumeFromISR(task_handle);
        }
        else
        {
            vTaskResume(task_handle);
        }
        return VRET_NO_ERROR;
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called to suspend a task.
/// @param p_ostcb task handle. can pass a NULL parameter to suspend yourself.
/// @return VRET_VALUE error code
VRET_VALUE VOSTaskSuspend(VOSTcb_t p_ostcb)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
	OS_ERR err = OS_ERR_NONE;
	OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
	OSTaskSuspend(p_tcb, &err);
	if (err == OS_ERR_NONE)
	{
		return VRET_NO_ERROR;
	}
	return VRET_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
	TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
	vTaskSuspend(task_handle);
	return VRET_NO_ERROR;
#else
    return VRET_ERROR;
#endif
}

/// @brief  This function is called to get current state of a task.
/// @param p_ostcb task handle.
/// @return task state. Is a bit pattern indicating which state(s)
VOSTaskState_t VOSTaskGetState(VOSTcb_t p_ostcb)
{
    VOSTaskState_t state = VOSTASK_STATE_BIT_NONE;
    if (p_ostcb != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
        if (p_tcb->TaskState & OS_TASK_STATE_BIT_DLY)
        {
            state |= VOSTASK_STATE_BIT_DLY;
        }
        if (p_tcb->TaskState & OS_TASK_STATE_BIT_PEND)
        {
            state |= VOSTASK_STATE_BIT_PEND;
        }
        if (p_tcb->TaskState & OS_TASK_STATE_BIT_SUSPENDED)
        {
            state |= VOSTASK_STATE_BIT_SUSPENDED;
        }
        if (state == 0)
        {
            state |= VOSTASK_STATE_BIT_READY;
            if (OSTCBCurPtr)
            {
                if (p_ostcb == (VOSTcb_t)((UINT32)OSTCBCurPtr - sizeof(struct vostcb_handle)))
                    state |= VOSTASK_STATE_BIT_RUNNING;
            }
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
        eTaskState taskstate = eTaskGetState(task_handle);
        switch (taskstate)
        {
        case eReady:
            state |= VOSTASK_STATE_BIT_READY;
            break;
        case eRunning:
            state |= VOSTASK_STATE_BIT_RUNNING;
            break;
        case eBlocked:
            state |= VOSTASK_STATE_BIT_PEND;
            break;
        case eSuspended:
            state |= VOSTASK_STATE_BIT_SUSPENDED;
            break;
        default:
            break;
        }
#endif
    }
    return state;
}

/// @brief  This function is called to get current information of a task.
/// @param p_ostcb task handle.
/// @param inform get this type of information
/// @return task information.
UINT32 VOSTaskGetInform(VOSTcb_t p_ostcb, VOSTaskInform_t inform)
{
    UINT32 value = 0;
    if (p_ostcb != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
        switch (inform)
        {
        case VOSTASK_StkBasePtr:
            value = (UINT32)p_tcb->StkBasePtr;
            break;
        case VOSTASK_StkSize:
            value = (UINT32)p_tcb->StkSize * sizeof(CPU_STK);
            break;
        case VOSTASK_PrioBase:
            value = p_tcb->Prio;
            break;
        default:
            break;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
        StaticTask_t *pxTaskBuffer = GET_HIDDEN_MEMBER(p_ostcb);
        switch (inform)
        {
        case VOSTASK_StkBasePtr:
            value = (UINT32)pxTaskBuffer->pxDummy6;
            break;
        case VOSTASK_StkSize:
            value = (UINT32)pxTaskBuffer->pxDummy8 - (UINT32)pxTaskBuffer->pxDummy6 + sizeof( StackType_t );
            break;
        case VOSTASK_PrioBase:
            value = uxTaskPriorityGet(task_handle);
            break;
        default:
            break;
        }
#endif
    }
    return value;
}

/// @brief This function allows you to change the priority of a task dynamically. Note that the new priority must be available.
/// @param p_ostcb task handle.
/// @param prio_new is the new priority.
/// @return VRET_VALUE error code.
VRET_VALUE VOSTaskChangePrio(VOSTcb_t p_ostcb, VOS_Prio_t prio_new)
{
    if (p_ostcb != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        OS_ERR err = OS_ERR_NONE;
        OS_TCB *p_tcb = GET_HIDDEN_MEMBER(p_ostcb);
        OSTaskChangePrio(p_tcb, (OS_PRIO)prio_new, &err);
        if (err == OS_ERR_NONE)
        {
            return VRET_NO_ERROR;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        if (prio_new < configMAX_PRIORITIES)
        {
            TaskHandle_t task_handle = GET_HIDDEN_MEMBER(p_ostcb);
            vTaskPrioritySet(task_handle, configMAX_PRIORITIES - prio_new - 1);
            return VRET_NO_ERROR;
        }
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is get the task handle of the current running task.
/// @param  no parameter.
/// @return task handle. (return null when the kernel TCB is returned)
VOSTcb_t VOSTaskGetCur(void)
{
    VOSTcb_t p_ostcb = NULL;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
#if (OS_CFG_TASK_TICK_EN == DEF_ENABLED)
    extern OS_TCB OSTickTaskTCB;
    if (OSTCBCurPtr == &OSTickTaskTCB)
        goto kernel_tcb;
#endif
#if (OS_CFG_TASK_IDLE_EN == DEF_ENABLED)
    extern OS_TCB OSIdleTaskTCB;
    if (OSTCBCurPtr == &OSIdleTaskTCB)
        goto kernel_tcb;
#endif
    if (OSTCBCurPtr != NULL)
    {
        p_ostcb = (VOSTcb_t)((UINT32)OSTCBCurPtr - sizeof(struct vostcb_handle));
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    TaskHandle_t VOSCurrentTCB = xTaskGetCurrentTaskHandle();
    if (VOSCurrentTCB == xTaskGetIdleTaskHandle())
        goto kernel_tcb;
#if (configUSE_TIMERS == 1)
    if (VOSCurrentTCB == xTimerGetTimerDaemonTaskHandle())
        goto kernel_tcb;
#endif
    if (VOSCurrentTCB != NULL)
    {
        p_ostcb = (VOSTcb_t)((UINT32)VOSCurrentTCB - sizeof(struct vostcb_handle));
    }
#endif
kernel_tcb:
    return p_ostcb;
}

/* ================================================================================================================== */
/*                                                 TICK MANAGEMENT                                                    */
/* ================================================================================================================== */

/// @brief This function is called to delay execution of the currently task until the specified number of time(ms). (task switching can be triggered)
/// @param Ms delay time (in ms), Note that the system uses tick for delay, so there is precision error, and the current function delay is not accurate.
/// @return VRET_VALUE error code.
VRET_VALUE VOSTimeDly(VOSTimeMs_t Ms)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OS_OPT opt = OS_OPT_TIME_TIMEOUT;
    OSTimeDly((OS_TICK)VOSTimeToTick(Ms), opt, &err);
    if (err != OS_ERR_NONE)
    {
        return VRET_ERROR;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    vTaskDelay((TickType_t)VOSTimeToTick(Ms));
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is called to get system time (in ms).
/// @param  no parameter.
/// @return system time (in ms)
VOSTimeMs_t VOSTimeGet(void)
{
    uint32_t tick = 0;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    tick = (uint32_t)OSTimeGet(&err);
    if (err != OS_ERR_NONE)
    {
        return 0;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    if (VOSIntNestingCtr > 0)
    {
        tick = xTaskGetTickCountFromISR();
    }
    else
    {
        tick = xTaskGetTickCount();
    }
#endif
    return VOSTickToTime(tick);
}

/// @brief This function is called to Calculating system time interval(in ms).
/// @param ms_start start system time
/// @param ms_end end system time
/// @return system time interval(in ms)
VOSTimeMs_t VOSTimeInterval(VOSTimeMs_t ms_start, VOSTimeMs_t ms_end)
{
    return (ms_end - ms_start);
}

/// @brief This function is called to converts system time (in ms) to system beats (in tick).
/// @param ms system time (in ms).
/// @return system beats (in tick).
uint32_t VOSTimeToTick(VOSTimeMs_t ms)
{ // The parameter ms unit is coarse and is not incrementing
    uint32_t tick = 0;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    if (ms == 0)
    {
        tick = 0;
    }
    else if (ms <= 1000 / OS_CFG_TICK_RATE_HZ)
    {
        tick = 1;
    }
    else
    {
        tick = (ms + 500 / OS_CFG_TICK_RATE_HZ) * OS_CFG_TICK_RATE_HZ / 1000;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    if (ms == VOSMAX_DELAY)
    {
        tick = VOSMAX_DELAY;
    }
    else if (ms == 0)
    {
        tick = 0;
    }
    else if (ms <= 1000 / configTICK_RATE_HZ)
    {
        tick = 1;
    }
    else
    {
        tick = (ms + 500 / configTICK_RATE_HZ) * configTICK_RATE_HZ / 1000;
    }
#endif
    return tick;
}

/// @brief This function is called to converts system beats (in tick) to system time (in ms).
/// @param tick system beats (in tick).
/// @return system time (in ms).
VOSTimeMs_t VOSTickToTime(uint32_t tick)
{
    unsigned int ms = 0;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    ms = tick * (1000 / OS_CFG_TICK_RATE_HZ);
    if (1000 / OS_CFG_TICK_RATE_HZ > 1 && ms < tick)
    {
        while (1)
        {
            // Excessive value
            ASSERT(0);
        }
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    ms = tick * (1000 / configTICK_RATE_HZ);
    if (1000 / configTICK_RATE_HZ > 1 && ms < tick)
    {
        while (1)
        {
            // Excessive value
            ASSERT(0);
        }
    }
#endif
    return ms;
}

/* ================================================================================================================== */
/*                                                    MISCELLANEOUS                                                   */
/* ================================================================================================================== */

/// @brief This function is used to initialize the internals of vkernel RTOS and must be called prior to creating any object and, prior to calling VOSStart().
/// @param  no parameter.
/// @return VRET_VALUE error code.
VRET_VALUE VOSInit(void)
{
    SYSVIEW_TRACE_INIT();
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OSInit(&err);
    if (err != OS_ERR_NONE)
    {
        return VRET_ERROR;
    }
    VSYS_TMR_Init();
    if (!VOSTaskCreate("VOS Task SysTimer", (VOSTaskFunc_t)VOS_TimerTask, 0, VOS_CFG_TIMER_TASK_PRIO, VOS_CFG_TIMER_TASK_STK_SIZE))
    {
        return VRET_ERROR;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    #if configGENERATE_RUN_TIME_STATS
    VOSTimer_t timer = VOSTimerCreate("CPUUsage",100,100,CPUUsage_Callback, NULL);
    if (timer)
        VOSTimerStart(timer);
    #endif
#endif
#if VOSIPCModule_SUPPORT
    if (VRET_NO_ERROR != vipc_service_init())
    {
        return VRET_ERROR;
    }
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is used to start the multitasking process which lets vkernel RTOS manages the task that you created. Before you can call OSStart(), you MUST have created at least one application task.
/// @param  no parameter.
/// @return VRET_VALUE error code.
VRET_VALUE VOSStart(void)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    VCPU_SysTick_Config(VOSTickToTime(1));
	VCPU_INT_SrcEnable(IRQ_SYSSW_INT);
    VCPU_INT_SrcEnable(IRQ_SYSTICK_INT);
    OSStart(&err);
    if (err != OS_ERR_NONE)
    {
        return VRET_ERROR;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    vTaskStartScheduler();
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is prevent task switching.
/// @param  no parameter.
/// @return VRET_VALUE error code.
VRET_VALUE VOSSchedLock(void)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OSSchedLock(&err);
    if (err != OS_ERR_NONE)
    {
        return VRET_ERROR;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    vTaskSuspendAll();
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is recover task switching.
/// @param  no parameter.
/// @return VRET_VALUE error code.
VRET_VALUE VOSSchedUnlock(void)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    OSSchedUnlock(&err);
    if (err != OS_ERR_NONE)
    {
        return VRET_ERROR;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    xTaskResumeAll();
#endif
    return VRET_NO_ERROR;
}

/// @brief This function is called to get current state of a scheduler.
/// @param  no parameter.
/// @return scheduler state. Is a bit pattern indicating which state(s)
VOSSchedState_t VOSSchedGetState(void)
{
    VOSSchedState_t state = VOSSCHED_STATE_BIT_NONE;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    if (OSRunning == OS_STATE_OS_STOPPED)
    {
        state |= VOSSCHED_STATE_BIT_STOPPED;
    }
    else if (OSRunning == OS_STATE_OS_RUNNING)
    {
        if (OSSchedLockNestingCtr > 0)
        {
            state |= VOSSCHED_STATE_BIT_SUSPENDED;
        }
        state |= VOSSCHED_STATE_BIT_RUNNING;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    BaseType_t scheduler_state = xTaskGetSchedulerState();
    if (scheduler_state == taskSCHEDULER_RUNNING)
    {
        state |= VOSSCHED_STATE_BIT_RUNNING;
    }
    else if (scheduler_state == taskSCHEDULER_SUSPENDED)
    {
        state |= VOSSCHED_STATE_BIT_SUSPENDED | VOSSCHED_STATE_BIT_RUNNING;
    }
    else if (scheduler_state == taskSCHEDULER_NOT_STARTED)
    {
        state |= VOSSCHED_STATE_BIT_STOPPED;
    }
#endif
    return state;
}

/// @brief  This function is used to notify vkernel RTOS that you are about to service an interrupt service routine (ISR).
/// @param  no parameter.
void VOSIntEnter(void)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OSIntEnter();
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    VOSIntNestingCtr++;
    traceISR_ENTER();
#endif
}

/// @brief This function is used to notify vkernel RTOS that you have completed servicing an ISR.
/// @param  no parameter.
void VOSIntExit(void)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OSIntExit();
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    if (--VOSIntNestingCtr == 0)
    {
        portYIELD_FROM_ISR(VOSYieldRequired);
        if (VOSYieldRequired != pdFALSE)
        {
            traceISR_EXIT_TO_SCHEDULER();
        }
        else
        {
            traceISR_EXIT();
        }
        VOSYieldRequired = 0;
    }
    else
    {
        traceISR_EXIT();
    }
#endif
}

/// @brief This function is called to get CPU usage rate.
/// @param  no parameter.
/// @return CPU usage rate. (units are in %)
uint32_t VOSGetCPUUsage(void)
{
    uint32_t usage = 0;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
    usage = (uint32_t)OSStatTaskCPUUsage;
#endif
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#if configGENERATE_RUN_TIME_STATS
	usage = (uint32_t)OSStatCPUUsage;
#endif
#endif
    return usage;
}

/* ================================================================================================================== */
/*                                                    HOOKS MANAGEMENT                                                */
/* ================================================================================================================== */

/// @brief This function is called to register various types of hook functions.
/// @param hook_type The type scenario of the hook function.
/// @param func is a pointer to the hook function.
void VOSHooksRegister(VOSHookType_t hook_type, VOS_CALLBACK_ARG func)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
#if OS_CFG_APP_HOOKS_EN > 0u
    switch (hook_type)
    {
    case VOSHOOK_TYPE_TASKCREATE:
        OS_AppTaskCreateHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_TASKDEL:
        OS_AppTaskDelHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_TASKRETURN:
        OS_AppTaskReturnHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_IDLETASK:
        OS_AppIdleTaskHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_STATTASK:
#if OS_CFG_STAT_TASK_EN > 0u
        OSStatTaskCPUUsageInit(&err);
#endif
        OS_AppStatTaskHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_TASKSW:
        OS_AppTaskSwHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_TIMETICK:
        OS_AppTimeTickHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_REDZONEHIT:
#if OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED
        OS_AppRedzoneHitHookPtr = (OS_APP_HOOK_TCB)func;
#endif
        break;
    default:
        break;
    }
#endif

#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    extern void (*vApplicationIdleHookPtr)(void);
    extern int (*vApplicationTimeTickHookPtr)(void);
    switch (hook_type)
    {
    case VOSHOOK_TYPE_TASKCREATE:
        // OS_AppTaskCreateHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_TASKDEL:
        // OS_AppTaskDelHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_TASKRETURN:
        // OS_AppTaskReturnHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    case VOSHOOK_TYPE_IDLETASK:
#if (configUSE_IDLE_HOOK == 1)
        vApplicationIdleHookPtr = (void (*)(void))func;
#endif
        break;
    case VOSHOOK_TYPE_STATTASK:
        // OS_AppStatTaskHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_TASKSW:
        // OS_AppTaskSwHookPtr = (OS_APP_HOOK_VOID)func;
        break;
    case VOSHOOK_TYPE_TIMETICK:
        vApplicationTimeTickHookPtr = (int (*)(void))func;
        break;
    case VOSHOOK_TYPE_REDZONEHIT:
        // OS_AppRedzoneHitHookPtr = (OS_APP_HOOK_TCB)func;
        break;
    default:
        break;
    }
#endif
}

/* ================================================================================================================== */
/*                                                 TIMER MANAGEMENT                                                   */
/*   Do not use OS-provided                                                                                           */
/* ================================================================================================================== */

/// @brief This function is called by your application code to create a timer.
/// @param p_name name of the timer.
/// @param init_ms Initial delay (in ms).
/// @param period_ms repeated delay for the timer (in ms). Note when the value is 0, repeated timing is not enabled.
/// @param p_callback Is a pointer to a callback function that will be called when the timer expires.
/// @param arg Is an argument (a pointer) that is passed to the callback function when it is called.
/// @return timer handle (need to check whether it is NULL)
VOSTimer_t VOSTimerCreate(VOSName_t p_name, VOSTimeMs_t init_ms, VOSTimeMs_t period_ms, VOS_CALLBACK_TIMER p_callback, void *arg)
{
    VOSTimer_t p_ostimer = NULL;
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    UINT32 ret = 0;
    p_ostimer = vmalloc(sizeof(struct vostimer_handle) + sizeof(VSYS_TMR_DATA));
    VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);

    ret = VSYS_TMR_Create_Static(tmr, init_ms, period_ms, p_callback, arg);
    if (ret != 0 || p_ostimer == NULL || tmr == NULL)
    {
        if (p_ostimer)
        {
            vfree(p_ostimer);
        }
        return NULL;
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    ASSERT(period_ms == 0 || init_ms == period_ms);
    p_ostimer = vmalloc(sizeof(struct vostimer_handle) + sizeof(StaticTimer_t));
    StaticTimer_t *pxTimerBuffer = GET_HIDDEN_MEMBER(p_ostimer);
    UBaseType_t xAutoReload = period_ms ? pdTRUE : pdFALSE;

    TimerHandle_t xTimer = xTimerCreateStatic(p_name, VOSTimeToTick(init_ms), xAutoReload, arg, (TimerCallbackFunction_t)p_callback, pxTimerBuffer);
    if (xTimer == NULL || p_ostimer == NULL || pxTimerBuffer == NULL)
    {
        if (p_ostimer)
        {
            vfree(p_ostimer);
        }
        return NULL;
    }
#endif
    if (p_ostimer != NULL)
    {
        p_ostimer->name = p_name;
        vmemsetowner(p_ostimer, __GET_RETURN_ADDR());
    }
    return p_ostimer;
}

/// @brief This function is called by your application code to delete a timer.
/// @param p_ostimer timer handle
/// @return VRET_VALUE error code.
VRET_VALUE VOSTimerDel(VOSTimer_t p_ostimer)
{
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        VSYS_TMR_Delete_Static(tmr);
        vfree(p_ostimer);
        return VRET_NO_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (xTimerDelete(xTimer, 0) == pdPASS)
        {
            vfree(p_ostimer);
            return VRET_NO_ERROR;
        }
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called by your application code to reinstall a timer.
/// @param p_ostimer timer handle.
/// @param init_ms Initial delay (in ms).
/// @param period_ms repeated delay for the timer (in ms). Note when the value is 0, repeated timing is not enabled.
/// @param p_callback Is a pointer to a callback function that will be called when the timer expires.
/// @param arg Is an argument (a pointer) that is passed to the callback function when it is called.
/// @return VRET_VALUE error code.
VRET_VALUE VOSTimerSet(VOSTimer_t p_ostimer, VOSTimeMs_t init_ms, VOSTimeMs_t period_ms, VOS_CALLBACK_TIMER p_callback, void *arg)
{
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        if (p_callback == NULL)
        {
            p_callback = tmr->pfn_callback;
            arg = tmr->p_callback_args;
        }
        VSYS_TMR_ChangeSetting(tmr, init_ms, period_ms, p_callback, arg);
        return VRET_NO_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        ASSERT(arg == NULL && (period_ms == 0 || init_ms == period_ms) && p_callback == NULL);
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xTimerChangePeriodFromISR(xTimer, VOSTimeToTick(init_ms), &xHigherPriorityTaskWoken) == pdTRUE)
            {
                VOSYieldRequired += xHigherPriorityTaskWoken;
                return VRET_NO_ERROR;
            }
        }
        else
        {
            if (xTimerChangePeriod(xTimer, VOSTimeToTick(init_ms), 0) == pdPASS)
            {
                return VRET_NO_ERROR;
            }
        }
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called to get the number of time before a timer times out.
/// @param p_ostimer timer handle.
/// @return number of time before a timer times out.
VOSTimeMs_t VOSTimerRemainGet(VOSTimer_t p_ostimer)
{
    VOSTimeMs_t remain = 0;
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        remain = VSYS_TMR_GetRemainderTime(tmr);
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (xTimerIsTimerActive(xTimer) != pdFALSE)
        {
            remain =VOSTickToTime(xTimerGetExpiryTime(xTimer) - xTaskGetTickCount());
        }
#endif
    }
    return remain;
}

/// @brief This function is called by your application code to start a timer.
/// @param p_ostimer timer handle.
/// @return VRET_VALUE error code.
VRET_VALUE VOSTimerStart(VOSTimer_t p_ostimer)
{
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        VSYS_TMR_Start(tmr);
        return VRET_NO_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xTimerStartFromISR(xTimer, &xHigherPriorityTaskWoken) == pdTRUE)
            {
                VOSYieldRequired += xHigherPriorityTaskWoken;
                return VRET_NO_ERROR;
            }
        }
        else
        {
            if (xTimerStart(xTimer, 0) == pdTRUE)
            {
                return VRET_NO_ERROR;
            }
        }
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called by your application code to stop a timer.
/// @param p_ostimer timer handle.
/// @return VRET_VALUE error code.
VRET_VALUE VOSTimerStop(VOSTimer_t p_ostimer)
{
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        VSYS_TMR_Stop(tmr);
        return VRET_NO_ERROR;
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (VOSIntNestingCtr > 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xTimerStopFromISR(xTimer, &xHigherPriorityTaskWoken) == pdTRUE)
            {
                VOSYieldRequired += xHigherPriorityTaskWoken;
                return VRET_NO_ERROR;
            }
        }
        else
        {
            if (xTimerStop(xTimer, 0) == pdTRUE)
            {
                return VRET_NO_ERROR;
            }
        }
#endif
    }
    return VRET_ERROR;
}

/// @brief This function is called to get current state of a timer.
/// @param p_ostimer timer handle.
/// @return timer state. Is a bit pattern indicating which state(s)
VOSTimerState_t VOSTimerGetState(VOSTimer_t p_ostimer)
{
    VOSTimerState_t state = VOSTIMER_STATE_BIT_NONE;
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        if (tmr == NULL) return VOSTIMER_STATE_BIT_NONE;
        if (tmr->state == VSYS_TMR_STATE_STOPPED)
        {
            state |= VOSTIMER_STATE_BIT_STOPPED;
        }
        else if (tmr->state == VSYS_TMR_STATE_RUNNING)
        {
            state |= VOSTIMER_STATE_BIT_RUNNING;
        }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        if (xTimerIsTimerActive(xTimer) != pdFALSE)
        {
            state |= VOSTIMER_STATE_BIT_RUNNING;
        }
        else
        {
            state |= VOSTIMER_STATE_BIT_STOPPED;
        }
#endif
    }
    return state;
}

/// @brief This function is get whether the current timer parameter.
/// @param p_ostimer timer handle.
/// @return argument (a pointer).
void *VOSTimerArgGet(VOSTimer_t p_ostimer)
{
    if (p_ostimer != NULL)
    {
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        VSYS_TMR_DATA *tmr = GET_HIDDEN_MEMBER(p_ostimer);
        if (tmr == NULL) return NULL;
        return (tmr->p_callback_args);
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
        TimerHandle_t xTimer = GET_HIDDEN_MEMBER(p_ostimer);
        return (pvTimerGetTimerID(xTimer));
#endif
    }
    return NULL;
}

/// @brief This function is synchronize the system timer clock again to ensure the accuracy of the timer.
/// @param ms synchronization deviation time (in ms).
void VOSTimerDynTick(VOSTimeMs_t ms)
{
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
    OS_ERR err = OS_ERR_NONE;
    uint32_t ticks = 0;

    VSYS_TMR_Synchronization(ms);

    ticks = VOSTimeToTick(ms);
    while (ticks--)
    {
        OSTaskSemPost(&OSTickTaskTCB, OS_OPT_POST_NONE, &err);
    }
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
    xTaskCatchUpTicks(VOSTimeToTick(ms));
#endif
}

#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
static void VOS_TimerTask(void)
{
    VSYS_TMR_Check();
}
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#if configGENERATE_RUN_TIME_STATS
static void CPUUsage_Callback(void *p_Tmr, void *arg)
{
    uint32_t CPUUsage = 0;
	static uint32_t AlltimeCount_prev = 0, IdletimeCount_prev = 0;
	uint32_t AlltimeCount = 0, IdletimeCount = 0;
	
	UNUSED(p_Tmr);
	UNUSED(arg);
	AlltimeCount = portGET_RUN_TIME_COUNTER_VALUE();
	IdletimeCount = xTaskGetIdleRunTimeCounter();
	
	if (AlltimeCount != AlltimeCount_prev)
	{
		CPUUsage = (uint64_t)10000 * (IdletimeCount - IdletimeCount_prev)/ (AlltimeCount - AlltimeCount_prev);
		if (CPUUsage > 10000) CPUUsage = 0;
		else CPUUsage = 10000 - CPUUsage;
		AlltimeCount_prev = AlltimeCount;
		IdletimeCount_prev = IdletimeCount;
	}
    OSStatCPUUsage = CPUUsage;
    VSYS_ResInfo_Update(NULL, TRUE);
}
#endif
#endif

/* ================================================================================================================== */
/*                                                    CRITICAL SECTION                                                */
/* ================================================================================================================== */

static UINT32 VOSCriticalNesting = 0;
static UINT32 VOSCpuSrValue = 0;

/// @brief This function is called to the system enters the critical section.
/// @param  no parameter.
void VOS_CRITICAL_ENTER(void)
{
    if (VOSCriticalNesting == 0)
    {
        VOSCpuSrValue = VCPU_SR_Save();
    }
    VOSCriticalNesting++;
    if (VOSCriticalNesting == (UINT32)-1)
    {
        while (1)
        {
            ASSERT(0);
        };
    }
}

/// @brief This function is called to the system exits the critical section.
/// @param  no parameter.
void VOS_CRITICAL_EXIT(void)
{
    VOSCriticalNesting--;
    if (VOSCriticalNesting == 0)
    {
        VCPU_SR_Restore(VOSCpuSrValue);
    }
    if (VOSCriticalNesting == (UINT32)-1)
    {
        while (1)
        {
            ASSERT(0);
        };
    }
}

/* ================================================================================================================== */
/*                                                 INTERRUPT SERVICING                                                */
/* ================================================================================================================== */
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
#if (defined __GNUC__)
extern void Software_IRQHandler(void);
__IRQ void VISR_PendSV(void)
{
	__ASM volatile( "j Software_IRQHandler" );
}
__IRQ void VISR_SysTick(void)
{
	extern void  OS_CPU_SysTickHandler (void);
    VCPU_SysTick_Reload(VOSTickToTime(1));
    OS_CPU_SysTickHandler();
}
#elif (defined __CC_ARM)
__IRQ void __ASM VISR_PendSV(void){
    IMPORT OS_CPU_PendSVHandler
        B OS_CPU_PendSVHandler} __IRQ void __ASM VISR_SysTick(void){
    IMPORT OS_CPU_SysTickHandler
        B OS_CPU_SysTickHandler}
#endif
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#if (defined __GNUC__)
__IRQ void VISR_PendSV(void)
{
	__ASM volatile( "j freertos_risc_software" );
}
__IRQ void VISR_SysTick(void)
{
	extern int (*vApplicationTimeTickHookPtr)(void);
    VCPU_SysTick_Reload(VOSTickToTime(1));
	if (vApplicationTimeTickHookPtr)
	{ 
		/* 由于定时器资源不足，部分功能需要共用systick定时器。
		   当vApplicationTimeTickHookPtr返回 0，表示计时值到达操作系统调度时间，需要执行操作系统调度
		 */
		if (vApplicationTimeTickHookPtr()) return;
	}
	
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known - therefore the slightly faster vPortRaiseBASEPRI() function is used
	in place of portSET_INTERRUPT_MASK_FROM_ISR(). */
	traceISR_ENTER();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portYIELD();
			traceISR_EXIT_TO_SCHEDULER();
		}else{
			traceISR_EXIT();
		}
	}
}
#elif (defined __CC_ARM)
__IRQ void __ASM VISR_SVCall(void){
    IMPORT vPortSVCHandler
        B vPortSVCHandler} __IRQ void __ASM VISR_PendSV(void){
    IMPORT xPortPendSVHandler
        B xPortPendSVHandler} __IRQ void __ASM VISR_SysTick(void)
{
    IMPORT xPortSysTickHandler
        B xPortSysTickHandler
}
#endif
#endif
