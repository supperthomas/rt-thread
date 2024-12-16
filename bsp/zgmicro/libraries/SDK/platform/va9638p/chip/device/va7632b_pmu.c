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
    2013.11.19           Xiongyan              update about va7632b_version_get, get more 9611 info(VER_SEL_LowHigh and VER_SEL_OldNew).
    2014.02.10           Xiongyan              update for 7621A, add MCP register.
    2014.02.18           xiongyan              add 7621SA0 and 7621GA0
    2019.06.05           xiongyan              add VA7632B_A0
    2022.04.15           zhanghaibiao          code refactoring
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "va7632b_reg.h"
#include "va7632b_pmu.h"
#include "vdrv_i2c.h"
#include "vdrv_gpio.h"

enum
{
    VA7632B_VOLTAGE_INVALID = 0xAA,  
    VA7632B_VOLTAGE_NONE    = 0xFF,

    VA7632B_F3_DCDC1_1_1V   = 0x08,                     VA7632B_F5_DCDC1_1_1V   = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_125V = 0x09,                     VA7632B_F5_DCDC1_1_125V = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_15V  = 0x0A,                     VA7632B_F5_DCDC1_1_15V  = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_163V = 0x0B,                     VA7632B_F5_DCDC1_1_163V = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_175V = 0x0C,                     VA7632B_F5_DCDC1_1_175V = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_188V = 0x0D,                     VA7632B_F5_DCDC1_1_188V = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_2V   = 0x0E, /*default*/         VA7632B_F5_DCDC1_1_2V   = VA7632B_VOLTAGE_INVALID,
    VA7632B_F3_DCDC1_1_216V = 0x0F,                     VA7632B_F5_DCDC1_1_216V = VA7632B_VOLTAGE_INVALID,               
    VA7632B_F3_DCDC1_1_225V = 0x00,                     VA7632B_F5_DCDC1_1_225V = 0x0B,
    VA7632B_F3_DCDC1_1_25V  = 0x01,                     VA7632B_F5_DCDC1_1_25V  = 0x0A,
    VA7632B_F3_DCDC1_1_3V   = 0x02,                     VA7632B_F5_DCDC1_1_3V   = 0x09,
    VA7632B_F3_DCDC1_1_35V  = 0x03,                     VA7632B_F5_DCDC1_1_35V  = 0x08,
    VA7632B_F3_DCDC1_1_4V   = 0x04,                     VA7632B_F5_DCDC1_1_4V   = 0x0F,
    VA7632B_F3_DCDC1_1_5V   = 0x05,                     VA7632B_F5_DCDC1_1_5V   = 0x0E,/*default*/
    VA7632B_F3_DCDC1_1_7V   = 0x06,                     VA7632B_F5_DCDC1_1_7V   = 0x0D,
    VA7632B_F3_DCDC1_1_8V   = VA7632B_VOLTAGE_INVALID,  VA7632B_F5_DCDC1_1_8V   = 0x0C,
    VA7632B_F3_DCDC1_1_9V   = 0x07,                     VA7632B_F5_DCDC1_1_9V   = VA7632B_VOLTAGE_INVALID,   

    VA7632B_F3_DCDC2_0_85V  = 0x00,                     VA7632B_F5_DCDC2_0_85V  = 0x08,                           
    VA7632B_F3_DCDC2_0_9V   = 0x01,                     VA7632B_F5_DCDC2_0_9V   = 0x09,                           
    VA7632B_F3_DCDC2_0_95V  = 0x02,                     VA7632B_F5_DCDC2_0_95V  = 0x0A,                           
    VA7632B_F3_DCDC2_1_0V   = 0x03,                     VA7632B_F5_DCDC2_1_0V   = 0x0B,                                            
    VA7632B_F3_DCDC2_1_1V   = 0x04,                     VA7632B_F5_DCDC2_1_1V   = 0x0C,                 
    VA7632B_F3_DCDC2_1_2V   = 0x05,                     VA7632B_F5_DCDC2_1_2V   = 0x0D, /*default*/                         
    VA7632B_F3_DCDC2_1_25V  = 0x06,                     VA7632B_F5_DCDC2_1_25V  = 0x0E,                           
    VA7632B_F3_DCDC2_1_275V = 0x07,                     VA7632B_F5_DCDC2_1_275V = 0x0F,                           
    VA7632B_F3_DCDC2_1_3V   = 0x08,                     VA7632B_F5_DCDC2_1_3V   = 0x06,                   
    VA7632B_F3_DCDC2_1_325V = 0x09,                     VA7632B_F5_DCDC2_1_325V = 0x01,                         
    VA7632B_F3_DCDC2_1_35V  = 0x0A,                     VA7632B_F5_DCDC2_1_35V  = 0x02,                         
    VA7632B_F3_DCDC2_1_4V   = 0x0B,                     VA7632B_F5_DCDC2_1_4V   = 0x07,                           
    VA7632B_F3_DCDC2_1_45V  = 0x0C,                     VA7632B_F5_DCDC2_1_45V  = 0x04,                 
    VA7632B_F3_DCDC2_1_5V   = 0x0D, /*default*/         VA7632B_F5_DCDC2_1_5V   = 0x05,                          
    VA7632B_F3_DCDC2_1_6V   = 0x0E,                     VA7632B_F5_DCDC2_1_6V   = VA7632B_VOLTAGE_INVALID,                         
    VA7632B_F3_DCDC2_1_7V   = 0x0F,                     VA7632B_F5_DCDC2_1_7V   = VA7632B_VOLTAGE_INVALID,

   
};
static uint8_t va7632b_f3_dcdc1_voltage_map[VA7632B_DCDC1_LIMIT] = { 
    VA7632B_VOLTAGE_NONE   , //va7632b DC-DC2 disable.
    VA7632B_F3_DCDC1_1_1V  , //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_125V, //only for 7632b f3 version            
    VA7632B_F3_DCDC1_1_15V , //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_163V, //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_175V, //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_188V, //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_2V  , //only for 7632b f3 version
    VA7632B_F3_DCDC1_1_216V, //only for 7632b f3 version            
    VA7632B_F3_DCDC1_1_225V,              
    VA7632B_F3_DCDC1_1_25V ,              
    VA7632B_F3_DCDC1_1_3V  ,              
    VA7632B_F3_DCDC1_1_35V ,              
    VA7632B_F3_DCDC1_1_4V  ,              
    VA7632B_F3_DCDC1_1_5V  ,              
    VA7632B_F3_DCDC1_1_7V  , 
    VA7632B_F3_DCDC1_1_8V  , //only for 7632b f5 version 
    VA7632B_F3_DCDC1_1_9V  , //only for 7632b f3 version 
};
static uint8_t va7632b_f5_dcdc1_voltage_map[VA7632B_DCDC1_LIMIT] = { 
    VA7632B_VOLTAGE_NONE   , //va7632b DC-DC2 disable.
    VA7632B_F5_DCDC1_1_1V  , //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_125V, //only for 7632b f3 version            
    VA7632B_F5_DCDC1_1_15V , //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_163V, //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_175V, //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_188V, //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_2V  , //only for 7632b f3 version
    VA7632B_F5_DCDC1_1_216V, //only for 7632b f3 version            
    VA7632B_F5_DCDC1_1_225V,              
    VA7632B_F5_DCDC1_1_25V ,              
    VA7632B_F5_DCDC1_1_3V  ,              
    VA7632B_F5_DCDC1_1_35V ,              
    VA7632B_F5_DCDC1_1_4V  ,              
    VA7632B_F5_DCDC1_1_5V  ,              
    VA7632B_F5_DCDC1_1_7V  , 
    VA7632B_F5_DCDC1_1_8V  , //only for 7632b f5 version 
    VA7632B_F5_DCDC1_1_9V  , //only for 7632b f3 version 
};

