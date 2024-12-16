/*------------------------------------------------------------------------------
					COPYRIGHT (C) 2017, Vimicro Corporation
							  ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
	gain

Description:
	.

Revision History:
	Date                  Author                   Description
	2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#ifndef _VDSP_ALG_GAIN_H_
#define _VDSP_ALG_GAIN_H_

#include "vstdlib.h"
#include "dsp_path.h"
#include "alg_gain.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		VDSP_ALG_HANDLE_t handle;
		Alg_Gain_Handle *hd;
		UINT8 data_dwidth;
		UINT8 volume_val;
	} VDSP_ALG_GAIN_Handle;

	typedef enum
	{
		VDSP_ALG_GAINcfg_SetConfig,
		VDSP_ALG_GAINcfg_Setdatalen,
		VDSP_ALG_GAINcfg_Setdwidth,
		VDSP_ALG_GAINcfg_Setvolume,
	} VDSP_ALG_GAIN_Cfg_e;

	typedef struct
	{
		VDSP_ALG_GAIN_Cfg_e cmd;
		UINT8 data[1];
	} VDSP_ALG_GAIN_CFG_t;

UINT32 VDsp_Alg_Gain_Init(void *arg);
UINT32 VDsp_Alg_Gain_Uninit(void *arg);
UINT32 VDsp_Alg_Gain_Handling(void *arg, void *in_buff, UINT8 **out_buff);
UINT32 VDsp_Alg_Gain_Oper(void *arg, void *buff);
	
VDSP_ALG_GAIN_Handle *VDsp_ALG_Gain_Create(UINT8 data_dwidth, UINT8 volume_val);
UINT32 VDsp_Path_Gain_Delete(VDSP_ALG_GAIN_Handle *handle);

#ifdef __cplusplus
}
#endif

#endif /* _VDSP_ALG_GAIN_H_ */
