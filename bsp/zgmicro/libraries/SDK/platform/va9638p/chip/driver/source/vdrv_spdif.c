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
 	SPDIF Driver (including DMA configuration)
    Support SPDIF writing now.
    
Revision History:
 	Date                  Author                   Description
    2020.4.2              ChenJianhong
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vdrv_gpio.h" 
#include "vdrv_spdif.h" 


void VSPDIF_SetGPIO()
{
	 VGPIO_SetLMuxFunction(GPIO_L_PIN_9, GPIO_L_PIN_9);
	 VGPIO_SetLMux2Function(GPIO_L_PIN_9, GPIO_L_PIN_9);
}

/*
[8:6]: SPDENC_S_CODE     3'b001
Audio sample rate 
3'b010 32KHz
3'b000 44.1KHz
3'b011 88.2KHz
3'b100 176.4KHz
3'b001 48KHz ( default)
3'b101 96KHz
3'b110 192KHz
*/
void VSPDIF_SetSampleRate(UINT8 SR)
{
	UINT32 reg;
	
	reg = READ_REG(REG_SPDIF_CTRL);
	WRITE_REG(REG_SPDIF_CTRL, ((SR << 6) | 
									(reg & (~(0x7 << 6)))));
}

/*
[0]: SPDENC_EN       1'h0
 1: enable     0:disable 
*/
void VSPDIF_SetTx(BOOL En)
{
	UINT32 reg;
	
	reg = READ_REG(REG_SPDIF_CTRL);
	WRITE_REG(REG_SPDIF_CTRL, ((En << 0) | 
									(reg & (~(1 << 0)))));
}

/*
[18:16]: spdif_samp_rate    3'h1
Sampling rate 
0: 32 KHz
1: 48 KHz
2: 96 KHz
3: 192 KHz
4: 44.1KHz
5: 88.2KHz
6: 176.4KHz
*/
void VSPDIF_CLK_SetSampleRate(UINT8 SR)
{
	UINT32 reg;
	
	reg = READ_REG(REG_SPDIF_CLK_CTRL);
	WRITE_REG(REG_SPDIF_CLK_CTRL, ((SR << 16) | 
									(reg & (~(0x7 << 16)))));
}

/*
[3]: use_bb_dac_en    1'b0
1: Use the bb_dac_en sigal to sync SPDIF Tx.
0: Do not use bb_dac_en. SPDIF Tx starts right after setting SPDENC_EN  1. 
*/

/*
[2:1]: spdif_data_width            2'b01
Data word width.
00: 16-bit.
01: 24-bit.
*/
void VSPDIF_CLK_SetDataWidth(UINT8 DW)
{
	UINT32 reg;
	
	reg = READ_REG(REG_SPDIF_CLK_CTRL);
	WRITE_REG(REG_SPDIF_CLK_CTRL, ((DW << 1) | 
									(reg & (~(0x3 << 1)))));
}

/*
[0]: spdif_clk_sel      1'b0
spdif clock source select:
0: from 384MHz clock source
1: from 768MHz clock source
*/     
void VSPDIF_SetClockSrcSel(UINT8 SelHi)
{
	UINT32 reg;
	
	reg = READ_REG(REG_SPDIF_CLK_CTRL);
	WRITE_REG(REG_SPDIF_CLK_CTRL, ((SelHi << 0) | 
									(reg & (~(0x1 << 0)))));
}