static int va7632b_f3_dcdc2_voltage_map[VA7632B_DCDC2_LIMIT] = 
{
    VA7632B_VOLTAGE_NONE    , 
    VA7632B_F3_DCDC2_0_85V  ,
    VA7632B_F3_DCDC2_0_9V   ,
    VA7632B_F3_DCDC2_0_95V  ,
    VA7632B_F3_DCDC2_1_0V   ,
    VA7632B_F3_DCDC2_1_1V   ,
    VA7632B_F3_DCDC2_1_2V   ,
    VA7632B_F3_DCDC2_1_25V  ,
    VA7632B_F3_DCDC2_1_275V ,
    VA7632B_F3_DCDC2_1_3V   ,
    VA7632B_F3_DCDC2_1_325V ,
    VA7632B_F3_DCDC2_1_35V  ,
    VA7632B_F3_DCDC2_1_4V   ,
    VA7632B_F3_DCDC2_1_45V  ,
    VA7632B_F3_DCDC2_1_5V   ,
    VA7632B_F3_DCDC2_1_6V   ,
    VA7632B_F3_DCDC2_1_7V   ,
};

static int va7632b_f5_dcdc2_voltage_map[VA7632B_DCDC2_LIMIT] = {
    VA7632B_VOLTAGE_NONE   ,
    VA7632B_F5_DCDC2_0_85V ,
    VA7632B_F5_DCDC2_0_9V  ,
    VA7632B_F5_DCDC2_0_95V ,
    VA7632B_F5_DCDC2_1_0V  ,
    VA7632B_F5_DCDC2_1_1V  ,
    VA7632B_F5_DCDC2_1_2V  ,
    VA7632B_F5_DCDC2_1_25V ,
    VA7632B_F5_DCDC2_1_275V,
    VA7632B_F5_DCDC2_1_3V  ,
    VA7632B_F5_DCDC2_1_325V,
    VA7632B_F5_DCDC2_1_35V ,
    VA7632B_F5_DCDC2_1_4V  ,
    VA7632B_F5_DCDC2_1_45V ,
    VA7632B_F5_DCDC2_1_5V  ,
    VA7632B_F5_DCDC2_1_6V  ,
    VA7632B_F5_DCDC2_1_7V  ,
};


static va7632b_pmu_en_sel_t pmu_sel = VA7632B_PMU_EN_SEL_PMU_EN_PIN;
static uint8_t last_int_status = 0;
static int va7632b_pmu_state = PMU_POWER_OFF;

static VRET_VALUE va76xx_write_reg ( UINT8 Reg, UINT8 Value )
{
    VRET_VALUE ret = 0;

    ret = VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,Reg,&Value,1);//ret = Vcdc_CmdBus_Write(I2C_MODE_SLOW, PMU_I2C_SLAVE_ADDR, Reg, Value);
    VPMU_WPRINTF("VPMU_WriteReg: Reg address: 0x%x, WriteValue: 0x%x\r\n", Reg, Value);

    return ret;
}

static VRET_VALUE va76xx_read_reg ( UINT8 Reg, UINT8 *p_Value )
{
    VRET_VALUE ret;

    ret = VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,PMU_I2C_SLAVE_ADDR,Reg,p_Value,1);//ret = Vcdc_CmdBus_Read(I2C_MODE_SLOW, PMU_I2C_SLAVE_ADDR, Reg, p_Value);

    VPMU_WPRINTF("VPMU_ReadReg: Reg address: 0x%x, ReadValue: 0x%x\r\n", Reg, *p_Value);

    return ret;
}

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
int va7632b_power_supply_config(va7632b_power_config_t config)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~(REG_VA7632B_MEN5_MEN5_6_MASK | REG_VA7632B_MEN5_MEN5_2_MASK);
        val |= config; 
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}

//enable
///
/// @brief Register to set PMU ON/OFF.
///        pmu_on="1", PMU will be set ON.
///        pmu_on="0", PMU will be set OFF.
/// 
/// @return int int operation result： 0 successful, -1 failed.
///
static int va7632b_pmu_on_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_PMU_ON, 0x01);
    return ret;
}

