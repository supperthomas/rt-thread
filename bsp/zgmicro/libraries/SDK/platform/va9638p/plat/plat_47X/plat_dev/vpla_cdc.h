/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
        vpla cdc

Description:
        cdc Device
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_CDC_H_
#define _VPLA_CDC_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		VSYS_CDC_CH0IN = 0,
		VSYS_CDC_CH0OUT = 1,
		VSYS_CDC_CH_LIMIT,
	} VDRVCDCSel_t;

	typedef enum
	{
		VSYS_CDC_WIDTH_BIT24 = 0,
		VSYS_CDC_WIDTH_BIT16 = 1,
		VSYS_CDC_WIDTH_BIT32 = 1,
	} VDRVCDCDwidth_t;

	typedef enum
	{
		VSYS_CDC_SR_8000 = 8000,
		VSYS_CDC_SR_16000 = 16000,
		VSYS_CDC_SR_24000 = 24000,
		VSYS_CDC_SR_48000 = 48000,
		VSYS_CDC_SR_96000 = 96000,
	} VDRVCDCSampleRate_t;

	typedef void (*VDRVCDC_ISR_CALLBACK)(UINT32 ErrorCode);

	typedef enum
	{
		VSYS_CDC_BOTH_CHANNEL_DISABLE = 0,
		VSYS_CDC_LEFT_CHANNEL_ENABLE = 1,
		VSYS_CDC_RIGHT_CHANNEL_ENABLE = 2,
		VSYS_CDC_BOTH_CHANNEL_ENABLE = 3,
	} VDRVCDCChannel_t;

	typedef enum
	{
		VSYS_CDC_DataMode_CROSS = 0,
		VSYS_CDC_DataMode_HEADTAIL = 1,
	} VDRVCDCDataMode_t;

	typedef struct
	{
		size_t len;
		UINT8* buff;
		VDRVCDCDwidth_t dwidth;
		VDRVCDCSampleRate_t SR;
		VDRVCDCChannel_t channel;
		VDRVCDC_ISR_CALLBACK callback;
		VDRVCDCDataMode_t data_mode;
		UINT8 ctrl_mode;//bit[0]:control PA, 0 means it will control pa register in interface; 1 means it doesn't control but fade pop will control pa register
	} VDRVCDCConfig_t;

	typedef enum
	{
		VSYS_CDC_Gain = 0,
		VSYS_CDCState,
		VSYS_CDCSyncSet,
		VSYS_CDCSyncEn,
	} VDRVCDCInfo_t;

	extern VRET_VALUE VSYS_CDC_Init(VDRVCDCSel_t cdc_sel, VDRVCDCConfig_t *CDC_config);
	extern void* VSYS_CDC_Uninit(VDRVCDCSel_t cdc_sel);
	extern void VSYS_CDC_Enable(VDRVCDCSel_t cdc_sel, BOOL en);
	extern size_t VSYS_CDC_Update(VDRVCDCSel_t cdc_sel, UINT8 *data, size_t size);
	extern UINT32 VSYS_CDC_GetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info);
	extern VRET_VALUE VSYS_CDC_SetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info, UINT32 val);

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_CDC_H_ */
