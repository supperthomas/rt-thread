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
	 2022.11.16             cxt              initial version
------------------------------------------------------------------------------*/
#ifndef _VSTDOS_H_
#define _VSTDOS_H_
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDOS_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES */
#include "vstdcfg.h"
#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "vstdcore.h"

#define VOS_CFG_TIMER_TASK_PRIO 3
#define VOS_CFG_TIMER_TASK_STK_SIZE (380 * 4)

typedef char *VOSName_t;

typedef uint32_t VOSTimeMs_t;
#define VOSMAX_DELAY (VOSTimeMs_t)0xffffffffUL

typedef uint32_t VOSEventFlags_t;
typedef struct vosevent_handle *VOSEvent_t;

typedef struct vosmutex_handle *VOSMutex_t;

typedef uint32_t VOSSemapCtr_t;
typedef struct vossemap_handle *VOSSemap_t;

typedef uint16_t VOSQueueNbr_t;
typedef struct vosqueue_handle *VOSQueue_t;

typedef uint16_t VOSMultiNbr_t;
typedef struct vosmulti_handle *VOSMulti_t;

typedef void (*VOSTaskFunc_t)(void *);
typedef uint8_t VOS_Prio_t;
typedef uint32_t VOSStkDepth_t;
typedef struct vostcb_handle *VOSTcb_t;

typedef struct vostimer_handle *VOSTimer_t;

typedef enum
{
    NORMAL_TRANSMISSION,
    IPC_TRANSMISSION,
} VTransmission_t;


/**
 * @brief Handle for VOS events.
 *
 */
struct vosevent_handle
{
    VOSName_t name;             /**< name of the event. */
    VTransmission_t transm;     /**< transmission type. */
#if VOSPEND_MULTI_EN_SUPPORT
    UINT8 bitmask;
    void *pend_multi;
#endif
};

/**
 * @brief Handle for VOS mutexes.
 * 
 */
struct vosmutex_handle
{
    VOSName_t name;             /**< name of the mutex. */
    VTransmission_t transm;     /**< transmission type. */
};


/**
 * @brief Handle for VOS semaphores.
 * 
 */
struct vossemap_handle
{
    VOSName_t name;             /**< name of the semaphore. */
    VTransmission_t transm;     /**< transmission type. */
#if VOSPEND_MULTI_EN_SUPPORT
    UINT8 bitmask;
    void *pend_multi;
#endif
};

/**
 * @brief Handle for VOS queues.
 * 
 */
struct vosqueue_handle
{
    VOSName_t name;             /**< name of the queue. */
    VTransmission_t transm;     /**< transmission type. */
#if VOSPEND_MULTI_EN_SUPPORT
    UINT8 bitmask;
    void *pend_multi;
#endif
};


/**
 * @brief handle for VOS multi-tasks.
 * 
 */
struct vosmulti_handle
{
    VOSName_t name;                 /**< name of the multi-task. */
    VOSMultiNbr_t multi_size;       /**< number of tasks in the multi-task. */
    VOSEventFlags_t flag;           /**< flags for the multi-task. */
    VOSEvent_t pend_event;          /**< event to signal when all tasks in the multi-task are ready. */
};

/**
 * @brief Handle for VOS task control block.
 * 
 */
struct vostcb_handle
{
    VOSName_t name;                 /**< name of the task. */
    UINT32 tkerrinfo;               /**< error information of the task. */
};


/**
 * @brief Handle for VOS timers.
 * 
 */
struct vostimer_handle
{
    VOSName_t name;                 /**< name of the timer. */
};

typedef void (*VOS_CALLBACK_VOID)(void);
typedef void (*VOS_CALLBACK_ARG)(void *);
typedef void (*VOS_CALLBACK_TIMER)(void *p_Tmr, void *p_Arg);

typedef enum
{
    VOSFLAG_ANY_CONSUME,
    VOSFLAG_ANY_PEEK,
    VOSFLAG_ALL_CONSUME,
    VOSFLAG_ALL_PEEK,
} VOSFlagOpt_t;

typedef enum
{
    VOSQUEUE_NBR_USED,
    VOSQUEUE_NBR_FREE,
    VOSQUEUE_NBR_SIZE,
} VOSQueueOpt_t;

