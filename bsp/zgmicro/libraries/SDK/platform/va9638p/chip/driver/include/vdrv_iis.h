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
#ifndef _VDRV_IIS_H_
#define _VDRV_IIS_H_

#include "vstdlib.h"
#ifdef __cplusplus
extern "C" {
#endif


#include "vdrv_dma.h"

typedef  void (*VI2S_HW_CALLBACK)(UINT8*, UINT8*, UINT32);

typedef struct _VI2S_HW_READ_DATA {
	UINT8* p_data0; /* for left data */
	UINT8* p_data1; /* for right data */
} VI2S_HW_READ_DATA;

/* I2S HW Device */
typedef struct _VI2S_HW_DEV {
	UINT8 stat;
    UINT8 int_stat;
    UINT8 chn;
	VI2S_HW_CALLBACK cm_callback;
    VI2S_HW_CALLBACK dsp_callback;
}VI2S_HW_DEV;

/* I2S HW */
typedef struct _VI2S_HW {
	BOOL init;
    UINT8 dma_size;         /* as same as data width */
    VI2S_HW_DEV rd_status;  /* not used */
    VI2S_HW_DEV wr_status;
}VI2S_HW;


#define I2S_CH0             (0)
#define I2S_CH1             (1)

#define I2S_SLAVE_MODE      (0)
#define I2S_MASTER_MODE     (1)

#define I2S_24BIT_WIDTH     (0)
#define I2S_16BIT_WIDTH     (1)
#define I2S_32BIT_WIDTH     (2)

#define I2S_MEM_CM          (0)
#define I2S_MEM_DSP         (1)

/* 
 * I2S0     format - 
            0: I2S format, serial data is active from the second sclk.
            1: reserved.
            2: Left-Justified.
            3: Right-Justified.
*/
#define I2S_STANDARD_FORMAT		      (0)
#define I2S_Left_JUSTIFIED_FORMAT     (2)
#define I2S_RIGHT_JUSTIFIED_FORMAT    (3)

/* I2S channel enable */
#define VDMA_I2S_BOTH_CHANNEL_DISABLE		0x00
#define VDMA_I2S_LEFT_CHANNEL_ENABLE		0x01
#define VDMA_I2S_RIGHT_CHANNEL_ENABLE		0x02
#define VDMA_I2S_BOTH_CHANNEL_ENABLE		0x03

void VI2S_SetGPIO(UINT8 channel);
void VI2S_Init(UINT8 channel, UINT8 mode, UINT8 dwidth, UINT32 SR, BOOL hi_clk, UINT8 format);
void VI2S_finit(UINT8 channel);
BOOL VI2S_GetTx(UINT8 channel);
BOOL VI2S_GetRx(UINT8 channel);
void VI2S_SetTx(UINT8 channel, BOOL En);
void VI2S_SetRx(UINT8 channel, BOOL En);
void VI2S_Both_SetTx(BOOL En);
void VI2S_Sync_SetTRx(UINT32 bitval);

/* I2S0 */
void VI2S0_DMA_WriteEnable(UINT8 WrEn);
void VI2S0_DMA_WriteAction(VDMA_ACTION Action);
void VI2S0_DMA_WriteConfig(BOOL WrSrc, UINT8 mode);
void VI2S0_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VI2S0_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VI2S0_DMA_GetWritenCount(void);
UINT32 VI2S0_DMA_GetWriteBufferLength(VDMA_BUF Buf);
VDMA_BUF VI2S0_DMA_GetWriteIntSource(void);
VDMA_BUF VI2S0_DMA_GetWriteActiveBuffer(void);
BOOL VI2S0_DMA_IsWriteStopped(void);


void VI2S0_DMA_ReadEnable(UINT8 RdEn);//RdEn should set to VDMA_I2S_RIGHT_CHANNEL_ENABLE
void VI2S0_DMA_ReadAction(VDMA_ACTION Action);
void VI2S0_DMA_ReadConfig(BOOL RdSrc, UINT8 mode);
void VI2S0_DMA_GetRtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
void VI2S0_DMA_GetLtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VI2S0_DMA_GetReadCount(void);
UINT32 VI2S0_DMA_GetReadBufferLength(VDMA_BUF Buf);
VDMA_BUF VI2S0_DMA_GetReadIntSource(void);
VDMA_BUF VI2S0_DMA_GetReadActiveBuffer(void);
BOOL VI2S0_DMA_IsReadStopped(void);
void VI2S0_DMA_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop);

void VI2S0_DMA_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1);
void VI2S0_DMA_ConfigReadRtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VI2S0_DMA_ConfigReadLtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);


/* I2S1 */
void VI2S1_DMA_WriteEnable(UINT8 WrEn);
void VI2S1_DMA_WriteAction(VDMA_ACTION Action);
void VI2S1_DMA_WriteConfig(BOOL WrSrc, UINT8 mode);
void VI2S1_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VI2S1_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VI2S1_DMA_GetWritenCount(void);
UINT32 VI2S1_DMA_GetWriteBufferLength(VDMA_BUF Buf);
VDMA_BUF VI2S1_DMA_GetWriteIntSource(void);
VDMA_BUF VI2S1_DMA_GetWriteActiveBuffer(void);
BOOL VI2S1_DMA_IsWriteStopped(void);

void VI2S1_DMA_ReadEnable(UINT8 RdEn);//RdEn should set to VDMA_I2S_RIGHT_CHANNEL_ENABLE
void VI2S1_DMA_ReadAction(VDMA_ACTION Action);
void VI2S1_DMA_ReadConfig(BOOL RdSrc, UINT8 mode);
void VI2S1_DMA_GetRtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
void VI2S1_DMA_GetLtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VI2S1_DMA_GetReadCount(void);
UINT32 VI2S1_DMA_GetReadBufferLength(VDMA_BUF Buf);
VDMA_BUF VI2S1_DMA_GetReadIntSource(void);
VDMA_BUF VI2S1_DMA_GetReadActiveBuffer(void);
BOOL VI2S1_DMA_IsReadStopped(void);
void VI2S1_DMA_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop);

void VI2S1_DMA_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1);
void VI2S1_DMA_ConfigReadRtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VI2S1_DMA_ConfigReadLtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);


void VI2S0_Set_BB_AD_Enable(BOOL enable);
void VI2S1_Set_BB_AD_Enable(BOOL enable);
void VI2S0_Set_BB_DA_Enable(BOOL enable);
void VI2S1_Set_BB_DA_Enable(BOOL enable);

void VI2S_InitClk_Once(void);
void VI2S_UninitClk_Once(void);

#ifdef __cplusplus
}
#endif

#endif  /* _VDRV_IIS_H_ */

