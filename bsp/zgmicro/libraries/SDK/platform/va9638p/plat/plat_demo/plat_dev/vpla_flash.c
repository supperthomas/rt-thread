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
        FLASH

Description:
        FLASH Device
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdcore.h"
#include "vpla_flash.h"
#include "vdrv_ckm.h"
#include "vdrv_spi.h"
#include "vdrv_spiflash.h"
#include "vdebug_errno.h"

VRET_VALUE VSYS_FLASH_Init(void)
{
	VRET_VALUE ret;
    ret = VSPI_FLASH_Init();
	if(ret == VRET_NO_ERROR)
		VSPI_ReconfigureSpiClk(VCKM_GetSysClockFreq());
	return ret;
}

VRET_VALUE VSYS_FLASH_Uninit(void)
{
    g_VSPI_FLASH_Info.init = 0;
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_FLASH_Erase(VDRVFLASHSel_t flash_sel, UINT32 addr, size_t siz)
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 isotp = FALSE;
    UINT8 flash = VSPI_FLASH_NONE;

    switch (flash_sel)
    {
        case VSYS_FLASH_INPACKAGE_OTP:
            if (VCORE_GetID() == CPU_ID_TOP)
            {
                __VERRNO_STORE(VERRNO_FLASH_LIMITS);
                return ret;
            }
            isotp = TRUE;
        case VSYS_FLASH_INPACKAGE:
            flash = VSPI_FLASH_INPACKAGE;
            break;
        case VSYS_FLASH_EXTERNAL_OTP:
            isotp = TRUE;
        case VSYS_FLASH_EXTERNAL:
            flash = VSPI_FLASH_EXTERNAL;
            break;
        default:
            __VERRNO_STORE(VERRNO_FLASH_ERASE);
            return ret;
    }
    if(isotp == FALSE){
        if(siz == VSYS_FLASH_ALL_SIZE){
            ret = VSPI_FLASH_EraseChip(flash);
        }else{
            ret = VSPI_FLASH_Erase(flash,addr,siz);
        }
    }else{
        ret = VSPI_OTP_Erase(flash,addr,siz);
    }
    return ret;
}

VRET_VALUE VSYS_FLASH_Program (VDRVFLASHSel_t flash_sel, UINT32 addr, const UINT8 *buf, size_t siz )
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 isotp = FALSE;
    UINT8 flash = VSPI_FLASH_NONE;

    switch (flash_sel)
    {
        case VSYS_FLASH_INPACKAGE_OTP:
            if (VCORE_GetID() == CPU_ID_TOP)
            {
                __VERRNO_STORE(VERRNO_FLASH_LIMITS);
                return ret;
            }
            isotp = TRUE;
        case VSYS_FLASH_INPACKAGE:
            flash = VSPI_FLASH_INPACKAGE;
            break;
        case VSYS_FLASH_EXTERNAL_OTP:
            isotp = TRUE;
        case VSYS_FLASH_EXTERNAL:
            flash = VSPI_FLASH_EXTERNAL;
            break;
        default:
            __VERRNO_STORE(VERRNO_FLASH_WR);
            return ret;
    }
    if(isotp == FALSE){
        ret = VSPI_FLASH_Program(flash,addr,buf,siz);
    }else{
        ret = VSPI_OTP_Program(flash,addr,buf,siz);
    }
    return ret;
}

VRET_VALUE VSYS_FLASH_Read (VDRVFLASHSel_t flash_sel, UINT32 addr, UINT8 *buf, size_t siz )
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 isotp = FALSE;
    UINT8 flash = VSPI_FLASH_NONE;

    switch (flash_sel)
    {
        case VSYS_FLASH_INPACKAGE_OTP:
            if (VCORE_GetID() == CPU_ID_TOP)
            {
                __VERRNO_STORE(VERRNO_FLASH_LIMITS);
                return ret;
            }
            isotp = TRUE;
        case VSYS_FLASH_INPACKAGE:
            flash = VSPI_FLASH_INPACKAGE;
            break;
        case VSYS_FLASH_EXTERNAL_OTP:
            isotp = TRUE;
        case VSYS_FLASH_EXTERNAL:
            flash = VSPI_FLASH_EXTERNAL;
            break;
        default:
            __VERRNO_STORE(VERRNO_FLASH_RD);
            return ret;
    }
    if(isotp == FALSE){
        ret = VSPI_FLASH_Read(flash,addr,buf,siz);
    }else{
        ret = VSPI_OTP_Read(flash,addr,buf,siz);
    }
    return ret;
}

