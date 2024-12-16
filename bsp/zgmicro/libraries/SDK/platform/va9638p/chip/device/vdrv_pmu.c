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
    PMU

Description:
    PMU(Power Management Unit) driver.

Revision History:
    Date                  Author                   Description
    2010.05.08           chenjingen              initial version
    2013.11.16           Xiongyan              update for 9611SE0, add MTRST_LOCK, VER_SEL.
    2013.11.19           Xiongyan              update about VPMU_GetVerInfo, get more 9611 info(VER_SEL_LowHigh and VER_SEL_OldNew).
    2014.02.10           Xiongyan              update for 7621A, add MCP register.
    2014.02.18      xiongyan            add 7621SA0 and 7621GA0
    2019.06.05      xiongyan            add VA7632B_A0
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vcommon.h"
#include "vdev_cfg.h"
#include "vstdos.h"
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_gpio.h"
#include "vdrv_ckm.h"
#include "vpla_i2c.h"
#include "vdrv_pmu.h"
#include "vdrv_cdc.h"

#if defined (HW_CDC_DEBUG)
//    #include "vcdc.h"
#endif
#include "vsys_power.h"
// #include "vcodec_hw.h"

#include "vsys_config.h"
#include "vnvic_interrupt.h"

#ifdef CORE_SEL_MCUTOP
#include "top_debug.h"
#include "tapp_top_mode.h"
#define OPERATION_I2C_SUPPORTED (!BT_I2C_SUPPORTED)
#else
#define OPERATION_I2C_SUPPORTED BT_I2C_SUPPORTED
#endif

volatile UINT8 g_VPMU_PowerState;
static BOOL g_VPMU_I2CLowSpeedFlag = TRUE;
UINT8 g_ISR_PMUState = 0;

#if defined (HW_CDC_DEBUG)
    extern VCDC_DEBUG g_VCDC_Debug;
#endif
extern VSYS_PWR_CFG g_VSYS_PWR_Config;

extern BOOL g_charge_bypass_enable;

#if	(OPERATION_I2C_SUPPORTED )

void VPMU_SetI2CLowSpeedFlag ( BOOL Flag )
{
    g_VPMU_I2CLowSpeedFlag = Flag;
}

void VPMU_Init ( void )
{
	VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
	VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);
    g_VPMU_PowerState = PMU_POWER_OFF;
}

UINT8 VPMU_GetPowerState ( void )
{
    return g_VPMU_PowerState;
}

UINT8 VPMU_GetPowerOnReq ( void )
{
    /* read GPIO_PIN_10 */
#if (VDEV_TYPE == TYPE_FPGA)
    /* on FPGA, button pressed level is low */
    return ( !VGPIO_GetPinInput ( GPIO_PWRON_SHFT ) );
#elif (VDEV_TYPE == TYPE_ASIC)
    return VGPIO_GetPinInput ( GPIO_PWRON_SHFT );
#endif
}

void VPMU_SetPMUEn ( BOOL IsEnable )
{
    if ( IsEnable ) {
        g_VPMU_PowerState = PMU_POWER_ON;
    } else {
        g_VPMU_PowerState = PMU_POWER_OFF;
    }

    /* write GPIO_PIN_10 */
#if (VDEV_TYPE == TYPE_FPGA)
    /* on FPGA, use LED6 to simulate PMU_EN, turn on if PMU_EN enabled else turn off */
    VGPIO_SetPinOutput ( GPIO_PWRON_SHFT, IsEnable );
#elif (VDEV_TYPE == TYPE_ASIC)
    VGPIO_SetPinOutput ( GPIO_PWRON_SHFT, IsEnable );
#endif
}

VRET_VALUE VPMU_GetChargerState ( UINT8 *p_State )
{
    VRET_VALUE ret = VRET_ERROR;

    ASSERT ( p_State != NULL );
    if ( p_State == NULL ) {
        return ret;
    }

    *p_State = 0;
    /* read I2C register */
    //if(g_VSYS_PWR_Config.cdc_ver > CDC_VERSION_7621_A) {//7621S, 7621G
    if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) {
        if ( VCDC_GetVerNo() == CDC_VERSION_7621_A ) { //9611, 7621A
            ret = VPMU_ReadReg ( PMU_REG_CHARGE_STATUS, p_State );
        } else {
            ret = VPMU_ReadReg ( PMU_REG_INT_STATUS, p_State );
            *p_State = ( *p_State ) >> 2;
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B + 7652 + 7638 
        ret = VPMU_ReadReg ( CDC_REG763X_INT_STATUS, p_State );
        *p_State = ( *p_State ) >> 2;
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }

    return ret;
}

void VPMU_SetStateISR ( UINT8 State )
{
    g_ISR_PMUState = State;
}
UINT8 VPMU_GetStateISR ( void )
{
    return g_ISR_PMUState;
}

VRET_VALUE VPMU_GetInvState(UINT8 *p_State)
{
	VRET_VALUE ret = VRET_ERROR;

	ASSERT(p_State != NULL);
	
	ret = VPMU_ReadReg(PMU_REG_INT_INV, p_State);
	return ret;
}

void VPMU_SetInvState(UINT8 state)
{
	VPMU_WriteReg(PMU_REG_INT_INV, state);
}


//before 7632B is Mbv[1~4], then 7632B had Mbv[1~4]==bit[0~3] & MVOL2==bit[4~6]
void VPMU_SetBatteryLowLevel ( UINT8 MBV )
{
    ASSERT ( VPMU_IsValidBattLowVol ( MBV & 0x0F ) );

    /* write I2C register */
    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
        VPMU_WriteReg ( PMU_REG_MBV, MBV );
    } else {// >=CDC_VERSION_7632B_A0
        VPMU_WriteReg ( PMU_REG763X_MBV, MBV );
    }
}
/******************************************************************************
Description:
    VPMU Set bypass register

Parameters:
    enable - the bypass switch, TRUE means charger bypass mode will be enabled,
    FALSE means charger bypass mode will be disabled

Return Value:
    None.
*****************************************************************************/

void VPMU_Set_Bypass (UINT8 Enable)
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 Tmp;

#if VDBG_EN
	//20210120xy, check the SV board may power support err cause system death;
	if(Enable == 0){
		VPMU_WPRINTF("VPMU_Set_Bypass disabled charger bypass and used battery!.\r\n");
		#ifndef DRIVER_TEST
		// OSTimeDly(1,OS_OPT_TIME_DLY,&err);
		//OSTimeDly(1);//delay 10ms;
		#endif
		// VCOMMON_Delay ( 10000 ); //10ms
	}
#endif

    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
        ret = VPMU_ReadReg ( PMU_REG_MEN5, &Tmp );
    } else {// >=CDC_VERSION_7632B_A0
        ret = VPMU_ReadReg ( PMU_REG7632X_MEN5, &Tmp );
    }
    if ( ret == VRET_NO_ERROR ) {
        if ( Enable ) {
            Tmp = Tmp & 0xFB; //Men5[2]="0", charger bypass mode will be enabled; used charge support power;
            g_VSYS_PWR_Config.pmu_ldo5 = ( g_VSYS_PWR_Config.pmu_ldo5 & 0xFB );
        } else {
            Tmp = Tmp | 0x04; //Men5[2]="1", charger bypass mode will be disabled; used battery close charge;
            g_VSYS_PWR_Config.pmu_ldo5 = ( g_VSYS_PWR_Config.pmu_ldo5 | 0x04 );
        }

        if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
            VPMU_WriteReg ( PMU_REG_MEN5, Tmp );
        } else {// >=CDC_VERSION_7632B_A0
            VPMU_WriteReg ( PMU_REG7632X_MEN5, Tmp );
        }
    } else {
        //read PMU_REG_MEN5 or  PMU_REG7632X_MEN5 error
    }

}

//set RF DC-DC output voltage;
void VPMU_SetDCOutputVoltage ( UINT8 Voltage )
{
    // ASSERT ( VPMU_IsValidDCOutVol ( Voltage ) );
    /* write I2C register */
    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
        VPMU_WriteReg ( PMU_REG_MVDC, Voltage );
	#if 1//20201204xy, for liqin&lvjie use at 9638p asic chip;
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 ) { //for VA7632BF5, not change the MVDCA&MVDC value still default at the power on;    
        VPMU_WriteReg ( PMU_REG763X_MVDC, Voltage );
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_E3 ) { //for VA7632BF3, should change the  MVDCA&MVDC value(default err)at the power on;  
		#ifndef FPGA_QA_BUILD	//20201209xy, if FPGA build then not need change the DC-DC output; if do change at 9625p&7632BF3 addition board normal but 9638c&7632BF3 affected about connect fail;
		VPMU_WriteReg ( PMU_REG763X_MVDCA, 0x05 );//change from 1.2v to 1.5v;  
		VPMU_WriteReg ( PMU_REG763X_MVDC, 0x05 );//change from 1.5v to 1.2v; 
		#else
		VPMU_WriteReg ( PMU_REG763X_MVDCA, 0x07 );//change from 1.2v to 1.9v; //20201218xy, for FPGA use 7632BF3, for the default CDC_VREF_SEL_DG set is 0x06==1.8v;
		#endif
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC ) {	
		VPMU_WriteReg ( PMU_REG763X_MVDCA, Voltage ); //set RF DC-DC;
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7638_A0) {	
		VPMU_WriteReg ( PMU_REG763X_MVDC, 0x04 ); //set RF DC-DC;
	#else
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        VPMU_WriteReg ( PMU_REG763X_MVDC, Voltage );
	#endif
    } else {
        VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}


