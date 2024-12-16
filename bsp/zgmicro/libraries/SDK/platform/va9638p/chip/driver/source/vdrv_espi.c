/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2013, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    VESPI

Description:

Revision History:
    Date                  Author                   Description
    2021.05.12           caoxuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vdrv_espi.h"
#include "vdrv_gpio.h"
#include "vdrv_dma.h"
#include "vstdcore.h"
#include "vreg_def.h"

/******************************************************************************
Description:

Parameters:

Return Value:
    None.

Notes:

*****************************************************************************/
void VESPI_Init ( void )
{
    VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_ESPI_AUX, GPIO_MUX_FUNC_ESPI_AUX);
    VGPIO_SetLMux2Function(GPIO_MUX_FUNC_ESPI_AUX, GPIO_MUX_FUNC_ESPI_AUX);

    CLR_REG_BIT2(REG_DMA_CHICKEN, 3);
    VESPI_ClkSelect(3);
	VCPU_INT_SrcEnable(IRQ_DMA_INT);
}

/******************************************************************************
Description:
    Read a register from analog test sub board.

Parameters:
    addr - lower 24 bits of the register address.

Return Value:
    None.

Notes:

*****************************************************************************/
VRET_VALUE VESPI_ReadReg ( UINT32 addr, UINT32 *reg_val )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 reg_val_tmp;
    UINT32 timeout = 0xfff;
    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    // check parameters.
    WRITE_REG(REG_SPI2_SPI2_CTRL_1, ((0<<VESPI_CTRLR1_RW_SEL_SHFT)|(1<<VESPI_CTRLR1_EN_SHFT)|(1 <<VESPI_CTRLR1_CLK_SHFT)));
    WRITE_REG(REG_SPI2_SPI2_CTRL_2, ((4<<VESPI_CTRLR2_SSH_SHFT)|(1<<VESPI_CTRLR2_POST_LIMIT_SHFT)|(1<<VESPI_CTRLR2_PRE_LIMIT_SHFT)));
    WRITE_REG(REG_SPI2_SPI2_CMD_ADDR, ESPI_ReadData_CMD | (addr <<8));
        
    reg_val_tmp = (3 <<VESPI_LENR_ADDR_SHFT) | (2 <<VESPI_LENR_DUMMY_SHFT) | (4 <<VESPI_LENR_DATA_SHFT);
    WRITE_REG(REG_SPI2_SPI2_LENGTH, reg_val_tmp);
    
    WRITE_REG(REG_SPI2_SPI2_TRANS_START, 1);

    while ( timeout ) {
        if ( GET_REG_BIT2 ( REG_SPI2_SPI2_STATUS,REG_SPI2_SPI2_STATUS_trans_done ) ) { // After a new start, this bit will automatically cleared.
            break;
        }
        timeout--;
    }
    if ( timeout ) {
        if ( reg_val )
            ( *reg_val ) = READ_REG ( REG_SPI2_SPI2_DATA_BUF_RD );
    } else {
        err = VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;
    }

    CLR_REG_BIT2 (REG_SPI2_SPI2_CTRL_1, REG_SPI2_SPI2_CTRL_1_spi_enable );
    VCPU_CRITICAL_EXIT();
    return err;
}

/******************************************************************************
Description:
    Write a register to analog test sub board.

Parameters:

Return Value:

Notes:
    addr - lower 24 bits of the register address.

*****************************************************************************/
VRET_VALUE VESPI_WriteReg ( UINT32 addr, UINT32 reg_val )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 reg_val_tmp;
    UINT32 timeout = 0xfff;
    
    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    WRITE_REG(REG_SPI2_SPI2_CTRL_1, ((1<<VESPI_CTRLR1_RW_SEL_SHFT)|(1<<VESPI_CTRLR1_EN_SHFT)|(1 <<VESPI_CTRLR1_CLK_SHFT)));
    WRITE_REG(REG_SPI2_SPI2_CTRL_2, ((4<<VESPI_CTRLR2_SSH_SHFT)|(1<<VESPI_CTRLR2_POST_LIMIT_SHFT)|(1<<VESPI_CTRLR2_PRE_LIMIT_SHFT)));
    WRITE_REG(REG_SPI2_SPI2_CMD_ADDR, ESPI_WriteData_CMD | (addr <<8));

    reg_val_tmp = (3 <<VESPI_LENR_ADDR_SHFT) | (0 <<VESPI_LENR_DUMMY_SHFT) | (4 <<VESPI_LENR_DATA_SHFT) ;
    WRITE_REG(REG_SPI2_SPI2_LENGTH, reg_val_tmp);
    WRITE_REG(REG_SPI2_SPI2_DATA_BUF_WR, reg_val);
    
    WRITE_REG(REG_SPI2_SPI2_TRANS_START, 1);

  while ( timeout ) {
        if ( GET_REG_BIT2 ( REG_SPI2_SPI2_STATUS,REG_SPI2_SPI2_STATUS_trans_done ) ) { // After a new start, this bit will automatically cleared.
            break;
        }
        timeout--;
    }
    if ( !timeout )
        err = VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;

    CLR_REG_BIT2 (REG_SPI2_SPI2_CTRL_1, REG_SPI2_SPI2_CTRL_1_spi_enable );
    VCPU_CRITICAL_EXIT();
    return err;
}

