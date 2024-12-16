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
    DMA DSP

Description:
     header file for vdma_dsp.c

Revision History:
     Date                  Author                   Description
     2013.03.24        Deng Zhigao        initial version
    2017.11.10      Chanjiang       update for 9638b
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef VMBOX_DSP_H
#define VMBOX_DSP_H

#include "vreg_def.h"

#define DSP_TO_CTP   1
#define DSP_TO_CBT   0

#ifdef CM3_TOP_CORE
    #define CM3_CORE_SEL       DSP_TO_CTP
#else
    #define CM3_CORE_SEL       DSP_TO_CBT
#endif

#define MAX_MSG_COUNT   3

#define IRQ_MCUIPC_INT					27

#define VMBOX_REG_CM3TP_CM3BT_INT_BIT 0
#define VMBOX_REG_CM3TP_DSP_INT_BIT   1
#define VMBOX_REG_CM3BT_CM3TP_INT_BIT 0
#define VMBOX_REG_CM3BT_DSP_INT_BIT 1

#define VMBOX_INT_TOP2BT_SET()   SET_REG_BIT2(REG_MSG_CM3MBX_CTRL0,0)
#define VMBOX_INT_TOP2BT_CLR()   CLR_REG_BIT2(REG_MSG_CM3MBX_CTRL0,0)
#define VMBOX_INT_TOP2DSP_SET()  SET_REG_BIT2(REG_MSG_CM3MBX_CTRL0,1)
#define VMBOX_INT_TOP2DSP_CLR()  CLR_REG_BIT2(REG_MSG_CM3MBX_CTRL0,1)
#define VMBOX_INT_BT2TOP_SET()   SET_REG_BIT2(REG_MSG_CM3MBX_CTRL1,0)
#define VMBOX_INT_BT2TOP_CLR()   CLR_REG_BIT2(REG_MSG_CM3MBX_CTRL1,0)
#define VMBOX_INT_BT2DSP_SET()   SET_REG_BIT2(REG_MSG_CM3MBX_CTRL1,1)
#define VMBOX_INT_BT2DSP_CLR()   CLR_REG_BIT2(REG_MSG_CM3MBX_CTRL1,1)

#define VMBOX_MSG_CM3BT_MSG0     REG(REG_MSG_CM3BT_MSG0)
#define VMBOX_MSG_CM3BT_MSG1     REG(REG_MSG_CM3BT_MSG1)
#define VMBOX_MSG_CM3BT_MSG2     REG(REG_MSG_CM3BT_MSG2)

#define VMBOX_MSG_DSP_MSG0       REG(REG_MSG_DSP_MSG0)
#define VMBOX_MSG_DSP_MSG1       REG(REG_MSG_DSP_MSG1)
#define VMBOX_MSG_DSP_MSG2       REG(REG_MSG_DSP_MSG2)

/* macro definition */
typedef  void (*VMBOX_DSP_CALLBACK)(void *);

#define DSP_RESET_MODE_CORE								0
#define DSP_RESET_MODE_WHOLE								1

#define DSP_HOLD_MODE_NONE								0			
#define DSP_HOLD_MODE_IO									(1<<0)
#define DSP_HOLD_MODE_CORE								(1<<1)
#define DSP_HOLD_MODE_WHOLE								(DSP_HOLD_MODE_IO|DSP_HOLD_MODE_CORE)

#define VMBOX_DSP_INT0										0
#define VMBOX_DSP_INT1										1
#define VMBOX_DSP_INT2										2

#define VMBOX_MESSAGE_SIZE_BYTE							16

/*Bit definition of ctrl2 */
#define VMBOX_REG_CTRL2_BIT_RESET_DSS					0
#define VMBOX_REG_CTRL2_BIT_RESET_DSP					1
#define VMBOX_REG_CTRL2_BIT_IO_HOLD				     	2
#define VMBOX_REG_CTRL2_BIT_CORE_HOLD					3
#define VMBOX_REG_CTRL2_BIT_DMS_HOLD_EN			    	4
#define VMBOX_REG_CTRL2_BIT_RESTART				    	5
#define VMBOX_REG_CTRL2_BIT_DSP_CLK_EN                  6
#define VMBOX_REG_CTRL2_BIT_DSP_96M_INDMA               7
#define VMBOX_REG_CTRL2_BITS_BREAK_IN					8

#define VMBOX_REG_CTRL2_RESET_MASK						0x3
#define VMBOX_REG_CTRL2_HOLD_MASK						(0x7<<2)
#define VMBOX_REG_CTRL2_BIT_HOLD						2
#define VMBOX_REG_CTRL2_DSP_CLK_MASK                   BIT6
#define VMBOX_REG_CTRL2_BIT_DSP_CLK_EN                 6

/*Bit definition of dsp stat */
#define VMBOX_REG_DSP_STAT_D2CTP_INT                    0
#define VMBOX_REG_DSP_STAT_D2CBT_INT                    1
#define VMBOX_REG_DSP_STAT_STOP_ACK                     4
#define VMBOX_REG_DSP_STAT_CORE_HOLD_ACK                5
#define VMBOX_REG_DSP_STAT_HALT_ACK                     6
#define VMBOX_REG_DSP_STAT_BREAK_OUT                    7
#define VMBOX_REG_DSP_STAT_IO_HOLD_USER              8

/*Bit definition of DSP_CACH_UPD */
#define VMBOX_REG_DSP_CACH_UPD_REQ                      0
#define VMBOX_REG_DSP_CACH_UPD_RDY                      4

#define VMBOX_C2D_INT_CLR0_MASK							0x00000001
#define VMBOX_C2D_INT_CLR1_MASK							0x00000001
#define VMBOX_C2D_INT_CLR2_MASK							0x00000001

#define VMBOX_D2C_INT_STATE0_MASK						0x00000001
#define VMBOX_D2C_INT_STATE1_MASK						0x00000002
#define VMBOX_D2C_INT_STATE2_MASK						0x00000004

void VMBOX_EnableInt(void);
void VMBOX_DisableInt(void);
void VMBOX_DSP_Interrupt_Routine_Service(void);
void VMBOX_DSP_RegiserCallback(UINT8 src, VMBOX_DSP_CALLBACK callback);
UINT8 VMBOX_DSP_GetState(void);
UINT8 VMBOX_DSP_INT_Get(void);
void VMBOX_DSP_INT_CLR(void);
void VMBOX_DSP_ClkEnable(UINT8 en);
void VMBOX_DSP_Reset(UINT8 mode);
void VMBOX_DSP_Enable(void);
void VMBOX_DSP_Hold(UINT8 mode);
void VMBOX_DSP_Unhold(void);
void VMBOX_DSP_MemHoldEnable(void);
void VMBOX_DSP_MemHoldDisable(void);
void VMBOX_DSP_Restart(void);
void VMBOX_DSP_ClrRestart(void);
void VMBOX_DSP_SetClk(UINT8 div);
UINT8 VMBOX_DSP_GetIntStat(int num);
void VMBOX_DSP_SetInterrupt(int num);
void VMBOX_DSP_MessageSend(int num);
void VMBOX_DSP_WriteAlgConfig(UINT32 *config, UINT32 length);
UINT32 VMBOX_DSP_GetDspState(void);
void VMBOX_DSP_Interrupt_Callback(UINT8 src, void *message);

#if VDEV_BSP32_SUPPORTED
void VMBOX_DSP_SetCacheUpload(void);
void VMBOX_DSP_ClrCacheUpload(void);    
void VMBOX_DSP_Interrupt_Routine_Service_Level1(UINT32 *p_d2c_fifo);
#endif

#endif


