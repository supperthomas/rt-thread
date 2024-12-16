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
    VSPI

Description:
    SPI driver for VA9621 serials with a SPI peripheral such as DA and so on.

Revision History:
    Date                  Author                   Description
    2013.07.26           zuolongjun              initial version
    2022.05.30            cxt                       9650
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_dma.h"
#include "vdrv_gpio.h"
#include "vdrv_ckm.h"
#include "vdrv_spi.h"
#include "vdrv_spiflash.h"


#if (VDEV_EXT_FLASH_SUPPORTED>0)
static volatile BOOL _VSPI_ExtFlash_Init = FALSE;
#endif

static volatile BOOL _VSPI_DMA_IsTransferDone = FALSE;
static volatile VDMA_ERR _VSPI_DMA_Error = ( VDMA_ERR ) VRET_NO_ERROR;

static void VSPI_DMA_Disable ( void );

static void VSPI_DMA_IntCallBack ( VDMA_ERR ErrorCode )
{
    VSPI_DMA_Disable();
    _VSPI_DMA_IsTransferDone = TRUE;
    _VSPI_DMA_Error = ErrorCode;
}

VRET_VALUE VSPI_DMA_WaitTransferDone ( UINT32 timeout )
{
    VRET_VALUE err = VRET_NO_ERROR;
	VCPU_SR_ALLOC();
    while ( timeout ) {
        err = VRET_ERROR;
        if ( _VSPI_DMA_IsTransferDone == TRUE ) {
            err = _VSPI_DMA_Error;
			VCPU_CRITICAL_ENTER();
            _VSPI_DMA_IsTransferDone = FALSE;
            _VSPI_DMA_Error = ( VDMA_ERR ) VRET_NO_ERROR;
			VCPU_CRITICAL_EXIT();
			break;
		}
		VCPU_CRITICAL_ENTER();
		if(READ_REG(REG_DMA_INTR_FNSH_STAT)&0xA000)
		{
			VDMA_ClearInt(0xA000);
			VSPI_DMA_Disable();
			VCPU_CRITICAL_EXIT();
			err = VRET_NO_ERROR;
			break;
		}
		VCPU_CRITICAL_EXIT();
        timeout --;
    }
    // If DMA interrupt is masked, we use the SPI transfer done flag.
    return err;//VRET_DRV_SPI_ERR_WAIT_DMA_DONE_TO;
}

static void VSPI_DMA_StartRead ( UINT32 Addr, UINT32 Len )
{
    // 单次DMA读长度不能超过256个字节
    VSPI_DMA_Disable(); // Disable both read and write channels before a new start.
    if ( 1) {
        VDMA_UnmaskInt ( VDMA_INT_MC_SPI_SRAM_DONE_MSK );
        VDMA_SPI_ReadDst ( 0 );
    } else {
        VDMA_UnmaskInt ( VDMA_INT_MC_SPI_DSPPM_DONE_MSK );
        VDMA_SPI_ReadDst ( 1 );
    }
    VDMA_SPI_ReadEnable ( TRUE );
    VDMA_SPI_ConfigReadPath ( Addr, Len );
    VDMA_SPI_ReadAction ( VDMA_ACTION_START );
}
static void VSPI_DMA_StartWrite ( UINT32 Addr, UINT32 Len )
{
    VSPI_DMA_Disable(); // Disable both read and write channels before a new start.
    VDMA_UnmaskInt ( VDMA_INT_MC_SRAM_SPI_DONE_MSK );
    VDMA_SPI_WriteEnable ( TRUE );
    VDMA_SPI_ConfigWritePath ( Addr, Len );
    VDMA_SPI_WriteAction ( VDMA_ACTION_START );
}

static void VSPI_DMA_Disable ( void )
{
    VDMA_MaskInt ( VDMA_INT_MC_SPI_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_SPI_DONE_MSK | VDMA_INT_MC_SPI_DSPPM_DONE_MSK );
    VDMA_SPI_WriteAction ( VDMA_ACTION_ABORT );
    VDMA_SPI_WriteEnable ( FALSE );
    VDMA_SPI_ReadAction ( VDMA_ACTION_ABORT );
    VDMA_SPI_ReadEnable ( FALSE );
}

