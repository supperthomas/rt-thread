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
    I2S

Description:
     I2S Driver
Revision History:
     Date                  Author                   Description
    2020.6.2			ChenJianhong			modify for 9638P
    2022.0620           cao.xuetao              
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vdrv_gpio.h"
#include "vdrv_dma.h"
#include "vdrv_iis.h" 

#if (0)
#define CHECK_ADDR_IS_MEMPOOL_VALID(addr,len)
#else
#define CHECK_ADDR_IS_MEMPOOL_VALID(addr,len)																			
#endif


/******************************************************************************

 						9638P I2S Control Register Bits

*******************************************************************************/
#define RBIT_I2S_CTRL_CLK_SRC_SEL           3
#define RBIT_I2S_CTRL_MASTER			2
#define RBIT_I2S_CTRL_RX_EN				1
#define RBIT_I2S_CTRL_TX_EN				0


/******************************************************************************

 						I2S DMA Register Bits

*******************************************************************************/
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_CTLSRC_SHFT         7
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_CTLSRC_MSK          0x00000080
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_SHFT            6
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_MSK             0x00000040
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_DST_SHFT            5
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_DST_MSK             0x00000020
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_SHFT         4
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_MSK          0x00000010
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_SHFT             2
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_MSK              0x0000000C
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_ABORT_SHFT          1
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_ABORT_MSK           0x00000002
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_START_SHFT          0
#define VDMA_REG_I2S_WR_CTRL_I2S_WR_START_MSK           0x00000001

#define VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_SHFT           4
#define VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_MSK            0x00000010
#define VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_SHFT        2
#define VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_MSK         0x0000000C
#define VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_SHFT            1
#define VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_MSK             0x00000002
#define VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_SHFT            0
#define VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_MSK             0x00000001

#define VDMA_REG_I2S_WR_ADD0_END_BUF0_SHFT              31
#define VDMA_REG_I2S_WR_ADD0_END_BUF0_MSK               0x80000000
#define VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT           0
#define VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK            0x7FFFFFFF

#define VDMA_REG_I2S_WR_ADD1_END_BUF1_SHFT              31
#define VDMA_REG_I2S_WR_ADD1_END_BUF1_MSK               0x80000000
#define VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT           0
#define VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK            0x7FFFFFFF



#define VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_SHFT             2
#define VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_MSK              0x0000000C
#define VDMA_REG_I2S_RD_CTRL_I2S_RD_SRC_MSK             0x00000020
#define VDMA_REG_I2S_RD_CTRL_I2S_RD_BITMOD_MSK          0x00000010
#define VDMA_REG_I2S_RD_CTRL_I2S_RD_START_MSK           0x00000001
#define VDMA_REG_I2S_RD_CTRL_I2S_RD_ABORT_MSK           0x00000002


#define VDMA_REG_I2S_RD_STAT_I2S_RD_STOP_SHFT           4
#define VDMA_REG_I2S_RD_STAT_I2S_RD_STOP_MSK            0x00000010
#define VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_MSK         0x0000000C
#define VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_SHFT        2
#define VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_SHFT            0
#define VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_MSK             0x00000001
#define VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_SHFT            1
#define VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_MSK             0x00000002


#define VDMA_REG_I2S_RD_RADD0_END_BUF0_MSK               0x80000000
#define VDMA_REG_I2S_RD_RADD1_END_BUF0_MSK               0x80000000
#define VDMA_REG_I2S_RD_LADD0_END_BUF0_MSK               0x80000000
#define VDMA_REG_I2S_RD_LADD1_END_BUF0_MSK               0x80000000

#define VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_SHFT           0
#define VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK            0x7FFFFFFF
#define VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_SHFT           0
#define VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK            0x7FFFFFFF
#define VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_SHFT           0
#define VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK            0x7FFFFFFF
#define VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_SHFT           0
#define VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK            0x7FFFFFFF

UINT32 _VI2S_get_ctrl_reg_addr(UINT8 channel)
{
	if (channel == I2S_CH0)
	{
		return REG_I2S_I2S_CTRL_0;
	}
	else
	{
		return REG_I2S_I2S_CTRL_1;
	}	
}

void VI2S_SetGPIO(UINT8 channel)
{
    if (channel == I2S_CH0)
    {
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_I2S0, GPIO_MUX_FUNC_I2S0);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_I2S0, GPIO_MUX_FUNC_NONE);
    }
    else
    {
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_I2S1, GPIO_MUX_FUNC_I2S1);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_I2S1, GPIO_MUX_FUNC_NONE);
    }
}

void VI2S_SetRx(UINT8 channel, BOOL En)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((En << RBIT_I2S_CTRL_RX_EN) | 
									(reg & (~(1 << RBIT_I2S_CTRL_RX_EN)))));
}

void VI2S_SetTx(UINT8 channel, BOOL En)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((En << RBIT_I2S_CTRL_TX_EN) | 
									(reg & (~(1 << RBIT_I2S_CTRL_TX_EN)))));
}

