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
		firmware

Description:
		firmware
Revision History:
		Date                  Author                   Description
	2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstddrv.h"
#include "vpla_fmw.h"

#undef VPLA_FMW_DEBUG
#define VPLA_FMW_DEBUG(...) DEBUG_PRINTF("[VPLA_FMW]", __VA_ARGS__)

static UINT8 g_VSYS_FMW_Global_Info_num = 0;
static VSYS_FMW_GLOBAL_INFO g_VSYS_FMW_Global_Info = {0};

UINT8 VPLA_FMW_Init(VSYS_FMW_GLOBAL_INFO *info)
{
	UINT16 crc_val;
	UINT32 pool_addr[2] = {0, 0x40000};
	VSYS_FMW_GLOBAL_INFO *img_info = &g_VSYS_FMW_Global_Info;

	for (UINT8 i = 0, j; g_VSYS_FMW_Global_Info_num == 0 && i < ARRAY_SIZE(pool_addr); i++)
	{
		if (VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, pool_addr[i], (UINT8*)img_info, sizeof(VSYS_FMW_GLOBAL_INFO)) != VRET_NO_ERROR)
		{
			VPLA_FMW_DEBUG("read global info%d failed!\r\n", i);
			continue;
		}
		crc_val = VCOMMON_CRC16((UINT8 *)&img_info->header, GET_CRC_OFFSET(VSYS_FMW_HEADER));
		if ((crc_val == img_info->header.crc) && (img_info->header.crc != 0))
		{
			for (j = 0; j < VSYS_FMW_PARTITION_MAX; j++)
			{
				crc_val = VCOMMON_CRC16((UINT8 *)(img_info->partition + j), GET_CRC_OFFSET(VSYS_FMW_PARTITION_INFO));
				if (crc_val != (img_info->partition + j)->crc && (img_info->partition + j)->crc != 0)
				{
					VPLA_FMW_DEBUG("global info%d Firmware partition info.crc check fail!\r\n", i);
					break;
				}
				if (img_info->partition[j].address % (UINT32)((img_info->flash_info.sector_size) << 10))
				{
					VPLA_FMW_DEBUG("global info%d Firmware partition info.address check fail!\r\n", i);
					break;
				}
			}
			if (j == VSYS_FMW_PARTITION_MAX)
			{
				g_VSYS_FMW_Global_Info_num = i + 1;
				break;
			}
		}
		else
			VPLA_FMW_DEBUG("global info%d Firmware file check fail %04x!", i, crc_val);
	}

	if (g_VSYS_FMW_Global_Info_num != 0 && info != NULL)
	{
		vmemcpy(info, img_info, sizeof(VSYS_FMW_GLOBAL_INFO));
	}
	if (g_VSYS_FMW_Global_Info_num == 0)
	{
		vmemset(img_info, 0, sizeof(VSYS_FMW_GLOBAL_INFO));
	}

	return g_VSYS_FMW_Global_Info_num;
}

UINT8 VPLA_FMW_DspCfg(VDRVDSPCfg_t *dsp_cfg)
{
	VPLA_FMW_Init(NULL);
	if (g_VSYS_FMW_Global_Info_num > 0)
	{
		dsp_cfg->start_addr = g_VSYS_FMW_Global_Info.dsp.address;
		VSYS_FMW_DSP_INFO vfmw_dsp_info;
		if (VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, dsp_cfg->start_addr, (UINT8 *)&vfmw_dsp_info, sizeof(VSYS_FMW_DSP_INFO)) == VRET_NO_ERROR)
		{
			dsp_cfg->xmem_addr = vfmw_dsp_info.x.address + dsp_cfg->start_addr;
			dsp_cfg->xmem_size = vfmw_dsp_info.x.length;
			dsp_cfg->ymem_addr = vfmw_dsp_info.y.address + dsp_cfg->start_addr;
			dsp_cfg->ymem_size = vfmw_dsp_info.y.length;
			dsp_cfg->pmem_addr = vfmw_dsp_info.p.address + dsp_cfg->start_addr;
			dsp_cfg->pmem_size = vfmw_dsp_info.p.length;
			dsp_cfg->dspkey = 0;
			dsp_cfg->datakey = 0;
			return 0;
		}
	}
	return 1;
}