typedef enum
{
    VOSTASK_STATE_BIT_NONE = 0x00,
    VOSTASK_STATE_BIT_READY = 0x01,
    VOSTASK_STATE_BIT_RUNNING = 0x02,
    VOSTASK_STATE_BIT_DLY = 0x04,
    VOSTASK_STATE_BIT_PEND = 0x08,
    VOSTASK_STATE_BIT_SUSPENDED = 0x10,
} VOSTaskState_t;

typedef enum
{
    VOSTASK_StkBasePtr = 0x00,
    VOSTASK_StkSize = 0x01,
    VOSTASK_PrioBase = 0x02,
    VOSTASK_TopOfStkPtr = 0x03,
} VOSTaskInform_t;

typedef enum
{
    VOSSCHED_STATE_BIT_NONE = 0x00,
    VOSSCHED_STATE_BIT_STOPPED = 0x01,
    VOSSCHED_STATE_BIT_RUNNING = 0x02,
    VOSSCHED_STATE_BIT_SUSPENDED = 0x04,
} VOSSchedState_t;

typedef enum
{
    VOSTIMER_STATE_BIT_NONE = 0x00,
    VOSTIMER_STATE_BIT_STOPPED = 0x01,
    VOSTIMER_STATE_BIT_RUNNING = 0x02,
} VOSTimerState_t;

typedef enum
{
    VOSHOOK_TYPE_TASKCREATE,
    VOSHOOK_TYPE_TASKDEL,
    VOSHOOK_TYPE_TASKRETURN,
    VOSHOOK_TYPE_IDLETASK,
    VOSHOOK_TYPE_STATTASK,
    VOSHOOK_TYPE_TASKSW,
    VOSHOOK_TYPE_TIMETICK,
    VOSHOOK_TYPE_REDZONEHIT,
} VOSHookType_t;


 /**
 * @addtogroup Task
 * 
 * @{
 */
/* ================================================================================================================== */
/*                                                 TASK MANAGEMENT                                                    */
/* ================================================================================================================== */

///@brief 此函数创建任务。任务可以在操作系统启动前创建，也可以由正在运行的任务创建。ISR 无法创建任务。
///@param p_name 任务的名称。
///@param p_task 指向任务代码的指针。
///@param p_arg  任务参数指针。
///@param prio   是任务的首要任务。必须为每个任务分配优先级，并且数字越低，优先级越高。
///@param stk_depth 任务堆栈的大小。
///@note stk_depth 栈大小以字节为单位,且必须为4的倍数。
///@return 任务句柄。
VOSTcb_t VOSTaskCreate(VOSName_t p_name, VOSTaskFunc_t p_task, void *p_arg, VOS_Prio_t prio, VOSStkDepth_t stk_depth);

///@brief 此函数删除任务。
///@param p_ostcb 任务句柄。可以传递 NULL 参数来删除自己。
///@return VRET_VALUE错误代码
VRET_VALUE VOSTaskDel(VOSTcb_t p_ostcb);

///@brief 调用此函数以恢复以前挂起的任务。
///@param p_ostcb 任务句柄。
///@return VRET_VALUE 错误代码
VRET_VALUE VOSTaskResume(VOSTcb_t p_ostcb);

///@brief 调用此函数以暂停任务。
///@param p_ostcb 任务句柄。可以传递 NULL 参数来暂停自己。
///@return VRET_VALUE错误代码
VRET_VALUE VOSTaskSuspend(VOSTcb_t p_ostcb);

///@brief 调用此函数以获取任务的当前状态。
///@param p_ostcb 任务句柄。
///@return 任务状态。指示哪个状态的位模式
VOSTaskState_t VOSTaskGetState(VOSTcb_t p_ostcb);

///@brief 调用此函数以获取任务的当前信息。
///@param p_ostcb 任务句柄。
///@param inform 通知获取此类信息
///@return 任务信息。
UINT32 VOSTaskGetInform(VOSTcb_t p_ostcb, VOSTaskInform_t inform);

///@brief 此功能允许您动态更改任务的优先级。请注意，新的优先级必须可用。
///@param p_ostcb 任务句柄。
///@param prio_new 是新的优先事项。
///@return VRET_VALUE错误代码
VRET_VALUE VOSTaskChangePrio(VOSTcb_t p_ostcb, VOS_Prio_t prio_new);

