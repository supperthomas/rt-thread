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
    Designed for EON, Giga, MX, KH, take atmel AT25DF serials into consideration as well.

Revision History:
    Date                  Author                   Description
    2013.07.26           zuolongjun              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_spi.h"
#include "vdrv_spiflash.h"
#include "vdrv_gpio.h"
#include "vdrv_dma.h"
#include "vdrv_ckm.h"
#include "vreg_def.h"

/* function. */
static VRET_VALUE VSPI_FLASH_wait_wip_status ( UINT8 flash_sel, UINT32 time_us );
static VRET_VALUE VSPI_FLASH_WaitReady ( UINT32 timeout_us, UINT8 flash_sel );

VRET_VALUE VSPI_FLASHMX_SetQuad ( BOOL quad_or_not, UINT8 flash_sel );

/* variable */
VSPI_FLASH_CONFIG g_VSPI_FLASH_Cfg;
VSPI_FLASH_INFO g_VSPI_FLASH_Info = {0};
BOOL g_VSPI_FLASH_Lock = TRUE;

/* function macro. */
#define VSPI_FLASH_WriteStatus(status)                    VSPI_FLASH_WriteStatusRegister(VSPI_FLASH_CMD_WRITE_STATUS_BYTE, status, 1, flash_sel)
#define VSPI_FLASH_WriteStatus_16bit(status)              VSPI_FLASH_WriteStatusRegister(VSPI_FLASH_CMD_WRITE_STATUS_BYTE, status, 2, flash_sel)
#define VSPI_FLASH_WriteStatus_32bit_1(status)            VSPI_FLASH_WriteStatusRegister(VSPI_FLASH_CMD_WRITE_STATUS_BYTE1, status, 1, flash_sel)
#define VSPI_FLASH_WriteStatus_32bit_2(status)            VSPI_FLASH_WriteStatusRegister(VSPI_FLASH_CMD_WRITE_STATUS_BYTE2, status, 1, flash_sel)

#define VSPI_FLASH_ReadStatusByte(cmd, status)            VSPI_FLASH_ReadStatusRegister(cmd, 1, status, flash_sel, 0)

#define VSPI_FLASHEN_Unlock                               VSPI_FLASHMX_SetQuad
#define VSPI_FLASH_SEL_INDEX()                            (( flash_sel == VSPI_FLASH_EXTERNAL ) ? 0 : 1)
#define VSPI_FLASH_IsReady()                              { err = VSPI_FLASH_CheckFlashRdy ( flash_sel ); if ( err != VRET_NO_ERROR ) return err; }
#define VSPI_FLASH_IsReadyEx()                            { if ( g_VSPI_FLASH_Info.ready & flash_sel ) {return VRET_NO_ERROR;}; }
#define VSPI_FLASH_IsLocked()                             { if ( !VSPI_FLASH_Lock() ) return VRET_DRV_SPI_FLASH_ERR_LOCKED; }

#define VSPI_FLASH_SingleCMD(cmd)                         VSPI_FLASH_ReadBytes ( cmd, 0, NULL, flash_sel, VSPI_FLASH_PROGRAM_TIM_SR )
#define VSPI_FLASH_EON_OTP_ENT()                          VSPI_FLASH_SingleCMD ( VSPI_FLASH_EON_CMD_OTP_ENTRY )
#define VSPI_FLASH_EON_OTP_DIS()                          VSPI_FLASH_SingleCMD ( VSPI_FLASH_CMD_WRITE_DISABLE )

static const UINT8 g_VSPI_FLASH_SupportedMIDs[] = { // To update
    VSPI_FLASH_MID_ATMEL,
    VSPI_FLASH_MID_MX_KH,
    VSPI_FLASH_MID_GIGA_Q,
    VSPI_FLASH_MID_GIGA_D,
    VSPI_FLASH_MID_EON,
    VSPI_FLASH_MID_WB,
    VSPI_FLASH_MID_PUYA
};


BOOL _VSPI_FLASH_IsMidSupported ( UINT8 mid )
{
    int i;
    for ( i = 0; i < sizeof ( g_VSPI_FLASH_SupportedMIDs ); i++ ) {
        if ( mid == g_VSPI_FLASH_SupportedMIDs[i] ) {
            return TRUE;
        }
    }
    return FALSE;
}

// Transfer raw flash parameters (from host PC) into driver format
// which is recognized by higher layer.
static void VSPI_FLASH_SetInfoCfg ( UINT8 flash_sel, const VSPI_FLASH_RAW_PARAM *p_param )
{
    VSPI_PER_FLASH_INFO *flash_info;
    VSPI_PER_FLASH_CONFIG *flash_cfg;
    UINT8 idx;

    idx = VSPI_FLASH_SEL_INDEX();
    flash_info = &g_VSPI_FLASH_Info.flash[idx];
    flash_cfg = &g_VSPI_FLASH_Cfg.flash[idx];

    flash_info->size = p_param->total_size * 1024;
    flash_info->sector_size = p_param->sector_size * 1024;
    flash_info->page_size = ( 1 << p_param->page_size );
    flash_cfg->spi_mode = p_param->spi_mode & 3;
    flash_cfg->dual_mode = p_param->dual_mode & 3;
}

// Transfer flash info into raw format which is stored in flash.
static void VSPI_FLASH_GetRawInfo ( UINT8 flash_sel, VSPI_FLASH_RAW_PARAM *p_param )
{
    VSPI_PER_FLASH_INFO *flash_info;
    VSPI_PER_FLASH_CONFIG *flash_cfg;
    UINT8 idx;

    idx = VSPI_FLASH_SEL_INDEX();
    flash_info = &g_VSPI_FLASH_Info.flash[idx];
    flash_cfg = &g_VSPI_FLASH_Cfg.flash[idx];

    p_param->magic_num = VSPI_FLASH_RAW_INFO_MAGIC_NUM;
    p_param->total_size = flash_info->size / 1024;
    p_param->sector_size = flash_info->sector_size / 1024;
    if ( flash_info->page_size == 0 ) {
        p_param->page_size = 0;
    } else {
        p_param->page_size = 31 - VCOMMON_CLZ ( flash_info->page_size );
    }
    p_param->spi_mode = flash_cfg->spi_mode;
    p_param->dual_mode = flash_cfg->dual_mode;
    p_param->crc = VCOMMON_CRC16 ( ( UINT8 * ) p_param, GET_CRC_OFFSET ( VSPI_FLASH_RAW_PARAM ) );
}

/******************************************************************************
Description:
    Lock the SPI flash to avoid access collision.

Parameters:

Return Value:

Notes:
    A lock shall be available (and then locked) before sending a command to flash, which is unlocked
    after the command has completed.

*****************************************************************************/
static BOOL VSPI_FLASH_Lock ( void )
{
    BOOL result;
    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    result = g_VSPI_FLASH_Lock;
    if ( result == TRUE ) {
        g_VSPI_FLASH_Lock = FALSE;
    }
    VCPU_CRITICAL_EXIT();
    return result;
}

static void VSPI_FLASH_Unlock ( void )
{
    VCPU_SR_ALLOC();
    VCPU_CRITICAL_ENTER();
    g_VSPI_FLASH_Lock = TRUE;
    VCPU_CRITICAL_EXIT();    
}

//
// get flash parameter
//
VRET_VALUE VSPI_GetFlashParam ( UINT32 *pExtSize, UINT32 *pIntSize, UINT32 *pSectorSize, UINT32 *pPageSize )
{
    VRET_VALUE res = VRET_NO_ERROR;
    UINT32 int_size, ext_size, page_size, sector_size;

#define FLASH_INFO(i) (g_VSPI_FLASH_Info.flash[i])

    ext_size = FLASH_INFO ( 0 ).size;
    int_size = FLASH_INFO ( 1 ).size;
    page_size = ( FLASH_INFO ( 0 ).size > 0 ) ? ( FLASH_INFO ( 0 ).page_size ) : ( FLASH_INFO ( 1 ).page_size );
    sector_size = ( FLASH_INFO ( 0 ).size > 0 ) ? ( FLASH_INFO ( 0 ).sector_size ) : ( FLASH_INFO ( 1 ).sector_size );

    /*if both flash are available, their sector size and page size should be equal,
      or else ignore the int-package flash*/
    if ( ( ( ext_size > 0 ) && ( int_size > 0 ) ) &&
            ( ( FLASH_INFO ( 1 ).sector_size != FLASH_INFO ( 0 ).sector_size )
              || ( FLASH_INFO ( 1 ).page_size != FLASH_INFO ( 0 ).page_size ) ) ) {
        int_size = 0;
    }

    if ( ( ( int_size == 0 ) && ( ext_size == 0 ) )
            || ( sector_size == 0 ) || ( page_size == 0 ) ) {
        res = VRET_DRV_SPI_FLASH_ERR_NOT_AVAILABLE;
        return res;
    }

    if ( pExtSize != NULL ) {
        *pExtSize = ext_size;
    }

    if ( pIntSize != NULL ) {
        *pIntSize = int_size;
    }

    if ( pSectorSize != NULL ) {
        *pSectorSize = sector_size;
    }

    if ( pPageSize != NULL ) {
        *pPageSize = page_size;
    }

    return res;
}