void VI2S_Both_SetTx(BOOL En)
{
	UINT32 reg0 = 0;
	UINT32 reg1 = 0;
	
	reg0 =  READ_REG(REG_I2S_I2S_CTRL_0);
	reg1 =  READ_REG(REG_I2S_I2S_CTRL_1);
	if(En){
		reg0 |= 1;
		reg1 |= 1;
	}else{
		reg0 &= ~1;
		reg1 &= ~1;
	}
	WRITE_REG(REG_I2S_I2S_CTRL_0, reg0);
	WRITE_REG(REG_I2S_I2S_CTRL_1, reg1);
	
}

void VI2S_Sync_SetTRx(UINT32 bitval)
{
	UINT32 reg0 = 0;
	UINT32 reg1 = 0;
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	reg0 =  READ_REG(REG_I2S_I2S_CTRL_0);
	reg1 =  READ_REG(REG_I2S_I2S_CTRL_1);
	reg0 |= (bitval&0x03);
	reg1 |= ((bitval>>2)&0x03);
	WRITE_REG(REG_I2S_I2S_CTRL_0, reg0);
	WRITE_REG(REG_I2S_I2S_CTRL_1, reg1);
	VCPU_CRITICAL_EXIT();
}

BOOL VI2S_GetTx(UINT8 channel)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	return ((1 << RBIT_I2S_CTRL_TX_EN) & reg);
}
BOOL VI2S_GetRx(UINT8 channel)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	return ((1 << RBIT_I2S_CTRL_RX_EN) & reg);
}
// i2s mode select    0:slave mode /1:master mode
void VI2S_SetMasterMode(UINT8 channel, BOOL En)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((En << RBIT_I2S_CTRL_MASTER) | 
									(reg & (~(1 << RBIT_I2S_CTRL_MASTER)))));
}

// [3]: i2s0_clk_src_sel    1'h0
// Select clock source for I2S channel #0:
//1: source from 768MHz.
//0: source form 384MHz.
void VI2S_SetClockSrcSel(UINT8 channel, BOOL SelHi)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((SelHi << RBIT_I2S_CTRL_CLK_SRC_SEL) | 
									(reg & (~(1 << RBIT_I2S_CTRL_CLK_SRC_SEL)))));
}

/*
[15:12]: SAMP_RATE    4'h4
Sampling rate of I2S.
0: 8 KHz
1: 12 KHz 
2: 16 KHz
3: 24 KHz
4: 48 KHz 
5: 96 KHz 
6: 192 KHz
7: 384 KHz
8: 11.025 KHz
9: 22.05 KHz
10: 44.1 KHz
11: 88.2 KHz
*/
void VI2S_SetSampleRate(UINT8 channel, UINT8 SR)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((SR << 12) | 
									(reg & (~(0xf << 12)))));
}

/*
[10]: WS_INV    1'h0
0: WS '0' indicates left channel, '1' indicates right channel.
1: WS '1' indicates left channel, '0' indicates right channel.
*/
void VI2S_SetWS_INV(UINT8 channel, UINT8 En)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((En << 10) | 
									(reg & (~(0x1 << 10)))));
}

/*
[9:8]: DWIDTH    2'h1
Data word width.
00: 16-bit.
01: 24-bit.
10: 32 bit
*/
void VI2S_SetDataWidth(UINT8 channel, UINT8 DW)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((DW << 8) | 
									(reg & (~(0x3 << 8)))));
}

/*
[7:6]: WS_SEL    2'h2
WS frequency selection for I2S #0.
00: SCLK=32*Fs.
01: SCLK=48*Fs. Note: NOT support this when I2S #0 as a master.
10: SCLK=64*Fs.
*/
void VI2S_SetWS_SEL(UINT8 channel, UINT8 WS_SEL)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((WS_SEL << 6) | 
									(reg & (~(0x3 << 6)))));
}

/*
[5:4]: FORMAT    2'h0
0: I2S format, serial data is active from the second sclk.
1: reserved.
2: Left-Justified.
3: Right-Justified.
*/
void VI2S_SetFormat(UINT8 channel, UINT8 format)
{
	UINT32 reg, addr;
	
	addr = _VI2S_get_ctrl_reg_addr(channel);
	
	reg = READ_REG(addr);
	WRITE_REG(addr, ((format << 4) | 
									(reg & (~(0x3 << 4)))));
}

