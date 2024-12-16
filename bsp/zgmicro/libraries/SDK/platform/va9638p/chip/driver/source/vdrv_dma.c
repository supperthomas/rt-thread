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
    UART

Description:
     UART Driver

Revision History:
     Date                  Author                   Description
     2013.08.12        chenjian            initial version
    2022.09.13        caoxuetao            
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vdrv_dma.h"

#define VDMA_REG_TRF_CTRL_DSP_HOLD_SHFT			29
#define VDMA_REG_TRF_CTRL_DSP_HOLD_MSK			0x20000000
#define VDMA_REG_TRF_CTRL_DSPDM_BM_CODE_SHFT			28
#define VDMA_REG_TRF_CTRL_DSPDM_BM_CODE_MSK			0x10000000
#define VDMA_REG_TRF_CTRL_DSPDM_BM_DATA_SHFT			27
#define VDMA_REG_TRF_CTRL_DSPDM_BM_DATA_MSK			0x08000000
#define VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_SHFT				24
#define VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_MSK				0x07000700
#define VDMA_REG_TRF_CTRL_DEV_PRIORITY_SHFT				8
#define VDMA_REG_TRF_CTRL_DEV_PRIORITY_MSK				0x00000700

#define VDMA_REG_AIF_CNT_AIF_CNT_FLAG_SHFT				31
#define VDMA_REG_AIF_CNT_AIF_CNT_FLAG_MSK				0x80000000
#define VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT				16
#define VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK				0x7FFF0000
#define VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT				0
#define VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK				0x0000FFFF

#define VDMA_REG_AIF_RD_CTRL_AIF_RD_CTLSRC_SHFT			7
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_CTLSRC_MSK			0x00000080
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_SRC_SHFT			6
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_SRC_MSK			0x00000040
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_DST_SHFT			5
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_DST_MSK			0x00000020
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_BITMOD_SHFT			4
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_BITMOD_MSK			0x00000010
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_SHFT				2
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK				0x0000000C
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_ABORT_SHFT			1
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_ABORT_MSK			0x00000002
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_START_SHFT			0
#define VDMA_REG_AIF_RD_CTRL_AIF_RD_START_MSK			0x00000001

#define VDMA_REG_AIF_RD_STAT_AIF_RD_STOP_SHFT			4
#define VDMA_REG_AIF_RD_STAT_AIF_RD_STOP_MSK			0x00000010
#define VDMA_REG_AIF_RD_STAT_AIF_RDINTR_SRC_SHFT		2
#define VDMA_REG_AIF_RD_STAT_AIF_RDINTR_SRC_MSK			0x0000000C
#define VDMA_REG_AIF_RD_STAT_AIF_RD_FE1_SHFT			1
#define VDMA_REG_AIF_RD_STAT_AIF_RD_FE1_MSK				0x00000002
#define VDMA_REG_AIF_RD_STAT_AIF_RD_FE0_SHFT			0
#define VDMA_REG_AIF_RD_STAT_AIF_RD_FE0_MSK				0x00000001

#define VDMA_REG_AIF_RD_RADD0_AIF_RD_RADD0_SHFT			0
#define VDMA_REG_AIF_RD_RADD0_AIF_RD_RADD0_MSK			0x7FFFFFFF

#define VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_SHFT			0
#define VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_MSK			0x7FFFFFFF

#define VDMA_REG_AIF_RD_LADD0_END_BUF0_SHFT				31
#define VDMA_REG_AIF_RD_LADD0_END_BUF0_MSK				0x80000000
#define VDMA_REG_AIF_RD_LADD0_AIF_RD_LADD0_SHFT			0
#define VDMA_REG_AIF_RD_LADD0_AIF_RD_LADD0_MSK			0x7FFFFFFF

#define VDMA_REG_AIF_RD_LADD1_END_BUF1_SHFT				31
#define VDMA_REG_AIF_RD_LADD1_END_BUF1_MSK				0x80000000
#define VDMA_REG_AIF_RD_LADD1_AIF_RD_LADD1_SHFT			0
#define VDMA_REG_AIF_RD_LADD1_AIF_RD_LADD1_MSK			0x7FFFFFFF

#define VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_SHFT			7
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK			0x00000080
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_SRC_SHFT			6
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_SRC_MSK			0x00000040
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_DST_SHFT			5
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_DST_MSK			0x00000020
#define VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_SHFT		4
#define VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK		0x00000010
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_EN_SHFT				2
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_EN_MSK				0x0000000C
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_ABORT_SHFT			1
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_ABORT_MSK			0x00000002
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_START_SHFT			0
#define VDMA_REG_AIF_WR_CTRL_AIF_WR_START_MSK			0x00000001

#define VDMA_REG_AIF_WR_STAT_AIF_WR_STOP_SHFT			4
#define VDMA_REG_AIF_WR_STAT_AIF_WR_STOP_MSK			0x00000010
#define VDMA_REG_AIF_WR_STAT_AIF_WRINTR_SRC_SHFT		2
#define VDMA_REG_AIF_WR_STAT_AIF_WRINTR_SRC_MSK			0x0000000C
#define VDMA_REG_AIF_WR_STAT_AIF_WR_FE1_SHFT			1
#define VDMA_REG_AIF_WR_STAT_AIF_WR_FE1_MSK				0x00000002
#define VDMA_REG_AIF_WR_STAT_AIF_WR_FE0_SHFT			0
#define VDMA_REG_AIF_WR_STAT_AIF_WR_FE0_MSK				0x00000001

#define VDMA_REG_AIF_WR_ADD0_END_BUF0_SHFT				31
#define VDMA_REG_AIF_WR_ADD0_END_BUF0_MSK				0x80000000
#define VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_SHFT			0
#define VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_MSK			0x7FFFFFFF

#define VDMA_REG_AIF_WR_ADD1_END_BUF1_SHFT				31
#define VDMA_REG_AIF_WR_ADD1_END_BUF1_MSK				0x80000000
#define VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_SHFT			0
#define VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_MSK			0x7FFFFFFF

#define VDMA_REG_SPI_RD_CTRL_SPI_RD_DST_SHFT			3
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_DST_MSK				0x00000008
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_EN_SHFT				2
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_EN_MSK				0x00000004
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_ABORT_SHFT			1
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_ABORT_MSK			0x00000002
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_START_SHFT			0
#define VDMA_REG_SPI_RD_CTRL_SPI_RD_START_MSK			0x00000001

#define VDMA_REG_SPI_RD_STAT_SPI_RD_STOP_SHFT			4
#define VDMA_REG_SPI_RD_STAT_SPI_RD_STOP_MSK			0x00000010

#define VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_SHFT				0
#define VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_SPI_WR_CTRL_SPI_WR_EN_SHFT				2
#define VDMA_REG_SPI_WR_CTRL_SPI_WR_EN_MSK				0x00000004
#define VDMA_REG_SPI_WR_CTRL_SPI_WR_ABORT_SHFT			1
#define VDMA_REG_SPI_WR_CTRL_SPI_WR_ABORT_MSK			0x00000002
#define VDMA_REG_SPI_WR_CTRL_SPI_WR_START_SHFT			0
#define VDMA_REG_SPI_WR_CTRL_SPI_WR_START_MSK			0x00000001

#define VDMA_REG_SPI_WR_STAT_SPI_WR_STOP_SHFT			4
#define VDMA_REG_SPI_WR_STAT_SPI_WR_STOP_MSK			0x00000010

#define VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_SHFT				0
#define VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_MSK				0x7FFFFFFF


#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_DST_SHFT			3
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_DST_MSK				0x00000008
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_EN_SHFT				2
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_EN_MSK				0x00000004
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_ABORT_SHFT			1
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_ABORT_MSK			0x00000002
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_START_SHFT			0
#define VDMA_REG_SPI1_RD_CTRL_SPI1_RD_START_MSK			0x00000001

#define VDMA_REG_SPI1_RD_STAT_SPI1_RD_STOP_SHFT			4
#define VDMA_REG_SPI1_RD_STAT_SPI1_RD_STOP_MSK			0x00000010

#define VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_SHFT				0
#define VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_EN_SHFT				2
#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_EN_MSK				0x00000004
#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_ABORT_SHFT			1
#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_ABORT_MSK			0x00000002
#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_START_SHFT			0
#define VDMA_REG_SPI1_WR_CTRL_SPI1_WR_START_MSK			0x00000001

#define VDMA_REG_SPI1_WR_STAT_SPI1_WR_STOP_SHFT			4
#define VDMA_REG_SPI1_WR_STAT_SPI1_WR_STOP_MSK			0x00000010

#define VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_SHFT				0
#define VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_BB_RD_CTRL_BB_RD_EN_SHFT				2
#define VDMA_REG_BB_RD_CTRL_BB_RD_EN_MSK				0x00000004
#define VDMA_REG_BB_RD_CTRL_BB_RD_ABORT_SHFT			1
#define VDMA_REG_BB_RD_CTRL_BB_RD_ABORT_MSK				0x00000002
#define VDMA_REG_BB_RD_CTRL_BB_RD_START_SHFT			0
#define VDMA_REG_BB_RD_CTRL_BB_RD_START_MSK				0x00000001

#define VDMA_REG_BB_RD_STAT_BB_RD_STOP_SHFT				4
#define VDMA_REG_BB_RD_STAT_BB_RD_STOP_MSK				0x00000010

#define VDMA_REG_BB_RD_ADD_BB_RD_ADD_SHFT				0
#define VDMA_REG_BB_RD_ADD_BB_RD_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_BB_WR_CTRL_BB_WR_EN_SHFT				2
#define VDMA_REG_BB_WR_CTRL_BB_WR_EN_MSK				0x00000004
#define VDMA_REG_BB_WR_CTRL_BB_WR_ABORT_SHFT			1
#define VDMA_REG_BB_WR_CTRL_BB_WR_ABORT_MSK				0x00000002
#define VDMA_REG_BB_WR_CTRL_BB_WR_START_SHFT			0
#define VDMA_REG_BB_WR_CTRL_BB_WR_START_MSK				0x00000001

#define VDMA_REG_BB_WR_STAT_BB_WR_STOP_SHFT				4
#define VDMA_REG_BB_WR_STAT_BB_WR_STOP_MSK				0x00000010

#define VDMA_REG_BB_WR_ADD_BB_WR_ADD_SHFT				0
#define VDMA_REG_BB_WR_ADD_BB_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_SD_RD_CTRL_SD_RD_BITMODE_SHFT			4
#define VDMA_REG_SD_RD_CTRL_SD_RD_BITMODE_MSK				0x00000030
#define VDMA_REG_SD_RD_CTRL_SD_RD_DST_SHFT			3
#define VDMA_REG_SD_RD_CTRL_SD_RD_DST_MSK				0x00000008
#define VDMA_REG_SD_RD_CTRL_SD_RD_EN_SHFT				2
#define VDMA_REG_SD_RD_CTRL_SD_RD_EN_MSK				0x00000004
#define VDMA_REG_SD_RD_CTRL_SD_RD_ABORT_SHFT			1
#define VDMA_REG_SD_RD_CTRL_SD_RD_ABORT_MSK			0x00000002
#define VDMA_REG_SD_RD_CTRL_SD_RD_START_SHFT			0
#define VDMA_REG_SD_RD_CTRL_SD_RD_START_MSK			0x00000001

#define VDMA_REG_SD_RD_STAT_SD_RD_STOP_SHFT			4
#define VDMA_REG_SD_RD_STAT_SD_RD_STOP_MSK			0x00000010

#define VDMA_REG_SD_RD_ADD_SD_RD_ADD_SHFT				0
#define VDMA_REG_SD_RD_ADD_SD_RD_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_SD_WR_CTRL_SD_WR_EN_SHFT				2
#define VDMA_REG_SD_WR_CTRL_SD_WR_EN_MSK				0x00000004
#define VDMA_REG_SD_WR_CTRL_SD_WR_ABORT_SHFT			1
#define VDMA_REG_SD_WR_CTRL_SD_WR_ABORT_MSK			0x00000002
#define VDMA_REG_SD_WR_CTRL_SD_WR_START_SHFT			0
#define VDMA_REG_SD_WR_CTRL_SD_WR_START_MSK			0x00000001

#define VDMA_REG_SD_WR_STAT_SD_WR_STOP_SHFT			4
#define VDMA_REG_SD_WR_STAT_SD_WR_STOP_MSK			0x00000010

#define VDMA_REG_SD_WR_ADD_SD_WR_ADD_SHFT				0
#define VDMA_REG_SD_WR_ADD_SD_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_LCD_WR_CTRL_LCD_WR_BITMODE_SHFT			4
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_BITMODE_MSK				0x00000030
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_SHFT			3
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_MSK				0x00000008
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_EN_SHFT				2
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_EN_MSK				0x00000004
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_ABORT_SHFT			1
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_ABORT_MSK			0x00000002
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_START_SHFT			0
#define VDMA_REG_LCD_WR_CTRL_LCD_WR_START_MSK			0x00000001

#define VDMA_REG_LCD_WR_STAT_LCD_WR_STOP_SHFT			4
#define VDMA_REG_LCD_WR_STAT_LCD_WR_STOP_MSK			0x00000010

#define VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_SHFT				0
#define VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_EN_SHFT				2
#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_EN_MSK				0x00000004
#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_ABORT_SHFT			1
#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_ABORT_MSK			0x00000002
#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_START_SHFT			0
#define VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_START_MSK			0x00000001

#define VDMA_REG_DSPPM_WR_STAT_DSPPM_WR_STOP_SHFT			4
#define VDMA_REG_DSPPM_WR_STAT_DSPPM_WR_STOP_MSK			0x00000010