/******************************************************************************
Description:
    Check whether flash to be operate on has already been ready.

Parameters:
    flash_rdy: bitmask, bit[0] = 1 inpackage flash is ready; bit[1] = 1 external flash is ready.
    flash_sel: 0, none; 1: in-package flash; 2: external flash. Can't select both at the same time.

Return Value:

Notes:

*****************************************************************************/
static VRET_VALUE VSPI_FLASH_CheckFlashRdy ( UINT8 flash_sel )
{
    if ( ( flash_sel & g_VSPI_FLASH_Info.ready ) > 0 ) {
        return VRET_NO_ERROR;
    } else {
        return VRET_DRV_SPI_FLASH_ERR_NOT_AVAILABLE;
    }
}

/******************************************************************************
Description:
    Read status register.

Parameters:

Return Value:

Notes:
    Some flashes (e.g, Atmel's & Giga's) have two status bytes, while some only have one (e.g, MX).
    The Giga device use a different command to read the 2nd status byte.
    As the 2nd byte is seldomly used, we only consider the 1st status byte here.

*****************************************************************************/
VRET_VALUE VSPI_FLASH_ReadStatusRegister ( UINT8 cmd, UINT8 len, UINT8 *bytes, UINT8 flash_sel, UINT32 count )
{
    VRET_VALUE err;
    UINT8 idx;

    idx = VSPI_FLASH_SEL_INDEX();

    VSPI_ConfigMaster ( VSPI_FLASH_READ_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( cmd, 0 );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN0, VSPI_FLASH_DUMMY_LEN0, len );
    VSPI_StartTransfer();
    err = VSPI_WaitTransferDone ( count );

    if ( ( err == VRET_NO_ERROR ) && len ) {
        VSPI_ReadData ( bytes, len );
    }
    VSPI_ChipSelect ( VSPI_FLASH_NONE );

    return err;
}



/******************************************************************************
Description:
    Read status register.

Parameters:

Return Value:

Notes:
    Some flashes (e.g, Atmel's & Giga's) have two status bytes, while some only have one (e.g, MX).
    The Giga device use a different command to read the 2nd status byte.
    As the 2nd byte is seldomly used, we only consider the 1st status byte here.

*****************************************************************************/
// read all three bytes of status，the status needs to be a three bytes array
VRET_VALUE VSPI_FLASH_ReadAllStatus ( UINT8 *status, UINT8 flash_sel )
{
    VRET_VALUE err;

    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, status );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }
    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, status + 1 );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }
    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG2, status + 2 );

    return err;
}

/******************************************************************************
Description:
    Wait until Flash is not busy.

Parameters:

Return Value:

Notes:

*****************************************************************************/
static VRET_VALUE VSPI_FLASH_WaitReady ( UINT32 timeout_us, UINT8 flash_sel )
{
    VRET_VALUE err;
    UINT32 i;
    UINT8 status[2];

    timeout_us = ( timeout_us > 0 ) ? timeout_us : 1;

    i = timeout_us * 96; // Considering the worst case: highest system frequency 96MHz.
    while ( i ) {
        err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, status );
        if ( err ) {
            return err;
        }

        if ( ! ( ( status[0] >> VSPI_FLASH_SR_RDY_BUSY_SHFT ) & VSPI_FLASH_SR_RDY_BUSY_MASK ) ) { // ready
            //@9. If the erasing operation is done, write "1" to register SPI_PROG_QUIT
            //add 20200211
            if ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_PROG_TIMEUP_SHFT ) == 0 ) {
                WRITE_REG ( VSPI_PROG_QUIT, 1 );
            }

            return VRET_NO_ERROR;
        }
        i --;
    }
    return VRET_DRV_SPI_FLASH_ERR_TIMEOUT;
}

/******************************************************************************
Description:
    Set the flash to be write-enabled or not.

Parameters:
    flash_sel: 0, none; 1: in-package flash; 2: external flash.

Return Value:

Notes:

*****************************************************************************/
VRET_VALUE VSPI_FLASH_ReadBytes ( UINT8 cmd, UINT8 len, UINT8 *bytes, UINT8 flash_sel, UINT32 count )
{
    VRET_VALUE err;

    UINT8 idx;

    idx = VSPI_FLASH_SEL_INDEX();

    VSPI_FLASH_IsLocked();

    VSPI_ConfigMaster ( VSPI_FLASH_READ_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( cmd, 0 );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN0, VSPI_FLASH_DUMMY_LEN0, len );
    VSPI_StartTransfer();
    if ( cmd == VSPI_FLASH_CMD_READ_MANUFACTURE_ID || \
            cmd == VSPI_FLASH_CMD_RESUME_FROM_DEEP_PWR_DOWN || \
            cmd == VSPI_FLASH_CMD_HPM ) {
        err = VSPI_WaitTransferDone ( count );
    } else {
        /* RSTEN, RST, WRITE_ENABLE, WRITE_DISABLE, */
        err = VSPI_FLASH_wait_wip_status ( flash_sel, 0 );
    }

    if ( ( err == VRET_NO_ERROR ) && len ) {
        VSPI_ReadData ( bytes, len );
    }
    VSPI_ChipSelect ( VSPI_FLASH_NONE );

    VSPI_FLASH_Unlock();

    return err;
}

static VRET_VALUE VSPI_FLASH_SetWriteEnable ( BOOL enable, UINT8 flash_sel )
{
    UINT8 cmd = ( enable == TRUE ) ? VSPI_FLASH_CMD_WRITE_ENABLE : VSPI_FLASH_CMD_WRITE_DISABLE;

    return VSPI_FLASH_SingleCMD ( cmd );
}

/******************************************************************************
Description:
    Write status register.

Parameters:

Return Value:

Notes:
    We need use quad mode flash, need write the 2byte status reg.
    As some devices' (e.g, Giga & MX) status bits are non-volatile, WIP bit shall be checked to determine whether
    they are programmed.

*****************************************************************************/
VRET_VALUE VSPI_FLASH_WriteStatusRegister ( UINT8 cmd, const UINT8 *status, UINT8 len, UINT8 flash_sel )
{
    VRET_VALUE err;
    UINT32 idx;

    idx = VSPI_FLASH_SEL_INDEX();

    err = VSPI_FLASH_SetWriteEnable ( TRUE, flash_sel );
    if ( err == VRET_NO_ERROR ) {
        VSPI_FLASH_IsLocked();
        //VSPI_ProgCycle_Tprog ( g_VSPI_FLASH_Info.flash[idx].tw_cycle );
        VSPI_ConfigMaster ( VSPI_FLASH_WRITE_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
        VSPI_ChipSelect ( flash_sel );
        VSPI_WriteCmdAddr ( cmd, 0 );
        VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN0, VSPI_FLASH_DUMMY_LEN0, len );
        VSPI_WriteData ( status, len );
        VSPI_StartTransfer();
        //err = VSPI_WaitTransferDone ( g_VSPI_FLASH_Info.flash[idx].tw_cycle );
        err = VSPI_FLASH_wait_wip_status ( flash_sel, 0 );
        VSPI_ChipSelect ( VSPI_FLASH_NONE );
        VSPI_FLASH_Unlock();
    }

    return err;
}

/* write all status */
VRET_VALUE VSPI_FLASH_WriteAllStatus ( const UINT8 *status, UINT8 flash_sel )
{
    VRET_VALUE err;

    err = VSPI_FLASH_WriteStatus ( status );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }
    err = VSPI_FLASH_WriteStatus_32bit_1 ( status + 1 );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }
    err = VSPI_FLASH_WriteStatus_32bit_2 ( status + 2 );

    return err;
}

/* set Hpm */
VRET_VALUE VSPI_FLASH_SetHpm ( UINT8 flash_sel )
{
    UINT32 idx = VSPI_FLASH_SEL_INDEX();

    if ( g_VSPI_FLASH_Info.flash[idx].mid[0] != VSPI_FLASH_MID_GIGA_Q ) {
        return VRET_NO_ERROR;
    }

    return VSPI_FLASH_ReadBytes ( VSPI_FLASH_CMD_HPM, 0, NULL, flash_sel, 0 );
}

/* Reset Hpm */
VRET_VALUE VSPI_FLASH_RstHpm ( UINT8 flash_sel )
{
    UINT32 idx = VSPI_FLASH_SEL_INDEX();

    if ( g_VSPI_FLASH_Info.flash[idx].mid[0] != VSPI_FLASH_MID_GIGA_Q ) {
        return VRET_NO_ERROR;
    }

    return VSPI_FLASH_ReadBytes ( VSPI_FLASH_CMD_RESUME_FROM_DEEP_PWR_DOWN, 0, NULL, flash_sel, 0 );
}

