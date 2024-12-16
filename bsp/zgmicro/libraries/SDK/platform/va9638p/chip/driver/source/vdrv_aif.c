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
	AIF

Description:
 	AIF(Audio InterFace) driver.

Revision History:
 	Date                  Author                   Description
 	2010.03.31           chenjingen              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_gpio.h" 
#include "vdrv_aif.h"

/******************************************************************************
Description:
 	Initial the AIF module by specified CODEC.

Parameters:
	CodecType - specifies the type of CODEC, can be one of the following 
	            constants: AIF_CODEC_INTERNAL, AIF_CODEC_EXTERNAL
Return Value:
	None.
*****************************************************************************/
void VAIF_Initial(UINT8 CodecType)
{
	//configure the CDC_CTRL,for test codec
	WRITE_REG(REG_AIF_CDC_CTRL, AIF_DEFAULT_CDCCTRL);//TODO AIF_CDC_CTRL (g_VCDC_Debug.aif_cdc_ctrl) 需要等DAC调试时修改

#if	0//20191123xy, remove here, if choose I2S, then config the AIF_CDC_CTRL by eck better;
if(VCDC_GetVerNo() == CDC_VERSION_PLUS){
#if (VDEV_7621_I2S_EN == 1)	
	//select I2S DAC - colin20151013
	WRITE_REG(REG_AIF_CDC_CTRL,(READ_REG(REG_AIF_CDC_CTRL) | (1 << RBIT_AIF_CDCCTRL_I2S_EN_DAC)));
#endif
}
#endif
/*	for 9628, extern not exist
	if (CodecType == AIF_CODEC_INTERNAL) {
		CLR_REG_BIT(REG_AIF_CTRL, RBIT_AIF_CTRL_CODEC_SEL);
	} else if (CodecType == AIF_CODEC_EXTERNAL) {
		SET_REG_BIT(REG_AIF_CTRL, RBIT_AIF_CTRL_CODEC_SEL);
	}
*/

	/* disable all DAC and ADC channels and interrupts */
	VAIF_DisableDAC(AIF_DAC_L | AIF_DAC_R);
	VAIF_DisableADC(AIF_ADC_L | AIF_ADC_R);
	VAIF_DisableDACFIFOInt();
	VAIF_DisableADCFIFOInt();
	VAIF_DisableErrInt();

	if (AIF_DAC_DATA_WIDTH == DATA_WIDTH_16B) {
		SET_REG_BIT(REG_AIF_AIF_CTRL_AWIDTH_16B);
		SET_REG_BIT(REG_AIF_AIF_CTRL_DWIDTH_16B);
	} else {
		CLR_REG_BIT(REG_AIF_AIF_CTRL_AWIDTH_16B);
		CLR_REG_BIT(REG_AIF_AIF_CTRL_DWIDTH_16B);
	}

	if (AIF_ADC_CHN_COMBINE == COMBINE_RFIRST) {
		SET_REG_BIT(REG_AIF_AIF_CDC_EN_ADC_CHANC);
	} else {
		CLR_REG_BIT(REG_AIF_AIF_CDC_EN_ADC_CHANC);
	}	

	
#if VDEV_24BIT_DAC_SUPPORTED//20200914xy, from 507(clip4)20200805 change, enable DATA_WIDTH_24B; for init config;
	 //CLR_REG_BIT ( REG_AIF_CDC_CTRL, RBIT_AIF_CTRL_DWIDTH_16B );//AIF reg 04, bit[3] = 0(24bit), =1(16bit)//for 
	 //CLR_REG_BIT(REG_AIF_AIF_CTRL_AWIDTH_16B);
	 CLR_REG_BIT(REG_AIF_AIF_CTRL_DWIDTH_16B);//AIF reg 0C,

	 //WRITE_REG ( REG_DMA_AIF_RD_CTRL, ( READ_REG ( REG_DMA_AIF_RD_CTRL )&(~0x00000010) ) );//DMAC reg 1014 ?bit[4] = 0(24bit), =1(16bit)	
	 WRITE_REG ( REG_DMA_AIF_WR_CTRL, ( READ_REG ( REG_DMA_AIF_WR_CTRL )&(~0x00000010) ) );//DMAC reg 103C ?bit[4] = 0(24bit), =1(16bit)
#endif
}

/******************************************************************************
Description:
 	Sets the data source for internal DAC input.

Parameters:
	DACPath - the data source of DAC, can be one of the following constants:
	          AIF_DAC_VCI, AIF_DAC_FIFO
Return Value:
	None.

Remarks:
    This function is only for AIF_CODEC_INTERNAL. 
    The DAC data path can be set only when all DAC channels are disabled. 
*****************************************************************************/
void VAIF_SetDACPath(UINT8 DACPath)
{
	if (DACPath == AIF_DAC_VCI) {
		CLR_REG_BIT(REG_AIF_AIF_CTRL_DAC_PATH_SEL);
	} else if (DACPath == AIF_DAC_FIFO) {
		SET_REG_BIT(REG_AIF_AIF_CTRL_DAC_PATH_SEL);
	}
}