///
/// @brief Register to set PMU ON/OFF.
///        pmu_on="1", PMU will be set ON.
///        pmu_on="0", PMU will be set OFF.
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_pmu_on_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_PMU_ON, 0x00);
    return ret;
}

///
/// @brief check power key is pressed or not.
/// 
/// @return int 1 pressed, 0 not pressed;
///
int va7632b_pmu_is_power_key_pressed(void)
{
    int is_pressed = 0;
    /* read GPIO_PIN_0 */
#if (VDEV_TYPE == TYPE_FPGA)
    /* on FPGA, power key pressed level is low */
    is_pressed = !VGPIO_GetPinInput(GPIO_PWRON_SHFT);
    
#elif (VDEV_TYPE == TYPE_ASIC)
    /* on ASIC, power key pressed level is high */
    is_pressed = VGPIO_GetPinInput(GPIO_PWRON_SHFT);
#endif
    VCOMMON_NPRINTF("%s:GPIO_PWRON_SHFT (%x) \r\n", FUNCTION_NAME, is_pressed);
    return is_pressed;
}
///
/// @brief Register to set PMU ON/OFF.
/// 
/// @return int 1 enable, 0 disable;
///
int va7632b_pmu_is_enable(void) 
{
    return va7632b_pmu_state;
}

///
/// @brief Register to set PMU ON/OFF.
/// 
/// @param sel Register to set mode of pmu_en_out. 
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_pmu_enable(va7632b_pmu_en_sel_t sel)
{
    va7632b_pmu_state = PMU_POWER_ON;
    if (sel == VA7632B_PMU_EN_SEL_PMU_EN_PIN)
    {
        /* write GPIO_PIN_0 */
        VGPIO_SetPinOutput(GPIO_PWRON_SHFT, 0x01);
    }
    else
    {
        // 1. pmu_sel设置为1
        // 2. pmu_on 设置为1
        va76xx_write_reg(REG_VA7632B_PMU_SEL, REG_VA7632B_PMU_SEL_PMU_SEL_MASK);
        va7632b_pmu_on_enable();
    }
    pmu_sel = sel;
    return 0;
}

///
/// @brief Register to set PMU ON/OFF.
/// 
/// @param sel Register to set mode of pmu_en_out.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_pmu_disable(void)
{
    va7632b_pmu_state = PMU_POWER_OFF;
    if (pmu_sel == VA7632B_PMU_EN_SEL_PMU_EN_PIN)
    {
        /* write GPIO_PIN_0 */
        VGPIO_SetPinOutput(GPIO_PWRON_SHFT, 0x00);
    }
    else
    {
        // 1. pmu_sel设置为0
        // 2. pmu_on 设置为0
        va76xx_write_reg(REG_VA7632B_PMU_SEL, 0x00);
        va7632b_pmu_on_disable();
    }
}

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
int va7632b_pmu_reset_mode(va7632b_pmu_reset_mode_t mode)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MMDC, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MMDC_RESET_MODE_MASK;
        val |= mode;
        ret = va76xx_write_reg(REG_VA7632B_MMDC, val);
    }
    return ret;
}   
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
int va7632b_deadlock_reset_delay_time_set(va7632b_deadlock_reset_delay_time_t time)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MTRST_LOCK, time);
    return ret;
}
//dcdc pwm frequency
///
/// @brief setting pwm frequency for buckrf&dcdca.
/// 
/// @param frequency destination frequency to change; input value enumeration.
///                 VA7632B_PWM_FREQUENCY_2_5MHz  0x00,
///                 VA7632B_PWM_FREQUENCY_1_5MHz  0x80,
///                 VA7632B_PWM_FREQUENCY_1_0MHz  0xA0,
/// @return int operation result： 0 successful, -1 failed.
///
int VA7632b_dcdc_power_pwm_frequency_set(va7632b_dcdc_pwm_frequency_t frequency)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~(REG_VA7632B_MEN5_MEN5_7_MASK | REG_VA7632B_MEN5_MEN5_5_MASK);
        val |= frequency;
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
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
int va7632b_over_temperature_threshold_set(va7632b_otp_threshold_t val)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MOTP, val);
    return ret;
}
// DC-DC 相关操作
///
/// @brief va7632b audio DC-DC1 enable.1'b1
///         DCA_en=0, audio DC-DC will be turned off.
///         DCA_en=1, audio DC-DC will be turned on.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_dcdc1_power_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_DCA_EN, 0x01);
    return ret;
}
///
/// @brief va7632b audio DC-DC1 enable.1'b1
///         DCA_en=0, audio DC-DC will be turned off.
///         DCA_en=1, audio DC-DC will be turned on.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_dcdc1_power_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_DCA_EN, 0x00);
    return ret;
}
///
/// @brief va7632b set dcdc1 mode
/// 
/// @param mode VA7632B_DCDC_MODE_AUTO = 0x00,
///             VA7632B_DCDC_MODE_PWM  = 0x01,
///             VA7632B_DCDC_MODE_PFM  = 0x03,
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_power_mode_set(va7632b_dcdc_mode_t mode)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MMDCA, mode);
    return ret;
}   
///
/// @brief Digital DC-DC1 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_voltage_set(va7632b_dcdc1_voltage_t volt)     // VA7632B_DCDC1_1_7V 16
{
    uint8_t chipid;
    int value = 0;
    int ret;
    if (volt == VA7632B_DCDC1_0_0V)
    {
        ret = va7632b_dcdc1_power_disable();
        return ret;
    }
    va7632b_chipid_get(&chipid);
    if (chipid == VA7632B_CHIPID_F3)
    {
        value = va7632b_f3_dcdc1_voltage_map[volt];
    }
    else if (chipid == VA7632B_CHIPID_F5)
    {
        value = va7632b_f5_dcdc1_voltage_map[volt];
    }
    else
    {
        return -1;
    }


    if (value == VA7632B_VOLTAGE_INVALID)
    {
        return -1;
    }

    va7632b_dcdc1_power_enable();
    ret = va76xx_write_reg(REG_VA7632B_MVDCA, value);
    return ret;
}

