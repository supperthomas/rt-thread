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
#ifndef _VAPP_DIE_TRANS_H_
#define _VAPP_DIE_TRANS_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TRA_UART_FIFO_LEN 32
#define TRA_UART_PAYLOAD_LEN 28

#define TAR_DIE_MEMMAP_LEN 32
#define TAR_DIE_CALLBACK_NUM 16

#include "vapp_die_cmd.h"

#define TRA_UART_CMD_NONE 0
#define TRA_UART_CMD_ORD 1 // 无请求内容，ACK应答即可
#define TRA_UART_CMD_ACK 2
#define TRA_UART_CMD_REQ 3 // 有请求内容，REP回应数据
#define TRA_UART_CMD_REP 4
#define TRA_UART_CMD_MEM 5 // 内存映射，字节为单位，MRE回应数据
#define TRA_UART_CMD_MRE 6
#define TRA_UART_CMD_CALL 7 // 回调函数，BACK回应数据
#define TRA_UART_CMD_BACK 8
#define TRA_UART_CMD_SYNC 9 // 同步包，SYRE回应数据
#define TRA_UART_CMD_SYRE 10

#ifdef TAR_DIE_MEMMAP_LEN
	extern UINT8 g_TAR_DIE_MemoryMap[TAR_DIE_MEMMAP_LEN];
#endif

	typedef UINT8 (*TAR_DIE_CALLBACK_TYPE)(UINT8 *arg, UINT8 *buff, UINT8 len);

	// timeout: the unit is 10us
	UINT8 TRA_UART_Rx_buff(UINT8 *buff, UINT8 len, UINT8 *ret_len);
	UINT8 TRA_UART_Send(UINT8 *buff, UINT8 len);
	UINT8 TRA_UART_Ack(void);
	UINT8 TRA_UART_Ack_Wait(UINT32 timeout);

	UINT8 TRA_UART_Request(UINT8 *buff, UINT8 len);
	UINT8 TRA_UART_Response(UINT8 *buff, UINT8 len);
	UINT8 TRA_UART_Response_Wait(UINT8 *buff, UINT8 len, UINT32 timeout);

	UINT8 TRA_UART_MemMap(UINT8 mem_index, UINT8 len, UINT8* buff);
	UINT8 TRA_UART_MemRes(UINT8 *buff, UINT8 len);
	UINT8 TRA_UART_MemRes_Wait(UINT8 *buff, UINT8 len, UINT32 timeout);

	UINT8 TRA_UART_CallBack(UINT8 call_index, UINT8 *buff, UINT8 len);
	UINT8 TRA_UART_CallRes(UINT8 *buff);
	UINT8 TRA_UART_CallRes_Wait(UINT8 *buff, UINT8 len, UINT32 timeout);

	UINT8 TRA_UART_Sync(void);
	UINT8 TRA_UART_SyncRes(UINT8 *buff);

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_DIE_TRANS_H_ */