/******************************************************************************
Description:
    Enable or disable SPI peripheral.

Parameters:

Return Value:
    None.

Notes:

*****************************************************************************/
void VSPI_SetEnable ( BOOL enable_or_not )
{
	CLR_REG_BIT2(VSPI_PAD_CTRL, VSPI_PADCTRL_E2I_SEL_SHFT);
	
	if(enable_or_not)
		SET_REG_BIT2(VSPI_CTRLR0, VSPI_CTRLR0_EN_SHFT);
	else
		CLR_REG_BIT2(VSPI_CTRLR0, VSPI_CTRLR0_EN_SHFT);	
#if (VDEV_EXT_FLASH_SUPPORTED>0)
    _VSPI_ExtFlash_Init = FALSE;
#endif
}
#if (VDEV_EXT_FLASH_SUPPORTED>0)
void VSPI_SetExtFlashPin(void)
{
    VGPIO_SetMuxFunction(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_SPI);
	VGPIO_SetMux2Function(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_NONE);
	VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_SPI_L);
	VGPIO_SetLMux2Function(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_NONE);

    _VSPI_ExtFlash_Init = TRUE;
}
#endif
/******************************************************************************
Description:
    Assert or disassert flash's chip select pin.

Parameters:

Return Value:
    None.

Notes:
    The SPI peripheral will decide.

*****************************************************************************/
void VSPI_ChipSelect ( UINT8 flash_sel )
{
#if (VDEV_EXT_FLASH_SUPPORTED>0)
    if ((flash_sel == VSPI_FLASH_EXTERNAL) && (!_VSPI_ExtFlash_Init))
    {
        VSPI_SetExtFlashPin();
    }
#endif
	if (flash_sel == VSPI_FLASH_EXTERNAL)
		SET_REG_BIT2(VSPI_CTRLR0, VSPI_CTRLR0_CS_SHFT);
	else
		CLR_REG_BIT2(VSPI_CTRLR0, VSPI_CTRLR0_CS_SHFT);
}
/******************************************************************************
Description:
    select spi_clk devide from fast_clock

Parameters:
    spi_clk_div: spi clk 分频系数
    1  -  clk_fast
    2  -  clk_fast/2
    4  -  clk_fast/4
    8  -  clk_fast/8

Return Value:
    None.

Notes:

*****************************************************************************/
void VSPI_ClkSelect(UINT8 spi_clk_div)
{
    UINT32 reg_val;
    UINT32 mask;
    UINT8 clk_sel;

    if ( spi_clk_div == 1 ) {
        clk_sel = 0;
    } else if ( spi_clk_div == 2 ) {
        clk_sel = 1;
    } else if ( spi_clk_div == 4 ) {
        clk_sel = 2;
    } else if ( spi_clk_div == 8 ) {
        clk_sel = 3;
    } else {
        clk_sel = 1;
    }
	
    mask = ( VSPI_CTRLR0_CLK_MASK << VSPI_CTRLR0_CLK_SHFT );
	reg_val = (READ_REG(VSPI_CTRLR0) & (~mask)) | (clk_sel <<VSPI_CTRLR0_CLK_SHFT);
    WRITE_REG ( VSPI_CTRLR0, reg_val );
}