/******************************************************************************
Description:
 	Gets the data source for internal DAC input.

Parameters:
    None.

Return Value:
	The data source of DAC, can be one of the following constants: 
	AIF_DAC_VCI, AIF_DAC_FIFO.

Remarks:
    This function is only for AIF_CODEC_INTERNAL.
*****************************************************************************/
UINT8 VAIF_GetDACPath(void)
{
	if (GET_REG_BIT(REG_AIF_AIF_CTRL_DAC_PATH_SEL)) {
		return AIF_DAC_FIFO;
	} else {
		return AIF_DAC_VCI;
	}
}

/******************************************************************************
Description:
 	Sets the data destination for internal ADC output.

Parameters:
	ADCPath - the data destination of ADC, can be one of the following constants:
	          AIF_ADC_VCI, AIF_ADC_FIFO
Return Value:
	None.

Remarks:
    This function is only for AIF_CODEC_INTERNAL.
    The ADC data path can be set only when all ADC channels are disabled.
*****************************************************************************/
void VAIF_SetADCPath(UINT8 ADCPath)
{
	if (ADCPath == AIF_ADC_VCI) {
		CLR_REG_BIT(REG_AIF_AIF_CTRL_ADC_PATH_SEL);
	} else if (ADCPath == AIF_ADC_FIFO) {
		SET_REG_BIT(REG_AIF_AIF_CTRL_ADC_PATH_SEL);
	}	
}

/******************************************************************************
Description:
 	Gets the data destination for internal ADC output.

Parameters:
    None.

Return Value:
	The data destination of ADC, can be one of the following constants: 
	AIF_ADC_VCI, AIF_ADC_FIFO.

Remarks:
    This function is only for AIF_CODEC_INTERNAL.
*****************************************************************************/
UINT8 VAIF_GetADCPath(void)
{
	if (GET_REG_BIT(REG_AIF_AIF_CTRL_ADC_PATH_SEL)) {
		return AIF_ADC_FIFO;
	} else {
		return AIF_ADC_VCI;
	}
}

/******************************************************************************
Description:
 	Enables the specified DAC channel(s).

Parameters:
    DACChn - which channel(s) to enable, can be any combination of following 
             constants by performing bit-wise OR operation:
             AIF_DAC_L, AIF_DAC_R.    

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableDAC(UINT8 DACChn)
{
	WRITE_REG(REG_AIF_AIF_CDC_EN, READ_REG(REG_AIF_AIF_CDC_EN) | (UINT32)DACChn);
}

/******************************************************************************
Description:
 	Disables the specified DAC channel(s).

Parameters:
    DACChn - which channel(s) to disable, can be any combination of following 
             constants by performing bit-wise OR operation:
             AIF_DAC_L, AIF_DAC_R.    

Return Value:
	None.
*****************************************************************************/
void VAIF_DisableDAC(UINT8 DACChn)
{
	WRITE_REG(REG_AIF_AIF_CDC_EN, READ_REG(REG_AIF_AIF_CDC_EN) & (~(UINT32)DACChn));
}

/******************************************************************************
Description:
 	Enables the specified ADC channel(s).

Parameters:
    ADCChn - which channel(s) to enable, can be any combination of following 
             constants by performing bit-wise OR operation:
             AIF_ADC_L, AIF_ADC_R.    

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableADC(UINT8 ADCChn)
{
	WRITE_REG(REG_AIF_AIF_CDC_EN, READ_REG(REG_AIF_AIF_CDC_EN) | (UINT32)ADCChn);
}

/******************************************************************************
Description:
 	Disables the specified ADC channel(s).

Parameters:
    ADCChn - which channel(s) to disable, can be any combination of following 
             constants by performing bit-wise OR operation:
             AIF_ADC_L, AIF_ADC_R.    

Return Value:
	None.
*****************************************************************************/
void VAIF_DisableADC(UINT8 ADCChn)
{

	WRITE_REG(REG_AIF_AIF_CDC_EN, READ_REG(REG_AIF_AIF_CDC_EN) & (~(UINT32)ADCChn));
}