/***********************
DMA ESPI CallBack function
***********************/
static volatile BOOL _VESPI_DMA_IsTransferDone = FALSE;
static volatile VDMA_ERR _VESPI_DMA_Error = (VDMA_ERR)VRET_NO_ERROR;

static void VESPI_DMA_Disable(void);
/******************************************************************************
Description:
 	VESPI DMA IntCallBack.
Parameters:
Return Value:
Notes:
*****************************************************************************/
static void VESPI_DMA_IntCallBack(VDMA_ERR ErrorCode) 
{ 	
	VESPI_DMA_Disable();
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	_VESPI_DMA_IsTransferDone = TRUE;
	_VESPI_DMA_Error = ErrorCode;
	VCPU_CRITICAL_EXIT();	
}

static void VESPI_DMA_StartRead(UINT32 Addr, UINT32 Len) 
{
	VESPI_DMA_Disable();
	VDMA_UnmaskInt_1(VDMA_INT_MC_SPI1_SRAM_DONE_MSK_1);
	VDMA_SPI1_ReadDst(0);
	VDMA_SPI1_ReadEnable(TRUE);
	VDMA_SPI1_ConfigReadPath(Addr, Len); 
	VDMA_SPI1_ReadAction(VDMA_ACTION_START);
}
static void VESPI_DMA_StartWrite(UINT32 Addr, UINT32 Len) 
{
	VESPI_DMA_Disable();
	VDMA_UnmaskInt_1(VDMA_INT_MC_SRAM_SPI1_DONE_MSK_1);
	VDMA_SPI1_WriteEnable(TRUE);
	VDMA_SPI1_ConfigWritePath(Addr, Len); 
	VDMA_SPI1_WriteAction(VDMA_ACTION_START);
}

static void VESPI_DMA_Disable(void) 
{
	VDMA_MaskInt_1(VDMA_INT_MC_SPI1_SRAM_DONE_MSK_1 | VDMA_INT_MC_SRAM_SPI1_DONE_MSK_1);
	VDMA_SPI1_WriteAction(VDMA_ACTION_ABORT);
	VDMA_SPI1_WriteEnable(FALSE);
	VDMA_SPI1_ReadAction(VDMA_ACTION_ABORT);
	VDMA_SPI1_ReadEnable(FALSE);
}

static VRET_VALUE VESPI_DMA_WaitTransferDone(UINT32 timeout) 
{ 
	VRET_VALUE err = VRET_NO_ERROR;
	while(timeout) {
		if(_VESPI_DMA_IsTransferDone == TRUE) {			
			err = _VESPI_DMA_Error;
			VCPU_SR_ALLOC();
	        VCPU_CRITICAL_ENTER();
			_VESPI_DMA_IsTransferDone = FALSE;
			_VESPI_DMA_Error = (VDMA_ERR)VRET_NO_ERROR;
			VCPU_CRITICAL_EXIT();
			return err;
		}
		timeout --;
	}
	return VRET_ERROR;
}

/******************************************************************************
Description:
 	Write data to SPI bus via DMA.

Parameters:
 	buf - 
 	siz - 
	
Return Value:

Notes:
	
*****************************************************************************/
VRET_VALUE VESPI_WriteData_DMA(const UINT8 *buf, UINT32 siz)
{	
	VESPI_DMA_StartWrite((UINT32)buf, siz);
	return VRET_NO_ERROR;	
}


VRET_VALUE VESPI_ReadData_DMA(UINT8 *buf, UINT32 siz)
{
	VESPI_DMA_StartRead((UINT32)buf, siz);
	return VRET_NO_ERROR;
}

