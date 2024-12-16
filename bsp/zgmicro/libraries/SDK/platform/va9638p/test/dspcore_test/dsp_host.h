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
	host

Description:
	trans

Revision History:
	Date                  Author                   Description
	2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#ifndef _VAPP_DSP_HOST_H_
#define _VAPP_DSP_HOST_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VDSP_ID_PATH_DATA_BASE (0x10)
#define VDSP_ID_PATH_CONFIG_BASE (0x20)
#define VDSP_ID_PATH_RETDATA_BASE (0xFF)

#pragma pack(1)
	typedef enum
	{
		VDSPQue_ID_COMM,
		VDSPQue_ID_PathCfg,
	} VDSPQue_ID_e;

	typedef enum
	{
		VDSPQue_COMM_SetCycl,
	} VDSPQue_COMM_e;

	typedef struct
	{
		VDSPQue_COMM_e cmd;
		UINT32 buff[1];
	} VDSPQue_COMM_t;

	typedef enum
	{
		VDSPQue_PathCfg_SetConfig,
		VDSPQue_PathCfg_Setdatalen,
		VDSPQue_PathCfg_Setdwidth,
		VDSPQue_PathCfg_Setvolume,
	} VDSPQue_PathCfg_e;

	typedef struct
	{
		UINT8 path;
		UINT32 buff[1];
	} VDSPQue_PathCfg_t;

	typedef struct
	{
		union
		{
			VDSPQue_COMM_t comm;
			VDSPQue_PathCfg_t pathCfg;
		} u;
	} VDSPCmd_Format_t;
#pragma pack()

UINT8 VDsp_Host_Init(void);
void VDsp_Host_Demo(void);

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_DSP_HOST_H_ */
