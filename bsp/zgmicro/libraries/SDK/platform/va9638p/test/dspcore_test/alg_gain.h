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
#ifndef _ALG_GAIN_H_
#define _ALG_GAIN_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		UINT32 pack_len;
		UINT8 data_dwidth;
		UINT8 volume_val;
		UINT16 fade;
	} Alg_Gain_Handle;

	Alg_Gain_Handle *Alg_Gain_Init(UINT32 pack_len, UINT8 data_dwidth, UINT8 volume_val);
	UINT32 Alg_Gain_Uninit(Alg_Gain_Handle *handle);
	UINT32 Alg_Gain_Exec(Alg_Gain_Handle *handle, void *in_buff, UINT8 *out_buff);
	UINT32 Alg_Gain_SetVolume(Alg_Gain_Handle *handle, UINT8 volume);

#ifdef __cplusplus
}
#endif

#endif /* _ALG_GAIN_H_ */
