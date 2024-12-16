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
    CDC

Description:
    Internale CoDeC module driver.

Revision History:
    Date                  Author                   Description
    2010.05.12           chenjingen              initial verison
    2011.04.06           xiongyan                add 9611s
    2011.04.09           xiongyan                9611s and 9611 compatible
    2011.12.19           xiongyan                add 9611s_B initial
    2011.12.27           xiongyan                add 9611s_B and 9611s_C
    2012.04.05           xiongyan                add CDC_DEALNOISE_DELAY mode for long delay(9611SB&SC)
    2012.10.17           xiongyan                updata for 9611S_D, 9611F
    2013.07.22           xiongyan                only keep the 9611S_D 9611F drivers, for condense code.
    2013.11.16           xiongyan                add 9611SE0, both single end mode(same as SD0) and fully differential mode(same as F0).
    2014.02.10      xiongyan            add VA7621BA0
    2014.02.18      xiongyan            add 7621SA0 and 7621GA0
    2019.06.10           xiongyan                add VA7632B_A0
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdos.h"
#include "vstdio.h"
#include "vdrv_ckm.h"
#include "vdrv_i2c.h"
#include "va7632b_cdc.h"
#include "va7632b_reg.h"
//#include "vcodec_hw.h"
//#include "vcodec.h"

#define va76xx_write_reg(reg, data) VI2C_Continue_write(I2C_SLAVE_ANA,I2C_MODE_STANDARD,PMU_I2C_SLAVE_ADDR,reg, data)
#define va76xx_read_reg(reg, pdata) VI2C_Continue_read(I2C_SLAVE_ANA,I2C_MODE_STANDARD,PMU_I2C_SLAVE_ADDR,reg, pdata)

VCDC_CONFIG g_VCDC_Config;
UINT8 g_VCDC_Input = CDC_INPUT_NONE;
UINT8 g_VCDC_Output = CDC_OUTPUT_NONE;

//for common new 7621 config
VCDC_PLUS_CONFIG g_VCDC_Plus_Config = {0};

#if 1 //VDEV_7621_I2S_EN
//for common new 7621I2S config
VCDC_I2S_CONFIG g_VCDC_I2S_Config = {0x02, 0xF0, 0x10}; //{0x02,0xF0,0x01};//0xE8,0xB8,0xF8(rxtx24bit)
#endif

#if 1
VCDC_DEBUG g_VCDC_Debug;

#define CDC_7621SD_HPAGAIN_MODE (g_VCDC_Debug.hpagain_mode)
#define CDC_7621A_PD_FD (g_VCDC_Debug.pd_chg_r)
//VA7632BA0
#define CDC_7632BA_PD_OUT_L_DG (g_VCDC_Debug.dac_debug_l)
#define CDC_7632BA_PD_OUT_R_DG (g_VCDC_Debug.dac_debug_r)
#define CDC_7632BA_PD_CHARGE_DG (g_VCDC_Debug.pop_initen)

#define CDC_7632BA_PGA_CUR_DG (g_VCDC_Debug.dg_pga_cur)
#define CDC_7632BA_ADCUR_DG (g_VCDC_Debug.dg_adcur)
#define CDC_7632BA_REF_DG (g_VCDC_Debug.dg_ref)
#define CDC_7632BA_VREF_SEL_DG (g_VCDC_Debug.dg_vref_sel)
#define CDC_7632BA_VCOM_SEL_DG (g_VCDC_Debug.dg_vcom_sel)
#define CDC_7632BA_CTSC_DG (g_VCDC_Debug.dg_ctsc)
#define CDC_7632BA_MBSEL_DG (g_VCDC_Debug.dg_mbsel)
#define CDC_7632BA_REFPD_HIGH_DG (g_VCDC_Debug.dg_refpd_high)
#define CDC_7632BA_DACCTL1_DG (g_VCDC_Debug.dg_dacctl1)
#define CDC_7632BA_DACCTL2_DG (g_VCDC_Debug.dg_dacctl2)
#define CDC_7632BA_MIX_DG (g_VCDC_Debug.dg_mix)
#define CDC_7632BA_ICTL_STBF_DG (g_VCDC_Debug.dg_ictl_stbf)
#define CDC_7632BA_PWD_HFOUT_DG (g_VCDC_Debug.dg_pwd_hfout)
#define CDC_7632BA_HP_ICTL_DG (g_VCDC_Debug.dg_hp_ictl)
#define CDC_7632BA_HPPD_OUT_DG (g_VCDC_Debug.dg_hppd_out)
#define CDC_7632BA_NMILLER_DG (g_VCDC_Debug.dg_nmiller)
#define CDC_7632BA_CBOOST_DG (g_VCDC_Debug.dg_cboost)
#define CDC_7632BA_VGATE_SEL_DG (g_VCDC_Debug.dg_vgate_sel)
#define CDC_7632BA_VSUB_HIGH_DG (g_VCDC_Debug.dg_vsub_high)
#define CDC_7632BA_HPA_NATIVE_DG (g_VCDC_Debug.dg_hpa_native)
#define CDC_7632BA_LINEIN_AMP_VREF_DG (g_VCDC_Debug.dg_linein_amp_vref)
#define CDC_7632BA_HPAGAIN_MODE_DG (g_VCDC_Debug.hpagain_mode)

#define CDC_7632BA_PD_OUT_L_0 (g_VCDC_Debug.dac_mode_l)
#define CDC_7632BA_PD_OUT_R_0 (g_VCDC_Debug.dac_mode_r)
#else
#define CDC_7621A_PD_FD 0x00      // 0x00 差分， 0x07>0 单端
#define CDC_7621SD_HPAGAIN_MODE 0x00
//VA7632BA0
#define CDC_7632BA_PD_OUT_L_DG 0x00
#define CDC_7632BA_PD_OUT_R_DG 0x00
#define CDC_7632BA_PD_CHARGE_DG 0xEE

#define CDC_7632BA_PGA_CUR_DG 0x33
#define CDC_7632BA_ADCUR_DG 0x33
#define CDC_7632BA_REF_DG 0x12
#define CDC_7632BA_VREF_SEL_DG 0x00
#define CDC_7632BA_VCOM_SEL_DG 0x06
#define CDC_7632BA_CTSC_DG 0x00
#define CDC_7632BA_MBSEL_DG 0x02
#define CDC_7632BA_REFPD_HIGH_DG 0x0F
#define CDC_7632BA_DACCTL1_DG 0x44
#define CDC_7632BA_DACCTL2_DG 0x00
#define CDC_7632BA_MIX_DG 0x00
#define CDC_7632BA_ICTL_STBF_DG 0x00
#define CDC_7632BA_PWD_HFOUT_DG 0x00
#define CDC_7632BA_HP_ICTL_DG 0x00
#define CDC_7632BA_HPPD_OUT_DG 0x33
#define CDC_7632BA_NMILLER_DG 0x00
#define CDC_7632BA_CBOOST_DG 0x00
#define CDC_7632BA_VGATE_SEL_DG 0x00
#define CDC_7632BA_VSUB_HIGH_DG 0x00
#define CDC_7632BA_HPA_NATIVE_DG 0x00
#define CDC_7632BA_LINEIN_AMP_VREF_DG 0x00
#define CDC_7632BA_HPAGAIN_MODE_DG 0x00