/******************************************************************************
Description:
 	Initial I2S Module for VA9638P

Parameters:
    channel - I2S_CH0 or I2S_CH1
    mode    - I2S_SLAVE_MODE or I2S_MASTER_MODE
    dwidth  - I2S_24BIT_WIDTH or I2S_16BIT_WIDTH
    SR     - I2S Sample Rate selection 
    hi_clk - I2S Hi clock 768MHz is selected
            TRUE:   enable
            FALSE:  disable
    format - 
            0: I2S format, serial data is active from the second sclk.
            1: reserved.
            2: Left-Justified.
            3: Right-Justified.
Return Value:
	None.
*****************************************************************************/
void VI2S_Init(UINT8 channel, UINT8 mode, UINT8 dwidth, UINT32 SR, BOOL hi_clk, UINT8 format)
{
	UINT8 Set_val = 0;
	UINT32 reg_val = 0;
	// enable i2s0 DMA regs 
	reg_val = READ_REG(REG_DMA_CHICKEN);
	reg_val &= ~(0x13);
	WRITE_REG(REG_DMA_CHICKEN, reg_val);

	VI2S_SetGPIO(channel);
	
	if (I2S_MASTER_MODE == mode)
	{
		Set_val = 1;
	}
	else
	{

		Set_val = 0;
	}
	VI2S_SetMasterMode(channel, Set_val);

	if (I2S_16BIT_WIDTH == dwidth)
	{
		Set_val = 0;
	}
	else if (I2S_24BIT_WIDTH == dwidth)
	{

		Set_val = 1;
	}
	else
	{
		Set_val = 2;
	}
	VI2S_SetDataWidth(channel, Set_val);

	switch (SR)
	{
		case 8000:
			Set_val = 0;
		break;
		
		case 12000:
			Set_val = 1;
		break;
		
		case 16000:
			Set_val = 2;
		break;
		
		case 24000:
			Set_val = 3;
		break;
		
		case 48000:
			Set_val = 4;
		break;
		
		case 96000:
			Set_val = 5;
		break;

		case 192000:
			Set_val = 6;
		break;
		
		case 384000:
			Set_val = 7;
		break;
		
		case 11025:
			Set_val = 8;
		break;
		
		case 22050:
			Set_val = 9;
		break;
		
		case 44100:
			Set_val = 10;
		break;
		
		case 88200:
			Set_val = 11;
		break;
		
		default:
			Set_val = 4;
			break;
	}
	VI2S_SetSampleRate(channel, Set_val);
	
	VI2S_SetFormat(channel, format);
	VI2S_SetClockSrcSel(channel, hi_clk);
}

/******************************************************************************
Description:
 	Disable I2S Module for VA9638P

Parameters:
	void
Return Value:
	None.
*****************************************************************************/
void VI2S_finit(UINT8 channel)
{
	UINT32 i2s_val = 0;
	i2s_val = 0x4184;
	if (channel == I2S_CH0)
	{
		WRITE_REG(REG_I2S_I2S_CTRL_0, i2s_val);
	}
	else
	{
		WRITE_REG(REG_I2S_I2S_CTRL_1, i2s_val);
	}
}


/******************************************************************************
 * 
 *  I2S0 DMA Configuration
 * 
 *****************************************************************************/