#define VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_SHFT				0
#define VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_BITMODE_SHFT			3
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_BITMODE_MSK				0x00000018
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_EN_SHFT				2
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_EN_MSK				0x00000004
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_ABORT_SHFT			1
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_ABORT_MSK			0x00000002
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_START_SHFT			0
#define VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_START_MSK			0x00000001

#define VDMA_REG_DSPDM_RD_STAT_DSPDM_RD_STOP_SHFT			4
#define VDMA_REG_DSPDM_RD_STAT_DSPDM_RD_STOP_MSK			0x00000010

#define VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_SHFT				0
#define VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_BITMODE_SHFT			3
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_BITMODE_MSK				0x00000018
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_EN_SHFT				2
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_EN_MSK				0x00000004
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_ABORT_SHFT			1
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_ABORT_MSK			0x00000002
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_START_SHFT			0
#define VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_START_MSK			0x00000001

#define VDMA_REG_DSPDM_WR_STAT_DSPDM_WR_STOP_SHFT			4
#define VDMA_REG_DSPDM_WR_STAT_DSPDM_WR_STOP_MSK			0x00000010

#define VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_SHFT				0
#define VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_USB_RD_CTRL_USB_RD_BITMODE_SHFT			21
#define VDMA_REG_USB_RD_CTRL_USB_RD_BITMODE_MSK				0x00600000

#define VDMA_REG_USB_CTRL_USB_SRAM_SIZE_SHFT			16
#define VDMA_REG_USB_CTRL_USB_SRAM_SIZE_MSK			0x00010000
#define VDMA_REG_USB_CTRL_USB_SRAM_EN_SHFT			10
#define VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK			0x00000400
#define VDMA_REG_USB_CTRL_USB_SRAM_ABORT_SHFT			5
#define VDMA_REG_USB_CTRL_USB_SRAM_ABORT_MSK			0x00000020
#define VDMA_REG_USB_CTRL_USB_SRAM_START_SHFT			0
#define VDMA_REG_USB_CTRL_USB_SRAM_START_MSK			0x00000001

#define VDMA_REG_USB_STAT_USB_SRAM2_STOP_SHFT			14
#define VDMA_REG_USB_STAT_USB_SRAM2_STOP_MSK            0x00004000
#define VDMA_REG_USB_STAT_USB_SRAM1_STOP_SHFT           9
#define VDMA_REG_USB_STAT_USB_SRAM1_STOP_MSK            0x00000200
#define VDMA_REG_USB_STAT_USB_SRAM0_STOP_SHFT           4
#define VDMA_REG_USB_STAT_USB_SRAM0_STOP_MSK            0x00000010
#define VDMA_REG_USB_STAT_USB_SRAM5_STOP_SHFT			14
#define VDMA_REG_USB_STAT_USB_SRAM5_STOP_MSK			0x00004000
#define VDMA_REG_USB_STAT_USB_SRAM4_STOP_SHFT			9
#define VDMA_REG_USB_STAT_USB_SRAM4_STOP_MSK			0x00000200
#define VDMA_REG_USB_STAT_USB_SRAM3_STOP_SHFT			4
#define VDMA_REG_USB_STAT_USB_SRAM3_STOP_MSK			0x00000010


#define VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT				0
#define VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK				0x7FFFFFFF

#define VDMA_REG_USB_WR_CTRL_USB_SRAM_LST_SHFT			21
#define VDMA_REG_USB_WR_CTRL_USB_SRAM_LST_MSK				0x00200000

static VDMA_ISR_CALLBACK g_VDMA_ISR_Callback[VDMA_DEV_TOTAL];
static VDMA_ISR_CALLBACK g_VDMA_ISR_Callback_1[VDMA_DEV1_TOTAL];

#if VDEV_BSP32_SUPPORTED
static VDMA_ISR_CALLBACK g_VDMA_BSP32_Callback[CODEC_DMA_BSP_NUM];
#endif

#if (VDMA_AIF_ADC_BUG_FIX == 1)
static UINT32 g_VDMA_AIF_ADC_Right_Addr1 = 0;
#endif

volatile UINT8 g_VDMA_CM32DSPDM_inUse = 0;
volatile UINT8 g_VDMA_DSPDM2CM3_inUse = 0;
volatile UINT8 g_VDMA_CM32DSPPM_inUse = 0;

void VDMA_DSPDM2SRAM_IntCallBack(UINT8 error)
{
	if (g_VDMA_DSPDM2CM3_inUse) {
		g_VDMA_DSPDM2CM3_inUse = 0;
		VDMA_DSPDM_ReadEnable(FALSE);
		#if VDEV_BSP32_SUPPORTED
		if(g_VDMA_BSP32_Callback[CODEC_DSPDM2SRAM_POST])
			g_VDMA_BSP32_Callback[CODEC_DSPDM2SRAM_POST]((VDMA_ERR)error);
		#endif
	}
}

void VDMA_SRAM2DSPDM_IntCallBack(UINT8 error)
{
	if (g_VDMA_CM32DSPDM_inUse) {
		g_VDMA_CM32DSPDM_inUse = 0;
		VDMA_DSPDM_WriteEnable(FALSE);
		#if VDEV_BSP32_SUPPORTED
		if(g_VDMA_BSP32_Callback[CODEC_SRAM2DSPDM_POST])
			g_VDMA_BSP32_Callback[CODEC_SRAM2DSPDM_POST]((VDMA_ERR)error);	
		#endif
	}
}

void VDMA_SRAM2DSPPM_IntCallBack(UINT8 error)
{
	if (g_VDMA_CM32DSPPM_inUse) {
		g_VDMA_CM32DSPPM_inUse = 0;
		VDMA_DSPPM_WriteEnable(FALSE);
	}
}

#if VDEV_BSP32_SUPPORTED
void VDMA_RegisterBsp32Callback(UINT8 index, VDMA_ISR_CALLBACK p_Callback)
{
	g_VDMA_BSP32_Callback[index] = p_Callback;
}

VDMA_ISR_CALLBACK VDMA_GetBsp32Callback(UINT8 index)
{
	return g_VDMA_BSP32_Callback[index];
}
#endif

void VDMA_MaskAllInt(void)
{
#ifdef CORE_SEL_MCUTOP
	WRITE_REG(REG_DMA_INTR_FNSH_MASK, VDMA_INT_MC_ALL_MSK);
	WRITE_REG(REG_DMA_INTR_ERR_MASK, VDMA_INT_MC_ALL_MSK);
#else
	WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT, VDMA_INT_MC_ALL_MSK);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT, VDMA_INT_MC_ALL_MSK);	
#endif	
}
void VDMA_MaskAllInt_1(void)
{
#ifdef CORE_SEL_MCUTOP
		WRITE_REG(REG_DMA_INTR_FNSH_MASK1, VDMA_INT_MC_ALL_MSK);
		WRITE_REG(REG_DMA_INTR_ERR_MASK1, VDMA_INT_MC_ALL_MSK);
#else
		WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1, VDMA_INT_MC_ALL_MSK);
		WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT1, VDMA_INT_MC_ALL_MSK);	
#endif	
}
/******************************************************************************
Description:
 	mask interrupts of DMA. 
Parameters:
 	Interrupt bit masks, definition of masked bits are prefixed with VDMA_INT_MC_xxxx,
 	see vdma.h
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_MaskInt(UINT32 BitMasks)
{
#ifdef CORE_SEL_MCUTOP
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	WRITE_REG(REG_DMA_INTR_ERR_MASK, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));
#else
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
#endif	
}
void VDMA_MaskInt_1(UINT32 BitMasks)
{
#ifdef CORE_SEL_MCUTOP
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));
#else
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
#endif	
}

void VDMA_MaskSdInt(UINT32 BitMasks)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
}
void VDMA_MaskSdInt_1(UINT32 BitMasks)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
}

void VDMA_MaskErrInt(UINT32 BitMasks)
{
	UINT32 reg;
	#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	WRITE_REG(REG_DMA_INTR_ERR_MASK, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));
	#else
	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
	#endif	
}
void VDMA_MaskErrInt_1(UINT32 BitMasks)
{
	UINT32 reg;
	#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_ERR_MASK1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));
	#else
	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT1, (reg | (BitMasks & VDMA_INT_MC_ALL_MSK)));	
	#endif	
}

/******************************************************************************
Description:
 	Unmask interrupts of DMA. 
Parameters:
 	Interrupt bit masks, definition of masked bits are prefixed with VDMA_INT_MC_xxxx,
 	see vdma.h
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_UnmaskInt(UINT32 BitMasks)
{
#ifdef CORE_SEL_MCUTOP
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK, (reg & (~BitMasks)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	WRITE_REG(REG_DMA_INTR_ERR_MASK, (reg & (~BitMasks)));
#else
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT, (reg & (~BitMasks)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT, (reg & (~BitMasks)));
#endif
}
void VDMA_UnmaskInt_1(UINT32 BitMasks)
{
#ifdef CORE_SEL_MCUTOP
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK1, (reg & (~BitMasks)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK1, (reg & (~BitMasks)));
#else
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1, (reg & (~BitMasks)));

	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT1);
	WRITE_REG(REG_DMA_INTR_ERR_MASK_MCUBT1, (reg & (~BitMasks)));
#endif
}

void VDMA_SD_UnmaskInt(UINT32 BitMasks)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK, (reg & (~BitMasks)));

	//reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	//WRITE_REG(REG_DMA_INTR_ERR_MASK, (reg & (~BitMasks)));
}
void VDMA_SD_UnmaskInt_1(UINT32 BitMasks)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_INTR_FNSH_MASK1);
	WRITE_REG(REG_DMA_INTR_FNSH_MASK1, (reg & (~BitMasks)));

	//reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	//WRITE_REG(REG_DMA_INTR_ERR_MASK, (reg & (~BitMasks)));
}

UINT32 VDMA_GetMaskInt(void)
{
	UINT32 reg = 0;
#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_FNSH_MASK);
#else
	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT);
#endif	
	return reg;
}
UINT32 VDMA_GetMaskInt_1(void)
{
	UINT32 reg = 0;
#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_FNSH_MASK1);
#else
	reg = READ_REG(REG_DMA_INTR_FNSH_MASK_MCUBT1);
#endif	
	return reg;
}

UINT32 VDMA_GetMaskErrInt(void)
{
	UINT32 reg = 0;
	#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_ERR_MASK);
	#else
	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT);
	#endif	
	return reg;
}

UINT32 VDMA_GetMaskErrInt_1(void)
{
	UINT32 reg = 0;
	#ifdef CORE_SEL_MCUTOP
	reg = READ_REG(REG_DMA_INTR_ERR_MASK1);
	#else
	reg = READ_REG(REG_DMA_INTR_ERR_MASK_MCUBT1);
	#endif
	return reg;	
}


/******************************************************************************
Description:
 	Clear interrupts of DMA. 
Parameters:
 	Interrupt bit masks, definition of masked bits are prefixed with VDMA_INT_MC_xxxx,
 	see vdma.h
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_ClearErrInt(UINT32 BitMasks)
{
	WRITE_REG(REG_DMA_INTR_ERR_CLR, ( (BitMasks & VDMA_INT_MC_ALL_MSK)));
}
void VDMA_ClearErrInt_1(UINT32 BitMasks)
{
	WRITE_REG(REG_DMA_INTR_ERR_CLR1, ( (BitMasks & VDMA_INT_MC_ALL_MSK)));
}

void VDMA_ClearInt(UINT32 BitMasks)
{
	 WRITE_REG(REG_DMA_INTR_FNSH_CLR, ( (BitMasks & VDMA_INT_MC_ALL_MSK)));
}
void VDMA_ClearInt_1(UINT32 BitMasks)
{
	 WRITE_REG(REG_DMA_INTR_FNSH_CLR1, ( (BitMasks & VDMA_INT_MC_ALL_MSK)));	
}

void VDMA_EnableInt(void)
{
    VCPU_INT_SrcEnable(IRQ_DMA_INT);
}

void VDMA_DisableInt(void)
{
    VCPU_INT_SrcDisable(IRQ_DMA_INT);
}

BOOL VDMA_IntPending(void)
{
    return VCPU_INT_SrcIsPending(IRQ_DMA_INT);
}

#if 1//for AEC
//#define INTR_FNSH_STAT_SRAM2AIF_WR_DONE_BIT		(1<<24)//bit24, for dac dma inter pending check
//#define INTR_FNSH_STAT_AIF2SRAM_RD_DONE_BIT		(1<<18)//bit18, for adc dma inter pending check
BOOL VDMA_AIF_IsDACIntPending(void)
{
	return (((READ_REG(REG_DMA_INTR_FNSH_STAT)&VDMA_INT_STATE_SRAM_AIF_DONE_MSK)==0)? FALSE: TRUE);
}

BOOL VDMA_AIF_IsADCIntPending(void)
{
	return (((READ_REG(REG_DMA_INTR_FNSH_STAT)&VDMA_INT_STATE_AIF_SRAM_DONE_MSK)==0)? FALSE: TRUE);
}

UINT32 VDMA_AIF_GetIntStat(void)
{
	return ((UINT32)READ_REG(REG_DMA_INTR_FNSH_STAT));
}

#endif

#if VCODEC_HWGAINSYC_SUPPORTED
/******************************************************************************
Description:
 	Sets the GainSync, for 9638p.

 ===========
 aud_rd_ctrl(ADC)	
Bit[7] bit[4]:  (data)   (sync gain)   (total data width)
2'b01:           16         0                 16
2'b11:           16         8                  24
2'b00:           24         0                  24
2'b10:           24         8                  32 
          or       32         0                  32       
===========
aud_wr_ctrl(DAC)
aud_wr_bitmod--bit[4]:data Bit width(donot include gain bit) mode selecting for aud DAC path.0h: 24bits  or 32bits mode.1h: 16bits mode.
NOTE:when audw_syncgain_en=1&audw_surc_mem_sel, the total data width from dspmem:data + gain,16+8.only support 16bits data mode.convmode register bit is invalided.
===========

Parameters:
	mode:  == 0 close the gain sync, ==1 only open dac gain sync, ==2 only open adc gain sync, ==3 open both dac&adc gain sync;

Return Value:
	None.
*****************************************************************************/
void VDMA_GainSyncSet (UINT8 mode)
{
	UINT32 reg;
	
	//for dac(audio write channel) gain sync set;
	reg = READ_REG(REG_DMA_AIF_WR_CTRL);
	if (mode & 0x01) {
		//WRITE_REG(REG_DMA_AIF_WR_CTRL, (reg&(~VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK))|VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK);// clear bit[4]&set bit[7];
		WRITE_REG(REG_DMA_AIF_WR_CTRL, (reg|VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK|VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK));// set bit[7]&set bit[4];
	} else {
		WRITE_REG(REG_DMA_AIF_WR_CTRL, (reg&(~VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK))|VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK);//clear bit[7] &set bit[4] as default;
	}

	//for adc(audio read channel) gain sync set;
	reg = READ_REG(REG_DMA_AIF_RD_CTRL);
	if (mode & 0x02) {
		//WRITE_REG(REG_DMA_AIF_RD_CTRL, (reg&(~0x00000010))|0x00000080);// clear bit[4]&set bit[7];
		WRITE_REG(REG_DMA_AIF_RD_CTRL, (reg|0x00000010|0x00000080));// set bit[7]&set bit[4];
	} else {
		WRITE_REG(REG_DMA_AIF_RD_CTRL, (reg&(~0x00000080))|0x00000010);//clear bit[7] &set bit[4] as default;
	}		
}
#endif


