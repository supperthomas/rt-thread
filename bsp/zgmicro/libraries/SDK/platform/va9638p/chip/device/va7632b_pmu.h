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
    2010.05.08           chenjingen            initial version
    2013.11.16           Xiongyan              update for 9611SE0, add MTRST_LOCK, VER_SEL.
    2013.11.19           Xiongyan              update for VER_REG_SEL(VER_SEL_LowHigh and VER_SEL_OldNew) by new VA9611S_E0 Spec.
    2014.02.10           Xiongyan              update for 7621A, add MCP register.
        2014.02.18      xiongyan            add 7621SA0 and 7621GA0
        2019.06.05      xiongyan            add VA7632B_A0
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VDRV_VA7632B_PMU_H__
#define __VDRV_VA7632B_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VRET_DRV_PMU_STATE_ERROR 						0x430
#define VRET_DRV_PMU_TERMB_TIMEOUT 						0x431

/* power state definitions */
#define PMU_POWER_OFF               0
#define PMU_POWER_ON                1

enum
{
    VA7632B_CHIPID_E3 = 0x0C,
    VA7632B_CHIPID_F3 = 0x0D,
    VA7632B_CHIPID_E5 = 0x11,
    VA7632B_CHIPID_F5 = 0x12,
};

enum
{
    VA7632B_VERSION_00 = 0,
    VA7632B_VERSION_01 = 1,
    VA7632B_VERSION_02 = 2,
    VA7632B_VERSION_03 = 3,
};

typedef enum
{
    VA7632B_POWER_MODE0 = 0x00, // LDOB、LDO1、LDO5 work when charger is disconnected; LDO3/LDO6、LDO4、LDO5 work when charger is connected. 当连接充电器时，使用充电器电路给系统供电
    VA7632B_POWER_MODE1 = 0x04, // LDOB、LDO1、LDO5 work when charger is disconnected; LDOB、LDO1、LDO5      work when charger is connected. 当连接充电器时，使用电池电路给系统供电(LDOB)
    VA7632B_POWER_MODE2 = 0x40, // Buck、LDO1、LDO5 work when charger is disconnected; LDO3/LDO6、LDO4、LDO5 work when charger is connected. 当连接充电器时，使用充电器电路给系统供电
    VA7632B_POWER_MODE3 = 0x44, // Buck、LDO1、LDO5 work when charger is disconnected; Buck、LDO1、LDO5      work when charger is connected. 当连接充电器时，使用电池电路给系统供电(BUCK)
}va7632b_power_config_t;
typedef enum
{
    VA7632B_PWM_FREQUENCY_2_5MHz = 0x00,
    VA7632B_PWM_FREQUENCY_1_5MHz = 0x80,
    VA7632B_PWM_FREQUENCY_1_0MHz = 0xA0,
}va7632b_dcdc_pwm_frequency_t;

typedef enum
{
    VA7632B_PMU_EN_SEL_PMU_ON_REGISTER = 0x00,  // 该模式下，需要I2C正常工作
    VA7632B_PMU_EN_SEL_PMU_EN_PIN      = 0x01,  // 默认值，该模式下，不依赖于I2C的正常工作
}va7632b_pmu_en_sel_t;
typedef enum
{
    VA7632B_RESET_MODE_POR      = (0x00 << 2),
    VA7632B_RESET_MODE_SHUTDOWN = (0x01 << 2),
}va7632b_pmu_reset_mode_t;

typedef enum
{
    VA7632B_DCDC_MODE_AUTO = 0x00,
    VA7632B_DCDC_MODE_PWM  = 0x01,
    VA7632B_DCDC_MODE_PFM  = 0x03,
} va7632b_dcdc_mode_t;

typedef enum
{
    VA7632B_DCDC1_0_0V  , //va7632b DC-DC1 disable
    VA7632B_DCDC1_1_1V  , //only for 7632b f3 version
    VA7632B_DCDC1_1_125V, //only for 7632b f3 version             
    VA7632B_DCDC1_1_15V , //only for 7632b f3 version
    VA7632B_DCDC1_1_163V, //only for 7632b f3 version
    VA7632B_DCDC1_1_175V, //only for 7632b f3 version
    VA7632B_DCDC1_1_188V, //only for 7632b f3 version
    VA7632B_DCDC1_1_2V  , //only for 7632b f3 version
    VA7632B_DCDC1_1_216V, //only for 7632b f3 version             
    VA7632B_DCDC1_1_225V,              
    VA7632B_DCDC1_1_25V ,              
    VA7632B_DCDC1_1_3V  ,              
    VA7632B_DCDC1_1_35V ,              
    VA7632B_DCDC1_1_4V  ,              
    VA7632B_DCDC1_1_5V  ,              
    VA7632B_DCDC1_1_7V  , 
    VA7632B_DCDC1_1_8V  , //only for 7632b f5 version 
    VA7632B_DCDC1_1_9V  , //only for 7632b f3 version 
    VA7632B_DCDC1_LIMIT ,             
}va7632b_dcdc1_voltage_t;