/******************************************************************************
Description:
 	Enables the specified ADC\DAC channel(s).

Parameters:
    ADCChn - which channel(s) to enable, can be any combination of following 
             constants by performing bit-wise OR operation:
             AIF_ADC_L, AIF_ADC_R.   AIF_DAC_L, AIF_DAC_R.      

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableADCDAC(UINT8 Chn)
{
	WRITE_REG(REG_AIF_AIF_CDC_EN, READ_REG(REG_AIF_AIF_CDC_EN) | (UINT32)Chn);
}

/******************************************************************************
Description:
 	Gets the AIF ADC or DAC enable, if adc or dac enable the clock is working.

Parameters:
    None.

Return Value:
	The AIF adc and dac enable.

Remarks:
    This function return the adc|dac enable.
*****************************************************************************/
UINT8 VAIF_GetEnStatus(void)
{
	return (READ_REG(REG_AIF_AIF_CDC_EN)&0x0F);
}
/******************************************************************************
Description:
 	Gets the DAC enable, if dac enable the clock is working.

Parameters:
    None.

Return Value:
	The AIF dac enable.

Remarks:
    This function return the dac enable.
*****************************************************************************/
UINT8 VAIF_GetDACEnStatus(void)
{
	return (READ_REG(REG_AIF_AIF_CDC_EN)&0x0C);
}
/******************************************************************************
Description:
 	Gets the ADC enable, if dac enable the clock is working.

Parameters:
    None.

Return Value:
	The AIF dac enable.

Remarks:
    This function return the dac enable.
*****************************************************************************/
UINT8 VAIF_GetADCEnStatus(void)
{
	return (READ_REG(REG_AIF_AIF_CDC_EN)&0x03);
}
/******************************************************************************
Description:
 	Reads current data from ADC FIFO.

Parameters:
    None.    

Return Value:
	Returns a 32-bit word combined with 2 16-bit audio samples.
*****************************************************************************/
UINT32 VAIF_ReadADCFIFO(void)
{
	return 0;
}

/******************************************************************************
Description:
 	Writes data to DAC FIFO.

Parameters:
    Data - the digital value of audio sample to write.

Return Value:
	None.

Remarks:
    The width of data is determined by the setting of AIF_DAC_DATA_WIDTH.
*****************************************************************************/
void VAIF_WriteDACFIFO(UINT32 Data)
{
	;
}

/******************************************************************************
Description:
 	Sets the trigger level for DAC FIFO.

Parameters:
    NumOfWords - the specified trigger level.

Return Value:
	None.

Remarks:
    IRQ_AIF_DAC_FWR interrupt will be triggered if the number of available data 
    in the FIFO is less than the specified level..
*****************************************************************************/
void VAIF_SetDACFIFOTrigLevel(UINT16 NumOfWords)
{
	;
}

/******************************************************************************
Description:
 	Sets the trigger level for ADC FIFO.

Parameters:
    NumOfWords - the specified trigger level.

Return Value:
	None.

Remarks:
    IRQ_AIF_ADC_FRD interrupt will be triggered if the number of available data 
    in the FIFO is more than the specified level..
*****************************************************************************/
void VAIF_SetADCFIFOTrigLevel(UINT16 NumOfWords)
{
	;
}

/******************************************************************************
Description:
 	Gets the trigger level for DAC FIFO.

Parameters:
    None.

Return Value:
	Returns the trigger level.
*****************************************************************************/
UINT16 VAIF_GetDACFIFOTrigLevel(void)
{
	return 0;
}

/******************************************************************************
Description:
 	Gets the trigger level for ADC FIFO.

Parameters:
    None.

Return Value:
	Returns the trigger level.
*****************************************************************************/
UINT16 VAIF_GetADCFIFOTrigLevel(void)
{
	return 0;
}

/******************************************************************************
Description:
 	Resets the DAC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_ResetDACFIFO(void)
{
	;
}

/******************************************************************************
Description:
 	Resets the ADC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_ResetADCFIFO(void)
{
	;
}

/******************************************************************************
Description:
 	Enables the interrupt of DAC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableDACFIFOInt(void)
{
	;
}

/******************************************************************************
Description:
 	Enables the interrupt of ADC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableADCFIFOInt(void)
{	
	;
}

/******************************************************************************
Description:
 	Disables the interrupt of DAC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_DisableDACFIFOInt(void)
{
	;
}

/******************************************************************************
Description:
 	Disables the interrupt of ADC FIFO.

Parameters:
    None.

Return Value:
	None.
*****************************************************************************/
void VAIF_DisableADCFIFOInt(void)
{
	;
}

/******************************************************************************
Description:
 	Gets the number of available data in DAC FIFO.

Parameters:
    None.

Return Value:
	Returns the number of available data in DAC FIFO.
*****************************************************************************/
UINT16 VAIF_GetDACFIFOCnt(void)
{
	return 0;
}

/******************************************************************************
Description:
 	Gets the number of available data in ADC FIFO.

Parameters:
    None.

Return Value:
	Returns the number of available data in ADC FIFO.
*****************************************************************************/
UINT16 VAIF_GetADCFIFOCnt(void)
{
	return 0;
}

