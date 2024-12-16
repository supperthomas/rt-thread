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
	slave die host

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstddrv.h"
#include "vcoroutine.h"
#include "vdrv_pmu.h"

typedef struct tag_VSYS_PWR_CFG {
	UINT8 charge_terminate_voltage;
	UINT8 charge_current;
	UINT8 battery_low_threshold;
	UINT8 shutdown_threshold;
	UINT8 deadlock_reset_time;	//for 9611SE, MTRST_LOCK, press time for rest pwr.
	UINT8 ldo9_out_vol;			//for 7621A, MCP; for 7621SDC0, inductor;
	UINT8 pmustat_isr_mask;		//for 7621S+, will set Reg PMU_REG_INT_MASK.
	//for external charger, show charging or not.
	UINT8 charge_sel;	//0==select 9611 to charge(internal), 1==charge by external IC(external).
	UINT8 charge_ext_gpio;	//use external charger, check the charge state by gpio(connect to CHRG-pin of ext-IC)
	UINT8 charge_ext_level;	//for external charger; gpio value==charge_ext_level means charging state, or disconnect.
	//PWR_DCDC_CFG
	UINT8 mode;
	UINT8 output;
	
	UINT8 cdc_ver;//Select the type of current used Chip, 9611 or 9611S
	
	//AIF test(CDC_CTRL)
	UINT32 aif_cdc_ctrl;//[2:0]hpf_sel,[3]clkadc_sel,[7:4]sdm_sel,[15:8]ditherpow,[19:16]decim_gain_sel
	//for LDO5 debug pin when close[0] & MEN5 set[7:2](VA7621)
	UINT8 pmu_ldo5;
	UINT8 pwr_mode; //for 7621, pmu by LDO5(0) or DCDC(1)
	UINT8 pmu_set_dac; //the DCDC set if adc not open; or the LDO5 mode set DCDC when codec open
	UINT8 pmu_set_adc; //the DCDC set when adc open
    
	UINT8 deal_popupnois;//Optimization methods for Popup Noise, at switching to DAC or ADC.

} VSYS_PWR_CFG;

// charge_current 的值设置为0x06表示关闭7632B的对电池的充电功能
VSYS_PWR_CFG g_VSYS_PWR_Config = {0x02/*4.2v*/,  0x06/*0mA*/, 0x5A, 0x56, 0x02/*10s*/, 0x0, 0x0, 
								  0x0/*internal*/, 0x0/*GPIO0*/, 0x0/*Low*/, 
								  0x01/*PWM*/,  0x0C, 0x10, 0x10070917, 0x51, 0x0, 0x7, 0x7, 0x9};

VRET_VALUE VPMU_WriteReg ( UINT8 Reg, UINT8 Value )
{
    VRET_VALUE ret = 0;

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
    VRET_VALUE ret;

    ret = VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,Reg,p_Value,1);

    VPMU_WPRINTF("VPMU_ReadReg: Reg address: 0x%x, ReadValue: 0x%x\r\n", Reg, *p_Value);

    return ret;
}

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
	} else if ( ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC)/* && ( g_VCDC_Debug.pwr_mode == CDC_PWR_DCDC )*/ ) { //VA7632B
       VPMU_WriteReg ( PMU_REG763X_MVDCA, Voltage ); //set RF DC-DC;
	#else
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC) { //VA7632B
       VPMU_WriteReg ( PMU_REG763X_MVDC, Voltage );
	#endif
    } else {
        VPMU_WPRINTF("%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo());
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

void VSYS_PWR_Set(void)
{
	VSYS_PWR_CFG volatile *p_config = &g_VSYS_PWR_Config;
  UINT8 digital_voltage = 0;

	/* set DCDC mode and output voltage */
	VPMU_SetLDO5En(FALSE);//VPMU_DCDC_Enable(1);
	VPMU_SetDCDCMode(p_config->mode);
	VPMU_SetDCOutputVoltage(p_config->output);

	/* Set LDO9 and charge-pump output voltage */
	VPMU_SetMCP(p_config->ldo9_out_vol);

}
