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
    VSPI_FLASH

Description:
    SPI flash driver.
    Designed for Giga, MX, KH, take atmel AT25DF serials into consideration as well.

Revision History:
    Date                  Author                   Description
    2013.01.27           zuolongjun              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VSPI_FLASH_H_
#define _VSPI_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vstdlib.h"
//
// Driver options
//
#define VSPI_FLASH_MAX_NUM         2

#define VSPI_FLASH_UNIQUEID_LEN   16    //12

/// lock spi flash over hold chip, 0 for success
/// origin is ipc_res_lock(VIPC_LOCK_FLASH)
#define VSPI_FLASH_GLOBAL_LOCK()   (0)

/// lock spi flash over hold chip, 0 for success
/// origin is ipc_res_lock(VIPC_LOCK_FLASH)
#define VSPI_FLASH_GLOBAL_UNLOCK()   (0)

/*
  * Determined by higher layers.
  * bit[7:4] for setting spi single/dual/quad mode; bit[3:0] for setting spi mode.
*/
typedef struct VSPI_PER_FLASH_CONFIG_TAG {
    UINT8       spi_mode: 4; // 0 or 3.
    UINT8       dual_mode: 4; // 0: spi single mode; 1: dual mode; 2 quad mode.
} VSPI_PER_FLASH_CONFIG;

typedef struct VSPI_FLASH_CONFIG_TAG {
    VSPI_PER_FLASH_CONFIG flash[VSPI_FLASH_MAX_NUM];
} VSPI_FLASH_CONFIG;

enum  {
    VSPI_FLASH_NONE = 0,
    VSPI_FLASH_EXTERNAL = 1,
    VSPI_FLASH_INPACKAGE = 2,
    VSPI_FLASH_BOTH
};

typedef enum{
         FLASH_NONE_VOL = 0,
         FLASH_1_8v_VOL = 1,
         FLASH_3_3v_VOL = 2,
}FLASH_VOL;


/*
  * Read only by higher layers.
*/
typedef struct VSPI_PER_FLASH_INFO_TAG {
    UINT8      mid[4]; //
    UINT8      Uid[VSPI_FLASH_UNIQUEID_LEN];
    UINT32     size; // .byte.
    UINT32     sector_size;
    UINT32     page_size;
    FLASH_VOL   device_voltage; //cxt 2020.12.11    [1]:1.8v     [2 or 0]: 3.3v
} VSPI_PER_FLASH_INFO;

typedef struct VSPI_FLASH_INFO_TAG {
    UINT8       ready, init;                              // 0-3: none / in-package flash / external flash /both flashes is ready.
    VSPI_PER_FLASH_INFO flash[VSPI_FLASH_MAX_NUM];  // Per-flash info. [1] for in-package, [0] for external flash.
} VSPI_FLASH_INFO;

/*
  * Raw flash info which resides in the 1st page of the flash.
  * the host transfers flash parameters in this format.
*/
typedef struct VSPI_FLASH_RAW_PARAM_TAG {
    UINT32 magic_num;
    UINT16 total_size;
    UINT8  sector_size;
    UINT8  page_size;
    UINT8  spi_mode;
    UINT8  dual_mode;
    UINT16 crc;
} VSPI_FLASH_RAW_PARAM;

//
// Constants
//
#define VSPI_FLASH_MID_ATMEL                    0x1F
#define VSPI_FLASH_MID_MX_KH                    0xC2
#define VSPI_FLASH_MID_GIGA_Q                   0xC8
#define VSPI_FLASH_MID_GIGA_D                   0x51
#define VSPI_FLASH_MID_EON                      0x1C
#define VSPI_FLASH_MID_WB                       0xEF
#define VSPI_FLASH_MID_PUYA                     0x85

#define VSPI_FLASH_CAP_4_BIT                   	0x13
#define VSPI_FLASH_CAP_16_BIT                   0x15
#define VSPI_FLASH_CAP_32_BIT                   0x16
#define VSPI_FLASH_CAP_64_BIT                   0x18
#define VSPI_FLASH_READ_MODE                    0
#define VSPI_FLASH_WRITE_MODE                   1

#define VSPI_FLASH_NO_DMA                       0
#define VSPI_FLASH_USE_DMA                      1

#define VSPI_FLASH_SING_MODE                    0
#define VSPI_FLASH_DUAL_MODE                    1
#define VSPI_FLASH_QUAD_MODE                    2

#define VSPI_FLASH_ADDR_LEN0                    0
#define VSPI_FLASH_ADDR_LEN3                    3
#define VSPI_FLASH_DUMMY_LEN0                   0
#define VSPI_FLASH_DUMMY_LEN1                   1
#define VSPI_FLASH_DUMMY_LEN3                   3
#define VSPI_FLASH_DUMMY_LEN4                   4
#define VSPI_FLASH_SFDP_LEN                     0x6c

