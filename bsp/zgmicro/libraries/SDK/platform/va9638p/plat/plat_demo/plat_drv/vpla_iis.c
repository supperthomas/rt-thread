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
	iis

Description:
	iis Driver
Revision History:
	Date                  Author                   Description
	2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_iis.h"
#include "vdrv_gpio.h"
#include "vdrv_iis.h"
#include "vdrv_dma.h"
#include "vreg_def.h"

static struct
{
	UINT8 data_width;
	UINT8 data_mode;
	UINT8 *buff;
	size_t len;
	VDMA_BUF buf_idx;
	size_t cnt;
	BOOL keep_clk;
	BOOL sync_en;
	VDRVIISChannel_t channel;
} g_iis_info[VSYS_IIS_CH_LIMIT];

VRET_VALUE VSYS_IIS_Init(VDRVIISSel_t iis_sel, VDRVIISConfig_t *iis_config)
{
	if (iis_config == NULL)
		return VRET_ERROR;
	UINT8 dma_mode = (iis_config->dwidth == VSYS_IIS_WIDTH_BIT16) ? 1 : 0;
	size_t buff_size = (iis_config->len / 2) & ~0x03;
	switch (iis_sel)
	{
	case VSYS_IIS_CH0IN:
		VI2S_Init(I2S_CH0, iis_config->mode, iis_config->dwidth, iis_config->SR, FALSE, iis_config->format);
		VDMA_ClearInt_1(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_ClearErrInt_1(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_UnmaskInt_1(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback_1(VDMA_DEV1_I2S02SRAM, TRUE, (VDMA_ISR_CALLBACK)iis_config->callback);
		VI2S0_DMA_ReadConfig(I2S_MEM_CM, dma_mode);
		if (iis_config->dwidth == VSYS_IIS_WIDTH_BIT32)
			SET_REG_BIT2(REG_DMA_I2S0_RD_CTRL,7);
		else
			CLR_REG_BIT2(REG_DMA_I2S0_RD_CTRL,7);
		VI2S0_DMA_ReadEnable(iis_config->channel);
		if (iis_config->channel == VSYS_IIS_LEFT_CHANNEL_ENABLE)
		{
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)NULL, (UINT32)iis_config->buff, buff_size, FALSE);
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)NULL, (UINT32)iis_config->buff + buff_size, buff_size, FALSE);
		}
		else if (iis_config->channel == VSYS_IIS_RIGHT_CHANNEL_ENABLE)
		{
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)iis_config->buff, (UINT32)NULL, buff_size, FALSE);
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size, (UINT32)NULL, buff_size, FALSE);
		}
		else
		{
			buff_size = (buff_size / 2) & ~0x03;
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)iis_config->buff + buff_size, (UINT32)iis_config->buff, buff_size, FALSE);
			VI2S0_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size * 3, (UINT32)iis_config->buff + buff_size * 2, buff_size, FALSE);
			buff_size *= 2;
		}
		break;
	case VSYS_IIS_CH0OUT:
		VI2S_Init(I2S_CH0, iis_config->mode, iis_config->dwidth, iis_config->SR, FALSE, iis_config->format);
		VDMA_ClearInt_1(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_ClearErrInt_1(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_UnmaskInt_1(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_RegisterISRCallback_1(VDMA_DEV1_SRAM2I2S0, TRUE, (VDMA_ISR_CALLBACK)iis_config->callback);
		VI2S0_DMA_WriteConfig(I2S_MEM_CM, dma_mode);
		VI2S0_DMA_WriteEnable(iis_config->channel);
		VI2S0_DMA_ConfigWritePath(VDMA_BUF_0, (UINT32)iis_config->buff, buff_size, FALSE);
		VI2S0_DMA_ConfigWritePath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size, buff_size, FALSE);
		break;
	case VSYS_IIS_CH1IN:
		VI2S_Init(I2S_CH1, iis_config->mode, iis_config->dwidth, iis_config->SR, FALSE, iis_config->format);
		VDMA_ClearInt(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_UnmaskInt(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_I2S2SRAM, TRUE, (VDMA_ISR_CALLBACK)iis_config->callback);
		VI2S1_DMA_ReadConfig(I2S_MEM_CM, dma_mode);
		if (iis_config->dwidth == VSYS_IIS_WIDTH_BIT32)
			SET_REG_BIT2(REG_DMA_I2S1_RD_CTRL,7);
		else
			CLR_REG_BIT2(REG_DMA_I2S1_RD_CTRL,7);
		VI2S1_DMA_ReadEnable(iis_config->channel);
		if (iis_config->channel == VSYS_IIS_LEFT_CHANNEL_ENABLE)
		{
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)NULL, (UINT32)iis_config->buff, buff_size, FALSE);
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)NULL, (UINT32)iis_config->buff + buff_size, buff_size, FALSE);
		}
		else if (iis_config->channel == VSYS_IIS_RIGHT_CHANNEL_ENABLE)
		{
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)iis_config->buff, (UINT32)NULL, buff_size, FALSE);
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size, (UINT32)NULL, buff_size, FALSE);
		}
		else
		{
			buff_size = (buff_size / 2) & ~0x03;
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_0, (UINT32)iis_config->buff + buff_size, (UINT32)iis_config->buff, buff_size, FALSE);
			VI2S1_DMA_ConfigReadPath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size *3, (UINT32)iis_config->buff + buff_size * 2, buff_size, FALSE);
			buff_size *= 2;
		}
		break;
	case VSYS_IIS_CH1OUT:
		VI2S_Init(I2S_CH1, iis_config->mode, iis_config->dwidth, iis_config->SR, FALSE, iis_config->format);
		VDMA_ClearInt(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_ClearErrInt(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_UnmaskInt(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_SRAM2I2S, TRUE, (VDMA_ISR_CALLBACK)iis_config->callback);
		VI2S1_DMA_WriteConfig(I2S_MEM_CM, dma_mode);
		VI2S1_DMA_WriteEnable(iis_config->channel);
		VI2S1_DMA_ConfigWritePath(VDMA_BUF_0, (UINT32)iis_config->buff, buff_size, FALSE);
		VI2S1_DMA_ConfigWritePath(VDMA_BUF_1, (UINT32)iis_config->buff + buff_size, buff_size, FALSE);
		break;
	default:
		return VRET_ERROR;
	}
	g_iis_info[iis_sel].buff = iis_config->buff;
	g_iis_info[iis_sel].len = buff_size;
	g_iis_info[iis_sel].data_mode = iis_config->data_mode;
	g_iis_info[iis_sel].data_width = dma_mode ? 16 : 32;
	g_iis_info[iis_sel].channel = iis_config->channel;
	VDMA_EnableInt();
	return VRET_NO_ERROR;
}

