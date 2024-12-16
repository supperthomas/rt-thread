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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdcore.h"
#include "vstdlib.h"
#include "vstdio.h"
#include "vdev_ipc.h"
#include "vsys_mem_tlsf.h"
#include "vsys_mem_info.h"
#include "vos_ipc.h"
#include "vpla_ipc_control.h"

static IPCEvent_t ipc_evt_shadow = NULL;
static vipc_lock_t ipc_pool_lock = {0};

/// ipc msg Init
/// reset the msg queue head and tail pointer
/// @return 0 for success,-1 for error
int VIPCListInit(vipc_list_hd_t *list_hd)
{
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	list_hd->head = list_hd->tail = NULL;
	list_hd->cnt = 0;
	VCPU_CRITICAL_EXIT();

	return 0;
}

int VIPCListEnQue(vipc_list_hd_t *list_hd, vipc_list_node_t *p_node, UINT8 where)
{
	ASSERT(list_hd != NULL);

	if (p_node == NULL)
	{ // malloc failed
		return -1;
	}

	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	if (VIPCListQueEmpty(list_hd))
	{
		list_hd->head = list_hd->tail = p_node;
	}
	else
	{
		if (where == 0)
		{ // insert to the tail
			list_hd->tail->next = p_node;
			p_node->prev = list_hd->tail;
			list_hd->tail = p_node;
		}
		else
		{ // insert to the head
			list_hd->head->prev = p_node;
			p_node->next = list_hd->head;
			list_hd->head = p_node;
		}
	}
	list_hd->cnt++;
	VCPU_CRITICAL_EXIT();
	return 0;
}

vipc_list_node_t *VIPCListDeQue(vipc_list_hd_t *list_hd)
{
	vipc_list_node_t *p_node = NULL;

	if ((list_hd == NULL) || (VIPCListQueEmpty(list_hd)))
	{
		return NULL;
	}
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	// delete the node from the linked queue
	p_node = list_hd->head;
	list_hd->head = p_node->next;
	list_hd->cnt--;

	if (list_hd->cnt == 0)
	{
		list_hd->head = NULL;
		list_hd->tail = NULL;
	}
	VCPU_CRITICAL_EXIT();
	return p_node;
}

vipc_list_node_t *VIPCListTakeQue(vipc_list_hd_t *list_hd, void *arg, vipc_list_check_t func)
{
	int ret = 0;
	vipc_list_node_t *p_node = NULL;
	ASSERT(list_hd != NULL);

	if ((list_hd == NULL) || (VIPCListQueEmpty(list_hd)) || func == NULL)
	{
		return NULL;
	}

	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	for (p_node = list_hd->head; p_node != NULL; p_node = p_node->next)
	{
		ret = func(p_node, arg);
		if (ret > 0)
		{
			if (p_node == list_hd->head)
			{
				list_hd->head = p_node->next;
			}
			else if (p_node == list_hd->tail)
			{
				list_hd->tail = p_node->prev;
			}
			else
			{
				p_node->prev->next = p_node->next;
				p_node->next->prev = p_node->prev;
			}
			list_hd->cnt--;
			if (list_hd->cnt == 0)
			{
				list_hd->head = NULL;
				list_hd->tail = NULL;
			}
			break;
		}
		if (p_node == list_hd->tail)
		{
			break;
		}
	}
	VCPU_CRITICAL_EXIT();
	if (ret > 0)
	{
		return p_node;
	}
	return NULL;
}

vipc_list_node_t *VIPCListPeekQue(vipc_list_hd_t *list_hd, void *arg, vipc_list_check_t func)
{
	int ret = 0;
	vipc_list_node_t *p_node = NULL;
	ASSERT(list_hd != NULL);

	if ((list_hd == NULL) || (VIPCListQueEmpty(list_hd)) || func == NULL)
	{
		return NULL;
	}

	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	for (p_node = list_hd->head; p_node != NULL; p_node = p_node->next)
	{
		ret = func(p_node, arg);
		if (ret > 0)
		{
			break;
		}
		if (p_node == list_hd->tail)
		{
			break;
		}
	}
	VCPU_CRITICAL_EXIT();
	if (ret > 0)
	{
		return p_node;
	}
	return NULL;
}

int VIPCListQueEmpty(vipc_list_hd_t *list_hd)
{
	int is_empty;

	ASSERT(list_hd != NULL);

	is_empty = list_hd? (list_hd->head == NULL) ? TRUE : FALSE : FALSE;

	return is_empty;
}

