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
    SPI driver.

Revision History:
    Date                  Author                   Description
    2013.01.27           zuolongjun              initial version
    2013.10.25           zuolongjun              According to "VA9613_SPI_V0.2",
                                                            bypass GPIOs to in-package flash so that it may be accessed by external equipments.
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSPI_H_
#define _VSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* SPI reserved error code from 0x440 ~ 0x44F */
#define VRET_DRV_SPI_ERROR								0x450
#define VRET_DRV_SPI_ERR_INVALID_PARAM					0x451
#define VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO			0x452
#define VRET_DRV_SPI_ERR_WAIT_DMA_DONE_TO			    0x453

#define VRET_DRV_SPI_FLASH_ERROR						0x460
#define VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM            0x461
#define VRET_DRV_SPI_FLASH_ERR_TIMEOUT					0x462
#define VRET_DRV_SPI_FLASH_ERR_UNKNOWN_MID				0x463
#define VRET_DRV_SPI_FLASH_ERR_PROTECTED				0x464
#define VRET_DRV_SPI_FLASH_ERR_NOT_AVAILABLE			0x465
#define VRET_DRV_SPI_FLASH_ERR_LOCKED                   0x466
#define VRET_DRV_SPI_FLASH_ERR_ERASE_TOO_QUICK          0x467
#define VRET_DRV_SPI_FLASH_ERR_SETQE                    0x468
#define VRET_DRV_SPI_FLASH_ERR_UNKNOWN_UID				0x469


/*******
 * spi 模块是否能被 cache 打断，  0-不能被打断，cache 优先级低于 spi； 1-可以被打断，cache 优先级高于 spi
 */
#define SPI_MODLE_INTIBLE_ENABLE    (1)


//
// SPI features
//
#define VSPI_NUM_SLAVES         1
#define VSPI_MAX_NDF            (64*1024L) // maxium number of frames to be received at once.
// AC timing
#define VSPI_CLK_FREQ           (48*1000*1000)
#define VSPI_PRE_LIMIT_DEFAULT    1
#define VSPI_POST_LIMIT_DEFAULT  1

//
// Driver config
//
#define VSPI_DEFAULT_SCPOL                1 // sclk polarity, for atmel device, [POL:PH] = 2'b11.
#define VSPI_DEFAULT_SCPH                 1 // sclk phase
#define VSPI_DEFAULT_FRF                  0 // 00, 01 and 11, 00 for Motorola SPI.

//
// Bit map for register CTRLR0
//
// bit[4], clk select.
// 0 – 48M
// 1 – 96M
#define VSPI_CTRLR0_CLK_MASK                ((UINT32)0x3)
#define VSPI_CTRLR0_CLK_SHFT                4
#define VSPI_CTRLR0_CS_MASK                 ((UINT32)0x1)
#define VSPI_CTRLR0_CS_SHFT                 3
// bit[2], dma mode, 1 - use DMA.
#define VSPI_CTRLR0_DMAM_MASK               ((UINT32)0x1)
#define VSPI_CTRLR0_DMAM_SHFT               2
// bit[1], Read or write (program, erase), 0: read; 1: write.
#define VSPI_CTRLR0_RW_SEL_MASK                 ((UINT32)0x1)
#define VSPI_CTRLR0_RW_SEL_SHFT                 1
// bit[0], SPI enable.
#define VSPI_CTRLR0_EN_MASK                 ((UINT32)0x1)
#define VSPI_CTRLR0_EN_SHFT                 0