/* Wh dis set */
VRET_VALUE VSPI_FLASH_EON_WHDIS_Set ( BOOL Disable, UINT8 flash_sel )
{
    UINT8 status;
    VRET_VALUE err;

    // err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, &status );
    // if ( err != VRET_NO_ERROR ) {
    //     return err;
    // }
    // err = VSPI_FLASH_WriteStatus (&status );
    // if ( err != VRET_NO_ERROR ) {
    //     return err;
    // }
    // // check whether    == 1 again, if not the flash is likely be hardware protected.
    // status = 0;
    // VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, &status );


    // //err = VSPI_FLASH_EON_OTP_ENT();
    // if ( err != VRET_NO_ERROR ) {
    //     return err;
    // }

    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, &status );

    if ( Disable ) { //Set WHDIS bit
        status |= 0x02;
    } else {     //reset WHDIS bit to 0
        status &= ~0x02;
    }

    err = VSPI_FLASH_WriteStatus_32bit_1 ( &status );

    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, &status );

    //err = VSPI_FLASH_EON_OTP_DIS();

    return err;
}

/* read Unique ID */
VRET_VALUE VSPI_FLASH_ReadUniqueID ( UINT8 *Uid, UINT8 flash_sel )
{
    VRET_VALUE err;
    UINT32 idx;
    UINT8 mid, cmd = VSPI_FLASH_CMD_READ_UNIQUE_ID;

    idx = VSPI_FLASH_SEL_INDEX();

    VSPI_FLASH_IsLocked();

    mid = g_VSPI_FLASH_Info.flash[idx].mid[0];
    if ( mid == VSPI_FLASH_MID_EON || mid == VSPI_FLASH_MID_MX_KH ) {
        cmd = VSPI_FLASH_CMD_READ_SFDP;
    }

    VSPI_ConfigMaster ( VSPI_FLASH_READ_MODE, VSPI_FLASH_USE_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( cmd, 0 );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN0, VSPI_FLASH_DUMMY_LEN4, VSPI_FLASH_UNIQUEID_LEN );

    VSPI_ReadData_DMA ( Uid, VSPI_FLASH_UNIQUEID_LEN );

    VSPI_StartTransfer();
    err = VSPI_WaitTransferDone ( 0 );

    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

//
// Read the flash's Serial Flash Discoverable Parameter (0x6C bytes).
//
VRET_VALUE VSPI_FLASH_ReadSFDP ( UINT8 *uSFDP, UINT8 flash_sel )
{
    VRET_VALUE err;
    UINT32 idx;

    idx = VSPI_FLASH_SEL_INDEX();

    VSPI_FLASH_IsLocked();

    VSPI_ConfigMaster ( VSPI_FLASH_READ_MODE, VSPI_FLASH_USE_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( VSPI_FLASH_CMD_READ_SFDP, 0 );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN3, VSPI_FLASH_DUMMY_LEN1, VSPI_FLASH_SFDP_LEN );

    VSPI_ReadData_DMA ( uSFDP, VSPI_FLASH_SFDP_LEN );

    VSPI_StartTransfer();
    err = VSPI_WaitTransferDone ( 0 );

    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

/******************************************************************************
Description:
    Initial SPI flash, determine whether flashes are available and the information of them by reading MID.

Parameters:

Return Value:

Notes:
    Get flash information such as MID, capacity and so on.
    manufacture ID = mid[0].
    For Giga & MX & EON & Windbond
        mid[2].bit[3:0] = 0~8 for 512Kb, 1Mb, 2Mb, 4Mb, 8Mb, 16Mb, 32Mb, 64Mb and 128Mbit.
    For Atmel
        mid[1].bit[4:0] = 0~5 for 256Kb ~ 8Mb.

    Deprecated.
    This routine is usually called in engineering/test mode in which the flash info is not available from
    inside the flash.

*****************************************************************************/
VRET_VALUE VSPI_FLASH_Init_Ex ( UINT8 flash_sel )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 *mid, *Uid;
    VSPI_PER_FLASH_INFO *flash_info;
    UINT8 idx;

    if ( ! ( flash_sel & VSPI_FLASH_BOTH ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }
    WRITE_REG ( VSPI_PROG_CYCL, 0xFFFFFFFF );

    g_VSPI_FLASH_Info.ready &= ( ~flash_sel );

    idx = VSPI_FLASH_SEL_INDEX();

    flash_info = &g_VSPI_FLASH_Info.flash[idx];
    mid = flash_info->mid;
    err = VSPI_FLASH_ReadMID ( mid, flash_sel );

    if ( err == VRET_NO_ERROR ) {
        if ( mid[0] == VSPI_FLASH_MID_ATMEL ) {
            flash_info->size = ( 1 << ( mid[1] & 0x1F ) ) * 32 * 1024; // .byte
            flash_info->sector_size = VSPI_FLASH_SECTOR_SIZE; // .byte
            flash_info->page_size = VSPI_FLASH_PAGE_SIZE; // .byte
            g_VSPI_FLASH_Info.ready |= flash_sel;
        } else if ( ( mid[0] == VSPI_FLASH_MID_MX_KH ) || \
                    ( mid[0] == VSPI_FLASH_MID_GIGA_Q ) || ( mid[0] == VSPI_FLASH_MID_GIGA_D ) || \
                    ( mid[0] == VSPI_FLASH_MID_EON ) || ( mid[0] == VSPI_FLASH_MID_PUYA ) || \
                    ( mid[0] == VSPI_FLASH_MID_WB ) ) {

            flash_info->size = ( 1 << ( mid[2] & 0x0F ) ) * 64 * 1024; // .byte
            flash_info->sector_size = VSPI_FLASH_SECTOR_SIZE; // .byte
            flash_info->page_size = VSPI_FLASH_PAGE_SIZE; // .byte
            g_VSPI_FLASH_Info.ready |= ( flash_sel );

            if ( mid[0] == VSPI_FLASH_MID_GIGA_Q ) {
                if(mid[1] == 0x60)
					flash_info->device_voltage = FLASH_1_8v_VOL; //1.8v
				else if(mid[1] == 0x40)
					flash_info->device_voltage = FLASH_3_3v_VOL; //3.3v
            } else if ( mid[0] == VSPI_FLASH_MID_PUYA ) {
                UINT8 uSFDP[VSPI_FLASH_SFDP_LEN] = {0,};
                VSPI_FLASH_ReadSFDP(uSFDP, flash_sel);
                if(*(UINT16 *)(uSFDP+96) == 0x2000)
                    flash_info->device_voltage = FLASH_1_8v_VOL; //1.8v
                else if(*(UINT16 *)(uSFDP+96) == 0x3600)
                    flash_info->device_voltage = FLASH_3_3v_VOL; //3.38v
            }
        } else {// other flash type
            if ( ( mid[0] != 0 ) && ( mid[0] != 0xFF ) && ( mid[2] != 0 ) && ( mid[2] != 0xFF ) ) {
                g_VSPI_FLASH_Info.ready |= flash_sel ;
                flash_info->size = 0x400000; // .byte
                flash_info->sector_size = VSPI_FLASH_SECTOR_SIZE; // .byte
                flash_info->page_size = VSPI_FLASH_PAGE_SIZE; // .byte
            }
        }
        Uid = flash_info->Uid;
        err = VSPI_FLASH_ReadUniqueID ( Uid, flash_sel );
    } //end of if ( err == VRET_NO_ERROR )

    VSPI_FLASH_IsReady();

    return VRET_NO_ERROR;
}

/* set cache quad and other config. */
VRET_VALUE VSPI_Cache_Flash_SetQuad ( BOOL quad_or_not ,UINT8 flash_sel)
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 idx = VSPI_FLASH_SEL_INDEX();
    UINT8 mid = g_VSPI_FLASH_Info.flash[idx].mid[0];

    VSPI_FLASH_IsReady();

    //Need to set WHDIS status bit for ESMT/EON Flash.
    if ( mid == VSPI_FLASH_MID_EON ) {
        //VSPI_FLASHEN_Unlock ( 0, flash_sel );
        VSPI_FLASH_EON_WHDIS_Set ( 1, flash_sel );
        //for EN25QH32B(2B)_for Enhanced Function only, need to modify resume/suspend command opcode 30h/B0h
        VSPI_CmdPer ( 0x30 );
        VSPI_CmdPes ( 0xB0 );
    }

    err = VSPI_FLASH_SetQuad ( quad_or_not, flash_sel );
    if ( ( err == VRET_NO_ERROR ) && quad_or_not ) { //set spi quad mode (4line)
        //Need to set Cache quad read command to EBH for ESMT/EON/MX Flash.
        if ( ( mid == VSPI_FLASH_MID_EON ) || ( mid == VSPI_FLASH_MID_MX_KH ) ) {
            //set 2 dummy byte for quad read command EBH
            SET_REG_BIT2 ( VSPI_CACH_CTRL, VSPI_PADCTRL_CACHE_QUAD_DUMMY_SHFT );
            //set Cache quad read command to EBH
            WRITE_REG ( VSPI_CACH_CMD, VSPI_FLASH_CMD_IO_QUAD_3DUMMY );
        }
        g_VSPI_FLASH_Cfg.flash[idx].dual_mode = VSPI_FLASH_QUAD_MODE;

        /* config spi cache ctrl, enable 4 lines */
        SET_REG_BIT2 ( VSPI_CACH_CTRL, VSPI_CACHECTRL_QUAD_ENABLE_SHFT );
    } else { //set spi single mode (1line)
        g_VSPI_FLASH_Cfg.flash[idx].dual_mode = VSPI_FLASH_SING_MODE;

        /* config spi cache ctrl, disable 4 lines */
        CLR_REG_BIT2 ( VSPI_CACH_CTRL, VSPI_CACHECTRL_QUAD_ENABLE_SHFT );
    }

    return err;
}

/******************************************************************************
Description:
    The host PC initialize parameters for the in-package and external flash.

Parameters:
    flash_sel: VSPI_FLASH_EXTERNAL, VSPI_FLASH_INPACKAGE, VSPI_FLASH_BOTH
    param[2]:
        It shall contain two elements even though less than two flashes is selected.
        [0] and [1] are parameters for in-package and external flash respectively, all zero for invalid parameters.

Return Value:
    g_VSPI_FLASH_Info will be updated so that the higher layer may operate on flashes properly.

Notes:

*****************************************************************************/
VRET_VALUE VSPI_FLASH_InitParam ( UINT8 flash_sel, const VSPI_FLASH_RAW_PARAM *param )
{
    if ( ( flash_sel & VSPI_FLASH_BOTH ) == 0 ) { // At least a flash shall be available.
        return VRET_DRV_SPI_FLASH_ERR_NOT_AVAILABLE;
    }

    // external flash
    if ( flash_sel & VSPI_FLASH_EXTERNAL ) {
        VSPI_FLASH_SetInfoCfg ( VSPI_FLASH_EXTERNAL, &param[0] );
    }

    // internal flash
    if ( flash_sel & VSPI_FLASH_INPACKAGE ) {
        VSPI_FLASH_SetInfoCfg ( VSPI_FLASH_INPACKAGE, &param[1] );
    }
    g_VSPI_FLASH_Info.ready = flash_sel;

    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Write flash parameters into the in-package/external flash.

Parameters:

Return Value:

Notes:
    Before calling this routine, the flashes shall have been initialized by host PC or by reading MID.
    The flash parameters will be written to the 1st page of in-package flash if it is available, otherwise to external flash.
    Make sure the sector0 has been erased before this routine is called.

*****************************************************************************/
VRET_VALUE VSPI_FLASH_WriteParam ( void )
{
    VRET_VALUE err = VRET_NO_ERROR;
    VSPI_FLASH_RAW_PARAM raw_info;
    UINT32 addr;
    UINT8 ready;

    ready = g_VSPI_FLASH_Info.ready;
    if ( ( ready & VSPI_FLASH_BOTH ) == 0 ) { // At least a flash shall be available.
        return VRET_DRV_SPI_FLASH_ERR_NOT_AVAILABLE;
    }

    // Write flash parameters into external flash
    if ( ready & VSPI_FLASH_EXTERNAL ) {
        // Write in-package flash info
        VSPI_FLASH_GetRawInfo ( VSPI_FLASH_EXTERNAL, &raw_info );
        addr = 0;
        err = VSPI_FLASH_Program ( VSPI_FLASH_EXTERNAL, addr, ( const UINT8 * ) ( &raw_info ), sizeof ( VSPI_FLASH_RAW_PARAM ) );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }

        // Write external flash info
        if ( ready & VSPI_FLASH_INPACKAGE ) {
            VSPI_FLASH_GetRawInfo ( VSPI_FLASH_INPACKAGE, &raw_info );
        } else {
            VCOMMON_MemSet ( &raw_info, 0, sizeof ( VSPI_FLASH_RAW_PARAM ) );
        }
        addr = sizeof ( VSPI_FLASH_RAW_PARAM );
        err = VSPI_FLASH_Program ( VSPI_FLASH_EXTERNAL, addr, ( const UINT8 * ) ( &raw_info ), sizeof ( VSPI_FLASH_RAW_PARAM ) );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }
    } else if ( ready & VSPI_FLASH_INPACKAGE ) { // write to in-package flash
        // Write in-package flash info
        VCOMMON_MemSet ( &raw_info, 0, sizeof ( VSPI_FLASH_RAW_PARAM ) );
        addr = 1 << 24;
        err = VSPI_FLASH_Program ( VSPI_FLASH_INPACKAGE, addr, ( const UINT8 * ) ( &raw_info ), sizeof ( VSPI_FLASH_RAW_PARAM ) );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }

        // Write external flash info
        VSPI_FLASH_GetRawInfo ( VSPI_FLASH_INPACKAGE, &raw_info );
        addr = ( 1 << 24 ) + sizeof ( VSPI_FLASH_RAW_PARAM );
        err = VSPI_FLASH_Program ( VSPI_FLASH_INPACKAGE, addr, ( const UINT8 * ) ( &raw_info ), sizeof ( VSPI_FLASH_RAW_PARAM ) );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }
    }

    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Global protect/unprotect.

