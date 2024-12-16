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
#ifndef _VAPP_MODE_SOFTIN_H_
#define _VAPP_MODE_SOFTIN_H_

#include "vstdlib.h"
#include "vstddrv.h"
#include "vmode_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		VMODESOFTinCMD_Enable,
		VMODESOFTinCMD_SetFreq,
	} VMODESOFTinCMD_e;
	
	typedef struct
	{
		UINT8 chan;
		UINT8 dwidth;
		UINT8 divisor;
		size_t pack_len;
		UINT32 SR;
		UINT32 freq;
	} VMODESOFTinConfig_t;

	typedef struct
	{
		UINT16 pcm_len;
		UINT16 pcm_offset;
		UINT8 *pcm_buff;
		UINT8 *cache;
		UINT8 init_cnt;
		UINT8 en;
		UINT32 syn_val;
	} VMODESOFTinHandle_t;

#define VMODE_SOFTIN_MODE ((SINT8*)"softin")


#ifdef __cplusplus
}
#endif

#endif /* _VAPP_MODE_SOFTIN_H_ */
