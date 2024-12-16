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
#include "vapp_die_transmit.h"
#include "vapp_die_cmd.h"

static UINT8 uart_buf[TRA_UART_FIFO_LEN];
static UINT8 tra_uart_rx_cmd;
static UINT8 tra_uart_rx_len;

#ifdef TAR_DIE_MEMMAP_LEN
UINT8 g_TAR_DIE_MemoryMap[TAR_DIE_MEMMAP_LEN] = {0};
#endif

#ifdef TAR_DIE_CALLBACK_NUM
TAR_DIE_CALLBACK_TYPE g_TAR_DIE_CallBack[TAR_DIE_CALLBACK_NUM] = {0};
#endif

static UINT8 tra_uart_tx_data(UINT8 cmd, UINT8 *buff, UINT8 len)
{
	if (len > 28)
		return 0;
	uart_buf[0] = 0x5A;
	uart_buf[1] = (~len & 0x3) << 6 | (len & 0x3) << 4 | (cmd & 0xF);
	uart_buf[2] = ((~len >> 2) & 0xF) << 4 | ((len >> 2) & 0xF);
	for (UINT8 i = 0; i < len; i++)
		uart_buf[3 + i] = buff[i];
	uart_buf[3 + len] = VCOMMON_CRC7(uart_buf + 3, len);
	VSYS_UART_CharTx(uart_buf, 4 + len);
	return len;
}

static UINT8 TRA_UART_Rx_Char(UINT8 ch)
{
	UINT8 ret = 0;
	static UINT8 tra_uart_rx_state = 0;
	static UINT8 tra_uart_rx_cnt;
	switch (tra_uart_rx_state)
	{
	case 0: // magic
	tra_uart_rx_state_err:
		if (ch == 0x5A)
			tra_uart_rx_state++;
		else
			tra_uart_rx_state = 0;
		break;
	case 1: // cmd/len
		if ((~ch >> 6 & 0x3) != (ch >> 4 & 0x3))
			goto tra_uart_rx_state_err;
		tra_uart_rx_cmd = ch & 0x0F;
		tra_uart_rx_len = ch >> 4 & 0x3;
		tra_uart_rx_state++;
		break;
	case 2: // len
		if ((~ch >> 4 & 0xF) != (ch & 0xF))
			goto tra_uart_rx_state_err;
		tra_uart_rx_cnt = 0;
		tra_uart_rx_len |= (ch & 0xF) << 2;
		tra_uart_rx_state++;
		if (tra_uart_rx_len > 0)
			break;
	case 3: // data
		if (tra_uart_rx_cnt < tra_uart_rx_len)
			uart_buf[tra_uart_rx_cnt++] = ch;
		if (tra_uart_rx_cnt == tra_uart_rx_len)
			tra_uart_rx_state++;
		break;
	case 4: // crc
		if (VCOMMON_CRC7(uart_buf, tra_uart_rx_len) != ch)
			goto tra_uart_rx_state_err;
		tra_uart_rx_state++;
	case 5: // submit
		ret = 1;
		tra_uart_rx_state = 0;
		break;
	default:
		goto tra_uart_rx_state_err;
	}
	return ret;
}

UINT8 TRA_UART_Rx_buff(UINT8 *buff, UINT8 len, UINT8 *ret_len)
{
	UINT8 ret, ch;
	while (VSYS_UART_CharRx(&ch, 1))
	{
		ret = TRA_UART_Rx_Char(ch);
		if (ret > 0)
		{
			len = MIN(len, tra_uart_rx_len);
			vmemcpy(buff, uart_buf, len);
			*ret_len = len;
			return tra_uart_rx_cmd;
		}
	}
	return TRA_UART_CMD_NONE;
}

static UINT8 TRA_UART_Rx_Wait(UINT8 cmd, UINT8 *buff, UINT8 len, UINT32 timeout)
{
	UINT8 ret_cmd, ret_len;
	do
	{
		ret_cmd = TRA_UART_Rx_buff(buff, len, &ret_len);
		if (ret_cmd > 0 && ret_cmd == cmd)
		{
			return ret_cmd;
		}
		vdelayus(10);
	} while (timeout == (UINT32)-1 || timeout-- > 0);
	return TRA_UART_CMD_NONE;
}

UINT8 TRA_UART_Send(UINT8 *buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_ORD, buff, len);
}

UINT8 TRA_UART_Ack(void)
{
	return tra_uart_tx_data(TRA_UART_CMD_ACK, NULL, 0);
}