///
/// @brief 调节DCDCA的OCP_PFM阈值电流.
/// 
/// @param cur setting for OCP_PFM阈值电流.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_over_current_threshold_set(va7632b_dcdc1_over_current_threshold_t cur)
{
     int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO1_D_LPO_4_MASK;
        val |= cur;
        ret = va76xx_write_reg(REG_VA7632B_D_LPO1, val);
    }
    return ret;
}

///
/// @brief setting for DCDCA RCP current.
/// 
/// @param cur setting for DCDC1 RCP current.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc1_reverse_current_threshold_set(va7632b_dcdc1_reverse_current_threshold_t cur)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO2, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO2_D_LPO_12_10_MASK;
        val |= cur;
        ret = va76xx_write_reg(REG_VA7632B_D_LPO2, val);
    }
    return ret;
}

///
/// @brief va7632b audio DC-DC2 enable.1'b1
///         if Men5[4]=”0”, RF DC-DC is turned off; 
///         if Men5[4]=”1”, RF DC-DC is turned on.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_dcdc2_power_enable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= REG_VA7632B_MEN5_MEN5_4_MASK; 
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief va7632b audio DC-DC2 enable.1'b1
///         if Men5[4]=”0”, RF DC-DC is turned off; 
///         if Men5[4]=”1”, RF DC-DC is turned on.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_dcdc2_power_disable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MEN5_MEN5_4_MASK; 
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief va7632b set dcdc2 mode
/// 
/// @param mode VA7632B_DCDC_MODE_AUTO = 0x00,
///             VA7632B_DCDC_MODE_PWM  = 0x01,
///             VA7632B_DCDC_MODE_PFM  = 0x03,
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_power_mode_set(va7632b_dcdc_mode_t mode)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MMDC, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~(REG_VA7632B_MMDC_MMDC1_MASK|REG_VA7632B_MMDC_MMDC2_MASK);
        val |= mode;
        ret = va76xx_write_reg(REG_VA7632B_MMDC, val);
    }
    return ret;
}   
///
/// @brief RF DC-DC output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_voltage_set(va7632b_dcdc2_voltage_t volt)     // VA7632B_DCDC1_1_7V 16
{
    uint8_t chipid;
    int value = 0;
    int ret;
    if (volt == VA7632B_DCDC2_0_0V)
    {
        ret = va7632b_dcdc2_power_disable();
        return ret;
    }
    va7632b_chipid_get(&chipid);
    if (chipid == VA7632B_CHIPID_F3)
    {
        value = va7632b_f3_dcdc2_voltage_map[volt];
    }
    else if (chipid == VA7632B_CHIPID_F5)
    {
        value = va7632b_f5_dcdc2_voltage_map[volt];
    }
    else
    {
        return -1;
    }

    if (value == VA7632B_VOLTAGE_INVALID)
    {
        return -1;
    }
    va7632b_dcdc2_power_enable();
    ret = va76xx_write_reg(REG_VA7632B_MVDC, value);
    
    return ret;
}

int va7632b_dcdc2_voltage_get(va7632b_dcdc2_voltage_t *p_volt)     // VA7632B_DCDC1_1_7V 16
{
    int ret;
    uint8_t chipid;
    UINT8 value = 0;
    int *voltage_map = NULL;
    va7632b_dcdc2_voltage_t volt = VA7632B_DCDC2_0_0V;

    ret = va76xx_read_reg(REG_VA7632B_MEN5, &value);
    if (ret == VRET_NO_ERROR)
    {
        if((value | REG_VA7632B_MEN5_MEN5_4_MASK) == 0) {
            return -1;
        }
    }else{
        return -1;
    }

    ret = va76xx_read_reg(REG_VA7632B_MVDC, &value);
    if (ret != VRET_NO_ERROR)
    {
        return volt;
    }
    va7632b_chipid_get(&chipid);
    if (chipid == VA7632B_CHIPID_F3)
    {
        voltage_map = va7632b_f3_dcdc2_voltage_map;
    }
    else if (chipid == VA7632B_CHIPID_F5)
    {
        voltage_map = va7632b_f5_dcdc2_voltage_map;
    }
    else
    {
        return -1;
    }

    for(volt = VA7632B_DCDC2_0_0V; volt < VA7632B_DCDC2_LIMIT;volt++){
        if(voltage_map[volt] == value){
            break;
        }
    }

    if (volt == VA7632B_DCDC2_LIMIT)
    {
        return -1;
    }
    *p_volt = volt;
    return 0;
}

///
/// @brief 调节BUCKRF的OCP_PFM阈值电流。
/// 
/// @param cur setting for OCP_PFM阈值电流.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_over_current_threshold_set(va7632b_dcdc2_over_current_threshold_t cur)
{
     int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO1_D_LPO_3_MASK;
        val |= cur;
        ret = va76xx_write_reg(REG_VA7632B_D_LPO1, val);
    }
    return ret;
}

///
/// @brief setting for BUCKRF RCP current.
/// 
/// @param cur setting for DCDC2 RCP current.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_dcdc2_reverse_current_threshold_set(va7632b_dcdc2_reverse_current_threshold_t cur)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO2, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO2_D_LPO_15_13_MASK;
        val |= cur;
        ret = va76xx_write_reg(REG_VA7632B_D_LPO2, val);
    }
    return ret;
}