/******************************************************************************
Description:
 	Sets the constant sample rate for internal DAC.

Parameters:
    SampleRate - valid sample rate to set.

Return Value:
	None.
	
Remarks:
    This function is only for AIF_CODEC_INTERNAL.
    The sample rate can be set only when all DAC and ADC channels are disabled.
*****************************************************************************/
void VAIF_SetDACSampleRate(UINT32 SampleRate)
{
	UINT32 fs_sel = 0x0;
	
	VAPP_VOICE_NPRINTF("aif SetDACSampleRate[%d].\r\n", SampleRate); 
	VCOMMON_NPRINTF("SetDAC SR=%d.\r\n", SampleRate);
	 
	if (SampleRate == 11025)
		WRITE_REG(REG_AIF_DACFS, (UINT32)0x00000F9F);	  
	else if (SampleRate == 22050)
		WRITE_REG(REG_AIF_DACFS, (UINT32)0x000007CF);
	else if (SampleRate == 44100)
		WRITE_REG(REG_AIF_DACFS, (UINT32)0x000003E7);
	else
		WRITE_REG(REG_AIF_DACFS, (UINT32)(48000000 / SampleRate - 1));
	
	switch (SampleRate) {
	case 8000:
		fs_sel = 0x00;
		break;
	case 11025:
		fs_sel = 0x21;
		break;
	case 12000:
		fs_sel = 0x01;
		break;
	case 16000:
		fs_sel = 0x02;
		break;
	case 22050:
		fs_sel = 0x23;
		break;
	case 24000:
		fs_sel = 0x03;
		break;
	case 32000:
		fs_sel = 0x04;
		break;
	case 44100:
		fs_sel = 0x25;

#if 0//VCODEC_HWGAINSYC_SUPPORTED	//20201231xy, remove here not need, or may set 48K==0x05, then play data not enough had many up0;
		if(VCODEC_ORIOLE_HWGainSycGetEn(VCODEC_ORIOLE_GAINSYC_DACMASK)){
			fs_sel = 0x05;//for gain sys set fs_sel->5 for SR ==44100
		}
#endif
		break;
	case 48000:
	case 96000:
		fs_sel = 0x05;
		break;	
	}
	
	WRITE_REG(REG_AIF_CDC_CTRL_2, (READ_REG(REG_AIF_CDC_CTRL_2) & (~0x00000001)));	

	WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL) & (~0x000003F0)) | (fs_sel << 4));

	//fs_96k_dac==AIF_CTRL[10]fs_96k_dac
	if(SampleRate == 96000){
		SET_REG_BIT(REG_AIF_AIF_CTRL_FS_DAC_96K);
	} else {
		CLR_REG_BIT(REG_AIF_AIF_CTRL_FS_DAC_96K);
	}
}

/******************************************************************************
Description:
 	Configure sample rate change. The SRC module is only for sample rates around 48KHz and 
 	44.1KHz. And the adjusting scope is 41.871KHz~54.857KHz.

Parameters:
    RatedSR - rated sample rate to set, the unit is Hz
    ActualSRx100000 - actual sample rate, the unit is 0.0001Hz
    
Return Value:
	the critical configure of src.
	
Remarks:
    This function is only for AIF_CODEC_INTERNAL, and only for chip version >= 9613IDA.
*****************************************************************************/
UINT16 VAIF_SetDACSampleRateChangeAccurately(UINT32 RatedSR, UINT32 ActualSRx10000)
{
	const UINT32 step = 428571; // 42.85714Hz
	UINT32 r1, r1_old, r1_center;
	UINT32 fs_sel;
	UINT32 fs_cycle;

	VCOMMON_NPRINTF("aif SetDACSampleRateChangeAccurately[%d-%d].\r\n", RatedSR, ActualSRx10000); 
	
	if (RatedSR == 48000) {
		fs_sel = 0x05;
		r1_center = r1 = 1120;
		fs_cycle = 874;
	} else if (RatedSR == 44100) {
		fs_sel = 0x25;
		r1_center = r1 = 1029;
		fs_cycle = 874;
	} else {
		// others don't support dynamic src
		VAIF_SetDACSampleRate(RatedSR);
		return 0;
	}
	
	WRITE_REG(REG_AIF_DACFS, fs_cycle);

	RatedSR *= 10000;
	if (ActualSRx10000 > RatedSR) {
		r1 += (ActualSRx10000 - RatedSR + step/2)/step;
	} else if (ActualSRx10000 < RatedSR) {
		r1 -= (RatedSR - ActualSRx10000 + step/2)/step;
	}
	
	if (r1 > 1280) {
		r1 = 1280;
	} else if (r1 < 977) {
		r1 = 977;
	}

	r1_old = (READ_REG(REG_AIF_CDC_CTRL_2) >> 4) & 0x7FF;
	if ((r1_old < r1_center && r1 > r1_center) || (r1_old > r1_center && r1 < r1_center)) {
		// r1 should not transfer over 1120/1029
		WRITE_REG(REG_AIF_CDC_CTRL_2, ((r1_center << 4) | 0x00000001));
		VCOMMON_Delay(100);
	}
	
	WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL) & (~0x000003F0)) | (fs_sel << 4));
	WRITE_REG(REG_AIF_CDC_CTRL_2, ((r1 << 4) | 0x00000001));

	return r1;	
}