UINT8 TRA_UART_Ack_Wait(UINT32 timeout)
{
	return TRA_UART_Rx_Wait(TRA_UART_CMD_ACK, NULL, 0, timeout);
}

UINT8 TRA_UART_Request(UINT8 *buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_REQ, buff, len);
}

UINT8 TRA_UART_Response(UINT8 *buff, UINT8 len)
{
	return tra_uart_tx_data(TRA_UART_CMD_REP, buff, len);
}

UINT8 TRA_UART_Response_Wait(UINT8 *buff, UINT8 len, UINT32 timeout)
{
	return TRA_UART_Rx_Wait(TRA_UART_CMD_REP, buff, len, timeout);
}

UINT8 TRA_UART_MemMap(UINT8 mem_index, UINT8 len, UINT8* buff)
{
	UINT8 sz = 2;
	VAPPDieCmd_AllFormat_t die_cmd = {0};
	die_cmd.cmd = mem_index;
	die_cmd.data[0] = len;
	if (buff != NULL)
	{
		sz = 2 + len;
		vmemcpy(&die_cmd.data[1], buff, len);
	}
	return tra_uart_tx_data(TRA_UART_CMD_MEM, (UINT8*)&die_cmd, sz);
}

UINT8 TRA_UART_MemRes(UINT8 *buff, UINT8 len)
{
#ifdef TAR_DIE_MEMMAP_LEN
	UINT8 mem_index = buff[0];
	UINT8 sz = buff[1];

	if (mem_index + sz <= TAR_DIE_MEMMAP_LEN)
	{
		if(len > 2 && len == sz + 2)
		{
			vmemcpy(g_TAR_DIE_MemoryMap + mem_index, &buff[2], sz);
		}
		return tra_uart_tx_data(TRA_UART_CMD_MRE, g_TAR_DIE_MemoryMap + mem_index, sz);
	}
	else
#endif
		return 0;
}

UINT8 TRA_UART_MemRes_Wait(UINT8 *buff, UINT8 len, UINT32 timeout)
{
	return TRA_UART_Rx_Wait(TRA_UART_CMD_MRE, buff, len, timeout);
}

UINT8 TRA_UART_CallBack(UINT8 call_index, UINT8 *buff, UINT8 len)
{
	VAPPDieCmd_AllFormat_t die_cmd = {0};
	if (len > membersizeof(VAPPDieCmd_AllFormat_t, data))
		return 0;
	die_cmd.cmd = call_index;
	vmemcpy(&die_cmd.data, buff, len);
	return tra_uart_tx_data(TRA_UART_CMD_CALL, (UINT8*)&die_cmd, membersizeof(VAPPDieCmd_AllFormat_t, cmd) + len);
}

UINT8 TRA_UART_CallRes(UINT8 *buff)
{
#ifdef TAR_DIE_CALLBACK_NUM
	UINT8 uart_tmp[TRA_UART_PAYLOAD_LEN], ret_len = 0, call_index = buff[0];
	if (call_index < TAR_DIE_CALLBACK_NUM && g_TAR_DIE_CallBack[call_index] != NULL)
	{
		ret_len = g_TAR_DIE_CallBack[call_index](buff + 1, uart_tmp, sizeof(uart_tmp));
		return tra_uart_tx_data(TRA_UART_CMD_BACK, uart_tmp, ret_len);
	}
	else
#endif
		return 0;
}

UINT8 TRA_UART_CallRes_Wait(UINT8 *buff, UINT8 len, UINT32 timeout)
{
	return TRA_UART_Rx_Wait(TRA_UART_CMD_BACK, buff, len, timeout);
}

UINT8 TRA_UART_Sync(void)
{
	static UINT16 sync_val = 0;
	UINT8 ret = 0;
	UINT16 sync_ret = 0;
	do
	{
		do
			sync_val++;
		while (sync_val == 0);
		tra_uart_tx_data(TRA_UART_CMD_SYNC, (UINT8 *)&sync_val, 2);
		do
			ret = TRA_UART_Rx_Wait(TRA_UART_CMD_SYRE, (UINT8 *)&sync_ret, 2, 100);
		while (ret > 0 && sync_ret != sync_val);
		vdelayus(1000 * 1000 * 2);
	} while (sync_ret != sync_val);
	return 0;
}

UINT8 TRA_UART_SyncRes(UINT8 *buff)
{
	return tra_uart_tx_data(TRA_UART_CMD_SYRE, buff, 2);
}