#define CDC_7632BA_PD_OUT_L_0 0x11 //diff
#define CDC_7632BA_PD_OUT_R_0 0x11
#endif

VRET_VALUE VCDC_WriteReg ( UINT8 Reg, UINT8 Value )
{
    return va76xx_write_reg(Reg, Value); 
}

VRET_VALUE VCDC_ReadReg ( UINT8 Reg, UINT8 *p_Value )
{
    return va76xx_read_reg(Reg, p_Value);
}


static int32_t va7632b_power(uint8_t val)
{
    int32_t ret;
    /* power on REF */
    // a)    0x40=0x00（MIC应用）/   0x08（LINEIN应用）；打开Bandgap，Mic bias；
    ret = ret | va76xx_write_reg(REG_VA7632B_CDC_REFPD_LOW, val); // CDC_REFPD[4:0] =5'b00000  //if open dac&adc, open Mbias for adc
    // 20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6;
    /* power on CTBUF */                                       /* power on SCBUF */
    ret = ret | va76xx_write_reg(REG_VA7632B_CDC_CTBUF, 0x00); // CDC_CTBUFPD = 0
    ret = ret | va76xx_write_reg(REG_VA7632B_CDC_SCBUF, 0x04); // CDC_SCBUFPD = 1
    return ret;
}
static void va7632b_input_micl(void)
{
    /* Power on ADC */
    va76xx_write_reg(REG_VA7632B_CDC_ADCLKEN, 0x01);  // CDC_ADCLKENL=1
    va76xx_write_reg(REG_VA7632B_CDC_ADPDIREF, 0x02); // CDC_ADPDIREFL=0
    va76xx_write_reg(REG_VA7632B_CDC_ADPD, 0x02);     // CDC_ADPDL=0
    va76xx_write_reg(REG_VA7632B_CDC_ADRSTB, 0x01);   // CDC_ADRSTBL=1

    /* Power on and unMute PGA */
    va76xx_write_reg(REG_VA7632B_CDC_PGA, 0x30);         // CDC_PGAMUTEL=0
    va76xx_write_reg(REG_VA7632B_CDC_PGA_PD_IREF, 0x10); // CDC_PGA_PD_IREF_L=0
    va76xx_write_reg(REG_VA7632B_CDC_LINEIN_SEL, 0x00);  // CDC_LINEIN_SELL=0
}
static void va7632b_input_micr(void)
{
    va76xx_write_reg(REG_VA7632B_CDC_ADCLKEN, 0x02);  // CDC_ADCLKENR=1
    va76xx_write_reg(REG_VA7632B_CDC_ADPDIREF, 0x01); // CDC_ADPDIREFR=0
    va76xx_write_reg(REG_VA7632B_CDC_ADPD, 0x01);     // CDC_ADPDR=0
    va76xx_write_reg(REG_VA7632B_CDC_ADRSTB, 0x02);   // CDC_ADRSTBR=1

    va76xx_write_reg(REG_VA7632B_CDC_PGA, 0x03);         // CDC_PGAMUTER=0
    va76xx_write_reg(REG_VA7632B_CDC_PGA_PD_IREF, 0x01); // CDC_PGA_PD_IREF_R=0
    va76xx_write_reg(REG_VA7632B_CDC_LINEIN_SEL, 0x00);  // CDC_LINEIN_SELR=0
}
static void va7632b_input_micd(void)
{
    va76xx_write_reg(REG_VA7632B_CDC_ADCLKEN, 0x03);  // CDC_ADCLKENL&R=1
    va76xx_write_reg(REG_VA7632B_CDC_ADPDIREF, 0x00); // CDC_ADPDIREFL&R=0
    va76xx_write_reg(REG_VA7632B_CDC_ADPD, 0x00);     // CDC_ADPDL&R=0
    va76xx_write_reg(REG_VA7632B_CDC_ADRSTB, 0x03);   // CDC_ADRSTBL&R=1

    va76xx_write_reg(REG_VA7632B_CDC_PGA, 0x00);         // CDC_PGAMUTEL&R=0
    va76xx_write_reg(REG_VA7632B_CDC_PGA_PD_IREF, 0x00); // CDC_PGA_PD_IREF_L&R=0
    va76xx_write_reg(REG_VA7632B_CDC_LINEIN_SEL, 0x00);  // CDC_LINEIN_SELL&R=0
}
static void va7632b_input_micoff(void)
{
    /* Power down ADC */
    //关闭ADC；
    va76xx_write_reg(REG_VA7632B_CDC_ADCLKEN, 0x00);  // CDC_ADCLKENL&R=0
    va76xx_write_reg(REG_VA7632B_CDC_ADPDIREF, 0x03); // CDC_ADPDIREFL&R=1
    va76xx_write_reg(REG_VA7632B_CDC_ADPD, 0x03);     // CDC_ADPDL&R=1
    va76xx_write_reg(REG_VA7632B_CDC_ADRSTB, 0x00);   // CDC_ADRSTBL&R=0

    /* Power down and Mute PGA */
    //关闭PGA；
    // va76xx_write_reg(REG_VA7632B_CDC_PGA, 0x22); //CDC_PGAPDL&R=1
    va76xx_write_reg(REG_VA7632B_CDC_PGA, 0x33);         // CDC_PGAMUTEL&R=1
    va76xx_write_reg(REG_VA7632B_CDC_PGA_PD_IREF, 0x11); // CDC_PGA_PD_IREF_L&R=1
    va76xx_write_reg(REG_VA7632B_CDC_LINEIN_SEL, 0x11);  // CDC_LINEIN_SELL&R=1
}