Parameters:

Return Value:
    We assume that the device is neither hardware nor software protected (WP_N = 1, SPRL = 0).
    If (SPRL == 1) we set it to 0.
    If failed, the flash may be hardware protected.

    We set BP = n'b0 to achieve a global unprotect;
    or set BP = n'b1 to achieve a global protect.
    n depends on the device ID.

    For MX devices:
        4'b0 <=>none protected;
        3'b111 => all protected.
    For Giga devices:
        4'b0 <=> none protected;
        3'b111 <=> all protected.
    For Atmel devices (As SR is volatile, spare SR wirtes matter a little):
        2'b0 <=> none protected.
        2'b1<=> all protected.

Notes:

*****************************************************************************/
VRET_VALUE _VSPI_FLASH_SetGlobalProtect ( BOOL protect_or_not, UINT8 flash_sel, UINT8 *status )
{   
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 statusr[1];
    err = VSPI_FLASH_WriteStatus_16bit ( status );

    VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, statusr );
    if ( statusr[0] & ( 1 << VSPI_FLASH_SR_SPRL_SHFT0 ) ) {
        err = VRET_DRV_SPI_FLASH_ERR_PROTECTED;
    }

    return err;
}

VRET_VALUE VSPI_FLASH_SetGlobalProtect ( BOOL protect_or_not, UINT8 flash_sel )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 status[3] = {0}, statusr[1];
    UINT8 bp_bits;
    UINT8 cmp_bits;
    BOOL need_write_sr = FALSE;

    err = VSPI_FLASH_ReadAllStatus ( status, flash_sel );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    // check whether SPRL0 == 0, if not the flash may be either software or hardware protected.
    if ( status[0] & ( 1 << VSPI_FLASH_SR_SPRL_SHFT0 ) ) {
        status[0] &= ~ ( 1 << VSPI_FLASH_SR_SPRL_SHFT0 );
        need_write_sr = TRUE;
    }

    // check BP[], write 0/1 for global unprotect/protect.
    bp_bits = ( status[0] >> VSPI_FLASH_SR_BP_SHFT ) & VSPI_FLASH_SR_BP_MASK;
    if ( protect_or_not == TRUE ) { // global protect
        if ( bp_bits != VSPI_FLASH_SR_BP_MASK ) { // The device may not be global protected
            status[0] |= ( VSPI_FLASH_SR_BP_MASK << VSPI_FLASH_SR_BP_SHFT );
            need_write_sr = TRUE;
        }
    } else { // unprotect
        if ( bp_bits != 0 ) { // The device may not be global unprotected
            status[0] &= ( ~ ( VSPI_FLASH_SR_BP_MASK << VSPI_FLASH_SR_BP_SHFT ) );
            need_write_sr = TRUE;
        }
    }

    //check CMP[],and set to 0
    cmp_bits = ( status[1] >> VSPI_FLASH_SR_CMP_SHFT ) & VSPI_FLASH_SR_CMP_MASK;
    if ( cmp_bits ) {
        need_write_sr = TRUE;
        status[1] &= ( ~ ( VSPI_FLASH_SR_CMP_MASK << VSPI_FLASH_SR_CMP_SHFT ) );
    }

    if ( need_write_sr == TRUE ) {
        VSPI_FLASH_WriteAllStatus ( status, flash_sel );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }
    }

    // check whether SPRL == 0 again, if not the flash is likely be hardware protected.
    //VSPI_FLASH_ReadAllStatus ( status, flash_sel );
    //if ( status[0] & ( 1 << VSPI_FLASH_SR_SPRL_SHFT0 ) ) {
    //    return VRET_DRV_SPI_FLASH_ERR_PROTECTED;
    //}
    
    VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, statusr );
    if ( statusr[0] & ( 1 << VSPI_FLASH_SR_SPRL_SHFT0 ) ) {
        err = VRET_DRV_SPI_FLASH_ERR_PROTECTED;
        if ( need_write_sr ) {
            err = _VSPI_FLASH_SetGlobalProtect ( protect_or_not, flash_sel, status );
        }
    }
    return err;
}