void *VSYS_IIS_Uninit(VDRVIISSel_t iis_sel)
{
	switch (iis_sel)
	{
	case VSYS_IIS_CH0IN:
		VI2S_SetRx(I2S_CH0, FALSE);
		VI2S0_DMA_ReadAction(VDMA_ACTION_ABORT);
		VI2S0_DMA_ReadEnable(VDMA_I2S_BOTH_CHANNEL_DISABLE);
		VDMA_MaskInt_1(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback_1(VDMA_DEV1_I2S02SRAM, TRUE, NULL);
		VI2S0_DMA_ConfigReadPath(VDMA_BUF_0, 0, 0, 0, FALSE);
		VI2S0_DMA_ConfigReadPath(VDMA_BUF_1, 0, 0, 0, FALSE);
		break;
	case VSYS_IIS_CH0OUT:
		VI2S_SetTx(I2S_CH0, FALSE);
		VI2S0_DMA_WriteAction(VDMA_ACTION_ABORT);
		VI2S0_DMA_WriteEnable(VDMA_I2S_BOTH_CHANNEL_DISABLE);
		VDMA_MaskInt_1(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_RegisterISRCallback_1(VDMA_DEV1_SRAM2I2S0, TRUE, NULL);
		VI2S0_DMA_ConfigWritePath(VDMA_BUF_0, 0, 0, FALSE);
		VI2S0_DMA_ConfigWritePath(VDMA_BUF_1, 0, 0, FALSE);
		break;
	case VSYS_IIS_CH1IN:
		VI2S_SetRx(I2S_CH1, FALSE);
		VI2S1_DMA_ReadAction(VDMA_ACTION_ABORT);
		VI2S1_DMA_ReadEnable(VDMA_I2S_BOTH_CHANNEL_DISABLE);
		VDMA_MaskInt(VDMA_INT_MC_I2S_SRAM_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_I2S2SRAM, TRUE, NULL);
		VI2S1_DMA_ConfigReadPath(VDMA_BUF_0, 0, 0, 0, FALSE);
		VI2S1_DMA_ConfigReadPath(VDMA_BUF_1, 0, 0, 0, FALSE);
		break;
	case VSYS_IIS_CH1OUT:
		VI2S_SetTx(I2S_CH1, FALSE);
		VI2S1_DMA_WriteAction(VDMA_ACTION_ABORT);
		VI2S1_DMA_WriteEnable(VDMA_I2S_BOTH_CHANNEL_DISABLE);
		VDMA_MaskInt(VDMA_INT_MC_SRAM_I2S_DONE_MSK);
		VDMA_RegisterISRCallback(VDMA_DEV_SRAM2I2S, TRUE, NULL);
		VI2S1_DMA_ConfigWritePath(VDMA_BUF_0, 0, 0, FALSE);
		VI2S0_DMA_ConfigWritePath(VDMA_BUF_1, 0, 0, FALSE);
		break;
	default:
		return NULL;
	}
	g_iis_info[iis_sel].len = 0;
	return g_iis_info[iis_sel].buff;
}

void VSYS_IIS_Enable(VDRVIISSel_t iis_sel, BOOL en)
{
	BOOL i2s_en;
	VDMA_ACTION action;
	if (en)
	{
		i2s_en = TRUE;
		action = VDMA_ACTION_START;
		g_iis_info[iis_sel].buf_idx = VDMA_BUF_1;
		g_iis_info[iis_sel].cnt = g_iis_info[iis_sel].len;
		if (g_iis_info[iis_sel].sync_en)
			i2s_en = FALSE;
	}
	else
	{
		i2s_en = FALSE;
		action = VDMA_ACTION_ABORT;
	}
	switch (iis_sel)
	{
	case VSYS_IIS_CH0IN:
		VI2S0_DMA_ReadAction(action);
		VI2S_SetRx(I2S_CH0, i2s_en);
		break;
	case VSYS_IIS_CH0OUT:
		VI2S0_DMA_WriteAction(action);
		VI2S_SetTx(I2S_CH0, i2s_en);
		break;
	case VSYS_IIS_CH1IN:
		VI2S1_DMA_ReadAction(action);
		VI2S_SetRx(I2S_CH1, i2s_en);
		break;
	case VSYS_IIS_CH1OUT:
		VI2S1_DMA_WriteAction(action);
		VI2S_SetTx(I2S_CH1, i2s_en);
		break;
	default:
		return;
	}
}

size_t VSYS_IIS_Update(VDRVIISSel_t iis_sel, UINT8 *data, size_t size)
{
	VDMA_BUF buf_idx = VDMA_BUF_NONE;
	size_t lr_size = size;
	UINT8 rd_or_wr = 0;
	UINT8 *buff, *rbuff;

	switch (iis_sel)
	{
	case VSYS_IIS_CH0IN:
		buf_idx = VI2S0_DMA_GetReadActiveBuffer();
		rd_or_wr = 1;
		break;
	case VSYS_IIS_CH0OUT:
		buf_idx = VI2S0_DMA_GetWriteActiveBuffer();
		break;
	case VSYS_IIS_CH1IN:
		buf_idx = VI2S1_DMA_GetReadActiveBuffer();
		rd_or_wr = 1;
		break;
	case VSYS_IIS_CH1OUT:
		buf_idx = VI2S1_DMA_GetWriteActiveBuffer();
		break;
	default:
		return 0;
	}
	if(buf_idx == VDMA_BUF_NONE)
		return 0;
	buf_idx = buf_idx == VDMA_BUF_0?VDMA_BUF_1:VDMA_BUF_0;
	if (buf_idx != g_iis_info[iis_sel].buf_idx)
	{
		lr_size = MIN(lr_size, g_iis_info[iis_sel].len);
		g_iis_info[iis_sel].buf_idx = buf_idx;
		g_iis_info[iis_sel].cnt = 0;
	}
	else if (g_iis_info[iis_sel].cnt < g_iis_info[iis_sel].len)
	{
		lr_size = MIN(lr_size, g_iis_info[iis_sel].len - g_iis_info[iis_sel].cnt);
	}
	else
	{
		return 0;
	}
	if (buf_idx == VDMA_BUF_1)
	{
		buff = g_iis_info[iis_sel].buff + g_iis_info[iis_sel].len + g_iis_info[iis_sel].cnt;
		rbuff = g_iis_info[iis_sel].buff + g_iis_info[iis_sel].len / 2 * 3 + g_iis_info[iis_sel].cnt;
	}
	else
	{
		buff = g_iis_info[iis_sel].buff + g_iis_info[iis_sel].cnt;
		rbuff = g_iis_info[iis_sel].buff + g_iis_info[iis_sel].len / 2 + g_iis_info[iis_sel].cnt;
	}
	if (rd_or_wr)
	{
		if (g_iis_info[iis_sel].channel != VSYS_IIS_BOTH_CHANNEL_ENABLE)
		{
			vmemcpy(data, buff, lr_size);
		}
		else if (g_iis_info[iis_sel].data_mode == VSYS_IIS_DataMode_CROSS)
		{
			if (g_iis_info[iis_sel].data_width == 16)
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
			vmemcpy(data, buff, lr_size/2);
			vmemcpy(data + lr_size/2, rbuff, lr_size/2);
		}
	}
	else
	{
		if (g_iis_info[iis_sel].channel != VSYS_IIS_BOTH_CHANNEL_ENABLE)
		{
			vmemcpy(buff, data, lr_size);
		}
		else if (g_iis_info[iis_sel].data_mode == VSYS_IIS_DataMode_HEADTAIL)
		{
			if (g_iis_info[iis_sel].data_width == 16)
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
	g_iis_info[iis_sel].cnt += lr_size;
	return lr_size;
}


UINT32 VSYS_IIS_GetParam(VDRVIISSel_t iis_sel, VDRVIISInfo_t info)
{
	UINT32 val = 0;
	switch (info)
	{
	case VSYS_IISState:
		switch (iis_sel)
		{
		case VSYS_IIS_CH0IN:
			val = VI2S_GetRx(I2S_CH0);
			break;
		case VSYS_IIS_CH0OUT:
			val = VI2S_GetTx(I2S_CH0);
			break;
		case VSYS_IIS_CH1IN:
			val = VI2S_GetRx(I2S_CH1);
			break;
		case VSYS_IIS_CH1OUT:
			val = VI2S_GetTx(I2S_CH1);
			break;
		default:
			break;
		}
		break;
	case VSYS_IISFreeCache:
		{
		VDMA_BUF buf_idx = VDMA_BUF_NONE;
		switch (iis_sel)
		{
			case VSYS_IIS_CH0IN:
				buf_idx = VI2S0_DMA_GetReadActiveBuffer();
				break;
			case VSYS_IIS_CH0OUT:
				buf_idx = VI2S0_DMA_GetWriteActiveBuffer();
				break;
			case VSYS_IIS_CH1IN:
				buf_idx = VI2S1_DMA_GetReadActiveBuffer();
				break;
			case VSYS_IIS_CH1OUT:
				buf_idx = VI2S1_DMA_GetWriteActiveBuffer();
				break;
		}
		if(buf_idx != VDMA_BUF_NONE)
		{
			buf_idx = buf_idx == VDMA_BUF_0?VDMA_BUF_1:VDMA_BUF_0;
			if (buf_idx != g_iis_info[iis_sel].buf_idx)
				val = g_iis_info[iis_sel].len;
			else if (g_iis_info[iis_sel].cnt < g_iis_info[iis_sel].len)
				val = g_iis_info[iis_sel].len - g_iis_info[iis_sel].cnt;
		}
		}
		break;
	default:
		break;
	}
	return val;
}

VRET_VALUE VSYS_IIS_SetParam(VDRVIISSel_t iis_sel, VDRVIISInfo_t info, UINT32 val)
{
	if (iis_sel >= VSYS_IIS_CH_LIMIT)
		return VRET_ERROR;
	switch (info)
	{
	case VSYS_IISKeepClk:
		g_iis_info[iis_sel].keep_clk = (BOOL)val;
		break;
	case VSYS_IISSyncSet:
		g_iis_info[iis_sel].sync_en = (BOOL)val;
		break;
	case VSYS_IISSyncEn:
	{
		UINT32 bitval = 0;
		if(g_iis_info[VSYS_IIS_CH0IN].sync_en && g_iis_info[VSYS_IIS_CH0IN].len)
			bitval |= 1<<1;
		if(g_iis_info[VSYS_IIS_CH0OUT].sync_en && g_iis_info[VSYS_IIS_CH0OUT].len)
			bitval |= 1<<0;
		if(g_iis_info[VSYS_IIS_CH1IN].sync_en && g_iis_info[VSYS_IIS_CH1IN].len)
			bitval |= 1<<3;
		if(g_iis_info[VSYS_IIS_CH1OUT].sync_en && g_iis_info[VSYS_IIS_CH1OUT].len)
			bitval |= 1<<2;
		VI2S_Sync_SetTRx(bitval);
		break;
	}
	default:
		break;
	}
	return VRET_NO_ERROR;
}