//LDO 相关操作
// LDO2
///
/// @brief Set LDO2 bypass mode. 
///        MV2=”1”, LDO2 works in bypass mode (Vout=Vin); 
///        MV2=”0”, LDO2 works in normal mode.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_ldo2_bypass_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MV2, 0x01);
    return ret;
}
///
/// @brief Set LDO2 bypass mode. 
///        MV2=”1”, LDO2 works in bypass mode (Vout=Vin); 
///        MV2=”0”, LDO2 works in normal mode.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_ldo2_bypass_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MV2, 0x00);
    return ret;
}
///
/// @brief ldo2 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo2_voltage_set(va7632b_ldo2_voltage_t volt)
{
    int ret = 0; 
    uint8_t val;
    
    if (volt == VA7632B_LDO2_USE_DCDC2_OUTPUT)
    {
        ret = va7632b_ldo2_bypass_enable();
    }
    else
    {
        ret = va7632b_ldo2_bypass_disable();
    }
    
    ret = va76xx_read_reg(REG_VA7632B_MBV, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MBV_MVOL2_MASK;
        val |= volt;
        ret = va76xx_write_reg(REG_VA7632B_MBV, val);
    }
    return ret;
}

// LDO5
///
/// @brief va7632b LDO5 enable
///        1’b0: disable LDO5.
///        1’b1: enable LDO5.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo5_enable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= REG_VA7632B_MEN5_MEN5_0_MASK;
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief va7632b LDO5 enable
///        1’b0: disable LDO5.
///        1’b1: enable LDO5.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_ldo5_disable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MEN5_MEN5_0_MASK; 
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief va7632b LDO5 operation mode control:
/// 
/// @param mode the operation mode select
///             Men5[1]=1'b1 && Men5[0]=1'b1 : LDO5 enter low power mode;
///             Men5[1]=1'b0 && Men5[0]=1'b1 : LDO5 stays at normal mode;
/// @return int 0: success, -1: fail
///
int va7632b_ldo5_mode_set(va7632b_ldo5_mode_t mode)
{
    int ret;
    uint8_t val;
    va7632b_ldo5_enable();
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MEN5_MEN5_1_MASK;
        val |= mode;
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief ldo5 output voltage setting.
/// 
/// @param volt destination voltage to change;
/// @return 0: success, -1: fail
///
int va7632b_ldo5_voltage_set(va7632b_ldo5_voltage_t volt)
{
    int ret = 0; 
    uint8_t chipid; 
    if (volt == VA7632B_LDO5_0_0V)
    {
        ret = va7632b_ldo5_disable();
        return ret;
    }
    va7632b_chipid_get(&chipid);
    if ((chipid == VA7632B_CHIPID_F5)&&(volt == VA7632B_LDO5_2_1V))
    {
        return -1;
    }
    ret = va76xx_write_reg(REG_VA7632B_MV5, volt);
    va7632b_ldo5_enable();
    return ret;
}

// charger 
///
/// @brief control the charging, can close the Charging Current.CHG_Stop =“0”  will not stop charger charging current.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_charging_current_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_CHG_STOP, ~REG_VA7632B_CHG_STOP_CHG_STOP_MASK);
    return ret;
}
///
/// @brief control the charging, can close the Charging Current.CHG_Stop =“1”  will stop charger charging current. 
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_charging_current_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_CHG_STOP, REG_VA7632B_CHG_STOP_CHG_STOP_MASK);
    return ret;
}
///
/// @brief [5:2] 4-bit setting for CC mode charging current.充电电流设置
/// 
/// @param cur destination current(mA) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_charging_current_set(va7632b_charging_current_t cur)
{
    int ret = 0; 
    uint8_t value;
    if (cur == VA7632B_CHARGE_CURRENT_0mA)
    {
        ret = va7632b_charger_charging_current_disable();
        return ret;
    }
    va7632b_charger_charging_current_enable();
    ret = va76xx_read_reg(REG_VA7632B_MCHV, &value);
    if (ret == VRET_NO_ERROR)
    {
        value &= ~REG_VA7632B_MCHV_MCHI_MASK;
        value |= cur;
        ret = va76xx_write_reg(REG_VA7632B_MCHV, value);
    }
    return ret;
}
///
/// @brief [1:0] Charging termination voltage control register.充电截止电压设置
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_termination_voltage_set(va7632b_charging_termination_voltage_t volt)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MCHV, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MCHV_MCHV_MASK;
        val |= volt;
        ret = va76xx_write_reg(REG_VA7632B_MCHV, volt);
    }
    return ret;
}

///
/// @brief 充电截止电流设置
/// 
/// @param cur destination current(mA) to change;
///        Mit=“0”, charging termination current is set to 10%*ICH; 
///        Mit=“1”, charging termination current is set to 5%*ICH.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_termination_current_set(va7632b_charging_termination_current_t cur)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MIT, cur);
    return ret;
}

///
/// @brief charger OVP enable signal. Active high.// 过压保护使能
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_ovp_enable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= REG_VA7632B_D_LPO1_D_LPO_2_MASK; 
        ret = va76xx_write_reg(REG_VA7632B_D_LPO1, val);
    }
    return ret;
}
///
/// @brief charger OVP enable signal. Active high.// 过压保护失能
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_ovp_disable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO1_D_LPO_2_MASK;  
        ret = va76xx_write_reg(REG_VA7632B_D_LPO1, val);
    }
    return ret;
}
///
/// @brief charger OVP voltage setting.
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_ovp_voltage_set(va7632b_charger_ovp_voltage_t volt)
{
    int ret;
    uint8_t val;
    if (volt == VA7632B_CHARGER_OVP_DISABLE)
    {
        ret = va7632b_charger_ovp_disable();
        return ret;
    }
    va7632b_charger_ovp_enable();
    ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_D_LPO1_D_LPO_1_0_MASK;
        val |= volt;
        ret = va76xx_write_reg(REG_VA7632B_D_LPO1, volt);
    }
    return ret;
}
///
/// @brief Enable signal for charging time.充电时间使能
///        1'b0 enable charger timing.
///        1'b1 disable charger timing.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_charging_time_enable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MCP, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MCP_MCP_0_MASK;
        ret = va76xx_write_reg(REG_VA7632B_MCP, val);
    }
    return ret;
}
///
/// @brief Enable signal for charging time.充电时间失能
///        1'b0 enable charger timing.
///        1'b1 disable charger timing.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_charger_charging_time_disable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MCP, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= REG_VA7632B_MCP_MCP_0_MASK; 
        ret = va76xx_write_reg(REG_VA7632B_MCP, val);
    }
    return ret;
}
///
/// @brief Setting for charging time.充电时间设置
///        Mcp[1]=0, charging time=1.8h.
///        Mcp[1]=1, charging time=3.6h.
/// 
/// @param volt destination voltage(V) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_charger_charging_time_set(va7632b_charging_time_t time)
{
    int ret;
    uint8_t val;
    if (time == VA7632B_CHARGING_TIME_UNLIMITED)
    {
        ret = va7632b_charger_charging_time_disable();
        return ret;
    }
    va7632b_charger_charging_time_enable();
    ret = va76xx_read_reg(REG_VA7632B_MCP, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MCP_MCP_1_MASK;
        val |= time;
        ret = va76xx_write_reg(REG_VA7632B_MCP, time);
    }
    return ret;
}

