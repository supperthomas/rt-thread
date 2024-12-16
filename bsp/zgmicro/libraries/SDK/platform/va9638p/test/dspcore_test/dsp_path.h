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
	path

Description:
	.

Revision History:
	Date                  Author                   Description
	2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#ifndef _VAPP_DSP_PATH_H_
#define _VAPP_DSP_PATH_H_

#include "vstdlib.h"
#include "vcoroutine.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DSP_PATH_MAX_SUPPORT (3)

	typedef UINT32 (*VDSP_ALG_Init)(void *arg);
	typedef UINT32 (*VDSP_ALG_Uninit)(void *arg);
	typedef UINT32 (*VDSP_ALG_Hand)(void *arg, void *in_buff, UINT8 **out_buff);
	typedef UINT32 (*VDSP_ALG_Oper)(void *arg, void *in_buff);

	struct vdsp_alg_handle_s;

	typedef struct
	{
		VCORTcb_t tcb;
		VCORQue_t p_corque;
		UINT32 packin_len;
		UINT32 packout_len;
		struct vdsp_alg_handle_s *head;
		struct vdsp_alg_handle_s *tail;
	} VDSP_PATH_Algorit_t;

	typedef struct vdsp_alg_handle_s
	{
		SINT8 name[8];
		VDSP_ALG_Init init;
		VDSP_ALG_Uninit uninit;
		VDSP_ALG_Hand handling;
		VDSP_ALG_Oper oper;
		VDSP_PATH_Algorit_t *path;
		struct vdsp_alg_handle_s *pre;
		struct vdsp_alg_handle_s *next;
	} VDSP_ALG_HANDLE_t;

	extern VDSP_PATH_Algorit_t vdsp_path_alg[DSP_PATH_MAX_SUPPORT];

	UINT32 VDsp_Path_Init(UINT8 path, UINT8 depth, UINT32 packin_len, UINT32 packout_len);
	UINT32 VDsp_Path_Uninit(UINT8 path);
	UINT32 VDsp_Path_Oper(UINT8 path, SINT8 *alg_name, void *arg);
	UINT32 VDsp_Path_Get_PackInLen(UINT8 path);
	UINT32 VDsp_Path_Get_PackOutLen(UINT8 path);

	UINT32 VDsp_Path_Alg_Add(UINT8 path, VDSP_ALG_HANDLE_t *handle);
	UINT32 VDsp_Path_Alg_Minus(VDSP_ALG_HANDLE_t *handle);
	UINT32 VDsp_Path_Alg_GetPackIn(VDSP_ALG_HANDLE_t *handle);
	UINT32 VDsp_Path_Alg_GetPackOut(VDSP_ALG_HANDLE_t *handle);
	void *VDsp_Path_Alg_GetHandle(VDSP_ALG_HANDLE_t *handle);
	UINT8 VDsp_Path_Alg_GetIndex(VDSP_ALG_HANDLE_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* _VAPP_DSP_PATH_H_ */