static void va7632b_output_hpl(UINT8 mode)
{
    /* Power on DAC and Mixer */
    // va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x04); // CDC_DACPDDIG_L = 1
    va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x0C); // CDC_DACPDLPF_L = 1

    va76xx_write_reg(REG_VA7632B_CDC_DACCTL3, 0x10); // CDC_DACRSTDIG_L = 0
    // va76xx_write_reg(REG_VA7632B_CDC_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0
    va76xx_write_reg(REG_VA7632B_CDC_HP_PWD_IREF, 0x01); // CDC_HP_PWD_IREF_L = 1
    va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x23);         // CDC_HPPD_L = 1

    va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, CDC_7621A_PD_FD); // set diff or sing mode.

    // va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x4C); //CDC_MIXPD_L = 1
    // va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x48); //CDC_MIXMUTE_L = 0
    //if (VCODEC_Check_PopBetter_Enable() == FALSE)
    if ( (mode & 0x01) == 0 ) {
        va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG); // CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
    }
    va76xx_write_reg(REG_VA7632B_CDC_PWD_STBF, 0x00);     // CDC_PWD_STBF_L=0
    va76xx_write_reg(REG_VA7632B_CDC_PD_CHARGE_DG, 0x00); // CDC_PD_CHARGE_L=0

    /* Power on HPPA */
    va76xx_write_reg(REG_VA7632B_CDC_HP0, 0x40); // CDC_HPMUTE_L=0
    // va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x23); //CDC_HPPD_L=1
    // va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x23); //CDC_HPPOP_L=0
}
static void va7632b_output_hpr(UINT8 mode)
{
    /* Power on DAC and Mixer */
    // va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x40); // CDC_DACPDDIG_R = 1
    va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0xC0); // CDC_DACPDLPF_R = 1

    va76xx_write_reg(REG_VA7632B_CDC_DACCTL3, 0x01); // CDC_DACRSTDIG_R = 0
    // va76xx_write_reg(REG_VA7632B_CDC_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_R=0
    va76xx_write_reg(REG_VA7632B_CDC_HP_PWD_IREF, 0x10); // CDC_HP_PWD_IREF_R = 1
    va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x32);         // CDC_HPPD_R = 1

    va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, CDC_7621A_PD_FD); // set diff or sing mode.

    // va76xx_write_reg(REG_VA7632B_CDC_MIX, 0xC4); //CDC_MIXPD_R = 1
    // va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x84); //CDC_MIXMUTE_R = 0
    //if (VCODEC_Check_PopBetter_Enable() == FALSE)
    if ( (mode & 0x01) == 0 ) {
        va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG); // CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
    }
    va76xx_write_reg(REG_VA7632B_CDC_PWD_STBF, 0x00);     // CDC_PWD_STBF_R=0
    va76xx_write_reg(REG_VA7632B_CDC_PD_CHARGE_DG, 0x00); // CDC_PD_CHARGE_R=0

    /* Power on HPPA */
    va76xx_write_reg(REG_VA7632B_CDC_HP0, 0x04); // CDC_HPMUTE_R=0
    // va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x32); //CDC_HPPD_R=1
    // va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x32); //CDC_HPPOP_R=0
}
static void va7632b_output_hp(UINT8 mode)
{
 /* Power on DAC and Mixer */
            //打开DAC；
            //va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x44); // CDC_DACPDDIG_L = 1, CDC_DACPDDIG_R = 1,
            va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0xCC); //CDC_DACPDLPF_L = 1, CDC_DACPDLPF_R = 1,
            //释放DAC digital reset；
            va76xx_write_reg(REG_VA7632B_CDC_DACCTL3, 0x00); //CDC_DACRSTDIG_L = 0,  CDC_DACRSTDIG_R = 0,
            //打开PA的IREF；
            va76xx_write_reg(REG_VA7632B_CDC_HP_PWD_IREF, 0x11); //CDC_HP_PWD_IREF_L = 1, CDC_HP_PWD_IREF_R = 1
            //打开PA；
            va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x33); //CDC_HPPD_L = 1, CDC_HPPD_R = 1
            //决定是单端输出还是差分输出，差分为0x00，单端为0x77；
            va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, CDC_7621A_PD_FD); //set diff or sing mode.

            //va76xx_write_reg(REG_VA7632B_CDC_MIX, 0xCC); //CDC_MIXPD_L = 1,      CDC_MIXPD_R = 1
            //va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x88); //CDC_MIXMUTE_L = 0,    CDC_MIXMUTE_R = 0

            //打开PA输出级电路，如果为差分电路，ECK合理值应为：0x11,0x22…,0x77；如果为单端电路，ECK合理值应为：0x01,0x02，…,0x07；
            //if (VCODEC_Check_PopBetter_Enable() == FALSE)
            if ( (mode & 0x01) == 0 ) {
                va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
                va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
            }
            //关闭POP buffer；
            va76xx_write_reg(REG_VA7632B_CDC_PWD_STBF, 0x00); //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
            //关闭POP charge；
            va76xx_write_reg(REG_VA7632B_CDC_PD_CHARGE_DG, 0x00); //CDC_PD_CHARGE_L=0, CDC_PD_CHARGE_R=0

            /* Power on HPPA */
            va76xx_write_reg(REG_VA7632B_CDC_HP0, 0x00); //CDC_HPMUTE_L=0,       CDC_HPMUTE_R=0
            //va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x33); //CDC_HPPD_L=1,     CDC_HPPD_R=1
            //va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x33); //CDC_HPPOP_L=0,        CDC_HPPOP_R=0
}
static void va7632b_output_hpoff(UINT8 mode)
{
 
            /* Power down HPPA */
            if ( (mode & 0x01) == 0 ) {//if (VCODEC_Check_PopBetter_Enable() == FALSE)
                //b)    0x8B=0x00,  0x8C=0x00；                     关闭PA输出电路
                va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, 0x00); //CDC_PD_OUT0I_R=0, CDC_PD_OUT1I_R=0, CDC_PD_OUT2I_R=0
                va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, 0x00); //CDC_PD_OUT0I_L=0, CDC_PD_OUT1I_L=0, CDC_PD_OUT2I_L=0
            }
            //Mute PA；
            va76xx_write_reg(REG_VA7632B_CDC_HP0, 0x44); //CDC_HPMUTE_L=1,       CDC_HPMUTE_R=1
            //关闭PA；
            va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x22); //CDC_HPPD_L=0,     CDC_HPPD_R=0
            //va76xx_write_reg(REG_VA7632B_CDC_HP1, 0x22); //CDC_HPPOP_L=1,        CDC_HPPOP_R=1
            //关闭PA的IREF；
            va76xx_write_reg(REG_VA7632B_CDC_HP_PWD_IREF, 0x00); //CDC_HP_PWD_IREF_L=0,          CDC_HP_PWD_IREF_R=0

            /* Power down DAC and Mixer */
            //DAC digital reset；
            va76xx_write_reg(REG_VA7632B_CDC_DACCTL3, 0x11); //CDC_DACRSTDIG_L = 1,  CDC_DACRSTDIG_R = 1,
            //关闭DAC；
            //va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x88); // CDC_DACPDDIG_L = 0, CDC_DACPDDIG_R = 0,
            va76xx_write_reg(REG_VA7632B_CDC_DACCTL2, 0x00); //CDC_DACPDLPF_L = 0, CDC_DACPDLPF_R = 0,

            //va76xx_write_reg(REG_VA7632B_CDC_PWD_STBF, 0x00);    //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
            //va76xx_write_reg(REG_VA7632B_CDC_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0, CDC_HP_PULLDOWN_EN_R=0

            //va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, 0x00);    //CDC_PD_CMFB_L=0,      CDC_PD_FDNCS_L=0,       CDC_PD_FDPCS_L=0
            //va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, 0x00);    //CDC_PD_CMFB_R=0,      CDC_PD_FDNCS_R=0,   CDC_PD_FDPCS_R=0

            //va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x00); //CDC_MIXPD_L = 0,      CDC_MIXPD_R = 0
            //va76xx_write_reg(REG_VA7632B_CDC_MIX, 0x44); //CDC_MIXMUTE_L = 1,    CDC_MIXMUTE_R = 1

            
}
static uint8_t va7632b_check_dac_mode(uint8_t channel)
{
    UINT8 Ret = CDC_OUTPUTMODE_SING; //default
    VRET_VALUE ret;
    UINT8 PD_FD_Val;
    ret = va76xx_read_reg(REG_VA7632B_CDC_PD_FD_DG, &PD_FD_Val);
    if (ret == VRET_NO_ERROR)
        {
            if (CDC_7621A_PD_FD != PD_FD_Val)
            {
                VCDC_EPRINTF("%s: CDC_REGS_PD_FD val[%d]!=config[%d].\r\n", FUNCTION_NAME, PD_FD_Val, CDC_7621A_PD_FD);
                ret = va76xx_write_reg(REG_VA7632B_CDC_PD_FD_DG, CDC_7621A_PD_FD);
                if (ret != VRET_NO_ERROR)
                {
                    VCDC_FPRINTF("%s: retry write CDC_REGS_PD_FD, but fail[0x%x]; maybe the 9611 err, not 7621A!\r\n", FUNCTION_NAME, ret);
                    //return Ret;   //return CDC_OUTPUTMODE_DIFF;   //7621A default
                    return ((PD_FD_Val > 0) ? CDC_OUTPUTMODE_SING : CDC_OUTPUTMODE_DIFF);
                }
                else
                {
                    PD_FD_Val = CDC_7621A_PD_FD;
                }
            }

            VCDC_NPRINTF("%s: CDC_REGS_PD_FD read val=%d, DacChn%d.\r\n", FUNCTION_NAME, PD_FD_Val, DacChn);
            //ASSERT(PD_FD_Val<=7);

            if (PD_FD_Val > 0)
            {
                Ret = CDC_OUTPUTMODE_SING;
            }
            else
            { //PD_FD_Val == 0
                Ret = CDC_OUTPUTMODE_DIFF;
            }
        }
        else
        {
            VCDC_EPRINTF("%s: CDC_REGS_PD_FD read fail[0x%x]!\r\n", FUNCTION_NAME, ret);
            Ret = (CDC_7621A_PD_FD > 0) ? CDC_OUTPUTMODE_SING : CDC_OUTPUTMODE_DIFF;
        }
    return Ret;
}