/******************************************************************************
Description:
 	Configure sample rate change estimately. The SRC module is only for sample rates around 
 	48KHz and 44.1KHz. And the adjusting scope is 41.871KHz~54.857KHz. The basic thought of
 	this function is to keep adjusting the sample rate, and the hypothesis is that the drift of 
 	estimated sample rate will go convergently. So the more times to call this function, the more
 	precise estimated sample rate is.
	The estimating process:
	A. find the basic zone of sample rate according to increase adjusting step size by power law
	B. adjust sub-zone after basic zone be sure.
Parameters:
    RatedSR - rated sample rate to set, the unit is Hz
    LastR1 - returned value from last time of calling this function, 0 if first time to call. If Trend is
    			set to INVERSE, LastR1 is useless.
    Trend - VAIF_SRC_TREND, to figure out the adjust driection
    
Return Value:
	the critical configure of src.
	
Remarks:
    This function is only for AIF_CODEC_INTERNAL, and only for chip version >= 9613IDA.
*****************************************************************************/
UINT16 VAIF_SetDACSampleRateChangeEstimately(UINT32 RatedSR, UINT16 LastR1, VAIF_SRC_TREND Trend)
{
	UINT16 r1;
	UINT32 fs_sel;
	UINT32 fs_cycle;
	UINT16 diff, residual, integer;
	SINT8 int_power = -1,i;
	BOOL sign;
	BOOL done = FALSE;

	VAPP_VOICE_NPRINTF("aif SetDACSampleRateChangeEstimately[%d-%d, %d].\r\n", RatedSR, LastR1, Trend); 
	
	if (RatedSR == 48000) {
		fs_sel = 0x05;
		r1 = 1120;
		fs_cycle = 874;
	} else if (RatedSR == 44100) {
		fs_sel = 0x25;
		r1 = 1029;
		fs_cycle = 874;
	} else {
		// others don't support dynamic src
		VAIF_SetDACSampleRate(RatedSR);
		return 0;
	}

	WRITE_REG(REG_AIF_DACFS, fs_cycle);

	if (Trend != VAIF_SRC_TREND_INVERSE) {
		if (LastR1 == 0) {
			// firstly calling this function
			LastR1 = r1;
		}

		sign = (LastR1 >= r1);
		diff = sign ? (LastR1 - r1) : (r1 - LastR1);

		for (i = 8;i >= 0;i--) {
			// max of diff is 0x12F(1280-977)
			if (diff & (0x1 << i)) {
				int_power = i;
				break;
			}
		}

		if (int_power >= 0) {
			integer = (0x1 << int_power);
		} else {
			integer = 0;
		}
		residual = diff - integer;

		if (Trend == VAIF_SRC_TREND_UPSTEP) {
			// upstep
			if (residual == 0) {
				// rough adjustment
				if (integer > 0)
					integer <<= 1;
				else
					integer = 1;
			} else {
				// detailed adjustment
				for (i = ((SINT8)int_power - 1);i >= 1;i--) {
					if (residual & (0x2 << (i-1))) {
						residual = residual | (0x3 << (i-1));
						done = TRUE;
						break;
					}
				}
				if (done == FALSE) {
					// it means the estimated SR is not convergent anymore
					residual += 1;
				}
			}
			
		} else {
			// downstep
			if (residual == 0) {
				// rough adjustment
				if (integer > 0) {
					integer >>= 1;
				} else {
					// lastr1==r1, inverse 
					integer = 1;
					sign = !sign;
				}
			} else {
				// detailed adjustment
				for (i = ((SINT8)int_power - 1);i >= 1;i--) {
					if (residual & (0x2 << (i-1))) {
						residual = residual & (~(0x2 << (i-1))) | (0x1 << (i-1));
						done = TRUE;
						break;
					}
				}
				if (done == FALSE) {
					// it means the estimated SR is not convergent anymore
					residual -= 1;
				}
			}
		}
		diff = integer + residual;
		if (sign) {
			r1 = r1 + diff;
		} else {
			r1 = r1 - diff;
		}

		if (r1 > 1280) {
			r1 = 1280;
		} else if (r1 < 977) {
			r1 = 977;
		}
	}else {
		// do nothing but set the rated sample rate
	}
	
	WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL) & (~0x000003F0)) | (fs_sel << 4));
	WRITE_REG(REG_AIF_CDC_CTRL_2, ((r1 << 4) | 0x00000001));

	return r1;
}