typedef enum
{
    VA7632B_PFM77mA_PWM46mA = (0x00 << 4),
    VA7632B_PFM109mA_PWM46mA = (0x01 << 4),
}va7632b_dcdc1_over_current_threshold_t;


typedef enum
{
    VA7632B_4_7uH_14_0mA = (0x00 << 2),
    VA7632B_4_7uH_20_5mA = (0x04 << 2),
    VA7632B_4_7uH_26_3mA = (0x02 << 2),
    VA7632B_2_2uH_20_5mA = (0x06 << 2),
    VA7632B_2_2uH_25_5mA = (0x01 << 2),
    VA7632B_2_2uH_29_8mA = (0x05 << 2),
} va7632b_dcdc1_reverse_current_threshold_t;


typedef enum
{
    VA7632B_DCDC2_0_0V   , //va7632b DC-DC2 disable.
    VA7632B_DCDC2_0_85V  ,
    VA7632B_DCDC2_0_9V   ,
    VA7632B_DCDC2_0_95V  ,
    VA7632B_DCDC2_1_0V   ,
    VA7632B_DCDC2_1_1V   ,
    VA7632B_DCDC2_1_2V   ,
    VA7632B_DCDC2_1_25V  ,
    VA7632B_DCDC2_1_275V ,
    VA7632B_DCDC2_1_3V   ,
    VA7632B_DCDC2_1_325V ,
    VA7632B_DCDC2_1_35V  ,
    VA7632B_DCDC2_1_4V   ,
    VA7632B_DCDC2_1_45V  ,
    VA7632B_DCDC2_1_5V   ,
    VA7632B_DCDC2_1_6V   , //only for 7632b f3 version
    VA7632B_DCDC2_1_7V   , //only for 7632b f3 version
    VA7632B_DCDC2_LIMIT  ,
} va7632b_dcdc2_voltage_t;

typedef enum
{
    VA7632B_PFM67mA_PWM42mA = (0x00 << 3),
    VA7632B_PFM115mA_PWM42mA = (0x01 << 3),
}va7632b_dcdc2_over_current_threshold_t;

typedef enum
{
    VA7632B_4_7uH_14_2mA = (0x00 << 5),
    VA7632B_4_7uH_20_6mA = (0x04 << 5),
    VA7632B_4_7uH_26_6mA = (0x02 << 5),
    VA7632B_2_2uH_21_1mA = (0x06 << 5),
    VA7632B_2_2uH_26_4mA = (0x01 << 5),
    VA7632B_2_2uH_31_5mA = (0x05 << 5),
    VA7632B_1_0uH_28_9mA = (0x03 << 5),
    VA7632B_1_0uH_35_7mA = (0x07 << 5),
} va7632b_dcdc2_reverse_current_threshold_t;


typedef enum 
{
    VA7632B_LDO2_0_9V  = (0x00 << 4),
    VA7632B_LDO2_0_95V = (0x01 << 4),
    VA7632B_LDO2_1_0V  = (0x02 << 4),
    VA7632B_LDO2_1_1V  = (0x03 << 4),
    VA7632B_LDO2_1_15V = (0x04 << 4),
    VA7632B_LDO2_1_2V  = (0x05 << 4),
    VA7632B_LDO2_USE_DCDC2_OUTPUT,
}va7632b_ldo2_voltage_t;
typedef enum
{
    VA7632B_LDO5_MODE_NORMAL    = (0x00 << 1),
    VA7632B_LDO5_MODE_LOW_POWER = (0x01 << 1),
}va7632b_ldo5_mode_t;

