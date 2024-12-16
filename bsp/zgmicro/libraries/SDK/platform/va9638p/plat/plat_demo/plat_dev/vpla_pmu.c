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
        pmu

Description:
        irq Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_pmu.h"
#include "va7632b_pmu.h"
#include "va7632b_cdc.h"
#include "vdrv_gpio.h"


extern VCDC_DEBUG g_VCDC_Debug;

volatile UINT8 g_VPMU_PowerState;


void VPMU_Init ( void )
{
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
    return va7632b_int_status_get(p_State);
}

void VPMU_SetBatteryLowLevel ( UINT8 MBV )
{
    va7632b_battery_low_voltage_threshold_set((va7632b_battery_low_voltage_threshold_t)MBV);
}

void VPMU_SetLDO5En ( BOOL IsEnable )
{
        if ( IsEnable ) { //open audio dcdc&LDO5
            va7632b_ldo5_enable();
        } else {
            va7632b_ldo5_disable();
        }
}

void VPMU_SetDCDC_CodecOn ( UINT8 Mode )
{
        if ( Mode == 2 ) { //open both adc&dac
            va7632b_dcdc1_voltage_set_legacy(g_VCDC_Debug.pmu_set_adc);
        } else {//VCODEC_HW_MODE_HIFIDAC, only open dac
            va7632b_dcdc1_voltage_set_legacy(g_VCDC_Debug.pmu_set_dac);
        }
}


//set RF DC-DC output voltage;
void VPMU_SetDCOutputVoltage ( UINT8 Voltage )
{
    va7632b_dcdc1_voltage_set(Voltage);
}

void VPMU_SetDigitalDCDC( DIGITALDCDC_VOL DCDC_Vol )
{
        va7632b_dcdc2_voltage_set(DCDC_Vol);
}

//20210803xy, for get the Digital DC-DC maybe revert;
UINT8 VPMU_GetDigitalDCDC(void)
{
        UINT8 Value = DIG_DCDC_0_0V;
        va7632b_dcdc2_voltage_get(&Value);
        return Value;
}

void VPMU_SetCharge ( UINT8 Voltage, UINT8 Current )
{
        va7632b_charger_termination_voltage_set((va7632b_charging_termination_voltage_t)Voltage); 
        va7632b_charger_charging_current_set((va7632b_charging_current_t)(Current << 2));
}

void VPMU_SetMCP ( UINT8 Mcp )
{
    va7632b_dcdc1_reverse_current_threshold_t dcdc1_current;
    va7632b_dcdc2_reverse_current_threshold_t dcdc2_current;
    dcdc1_current = Mcp& 0x07;
    dcdc2_current = (Mcp >> 3) & 0x07;
    va7632b_dcdc1_reverse_current_threshold_set(dcdc1_current);
    va7632b_dcdc2_reverse_current_threshold_set(dcdc1_current);    
} 

void VPMU_SetDCDCMode ( UINT8 Mode )
{
    va7632b_dcdc1_power_mode_set((va7632b_dcdc_mode_t) Mode);
    va7632b_dcdc2_power_mode_set((va7632b_dcdc_mode_t) Mode);
}

void VPMU_SetPowerRstDelay ( UINT8 DelaySel )
{
    va7632b_deadlock_reset_delay_time_set((va7632b_deadlock_reset_delay_time_t)DelaySel);
}

VRET_VALUE VPMU_GetCoulombVD ( UINT8 *p_VD )
{
    return va7632b_battery_voltage_get(p_VD);
}

VRET_VALUE VPMU_GetVerInfo ( UINT8 *p_VerInfo )
{
    return va7632b_version_get(p_VerInfo);
}

uint8_t VPMU_IntStatusChangeCheck (void )
{
    return va7632b_int_status_change_check();
}

int VPMU_IntMaskConfig ( UINT8 *p_MaskConfig, UINT8 mode )
{

        if ( mode == 0 ) { //read
                //pass
                return 0;
        } else { //write
                return va7632b_int_mask_set(*p_MaskConfig);
        }
}

void VPMU_SetChargingDisable ( UINT8 Ctrl )
{
        if(Ctrl){
                va7632b_charger_charging_current_set(VA7632B_CHARGE_CURRENT_0mA);
        } else {
                va7632b_charger_charging_current_set(VA7632B_CHARGE_CURRENT_100mA);
        }
}


void VPMU_Set7621PMUEn ( UINT8 Enable )
{
        if ( Enable ) { //set PMU ON
                va7632b_pmu_enable(VA7632B_PMU_EN_SEL_PMU_ON_REGISTER);
        } else {//set PMU OFF
                va7632b_pmu_disable();
        }
}

void VPMU_Config7632AIO ( UINT8 Mode ) //after 7632B add this new function;
{
        if ( Mode == 0 ) { //detect AIO_ADC(NTC)
        va7632b_aio_adc_detect_target_set(VA7632B_ADC_DETECT_AIO_VOLT);
        } else if ( Mode == 1 ) { //detect VBAT
        va7632b_aio_adc_detect_target_set(0x20);
        } else if ( Mode == 2 ) { //detect VCHG
        va7632b_aio_adc_detect_target_set(VA7632B_ADC_DETECT_VCHG);
        } else {//not define mode.
        ;//
        }
}

// this had delay so can not call by interrupts.
VRET_VALUE VPMU_Get7632AIO ( UINT16 *p_AIO ) //after 7632B add this new function;
{
    return va7632b_aio_adc_output_data_get(VA7632B_ADC_DETECT_AIO_VOLT,p_AIO);
}

void VPMU_Set_Bypass (UINT8 Enable)
{
        if ( Enable ) {
            va7632b_power_supply_config(VA7632B_POWER_MODE0);
        } else {
            va7632b_power_supply_config(VA7632B_POWER_MODE0);
        }
}