void VDMA_RegisterISRCallback(VDMA_DEV Dev, BOOL bflag, VDMA_ISR_CALLBACK p_Callback)
{
	g_VDMA_ISR_Callback[Dev] = p_Callback;
}
void VDMA_RegisterISRCallback_1(VDMA_DEV1 Dev, BOOL bflag, VDMA_ISR_CALLBACK p_Callback)
{
	g_VDMA_ISR_Callback_1[Dev] = p_Callback;
}

/******************************************************************************
Description:
 	Get the error state indicated by error interrupt
Parameters:
 	void
Return Value:
	the error states composed by error bits
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_Interrupt_Routine_Service()
{
	UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
	UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);
	UINT32 error_mask = VDMA_GetMaskErrInt();
	UINT32 fnfh_mask = VDMA_GetMaskInt();
	
	error_reg &= ~error_mask;
	fnfh_reg &= ~fnfh_mask;
	if(error_reg !=0 )
	{
		for(UINT32 i = 0; i < 32; i++)
		{
			if(((UINT32)1<<i) & error_reg)
			{				
				VDMA_ClearErrInt(1 << i);
				if(g_VDMA_ISR_Callback[i] !=  NULL)
					(g_VDMA_ISR_Callback[i])(VDMA_ERR_OCCUR);				 	 
			}
		}
	}
	if(fnfh_reg !=0 )
	{
		for(UINT32 i = 0; i < 32; i++)
		{
			if(((UINT32)1<<i) & fnfh_reg)
			{
				VDMA_ClearInt(1 << i);
				if(g_VDMA_ISR_Callback[i] !=  NULL)
				  (g_VDMA_ISR_Callback[i])(VDMA_ERR_NONE);
			}		
		}
	}		
}
void VDMA_Interrupt_Routine_Service_1()
{
	UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT1);
	UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT1);
	UINT32 error_mask = VDMA_GetMaskErrInt_1();
	UINT32 fnfh_mask = VDMA_GetMaskInt_1();
	
	error_reg &= ~error_mask;
	fnfh_reg &= ~fnfh_mask;
	if(error_reg !=0 )
	{
		for(UINT32 i = 0; i < 32; i++)
		{
			if(((UINT32)1<<i) & error_reg)
			{				
				VDMA_ClearErrInt_1(1 << i);
				if(g_VDMA_ISR_Callback_1[i] !=  NULL)
					(g_VDMA_ISR_Callback_1[i])(VDMA_ERR_OCCUR);				 	 
			}
		}
	}

	if(fnfh_reg !=0 )
	{
		for(UINT32 i = 0; i < 32; i++)
		{	
			if(((UINT32)1<<i) & fnfh_reg)
			{
				VDMA_ClearInt_1(1 << i);
				if(g_VDMA_ISR_Callback_1[i] !=  NULL)
				  (g_VDMA_ISR_Callback_1[i])(VDMA_ERR_NONE);
			}		
		}
	}		
}

/******************************************************************************
Description:
 	Set the access priorities of peer devices. The peer devices are include SPI, AIF and Baseband.
 	The priority determined the access sequences of devices when there are collision among them.
 	Unless the lower priority device had no room in its FIFOs, higher priority device will keep the
 	transport bus.
Parameters:
 	one of below definition, device on the left has higher priority:
 		VDMA_DEV_PRIO_SPI_BB_AIF			
		VDMA_DEV_PRIO_SPI_AIF_BB			
		VDMA_DEV_PRIO_BB_SPI_AIF			
		VDMA_DEV_PRIO_BB_AIF_SPI			
		VDMA_DEV_PRIO_AIF_SPI_BB			
		VDMA_DEV_PRIO_AIF_BB_SPI			
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SetSram2DevPriority(UINT8 Prio)
{
	UINT32 reg;
	
	if (Prio > VDMA_DEV_PRIO_AIF_BB_SPI)
		return;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	WRITE_REG(REG_DMA_TRF_CTRL, ((Prio << VDMA_REG_TRF_CTRL_DEV_PRIORITY_SHFT) | 
								(reg & (~VDMA_REG_TRF_CTRL_DEV_PRIORITY_MSK))));
}

void VDMA_SetDspdm2DevPriority(UINT8 Prio)
{
	UINT32 reg;
	
	if (Prio > VDMA_DEV_PRIO_AIF_SD_USB_LCD)
		return;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	WRITE_REG(REG_DMA_TRF_CTRL, ((Prio << VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_SHFT) | 
								(reg & (~VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_MSK))));	
}

/******************************************************************************
Description:
 	Get the access priorities of peer devices. The peer devices are include SPI, AIF and Baseband.
 	The priority determined the access sequences of devices when there are collision among them.
 	Unless the lower priority device had no room in its FIFOs, higher priority device will keep the
 	transport bus.
Parameters:
 	void			
Return Value:
	one of below definition, device on the left has higher priority:
 		VDMA_DEV_PRIO_SPI_BB_AIF			
		VDMA_DEV_PRIO_SPI_AIF_BB			
		VDMA_DEV_PRIO_BB_SPI_AIF			
		VDMA_DEV_PRIO_BB_AIF_SPI			
		VDMA_DEV_PRIO_AIF_SPI_BB			
		VDMA_DEV_PRIO_AIF_BB_SPI
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT8 VDMA_GetSram2DevPriority(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	return ((reg & VDMA_REG_TRF_CTRL_DEV_PRIORITY_MSK) >> VDMA_REG_TRF_CTRL_DEV_PRIORITY_SHFT);	
}

UINT8 VDMA_GetDspdm2DevPriority(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	return ((reg & VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_MSK) >> VDMA_REG_TRF_CTRL_DSPDM2DEV_PRIORITY_SHFT);
}

void VDMA_SetDspdma_Burstmode(UINT8 sel, UINT8  mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	switch (sel) {
	case 0:
		reg = (mode << VDMA_REG_TRF_CTRL_DSPDM_BM_DATA_SHFT) | 
				(reg & (~VDMA_REG_TRF_CTRL_DSPDM_BM_DATA_MSK));
		break;
	case 1:
		reg = (mode << VDMA_REG_TRF_CTRL_DSPDM_BM_CODE_SHFT) | 
				(reg & (~VDMA_REG_TRF_CTRL_DSPDM_BM_CODE_MSK));
		break;
	default:
		return;
	}
	WRITE_REG(REG_DMA_TRF_CTRL, reg);
}

void VDMA_Hold_Dspcore(BOOL flag)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_TRF_CTRL);
	reg = (flag << VDMA_REG_TRF_CTRL_DSP_HOLD_SHFT) | 
		(reg & (~VDMA_REG_TRF_CTRL_DSP_HOLD_MSK));
	
	WRITE_REG(REG_DMA_TRF_CTRL, reg);
}
/******************************************************************************
Description:
 	Check if all DMA channels are disabled.
Parameters:
 	
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
BOOL VDMA_IsEnabled(void)
{
	return (VDMA_AIF_IsReadEnabled() || VDMA_AIF_IsWriteEnabled() || \
		VDMA_SPI_IsReadEnabled() || VDMA_SPI_IsWriteEnabled() || \
		VDMA_BB_IsReadEnabled() || VDMA_BB_IsWriteEnabled());	
}


/******************************************************************************
Description:
 	Enable/disable DMA read path for AIF.
Parameters:
 	RdEn - one of below definitions:
			VDMA_AIF_BOTH_CHANNEL_DISABLE		
			VDMA_AIF_LEFT_CHANNEL_ENABLE		
			VDMA_AIF_RIGHT_CHANNEL_ENABLE		
			VDMA_AIF_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_AIF_ReadEnable(UINT8 RdEn)
{
	UINT32 reg;

#if (VDMA_AIF_ADC_BUG_FIX == 1)
	g_VDMA_AIF_ADC_Right_Addr1 = 0x00000000;
#endif

	reg = READ_REG(REG_DMA_AIF_RD_CTRL);
	WRITE_REG(REG_DMA_AIF_RD_CTRL, ((RdEn << VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_SHFT) | 
									(reg & (~VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK))));	
}

// RdSrc = 0 src is aif, 1: src is i2s, RdDst = 0 dst is cm3, 1: dst is dsp, mode = 0 bit is 24, 1: bit is 16
void VDMA_AIF_ReadConfig(BOOL RdSrc, BOOL RdDst, BOOL mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_RD_CTRL);
	reg = RdSrc ? (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_SRC_MSK) : (reg & (~VDMA_REG_AIF_RD_CTRL_AIF_RD_SRC_MSK));
	reg = RdDst ? (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_DST_MSK) : (reg & (~VDMA_REG_AIF_RD_CTRL_AIF_RD_DST_MSK));
	reg = mode ? (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_BITMOD_MSK) : (reg & (~VDMA_REG_AIF_RD_CTRL_AIF_RD_BITMOD_MSK));
	WRITE_REG(REG_DMA_AIF_RD_CTRL,reg);
}

// RdCtrl Src = 0 src is cm3, 1: src is dsp
void VDMA_AIF_ReadCtrlSrc(BOOL RdCtrlSrc)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_RD_CTRL);
	WRITE_REG(REG_DMA_AIF_RD_CTRL, 
		(RdCtrlSrc ? (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_CTLSRC_MSK) : (reg & (~VDMA_REG_AIF_RD_CTRL_AIF_RD_CTLSRC_MSK))));
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for AIF.
Parameters:
 	WrEn - one of below definitions:
			VDMA_AIF_BOTH_CHANNEL_DISABLE		
			VDMA_AIF_LEFT_CHANNEL_ENABLE		
			VDMA_AIF_RIGHT_CHANNEL_ENABLE		
			VDMA_AIF_BOTH_CHANNEL_ENABLE
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_AIF_WriteEnable(UINT8 WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_WR_CTRL);
	WRITE_REG(REG_DMA_AIF_WR_CTRL, ((WrEn << VDMA_REG_AIF_WR_CTRL_AIF_WR_EN_SHFT) | 
									(reg & (~VDMA_REG_AIF_WR_CTRL_AIF_WR_EN_MSK))));
}

// WrDst = 0 dst is aif, 1: dst is i2s, WrSrc = 0 src is cm3, 1: src is dsp, mode = 0 bit is 24, 1: bit is 16
void VDMA_AIF_WriteConfig(BOOL WrSrc, BOOL WrDst, BOOL mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_WR_CTRL);
	reg = WrSrc ? (reg | VDMA_REG_AIF_WR_CTRL_AIF_WR_SRC_MSK) : (reg & (~VDMA_REG_AIF_WR_CTRL_AIF_WR_SRC_MSK));
	reg = WrDst ? (reg | VDMA_REG_AIF_WR_CTRL_AIF_WR_DST_MSK) : (reg & (~VDMA_REG_AIF_WR_CTRL_AIF_WR_DST_MSK));
	reg = mode ? (reg | VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK) : (reg & (~VDMA_REG_AIF_WR_CTRL_AIF_AIF_WR_BITMOD_MSK));
	WRITE_REG(REG_DMA_AIF_WR_CTRL,reg);
}

// RdCtrl Src = 0 src is cm3, 1: src is dsp
void VDMA_AIF_WriteCtrlSrc(BOOL WrCtrlSrc)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_WR_CTRL);
	WRITE_REG(REG_DMA_AIF_WR_CTRL, 
		(WrCtrlSrc ? (reg | VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK) : (reg & (~VDMA_REG_AIF_WR_CTRL_AIF_WR_CTLSRC_MSK))));
}

BOOL VDMA_AIF_IsReadEnabled(void)
{
	return ((READ_REG(REG_DMA_AIF_RD_CTRL) & VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK)? TRUE:FALSE);	
}

BOOL VDMA_AIF_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_AIF_WR_CTRL) & VDMA_REG_AIF_WR_CTRL_AIF_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate AIF read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
			VDMA_ACTION_STOP   // after cuurent FIFO transmission, stop to read
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_AIF_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_AIF_RD_CTRL);
		WRITE_REG(REG_DMA_AIF_RD_CTRL, (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_AIF_RD_CTRL);
		WRITE_REG(REG_DMA_AIF_RD_CTRL, (reg | VDMA_REG_AIF_RD_CTRL_AIF_RD_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_AIF_RD_STAT);
		if (reg & VDMA_REG_AIF_RD_STAT_AIF_RD_FE0_MSK) {
			reg = READ_REG(REG_DMA_AIF_RD_LADD0);
			reg |= VDMA_REG_AIF_RD_LADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_AIF_RD_LADD0, reg);
		} else if (reg & VDMA_REG_AIF_RD_STAT_AIF_RD_FE1_MSK) {
			reg = READ_REG(REG_DMA_AIF_RD_LADD1);
			reg |= VDMA_REG_AIF_RD_LADD1_END_BUF1_MSK;
			WRITE_REG(REG_DMA_AIF_RD_LADD1, reg);
		}
		break;
	}
}

/******************************************************************************
Description:
 	To operate AIF write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
			VDMA_ACTION_STOP   // after cuurent FIFO transmission, stop to write
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_AIF_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_AIF_WR_CTRL);
		WRITE_REG(REG_DMA_AIF_WR_CTRL, (reg | VDMA_REG_AIF_WR_CTRL_AIF_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_AIF_WR_CTRL);
		WRITE_REG(REG_DMA_AIF_WR_CTRL, (reg | VDMA_REG_AIF_WR_CTRL_AIF_WR_ABORT_MSK));
		break;
	case VDMA_ACTION_STOP:
		reg = READ_REG(REG_DMA_AIF_WR_STAT);
		if (reg & VDMA_REG_AIF_WR_STAT_AIF_WR_FE0_MSK) {
			reg = READ_REG(REG_DMA_AIF_WR_ADD0);
			reg |= VDMA_REG_AIF_WR_ADD0_END_BUF0_MSK;
			WRITE_REG(REG_DMA_AIF_WR_ADD0, reg);
		} else if (reg & VDMA_REG_AIF_WR_STAT_AIF_WR_FE1_MSK) {
			reg = READ_REG(REG_DMA_AIF_WR_ADD1);
			reg |= VDMA_REG_AIF_WR_ADD1_END_BUF1_MSK;
			WRITE_REG(REG_DMA_AIF_WR_ADD1, reg);
		}
		break;
	}	
}

/******************************************************************************
Description:
 	Configure parameters of AIF read path, including target buffer, memory address, transmission 
 	size, and stop flag.
Parameters:
 	Buf - select the target buffer of 2 ping-pang buffers
 	RightAddr - memory address for right channel data
 	LeftAddr - memory address for left channel data
 	Len - size of ONE channel to read in byte, whatever both channels or only one enabled
 	Stop - to tell if stop AIF readl transmission after current buffer transport
Return Value:
	void
Remarks:
	Ping-pang-buffers is designed to avoid data missing. Once one buffer transport completed, the
	transport will switch to the other buffer automatically if it has been configured. 
Example:
 	None.
See Also:
	VDMA_AIF_ConfigReadPath_Auto.
*****************************************************************************/
UINT32 g_TmpAEC_Len[6]={0};//for AEC test.