typedef enum
{
    VA7632B_LDO5_0_0V = 0xFF,  // disable LDO5, output 0V
    VA7632B_LDO5_2_0V = 0x00,
    VA7632B_LDO5_1_9V = 0x01,
    VA7632B_LDO5_1_8V = 0x02,  // default value in f3 or f5
    VA7632B_LDO5_2_1V = 0x03,  // only vailid in va7632b f3
} va7632b_ldo5_voltage_t;

typedef enum
{
    VA7632B_BATTERY_LOW_VOLT_0_0V  = 0xFF,   // disable battery low voltage threshold
    VA7632B_BATTERY_LOW_VOLT_2_70V = 0x00,
    VA7632B_BATTERY_LOW_VOLT_2_75V = 0x01,
    VA7632B_BATTERY_LOW_VOLT_2_80V = 0x02,
    VA7632B_BATTERY_LOW_VOLT_2_85V = 0x03,
    VA7632B_BATTERY_LOW_VOLT_2_90V = 0x04,
    VA7632B_BATTERY_LOW_VOLT_2_95V = 0x05,
    VA7632B_BATTERY_LOW_VOLT_3_00V = 0x06,
    VA7632B_BATTERY_LOW_VOLT_3_05V = 0x07,
    VA7632B_BATTERY_LOW_VOLT_3_10V = 0x08,
    VA7632B_BATTERY_LOW_VOLT_3_15V = 0x09,
    VA7632B_BATTERY_LOW_VOLT_3_20V = 0x0A,
    VA7632B_BATTERY_LOW_VOLT_3_25V = 0x0B,
    VA7632B_BATTERY_LOW_VOLT_3_30V = 0x0C,
    VA7632B_BATTERY_LOW_VOLT_3_35V = 0x0D,
    VA7632B_BATTERY_LOW_VOLT_3_40V = 0x0E, //(default)
    VA7632B_BATTERY_LOW_VOLT_3_45V = 0x0F
}va7632b_battery_low_voltage_threshold_t;

typedef enum
{
    VA7632B_CHARGE_CURRENT_0mA   = 0xFF,   // stop charger charging current.
    VA7632B_CHARGE_CURRENT_10mA  = (0x00 << 2),
    VA7632B_CHARGE_CURRENT_20mA  = (0x01 << 2),
    VA7632B_CHARGE_CURRENT_30mA  = (0x02 << 2),
    VA7632B_CHARGE_CURRENT_40mA  = (0x03 << 2),
    VA7632B_CHARGE_CURRENT_50mA  = (0x04 << 2),
    VA7632B_CHARGE_CURRENT_60mA  = (0x05 << 2),
    VA7632B_CHARGE_CURRENT_70mA  = (0x06 << 2),
    VA7632B_CHARGE_CURRENT_80mA  = (0x07 << 2),
    VA7632B_CHARGE_CURRENT_90mA  = (0x08 << 2),
    VA7632B_CHARGE_CURRENT_100mA = (0x09 << 2), //(default)
    VA7632B_CHARGE_CURRENT_130mA = (0x0A << 2),
    VA7632B_CHARGE_CURRENT_160mA = (0x0B << 2),
    VA7632B_CHARGE_CURRENT_180mA = (0x0C << 2),
    VA7632B_CHARGE_CURRENT_200mA = (0x0D << 2),
    VA7632B_CHARGE_CURRENT_250mA = (0x0E << 2),
    VA7632B_CHARGE_CURRENT_300mA = (0x0F << 2),
}va7632b_charging_current_t;

typedef enum
{
    VA7632B_CHARGE_TERM_VOLT_4_35V = 0x00,
    VA7632B_CHARGE_TERM_VOLT_4_1V  = 0x01,
    VA7632B_CHARGE_TERM_VOLT_4_2V  = 0x02, //(default)
    VA7632B_CHARGE_TERM_VOLT_4_25V = 0x03,
}va7632b_charging_termination_voltage_t;

typedef enum
{
    VA7632B_CHARGE_TERM_CURRENT_PERCENT_10 = 0x00,
    VA7632B_CHARGE_TERM_CURRENT_PERCENT_05 = 0x01,
}va7632b_charging_termination_current_t;

typedef enum
{
    VA7632B_CHARGER_OVP_DISABLE = 0xFF,   // disable charger over voltage protection voltage
    VA7632B_CHARGER_OVP_6_5V    = 0x00,
    VA7632B_CHARGER_OVP_7_2V    = 0x01,
    VA7632B_CHARGER_OVP_8_0V    = 0x02, //(default)
}va7632b_charger_ovp_voltage_t;