//xy: 20140210 update for 7621, set charge Current&TerminateVoltage by same register(PMU_REG_MCHV).
void VPMU_SetCharge ( UINT8 Voltage, UINT8 Current )
{
    ASSERT ( VPMU_IsValidChargeTerminateVol ( Voltage ) );

    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //7632B
        VPMU_WriteReg ( PMU_REG763X_MCHV, Voltage | ( Current << 2 ) );
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
        UINT8 Tmp = 0;

        VPMU_WriteReg ( PMU_REG_MCHV, Voltage | ( ( Current << 2 ) & 0x0C ) );
        /* read I2C register */
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG_D_LPO1, &Tmp ) ) {
            VPMU_WriteReg ( PMU_REG_D_LPO1, ( Tmp & 0xF3 ) | ( Current & 0x0C ) );
        }
    } else {//other 7621
        ASSERT ( VPMU_IsValidChargeCurrent ( Current ) );
        /* write I2C register */
        VPMU_WriteReg ( PMU_REG_MCHV, Voltage | ( Current << 2 ) );
    }
}

void VPMU_SetMCP ( UINT8 Mcp )
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //7632B
        UINT8 Tmp = 0;
        /* read I2C register */
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO2, &Tmp ) ) {
            VPMU_WriteReg ( PMU_REG763X_D_LPO2, ( Tmp & 0x03 ) | ( Mcp << 2 ) );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) { // for Adjust the reverse current of RCP_BUCK by inductor;
        UINT8 Tmp = 0;
        /* read I2C register */
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG_D_LPO1, &Tmp ) ) {
            VPMU_WriteReg ( PMU_REG_D_LPO1, Tmp | ( Mcp & 0x03 ) );
        }
    } else {//other 7621
        ASSERT ( VPMU_IsValidMcpOutVol ( Mcp ) );
        /* write I2C register */
        VPMU_WriteReg ( PMU_REG_MCP, Mcp );
    }
}

