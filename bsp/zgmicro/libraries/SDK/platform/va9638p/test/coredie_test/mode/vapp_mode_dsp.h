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
#ifndef _VAPP_MODE_DSP_H_
#define _VAPP_MODE_DSP_H_

#include "vstdlib.h"
#include "vstddrv.h"
#include "vmode_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VDSP_PATH_MAX_SUPPORT (3)


#define VMODE_DSP_MODE ((SINT8 *)"dsp")

	typedef struct
	{
		UINT32 in_len;
		UINT32 out_len;
		UINT8 dwidth;
	} VMODEDSPPathCfg_t;
	
	typedef struct
	{
		UINT32 pack_len;
		UINT8 dwidth;
		UINT8 volume_val;
		VDRVCKMDspClk_t freq;
		VMODEDSPPathCfg_t path[VDSP_PATH_MAX_SUPPORT];
	} VMODEDSPCfg_t;

	typedef struct
	{
		UINT8 state;
		UINT8 dsppack_num;
	} VMODEDSPPath_t;

	typedef struct
	{
		UINT8* cache;
		VCORTcb_t msgBox_tcb;
		UINT8 cache_id;
		BOOL MsgBox_TxReq;
		UINT8 bypass;
		VMODEDSPPath_t path[VDSP_PATH_MAX_SUPPORT];
		UINT32 data_len;
	} VMODEDSPHandle_t;

	typedef enum
	{
		VMODEDSPCMD_SetVolume,
		VMODEDSPCMD_InitPath,
		VMODEDSPCMD_UninitPath,
		VMODEDSPCMD_ByPass,
		VMODEDSPCMD_MemDisplay,
		VMODEDSPCMD_DebugCheck,
		VMODEDSPCMD_Test1 = 0xF0,
		VMODEDSPCMD_Test2 = 0xF1,
		VMODEDSPCMD_Test3 = 0xF2,
	} VMODEDSPCMD_e;

#pragma anon_unions

//-----------msgbox id-----------------
#define VDSP_ID_BOX_PATH_DATA_BASE (0x0010)
#define VDSP_ID_BOX_PATH_CONFIG_BASE (0x0020)
#define VDSP_ID_BOX_PRINT_BASE (0xFFFF)

//-----------msgbox send id-----------------
#define VDSP_ID_BOX_COMM_BASE (0xFFFF)

	typedef enum
	{
		VDSPQue_ID_COMM,
		VDSPQue_ID_PathCfg,
	} VDSPQue_ID_e;

	typedef enum
	{
		VDSPQue_COMM_SetCycl,
		VDSPQue_COMM_OpenPath,
		VDSPQue_COMM_ClosePath,
		VDSPQue_COMM_MemDisplay,
		VDSPQue_COMM_Test1 = 0xF0,
		VDSPQue_COMM_Test2 = 0xF1,
		VDSPQue_COMM_Test3 = 0xF2,
	} VDSPQue_COMM_e;

	typedef struct
	{
		VDSPQue_COMM_e cmd;
		UINT32 buff[1];
	} VDSPQue_COMM_t;

	typedef struct
	{
		UINT8 path;
		SINT8 name[8];
		UINT32 buff[1];
	} VDSPQue_PathCfg_t;

	//--------------------alg---------------------------------
	typedef enum
	{
		VDSP_ALG_GAINcfg_SetConfig,
		VDSP_ALG_GAINcfg_Setdatalen,
		VDSP_ALG_GAINcfg_Getvolume,
		VDSP_ALG_GAINcfg_Setvolume,
	} VDSP_ALG_GAIN_Cfg_e;

	typedef struct
	{
		VDSP_ALG_GAIN_Cfg_e cmd;
		UINT8 data[1];
	} VDSP_ALG_GAIN_CFG_t;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_MODE_DSP_H_ */