/******************************************************************************
Description:
    Enable and config the SPI peripheral. CTRL0 and CTRL1.

Parameters:
    is_write - Is it a write (program, erase, etc.) command.
    use_dma - Whether use DMA for transfering data.
    dual_mode - Both SI and SO are used to transfer data, depend on the Flash command.

Return Value:
    None.

Notes:

*****************************************************************************/
void VSPI_ConfigMaster ( BOOL is_write, BOOL use_dma, UINT8 dual_mode, UINT8 spi_mode )
{
    UINT32 reg_val;
    UINT32 mask;

    if ( use_dma ) {
        VDMA_RegisterISRCallback ( VDMA_DEV_SPI2SRAM, TRUE, VSPI_DMA_IntCallBack );
        VDMA_RegisterISRCallback ( VDMA_DEV_SRAM2SPI, TRUE, VSPI_DMA_IntCallBack );
    }

	if(is_write == VSPI_FLASH_WRITE_MODE) 
	{
    SET_REG_BIT2 ( VSPI_CTRLR1, VSPI_CTRLR1_SPI_INIT_SHFT );
	}
	else 
	{
    CLR_REG_BIT2 ( VSPI_CTRLR1, VSPI_CTRLR1_SPI_INIT_SHFT );
    }

    mask = ( VSPI_CTRLR0_DMAM_MASK << VSPI_CTRLR0_DMAM_SHFT ) | \
           ( VSPI_CTRLR0_RW_SEL_MASK << VSPI_CTRLR0_RW_SEL_SHFT );
    reg_val = ( READ_REG ( VSPI_CTRLR0 ) & ( ~mask ) ) | \
              ( use_dma << VSPI_CTRLR0_DMAM_SHFT ) | \
              ( is_write << VSPI_CTRLR0_RW_SEL_SHFT );
    WRITE_REG ( VSPI_CTRLR0, reg_val );

    mask = ( VSPI_CTRLR1_SPI_MODE_MASK << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
           ( VSPI_CTRLR1_DUAL_MASK << VSPI_CTRLR1_DUAL_SHFT ) | \
           ( VSPI_CTRLR1_SPI_QUAD_MASK << VSPI_CTRLR1_SPI_QUAD_SHFT );

    if ( dual_mode == VSPI_FLASH_SING_MODE )
        reg_val = ( READ_REG ( VSPI_CTRLR1 ) & ( ~mask ) ) | ( 0 << VSPI_CTRLR1_DUAL_SHFT ) | \
                  ( 0 << VSPI_CTRLR1_SPI_QUAD_SHFT ) | \
                  ( ( spi_mode & VSPI_CTRLR1_SPI_MODE_MASK ) << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
                  ( VSPI_PRE_LIMIT_DEFAULT << VSPI_CTRLR1_PRE_LIMIT_SHFT ) | ( VSPI_POST_LIMIT_DEFAULT << VSPI_CTRLR1_POST_LIMIT_SHFT );
    else if ( dual_mode == VSPI_FLASH_DUAL_MODE )
        reg_val = ( READ_REG ( VSPI_CTRLR1 ) & ( ~mask ) ) | ( VSPI_CTRLR1_DUAL_MASK << VSPI_CTRLR1_DUAL_SHFT ) | \
                  ( 0 << VSPI_CTRLR1_SPI_QUAD_SHFT ) | \
                  ( ( spi_mode & VSPI_CTRLR1_SPI_MODE_MASK ) << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
                  ( VSPI_PRE_LIMIT_DEFAULT << VSPI_CTRLR1_PRE_LIMIT_SHFT ) | ( VSPI_POST_LIMIT_DEFAULT << VSPI_CTRLR1_POST_LIMIT_SHFT );
    else if ( dual_mode == VSPI_FLASH_QUAD_MODE )
        reg_val = ( READ_REG ( VSPI_CTRLR1 ) & ( ~mask ) ) | ( 0 << VSPI_CTRLR1_DUAL_SHFT ) | \
                  ( VSPI_CTRLR1_SPI_QUAD_MASK << VSPI_CTRLR1_SPI_QUAD_SHFT ) | \
                  ( ( spi_mode & VSPI_CTRLR1_SPI_MODE_MASK ) << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
                  ( VSPI_PRE_LIMIT_DEFAULT << VSPI_CTRLR1_PRE_LIMIT_SHFT ) | ( VSPI_POST_LIMIT_DEFAULT << VSPI_CTRLR1_POST_LIMIT_SHFT );
    WRITE_REG ( VSPI_CTRLR1, reg_val );
}

__inline void VSPI_StartTransfer()
{
    WRITE_REG ( VSPI_START, 1 );

    /*
     *  参考文档<VA9638P_SPI_MAS_V1.7.docx> 3.4, 3.5, 3.6 章节
     *  6. Write ‘1’ to register SPI_TRANS_START to start current operation. 
     *  7. Delay about 1us, then polling register SPI_STATUS[8](cmd_wait) to check if this command is blocking due to cache’s access to flash. 
     *     Go to next step if register cmd_wait is 0.
     */
    do {
        vdelayus(1);
    } while ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_CMD_WAIT_SHFT ) );
}

__inline void VSPI_WriteCmdAddr ( UINT8 cmd, UINT32 addr )
{
    WRITE_REG ( VSPI_CMD_ADDR, cmd | ( addr << 8 ) );
}

void VSPI_WriteLength ( UINT32 addr_bytes, UINT32 dummy_bytes, UINT32 data_len )
{
    UINT32 reg_val;
    reg_val = ( addr_bytes << VSPI_LENR_ADDR_SHFT ) |
              ( dummy_bytes << VSPI_LENR_DUMMY_SHFT ) |
              ( data_len << VSPI_LENR_DATA_SHFT ) |
              ( VSPI_LENR_CMD_MASK << VSPI_LENR_CMD_SHFT );
    WRITE_REG ( VSPI_LENGTH, reg_val );
}

void VSPI_CrmLength ( BOOL crm )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_LENR_CRM_MASK << VSPI_LENR_CRM_SHFT );
    reg_val = ( READ_REG ( VSPI_LENGTH ) & ( ~mask ) ) | ( crm << VSPI_LENR_CRM_SHFT );
    WRITE_REG ( VSPI_LENGTH, reg_val );
}

