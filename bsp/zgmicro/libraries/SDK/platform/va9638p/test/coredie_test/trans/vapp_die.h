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
#ifndef _VAPP_DIE_H_
#define _VAPP_DIE_H_

#include "vstdlib.h"
#include "vapp_die_transmit.h"

#ifdef __cplusplus
extern "C"
{
#endif
	void Vapp_SlaveDie_Init(void);
	void Vapp_SlaveDie_Set_state(UINT8 state);
	UINT8 Vapp_SlaveDie_Get_mode(void);

	UINT8 Vapp_SlaveDie_HostSend(VAPPDieCmd_REQ_t cmd, UINT8 *arg, UINT8 size);
	UINT8 Vapp_SlaveDie_MountMode(SINT8 *p_name, SINT8 *m_name, UINT8 depth);
	UINT8 Vapp_SlaveDie_UnmountMode(SINT8 *m_name);
	UINT8 Vapp_SlaveDie_OperMode(SINT8 *m_name, UINT8 oper, UINT8 *arg, UINT8 size);

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_DIE_H_ */