void VESPI_SetCmdAddr(UINT8 cmd, UINT32 addr)
{
    WRITE_REG(REG_SPI2_SPI2_CMD_ADDR, cmd | (addr <<8));
}

void VESPI_SetLength(UINT32 addr_bytes, UINT32 dummy_bytes, UINT32 data_len)
{
    UINT32 reg_val;
    reg_val = (addr_bytes <<VESPI_LENR_ADDR_SHFT)|(dummy_bytes <<VESPI_LENR_DUMMY_SHFT)|(data_len <<VESPI_LENR_DATA_SHFT);
    WRITE_REG(REG_SPI2_SPI2_LENGTH, reg_val);    
}
void VESPI_SetClockLimit(UINT8 pre_limit,UINT8 post_limit,UINT8 ssh_high)
{
    UINT32 reg_val;
    UINT32 mask;
    
    mask = (VESPI_CTRLR2_PRE_LIMIT_MASK <<VESPI_CTRLR2_PRE_LIMIT_SHFT) | \
                 (VESPI_CTRLR2_POST_LIMIT_MASK <<VESPI_CTRLR2_POST_LIMIT_SHFT) | \
                 (VESPI_CTRLR2_SSH_MASK <<VESPI_CTRLR2_SSH_SHFT);
    reg_val = ((pre_limit & VESPI_CTRLR2_PRE_LIMIT_MASK) << VESPI_CTRLR2_PRE_LIMIT_SHFT) |\
                        ((post_limit & VESPI_CTRLR2_POST_LIMIT_MASK) << VESPI_CTRLR2_POST_LIMIT_SHFT) |\
                        ((ssh_high & VESPI_CTRLR2_SSH_MASK) << VESPI_CTRLR2_SSH_SHFT);
    reg_val = ((READ_REG(REG_SPI2_SPI2_CTRL_2) & (~mask)) | reg_val);

    WRITE_REG(REG_SPI2_SPI2_CTRL_2, reg_val);
}

void VESPI_StartTransfer()
{
    WRITE_REG(REG_SPI2_SPI2_TRANS_START, 1);
}
/******************************************************************************
Description:
     Write data to ESPI bus.

Parameters:
    
Return Value:

Notes:
    Can write at most 4 bytes as the write buffer is only one register.
    
*****************************************************************************/
VRET_VALUE VESPI_WriteData_Reg(const UINT8 *buf, UINT32 siz)
{
    UINT32 reg_val = 0;

     if(siz >4) return VRET_DRV_SPI_ERR_INVALID_PARAM;

    if(siz == 1)
        reg_val = buf[0];
    else if(siz == 2)
        reg_val = buf[0] | (buf[1] <<8);
    else if(siz == 3)
        reg_val = buf[0] | (buf[1] <<8) | (buf[2] <<16);
    else if(siz == 4)
        reg_val = buf[0] | (buf[1] <<8) | (buf[2] <<16) | (buf[3] <<24);
    
    WRITE_REG(REG_SPI2_SPI2_DATA_BUF_WR, reg_val);
    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
     Read data from ESPI bus.

Parameters:
    
Return Value:

Notes:
    Can read at most 4 bytes as the read buffer is only one register.
    
*****************************************************************************/
VRET_VALUE VESPI_ReadData_Reg(UINT8 *buf, UINT32 siz)
{
    UINT32 reg_val;

    if(siz >4) return VRET_DRV_SPI_ERR_INVALID_PARAM;
    
    reg_val = READ_REG(REG_SPI2_SPI2_DATA_BUF_RD);
    if(siz >= 1)
        buf[0] = (UINT8)reg_val;

    if(siz >= 2)
        buf[1] = (UINT8)(reg_val >>8);

    if(siz >= 3)
        buf[2] = (UINT8)(reg_val >>16);

    if(siz >= 4)
        buf[3] = (UINT8)(reg_val >>24);

    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
     Wait until transfer done.

Parameters:
     timeout - Depend on the number of bytes to tranfer and the SPI clock.
    
Return Value:
    When DMA is used, and SPI in
    1) Read mode: DMA transfer done interrupt comes later than SPI transfer done flag is set.
    2) Write mode: DMA transfer done interrupt comes earlier than SPI transfer done flag is set.
Notes:
    