/*
 *  new method  check wip status
*/
static VRET_VALUE get_flash_status_in_progcycle ( UINT8 flash_sel, UINT8 *status )
{
    volatile UINT32 trans_done, cmd_wait;
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 timeout;

    //读SPI_STATUS[9](prog_timeup). 如果是1 , erase / program 已经完成,不再支持硬件自动查询Flash status
    if ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_PROG_TIMEUP_SHFT ) ) {

        //为了确保Flash erase/program 真正完成，需要再发0x05命令读Flash status(WIP)来确认是否已经完成
        //首先对Flash unlock，在本函数外对Flash lock，而函数VSPI_FLASH_ReadStatus中又要flash lock，为了能成功调用此函数所以这里需要先Flash unlock
        VSPI_FLASH_Unlock();
        err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, status );

        return err;
    }

    //in prog cycle 中读flash状态,直接设置spi start ,wait done。硬件自动查询
    VSPI_StartTransfer();
    timeout = 400000;   //about 2s
    while ( timeout ) {
        VCOMMON_Delay ( 5 );

        trans_done = GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_TRANSFER_DONE_SHFT );
        cmd_wait =  GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_CMD_WAIT_SHFT );
        if ( ( trans_done == 1 ) && ( cmd_wait == 0 ) ) {
            break;
        }

        timeout--;
    }
    /* timeout */
    if ( timeout == 0 ) {
        if ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_PROG_TIMEUP_SHFT ) == 0 ) {
            WRITE_REG ( VSPI_PROG_QUIT, 1 );
        }
        return VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;
    }

    VSPI_ReadData ( status, 1 );

    return VRET_NO_ERROR;
}

static VRET_VALUE VSPI_FLASH_wait_wip_status ( UINT8 flash_sel, UINT32 time_us )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 status[2],use_dma;
    UINT32 timeout;
    volatile UINT32 trans_done, cmd_wait;


    //等待 erase/ program 命令发送完成。SPI_STATUS[8](cmd_wait)是0,且SPI_STATUS[1](trans_done)是1
    // Wait until transactions on SPI bus have completed and Tx cmd wait is 0.
    timeout = 1000000;   //about 5s
    use_dma = GET_REG_BIT2 ( VSPI_CTRLR0, VSPI_CTRLR0_DMAM_SHFT );
    while ( timeout ) {
        VCOMMON_Delay ( 5 ); //5

        trans_done = GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_TRANSFER_DONE_SHFT );
        cmd_wait =  GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_CMD_WAIT_SHFT );

        if ( ( trans_done == 1 ) && ( cmd_wait == 0 ) ) {
            break;
        }
        timeout --;
    }

    if ( timeout == 0 ) {
        if ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_PROG_TIMEUP_SHFT ) == 0 ) {
            WRITE_REG ( VSPI_PROG_QUIT, 1 );
        }
        return VRET_DRV_SPI_ERR_WAIT_TRANSFER_DONE_TO;
    }

    //检查flash状态 WIP
    status[0] = 0xFF;
    status[1] = 0xFF;

    //check if prog is done.
    if ( time_us == 0 ) {
        timeout = 300000;
    } else {
        timeout = time_us / 100;    //time_us us, time_us / 100  转换成次数
    }

    VCOMMON_Delay ( 10 ); //20
    while ( timeout ) {
        //读 flash状态寄存器
        err = get_flash_status_in_progcycle ( flash_sel, status );

        if ( err != VRET_NO_ERROR ) {
            break;
        }

        if ( ( status[0] & 0x01 ) == 0 ) {
            break;
        }

        timeout--;
        VCOMMON_Delay ( 100 );   //500
    }

    if ( GET_REG_BIT2 ( VSPI_STATUS, VSPI_SR_PROG_TIMEUP_SHFT ) == 0 ) {
        WRITE_REG ( VSPI_PROG_QUIT, 1 );
    }
    if(use_dma){
        VSPI_DMA_WaitTransferDone(0);
    }
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    if ( timeout == 0 ) {
        return VRET_DRV_SPI_FLASH_ERR_TIMEOUT;
    }

    return VRET_NO_ERROR;
}
/******************************************************************************
Description:
    Erase the whole flash.

Parameters:

Return Value:

Notes:

*****************************************************************************/
VRET_VALUE VSPI_FLASH_TryEraseChip ( UINT8 flash_sel, UINT8 cmd )
{
    VRET_VALUE err = VRET_NO_ERROR;
    volatile UINT32 idx, siz;

    // check parameters.
    VSPI_FLASH_IsReady();

    idx = VSPI_FLASH_SEL_INDEX();
    siz = g_VSPI_FLASH_Info.flash[idx].size;

    // unprotect sectors.
    VSPI_FLASH_SetGlobalProtect ( FALSE, flash_sel );

    VSPI_FLASH_SetWriteEnable ( TRUE, flash_sel );

    VSPI_FLASH_IsLocked();

    VSPI_ProgCycle_Tprog ( 0xFFFF );
    VSPI_ConfigMaster ( VSPI_FLASH_WRITE_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );

    VSPI_WriteCmdAddr ( cmd, 0 );

    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN0, VSPI_FLASH_DUMMY_LEN0, 0 );

    VSPI_StartTransfer();
    err = VSPI_FLASH_wait_wip_status ( flash_sel, VSPI_FLASH_ERASE_TIM_CHIP );

    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

