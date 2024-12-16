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
                IPC

Description:
                Internal Processor Communication.

Revision History:
        Date                  Author                   Description
		2023.04.3            cxt   						create
------------------------------------------------------------------------------*/

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VOS_IPC_H__
#define __VOS_IPC_H__

#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vdev_ipc.h"
#include "vpla_ipc_control.h"

#if 1
#define OSIPC_DEBUG(...) 	DEBUG_PRINTF("[OSIPC]",##__VA_ARGS__)
#else
#define OSIPC_DEBUG(...) 	
#endif

#pragma pack (1)

typedef enum
{
	IPC_MUTEX_TYPE_CREATE = IPC_MUTEX_HANDLE,
	IPC_SEMAPHORE_TYPE_CREATE = IPC_SEMAPHORE_HANDLE,
	IPC_FLAGS_TYPE_CREATE = IPC_FLAGS_HANDLE,
	IPC_QUE_TYPE_CREATE = IPC_QUE_HANDLE,
	IPC_RETURN_TYPE_VALUE,
	IPC_TYPE_LIST_LIMIT,
} ipc_type_t;

#define VIPC_FLAG_RESOURCES (0x001)

typedef struct
{
	ipc_type_t type;
	union
	{
		struct
		{
			void *p_handle;
			SINT8 name[8];
		};
		struct
		{
			UINT32 idnum;
			UINT32 size;
			UINT8 payload[4];
		};
	};
} ipc_handle_t;

typedef struct
{
	spin_lock_t lock;
	UINT32 used_mask;
} vipc_ctr_t;

typedef struct
{
	UINT16 type;
	UINT16 size;
	UINT8 payload[1];
} vipc_que_msg_t;

typedef struct
{
	vipc_list_t info;
	UINT32 msg_max;
	spin_lock_t lock;
	vipc_list_hd_t que;
} vipc_que_ctr_t;

typedef struct
{
	vipc_list_t info;
	spin_lock_t lock;
	UINT32 val;
} vipc_val_ctr_t;

typedef struct
{
	vipc_list_t info;
	IPCEvent_t p_osevent;
	vipc_que_ctr_t *handle_ctr;
} ipc_que_t;

typedef enum
{
	VALUE_TYPE_MUTEX = 0,
	VALUE_TYPE_SEMAPHORE,
	VALUE_TYPE_FLAGS_SET,
	VALUE_TYPE_FLAGS_CLR,
	VALUE_TYPE_FLAGS_SETALL,
	VALUE_TYPE_FLAGS_CLRALL,
} ipc_valtype_t;

typedef struct
{
	vipc_list_t info;
	IPCEvent_t p_osevent;
	vipc_val_ctr_t *handle_ctr;
} ipc_val_t;

typedef struct
{
	vipc_list_t info;
	IPCEvent_t p_osevent;
	UINT32 index;
	UINT8 *buff;
	UINT32 buff_len;
} ipc_return_t;

#pragma pack () 

SINT32 vipc_service_init(void);
ipc_que_t *ipc_create_queue(SINT8 *name, UINT32 cnt);
SINT32 ipc_connect_queue(vipc_dir_typ_t dir, SINT8 *name, vipc_que_ctr_t *handle_ctr);
SINT32 ipc_del_queue(ipc_que_t *ipc_que);
int ipc_send_queue(ipc_que_t *ipc_que, void *msg, UINT32 size, UINT8 opt, UINT32 type);
void *ipc_recv_queue(ipc_que_t *ipc_que, UINT32 timeout, UINT32 *type);

ipc_val_t *ipc_create_value(SINT8 *name, ipc_type_t type);
SINT32 ipc_connect_value(vipc_dir_typ_t dir, ipc_type_t type, SINT8 *name, vipc_val_ctr_t *handle_ctr);
SINT32 ipc_del_value(ipc_type_t type, ipc_val_t *ipc_val);
SINT32 ipc_set_value(ipc_val_t *ipc_val, UINT32 val, ipc_valtype_t flag);
UINT32 ipc_get_value(ipc_val_t *ipc_val, UINT32 val, ipc_valtype_t flag, UINT32 timeout);

ipc_return_t *ipc_create_return(SINT8 *name, UINT32 dir_mask);
SINT32 ipc_connect_return(vipc_dir_typ_t dir, UINT32 idnum, UINT8 *payload, UINT32 size);
SINT32 ipc_del_return(ipc_return_t *ipc_return);
SINT32 ipc_send_return(UINT32 ID_num, void *msg, UINT32 size);
void * ipc_recv_return(ipc_return_t *ipc_return, UINT32 timeout);

void *ipc_execute_callback(ipc_que_t *ipc_que, void *msg, UINT32 size, UINT32 timeout);


//---------------------------------
#define vipc_queue_create(name, cnt)				ipc_create_queue(name, cnt)
#define vipc_queue_del(que)							ipc_del_queue(que)
#define vipc_queue_send(que, msg, size)				ipc_send_queue(que, msg, size, 0x00, 0)
#define vipc_queue_sendtofront(que, msg, size)		ipc_send_queue(que, msg, size, 0x01, 0)
#define vipc_queue_recv(que, timeout)				ipc_recv_queue(que, timeout, NULL)

#define vipc_mutex_create(name)						ipc_create_value(name, IPC_MUTEX_TYPE_CREATE)
#define vipc_mutex_del(lock)						ipc_del_value(IPC_MUTEX_TYPE_CREATE, lock)
#define vipc_mutex_lock(lock, timeout)				ipc_get_value(lock, 0, VALUE_TYPE_MUTEX, timeout)
#define vipc_mutex_unlock(lock)						ipc_set_value(lock, 0, VALUE_TYPE_MUTEX)

#define vipc_semaphore_create(name)					ipc_create_value(name, IPC_SEMAPHORE_TYPE_CREATE)
#define vipc_semaphore_del(sem)						ipc_del_value(IPC_SEMAPHORE_TYPE_CREATE, sem)
#define vipc_semaphore_give(sem)					ipc_set_value(sem, 0, VALUE_TYPE_SEMAPHORE)
#define vipc_semaphore_take(sem, timeout)			ipc_get_value(sem, 0, VALUE_TYPE_SEMAPHORE, timeout)

#define vipc_flags_create(name)						ipc_create_value(name, IPC_FLAGS_TYPE_CREATE)
#define vipc_flags_del(event)						ipc_del_value(IPC_FLAGS_TYPE_CREATE, event)
#define vipc_flags_set(event, flag)					ipc_set_value(event, flag, VALUE_TYPE_FLAGS_SET)
#define vipc_flags_clr(event, flag)					ipc_set_value(event, flag, VALUE_TYPE_FLAGS_CLR)
#define vipc_flags_wait(event, flag, opt,timeout)	ipc_get_value(event, flag, opt, timeout)

#define vipc_return_recv(que, type, timeout)		ipc_recv_queue(que, timeout, type)

#endif // #ifndef __VOS_IPC_H__