///@brief 此函数是获取当前正在运行的任务的任务句柄。
///@return 任务句柄。(返回内核 TCB 时返回 null)
VOSTcb_t VOSTaskGetCur(void);
/**@}*/


 /**
 * @addtogroup Timer
 * 
 * @{
 */

/* ================================================================================================================== */
/*                                                 TIMER MANAGEMENT                                                   */
/*   Do not use OS-provided                                                                                           */
/* ================================================================================================================== */

///@brief 应用程序代码调用此函数以创建计时器。
///@param p_name 计时器的名称。
///@param init_ms 初始延迟（以毫秒为单位）。
///@param period_ms 重复延迟（以毫秒为单位）。请注意，当该值为 0 时，不会启用重复计时。
///@param p_callback 指向回调函数的指针，该函数将在计时器过期时调用。
///@param arg 是一个参数（指针），在调用回调函数时传递给回调函数。
///@return 定时器句柄（需要检查是否为NULL）
VOSTimer_t VOSTimerCreate(VOSName_t p_name, VOSTimeMs_t init_ms, VOSTimeMs_t period_ms, VOS_CALLBACK_TIMER p_callback, void *arg);

///@brief 应用程序代码调用此函数以删除计时器。
///@param p_ostimer 定时器手柄
///@return VRET_VALUE错误代码。
VRET_VALUE VOSTimerDel(VOSTimer_t p_ostimer);


///@brief 应用程序代码调用此函数以启动计时器。
///@param p_ostimer 计时器手柄。
///@return VRET_VALUE错误代码。
VRET_VALUE VOSTimerStart(VOSTimer_t p_ostimer);

///@brief 应用程序代码调用此函数以停止计时器。
///@param p_ostimer 计时器手柄。
///@return VRET_VALUE错误代码。
VRET_VALUE VOSTimerStop(VOSTimer_t p_ostimer);

///@brief 调用此函数以获取计时器的当前状态。
///@param p_ostimer 计时器手柄。
///@return 计时器状态。指示哪个状态的位模式
VOSTimerState_t VOSTimerGetState(VOSTimer_t p_ostimer);

///@brief 应用程序代码调用此函数以重新安装计时器。
///@param p_ostimer 计时器手柄。
///@param init_ms 初始延迟（以毫秒为单位）。
///@param period_ms 重复延迟（以毫秒为单位）。请注意，当该值为 0 时，不会启用重复计时。
///@param p_callback 指向回调函数的指针，该函数将在计时器过期时调用。
///@param arg 是一个参数（指针），在调用回调函数时传递给回调函数。
///@return VRET_VALUE错误代码。
VRET_VALUE VOSTimerSet(VOSTimer_t p_ostimer, VOSTimeMs_t init_ms, VOSTimeMs_t period_ms, VOS_CALLBACK_TIMER p_callback, void *arg);

///@brief 此函数是获取当前定时器参数。
///@param p_ostimer 计时器手柄。
///@return 参数（指针）。
void *VOSTimerArgGet(VOSTimer_t p_ostimer);

///@brief 调用此函数以获取计时器超时之前的时间数。
///@param p_ostimer 计时器手柄。
///@return 计时器超时之前的时间数。
VOSTimeMs_t VOSTimerRemainGet(VOSTimer_t p_ostimer);

/**@}*/


 /**
 * @addtogroup Time
 * 
 * @{
 */
/* ================================================================================================================== */
/*                                                 TICK MANAGEMENT                                                    */
/* ================================================================================================================== */

///@brief 调用此函数是为了将当前任务的执行延迟到指定的时间（ms）为止。（可以触发任务切换）
///@param Ms 延迟时间（以毫秒为单位），请注意，系统使用tick表示延迟，因此存在精度误差，当前功能延迟不准确。
///@return VRET_VALUE错误代码。
///@note 根据当前os配置的tick的精度做四舍五入操作。不足一个tick的延迟时间，将被延迟到下一个tick。
VRET_VALUE VOSTimeDly(VOSTimeMs_t Ms);

///@brief 调用此函数可将系统时间（以毫秒为单位）转换为系统节拍（以滴答为单位）。
///@param ms 毫秒系统时间（以毫秒为单位）。
///@return 系统节拍（tick）数。
uint32_t VOSTimeToTick(VOSTimeMs_t ms);

