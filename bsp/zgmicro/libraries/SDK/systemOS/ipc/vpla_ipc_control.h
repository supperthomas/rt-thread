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
    IPC message

Description:
    Internal Processor Communication.

Revision History:
    Date                  Author                   Description
		2023.04.3            cxt   						create
------------------------------------------------------------------------------*/

#ifndef __VPA_IPC_CONTROL_H__
#define __VPA_IPC_CONTROL_H__

#include "vstdlib.h"
#include "vdev_ipc.h"

#define VOS_SUPPORT_OPERATING_SYSTEM        (1)

#if VOS_SUPPORT_OPERATING_SYSTEM
#include "vstdos.h"
#define VIPC_SERVICE_TASK_PRIO 4
#define VIPC_SERV_TASK_STK_SIZE 400
typedef VOSTcb_t      IPCTcb_t;
typedef VOSEvent_t      IPCEvent_t;
typedef VOSMutex_t      IPCMutex_t;
#define IPCTaskCreate(name,fun,arg)                             VOSTaskCreate((VOSName_t)name,fun,arg, VIPC_SERVICE_TASK_PRIO, VIPC_SERV_TASK_STK_SIZE * 4)
#define IPCTaskDel(tcb)                                         VOSTaskDel(tcb)
#define IPCTimeGet()                                            VOSTimeGet()
#define IPCTimeInterval(start,end)                              VOSTimeInterval(start,end)
#define IPCSchedIsRun()                                         (VOSSchedGetState()&VOSSCHED_STATE_BIT_RUNNING)
#define IPCFlagCreate(name,flag)                                VOSFlagCreate((VOSName_t)name, flag)
#define IPCFlagDel(event)                                       VOSFlagDel(event)
#define IPCFlagSet(event,flag)                                  VOSFlagSet(event, flag)
#define IPCFlagClr(event,flag)                                  VOSFlagClr(event, flag)
#define IPCFlagWaitAny(event,flag,timeout)                      VOSFlagWait(event,flag,VOSFLAG_ANY_CONSUME,timeout)
#define IPCFlagWaitAll(event,flag,timeout)                      VOSFlagWait(event,flag,VOSFLAG_ALL_CONSUME,timeout)
#define IPCFlagWaitAnyPeek(event,flag,timeout)                  VOSFlagWait(event,flag,VOSFLAG_ANY_PEEK,timeout)
#define IPCFlagWaitAllPeek(event,flag,timeout)                  VOSFlagWait(event,flag,VOSFLAG_ALL_PEEK,timeout)
#define IPCMutexCreate(name)                                    VOSMutexCreate((VOSName_t)name)
#define IPCMutexDel(lock)                                       VOSMutexDel(lock)
#define IPCMutexLock(lock,timeout)                              VOSMutexLock(lock, timeout)
#define IPCMutexUnlock(lock)                                    VOSMutexUnlock(lock)
#else
typedef UINT32*         IPCTcb_t;
typedef UINT32*         IPCEvent_t;
typedef UINT32*         IPCMutex_t;
#define IPCTaskCreate(name,fun,arg)                             ((IPCTcb_t)TRUE)
#define IPCTaskDel(tcb)                                         
#define IPCTimeGet()                                            (vtimeusget())
#define IPCTimeInterval(start,end)                              (vtimeusinterval(start,end)/1000)
#define IPCSchedIsRun()                                         (FALSE)
#define IPCFlagCreate(name,flag)                                ({UINT32 *p_flag = vmalloc(sizeof(UINT32));if(p_flag != NULL)*p_flag = flag;p_flag;})
#define IPCFlagSet(event,flag)                                  do{*(event) |= flag;}while(0)
#define IPCFlagClr(event,flag)                                  do{*(event) &= ~flag;}while(0)
#define IPCFlagWaitAny(event,flag,consume,timeout)              ({int time = 0; do{if(*(event)&flag){*(event) &= ~flag;break;}vdelayus(10);}while(time++ < timeout);1;})//delay timeout/100
#define IPCFlagWaitAll(event,flag,consume,timeout)              ({int time = 0; do{if(*(event)&flag==flag){*(event) &= ~flag;break;}vdelayus(10);}while(time++ < timeout);1;})//delay timeout/100
#define IPCFlagWaitAnyPeek(event,flag,timeout)                  ({int time = 0; do{if(*(event)&flag){break;}vdelayus(10);}while(time++ < timeout);1;})//delay timeout/100
#define IPCFlagWaitAllPeek(event,flag,timeout)                  ({int time = 0; do{if(*(event)&flag==flag){break;}vdelayus(10);}while(time++ < timeout);1;})//delay timeout/100
#define IPCFlagDel(event)                                       vfree(event)
#define IPCMutexCreate(name)                                    ({UINT32 *p_lock = vmalloc(sizeof(UINT32));if(p_lock != NULL)*p_lock = 1;p_lock;})
#define IPCMutexDel(lock)                                       vfree(lock)
#define IPCMutexLock(lock,timeout)                              ({int time = 0; do{if(*(lock) == 0){*(lock) = 1;break;}vdelayus(10);}while(time++ < timeout);1;})//delay timeout/100
#define IPCMutexUnlock(lock)                                    (*(lock) = 0)
#endif