//xy: 20170523 update for 7621SDC0, set charging control; this feature support form 7621SC8(>=7621CS8).
extern VSYS_PWR_CFG g_VSYS_PWR_Config;
void VPMU_SetChargingDisable ( UINT8 Ctrl )
{
    UINT8 Reg_Val = 0;
    /* read I2C register */
    if ( VCDC_GetVerNo() <= CDC_VERSION_7621SD_C0 ) {
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG_D_LPO1, &Reg_Val ) ) {
            //control by D_LPO[6];
            //if ( g_VSYS_PWR_Config.forbid_charging ) {
            //    //if enable forbid charging no matter when disable charging
            //    VPMU_WriteReg ( PMU_REG_D_LPO1, Reg_Val | BIT_MASK ( 6 ) ); //set bit[6],0x40
            //    return;
            //}

            if ( Ctrl ) { //disable charging
                VPMU_WriteReg ( PMU_REG_D_LPO1, Reg_Val | BIT_MASK ( 6 ) ); //set bit[6],0x40
            } else {//enable charing
                VPMU_WriteReg ( PMU_REG_D_LPO1, Reg_Val & ( ~BIT_MASK ( 6 ) ) ); //clear bit[6], 0xBF
            }
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        if ( Ctrl ) { //disable charging
            VPMU_WriteReg ( PMU_REG763X_CHG_STOP, 0x01 );
        } else {//enable charing
            VPMU_WriteReg ( PMU_REG763X_CHG_STOP, 0x00 );
        }
    } else {
        //VPMU_FPRINTF("%s: cdc_ver = %d err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}

//xy: 20181025 update for 7621S2A2, set PMU ON/OFF by 7621, if true then arm reset also have power not case .
void VPMU_Set7621PMUEn ( UINT8 Enable )
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7621SD_C0 ) { //after 7621S2A2 add this new function; 7621SD not.
        if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
            UINT8 Reg_Val = 0;
            /* read I2C register */
            if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG_D_LPO1, &Reg_Val ) ) {
                //control by D_LPO[5] to set PMU ON/OFF
                if ( Enable ) { //set PMU ON
                    VPMU_WriteReg ( PMU_REG_D_LPO1, Reg_Val | BIT_MASK ( 5 ) ); //set bit[5],0x20
                } else {//set PMU OFF
                    VPMU_WriteReg ( PMU_REG_D_LPO1, Reg_Val & ( ~BIT_MASK ( 5 ) ) ); //clear bit[5], 0xDF
                }
            }
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
            if ( Enable ) { //set PMU ON
                VPMU_WriteReg ( PMU_REG763X_PMU_ON, 0x01 ); //
            } else {//set PMU OFF
                VPMU_WriteReg ( PMU_REG763X_PMU_ON, 0x00 );
            }
        } else {
            //VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
        }
    }
}
VRET_VALUE VPMU_Get7621PMUEn ( UINT8 *p_State )
{
    VRET_VALUE ret = VRET_DRV_ERROR;

    ASSERT ( p_State != NULL );
    if (p_State == NULL) {
        return VRET_ERROR;
    }   

    *p_State = 0;
    /* read I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7621SD_C0 ) { //after 7621S2A2 add this new function; 7621SD not.
        if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
            ret = VPMU_ReadReg ( PMU_REG_D_LPO1, p_State );
            *p_State = ( ( *p_State ) >> 5 ) & 0x01;
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
            ret = VPMU_ReadReg ( PMU_REG763X_PMU_ON, p_State );
        } else {
            //VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
        }
    }

    return ret;
}


//for 7632B add
void VPMU_Set7632PMUSel ( UINT8 Sel )
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //after 7632B add this new function;
        VPMU_WriteReg ( PMU_REG763X_PMU_SEL, Sel );
    } else {
        ;
    }
}

//mode==0, disable charger timing; mode==1, charging time=1.8h; mode==2, charging time=3.6h.
void VPMU_Set7632ChargingTime ( UINT8 Mode ) //after 7632B add this new function;
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //after 7632B add this new function;
        /* write I2C register */
        if ( Mode == 0 ) { //disable charger timing
            VPMU_WriteReg ( PMU_REG763X_MCP, 0x01 );
        } else if ( Mode == 1 ) { //charging time=1.8h
            VPMU_WriteReg ( PMU_REG763X_MCP, 0x00 );
        } else if ( Mode == 2 ) { //charging time=3.6h
            VPMU_WriteReg ( PMU_REG763X_MCP, 0x02 );
        }
    } else {
        //VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}


//mode==0, detect AIO_ADC(NTC) default; mode==1, detect VBAT; mode==2, detect VCHG.
void VPMU_Config7632AIO ( UINT8 Mode ) //after 7632B add this new function;
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //after 7621S2A2 add this new function; 7621SD not.
        UINT8 Reg_Val = 0;
        /* read I2C register */
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO2, &Reg_Val ) ) {
            if ( Mode == 0 ) { //detect AIO_ADC(NTC)
                VPMU_WriteReg ( PMU_REG763X_D_LPO2, Reg_Val & ( ~BIT_MASK ( 0 ) ) ); //clear bit[0]=0
            } else if ( Mode == 1 ) { //detect VBAT
                VPMU_WriteReg ( PMU_REG763X_D_LPO2, Reg_Val | BIT_MASK ( 0 ) ); //set bit[0]=1

                if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO1, &Reg_Val ) ) {
                    VPMU_WriteReg ( PMU_REG763X_D_LPO1, Reg_Val | BIT_MASK ( 5 ) ); //set bit[5]=1
                } else {
                    VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg LPO1 err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
            } else if ( Mode == 2 ) { //detect VCHG
                VPMU_WriteReg ( PMU_REG763X_D_LPO2, Reg_Val | BIT_MASK ( 0 ) ); //set bit[0]=1

                if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO1, &Reg_Val ) ) {
                    VPMU_WriteReg ( PMU_REG763X_D_LPO1, Reg_Val & ( ~BIT_MASK ( 5 ) ) ); //clear bit[5]=0
                } else {
                    VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg LPO1 err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
            } else {//not define mode.
                ;//
            }
        } else {
            VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg LPO2 err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) { //after 7621S2A2 add this new function; 7621SD not.
        UINT8 Reg_Val = 0;
        /* read I2C register */
        if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG_7621I2S_PO_DUM2, &Reg_Val ) ) {
            if ( Mode == 0 ) { //detect AIO_ADC(NTC)
                VPMU_WriteReg ( PMU_REG_7621I2S_PO_DUM2, Reg_Val | BIT_MASK ( 0 ) ); //set bit[0]=1
            } else if ( Mode == 1 ) { //detect VCHAR
                VPMU_WriteReg ( PMU_REG_7621I2S_PO_DUM2, Reg_Val & ( ~BIT_MASK ( 0 ) ) ); //clear bit[0]=0
            } else {//not define mode.
                ;//
            }
        } else {
            VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg PO_DUM2 err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
        }
    } else {
        //VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}

// this had delay so can not call by interrupts.
VRET_VALUE VPMU_Get7632AIO ( UINT16 *p_AIO ) //after 7632B add this new function;
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 Tmp;
    ASSERT ( p_AIO != NULL );
    if (p_AIO == NULL) {
        return VRET_ERROR;
    }

    *p_AIO = 0;
    /* read I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        ret = VPMU_ReadReg ( PMU_REG763X_MEN_AIOADC, &Tmp ); //read MEN_AIOADC

        if ( VRET_NO_ERROR == ret ) {
            if ( Tmp == 0 ) {
                UINT16 retry = 10;
                ret = VPMU_WriteReg ( PMU_REG763X_MEN_AIOADC, 0x01 ); // Turn on AIO_ADC, active high.

                //check the AIO ready signal, (100us~2ms, default is 550us)
                do {
                    //OSTimeDly(1);//delay 10ms;
                    VCOMMON_Delay ( 300 ); //300us

                    //
                    ret = VPMU_ReadReg ( PMU_REG763X_TERM_AIOADC, &Tmp );
                    if ( Tmp & 0x1 ) {
                        Tmp = 0; //ready
                    } else {
                        Tmp = 1; //not ready
                    }

                    if ( VRET_NO_ERROR != ret ) {
                        Tmp = 1;
                        break;//return ret;
                    }

                    if ( Tmp == 0 ) { //AIO is ready to be read by 9613i.
                        break;
                    }

                    retry--;
                } while ( retry );

                if ( retry == 0 ) {
                    Tmp = 1;
                    ret = VRET_DRV_PMU_TERMB_TIMEOUT;
                    VPMU_WPRINTF ( "%s:err ,The Coulomb Meter'termb has time out! \r\n", FUNCTION_NAME );
                }

                if ( Tmp == 0 ) {
                    //ret = VPMU_ReadReg(PMU_REG_VD, p_VD);//read AIOoutput data.
                    ret = VPMU_ReadReg ( PMU_REG763X_AIO_DATA2, &Tmp ); //read AIO output data, AIO_Data[9:8].
                    if ( VRET_NO_ERROR != ret ) {
                        ret = VRET_DRV_PMU_STATE_ERROR;
                        VPMU_WPRINTF ( "%s:err ,The AIO_DATA2 err[0x%X]!\r\n", FUNCTION_NAME, ret );
                    } else {
                        *p_AIO = ( UINT16 ) Tmp; //AIO_Data[9:8]
                        *p_AIO = ( *p_AIO ) << 8;
                        ret = VPMU_ReadReg ( PMU_REG763X_AIO_DATA1, &Tmp ); //read AIO output data, AIO_Data[7:0].
                        if ( VRET_NO_ERROR != ret ) {
                            ret = VRET_DRV_PMU_STATE_ERROR;
                            VPMU_WPRINTF ( "%s:err ,The AIO_DATA1 err[0x%X]!\r\n", FUNCTION_NAME, ret );
                        } else {
                            //*p_AIO += Tmp;
                            *p_AIO |= Tmp;//AIO_Data[7:0]
                        }
                    }
                }
            } else {//error, AIO_ADC had been turned on.
                ret = VRET_DRV_PMU_STATE_ERROR;
                VPMU_WPRINTF ( "%s:err ,The AIO_ADC had been on!\r\n", FUNCTION_NAME );
            }
        } else {
            VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg MEN_AIOADC err[0x%X]!\r\n", FUNCTION_NAME, VCDC_GetVerNo(), ret );
        }

        VPMU_WriteReg ( PMU_REG763X_MEN_AIOADC, 0 ); //Turn off AIO_ADC
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
        ret = VPMU_ReadReg ( PMU_REG_7621I2S_MEN_AIOADC, &Tmp ); //read MEN_AIOADC

        if ( VRET_NO_ERROR == ret ) {
            if ( Tmp == 0 ) {
                UINT16 retry = 10;
                ret = VPMU_WriteReg ( PMU_REG_7621I2S_MEN_AIOADC, 0x01 ); // Turn on AIO_ADC, active high.

                //check the AIO ready signal, (100us~2ms, default is 550us)
                do {
                    //OSTimeDly(1);//delay 10ms;
                    VCOMMON_Delay ( 300 ); //300us

                    //
                    ret = VPMU_ReadReg ( PMU_REG_7621I2S_TERM_AIOADC, &Tmp );
                    if ( Tmp & 0x1 ) {
                        Tmp = 0; //ready
                    } else {
                        Tmp = 1; //not ready
                    }

                    if ( VRET_NO_ERROR != ret ) {
                        Tmp = 1;
                        break;//return ret;
                    }

                    if ( Tmp == 0 ) { //AIO is ready to be read by 9613i.
                        break;
                    }

                    retry--;
                } while ( retry );

                if ( retry == 0 ) {
                    Tmp = 1;
                    ret = VRET_DRV_PMU_TERMB_TIMEOUT;
                    VPMU_WPRINTF ( "%s:err ,The Coulomb Meter'termb has time out! \r\n", FUNCTION_NAME );
                }

                if ( Tmp == 0 ) {
                    //ret = VPMU_ReadReg(PMU_REG_VD, p_VD);//read AIOoutput data.
                    ret = VPMU_ReadReg ( PMU_REG_7621I2S_AIO_DATA2, &Tmp ); //read AIO output data, AIO_Data[9:8].
                    if ( VRET_NO_ERROR != ret ) {
                        ret = VRET_DRV_PMU_STATE_ERROR;
                        VPMU_WPRINTF ( "%s:err ,The AIO_DATA2 err[0x%X]!\r\n", FUNCTION_NAME, ret );
                    } else {
                        *p_AIO = ( UINT16 ) Tmp; //AIO_Data[9:8]
                        *p_AIO = ( *p_AIO ) << 8;
                        ret = VPMU_ReadReg ( PMU_REG_7621I2S_AIO_DATA1, &Tmp ); //read AIO output data, AIO_Data[7:0].
                        if ( VRET_NO_ERROR != ret ) {
                            ret = VRET_DRV_PMU_STATE_ERROR;
                            VPMU_WPRINTF ( "%s:err ,The AIO_DATA1 err[0x%X]!\r\n", FUNCTION_NAME, ret );
                        } else {
                            //*p_AIO += Tmp;
                            *p_AIO |= Tmp;//AIO_Data[7:0]
                        }
                    }
                }
            } else {//error, AIO_ADC had been turned on.
                ret = VRET_DRV_PMU_STATE_ERROR;
                VPMU_WPRINTF ( "%s:err ,The AIO_ADC had been on!\r\n", FUNCTION_NAME );
            }
        } else {
            VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg MEN_AIOADC err[0x%X]!\r\n", FUNCTION_NAME, VCDC_GetVerNo(), ret );
        }

        VPMU_WriteReg ( PMU_REG_7621I2S_MEN_AIOADC, 0 ); //Turn off AIO_ADC
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }

    return ret;
}
////////lcy:add by 20220803//////////////
VRET_VALUE VCDC_PMU_GetAIO_Active(void)
{
	VRET_VALUE ret = VRET_ERROR;
	UINT8 Tmp;
	
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_ReadReg ( PMU_REG763X_MEN_AIOADC, &Tmp ); //read MEN_AIOADC
		if ( VRET_NO_ERROR == ret ) {
			if ( Tmp == 0 ) {
				 ret = VPMU_WriteReg ( PMU_REG763X_MEN_AIOADC, 0x01 ); // Turn on AIO_ADC, active high.
			}
			else{
				ret = VRET_DRV_PMU_STATE_ERROR;
        VPMU_WPRINTF ( "%s:err ,The AIO_ADC had been on!\r\n", FUNCTION_NAME );
			}
		}
		else{
			VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg MEN_AIOADC err[0x%X]!\r\n", FUNCTION_NAME, VCDC_GetVerNo(), ret );		
		}		
	}else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
		ret = VPMU_ReadReg ( PMU_REG_7621I2S_MEN_AIOADC, &Tmp ); //read MEN_AIOADC
		if ( VRET_NO_ERROR == ret ) {
			if ( Tmp == 0 ) {
				ret = VPMU_WriteReg ( PMU_REG_7621I2S_MEN_AIOADC, 0x01 ); // Turn on AIO_ADC, active high.
			}
			else{
				ret = VRET_DRV_PMU_STATE_ERROR;
        VPMU_WPRINTF ( "%s:err ,The AIO_ADC had been on!\r\n", FUNCTION_NAME );
			}
		}
		else{
			VPMU_EPRINTF ( "%s: cdc_ver = %d, ReadReg MEN_AIOADC err[0x%X]!\r\n", FUNCTION_NAME, VCDC_GetVerNo(), ret );		
		}		
	}	
	return ret;
}
VRET_VALUE VCDC_PMU_GetAIO_ReadReady(UINT8* p_ready)
{
	VRET_VALUE ret = VRET_ERROR;
	UINT8 Tmp ;
	if(p_ready == NULL)
		return 1;
	
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		 ret = VPMU_ReadReg ( PMU_REG763X_TERM_AIOADC, &Tmp );
		 if(ret != VRET_NO_ERROR){
			 *p_ready = 1; //noready
			 return ret;
		 }
		 else{
			  if ( Tmp & 0x1 ) {
					*p_ready = 0; //ready
        } else {
					*p_ready = 1; //not ready
				}				 
		 }			 
	}else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
		 ret = VPMU_ReadReg ( PMU_REG_7621I2S_MEN_AIOADC, &Tmp );
		 if(ret != VRET_NO_ERROR){
			 *p_ready = 1; //noready
			 return ret;
		 }
		 else{
			  if ( Tmp & 0x1 ) {
					*p_ready = 0; //ready
        } else {
					*p_ready = 1; //not ready
				}				 
		 }			 
	}
	return ret;
}
VRET_VALUE VCDC_PMU_GetAIO_ReadData(UINT16* p_AIO)
{
	VRET_VALUE ret = VRET_ERROR;
	UINT8 Tmp;
	
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_ReadReg ( PMU_REG763X_AIO_DATA2, &Tmp ); //read AIO output data, AIO_Data[9:8].
		if ( VRET_NO_ERROR != ret ) {
			ret = VRET_DRV_PMU_STATE_ERROR;
			VPMU_WPRINTF ( "%s:err ,The AIO_DATA2 err[0x%X]!\r\n", FUNCTION_NAME, ret );
		} else {
			*p_AIO = ( UINT16 ) Tmp; //AIO_Data[9:8]]
			*p_AIO = ( *p_AIO ) << 8;
			ret = VPMU_ReadReg ( PMU_REG763X_AIO_DATA1, &Tmp ); //read AIO output data, AIO_Data[7:0].
			if ( VRET_NO_ERROR != ret ) {
				ret = VRET_DRV_PMU_STATE_ERROR;
				VPMU_WPRINTF ( "%s:err ,The AIO_DATA1 err[0x%X]!\r\n", FUNCTION_NAME, ret );
			} else {
				*p_AIO |= Tmp;//AIO_Data[7:0]
			}
		}	
	}else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
		ret = VPMU_ReadReg ( PMU_REG_7621I2S_AIO_DATA2, &Tmp ); //read AIO output data, AIO_Data[9:8].
		if ( VRET_NO_ERROR != ret ) {
			ret = VRET_DRV_PMU_STATE_ERROR;
			VPMU_WPRINTF ( "%s:err ,The AIO_DATA2 err[0x%X]!\r\n", FUNCTION_NAME, ret );
		} else {
			*p_AIO = ( UINT16 ) Tmp; //AIO_Data[9:8]]
			*p_AIO = ( *p_AIO ) << 8;
			ret = VPMU_ReadReg ( PMU_REG_7621I2S_AIO_DATA1, &Tmp ); //read AIO output data, AIO_Data[7:0].
			if ( VRET_NO_ERROR != ret ) {
				ret = VRET_DRV_PMU_STATE_ERROR;
				VPMU_WPRINTF ( "%s:err ,The AIO_DATA1 err[0x%X]!\r\n", FUNCTION_NAME, ret );
			} else {
				*p_AIO |= Tmp;//AIO_Data[7:0]
			}
		}
	}
	return ret;	
}
VRET_VALUE VCDC_PMU_GetAIO_Sleep(void)
{
	VRET_VALUE ret = VRET_ERROR;
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_WriteReg ( PMU_REG763X_MEN_AIOADC, 0 ); //Turn off AIO_ADC
	}else if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
		ret = VPMU_WriteReg ( PMU_REG_7621I2S_MEN_AIOADC, 0 ); //Turn off AIO_ADC
	}	
	return ret;
}
////////lcy end ////////////////////////
//* [1] Mmdc2,  "0" means PWM mode for RF&Dig DCDC. "1" means PFM mode for RF&Dig DCDC
//* [0] Mmdc1, "0" means RF&Dig DCDC decide PFM/PWM mode automatically.  "1" means RF&Dig DCDC PFM/PWM mode will be decided by Mmdc2.
void VPMU_Set7632MMDC ( UINT8 Mode )
{
    /* write I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //7632B
        //0x03[2]( Reset_Mode): it is used to set reset mode or shutdown mode when PMU_EN goes low.  Default 0 1 
        // 0x03[1] (Mmdc2) 0x03 0 0x03 [0] "1" 1 0x03 [1] 0x03 
        VPMU_WriteReg ( PMU_REG763X_MMDC, Mode | 0x04 );
    }
}
//end for 7632B


//xy: 20131116 add registers for 9611S_E, Deadlock reset delay time control register.
void VPMU_SetPowerRstDelay ( UINT8 DelaySel )
{
    ASSERT ( VPMU_IsValidPowerRstDelay ( DelaySel ) );
    /* write I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //7632B
        VPMU_WriteReg ( PMU_REG763X_MTRST_LOCK, DelaySel );
    } else {
        VPMU_WriteReg ( PMU_REG_MTRST_LOCK, DelaySel );
    }
}

#if VPMU_7632BATLOW_HWBUGFIX
#define VPMU_AIO_ADC_BATTERY_28V 	0x1B3	//the AIO_ADC is 10-bit ADC about 2.8v; //0323, the log show, not battery AIO_Data[9:0]=0x32, then had 4v battery AIO_Data[9:0]=0x270;
//xy20230321, add for battery plug in check; return value ==0xFF invalue, ==0 battery plugout, ==1 battery plugin; 
//zhuhantao--  D_LPO[5]=1 D_LPO[8]=1 MEN_AIOADC=1 SAR_ADC 19H 1AH VBAT>2.8V 
// 䣬 VBAT SOC_ON 2-3s PWR 
//
UINT8 VPMU_CheckBatteryPlug ( void )
{// also can use, the VPMU_Config7632AIO(1) + VPMU_Get7632AIO() then check battery plug?
	UINT8 ret=0xFF;
	UINT8 Tmp = 0;
	UINT16 AIO_ADC = 0, retry = 10;

	//D_LPO[8]=1,  D_LPO[5]="1", detect VBAT + Turn on AIO_ADC
    if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO1, &Tmp ) ) {
		VPMU_NPRINTF("VPMU_CheckBatteryPlug read PMU_REG763X_D_LPO1= 0x%x.\r\n", Tmp);
		if((Tmp&0x20)==0){
        	VPMU_WriteReg ( PMU_REG763X_D_LPO1, (Tmp|0x20));//set  D_LPO[5]=1;
		}
    } else {
    	VPMU_NPRINTF("VPMU_CheckBatteryPlug but PMU_REG763X_D_LPO1 read fail!\r\n");
		return ret;
    }

	if ( VRET_NO_ERROR == VPMU_ReadReg ( PMU_REG763X_D_LPO2, &Tmp ) ) {
		VPMU_NPRINTF("VPMU_CheckBatteryPlug read PMU_REG763X_D_LPO2= 0x%x.\r\n", Tmp);
		if((Tmp&0x01)==0){
        	VPMU_WriteReg ( PMU_REG763X_D_LPO2, (Tmp|0x01));//set  D_LPO[8]=1;
		}
    } else {
    	VPMU_NPRINTF("VPMU_CheckBatteryPlug but PMU_REG763X_D_LPO2 read fail!\r\n");
		return ret;
    }

	//set  MEN_AIOADC=1;
	VPMU_WriteReg (PMU_REG763X_MEN_AIOADC, 0x01);

	//check the AIO ready signal, (100us~2ms, default is 550us)
    do {
        //OSTimeDly(1);//delay 10ms;
        VCOMMON_Delay ( 300 ); //300us

        //
        ret = VPMU_ReadReg ( PMU_REG763X_TERM_AIOADC, &Tmp );
        if ( Tmp & 0x1 ) {
            Tmp = 0; //ready
        } else {
            Tmp = 1; //not ready
        }

        if ( VRET_NO_ERROR != ret ) {
            Tmp = 1;
            break;//return ret;
        }
        if ( Tmp == 0 ) { //AIO is ready to be read by 9613i.
            break;
        }

        retry--;
    } while ( retry );
    if ( retry == 0 ) {
        //ret = VRET_DRV_PMU_TERMB_TIMEOUT;
        VPMU_NPRINTF ( "%s:err ,The AIOADC Meter'termb has time out! \r\n", FUNCTION_NAME );
		return ret;
    }
				
	//read AIO_DATA1+AIO_DATA2 data;
	if ( VRET_NO_ERROR == VPMU_ReadReg (PMU_REG763X_AIO_DATA2, &Tmp) ) {
		VPMU_NPRINTF("VPMU_CheckBatteryPlug read PMU_REG763X_AIO_DATA2= 0x%x.\r\n", Tmp);
		AIO_ADC |= Tmp;
		AIO_ADC = AIO_ADC<<8;//AIO_Data[9:8]
    } else {
    	VPMU_NPRINTF("VPMU_CheckBatteryPlug but PMU_REG763X_AIO_DATA2 read fail!\r\n");
		return ret;
    }
	if ( VRET_NO_ERROR == VPMU_ReadReg (PMU_REG763X_AIO_DATA1, &Tmp) ) {
		VPMU_NPRINTF("VPMU_CheckBatteryPlug read PMU_REG763X_AIO_DATA1= 0x%x.\r\n", Tmp);
		AIO_ADC |= Tmp;//AIO_Data[7:0]
    } else {
    	VPMU_NPRINTF("VPMU_CheckBatteryPlug but PMU_REG763X_AIO_DATA1 read fail!\r\n");
		return ret;
    }
	VPMU_WriteReg (PMU_REG763X_MEN_AIOADC, 0x00);//close AIO_ADC;
	
	//check VBAT>2.8V  then battery plugin;
	if(AIO_ADC > VPMU_AIO_ADC_BATTERY_28V){
		ret=1;//plugin
	} else {
		ret=0;//plugout
	}
	VPMU_NPRINTF("VPMU_CheckBatteryPlug AIO_ADC[0x%x-%d].\r\n", AIO_ADC, ret);
		
	return ret;
}


//ZHT  BATLOW 飬 BATLOW 
VRET_VALUE VPMU_7632Close_BATLOW( void )
{
	VRET_VALUE ret = VRET_ERROR;
    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
		//set MEN_CMeter= 1 active then close BAT_LOW, void the hw bug maybe power down;
        if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
            ret = VPMU_WriteReg ( PMU_REG_MEN_CMeter, 0x01 );    
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        	#if VPMU_DBG_EN
        	UINT8 ver=0;
			VRET_VALUE tmp;
			#endif
            ret = VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 0x01);
			#if VPMU_DBG_EN
            tmp = VPMU_ReadReg ( PMU_REG763X_MEN_CMeter, &ver);
            VPMU_NPRINTF("VPMU_7632Close_BATLOW [0x%x=0x%x, ret=%d]keep MEN_CMeter=1.\r\n", PMU_REG763X_MEN_CMeter, ver, tmp);
            #endif
        }
    }
    return ret;
}
#endif

// add new Coulomb Meter function; this had delay so can not call by interrupts.
VRET_VALUE VPMU_GetCoulombVD ( UINT8 *p_VD )
{
    VRET_VALUE ret = VRET_ERROR;
    UINT8 Tmp;
    ASSERT ( p_VD != NULL );
    if (p_VD == NULL) {
        return VRET_ERROR;
    }
    *p_VD = 0;

#if VPMU_7632BATLOW_HWBUGFIX
	//20230418 xy, fix the 7632B HW bug about the BATLOW; set close as default sw fix limited;
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		if(VPMU_CheckBatteryPlug()==1){
			VPMU_NPRINTF("VPMU_GetCoulombVD and had battery then do CoulombVD.\r\n");
		} else {
			//Keep the "Coulomb Meter enter active mode", then also close the BAT_LOW function;
			#if 1
			VPMU_7632Close_BATLOW();
			#else
			VRET_VALUE ret;
			UINT8 ver=0;
			
			VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 0x01);
			ret = VPMU_ReadReg ( PMU_REG763X_MEN_CMeter, &ver);
			
			VPMU_NPRINTF("VPMU_GetCoulombVD and not battery[0x%x=0x%x, ret=%d]keep 05H=1.\r\n", PMU_REG763X_MEN_CMeter, ver, ret);
			
			*p_VD = 0;
			#endif
			
			return VRET_DRV_PMU_TERMB_TIMEOUT;
		}
	}
#endif

    /* read I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
        if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
            ret = VPMU_ReadReg ( PMU_REG_MEN_CMeter, &Tmp );    //read MEN_CMeter
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
            ret = VPMU_ReadReg ( PMU_REG763X_MEN_CMeter, &Tmp );
        }

        if ( VRET_NO_ERROR == ret ) {
            if ( Tmp == 0 ) {
                UINT16 retry = 30;
                if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
                    ret = VPMU_WriteReg ( PMU_REG_MEN_CMeter, 1 ); // let Coulomb Meter enter active mode
                    retry = 22;
                } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
                    ret = VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 1 );
                    retry = 30;//retry = 2;
                }

                //check the VD ready signal, (100us~2ms)
                do {
                    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
                        VCOMMON_Delay ( 100 ); //100us
                        //the VPMU_ReadReg may also have some delay
                        if ( VCDC_GetVerNo() > CDC_VERSION_7621_A ) { //7621S, 7621G
                            ret = VPMU_ReadReg ( PMU_REG_INT_STATUS, &Tmp );

                            if ( Tmp & 0x2 ) {
                                Tmp = 0; //ready
                            } else {
                                Tmp = 1; //not ready
                            }
                        } else {    //9611
                            ret = VPMU_ReadReg ( PMU_REG_TERMB, &Tmp ); //read termb
                        }
                    } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
                        //7632B need 7ms delay after MEN_CMeter set active.
                        //VCOMMON_Delay(1000*7);//delay 7ms;
                        //OSTimeDly(1);//delay 10ms;
						VCOMMON_Delay ( 600 ); //100us
                        //
                        ret = VPMU_ReadReg ( CDC_REG763X_INT_STATUS, &Tmp );
                        if ( Tmp & 0x2 ) {
                            Tmp = 0; //ready
                        } else {
                            Tmp = 1; //not ready
                        }
                    }

                    if ( VRET_NO_ERROR != ret ) {
                        Tmp = 1;
                        break;//return ret;
                    }

                    if ( Tmp == 0 ) { //VD is ready to be read by 9613i.
                        break;
                    }

                    retry--;
                } while ( retry );

                if ( retry == 0 ) {
                    Tmp = 1;
                    ret = VRET_DRV_PMU_TERMB_TIMEOUT;
                    VPMU_WPRINTF ( "%s:err ,The Coulomb Meter'termb has time out! \r\n", FUNCTION_NAME );
                }

                if ( Tmp == 0 ) {
                    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
                        ret = VPMU_ReadReg ( PMU_REG_VD, p_VD ); //read Coulomb Meter output data.
                    } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
                        ret = VPMU_ReadReg ( PMU_REG763X_VD, p_VD );
                    }
                }

            } else {//error, Coulomb Meter had entered active mode
                ret = VRET_DRV_PMU_STATE_ERROR;
                VPMU_WPRINTF ( "%s:err ,The Coulomb Meter had entered active mode! (0x%X)\r\n", FUNCTION_NAME, ret );
            }
        }

        if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
            VPMU_WriteReg ( PMU_REG_MEN_CMeter, 0 );    // let Coulomb Meter enter sleep mode (low power consumption)
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
            VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 0 );
        }
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }

    return ret;
}
/////////// lcy : add by 20220802///////////////////////
VRET_VALUE VPMU_GetCoulombVD_Active(void)
{
	VRET_VALUE ret = VRET_ERROR;
  UINT8 Tmp;	

	/* read I2C register */
	if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
		if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
				ret = VPMU_ReadReg ( PMU_REG_MEN_CMeter, &Tmp );    //read MEN_CMeter
			} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
				ret = VPMU_ReadReg ( PMU_REG763X_MEN_CMeter, &Tmp );
			}
			if ( VRET_NO_ERROR == ret ) {
				if ( Tmp == 0 ) {
					if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
							ret = VPMU_WriteReg ( PMU_REG_MEN_CMeter, 1 ); // let Coulomb Meter enter active mode
						} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
							ret = VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 1 );
						}
				}
				else{
					ret = VRET_DRV_PMU_STATE_ERROR;
					VPMU_WPRINTF ( "%s:err ,The Coulomb Meter had entered active mode! (0x%X)\r\n", FUNCTION_NAME, ret );
					
					if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
						VPMU_WriteReg ( PMU_REG_MEN_CMeter, 0 );    // let Coulomb Meter enter sleep mode (low power consumption)
					} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
						VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 0 );
					}
				}
			}
		}
		return ret;
}
VRET_VALUE VPMU_GetCoulombVD_WaitReady(UINT8* p_ready)
{
	VRET_VALUE ret = VRET_ERROR ;
	UINT8 Tmp = 0;
	
	if(p_ready == NULL){
		ret =VRET_ERROR;
		return ret;
	}
	
	if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
		//the VPMU_ReadReg may also have some delay
		 if ( VCDC_GetVerNo() > CDC_VERSION_7621_A ) { //7621S, 7621G
			 ret = VPMU_ReadReg ( PMU_REG_INT_STATUS, &Tmp );
		 } else {    //9611
			 ret = VPMU_ReadReg ( PMU_REG_TERMB, &Tmp ); //read termb
		}
	} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_ReadReg ( CDC_REG763X_INT_STATUS, &Tmp );
	}
	
	if(Tmp & 0x2)
		* p_ready = 0; //ready
	else
		* p_ready = 1; //no ready
	
	return ret;
}
VRET_VALUE VPMU_GetCoulombVD_ReadData(UINT8 *p_VD)
{
	VRET_VALUE ret = VRET_ERROR;
	if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
		ret = VPMU_ReadReg ( PMU_REG_VD, p_VD ); //read Coulomb Meter output data.
	} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_ReadReg ( PMU_REG763X_VD, p_VD );
	}
	return ret;
}
VRET_VALUE VPMU_GetCoulombVD_Sleep(void)
{
	VRET_VALUE ret = VRET_ERROR;
	
	if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
		ret = VPMU_WriteReg ( PMU_REG_MEN_CMeter, 0 );    // let Coulomb Meter enter sleep mode (low power consumption)
	} else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		ret = VPMU_WriteReg ( PMU_REG763X_MEN_CMeter, 0 );
  }
	return ret;
}
/////////////////lcy end/////////////////////////