///@brief 调用此函数可将系统节拍（以滴答为单位）转换为系统时间（以毫秒为单位）。
///@param tick 滴答节拍（tick）数。
///@return 系统时间（以毫秒为单位）。
VOSTimeMs_t VOSTickToTime(uint32_t tick);

///@brief 调用此函数以获取系统时间（以毫秒为单位）。
///@return 系统时间（以毫秒为单位）
VOSTimeMs_t VOSTimeGet(void);

///@brief 此函数被调用来计算系统时间间隔（以毫秒为单位）。
///@param ms_start 启动系统时间
///@param ms_end 结束系统时间
///@return 系统时间间隔（以毫秒为单位）
VOSTimeMs_t VOSTimeInterval(VOSTimeMs_t ms_start, VOSTimeMs_t ms_end);
/**@}*/



 /**
 * @addtogroup Queue
 * 
 * @{
 */
/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

///@brief 此函数创建消息队列。（支持核间通信）
///@param p_name 消息队列的名称
///@param max_qty 指示消息队列的最大大小（必须为非零）。
///@note 请注意，大小也不能大于可用味精的最大数量。
///@return 消息队列句柄（需要检查是否为NULL）
VOSQueue_t VOSQueueCreate(VOSName_t p_name, VOSQueueNbr_t max_qty);

///@brief 此函数删除消息队列并准备队列上的所有待处理任务。（支持核间通信）
///@param p_osmsg 消息队列句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSQueueDel(VOSQueue_t p_osmsg);

///@brief 此函数接收要发送到队列的消息。（可能需要等待）（支持核间通信）
///@param p_osmsg 消息队列句柄
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待   
///@return 是指向接收到的消息的指针（需要检查是否为NULL）
void *VOSQueueReceive(VOSQueue_t p_osmsg, VOSTimeMs_t timeout);

///@brief 此函数向队列发送消息。（把队列的末尾放进去）。（支持核间通信）
///使用“size”参数，您可以执行地址发送或复制数据发送。
///@param p_osmsg 消息队列句柄
///@param p_void 是指向要发送的消息的指针。
///@param size 指定消息的大小（以字节为单位）
///'size' == 0： do address send （不支持核间通信）
///'size' ！= 0：复制数据发送（支持核间通信）
///@return VRET_VALUE错误代码
VRET_VALUE VOSQueueSend(VOSQueue_t p_osmsg, void *p_void, size_t size);

///@brief 此函数向队列发送消息。（放在队列的前面）。（支持核间通信）
///使用“size”参数，您可以执行地址发送或复制数据发送。
///@param p_osmsg 消息队列句柄
///@param p_void 是指向要发送的消息的指针。
///@param size 指定消息的大小（以字节为单位）
///'size' == 0： do address send （不支持核间通信）
///'size' ！= 0：复制数据发送（支持核间通信）
///@return VRET_VALUE错误代码
VRET_VALUE VOSQueueSendToFront(VOSQueue_t p_osmsg, void *p_void, size_t size);

///@brief 此函数中止并准备当前在队列中等待的任何任务。 （仅支持核心间通信以自行取消挂起）
///此函数应用于故障中止队列上的等待。
///@param p_osmsg 消息队列句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSQueuePendAbort(VOSQueue_t p_osmsg);

///@brief 此函数可查看要发送到队列的消息。（秘密查看，不影响队列中的数据）（不支持核间通信）
///@param p_osmsg 消息队列句柄
///@return 是指向接收到的消息的指针（需要检查是否为NULL）
void *VOSQueuePeek(VOSQueue_t p_osmsg);

///@brief 此函数获取消息数。使用“opt”参数，您可以获得不同类型的数量。
///@param p_osmsg 消息队列句柄
///@param opt 确定获取消息数的类型。
///@return 消息数
VOSQueueNbr_t VOSQueueGetNbr(VOSQueue_t p_osmsg, VOSQueueOpt_t opt);

///@brief 此函数接收要发送到队列的消息。（可能需要等待）（支持核间通信）
///@param p_osmsg 消息队列句柄
///@param ret_handle 消息队列应答句柄
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待
///@return 是指向接收到的消息的指针（需要检查是否为NULL）
void *VOSQueueReplyReceive(VOSQueue_t p_osmsg, UINT32 *ret_handle, VOSTimeMs_t timeout);