/******************************************************************************
Description:
 	Initial SPDIF Module for VA9638P

Parameters:
    mode    - SPDIF_SLAVE_MODE or SPDIF_MASTER_MODE
 	dwidth  - SPDIF_24BIT_WIDTH or SPDIF_16BIT_WIDTH
    msb     - SPDIF MSB selection 
            TRUE: msb is first
            FALSE: lsb is first
 	sec_active - SPDIF second clock is selected
            TRUE:   enable
            FALSE:  disable
Return Value:
	None.
*****************************************************************************/
void VSPDIF_Init(UINT32 SampleRate, UINT8 dwidth)
{
	UINT8 Set_val = 0;
	UINT32 reg_val = 0;
	// enable DMA regs 
	reg_val = READ_REG(REG_DMA_CHICKEN);
	reg_val &= ~(0x13);
	WRITE_REG(REG_DMA_CHICKEN, reg_val);
	
	VSPDIF_SetGPIO();	
	switch (SampleRate)
	{
		case 32000:
			Set_val = 2;
		break;
		
		case 44100:
			Set_val = 0;
		break;
		
		case 48000:
			Set_val = 1;
		break;
		
		case 96000:
			Set_val = 5;
		break;
		
		default:
			Set_val = 1;
			break;
	}
	VSPDIF_SetSampleRate(Set_val);
	
	switch (SampleRate)
	{
		case 32000:
			Set_val = 0;
		break;
		
		case 44100:
			Set_val = 4;
		break;
		
		case 48000:
			Set_val = 1;
		break;
		
		case 96000:
			Set_val = 2;
		break;
		
		default:
			Set_val = 1;
			break;
	}
	VSPDIF_CLK_SetSampleRate(Set_val);
	
	if (SPDIF_16BIT_WIDTH == dwidth)
	{
		Set_val = 0;
	}
	else if (SPDIF_24BIT_WIDTH == dwidth)
	{

		Set_val = 1;
	}
	else
	{
		Set_val = 2;
	}
	VSPDIF_CLK_SetDataWidth(Set_val);
	VSPDIF_SetTx(TRUE);
}

/******************************************************************************
Description:
 	Disable SPDIF Module for VA9638P

Parameters:
	void
Return Value:
	None.
*****************************************************************************/
void VSPDIF_finit(void)
{
    UINT32 spdif_val = 0;
	spdif_val = 0x0180B0C0;

	WRITE_REG(REG_SPDIF_CTRL, spdif_val);
}


/******************************************************************************
 * 
 *  SPDIF DMA Configuration
 * 
 *****************************************************************************/