void VDMA_AIF_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop)
{
	UINT32 reg;
	
	switch (Buf) {
	case VDMA_BUF_0:
		reg = (RightAddr & VDMA_REG_AIF_RD_RADD0_AIF_RD_RADD0_MSK) << VDMA_REG_AIF_RD_RADD0_AIF_RD_RADD0_SHFT;
		WRITE_REG(REG_DMA_AIF_RD_RADD0, reg);
#if (VDMA_AIF_ADC_BUG_FIX == 1)
		g_VDMA_AIF_ADC_Right_Addr1 = (UINT32)(READ_REG(REG_DMA_AIF_RD_RADD1) & VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_MSK);
		WRITE_REG(REG_DMA_AIF_RD_RADD1, reg);
#endif
		
		reg = (LeftAddr & VDMA_REG_AIF_RD_LADD0_AIF_RD_LADD0_MSK) << VDMA_REG_AIF_RD_LADD0_AIF_RD_LADD0_SHFT;
		if (Stop) {
			reg |= VDMA_REG_AIF_RD_LADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_AIF_RD_LADD0, reg);
		WRITE_REG(REG_DMA_AIF_RD_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (RightAddr & VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_MSK) << VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_SHFT;
#if (VDMA_AIF_ADC_BUG_FIX == 1)
		g_VDMA_AIF_ADC_Right_Addr1 = reg;
#endif
		
		WRITE_REG(REG_DMA_AIF_RD_RADD1, reg);
		reg = (LeftAddr & VDMA_REG_AIF_RD_LADD1_AIF_RD_LADD1_MSK) << VDMA_REG_AIF_RD_LADD1_AIF_RD_LADD1_SHFT;
		if (Stop) {
			reg |= VDMA_REG_AIF_RD_LADD1_END_BUF1_MSK;
		}
		WRITE_REG(REG_DMA_AIF_RD_LADD1, reg);
		WRITE_REG(REG_DMA_AIF_RD_LENGTH1, Len);
		break;
	}
	g_TmpAEC_Len[Buf] = Len;//for test.	
}

/******************************************************************************
Description:
 	Configure parameters of AIF write path, including target buffer, memory address, transmission 
 	size, and stop flag.
Parameters:
 	Buf - select the target buffer of 2 ping-pang buffers
 	Addr - memory address for data
 	Len - size of ONE channel to read in byte, whatever both channels or only one enabled
 	Stop - to tell if stop AIF readl transmission after current buffer transport
Return Value:
	void
Remarks:
	Ping-pang-buffers is designed to avoid data missing. Once one buffer transport completed, the
	transport will switch to the other buffer automatically if it has been configured. 
	The write path combine both channel data together, so only one memory address needed.
Example:
 	None.
See Also:
	VDMA_AIF_ConfigWritePath_Auto.
*****************************************************************************/
void VDMA_AIF_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
{
	UINT32 reg;

	switch (Buf) {
	case VDMA_BUF_0:
		reg = (Addr & VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_MSK) << VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_SHFT;
		if (Stop) {
			reg |= VDMA_REG_AIF_WR_ADD0_END_BUF0_MSK;
		}
		WRITE_REG(REG_DMA_AIF_WR_ADD0, reg);
		WRITE_REG(REG_DMA_AIF_WR_LENGTH0, Len);
		break;
	case VDMA_BUF_1:
		reg = (Addr & VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_MSK) << VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_SHFT;
		if (Stop) {
			reg |= VDMA_REG_AIF_WR_ADD1_END_BUF1_MSK;
		}
		WRITE_REG(REG_DMA_AIF_WR_ADD1, reg);
		WRITE_REG(REG_DMA_AIF_WR_LENGTH1, Len);
		break;
	}

	g_TmpAEC_Len[3+Buf] = Len;//for test.
}

/******************************************************************************
Description:
 	Get the configured memory addresses of  read buffers
Parameters:
 	p_RBuf0[out] - returned memory address of right channel buffer 0
 	p_RBuf1[out] - returned memory address of right channel buffer 1
 	p_LBuf0[out] - returned memory address of left channel buffer 0
 	p_LBuf1[out] - returned memory address of left channel buffer 1
Return Value:
	void
Remarks:
	The configured paramters will be reset after path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_AIF_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1)
{
	UINT8 en;

	en = (READ_REG(REG_DMA_AIF_RD_CTRL) & VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK) >> VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_SHFT;
	
	if (en & VDMA_AIF_RIGHT_CHANNEL_ENABLE) {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = (UINT32)(READ_REG(REG_DMA_AIF_RD_RADD0) & VDMA_REG_AIF_RD_RADD0_AIF_RD_RADD0_MSK);
		if (p_RBuf1 != NULL) {
#if (VDMA_AIF_ADC_BUG_FIX == 1)
			*p_RBuf1 = g_VDMA_AIF_ADC_Right_Addr1;
#else			
			*p_RBuf1 = (UINT32)(READ_REG(REG_DMA_AIF_RD_RADD1) & VDMA_REG_AIF_RD_RADD1_AIF_RD_RADD1_MSK);
#endif			
		}
	} else {
		if (p_RBuf0 != NULL)
			*p_RBuf0 = 0;
		if (p_RBuf1 != NULL)
			*p_RBuf1 = 0;
	}
	
	if (en & VDMA_AIF_LEFT_CHANNEL_ENABLE) {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = (UINT32)(READ_REG(REG_DMA_AIF_RD_LADD0) & VDMA_REG_AIF_RD_LADD0_AIF_RD_LADD0_MSK);
		if (p_LBuf1 != NULL)
			*p_LBuf1 = (UINT32)(READ_REG(REG_DMA_AIF_RD_LADD1) & VDMA_REG_AIF_RD_LADD1_AIF_RD_LADD1_MSK);
	} else {
		if (p_LBuf0 != NULL)
			*p_LBuf0 = 0;
		if (p_LBuf1 != NULL)
			*p_LBuf1 = 0;
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
void VDMA_AIF_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1)
{
	*p_Buf0 = (UINT32)((READ_REG(REG_DMA_AIF_WR_ADD0) & VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_MSK) >> VDMA_REG_AIF_WR_ADD0_AIF_WR_ADD0_SHFT);
	*p_Buf1 = (UINT32)((READ_REG(REG_DMA_AIF_WR_ADD1) & VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_MSK) >> VDMA_REG_AIF_WR_ADD1_AIF_WR_ADD1_SHFT);
}

/******************************************************************************
Description:
 	Get the actually read count in byte. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	read count in byte, this count only indicate ONE channel data number, the other one if exsit
	will has same number.
Remarks:
	The count will be reset after read path disabled!
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_AIF_GetReadCount(void)
{
	return READ_REG(REG_DMA_AIF_RD_CNT); // it's only count of 1 channel
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
UINT32 VDMA_AIF_GetWritenCount(void)
{
	return READ_REG(REG_DMA_AIF_WR_CNT);
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_AIF_IsReadStopped(void)
{
	return (BOOL)(READ_REG(REG_DMA_AIF_RD_STAT) & VDMA_REG_AIF_RD_STAT_AIF_RD_STOP_MSK);
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
BOOL VDMA_AIF_IsWriteStopped(void)
{
	return (BOOL)(READ_REG(REG_DMA_AIF_WR_STAT) & VDMA_REG_AIF_WR_STAT_AIF_WR_STOP_MSK);
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
VDMA_BUF VDMA_AIF_GetReadIntSource(void)
{
	return (VDMA_BUF)((READ_REG(REG_DMA_AIF_RD_STAT) & VDMA_REG_AIF_RD_STAT_AIF_RDINTR_SRC_MSK) >> 
					VDMA_REG_AIF_RD_STAT_AIF_RDINTR_SRC_SHFT);
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
VDMA_BUF VDMA_AIF_GetWriteIntSource(void)
{
	return (VDMA_BUF)((READ_REG(REG_DMA_AIF_WR_STAT) & VDMA_REG_AIF_WR_STAT_AIF_WRINTR_SRC_MSK) >> 
					VDMA_REG_AIF_WR_STAT_AIF_WRINTR_SRC_SHFT);
}

/******************************************************************************
Description:
 	To tell which buffer in ping-pang buffers is active working.
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
VDMA_BUF VDMA_AIF_GetReadActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_RD_STAT);
	if (reg & VDMA_REG_AIF_RD_STAT_AIF_RD_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_AIF_RD_STAT_AIF_RD_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}
}

/******************************************************************************
Description:
 	To tell which buffer in ping-pang buffers is active working.
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
VDMA_BUF VDMA_AIF_GetWriteActiveBuffer(void)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_AIF_WR_STAT);
	if (reg & VDMA_REG_AIF_WR_STAT_AIF_WR_FE0_MSK) {
		return VDMA_BUF_0;
	} else if (reg & VDMA_REG_AIF_WR_STAT_AIF_WR_FE1_MSK) {
		return VDMA_BUF_1;
	} else {
		return VDMA_BUF_NONE;
	}
}

/******************************************************************************
Description:
 	Get the configured length of figured buffer.
Parameters:
 	void
Return Value:
	buffer length in byte, indicate only one channel data length.
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_AIF_GetReadBufferLength(VDMA_BUF Buf)
{
	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_AIF_RD_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_AIF_RD_LENGTH1);
	default:
		VAPP_VOICE_EPRINTF("%s:%d err.\r\n", Buf);
		return 0;
	}
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
UINT32 VDMA_AIF_GetWriteBufferLength(VDMA_BUF Buf)
{
	switch (Buf) {
	case VDMA_BUF_0:
		return READ_REG(REG_DMA_AIF_WR_LENGTH0);
	case VDMA_BUF_1:
		return READ_REG(REG_DMA_AIF_WR_LENGTH1);
	default:
		VAPP_VOICE_EPRINTF("%s:%d err.\r\n", Buf);
		return 0;
	}	
}

/******************************************************************************
Description:
 	Get the data number in hardware FIFOs. This function can be used to get detailed path
 	latency. The reuslt is useful for some audio algrithm link AEC.
Parameters:
 
 	p_WFifoCnt[out] - returned write fifo data number, dac had move from sram to aif.
 	p_RFifoCnt[out] - returned read fifo data number, adc had fill from aif to sram.
Return Value:
	VRET_NO_ERROR, count got are valid, otherwise got failed.
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_AIF_GetTransferedCnt(UINT16* p_WFifoCnt, UINT16* p_RFifoCnt)
{
	UINT32 reg;
	
	#if 1//20180703xy, not need check the VDMA_REG_AIF_CNT_AIF_CNT_FLAG_MSK.
	reg = READ_REG(REG_DMA_AIF_RD_CTRL);//for AEC then adc should open, just check REG_DMA_AIF_RD_CTRL is ok, not check REG_DMA_AIF_WR_CTRL.
	if (((reg & VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK) >> VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_SHFT)&VDMA_AIF_BOTH_CHANNEL_ENABLE == 0) {
		VAPP_VOICE_WPRINTF("VDMA_AIF_GetTransferedCnt, fail[0x%x, 0x%x].\r\n", reg, READ_REG(REG_DMA_AIF_RD_STAT));
		return VRET_DRV_DMA_READ_AIF_CNT_FAIL;
	}
		
	
/* 2018/7/2 ( ) 12:23  :  Qosound  yu.xiaoyi:
 zhuwei aud_cnt_flg bit aud_wfifo_cnt aud_wfifo_cnt.  sram codec ADC buffer DAC fuffer 2-4 sample 
 DMA ADC,DAC buffer 4*32bits 4 words,  ADC DAC sample DAC buffer 4 word ADC buffer DAC fuffer 
aud_cnt_flg 
9628 dmac 汾 9638,9645 sram fifo flag 
*/
	reg = READ_REG(REG_DMA_AIF_CNT);
	*p_WFifoCnt = /*VDMA_AIF_DMAC_DAC_BUF +*/(UINT16)((reg & VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT);
	*p_RFifoCnt = /*VDMA_AIF_DMAC_ADC_BUF +*/(UINT16)((reg & VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT);
	//VAPP_VOICE_NPRINTF("VDMA_AIF_GetTransferedCnt, ok[WCnt = 0x%x, RCnt = 0x%x].\r\n", (*p_WFifoCnt), (*p_RFifoCnt));
	VAPP_VOICE_NPRINTF("Get[WCnt = 0x%x, RCnt = 0x%x].\r\n", (*p_WFifoCnt), (*p_RFifoCnt));
	#if 0//just for test about get aif cnt change.
	reg = READ_REG(REG_DMA_AIF_CNT);
	*p_WFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT);
	*p_RFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT);
	VAPP_VOICE_NPRINTF("1=[WCnt = 0x%x, RCnt = 0x%x].\r\n", (*p_WFifoCnt), (*p_RFifoCnt));
	
	reg = READ_REG(REG_DMA_AIF_CNT);
	*p_WFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT);
	*p_RFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT);
	VAPP_VOICE_NPRINTF("2=[0x%x,0x%x].\r\n", (*p_WFifoCnt), (*p_RFifoCnt));

	reg = READ_REG(REG_DMA_AIF_CNT);
	*p_WFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT);
	*p_RFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT);
	VAPP_VOICE_NPRINTF("0x%x 0x%x]\r\n", (*p_WFifoCnt), (*p_RFifoCnt));
	#endif
	
	return VRET_NO_ERROR;
	
	#else
	
	reg = READ_REG(REG_DMA_AIF_RD_CTRL);
	if (((((reg & VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_MSK) >> VDMA_REG_AIF_RD_CTRL_AIF_RD_EN_SHFT) & VDMA_AIF_BOTH_CHANNEL_ENABLE) == VDMA_AIF_BOTH_CHANNEL_DISABLE) ||
		(READ_REG(REG_DMA_AIF_RD_STAT) & VDMA_REG_AIF_RD_STAT_AIF_RD_STOP_MSK)) {
		return VRET_DRV_DMA_READ_AIF_CNT_FAIL;
	}
		
	do {
		 
		reg = READ_REG(REG_DMA_AIF_CNT);
		if (reg & VDMA_REG_AIF_CNT_AIF_CNT_FLAG_MSK) {
			*p_WFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_WFIFO_CNT_SHFT);
			*p_RFifoCnt = (UINT16)((reg & VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_MSK) >> VDMA_REG_AIF_CNT_AIF_RFIFO_CNT_SHFT);
			return VRET_NO_ERROR;
		} else {
			VCOMMON_Delay(5); // delay 5us
			timeout += 5;
		}
	} while (timeout < 150);

	return VRET_DRV_DMA_READ_AIF_CNT_FAIL;
	#endif
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for SPI.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_ReadEnable(BOOL RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI_RD_CTRL);
	WRITE_REG(REG_DMA_SPI_RD_CTRL, 
		(RdEn ? (reg | VDMA_REG_SPI_RD_CTRL_SPI_RD_EN_MSK) : (reg & (~VDMA_REG_SPI_RD_CTRL_SPI_RD_EN_MSK))));
}

