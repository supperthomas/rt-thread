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
		cdc

Description:
		cdc Device
Revision History:
		Date                  Author                   Description
		2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_cdc.h"
#include "vdrv_aif.h"
#include "vdrv_cdc.h"
#include "vdrv_dma.h"
#include "vdrv_pmu.h"
#include "vreg_def.h"

#define VSYS_CDC_PRINT(...)

static struct
{
	UINT8 data_width;
	UINT8 data_mode;
	UINT8 *buff;
	size_t len;
	VDMA_BUF buf_idx;
	size_t cnt;
	VDRVCDCChannel_t channel;
	UINT8 ctrl_mode;
} g_cdc_info[VSYS_CDC_CH_LIMIT];

VRET_VALUE VSYS_CDC_Init(VDRVCDCSel_t cdc_sel, VDRVCDCConfig_t *cdc_config)
{
	UINT32 reg;
	if (cdc_config == NULL)
		return VRET_ERROR;
	UINT8 dma_mode = (cdc_config->dwidth == VSYS_CDC_WIDTH_BIT16) ? 1 : 0;
	size_t buff_size = (cdc_config->len / 2) & ~0x03;

	VCDC_Init();
	// TODO 		VPMU_SetLDO5En(TRUE);
	// TODO 		VPMU_SetDCDC_CodecOn(2); // for 7621 set DCDC Vol
	VCDC_WriteReg(0x0D, 0x01); // audio dcdc on
	VCDC_WriteReg(0x02, 0x51); // LDO5 Enable
	VAIF_Initial(AIF_CODEC_INTERNAL);
	switch (cdc_sel)
	{
	case VSYS_CDC_CH0IN:
		VAIF_SetADCPath(AIF_ADC_FIFO);
		VAIF_SetADCSampleRate(cdc_config->SR);
		if (cdc_config->channel == VSYS_CDC_LEFT_CHANNEL_ENABLE)
		{
			VCDC_ConfigInput(CDC_INPUT_MICL);
			VDMA_AIF_ReadEnable(VDMA_AIF_LEFT_CHANNEL_ENABLE);
			VDMA_AIF_ConfigReadPath(VDMA_BUF_0, (UINT32)NULL, (UINT32)cdc_config->buff, buff_size, FALSE);
			VDMA_AIF_ConfigReadPath(VDMA_BUF_1, (UINT32)NULL, (UINT32)cdc_config->buff + buff_size, buff_size, FALSE);
		}
		else if (cdc_config->channel == VSYS_CDC_RIGHT_CHANNEL_ENABLE)
		{
			VCDC_ConfigInput(CDC_INPUT_MICR);
			VDMA_AIF_ReadEnable(VDMA_AIF_RIGHT_CHANNEL_ENABLE);
			VDMA_AIF_ConfigReadPath(VDMA_BUF_0, (UINT32)cdc_config->buff, (UINT32)NULL, buff_size, FALSE);
			VDMA_AIF_ConfigReadPath(VDMA_BUF_1, (UINT32)cdc_config->buff + buff_size, (UINT32)NULL, buff_size, FALSE);
		}
		else
		{
			VCDC_ConfigInput(CDC_INPUT_DUAL_MIC);
			VDMA_AIF_ReadEnable(VDMA_AIF_BOTH_CHANNEL_ENABLE);
			buff_size = (buff_size / 2) & ~0x03;
			VDMA_AIF_ConfigReadPath(VDMA_BUF_0, (UINT32)cdc_config->buff + buff_size, (UINT32)cdc_config->buff, buff_size, FALSE);
			VDMA_AIF_ConfigReadPath(VDMA_BUF_1, (UINT32)cdc_config->buff + buff_size * 3, (UINT32)cdc_config->buff + buff_size * 2, buff_size, FALSE);
			buff_size *= 2;
		}
		VCDC_UnmuteInput();
		VAIF_ResetADCFIFO();
		VDMA_SetSram2DevPriority(VDMA_DEV_PRIO_AIF_SD_USB_LCD);
		VDMA_ClearInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_UnmaskInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_AIF2SRAM, TRUE, (VDMA_ISR_CALLBACK)cdc_config->callback);
		break;
	case VSYS_CDC_CH0OUT:
		VAIF_SetDACPath(AIF_DAC_FIFO);
		VAIF_SetDACSampleRate(cdc_config->SR);
		if (cdc_config->channel == VSYS_CDC_LEFT_CHANNEL_ENABLE)
		{
			VAIF_EnableDAC(AIF_DAC_L);
			VCDC_ConfigOutput(CDC_OUTPUT_HPL, cdc_config->ctrl_mode);
			VAIF_DisableDAC(AIF_DAC_L);
			VDMA_AIF_WriteEnable(VDMA_AIF_LEFT_CHANNEL_ENABLE);
		}
		else if (cdc_config->channel == VSYS_CDC_RIGHT_CHANNEL_ENABLE)
		{
			VAIF_EnableDAC(AIF_DAC_R);
			VCDC_ConfigOutput(CDC_OUTPUT_HPR, cdc_config->ctrl_mode);
			VAIF_DisableDAC(AIF_DAC_R);
			VDMA_AIF_WriteEnable(VDMA_AIF_RIGHT_CHANNEL_ENABLE);
		}
		else
		{
			VAIF_EnableDAC(AIF_DAC_L | AIF_DAC_R);
			VCDC_ConfigOutput(CDC_OUTPUT_HP, cdc_config->ctrl_mode);
			VAIF_DisableDAC(AIF_DAC_L | AIF_DAC_R);
			VDMA_AIF_WriteEnable(VDMA_AIF_BOTH_CHANNEL_ENABLE);
		}
		VDMA_AIF_ConfigWritePath(VDMA_BUF_0, (UINT32)cdc_config->buff, buff_size, FALSE);
		VDMA_AIF_ConfigWritePath(VDMA_BUF_1, (UINT32)cdc_config->buff + buff_size, buff_size, FALSE);
		VDMA_SetSram2DevPriority(VDMA_DEV_PRIO_AIF_SD_USB_LCD);
		VDMA_ClearInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_UnmaskInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_SRAM2AIF, TRUE, (VDMA_ISR_CALLBACK)cdc_config->callback);
		break;
	default:
		return VRET_ERROR;
	}
	g_cdc_info[cdc_sel].buff = cdc_config->buff;
	g_cdc_info[cdc_sel].len = buff_size;
	g_cdc_info[cdc_sel].data_mode = cdc_config->data_mode;
	g_cdc_info[cdc_sel].data_width = dma_mode ? 16 : 32;
	g_cdc_info[cdc_sel].channel = cdc_config->channel;
	g_cdc_info[cdc_sel].ctrl_mode = cdc_config->ctrl_mode;
	VDMA_EnableInt();
	return VRET_NO_ERROR;
}