*****************************************************************************/
VRET_VALUE VESPI_WaitTransferDone(UINT32 count)
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 reg_val;
    UINT32 len;
    UINT32 timeout;
	BOOL use_dma;

    reg_val = READ_REG(REG_SPI2_SPI2_LENGTH);
    len = ((reg_val >>VESPI_LENR_DATA_SHFT) & VESPI_LENR_DATA_MASK) + \
        ((reg_val >>VESPI_LENR_DUMMY_SHFT) & VESPI_LENR_DUMMY_MASK) + \
        ((reg_val >>VESPI_LENR_ADDR_SHFT) & VESPI_LENR_ADDR_MASK) + 1;


    reg_val = (GET_REG_BIT2(REG_SPI2_SPI2_CTRL_1, VESPI_CTRLR1_CLK_SHFT));
    timeout = (count+len) * 8000 * (1<<reg_val);

	use_dma = GET_REG_BIT2(REG_SPI2_SPI2_CTRL_1, VESPI_CTRLR1_DMAM_SHFT);
    while(timeout) {
        if(GET_REG_BIT2(REG_SPI2_SPI2_STATUS, VESPI_SR_TRANSFER_DONE_SHFT)){
              break;
        }
        timeout --;
    }
    
    if(timeout == 0) err = VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;
    if(err != VRET_NO_ERROR) return err;

	if(use_dma) {
		VESPI_DMA_WaitTransferDone(10000);
	}
    return err;    
}

/******************************************************************************
Description:
     select espi_clk devide from fast_clock

Parameters:    
    0  -  clk_fast
  1  -  clk_fast/2
  2  -  clk_fast/4
  3  -  clk_fast/8
    
Return Value:
    None.

Notes:
    
*****************************************************************************/
void VESPI_ClkSelect(UINT8 Clk_Sel)
{
    UINT32 reg_val;
    UINT32 mask;
    
    mask = (VESPI_CTRLR1_CLK_MASK <<VESPI_CTRLR1_CLK_SHFT);
    reg_val = (READ_REG(REG_SPI2_SPI2_CTRL_1) & (~mask)) | (Clk_Sel <<VESPI_CTRLR1_CLK_SHFT);
    WRITE_REG(REG_SPI2_SPI2_CTRL_1, reg_val);
}

UINT8 VESPI_GetClk(void)
{
    UINT32 reg_val;
    UINT32 mask;
    
    mask = (VESPI_CTRLR1_CLK_MASK <<VESPI_CTRLR1_CLK_SHFT);
    reg_val =    READ_REG(REG_SPI2_SPI2_CTRL_1);
    
    return ((reg_val & mask) >> VESPI_CTRLR1_CLK_SHFT);
}

/******************************************************************************
Description:
     Enable and config the ESPI peripheral.

Parameters:
     is_write - Is it a write (program, erase, etc.) command.
     use_dma - Whether use DMA for transfering data.
     dual_mode - Both SI and SO are used to transfer data.
    
Return Value:
    None.

Notes:
    
*****************************************************************************/
void VESPI_ConfigMaster(BOOL is_write, BOOL use_dma, UINT8 dual_mode)
{
    UINT32 reg_val;
	if(use_dma) {
		VDMA_RegisterISRCallback_1(VDMA_DEV1_SPI12SRAM,TRUE,VESPI_DMA_IntCallBack);	
		VDMA_RegisterISRCallback_1(VDMA_DEV1_SRAM2SPI1,TRUE,VESPI_DMA_IntCallBack);	
	}
	
    reg_val = READ_REG(REG_SPI2_SPI2_CTRL_1);
    reg_val &= ~(VESPI_CTRLR1_RW_SEL_MASK << VESPI_CTRLR1_RW_SEL_SHFT);
    reg_val &= ~(VESPI_CTRLR1_DMAM_MASK << VESPI_CTRLR1_DMAM_SHFT);
    reg_val |= (is_write << VESPI_CTRLR1_RW_SEL_SHFT);
    reg_val |= (use_dma << VESPI_CTRLR1_DMAM_SHFT);
    WRITE_REG(REG_SPI2_SPI2_CTRL_1,reg_val);

    reg_val = READ_REG(REG_SPI2_SPI2_CTRL_2);
    reg_val &= ~(VESPI_CTRLR2_DUAL_ADDR_MASK << VESPI_CTRLR2_DUAL_ADDR_SHFT);
    reg_val &= ~(VESPI_CTRLR2_DUAL_DATA_MASK << VESPI_CTRLR2_DUAL_DATA_SHFT);
    reg_val |= (dual_mode << VESPI_CTRLR2_DUAL_ADDR_MASK);
    reg_val |= (dual_mode << VESPI_CTRLR2_DUAL_DATA_MASK);
    WRITE_REG(REG_SPI2_SPI2_CTRL_2,reg_val);
    VESPI_SetEnable(TRUE);
}