// We first try the 1st erase command, and then the 2nd.
// Flashes supporting either command will work properly.
VRET_VALUE VSPI_FLASH_EraseChip ( UINT8 flash_sel )
{
    if ( VSPI_FLASH_TryEraseChip ( flash_sel, VSPI_FLASH_CMD_ERASE_CHIP1 ) != VRET_NO_ERROR ) {
        return VSPI_FLASH_TryEraseChip ( flash_sel, VSPI_FLASH_CMD_ERASE_CHIP2 );
    }
    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Erase flash.

Parameters:
    addr - addr[24] = 0/1 for reading in-package / external flash. addr[23:0] address inside flash.

Return Value:

Notes:
    Commands erasing 32KB or 64KB may be supported in some flashes, but currently we only
    use the ERASE-4KB command.

*****************************************************************************/
VRET_VALUE VSPI_FLASH_Erase_Block ( UINT8 flash_sel, UINT8 spi_mode, UINT8 eraseCmd, UINT32 addr, UINT32 timeout )
{
    VRET_VALUE err = VRET_NO_ERROR;

    VSPI_FLASH_SetWriteEnable ( TRUE, flash_sel );

    VSPI_FLASH_IsLocked();

    VSPI_ProgCycle_Tprog ( 0xFFFF );
    VSPI_ConfigMaster ( VSPI_FLASH_WRITE_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, spi_mode );

    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( eraseCmd, addr );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN3, VSPI_FLASH_DUMMY_LEN0, 0 );

    VSPI_StartTransfer();

    //wait flash wip status.
    err = VSPI_FLASH_wait_wip_status ( flash_sel, timeout );

    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

VRET_VALUE VSPI_FLASH_Erase ( UINT8 flash_sel, UINT32 addr, UINT32 siz )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8  spi_mode;
    UINT32 idx;

    // check parameters.
    VSPI_FLASH_IsReady();

    if ( ( ( addr & ( VSPI_FLASH_SECTOR_SIZE - 1 ) ) != 0 ) || \
            ( ( siz & ( VSPI_FLASH_SECTOR_SIZE - 1 ) ) != 0 ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }
    idx = VSPI_FLASH_SEL_INDEX();

    spi_mode = g_VSPI_FLASH_Cfg.flash[idx].spi_mode;

    // unprotect sectors.
    VSPI_FLASH_SetGlobalProtect ( FALSE, flash_sel );

    //64KB erase
    while ( ( ( addr & ( ( VSPI_FLASH_SECTOR_SIZE << 4 ) - 1 ) ) == 0 ) && ( siz >= ( VSPI_FLASH_SECTOR_SIZE << 4 ) ) && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Erase_Block ( flash_sel, spi_mode, VSPI_FLASH_CMD_ERASE_BLOCK_64K, addr, VSPI_FLASH_ERASE_TIM_BLOCK64K );

        siz -= ( VSPI_FLASH_SECTOR_SIZE << 4 );
        addr += ( VSPI_FLASH_SECTOR_SIZE << 4 );
    }

    //32KB erase
    while ( ( ( addr & ( ( VSPI_FLASH_SECTOR_SIZE << 3 ) - 1 ) ) == 0 ) && ( siz >= ( VSPI_FLASH_SECTOR_SIZE << 3 ) ) && ( err == VRET_NO_ERROR ) ) {

        err = VSPI_FLASH_Erase_Block ( flash_sel, spi_mode, VSPI_FLASH_CMD_ERASE_BLOCK_32K, addr, VSPI_FLASH_ERASE_TIM_BLOCK32K );

        siz -= ( VSPI_FLASH_SECTOR_SIZE << 3 );
        addr += ( VSPI_FLASH_SECTOR_SIZE << 3 );
    }

    // erase multiple of 4KB blocks.
    while ( ( siz >= VSPI_FLASH_SECTOR_SIZE ) && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Erase_Block ( flash_sel, spi_mode, VSPI_FLASH_CMD_ERASE_BLOCK_4K, addr, VSPI_FLASH_ERASE_TIM_BLOCK4K );

        siz -= VSPI_FLASH_SECTOR_SIZE;
        addr += VSPI_FLASH_SECTOR_SIZE;
    }

    return err;
}

/******************************************************************************
Description:
    Page/Byte write.

Parameters:
    addr - addr[24] = 0/1 for reading in-package / external flash. addr[23:0] address inside flash.
    If addr is page aligned, more than one page can be programmed,
    otherwise, (addr + siz) shall not wrap-around to the next page.
Return Value:

Notes:

*****************************************************************************/
VRET_VALUE VSPI_FLASH_Program_Page ( UINT8 flash_sel, UINT32 addr, const UINT8 *buf, UINT32 siz, BOOL isOTP )
{
    VRET_VALUE err = VRET_NO_ERROR;
    BOOL use_dual;
    UINT8 spi_mode, idx, cmd, dma_sel;
    UINT32 mask, reg_val;

    idx = VSPI_FLASH_SEL_INDEX();

    if ( isOTP ) {
        use_dual = VSPI_FLASH_SING_MODE;
        cmd = VSPI_OTP_CMD_PROGRAM_PAGE;
    } else {
        if ( g_VSPI_FLASH_Cfg.flash[idx].dual_mode != VSPI_FLASH_QUAD_MODE ) {
            use_dual = VSPI_FLASH_SING_MODE;
            cmd = VSPI_FLASH_CMD_PROGRAM_PAGE;
        } else {
            use_dual = VSPI_FLASH_QUAD_MODE;
            if ( g_VSPI_FLASH_Info.flash[idx].mid[0] == VSPI_FLASH_MID_MX_KH ) {
                cmd = VSPI_FLASH_MX_CMD_PROGRAM_PAGE_QUAD;
            } else {
                cmd = VSPI_FLASH_CMD_PROGRAM_PAGE_QUAD;
            }
        }
    }

    VSPI_FLASH_SetWriteEnable ( TRUE, flash_sel );
    VSPI_FLASH_IsLocked();
    VSPI_ProgCycle_Tprog ( 0xFFFF );

    spi_mode = g_VSPI_FLASH_Cfg.flash[idx].spi_mode;
    dma_sel = ( siz > 4 ) ? VSPI_FLASH_USE_DMA : VSPI_FLASH_NO_DMA;

    VSPI_ConfigMaster ( VSPI_FLASH_WRITE_MODE, dma_sel, use_dual, spi_mode );
    if ( cmd == VSPI_FLASH_CMD_PROGRAM_PAGE_QUAD ) {
        mask = ( VSPI_CTRLR1_SPI_MODE_MASK << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
               ( VSPI_CTRLR1_DUAL_MASK << VSPI_CTRLR1_DUAL_SHFT ) | \
               ( VSPI_CTRLR1_SPI_QUAD_MASK << VSPI_CTRLR1_SPI_QUAD_SHFT );

        reg_val = ( READ_REG ( VSPI_CTRLR1 ) & ( ~mask ) ) | ( 0 << VSPI_CTRLR1_DUAL_SHFT ) | \
                  ( 0x2 << VSPI_CTRLR1_SPI_QUAD_SHFT ) | \
                  ( ( spi_mode & VSPI_CTRLR1_SPI_MODE_MASK ) << VSPI_CTRLR1_SPI_MODE_SHFT ) | \
                  ( VSPI_PRE_LIMIT_DEFAULT << VSPI_CTRLR1_PRE_LIMIT_SHFT ) | ( VSPI_POST_LIMIT_DEFAULT << VSPI_CTRLR1_POST_LIMIT_SHFT );
        WRITE_REG ( VSPI_CTRLR1, reg_val );
    }

    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( cmd, addr );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN3, VSPI_FLASH_DUMMY_LEN0, siz );

    if ( dma_sel == VSPI_FLASH_USE_DMA ) {
        VSPI_WriteData_DMA ( buf, siz );    // Start DMA first and then SPI transfer.
    } else {
        VSPI_WriteData ( buf, siz );
    }

    VSPI_StartTransfer();

    //wait flash wip status.
    err = VSPI_FLASH_wait_wip_status ( flash_sel, VSPI_FLASH_PROGRAM_TIM_PAGE );

    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

VRET_VALUE VSPI_FLASH_Program ( UINT8 flash_sel, UINT32 addr, const UINT8 *buf, UINT32 siz )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 *pi;
    UINT32 tmp;

    // check parameters.
    VSPI_FLASH_IsReady();

    tmp = addr & ( VSPI_FLASH_PAGE_SIZE - 1 );
    if ( tmp && ( tmp + siz > VSPI_FLASH_PAGE_SIZE ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }

    // unprotect sectors.
    VSPI_FLASH_SetGlobalProtect ( FALSE, flash_sel );

    // multiple of pages
    pi = ( UINT8 * ) buf;
    while ( ( siz >= VSPI_FLASH_PAGE_SIZE ) && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Program_Page ( flash_sel, addr, pi, VSPI_FLASH_PAGE_SIZE, FALSE );

        siz -= VSPI_FLASH_PAGE_SIZE;
        addr += VSPI_FLASH_PAGE_SIZE;
        pi += VSPI_FLASH_PAGE_SIZE;
    }

    // data less than a pgae.
    if ( ( siz )  && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Program_Page ( flash_sel, addr, pi, siz, FALSE );
    }

    return err;
}


static VRET_VALUE VSPI_FLASH_ReadData ( UINT8 flash_sel, UINT8 cmd, UINT8 dummy_len, BOOL use_dual, UINT32 addr, UINT8 *buf, UINT32 siz )
{
    UINT8 idx, dma_sel;
    VRET_VALUE err = VRET_NO_ERROR;

    idx = VSPI_FLASH_SEL_INDEX();
    dma_sel = ( siz > 4 ) ? VSPI_FLASH_USE_DMA : VSPI_FLASH_NO_DMA;

    VSPI_FLASH_IsLocked();

    VSPI_ConfigMaster ( VSPI_FLASH_READ_MODE, dma_sel, use_dual, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );
    VSPI_ChipSelect ( flash_sel );
    VSPI_WriteCmdAddr ( cmd, addr );
    VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN3, dummy_len, siz );
    if ( dma_sel == VSPI_FLASH_USE_DMA ) {
        VSPI_ReadData_DMA ( buf, siz );    // Start DMA first and then SPI transfer.
    }
    VSPI_StartTransfer();
    err = VSPI_WaitTransferDone ( 0 );
    if ( ( err == VRET_NO_ERROR ) && ( siz <= 4 ) ) {
        VSPI_ReadData ( buf, siz );
    }
    VSPI_ChipSelect ( VSPI_FLASH_NONE );
    VSPI_FLASH_Unlock();

    return err;
}

/******************************************************************************
Description:
    Read flash.

Parameters:
    addr - addr[24] = 0/1 for reading in-package / external flash. addr[23:0] address inside flash.

Return Value:
    None.

Notes:

*****************************************************************************/
VRET_VALUE VSPI_FLASH_Read ( UINT8 flash_sel, UINT32 addr, UINT8 *buf, UINT32 siz )
{
    VRET_VALUE err = VRET_NO_ERROR;
    BOOL use_dual;
    BOOL quad_or_not;
    UINT8 cmd, idx;
    UINT8 dummy_len = VSPI_FLASH_DUMMY_LEN1;
    UINT8 *pi;

    // check parameters.
    VSPI_FLASH_IsReady();

    if ( ( buf == NULL ) || ( siz == 0 ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }

    idx = VSPI_FLASH_SEL_INDEX();
    use_dual = g_VSPI_FLASH_Cfg.flash[idx].dual_mode;
    if ( use_dual == VSPI_FLASH_SING_MODE ) {
        cmd = VSPI_FLASH_CMD_READ_85M_1DUMMY;
        quad_or_not = FALSE;
    } else if ( use_dual == VSPI_FLASH_DUAL_MODE ) {
        cmd = VSPI_FLASH_CMD_IO_DUAL_1DUMMY;
        quad_or_not = FALSE;
    } else if ( use_dual == VSPI_FLASH_QUAD_MODE ) {
        dummy_len = VSPI_FLASH_DUMMY_LEN3;
        cmd = VSPI_FLASH_CMD_IO_QUAD_3DUMMY;
        quad_or_not = TRUE;
    }
    VSPI_FLASH_SetQuad ( quad_or_not, flash_sel );

    // 分包读,每次最多读1024字节
    #define VSPI_FLASH_READ_SIZE       (1024)
    pi = ( UINT8 * ) buf;
    while ( ( siz >= VSPI_FLASH_READ_SIZE ) && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_ReadData ( flash_sel, cmd, dummy_len, use_dual, addr, pi, VSPI_FLASH_READ_SIZE );
        if (err != VRET_NO_ERROR)
        {
            return err;
        }
        siz -= VSPI_FLASH_READ_SIZE;
        addr += VSPI_FLASH_READ_SIZE;
        pi += VSPI_FLASH_READ_SIZE;
    }

    if ( ( siz )  && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_ReadData ( flash_sel, cmd, dummy_len, use_dual, addr, pi, siz );
    }

    return err;
}

// to match status register number is 2 or 3, and the interface to update the status.
VRET_VALUE _VSPI_FLASHGD_SetQuad ( BOOL quad_or_not, UINT8 flash_sel, UINT8 *status )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 mid_cap, statusr[1];
    UINT8 idx = VSPI_FLASH_SEL_INDEX();

    mid_cap = g_VSPI_FLASH_Info.flash[idx].mid[2];
    if ( mid_cap == VSPI_FLASH_CAP_16_BIT ) {
        err = VSPI_FLASH_WriteStatus_32bit_1 ( status + 1 );
    } else if ( mid_cap == VSPI_FLASH_CAP_32_BIT ) {
        err = VSPI_FLASH_WriteStatus_16bit ( status );
    } 
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    // check whether QE == 1 again, if not the flash is likely be hardware protected.
    err = VSPI_FLASH_WaitReady ( VSPI_FLASH_PROGRAM_TIM_SR, flash_sel );
    statusr[0] = 0;
    VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, statusr );
    if ( ( ( statusr[0]&VSPI_FLASH_GD_SR_QE_BIT ) >> VSPI_FLASH_GD_SR_QE_SHFT ) != quad_or_not ) {
        err = VRET_DRV_SPI_FLASH_ERR_SETQE;
    }

    return err;
}