VRET_VALUE VSYS_FLASH_GetParam (VDRVFLASHSel_t flash_sel, VDRVFLASHInfo_t info, UINT8 *buff, size_t size)
{
    UINT8 flash = VSPI_FLASH_NONE;
    VRET_VALUE ret = VRET_ERROR;
    void *buff_p = NULL;
    UINT8 __align(4) tmp[VSPI_FLASH_UNIQUEID_LEN>16?VSPI_FLASH_UNIQUEID_LEN:16]  = {0};
    size_t len = 0;
    
    if(buff == NULL || size < 1){
        __VERRNO_STORE(VERRNO_PARA_INVALID);
        return ret;
    }
    switch (flash_sel)
    {
        case VSYS_FLASH_INPACKAGE:
        case VSYS_FLASH_INPACKAGE_OTP:
            flash = VSPI_FLASH_INPACKAGE;
            break;
        case VSYS_FLASH_EXTERNAL:
        case VSYS_FLASH_EXTERNAL_OTP:
            flash = VSPI_FLASH_EXTERNAL;
            break;
        default:
            __VERRNO_STORE(VERRNO_FLASH_INVALID);
            return ret;
    }
    switch (info)
    {
        case VSYS_FLASH_MID:{
            if ( g_VSPI_FLASH_Info.ready & flash )
            {
                buff_p = &g_VSPI_FLASH_Info.flash[((flash == VSPI_FLASH_EXTERNAL)?0:1)].mid;
                len = 3;
                ret = VRET_NO_ERROR;
            }else
            {
                buff_p = tmp;
                len = 3;
                ret = VSPI_FLASH_ReadMID(buff_p,flash);
            }
        }   break;
        case VSYS_FLASH_UID:{
            if (flash_sel == VSYS_FLASH_INPACKAGE_OTP)
            {
                buff_p = tmp;
                len = 16;
                ret = VSPI_OTP_Read(VSPI_FLASH_INPACKAGE,0x3000 + 75,buff_p, len);
            }
            else if ( g_VSPI_FLASH_Info.ready & flash )
            {
                buff_p = &g_VSPI_FLASH_Info.flash[((flash == VSPI_FLASH_EXTERNAL)?0:1)].Uid;
                len = VSPI_FLASH_UNIQUEID_LEN;
                ret = VRET_NO_ERROR;
            }else
            {
                buff_p = tmp;
                len = VSPI_FLASH_UNIQUEID_LEN;
                ret = VSPI_FLASH_ReadUniqueID(buff_p,flash);
            }  
        }   break;
        case VSYS_FLASH_TOTALSIZE:
            buff_p = &g_VSPI_FLASH_Info.flash[((flash == VSPI_FLASH_EXTERNAL)?0:1)].size;
            len = 4;
            ret = VRET_NO_ERROR;
            break;
        case VSYS_FLASH_VOLTAGE:
            buff_p = &g_VSPI_FLASH_Info.flash[((flash == VSPI_FLASH_EXTERNAL)?0:1)].device_voltage;
            len = 1;
            ret = VRET_NO_ERROR;
            break;
        case VSYS_FLASH_TOPBASE:
            buff_p = tmp;
            len = 4;
            *(UINT32*)buff_p = VSPI_Get_TOPBase();
            ret = VRET_NO_ERROR;
            break;
        case VSYS_FLASH_BTBASE:
            buff_p = tmp;
            len = 4;
            *(UINT32*)buff_p = VSPI_Get_BTBase();
            ret = VRET_NO_ERROR;
            break;
        case VSYS_FLASH_DSPBASE:
            buff_p = tmp;
            len = 4;
            *(UINT32*)buff_p = VSPI_Get_DSPBase();
            ret = VRET_NO_ERROR;
            break;
        default:
            __VERRNO_STORE(VERRNO_FLASH_INVALID);
            return ret;
    }
    if(buff_p != NULL){
        vmemcpy(buff,buff_p,MIN(size,len));
    }
    return ret;
}