// VOD: voltage over detect 电池的低电压检测
///
/// @brief va7632b_battery_low_voltage_detect_enable
///        Men5[3]="0", VOD Enable. 
///        Men5[3]="1", VOD Disable. 
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_battery_low_voltage_detect_enable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MEN5_MEN5_3_MASK;  //Men5[3]="0", VOD Enable. 
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief va7632b_battery_low_voltage_detect_disable
///        Men5[3]="0", VOD Enable. 
///        Men5[3]="1", VOD Disable. 
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_battery_low_voltage_detect_disable(void)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_MEN5, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= REG_VA7632B_MEN5_MEN5_3_MASK;  //Men5[3]="1", VOD Disable.
        ret = va76xx_write_reg(REG_VA7632B_MEN5, val);
    }
    return ret;
}
///
/// @brief  Set Battery Low Detection threshold. When Battery voltage is lower than this threshold, BAT_Low will become high.达到阈值会产生中断，给到9655
/// 
/// @param volt destination voltage(hex) to change;
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_battery_low_voltage_threshold_set(va7632b_battery_low_voltage_threshold_t volt)
{
    int ret;
    uint8_t val;
    if (volt == VA7632B_BATTERY_LOW_VOLT_0_0V)
    {
        ret = va7632b_battery_low_voltage_detect_disable();
        return ret;
    }
    va7632b_battery_low_voltage_detect_enable();
    ret = va76xx_read_reg(REG_VA7632B_MBV, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~REG_VA7632B_MBV_MBV_MASK;
        val |= volt;
        ret = va76xx_write_reg(REG_VA7632B_MBV, volt);
    }
    return ret;
}

// 电压检测模块，检测电池电压，使用库仑计，实现
///
/// @brief is Coulomb Meter enable?
/// 
/// @param val Coulomb Meter enable stata, 0x01 enable; 0x00 disable
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_is_coulomb_meter_enable(uint8_t *val)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_MEN_CMETER, val); // read Coulomb Meter enable signal.
    return ret;
}
///
/// @brief Coulomb Meter enable signal.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_coulomb_meter_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MEN_CMETER, REG_VA7632B_MEN_CMETER_MEN_CMETER_MASK);
    return ret;
}
///
/// @brief Coulomb Meter enable signal.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_coulomb_meter_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MEN_CMETER, ~REG_VA7632B_MEN_CMETER_MEN_CMETER_MASK);
    return ret;
}
///
/// @brief Coulomb Meter output data, which demonstrate the battery voltage.获取电池电压
/// 
/// @param val The storage address to get the value
///        
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_battery_voltage_get(uint8_t *val)
{
    int ret;
    uint8_t status = 0, retry = 30;  
    /* read I2C register */
    ret = va7632b_is_coulomb_meter_enable(&status); // read Coulomb Meter enable signal.
    if (ret != VRET_NO_ERROR)
    {
        return -1;
    }

    if (status != 0)
    {
        ret = VRET_DRV_PMU_STATE_ERROR;
        VCOMMON_NPRINTF("%s:err ,The Coulomb Meter had entered active mode! (0x%X)\r\n", FUNCTION_NAME, ret);
        return ret;
    }

    va7632b_coulomb_meter_enable(); // Turn on AIO_ADC, active high.
    status = 0;                 
    // check the VD ready signal, (100us~2ms)
    do
    {
        VCOMMON_Delay(100); // 300us
        ret = va7632b_int_status_get(&status);
        if (ret == VRET_NO_ERROR)
        {
            if (status & REG_VA7632B_INT_STATUS_TERMB_INV_MASK)
            {
                break; // ready
            }
        }
        retry--;
    } while (retry);

    if (retry == 0)
    {
        ret = VRET_DRV_PMU_TERMB_TIMEOUT;
        VCOMMON_NPRINTF("%s:err ,The Coulomb Meter'termb has time out! \r\n", FUNCTION_NAME);
    }
    else
    {
        ret = va76xx_read_reg(REG_VA7632B_VD, val);
    }
    va7632b_coulomb_meter_disable();
    return ret;
}