typedef enum
{
    VA7632B_CHARGING_TIME_UNLIMITED = 0xFF, // disable charging time
    VA7632B_CHARGING_TIME_1_8h = 0x00,
    VA7632B_CHARGING_TIME_3_6h = 0x01,
}va7632b_charging_time_t;

typedef enum
{
    VA7632B_DELAY_7s  = 0x00,
    VA7632B_DELAY_15s = 0x01,
    VA7632B_DELAY_10s = 0x02,
    VA7632B_DELAY_3s  = 0x03,
}va7632b_deadlock_reset_delay_time_t;

typedef enum
{
    VA7632B_TH140_TL100_HY40 = 0x00, //(default)
    VA7632B_TH120_TL90_HY30  = 0x01,
    VA7632B_TH100_TL80_HY20  = 0x02,
    VA7632B_TH80_L60_HY20    = 0x03,
}va7632b_otp_threshold_t;  //当温度上升至TH以上时，OTP触发，在OT触发后，等温度下降至TL以下时，OTP解除。Hysteresis代表迟滞，就是TH与TL的差值

typedef enum
{
    VA7632B_ADC_DETECT_VCHG = 0x00,
    VA7632B_ADC_DETECT_VBAT = 0x01,
    VA7632B_ADC_DETECT_AIO_VOLT = 0x02,
}va7632b_adc_detect_target_t;

// 电源管理配置
///
/// @brief va7632b select LDO to work, LDO3 replaces BUCKRF when charger is connected; and LDO6 replaces DCDCA when charger is connected.
/// 
/// @param config the ldo switch
///             If Men5[6]="0" & Men5[2]="0", LDOB、LDO1、LDO5 work when charger is disconnected; LDO3/LDO6、LDO4、LDO5 work when charger is connected. 
///             If Men5[6]="0" & Men5[2]="1", LDOB、LDO1、LDO5 work when charger is disconnected; LDOB、LDO1、LDO5 work when charger is connected.
///             If Men5[6]="1" & Men5[2]="0", Buck、LDO1、LDO5 work when charger is disconnected; LDO3/LDO6、LDO4、LDO5 work when charger is connected.
///             If Men5[6]="1" & Men5[2]="1", Buck、LDO1、LDO5 work when charger is disconnected; Buck、LDO1、LDO5 work when charger is connected.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_power_supply_config(va7632b_power_config_t config);

///
/// @brief check power key is pressed or not.
/// 
/// @return int 1 pressed, 0 not pressed;
///
int va7632b_pmu_is_power_key_pressed(void);
///
/// @brief Register to set PMU ON/OFF.
/// 
/// @return int 1 enable, 0 disable;
///
int va7632b_pmu_is_enable(void);
///
/// @brief Register to set PMU ON/OFF.
/// 
/// @param sel Register to set mode of pmu_en_out. 
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_pmu_enable(va7632b_pmu_en_sel_t sel);
///
/// @brief Register to set PMU ON/OFF.
/// 
/// @param sel Register to set mode of pmu_en_out.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_pmu_disable(void);

//reset
///
/// @brief Reset_Mode: it is used to set reset mode or shutdown mode when PMU_EN goes low. 
///        If Reset_Mode=”1”, VA7633 will generate a reset pulse (POR signal) to reset SOC chip when PMU_EN goes low. 
///        The low-level time of the reset pulse is same as the low-level time of power-on reset. 
///        If Reset_Mode=”0”, VA7633 will shut down the system and all power supplies when PMU_EN goes low.
/// 
/// @param mode VA7632B_RESET_PULS = (0x00 << 2),
///             VA7632B_SHUT_DOWN = (0x01 << 2),
///           
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_pmu_reset_mode(va7632b_pmu_reset_mode_t mode);
///
/// @brief Deadlock reset delay time control register.
/// 
/// @param time 2'b00    7s (default)
///             2'b01    15s
///             2'b10    10s
///             2'b11    3s
///
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_deadlock_reset_delay_time_set(va7632b_deadlock_reset_delay_time_t time);

///
/// @brief setting pwm frequency for buckrf&dcdca.
/// 
/// @param frequency destination frequency to change; input value enumeration.
///                 VA7632B_PWM_FREQUENCY_2_5MHz  0x00,
///                 VA7632B_PWM_FREQUENCY_2_0MHz  0x20,
///                 VA7632B_PWM_FREQUENCY_1_5MHz  0x80,
///                 VA7632B_PWM_FREQUENCY_1_0MHz  0xA0,
/// @return int operation result： 0 successful, -1 failed.
///
int VA7632b_dcdc_power_pwm_frequency_set(va7632b_dcdc_pwm_frequency_t frequency);