/******************************************************************************
Description:
    Get 9611 information from VER_REG_SEL reg.

Parameters:
    p_VerInfo: the 9611 information by read   VER_REG_SEL reg;
               [7:4] show the hardware version information,  bit[4] VER_SEL_OldNew(not use by now)
               [3:0] show the software feature version select, bit[0] VER_SEL_LowHigh(show feature select for headset or loudspeaker)
                    [p_VerInfo]           Show 9611 info
                        FF                  old 9611(9611SD0 or 9611F0)
                        01                  new 9611(9611SE0), high-end version
                        00          new 9611(9611SE0), low-end version

Return Value:
    Returns VRET_NO_ERROR if succeed, else returns error code.
*****************************************************************************/
VRET_VALUE VPMU_GetVerInfo ( UINT8 *p_VerInfo )
{
    VRET_VALUE ret = VRET_ERROR;
    ASSERT ( p_VerInfo != NULL );
    if (p_VerInfo  == NULL) {
        return VRET_ERROR;
    }

    *p_VerInfo = 0x55;//invalid value; if the reg not used, maybe return 0xff.
    /* read I2C register */
    if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
        if ( 1/*g_VSYS_PWR_Config.cdc_ver > CDC_VERSION_7621_A*/ ) { //7621S, 7621G
            ret = VPMU_ReadReg ( VER_REG_SEL_7621S, p_VerInfo );
        } else {    //9611, 7621A
            ret = VPMU_ReadReg ( VER_REG_SEL, p_VerInfo );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        ret = VPMU_ReadReg ( PMU_REG763X_VER_SEL, p_VerInfo );
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }

    return ret;
}