#define VSPI_FLASH_RD_DUAL_MASK                 1
#define VSPI_FLASH_RD_QUAD_MASK                 2

#define VSPI_FLASH_RAW_INFO_MAGIC_NUM           (0xA55A5AA5)
//
// Chip features
//
// Capacity features
#define VSPI_FLASH_OTP_SECTOR_SIZE              (1*1024)
#define VSPI_FLASH_SECTOR_SIZE                  (4*1024)
#define VSPI_FLASH_PAGE_SIZE                    (256) // Each page contains 256 bytes
// AC timing parameters
#define VSPI_FLASH_ERASE_TIM_BLOCK4K            (200*1000L) // .us
#define VSPI_FLASH_ERASE_TIM_BLOCK32K           (600*1000L)
#define VSPI_FLASH_ERASE_TIM_BLOCK64K           (950*1000L)
#define VSPI_FLASH_ERASE_TIM_CHIP               (28*1000*1000L) // 28s, 8M
#define VSPI_FLASH_PROGRAM_TIM_PAGE             (3*1000L) // maxium .us
#define VSPI_FLASH_PROGRAM_TIM_BYTE             7 // maxium .us
#define VSPI_FLASH_PROGRAM_TIM_SR               40*1000 // write status register time, us.
// others
#define VSPI_FLASH_DUMMY_BYTE                    0xFF // dummy bytes after command.

//
// Command listing
//
// Read commands
#define VSPI_FLASH_CMD_READ_50M_0DUMMY           0x03
#define VSPI_FLASH_CMD_READ_85M_1DUMMY           0x0B
#define VSPI_FLASH_CMD_READ_DUAL_1DUMMY          0x3B
#define VSPI_FLASH_CMD_READ_QUAD_1DUMMY          0x6B
#define VSPI_FLASH_CMD_IO_DUAL_1DUMMY            0xBB
#define VSPI_FLASH_CMD_IO_QUAD_3DUMMY            0xEB

// Program and erase commands
#define VSPI_FLASH_CMD_ERASE_BLOCK_4K            0x20
#define VSPI_FLASH_CMD_ERASE_BLOCK_32K           0x52
#define VSPI_FLASH_CMD_ERASE_BLOCK_64K           0xD8
#define VSPI_FLASH_CMD_ERASE_CHIP1               0x60 // Two commands for chip erase.
#define VSPI_FLASH_CMD_ERASE_CHIP2               0xC7
#define VSPI_FLASH_CMD_PROGRAM_PAGE              0x02
//#define VSPI_FLASH_CMD_PROGRAM_PAGE_DUAL       0xA2
#define VSPI_FLASH_CMD_PROGRAM_PAGE_QUAD         0x32
#define VSPI_FLASH_MX_CMD_PROGRAM_PAGE_QUAD             0x38

// Protection commands
#define VSPI_FLASH_CMD_WRITE_ENABLE                0x06
#define VSPI_FLASH_CMD_WRITE_DISABLE               0x04
#define VSPI_FLASH_CMD_SECTOR_PROTECT              0x36
#define VSPI_FLASH_CMD_SECTOR_UNPROTECT            0x39
//#define VSPI_FLASH_CMD_GLOBAL_PROTECT            Use Write Status Register Byte 1 Command
//#define VSPI_FLASH_CMD_GLOBAL_UNPROTECT          Use Write Status Register Byte 1 Command
#define VSPI_FLASH_CMD_READ_SECTOR_PROTECTION      0x3C

// Security commands
#define VSPI_FLASH_CMD_ERASE_SECURITY_REGISTER     0x44
#define VSPI_FLASH_CMD_PROGRAM_SECURITY_REGISTER   0x42
#define VSPI_FLASH_CMD_READ_SECURITY_REGISTER      0x48

// OTP mode for ESMT/EON Flash
#define VSPI_FLASH_EON_CMD_OTP_ENTRY                       0x3A

// Status register commands
#define VSPI_FLASH_CMD_READ_STATUS_REG              0x05
#define VSPI_FLASH_CMD_READ_STATUS_REG1             0x35
#define VSPI_FLASH_CMD_READ_STATUS_REG2             0x15
#define VSPI_FLASH_CMD_WRITE_STATUS_BYTE            0x01
#define VSPI_FLASH_CMD_WRITE_STATUS_BYTE1           0x31
#define VSPI_FLASH_CMD_WRITE_STATUS_BYTE2           0x11