/******************************************************************************
Description:
 	Configure sample rate change. The SRC module is only for sample rates around 48KHz and 
 	44.1KHz. And the adjusting scope is 41.871KHz~54.857KHz.

Parameters:
    RatedSR - rated sample rate to set, the unit is Hz
    changeRate:
    
Return Value:
	the critical configure of src.
	
Remarks:
    This function is only for AIF_CODEC_INTERNAL, and only for chip version >= 9613IDA.
*****************************************************************************/
void VAIF_SetDACSampleRateChangeTWS(UINT32 RatedSR, SINT32 changeRate)
{
	UINT32 r1, r1_old, r1_center;
	UINT32 fs_sel;
	UINT32 fs_cycle;
 
#if 0//(VDBG_EN > 0)
	// static UINT8 g_DACSRCTWS_Cnt = 0;
	//if(!g_DACSRCTWS_Cnt++)
	//	VCOMMON_NPRINTF("aif_SetDACSRCTWS[%d,%d].\r\n", RatedSR, changeRate);
	
	#if VDEV_A2DPBROADCAST_SUPPORTED
	extern UINT32 VCODEC_HW_GetDACSampleCnt(void);
	extern UINT8 HLP_SM_GetA2dpBroadcastState(void);
	
	if(!HLP_SM_GetA2dpBroadcastState()){
		static UINT16 g_TmpSRC_Cnt = 1000;
		if(changeRate == 0){
			if(((g_TmpSRC_Cnt++)%1000)==0){
				VCOMMON_NPRINTF("D-%d\r\n", VCODEC_HW_GetDACSampleCnt());
			}
        }
        else
        {
        //    VCOMMON_NPRINTF("sr%d", changeRate);
		}
	}
	#else	
	if(changeRate != 0){
		//VAPP_VOICE_NPRINTF("SRT[%d].\r\n", changeRate); 
		if(!HLP_SM_GetA2dpBroadcastState() )
			VCOMMON_NPRINTF("SRT[%d]\r\n", changeRate); 
	}
	#endif
#endif

	if (RatedSR == 48000) {
		fs_sel = 0x05;
		r1_center = r1 = 1120;
		fs_cycle = 874;
	} else if (RatedSR == 44100) {
		fs_sel = 0x25;
		r1_center = r1 = 1029;
		fs_cycle = 874;
	} else {
		// others don't support dynamic src
		VAIF_SetDACSampleRate(RatedSR);
		return;
	}

	r1 = r1_center + changeRate;
	r1_old = (READ_REG(REG_AIF_CDC_CTRL_2) >> 4) & 0x7FF;

	#if 0//20200910xy, if r1_old == r1 also should set the SampleRate; //0911 close here;
	if (r1_old == r1){
		VAIF_SetDACSampleRate(RatedSR);
		return;
	}
	#else
	if (r1_old == r1)
		return;
	#endif
	
	WRITE_REG(REG_AIF_DACFS, fs_cycle);
	if ((r1_old < r1_center && r1 > r1_center) || (r1_old > r1_center && r1 < r1_center)) {
		// r1 should not transfer over 1120/1029
		WRITE_REG(REG_AIF_CDC_CTRL_2, ((r1_center << 4) | 0x00000001));
		//VCOMMON_Delay(100);
	}
	
	WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL) & (~0x000003F0)) | (fs_sel << 4));
	WRITE_REG(REG_AIF_CDC_CTRL_2, ((r1 << 4) | 0x00000001));
}

/******************************************************************************
Description:
 	Sets the sample rate for internal ADC.

Parameters:
    SampleRate - valid sample rate to set: 8000, 16000, and 48000.

Return Value:
	None.
	
Remarks:
    This function is only for AIF_CODEC_INTERNAL.
    The sample rate can be set only when all DAC and ADC channels are disabled.
*****************************************************************************/
void VAIF_SetADCSampleRate(UINT32 SampleRate)
{
	UINT32 deci_fs_sel;

	switch (SampleRate) {
	case 8000:
		deci_fs_sel = 0;
		break;
	case 16000:
		deci_fs_sel = 1;
		break;
	case 24000:
		deci_fs_sel = 2;
		break;
	case 32000:
		deci_fs_sel = 3;
		break;
	case 48000:
	case 96000:
		deci_fs_sel = 4;
		break;
	default:
		deci_fs_sel = 0;
		break;
	}

	WRITE_REG(REG_AIF_CDC_CTRL_2, (READ_REG(REG_AIF_CDC_CTRL_2) & (~0x000F0000)) | (deci_fs_sel << 16)); //deci_fs_sel[3:0]==CDC_CTRL_2[19:16]DECI_FS_SEL

	//fs_96k_adc==AIF_CTRL[25]fs_96k_adc
	if(SampleRate == 96000){
		SET_REG_BIT(REG_AIF_AIF_CTRL_FS_ADC_96K);
	} else {
		CLR_REG_BIT(REG_AIF_AIF_CTRL_FS_ADC_96K);
	}
}