///@brief 此函数将消息发送到队列并等待应答。（支持核间通信）
///@param p_osmsg 消息队列句柄
///@param p_void 是指向要发送的消息的指针。
///@param size 指定消息的大小（以字节为单位）（它必须为非零）
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待
///@return VRET_VALUE错误代码
void *VOSQueueRequest(VOSQueue_t p_osmsg, void *p_void, size_t size, VOSTimeMs_t timeout);

///@brief 此函数向队列发送回复消息。（支持核间通信）
///@param msg_id 消息 ID.（收到消息的地址用作回复的 ID）
///@param p_void 是指向要发送的消息的指针。
///@param size 指定消息的大小（以字节为单位）
///'size' == 0： do address send （不支持核间通信）
///'size' ！= 0：复制数据发送（支持核间通信）
///@return VRET_VALUE错误代码
VRET_VALUE VOSQueueResponse(UINT32 msg_id, void *p_void, size_t size);

/**@}*/



 /**
 * @addtogroup Semaphore
 * 
 * @{
 */
/* ================================================================================================================== */
/*                                                     SEMAPHORES                                                     */
/* ================================================================================================================== */

///@brief 此函数创建信号量。（支持核间通信）
///@param p_name 名称
///@param cnt 是信号量的初始值。如果用于共享资源，则应初始化为可用资源数。
///如果用于发出事件发生的信号，则应初始化为 0。
///@return 信号量句柄（需要检查是否为NULL）
VOSSemap_t VOSSemapCreate(VOSName_t p_name, VOSSemapCtr_t cnt);

///@brief 此函数删除信号量。（支持核间通信）
///@param p_ossem 信号量手柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSSemapDel(VOSSemap_t p_ossem);

///@brief 此函数采用信号量。（可能需要等待）（支持核间通信）
///@param p_ossem 信号量手柄
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待
///@return VRET_VALUE错误代码
VRET_VALUE VOSSemapTake(VOSSemap_t p_ossem, VOSTimeMs_t timeout);

///@brief 此函数提供信号量。（支持核间通信）
///@param p_ossem 信号量手柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSSemapGive(VOSSemap_t p_ossem);

///@brief 此函数中止并准备当前在信号量上等待的任何任务。（仅支持核心间通信以自行取消挂起）
///       此函数应用于故障中止信号量上的等待。
///@param p_ossem 信号量手柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSSemapPendAbort(VOSSemap_t p_ossem);

///@brief 此函数获取信号量的数量。（支持核间通信）
///@param p_ossem 信号量手柄
///@return 当前信号量数量
VOSSemapCtr_t VOSSemapCtr(VOSSemap_t p_ossem);

/**@}*/


 /**
 * @addtogroup Mutex
 * 
 * @{
 */
/* ================================================================================================================== */
/*                                             MUTUAL EXCLUSION SEMAPHORES                                            */
/* ================================================================================================================== */

///@brief 此函数创建互斥锁。它用于资源保护，以防止优先级逆转
///@param p_name 名称
///@return 互斥锁句柄（需要检查是否为NULL）
VOSMutex_t VOSMutexCreate(VOSName_t p_name);

///@brief 此函数删除互斥锁并准备互斥锁上挂起的所有任务。
///@param p_osmutex 互斥句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSMutexDel(VOSMutex_t p_osmutex);

///@brief 互斥锁的此函数锁。（可能需要等待）
///@param p_osmutex 互斥句柄
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待
///@return VRET_VALUE错误代码
VRET_VALUE VOSMutexLock(VOSMutex_t p_osmutex, VOSTimeMs_t timeout);

///@brief 此功能为互斥锁解锁。
///@param p_osmutex 互斥句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSMutexUnlock(VOSMutex_t p_osmutex);

/**@}*/

 /**
 * @addtogroup Flag
 * 
 * @{
 */

/* ================================================================================================================== */
/*                                                    EVENT FLAGS                                                     */
/* ================================================================================================================== */

///@brief 调用此函数以创建事件标志组。（支持核间通信）
///@param p_name 事件标志组的名称
///@param flags 标志包含要存储在事件标志组中的初始值（通常为 0）。
///@return 事件标志组句柄（需要检查是否为 NULL）
VOSEvent_t VOSFlagCreate(VOSName_t p_name, VOSEventFlags_t flags);