//
// Bit map for register CTRLR1
//
// bit[18], spi init enable.
#define VSPI_CTRLR1_SPI_INIT_MASK               ((UINT32)0x1)
#define VSPI_CTRLR1_SPI_INIT_SHFT               18
// bit[17:16], spi quad addr mode.
#define VSPI_CTRLR1_SPI_QUAD_MASK               ((UINT32)0x3)
#define VSPI_CTRLR1_SPI_QUAD_SHFT               16
// bit[15:12], number of cycles of CS high, default to 4.
#define VSPI_CTRLR1_SSH_MASK                ((UINT32)0xF)
#define VSPI_CTRLR1_SSH_SHFT                12
// bit[11:8], sck inactive to CSN rising edge, default to 0.
#define VSPI_CTRLR1_POST_LIMIT_MASK                 ((UINT32)0xF)
#define VSPI_CTRLR1_POST_LIMIT_SHFT                 8
// bit[7:4], CSN falling edge to sck active, default to 0.
#define VSPI_CTRLR1_PRE_LIMIT_MASK              ((UINT32)0xF)
#define VSPI_CTRLR1_PRE_LIMIT_SHFT              4
// bit[2:1], dual mode. Address and data bits are shifted out via both SI and SO.
#define VSPI_CTRLR1_DUAL_MASK               ((UINT32)0x3)
#define VSPI_CTRLR1_DUAL_SHFT               1
// bit[0], spi mode. 0 - SPI mode 0; 1 - SPI mode 3.
#define VSPI_CTRLR1_SPI_MODE_MASK               ((UINT32)0x1)
#define VSPI_CTRLR1_SPI_MODE_SHFT               0

//
// Bit map for register START
//
#define VSPI_START_EN_MASK              ((UINT32)0x1)
#define VSPI_START_EN_SHFT              0

//
// Bit map for register CMD_ADDR
//
#define VSPI_CADDR_ADDR_MASK                ((UINT32)0xFFFFFF)
#define VSPI_CADDR_ADDR_SHFT                8
#define VSPI_CADDR_CMD_MASK                 ((UINT32)0xFF)
#define VSPI_CADDR_CMD_SHFT                 0


//
// Bit map for register LENGTH
//

#define VSPI_LENR_CRM_MASK              ((UINT32)0x1)
#define VSPI_LENR_CRM_SHFT               25
#define VSPI_LENR_CMD_MASK              ((UINT32)0x1)
#define VSPI_LENR_CMD_SHFT               24
#define VSPI_LENR_DATA_MASK                 ((UINT32)0xFFFF)
#define VSPI_LENR_DATA_SHFT                 8
#define VSPI_LENR_DUMMY_MASK                ((UINT32)0xF)
#define VSPI_LENR_DUMMY_SHFT                4
#define VSPI_LENR_ADDR_MASK                 ((UINT32)0x3)
#define VSPI_LENR_ADDR_SHFT                 0

//
// Bit map for register PROG CYCLE
//
#define VSPI_PROG_CYCLE_TPER_MASK           ((UINT32)0x3F)
#define VSPI_PROG_CYCLE_TPER_SHFT           26
#define VSPI_PROG_CYCLE_TSUS_MASK               ((UINT32)0x3FF)
#define VSPI_PROG_CYCLE_TSUS_SHFT               16
#define VSPI_PROG_CYCLE_TPROG_MASK              ((UINT32)0xFFFF)
#define VSPI_PROG_CYCLE_TPROG_SHFT              0

//
// Bit map for register CMDS
//
#define VSPI_CMDS_PER_MASK              ((UINT32)0xFF)
#define VSPI_CMDS_PER_SHFT               24
#define VSPI_CMDS_PES_MASK              ((UINT32)0xFF)
#define VSPI_CMDS_PES_SHFT              16
#define VSPI_CMDS_CRMR_MASK                 ((UINT32)0xFF)
#define VSPI_CMDS_CRMR_SHFT                 8
#define VSPI_CMDS_CRMB_MASK                 ((UINT32)0xFF)
#define VSPI_CMDS_CRMB_SHFT                 0

//
// Bit map for register SR, status
//
// bit[9], read only
#define VSPI_SR_PROG_TIMEUP_MASK            ((UINT32)0x1)
#define VSPI_SR_PROG_TIMEUP_SHFT            9

// bit[8], read only
#define VSPI_SR_CMD_WAIT_MASK               ((UINT32)0x1)
#define VSPI_SR_CMD_WAIT_SHFT               8

// bit[4], write '1' to clear
#define VSPI_SR_TX_UNDERFLOW_MASK               ((UINT32)0x1)
#define VSPI_SR_TX_UNDERFLOW_SHFT               4
// bit[3], read only
#define VSPI_SR_TX_EMPTY_MASK               ((UINT32)0x1)
#define VSPI_SR_TX_EMPTY_SHFT               3
// bit[2], write '1' to clear
#define VSPI_SR_RX_OVERFLOW_MASK                ((UINT32)0x1)
#define VSPI_SR_RX_OVERFLOW_SHFT                2
// bit[1], read only
#define VSPI_SR_RX_FULL_MASK                ((UINT32)0x1)
#define VSPI_SR_RX_FULL_SHFT                1
// bit[0],
#define VSPI_SR_TRANSFER_DONE_MASK              ((UINT32)0x1)
#define VSPI_SR_TRANSFER_DONE_SHFT              0

