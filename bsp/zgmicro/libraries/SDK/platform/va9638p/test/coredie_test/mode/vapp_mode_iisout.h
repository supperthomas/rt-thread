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
#ifndef _VAPP_MODE_IISOUT_H_
#define _VAPP_MODE_IISOUT_H_

#include "vstdlib.h"
#include "vstddrv.h"
#include "vmode_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum
	{
		VMODEIISOutCMD_Enable,
		VMODEIISOutCMD_ListNum,
		VMODEIISOutCMD_ArgSet,
	} VMODEIISOutCMD_e;
	
	typedef struct
	{
		size_t pack_len;
		VDRVIISSel_t iis_sel;
		VDRVIISMode_t mode;
		VDRVIISDwidth_t dwidth;
		VDRVIISSampleRate_t SR;
		VDRVIIS_ISR_CALLBACK callback;
		UINT8 sync_en;
	} VMODEIISoutConfig_t;

	typedef struct
	{
		UINT8 *cache;
		SINT32 updata_diff;
		VDRVIISSel_t iis_sel;
		VDRVIISConfig_t config;
	} VMODEIISoutHandle_t;

#define VMODE_IISOUT_MODE ((SINT8 *)"iisout")
#define VMODE_IISOUT1_MODE ((SINT8 *)"iisout1")
	

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_MODE_IISOUT_H_ */