void va7632b_input_config(uint8_t input)
{
    if (input != CDC_INPUT_NONE)
    {
        if (g_VCDC_Output == CDC_OUTPUT_NONE)
        {
            //va7632b_power(~REG_VA7632B_CDC_REFPD_LOW_CDC_REFPD_MASK);
                        /* power on REF */
                //a)    0x40=0x00（MIC应用）/   0x08（LINEIN应用）；打开Bandgap，Mic bias；
                va76xx_write_reg ( REG_VA7632B_CDC_REFPD_LOW, 0x00 ); //CDC_REFPD[4:0] =5'b00000  //if open dac&adc, open Mbias for adc
                     
                        //20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6; 
                        /* power on CTBUF */
                va76xx_write_reg ( REG_VA7632B_CDC_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
                    /* power on SCBUF */
                va76xx_write_reg ( REG_VA7632B_CDC_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
        }
    }

    switch (input)
    {
    case CDC_INPUT_MICL:
        va7632b_input_micl();
        break;
    case CDC_INPUT_MICR:
        va7632b_input_micr();
        break;
    case CDC_INPUT_DUAL_MIC:
        va7632b_input_micd();
        break;
    case CDC_INPUT_NONE:
        if (g_VCDC_Output == CDC_OUTPUT_NONE)
        {
            va7632b_power(REG_VA7632B_CDC_REFPD_LOW_CDC_REFPD_MASK);
        }
        va7632b_input_micoff();
        break;
    }
    g_VCDC_Input = input;
}

void va7632b_output_config(uint8_t output,uint8_t mode)
{
    if (output == CDC_OUTPUT_HP)
        {
            output = g_VCDC_Config.dac_chn;
            //set the volume to 0 at begin; minish popup noise when power on
            if (g_VCDC_Config.deal_popupnois != CDC_DEALNOISE_NONE)
            {
                va76xx_write_reg(CDC_REGS_HPVOL, 0);
            }

            //if (Output != CDC_OUTPUT_NONE) {
            if (g_VCDC_Input == CDC_INPUT_NONE)
            {
                /* power on REF */
                //打开Bandgap；
                //va76xx_write_reg(REG_VA7632B_CDC_REFPD_LOW, 0x00); //CDC_REFPD[4:0] =5'b00000    //if open dac&adc, open Mbias for adc
                //va7632b_power(REG_VA7632B_CDC_REFPD_LOW_MIC_BIAS_MASK); //the output is power on only for dac, close Mbias
                /* power on REF */
                    //打开Bandgap；
                    //VCDC_WriteReg(CDC_REG763X_REFPD_LOW, 0x00); //CDC_REFPD[4:0] =5'b00000    //if open dac&adc, open Mbias for adc
                    va76xx_write_reg ( REG_VA7632B_CDC_REFPD_LOW, 0x08 ); //the output is power on only for dac, close Mbias
                                
                                //20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6; 
                                /* power on CTBUF */
                    va76xx_write_reg ( REG_VA7632B_CDC_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
                                /* power on SCBUF */
                    va76xx_write_reg ( REG_VA7632B_CDC_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
                        }
            //}
        }

        switch (output)
        {
        case CDC_OUTPUT_HPL: //left channel
            va7632b_output_hpl(mode);
            break;
        case CDC_OUTPUT_HPR: //right channel
            va7632b_output_hpr(mode);
            break;
        case CDC_OUTPUT_HP: //dual channel
            va7632b_output_hp(mode);
            break;
        case CDC_OUTPUT_NONE: //close dac
        //a)    0x8E=0x00（差分电路）；ECK（单端电路）；        控制POP charge；
            if (va7632b_check_dac_mode(output) == CDC_OUTPUTMODE_DIFF)
            {
                //diff:
                va76xx_write_reg(REG_VA7632B_CDC_PD_CHARGE_DG, 0x00); //CDC_PD_CMFB_L=0,  CDC_PD_FDNCS_L=0,   CDC_PD_FDPCS_L=0
            }
            else
            { //CDC_OUTPUTMODE_SING
                //sing:
                va76xx_write_reg(REG_VA7632B_CDC_PD_CHARGE_DG, CDC_7632BA_PD_CHARGE_DG); //eck
            }
            va7632b_output_hpoff(mode);
            if (g_VCDC_Input == CDC_INPUT_NONE)
            {
                /* Power down REF、SCBUF and CTBUF */
                //关闭Bandgap；
                va7632b_power(REG_VA7632B_CDC_REFPD_LOW_CDC_REFPD_MASK);
            }
            break;
        }
    g_VCDC_Output = output;
}
void va7632b_vol_shadow(uint8_t destreg, uint8_t curvol, uint8_t delaytime)
{
    UINT8 CurVol = 0;
    UINT8 CurVolL = 0, CurVolR = 0;
    UINT8 DestVolL = 0, DestVolR = 0;
    va76xx_read_reg(destreg, &CurVol);

        while (CurVol != curvol)
        {
            if (CurVol < curvol)
            {
                CurVol++;
            }
            else
            { //CurVolL > DestVolL
                CurVol--;
            }

            va76xx_write_reg(destreg, CurVol);
            VCOMMON_Delay(delaytime);
        }
}
void va7632b_input_gain(uint8_t gainvol)
{
    if ((g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD) != 0)
    { //#define CDC_DEALNOISE_VOLSHAD  1
        va7632b_vol_shadow(REG_VA7632B_CDC_ADVOLL_INIT, gainvol, g_VCDC_Config.adc_volset_delayus);
        va7632b_vol_shadow(REG_VA7632B_CDC_ADVOLR_INIT, gainvol, g_VCDC_Config.adc_volset_delayus); // the adc R at 7632B is invalid.
    }
    else
    {
        va76xx_write_reg(REG_VA7632B_CDC_ADVOLL_INIT, gainvol);
        va76xx_write_reg(REG_VA7632B_CDC_ADVOLR_INIT, gainvol); // the adc R at 7632B is invalid.
    }
}
//20190701xy, to config debug pin by eck, these only do config once at VSYS_SOC_InitCODEC and not change; at  adc&dac power up/down not change these registers.
//20191112xy, change the name from VCDC_ConfigDebug to VCDC_ConfigHW, do config codec(internal&external) after power on do config once, the first time;
void va7632b_debug_config(void)
{
    //set debug eck.
    //wujie:    a)  0x80---0x8A，0x8D，0x90---0x97,0x99,0x9B；这些debug 寄存器需要一直保持ECK的值；
    
    va76xx_write_reg(REG_VA7632B_CDC_PGA_CUR_DG, CDC_7632BA_PGA_CUR_DG); //0x80
    va76xx_write_reg(REG_VA7632B_CDC_ADCUR_DG, CDC_7632BA_ADCUR_DG);
    va76xx_write_reg(REG_VA7632B_CDC_REF_DG, CDC_7632BA_REF_DG);
    va76xx_write_reg(REG_VA7632B_CDC_VREF_SEL_DG, CDC_7632BA_VREF_SEL_DG);
    va76xx_write_reg(REG_VA7632B_CDC_VCOM_SEL_DG, CDC_7632BA_VCOM_SEL_DG);
    va76xx_write_reg(REG_VA7632B_CDC_CTSC_DG, CDC_7632BA_CTSC_DG);
    va76xx_write_reg(REG_VA7632B_CDC_MBSEL_DG, CDC_7632BA_MBSEL_DG);
    va76xx_write_reg(REG_VA7632B_CDC_REFPD_HIGH_DG, CDC_7632BA_REFPD_HIGH_DG);
    va76xx_write_reg(REG_VA7632B_CDC_DACCTL1_DG, CDC_7632BA_DACCTL1_DG);
    va76xx_write_reg(REG_VA7632B_CDC_DACCTL2_DG, CDC_7632BA_DACCTL2_DG);
    va76xx_write_reg(REG_VA7632B_CDC_MIX_DG, CDC_7632BA_MIX_DG);             //0x8A
    va76xx_write_reg(REG_VA7632B_CDC_ICTL_STBF_DG, CDC_7632BA_ICTL_STBF_DG); //0x8D
    va76xx_write_reg(REG_VA7632B_CDC_PWD_HFOUT_DG, CDC_7632BA_PWD_HFOUT_DG); //0x90
    va76xx_write_reg(REG_VA7632B_CDC_HP_ICTL_DG, CDC_7632BA_HP_ICTL_DG);
    va76xx_write_reg(REG_VA7632B_CDC_HPPD_OUT_DG, CDC_7632BA_HPPD_OUT_DG);
    va76xx_write_reg(REG_VA7632B_CDC_NMILLER_DG, CDC_7632BA_NMILLER_DG);
    va76xx_write_reg(REG_VA7632B_CDC_CBOOST_DG, CDC_7632BA_CBOOST_DG);
    va76xx_write_reg(REG_VA7632B_CDC_VGATE_SEL_DG, CDC_7632BA_VGATE_SEL_DG);
    va76xx_write_reg(REG_VA7632B_CDC_VSUB_HIGH_DG, CDC_7632BA_VSUB_HIGH_DG);
    va76xx_write_reg(REG_VA7632B_CDC_HPA_NATIVE_DG, CDC_7632BA_HPA_NATIVE_DG);           //0x97
    va76xx_write_reg(REG_VA7632B_CDC_LINEIN_AMP_VREF_DG, CDC_7632BA_LINEIN_AMP_VREF_DG); //0x99
    va76xx_write_reg(REG_VA7632B_CDC_HPAGAIN_MODE_DG, CDC_7632BA_HPAGAIN_MODE_DG);       //0x9B

    //at system power on need do audio power down;
    VCDC_ConfigOutput(CDC_OUTPUT_NONE,0);
    VCDC_ConfigInput(CDC_INPUT_NONE);
}

#if 0// VCODEC_HWGAINSYC_SUPPORTED
void va7632b_gainsync_advoll(uint8_t channel, uint8_t vol)
{
    if (channel & CDC_INPUT_MICL)
        {
            va76xx_write_reg(REG_VA7632B_CDC_ADVOLL_CUR, vol);
        }
        if (channel & CDC_INPUT_MICR)
        {
            va76xx_write_reg(REG_VA7632B_CDC_ADVOLR_CUR, vol); //the adc R at 7632B is invalid.
        }
}

//mode == 0 close the gain sync, ==1 only open dac gain sync, ==2 only open adc gain sync, ==3 open both dac&adc gain sync;
void va7632b_gainsync_mode(uint8_t mode)
{
     if ((mode & 0x01) != 0)
    {
        va76xx_write_reg(REG_VA7632B_CDC_LINEIN_AMP_PD_DG, 0x00); //open the Linein_Amp_Det;
    }
    else
    {
        va76xx_write_reg(REG_VA7632B_CDC_LINEIN_AMP_PD_DG, 0x01); //close the  Linein_Amp_Det;
    }

    if (mode == 0)
    { //close gain sync, as default;
        //“并行”传输模式
        va76xx_write_reg(REG_VA7632B_CDC_SERD_EN, 0x10);
        va76xx_write_reg(REG_VA7632B_CDC_DATA_RSTN, 0x00);
    }
    else if (mode == 1)
    { //dac gain sync
        //“串行”传输模式（增益同步模式）
        va76xx_write_reg(REG_VA7632B_CDC_SERD_EN, 0x15);
        va76xx_write_reg(REG_VA7632B_CDC_DATA_RSTN, 0x01);
    }
    else if (mode == 2)
    { //adc gain sync
        va76xx_write_reg(REG_VA7632B_CDC_SERD_EN, 0x0A);
        va76xx_write_reg(REG_VA7632B_CDC_DATA_RSTN, 0x02);
        va76xx_write_reg(REG_VA7632B_CDC_PGA_GAIN_CONF, 0x05);
        //va76xx_write_reg ( REG_VA7632B_CDC_W_SEL_ADC, 0x00 );
    }
    else if (mode == 3)
    { //dac&adc gain sync
        va76xx_write_reg(REG_VA7632B_CDC_SERD_EN, 0x0F);
        va76xx_write_reg(REG_VA7632B_CDC_DATA_RSTN, 0x03);
        va76xx_write_reg(REG_VA7632B_CDC_PGA_GAIN_CONF, 0x05);
        va76xx_write_reg(REG_VA7632B_CDC_W_SEL_ADC, 0x00);
    }
    else
    {
        ;
    }
}
#endif

void va7632b_output_gain(uint8_t gainvol)
{
     if (VCDC_Get7621GainMode() & 0x01)
        {
            va76xx_write_reg(REG_VA7632B_CDC_HPVOL_6BIT_L, gainvol);
            va76xx_write_reg(REG_VA7632B_CDC_HPVOL_6BIT_R, gainvol);
        }
        else
        {
            va76xx_write_reg(REG_VA7632B_CDC_HPVOL_4BIT, ((gainvol << 4) | (gainvol)));
        }
}
void va7632b_output_gain_6bit_l(uint8_t gainvol)
{
    va76xx_write_reg(REG_VA7632B_CDC_HPVOL_6BIT_L, gainvol);
}
void va7632b_output_gain_6bit_r(uint8_t gainvol)
{
    va76xx_write_reg(REG_VA7632B_CDC_HPVOL_6BIT_R, gainvol);
}
void va7632b_output_gain_4bit_l(uint8_t gainvol)
{
    uint8_t vol;
    va76xx_read_reg(REG_VA7632B_CDC_HPVOL_4BIT, &vol);

    vol = (vol & 0xF0) | gainvol;
    va76xx_write_reg(REG_VA7632B_CDC_HPVOL_4BIT, vol);
}
void va7632b_output_gain_4bit_r(uint8_t gainvol)
{
    uint8_t vol;
    va76xx_read_reg(REG_VA7632B_CDC_HPVOL_4BIT, &vol);
    vol = (vol & 0x0F) | (gainvol << 4);
    va76xx_write_reg(REG_VA7632B_CDC_HPVOL_4BIT, vol);
}
int va7632b_get_output_gain_4bit(uint8_t *gainvol)
{
    return va76xx_read_reg(REG_VA7632B_CDC_HPVOL_4BIT, gainvol);
}
void va7632b_output_gain_4bit(uint8_t gainvol)
{
    va76xx_write_reg(REG_VA7632B_CDC_HPVOL_4BIT, gainvol);
}
void va7632b_pdout_open(void)
{
        //va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG); // CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
        //va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG); // CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
    if ( g_VCDC_Output & CDC_OUTPUT_HPL ) {
        va76xx_write_reg ( REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG );    //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
    }
    if ( g_VCDC_Output & CDC_OUTPUT_HPR ) {
        va76xx_write_reg ( REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG );    //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
    }    
}

void va7632b_pdout_close(void)
{
     va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, 0x00); //CDC_PD_OUT0I_R=0, CDC_PD_OUT1I_R=0, CDC_PD_OUT2I_R=0
     va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, 0x00); //CDC_PD_OUT0I_L=0, CDC_PD_OUT1I_L=0, CDC_PD_OUT2I_L=0
}
uint8_t va7632b_get_dac_channel(void)
{
    return g_VCDC_Config.dac_chn;
}

#define CDC_7632BA_PDOUT_MAXGAIN_THRESHOLD (g_VCDC_Config.cdc_powerup1_delay1)
void va7632b_pdout_config(uint16_t gain)
{
     /* by wujie 20190904
    寄存器0x8B，0x8C寄存器的调整方案：
    1）在ECK中增加一个寄存器，作为增益的阈值
    2）初始化的时候，判断实际总增益是否大于阈值；如果大于，那么，0x8B，0x8C写ECK中的值；如果小于，那么，差分电路：0x8B=0x11，0x8C=0x11；单端电路：0x8B=0x01，0x8C=0x01；
    3）在增益发生变化的时候，同样：判断实际总增益是否大于阈值；如果大于，那么，0x8B，0x8C写ECK中的值；如果小于，那么，差分电路：0x8B=0x11，0x8C=0x11；单端电路：0x8B=0x01，0x8C=0x01；
    */
    //only for VA7632B
    //the Gain = DigitGain+AnalogGain
    if (gain > CDC_7632BA_PDOUT_MAXGAIN_THRESHOLD)
    {
        va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
        va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
    }
    else
    {
        
            va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_0); //va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, 0x01);
            va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_0); //va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, 0x01);
    }
}

