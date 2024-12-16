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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VAIF_H_
#define _VAIF_H_

#ifdef __cplusplus 
extern "C" {
#endif


#if defined (HW_CDC_DEBUG) && !defined(CORE_SEL_MCUTOP)
#include "vdrv_cdc.h"
extern VCDC_DEBUG 					    g_VCDC_Debug;
#define AIF_DEFAULT_CDCCTRL       		g_VCDC_Debug.aif_cdc_ctrl
#else
//for codec test 
#define AIF_DEFAULT_CDCCTRL 			0x00070914	//CDC_CTRL //[2:0]hpf_sel,[3]clkadc_sel,[7:4]sdm_sel,[15:8]ditherpow,[19:16]decim_gain_sel
#endif

/* DAC data width setting */
#define DATA_WIDTH_24B 					0
#define DATA_WIDTH_16B  				1
#define AIF_DAC_DATA_WIDTH 				DATA_WIDTH_16B

/* ADC data left and right channel data combination mode setting */
#define COMBINE_LFIRST 					0 // ADC data combined due to cdc_hpf_vao_l
#define COMBINE_RFIRST 					1 // ADC data combined due to cdc_hpf_vao_r
#define AIF_ADC_CHN_COMBINE 			COMBINE_LFIRST 	

#define AIF_ADC_FIFO_DEPTH 				16
#define AIF_DAC_FIFO_DEPTH 				64

#define AIF_CODEC_INTERNAL 				0 	
#define AIF_CODEC_EXTERNAL 				1

/* DAC data path selection */
#define AIF_DAC_VCI 					0 // DAC data is from CEVA VCI
#define AIF_DAC_FIFO 					1 // DAC data is from DAC FIFO

/* ADC data path selection */
#define AIF_ADC_VCI 					0 // ADC data is output to CEVA VCI
#define AIF_ADC_FIFO 					1 // ADC data is output to FIFO

/* ADC & DAC channels */
#define AIF_ADC_L 						((UINT8)0x1)
#define AIF_ADC_R 						((UINT8)0x2)
#define AIF_DAC_L 						((UINT8)0x4)
#define AIF_DAC_R 						((UINT8)0x8)

/* ADC &  DAC FIFO errors */
#define AIF_ERR_ADC_UNDERFLOW 			((UINT8)0x1)
#define AIF_ERR_ADC_OVERFLOW 			((UINT8)0x2)
#define AIF_ERR_DAC_UNDERFLOW 			((UINT8)0x4)
#define AIF_ERR_DAC_OVERFLOW 			((UINT8)0x8)

/* bits definition of ADC FIFO and DAC FIFO Reset Register */
#define RBIT_AIF_FRST_ADCRST 			0
#define RBIT_AIF_FRST_DACRST 			1
// [31:2], reserved

/* bits definition of AIF Control Register */
#define RBIT_AIF_CTRL_ADC_PATH_SEL 		0
#define RBIT_AIF_CTRL_DAC_PATH_SEL 		1
#define RBIT_AIF_CTRL_AWIDTH_16B 		2
#define RBIT_AIF_CTRL_DWIDTH_16B 		3
// [9:4], FS_SEL, sample frequency selection signal. 
// [31:10], reserved

/* bits definition of AIF & CODEC Work Enable Register */
#define RBIT_AIF_CDCEN_ADCL_EN 			0
#define RBIT_AIF_CDCEN_ADCR_EN 			1
#define RBIT_AIF_CDCEN_DACL_EN 			2
#define RBIT_AIF_CDCEN_DACR_EN 			3
#define RBIT_AIF_CDCEN_ADC_CHANC 		4
// [31:5], reserved

/* bits definition of CODEC Control Register */
// [2:0], HPF_SEL, ADC Digital Channel Filter Configuration
// [3], CLKADC_SEL, The frequency of clkadc configuration signal
#define RBIT_AIF_CDCCTRL_CLKADC_SEL 	3
// [7:4], SDM_SEL, "Sigma-Delta Modulator L/R" block configuration signal
// [15:8], DITHERPOW, DAC Digital Channel Filter Configuration
//[19:16], DECIM_GAIN_SEL, Decimation gain selection.
//[21:20], DECI_FS_SEL, Decimation Fs selection.
//[22], LOOPBACK_EN, loopback enable.
//[23], I2S_EN_ADC, Select data transfer mode between VA7621 and VA9628, for ADC channel.0 - direct transfer data, for 1bit ADC;1 - I2S interface enable, for 2bit~4bit ADC.
//[25:24], W_SEL_ADC, Select the quantization width of analog Sigma-Delta ADC:2'b00 - 1bit ADC,2'b01 - 2bit ADC,2'b10 - 3bit ADC,2'b11 - 4bit ADC,
#define RBIT_AIF_CDCCTRL_WSEL_ADC 	24
//[26], I2S_EN_DAC, Select data transfer mode between VA7621 and VA9628, for DAC channel.0 - direct transfer data;1 - I2S interface enable.
#define RBIT_AIF_CDCCTRL_I2S_EN_DAC     26
//[27], I2CCLK_OUT_EN, I2C clock enable signal for 1bit ADC, active high

//[28], decim_gain_mul2, enable "*2" gain of decimation filter, active high.0 - close and should fixed by SW;1 - open the ADC 1bit will lost bug fixed by aif.
#define RBIT_AIF_CDCCTRL_DECIM_GAIN_MUL2     28

//[31:28], Reserved.


typedef enum {
	VAIF_SRC_TREND_UPSTEP,
	VAIF_SRC_TREND_DOWNSTEP,
	VAIF_SRC_TREND_INVERSE
} VAIF_SRC_TREND;

void VAIF_Initial(UINT8 CodecType);
void VAIF_SetDACPath(UINT8 DACPath);
UINT8 VAIF_GetDACPath(void);
void VAIF_SetADCPath(UINT8 ADCPath);
UINT8 VAIF_GetADCPath(void);
void VAIF_EnableDAC(UINT8 DACChn);
void VAIF_DisableDAC(UINT8 DACChn);
void VAIF_EnableADC(UINT8 ADCChn);
void VAIF_DisableADC(UINT8 ADCChn);
void VAIF_EnableADCDAC(UINT8 Chn);
UINT32 VAIF_ReadADCFIFO(void);
void VAIF_WriteDACFIFO(UINT32 Data);
void VAIF_ResetDACFIFO(void);
void VAIF_ResetADCFIFO(void);
void VAIF_SetDACFIFOTrigLevel(UINT16 NumOfWords);
void VAIF_SetADCFIFOTrigLevel(UINT16 NumOfWords);
UINT16 VAIF_GetDACFIFOTrigLevel(void);
UINT16 VAIF_GetADCFIFOTrigLevel(void);
void VAIF_EnableDACFIFOInt(void);
void VAIF_EnableADCFIFOInt(void);
void VAIF_DisableDACFIFOInt(void);
void VAIF_DisableADCFIFOInt(void);
UINT16 VAIF_GetDACFIFOCnt(void);
UINT16 VAIF_GetADCFIFOCnt(void);
void VAIF_SetDACSampleRate(UINT32 SampleRate);
UINT16 VAIF_SetDACSampleRateChangeAccurately(UINT32 RatedSR, UINT32 ActualSRx100000);
UINT16 VAIF_SetDACSampleRateChangeEstimately(UINT32 RatedSR, UINT16 LastR1, VAIF_SRC_TREND Trend);
void VAIF_SetDACSampleRateChangeTWS(UINT32 RatedSR, SINT32 changeRate);
void VAIF_SetADCSampleRate(UINT32 SampleRate);
UINT8 VAIF_GetErrStatus(void);
void VAIF_MaskError(UINT8 ErrToMask);
void VAIF_UnMaskError(UINT8 ErrToUnMask);
void VAIF_ClearError(UINT8 ErrToClr);
void VAIF_EnableErrInt(void);
void VAIF_DisableErrInt(void);
UINT8 VAIF_GetEnStatus(void);
UINT8 VAIF_GetDACEnStatus(void);
UINT8 VAIF_GetADCEnStatus(void);

UINT8 VAIF_GetWSELADC(void);
void VAIF_GainSyncSet (UINT8 mode, UINT32 SampleRate);
UINT8 VAIF_GetDGAIN2MUL(void);
void VAIF_Set_BB_AD_Enable(BOOL enable);
void VAIF_Set_BB_DA_Enable(BOOL enable);

void VAIF_SetGPIO_PDM_mode(void);
void VAIF_PDM_MIC_Enable(BOOL Enable);
void VAIF_PDM_MIC_CLKDIV2_Enable(BOOL Enable);
void VAIF_PDM_MIC_LRCH_ONEIO_Enable(BOOL Enable);

#ifdef __cplusplus
}
#endif 

#endif  /* _VAIF_H_ */