///@brief 此函数删除事件标志组，并准备事件标志组上挂起的所有任务。（支持核间通信）
///@param p_osevent 事件标志组句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSFlagDel(VOSEvent_t p_osevent);

///@brief 调用此函数以等待在事件标志组中设置位组合。（支持核间通信）
///@param p_osevent 事件标志组句柄
///@param flags 是一种位模式，指示您希望等待哪些位（即标志）。
///@param osopt 指定是要设置所有位还是设置任何位。查看枚举支持的功能
///@param timeout 超时等待时长（毫秒），VOSMAX_DELAY设置为永久等待，0设置为无等待
///@return 使任务准备就绪的事件标志组中的标志，如果发生超时或错误，则为 0。
VOSEventFlags_t VOSFlagWait(VOSEvent_t p_osevent, VOSEventFlags_t flags, VOSFlagOpt_t osopt, VOSTimeMs_t timeout);

///@brief 调用此函数以设置事件标志组中的某些位。（支持核间通信）
///@param p_osevent 事件标志组句柄
///@param flags 是一种位模式，指示要设置哪些位（即标志）。
///@return VRET_VALUE错误代码
VRET_VALUE VOSFlagSet(VOSEvent_t p_osevent, VOSEventFlags_t flags);

///@brief 调用此函数以清除事件标志组中的某些位。（支持核间通信）
///@param p_osevent 事件标志组句柄
///@param flags 是一种位模式，指示要清除哪些位（即标志）。
///@return VRET_VALUE错误代码
VRET_VALUE VOSFlagClr(VOSEvent_t p_osevent, VOSEventFlags_t flags);

///@brief 此函数中止并准备当前在事件标志组上等待的任何任务。 （仅支持核心间通信以自行取消挂起）
///此函数应用于故障中止对事件标志组的等待。
///@param p_osevent 事件标志组句柄
///@return VRET_VALUE错误代码
VRET_VALUE VOSFlagPendAbort(VOSEvent_t p_osevent);


/**@}*/

 /**
 * @addtogroup Misc
 * 
 * @{
 */

///@brief 此函数用于通知 vkernel RTOS 您即将为中断服务例程 （ISR） 提供服务。
void VOSIntEnter(void);

///@brief 此函数用于通知 vkernel RTOS 您已完成对 ISR 的维护。
void VOSIntExit(void);


///@brief 调用此函数系统将进入临界区。
void VOS_CRITICAL_ENTER(void);

///@brief 调用此函数以系统退出临界区。
void VOS_CRITICAL_EXIT(void);

///@brief 调用此函数以获取 CPU 使用率。
///@return CPU 使用率。（单位单位为 %）
uint32_t VOSGetCPUUsage(void);

/* ================================================================================================================== */
/*                                                    HOOKS MANAGEMENT                                                */
/* ================================================================================================================== */

///@brief 调用此函数来注册各种类型的钩子函数。
///@param hook_type hook 函数的类型场景。
///@param func 是指向 hook 函数的指针。
void VOSHooksRegister(VOSHookType_t hook_type, VOS_CALLBACK_ARG func);
/**@}*/

	//	  TIMER MANAGEMENT
	extern void VOSTimerDynTick(VOSTimeMs_t ms);
	
	//	  MISCELLANEOUS
	extern VRET_VALUE VOSInit(void);
	extern VRET_VALUE VOSStart(void);
	extern VRET_VALUE VOSSchedLock(void);
	extern VRET_VALUE VOSSchedUnlock(void);
	extern VOSSchedState_t VOSSchedGetState(void);

	//	  MULTI PEND
	extern VOSMulti_t VOSMultiCreate(VOSName_t p_name, VOSMultiNbr_t multi_size, void *obj, ...);
	extern VRET_VALUE VOSMultiDel(VOSMulti_t p_osmulti);
	extern VOSMultiNbr_t VOSMultiWait(VOSMulti_t p_osmulti, VOSTimeMs_t timeout);
	extern void *VOSMultiParse(VOSMulti_t p_osmulti, void *obj);

#ifdef __cplusplus
}
#endif

#ifdef ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDOS_H_
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDOS_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDOS_H_ */
#endif /* _VSTDOS_H_ */
