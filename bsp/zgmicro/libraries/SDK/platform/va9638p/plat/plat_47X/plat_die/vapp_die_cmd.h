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
#ifndef _VAPP_DIE_CMD_H_
#define _VAPP_DIE_CMD_H_

#include "vstdlib.h"
#include "vapp_die_transmit.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TAR_DIE_MODE_NAME_LEN 8

#pragma anon_unions
#pragma pack (1)
	typedef enum
	{
		// fast command response, register callbacks if necessary
		VAPP_Die_CMDORD_state = 0x00,
		VAPP_Die_CMDORD_LIST_LIMIT, // this command is a delimiter and cannot be used
	} VAPPDieCmd_ORD_t;

	typedef enum
	{
		VAPP_Die_CMDREQ_MountMode,
		VAPP_Die_CMDREQ_UnmountMode,
		VAPP_Die_CMDREQ_OperMode,
		VAPP_Die_CMDREQ_DisplayMode,
		VAPP_Die_CMDREQ_AdjustDPLL,
		VAPP_Die_CMDREQ_StartAudio,
		VAPP_Die_CMDREQ_LIST_LIMIT, // this command is a delimiter and cannot be used
	} VAPPDieCmd_REQ_t;

	typedef struct
	{
		UINT8 cmd;
		UINT8 data[TRA_UART_PAYLOAD_LEN - 1];
	} VAPPDieCmd_AllFormat_t;

	typedef struct
	{
		SINT8 m_name[TAR_DIE_MODE_NAME_LEN];
		SINT8 p_name[TAR_DIE_MODE_NAME_LEN];
		UINT8 depth;
	} VAPPDie_ModeMount_t;

	typedef struct
	{
		SINT8 m_name[TAR_DIE_MODE_NAME_LEN];
	} VAPPDie_ModeUnmount_t;

	typedef struct
	{
		SINT8 m_name[TAR_DIE_MODE_NAME_LEN];
		UINT8 oper;
		UINT8 arg[TRA_UART_PAYLOAD_LEN - TAR_DIE_MODE_NAME_LEN - 2];
	} VAPPDie_ModeOper_t;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_DIE_CMD_H_ */