// Miscellaneous commands
#define VSPI_FLASH_CMD_RSTEN                       0x66
#define VSPI_FLASH_CMD_RST                         0x99
#define VSPI_FLASH_CMD_CRMR                        0xFF
#define VSPI_FLASH_CMD_HPM                         0xA3
#define VSPI_FLASH_CMD_READ_MANUFACTURE_ID         0x9F
#define VSPI_FLASH_CMD_DEEP_PWR_DOWN               0xB9
#define VSPI_FLASH_CMD_RESUME_FROM_DEEP_PWR_DOWN   0xAB
#define VSPI_FLASH_CMD_READ_UNIQUE_ID              0x4B
#define VSPI_FLASH_CMD_READ_SFDP                   0x5A

/************ OTP ***************/
#define VSPI_OTP_CMD_READ_85M_1DUMMY             0x48
#define VSPI_OTP_CMD_PROGRAM_PAGE                0x42
#define VSPI_OTP_CMD_ERASE_SECTOR_1K             0x44

//
// Bit map for registers

/* otp region lock bit,LB3~1,for chip 25Q32C */
#define VSPI_FLASH_OTP1_ADDR_BASE                  0x01000
#define VSPI_FLASH_OTP2_ADDR_BASE                  0x02000
#define VSPI_FLASH_OTP3_ADDR_BASE                  0x03000
#define VSPI_FLASH_OTP_LEN                         1024


// status[1]
// S15  S14 S13 S12 S11 S10  S9 S8
// SUS1 CMP LB3 LB2 LB1 SUS2 QE SRP1

#define VSPI_FLASH_OPT_UNLOCK                     0
#define VSPI_FLASH_OTP_LOCK                       1

// bit[5], LB3
#define VSPI_FLASHGD_SR_LB3_MASK                   ((UINT8)0x1)
#define VSPI_FLASHGD_SR_LB3_SHFT                   5

// bit[4], LB2
#define VSPI_FLASHGD_SR_LB2_MASK                   ((UINT8)0x1)
#define VSPI_FLASHGD_SR_LB2_SHFT                   4

// bit[3], LB1
#define VSPI_FLASHGD_SR_LB1_MASK                   ((UINT8)0x1)
#define VSPI_FLASHGD_SR_LB1_SHFT                   3

#define VSPI_FLASHGD_SR_REGION_LB1                 0x01
#define VSPI_FLASHGD_SR_REGION_LB2                 0x02
#define VSPI_FLASHGD_SR_REGION_LB3                 0x03

// status register byte1,2

//byte1

// bit[7], SUS
#define VSPI_FLASH_SR_SUS_MASK                  ((UINT8)0x1)
#define VSPI_FLASH_SR_SUS_SHFT                          7
// bit[6], CMP
#define VSPI_FLASH_SR_CMP_MASK                  ((UINT8)0x1)
#define VSPI_FLASH_SR_CMP_SHFT                  6

// bit[5], lb3

// bit[4], lb2

// bit[3], lb1
#define VSPI_FLASHGD_SR_LB_MASK                     ((UINT8)0x1)
#define VSPI_FLASHGD_SR_LB_SHFT                     2

// bit[2], sus2

// bit[1], QE enable status (SR9) for GD/PUYA/WB Flash.
#define VSPI_FLASH_GD_SR_QE_BIT                     ((UINT8)0x02)
#define VSPI_FLASH_GD_SR_QE_SHFT                     1

// bit[6], QE enable statu (SR6) for MX Flash.
#define VSPI_FLASH_MX_SR_QE_BIT                                   ((UINT8)0x40)
#define VSPI_FLASH_MX_SR_QE_SHFT                     6

// bit[0], sector protection registers locked
#define VSPI_FLASH_SR_SPRL_MASK1                    ((UINT8)0x1)
#define VSPI_FLASH_SR_SPRL_SHFT1                            0

//byte2

// bit[7], sector protection registers locked
#define VSPI_FLASH_SR_SPRL_MASK0                    ((UINT8)0x1)
#define VSPI_FLASH_SR_SPRL_SHFT0                            7

// bit[6:2], BP[3:0].
#define VSPI_FLASH_SR_BP_MASK                   ((UINT8)0xF)
#define VSPI_FLASH_SR_BP_SHFT                   2

// bit[1], write enable latch status.
#define VSPI_FLASH_SR_WEL_MASK                  ((UINT8)0x1)
#define VSPI_FLASH_SR_WEL_SHFT                  1
// bit[0], ready or busy. 0 - rerady; 1 - busy.
#define VSPI_FLASH_SR_RDY_BUSY_MASK                 ((UINT8)0x1)
#define VSPI_FLASH_SR_RDY_BUSY_SHFT                 0