/******************************************************************************
Description:
 	Configure parameters of I2S write path, including target buffer, memory address, transmission 
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
void VI2S0_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;
	
		CHECK_ADDR_IS_MEMPOOL_VALID(Addr,Len);

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_WR_ADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S0_WR_ADD0, reg);
		WRITE_REG(REG_DMA_I2S0_WR_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_WR_ADD1_END_BUF1_MSK;
		}
		WRITE_REG(REG_DMA_I2S0_WR_ADD1, reg);
		WRITE_REG(REG_DMA_I2S0_WR_LENGTH1, Len);
		break;
    default:
        break;
	}
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for I2S.
Parameters:
 	WrEn - one of below definitions:
			VDMA_I2S_BOTH_CHANNEL_DISABLE		
			VDMA_I2S_LEFT_CHANNEL_ENABLE		
			VDMA_I2S_RIGHT_CHANNEL_ENABLE		
			VDMA_I2S_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VI2S0_DMA_WriteEnable(UINT8 WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S0_WR_CTRL);
	WRITE_REG(REG_DMA_I2S0_WR_CTRL, ((WrEn << VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_SHFT) | 
									(reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_MSK))));

}

//WrSrc = 0 src is cm3, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VI2S0_DMA_WriteConfig(BOOL WrSrc, UINT8 mode)
{
	UINT32 reg = 0;
	//When configuring parameters, ensure that the register value is the default value
	//reg = READ_REG(REG_DMA_I2S0_WR_CTRL);
	reg = WrSrc ? (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_MSK) : (reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_MSK));
	reg = mode ? (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_MSK) : (reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_MSK));
	WRITE_REG(REG_DMA_I2S0_WR_CTRL,reg);
}

VDMA_BUF VI2S0_DMA_GetWriteActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S0_WR_STAT);
	if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}
}

void VI2S0_DMA_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_I2S0_WR_CTRL);
		WRITE_REG(REG_DMA_I2S0_WR_CTRL, (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_I2S0_WR_CTRL);
		WRITE_REG(REG_DMA_I2S0_WR_CTRL, (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_I2S0_WR_STAT);
        //HDS DMA fifox , indicate End_buf
		if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_MSK) {
			reg = READ_REG(REG_DMA_I2S0_WR_ADD0);
			reg |= VDMA_REG_I2S_WR_ADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S0_WR_ADD0, reg);
		} else if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_MSK) {
			reg = READ_REG(REG_DMA_I2S0_WR_ADD1);
			reg |= VDMA_REG_I2S_WR_ADD1_END_BUF1_MSK;
			WRITE_REG(REG_DMA_I2S0_WR_ADD1, reg);
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
UINT32 VI2S0_DMA_GetWritenCount(void)
{
	return READ_REG(REG_DMA_I2S0_WR_CNT);
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
BOOL VI2S0_DMA_IsWriteStopped(void)
{
	return (BOOL)(READ_REG(REG_DMA_I2S0_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_MSK);
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
VDMA_BUF VI2S0_DMA_GetWriteIntSource(void)
{
	return (VDMA_BUF)((READ_REG(REG_DMA_I2S0_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_MSK) >> 
					VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_SHFT);
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
UINT32 VI2S0_DMA_GetWriteBufferLength(VDMA_BUF Buf)
{
	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_I2S0_WR_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_I2S0_WR_LENGTH1);
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
void VI2S0_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);
}

/******************************************************************************
Description:
 	Configure parameters of I2S right read path, including target buffer, memory address, transmission 
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
void VI2S0_DMA_ConfigReadRtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		//if (Stop) {
			//reg |= VDMA_REG_I2S_RD_RADD0_END_BUF0_MSK;
		//}
		WRITE_REG(REG_DMA_I2S0_RD_RADD0, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK);
		WRITE_REG(REG_DMA_I2S0_RD_RADD1, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH1, Len);
		break;
    default:
        break;
	}
}

/******************************************************************************
Description:
 	Configure parameters of I2S left read path, including target buffer, memory address, transmission 
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
void VI2S0_DMA_ConfigReadLtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		//if (Stop) {
			//reg |= VDMA_REG_I2S_RD_RADD0_END_BUF0_MSK;
		//}
		WRITE_REG(REG_DMA_I2S0_RD_LADD0, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH0, Len);	
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
		WRITE_REG(REG_DMA_I2S0_RD_LADD1, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH1, Len);
		break;
    default:
        break;
	}
}

void VI2S0_DMA_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop)
{
	UINT32 reg;
	
	CHECK_ADDR_IS_MEMPOOL_VALID(RightAddr,Len);
	CHECK_ADDR_IS_MEMPOOL_VALID(LeftAddr,Len);
	
	switch (Buf) {
	case VDMA_BUF_0:
		reg = (RightAddr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		WRITE_REG(REG_DMA_I2S0_RD_RADD0, reg);
		
		reg = (LeftAddr & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_RD_LADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S0_RD_LADD0, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (RightAddr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		WRITE_REG(REG_DMA_I2S0_RD_RADD1, reg);
		reg = (LeftAddr & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_RD_LADD1_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S0_RD_LADD1, reg);
		WRITE_REG(REG_DMA_I2S0_RD_LENGTH1, Len);
		break;
    default:
        break;
	}	
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for I2S.
Parameters:
 	RdEn - one of below definitions:
			VDMA_I2S_BOTH_CHANNEL_DISABLE		
			VDMA_I2S_LEFT_CHANNEL_ENABLE		
			VDMA_I2S_RIGHT_CHANNEL_ENABLE		
			VDMA_I2S_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VI2S0_DMA_ReadEnable(UINT8 RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S0_RD_CTRL);
	WRITE_REG(REG_DMA_I2S0_RD_CTRL, ((RdEn << VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_SHFT) | 
									(reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_MSK))));

}

//RdSrc = 0 src is cm3 sram, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VI2S0_DMA_ReadConfig(BOOL RdSrc, UINT8 mode)
{
	UINT32 reg = 0;
	//When configuring parameters, ensure that the register value is the default value
	//reg = READ_REG(REG_DMA_I2S0_RD_CTRL);
	reg = RdSrc ? (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_SRC_MSK) : (reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_SRC_MSK));
	reg = mode ? (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_BITMOD_MSK) : (reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_BITMOD_MSK));
	WRITE_REG(REG_DMA_I2S0_RD_CTRL,reg);
}

VDMA_BUF VI2S0_DMA_GetReadActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S0_RD_STAT);
	if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}	
}

void VI2S0_DMA_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {//ONLY DO right channel
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_I2S0_RD_CTRL);
		WRITE_REG(REG_DMA_I2S0_RD_CTRL, (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		//reg = READ_REG(REG_DMA_HDS_WR_CTRL);//REG_DMA_HDS_WR_CTRL is wrong
		reg = READ_REG(REG_DMA_I2S0_RD_CTRL);
		reg &= ~(VDMA_REG_I2S_RD_CTRL_I2S_RD_START_MSK);
		WRITE_REG(REG_DMA_I2S0_RD_CTRL, (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_I2S0_RD_STAT);
        //HDS DMA fifox , indicate End_buf
		if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_MSK) {
			reg = READ_REG(REG_DMA_I2S0_RD_LADD0);
			reg |= VDMA_REG_I2S_RD_LADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S0_RD_LADD0, reg);//????			
		} else if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_MSK) {
			reg = READ_REG(REG_DMA_I2S0_RD_LADD1);
			reg |= VDMA_REG_I2S_RD_LADD1_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S0_RD_LADD1, reg);		
		}
		break;
	}		
}

/******************************************************************************
Description:
 	Get the actually read count in byte. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	read count in byte
Remarks:
	The count will be reset after path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VI2S0_DMA_GetReadCount(void)
{
	return READ_REG(REG_DMA_I2S0_RD_CNT);
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
BOOL VI2S0_DMA_IsReadStopped(void)
{
	//return (BOOL)(READ_REG(REG_DMA_I2S0_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_MSK);
	return (BOOL)(READ_REG(REG_DMA_I2S0_RD_STAT) & VDMA_REG_I2S_RD_STAT_I2S_RD_STOP_MSK);
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
VDMA_BUF VI2S0_DMA_GetReadIntSource(void)
{
	//return (VDMA_BUF)((READ_REG(REG_DMA_I2S0_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_MSK) >> 
					//VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_SHFT);
	return (VDMA_BUF)((READ_REG(REG_DMA_I2S0_RD_STAT) & VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_MSK) >> 
					VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_SHFT);

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
UINT32 VI2S0_DMA_GetReadBufferLength(VDMA_BUF Buf)
{
	//switch (Buf) {
	//case VDMA_BUF_0:
		//return READ_REG(REG_DMA_I2S0_WR_LENGTH0);
	//case VDMA_BUF_1:
		//return READ_REG(REG_DMA_I2S0_WR_LENGTH1);
	//default:
		//return 0;

	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_I2S0_RD_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_I2S0_RD_LENGTH1);
	default:
		return 0;

	}
}

/******************************************************************************
Description:
 	Get the configured memory addresses of right read buffers
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
void VI2S0_DMA_GetRtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	//*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	//*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);

	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S0_RD_RADD0) & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK) >> VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S0_RD_RADD1) & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK) >> VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_SHFT);
}

/******************************************************************************
Description:
 	Get the configured memory addresses of left read buffers
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

void VI2S0_DMA_GetLtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	//*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	//*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S0_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);

	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S0_RD_LADD0) & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK) >> VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S0_RD_LADD1) & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK) >> VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_SHFT);
}

void VI2S0_DMA_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1)
{
	UINT8 en;

	en = (READ_REG(REG_DMA_I2S0_RD_CTRL) & VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_MSK) >> VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_SHFT;
	
	if (en & VDMA_I2S_RIGHT_CHANNEL_ENABLE) {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = (UINT32)(READ_REG(REG_DMA_I2S0_RD_RADD0) & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		if (p_RBuf1 != NULL) {
			*p_RBuf1 = (UINT32)(READ_REG(REG_DMA_I2S0_RD_RADD1) & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK);
		}
	} else {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = 0;
		if (p_RBuf1 != NULL)
			*p_RBuf1 = 0;
	}
	
	if (en & VDMA_I2S_LEFT_CHANNEL_ENABLE) {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = (UINT32)(READ_REG(REG_DMA_I2S0_RD_LADD0) & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		if (p_LBuf1 != NULL)
			*p_LBuf1 = (UINT32)(READ_REG(REG_DMA_I2S0_RD_LADD1) & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
	} else {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = 0;
		if (p_LBuf1 != NULL)
			*p_LBuf1 = 0;
	}	
}


/******************************************************************************
 * 
 *  I2S1 DMA Configuration
 * 
 *****************************************************************************/

