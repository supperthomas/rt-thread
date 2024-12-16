/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    dsp

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vstdcore.h"
#include "vpla_flash.h"
#include "vpla_dsp.h"
#include "vdrv_dma.h"
#include "vdrv_mbox.h"
#include "vdebug_errno.h"

void VDSP_DownloadPM(UINT32 *source, UINT32 *dest, UINT32 length)
{
    if (length > 0)
    {
        VDMA_DSPPM_ConfigWritePath((UINT32)dest>>2, (UINT32)source, length);
        VDMA_DSPPM_WriteEnable(TRUE);
        VDMA_DSPPM_WriteAction(VDMA_ACTION_START);
    }
}

void VSYS_DSP_WriteDM(UINT8 XYMEM, UINT32 *source, UINT32 *dest, UINT32 length)
{
    if (length > 0)
    {
#if (CODEC_DSP_HALT_LOCK_EN == 1)
        CODEC_DSP_HoldLock(TRUE);
#endif
        VDMA_DSPDM_SetWriteBitmode(CODEC_DSP_BITMODE_LOW24BIT);
        VDMA_DSPDM_ConfigWritePath((((XYMEM & 1) << 22) | ((UINT32)dest>>2 & 0x3FFFFF)), (UINT32)source, length);
        VDMA_DSPDM_WriteEnable(TRUE);
#if (CODEC_DSP_HALT_LOCK_EN == 1)
        VCODEC_DSP_WakeupDsp();
#endif
        VDMA_DSPDM_WriteAction(VDMA_ACTION_START);
    }
}

void VSYS_DSP_ReadDM(UINT8 XYMEM, UINT32 *source, UINT32 *dest, UINT32 length)
{
    if (length > 0)
    {
        VDMA_DSPDM_SetReadBitmode(CODEC_DSP_BITMODE_LOW24BIT);
        VDMA_DSPDM_ConfigReadPath((UINT32)source, (((XYMEM & 1) << 22) | ((UINT32)dest>>2 & 0x3FFFFF)), length);
        VDMA_DSPDM_ReadEnable(TRUE);
        VDMA_DSPDM_ReadAction(VDMA_ACTION_START);
    }
}