/******************************************************************************
Description:
 	Gets the error status of ADC or DAC FIFO.

Parameters:
    None.

Return Value:
	Returns a bit-packed byte to indicate errors, might be any combination of 
	following constants by performing bit-wise OR operation: 
 	AIF_ERR_ADC_UNDERFLOW, AIF_ERR_ADC_OVERFLOW, AIF_ERR_DAC_UNDERFLOW and 
 	AIF_ERR_DAC_OVERFLOW.
*****************************************************************************/
UINT8 VAIF_GetErrStatus(void)
{
	return (UINT8)READ_REG(REG_AIF_FERR);
}

/******************************************************************************
Description:
 	Masks error(s) for DAC and ADC FIFOs.

Parameters:
 	ErrToMask - which error(s) to mask, can be any combination of following 
 	            constants by performing bit-wise OR operation: 
 	            AIF_ERR_ADC_UNDERFLOW, AIF_ERR_ADC_OVERFLOW, AIF_ERR_DAC_UNDERFLOW 
 	            and AIF_ERR_DAC_OVERFLOW.

Return Value:
	None.
*****************************************************************************/
void VAIF_MaskError(UINT8 ErrToMask)
{
	WRITE_REG(REG_AIF_FERR_MSK, READ_REG(REG_AIF_FERR_MSK) | (UINT32)ErrToMask);
}

/******************************************************************************
Description:
 	Unmasks error(s) for DAC and ADC FIFOs.

Parameters:
 	ErrToUnMask - which error(s) to unmask, can be any combination of following 
 	              constants by performing bit-wise OR operation: 
 	              AIF_ERR_ADC_UNDERFLOW, AIF_ERR_ADC_OVERFLOW, AIF_ERR_DAC_UNDERFLOW 
 	              and AIF_ERR_DAC_OVERFLOW.

Return Value:
	None.
*****************************************************************************/
void VAIF_UnMaskError(UINT8 ErrToUnMask)
{
	WRITE_REG(REG_AIF_FERR_MSK, READ_REG(REG_AIF_FERR_MSK) & (~ ((UINT32)ErrToUnMask)));
}

/******************************************************************************
Description:
 	Clears error(s) for DAC and ADC FIFOs.

Parameters:
 	ErrToClr - which error(s) to clear, can be any combination of following 
 	           constants by performing bit-wise OR operation: 
 	           AIF_ERR_ADC_UNDERFLOW, AIF_ERR_ADC_OVERFLOW, AIF_ERR_DAC_UNDERFLOW 
 	           and AIF_ERR_DAC_OVERFLOW.

Return Value:
	None.
*****************************************************************************/
void VAIF_ClearError(UINT8 ErrToClr)
{
	WRITE_REG(REG_AIF_FERRCLR, (UINT32)ErrToClr);
}

/******************************************************************************
Description:
 	Enables error interrupt for DAC and ADC FIFOs.

Parameters:
 	None.

Return Value:
	None.
*****************************************************************************/
void VAIF_EnableErrInt(void)
{	
}

/******************************************************************************
Description:
 	Disables error interrupt for DAC and ADC FIFOs.

Parameters:
 	None.

Return Value:
	None.
*****************************************************************************/
void VAIF_DisableErrInt(void)
{
}

/******************************************************************************
Description:
 	Gets the quantization width of analog Sigma-Delta ADC, for 9638.

Parameters:
    None.

Return Value:
	Select the quantization width of analog Sigma-Delta ADC:
	2'b00 - 1bit ADC,
	2'b01 - 2bit ADC,
	2'b10 - 3bit ADC,
	2'b11 - 4bit ADC,
*****************************************************************************/
UINT8 VAIF_GetWSELADC(void)
{
	return (UINT8)((READ_REG(REG_AIF_CDC_CTRL)>>RBIT_AIF_CDCCTRL_WSEL_ADC)&0x03);
}

