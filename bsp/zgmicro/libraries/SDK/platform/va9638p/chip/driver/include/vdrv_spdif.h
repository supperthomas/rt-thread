/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2020, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
	SPDIF

Description:
 	SPDIF Driver

Revision History:
 	Date                  Author                   Description
	2020-4-1				chen jianhong		
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSPDIF_H_
#define _VSPDIF_H_

#include "vdrv_dma.h"


#define SPDIF_24BIT_WIDTH     (0)
#define SPDIF_16BIT_WIDTH     (1)

#define SPDIF_MEM_CM          (0)
#define SPDIF_MEM_DSP         (1)

 
/* I2S1 channel enable */
#define VDMA_SPDIF_BOTH_CHANNEL_DISABLE		0x00
#define VDMA_SPDIF_LEFT_CHANNEL_ENABLE		0x01
#define VDMA_SPDIF_RIGHT_CHANNEL_ENABLE		0x02
#define VDMA_SPDIF_BOTH_CHANNEL_ENABLE		0x03

void VSPDIF_SetGPIO(void);
void VSPDIF_Init(UINT32 SampleRate, UINT8 dwidth);
void VSPDIF_finit(void);

void VSPDIF_DMA_WriteEnable(UINT8 WrEn);
void VSPDIF_DMA_WriteAction(VDMA_ACTION Action);
void VSPDIF_DMA_WriteConfig(BOOL WrSrc, UINT8 mode);
void VSPDIF_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VSPDIF_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VSPDIF_DMA_GetWritenCount(void);
UINT32 VSPDIF_DMA_GetWriteBufferLength(VDMA_BUF Buf);
VDMA_BUF VSPDIF_DMA_GetWriteIntSource(void);
VDMA_BUF VSPDIF_DMA_GetWriteActiveBuffer(void);
BOOL VSPDIF_DMA_IsWriteStopped(void);


#endif
