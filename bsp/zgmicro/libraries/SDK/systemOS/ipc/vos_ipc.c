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
		Processor Communication.

Revision History:
		Date                  Author                   Description
		2023.04.3            cxt   						create
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdcore.h"
#include "vstdlib.h"
#include "vstdio.h"
#include "vdev_ipc.h"
#include "vpla_ipc_control.h"
#include "vos_ipc.h"

#undef ASSERT
#define ASSERT(en)                                                              \
	do                                                                          \
	{                                                                           \
		if (!(en))                                                              \
		{                                                                       \
			DEBUG_PRINTF("[OSIPC]", "(%d)%s %s", __LINE__, #en, "failed!\r\n"); \
		}                                                                       \
	} while (0)

static IPCTcb_t IPCServTCB = 0;
static IPCMutex_t IPCMutexLock = 0;
static vipc_list_hd_t ipc_queue_list = {0};
static vipc_list_hd_t ipc_mutex_list = {0};
static vipc_list_hd_t ipc_semaphore_list = {0};
static vipc_list_hd_t ipc_flags_list = {0};
static vipc_list_hd_t ipc_return_list = {0};

static void ipc_msg_control(vipc_dir_typ_t dir)
{
	ipc_handle_t *p_msg = NULL;

	while (NULL != (p_msg = ipc_controlmsg_get(dir)))
	{
		switch (p_msg->type)
		{
		case IPC_QUE_TYPE_CREATE:
			ipc_connect_queue(dir, p_msg->name, p_msg->p_handle);
			break;
		case IPC_MUTEX_TYPE_CREATE:
			ipc_connect_value(dir, IPC_MUTEX_TYPE_CREATE, p_msg->name, p_msg->p_handle);
			break;
		case IPC_SEMAPHORE_TYPE_CREATE:
			ipc_connect_value(dir, IPC_SEMAPHORE_TYPE_CREATE, p_msg->name, p_msg->p_handle);
			break;
		case IPC_FLAGS_TYPE_CREATE:
			ipc_connect_value(dir, IPC_FLAGS_TYPE_CREATE, p_msg->name, p_msg->p_handle);
			break;
		case IPC_RETURN_TYPE_VALUE:
			ipc_connect_return(dir, p_msg->idnum, p_msg->payload, p_msg->size);
			break;
		default:
			break;
		}
		ipc_controlmsg_del(p_msg);
	}
}

static int ipc_list_check_val(vipc_list_node_t *node, void *arg)
{
	UINT32 flag = 0;
	switch ((vipc_evt_typ_t)(UINT32)arg)
	{
	case VIPC_EVT_TYP_MUTEX_UNLOCK:
	case VIPC_EVT_TYP_SEMAPHORE_GIVE:
		if (((ipc_val_t *)node)->handle_ctr->val != 0)
		{
			IPCFlagSet(((ipc_val_t *)node)->p_osevent, VIPC_FLAG_RESOURCES);
		}
		break;
	case VIPC_EVT_TYP_FLAGS_SET:
		IPCFlagClr(((ipc_val_t *)node)->p_osevent, 0xFFFFFFFF);
		IPCFlagSet(((ipc_val_t *)node)->p_osevent, ((ipc_val_t *)node)->handle_ctr->val);
		break;
	case VIPC_EVT_TYP_QUE_SEND:
		if (((ipc_que_t *)node)->handle_ctr->que.cnt)
		{
			IPCFlagSet(((ipc_que_t *)node)->p_osevent, VIPC_FLAG_RESOURCES);
		}
		break;
	default:
		break;
	}
	return 0;
}

static int ipc_list_check_idnum(vipc_list_node_t *node, void *arg)
{
	if (((ipc_return_t *)node)->index == *(UINT32 *)arg)
	{
		return 1;
	}
	return 0;
}

static int ipc_reset_ctr(vipc_dir_typ_t dir)
{
	unsigned int ipc_sr = 0;
	UINT32 list_num = 0, mask = 0xFF;
	vipc_list_hd_t *ipc_list = NULL;
	vipc_val_ctr_t *ctr = NULL;

	for (ipc_type_t type = IPC_MUTEX_TYPE_CREATE; type < IPC_TYPE_LIST_LIMIT; type++)
	{
		if (type == IPC_MUTEX_TYPE_CREATE)
			ipc_list = &ipc_mutex_list;
		else if (type == IPC_SEMAPHORE_TYPE_CREATE)
			ipc_list = &ipc_semaphore_list;
		else if (type == IPC_FLAGS_TYPE_CREATE)
			ipc_list = &ipc_flags_list;
		else if (type == IPC_QUE_TYPE_CREATE)
			ipc_list = &ipc_queue_list;

		list_num = ipc_list->cnt;
		for (UINT32 i = 0; i < list_num; i++)
		{
			ipc_val_t *ipc_val = (ipc_val_t *)VIPCListPeekQue(ipc_list, (void *)i, ipc_list_check_index); // ipc_val_t
			if (ipc_val == NULL || ipc_val->handle_ctr == NULL)
				continue;
			ctr = ipc_val->handle_ctr;
			if (ipc_spin_lock_catch(&ctr->lock, 2, &ipc_sr))
			{
				ctr->info.used_mask &= ~BIT_MASK(dir);
				mask = ctr->info.used_mask;
				ipc_spin_lock_release(&ctr->lock, ipc_sr);
			}
			if (mask == 0)
			{
				ipc_val->handle_ctr = NULL;
				ipc_controlpool_free(ctr);
			}
		}
	}

	return 0;
}

static void ipc_service(void *arg)
{
	UINT32 evt;

	while (1)
	{
		vipc_dir_typ_t dir = VIPC_DIR_2TP;
		evt = ipc_controlevt_wait((1 << VIPC_EVT_LIMIT) - 1, IPCMAX_DELAY);

		for (; dir <= VIPC_DIR_ALL; dir++)
		{
			if (evt & BIT_MASK(dir))
				break;
		}
		if (dir == VIPC_DIR_ALL || (evt >> VIPC_DIR_ALL == 0))
		{
			continue;
		}
		for (vipc_evt_typ_t i = VIPC_EVT_TYP_MSG_TRANSMIT; i < VIPC_EVT_LIMIT && evt >> i; i++)
		{
			if (evt & BIT_MASK(i))
			{
				switch (i)
				{
				case VIPC_EVT_TYP_MSG_TRANSMIT:
					ipc_msg_control(dir);
					break;
				case VIPC_EVT_TYP_MUTEX_UNLOCK:
					VIPCListPeekQue(&ipc_mutex_list, (void *)VIPC_EVT_TYP_MUTEX_UNLOCK, ipc_list_check_val);
					break;
				case VIPC_EVT_TYP_SEMAPHORE_GIVE:
					VIPCListPeekQue(&ipc_semaphore_list, (void *)VIPC_EVT_TYP_SEMAPHORE_GIVE, ipc_list_check_val);
					break;
				case VIPC_EVT_TYP_FLAGS_SET:
					VIPCListPeekQue(&ipc_flags_list, (void *)VIPC_EVT_TYP_FLAGS_SET, ipc_list_check_val);
					break;
				case VIPC_EVT_TYP_QUE_SEND:
					VIPCListPeekQue(&ipc_queue_list, (void *)VIPC_EVT_TYP_QUE_SEND, ipc_list_check_val);
					break;
				case VIPC_EVT_TYP_RESET_CTR:
					ipc_reset_ctr(dir);
					break;
				default:
					break;
				}
			}
		}
	}
}

/// Init IPC Service, create the service task
/// @return VRET_NO_ERROR else init fail
int vipc_service_init(void)
{
	int ret = 0;
	ipc_controlevt_init();
	ipc_controlmsg_init();
	ipc_controlhanlde_init();

	VIPC_Info_Get()->magic = IPC_INFO_MAGIC;

	IPCServTCB = IPCTaskCreate("IPC Service", ipc_service, NULL);
	IPCMutexLock = IPCMutexCreate("IPC Mutex");
	if (IPCServTCB == NULL || IPCMutexLock == NULL)
	{
		if (IPCServTCB)
			IPCTaskDel(IPCServTCB);
		if (IPCMutexLock)
			IPCMutexDel(IPCMutexLock);
		return -1;
	}
	VCPU_INT_SrcEnable(IRQ_MCUIPC_INT);
	return VRET_NO_ERROR;
}

ipc_que_t *ipc_create_queue(SINT8 *name, UINT32 cnt)
{
	int ret;
	unsigned int ipc_sr = 0;
	ipc_handle_t *ipc_msg = NULL;
	ipc_que_t *ipc_que = NULL;

	ipc_que = (ipc_que_t *)VIPCListPeekQue(&ipc_queue_list, name, ipc_list_check_name);
	if (ipc_que == NULL)
	{
		ipc_que = vcalloc(sizeof(ipc_que_t), 1);
		if (ipc_que == NULL)
		{
			goto ret_err;
		}

		vstrncpy(ipc_que->info.name, name, sizeof(ipc_que->info.name));
		ipc_que->p_osevent = IPCFlagCreate(name, 0);
		if (ipc_que->p_osevent == NULL)
		{
			goto ret_err;
		}

		ipc_que->handle_ctr = (vipc_que_ctr_t *)ipc_controlhanlde_new(name, cnt, IPC_QUE_HANDLE);
		if (ipc_que->handle_ctr == NULL)
		{
			goto ret_err;
		}
		VIPCListEnQue(&ipc_queue_list, (vipc_list_node_t *)ipc_que, 0);

		ipc_que->info.used_mask = ipc_que->handle_ctr->info.used_mask;
		for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
		{
			if (ipc_que->info.used_mask & BIT_MASK(i))
			{
				if (i != VCORE_GetID())
				{
					ipc_msg = ipc_controlmsg_new(sizeof(ipc_handle_t));
					if (ipc_msg == NULL)
					{
						break;
					}
					vstrncpy(ipc_msg->name, name, sizeof(ipc_msg->name));
					ipc_msg->type = IPC_QUE_TYPE_CREATE;
					ipc_msg->p_handle = ipc_que->handle_ctr;
					ret = ipc_controlmsg_post(i, ipc_msg);
					if (ret != 0)
					{
						ipc_controlmsg_del(ipc_msg);
					}
				}
			}
		}
	}
	return ipc_que;

ret_err:
	if (ipc_que)
	{
		if (ipc_que->p_osevent)
		{
			IPCFlagDel(ipc_que->p_osevent);
		}
		vfree(ipc_que);
	}
	return NULL;
}

SINT32 ipc_connect_queue(vipc_dir_typ_t dir, SINT8 *name, vipc_que_ctr_t *handle_ctr)
{
	SINT32 ret = 0;
	unsigned int ipc_sr = 0;
	ipc_que_t *ipc_que = NULL;

	if (handle_ctr->info.used_mask & BIT_MASK(VCORE_GetID()))
	{
		OSIPC_DEBUG("core%d queue[%s] connect ~~\r\n", dir, name);
	}

	return ret;
}

int ipc_del_queue(ipc_que_t *ipc_que)
{
	if (ipc_que == NULL)
	{
		return -1;
	}

	VIPCListTakeQue(&ipc_queue_list, ipc_que, ipc_list_check_node);
	if (ipc_que->p_osevent)
	{
		IPCFlagDel(ipc_que->p_osevent);
	}
	if (ipc_que->handle_ctr)
	{
		ipc_controlhanlde_del((vipc_list_t *)ipc_que->handle_ctr, IPC_QUE_HANDLE);
	}
	vfree(ipc_que);

	return 0;
}

int ipc_send_queue(ipc_que_t *ipc_que, void *msg, UINT32 size, UINT8 opt, UINT32 type)
{ // where == 0 insert to the tail else insert to the head
	int ret = 0;
	unsigned int ipc_sr = 0;
	UINT16 used_mask = 0, req_mask = 0;
	vipc_que_ctr_t *handle_ctr = NULL;
	vipc_list_node_t *p_node = NULL;
	UINT8 type_len = (opt & 0x02) ? 4 : 0;

	if (ipc_que == NULL || ipc_que->handle_ctr == NULL)
	{
		return -1;
	}
	handle_ctr = ipc_que->handle_ctr;
	if (handle_ctr == NULL || handle_ctr->que.cnt >= handle_ctr->msg_max)
	{
		return -1;
	}
	p_node = ipc_controlpool_malloc(sizeof(vipc_list_node_t) - 1 + sizeof(vipc_que_msg_t) - 1 + type_len + size);
	if (p_node == NULL)
	{
		return -1;
	}
	((vipc_que_msg_t *)p_node->payload)->type = type_len;
	vmemcpy(((vipc_que_msg_t *)p_node->payload)->payload, &type, type_len);
	vmemcpy(((vipc_que_msg_t *)p_node->payload)->payload + type_len, msg, size);
	((vipc_que_msg_t *)p_node->payload)->size = size;

	if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
	{
		ret = VIPCListEnQue(&handle_ctr->que, p_node, opt & 0x01);
		used_mask = handle_ctr->info.used_mask;
		req_mask = handle_ctr->info.req_mask;
		handle_ctr->info.req_mask = 0;
		ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
	}
	else
	{
		ipc_controlpool_free(p_node);
		ret = -1;
	}
	if (ret == 0)
	{
		for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
		{
			if ((used_mask & BIT_MASK(i)) && (req_mask & BIT_MASK(i)))
			{
				if (i == VCORE_GetID())
				{
					IPCFlagSet(ipc_que->p_osevent, VIPC_FLAG_RESOURCES);
				}
				else
				{
					ipc_controlevt_post(i, BIT_MASK(VIPC_EVT_TYP_QUE_SEND), 2);
				}
			}
		}
	}

	return ret;
}

void *ipc_recv_queue(ipc_que_t *ipc_que, UINT32 timeout, UINT32 *type)
{
	UINT32 event = 0xFF, add_flag = FALSE;
	unsigned int ipc_sr = 0;
	void *msg = NULL;
	vipc_que_ctr_t *handle_ctr = NULL;
	vipc_list_node_t *p_node = NULL;
	UINT32 startms = 0, thisms = timeout;

	if (ipc_que == NULL || ipc_que->handle_ctr == NULL)
	{
		return NULL;
	}
	handle_ctr = ipc_que->handle_ctr;
	startms = IPCTimeGet();
	while (TRUE)
	{
		if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
		{
			p_node = VIPCListDeQue(&handle_ctr->que);
			add_flag = !VIPCListQueEmpty(&handle_ctr->que);
			if (!add_flag)
			{
				handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
			}
			ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
		}
		if (p_node)
		{
			UINT32 size = ((vipc_que_msg_t *)p_node->payload)->size;
			msg = vmalloc(size);
			if (msg == NULL)
			{
				if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
				{
					VIPCListEnQue(&handle_ctr->que, p_node, 1);
					ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
					add_flag = TRUE;
				}
				else
				{
					ASSERT(msg == NULL);
				}
			}
			else
			{
				UINT16 type_len = ((vipc_que_msg_t *)p_node->payload)->type;
				if (type_len && type != NULL)
				{
					vmemcpy(type, ((vipc_que_msg_t *)p_node->payload)->payload, type_len);
				}
				vmemcpy(msg, ((vipc_que_msg_t *)p_node->payload)->payload + type_len, size);
				ipc_controlpool_free(p_node);
			}
			if (event == VIPC_FLAG_RESOURCES && add_flag)
			{
				IPCFlagSet(ipc_que->p_osevent, VIPC_FLAG_RESOURCES);
			}
			break;
		}
		if (event == 0)
		{
			break;
		}
		if (timeout != IPCMAX_DELAY)
		{
			if (!(thisms -= MIN(IPCTimeInterval(startms, IPCTimeGet()), thisms)))
			{
				break;
			}
			startms = IPCTimeGet();
		}
		event = IPCFlagWaitAny(ipc_que->p_osevent, VIPC_FLAG_RESOURCES, thisms);
	}

	return msg;
}

ipc_val_t *ipc_create_value(SINT8 *name, ipc_type_t type)
{
	int ret;
	unsigned int ipc_sr = 0;
	ipc_handle_t *ipc_msg = NULL;
	ipc_val_t *ipc_val = NULL;
	vipc_list_hd_t *list_hd = NULL;
	UINT32 value = 0;

	switch (type)
	{
	case IPC_MUTEX_TYPE_CREATE:
		list_hd = &ipc_mutex_list;
		value = 1;
		break;
	case IPC_SEMAPHORE_TYPE_CREATE:
		list_hd = &ipc_semaphore_list;
		break;
	case IPC_FLAGS_TYPE_CREATE:
		list_hd = &ipc_flags_list;
		break;
	default:
		return NULL;
	}
	ipc_val = (ipc_val_t *)VIPCListPeekQue(list_hd, name, ipc_list_check_name);
	if (ipc_val == NULL)
	{
		ipc_val = vcalloc(sizeof(ipc_val_t), 1);
		if (ipc_val == NULL)
			goto ret_err;

		vstrncpy(ipc_val->info.name, name, sizeof(ipc_val->info.name));
		ipc_val->info.name[sizeof(ipc_val->info.name) - 1] = '\0';
		ipc_val->p_osevent = IPCFlagCreate(name, 0);
		if (ipc_val->p_osevent == NULL)
			goto ret_err;

		ipc_val->handle_ctr = (vipc_val_ctr_t *)ipc_controlhanlde_new(name, value, (vipc_handle_t)type);
		if (ipc_val->handle_ctr == NULL)
			goto ret_err;
		VIPCListEnQue(list_hd, (vipc_list_node_t *)ipc_val, 0);

		ipc_val->info.used_mask = ipc_val->handle_ctr->info.used_mask;
		for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
		{
			if (ipc_val->info.used_mask & BIT_MASK(i))
			{
				if (i != VCORE_GetID())
				{
					ipc_msg = ipc_controlmsg_new(sizeof(ipc_handle_t));
					if (ipc_msg == NULL)
						break;
					vstrncpy(ipc_msg->name, name, sizeof(ipc_msg->name));
					ipc_msg->type = type;
					ipc_msg->p_handle = ipc_val->handle_ctr;
					ret = ipc_controlmsg_post(i, ipc_msg);
					if (ret != 0)
						ipc_controlmsg_del(ipc_msg);
				}
			}
		}
	}

	return ipc_val;

ret_err:
	if (ipc_val)
	{
		if (ipc_val->p_osevent)
			IPCFlagDel(ipc_val->p_osevent);
		vfree(ipc_val);
	}
	return NULL;
}

SINT32 ipc_connect_value(vipc_dir_typ_t dir, ipc_type_t type, SINT8 *name, vipc_val_ctr_t *handle_ctr)
{
	SINT32 ret = 0;

	if (handle_ctr->info.used_mask & BIT_MASK(VCORE_GetID()))
	{
		OSIPC_DEBUG("core%d val(%d)[%s] connect ~~\r\n", dir, type, name);
	}

	return ret;
}

SINT32 ipc_del_value(ipc_type_t type, ipc_val_t *ipc_val)
{
	vipc_list_hd_t *list_hd = NULL;

	if (ipc_val == NULL)
	{
		return -1;
	}

	switch (type)
	{
	case IPC_MUTEX_TYPE_CREATE:
		list_hd = &ipc_mutex_list;
		break;
	case IPC_SEMAPHORE_TYPE_CREATE:
		list_hd = &ipc_semaphore_list;
		break;
	case IPC_FLAGS_TYPE_CREATE:
		list_hd = &ipc_flags_list;
		break;
	default:
		return -1;
	}

	VIPCListTakeQue(list_hd, ipc_val, ipc_list_check_node);

	if (ipc_val->p_osevent)
	{
		IPCFlagDel(ipc_val->p_osevent);
	}
	if (ipc_val->handle_ctr)
	{
		ipc_controlhanlde_del((vipc_list_t *)ipc_val->handle_ctr, (vipc_handle_t)type);
	}
	vfree(ipc_val);

	return 0;
}

SINT32 ipc_set_value(ipc_val_t *ipc_val, UINT32 val, ipc_valtype_t flag)
{
	SINT32 ret = 0;
	UINT32 ipc_sr = 0;
	UINT16 used_mask = 0, req_mask = 0;
	vipc_val_ctr_t *handle_ctr = NULL;
	UINT32 event = 0, value = 0;

	if (ipc_val == NULL || ipc_val->handle_ctr == NULL)
	{
		return -1;
	}
	handle_ctr = ipc_val->handle_ctr;

	if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
	{
		switch (flag)
		{
		case VALUE_TYPE_MUTEX:
			if (handle_ctr->val > 0)
			{
				ret = -1;
			}
			else
			{
				handle_ctr->val = 1;
			}
			event = BIT_MASK(VIPC_EVT_TYP_MUTEX_UNLOCK);
			break;
		case VALUE_TYPE_SEMAPHORE:
			handle_ctr->val++;
			event = BIT_MASK(VIPC_EVT_TYP_SEMAPHORE_GIVE);
			break;
		case VALUE_TYPE_FLAGS_SET:
		case VALUE_TYPE_FLAGS_SETALL:
			handle_ctr->val |= val;
			value = handle_ctr->val;
			event = BIT_MASK(VIPC_EVT_TYP_FLAGS_SET);
			break;
		case VALUE_TYPE_FLAGS_CLR:
		case VALUE_TYPE_FLAGS_CLRALL:
			handle_ctr->val &= ~val;
			value = handle_ctr->val;
			event = BIT_MASK(VIPC_EVT_TYP_FLAGS_SET);
			break;
		default:
			break;
		}
		if (ret == 0)
		{
			used_mask = handle_ctr->info.used_mask;
			req_mask = handle_ctr->info.req_mask;
			handle_ctr->info.req_mask = 0;
		}
		ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
	}
	else
	{
		ret = -1;
	}
	if (ret == 0)
	{
		for (vipc_dir_typ_t i = VIPC_DIR_2TP; i < VIPC_DIR_ALL; i++)
		{
			if ((used_mask & BIT_MASK(i)) && (req_mask & BIT_MASK(i)))
			{
				if (i == VCORE_GetID())
					if (event == BIT_MASK(VIPC_EVT_TYP_FLAGS_SET))
					{
						IPCFlagClr(ipc_val->p_osevent, 0xFFFFFFFF);
						IPCFlagSet(ipc_val->p_osevent, value);
					}
					else
						IPCFlagSet(ipc_val->p_osevent, VIPC_FLAG_RESOURCES);
				else
					ret = ipc_controlevt_post(i, event, 2);
			}
		}
	}

	return ret;
}

UINT32 ipc_get_value(ipc_val_t *ipc_val, UINT32 val, ipc_valtype_t flag, UINT32 timeout)
{
	UINT32 val_t = 0, event = 0xFF, add_flag = FALSE;
	unsigned int ipc_sr = 0;
	vipc_val_ctr_t *handle_ctr = NULL;
	vipc_list_node_t *p_node = NULL;
	UINT32 startms = 0, thisms = timeout;

	if (ipc_val == NULL || ipc_val->handle_ctr == NULL)
	{
		return 0;
	}
	handle_ctr = ipc_val->handle_ctr;

	startms = IPCTimeGet();
	while (TRUE)
	{
		switch (flag)
		{
		case VALUE_TYPE_MUTEX:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if (handle_ctr->val == 1)
				{
					val_t = 1;
					handle_ctr->val = 0;
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
		case VALUE_TYPE_SEMAPHORE:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if (handle_ctr->val > 0)
				{
					val_t = handle_ctr->val;
					handle_ctr->val--;
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
			if (val_t > 1 && add_flag)
			{
				IPCFlagSet(ipc_val->p_osevent, VIPC_FLAG_RESOURCES);
			}
			break;
		case VALUE_TYPE_FLAGS_SET:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if (handle_ctr->val & val)
				{
					val_t = (handle_ctr->val & val);
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
			if (val_t > 0 && add_flag)
			{
				IPCFlagSet(ipc_val->p_osevent, val_t);
			}
			break;
		case VALUE_TYPE_FLAGS_CLR:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if (handle_ctr->val & val)
				{
					val_t = (handle_ctr->val & val);
					handle_ctr->val &= ~val;
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
			break;
		case VALUE_TYPE_FLAGS_SETALL:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if ((handle_ctr->val & val) == val)
				{
					val_t = val;
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
			if (val_t > 0 && add_flag)
			{
				IPCFlagSet(ipc_val->p_osevent, val_t);
			}
			break;
		case VALUE_TYPE_FLAGS_CLRALL:
			if (ipc_spin_lock_catch(&handle_ctr->lock, 2, &ipc_sr))
			{
				if ((handle_ctr->val & val) == val)
				{
					val_t = val;
					handle_ctr->val &= ~val;
				}
				else
				{
					handle_ctr->info.req_mask |= BIT_MASK(VCORE_GetID());
				}
				ipc_spin_lock_release(&handle_ctr->lock, ipc_sr);
			}
			break;
		default:
			break;
		}

		if (val_t > 0 || event == 0)
		{
			break;
		}
		if (timeout != IPCMAX_DELAY)
		{
			if (!(thisms -= MIN(IPCTimeInterval(startms, IPCTimeGet()), thisms)))
			{
				break;
			}
			startms = IPCTimeGet();
		}
		switch (flag)
		{
		case VALUE_TYPE_MUTEX:
		case VALUE_TYPE_SEMAPHORE:
			event = IPCFlagWaitAny(ipc_val->p_osevent, VIPC_FLAG_RESOURCES, thisms);
			break;
		case VALUE_TYPE_FLAGS_SET:
			event = IPCFlagWaitAny(ipc_val->p_osevent, val, thisms);
			break;
		case VALUE_TYPE_FLAGS_CLR:
			event = IPCFlagWaitAny(ipc_val->p_osevent, val, thisms);
			break;
		case VALUE_TYPE_FLAGS_SETALL:
			event = IPCFlagWaitAll(ipc_val->p_osevent, val, thisms);
			break;
		case VALUE_TYPE_FLAGS_CLRALL:
			event = IPCFlagWaitAll(ipc_val->p_osevent, val, thisms);
			break;
		default:
			event = 0;
			break;
		}
		if (event > 0)
		{
			add_flag = TRUE;
		}
	}

	return val_t;
}

ipc_return_t *ipc_create_return(SINT8 *name, UINT32 dir_mask)
{
	static UINT32 ID_num = 0;
	UINT32 index = 0;
	ipc_return_t *ipc_return = NULL;

	ipc_return = vcalloc(sizeof(ipc_return_t), 1);
	if (ipc_return == NULL)
	{
		return NULL;
	}
	ipc_return->p_osevent = IPCFlagCreate("return", 0);
	if (ipc_return->p_osevent == NULL)
	{
		vfree(ipc_return);
		return NULL;
	}
	index = 0xFFFFFF & ((ID_num++ << 4) | 0x01);
	ipc_return->index = (VCORE_GetID() << 24) | index;
	vstrncpy(ipc_return->info.name, name, sizeof(ipc_return->info.name));
	ipc_return->info.used_mask = dir_mask;
	VIPCListEnQue(&ipc_return_list, (vipc_list_node_t *)ipc_return, 0);

	return ipc_return;
}

SINT32 ipc_connect_return(vipc_dir_typ_t dir, UINT32 idnum, UINT8 *payload, UINT32 size)
{
	ipc_return_t *ipc_return = NULL;

	IPCMutexLock(IPCMutexLock, IPCMAX_DELAY);
	ipc_return = (ipc_return_t *)VIPCListTakeQue(&ipc_return_list, &idnum, ipc_list_check_idnum);
	if (ipc_return != NULL)
	{
		ipc_return->buff = vmalloc(size);
		if (ipc_return->buff)
		{
			ipc_return->buff_len = size;
			vmemcpy(ipc_return->buff, payload, size);
			IPCFlagSet(ipc_return->p_osevent, VIPC_FLAG_RESOURCES);
		}
	}
	IPCMutexUnlock(IPCMutexLock);
	return 0;
}

SINT32 ipc_del_return(ipc_return_t *ipc_return)
{

	if (ipc_return == NULL)
	{
		return -1;
	}
	IPCMutexLock(IPCMutexLock, IPCMAX_DELAY);
	VIPCListTakeQue(&ipc_return_list, ipc_return, ipc_list_check_node);
	if (ipc_return->buff)
	{
		vfree(ipc_return->buff);
	}
	IPCMutexUnlock(IPCMutexLock);
	if (ipc_return->p_osevent)
	{
		IPCFlagDel(ipc_return->p_osevent);
	}
	vfree(ipc_return);

	return 0;
}

SINT32 ipc_send_return(UINT32 ID_num, void *msg, UINT32 size)
{
	int ret = 0;
	ipc_handle_t *buff = NULL;
	vipc_dir_typ_t dir = (vipc_dir_typ_t)(ID_num >> 24);

	if (dir == VCORE_GetID())
	{
		ipc_connect_return(dir, ID_num, msg, size);
	}
	else
	{
		buff = ipc_controlmsg_new(sizeof(ipc_handle_t) - 4 + size);
		if (buff == NULL)
		{
			return -1;
		}
		buff->type = IPC_RETURN_TYPE_VALUE;
		buff->idnum = ID_num;
		buff->size = size;
		vmemcpy(buff->payload, msg, size);

		ret = ipc_controlmsg_post(dir, buff);
		if (ret != 0)
		{
			ipc_controlmsg_del(buff);
		}
	}
	return ret;
}

void *ipc_recv_return(ipc_return_t *ipc_return, UINT32 timeout)
{
	void *buff = NULL;
	if (ipc_return == NULL)
	{
		return NULL;
	}
	IPCFlagWaitAny(ipc_return->p_osevent, VIPC_FLAG_RESOURCES, timeout);
	IPCMutexLock(IPCMutexLock, IPCMAX_DELAY);
	buff = ipc_return->buff;
	ipc_return->buff = NULL;
	IPCMutexUnlock(IPCMutexLock);
	return buff;
}

void *ipc_execute_callback(ipc_que_t *ipc_que, void *msg, UINT32 size, UINT32 timeout)
{
	void *ret_buff = NULL;
	if (ipc_que == NULL || msg == NULL || size == 0)
	{
		return NULL;
	}
	ipc_return_t *ipc_return = ipc_create_return(ipc_que->info.name, ipc_que->info.used_mask);
	if (ipc_return == NULL)
	{
		return NULL;
	}
	if (0 == ipc_send_queue(ipc_que, msg, size, 0x02, ipc_return->index))
	{
		ret_buff = ipc_recv_return(ipc_return, timeout);
	}
	else
	{
		vfree(msg);
	}
	ipc_del_return(ipc_return);

	return ret_buff;
}