void *VSYS_CDC_Uninit(VDRVCDCSel_t cdc_sel)
{
	UINT32 reg;
	switch (cdc_sel)
	{
	case VSYS_CDC_CH0IN:
		VDMA_MaskInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_ClearInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_AIF_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_AIF2SRAM, TRUE, NULL);
		VDMA_AIF_ConfigReadPath(VDMA_BUF_0, 0, 0, 0, FALSE);
		VDMA_AIF_ConfigReadPath(VDMA_BUF_1, 0, 0, 0, FALSE);
		VCDC_ConfigInput(CDC_INPUT_NONE); //?  why not close adc before
		/* disalbe LDO5 for CODEC */
		// TODO VPMU_SetLDO5En(FALSE);
		break;
	case VSYS_CDC_CH0OUT:
		VDMA_MaskInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_ClearInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_SRAM_AIF_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_SRAM2AIF, TRUE, NULL);
		VDMA_AIF_ConfigWritePath(VDMA_BUF_0, 0, 0, FALSE);
		VDMA_AIF_ConfigWritePath(VDMA_BUF_1, 0, 0, FALSE);
		VCDC_ConfigOutput(CDC_OUTPUT_NONE, g_cdc_info[cdc_sel].ctrl_mode);
		// TODO VPMU_SetLDO5En(FALSE);
		break;
	default:
		return NULL;
	}
	g_cdc_info[cdc_sel].len = 0;
	return g_cdc_info[cdc_sel].buff;
}