void VSPI_CmdLength ( BOOL cmd )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_LENR_CMD_MASK << VSPI_LENR_CMD_SHFT );
    reg_val = ( READ_REG ( VSPI_LENGTH ) & ( ~mask ) ) | ( cmd << VSPI_LENR_CMD_SHFT );
    WRITE_REG ( VSPI_LENGTH, reg_val );
}

/******************************************************************************
Description:
    Write data to SPI bus.

Parameters:

Return Value:

Notes:
    Can write at most 4 bytes as the write buffer is only one register.

*****************************************************************************/
VRET_VALUE VSPI_WriteData ( const UINT8 *buf, UINT32 siz )
{
    UINT32 reg_val = 0;
    int i; 

    if ( siz > 4 ) 
	{
        return VRET_DRV_SPI_ERR_INVALID_PARAM;
    }

    for (i=0; i<siz; i++)
    {
        reg_val |= buf[i] << (i<<3);
    }

    WRITE_REG ( VSPI_WR_BUF, reg_val );
   
    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Read data from SPI bus.

Parameters:

Return Value:

Notes:
    Can read at most 4 bytes as the read buffer is only one register.

*****************************************************************************/
VRET_VALUE VSPI_ReadData ( UINT8 *buf, UINT32 siz )
{
    UINT32 reg_val;
    int i;
	
    if ( siz > 4 ) 
	{
        return VRET_DRV_SPI_ERR_INVALID_PARAM;
    }
    reg_val = READ_REG ( VSPI_RD_BUF );

    for (i=0; i<siz; i++)
    {
        buf[i] = (UINT8)(reg_val >> (i<<3));
    }

    return VRET_NO_ERROR;
}

void VSPI_CmdCrmb ( UINT8 cmd )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_CMDS_CRMB_MASK << VSPI_CMDS_CRMB_SHFT );
    reg_val = ( READ_REG ( VSPI_CMDS ) & ( ~mask ) ) | ( cmd << VSPI_CMDS_CRMB_SHFT );
    WRITE_REG ( VSPI_CMDS, reg_val );
}

void VSPI_CmdCrmr ( UINT8 cmd )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_CMDS_CRMR_MASK << VSPI_CMDS_CRMR_SHFT );
    reg_val = ( READ_REG ( VSPI_CMDS ) & ( ~mask ) ) | ( cmd << VSPI_CMDS_CRMR_SHFT );
    WRITE_REG ( VSPI_CMDS, reg_val );
}

void VSPI_CmdPes ( UINT8 cmd )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_CMDS_PES_MASK << VSPI_CMDS_PES_SHFT );
    reg_val = ( READ_REG ( VSPI_CMDS ) & ( ~mask ) ) | ( cmd << VSPI_CMDS_PES_SHFT );
    WRITE_REG ( VSPI_CMDS, reg_val );
}