// 过温保护
///
/// @brief Two bits for OTP (Over-Temperature Protection) threshold programming.过温保护阈值
/// 
/// @param val HY(hysteresis)
///            VA7632B_TH140_TL100_HY40 = 0x00,//(default)
///            VA7632B_TH120_TL90_HY30  = 0x01,
///            VA7632B_TH100_TL80_HY20  = 0x02,
///            VA7632B_TH80_L60_HY20    = 0x03,
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_over_temperature_threshold_set(va7632b_otp_threshold_t val);


//DCDC
///
/// @brief va7632b set dcdc1 mode
/// 
/// @param mode VA7632B_DCDC_MODE_AUTO = 0x00,
///             VA7632B_DCDC_MODE_PWM  = 0x01,
///             VA7632B_DCDC_MODE_PFM  = 0x03,
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_power_mode_set(va7632b_dcdc_mode_t mode);
///
/// @brief Digital DC-DC1 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_voltage_set(va7632b_dcdc1_voltage_t volt);
///
/// @brief 调节DCDCA的OCP_PFM阈值电流.
/// 
/// @param cur setting for OCP_PFM阈值电流.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_over_current_threshold_set(va7632b_dcdc1_over_current_threshold_t cur);
///
/// @brief setting for DCDCA RCP current.
/// 
/// @param cur setting for DCDC1 RCP current.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_reverse_current_threshold_set(va7632b_dcdc1_reverse_current_threshold_t cur);
///
/// @brief va7632b set dcdc2 mode
/// 
/// @param mode VA7632B_DCDC_MODE_AUTO = 0x00,
///             VA7632B_DCDC_MODE_PWM  = 0x01,
///             VA7632B_DCDC_MODE_PFM  = 0x03,
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_power_mode_set(va7632b_dcdc_mode_t mode);
///
/// @brief RF DC-DC output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_voltage_set(va7632b_dcdc2_voltage_t volt);


int va7632b_dcdc2_voltage_get(va7632b_dcdc2_voltage_t *volt);

///
/// @brief 调节BUCKRF的OCP_PFM阈值电流。
/// 
/// @param cur setting for OCP_PFM阈值电流.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_over_current_threshold_set(va7632b_dcdc2_over_current_threshold_t cur);
///
/// @brief setting for BUCKRF RCP current.
/// 
/// @param cur setting for DCDC2 RCP current.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_reverse_current_threshold_set(va7632b_dcdc2_reverse_current_threshold_t cur);
//LDO 相关操作
///
/// @brief ldo2 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo2_voltage_set(va7632b_ldo2_voltage_t volt);
///
/// @brief va7632b LDO5 operation mode control:
/// 
/// @param mode the operation mode select
///             Men5[1]=1'b1 && Men5[0]=1'b1 : LDO5 enter low power mode;
///             Men5[1]=1'b0 && Men5[0]=1'b1 : LDO5 stays at normal mode;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo5_mode_set(va7632b_ldo5_mode_t mode);
///
/// @brief ldo5 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo5_voltage_set(va7632b_ldo5_voltage_t volt);

// charger 
///
/// @brief [5:2] 4-bit setting for CC mode charging current.充电电流设置
/// 
/// @param cur destination current(mA) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_charging_current_set(va7632b_charging_current_t cur);
///
/// @brief [1:0] Charging termination voltage control register.充电截止电压设置
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_termination_voltage_set(va7632b_charging_termination_voltage_t volt);
///
/// @brief 充电截止电流设置
/// 
/// @param cur destination current(mA) to change;
///        Mit=“0”, charging termination current is set to 10%*ICH; 
///        Mit=“1”, charging termination current is set to 5%*ICH.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_termination_current_set(va7632b_charging_termination_current_t cur);
///
/// @brief charger OVP voltage setting.
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_ovp_voltage_set(va7632b_charger_ovp_voltage_t volt);
///
/// @brief Setting for charging time.充电时间设置
///        Mcp[1]=0, charging time=1.8h.
///        Mcp[1]=1, charging time=3.6h.
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_charging_time_set(va7632b_charging_time_t time);