uint8_t va7632b_get_dacstatus(void)
{
    return g_VCDC_Output;
}

uint8_t va7632b_get_adcstatus(void)
{
    return g_VCDC_Input;
}























void VCDC_ConfigInput(UINT8 input)
{
    va7632b_input_config(input);
}

void VCDC_ConfigOutput(UINT8 output,UINT8 mode)
{
    va7632b_output_config(output, mode);
}
void VCDC_SetInputGain(UINT8 gainvol)
{
   va7632b_input_gain(gainvol);
}

#if VCODEC_HWGAINSYC_SUPPORTED
void VCDC_GainSync_SetADVOL(UINT8 AdcChn, UINT8 LVol, UINT8 RVol)
{
    
    if (AdcChn & CDC_INPUT_MICL)
        {
            va7632b_gainsync_advoll(AdcChn, LVol);
        }
        if (AdcChn & CDC_INPUT_MICR)
        {
            va7632b_gainsync_advoll(AdcChn, RVol); //the adc R at 7632B is invalid.
        }
}

void VCDC_GainSyncSet(UINT8 mode)
{
   va7632b_gainsync_mode(mode);
}
#endif

void VCDC_ConfigHW(void)
{
    va7632b_debug_config();
}

void VCDC_SetOutputGain(UINT8 gainvol)
{
   va7632b_output_gain(gainvol);
}