//cxt 2021.7.21 The polling WIP mode is used for the operation of flash, and the following three functions are invalid
void VSPI_ProgCycle_Tpers(UINT32 count)
{
#if 0
	UINT32 reg_val;
	UINT32 mask;

	mask = (VSPI_PROG_CYCLE_TPER_MASK <<VSPI_PROG_CYCLE_TPER_SHFT);
	reg_val = (READ_REG(VSPI_PROG_CYCL) & (~mask)) | (count <<VSPI_PROG_CYCLE_TPER_SHFT);
	WRITE_REG(VSPI_PROG_CYCL, reg_val);
#endif
}

void VSPI_ProgCycle_Tsus ( UINT32 count )
{
#if 0
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_PROG_CYCLE_TSUS_MASK << VSPI_PROG_CYCLE_TSUS_SHFT );
    reg_val = ( READ_REG ( VSPI_PROG_CYCL ) & ( ~mask ) ) | ( count << VSPI_PROG_CYCLE_TSUS_SHFT );
    WRITE_REG ( VSPI_PROG_CYCL, reg_val );
#endif
}

void VSPI_ProgCycle_Tprog ( UINT32 ms )
{
#if 0
    UINT32 reg_val;
    UINT32 mask;
    UINT32 count = 0;
    UINT32 spi_clk;
    

//    count = (ms*(spi_clk>>20))/96;
//    count++;

    count = ms << ((7 - ((READ_REG(VSPI_CTRLR0)>>VSPI_CTRLR0_CLK_SHFT) & VSPI_CTRLR0_CLK_MASK)*2)*2);
    
    if(count > VSPI_PROG_CYCLE_TPROG_MASK)
        count = VSPI_PROG_CYCLE_TPROG_MASK;
    mask = ( VSPI_PROG_CYCLE_TPROG_MASK << VSPI_PROG_CYCLE_TPROG_SHFT );
    reg_val = ( READ_REG ( VSPI_PROG_CYCL ) & ( ~mask ) ) | ( count << VSPI_PROG_CYCLE_TPROG_SHFT );
    WRITE_REG ( VSPI_PROG_CYCL, reg_val );
#endif
}

void VSPI_CmdPer ( UINT8 cmd )
{
    UINT32 reg_val;
    UINT32 mask;

    mask = ( VSPI_CMDS_PER_MASK << VSPI_CMDS_PER_SHFT );
    reg_val = ( READ_REG ( VSPI_CMDS ) & ( ~mask ) ) | ( cmd << VSPI_CMDS_PER_SHFT );
    WRITE_REG ( VSPI_CMDS, reg_val );
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
VRET_VALUE VSPI_WriteData_DMA ( const UINT8 *buf, UINT32 siz )
{
    VSPI_DMA_StartWrite ( ( UINT32 ) buf, siz );
    return VRET_NO_ERROR;
}

//
// read data with DMA.
// At most 4 bytes as there is only one register for read.
// To execute commands with data more than 4 bytes, use the DMA- routine.
//
VRET_VALUE VSPI_ReadData_DMA ( UINT8 *buf, UINT32 siz )
{
    VSPI_DMA_StartRead ( ( UINT32 ) buf, siz );
    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Wait until transfer done.

Parameters:
    use_dma - Whether use DMA for transfering data.
    timeout - Depend on the number of bytes to tranfer and the SPI clock.

Return Value:
    When DMA is used, and SPI in
    1) Read mode: DMA transfer done interrupt comes later than SPI transfer done flag is set.
    2) Write mode: DMA transfer done interrupt comes earlier than SPI transfer done flag is set.
Notes:

*****************************************************************************/
VRET_VALUE VSPI_WaitTransferDone ( UINT32 count )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 reg_val;
    UINT32 len;
    UINT32 timeout;
    BOOL use_dma;

    reg_val = READ_REG ( VSPI_LENGTH );
    len = ( ( reg_val >> VSPI_LENR_DATA_SHFT ) & VSPI_LENR_DATA_MASK ) + \
          ( ( reg_val >> VSPI_LENR_DUMMY_SHFT ) & VSPI_LENR_DUMMY_MASK ) + \
          ( ( reg_val >> VSPI_LENR_ADDR_SHFT ) & VSPI_LENR_ADDR_MASK ) + 1;

	if(count == 0) 
		timeout = len*16*96;// Considering the worst case: highest system frequency 96MHz, SPI clock = 1MHz.
	else 
		if(GET_REG_BIT2(VSPI_CTRLR0, VSPI_CTRLR0_CLK_SHFT))
			timeout = count * 8000 * 96;
		else
			timeout = count * 8000 * 48;

    use_dma = GET_REG_BIT2 ( VSPI_CTRLR0, VSPI_CTRLR0_DMAM_SHFT );
	
    // Wait until transactions on SPI bus have completed.
    while ( timeout ) {
            //When this bit is '1', it indicates the timer to waiting for current program/erase operation finished is up.
		if(GET_REG_BIT2(VSPI_STATUS, VSPI_SR_TRANSFER_DONE_SHFT) && (GET_REG_BIT2(VSPI_STATUS, VSPI_SR_CMD_WAIT_SHFT) == 0)) // After a new start, this bit will automatically cleared.
		{
			/* program or erase */
			if(GET_REG_BIT2(VSPI_CTRLR0, 1))
			{
			  if(GET_REG_BIT2(VSPI_STATUS,VSPI_SR_PROG_TIMEUP_SHFT))
                break;
            }
			else
			{
                break;
            }
        }
        timeout --;
    }
	if(timeout == 0) err = VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;
	//if(err != VRET_NO_ERROR) return err;
    // Wait DMA trasfer done. DMA finish later when reading.
    // The DMA will finish soon after all the data have been read from SPI bus,
    // with a latency dependent on the SPI's buffer depth (1 byte).
    if ( use_dma ) {
        //err = VSPI_DMA_WaitTransferDone(timeout);
        err = VSPI_DMA_WaitTransferDone ( 100000 );
    }
    return err;
}

/******************************************************************************
Description:
    Connect GPIOL0-5 to in-package flash so that it may be accessed by external equipments.

Parameters:

Return Value:

Notes:

*****************************************************************************/
#if (VDEV_EXT_FLASH_SUPPORTED>0)
void VSPI_BypassGPIO2Inpackage ( void )
{
	VGPIO_SetMuxFunction(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetMux2Function(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_SPI);
	VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetLMux2Function(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_SPI_L);
	SET_REG_BIT2(VSPI_PAD_CTRL, VSPI_PADCTRL_E2I_SEL_SHFT);
	SET_REG_BIT2(VSPI_PAD_CTRL, VSPI_PADCTRL_E2I_SO_SHFT);
}

/******************************************************************************
Description:
    GPIOs are not mapped for in-package flash so that it may be accessed by VA9625.

Parameters:

Return Value:

Notes:

*****************************************************************************/
void VSPI_RetrieveGPIO4Inpackage ( void )
{
	VGPIO_SetMuxFunction(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_SPI);	
	VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_SPI_L);	
	CLR_REG_BIT2(VSPI_PAD_CTRL, VSPI_PADCTRL_E2I_SEL_SHFT);	
	CLR_REG_BIT2(VSPI_PAD_CTRL, VSPI_PADCTRL_E2I_SO_SHFT);
}