//battery
// VOD: voltage over detect 电池的低电压检测
///
/// @brief  Set Battery Low Detection threshold. When Battery voltage is lower than this threshold, BAT_Low will become high.达到阈值会产生中断，给到9655
/// 
/// @param volt destination voltage(hex) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_battery_low_voltage_threshold_set(va7632b_battery_low_voltage_threshold_t volt);
// 电压检测模块，检测电池电压，使用库仑计，实现
///
/// @brief Coulomb Meter output data, which demonstrate the battery voltage.获取电池电压
/// 
/// @param val The storage address to get the value
///        
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_battery_voltage_get(uint8_t *val);

// AIO_ADC
///
/// @brief AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
/// 
/// @param val The storage address to get the value
///             1: data ready
///             0: data not ready
///        
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_aio_adc_output_data_get(va7632b_adc_detect_target_t target, uint16_t *data);

// Interrupt, 中断
///
/// @brief [5]: KeyON 1'b0,()KeyON 1b0
///        [4]: BAT_Low 1'bx,When Battery voltage is lower than this threshold, BAT_Low will become high
///        [3]: Pg 1'bx,Indicating if charger input voltage is higher than battery voltage: 
///             1 means charger input voltage is high enough for charging; 
///             0 means charger input voltage is too low
///        [2]: Stat 1'bx,Charging flag: 1 indicating charging; 0 indicating no charging
///        [1]: termb_inv 1'bx,VD4~VD0 ready signal, active high.1'b0: VD4~VD0 is not ready; 1'b1: VD4~VD0 is ready to be read by 9613i. 
///        [0]: CDC_LINE_DET 1'bx,line in detect; 0=detect, 1=no detect
/// 
/// @param status Interrupt status value
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_int_status_get(uint8_t *status);
///
/// @brief 查询中断状态寄存器哪些bits发生改变
/// 
/// @return uint8_t 变化了的那些bits值
///
uint8_t va7632b_int_status_change_check(void);
///
/// @brief [5]: INT_MASK_KeyON 1'b1,Close the interrupt function from register "KeyON", active high.
///        [4]: INT_MASK_BAT_Low 1'b1,Close the interrupt function from register "BAT_Low", active high.
///        [3]: INT_MASK_Pg 1'b1,Close the interrupt function from register "Pg", active high.
///        [2]: INT_MASK_Stat 1'b1,Close the interrupt function from register "Stat", active high.
///        [1]: INT_MASK_termb_inv 1'b1,Close the interrupt function from register "termb_inv", active high.
///        [0]: INT_MASK_LINE_DET 1'b1,Close the interrupt function from register "CDC_LINE_DET", active high.
/// 
/// @param mask_val Interrupt mask value
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_int_mask_set(uint8_t mask_val);
///
/// @brief [5]: INT_MASK_KeyON 1'b1,Close the interrupt function from register "KeyON", active high.
///        [4]: INT_MASK_BAT_Low 1'b1,Close the interrupt function from register "BAT_Low", active high.
///        [3]: INT_MASK_Pg 1'b1,Close the interrupt function from register "Pg", active high.
///        [2]: INT_MASK_Stat 1'b1,Close the interrupt function from register "Stat", active high.
///        [1]: INT_MASK_termb_inv 1'b1,Close the interrupt function from register "termb_inv", active high.
///        [0]: INT_MASK_LINE_DET 1'b1,Close the interrupt function from register "CDC_LINE_DET", active high.
/// 
/// @param unmask_val Interrupt mask value
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_int_unmask_set(uint8_t unmask_val);
//版本, ID
///
/// @brief va7632b get int version indicator
/// 
/// @param status Version indicator:
///               0=low-end version,
///               1=high-end version.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_version_get(uint8_t *version);
///
/// @brief get va7632b chip id 
/// 
/// @param chipid version for metal change:
///               VA7632B_E5: 8'b0001_0001
///               VA7632B_F5: 8'b0001_0010
///               VA7632B_E3：8'b0000_1100
///               VA7632B_F3：8'b0000_1101
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_chipid_get(uint8_t *chipid);


int va7632b_test(void);


int va7632b_dcdc1_voltage_set_legacy(uint8_t reg_value);

int va7632b_ldo5_enable(void);
int va7632b_ldo5_disable(void);
int va7632b_aio_adc_detect_target_set(va7632b_adc_detect_target_t target);

#ifdef __cplusplus
}
#endif

#endif  /* __VDRV_VA7632B_PMU_H__ */