/******************************************************************************
Description:
     Enable or disable EPI peripheral.

Parameters:    
    
Return Value:
    None.

Notes:
    
*****************************************************************************/
void VESPI_SetEnable(BOOL enable_or_not)
{
    if (enable_or_not)
    {
        SET_REG_BIT2(REG_SPI2_SPI2_CTRL_1,0);
    }
    else
    {
        CLR_REG_BIT2(REG_SPI2_SPI2_CTRL_1,0);
    }
}


/******************************************************************************
VSPI_ANATEST
*****************************************************************************/
void VSPI_ANATEST_Init(void)
{
#if (VSPI_ANATEST_SUPPORTED == 1)
    //VGPIO_EnableClock();
    //VGPIO_SetMuxFunction(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_SPI);    
#endif
}

VRET_VALUE VSPI_ANATEST_ReadReg(UINT32 addr, UINT32* reg_val)
{
#if (VSPI_ANATEST_SUPPORTED == 1)
        return VESPI_ReadReg(addr, reg_val);
#else
    return VRET_NO_ERROR;
#endif
}

/******************************************************************************
Description:
     Write a register to analog test sub board.

Parameters:
    
Return Value:    
    
Notes:
    addr - lower 24 bits of the register address.
    
*****************************************************************************/
VRET_VALUE VSPI_ANATEST_WriteReg(UINT32 addr, UINT32 reg_val)
{
#if (VSPI_ANATEST_SUPPORTED == 1)
    return VESPI_WriteReg (addr, reg_val );
#else
    return VRET_NO_ERROR;
#endif
}

/******************************************************************************
Description:
    Read a register from analog test sub board.

Parameters:
    addr - lower 24 bits of the register address.

Return Value:
    None.

Notes:

*****************************************************************************/
VRET_VALUE VESPI_ReadData (UINT32 addr,UINT8 *buf, UINT32 siz )
{
	VRET_VALUE err = VRET_NO_ERROR;
	UINT8 dma_sel;
	
	dma_sel = (siz > 4) ? VESPI_USE_DMA : VESPI_NO_DMA;
	VESPI_ConfigMaster(VESPI_READ_MODE, dma_sel, VESPI_SING_MODE);
	VESPI_SetClockLimit(1,1,4);
	VESPI_SetCmdAddr(ESPI_ReadData_CMD, addr);
	VESPI_SetLength(VESPI_FLASH_ADDR_LEN0, VESPI_FLASH_DUMMY_LEN1, siz);
	if(siz > 4)
		VESPI_ReadData_DMA(buf,siz);
	VESPI_StartTransfer();
	err = VESPI_WaitTransferDone(0);
	
	if((err == VRET_NO_ERROR) && (siz <= 4)){
		VESPI_ReadData_Reg (buf, siz);
	}
	VESPI_SetEnable(FALSE);
	return err;
}

/******************************************************************************
Description:
    Write a register to analog test sub board.

Parameters:

Return Value:

Notes:
    addr - lower 24 bits of the register address.

*****************************************************************************/
VRET_VALUE VESPI_WriteData (UINT32 addr,UINT8 *buf, UINT32 siz)
{
	VRET_VALUE err = VRET_NO_ERROR;
	UINT8 dma_sel;
	
	dma_sel = (siz > 4) ? VESPI_USE_DMA : VESPI_NO_DMA;
	VESPI_ConfigMaster(VESPI_WRITE_MODE, dma_sel, VESPI_SING_MODE);
	VESPI_SetClockLimit(1,1,4);
	VESPI_SetCmdAddr(ESPI_WriteData_CMD, addr);
	VESPI_SetLength(VESPI_FLASH_ADDR_LEN3, VESPI_FLASH_DUMMY_LEN0, siz);
	if(siz > 4)
		VESPI_WriteData_DMA(buf,siz);
	else
		VESPI_WriteData_Reg(buf, siz);
	
	VESPI_StartTransfer();
	err = VESPI_WaitTransferDone(0);
	VESPI_SetEnable(FALSE);
	return err;
}