/******************************************************************************
Description:
    Set the SPI-Flash GPIOs as high-z so that extenal equipments can access the on-board flash.

Parameters:

Return Value:

Notes:

*****************************************************************************/
void VSPI_GiveupBus ( void )
{
	VGPIO_SetMuxFunction(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetMux2Function(GPIO_MUX_FUNC_SPI, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetLMuxFunction(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetLMux2Function(GPIO_MUX_FUNC_SPI_L, GPIO_MUX_FUNC_NONE);	
	VGPIO_SetMode(GPIO_MUX_FUNC_SPI, GPIO_MODE_Z, 0);	
	VGPIO_SetLMode(GPIO_MUX_FUNC_SPI_L, GPIO_MODE_Z, 0);	
}
#endif
/******************************************************************************
Description:
 	Enable Firmware Protection function to encrypt/decrypt data/code in flash

Parameters:
	
Return Value:
	
Notes:
	
*****************************************************************************/
void VSPI_EncryptEn(void)
{
	SET_REG_BIT2(VSPI_ENCRYPT_EN, VSPI_ENCRYPT_KEY_SHFT);
	//SET_REG_BIT2(REG_CACHE_SPI_ENC_EN, VSPI_ENCRYPT_KEY_SHFT);
}

/******************************************************************************
Description:
 	Disable Firmware Protection function to encrypt/decrypt data/code in flash

Parameters:
	
Return Value:
	
Notes:
	
*****************************************************************************/
void VSPI_EncryptDis(void)
{
	CLR_REG_BIT2(VSPI_ENCRYPT_EN, VSPI_ENCRYPT_KEY_SHFT);
	//CLR_REG_BIT2(REG_CACHE_SPI_ENC_EN, VSPI_ENCRYPT_KEY_SHFT);
}

/******************************************************************************
Description:
 	Configure 16bit Key for Encrypt/Decrypt function.

Parameters:
	
Return Value:
	
Notes:
	
*****************************************************************************/
void VSPI_EncryptKey(UINT16 key)
{
    WRITE_REG(VSPI_ENCRYPT_KEY, (UINT32)key);
    //WRITE_REG(REG_CACHE_SPI_ENC_KEY, (UINT32)key);	
}

/******************************************************************************
Description:
 	Configure 16bit Key for MCU cache Encrypt/Decrypt function.

Parameters:
	
Return Value:
	
Notes:
	
*****************************************************************************/
void VSPI_Set_EncryptKey(UINT16 key_top, UINT16 key_bt)
{
	UINT32 reg;
	
	reg = key_bt << 16 | key_top;
    WRITE_REG(VSPI_ENCRYPT_KEY, reg);
}

/******************************************************************************
Description:
 	Configure 16bit Key for DSP cache Encrypt/Decrypt function.

Parameters:
	
Return Value:
	
Notes:
	
*****************************************************************************/
void VSPI_Set_EncryptKey2(UINT16 key_dsp, UINT16 key_data)
{
	UINT32 reg;
	
	reg = key_data << 16 | key_dsp;
    WRITE_REG(SPI_ENCRYPT_KEY2, reg);
}

/******************************************************************************
Description:
 	Configure Base address of MCUTOP code space in flash function.

Parameters:
    addr:  Base address of MCUTOP code space in flash
Return Value:
	
Notes:
	
*****************************************************************************/
VRET_VALUE VSPI_Set_TOPBase(UINT32 addr)
{
	WRITE_REG(VSPI_TPBS_LOCK,0);
	WRITE_REG(VSPI_TP_BASE,addr);

	WRITE_REG(VSPI_TPBS_LOCK,1);
	
	return VRET_NO_ERROR;
}


UINT32 VSPI_Get_TOPBase(void)
{
    return READ_REG( VSPI_TP_BASE );
}
	
/******************************************************************************
Description:
 	Configure Base address of MCUBT code space in flash function.

Parameters:
    addr:  Base address of MCUBT code space in flash
Return Value:

Notes:

*****************************************************************************/
VRET_VALUE VSPI_Set_BTBase ( UINT32 addr )
{
    //  if((addr & (0x10000-1)) != 0)
    //      return VRET_ERROR;

    WRITE_REG ( VSPI_BTBS_LOCK, 0 );
    WRITE_REG ( VSPI_BT_BASE, addr );

    WRITE_REG ( VSPI_BTBS_LOCK, 1 );

    return VRET_NO_ERROR;
}

UINT32 VSPI_Get_BTBase(void)
{
    return READ_REG( VSPI_BT_BASE );
}
	

/******************************************************************************
Description:
    Configure Base address of DSP code space in flash function.

Parameters:
    addr:  Base address of DSP code space in flash
Return Value:

Notes:

*****************************************************************************/
VRET_VALUE VSPI_Set_DSPBase ( UINT32 addr )
{
    //  if((addr & (0x10000-1)) != 0)
    //      return VRET_ERROR;

    WRITE_REG ( VSPI_DSPBS_LOCK, 0 );
    WRITE_REG ( VSPI_DSP_BASE, addr );

    WRITE_REG ( VSPI_DSPBS_LOCK, 1 );

    return VRET_NO_ERROR;
}

UINT32 VSPI_Get_DSPBase(void)
{
    return READ_REG( VSPI_DSP_BASE );
}