int ipc_list_check_name(vipc_list_node_t *node, void *arg)
{
	if (!vstrncmp(((vipc_list_t *)node)->name, arg, membersizeof(vipc_list_t, name)))
	{
		return 1;
	}
	return 0;
}

int ipc_list_check_node(vipc_list_node_t *node, void *arg)
{
	if (node == arg)
	{
		return 1;
	}
	return 0;
}

int ipc_list_check_index(vipc_list_node_t *node, void *arg)
{
	static UINT32 index = 0;
	if ((UINT32)arg > index)
	{
		index = (UINT32)arg;
	}
	if (0 == index)
	{
		return 1;
	}
	if (index > 0)
	{
		index--;
	}
	return 0;
}

/*-----------------------------------------------------------------------------*/
/*
ipc event functions
*/
void ipc_controlevt_hdle(vipc_dir_typ_t dir, UINT32 event)
{
	if (dir >= VIPC_DIR_ALL)
	{
		return;
	}
	IPCFlagSet(ipc_evt_shadow, event | 1 << dir);

	return;
}

int ipc_controlevt_init(void)
{
	VIPC_Init();
	ipc_evt_shadow = IPCFlagCreate("ipcevt", 0);

	VIPC_EvtReg(ipc_controlevt_hdle);

	return 0;
}

int ipc_controlevt_post(vipc_dir_typ_t dir, UINT32 event, int timeout)
{
	int ret = 0;

	if (dir > VIPC_DIR_ALL)
	{
		return -1;
	}

	if (dir == VIPC_DIR_ALL)
	{
		for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
		{
			ret = VIPC_EvtSet(i, event, timeout);
		}
	}
	else
	{
		ret = VIPC_EvtSet(dir, event, timeout);
	}

	if (ret != 0)
	{
		return -1;
	}

	return 0;
}

UINT32 ipc_controlevt_wait(UINT32 event, UINT32 timeout)
{
	UINT32 ret = 0;

	ret = IPCFlagWaitAny(ipc_evt_shadow, event, timeout);

	return ret;
}

/*-----------------------------------------------------------------------------*/
/*
ipc msg functions
*/

int ipc_controlmsg_init(void)
{
	vipc_mem_pool_t *pool = VIPC_Pool_Get();

	ipc_controllock_init(&ipc_pool_lock, &pool->lock);

	if (VIPC_Info_Get()->magic != IPC_INFO_MAGIC)
	{
		vipc_msg_tbl_t *msg_tbl = &VIPC_Info_Get()->msg_tbl;
		vipc_msg_que_t *p_msg_que = NULL;

		for (int i = 0; i < (sizeof(vipc_msg_tbl_t) / sizeof(vipc_msg_que_t)); i++)
		{
			p_msg_que = &((vipc_msg_que_t *)msg_tbl)[i];
			ipc_spin_lock_init(&p_msg_que->lock);
			VIPCListInit(&p_msg_que->que);
		}
		if (ipc_controlpool_init() == 0)
		{
			return -1;
		}
	}

	return 0;
}

void *ipc_controlmsg_new(UINT32 size)
{
	vipc_list_node_t *p_msg_node = NULL;

	p_msg_node = ipc_controlpool_malloc(sizeof(vipc_list_node_t) + size - 1);

	if (p_msg_node == NULL)
	{ // check
		return NULL;
	}

	return &p_msg_node->payload[0];
}

int ipc_controlmsg_post(vipc_dir_typ_t dir, void *msg)
{
	int ret = -1, i;
	unsigned int ipc_sr = 0;
	vipc_msg_que_t *msg_que = NULL;
	vipc_list_node_t *p_node = NULL;

	i = VIPC_GetExport(dir);
	if (i < 0)
	{
		return -1;
	}
	msg_que = &VIPC_Info_Get()->msg_tbl.que[i];

	p_node = vcontainer_of(msg, vipc_list_node_t, payload);
	if (ipc_spin_lock_catch(&msg_que->lock, 3, &ipc_sr))
	{
		ret = VIPCListEnQue(&msg_que->que, p_node, 0);
		ipc_spin_lock_release(&msg_que->lock, ipc_sr);
	}
	if (ret != 0)
	{
		return -1;
	}
	ret = ipc_controlevt_post(dir, BIT_MASK(VIPC_EVT_TYP_MSG_TRANSMIT), 2);
	if (ret != 0)
	{
		return -1;
	}
	return 0;
}