void VCDC_7621_PA_Opne(void)
{
    va7632b_pdout_open();
    //  if (g_VCDC_Output & CDC_OUTPUT_HPL)
    //     {
    //         va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
    //     }
    //     if (g_VCDC_Output & CDC_OUTPUT_HPR)
    //     {
    //         va76xx_write_reg(REG_VA7632B_CDC_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
    //     }
}

void VCDC_7621_PA_Close(void)
{
    va7632b_pdout_close();
}

void VCDC_Init(void)
{
    g_VCDC_Input = CDC_INPUT_NONE;
    g_VCDC_Output = CDC_OUTPUT_NONE;
}

UINT8 VCDC_GetDACChan(void)
{
    //return gva7632b_get_dac_channel();
        return 1;
}

void VCDC_PDOUTConfigByGain(SINT16 gain)
{
   va7632b_pdout_config(gain);
}

UINT8 VCDC_Get7621GainMode(void)
{
     //if ((VCDC_GetVerNo() == CDC_VERSION_7621SD_C0) || (VCDC_GetVerNo() == CDC_VERSION_7632B_A0) || (VCDC_GetVerNo() == CDC_VERSION_7632B_E3))
     {
         if (CDC_7621SD_HPAGAIN_MODE & 0x01)
         { //0.5db
             return 1;
         }
         else
         {
             return 0;
         }
     }
     //else
     {
     //    return 0; //other 7621 2.0db
     }
}

