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
	slave die

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
#include "vapp_die.h"
#include "vapp_die_transmit.h"

#undef VAPP_DIE_DEBUG
#define VAPP_DIE_DEBUG(...) DEBUG_PRINTF("[VAPP_DIE]", __VA_ARGS__)

extern VCORQue_t ConfigList;

__IRQ void VISR_Uart(void)
{
	UINT32 state;
	UINT8 buff[32], cmd, len;
	VRET_VALUE ret = VRET_NO_ERROR;

	state = VSYS_UART_GetStatus(VSYS_UART_Interrupt_Status);

	if (state & VSYS_UART_RX_INT)
	{
		cmd = TRA_UART_Rx_buff(buff, sizeof(buff), &len);
		if (cmd > 0)
		{
			if (TRA_UART_CMD_MEM == cmd)
			{
				if (len >= 2)
					TRA_UART_MemRes(buff, len);
			}
			else if (TRA_UART_CMD_CALL == cmd)
			{
				if (len > 0)
					TRA_UART_CallRes(buff);
			}
			else if (TRA_UART_CMD_SYNC == cmd)
			{
				if (len == 2)
					TRA_UART_SyncRes(buff);
			}
			else if (cmd & 0x01)
			{
				if (TRA_UART_CMD_ORD == cmd)
					TRA_UART_Ack();
				ret = vcorquesend(ConfigList, cmd, buff, len);
				if (ret != VRET_NO_ERROR)
					VAPP_DIE_DEBUG("%d send error\r\n", __LINE__);
			}
		}
	}
	VSYS_UART_ClearInt();
}

void Vapp_SlaveDie_Set_state(UINT8 state)
{
	g_TAR_DIE_MemoryMap[0] = state;
}

UINT8 Vapp_SlaveDie_Get_mode(void)
{
	while((*(volatile UINT8*)&g_TAR_DIE_MemoryMap[1]) == 0);
	return g_TAR_DIE_MemoryMap[1];
}

void Vapp_SlaveDie_Init(void)
{
	VDRVUartConfig_t uartconfig = {0};
	uartconfig.Baudrate = 921600;//512000
	uartconfig.port_sel = VSYS_UART_PORT_SEL_AUX;//VSYS_UART_PORT_SEL_AUX;//VSYS_UART_PORT_SEL_PRIMARY;
	uartconfig.Parity = VSYS_UART_PARITY_NONE;
	uartconfig.StopBit = VSYS_UART_STOP_BIT1;
	uartconfig.BitWidth = VSYS_UART_CHAR_BIT8;
	uartconfig.mode = VSYS_UART_INT_MODE;
	uartconfig.RxFifolevel = 16;
	VSYS_UART_Init(&uartconfig);
}

UINT8 Vapp_SlaveDie_HostSend(VAPPDieCmd_REQ_t cmd, UINT8 *arg, UINT8 size)
{
	VAPPDieCmd_AllFormat_t all_format;

	if (size > sizeof(all_format.data))
		return 1;
	all_format.cmd = cmd;
	vmemcpy(all_format.data, arg, size);

	vcorquesend(ConfigList, TRA_UART_CMD_ORD, (UINT8 *)&all_format, size + sizeof(all_format.cmd));
	return 0;
}

UINT8 Vapp_SlaveDie_MountMode(SINT8 *p_name, SINT8 *m_name, UINT8 depth)
{
	VAPPDie_ModeMount_t die_cmd;

	vstrncpy(die_cmd.m_name, m_name, sizeof(die_cmd.m_name));
	if (p_name == NULL)
		die_cmd.p_name[0] = '\0';
	else
		vstrncpy(die_cmd.p_name, p_name, sizeof(die_cmd.m_name));
	die_cmd.depth = depth;

	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_MountMode, (UINT8*)&die_cmd, sizeof(die_cmd));
	return 0;
}

UINT8 Vapp_SlaveDie_UnmountMode(SINT8 *m_name)
{
	VAPPDie_ModeUnmount_t die_cmd;

	vstrncpy(die_cmd.m_name, m_name, sizeof(die_cmd.m_name));

	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_MountMode, (UINT8*)&die_cmd, sizeof(die_cmd));
	return 0;
}

UINT8 Vapp_SlaveDie_OperMode(SINT8 *m_name, UINT8 oper, UINT8 *arg, UINT8 size)
{
	VAPPDie_ModeOper_t die_cmd;

	if (size > sizeof(die_cmd.arg))
		return 1;
	vstrncpy(die_cmd.m_name, m_name, sizeof(die_cmd.m_name));
	die_cmd.oper = oper;
	vmemcpy(die_cmd.arg, arg, size);

	Vapp_SlaveDie_HostSend(VAPP_Die_CMDREQ_OperMode, (UINT8*)&die_cmd, size + sizeof(die_cmd.oper) + sizeof(die_cmd.m_name));
	return 0;
}