/******************************************************************************
Description:
 	Configure parameters of I2S write path, including target buffer, memory address, transmission 
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
void VI2S1_DMA_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;
	
		CHECK_ADDR_IS_MEMPOOL_VALID(Addr,Len);

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_WR_ADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S1_WR_ADD0, reg);
		WRITE_REG(REG_DMA_I2S1_WR_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_WR_ADD1_END_BUF1_MSK;
		}
		WRITE_REG(REG_DMA_I2S1_WR_ADD1, reg);
		WRITE_REG(REG_DMA_I2S1_WR_LENGTH1, Len);
		break;
    default:
        break;
	}
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for I2S.
Parameters:
 	WrEn - one of below definitions:
			VDMA_I2S_BOTH_CHANNEL_DISABLE		
			VDMA_I2S_LEFT_CHANNEL_ENABLE		
			VDMA_I2S_RIGHT_CHANNEL_ENABLE		
			VDMA_I2S_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VI2S1_DMA_WriteEnable(UINT8 WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S1_WR_CTRL);
	WRITE_REG(REG_DMA_I2S1_WR_CTRL, ((WrEn << VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_SHFT) | 
									(reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_EN_MSK))));

}

//WrSrc = 0 src is cm3, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VI2S1_DMA_WriteConfig(BOOL WrSrc, UINT8 mode)
{
	UINT32 reg = 0;
	//When configuring parameters, ensure that the register value is the default value
	//reg = READ_REG(REG_DMA_I2S1_WR_CTRL);
	reg = WrSrc ? (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_MSK) : (reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_SRC_MSK));
	reg = mode ? (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_MSK) : (reg & (~VDMA_REG_I2S_WR_CTRL_I2S_WR_BITMOD_MSK));
	WRITE_REG(REG_DMA_I2S1_WR_CTRL,reg);
}

VDMA_BUF VI2S1_DMA_GetWriteActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S1_WR_STAT);
	if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}
}

void VI2S1_DMA_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_I2S1_WR_CTRL);
		WRITE_REG(REG_DMA_I2S1_WR_CTRL, (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_I2S1_WR_CTRL);
		WRITE_REG(REG_DMA_I2S1_WR_CTRL, (reg | VDMA_REG_I2S_WR_CTRL_I2S_WR_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_I2S1_WR_STAT);
        //HDS DMA fifox , indicate End_buf
		if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE0_MSK) {
			reg = READ_REG(REG_DMA_I2S1_WR_ADD0);
			reg |= VDMA_REG_I2S_WR_ADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S1_WR_ADD0, reg);
		} else if (reg & VDMA_REG_I2S_WR_STAT_I2S_WR_FE1_MSK) {
			reg = READ_REG(REG_DMA_I2S1_WR_ADD1);
			reg |= VDMA_REG_I2S_WR_ADD1_END_BUF1_MSK;
			WRITE_REG(REG_DMA_I2S1_WR_ADD1, reg);
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
UINT32 VI2S1_DMA_GetWritenCount(void)
{
	return READ_REG(REG_DMA_I2S1_WR_CNT);
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
BOOL VI2S1_DMA_IsWriteStopped(void)
{
	return (BOOL)(READ_REG(REG_DMA_I2S1_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_MSK);
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
VDMA_BUF VI2S1_DMA_GetWriteIntSource(void)
{
	return (VDMA_BUF)((READ_REG(REG_DMA_I2S1_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_MSK) >> 
					VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_SHFT);
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
UINT32 VI2S1_DMA_GetWriteBufferLength(VDMA_BUF Buf)
{
	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_I2S1_WR_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_I2S1_WR_LENGTH1);
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
void VI2S1_DMA_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);

}

/******************************************************************************
Description:
 	Configure parameters of I2S right read path, including target buffer, memory address, transmission 
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
void VI2S1_DMA_ConfigReadRtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		//if (Stop) {
			//reg |= VDMA_REG_I2S_RD_RADD0_END_BUF0_MSK;
		//}
		WRITE_REG(REG_DMA_I2S1_RD_RADD0, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK);
		WRITE_REG(REG_DMA_I2S1_RD_RADD1, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH1, Len);
		break;
    default:
        break;
	}
}

/******************************************************************************
Description:
 	Configure parameters of I2S left read path, including target buffer, memory address, transmission 
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
void VI2S1_DMA_ConfigReadLtPath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
    UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		//if (Stop) {
			//reg |= VDMA_REG_I2S_RD_RADD0_END_BUF0_MSK;
		//}
		WRITE_REG(REG_DMA_I2S1_RD_LADD0, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH0, Len);	
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
		WRITE_REG(REG_DMA_I2S1_RD_LADD1, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH1, Len);
		break;
    default:
        break;
	}
}

void VI2S1_DMA_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop)
{
	UINT32 reg;
	
		CHECK_ADDR_IS_MEMPOOL_VALID(RightAddr,Len);
	CHECK_ADDR_IS_MEMPOOL_VALID(LeftAddr,Len);
	
	switch (Buf) {
	case VDMA_BUF_0:
		reg = (RightAddr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		WRITE_REG(REG_DMA_I2S1_RD_RADD0, reg);
		
		reg = (LeftAddr & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_RD_LADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S1_RD_LADD0, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (RightAddr & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		WRITE_REG(REG_DMA_I2S1_RD_RADD1, reg);
		reg = (LeftAddr & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
		if (Stop) {
			reg |= VDMA_REG_I2S_RD_LADD1_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_I2S1_RD_LADD1, reg);
		WRITE_REG(REG_DMA_I2S1_RD_LENGTH1, Len);
		break;
    default:
        break;
	}	
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for I2S.
Parameters:
 	RdEn - one of below definitions:
			VDMA_I2S_BOTH_CHANNEL_DISABLE		
			VDMA_I2S_LEFT_CHANNEL_ENABLE		
			VDMA_I2S_RIGHT_CHANNEL_ENABLE		
			VDMA_I2S_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VI2S1_DMA_ReadEnable(UINT8 RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S1_RD_CTRL);
	WRITE_REG(REG_DMA_I2S1_RD_CTRL, ((RdEn << VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_SHFT) | 
									(reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_MSK))));
	
}

//RdSrc = 0 src is cm3 sram, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VI2S1_DMA_ReadConfig(BOOL RdSrc, UINT8 mode)
{
	UINT32 reg = 0;
	//When configuring parameters, ensure that the register value is the default value
	//reg = READ_REG(REG_DMA_I2S1_RD_CTRL);
	reg = RdSrc ? (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_SRC_MSK) : (reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_SRC_MSK));
	reg = mode ? (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_BITMOD_MSK) : (reg & (~VDMA_REG_I2S_RD_CTRL_I2S_RD_BITMOD_MSK));
	WRITE_REG(REG_DMA_I2S1_RD_CTRL,reg);
}

VDMA_BUF VI2S1_DMA_GetReadActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_I2S1_RD_STAT);
	if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}		
}

void VI2S1_DMA_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {//ONLY DO right channel
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_I2S1_RD_CTRL);
		WRITE_REG(REG_DMA_I2S1_RD_CTRL, (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		//reg = READ_REG(REG_DMA_HDS_WR_CTRL);//REG_DMA_HDS_WR_CTRL is wrong
		reg = READ_REG(REG_DMA_I2S1_RD_CTRL);
		reg &= ~(VDMA_REG_I2S_RD_CTRL_I2S_RD_START_MSK);
		WRITE_REG(REG_DMA_I2S1_RD_CTRL, (reg | VDMA_REG_I2S_RD_CTRL_I2S_RD_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_I2S1_RD_STAT);
        //HDS DMA fifox , indicate End_buf
		if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE0_MSK) {
			reg = READ_REG(REG_DMA_I2S1_RD_LADD0);
			reg |= VDMA_REG_I2S_RD_LADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S1_RD_LADD0, reg);//????			
		} else if (reg & VDMA_REG_I2S_RD_STAT_I2S_RD_FE1_MSK) {
			reg = READ_REG(REG_DMA_I2S1_RD_LADD1);
			reg |= VDMA_REG_I2S_RD_LADD1_END_BUF0_MSK;
			WRITE_REG(REG_DMA_I2S1_RD_LADD1, reg);		
		}
		break;
	}		
}

/******************************************************************************
Description:
 	Get the actually read count in byte. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	read count in byte
Remarks:
	The count will be reset after path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VI2S1_DMA_GetReadCount(void)
{
	return READ_REG(REG_DMA_I2S1_RD_CNT);
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
BOOL VI2S1_DMA_IsReadStopped(void)
{
	//return (BOOL)(READ_REG(REG_DMA_I2S1_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WR_STOP_MSK);
	return (BOOL)(READ_REG(REG_DMA_I2S1_RD_STAT) & VDMA_REG_I2S_RD_STAT_I2S_RD_STOP_MSK);
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
VDMA_BUF VI2S1_DMA_GetReadIntSource(void)
{
	//return (VDMA_BUF)((READ_REG(REG_DMA_I2S1_WR_STAT) & VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_MSK) >> 
					//VDMA_REG_I2S_WR_STAT_I2S_WRINTR_SRC_SHFT);
	return (VDMA_BUF)((READ_REG(REG_DMA_I2S1_RD_STAT) & VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_MSK) >> 
					VDMA_REG_I2S_RD_STAT_I2S_RDINTR_SRC_SHFT);
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
UINT32 VI2S1_DMA_GetReadBufferLength(VDMA_BUF Buf)
{
	//switch (Buf) {
	//case VDMA_BUF_0:
		//return READ_REG(REG_DMA_I2S1_WR_LENGTH0);
	//case VDMA_BUF_1:
		//return READ_REG(REG_DMA_I2S1_WR_LENGTH1);
	//default:
		//return 0;

	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_I2S1_RD_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_I2S1_RD_LENGTH1);
	default:
		return 0;

	}	
}

/******************************************************************************
Description:
 	Get the configured memory addresses of right read buffers
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
void VI2S1_DMA_GetRtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	//*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	//*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);

	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S1_RD_RADD0) & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK) >> VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S1_RD_RADD1) & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK) >> VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_SHFT);
}

/******************************************************************************
Description:
 	Get the configured memory addresses of left read buffers
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

void VI2S1_DMA_GetLtReadBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	//*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD0) & VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_MSK) >> VDMA_REG_I2S_WR_ADD0_I2S_WR_ADD0_SHFT);
	//*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S1_WR_ADD1) & VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_MSK) >> VDMA_REG_I2S_WR_ADD1_I2S_WR_ADD1_SHFT);

	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_I2S1_RD_LADD0) & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK) >> VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_I2S1_RD_LADD1) & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK) >> VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_SHFT);
}

void VI2S1_DMA_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1)
{
	UINT8 en;

	en = (READ_REG(REG_DMA_I2S1_RD_CTRL) & VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_MSK) >> VDMA_REG_I2S_RD_CTRL_I2S_RD_EN_SHFT;
	
	if (en & VDMA_I2S_RIGHT_CHANNEL_ENABLE) {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = (UINT32)(READ_REG(REG_DMA_I2S1_RD_RADD0) & VDMA_REG_I2S_RD_RADD0_I2S_RD_RADD0_MSK);
		if (p_RBuf1 != NULL) {
			*p_RBuf1 = (UINT32)(READ_REG(REG_DMA_I2S1_RD_RADD1) & VDMA_REG_I2S_RD_RADD1_I2S_RD_RADD1_MSK);
		}
	} else {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = 0;
		if (p_RBuf1 != NULL)
			*p_RBuf1 = 0;
	}
	
	if (en & VDMA_I2S_LEFT_CHANNEL_ENABLE) {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = (UINT32)(READ_REG(REG_DMA_I2S1_RD_LADD0) & VDMA_REG_I2S_RD_LADD0_I2S_RD_LADD0_MSK);
		if (p_LBuf1 != NULL)
			*p_LBuf1 = (UINT32)(READ_REG(REG_DMA_I2S1_RD_LADD1) & VDMA_REG_I2S_RD_LADD1_I2S_RD_LADD1_MSK);
	} else {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = 0;
		if (p_LBuf1 != NULL)
			*p_LBuf1 = 0;
	}	
}

void VI2S0_Set_BB_AD_Enable(BOOL enable)
{
#if VDEV_CHIP_VERSION >=CHIP_9638P
    if(enable) {
        SET_REG_BIT(REG_I2S_I2S_CTRL_0_I2S0_USE_BB_ADC_EN);
    }
    else {
        CLR_REG_BIT(REG_I2S_I2S_CTRL_0_I2S0_USE_BB_ADC_EN);
    }
#endif    
}
void VI2S1_Set_BB_AD_Enable(BOOL enable)
{
#if VDEV_CHIP_VERSION >=CHIP_9638P
    if(enable) {
        SET_REG_BIT(REG_I2S_I2S_CTRL_1_I2S1_USE_BB_ADC_EN);
    }
    else {
        CLR_REG_BIT(REG_I2S_I2S_CTRL_1_I2S1_USE_BB_ADC_EN);
    }
#endif    
}
void VI2S0_Set_BB_DA_Enable(BOOL enable)
{
#if VDEV_CHIP_VERSION >=CHIP_9638P
    if(enable) {
        SET_REG_BIT(REG_I2S_I2S_CTRL_0_I2S0_USE_BB_DAC_EN);
    }
    else {
        CLR_REG_BIT(REG_I2S_I2S_CTRL_0_I2S0_USE_BB_DAC_EN);
    }
#endif    
}
void VI2S1_Set_BB_DA_Enable(BOOL enable)
{
#if VDEV_CHIP_VERSION >=CHIP_9638P
    if(enable) {
        SET_REG_BIT(REG_I2S_I2S_CTRL_1_I2S1_USE_BB_DAC_EN);
    }
    else {
        CLR_REG_BIT(REG_I2S_I2S_CTRL_1_I2S1_USE_BB_DAC_EN);
    }
#endif    
}

void VI2S_InitClk_Once(void)
{
	#if (((CONFIG_OUTPUT_DEVICE & (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) \
			== (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) && \
			((CONFIG_INPUT_DEVICE & (CONFIG_INPUT_DEVICE_I2S0|CONFIG_INPUT_DEVICE_I2S1)) \
			== (CONFIG_INPUT_DEVICE_I2S0|CONFIG_INPUT_DEVICE_I2S1)))
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_Sync_SetTRx(0x07);
	#elif ((CONFIG_OUTPUT_DEVICE & (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) \
			== (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1))
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_Both_SetTx(TRUE);
	#elif (CONFIG_OUTPUT_DEVICE & CONFIG_OUTPUT_DEVICE_I2S0)
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_SetTx(0, TRUE);
	#elif (CONFIG_OUTPUT_DEVICE & CONFIG_OUTPUT_DEVICE_I2S1)
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_SetTx(1, TRUE);
	#endif
}

void VI2S_UninitClk_Once(void)
{
	#if (((CONFIG_OUTPUT_DEVICE & (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) \
			== (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) && \
			((CONFIG_INPUT_DEVICE & (CONFIG_INPUT_DEVICE_I2S0|CONFIG_INPUT_DEVICE_I2S1)) \
			== (CONFIG_INPUT_DEVICE_I2S0|CONFIG_INPUT_DEVICE_I2S1)))
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_Sync_SetTRx(0x07);
	#elif ((CONFIG_OUTPUT_DEVICE & (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1)) \
			== (CONFIG_OUTPUT_DEVICE_I2S0|CONFIG_OUTPUT_DEVICE_I2S1))
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_Both_SetTx(TRUE);
	#elif (CONFIG_OUTPUT_DEVICE & CONFIG_OUTPUT_DEVICE_I2S0)
		VI2S_Init(0, 1, 0, 48000, FALSE, 0);
		VI2S_SetTx(0, TRUE);
	#elif (CONFIG_OUTPUT_DEVICE & CONFIG_OUTPUT_DEVICE_I2S1)
		VI2S_Init(1, 1, 0, 48000, FALSE, 0);
		VI2S_SetTx(1, TRUE);
	#endif
}