#define IPCMAX_DELAY                                            ((UINT32)0xffffffffUL)

typedef struct
{       
    vresspin_lock_t *resspin_lock; 
    unsigned int ipc_sr;
    UINT8    flagbit;
} vipc_lock_t;

typedef struct
{
	vipc_list_node_pt head;
	UINT16 used_mask;
    UINT16 req_mask;
	SINT8  name[8];
} vipc_list_t;

int VIPCListInit(vipc_list_hd_t *list_hd);
int VIPCListEnQue(vipc_list_hd_t *list_hd, vipc_list_node_t *p_node, UINT8 where);
vipc_list_node_t *VIPCListDeQue(vipc_list_hd_t *list_hd);
vipc_list_node_t* VIPCListTakeQue(vipc_list_hd_t *list_hd, void * arg, vipc_list_check_t func);
vipc_list_node_t* VIPCListPeekQue(vipc_list_hd_t *list_hd, void * arg, vipc_list_check_t func);
int VIPCListQueEmpty(vipc_list_hd_t *list_hd);
int ipc_list_check_name(vipc_list_node_t *node, void *arg);
int ipc_list_check_node(vipc_list_node_t *node, void *arg);
int ipc_list_check_index(vipc_list_node_t *node, void *arg);

void ipc_controlevt_hdle(vipc_dir_typ_t dir, UINT32 event);
int ipc_controlevt_init(void);
int ipc_controlevt_post(vipc_dir_typ_t dir, UINT32 event, int timeout);
UINT32 ipc_controlevt_wait(UINT32 event, UINT32 timeout);

int ipc_controlmsg_init(void);
void *ipc_controlmsg_new(UINT32 size);
int ipc_controlmsg_post(vipc_dir_typ_t dir, void *msg);
void *ipc_controlmsg_get(vipc_dir_typ_t dir);
void ipc_controlmsg_del(void *msg);

int ipc_controlhanlde_init(void);
vipc_list_t *ipc_controlhanlde_new(SINT8* name, UINT32 val, vipc_handle_t type);
int ipc_controlhanlde_del(vipc_list_t *handle_ctr, vipc_handle_t type);

int ipc_controllock_init(vipc_lock_t* reslock , vresspin_lock_t *lock);
int ipc_controllock_check(vipc_lock_t* reslock);
BOOL ipc_controllock(vipc_lock_t* reslock, int timeout);
void ipc_controlunlock(vipc_lock_t* reslock);

UINT32 ipc_controlpool_init(void);
void *ipc_controlpool_malloc(UINT32 sz);
void ipc_controlpool_free(void *addr);
int ipc_controlpool_addr_valid(void *addr);

#endif // #ifndef __VPA_IPC_CONTROL_H__