void *ipc_controlmsg_get(vipc_dir_typ_t dir)
{
	int i;
	unsigned int ipc_sr = 0;
	void *p_msg = NULL;
	vipc_msg_que_t *msg_que = NULL;
	vipc_list_node_t *p_node = NULL;

	i = VIPC_GetImport(dir);
	if (i < 0)
	{
		return NULL;
	}
	msg_que = &VIPC_Info_Get()->msg_tbl.que[i];

	if (ipc_spin_lock_catch(&msg_que->lock, 3, &ipc_sr))
	{
		p_node = VIPCListDeQue(&msg_que->que);
		ipc_spin_lock_release(&msg_que->lock, ipc_sr);
		if (p_node != NULL)
		{
			p_msg = p_node->payload;
		}
	}

	return p_msg;
}

void ipc_controlmsg_del(void *msg)
{
	vipc_list_node_t *p_node = NULL;
	ASSERT(msg != NULL);

	if (msg != NULL)
	{
		p_node = vcontainer_of(msg, vipc_list_node_t, payload);
		if (p_node != NULL)
		{
			ipc_controlpool_free(p_node);
		}
	}
}

/*-----------------------------------------------------------------------------*/
/*
ipc handle functions
*/

int ipc_controlhanlde_init(void)
{
	if (VIPC_Info_Get()->magic != IPC_INFO_MAGIC)
	{
		vipc_handle_tbl_t *handle_tbl = &VIPC_Info_Get()->handle_tbl;
		vipc_msg_que_t *p_handle_que = NULL;

		for (int i = 0; i < (sizeof(vipc_handle_tbl_t) / sizeof(vipc_msg_que_t)); i++)
		{
			p_handle_que = &((vipc_msg_que_t *)handle_tbl)[i];
			ipc_spin_lock_init(&p_handle_que->lock);
			VIPCListInit(&p_handle_que->que);
		}
	}

	return 0;
}

vipc_list_t *ipc_controlhanlde_new(SINT8 *name, UINT32 val, vipc_handle_t type)
{
	unsigned int ipc_sr;
	vipc_list_t *handle_ctr = NULL, *handlet_ctr;
	vipc_msg_que_t *p_handle_que;

	p_handle_que = &VIPC_Info_Get()->handle_tbl.handle_list[type];

	if (ipc_spin_lock_catch(&p_handle_que->lock, 5, &ipc_sr))
	{
		handle_ctr = (vipc_list_t *)VIPCListPeekQue(&p_handle_que->que, name, ipc_list_check_name);
		if (handle_ctr != NULL)
		{
			if (type == IPC_QUE_HANDLE && ((vipc_que_ctr_t *)handle_ctr)->msg_max < val)
			{
				((vipc_que_ctr_t *)handle_ctr)->msg_max = val;
			}
			handle_ctr->used_mask |= BIT_MASK(VCORE_GetID());
		}
		ipc_spin_lock_release(&p_handle_que->lock, ipc_sr);
	}
	if (handle_ctr == NULL)
	{
		if (type == IPC_QUE_HANDLE)
		{
			handlet_ctr = (vipc_list_t *)ipc_controlpool_malloc(sizeof(vipc_que_ctr_t));
		}
		else
		{
			handlet_ctr = (vipc_list_t *)ipc_controlpool_malloc(sizeof(vipc_val_ctr_t));
		}
		if (handlet_ctr != NULL)
		{
			if (type == IPC_QUE_HANDLE)
			{
				vipc_que_ctr_t *que_ctr = (vipc_que_ctr_t *)handlet_ctr;
				ipc_spin_lock_init(&que_ctr->lock);
				VIPCListInit(&que_ctr->que);
				que_ctr->msg_max = val;
			}
			else
			{
				vipc_val_ctr_t *val_ctr = (vipc_val_ctr_t *)handlet_ctr;
				ipc_spin_lock_init(&val_ctr->lock);
				val_ctr->val = val;
			}
			handlet_ctr->used_mask = BIT_MASK(VCORE_GetID());
			handlet_ctr->req_mask = 0;
			vstrncpy(handlet_ctr->name, name, membersizeof(vipc_list_t, name));
			if (ipc_spin_lock_catch(&p_handle_que->lock, 5, &ipc_sr))
			{
				if (NULL == (handle_ctr = (vipc_list_t *)VIPCListPeekQue(&p_handle_que->que, name, ipc_list_check_name)))
				{
					VIPCListEnQue(&p_handle_que->que, (vipc_list_node_t *)handlet_ctr, 1);
					handle_ctr = handlet_ctr;
				}
				else if (!(handle_ctr->used_mask & BIT_MASK(VCORE_GetID())))
				{
					handle_ctr->used_mask |= BIT_MASK(VCORE_GetID());
					if (type == IPC_QUE_HANDLE && ((vipc_que_ctr_t *)handle_ctr)->msg_max < val)
					{
						((vipc_que_ctr_t *)handle_ctr)->msg_max = val;
					}
				}
				else
				{
					handle_ctr = NULL;
				}
				ipc_spin_lock_release(&p_handle_que->lock, ipc_sr);
			}
			if (handle_ctr != handlet_ctr)
			{
				ipc_controlpool_free(handlet_ctr);
			}
		}
		vmemsetowner(handlet_ctr, __GET_RETURN_ADDR());
	}
	return handle_ctr;
}