/******************************************************************************
Description:
 	Sets the GainSync, for 9638p.

Parameters:
	mode:  == 0 close the gain sync, ==1 only open dac gain sync, ==2 only open adc gain sync, ==3 open both dac&adc gain sync;
	SampleRate -- valid sample rate for GainSync set.

Return Value:
	None.
*****************************************************************************/
void VAIF_GainSyncSet (UINT8 mode, UINT32 SampleRate)
{
	UINT32 tmp;
	//for dac gain sync set;
	VAPP_VOICE_NPRINTF("REG_AIF_AIF_CTRL1 = %x mode=%d \r\n",READ_REG(REG_AIF_AIF_CTRL),mode);
	if (mode & 0x01) {
		SET_REG_BIT(REG_AIF_CDC_CTRL_I2S_EN_DAC);
		tmp = (READ_REG(REG_AIF_AIF_CTRL)|(1 << 16))&0xFE01FFFF;//set bit16 and clear bit[17~24]

		if(SampleRate == 8000){
			WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x0E<< 17));//set bit[17~24] as (8K)8'd255  312 255 
		} else if(SampleRate == 16000){
			WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x0E<< 17));//set bit[17~24] as  (16K)8'd169
		} else if(SampleRate == 44100) {
			WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x0E<< 17));//set bit[17~24] as  (44.1K)8'd76//4c
		} else if(SampleRate == 48000) {
			//WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x4A<< 17));//set bit[17~24] as  (48K)8'd74
			WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x0E<< 17));//20200520xy, for VA9638p_Audio_CODEC_DigitalPart_DesignSpec_v0p2;
		} else {
			;
		}
		VAPP_VOICE_NPRINTF("REG_AIF_AIF_CTRL2 = %x \r\n",READ_REG(REG_AIF_AIF_CTRL));
	} else {
		CLR_REG_BIT(REG_AIF_CDC_CTRL_I2S_EN_DAC);
		WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL)&(~(BIT_MASK(16)))));//20200915xy, dac clear bit16;
	}

	//for adc gain sync set;
	if (mode & 0x02) {
		SET_REG_BIT(REG_AIF_CDC_CTRL_I2S_EN_ADC);
		tmp = (READ_REG(REG_AIF_AIF_CTRL)|(1 << 15))&0xFFFF87FF;//set bit15 and clear bit[11~14]
		WRITE_REG(REG_AIF_AIF_CTRL, tmp|(0x6<< 11));//set bit[11~14] as 4'd10;A
	} else {
		CLR_REG_BIT(REG_AIF_CDC_CTRL_I2S_EN_ADC);
		WRITE_REG(REG_AIF_AIF_CTRL, (READ_REG(REG_AIF_AIF_CTRL)&(~(BIT_MASK(15)))));//20200915xy, adc clear bit15;
	}
}

/******************************************************************************
Description:
 	Gets the config of  "*2" gain of decimation filter, decim_gain_mul2=CDC_CTRL[28], add from 9638D for fix ADC 1bit will lost bug find at 9638B.

Parameters:
    None.

Return Value:
	0 - close and should fixed by SW(VCODEC_HandleAdc_9638proc);1 - open the ADC 1bit will lost bug fixed by aif, or no hw bug;
*****************************************************************************/
UINT8 VAIF_GetDGAIN2MUL(void)
{
	return (UINT8)((READ_REG(REG_AIF_CDC_CTRL)>>RBIT_AIF_CDCCTRL_DECIM_GAIN_MUL2)&0x01);//set by eck;
}

void VAIF_SetGPIO_PDM_mode(void)
{
 	VGPIO_SetMuxFunction(GPIO_MUX_FUNC_I2S0, GPIO_MUX_FUNC_I2S0);
 	VGPIO_SetMux2Function(GPIO_MUX_FUNC_I2S0, GPIO_MUX_FUNC_NONE);
 	VGPIO_SetMux2Function(GPIO_MUX_FUNC_I2S0, GPIO_MUX_FUNC_I2S0);
}

void VAIF_PDM_MIC_Enable(BOOL Enable)
{
	if(Enable)
	{
		SET_REG_BIT(REG_AIF_PDM_CTRL_PDM_MIC_EN);
	}
	else
	{
		CLR_REG_BIT(REG_AIF_PDM_CTRL_PDM_MIC_EN);
	}
}

void VAIF_PDM_MIC_CLKDIV2_Enable(BOOL Enable)
{
	if(Enable)
	{
		SET_REG_BIT(REG_AIF_PDM_CTRL_PDM_CLKDIV2_EN);
	}
	else
	{
		CLR_REG_BIT(REG_AIF_PDM_CTRL_PDM_CLKDIV2_EN);
	}
}

void VAIF_PDM_MIC_LRCH_ONEIO_Enable(BOOL Enable)
{
	if(Enable)
	{
		SET_REG_BIT(REG_AIF_PDM_CTRL_PDM_RCH_SEL);
	}
	else
	{
		CLR_REG_BIT(REG_AIF_PDM_CTRL_PDM_RCH_SEL);
	}
}

void VAIF_Set_BB_AD_Enable(BOOL enable)
{
    if(enable) {
        SET_REG_BIT(REG_AIF_AIF_CDC_EN_bb_syn_adc_sel);
    }
    else {
        CLR_REG_BIT(REG_AIF_AIF_CDC_EN_bb_syn_adc_sel);
    }  
}

void VAIF_Set_BB_DA_Enable(BOOL enable)
{
    if(enable) {
        SET_REG_BIT(REG_AIF_AIF_CDC_EN_bb_syn_dac_sel);
    }
    else {
        CLR_REG_BIT(REG_AIF_AIF_CDC_EN_bb_syn_dac_sel);
    }
}
