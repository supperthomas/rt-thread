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
	slave die mode

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstddrv.h"
#include "vcoroutine.h"
#include "vmode_common.h"

#undef VMODE_COMMON_DEBUG
#define VMODE_COMMON_DEBUG(...) DEBUG_PRINTF("[VMODE_COMMON]", __VA_ARGS__)

VMODECorHandle invalid;

INIT_MODE_EXPORT(VMODE_START, invalid, NULL, NULL, NULL, NULL, "0");

INIT_MODE_EXPORT(VMODE_FAST_START, invalid, NULL, NULL, NULL, NULL, "0.end");

INIT_MODE_EXPORT(VMODE_FAST_END, invalid, NULL, NULL, NULL, NULL, "1.end");

INIT_MODE_EXPORT(VMODE_END, invalid, NULL, NULL, NULL, NULL, "2.end");

VMODEInitDesc *VMODE_Find_Mode_name(SINT8 *name)
{
	const VMODEInitDesc *desc;
	for (desc = &__mode_init_desc_VMODE_START; desc < &__mode_init_desc_VMODE_END; desc++)
	{
		if (vstrcmp(desc->name, name) == 0)
		{
			return (VMODEInitDesc *)desc;
		}
	}
	return NULL;
}

VMODEInitDesc *VMODE_Find_Mode_node(VMODECorHandle *node, UINT8 *index)
{
	const VMODEInitDesc *desc;
	for (desc = &__mode_init_desc_VMODE_START; desc < &__mode_init_desc_VMODE_END; desc++)
	{
		for (UINT8 i = 0; i < desc->handle_num; i++)
		{
			if (node == desc->node + i)
			{
				if (index != NULL)
					*index = i;
				return (VMODEInitDesc *)desc;
			}
		}
	}
	return NULL;
}

UINT8 VMODE_Mount_Handle(VMODECorHandle *m_node, VMODECorHandle *p_node, VCORQue_t que_list)
{
	if (p_node != NULL)
	{
		m_node->send = p_node->send;
		p_node->send = que_list;
		m_node->next = p_node->next;
		if (p_node->next != NULL)
			p_node->next->pre = m_node;
		p_node->next = m_node;
	}
	else
	{
		m_node->send = NULL;
		m_node->next = NULL;
	}
	m_node->receive = que_list;
	m_node->pre = p_node;

	return 0;
}

UINT8 VMODE_Mount_Init(void)
{
	UINT8 ret = 0;
	const VMODEInitDesc *desc;
	for (desc = &__mode_init_desc_VMODE_START; desc < &__mode_init_desc_VMODE_END; desc++)
	{
		if (desc->init != NULL && desc->init() != 0)
		{
			ret++;
		}
	}
	return ret;
}

UINT8 VMODE_Mount_Mode(SINT8 *mode_name, SINT8 *pre_name, UINT8 que_depth)
{
	UINT8 ret;
	VMODEInitDesc *p_mode, *m_mode;
	VMODECorHandle *p_node = NULL, *m_node;
	VCORQue_t que_list = NULL;

	if (mode_name == NULL)
		return 1;

	if (pre_name != NULL && vstrlen(pre_name) > 0)
	{
		p_mode = VMODE_Find_Mode_name(pre_name);
		if (p_mode == NULL || p_mode->node == NULL)
		{
			VMODE_COMMON_DEBUG("%s mode find fail\r\n", pre_name);
			return 1;
		}
		p_node = (VMODECorHandle *)p_mode->node;
	}

	m_mode = VMODE_Find_Mode_name(mode_name);
	if (m_mode == NULL || m_mode->node == NULL || m_mode->mount == NULL)
	{
		VMODE_COMMON_DEBUG("%s mode find fail\r\n", mode_name);
		return 1;
	}

	m_node = (VMODECorHandle *)m_mode->node;
	ret = m_mode->mount(m_node);
	if (ret != 0)
	{
		VMODE_COMMON_DEBUG("%s->%s mode mount fail%d\r\n", pre_name, mode_name, ret);
		return 1;
	}
	
	if (que_depth > 0)
	{
		que_list = VCORQueCreate(que_depth);
		if (que_list == NULL)
		{
			VMODE_COMMON_DEBUG("VCORQue Create fail%d\r\n", que_depth);
			return 1;
		}
	}

	VMODE_Mount_Handle(m_node, p_node, que_list);

	return 0;
}

UINT8 VMODE_Mount_AddMode(VMODECorHandle *m_node, SINT8 *pre_name, UINT8 que_depth)
{
	UINT8 ret;
	VMODEInitDesc *p_mode;
	VMODECorHandle *p_node;
	VCORQue_t que_list = NULL;

	if (pre_name == NULL)
	{
		VMODE_COMMON_DEBUG("NULL mode find fail\r\n");
		return 1;
	}

	p_mode = VMODE_Find_Mode_name(pre_name);
	if (p_mode == NULL || p_mode->node == NULL)
	{
		VMODE_COMMON_DEBUG("%s mode find fail\r\n", pre_name);
		return 1;
	}
	p_node = (VMODECorHandle *)p_mode->node;

	if (que_depth > 0)
	{
		que_list = VCORQueCreate(que_depth);
		if (que_list == NULL)
		{
			VMODE_COMMON_DEBUG("VCORQue Create fail%d\r\n", que_depth);
			return 1;
		}
	}

	VMODE_Mount_Handle(m_node, p_node, que_list);

	return 0;
}