void VSYS_CDC_Enable(VDRVCDCSel_t cdc_sel, BOOL en)
{
	BOOL cdc_en;
	VDMA_ACTION action;
	if (en)
	{
		cdc_en = TRUE;
		action = VDMA_ACTION_START;
		g_cdc_info[cdc_sel].buf_idx = VDMA_BUF_1;
		g_cdc_info[cdc_sel].cnt = g_cdc_info[cdc_sel].len;
	}
	else
	{
		cdc_en = FALSE;
		action = VDMA_ACTION_ABORT;
	}
	switch (cdc_sel)
	{
	case VSYS_CDC_CH0IN:
		VDMA_AIF_ReadAction(action);
		if (cdc_en)
		{
			if (g_cdc_info[cdc_sel].channel == VSYS_CDC_LEFT_CHANNEL_ENABLE)
				VAIF_EnableADC(AIF_ADC_L);
			else if (g_cdc_info[cdc_sel].channel == VSYS_CDC_RIGHT_CHANNEL_ENABLE)
				VAIF_EnableADC(AIF_ADC_R);
			else
				VAIF_EnableADC(AIF_ADC_L | AIF_ADC_R);
		}
		else
		{
			VAIF_DisableADC(AIF_ADC_L | AIF_ADC_R);
		}
		break;
	case VSYS_CDC_CH0OUT:
		VDMA_AIF_WriteAction(action);
		if (cdc_en)
		{
			if (g_cdc_info[cdc_sel].channel == VSYS_CDC_LEFT_CHANNEL_ENABLE)
				VAIF_EnableDAC(AIF_DAC_L);
			else if (g_cdc_info[cdc_sel].channel == VSYS_CDC_RIGHT_CHANNEL_ENABLE)
				VAIF_EnableDAC(AIF_DAC_R);
			else
				VAIF_EnableDAC(AIF_DAC_L | AIF_DAC_R);
		}
		else
		{
			VAIF_DisableDAC(AIF_DAC_L | AIF_DAC_R);
		}
		break;

	default:
		return;
	}
}