//20201218xy, add to get the version for metal change from 7632B; //some as VPMU_GetVerNo();
VRET_VALUE VPMU_GetMCVer ( UINT8 *p_VerInfo )
{
    VRET_VALUE ret = VRET_ERROR;
    ASSERT ( p_VerInfo != NULL );

    if (p_VerInfo  == NULL) {
        return VRET_ERROR;
    }

    *p_VerInfo = 0x00;//invalid value;
    /* read I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        ret = VPMU_ReadReg ( PMU_REG763X_CHIP_ID3, p_VerInfo );
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }

    return ret;
}

//20140218 add for 7621S
VRET_VALUE VPMU_LineInDetect ( UINT8 *p_LineIn )
{
    VRET_VALUE ret;

    ASSERT ( p_LineIn != NULL );
    if (p_LineIn  == NULL) {
        return VRET_ERROR;
    }
    *p_LineIn = 0;
    /* read I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        ret = VPMU_ReadReg ( CDC_REG763X_INT_STATUS, p_LineIn );
    } else {
        ret = VPMU_ReadReg ( PMU_REG_INT_STATUS, p_LineIn );
    }
    *p_LineIn = ( *p_LineIn ) & 0x01; //bit[0] CDC_LINE_DET, line in detect; 1=detect, 0=no detect

    return ret;
}

VRET_VALUE VPMU_IntInverseConfig ( UINT8 *p_InvConfig, UINT8 mode )
{
    VRET_VALUE ret;

    ASSERT ( p_InvConfig != NULL );
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        if ( mode == 0 ) { //read
            /* read I2C register */
            ret = VPMU_ReadReg ( CDC_REG763X_INT_INV, p_InvConfig );
        } else { //write
            /* write I2C register */
            ret = VPMU_WriteReg ( CDC_REG763X_INT_INV, ( *p_InvConfig ) );
        }
    } else {
        if ( mode == 0 ) { //read
            /* read I2C register */
            ret = VPMU_ReadReg ( PMU_REG_INT_INV, p_InvConfig );
        } else { //write
            /* write I2C register */
            ret = VPMU_WriteReg ( PMU_REG_INT_INV, ( *p_InvConfig ) );
        }
    }
    return ret;
}

