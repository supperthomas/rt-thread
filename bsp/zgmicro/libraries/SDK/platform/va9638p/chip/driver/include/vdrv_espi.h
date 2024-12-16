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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VESPI_H_
#define _VESPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VRET_DRV_SPI_ERROR								0x450
#define VRET_DRV_SPI_ERR_INVALID_PARAM					0x451
#define VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO			0x452
#define VRET_DRV_SPI_ERR_WAIT_DMA_DONE_TO			    0x453

#define  GPIO_MUX_FUNC_ESPI_SCK         (GPIO_L_PIN_7)
#define  GPIO_MUX_FUNC_ESPI_CSN         (GPIO_L_PIN_4)
#define  GPIO_MUX_FUNC_ESPI_DI             (GPIO_L_PIN_6)
#define  GPIO_MUX_FUNC_ESPI_DO             (GPIO_L_PIN_5)
#define  GPIO_MUX_FUNC_ESPI_AUX          (GPIO_MUX_FUNC_ESPI_SCK|GPIO_MUX_FUNC_ESPI_CSN|GPIO_MUX_FUNC_ESPI_DI|GPIO_MUX_FUNC_ESPI_DO)

//SPI clock frequency selection
#define VESPI_CTRLR1_CLK_MASK                ((UINT32)0x3) 
#define VESPI_CTRLR1_CLK_SHFT                4
//select slave mode
#define VESPI_CTRLR1_CS_MASK                 ((UINT32)0x1) 
#define VESPI_CTRLR1_CS_SHFT                 3
// bit[2], dma mode, 1 - use DMA.
#define VESPI_CTRLR1_DMAM_MASK            ((UINT32)0x1) 
#define VESPI_CTRLR1_DMAM_SHFT            2
// bit[1], Read or write (program, erase), 0: read; 1: write.
#define VESPI_CTRLR1_RW_SEL_MASK        ((UINT32)0x1) 
#define VESPI_CTRLR1_RW_SEL_SHFT        1
// bit[0], SPI enable.
#define VESPI_CTRLR1_EN_MASK                 ((UINT32)0x1) 
#define VESPI_CTRLR1_EN_SHFT                 0

// bit[15:12], number of cycles of CS high, default to 4.
#define VESPI_CTRLR2_SSH_MASK                    ((UINT32)0xF)
#define VESPI_CTRLR2_SSH_SHFT                    12
// bit[11:8], sck inactive to CSN rising edge, default to 0.
#define VESPI_CTRLR2_POST_LIMIT_MASK     ((UINT32)0xF)
#define VESPI_CTRLR2_POST_LIMIT_SHFT     8
// bit[7:4], CSN falling edge to sck active, default to 0.
#define VESPI_CTRLR2_PRE_LIMIT_MASK     ((UINT32)0xF)
#define VESPI_CTRLR2_PRE_LIMIT_SHFT     4
// bit[2:1], dual mode. data bits are shifted out via both SI and SO.
#define VESPI_CTRLR2_DUAL_DATA_MASK     ((UINT32)0x1)
#define VESPI_CTRLR2_DUAL_DATA_SHFT     2
// bit[2:1], dual mode. Address bits are shifted out via both SI and SO.
#define VESPI_CTRLR2_DUAL_ADDR_MASK     ((UINT32)0x1)
#define VESPI_CTRLR2_DUAL_ADDR_SHFT     1
// bit[0], spi mode. 0 - SPI mode 0; 1 - SPI mode 3.
#define VESPI_CTRLR2_SPI_MODE_MASK         ((UINT32)0x1)
#define VESPI_CTRLR2_SPI_MODE_SHFT         0

#define VESPI_LENR_DATA_MASK                 ((UINT32)0xFFFF)
#define VESPI_LENR_DATA_SHFT                 8
#define VESPI_LENR_DUMMY_MASK                 ((UINT32)0xF)
#define VESPI_LENR_DUMMY_SHFT                 4
#define VESPI_LENR_ADDR_MASK                 ((UINT32)0x3)
#define VESPI_LENR_ADDR_SHFT                 0

// bit[4], write '1' to clear
#define VESPI_SR_TX_UNDERFLOW_MASK           ((UINT32)0x1)
#define VESPI_SR_TX_UNDERFLOW_SHFT           4
// bit[3], read only
#define VESPI_SR_TX_EMPTY_MASK               ((UINT32)0x1)
#define VESPI_SR_TX_EMPTY_SHFT               3
// bit[2], write '1' to clear
#define VESPI_SR_RX_OVERFLOW_MASK            ((UINT32)0x1)
#define VESPI_SR_RX_OVERFLOW_SHFT            2
// bit[1], read only
#define VESPI_SR_RX_FULL_MASK                ((UINT32)0x1)
#define VESPI_SR_RX_FULL_SHFT                1
// bit[0],
#define VESPI_SR_TRANSFER_DONE_MASK          ((UINT32)0x1)
#define VESPI_SR_TRANSFER_DONE_SHFT          0
 
#define VESPI_READ_MODE          0
#define VESPI_WRITE_MODE         1

#define VESPI_SING_MODE 0
#define VESPI_DUAL_MODE 1

#define VESPI_NO_DMA     0
#define VESPI_USE_DMA 1

#define VESPI_FLASH_ADDR_LEN0      0
#define VESPI_FLASH_ADDR_LEN3      3
#define VESPI_FLASH_DUMMY_LEN0     0
#define VESPI_FLASH_DUMMY_LEN1     1
#define VESPI_FLASH_DUMMY_LEN3     3

#define ESPI_WriteData_CMD     0x02
#define ESPI_ReadData_CMD     0x4B

void VESPI_Init ( void );
void VESPI_SetEnable(BOOL enable_or_not);
void VESPI_ClkSelect(UINT8 Clk_Sel);
VRET_VALUE VESPI_ReadReg ( UINT32 addr, UINT32 *reg_val );
VRET_VALUE VESPI_WriteReg ( UINT32 addr, UINT32 reg_val );

#define VSPI_ANATEST_SUPPORTED (1)
#define VSPI_ANATEST_CMD_READ_REGISTER    0x03
#define VSPI_ANATEST_CMD_WRITE_REGISTER   0x02

#if (VSPI_ANATEST_SUPPORTED == 1)
#undef ESPI_WriteData_CMD
#undef ESPI_ReadData_CMD
#define ESPI_WriteData_CMD     VSPI_ANATEST_CMD_WRITE_REGISTER
#define ESPI_ReadData_CMD     VSPI_ANATEST_CMD_READ_REGISTER
void VSPI_ANATEST_Init(void);
VRET_VALUE VSPI_ANATEST_ReadReg(UINT32 addr, UINT32* reg_val);
VRET_VALUE VSPI_ANATEST_WriteReg(UINT32 addr, UINT32 reg_val);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _VESPI_H_ */