int ipc_controlhanlde_del(vipc_list_t *handle_ctr, vipc_handle_t type)
{
	int ret = 0, mask = 0xFF;
	unsigned int ipc_sr;
	vipc_msg_que_t *p_handle_que;

	if (handle_ctr == NULL)
	{
		return -1;
	}

	p_handle_que = &VIPC_Info_Get()->handle_tbl.handle_list[type];

	if (ipc_spin_lock_catch(&p_handle_que->lock, 5, &ipc_sr))
	{
		if (handle_ctr->used_mask & BIT_MASK(VCORE_GetID()))
		{
			handle_ctr->used_mask &= ~BIT_MASK(VCORE_GetID());
			mask = handle_ctr->used_mask;
			if (0 == mask)
			{
				if (handle_ctr != (vipc_list_t *)VIPCListTakeQue(&p_handle_que->que, handle_ctr, ipc_list_check_node))
				{
					ret = -2;
				}
			}
		}
		else
		{
			ret = -1;
		}
		ipc_spin_lock_release(&p_handle_que->lock, ipc_sr);
	}

	if (0 == mask)
	{
		ipc_controlpool_free(handle_ctr);
	}
	return ret;
}

/*-----------------------------------------------------------------------------*/
/*
ipc lock functions
*/

int ipc_controllock_init(vipc_lock_t *reslock, vresspin_lock_t *lock)
{
	static UINT8 lock_numb = 0;
	if (reslock != NULL && lock_numb < 8)
	{
		reslock->resspin_lock = lock;
		if (VIPC_Info_Get()->magic != IPC_INFO_MAGIC)
		{
			ipc_spin_lock_init(&lock->lock);
			lock->lflag = 0;
			lock->reqlock = 0;
		}
		reslock->flagbit = lock_numb++;
		return 0;
	}

	return -1;
}

int ipc_controllock_check(vipc_lock_t *reslock)
{
	if (reslock != NULL && reslock->resspin_lock != NULL)
	{
		return reslock->resspin_lock->lflag;
	}

	return 0;
}

BOOL ipc_controllock(vipc_lock_t *reslock, int timeout_ms)
{
	int dly_tm = timeout_ms * 1000;
	BOOL ret = FALSE;
	UINT8 lock_numb = 0;

	if (reslock != NULL && reslock->resspin_lock != NULL)
	{
		UINT8 *lock_member = (UINT8 *)&reslock->resspin_lock->reqlock;
		if (reslock->resspin_lock->lflag == 0)
		{
			if (ipc_spin_lock_catch(&reslock->resspin_lock->lock, 2, &reslock->ipc_sr))
			{
				if (reslock->resspin_lock->lflag == 0)
				{
					reslock->resspin_lock->lflag = 1;
					ret = TRUE;
					ipc_spin_lowlock_release(&reslock->resspin_lock->lock);
				}
				else
				{
					lock_numb = 1 << reslock->flagbit;
					lock_member[VCORE_GetID()] = lock_numb;
					ipc_spin_lock_release(&reslock->resspin_lock->lock, reslock->ipc_sr);
				}
			}
		}
		if (ret == TRUE)
		{
			return ret;
		}
		while (dly_tm > 0)
		{
			if (IPCSchedIsRun() && !VCPU_INT_IsExec())
			{
				ipc_controlevt_wait(lock_numb << VIPC_EVT_LIMIT, 1);
				dly_tm -= 1000;
			}
			else
			{
				vdelayus(1);
				dly_tm--;
			}
			if (reslock->resspin_lock->lflag == 0)
			{
				if (ipc_spin_lock_catch(&reslock->resspin_lock->lock, 2, &reslock->ipc_sr))
				{
					if (reslock->resspin_lock->lflag == 0)
					{
						reslock->resspin_lock->lflag = 1;
						ret = TRUE;
						lock_member[VCORE_GetID()] = 0;
						ipc_spin_lowlock_release(&reslock->resspin_lock->lock);
						break;
					}
					else
					{
						ipc_spin_lock_release(&reslock->resspin_lock->lock, reslock->ipc_sr);
					}
				}
			}
		}
	}
	return ret;
}