VRET_VALUE VPMU_IntMaskConfig ( UINT8 *p_MaskConfig, UINT8 mode )
{
    VRET_VALUE ret;

    ASSERT ( p_MaskConfig != NULL );
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        if ( mode == 0 ) { //read
            /* read I2C register */
            ret = VPMU_ReadReg ( CDC_REG763X_INT_MASK, p_MaskConfig );
        } else { //write
            /* write I2C register */
            ret = VPMU_WriteReg ( CDC_REG763X_INT_MASK, ( *p_MaskConfig ) );
        }
    } else {
        if ( mode == 0 ) { //read
            /* read I2C register */
            ret = VPMU_ReadReg ( PMU_REG_INT_MASK, p_MaskConfig );
        } else { //write
            /* write I2C register */
            ret = VPMU_WriteReg ( PMU_REG_INT_MASK, ( *p_MaskConfig ) );
        }
    }
    return ret;
}

/******************************************************************************
Description:
    Get the select 76XX version No by chip id.

Parameters:
    void.

Return Value:
    UINT8 - the cdc ver;

Remarks:
    .
*****************************************************************************/
UINT8 VPMU_GetVerNo ( void )
{
	//20230925xy, driver should support automatic recognition by Reg chip ID;  //some as VPMU_GetMCVer();
	VRET_VALUE ret=0;
	UINT8 ver=0;

	ret = VPMU_ReadReg ( PMU_REG763X_CHIP_ID3, &ver);
	//VCOMMON_NPRINTF("GetVerNo, CHIP_ID3=%x[ret=%x]!\r\n", ver, ret);
	
	if(VRET_NO_ERROR == ret) {
		if((ver==0x11)||(ver==0x12)) { //VA7632B_E5==0x11, VA7632B_F5==0x12, and audio just one adc ; 
	        return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_F5);
		} else if(ver==0x01) { //VA7638_A0(for 9648C/9647X)==0x01 have audio and two adc, just PMU DCDC had different to VA7632B_F5;
	        return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7638_A0);
		} else if((ver==0x0C)||(ver==0x0D)) { //VA7632B_E3==0x0C, VA7632B_F3==0x0D;
	        return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_E3);
	    } else {//maybe the VA7652PMU_A0=0x01 for 9652;
	        return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_F5);
	    }
	} else {
    	VCOMMON_EPRINTF("VPMU_GetVerNo, but IIC read fail[ret=%x]!\r\n", ret);
        return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
    }
}



#ifdef CORE_SEL_MCUTOP
extern DM_DATA_FEATURE		g_DM_data_features;

extern BOOL DM_GetPMUIsr_En(void)
{
	if(g_DM_data_features.pmu_statisr_en)
		return TRUE;
	else
		return FALSE;
}

BOOL VPMU_StatIsr_En ( void )
{
    return DM_GetPMUIsr_En();
}

/******************************************************************************
Description:
    Get the select 76XX version No.

Parameters:
    void.

Return Value:
    UINT8 - the cdc ver;

Remarks:
    .
*****************************************************************************/
UINT8 VCDC_GetVerNo ( void )
{
	#if 1//20240123xy, support automatic recognition by Reg chip ID, add VA7638_A0(just PMU DCDC had different to VA7632B_F5) ; 
	if(CDC_VERSION_DEFAULT == CDC_VERSION_CHECK){
		VRET_VALUE ret=0;
		UINT8 ver=0;

		//ret = VPMU_GetMCVer(&ver);
		ret = VPMU_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
		//VCOMMON_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
		
		if(VRET_NO_ERROR == ret) {
			if((ver==0x11)||(ver==0x12)) { //VA7632B_E5==0x11, VA7632B_F5==0x12, and audio just one adc ;
 				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_F5);
			} else if(ver==0x01) { 
				//20240201xy, when CHIP_ID3=0x01 then should distinguish the had done chip is 7652PMU_CSMC or 7638_A0, then should check reg CHIP_ID2; 
 				UINT8 chip_ID2=0; 
				if(VRET_NO_ERROR ==VPMU_ReadReg ( 0xF4/*PMU_REG763X_CHIP_ID2*/, &chip_ID2)){
					if(chip_ID2==0xba) {
						//VA7638_A0(for 9648C/9647X)==0x01 have audio and two adc, just PMU DCDC had different to VA7632B_F5;
						return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7638_A0);
					} else if(chip_ID2==0xbb) {
						//CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
						return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
					}else {
						VCOMMON_EPRINTF("GetVerNo, but CHIP_ID2=%x, not define!\r\n", chip_ID2);
						return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
					}
				} else {
					VCOMMON_EPRINTF("GetVerNo, but IIC read CHIP_ID2 fai!\r\n");
					return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
				}
			} else if((ver==0x0C)||(ver==0x0D)) { //VA7632B_E3==0x0C, VA7632B_F3==0x0D;
				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_E3);
			} else if((ver>=0x01)&&(ver<=0x04)) { //CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
			} else if((ver>=0xa0)&&(ver<=0xcf)) { //TSMC 7652PMU(for 9652, remove audio) ==0xa0~0xcf will been done, the pmu driver most some as 7652PMU_CSMC, different at 02H MEN5[1:0];
				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7652PMU_TSMC);
			} else {
				VCOMMON_EPRINTF("GetVerNo, but CHIP_ID3=%x, not define!\r\n", ver);
				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
			}
		} else {
			VCOMMON_EPRINTF("GetVerNo, but IIC read fail[ret=%x]!\r\n", ret);
			return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
		}
	} else {
		return CDC_VERSION_DEFAULT; //may optimization for code size down;
	}
	#else
	#if 1//20230926xy, driver should support automatic recognition by Reg chip ID; 
	if(CDC_VERSION_DEFAULT == CDC_VERSION_CHECK){
		VRET_VALUE ret=0;
		UINT8 ver=0;

		//ret = VPMU_GetMCVer(&ver);
		ret = VPMU_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
		//VCOMMON_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
		
        if(VRET_NO_ERROR == ret) {
			if((ver==0x11)||(ver==0x12)||(ver==0x01)) { //VA7632B_E5==0x11, VA7632B_F5==0x12,  VA7638_A0(for 9648C)==0x01 have audio and two adc ;
            	return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_F5);
			} else if((ver==0x0C)||(ver==0x0D)) { //VA7632B_E3==0x0C, VA7632B_F3==0x0D;
            	return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_E3);
			} else if((ver>=0x01)&&(ver<=0x04)) { //CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
            	return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
			} else if((ver>=0xa0)&&(ver<=0xcf)) { //TSMC 7652PMU(for 9652, remove audio) ==0xa0~0xcf will been done, the pmu driver most some as 7652PMU_CSMC, different at 02H MEN5[1:0];
            	return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7652PMU_TSMC);
			} else {
				VCOMMON_EPRINTF("GetVerNo, but CHIP_ID3=%x, not define!\r\n", ver);
				return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
			}
        } else {
        	VCOMMON_EPRINTF("GetVerNo, but IIC read fail[ret=%x]!\r\n", ret);
            return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_NONE);
        }
	} else {
		return CDC_VERSION_DEFAULT; //may optimization for code size down;
	}
	
	#else
	#if 1//20210105xy, temporary special check F3 or F5; if F3F5 the bt bin more 23K size (from 728K to 751K);
	if(CDC_VERSION_DEFAULT == CDC_VERSION_7632B_F3F5){
		VRET_VALUE ret=0;
		UINT8 ver=0;

		if((g_VSYS_PWR_Config.cdc_ver == CDC_VERSION_7632B_E3) || (g_VSYS_PWR_Config.cdc_ver == CDC_VERSION_7632B_F5)) 
			  return g_VSYS_PWR_Config.cdc_ver;
		else {
				//ret = VPMU_GetMCVer(&ver);
				ret = VPMU_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
				VPMU_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
				
				// if((VRET_NO_ERROR == ret)&&((ver==0x11)||(ver==0x12)))
                if (1) {    // 兼容其他chip_id，直接使用F5的型号
                    return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_F5);
                }
                else {
                    return  (g_VSYS_PWR_Config.cdc_ver = CDC_VERSION_7632B_E3);
                }
		}	
		
	} else {
				return CDC_VERSION_DEFAULT;
	}
	
	#else
    return CDC_VERSION_DEFAULT;//g_VCDC_Config.cdc_ver; 
	#endif
	#endif
	#endif
}






void VPMU_EnableInt(void)
{
    VCPU_INT_SrcEnable(IRQ_VA76XX_INT1);
}

void VPMU_DisableInt(void)
{
    VCPU_INT_SrcDisable(IRQ_VA76XX_INT1);

}

void VPMU_RestoreInt(BOOL enable)
{
    if (enable) {
        VCPU_INT_SrcEnable(IRQ_VA76XX_INT1);
    } else {
        VCPU_INT_SrcDisable(IRQ_VA76XX_INT1);
    }
}

UINT8 VPMU_GetIntStatus(void)
{
    UINT8 reg = 0;
    VPMU_ReadReg(CDC_REG763X_INT_STATUS, &reg);
    return reg;
}

void VPMU_ClrIntStatus(UINT8 status)
{
    VPMU_WriteReg(CDC_REG763X_INT_INV, status);
}

UINT8 VPMU_GetIntMask(void)
{
    UINT8 mask = 0;
    VPMU_ReadReg(CDC_REG763X_INT_MASK, &mask);
    return mask;
}

void VPMU_SetIntMask(UINT8 mask)
{
    VPMU_WriteReg(CDC_REG763X_INT_MASK, mask);
}