UINT8 VCDC_GetHPVOL_7621SDC0(SINT16 Gain) //0.1db
{
    
    // UINT8 i = 0;
    //     SINT8 AnalogGain = Gain / 5;
    //     if (CDC_7621SD_HPAGAIN_MODE & 0x01)
    //     { //0.5db
    //         for (i = 0; i < VCDC_7621SDC0_GAIN_NUM; i++)
    //         {
    //             if (AnalogGain <= g_7621SDC0_GainTable[i].gain_db)
    //             {
    //                 return g_7621SDC0_GainTable[i].HP_val;
    //             }
    //         }
    //     }
    //     else
    //     { // 2.0db, CDC_HPVOL_R[3:0] control both.
    //         UINT8 HP_val = (AnalogGain - (-18) * 2) / 4;
    //         return (HP_val << 4);
    //     }
    // return 0;
}
//Gain: input gain 0.1db; return g_codec_hw.dac.vol for 0.5db 7621SD or 7632B and 0.1db others.
UINT8 VCDC_GetHPVOL(SINT16 Gain)
{
    if ( VCDC_Get7621GainMode() & 0x01 ) {
        return ( Gain - ( -18 ) * 10 ) / 5; //0.5db
    } else {//other 7621
        return ( Gain - ( -18 ) * 10 ) / 20; //2.0db
    }
}


// UINT8 g_7621Reg_Value[128] = {0};

/******************************************************************************
Description:
    make the volume shadow(descending or increasing).

Parameters:
    DestReg: change volume register.(dac-CDC_HPVOL,adc-CDC_ADVOL)
    DestVol: destination volume to change;[3:0]-left vol, [7:4]-right vol
    DelayTime: change volume register, MicroSec(us).

Return Value:
    void.

*****************************************************************************/
void VCDC_VolShadow(UINT8 DestReg, UINT8 DestVol, UINT16 DelayTime)
{
   va7632b_vol_shadow( DestReg, DestVol, DelayTime);
}

/******************************************************************************
Description:
    Get the current working state of 7621 audio Driver.

Parameters:
    void.

Return Value:
    UINT8 - the cdc state

Remarks:
    .
*****************************************************************************/
UINT8 VCDC_GetDACStat(void)
{
    return g_VCDC_Output;
}

UINT8 VCDC_GetADCStat(void)
{
    return g_VCDC_Input;
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
UINT8 VCDC_GetVerNo(void)
{
    #ifdef FPGA_QA_BUILD
    return CDC_VERSION_DEFAULT;//g_VCDC_Config.cdc_ver; //CDC_VERSION_DEFAULT
        #else
        return CDC_VERSION_DEFAULT;//g_VCDC_Config.cdc_ver; //
        #endif
}

//0==not I2S, 1==7621I2S-Dac,
UINT8 VCDC_I2S_GetMode(void)
{
    return 0;
}

//FOUT_EN[4:3] pll_ref_src_sel[1:0], referecne clock selection: 0==dac clock, 1==adc clock, 2==hd_scl
//return value: 0==adc clock, 1==dac clock, 2==hd_scl;
UINT8 VCDC_I2S_GetRefClkSel(void)
{
    return g_VCDC_I2S_Config.i2s_clk_sel;
}
void VCDC_SetADCVol(UINT8 AdcChn, UINT8 LVol, UINT8 RVol)
{
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD  1
        if ( AdcChn & CDC_INPUT_MICL ) {
            VCDC_VolShadow ( REG_VA7632B_CDC_ADVOLL_INIT, LVol, g_VCDC_Config.adc_volset_delayus );
        }
        if ( AdcChn & CDC_INPUT_MICR ) {
            VCDC_VolShadow ( REG_VA7632B_CDC_ADVOLR_INIT, RVol, g_VCDC_Config.adc_volset_delayus );    //the adc R at 7632B is invalid.
        }
    } else {
        if ( AdcChn & CDC_INPUT_MICL ) {
            va76xx_write_reg ( REG_VA7632B_CDC_ADVOLL_INIT, LVol );
        }
        if ( AdcChn & CDC_INPUT_MICR ) {
            va76xx_write_reg ( REG_VA7632B_CDC_ADVOLR_INIT, RVol );    //the adc R at 7632B is invalid.
        }
    }
}

/******************************************************************************
Description:
        cdc driver for extern codec(dsp&dac&adc)  control.

Parameters:
        UINT8 Mode: ==0 for ex-codec power on/off, ==1 for ex-codec init/uninit(had power on); 
        UINT8 Ctrl:==0 for ex-codec close, ==1 for ex-codec open;

Return Value:
        .
*****************************************************************************/
UINT8 VCDC_ExCodecConfig ( UINT8 Mode, UINT8 Ctrl )
{
        // VCOMMON_NPRINTF("VCDC_ExCodecConfig=%d-%d[stat=%x][Ret=0x%08X].\r\n", Ctrl, Mode, VCODEC_GetState(), __GET_RETURN_ADDR());

        // if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S) {
        //      if(Mode == 0){//for ex-codec power on/off
        //              if(Ctrl == 0){
        //                      //for power off, to SYS_PMGR_STAT_PWRSAVE
        //                      VCDC_I2S_Clock_En(FALSE);//close IIS for low power; 
        //              } else {
        //                      //for power on, to SYS_PMGR_STAT_ACTIVE
        //                      VCDC_I2S_Clock_En(TRUE);
        //                      /*// move these operations into codec adapter init to fix the BUG of noise aftre boot's tone.
        //                      VCOMMON_NPRINTF("bb2dminit\r\n");
        //                      Init_DAC_ADC();
        //                      Init_DSP();
        //                      AKMSwitchInputSrc(FALSE);
        //                      AKMAuxBTBalance(TRUE);*/
        //                      Init_7755DSP();
        //                      Init_5121DAC_5140ADC();
        //              }
        //      } else if(Mode == 1) {//for ex-codec init/uninit(had power on)
        //              if(Ctrl == 0){
        //                      //for uninit, not work
        //                      AKMSetVolume(0);
        //                      //AKMAuxBTBalance(TRUE);
        //              } else {
        //                      //for init, will work;
        //                      #if 1//20191117xy, optimization and discuss with zhouhuijun;
        //                      if(VCODEC_GetState() == VCODEC_STATE_LINEIN_PLAYING){
        //                              AKMAuxBTBalance(FALSE);
        //                      } else {
        //                              AKMAuxBTBalance(TRUE);
        //                      }
        //                      #else
        //                      AKMdsp_ReConfig7755(0);//VCODEC_Adatper_Config_ExtDSP(p_HwInitParam->dac_gain);  //??
        //                      Config_5121DAC_5140ADC();       //VCODEC_Adatper_Config_ExtAdcDac();
                                
        //                      //here will be config by music or Aux state diff;
        //                      AKMAuxBTBalance(TRUE);
        //                      #endif
        //              }
        //      } else {
        //              VCOMMON_EPRINTF("VCDC_ExCodecConfig param[%d] err!\r\n", Mode);
        //      }
        // }
        // return 0;
}