//
// Bit map for register PAD_CTRL, status
//
// bit[9], 1 - map the in-package to GPIO
#define VSPI_PADCTRL_LVHOLD_MASK                ((UINT32)0x1)
#define VSPI_PADCTRL_LVHOLD_SHFT                9
// bit[8], 1 - map the in-package to GPIO
#define VSPI_PADCTRL_LVWP_MASK              ((UINT32)0x1)
#define VSPI_PADCTRL_LVWP_SHFT      8

#define VSPI_PADCTRL_E2I_SO_MASK                ((UINT32)0x1)
#define VSPI_PADCTRL_E2I_SO_SHFT                3

// bit[2], 1 - map the in-package to GPIO
#define VSPI_PADCTRL_E2I_SEL_MASK               ((UINT32)0x1)
#define VSPI_PADCTRL_E2I_SEL_SHFT               2
// bit[1],
#define VSPI_PADCTRL_SO_DRIVE_MASK              ((UINT32)0x1)
#define VSPI_PADCTRL_SO_DRIVE_SHFT              1

// bit[0],
#define VSPI_PADCTRL_SCK_DRIVE_MASK                 ((UINT32)0x1)
#define VSPI_PADCTRL_SCK_DRIVE_SHFT                 0

#define VSPI_PADCTRL_CACHE_QUAD_DUMMY_MASK              ((UINT32)0x1)
#define VSPI_PADCTRL_CACHE_QUAD_DUMMY_SHFT              1
// bit[0],
#define VSPI_CACHECTRL_QUAD_ENABLE_MASK                 ((UINT32)0x1)
#define VSPI_CACHECTRL_QUAD_ENABLE_SHFT                 0
//
// Bit map for register ENCRYPT_EN,
//
#define VSPI_ENCRYPT_EN_MASK                        ((UINT32)0x1)
#define VSPI_ENCRYPT_EN_SHFT                        0

//
// Bit map for register ENCRYPT_KEY,
//
#define VSPI_ENCRYPT_KEY_MASK                        ((UINT32)0xFFFF)
#define VSPI_ENCRYPT_KEY_SHFT                        0

//
// Exported routines
//
void VSPI_SetEnable ( BOOL enable_or_not );
void VSPI_ChipSelect ( UINT8 flash_sel );
void VSPI_ClkSelect ( UINT8 spi_clk_div );
void VSPI_ConfigMaster ( BOOL is_write, BOOL use_dma, UINT8 dual_mode, UINT8 spi_mode );
void VSPI_WriteCmdAddr ( UINT8 cmd, UINT32 addr );
void VSPI_WriteLength ( UINT32 addr_bytes, UINT32 dummy_bytes, UINT32 data_len );
void VSPI_CrmLength ( BOOL crm );
void VSPI_CmdLength ( BOOL cmd );
void VSPI_StartTransfer ( void );
VRET_VALUE VSPI_WriteData ( const UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_ReadData ( UINT8 *buf, UINT32 siz );
void VSPI_CmdCrmb ( UINT8 cmd );
void VSPI_CmdCrmr ( UINT8 cmd );
void VSPI_CmdPes ( UINT8 cmd );
void VSPI_CmdPer ( UINT8 cmd );
VRET_VALUE VSPI_WriteData_DMA ( const UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_ReadData_DMA ( UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_WaitTransferDone ( UINT32 count );
VRET_VALUE VSPI_DMA_WaitTransferDone ( UINT32 timeout );

void VSPI_ProgCycle_Tprog ( UINT32 ms );
void VSPI_ProgCycle_Tsus ( UINT32 count );

void VSPI_BypassGPIO2Inpackage ( void );
void VSPI_RetrieveGPIO4Inpackage ( void );
void VSPI_GiveupBus ( void );


#ifdef __cplusplus
}
#endif

#endif  /* _VSPI_H_ */