extern int g_need_pmu_workaround;
extern UINT8 get_current_coulomb_value(void);
extern void is_batt_low_check(void);
volatile BOOL g_pg_insert_flag = FALSE;
__attribute__((weak)) void is_batt_low_check(void){}

void VPMU_Interrupt_Routine_Service ( void )
{
    UINT8 pmu_int_status = 0;

    pmu_int_status = VPMU_GetIntStatus();
    VPMU_ClrIntStatus(pmu_int_status);
	
	if (g_need_pmu_workaround == 0)
	{
		return;
	}

    if (pmu_int_status & (1 << 3)) {
//		TOP_DEBUG("-----In\r\n");
//		if (! VGPIO_GetPinInput(DC_DETECT_PIN)) // charger detect
		{	
			TOP_DEBUG("***Pg is plug in\r\n");	
			g_pg_insert_flag = TRUE;
			
//			// VBAT<3.35V enable charge bypass
//			if (get_current_coulomb_value() < 4) { // Peridical update (1s) by HLP_PMT_ReadCoulomb()
//				
//				if (! g_charge_bypass_enable) {
//					VPMU_Set_Bypass(TRUE);
//					g_charge_bypass_enable = TRUE;
//					TOP_DEBUG("***Charge Bypass is enable!\r\n");
//				}
//			}
		}
    } else {
//		TOP_DEBUG("------Out\r\n");
		TOP_DEBUG("***Pg is pull out\r\n.");
		g_pg_insert_flag = FALSE;
		//if (VGPIO_GetPinInput(DC_DETECT_PIN)) // charger detect
#ifdef CORE_SEL_MCUTOP		
		if (TOP_MODE_DEMO != top_mode_get()) // 修复demo mode 快速插拔死机问题(Go4/Clip5已验证)
#endif			
		{
			if (g_charge_bypass_enable) {
				VPMU_Set_Bypass(FALSE);
				g_charge_bypass_enable = FALSE;
				TOP_DEBUG("***Charge Bypass is disable!\r\n");
			}
			
			is_batt_low_check();
		}
    }
}

#else

extern BOOL DM_GetPMUIsr_En ( void );


BOOL VPMU_StatIsr_En ( void )
{
    return DM_GetPMUIsr_En();
}

#endif





//set the DCDC or LDO by the MEN5 reg;
void VPMU_SetLDO5En ( BOOL IsEnable )
{
    if ( VCDC_GetVerNo() < CDC_VERSION_7621_A ) { //9611
        /* write I2C register */
        VPMU_WriteReg ( PMU_REG_MEN5, IsEnable );
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) {
        //for 7621
        if ( g_VSYS_PWR_Config.pwr_mode == CDC_PWR_DCDC ) {
            VPMU_WriteReg ( PMU_REG_MEN5, g_VSYS_PWR_Config.pmu_ldo5 ); //set DCDC, F0 or 70
            if ( IsEnable == FALSE ) {
                VPMU_WriteReg ( PMU_REG_MVDC, g_VSYS_PWR_Config.output );
            }
        } else {//CDC_PWR_LDO5 as default
            if ( IsEnable ) { //open LDO5
                VPMU_WriteReg ( PMU_REG_MEN5, ( g_VSYS_PWR_Config.pmu_ldo5 & 0xfc ) | 0x01 ); //enter normal work mode, MEN5 [0] =1, MEN5 [1] =0;
                if ( VCDC_GetVerNo() == CDC_VERSION_7621S_A ) { //VA7621S, set DCDC is 1.8V, only for SA0 version.
                    VPMU_WriteReg ( PMU_REG_MVDC, g_VSYS_PWR_Config.pmu_set_dac/*0x01*/ );
                }
            } else {//close LDO5, debug pin
            	if ( VCDC_GetVerNo() != CDC_VERSION_7621_I2S)  // 7621i2s not Enter low power mode
            	{
                	VPMU_WriteReg ( PMU_REG_MEN5, ( g_VSYS_PWR_Config.pmu_ldo5 ) | 0x03 ); //Enter low power mode, MEN5[0]=1, MEN5[1]=1;
            	}

                if ( VCDC_GetVerNo() == CDC_VERSION_7621S_A ) { //VA7621S.
                    VPMU_WriteReg ( PMU_REG_MVDC, g_VSYS_PWR_Config.output ); //VPMU_WriteReg(PMU_REG_MVDC, 0x00);    //
                }
            }
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 ) { //VA7632B
        if ( IsEnable ) { //open audio dcdc&LDO5
            //Power on audio DCDC\LDO
            VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); //on audio dcdc

            if ( g_VSYS_PWR_Config.pwr_mode == CDC_PWR_DCDC ) {
                VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5/*0x41*/ ); //set DCDC, FX or 7X //on audio LDO5--bit0=1, bit6=1? audio dcdc  dcdc 
            } else {//CDC_PWR_LDO5 as default
                VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5 | 0x01/*0x01*/ ); //set DCDC, BX  //on audio LDO5--bit0=1, bit6=0? audio dcdc  LDO , bit[0] only 
            }
            //VPMU_WriteReg(PMU_REG763X_DCA_EN, 0x01);
        } else {//close audio dcdc&LDO5
            VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x00 ); //off audio dcdc
            VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5/*0xB0*/ ); //off audio LDO5--bit0=0; normal dcdc set 0xF0, LDO set 0xB0;
        }
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { 
            VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); // audio dcdc on
            VPMU_WriteReg ( PMU_REG7632X_MEN5, 0x51);   // LDO5 Enable
    #if 0   // TODO: 需要优化PMU驱动
                if ( IsEnable ) { //open audio dcdc&LDO5
                //Power on audio DCDC\LDO
                VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); //on audio dcdc

                if ( g_VSYS_PWR_Config.pwr_mode == CDC_PWR_DCDC ) {
                    VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5/*0x41*/ ); //set DCDC, FX or 7X //on audio LDO5--bit0=1, bit6=1? audio dcdc  dcdc 
                } else {//CDC_PWR_LDO5 as default
                    VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5 | 0x01/*0x01*/ ); //set DCDC, BX  //on audio LDO5--bit0=1, bit6=0? audio dcdc  LDO , bit[0] only 
                }
                //VPMU_WriteReg(PMU_REG763X_DCA_EN, 0x01);
            } else {//close audio dcdc&LDO5
            //9648 need F5,E5,E3,F3 version keep reg 0x0D value 0x1 
            VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); //should keep on.
            VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5/*0xB0*/ ); //off audio LDO5--bit0=0; normal dcdc set 0xF0, LDO set 0xB0;
            }
    #endif
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC) { 
            VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); // audio dcdc on
            VPMU_WriteReg ( PMU_REG7632X_MEN5, 0x50);   // LDO5 Enable
#if 0   // TODO: 需要优化PMU驱动
		VPMU_WriteReg ( PMU_REG763X_DCA_EN, 0x01 ); //should keep on?.
		VPMU_WriteReg ( PMU_REG7632X_MEN5, g_VSYS_PWR_Config.pmu_ldo5/*0xB1*/ ); //off audio LDO5--bit0=1;  default set 0xB1;
#endif
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}

void VPMU_SetDCDC_CodecOn ( UINT8 Mode )
{
#ifndef VCODEC_HW_MODE_HIFICODEC
#define VCODEC_HW_MODE_HIFICODEC 	2 		//hifi CODEC, FIFO
#endif
    //only for 7621, codec open
    if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) && ( g_VSYS_PWR_Config.pwr_mode == CDC_PWR_DCDC ) ) {
        if ( Mode == VCODEC_HW_MODE_HIFICODEC ) { //open both adc&dac
            VPMU_WriteReg ( PMU_REG_MVDC, g_VSYS_PWR_Config.pmu_set_adc );
        } else {//VCODEC_HW_MODE_HIFIDAC, only open dac
            VPMU_WriteReg ( PMU_REG_MVDC, g_VSYS_PWR_Config.pmu_set_dac );
        }
	#if 1//20201204xy, for liqin&lvjie use at 9638p asic chip;
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 ) { //for VA7632BF5    
		if ( Mode == VCODEC_HW_MODE_HIFICODEC ) { //open both adc&dac
            VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_adc );
        } else {//VCODEC_HW_MODE_HIFIDAC, only open dac
            VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_dac );
        }
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //for VA7632BF3, had some bug, tmp used for SV debug; when product will use VA7632BF5;    
		#if 0//20201218xy, the 7632BF5 audio power from LDO not DCDC, so not set the dcdc; F3 used to replaced by F5;
		#ifdef FPGA_QA_BUILD	//20201217xy, if FPGA use default value the LineIn not sound, should set high DC-DC output voltage; wujie say the dac or dac&adc can work at the some voltage;
		//if ( Mode == VCODEC_HW_MODE_HIFICODEC ) { //open both adc&dac
        //    VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_adc );
        //} else {//VCODEC_HW_MODE_HIFIDAC, only open dac
            VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_dac );
        //}
		#endif
		#endif
	#else
    } else if ( ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0)/* && ( g_VCDC_Debug.pwr_mode == CDC_PWR_DCDC )*/ ) { //VA7632B
            if ( Mode == VCODEC_HW_MODE_HIFICODEC ) { //open both adc&dac
            VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_adc );
        } else {//VCODEC_HW_MODE_HIFIDAC, only open dac
            VPMU_WriteReg ( PMU_REG763X_MVDCA, g_VSYS_PWR_Config.pmu_set_dac );
        }
	#endif
    } else {
        ;//VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
    }
}