/******************************************************************************
Description:
 	Configure parameters of SPDIF write path, including target buffer, memory address, transmission 
 	size, and stop flag.
Parameters:
 	Buf - select the target buffer of 2 ping-pang buffers
 	Addr - memory address for data
 	Len - size of ONE channel to read in byte, whatever both channels or only one enabled
 	Stop - to tell if stop HDS readl transmission after current buffer transport
Return Value:
	void
Remarks:
	Ping-pang-buffers is designed to avoid data missing. Once one buffer transport completed, the
	transport will switch to the other buffer automatically if it has been configured. 
	The write path combine both channel data together, so only one memory address needed.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VSPDIF_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_SPDIF_WR_ADD0_SPDIF_WR_ADD0_MSK);
		if (Stop) {
			reg |= VDMA_REG_SPDIF_WR_ADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_SPDIF_WR_ADD0, reg);
		WRITE_REG(REG_DMA_SPDIF_WR_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_SPDIF_WR_ADD1_SPDIF_WR_ADD1_MSK);
		if (Stop) {
			reg |= VDMA_REG_SPDIF_WR_ADD1_END_BUF1_MSK;
		}
		WRITE_REG(REG_DMA_SPDIF_WR_ADD1, reg);
		WRITE_REG(REG_DMA_SPDIF_WR_LENGTH1, Len);
		break;
    default:
        break;
	}
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for SPDIF.
Parameters:
 	WrEn - one of below definitions:
			VDMA_SPDIF_BOTH_CHANNEL_DISABLE		
			VDMA_SPDIF_LEFT_CHANNEL_ENABLE		
			VDMA_SPDIF_RIGHT_CHANNEL_ENABLE		
			VDMA_SPDIF_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VSPDIF_DMA_WriteEnable(UINT8 WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
	WRITE_REG(REG_DMA_SPDIF_WR_CTRL, ((WrEn << VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_SHFT) | 
									(reg & (~VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_MSK))));

}

//WrSrc = 0 src is cm3, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VSPDIF_DMA_WriteConfig(BOOL WrSrc, UINT8 mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
	reg = WrSrc ? (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_SRC_MSK) : (reg & (~VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_SRC_MSK));
	reg = mode ? (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_MSK) : (reg & (~VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_MSK));
	WRITE_REG(REG_DMA_SPDIF_WR_CTRL,reg);
}

VDMA_BUF VSPDIF_DMA_GetWriteActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_STAT);
	if (reg & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}	
}

void VSPDIF_DMA_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
		WRITE_REG(REG_DMA_SPDIF_WR_CTRL, (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
		reg = 0x00;
		WRITE_REG(REG_DMA_SPDIF_WR_CTRL, (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_SPDIF_WR_STAT);
        //HDS DMA fifox , indicate End_buf
		if (reg & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE0_MSK) {
			reg = READ_REG(REG_DMA_SPDIF_WR_ADD0);
			reg |= VDMA_REG_SPDIF_WR_ADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_SPDIF_WR_ADD0, reg);
		} else if (reg & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE1_MSK) {
			reg = READ_REG(REG_DMA_SPDIF_WR_ADD1);
			reg |= VDMA_REG_SPDIF_WR_ADD1_END_BUF1_MSK;
			WRITE_REG(REG_DMA_SPDIF_WR_ADD1, reg);
		}
		break;
	}
}

/******************************************************************************
Description:
 	Get the actually written count in byte. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	written count in byte
Remarks:
	The count will be reset after path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VSPDIF_DMA_GetWritenCount(void)
{
	return READ_REG(REG_DMA_SPDIF_WR_CNT);
}

/******************************************************************************
Description:
 	To tell if write path stopped/aborted.
Parameters:
 	void
Return Value:
	TRUE if stopped/aborted, otherwise FALSE
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
BOOL VSPDIF_DMA_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPDIF_WR_STAT) & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_STOP_MSK) >> VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	To tell which buffer in ping-pang buffers introduce the interrupt. Using this function, you can
 	know which buffer shall be configured in next step and which memory is done.
Parameters:
 	void
Return Value:
	The buffer tag.
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
VDMA_BUF VSPDIF_DMA_GetWriteIntSource(void)
{
	return (VDMA_BUF)((READ_REG(REG_DMA_SPDIF_WR_STAT) & VDMA_REG_SPDIF_WR_STAT_SPDIF_WRINTR_SRC_MSK) >> 
					VDMA_REG_SPDIF_WR_STAT_SPDIF_WRINTR_SRC_SHFT);
}

/******************************************************************************
Description:
 	Get the configured length of figured buffer.
Parameters:
 	void
Return Value:
	buffer length in byte
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VSPDIF_DMA_GetWriteBufferLength(VDMA_BUF Buf)
{
	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_SPDIF_WR_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_SPDIF_WR_LENGTH1);
	default:
		return 0;
	}	
}

/******************************************************************************
Description:
 	Get the configured memory addresses of  write buffers
Parameters:
 	p_Buf0[out] - returned memory address of buffer 0
 	p_Buf1[out] - returned memory address of buffer 1
Return Value:
	void
Remarks:
	The configured paramters will be reset after path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VSPDIF_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_SPDIF_WR_ADD0) & VDMA_REG_SPDIF_WR_ADD0_SPDIF_WR_ADD0_MSK) >> VDMA_REG_SPDIF_WR_ADD0_SPDIF_WR_ADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_SPDIF_WR_ADD1) & VDMA_REG_SPDIF_WR_ADD1_SPDIF_WR_ADD1_MSK) >> VDMA_REG_SPDIF_WR_ADD1_SPDIF_WR_ADD1_SHFT);
}