//status bit definitions for ESMT/EON OTP mode
//SR6 WHDIS bit, 1-WP# and HOLD# Disable; 0-Enable
#define VSPI_FLASH_EON_SR_WHDIS_BIT       ((UINT8)0x40)
#define VSPI_FLASH_EON_SR_WHDIS_SHFT      6

extern VSPI_FLASH_CONFIG g_VSPI_FLASH_Cfg;
extern VSPI_FLASH_INFO g_VSPI_FLASH_Info;

#define VSPI_EXT_FLASH_SIZE   (g_VSPI_FLASH_Info.flash[0].size)
#define VSPI_INT_FLASH_SIZE   (g_VSPI_FLASH_Info.flash[1].size)

//
// Exported routines
//
VRET_VALUE VSPI_FLASH_ReadMID ( UINT8 *id, UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_ReadUniqueID ( UINT8 *Uid, UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_ReadSFDP ( UINT8 *uSFDP, UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_InitParam ( UINT8 flash_sel, const VSPI_FLASH_RAW_PARAM *param );

VRET_VALUE VSPI_FLASH_EraseChip ( UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_Erase ( UINT8 flash_sel, UINT32 addr, UINT32 siz );
VRET_VALUE VSPI_FLASH_Program ( UINT8 flash_sel, UINT32 addr, const UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_FLASH_Read ( UINT8 flash_sel, UINT32 addr, UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_GetFlashParam ( UINT32 *pExtSize, UINT32 *pIntSize, UINT32 *pSectorSize, UINT32 *pPageSize );
VRET_VALUE VSPI_FLASH_WriteParam ( void );
VRET_VALUE VSPI_FLASH_SetQuad ( BOOL quad_or_not, UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_RstCrm ( UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_RstHpm ( UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_SetHpm ( UINT8 flash_sel );
void VSPI_FLASH_ExposeAccess ( void );
BOOL VSPI_FLASH_Is_CS_Toggled ( void );

VRET_VALUE VSPI_FLASH_EON_WHDIS_Set ( BOOL Disable, UINT8 flash_sel );

void VSPI_EncryptEn ( void );
void VSPI_EncryptDis ( void );
void VSPI_EncryptKey ( UINT16 key );

//add 20200819
VRET_VALUE VSPI_FLASH_Init_Ex ( UINT8 flash_sel );


//add 20200221
VRET_VALUE VSPI_Cache_Flash_SetQuad ( BOOL quad_or_not ,UINT8 flash_sel);

//add 20200228
VRET_VALUE VSPI_Set_TOPBase ( UINT32 addr );
UINT32 VSPI_Get_TOPBase ( void );

VRET_VALUE VSPI_Set_BTBase ( UINT32 addr );
UINT32 VSPI_Get_BTBase(void);

VRET_VALUE VSPI_Set_DSPBase ( UINT32 addr );
UINT32 VSPI_Get_DSPBase(void);

void VSPI_Set_EncryptKey ( UINT16 key_top, UINT16 key_bt );
void VSPI_Set_EncryptKey2(UINT16 key_dsp, UINT16 key_data);

/************ OTP *************/
VRET_VALUE VSPI_OTP_Erase ( UINT8 flash_sel, UINT32 addr, UINT32 siz );
VRET_VALUE VSPI_OTP_Program ( UINT8 flash_sel, UINT32 addr, const UINT8 *buf, UINT32 siz );
VRET_VALUE VSPI_OTP_Read ( UINT8 flash_sel, UINT32 addr, UINT8 *buf, UINT32 siz );



#define FLASH_QUAD_ENABLE 1  /* 1: QUAD; 0: SINGLE */

VRET_VALUE VSPI_FLASH_Init ( void );
VRET_VALUE VSPI_FLASH_Init_Mode ( BOOL quad_or_not, UINT8 flash_sel );
VRET_VALUE VSPI_FLASH_ReadBytes ( UINT8 cmd, UINT8 len, UINT8 *bytes, UINT8 flash_sel, UINT32 count );

/* Read the flash's manufacture ID (4 bytes). */
#define VSPI_FLASH_ReadMID(id, flash_sel) VSPI_FLASH_ReadBytes(VSPI_FLASH_CMD_READ_MANUFACTURE_ID, 3, id, flash_sel, 200 )

void VSPI_ReconfigureSpiClk (UINT32 Sys_clock_freq);
FLASH_VOL VSPI_GetFLASH_voltage(UINT8 flash_sel);

#ifdef __cplusplus
}
#endif

#endif  /* _VSPI_FLASH_H_ */