//Set QE.bit(SR9) for GD/PUYA/Winbond flash.
VRET_VALUE VSPI_FLASHGD_SetQuad ( BOOL quad_or_not, UINT8 flash_sel )
{
    VRET_VALUE err = VRET_NO_ERROR;
    BOOL need_write_sr = FALSE;
    UINT8 status[2], statusr[1];
    UINT8 mid_cap = 0;
    UINT8 idx = VSPI_FLASH_SEL_INDEX();

    mid_cap = g_VSPI_FLASH_Info.flash[idx].mid[2];
    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, status + 0 );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, status + 1 );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    // check QE[], write 0/1 for qe enable.
    if ( quad_or_not ) {
        if ( ! ( status[1] & VSPI_FLASH_GD_SR_QE_BIT ) ) {
            status[1] |= VSPI_FLASH_GD_SR_QE_BIT;
            need_write_sr = TRUE;
        }
    } else { // disable
        if ( status[1] &  VSPI_FLASH_GD_SR_QE_BIT ) {
            status[1] &= ( ~ VSPI_FLASH_GD_SR_QE_BIT );
            need_write_sr = TRUE;
        }
    }
    if ( need_write_sr ) {
        if ( mid_cap == VSPI_FLASH_CAP_16_BIT || mid_cap == VSPI_FLASH_CAP_4_BIT) {
            err = VSPI_FLASH_WriteStatus_16bit ( status );
        } else if ( mid_cap == VSPI_FLASH_CAP_32_BIT || mid_cap == VSPI_FLASH_CAP_64_BIT) {
            err = VSPI_FLASH_WriteStatus_32bit_1 ( status + 1 );
        }
        if ( err != VRET_NO_ERROR ) {
            return err;
        }
		// check whether QE == 1 again, if not the flash is likely be hardware protected.
		err = VSPI_FLASH_WaitReady ( VSPI_FLASH_PROGRAM_TIM_SR, flash_sel );
		//status[1] = 0;
		//VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, status + 1 );
		//if ( ( ( status[1]&VSPI_FLASH_GD_SR_QE_BIT ) >> VSPI_FLASH_GD_SR_QE_SHFT ) != quad_or_not ) {
		//    return VRET_DRV_SPI_FLASH_ERR_SETQE;
		//}
		statusr[0] = 0;
		VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG1, statusr );
		if ( ( ( statusr[0]&VSPI_FLASH_GD_SR_QE_BIT ) >> VSPI_FLASH_GD_SR_QE_SHFT ) != quad_or_not ) {
			err = VRET_DRV_SPI_FLASH_ERR_SETQE;
			err = _VSPI_FLASHGD_SetQuad( quad_or_not, flash_sel, status );
		}
    }

    return err;
}

//set QE.bit(SR6) for MX Flash.
VRET_VALUE VSPI_FLASHMX_SetQuad ( BOOL quad_or_not, UINT8 flash_sel )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 status;
    BOOL need_write_sr = FALSE;

    err = VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, &status );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    // check QE[], write 0/1 for qe enable.
    if ( quad_or_not ) {
        if ( ! ( status & VSPI_FLASH_MX_SR_QE_BIT ) ) {
            status |= VSPI_FLASH_MX_SR_QE_BIT;
            need_write_sr = TRUE;
        }
    } else { // disable
        if ( status & VSPI_FLASH_MX_SR_QE_BIT ) {
            status &= ( ~ VSPI_FLASH_MX_SR_QE_BIT );
            need_write_sr = TRUE;
        }
    }
    if ( need_write_sr ) {
        err = VSPI_FLASH_WriteStatus ( &status );
        if ( err != VRET_NO_ERROR ) {
            return err;
        }
		// check whether QE == 1 again, if not the flash is likely be hardware protected.
		status = 0;
		VSPI_FLASH_ReadStatusByte ( VSPI_FLASH_CMD_READ_STATUS_REG, &status );
		if ( ( ( status & VSPI_FLASH_MX_SR_QE_BIT ) >> VSPI_FLASH_MX_SR_QE_SHFT ) != quad_or_not ) {
			return VRET_DRV_SPI_FLASH_ERR_SETQE;
		}
    }

    return err;
}

VRET_VALUE VSPI_FLASH_SetQuad ( BOOL quad_or_not, UINT8 flash_sel )
{
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 idx = VSPI_FLASH_SEL_INDEX();
    UINT8 mid = g_VSPI_FLASH_Info.flash[idx].mid[0];

    if ( mid == VSPI_FLASH_MID_MX_KH ) {
        err = VSPI_FLASHMX_SetQuad ( quad_or_not, flash_sel );
    } else if ( mid != VSPI_FLASH_MID_EON ) {
        err = VSPI_FLASHGD_SetQuad ( quad_or_not, flash_sel );
    }

    return err;
}

VRET_VALUE VSPI_FLASH_RstCrm ( UINT8 flash_sel )
{
    VRET_VALUE err;

    err = VSPI_FLASH_SingleCMD ( VSPI_FLASH_CMD_RSTEN );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    err = VSPI_FLASH_SingleCMD ( VSPI_FLASH_CMD_RST );

    return err;
}

/********************** OTP ************************/