UINT8 VMODE_Mount_INVMode(SINT8 *m_name, VMODECorHandle *p_node, UINT8 que_depth)
{
	UINT8 ret;
	VMODEInitDesc *m_mode;
	VMODECorHandle *m_node;
	VCORQue_t que_list = NULL;

	if (m_name == NULL)
	{
		VMODE_COMMON_DEBUG("NULL mode find fail\r\n");
		return 1;
	}

	m_mode = VMODE_Find_Mode_name(m_name);
	if (m_mode == NULL || m_mode->node == NULL || m_mode->mount == NULL)
	{
		VMODE_COMMON_DEBUG("%s mode find fail\r\n", m_name);
		return 1;
	}

	if (que_depth > 0)
	{
		que_list = VCORQueCreate(que_depth);
		if (que_list == NULL)
		{
			VMODE_COMMON_DEBUG("VCORQue Create fail%d\r\n", que_depth);
			return 1;
		}
	}

	m_node = (VMODECorHandle *)m_mode->node;
	ret = m_mode->mount(m_node);
	if (ret != 0)
	{
		VMODE_COMMON_DEBUG("INV->%s mode mount fail%d\r\n", m_name, ret);
		return 1;
	}

	VMODE_Mount_Handle(m_node, p_node, que_list);

	return 0;
}

UINT8 VMODE_Unmount_Mode(SINT8 *mode_name)
{
	UINT8 ret;
	VMODEInitDesc *m_mode;
	UINT8 *buff = NULL, id;

	if (mode_name == NULL)
		return 1;

	m_mode = VMODE_Find_Mode_name(mode_name);
	if (m_mode == NULL || m_mode->node == NULL || m_mode->unmount == NULL)
	{
		VMODE_COMMON_DEBUG("%s mode find fail\r\n", mode_name);
		return 1;
	}

	ret = m_mode->unmount((VMODECorHandle *)m_mode->node);
	if (ret != 0)
	{
		VMODE_COMMON_DEBUG("%s mode unmount fail%d\r\n", mode_name, ret);
		return 1;
	}

	return 0;
}

UINT8 VMODE_Clean_Handle(VMODECorHandle *m_node)
{
	UINT8 *buff = NULL, id;

	if (m_node == NULL)
		return 1;

	while (vcorquereceive(m_node->receive, &id, &buff) == VRET_NO_ERROR)
	{
		if (vcorquesend(m_node->send, id, buff, 0) != VRET_NO_ERROR)
			vfree(buff);
	}

	if (m_node->pre != NULL)
	{
		m_node->pre->send = m_node->send;
		m_node->pre->next = m_node->next;
	}
	if (m_node->next != NULL)
	{
		m_node->next->pre = m_node->pre;
	}
	VCORQueDel(m_node->receive);

	return 0;
}

UINT8 VMODE_Mode_Operate(SINT8 *mode_name, UINT8 cmd, void *arg)
{
	UINT8 ret;
	VMODEInitDesc *m_mode;
	UINT8 *buff = NULL, id;

	if (mode_name == NULL)
		return 1;

	m_mode = VMODE_Find_Mode_name(mode_name);
	if (m_mode == NULL || m_mode->node == NULL || m_mode->unmount == NULL)
	{
		VMODE_COMMON_DEBUG("%s mode find fail\r\n", mode_name);
		return 1;
	}

	ret = m_mode->operate((VMODECorHandle *)m_mode->node, cmd, arg);
	if (ret != 0)
	{
		VMODE_COMMON_DEBUG("%s mode unmount fail%d\r\n", mode_name, ret);
		return 1;
	}

	return 0;
}

UINT8 VMODE_Display_Mode(void)
{
	UINT8 ret, index;
	VMODEInitDesc *m_mode;
	VMODECorHandle *node;
	SINT8 str_buff[256] = "";

	const VMODEInitDesc *desc, *tmp;
	for (desc = &__mode_init_desc_VMODE_START; desc < &__mode_init_desc_VMODE_END; desc++)
	{
		if (desc->node != &invalid && desc->node->state != 0)
		{
			for (UINT8 i = 0; i < desc->handle_num; i++)
			{
				node = (VMODECorHandle *)desc->node + i;
				if (node != NULL && node->pre == NULL)
				{
					if (i > 0)
					{
						if (node->next == NULL)
							continue;
						vcom_sprintf(&str_buff[vstrlen(str_buff)], "{%s(%d)}", desc->name, i);
					}
					else
						vcom_sprintf(&str_buff[vstrlen(str_buff)], "{%s}", desc->name);
					while (node != NULL && node->next != NULL)
					{
						tmp = VMODE_Find_Mode_node(node->next, &index);
						if (tmp == NULL)
							break;
						if (index > 0)
							vcom_sprintf(&str_buff[vstrlen(str_buff)], "->{%s(%d)}", tmp->name, index);
						else
							vcom_sprintf(&str_buff[vstrlen(str_buff)], "->{%s}", tmp->name);
						node = (VMODECorHandle *)tmp->node + index;
					}
					VMODE_COMMON_DEBUG("%s\r\n", str_buff);
					str_buff[0] = '\0';
				}
			}
		}
	}
	return 0;
}

//
void vmode_quereceive_merge(VCORQue_t p_corque, UINT8 **buff, UINT8 len)
{
	UINT8 id, *t_buff;
	vcorquereceive(p_corque, &id, &t_buff);
	while (t_buff != NULL)
	{
		if (id >= len)
		{
			VMODE_COMMON_DEBUG("index%d beyond range\r\n", id);
			vfree(t_buff);
		}
		else
		{
			buff[id] = t_buff;
			if (id == 0)
				break;
		}
		vcorquepeek(p_corque, &id, &t_buff);
		if (t_buff != NULL && id < len && buff[id] != NULL)
		{
			break;
		}
		vcorquereceive(p_corque, &id, &t_buff);
	}
	return;
}