VRET_VALUE VSYS_FLASH_SetParam(VDRVFLASHSel_t flash_sel, VDRVFLASHInfo_t info, UINT8 *buff, size_t size)
{
    UINT8 flash = VSPI_FLASH_NONE, cont = 0;
    VRET_VALUE ret = VRET_ERROR;

    if(buff == NULL || size < 1){
        __VERRNO_STORE(VERRNO_PARA_INVALID);
        return ret;
    }
    switch (flash_sel)
    {
        case VSYS_FLASH_INPACKAGE:
        case VSYS_FLASH_INPACKAGE_OTP:
            flash = VSPI_FLASH_INPACKAGE;
            break;
        case VSYS_FLASH_EXTERNAL:
        case VSYS_FLASH_EXTERNAL_OTP:
            flash = VSPI_FLASH_EXTERNAL;
            break;
        case VSYS_FLASH_BOTH:
            flash = VSPI_FLASH_EXTERNAL;
            cont = 1;
        default:
            __VERRNO_STORE(VERRNO_FLASH_INVALID);
            return ret;
    }
    do{
        switch (info)
        {
            case VSYS_FLASH_DUALMODE:
                if(size >= sizeof(UINT8)){
                    if ( g_VSPI_FLASH_Info.ready & flash )
                    {
                        g_VSPI_FLASH_Cfg.flash[((flash == VSPI_FLASH_EXTERNAL)?0:1)].dual_mode = buff[0];
                    }else
                    {
                        VSPI_FLASH_Init_Mode ( buff[0], VSPI_FLASH_EXTERNAL );
                    }
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_TOPBASE:
                if(size >= sizeof(UINT32)){
                    VSPI_Set_TOPBase(*(UINT32*)buff);
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_BTBASE:
                if(size >= sizeof(UINT32)){
                    VSPI_Set_BTBase(*(UINT32*)buff);
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_DSPBASE:
                if(size >= sizeof(UINT32)){
                    VSPI_Set_DSPBase(*(UINT32*)buff);
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_ENCRYPTKEY:
                if(size >= sizeof(UINT32)){
                    VSPI_Set_EncryptKey ( *(UINT16*)buff, *(UINT16*)(buff+2) );
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_ENCRYPTKEY2:
                if(size >= sizeof(UINT32)){
                    VSPI_Set_EncryptKey2 ( *(UINT16*)buff, *(UINT16*)(buff+2) );
                    ret = VRET_NO_ERROR;
                }
                break;
            case VSYS_FLASH_CLKDIV:
                if(size >= sizeof(UINT8)){
                    VSPI_ClkSelect (buff[0]);
                    ret = VRET_NO_ERROR;
                }
                break;
            default:
                __VERRNO_STORE(VERRNO_FLASH_INVALID);
                return ret;
        }
        flash++;
    }while(cont--);
    
    return ret;
}

static void flash_load(UINT32 src, UINT32 des, UINT32 len)
{
#define LOAD_READ_REG(reg)                   (*((volatile unsigned long *)(reg)))
#define LOAD_WRITE_REG(reg, val)             (LOAD_READ_REG(reg) = (val))

	if(LOAD_READ_REG(0x40030040)&0x01){
		
	}else{
		//while(1);
	}
    LOAD_WRITE_REG ( 0x40030000, (LOAD_READ_REG ( 0x40030000 ) & ( ~0x0E ) ) | 0x04);
    LOAD_WRITE_REG ( 0x40030004, (LOAD_READ_REG ( 0x40030004 ) & ( ~0x70003 ) ) | 0x30FF0);
	LOAD_WRITE_REG ( 0x4003000C, 0xEB | ( src << 8 ) );
    LOAD_WRITE_REG ( 0x40030010, 0x1000033|(len<< 8));
	
	#ifdef CORE_SEL_MCUTOP
		LOAD_WRITE_REG(0x4002F004, ((LOAD_READ_REG(0x4002F004) & (~0x00002000)) | 0x0000C000));
		LOAD_WRITE_REG(0x4002F008, (LOAD_READ_REG(0x4002F008) | 0x0000E000));
	#else
		LOAD_WRITE_REG(0x4002F804, ((LOAD_READ_REG(0x4002F804) & (~0x00002000)) | 0x0000C000));
		LOAD_WRITE_REG(0x4002F808, (LOAD_READ_REG(0x4002F808) | 0x0000E000));
	#endif

	LOAD_WRITE_REG(0x4002D0C4, (LOAD_READ_REG(0x4002D0C4) & (~0x00000004))|0x00000002);
    LOAD_WRITE_REG(0x4002D0AC, (LOAD_READ_REG(0x4002D0AC) & (~0x00000004))|0x00000002);
	LOAD_WRITE_REG(0x4002D0AC, (LOAD_READ_REG(0x4002D0AC) & (~0x00000008))|0x00000004);
	
    LOAD_WRITE_REG(0x4002D0B4, des);
	LOAD_WRITE_REG(0x4002D0B8, len);	
	LOAD_WRITE_REG(0x4002D0AC, (LOAD_READ_REG(0x4002D0AC) | 0x00000001));
	
	LOAD_WRITE_REG ( 0x40030008, 1 );

    while ((LOAD_READ_REG(0x40030024)&0x101) != 1);

    while(!(LOAD_READ_REG(0x4002F014)&0x2000));
	LOAD_WRITE_REG(0x4002F00C, 0x2000);	
}
void coderam_load(void)
{
	UINT32 offset = *(UINT32*)0x20;
	if (offset && !(offset&0x3))
	{
		UINT32 sector[128];
		UINT32 flash_src = (LOAD_READ_REG(0x40030080) + offset);
		UINT32 *cache_src = (UINT32*)offset;
		UINT32 *des = (UINT32*)0x400000;
		UINT32 len = 0x10000, len_t = 0;
		if (offset < 0x100000)
		{
			len_t = 0x100000 - offset;
			len_t = len_t < len?len_t:len;
			for(UINT32 i = 0;i < len_t/4;i++)
			{
				des[i] = cache_src[i];
			}
			des += len_t/4;
			flash_src += len_t;
			len -= len_t;
		}
		while (len > 0)
		{
			len_t = sizeof(sector) < len?sizeof(sector):len;
			flash_load(flash_src,(UINT32)&sector,len_t);
			for(UINT32 i = 0;i < len_t/4;i++)
			{
				des[i] = sector[i];
			}
			des += len_t/4;
			flash_src += len_t;
			len -= len_t;
		}
	}
}

