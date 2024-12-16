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
    IIS

Description:
    IIS driver.

Revision History:
    Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDRV_ASRC_H_
#define _VDRV_ASRC_H_

#include "vstdlib.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************

 						Macro Definition

*******************************************************************************/


/******************************************************************************

 						Function Prototypes

*******************************************************************************/
void VASRC_SoftReset(UINT8 ASRC_Num);
void VASRC_SetAudioChannel(UINT8 ASRC_Num,UINT8 Channel);
void VASRC_SetSCDIV_step(UINT8 ASRC_Num,UINT32 step);
void VASRC_SetSCDIV_ceiling (UINT8 ASRC_Num,UINT32 ceiling);
UINT32 VASRC_GetSCDIV_step(UINT8 ASRC_Num);
UINT32 VASRC_GetSCDIV_ceiling (UINT8 ASRC_Num);
void VASRC_SetSampleRate(UINT8 ASRC_Num,UINT32 SR);
BOOL VASRC_Get_STATUS_locked(UINT8 ASRC_Num);
BOOL VASRC_Get_STATUS_fifo_overflow(UINT8 ASRC_Num);
void VASRC_LeAudioSetSampleRate(UINT8 ASRC_Num,UINT32 SR);

void VASRC_ReSampleEn(UINT8 ASRC_Num, BOOL enable);

#ifdef __cplusplus
}
#endif

#endif  /* _VDRV_ASRC_H_ */