/// Unlock shared resource lock
void ipc_controlunlock(vipc_lock_t *reslock)
{
	UINT32 reqlock = 0;
	if (reslock != NULL && reslock->resspin_lock != NULL)
	{
		if (ipc_spin_lowlock_catch(&reslock->resspin_lock->lock, 2))
		{
			reslock->resspin_lock->lflag = 0;
			reqlock = reslock->resspin_lock->reqlock;
			ipc_spin_lock_release(&reslock->resspin_lock->lock, reslock->ipc_sr);
		}
		if (IPCSchedIsRun())
		{
			UINT8 *lock_member = (UINT8 *)&reqlock;
			for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
			{
				if (lock_member[i] != 0)
				{
					ipc_controlevt_post(i, lock_member[i] << VIPC_EVT_LIMIT, 2);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------------*/
/*
ipc pool functions
*/
/// init the shared pool with tlsf ways
///@return   0 for success -1 for failed
UINT32 ipc_controlpool_init(void)
{
	void *p_mem_pool = NULL;
	UINT32 pool_size;
	int ret = 0;
	vipc_mem_pool_t *pool = VIPC_Pool_Get();

	p_mem_pool = (UINT8 *)&(pool->block[0]);
	pool_size = sizeof(pool->block);

	if (ipc_controllock(&ipc_pool_lock, 2))
	{
		destroy_memory_pool(p_mem_pool);
		ret = tlsf_init(pool_size, p_mem_pool);
		ipc_controlunlock(&ipc_pool_lock);
	}

	return ret;
}

/// malloc a mem according to the given args
///@return   block addr for success, NULL for failed
void *ipc_controlpool_malloc(UINT32 sz)
{
	void *p_block = NULL;
	UINT32 alloc_size = sz;
	UINT8 *p_mem_pool = (UINT8 *)&(VIPC_Pool_Get()->block[0]);

#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
	alloc_size = VSYS_MEM_RecalcSizeWithMagic(sz);
#endif

	if (ipc_controllock(&ipc_pool_lock, 2))
	{
		p_block = tlsf_malloc(p_mem_pool, alloc_size);
		ipc_controlunlock(&ipc_pool_lock);
	}
	if (p_block == NULL)
	{
		VSYS_MEM_CPRINTF("IPC Malloc fail,Addr: x%08X;Return:0x%08X;size=%d\r\n", p_block, __GET_RETURN_ADDR(), sz);
		VSYS_MEM_CheckMalloc_ERR(p_mem_pool);
	}
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
	VSYS_MEM_SetOverwriteMagic(p_block, sz, __GET_RETURN_ADDR());
	p_block = VSYS_MEM_UserDataPtr(p_block);
#endif
	return p_block;
}

/// free a mem according to the given address
///@return  none
void ipc_controlpool_free(void *addr)
{
	UINT8 *p_mem_pool = (UINT8 *)&(VIPC_Pool_Get()->block[0]);

	if ((addr == NULL))
	{
		VSYS_MEM_EPRINTF("IPC vfree fail,Return: 0x%08X[addr=0x%08X]\r\n", __GET_RETURN_ADDR(), (UINT32)addr);
		return;
	}
#if (VSYS_MEM_CHK_OVERFLOW_EN > 0)
	if (VSYS_MEM_CheckOverwrite(addr) != VSYS_MEM_OK)
	{
		VSYS_MEM_WPRINTF("IPC free Return: 0x%08X[addr=0x%08X] had Mem OverFlow!\r\n", __GET_RETURN_ADDR(), (UINT32)addr);
	}
	addr = (void *)VSYS_MEM_BlockPtr(addr);
#endif

	if (ipc_controllock(&ipc_pool_lock, 2))
	{
		tlsf_free(p_mem_pool, addr);
		ipc_controlunlock(&ipc_pool_lock);
	}
	return;
}

int ipc_controlpool_addr_valid(void *addr)
{
	return tlsf_is_addr_valid((UINT8 *)&(VIPC_Pool_Get()->block[0]), addr);
}