// AIO_ADC
///
/// @brief Turn on AIO_ADC, active high.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_aio_adc_enable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MEN_AIOADC, REG_VA7632B_MEN_AIOADC_MEN_AIOADC_MASK);
    return ret;
}
///
/// @brief Turn on AIO_ADC, active high.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_aio_adc_disable(void)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_MEN_AIOADC, ~REG_VA7632B_MEN_AIOADC_MEN_AIOADC_MASK);
    return ret;
}
///
/// @brief is AIO_ADC enable?
/// 
/// @param val AIO_ADC enable stata, 0x01 enable; 0x00 disable
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_is_aio_adc_enable(uint8_t *val)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_MEN_AIOADC, val); // read Turn on AIO_ADC, active high.
    return ret;
}
///
/// @brief if D_LPO[8]=1,  D_LPO[5]="1", detect VBAT ;
///        D_LPO[5]="0", detect VCHG.
///        If D_LPO[8]=“1”, this ADC is used to detect VCHG or VBAT according to D_LPO[5]; 
///        If D_LPO[8]=“0”, this ADC is used to detect AIO_ADC.
/// 
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_aio_adc_detect_target_set(va7632b_adc_detect_target_t target)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_D_LPO2, &val);
    if (ret == VRET_NO_ERROR)
    {
        if (target == VA7632B_ADC_DETECT_AIO_VOLT)
        {
            val &= ~REG_VA7632B_D_LPO2_D_LPO_8_MASK;
            ret = va76xx_write_reg(REG_VA7632B_D_LPO2, val);
        }
        else
        {
            val =  val | REG_VA7632B_D_LPO2_D_LPO_8_MASK;
            ret = va76xx_write_reg(REG_VA7632B_D_LPO2, val);
            ret = va76xx_read_reg(REG_VA7632B_D_LPO1, &val);
            if (ret == VRET_NO_ERROR)
            {
                val &= ~REG_VA7632B_D_LPO1_D_LPO_5_MASK;
                val |= target;
                va76xx_write_reg(REG_VA7632B_D_LPO1, val); // set bit[5]=1
            }
        }
    }
}
///
/// @brief "term_AIOADC" is AIO_ADC output to indicate data ready.
/// 
/// @param val The storage address to get the value
///             1: data ready
///             0: data not ready
///        
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_is_aio_adc_detect_results_ready(uint8_t *val)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_TERM_AIOADC, val);
    return ret;
}
///
/// @brief AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
/// 
/// @param val The storage address to get the value
///             1: data ready
///             0: data not ready
///        
/// @return int operation result： 0 successful, -1 failed.
///

///
/// @brief get AIO_ADC data output. AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
/// 
/// @param target Target to be detected.
/// @param p_AIO The storage address to get the value.
/// @return int int operation result： 0 successful, -1 failed.
///
int va7632b_aio_adc_output_data_get(va7632b_adc_detect_target_t target, uint16_t *p_AIO)
{
    int ret;
    uint8_t val, retry = 10;
    uint16_t data;
    ret = va7632b_is_aio_adc_enable(&val); // read Turn on AIO_ADC, active high.
    if (ret != VRET_NO_ERROR)
    {
        VCOMMON_NPRINTF("%s:err ,va7632b_is_aio_adc_enable!\r\n", FUNCTION_NAME);
        return -1;
    }

    if (val != 0)
    {// error, AIO_ADC had been turned on.
        VCOMMON_NPRINTF("%s:err ,The AIO_ADC had been on, detecting!\r\n", FUNCTION_NAME);
        return VRET_DRV_PMU_STATE_ERROR;
    }
    va7632b_aio_adc_enable(); // Turn on AIO_ADC, active high.
    va7632b_aio_adc_detect_target_set(target);
    // check the AIO ready signal, (100us~2ms, default is 550us)
    do
    {
        VCOMMON_Delay(300); // 300us
        // term_AIOADC" is AIO_ADC output to indicate data ready.1: data ready,   0: data not ready
        ret = va7632b_is_aio_adc_detect_results_ready(&val);
        if (ret == VRET_NO_ERROR)
        {
            if (val & REG_VA7632B_TERM_AIOADC_TERM_AIOADC_MASK)
            {
                break; // ready
            }
        }
        retry--;
    } while (retry);
    
    if (retry != 0)
    {
        ret = va76xx_read_reg(REG_VA7632B_AIO_DATA2, &val); // read AIO output data, AIO_Data[9:8].
        if (ret == VRET_NO_ERROR)
        {
            data = (uint16_t)val << 8; // AIO_Data[9:8]
        }
        else
        {
            ret = VRET_DRV_PMU_STATE_ERROR;
            VCOMMON_NPRINTF("%s:error, The AIO_DATA2 err[0x%x]!\r\n", FUNCTION_NAME, ret);
        }

        ret = va76xx_read_reg(REG_VA7632B_AIO_DATA1, &val); // read AIO output data, AIO_Data[7:0].
        if (ret == VRET_NO_ERROR)
        {
            data |= val; // AIO_Data[7:0]
        }
        else
        {
            ret = VRET_DRV_PMU_STATE_ERROR;
            VCOMMON_NPRINTF("%s:error, The AIO_DATA1 err[0x%x]!\r\n", FUNCTION_NAME, ret);
        }
    }
    else
    {
        VCOMMON_NPRINTF("%s:error, get aio_adc output data has time out! \r\n", FUNCTION_NAME);
        ret = VRET_DRV_PMU_TERMB_TIMEOUT;
    }
    *p_AIO = data;
    va7632b_aio_adc_disable(); // Turn off AIO_ADC
    return ret;
}

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
int va7632b_int_status_get(uint8_t *status)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_INT_STATUS, status);
    return ret;
}