/******************************************************************************
Description:
    Read OTP.

Parameters:
    otp_block - OTP区块选择， 1~3
    addr - 每个OTP 块内的地址， 取值范围： 0~0x3FF . 在当前块内，地址达到0x3FF后，会复位成0

Return Value:
    None.

Notes:

*****************************************************************************/
VRET_VALUE VSPI_OTP_Read ( UINT8 flash_sel, UINT32 addr, UINT8 *buf, UINT32 siz )
{
	#ifdef CORE_TOP_OTP_LIB
	if (addr != (0x3000 + 75) || siz != 16)
	{
		return VRET_ERROR;
	}
	#endif
    VRET_VALUE err = VRET_NO_ERROR;
    BOOL use_dual;
    UINT8 cmd;
    UINT8 dummy_len = VSPI_FLASH_DUMMY_LEN1;

    // check parameters.
    VSPI_FLASH_IsReady();

    if ( ( buf == NULL ) || ( siz == 0 ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }

    use_dual = VSPI_FLASH_SING_MODE;

    cmd = VSPI_OTP_CMD_READ_85M_1DUMMY;

    return VSPI_FLASH_ReadData ( flash_sel, cmd, dummy_len, use_dual, addr, buf, siz );
}

VRET_VALUE VSPI_OTP_Program ( UINT8 flash_sel, UINT32 addr, const UINT8 *buf, UINT32 siz )
{
	#ifdef CORE_TOP_OTP_LIB
	return VRET_ERROR;
	#else
    VRET_VALUE err = VRET_NO_ERROR;
    UINT8 *pi;
    UINT32 tmp;

    // check parameters.
    VSPI_FLASH_IsReady();

    tmp = addr & ( VSPI_FLASH_PAGE_SIZE - 1 );
    if ( tmp && ( tmp + siz > VSPI_FLASH_PAGE_SIZE ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }

    // unprotect sectors.
    VSPI_FLASH_SetGlobalProtect ( FALSE, flash_sel );

    // multiple of pages
    pi = ( UINT8 * ) buf;
    while ( ( siz >= VSPI_FLASH_PAGE_SIZE ) && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Program_Page ( flash_sel, addr, pi, VSPI_FLASH_PAGE_SIZE, TRUE );
        siz -= VSPI_FLASH_PAGE_SIZE;
        addr += VSPI_FLASH_PAGE_SIZE;
        pi += VSPI_FLASH_PAGE_SIZE;
    }

    // data less than a pgae.
    if ( ( siz )  && ( err == VRET_NO_ERROR ) ) {
        err = VSPI_FLASH_Program_Page ( flash_sel, addr, pi, siz, TRUE );
    }

    return err;
	#endif
}

/* OTP erase */
VRET_VALUE VSPI_OTP_Erase ( UINT8 flash_sel, UINT32 addr, UINT32 siz )
{
	#ifdef CORE_TOP_OTP_LIB
	return VRET_ERROR;
	#else
    VRET_VALUE err = VRET_NO_ERROR;
    UINT32 idx;

    // check parameters.
    VSPI_FLASH_IsReady();

    if ( ( ( addr & ( VSPI_FLASH_OTP_SECTOR_SIZE - 1 ) ) != 0 ) || ( ( siz & ( VSPI_FLASH_OTP_SECTOR_SIZE - 1 ) ) != 0 ) ) {
        return VRET_DRV_SPI_FLASH_ERR_INVALID_PARAM;
    }

    idx = VSPI_FLASH_SEL_INDEX();

    // unprotect sectors.
    VSPI_FLASH_SetGlobalProtect ( FALSE, flash_sel );

    siz = ( siz > VSPI_FLASH_OTP_SECTOR_SIZE ) ? VSPI_FLASH_OTP_SECTOR_SIZE : siz;

    // erase multiple of 1KB blocks.
    while ( ( siz >= VSPI_FLASH_OTP_SECTOR_SIZE ) && ( err == VRET_NO_ERROR ) /*&& (addr <= (VSPI_FLASH_OTP3_ADDR_BASE+VSPI_FLASH_OTP_SECTOR_SIZE))*/ ) {
        VSPI_FLASH_SetWriteEnable ( TRUE, flash_sel );

        VSPI_FLASH_IsLocked();

        VSPI_ProgCycle_Tprog ( 0xFFFF );
        VSPI_ConfigMaster ( VSPI_FLASH_WRITE_MODE, VSPI_FLASH_NO_DMA, VSPI_FLASH_SING_MODE, g_VSPI_FLASH_Cfg.flash[idx].spi_mode );

        VSPI_ChipSelect ( flash_sel );
        VSPI_WriteCmdAddr ( VSPI_OTP_CMD_ERASE_SECTOR_1K, addr );
        VSPI_WriteLength ( VSPI_FLASH_ADDR_LEN3, VSPI_FLASH_DUMMY_LEN0, 0 );

        VSPI_StartTransfer();

        err = VSPI_FLASH_wait_wip_status ( flash_sel, VSPI_FLASH_ERASE_TIM_BLOCK4K );

        VSPI_ChipSelect ( VSPI_FLASH_NONE );
        VSPI_FLASH_Unlock();

        siz -= VSPI_FLASH_OTP_SECTOR_SIZE;
        addr += VSPI_FLASH_SECTOR_SIZE;
    }

    return err;
	#endif
}

/*
    init e/iflash to ready.
*/
VRET_VALUE VSPI_FLASH_Init_Mode ( BOOL quad_or_not, UINT8 flash_sel )
{
    VRET_VALUE err;
    UINT8 idx;

    VSPI_FLASH_IsReadyEx();

    idx = VSPI_FLASH_SEL_INDEX();

    if ( flash_sel == VSPI_FLASH_EXTERNAL ) {
        //VSPI_FLASH_RstCrm ( flash_sel );
    }
    
    err = VSPI_FLASH_Init_Ex ( flash_sel );
    if ( err != VRET_NO_ERROR ) {
        return err;
    }

    /* set external flash = quad mode */
    err = VSPI_FLASH_SetQuad ( quad_or_not, flash_sel );
    if ( err == VRET_NO_ERROR ) {
        g_VSPI_FLASH_Cfg.flash[idx].dual_mode = VSPI_FLASH_QUAD_MODE;
    } else {
        g_VSPI_FLASH_Cfg.flash[idx].dual_mode = VSPI_FLASH_SING_MODE;
    }

    return err;
}

/* flash init. */
VRET_VALUE VSPI_FLASH_Init ( void )
{
    VRET_VALUE err = VRET_NO_ERROR;
    if (g_VSPI_FLASH_Info.init) 
        return err;
    //Initialize SPI Flash and DMA module.
//    VDMA_MaskAllInt();
//    VDMA_EnableInt();

    /*Spi enable*/
    VSPI_SetEnable ( TRUE );

    //SPI config
#ifdef BOOT_LOADER
#ifdef FPGA_QA_BUILD
	//fpga
    VSPI_ClkSelect ( 2 );   
#else
    VSPI_ClkSelect ( 2 );// 26M clock
#endif
	VSPI_FLASH_RstCrm ( VSPI_FLASH_INPACKAGE );
#endif
	
	#if (VDEV_EXT_FLASH_SUPPORTED>0)
    //err = VSPI_FLASH_Init_Ex ( VSPI_FLASH_EXTERNAL );
	#endif
    err = VSPI_FLASH_Init_Ex ( VSPI_FLASH_INPACKAGE );
    

    VSPI_Cache_Flash_SetQuad ( FLASH_QUAD_ENABLE ,VSPI_FLASH_INPACKAGE);

    g_VSPI_FLASH_Info.init = 1;
    return err;
}

/******************************************************************************
Description:
     Reconfigure SPI clock according to system clock  (9648B L99 or H99)
Parameters:
	None.
Return Value:
	None.
 1.8v Flash Max frequency 38MHz
 3.3v Flash Max frequency 77MHz	

    SOCCLK(MHz)				3.3v Flash SPICLK(MHz)			1.8v Flash SPICLK(MHz)
		24							24 (1)                       24 (1)
		39(156/4)					39 (1)                       39 (1)
		48							48 (1)                       24 (2)
		64							64 (1)                       32 (2)
    	78(156/2)            		78 (1)                       39 (2)
		96                    		48 (2)                       24 (4)
      	128   						64 (2)                       32 (4)
    	156 192						64 (128/2)                   32 (128/4)     default
//		156 192						96 (96/1)                    48 (96/2)
                                                         
******************************************************************************/
void VSPI_ReconfigureSpiClk (UINT32 Sys_clock_freq)
{
	// g_VSPI_FLASH_Info.flash[1].device_voltage = FLASH_1_8v_VOL;
	if(g_VSPI_FLASH_Info.flash[1].device_voltage == FLASH_1_8v_VOL){ //Flash used 1.8v , Max frequency 40MHz
		//	printf("---------------FLASH_1_8v_VOL--------\r\n");
		  if(Sys_clock_freq <= 40000000){
			 VCKM_SPIClockSelect( 0, 0);  //Restore default
			 VSPI_ClkSelect(1);		
		 }else if(Sys_clock_freq <= 80000000){
			 VCKM_SPIClockSelect( 0, 0);  //Restore default
			 VSPI_ClkSelect(2);		
		 }else if(Sys_clock_freq <= 128000000){
			 VCKM_SPIClockSelect( 0, 0);  //Restore default
			 VSPI_ClkSelect(4);		
		 }else{
		   // the SPI to be divided into Fixed frequency or configured with SPI_CLKSEL;
			 VCKM_SPIClockSelect( 1, 1);
			 VSPI_ClkSelect(4);          //SPI Overclock test cxt     //spi clk 32MHz
			 
		 }
	}else {        //Flash used 3.3v , Max frequency 80MHz
	//	 printf("---------------FLASH_3_3v_VOL--------\r\n");
		 if(Sys_clock_freq <= 80000000){
			 VCKM_SPIClockSelect( 0, 0);  //Restore default
			 VSPI_ClkSelect(1);
		 }else if(Sys_clock_freq <= 128000000){
			 VCKM_SPIClockSelect( 0, 0);  //Restore default
			 VSPI_ClkSelect(2);
		 }else{
		   // the SPI to be divided into Fixed frequency or configured with SPI_CLKSEL;
			 VCKM_SPIClockSelect( 1, 1);
			 VSPI_ClkSelect(2);          //SPI Overclock test    //spi clk 64MHz
			 
		 }
	}
}

FLASH_VOL VSPI_GetFLASH_voltage(UINT8 flash_sel)
{
  UINT32 idx;
	idx = ( flash_sel == VSPI_FLASH_EXTERNAL ) ? 0 : 1;
	
	return (g_VSPI_FLASH_Info.flash[idx].device_voltage);
}