UINT8 VCDC_ExCodecCheck (  UINT8 Mode )
{
    return 0;
}
void VCDC_MuteOutput ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else {
        switch ( g_VCDC_Output ) {
            case CDC_OUTPUT_HP:
                // if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
                //     /* mute PA */
                //     VCDC_WriteReg ( CDC_REGS_HP0, ( 0x44 |  CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );
                //     /* mute MIXER */
                //     if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
                //         VCDC_WriteReg ( CDC_REGS_MIX, ( 0xCC | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                //     } else {
                //         VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                //     }
                // } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 ) {
                //     ;
                // } else {
                //     VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                // }
                break;
        }
    }
}

void VCDC_UnmuteOutput ( void )
{

}
void VCDC_MuteInput ( void )
{

}
void VCDC_UnmuteInput ( void )
{

}

void VCDC_CloseMBias ( void )
{
    UINT8 GainVal = 0;

    va76xx_write_reg ( REG_VA7632B_CDC_REFPD_LOW, 0x08 ); //the output is power on only for dac, close Mbias
}

void VCDC_I2S_Clock_En(BOOL CLK_Open)
{
//     VRET_VALUE ret;
//     UINT8 val;
//     ret = va76xx_read_reg(CDC_REGS_SPORT_MODE, &val);

//     if (TRUE == CLK_Open)
//     {
// #if VDEV_HDS_SUPPORTED
//         if (VRET_NO_ERROR == ret)
//         {
//             // set I2S Master mode
//             val &= ~(HDS_SM_I2S_MODE);
//             va76xx_write_reg(CDC_REGS_SPORT_MODE, val);
//         }
// #endif // end add 19-5-14

//         va76xx_write_reg(CDC_REGS_I2S_CTRL, g_VCDC_I2S_Config.i2s_ctrl);
//         // change pll frq up, to reduce jitter
//         if (g_VCDC_I2S_Config.i2s_clk_sel != 2)
//         {
//             //for 6M clock
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_N_LOW, 0x00);
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_N_HIGH, 0x02);
//         }
//         else
//         {
//             //for hd 16M clock; // change pll frq ref hds clock 16M
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_N_LOW, 0xC0);  // N low
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_N_HIGH, 0x00); // N high
//         }
//         va76xx_write_reg(CDC_REGS_PLL_CTRL_R, 0x05); //use default value is ok;

//         if (0 /*32000 == VCODEC_HW_GetDACSampleRate()*/)
//         {
//             //if choose dac as IIS output clock and DACSampleRate =32K then the dac clock is 4M(not 6M) so will config it;
//             va76xx_write_reg(CDC_REGS_PLLOUT_DIV, 150); // for 32k test
//         }
//         else
//         {
//             va76xx_write_reg(CDC_REGS_PLLOUT_DIV, 100);
//         }
//         // end change pll frq up

//         if (g_VCDC_I2S_Config.i2s_clk_sel == 1)
//         { //dac out IIS clock
//             va76xx_write_reg(CDC_REGS_FOUT_EN, 0x04);
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_OTHERS, 0x10);
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_OTHERS, 0x01);
//             va76xx_write_reg(CDC_REGS_PLLDIV_OEN, 0x01);
//             va76xx_write_reg(CDC_REGS_FOUT_EN, 0x07); // ref dac clock to PLL
//                                                    //        va76xx_write_reg(0x41, 0xb8);
//         }
//         else if (g_VCDC_I2S_Config.i2s_clk_sel == 0)
//         { //adc out IIS clock
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_OTHERS, 0x01);
//             va76xx_write_reg(CDC_REGS_PLLDIV_OEN, 0x01);
//             va76xx_write_reg(CDC_REGS_FOUT_EN, 0x0f); // ref Adc clock to PLL
//         }
//         else if (g_VCDC_I2S_Config.i2s_clk_sel == 2)
//         { //hd out IIS clock
//             va76xx_write_reg(CDC_REGS_PLL_CTRL_OTHERS, 0x01);
//             va76xx_write_reg(CDC_REGS_PLLDIV_OEN, 0x01);
//             va76xx_write_reg(CDC_REGS_FOUT_EN, 0x17); // ref hd clock to PLL
//         }

//         VCOMMON_Delay(10000);
//         // 2019-11-16 by chen jh, remove becuse Ex Codec 7755 & Ex adc dac Not powor on when INIT soc.
//         //VCDC_ExCodecConfig(1);
//         //VCOMMON_NPRINTF("VCDC_configHW.\r\n");
//     }
//     else
//     {
//         UINT8 I2sCtrlVal = 0, i = 0;
//         if (!g_VCDC_I2S_Config.i2s_ctrl)
//         {
//             return;
//         }

//         if (g_VCDC_I2S_Config.i2s_clk_sel == 2)
//         {
//             I2sCtrlVal = g_VCDC_I2S_Config.i2s_ctrl;
//             I2sCtrlVal &= ~(BIT5 | BIT6); // close i2s RX and TX enable
//             va76xx_write_reg(CDC_REGS_I2S_CTRL, I2sCtrlVal);

//             if (g_VCDC_I2S_Config.i2s_clk_sel == 2)
//             {
//                 // disable FOUT_EN: pll and decim off
//                 va76xx_write_reg(CDC_REGS_FOUT_EN, 0x04);
//                 va76xx_write_reg(CDC_REGS_PLLDIV_OEN, 0x00);
//                 va76xx_write_reg(CDC_REGS_PLL_CTRL_OTHERS, 0x10);
//             }
//         }

// #if VDEV_HDS_SUPPORTED
//         if (VRET_NO_ERROR == ret)
//         {
//             // set I2S Slave mode, bclk, lr_clk output low.
//             val |= HDS_SM_I2S_MODE;
//             va76xx_write_reg(CDC_REGS_SPORT_MODE, val);
//         }
// #endif // end add 19-5-14
//     }
}