///
/// @brief Clear the corresponding bit of the interrupt state register
///        [5]: INT_INV_KeyON 1'b0,Inverse the interrupt input signal from register "KeyON", active high.
///        [4]: INT_INV_BAT_Low 1'b0,Inverse the interrupt input signal from register "BAT_Low", active high.
///        [3]: INT_INV_Pg 1'b0,Inverse the interrupt input signal from register "Pg", active high.
///        [2]: INT_INV_Stat 1'b0,Inverse the interrupt input signal from register "Stat", active high.
///        [1]: INT_INV_termb_inv 1'b0,Inverse the interrupt input signal from register "termb_inv", active high.
///        [0]: INT_INV_LINE_DET 1'b0,Inverse the interrupt input signal from register "CDC_LINE_DET", active high.
/// 
/// @param val To clear the corresponding values of interrupt status
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_int_inverse_set(uint8_t val)
{
    int ret;
    ret = va76xx_write_reg(REG_VA7632B_INT_INV, val);
    return ret;
}
///
/// @brief [5]: INT_INV_KeyON 1'b0,Inverse the interrupt input signal from register "KeyON", active high.
///        [4]: INT_INV_BAT_Low 1'b0,Inverse the interrupt input signal from register "BAT_Low", active high.
///        [3]: INT_INV_Pg 1'b0,Inverse the interrupt input signal from register "Pg", active high.
///        [2]: INT_INV_Stat 1'b0,Inverse the interrupt input signal from register "Stat", active high.
///        [1]: INT_INV_termb_inv 1'b0,Inverse the interrupt input signal from register "termb_inv", active high.
///        [0]: INT_INV_LINE_DET 1'b0,Inverse the interrupt input signal from register "CDC_LINE_DET", active high.
/// 
/// @param val get Interrupt inverse value
/// @return int operation result： 0 successful, -1 failed.
///
static int va7632b_int_inverse_get(uint8_t *val)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_INT_INV, val);
    return ret;
}

///
/// @brief 查询中断状态寄存器, 相比上一次查询结果, 哪些bits发生改变
/// 
/// @return uint8_t 变化了的那些bits值
///
uint8_t va7632b_int_status_change_check(void)
{
    uint8_t int_state = 0, inv_config = 0, change_bits = 0, ret = 0;
    va7632b_int_status_get(&int_state);

    change_bits = int_state ^ last_int_status;

    ret = va7632b_int_inverse_get(&inv_config);
    if (ret == VRET_NO_ERROR)
    {
        inv_config ^= change_bits;
        va7632b_int_inverse_set(inv_config);
    }
    last_int_status = int_state;

    return change_bits;
}

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
int va7632b_int_mask_set(uint8_t mask_val)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_INT_MASK, &val);
    if (ret == VRET_NO_ERROR)
    {
        val |= mask_val;
        ret = va76xx_write_reg(REG_VA7632B_INT_MASK, val);
    }
    return ret;
}

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
int va7632b_int_unmask_set(uint8_t unmask_val)
{
    int ret;
    uint8_t val;
    ret = va76xx_read_reg(REG_VA7632B_INT_MASK, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~unmask_val;
        ret = va76xx_write_reg(REG_VA7632B_INT_MASK, val);
    }
    return ret;
}

//版本, ID
///
/// @brief va7632b get int version indicator
/// 
/// @param status Version indicator:
///               0=low-end version,
///               1=high-end version.
/// @return int operation result： 0 successful, -1 failed.
///
int va7632b_version_get(uint8_t *version)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_VER_SEL, version);
    return ret;
}
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
int va7632b_chipid_get(uint8_t *chipid)
{
    int ret;
    ret = va76xx_read_reg(REG_VA7632B_CHIP_ID3, chipid);
    return ret;
}

int va7632b_voltage_test(void)
{
    VCOMMON_NPRINTF("%s: va7632b_voltage_test starting!\r\n", FUNCTION_NAME);
    for (uint8_t i = 1; i < VA7632B_DCDC1_LIMIT; i++)
    {
        va7632b_dcdc1_voltage_set(i);
        VCOMMON_Delay(300000);
    }
    VCOMMON_Delay(3000000);
    for (uint8_t i = VA7632B_LDO2_0_9V; i <= VA7632B_LDO2_USE_DCDC2_OUTPUT; i++)
    {
        va7632b_dcdc2_voltage_set(i);
        VCOMMON_Delay(300000);
    }
    VCOMMON_Delay(3000000);
    for (uint8_t i = 1; i < VA7632B_DCDC2_LIMIT; i++)
    {
        va7632b_dcdc2_voltage_set(i);
        VCOMMON_Delay(3000000);
    }
    VCOMMON_Delay(3000000);
    for (uint8_t i = VA7632B_LDO5_2_0V; i <= VA7632B_LDO5_2_1V; i++)
    {
        va7632b_ldo5_voltage_set(i);
        VCOMMON_Delay(3000000);
    }
    va7632b_ldo5_voltage_set(VA7632B_LDO5_0_0V);
    va7632b_dcdc1_voltage_set(VA7632B_DCDC1_0_0V);
    va7632b_dcdc2_voltage_set(VA7632B_DCDC2_0_0V);
}


int va7632b_test(void)
{
    uint16_t data;
    // va7632b_voltage_test();
    // va7632b_aio_adc_output_data_get(VA7632B_ADC_DETECT_AIO_VOLT, &data);
    // VCOMMON_NPRINTF("%s: va7632b_aio_adc_output_data_get detecting!(0x%x)\r\n", FUNCTION_NAME, data);
    va7632b_deadlock_reset_delay_time_set(VA7632B_DELAY_3s);
}


static uint8_t va7632b_dcdc1_convert_to_voltage(uint8_t reg_value)
{
    uint8_t chipid, number;
    va7632b_chipid_get(&chipid);
    if (chipid == VA7632B_CHIPID_F3)
    {
        for (uint8_t i = 0; i < VA7632B_DCDC1_LIMIT; i++)
        {
            if (reg_value == va7632b_f3_dcdc1_voltage_map[i])
            {
                number = i;
                break;
            }
            
        }
    }
    else if (chipid == VA7632B_CHIPID_F5)
    {
        for (uint8_t i = 0; i < VA7632B_DCDC1_LIMIT; i++)
        {
            if (reg_value == va7632b_f5_dcdc1_voltage_map[i])
            {
                number = i;
                break;
            }
            
        }
    }
   return number;
}

int va7632b_dcdc1_voltage_set_legacy(uint8_t reg_value)
{
    va7632b_dcdc1_voltage_t volt = 0; 
    volt = va7632b_dcdc1_convert_to_voltage(reg_value);
    va7632b_dcdc1_voltage_set(volt);
}



