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
    9900

Description:
    Internale 9900 module driver.

Revision History:
    Date                  Author                   Description
   2020.11.30                           caoxuetao               initial verison
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_i2c.h"
#include "vdrv_9900.h"


#define VA9900_I2C_SLAVE_ADDR       0x25

#define DRV_9900_DBG_EN								0

#if (DRV_9900_DBG_EN > 0)
UINT8 g_9900Reg_Write_Value[128] = {0};
UINT8 g_9900Reg_Read_Value[128] = {0};
#endif

UINT8 g_is_9900_exist;

#if (VDEV_9900_SUPPORTED > 0)

HW_RADIO_9900_REG_INIT g_HW_RADIO_9900_Reg_Init = {
    {0xFF, 0xFF},
    {
        0x70, 0x07, 0x01, 0x33, 0x07,  /*0-4*/
        0x07, 0x00, 0x00, 0x00, 0x00,  /*5-9*/
        0x00, 0x00, 0xF1, 0xF3
    },
    1
};

extern VRET_VALUE VI2C_register_read ( UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value );
extern VRET_VALUE VI2C_register_write ( UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value );

/* 
**  reset 9900 iic,by BYPASS signal 
**  becareful all the  9900 registers will recover,must use with DRV_9900_Init,cannot use single
*/
void DRV_9900_RST(void)
{
    SET_REG_BIT(REG_BB_KXLNAPA_EN_RGF_KXBPS_EN);
    VCOMMON_Delay(50);
    CLR_REG_BIT(REG_BB_KXLNAPA_EN_RGF_KXBPS_EN);
}
/* enable va9900(GPIO_L_13),set gpio and rst (GPIO_L_12)*/
void Enable_VA9900 ( void )
{
    /*Enable*/
    //VGPIO_SetLMode(GPIO_L_PIN_13 | GPIO_L_PIN_12, GPIO_MODE_OUTPUT, GPIO_OUTPUT_HIGH_STRENGTH);

    //VGPIO_SetLPinOutput(13,1);

    /*rst*/
    //VGPIO_SetLPinOutput(12,1);

    WRITE_REG ( REG_I2C_I2C_EN_9900, 0x01 );

    __NOP();
    __NOP();
    __NOP();
}

/* disable va9900(GPIO_L_13),set gpio and rst (GPIO_L_12)*/
void Disable_VA9900 ( void )
{
    /*Enable*/
    //VGPIO_SetLMode(GPIO_L_PIN_13 | GPIO_L_PIN_12, GPIO_MODE_OUTPUT, GPIO_OUTPUT_HIGH_STRENGTH);

    //VGPIO_SetLPinOutput(13,0);
    /*rst ,set 0,but after rst the  registers will be reset*/
    //VGPIO_SetLPinOutput(12,1);

    WRITE_REG ( REG_I2C_I2C_EN_9900, 0x00 );

    __NOP();
    __NOP();
    __NOP();
}


VRET_VALUE DRV_9900_WriteReg ( UINT8 Reg, UINT8 Value )
{
    VRET_VALUE ret;

    Enable_VA9900();

    ret = VI2C_register_write ( I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR, Reg, Value );

    Disable_VA9900();

    if ( ret ) {
        VCDC_FPRINTF ( "%s: CDC config failed!\r\n", FUNCTION_NAME );
    } else {
#if (DRV_9900_DBG_EN > 0)
        if ( Reg < 128 ) {
            g_9900Reg_Write_Value[Reg] = Value;
        }
#endif
    }

    return ret;
}


VRET_VALUE DRV_9900_ReadReg ( UINT8 Reg, UINT8 *p_Value )
{
    VRET_VALUE ret;

    Enable_VA9900();

    ret = VI2C_register_read ( I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR, Reg, p_Value );

    Disable_VA9900();

    if ( ret ) {
        VCDC_FPRINTF ( "%s: CDC reg read failed!\r\n", FUNCTION_NAME );
    }
#if (DRV_9900_DBG_EN > 0)
        if ( Reg < 128 ) {
            g_9900Reg_Read_Value[Reg] = *p_Value;
        }
#endif

    return ret;
}

VRET_VALUE DRV_9900_Init (UINT8 isClose9900)
{
    UINT8 i, j;
    VRET_VALUE ret = VRET_NO_ERROR;

#if 1

    /* reset 9900 iic first,will recover all the 9900 registers default value */
    DRV_9900_RST();

    Enable_VA9900();

    for ( i = 0; i < HW_RADIO_9900_REG_TOTAL_NUM; i++ ) {
        
        j = i % 8;
        if ( g_HW_RADIO_9900_Reg_Init.regs_valid[i / 8] & ( 0x1 << j ) )  {

            ret = VI2C_register_write ( I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR,
                                        VA9900_BASE + i, g_HW_RADIO_9900_Reg_Init.regs_init[i] );
            if ( ret ) {
                VCDC_FPRINTF ( "%s: CDC config failed!\r\n", FUNCTION_NAME );
                break;
            }
        }
    }

    Disable_VA9900();

    /* if INIT_9900_CLOSE_TX_RX,close 9900 RX and TX,else maybe decided by g_HW_RADIO_9900_Reg_Init parameter */
	if(isClose9900 == INIT_9900_CLOSE_TX_RX)
	{
	    DRV_9900_WriteReg(REG_FEM_INT_CTR_EN,0x01);
	    DRV_9900_WriteReg(REG_FEM_RXON,0x00);
	    DRV_9900_WriteReg(REG_FEM_TXON,0x00);
	}
    
#else
    
    DRV_9900_WriteReg(0x00, 0x70);
    DRV_9900_WriteReg(0x01, 0x07);
    DRV_9900_WriteReg(0x02, 0x01);
    DRV_9900_WriteReg(0x03, 0x33);
    DRV_9900_WriteReg(0x05, 0x07);
    
#endif
    
    return ret;
}

/******************************************************************************
Description:
    judge whether if 9900 version chip.

Parameters:
    void

Return Value:
    UINT8:
    1 : 9900 version chip
    0 : not 9900 version chip

Notes:
    set g_is_9900_exist = 1 if 9900 exist,used in controller.
*****************************************************************************/
UINT8 Is_VA9900_VersionChip ( void )
{
    // UINT8 ret;
    UINT8 data;

    if ( DRV_9900_ReadReg ( 0x00, &data ) ) {  //VA9900_ReadReg ( 0x00, &data ) = VRET_NO_ERROR stands for support for 9900
        g_is_9900_exist = 0x00;
        return 0x00;
    } else {
         g_is_9900_exist = 0x01;
        return 0x01;
    }
}

#else

VRET_VALUE DRV_9900_WriteReg ( UINT8 Reg, UINT8 Value )
{
    return VRET_ERROR;
}


VRET_VALUE DRV_9900_ReadReg ( UINT8 Reg, UINT8 *p_Value )
{
    return VRET_ERROR;
}


#endif // #if (VDEV_9900_SUPPORTED > 0)