//20201218xy, add for 9638p+ 7632BF3/7632F5 chip Overclocking configuration;
void VPMU_SetDigitalDCDC( DIGITALDCDC_VOL DCDC_Vol )
{
    NOTICE_PRINTF("TOP Core: VPMU_SetDigitalDCDC set to %d, return address: 0x%x\r\n", DCDC_Vol, __GET_RETURN_ADDR());
	//for overclocking may set from default 1.2v to 1.3v;  
	if ( VCDC_GetVerNo() == CDC_VERSION_7632B_E3){
			VPMU_WriteReg ( PMU_REG763X_MVDC,  (DCDC_Vol&0x0F)); //set Digital DC-DC;
  } else if(VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC) {	
			VPMU_WriteReg ( PMU_REG763X_MVDC, (DCDC_Vol+8)&0x0F ); //set Digital DC-DC;
  } else if( VCDC_GetVerNo() == CDC_VERSION_7638_A0) {	
			VPMU_WriteReg ( PMU_REG763X_MVDCA, (DCDC_Vol+8)&0x0F ); //set Digital DC-DC;	
  } else {
        VPMU_WPRINTF("%s: cdc_ver = %d, the DigitalDCDC set err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
  }
}

//20210803xy, for get the Digital DC-DC maybe revert;
UINT8 VPMU_GetDigitalDCDC(void)
{
	UINT8 Value = DIG_DCDC_1_2V;
	if ( VCDC_GetVerNo() == CDC_VERSION_7632B_E3){
			VPMU_ReadReg ( PMU_REG763X_MVDC,  &Value); //get Digital DC-DC;
  } else if(VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC) {	
			VPMU_ReadReg ( PMU_REG763X_MVDC,  &Value); //get Digital DC-DC;
			Value += 0x08;//as DIGITALDCDC_VOL, by the 7632BE3, but F3 should +0x08; 
			Value &= 0x0F;
  } else if(VCDC_GetVerNo() == CDC_VERSION_7638_A0) {	
			VPMU_ReadReg ( PMU_REG763X_MVDCA,  &Value); //get Digital DC-DC;
			Value += 0x08;//as DIGITALDCDC_VOL, by the 7632BE3, but F3 should +0x08; 
			Value &= 0x0F;
  } else {
        VPMU_WPRINTF("%s: cdc_ver = %d, the DigitalDCDC get err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
  }
	
	return Value;
}


void VPMU_SetDCDCMode ( UINT8 Mode )
{
    ASSERT ( VPMU_IsValidDCDCMode ( Mode ) );
    /* write I2C register */
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) { //7632B
        VPMU_WriteReg ( PMU_REG763X_MMDCA, Mode );
    } else {
        VPMU_WriteReg ( PMU_REG_MMDC, Mode );
    }
}


/******************************************************************************
Description:
    Get the 7652PMU chip version.

Parameters:
    void.

Return Value:
    UINT8 - the chip version;

Remarks:
    .
*****************************************************************************/
UINT8 VPMU_Get7652Ver ( void )
{
	//20231003xy, driver should choose some function by the chip version for new 7652PMU;  
	VRET_VALUE ret = VRET_ERROR;
	UINT8 ver=0;

	ret = VPMU_ReadReg ( PMU_REG7652_OTP_DUMMY, &ver);
	if(VRET_NO_ERROR == ret) {
		VCOMMON_NPRINTF("Get7652Ver, OTP_DUMMY=%x.\r\n", ver);
    } else {
    	VCOMMON_EPRINTF("VPMU_Get7652Ver, but IIC read fail[ret=%x]!\r\n", ret);
    }

	return ver;
}


//return 1 means need the softwave workaround for charger fix, retrun 0 not need;
UINT8 VPMU_Get7652ChargerFix( void )
{
	//20231003xy, check wether need the workaround function to fix repeat plug in/out charge cause system fault for new 7652PMU;  
	if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC) {
		//check if chip_id3 [7:0] 8'ha0~8'hcf, and if OTP_dummy[2]=0 then need the softwave workaround for charger fix, or OTP_dummy[2]=1 not need the workaround; 
		if((VPMU_Get7652Ver()&0x4)==0){
			return 1;//OTP_dummy[2]=0 then need the softwave workaround for charger fix
		} else {
			return 0;//OTP_dummy[2]=1 not need the workaround
		}
	} else {
		return 1;//??other not 7652PMU_TSMC, will should open this fix?
	}
}



#if VCDC_DBG_EN
UINT8 g_7621Reg_Value[256];
#endif
VRET_VALUE VPMU_WriteReg ( UINT8 Reg, UINT8 Value )
{
    VRET_VALUE ret = VRET_ERROR;

    ret = VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,Reg,&Value,1);//ret = Vcdc_CmdBus_Write(I2C_MODE_SLOW, PMU_I2C_SLAVE_ADDR, Reg, Value);
    VPMU_WPRINTF("VPMU_WriteReg: Reg address: 0x%x, WriteValue: 0x%x\r\n", Reg, Value);

    if ( ret ) {
        ;//VPMU_FPRINTF ( "%s: PMU config failed!\r\n", FUNCTION_NAME );
    } else {
        #if VCDC_DBG_EN
        if ( Reg < 256 ) {
            g_7621Reg_Value[Reg] = Value;
        }
	#endif
    }

    return ret;
}

VRET_VALUE VPMU_ReadReg ( UINT8 Reg, UINT8 *p_Value )
{
    VRET_VALUE ret = VRET_ERROR;

    ret = VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,Reg,p_Value,1);//ret = Vcdc_CmdBus_Read(I2C_MODE_SLOW, PMU_I2C_SLAVE_ADDR, Reg, p_Value);

    VPMU_WPRINTF("VPMU_ReadReg: Reg address: 0x%x, ReadValue: 0x%x\r\n", Reg, *p_Value);

    return ret;
}

#else

void VPMU_Interrupt_Routine_Service ( void )
{
}

VRET_VALUE VPMU_GetInvState(UINT8 *p_State)
{
	return VRET_ERROR;
}
void VPMU_SetInvState(UINT8 state)
{
//	return VRET_ERROR;
}

void VPMU_Set_Bypass (UINT8 Enable)
{
//	return VRET_ERROR;
}

void VPMU_SetBatteryLowLevel ( UINT8 MBV )
{
//	return VRET_ERROR;
}

void VPMU_Set7632ChargingTime ( UINT8 Mode ) //after 7632B add this new function;
{
//	return VRET_ERROR;
}
  
void VPMU_Set7632PMUSel ( UINT8 Sel )
{
//	return VRET_ERROR;
}

VRET_VALUE VPMU_Get7621PMUEn ( UINT8 *p_State )
{
	return VRET_ERROR;
}

void VPMU_Set7621PMUEn ( UINT8 Enable )
{
//	return VRET_ERROR;
}

void VPMU_SetChargingDisable ( UINT8 Ctrl )
{
//	return VRET_ERROR;
}

void VPMU_SetMCP ( UINT8 Mcp )
{
//	return VRET_ERROR;
}

void VPMU_SetCharge ( UINT8 Voltage, UINT8 Current )
{
//	return VRET_ERROR;
}

void VPMU_SetDCOutputVoltage ( UINT8 Voltage )
{
//	return VRET_ERROR;
}

VRET_VALUE VCDC_PMU_GetAIO_Sleep(void)
{
	return VRET_ERROR;
}


VRET_VALUE VCDC_PMU_GetAIO_ReadData(UINT16* p_AIO)
{
	return VRET_ERROR;
}


VRET_VALUE VCDC_PMU_GetAIO_ReadReady(UINT8* p_ready)
{
	return VRET_ERROR;
}


VRET_VALUE VCDC_PMU_GetAIO_Active(void)
{
	return VRET_ERROR;
}


VRET_VALUE VPMU_Get7632AIO ( UINT16 *p_AIO ) //after 7632B add this new function;
{
	return VRET_ERROR;
}

void VPMU_Config7632AIO ( UINT8 Mode ) //after 7632B add this new function;
{
//	return VRET_ERROR;
}

void VPMU_SetPowerRstDelay ( UINT8 DelaySel )
{
//	return VRET_ERROR;
}

void VPMU_Set7632MMDC ( UINT8 Mode )
{
//	return VRET_ERROR;
}

VRET_VALUE VPMU_GetCoulombVD ( UINT8 *p_VD )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_7632Close_BATLOW( void )
{
	return VRET_ERROR;
}


UINT8 VPMU_CheckBatteryPlug ( void )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetCoulombVD_Active(void)
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetCoulombVD_WaitReady(UINT8* p_ready)
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetCoulombVD_ReadData(UINT8 *p_VD)
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetCoulombVD_Sleep(void)
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetVerInfo ( UINT8 *p_VerInfo )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_GetMCVer ( UINT8 *p_VerInfo )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_LineInDetect ( UINT8 *p_LineIn )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_IntInverseConfig ( UINT8 *p_InvConfig, UINT8 mode )
{
	return VRET_ERROR;
}

VRET_VALUE VPMU_IntMaskConfig ( UINT8 *p_MaskConfig, UINT8 mode )
{
    return VRET_ERROR;
}

BOOL VPMU_StatIsr_En ( void )
{
    return VRET_ERROR;
}

void VPMU_SetI2CLowSpeedFlag ( BOOL Flag )
{

}

void VPMU_Init ( void )
{

}
 
UINT8 VPMU_GetPowerState ( void )
{
    return g_VPMU_PowerState;
}

UINT8 VPMU_GetPowerOnReq ( void )
{
    return 0;
}

void VPMU_SetPMUEn ( BOOL IsEnable )
{
}

void VPMU_SetStateISR ( UINT8 State )
{
    g_ISR_PMUState = State;
}

UINT8 VPMU_GetStateISR ( void )
{
    return g_ISR_PMUState;
}
VRET_VALUE VPMU_GetChargerState ( UINT8 *p_State )
{
		return VRET_ERROR;
}

void VPMU_SetLDO5En ( BOOL IsEnable )
{

}

void VPMU_SetDCDC_CodecOn ( UINT8 Mode )
{

}

void VPMU_SetDigitalDCDC( DIGITALDCDC_VOL DCDC_Vol )
{

}

UINT8 VPMU_GetDigitalDCDC(void)
{
    return VRET_ERROR;
}


void VPMU_SetDCDCMode ( UINT8 Mode )
{
    
}
#endif