size_t VSYS_CDC_Update(VDRVCDCSel_t cdc_sel, UINT8 *data, size_t size)
{
	VDMA_BUF buf_idx = VDMA_BUF_NONE;
	size_t lr_size = size;
	UINT8 rd_or_wr = 0;
	UINT8 *buff, *rbuff;

	switch (cdc_sel)
	{
	case VSYS_CDC_CH0IN:
		buf_idx = VDMA_AIF_GetReadActiveBuffer();
		rd_or_wr = 1;
		break;
	case VSYS_CDC_CH0OUT:
		buf_idx = VDMA_AIF_GetWriteActiveBuffer();
		break;
	default:
		return 0;
	}
	if(buf_idx == VDMA_BUF_NONE)
		return 0;
	buf_idx = buf_idx == VDMA_BUF_0?VDMA_BUF_1:VDMA_BUF_0;
	if (buf_idx != g_cdc_info[cdc_sel].buf_idx)
	{
		lr_size = MIN(lr_size, g_cdc_info[cdc_sel].len);
		g_cdc_info[cdc_sel].buf_idx = buf_idx;
		g_cdc_info[cdc_sel].cnt = 0;
	}
	else if (g_cdc_info[cdc_sel].cnt < g_cdc_info[cdc_sel].len)
	{
		lr_size = MIN(lr_size, g_cdc_info[cdc_sel].len - g_cdc_info[cdc_sel].cnt);
	}
	else
	{
		return 0;
	}
	if (buf_idx == VDMA_BUF_1)
	{
		buff = g_cdc_info[cdc_sel].buff + g_cdc_info[cdc_sel].len + g_cdc_info[cdc_sel].cnt;
		rbuff = g_cdc_info[cdc_sel].buff + g_cdc_info[cdc_sel].len / 2 * 3 + g_cdc_info[cdc_sel].cnt;
	}
	else
	{
		buff = g_cdc_info[cdc_sel].buff + g_cdc_info[cdc_sel].cnt;
		rbuff = g_cdc_info[cdc_sel].buff + g_cdc_info[cdc_sel].len / 2 + g_cdc_info[cdc_sel].cnt;
	}
	if (rd_or_wr)
	{
		if (g_cdc_info[cdc_sel].channel != VSYS_CDC_BOTH_CHANNEL_ENABLE)
		{
			vmemcpy(data, buff, lr_size);
		}
		else if (g_cdc_info[cdc_sel].data_mode == VSYS_CDC_DataMode_CROSS)
		{
			if (g_cdc_info[cdc_sel].data_width == 16)
			{
				UINT16 *data_p = (UINT16 *)data, *buff_p = (UINT16 *)buff, *rbuff_p = (UINT16 *)rbuff;
				for (size_t i = 0; i < lr_size / 4; i++)
				{
					*data_p = *buff_p;
					data_p++;
					*data_p = *rbuff_p;
					data_p++;
					buff_p++;
					rbuff_p++;
				}
			}
			else
			{
				UINT32 *data_p = (UINT32 *)data, *buff_p = (UINT32 *)buff, *rbuff_p = (UINT32 *)rbuff;
				for (size_t i = 0; i < lr_size / 8; i++)
				{
					*data_p = *buff_p;
					data_p++;
					*data_p = *rbuff_p;
					data_p++;
					buff_p++;
					rbuff_p++;
				}
			}
		}
		else
		{
			vmemcpy(data, buff, lr_size / 2);
			vmemcpy(data + lr_size / 2, rbuff, lr_size / 2);
		}
	}
	else
	{
		if (g_cdc_info[cdc_sel].channel != VSYS_CDC_BOTH_CHANNEL_ENABLE)
		{
			vmemcpy(buff, data, lr_size);
		}
		else if (g_cdc_info[cdc_sel].data_mode == VSYS_CDC_DataMode_HEADTAIL)
		{
			if (g_cdc_info[cdc_sel].data_width == 16)
			{
				UINT16 *data_p = (UINT16 *)data, *buff_p = (UINT16 *)buff;
				for (size_t i = 0; i < lr_size / 4; i++)
				{
					*buff_p = *data_p;
					buff_p++;
					*buff_p = *(data_p + lr_size / 4);
					buff_p++;
					data_p++;
				}
			}
			else
			{
				UINT32 *data_p = (UINT32 *)data, *buff_p = (UINT32 *)buff;
				for (size_t i = 0; i < lr_size / 8; i++)
				{
					*buff_p = *data_p;
					buff_p++;
					*buff_p = *(data_p + lr_size / 8);
					buff_p++;
					data_p++;
				}
			}
		}
		else
		{
			vmemcpy(buff, data, lr_size);
		}
	}
	g_cdc_info[cdc_sel].cnt += lr_size;
	return lr_size;
}

UINT32 VSYS_CDC_GetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info)
{
	return 0;
}

VRET_VALUE VSYS_CDC_SetParam(VDRVCDCSel_t cdc_sel, VDRVCDCInfo_t info, UINT32 val)
{
	switch (cdc_sel)
	{
	case VSYS_CDC_CH0IN:
		switch (info)
		{
		case VSYS_CDC_Gain:
			VCDC_SetInputGain(val);
			break;
		default:
			return VRET_ERROR;
		}
		break;
	case VSYS_CDC_CH0OUT:
		switch (info)
		{
		case VSYS_CDC_Gain:
			VCDC_SetOutputGain(val);
			break;
		default:
			return VRET_ERROR;
		}
		break;
	default:
		return VRET_ERROR;
	}
	return VRET_NO_ERROR;
}