// RdDst = 0 dst is cm3, 1: dst is dsppm
void VDMA_SPI_ReadDst(BOOL RdDst)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI_RD_CTRL);
	WRITE_REG(REG_DMA_SPI_RD_CTRL, 
		(RdDst ? (reg | VDMA_REG_SPI_RD_CTRL_SPI_RD_DST_MSK) : (reg & (~VDMA_REG_SPI_RD_CTRL_SPI_RD_DST_MSK))));
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for SPI.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI_WR_CTRL);
	WRITE_REG(REG_DMA_SPI_WR_CTRL, 
		(WrEn ? (reg | VDMA_REG_SPI_WR_CTRL_SPI_WR_EN_MSK) : (reg & (~VDMA_REG_SPI_WR_CTRL_SPI_WR_EN_MSK))));
}

BOOL VDMA_SPI_IsReadEnabled(void)
{
	return ((READ_REG(REG_DMA_SPI_RD_CTRL) & VDMA_REG_SPI_RD_CTRL_SPI_RD_EN_MSK)? TRUE:FALSE);	
}

BOOL VDMA_SPI_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_SPI_WR_CTRL) & VDMA_REG_SPI_WR_CTRL_SPI_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate SPI read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPI_RD_CTRL);
		WRITE_REG(REG_DMA_SPI_RD_CTRL, (reg | VDMA_REG_SPI_RD_CTRL_SPI_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPI_RD_CTRL);
		WRITE_REG(REG_DMA_SPI_RD_CTRL, (reg | VDMA_REG_SPI_RD_CTRL_SPI_RD_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	To operate SPI write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPI_WR_CTRL);
		WRITE_REG(REG_DMA_SPI_WR_CTRL, (reg | VDMA_REG_SPI_WR_CTRL_SPI_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPI_WR_CTRL);
		WRITE_REG(REG_DMA_SPI_WR_CTRL, (reg | VDMA_REG_SPI_WR_CTRL_SPI_WR_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	Configure parameters of SPI read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_ConfigReadPath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_SPI_RD_ADD, (Addr << VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_SHFT) & VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_MSK);
	WRITE_REG(REG_DMA_SPI_RD_LENGTH, Len);	
}

/******************************************************************************
Description:
 	Configure parameters of SPI write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI_ConfigWritePath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_SPI_WR_ADD, (Addr << VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_SHFT) & VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_MSK);
	WRITE_REG(REG_DMA_SPI_WR_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI_GetReadBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_SPI_RD_ADD) >> VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_SHFT) & VDMA_REG_SPI_RD_ADD_SPI_RD_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_SPI_WR_ADD) >> VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_SHFT) & VDMA_REG_SPI_WR_ADD_SPI_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI_GetReadCount(void)
{
	return READ_REG(REG_DMA_SPI_RD_CNT);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI_GetWritenCount(void)
{
	return READ_REG(REG_DMA_SPI_WR_CNT);
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_SPI_IsReadStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPI_RD_STAT) & VDMA_REG_SPI_RD_STAT_SPI_RD_STOP_MSK) >> VDMA_REG_SPI_RD_STAT_SPI_RD_STOP_SHFT);
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
BOOL VDMA_SPI_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPI_WR_STAT) & VDMA_REG_SPI_WR_STAT_SPI_WR_STOP_MSK) >> VDMA_REG_SPI_WR_STAT_SPI_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for SPI1.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_ReadEnable(BOOL RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI1_RD_CTRL);
	WRITE_REG(REG_DMA_SPI1_RD_CTRL, 
		(RdEn ? (reg | VDMA_REG_SPI1_RD_CTRL_SPI1_RD_EN_MSK) : (reg & (~VDMA_REG_SPI1_RD_CTRL_SPI1_RD_EN_MSK))));
}

// RdDst = 0 dst is cm3, 1: dst is dsppm
void VDMA_SPI1_ReadDst(BOOL RdDst)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI1_RD_CTRL);
	WRITE_REG(REG_DMA_SPI1_RD_CTRL, 
		(RdDst ? (reg | VDMA_REG_SPI1_RD_CTRL_SPI1_RD_DST_MSK) : (reg & (~VDMA_REG_SPI1_RD_CTRL_SPI1_RD_DST_MSK))));
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for SPI1.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPI1_WR_CTRL);
	WRITE_REG(REG_DMA_SPI1_WR_CTRL, 
		(WrEn ? (reg | VDMA_REG_SPI1_WR_CTRL_SPI1_WR_EN_MSK) : (reg & (~VDMA_REG_SPI1_WR_CTRL_SPI1_WR_EN_MSK))));
}

BOOL VDMA_SPI1_IsReadEnabled(void)
{
	return ((READ_REG(REG_DMA_SPI1_RD_CTRL) & VDMA_REG_SPI1_RD_CTRL_SPI1_RD_EN_MSK)? TRUE:FALSE);	
}

BOOL VDMA_SPI1_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_SPI1_WR_CTRL) & VDMA_REG_SPI1_WR_CTRL_SPI1_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate SPI1 read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPI1_RD_CTRL);
		WRITE_REG(REG_DMA_SPI1_RD_CTRL, (reg | VDMA_REG_SPI1_RD_CTRL_SPI1_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPI1_RD_CTRL);
		WRITE_REG(REG_DMA_SPI1_RD_CTRL, (reg | VDMA_REG_SPI1_RD_CTRL_SPI1_RD_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	To operate SPI1 write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPI1_WR_CTRL);
		WRITE_REG(REG_DMA_SPI1_WR_CTRL, (reg | VDMA_REG_SPI1_WR_CTRL_SPI1_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPI1_WR_CTRL);
		WRITE_REG(REG_DMA_SPI1_WR_CTRL, (reg | VDMA_REG_SPI1_WR_CTRL_SPI1_WR_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	Configure parameters of SPI1 read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_ConfigReadPath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_SPI1_RD_ADD, (Addr << VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_SHFT) & VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_MSK);
	WRITE_REG(REG_DMA_SPI1_RD_LENGTH, Len);	
}

/******************************************************************************
Description:
 	Configure parameters of SPI1 write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPI1_ConfigWritePath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_SPI1_WR_ADD, (Addr << VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_SHFT) & VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_MSK);
	WRITE_REG(REG_DMA_SPI1_WR_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI1_GetReadBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_SPI1_RD_ADD) >> VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_SHFT) & VDMA_REG_SPI1_RD_ADD_SPI1_RD_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI1_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_SPI1_WR_ADD) >> VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_SHFT) & VDMA_REG_SPI1_WR_ADD_SPI1_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI1_GetReadCount(void)
{
	return READ_REG(REG_DMA_SPI1_RD_CNT);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPI1_GetWritenCount(void)
{
	return READ_REG(REG_DMA_SPI1_WR_CNT);
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_SPI1_IsReadStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPI1_RD_STAT) & VDMA_REG_SPI1_RD_STAT_SPI1_RD_STOP_MSK) >> VDMA_REG_SPI1_RD_STAT_SPI1_RD_STOP_SHFT);
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
BOOL VDMA_SPI1_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPI1_WR_STAT) & VDMA_REG_SPI1_WR_STAT_SPI1_WR_STOP_MSK) >> VDMA_REG_SPI1_WR_STAT_SPI1_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for SPI.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_ReadEnable(BOOL RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_BB_RD_CTRL);
	WRITE_REG(REG_DMA_BB_RD_CTRL, 
		(RdEn ? (reg | VDMA_REG_BB_RD_CTRL_BB_RD_EN_MSK) : (reg & (~VDMA_REG_BB_RD_CTRL_BB_RD_EN_MSK))));
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for SPI.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_BB_WR_CTRL);
	WRITE_REG(REG_DMA_BB_WR_CTRL, 
		(WrEn ? (reg | VDMA_REG_BB_WR_CTRL_BB_WR_EN_MSK) : (reg & (~VDMA_REG_BB_WR_CTRL_BB_WR_EN_MSK))));
}

BOOL VDMA_BB_IsReadEnabled(void)
{
	return ((READ_REG(REG_DMA_BB_RD_CTRL) & VDMA_REG_BB_RD_CTRL_BB_RD_EN_MSK)? TRUE:FALSE);	
}

BOOL VDMA_BB_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_BB_WR_CTRL) & VDMA_REG_BB_WR_CTRL_BB_WR_EN_MSK)? TRUE:FALSE);
}

/******************************************************************************
Description:
 	To operate BB read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	No need to clear.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_BB_RD_CTRL);
		WRITE_REG(REG_DMA_BB_RD_CTRL, (reg | VDMA_REG_BB_RD_CTRL_BB_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_BB_RD_CTRL);
		WRITE_REG(REG_DMA_BB_RD_CTRL, (reg | VDMA_REG_BB_RD_CTRL_BB_RD_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	To operate BB write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_BB_WR_CTRL);
		WRITE_REG(REG_DMA_BB_WR_CTRL, (reg | VDMA_REG_BB_WR_CTRL_BB_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_BB_WR_CTRL);
		WRITE_REG(REG_DMA_BB_WR_CTRL, (reg | VDMA_REG_BB_WR_CTRL_BB_WR_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	Configure parameters of BB read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_ConfigReadPath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_BB_RD_ADD, (Addr << VDMA_REG_BB_RD_ADD_BB_RD_ADD_SHFT) & VDMA_REG_BB_RD_ADD_BB_RD_ADD_MSK);
	WRITE_REG(REG_DMA_BB_RD_LENGTH, Len);
}

/******************************************************************************
Description:
 	Configure parameters of BB write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_ConfigWritePath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_BB_WR_ADD, (Addr << VDMA_REG_BB_WR_ADD_BB_WR_ADD_SHFT) & VDMA_REG_BB_WR_ADD_BB_WR_ADD_MSK);
	WRITE_REG(REG_DMA_BB_WR_LENGTH, Len);	
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_BB_GetReadBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_BB_RD_ADD) >> VDMA_REG_BB_RD_ADD_BB_RD_ADD_SHFT) & VDMA_REG_BB_RD_ADD_BB_RD_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_BB_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_BB_WR_ADD) >> VDMA_REG_BB_WR_ADD_BB_WR_ADD_SHFT) & VDMA_REG_BB_WR_ADD_BB_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_BB_GetReadCount(void)
{
	return READ_REG(REG_DMA_BB_RD_CNT);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The count value is only valid after a DMA interrupt () has happened.
	The configured parameters are reset after path disabled.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_BB_GetWritenCount(void)
{
	return READ_REG(REG_DMA_BB_WR_CNT);
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_BB_IsReadStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_BB_RD_STAT) & VDMA_REG_BB_RD_STAT_BB_RD_STOP_MSK) >> VDMA_REG_BB_RD_STAT_BB_RD_STOP_SHFT);
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
BOOL VDMA_BB_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_BB_WR_STAT) & VDMA_REG_BB_WR_STAT_BB_WR_STOP_MSK) >> VDMA_REG_BB_WR_STAT_BB_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable / disable receiving packets from BB.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	Use BB's interrupt instead of DMA's to indicate when a packet has been completely received.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_EnableReceive(BOOL is_enable, UINT32 Addr, UINT32 Len, UINT32 BBLE_sel)
{
	if(is_enable == TRUE) {
		VDMA_BB_ReadAction(VDMA_ACTION_ABORT); // Make sure the DMA starts from a known state.
		VDMA_BB_ReadEnable(FALSE); 
		WRITE_REG(REG_BB_BB_SEL, BBLE_sel);

        /* zuolongjun: Start an extra DMA read to clean residue BB data 
		 * In dual mode, if a BLE connection event interrupts BB while it is receiving a ACL packet(e.g. 2DH5), 
		 * DMA may transfer 4 extra bytes to the head of acl buffer at the next receiving slot. 
		*/
        VDMA_BB_ReadEnable(TRUE);
        VDMA_MaskInt(VDMA_INT_MC_BB_ALL_MSK); 
		VDMA_BB_ConfigReadPath(Addr, Len);
		VDMA_BB_ReadAction(VDMA_ACTION_START);
		VCOMMON_Delay(8); // at least 3us, Measured: 8 -> 5.46us	
		VDMA_BB_ReadAction(VDMA_ACTION_ABORT); // Make sure the DMA starts from a known state.
		VDMA_BB_ReadEnable(FALSE); 
		
		VDMA_BB_ReadEnable(TRUE);
		VDMA_MaskInt(VDMA_INT_MC_BB_ALL_MSK); 
		VDMA_BB_ConfigReadPath(Addr, Len);
		VDMA_BB_ReadAction(VDMA_ACTION_START);	
	}
	else {
		VDMA_BB_ReadAction(VDMA_ACTION_ABORT);
		VDMA_BB_ReadEnable(FALSE); 
	}
}

/******************************************************************************
Description:
 	Enable / disable trasmitting packets to BB.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	Use BB's interrupt instead of DMA's to indicate when a packet has been completely trasmitted.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_BB_EnableTransmit(BOOL is_enable, UINT32 Addr, UINT32 Len, UINT32 BBLE_sel)
{
	if(is_enable == TRUE) {
		WRITE_REG(REG_BB_BB_SEL, BBLE_sel);
		VDMA_BB_WriteAction(VDMA_ACTION_ABORT); // Make sure the DMA starts from a known state.
		VDMA_BB_WriteEnable(FALSE); 
		
		VDMA_BB_WriteEnable(TRUE);
		VDMA_MaskInt(VDMA_INT_MC_BB_ALL_MSK); 
		VDMA_BB_ConfigWritePath(Addr, Len);
		VDMA_BB_WriteAction(VDMA_ACTION_START);	// The start flag will be cleared by HW automatically.
	}
	else {
		VDMA_BB_WriteAction(VDMA_ACTION_ABORT);
		VDMA_BB_WriteEnable(FALSE); 
	}
}

/******************************************************************************
Description:
 	Wait until DMA's transfer count value has reached to a threshold.
Parameters:

Return Value:
	void
Remarks:
	DMA may has not transferred all data to RAM after BB' PKD interrupt.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
BOOL VDMA_BB_WaitTransferCnt(UINT32 threshold)
{
	UINT32 timeout = 5*96/4; // 1us when CPU runs at 96M, 5us is enough.
	UINT32 dma_cnt;
	do {
		dma_cnt = READ_REG(REG_DMA_BB_CRRT_RD_CNT);
		if(dma_cnt >= threshold) {	
            ASSERT((dma_cnt <= threshold + 4) || (threshold==0));
			if((dma_cnt > threshold) && (threshold!=0)) {
				// zuolongjun: DMA may transfer 4 extra bytes at the head of acl buffer.
                VDMA_NPRINTF("BBDMA:%d>%d!\r\n", dma_cnt, threshold);
			}			
			return TRUE;
	    }
		timeout --;
	} while(timeout !=0);
	if (dma_cnt !=0) {
	    VDMA_NPRINTF("BBDMA:%d<%d!\r\n", dma_cnt, threshold);
    }
	return FALSE;
}

#if (VDEV_DMA_SD_SUPPORTED == 1)

/******************************************************************************
Description:
 	Enable/disable DMA read path for SD.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_ReadEnable(BOOL RdEn)
{
//	UINT32 reg;

//	reg = READ_REG(REG_DMA_SD_RD_CTRL);
//	WRITE_REG(REG_DMA_SD_RD_CTRL, 
//		(RdEn ? (reg | VDMA_REG_SD_RD_CTRL_SD_RD_EN_MSK) : (reg & (~VDMA_REG_SD_RD_CTRL_SD_RD_EN_MSK))));
}

// RdDst = 0 dst is cm3, 1: dst is dspdm
void VDMA_SD_ReadDst(BOOL RdDst)
{
//	UINT32 reg;

//	reg = READ_REG(REG_DMA_SD_RD_CTRL);
//	WRITE_REG(REG_DMA_SD_RD_CTRL, 
//		(RdDst ? (reg | VDMA_REG_SD_RD_CTRL_SD_RD_DST_MSK) : (reg & (~VDMA_REG_SD_RD_CTRL_SD_RD_DST_MSK))));
}

void VDMA_SD_SetBitmode(UINT8 mode)
{
//	UINT32 reg;

//	reg = READ_REG(REG_DMA_SD_RD_CTRL);
//	reg = (reg & (~VDMA_REG_SD_RD_CTRL_SD_RD_BITMODE_MSK)) | (mode << VDMA_REG_SD_RD_CTRL_SD_RD_BITMODE_SHFT);
//	WRITE_REG(REG_DMA_SD_RD_CTRL, reg);
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for SD.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_WriteEnable(BOOL WrEn)
{
//	UINT32 reg;

//	reg = READ_REG(REG_DMA_SD_WR_CTRL);
//	WRITE_REG(REG_DMA_SD_WR_CTRL, 
//		(WrEn ? (reg | VDMA_REG_SD_WR_CTRL_SD_WR_EN_MSK) : (reg & (~VDMA_REG_SD_WR_CTRL_SD_WR_EN_MSK))));	
}

BOOL VDMA_SD_IsReadEnabled(void)
{
//	return ((READ_REG(REG_DMA_SD_RD_CTRL) & VDMA_REG_SD_RD_CTRL_SD_RD_EN_MSK)? TRUE:FALSE);	
		return FALSE;
}

BOOL VDMA_SD_IsWriteEnabled(void)
{
//	return ((READ_REG(REG_DMA_SD_WR_CTRL) & VDMA_REG_SD_WR_CTRL_SD_WR_EN_MSK)? TRUE:FALSE);	
		return FALSE;
}

/******************************************************************************
Description:
 	To operate SD read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_ReadAction(VDMA_ACTION Action)
{
	switch (Action) {
	case VDMA_ACTION_START:
//		reg = READ_REG(REG_DMA_SD_RD_CTRL);
//		WRITE_REG(REG_DMA_SD_RD_CTRL, (reg | VDMA_REG_SD_RD_CTRL_SD_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
//		reg = READ_REG(REG_DMA_SD_RD_CTRL);
//		WRITE_REG(REG_DMA_SD_RD_CTRL, (reg | VDMA_REG_SD_RD_CTRL_SD_RD_ABORT_MSK));
		break;
	}	
}

/******************************************************************************
Description:
 	To operate SD write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
//		reg = READ_REG(REG_DMA_SD_WR_CTRL);
//		WRITE_REG(REG_DMA_SD_WR_CTRL, (reg | VDMA_REG_SD_WR_CTRL_SD_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SD_WR_CTRL);
		WRITE_REG(REG_DMA_SD_WR_CTRL, (reg | VDMA_REG_SD_WR_CTRL_SD_WR_ABORT_MSK));
		break;
	}
}

/******************************************************************************
Description:
 	Configure parameters of SD read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_ConfigReadPath(UINT32 Addr, UINT32 Len)
{
//	WRITE_REG(REG_DMA_SD_RD_ADD, (Addr << VDMA_REG_SD_RD_ADD_SD_RD_ADD_SHFT) & VDMA_REG_SD_RD_ADD_SD_RD_ADD_MSK);
//	WRITE_REG(REG_DMA_SD_RD_LENGTH, Len);
}

/******************************************************************************
Description:
 	Configure parameters of SD write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SD_ConfigWritePath(UINT32 Addr, UINT32 Len)
{
//	WRITE_REG(REG_DMA_SD_WR_ADD, (Addr << VDMA_REG_SD_WR_ADD_SD_WR_ADD_SHFT) & VDMA_REG_SD_WR_ADD_SD_WR_ADD_MSK);
//	WRITE_REG(REG_DMA_SD_WR_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SD_GetReadBufAddress(void)
{
//	return (UINT32)((READ_REG(REG_DMA_SD_RD_ADD) >> VDMA_REG_SD_RD_ADD_SD_RD_ADD_SHFT) & VDMA_REG_SD_RD_ADD_SD_RD_ADD_MSK);
		return 0;
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SD_GetWriteBufAddress(void)
{
//	return (UINT32)((READ_REG(REG_DMA_SD_WR_ADD) >> VDMA_REG_SD_WR_ADD_SD_WR_ADD_SHFT) & VDMA_REG_SD_WR_ADD_SD_WR_ADD_MSK);
		return 0;
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SD_GetReadCount(void)
{
//	return READ_REG(REG_DMA_SD_RD_CNT);
		return 0;
}

UINT32 VDMA_SD_GetReadCurtCount(void)
{
//	return READ_REG(REG_DMA_SD_RD_CURTCNT);
		return 0;
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SD_GetWritenCount(void)
{
//	return READ_REG(REG_DMA_SD_WR_CNT);
		return 0;
}

UINT32 VDMA_SD_GetWritenCurtCount(void)
{
//	return READ_REG(REG_DMA_SD_WR_CURTCNT);
		return 0;
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_SD_IsReadStopped(void)
{
//	return (BOOL)((READ_REG(REG_DMA_SD_RD_STAT) & VDMA_REG_SD_RD_STAT_SD_RD_STOP_MSK) >> VDMA_REG_SD_RD_STAT_SD_RD_STOP_SHFT);
		return 0;
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
BOOL VDMA_SD_IsWriteStopped(void)
{
//	return (BOOL)((READ_REG(REG_DMA_SD_WR_STAT) & VDMA_REG_SD_WR_STAT_SD_WR_STOP_MSK) >> VDMA_REG_SD_WR_STAT_SD_WR_STOP_SHFT);
		return 0;
}
#endif //#if (VDEV_DMA_SD_SUPPORTED == 1)

// WrSrc = 0 src is cm3, 1: src is dspdm
void VDMA_LCD_WriteSrc(BOOL WrSrc)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_LCD_WR_CTRL);
	WRITE_REG(REG_DMA_LCD_WR_CTRL, 
		(WrSrc ? (reg | VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_MSK) : (reg & (~VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_MSK))));
}

void VDMA_LCD_SetBitmode(UINT8 mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_LCD_WR_CTRL);
	reg = (reg & (~VDMA_REG_LCD_WR_CTRL_LCD_WR_BITMODE_MSK)) | (mode << VDMA_REG_LCD_WR_CTRL_LCD_WR_BITMODE_SHFT);
	WRITE_REG(REG_DMA_LCD_WR_CTRL, reg);
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for LCD.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_LCD_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_LCD_WR_CTRL);
	WRITE_REG(REG_DMA_LCD_WR_CTRL, 
		(WrEn ? (reg | VDMA_REG_LCD_WR_CTRL_LCD_WR_EN_MSK) : (reg & (~VDMA_REG_LCD_WR_CTRL_LCD_WR_EN_MSK))));
}

BOOL VDMA_LCD_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_LCD_WR_CTRL) & VDMA_REG_LCD_WR_CTRL_LCD_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate LCD write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_LCD_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_LCD_WR_CTRL);
		WRITE_REG(REG_DMA_LCD_WR_CTRL, (reg | VDMA_REG_LCD_WR_CTRL_LCD_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_LCD_WR_CTRL);
		WRITE_REG(REG_DMA_LCD_WR_CTRL, (reg | VDMA_REG_LCD_WR_CTRL_LCD_WR_ABORT_MSK));
		break;
	}
}

/******************************************************************************
Description:
 	Configure parameters of LCD write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_LCD_ConfigWritePath(UINT32 Addr, UINT32 Len)
{
	WRITE_REG(REG_DMA_LCD_WR_ADD, (Addr << VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_SHFT) & VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_MSK);
	WRITE_REG(REG_DMA_LCD_WR_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_LCD_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_LCD_WR_ADD) >> VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_SHFT) & VDMA_REG_LCD_WR_ADD_LCD_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_LCD_GetWritenCount(void)
{
	return READ_REG(REG_DMA_LCD_WR_CNT);
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
BOOL VDMA_LCD_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_LCD_WR_STAT) & VDMA_REG_LCD_WR_STAT_LCD_WR_STOP_MSK) >> VDMA_REG_LCD_WR_STAT_LCD_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for DSPPM.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPPM_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_RAM_DSPPM_CTRL);
	WRITE_REG(REG_DMA_RAM_DSPPM_CTRL, 
		(WrEn ? (reg | VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_EN_MSK) : (reg & (~VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_EN_MSK))));
}

BOOL VDMA_DSPPM_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_RAM_DSPPM_CTRL) & VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate DSPPM write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPPM_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		while (g_VDMA_CM32DSPPM_inUse);
		g_VDMA_CM32DSPPM_inUse = 1;
		reg = READ_REG(REG_DMA_RAM_DSPPM_CTRL);
		WRITE_REG(REG_DMA_RAM_DSPPM_CTRL, (reg | VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_RAM_DSPPM_CTRL);
		WRITE_REG(REG_DMA_RAM_DSPPM_CTRL, (reg | VDMA_REG_DSPPM_WR_CTRL_DSPPM_WR_ABORT_MSK));
		break;
	}
	while (g_VDMA_CM32DSPPM_inUse);	
}

/******************************************************************************
Description:
 	Configure parameters of DSPPM write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPPM_ConfigWritePath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len)
{
	WRITE_REG(REG_DMA_RAM_DSPPM_DESTADD, (DsetAddr << VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_SHFT) & VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_MSK);
	WRITE_REG(REG_DMA_RAM_DSPPM_SRCADD, (SrcAddr << VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_SHFT) & VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_MSK);
	WRITE_REG(REG_DMA_RAM_DSPPM_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPPM_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_RAM_DSPPM_DESTADD) >> VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_SHFT) & VDMA_REG_DSPPM_WR_ADD_DSPPM_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPPM_GetWritenCount(void)
{
	return READ_REG(REG_DMA_RAM_DSPPM_CNT);
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
BOOL VDMA_DSPPM_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_RAM_DSPPM_STAT) & VDMA_REG_DSPPM_WR_STAT_DSPPM_WR_STOP_MSK) >> VDMA_REG_DSPPM_WR_STAT_DSPPM_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for DSPDM.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_ReadEnable(BOOL RdEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_DSPDM_RAM_CTRL);
	WRITE_REG(REG_DMA_DSPDM_RAM_CTRL, 
		(RdEn ? (reg | VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_EN_MSK) : (reg & (~VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_EN_MSK))));
}

void VDMA_DSPDM_SetReadBitmode(UINT8 mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_DSPDM_RAM_CTRL);
	reg = (reg & (~VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_BITMODE_MSK)) | (mode << VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_BITMODE_SHFT);
	WRITE_REG(REG_DMA_DSPDM_RAM_CTRL, reg);
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for DSPDM.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_WriteEnable(BOOL WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_RAM_DSPDM_CTRL);
	WRITE_REG(REG_DMA_RAM_DSPDM_CTRL, 
		(WrEn ? (reg | VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_EN_MSK) : (reg & (~VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_EN_MSK))));
}

void VDMA_DSPDM_SetWriteBitmode(UINT8 mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_RAM_DSPDM_CTRL);
	reg = (reg & (~VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_BITMODE_MSK)) | (mode << VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_BITMODE_SHFT);
	WRITE_REG(REG_DMA_RAM_DSPDM_CTRL, reg);
}

BOOL VDMA_DSPDM_IsReadEnabled(void)
{
	return ((READ_REG(REG_DMA_DSPDM_RAM_CTRL) & VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_EN_MSK)? TRUE:FALSE);	
}

BOOL VDMA_DSPDM_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_RAM_DSPDM_CTRL) & VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate DSPDM read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_ReadAction(VDMA_ACTION Action)
{
	UINT32 reg;

	switch (Action) {
	case VDMA_ACTION_START:
		//while (g_VDMA_DSPDM2CM3_inUse);
		g_VDMA_DSPDM2CM3_inUse = 1;
		reg = READ_REG(REG_DMA_DSPDM_RAM_CTRL);
		WRITE_REG(REG_DMA_DSPDM_RAM_CTRL, (reg | VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_DSPDM_RAM_CTRL);
		WRITE_REG(REG_DMA_DSPDM_RAM_CTRL, (reg | VDMA_REG_DSPDM_RD_CTRL_DSPDM_RD_ABORT_MSK));
		break;
	}
	#if VDEV_BSP32_SUPPORTED
	if(g_VDMA_BSP32_Callback[CODEC_DSPDM2SRAM_PEND]){
		g_VDMA_BSP32_Callback[CODEC_DSPDM2SRAM_PEND](VDMA_ERR_NONE);
		if(g_VDMA_DSPDM2CM3_inUse){
			g_VDMA_DSPDM2CM3_inUse = 0;
			VDMA_DSPDM_ReadEnable(FALSE);
		}
	}else
		while (g_VDMA_DSPDM2CM3_inUse);
	#else
	while (g_VDMA_DSPDM2CM3_inUse);
	#endif
}

/******************************************************************************
Description:
 	To operate DSPDM write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		//while (g_VDMA_CM32DSPDM_inUse);
		g_VDMA_CM32DSPDM_inUse = 1;
		reg = READ_REG(REG_DMA_RAM_DSPDM_CTRL);
		WRITE_REG(REG_DMA_RAM_DSPDM_CTRL, (reg | VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_RAM_DSPDM_CTRL);
		WRITE_REG(REG_DMA_RAM_DSPDM_CTRL, (reg | VDMA_REG_DSPDM_WR_CTRL_DSPDM_WR_ABORT_MSK));
		break;
	}
	#if VDEV_BSP32_SUPPORTED
	if(g_VDMA_BSP32_Callback[CODEC_SRAM2DSPDM_PEND]){
		g_VDMA_BSP32_Callback[CODEC_SRAM2DSPDM_PEND](VDMA_ERR_NONE);
		if(g_VDMA_CM32DSPDM_inUse){
			g_VDMA_CM32DSPDM_inUse = 0;
			VDMA_DSPDM_WriteEnable(FALSE);
		}
	}else
		while (g_VDMA_CM32DSPDM_inUse);
	#else
	while (g_VDMA_CM32DSPDM_inUse);
	#endif
}

/******************************************************************************
Description:
 	Configure parameters of DSPDM read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_ConfigReadPath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len)
{
	WRITE_REG(REG_DMA_DSPDM_RAM_SRCADD, (SrcAddr << VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_SHFT) & VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_MSK);
	WRITE_REG(REG_DMA_DSPDM_RAM_DESTADD, (DsetAddr << VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_SHFT) & VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_MSK);
	WRITE_REG(REG_DMA_DSPDM_RAM_LENGTH, Len);	
}

/******************************************************************************
Description:
 	Configure parameters of DSPDM write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_DSPDM_ConfigWritePath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len)
{
	WRITE_REG(REG_DMA_RAM_DSPDM_SRCADD, (SrcAddr << VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_SHFT) & VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_MSK);
	WRITE_REG(REG_DMA_RAM_DSPDM_DESTADD, (DsetAddr << VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_SHFT) & VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_MSK);
	WRITE_REG(REG_DMA_RAM_DSPDM_LENGTH, Len);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPDM_GetReadBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_DSPDM_RAM_DESTADD) >> VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_SHFT) & VDMA_REG_DSPDM_RD_ADD_DSPDM_RD_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPDM_GetWriteBufAddress(void)
{
	return (UINT32)((READ_REG(REG_DMA_RAM_DSPDM_DESTADD) >> VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_SHFT) & VDMA_REG_DSPDM_WR_ADD_DSPDM_WR_ADD_MSK);
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPDM_GetReadCount(void)
{
	return READ_REG(REG_DMA_DSPDM_RAM_CNT);
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_DSPDM_GetWritenCount(void)
{
	return READ_REG(REG_DMA_RAM_DSPDM_CNT);
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_DSPDM_IsReadStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_DSPDM_RAM_STAT) & VDMA_REG_DSPDM_RD_STAT_DSPDM_RD_STOP_MSK) >> VDMA_REG_DSPDM_RD_STAT_DSPDM_RD_STOP_SHFT);
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
BOOL VDMA_DSPDM_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_RAM_DSPDM_STAT) & VDMA_REG_DSPDM_WR_STAT_DSPDM_WR_STOP_MSK) >> VDMA_REG_DSPDM_WR_STAT_DSPDM_WR_STOP_SHFT);
}

/******************************************************************************
Description:
 	Enable/disable DMA read path for USB.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_ReadEnable(VDMA_USB_PATH chanel, BOOL RdEn)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;

    if(chanel >= VDMA_USB_SRAM3){
        UINT8 shift = (chanel-VDMA_USB_SRAM3);
        reg = READ_REG(REG_DMA_USB_RD_CTRL1);
        reg = RdEn ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK <<  shift)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK <<  shift)));
    
        WRITE_REG(REG_DMA_USB_RD_CTRL1, reg);
    }else{      
        reg = READ_REG(REG_DMA_USB_RD_CTRL);
        reg = RdEn ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << chanel)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << chanel)));

        WRITE_REG(REG_DMA_USB_RD_CTRL, reg);
    }
}

void VDMA_USB_ReadUrbSize(VDMA_USB_PATH chanel, BOOL size)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;
    
    reg = READ_REG(REG_DMA_USB_RD_CTRL);
    reg = size ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_SIZE_MSK << chanel)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_SIZE_MSK << chanel)));
    
    WRITE_REG(REG_DMA_USB_RD_CTRL, reg);
}

/******************************************************************************
Description:
 	Enable/disable DMA write path for USB.
Parameters:
 	RdEn - TRUE for enable, FALSE for disable
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_WriteEnable(VDMA_USB_PATH chanel, BOOL WrEn)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;

    if(chanel >= VDMA_USB_SRAM3){
        UINT8 shift = chanel-VDMA_USB_SRAM3;
        reg = READ_REG(REG_DMA_USB_WR_CTRL1);
        reg = WrEn ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << shift)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << shift)));
        
        WRITE_REG(REG_DMA_USB_WR_CTRL1, reg);
    }else{  
        reg = READ_REG(REG_DMA_USB_WR_CTRL);
        reg = WrEn ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << chanel)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_EN_MSK << chanel)));
        
        WRITE_REG(REG_DMA_USB_WR_CTRL, reg);
    }
}

void VDMA_USB_WriteUrbSize(VDMA_USB_PATH chanel, BOOL size)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;
    
    reg = READ_REG(REG_DMA_USB_WR_CTRL);
    reg = size ? (reg | (VDMA_REG_USB_CTRL_USB_SRAM_SIZE_MSK << chanel)) : (reg & (~(VDMA_REG_USB_CTRL_USB_SRAM_SIZE_MSK << chanel)));
    
    WRITE_REG(REG_DMA_USB_WR_CTRL, reg);
}

void VDMA_USB_WriteLst(VDMA_USB_PATH chanel, BOOL lst)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;

    reg = READ_REG(REG_DMA_USB_WR_CTRL);
    reg = lst ? (reg | (VDMA_REG_USB_WR_CTRL_USB_SRAM_LST_MSK << chanel)) : (reg & (~(VDMA_REG_USB_WR_CTRL_USB_SRAM_LST_MSK << chanel)));

    WRITE_REG(REG_DMA_USB_WR_CTRL, reg);
}

/******************************************************************************
Description:
 	To operate USB read path of DMA. All of these actions can work after read path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to read
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_ReadAction(VDMA_USB_PATH chanel, VDMA_ACTION Action)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;

    if(chanel >= VDMA_USB_SRAM3){
        UINT8 shift = (chanel-VDMA_USB_SRAM3);
        reg = READ_REG(REG_DMA_USB_RD_CTRL1);
    
        switch (Action) {
            case VDMA_ACTION_START:
            WRITE_REG(REG_DMA_USB_RD_CTRL1, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_START_MSK << shift)));
            break;
            case VDMA_ACTION_ABORT:
            WRITE_REG(REG_DMA_USB_RD_CTRL1, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_ABORT_MSK << shift)));
            break;
        }
    }else{
        reg = READ_REG(REG_DMA_USB_RD_CTRL);
        
        switch (Action) {
        case VDMA_ACTION_START:
            WRITE_REG(REG_DMA_USB_RD_CTRL, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_START_MSK << chanel)));
            break;
        case VDMA_ACTION_ABORT:
            WRITE_REG(REG_DMA_USB_RD_CTRL, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_ABORT_MSK << chanel)));
            break;
        }
    }
}

/******************************************************************************
Description:
 	To operate USB write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_WriteAction(VDMA_USB_PATH chanel, VDMA_ACTION Action)
{
    UINT32 reg;

    if (chanel > VDMA_USB_PATH_LIMIT)
        return;

    if(chanel >= VDMA_USB_SRAM3){
        UINT8 shift = (chanel-VDMA_USB_SRAM3);
        reg = READ_REG(REG_DMA_USB_WR_CTRL1);

        switch (Action) {
        case VDMA_ACTION_START:
            WRITE_REG(REG_DMA_USB_WR_CTRL1, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_START_MSK << shift)));
            break;
        case VDMA_ACTION_ABORT:
            WRITE_REG(REG_DMA_USB_WR_CTRL1, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_ABORT_MSK << shift)));
            break;
        }            
    }else{
        reg = READ_REG(REG_DMA_USB_WR_CTRL);

        switch (Action) {
        case VDMA_ACTION_START:
            WRITE_REG(REG_DMA_USB_WR_CTRL, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_START_MSK << chanel)));
            break;
        case VDMA_ACTION_ABORT:
            WRITE_REG(REG_DMA_USB_WR_CTRL, (reg | (VDMA_REG_USB_CTRL_USB_SRAM_ABORT_MSK << chanel)));
            break;
        }
    }
}

/******************************************************************************
Description:
 	Configure parameters of USB read path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_ConfigReadPath(VDMA_USB_PATH chanel, UINT32 Addr, UINT32 Len)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
            return;
    if(chanel == VDMA_USB_SRAM0) {
        WRITE_REG(REG_DMA_USB_RD_RAM0_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM0_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM1) {
        WRITE_REG(REG_DMA_USB_RD_RAM1_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM1_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM2) {
        WRITE_REG(REG_DMA_USB_RD_RAM2_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM2_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM3) {
        WRITE_REG(REG_DMA_USB_RD_RAM3_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM3_LENGTH, Len);        
    }
    else if(chanel == VDMA_USB_SRAM4) {
        WRITE_REG(REG_DMA_USB_RD_RAM4_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM4_LENGTH, Len);           
    }       
    else if(chanel == VDMA_USB_SRAM5) {
        WRITE_REG(REG_DMA_USB_RD_RAM5_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_RD_RAM5_LENGTH, Len);           
    }
       
}

/******************************************************************************
Description:
 	Configure parameters of USB write path, including memory address and transmission size.
Parameters:
 	Addr - memory address for data
 	Len - size in byte
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_USB_ConfigWritePath(VDMA_USB_PATH chanel, UINT32 Addr, UINT32 Len)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
            return;
    if(chanel == VDMA_USB_SRAM0) {
        WRITE_REG(REG_DMA_USB_WR_RAM0_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM0_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM1) {
        WRITE_REG(REG_DMA_USB_WR_RAM1_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM1_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM2) {
        WRITE_REG(REG_DMA_USB_WR_RAM2_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM2_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM3) {
        WRITE_REG(REG_DMA_USB_WR_RAM3_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM3_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM4) {
        WRITE_REG(REG_DMA_USB_WR_RAM4_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM4_LENGTH, Len);
    }
    else if(chanel == VDMA_USB_SRAM5) {
        WRITE_REG(REG_DMA_USB_WR_RAM5_ADD, (Addr << VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
        WRITE_REG(REG_DMA_USB_WR_RAM5_LENGTH, Len);
    }
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_USB_GetReadBufAddress(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;
    if(chanel == VDMA_USB_SRAM0) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM0_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if(chanel == VDMA_USB_SRAM1) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM1_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if(chanel == VDMA_USB_SRAM2) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM2_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if(chanel == VDMA_USB_SRAM3) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM3_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if(chanel == VDMA_USB_SRAM4) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM4_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if(chanel == VDMA_USB_SRAM5) {
        return (UINT32)((READ_REG(REG_DMA_USB_RD_RAM5_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    return 0;
}

/******************************************************************************
Description:
 	Get the configured memory address
Parameters:
 	void
Return Value:
	The memory address
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_USB_GetWriteBufAddress(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;
    if(chanel == VDMA_USB_SRAM0) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM0_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else if (chanel == VDMA_USB_SRAM1) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM1_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else  if (chanel == VDMA_USB_SRAM2) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM2_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else  if (chanel == VDMA_USB_SRAM3) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM3_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else  if (chanel == VDMA_USB_SRAM4) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM4_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    else  if (chanel == VDMA_USB_SRAM5) {
        return (UINT32)((READ_REG(REG_DMA_USB_WR_RAM5_ADD) >> VDMA_REG_USB_ADD_USB_SRAM_ADD_SHFT) & VDMA_REG_USB_ADD_USB_SRAM_ADD_MSK);
    }
    return 0;
}

/******************************************************************************
Description:
 	Get the read count to memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The read count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_USB_GetReadCount(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;
    if(chanel == VDMA_USB_SRAM0) {
        return READ_REG(REG_DMA_USB_RD_RAM0_CNT);
    }
    else if(chanel == VDMA_USB_SRAM1) {
        return READ_REG(REG_DMA_USB_RD_RAM1_CNT);
    }
    else if(chanel == VDMA_USB_SRAM2) {
        return READ_REG(REG_DMA_USB_RD_RAM2_CNT);
    }
    else if(chanel == VDMA_USB_SRAM3) {
        return READ_REG(REG_DMA_USB_RD_RAM3_CNT);
    }
    else if(chanel == VDMA_USB_SRAM4) {
        return READ_REG(REG_DMA_USB_RD_RAM4_CNT);
    }
    else if(chanel == VDMA_USB_SRAM5) {
        return READ_REG(REG_DMA_USB_RD_RAM5_CNT);
    }	
    return 0;
}

UINT32 VDMA_USB_GetReadCurtCount(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;

    if(chanel == VDMA_USB_SRAM0) {
        return READ_REG(REG_DMA_USB_RD_RAM0_CURTCNT);
    }
    else if (chanel == VDMA_USB_SRAM1) {
        return READ_REG(REG_DMA_USB_RD_RAM0_CURTCNT);
    }
    else  if (chanel == VDMA_USB_SRAM2) {
        return READ_REG(REG_DMA_USB_RD_RAM2_CURTCNT);
    }
    else  if (chanel == VDMA_USB_SRAM3) {
        return READ_REG(REG_DMA_USB_RD_RAM3_CURTCNT);
    }
    else  if (chanel == VDMA_USB_SRAM4) {
        return READ_REG(REG_DMA_USB_RD_RAM4_CURTCNT);
    }
    else  if (chanel == VDMA_USB_SRAM5) {
        return READ_REG(REG_DMA_USB_RD_RAM5_CURTCNT);
    }	
    return 0;
}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_USB_GetWritenCount(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;

    if(chanel == VDMA_USB_SRAM0) {
        return READ_REG(REG_DMA_USB_WR_RAM0_CNT);
    }
    else if (chanel == VDMA_USB_SRAM1) {
        return READ_REG(REG_DMA_USB_WR_RAM1_CNT);
    }
    else  if (chanel == VDMA_USB_SRAM2)  {
        return READ_REG(REG_DMA_USB_WR_RAM2_CNT);
    }
    else  if (chanel == VDMA_USB_SRAM3)  {
        return READ_REG(REG_DMA_USB_WR_RAM3_CNT);
    }
    else  if (chanel == VDMA_USB_SRAM4)  {
        return READ_REG(REG_DMA_USB_WR_RAM4_CNT);
    }
    else  if (chanel == VDMA_USB_SRAM5)  {
        return READ_REG(REG_DMA_USB_WR_RAM5_CNT);
    }	
    return 0;
}

/******************************************************************************
Description:
 	To tell if read path stopped/aborted.
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
BOOL VDMA_USB_IsReadStopped(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;

    if(chanel == VDMA_USB_SRAM0) {
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT) & VDMA_REG_USB_STAT_USB_SRAM0_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM0_STOP_SHFT);
    }
    else if(chanel == VDMA_USB_SRAM1) {
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT) & VDMA_REG_USB_STAT_USB_SRAM1_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM1_STOP_SHFT);
    }
    else if(chanel == VDMA_USB_SRAM2){
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT) & VDMA_REG_USB_STAT_USB_SRAM2_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM2_STOP_SHFT);
    }
    else if(chanel == VDMA_USB_SRAM3){
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT1) & VDMA_REG_USB_STAT_USB_SRAM3_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM3_STOP_SHFT);
    }
    else if(chanel == VDMA_USB_SRAM4){
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT1) & VDMA_REG_USB_STAT_USB_SRAM4_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM4_STOP_SHFT);
    }
    else if(chanel == VDMA_USB_SRAM5){
        return (BOOL)((READ_REG(REG_DMA_USB_RD_STAT1) & VDMA_REG_USB_STAT_USB_SRAM5_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM5_STOP_SHFT);
    }	
    return FALSE;
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
BOOL VDMA_USB_IsWriteStopped(VDMA_USB_PATH chanel)
{
    if (chanel > VDMA_USB_PATH_LIMIT)
        return 0;

    if(chanel == VDMA_USB_SRAM0) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT) & VDMA_REG_USB_STAT_USB_SRAM0_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM0_STOP_SHFT);
    }
    else if (chanel == VDMA_USB_SRAM1) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT) & VDMA_REG_USB_STAT_USB_SRAM1_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM1_STOP_SHFT);
    }
    else if (chanel == VDMA_USB_SRAM2) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT) & VDMA_REG_USB_STAT_USB_SRAM2_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM2_STOP_SHFT);
    }
    else if (chanel == VDMA_USB_SRAM3) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT1) & VDMA_REG_USB_STAT_USB_SRAM3_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM3_STOP_SHFT);
    }
    else if (chanel == VDMA_USB_SRAM4) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT1) & VDMA_REG_USB_STAT_USB_SRAM4_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM4_STOP_SHFT);
    }
    else if (chanel == VDMA_USB_SRAM5) {
        return (BOOL)((READ_REG(REG_DMA_USB_WR_STAT1) & VDMA_REG_USB_STAT_USB_SRAM5_STOP_MSK) >> VDMA_REG_USB_STAT_USB_SRAM5_STOP_SHFT);
    }	
    return FALSE;
}

/******************************************************************************
Description:
 	Get the current read count to memory. 
Parameters:
 	void
Return Value:
	The read count
Remarks:
	
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_BB_GetReadCrrtCount(void)
{
	return READ_REG(REG_DMA_BB_CRRT_RD_CNT);
}


#if 0//(VDEV_DMA_SPDIF_SUPPORTED == 1)
// WrSrc = 0 src is cm3, 1: src is dspdm
void VDMA_SPDIF_WriteSrc(BOOL WrSrc)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
	WRITE_REG(REG_DMA_SPDIF_WR_CTRL, 
		(WrSrc ? (reg | VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_MSK) : (reg & (~VDMA_REG_LCD_WR_CTRL_LCD_WR_DST_MSK))));
}

void VDMA_SPDIF_SetBitmode(UINT8 mode)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
	reg = (reg & (~VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_MSK)) | (mode << VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_SHFT);
	WRITE_REG(REG_DMA_SPDIF_WR_CTRL, reg);
}


BOOL VDMA_SPDIF_IsWriteEnabled(void)
{
	return ((READ_REG(REG_DMA_SPDIF_WR_CTRL) & VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_MSK)? TRUE:FALSE);	
}

/******************************************************************************
Description:
 	To operate SPDIF write path of DMA. All of these actions can work after write path enable.
Parameters:
 	Action - one of below definitions,
 			VDMA_ACTION_START //start to write
			VDMA_ACTION_ABORT //abort current transmission inmmediately
Return Value:
	void
Remarks:
	None.
Example:
 	None.
See Also:
	None.
*****************************************************************************/
void VDMA_SPDIF_WriteAction(VDMA_ACTION Action)
{
	UINT32 reg;
	
	switch (Action) {
	case VDMA_ACTION_START:
		reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
		WRITE_REG(REG_DMA_SPDIF_WR_CTRL, (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_START_MSK));
		break;
	case VDMA_ACTION_ABORT:
		reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
		WRITE_REG(REG_DMA_SPDIF_WR_CTRL, (reg | VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_ABORT_MSK));
		break;
	}	
}

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
void VDMA_SPDIF_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop)
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
void VDMA_SPDIF_WriteEnable(UINT8 WrEn)
{
	UINT32 reg;

	reg = READ_REG(REG_DMA_SPDIF_WR_CTRL);
	WRITE_REG(REG_DMA_SPDIF_WR_CTRL, ((WrEn << VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_SHFT) | 
									(reg & (~VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_MSK))));

}

/******************************************************************************
Description:
 	Get the written count from memory. This number will be updated at the end of each DMA
 	transmission, which means this function should be called in or after read interrupt.
Parameters:
 	void
Return Value:
	The written count
Remarks:
	The configured parameters are reset after path disabled
Example:
 	None.
See Also:
	None.
*****************************************************************************/
UINT32 VDMA_SPDIF_GetWritenCount(void)
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
BOOL VDMA_SPDIF_IsWriteStopped(void)
{
	return (BOOL)((READ_REG(REG_DMA_SPDIF_WR_STAT) & VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_STOP_MSK) >> VDMA_REG_LCD_WR_STAT_LCD_WR_STOP_SHFT);
}

#endif	// #if (VDEV_DMA_SPDIF_SUPPORTED == 1)

void VDMA_Chick_En(UINT8 id, BOOL en)
{
	uint32_t reg = READ_REG(REG_DMA_CHICKEN);
    
	if (en) {
        reg |= (1 << id);
    }
	else {
		reg &= ~(1 << id);
	}
    WRITE_REG(REG_DMA_CHICKEN, reg);
}