VRET_VALUE VSYS_DSP_Init(VDRVDSPCfg_t *cfg)
{
    UINT8 *pData;
    UINT32 size = 512;
    UINT32 i, src_addr, src_len, dst_addr;
    UINT16 key[2] = {0};
    VRET_VALUE ret;

    pData = vmalloc(size);
    if (!pData)
        return VRET_ERROR;

    VDMA_RegisterISRCallback(VDMA_DEV_DSPDM2SRAM, TRUE, (VDMA_ISR_CALLBACK)VDMA_DSPDM2SRAM_IntCallBack);
    VDMA_RegisterISRCallback(VDMA_DEV_SRAM2DSPDM, TRUE, (VDMA_ISR_CALLBACK)VDMA_SRAM2DSPDM_IntCallBack);
    VDMA_RegisterISRCallback(VDMA_DEV_SRAM2DSPPM, TRUE, (VDMA_ISR_CALLBACK)VDMA_SRAM2DSPPM_IntCallBack);

    VDMA_UnmaskInt(VDMA_INT_MC_DSPDM_SRAM_DONE_MSK);
    VDMA_UnmaskInt(VDMA_INT_MC_SRAM_DSPDM_DONE_MSK);
    VDMA_UnmaskInt(VDMA_INT_MC_SRAM_DSPPM_DONE_MSK);

    VDMA_MaskErrInt(VDMA_INT_MC_DSPDM_SRAM_DONE_MSK);
    VDMA_MaskErrInt(VDMA_INT_MC_SRAM_DSPDM_DONE_MSK);
    VDMA_MaskErrInt(VDMA_INT_MC_SRAM_DSPPM_DONE_MSK);
	
	VDMA_EnableInt();

    VSYS_CKM_SetParam(VSYS_DspClockFreq, cfg->DSPFreqClk);
    VSYS_CKM_SetParam(VSYS_DSPClockEnable, TRUE);

    VMBOX_DSP_ClkEnable(TRUE);
    VMBOX_DSP_Reset(DSP_RESET_MODE_CORE);

    VMBOX_DSP_SetCacheUpload();
    VSYS_FLASH_SetParam(VSYS_FLASH_INPACKAGE, VSYS_FLASH_DSPBASE, (UINT8 *)&cfg->pmem_addr, 4);
    key[0] = cfg->dspkey;
    key[1] = cfg->datakey;
    VSYS_FLASH_SetParam(VSYS_FLASH_INPACKAGE, VSYS_FLASH_ENCRYPTKEY2, (UINT8 *)&key, sizeof(key));

    VMBOX_EnableInt();

    /* download P Memory */
    dst_addr = 0;
    src_addr = cfg->pmem_addr;
    src_len = cfg->pmem_size > VSYS_DSP_PMEM_CACHE_SIZE ? VSYS_DSP_PMEM_CACHE_SIZE : cfg->pmem_size;
    for (i = 0; i < src_len; i += size)
    {
        VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, src_addr, pData, size);
        VDSP_DownloadPM((UINT32 *)pData, (UINT32 *)(dst_addr), size);
        dst_addr += size;
        src_addr += size;
    }

    /* download X Memory */
    dst_addr = 0;
    src_addr = cfg->xmem_addr;
    src_len = cfg->xmem_size;
    for (i = 0; i < src_len; i += size)
    {
        VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, src_addr, pData, size);
        VSYS_DSP_WriteDM(CODEC_DSP_XMEM, (UINT32 *)pData, (UINT32 *)(dst_addr), size);
        dst_addr += size;
        src_addr += size;
    }

    /* download Y Memory */
    dst_addr = 0;
    src_addr = cfg->ymem_addr;
    src_len = cfg->ymem_size;
    for (i = 0; i < src_len; i += size)
    {
        VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE, src_addr, pData, size);
        VSYS_DSP_WriteDM(CODEC_DSP_YMEM, (UINT32 *)pData, (UINT32 *)(dst_addr), size);
        dst_addr += size;
        src_addr += size;
    }

    vfree(pData);
    key[0] = 0;
    key[1] = 0;
    VSYS_FLASH_SetParam(VSYS_FLASH_INPACKAGE, VSYS_FLASH_ENCRYPTKEY2, (UINT8 *)&key, sizeof(key));
    key[0] = cfg->dspkey;
    key[1] = 0;
    VSYS_FLASH_SetParam(VSYS_FLASH_INPACKAGE, VSYS_FLASH_ENCRYPTKEY2, (UINT8 *)&key, sizeof(key));
    VMBOX_DSP_ClrCacheUpload();

    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_DSP_Uninit(void)
{
    VSYS_CKM_SetParam(VSYS_DSPClockEnable, FALSE);
    VMBOX_DSP_ClkEnable(FALSE);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_DSP_Run(void)
{
    VMBOX_DSP_Enable();
    return VRET_NO_ERROR;
}

UINT32 VSYS_DSP_GetStatus(VDRVDSPStatus_t status)
{
    UINT32 ret = 0;

    switch (status)
    {
    case VSYS_DSP_IOHOLDUSER:
        ret = VMBOX_DSP_GetState() & (1 << VMBOX_REG_DSP_STAT_IO_HOLD_USER) ? 1 : 0;
        break;
    case VSYS_DSP_HALTACK:
        ret = VMBOX_DSP_GetState() & (1 << VMBOX_REG_DSP_STAT_HALT_ACK) ? 1 : 0;
        break;
    case VSYS_DSP_SELFINTSTAT:
        ret = VMBOX_DSP_GetIntStat(0);
        break;
    default:
        break;
    }
    return ret;
}

VRET_VALUE VSYS_DSP_Control(VDRVDSPControl_t control, /*VDRVDSPParam_t*/ UINT32 val)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    switch (control)
    {
    case VSYS_DSP_HOLD:
        if (val == VSYS_DSP_HOLD_NONE)
        {
            VMBOX_DSP_Unhold();
        }
        else
        {
            VMBOX_DSP_Hold((UINT8)val);
        }
        break;
    case VSYS_DSP_RESET:
        if (val & VSYS_DSP_RESET_DSS)
        {
            VMBOX_DSP_Reset(DSP_RESET_MODE_WHOLE);
        }
        if (val & VSYS_DSP_RESET_DSP)
        {
            VMBOX_DSP_Reset(DSP_RESET_MODE_CORE);
        }
        if (val & VSYS_DSP_RESET_RESTART)
        {
            VMBOX_DSP_Restart();
        }
        break;
    case VSYS_DSP_SETINT:
        VMBOX_DSP_SetInterrupt(0);
        break;
    default:
        __VERRNO_STORE(VERRNO_DSP_INVALID);
        ret = VRET_ERROR;
        break;
    }

    return ret;
}
