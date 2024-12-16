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
#include "vstddrv.h"

#if  VDEV_AUDIOBROADCAST_SUPPORTED
extern UINT8 VCODEC_MUSIC_CheckSelAuxGain(void);
#endif

VCDC_CONFIG g_VCDC_Config = {0};
UINT8 g_VCDC_Input;
UINT8 g_VCDC_Output;

//for common new 7621 config
VCDC_PLUS_CONFIG g_VCDC_Plus_Config = {0};

#if 1//VDEV_7621_I2S_EN
//for common new 7621I2S config
VCDC_I2S_CONFIG g_VCDC_I2S_Config = {0x02,0xF0,0x10};//{0x02,0xF0,0x01};//0xE8,0xB8,0xF8(rxtx24bit)
#endif

extern BOOL VCODEC_Check_PopBetter_Enable ( void );

#if defined (HW_CDC_DEBUG)
    VCDC_DEBUG g_VCDC_Debug;
    //9611
    #define CDC_9611_ADCURL             (g_VCDC_Debug.adc_cur_l)
    #define CDC_9611_ADCURR             (g_VCDC_Debug.adc_cur_r)
    #define CDC_9611_CTBUFFICTL         (g_VCDC_Debug.ctbuff_ictl)
    #define CDC_9611_DACDEBUG           (g_VCDC_Debug.dac_debug_l)//(g_VCDC_Debug.dac_debug)
    #define CDC_9611_DACMODE            (g_VCDC_Debug.dac_mode_l)//(g_VCDC_Debug.dac_mode)
    #define CDC_9611_DACDISDWA          (g_VCDC_Debug.dac_debug_r)//(g_VCDC_Debug.dac_dis_dwa)
    #define CDC_9611_DACCTL             (g_VCDC_Debug.dac_ctl_l)//(g_VCDC_Debug.dac_ctl)
    #define CDC_9611_DACLPFICTL         (g_VCDC_Debug.dac_lpfictl_l)//(g_VCDC_Debug.dac_lpf_ictl)
    #define CDC_9611_HPDOCP             (g_VCDC_Debug.hp_docp_r)//(g_VCDC_Debug.hp_doc_p)
    #define CDC_9611_HPDOCN             (g_VCDC_Debug.hp_docn_l)//(g_VCDC_Debug.hp_doc_n)
    #define CDC_9611_HPPOP              (g_VCDC_Debug.dac_mode_r)//(g_VCDC_Debug.hp_pop)
    #define CDC_9611_HPOC               (g_VCDC_Debug.dac_ctl_r)//(g_VCDC_Debug.hp_oc)
    #define CDC_9611_HPLP               (g_VCDC_Debug.hp_lp_l)//(g_VCDC_Debug.hp_lp)
    #define CDC_9611_MBSEL              (g_VCDC_Debug.dac_lpfictl_r)//(g_VCDC_Debug.mb_sel)
    #define CDC_9611_MIXICTL            (g_VCDC_Debug.mix_ictl_l)//(g_VCDC_Debug.mix_ictl)
    #define CDC_9611_REFVRFSEL          (g_VCDC_Debug.ref_vrf_sel)
    #define CDC_9611_REFIOPT            (g_VCDC_Debug.ref_iopt)
    #define CDC_9611_SCBUFFICTL         (g_VCDC_Debug.scbuff_ictl)
    //9611s
    #define CDC_9611S_ADCURL            (g_VCDC_Debug.adc_cur_l)
    #define CDC_9611S_ADCURR            (g_VCDC_Debug.adc_cur_r)
    #define CDC_9611S_CTBUFFICTL        (g_VCDC_Debug.ctbuff_ictl)
    #define CDC_9611S_DACDEBUG_L        (g_VCDC_Debug.dac_debug_l)
    #define CDC_9611S_DACDEBUG_R        (g_VCDC_Debug.dac_debug_r)
    #define CDC_9611S_DACMODE_L         (g_VCDC_Debug.dac_mode_l)
    #define CDC_9611S_DACMODE_R         (g_VCDC_Debug.dac_mode_r)
    #define CDC_9611S_DACCTL_L          (g_VCDC_Debug.dac_ctl_l)
    #define CDC_9611S_DACCTL_R          (g_VCDC_Debug.dac_ctl_r)
    #define CDC_9611S_DACLPFICTL_L      (g_VCDC_Debug.dac_lpfictl_l)
    #define CDC_9611S_DACLPFICTL_R      (g_VCDC_Debug.dac_lpfictl_r)
    #define CDC_9611S_HPDOCN_L          (g_VCDC_Debug.hp_docn_l)
    #define CDC_9611S_HPDOCN_R          (g_VCDC_Debug.hp_docn_r)
    #define CDC_9611S_HPDOCP_L          (g_VCDC_Debug.hp_docp_l)
    #define CDC_9611S_HPDOCP_R          (g_VCDC_Debug.hp_docp_r)
    #define CDC_9611S_HPLP_L            (g_VCDC_Debug.hp_lp_l)//for CDC_HPOC_L(9611SD)
    #define CDC_9611S_HPLP_R            (g_VCDC_Debug.hp_lp_r)//for CDC_HPOC_R(9611SD)
    #define CDC_9611S_MIXICTL_L         (g_VCDC_Debug.mix_ictl_l)
    #define CDC_9611S_MIXICTL_R         (g_VCDC_Debug.mix_ictl_r)
    #define CDC_9611S_REFVRFSEL         (g_VCDC_Debug.ref_vrf_sel)
    #define CDC_9611S_REFIOPT           (g_VCDC_Debug.ref_iopt)
    #define CDC_9611S_VCMSEL            (g_VCDC_Debug.vcm_sel)
    #define CDC_9611S_SCBUFFICTL        (g_VCDC_Debug.scbuff_ictl)
    //9611S_B
    //#define CDC_9611SB_POP_DEBUG1         (g_VCDC_Debug.pop_debug1)
    //#define CDC_9611SB_POP_DEBUG2         (g_VCDC_Debug.pop_debug2)
    //#define CDC_9611SB_INITEN         (g_VCDC_Debug.pop_initen)
    //9611S_C
    //#define CDC_9611SC_SOFTSTART      (g_VCDC_Debug.pop_debug1)//(g_VCDC_Debug.soft_start)
    //#define CDC_9611SC_INITEN             (g_VCDC_Debug.pop_debug2)//(g_VCDC_Debug.pop_initen)

    //9611S_D
    #define CDC_9611SD_HPPOP            (g_VCDC_Debug.hp_pop)
    #define CDC_9611SD_MBSEL            (g_VCDC_Debug.mb_sel)
    #define CDC_9611SD_POP_ICTL1        (g_VCDC_Debug.pop_debug2)//(g_VCDC_Debug.pop_ictl1)
    #define CDC_9611SD_POP_ICTL2        (g_VCDC_Debug.pop_initen)//(g_VCDC_Debug.pop_ictl2)
    #define CDC_9611SD_PD_OUTXI         (g_VCDC_Debug.pop_debug1)//(g_VCDC_Debug.pd_outxI)
    #define CDC_9611SD_POP_SW_EN        (g_VCDC_Debug.pop_sw_en)
    #define CDC_9611SD_PD_COMP_L        (g_VCDC_Debug.pd_comp_l)
    #define CDC_9611SD_PD_COMP_R        (g_VCDC_Debug.pd_comp_r)
    #define CDC_9611SD_DFF_SET_L        (g_VCDC_Debug.dff_set_l)
    #define CDC_9611SD_DFF_SET_R        (g_VCDC_Debug.dff_set_r)
    #define CDC_9611SD_BYPASS_L         (g_VCDC_Debug.bypass_l)
    #define CDC_9611SD_BYPASS_R         (g_VCDC_Debug.bypass_r)
    #define CDC_9611SD_INIT_EN_L        (g_VCDC_Debug.init_en_l)
    #define CDC_9611SD_INIT_EN_R        (g_VCDC_Debug.init_en_r)
    #define CDC_9611SD_PD_CHG_L         (g_VCDC_Debug.pd_chg_l)
    #define CDC_9611SD_PD_CHG_R         (g_VCDC_Debug.pd_chg_r)
    #define CDC_9611SD_PD_STBF          (g_VCDC_Debug.pd_stbf)
    #define CDC_9611SD_PREC_EN          (g_VCDC_Debug.prec_en)
    #define CDC_9611SD_PD_LEVEL         (g_VCDC_Debug.pd_level)
    #define CDC_9611SD_PD_BF_HPOUT      (g_VCDC_Debug.pd_bf_hpout)

    //9611S_E
    #define CDC_9611SE_D2S              (g_VCDC_Debug.d2s)

    //7621A
    #define CDC_7621A_HPPOP             (g_VCDC_Debug.hp_pop)
    #define CDC_7621A_HP_PULLDOWN_EN    (g_VCDC_Debug.pop_sw_en) //(g_VCDC_Debug.hp_pulldown_en)
    #define CDC_7621A_PWD_HFOUT         (g_VCDC_Debug.pop_debug1)//(g_VCDC_Debug.pwd_hfout)
    #define CDC_7621A_PD_CHARGE         (g_VCDC_Debug.pop_initen)//(g_VCDC_Debug.pd_charge)
    #define CDC_7621A_PD_OUT            (g_VCDC_Debug.pop_debug2)//(g_VCDC_Debug.pd_out)//
    #define CDC_7621A_PD_FD             (g_VCDC_Debug.pd_chg_r)  //(g_VCDC_Debug.pd_fd)
    #define CDC_7621A_REFPD_HIGH        (g_VCDC_Debug.pd_comp_r) //(g_VCDC_Debug.refpd_high)
    #define CDC_7621A_REFPD_LOW         (g_VCDC_Debug.pd_comp_l) //(g_VCDC_Debug.refpd_low)
    //VA7621S, VA7621G
    #define CDC_7621A_PD_OUT_R          (g_VCDC_Debug.dff_set_r)//(g_VCDC_Debug.pd_out_r)
    #define CDC_7621A_PD_OUT_L          (g_VCDC_Debug.dff_set_l)//(g_VCDC_Debug.pd_out_l)
    #define CDC_7621A_CDC_NMILLER       (g_VCDC_Debug.pop_debug2)//(g_VCDC_Debug.nmiller)
    #define CDC_7621A_RESERVE           (g_VCDC_Debug.bypass_l)  //(g_VCDC_Debug.reserve_s)
    #define CDC_7621A_PGA_CUR           (g_VCDC_Debug.bypass_r) //(g_VCDC_Debug.pga_cur)
    #define CDC_7621A_HP_ICTL           (g_VCDC_Debug.init_en_l)//(g_VCDC_Debug.hp_ictl)
    #define CDC_7621A_ICTL_STBF         (g_VCDC_Debug.init_en_r)//(g_VCDC_Debug.ictl_stbf)
    #define CDC_7621A_HPPD_OUT          (g_VCDC_Debug.pd_chg_l)//(g_VCDC_Debug.hppd_out)
    //VA7621SD
    #define CDC_7621SD_HPAGAIN_MODE     (g_VCDC_Debug.hpagain_mode)

    //VA7632BA0
    #define CDC_7632BA_PD_OUT_L_DG      (g_VCDC_Debug.dac_debug_l)
    #define CDC_7632BA_PD_OUT_R_DG      (g_VCDC_Debug.dac_debug_r)
    #define CDC_7632BA_PD_CHARGE_DG     (g_VCDC_Debug.pop_initen)

    #define CDC_7632BA_PGA_CUR_DG           (g_VCDC_Debug.dg_pga_cur)
    #define CDC_7632BA_ADCUR_DG             (g_VCDC_Debug.dg_adcur)
    #define CDC_7632BA_REF_DG               (g_VCDC_Debug.dg_ref)
    #define CDC_7632BA_VREF_SEL_DG          (g_VCDC_Debug.dg_vref_sel)
    #define CDC_7632BA_VCOM_SEL_DG          (g_VCDC_Debug.dg_vcom_sel)
    #define CDC_7632BA_CTSC_DG              (g_VCDC_Debug.dg_ctsc)
    #define CDC_7632BA_MBSEL_DG             (g_VCDC_Debug.dg_mbsel)
    #define CDC_7632BA_REFPD_HIGH_DG        (g_VCDC_Debug.dg_refpd_high)
    #define CDC_7632BA_DACCTL1_DG           (g_VCDC_Debug.dg_dacctl1)
    #define CDC_7632BA_DACCTL2_DG           (g_VCDC_Debug.dg_dacctl2)
    #define CDC_7632BA_MIX_DG               (g_VCDC_Debug.dg_mix)
    #define CDC_7632BA_ICTL_STBF_DG         (g_VCDC_Debug.dg_ictl_stbf)
    #define CDC_7632BA_PWD_HFOUT_DG         (g_VCDC_Debug.dg_pwd_hfout)
    #define CDC_7632BA_HP_ICTL_DG           (g_VCDC_Debug.dg_hp_ictl)
    #define CDC_7632BA_HPPD_OUT_DG          (g_VCDC_Debug.dg_hppd_out)
    #define CDC_7632BA_NMILLER_DG           (g_VCDC_Debug.dg_nmiller)
    #define CDC_7632BA_CBOOST_DG            (g_VCDC_Debug.dg_cboost)
    #define CDC_7632BA_VGATE_SEL_DG         (g_VCDC_Debug.dg_vgate_sel)
    #define CDC_7632BA_VSUB_HIGH_DG         (g_VCDC_Debug.dg_vsub_high)
    #define CDC_7632BA_HPA_NATIVE_DG        (g_VCDC_Debug.dg_hpa_native)
    #define CDC_7632BA_LINEIN_AMP_VREF_DG   (g_VCDC_Debug.dg_linein_amp_vref)
    #define CDC_7632BA_HPAGAIN_MODE_DG      (g_VCDC_Debug.hpagain_mode)

    #define CDC_7632BA_PD_OUT_L_0           (g_VCDC_Debug.dac_mode_l)
    #define CDC_7632BA_PD_OUT_R_0           (g_VCDC_Debug.dac_mode_r)
#else
    //9611
    #define CDC_9611_ADCURL                 0x03
    #define CDC_9611_ADCURR                 0x30
    #define CDC_9611_CTBUFFICTL             0x00
    #define CDC_9611_DACDEBUG               0x00
    #define CDC_9611_DACMODE                0x00
    #define CDC_9611_DACDISDWA              0x00
    #define CDC_9611_DACCTL                 0x00
    #define CDC_9611_DACLPFICTL             0x00
    #define CDC_9611_HPDOCP                 0xF0//0xC0
    #define CDC_9611_HPDOCN                 0x0F//0x0C
    #define CDC_9611_HPPOP                  0x00
    #define CDC_9611_HPOC                   0x00
    #define CDC_9611_HPLP                   0x03
    #define CDC_9611_MBSEL                  0x02
    #define CDC_9611_MIXICTL                0x00
    #define CDC_9611_REFVRFSEL              0x00
    #define CDC_9611_REFIOPT                0x02
    #define CDC_9611_SCBUFFICTL             0x00
    //9611s
    #define CDC_9611S_ADCURL                0x05//for 7621      //0x03
    #define CDC_9611S_ADCURR                0x50//for 7621      //0x30
    #define CDC_9611S_CTBUFFICTL            0x00
    #define CDC_9611S_DACDEBUG_L            0x00
    #define CDC_9611S_DACDEBUG_R            0x00
    #define CDC_9611S_DACMODE_L             0x00
    #define CDC_9611S_DACMODE_R             0x00
    #define CDC_9611S_DACCTL_L              0x00
    #define CDC_9611S_DACCTL_R              0x00
    #define CDC_9611S_DACLPFICTL_L          0x00
    #define CDC_9611S_DACLPFICTL_R          0x00
    #define CDC_9611S_HPDOCN_L              0x0C
    #define CDC_9611S_HPDOCN_R              0xC0
    #define CDC_9611S_HPDOCP_L              0x0C
    #define CDC_9611S_HPDOCP_R              0xC0
    #define CDC_9611S_HPLP_L                0x03
    #define CDC_9611S_HPLP_R                0x30
    #define CDC_9611S_MIXICTL_L             0x00
    #define CDC_9611S_MIXICTL_R             0x00
    #define CDC_9611S_REFVRFSEL             0x00
    #define CDC_9611S_REFIOPT               0x02
    #define CDC_9611S_VCMSEL                0x10
    #define CDC_9611S_SCBUFFICTL            0x00

    //9611S_B
    //#define CDC_9611SB_POP_DEBUG1             0x70//for (CDC_9611SB_STICTL|CDC_9611SB_SDICTL|CDC_9611SB_PD_OUT1I|CDC_9611SB_PD_OUT2I|CDC_9611SB_PD_OUT4I)
    //#define CDC_9611SB_POP_DEBUG2             0x03
    //#define CDC_9611SB_INITEN                 0x80//CDC_POP_DEBUG1[7]
    //9611S_C
    //#define CDC_9611SC_SOFTSTART          0x70//0x00//for (CDC_9611SC_STICTL|CDC_9611SC_PD_OUT1I|CDC_9611SC_PD_OUT2I|CDC_9611SC_PD_OUT4I|CDC_9611SC_POP_BYPASS)
    //#define CDC_9611SC_INITEN                 0x00//CDC_INITEN,CDC_SOFTSTART[2]
    //#define CDC_9611SC_STICTL                 0x00//CDC_STICTL[1:0],CDC_SOFTSTART[1:0]
    //#define CDC_9611SC_PD_OUT                 0x70//CDC_9611SC_PD_OUT1I|CDC_9611SC_PD_OUT2I|CDC_9611SC_PD_OUT4I,CDC_SOFTSTART[6:4]

    //9611S_D
    #define CDC_9611SD_HPPOP            0x00
    #define CDC_9611SD_MBSEL            0x02
    #define CDC_9611SD_POP_ICTL1        0x00
    #define CDC_9611SD_POP_ICTL2        0x80
    #define CDC_9611SD_PD_OUTXI         0x70
    #define CDC_9611SD_POP_SW_EN        0x00
    #define CDC_9611SD_PD_COMP_L        0x01
    #define CDC_9611SD_PD_COMP_R        0x02
    #define CDC_9611SD_DFF_SET_L        0x01
    #define CDC_9611SD_DFF_SET_R        0x02
    #define CDC_9611SD_BYPASS_L         0x00
    #define CDC_9611SD_BYPASS_R         0x00
    #define CDC_9611SD_INIT_EN_L        0x00
    #define CDC_9611SD_INIT_EN_R        0x00
    #define CDC_9611SD_PD_CHG_L         0x01
    #define CDC_9611SD_PD_CHG_R         0x02
    #define CDC_9611SD_PD_STBF          0x03
    #define CDC_9611SD_PREC_EN          0x00
    #define CDC_9611SD_PD_LEVEL         0x03
    #define CDC_9611SD_PD_BF_HPOUT      0x03

    //9611S_E
    #define CDC_9611SE_D2S              0x03

    //7621A
    #define CDC_7621A_HPPOP             0x00   //CDC_9611SD_HPPOP
    #define CDC_7621A_HP_PULLDOWN_EN    0x00
    #define CDC_7621A_PWD_HFOUT         0x77    //0x66--sing,  0x77--diff
    #define CDC_7621A_PD_CHARGE         0x0E
    #define CDC_7621A_PD_OUT            0x00    //0x70--sing,  0x00--diff, only for 7621BA
    #define CDC_7621A_PD_FD             0x00    // 0x00  0x07>0  
    #define CDC_7621A_REFPD_HIGH        0x01    //CDC_REFPD[5](vcom=4/9vdd)=1 to close
    #define CDC_7621A_REFPD_LOW         0x10    //CDC_REFPD[3:0](vcom=1/2vdd)=0 to open; CDC_REFPD[4](vcom=5/9vdd)=1 to close
    //VA761S, VA7621G
    #define CDC_7621A_PD_OUT_R          0x00    //0x70--sing,  0x00--diff, for 7621SA&7621GA
    #define CDC_7621A_PD_OUT_L          0x00    //0x70--sing,  0x00--diff, for 7621SA&7621GA
    #define CDC_7621A_CDC_NMILLER       0x00
    #define CDC_7621A_RESERVE           0x00
    #define CDC_7621A_PGA_CUR           0x33
    #define CDC_7621A_HP_ICTL           0x00
    #define CDC_7621A_ICTL_STBF         0x00
    #define CDC_7621A_HPPD_OUT          0x33
    //VA7621SD
    #define CDC_7621SD_HPAGAIN_MODE     0x00

    //VA7632BA0
    #define CDC_7632BA_PD_OUT_L_DG      0x77
    #define CDC_7632BA_PD_OUT_R_DG      0x77
    #define CDC_7632BA_PD_CHARGE_DG     0xEE

    #define CDC_7632BA_PGA_CUR_DG           0x33
    #define CDC_7632BA_ADCUR_DG             0x33
    #define CDC_7632BA_REF_DG               0x12
    #define CDC_7632BA_VREF_SEL_DG          0x00
    #define CDC_7632BA_VCOM_SEL_DG          0x06
    #define CDC_7632BA_CTSC_DG              0x00
    #define CDC_7632BA_MBSEL_DG             0x02
    #define CDC_7632BA_REFPD_HIGH_DG        0x0F
    #define CDC_7632BA_DACCTL1_DG           0x44
    #define CDC_7632BA_DACCTL2_DG           0x00
    #define CDC_7632BA_MIX_DG               0x00
    #define CDC_7632BA_ICTL_STBF_DG         0x00
    #define CDC_7632BA_PWD_HFOUT_DG         0x00
    #define CDC_7632BA_HP_ICTL_DG           0x00
    #define CDC_7632BA_HPPD_OUT_DG          0x33
    #define CDC_7632BA_NMILLER_DG           0x00
    #define CDC_7632BA_CBOOST_DG            0x00
    #define CDC_7632BA_VGATE_SEL_DG         0x00
    #define CDC_7632BA_VSUB_HIGH_DG         0x00
    #define CDC_7632BA_HPA_NATIVE_DG        0x00
    #define CDC_7632BA_LINEIN_AMP_VREF_DG   0x00
    #define CDC_7632BA_HPAGAIN_MODE_DG      0x00

    #define CDC_7632BA_PD_OUT_L_0           0x11//diff
    #define CDC_7632BA_PD_OUT_R_0           0x11

#endif

UINT8 VCDC_ModeCheck_Output ( UINT8 DacChn );

void VCDC_Init ( void )
{
   if(g_VCDC_Input !=CDC_INPUT_NONE || g_VCDC_Output != CDC_OUTPUT_NONE){
       return;
   }
    g_VCDC_Input = CDC_INPUT_NONE;
    g_VCDC_Output = CDC_OUTPUT_NONE;
	VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
	VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);

	if(VCDC_GetVerNo() == CDC_VERSION_PLUS){
	    UINT8 cnt = 0;
	    //for future 7621 ver.
	    for ( cnt = 0; cnt < CDC_PLUS_INIT_REGNUM; cnt++ ) {
	        if ( g_VCDC_Plus_Config.CDC_Init[cnt].cdc_reg_addr ) {
	            VCDC_WriteReg ( g_VCDC_Plus_Config.CDC_Init[cnt].cdc_reg_addr, g_VCDC_Plus_Config.CDC_Init[cnt].cdc_on_value );
	        }
	    }
	}else if(VCDC_GetVerNo() == CDC_VERSION_7621SD_C0){	
	
		VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x00 );
		VCDC_WriteReg ( CDC_REGS_ADCUR, ( 0x00 | CDC_9611S_ADCURL | CDC_9611S_ADCURR ) );
		VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x03 );
		VCDC_WriteReg ( CDC_REGS_ADPD, 0x03 );
		VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x00 );
		VCDC_WriteReg ( CDC_REGS_ADVOL, 0x11 );
		VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL0, ( CDC_9611S_DACDEBUG_L | CDC_9611S_DACDEBUG_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x44 | CDC_9611S_DACMODE_L | CDC_9611S_DACMODE_R | \
		                                    CDC_9611S_DACCTL_L | CDC_9611S_DACCTL_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x00 | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x11 );
		VCDC_WriteReg ( CDC_REGS_HPDOCN, ( 0x00 | CDC_9611S_HPDOCN_L ) );
		VCDC_WriteReg ( CDC_REGS_HPDOCP, ( 0x00 | CDC_9611S_HPDOCP_L ) );
		VCDC_WriteReg ( CDC_REGS_HP0, ( 0x44 | CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );
		VCDC_WriteReg ( CDC_REGS_HP1, ( 0x22 | CDC_7621A_HPPOP ) );
		VCDC_WriteReg ( CDC_REGS_HPVOL, 0x00 );
		VCDC_WriteReg ( CDC_REGS_MBSEL, CDC_9611SD_MBSEL );
		VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
		VCDC_WriteReg ( CDC_REGS_PGA, 0x33 ); //
		VCDC_WriteReg ( CDC_REGS_REF, ( 0x00 | CDC_9611S_REFVRFSEL | CDC_9611S_REFIOPT | CDC_9611S_VCMSEL ) );
		VCDC_WriteReg ( CDC_REGS_REFPD_LOW, 0x1f ); //?? //VCDC_WriteReg(CDC_REGS_REFPD, 0x1f);
		VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
		VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x00 );
		VCDC_WriteReg ( CDC_REGS_HPLP, 0x00 );
		VCDC_WriteReg ( CDC_REGS_DF_RESERVE, 0x00 );
		VCDC_WriteReg ( CDC_REGS_POP_DEBUG, 0x00 );
		VCDC_WriteReg ( CDC_REGS_PD_FD, CDC_7621A_PD_FD );
		VCDC_WriteReg ( CDC_REGS_ICTL_STBF, CDC_7621A_ICTL_STBF );
		//20190122 add for diff to keep CDC_REGS_PD_CHARGE close
		if ( VCDC_ModeCheck_Output ( g_VCDC_Config.dac_chn ) == CDC_OUTPUTMODE_DIFF ) {
		    VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0x00 );
		} else {
		    VCDC_WriteReg ( CDC_REGS_PD_CHARGE, CDC_7621A_PD_CHARGE );
		}
		VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x00 );
		VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x00 ); 
		VCDC_WriteReg ( CDC_REGS_PD_OUT, CDC_7621A_RESERVE );    //(7621S, CDC_RESERVE)
		VCDC_WriteReg ( CDC_REGS_PGA_CUR, CDC_7621A_PGA_CUR );
		VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x11 );
		VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT );
		VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
		VCDC_WriteReg ( CDC_REGS_REFPD_HIGH, 0x0F );
		VCDC_WriteReg ( CDC_REGS_HP_PULLDOWN_EN, CDC_7621A_HP_PULLDOWN_EN );
		VCDC_WriteReg ( CDC_REGS_HP_ICTL, CDC_7621A_HP_ICTL );
		VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x00 ); 
		VCDC_WriteReg ( CDC_REGS_HPPD_OUT, CDC_7621A_HPPD_OUT );
		VCDC_WriteReg ( CDC_REGS_NMILLER, CDC_7621A_CDC_NMILLER );
		VCDC_WriteReg ( CDC_REGS_CDC_HPAGAIN_MODE, CDC_7621SD_HPAGAIN_MODE );

    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621S_A ) {
	
		VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x00 );
		VCDC_WriteReg ( CDC_REGS_ADCUR, ( 0x00 | CDC_9611S_ADCURL | CDC_9611S_ADCURR ) );
		VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x03 );
		VCDC_WriteReg ( CDC_REGS_ADPD, 0x03 );
		VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x00 );
		VCDC_WriteReg ( CDC_REGS_ADVOL, 0x11 );
		VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL0, ( CDC_9611S_DACDEBUG_L | CDC_9611S_DACDEBUG_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x44 | CDC_9611S_DACMODE_L | CDC_9611S_DACMODE_R | \
		                                    CDC_9611S_DACCTL_L | CDC_9611S_DACCTL_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0xcc | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
		VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x11 );
		VCDC_WriteReg ( CDC_REGS_HPDOCN, ( 0x00 | CDC_9611S_HPDOCN_L ) );
		VCDC_WriteReg ( CDC_REGS_HPDOCP, ( 0x00 | CDC_9611S_HPDOCP_L ) );
		VCDC_WriteReg ( CDC_REGS_HP0, ( 0x44 | CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );
		VCDC_WriteReg ( CDC_REGS_HP1, ( 0x33 | CDC_7621A_HPPOP ) ); //
		VCDC_WriteReg ( CDC_REGS_HPVOL, 0x00 );
		VCDC_WriteReg ( CDC_REGS_MBSEL, CDC_9611SD_MBSEL );
		VCDC_WriteReg ( CDC_REGS_MIX, ( 0xcc | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
		VCDC_WriteReg ( CDC_REGS_PGA, 0x33 ); //
		VCDC_WriteReg ( CDC_REGS_REF, ( 0x00 | CDC_9611S_REFVRFSEL | CDC_9611S_REFIOPT | CDC_9611S_VCMSEL ) );
		VCDC_WriteReg ( CDC_REGS_REFPD_LOW, 0x1f ); //?? //VCDC_WriteReg(CDC_REGS_REFPD, 0x1f);
		VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
		VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x00 );
		VCDC_WriteReg ( CDC_REGS_HPLP, 0x00 );
		VCDC_WriteReg ( CDC_REGS_DF_RESERVE, 0x00 );
		VCDC_WriteReg ( CDC_REGS_POP_DEBUG, 0x00 );
		VCDC_WriteReg ( CDC_REGS_ICTL_STBF, CDC_7621A_ICTL_STBF );
		VCDC_WriteReg ( CDC_REGS_PD_CHARGE, CDC_7621A_PD_CHARGE );
		VCDC_WriteReg ( CDC_REGS_PD_FD, CDC_7621A_PD_FD );
		VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x77 );
		VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x77 );		
		VCDC_WriteReg ( CDC_REGS_PD_OUT, CDC_7621A_RESERVE ); //(7621S, CDC_RESERVE)
		VCDC_WriteReg ( CDC_REGS_PGA_CUR, CDC_7621A_PGA_CUR );
		VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x11 );
		VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT );
		VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x11 ); 
		VCDC_WriteReg ( CDC_REGS_REFPD_HIGH, 0x0F );
		VCDC_WriteReg ( CDC_REGS_HP_PULLDOWN_EN, CDC_7621A_HP_PULLDOWN_EN );
		VCDC_WriteReg ( CDC_REGS_HP_ICTL, CDC_7621A_HP_ICTL );
		VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x11 );
		VCDC_WriteReg ( CDC_REGS_HPPD_OUT, CDC_7621A_HPPD_OUT );
		VCDC_WriteReg ( CDC_REGS_NMILLER, CDC_7621A_CDC_NMILLER );
    } else if(VCDC_GetVerNo() == CDC_VERSION_7621_I2S){	
    	;
 	} else if(VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC || VCDC_GetVerNo() == CDC_VERSION_7638_A0){
    	;
	} else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

void VCDC_I2S_Clock_En(BOOL CLK_Open)
{
	UINT8 val;
	VCDC_ReadReg(CDC_REGS_SPORT_MODE, &val);

	if (TRUE == CLK_Open)
	{
		VCDC_WriteReg ( CDC_REGS_I2S_CTRL, g_VCDC_I2S_Config.i2s_ctrl );
		// change pll frq up, to reduce jitter
		if(g_VCDC_I2S_Config.i2s_clk_sel!=2) {
			//for 6M clock
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_N_LOW, 0x00 );
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_N_HIGH, 0x02 );
		} else {
			//for hd 16M clock; // change pll frq ref hds clock 16M
			VCDC_WriteReg(CDC_REGS_PLL_CTRL_N_LOW, 0xC0);	// N low
			VCDC_WriteReg(CDC_REGS_PLL_CTRL_N_HIGH, 0x00);	// N high
		}
		VCDC_WriteReg ( CDC_REGS_PLL_CTRL_R, 0x05 );//use default value is ok;
		
		if ( 0/*32000 == VCODEC_HW_GetDACSampleRate()*/) {
			//if choose dac as IIS output clock and DACSampleRate =32K then the dac clock is 4M(not 6M) so will config it; 
			VCDC_WriteReg ( CDC_REGS_PLLOUT_DIV, 150 ); // for 32k test
		} else {
			VCDC_WriteReg ( CDC_REGS_PLLOUT_DIV, 100 );
		}
		// end change pll frq up
		
		if ( g_VCDC_I2S_Config.i2s_clk_sel==1 ) {//dac out IIS clock 
			VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x04 );
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x10 );
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
			VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
			VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x07 );// ref dac clock to PLL
			//	VCDC_WriteReg(0x41, 0xb8);
		} else if(g_VCDC_I2S_Config.i2s_clk_sel==0){//adc out IIS clock 
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
			VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
			VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x0f ); // ref Adc clock to PLL
		} else if(g_VCDC_I2S_Config.i2s_clk_sel==2){//hd out IIS clock 
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
			VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
			VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x17 ); // ref hd clock to PLL
		}
		
		VCOMMON_Delay(10000);
		// 2019-11-16 by chen jh, remove becuse Ex Codec 7755 & Ex adc dac Not powor on when INIT soc.
		//VCDC_ExCodecConfig(1);
		//VCOMMON_NPRINTF("VCDC_configHW.\r\n");
	}
	else
	{
		UINT8 I2sCtrlVal = 0; //, i = 0
		if ( !g_VCDC_I2S_Config.i2s_ctrl ) {
			return;
		}

        if ( g_VCDC_I2S_Config.i2s_clk_sel == 2) {
            I2sCtrlVal = g_VCDC_I2S_Config.i2s_ctrl;
            I2sCtrlVal &= ~ ( BIT5 | BIT6 ); // close i2s RX and TX enable
            VCDC_WriteReg ( CDC_REGS_I2S_CTRL, I2sCtrlVal );
			
            if(g_VCDC_I2S_Config.i2s_clk_sel == 2){
                // disable FOUT_EN: pll and decim off
                VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x04 );
                VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x00 );
                VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x10 );
            }
        }
	}
}

//20191114xy, for extern codec do init;  the mode==0, do close extern codec, ==1 do config extern codec, ==2 do re-config extern codec;
//20191121xy, for extern codec control; the mode ==0 for ex-codec power on/off, ==1 for ex-codec init/uninit(had power on); the ctrol ==0 for ex-codec close, ==1 for ex-codec open; 
/******************************************************************************
Description:
	cdc driver for extern codec(dsp&dac&adc)  control.

Parameters:
	UINT8 Mode: ==0 for ex-codec power on/off, ==1 for ex-codec init/uninit(had power on); 
	UINT8 Ctrl:==0 for ex-codec close, ==1 for ex-codec open;

Return Value:
	.
*****************************************************************************/
#if defined (HW_CDC_DEBUG)
#include "vcodec.h"
#include "vmcc_external_cdc.h"
UINT8 VCDC_ExCodecConfig ( UINT8 Mode, UINT8 Ctrl )
{ 
	VCOMMON_NPRINTF("VCDC_ExCodecConfig=%d-%d[stat=%x][Ret=0x%08X].\r\n", Ctrl, Mode, VCODEC_GetState(), __GET_RETURN_ADDR());

	#if (VDEV_CHIP_VERSION < CHIP_9638P)
	if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S)
	#else //9648, had IIS, all will use ex-IIS not by 7621IIS;
	if(1)
	#endif
	{
		if(Mode == 0){//for ex-codec power on/off
			if(Ctrl == 0){
				//for power off, to SYS_PMGR_STAT_PWRSAVE
				VCDC_I2S_Clock_En(FALSE);//close IIS for low power; 
                void VCODEC_PLUG_TurnOff_DLY(BOOL needDly);
				VCODEC_PLUG_TurnOff_DLY(FALSE);//sometime,the PA not closed,so close it here.
			} else {
				//for power on, to SYS_PMGR_STAT_ACTIVE
				VCDC_I2S_Clock_En(TRUE);
#if 1 
					/*// move these operations into codec adapter init to fix the BUG of noise aftre boot's tone.
					VCOMMON_NPRINTF("bb2dminit\r\n");
					Init_DAC_ADC();
					Init_DSP();
					AKMSwitchInputSrc(FALSE);
					AKMAuxBTBalance(TRUE);*/
					
					if (extcdc.init != NULL) extcdc.init();
					//if (extcdc.volume_set != NULL) extcdc.volume_set(1);
					//NOTICE_PRINTF("[%s],volset2 begin:%u\n\r",__func__,1);  
					//vol = 1;
					//HLP_MSG_Send2(EVENT_USER_MCC_VOL_SET,&vol,sizeof(UINT8 ));
					//NOTICE_PRINTF("[%s],volset2 end:%u\n\r",__func__,1);	
	
#endif
				}
			}
#if 1
			else if(Mode == 1) {//for ex-codec init/uninit(had power on)
				if(Ctrl == 0){//for uninit, not work
					if (extcdc.volume_set != NULL) extcdc.volume_set(1);
	
					//AKMAuxBTBalance(TRUE);
				} else {
					//for init, will work;
	#if 1//20191117xy, optimization and discuss with zhouhuijun;
					if(VCODEC_GetState() == VCODEC_STATE_MUSIC_PLAYING){//the state: linein csb:music playing;linein : idle(change to linein playing after init).
		#if  VDEV_AUDIOBROADCAST_SUPPORTED
						if(VCODEC_MUSIC_CheckSelAuxGain())
						{
							//AKMAuxBTBalance(FALSE);
							if (extcdc.balance_set != NULL) extcdc.balance_set(FALSE);
						}else
		#endif
						{
							//AKMAuxBTBalance(TRUE);
							if (extcdc.balance_set != NULL) extcdc.balance_set(TRUE);
						}
					} else {//because linein state update after adapter_init.so move to else branch.
						//AKMAuxBTBalance(FALSE);
						if (extcdc.balance_set != NULL) extcdc.balance_set(FALSE);
					} 
	 
	#else
					AKMdsp_ReConfig7755(0);//VCODEC_Adatper_Config_ExtDSP(p_HwInitParam->dac_gain);  //??
					Config_5121DAC_5140ADC();	//VCODEC_Adatper_Config_ExtAdcDac();
					
					//here will be config by music or Aux state diff;
					AKMAuxBTBalance(TRUE);
	#endif
				}
			} 
#endif
			else {	

			VCOMMON_EPRINTF("VCDC_ExCodecConfig param[%d] err!\r\n", Mode);
		}
	}
	return 0;
}

#endif
//do check ex-codec(dsp&dac&adc) ; the return 0==all right, others show which ex-codec had stat err and err state;
UINT8 VCDC_ExCodecCheckStat ( UINT8 Mode )
{
	UINT8 ret = 0;
	return ret;
}

//do re-config ex-codec(dsp&dac&adc) by the return err of VCDC_ExCodecCheckStat ; the return 0==all right, others show which ex-codec re-config err;
UINT8 VCDC_ExCodecCheckReconfig ( UINT8 err )
{
	UINT8 ret = 0;
	return ret;
}

/******************************************************************************
Description:
	cdc driver for extern codec(dsp&dac&adc)  state check ; if had err maybe do reconfig; 

Parameters:
	UINT8 Mode: for ex-codec check choose or some special need; 0==not do check; 1==dac open, 3==dac&adc both open;

Return Value:
	return the check result; 0==stat ok, others had some err;
	.
*****************************************************************************/
#define CDC_EXCODEC_CHECKCYCLE 		1000//do once ec-codec check cycle ;
UINT16 g_VCDC_ExCodec_CheckCnt = 0;
UINT8 VCDC_ExCodecCheck ( UINT8 Mode )
{
	UINT8 ret = 0;
	#if (VDEV_CHIP_VERSION < CHIP_9638P)
	if((g_VCDC_ExCodec_CheckCnt++ > CDC_EXCODEC_CHECKCYCLE)&&(VCDC_GetVerNo() == CDC_VERSION_7621_I2S))
	#else //9648, had IIS, all will use ex-IIS not by 7621IIS;
	if(g_VCDC_ExCodec_CheckCnt++ > CDC_EXCODEC_CHECKCYCLE)
	#endif
	{
		if (Mode >0) {
			ret = VCDC_ExCodecCheckStat(Mode);//do ex-codec state check;
			if(ret != 0){//ex-codec state check err;
				VCDC_WPRINTF("VCDC_ExCodecCheck err=%d.\r\n", ret);
				ret = VCDC_ExCodecCheckReconfig(ret);//do ex-codec reconfig by the err return;
				if(ret != 0){//do ex-codec reconfig also err;
					VCDC_EPRINTF("VCDC_ExCodecCheck reconfig err=%d!\r\n", ret);
				} else {//do ex-codec reconfig ok, will re-check;
					VCDC_NPRINTF("VCDC_ExCodecCheck reconfig ok.\r\n");
					ret = VCDC_ExCodecCheckStat(Mode);//do ex-codec state re-check;
					if(ret != 0){
						VCDC_FPRINTF("VCDC_ExCodecCheck reconfig ok but re-check err=%d!\r\n", ret);
					} else {
						VCDC_NPRINTF("VCDC_ExCodecCheck reconfig and re-check ok.\r\n");
					}
				}
			} else {//ex-codec state check ok;
				VCDC_NPRINTF("VCDC_ExCodecCheck ok.\r\n");
			}
		}
		g_VCDC_ExCodec_CheckCnt = 0;//reset the count to 0;
	} 
	return ret;
}


//20190701xy, to config debug pin by eck, these only do config once at VSYS_SOC_InitCODEC and not change; at  adc&dac power up/down not change these registers.
//20191112xy, change the name from VCDC_ConfigDebug to VCDC_ConfigHW, do config codec(internal&external) after power on do config once, the first time;
void VCDC_ConfigHW ( void )
{
    if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
        if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
            //set debug eck.
            //wujie:    a)  0x80---0x8A 0x8D 0x90---0x97,0x99,0x9B debug  ECK 
            VCDC_WriteReg ( CDC_REG763X_PGA_CUR_DG, CDC_7632BA_PGA_CUR_DG ); //0x80
            VCDC_WriteReg ( CDC_REG763X_ADCUR_DG, CDC_7632BA_ADCUR_DG );
            VCDC_WriteReg ( CDC_REG763X_REF_DG, CDC_7632BA_REF_DG );
            VCDC_WriteReg ( CDC_REG763X_VREF_SEL_DG, CDC_7632BA_VREF_SEL_DG );
            VCDC_WriteReg ( CDC_REG763X_VCOM_SEL_DG, CDC_7632BA_VCOM_SEL_DG );
            VCDC_WriteReg ( CDC_REG763X_CTSC_DG, CDC_7632BA_CTSC_DG );
            VCDC_WriteReg ( CDC_REG763X_MBSEL_DG, CDC_7632BA_MBSEL_DG );
            VCDC_WriteReg ( CDC_REG763X_REFPD_HIGH_DG, CDC_7632BA_REFPD_HIGH_DG );
            VCDC_WriteReg ( CDC_REG763X_DACCTL1_DG, CDC_7632BA_DACCTL1_DG );
            VCDC_WriteReg ( CDC_REG763X_DACCTL2_DG, CDC_7632BA_DACCTL2_DG );
            VCDC_WriteReg ( CDC_REG763X_MIX_DG, CDC_7632BA_MIX_DG ); //0x8A
            VCDC_WriteReg ( CDC_REG763X_ICTL_STBF_DG, CDC_7632BA_ICTL_STBF_DG ); //0x8D
            VCDC_WriteReg ( CDC_REG763X_PWD_HFOUT_DG, CDC_7632BA_PWD_HFOUT_DG ); //0x90
            VCDC_WriteReg ( CDC_REG763X_HP_ICTL_DG, CDC_7632BA_HP_ICTL_DG );
            VCDC_WriteReg ( CDC_REG763X_HPPD_OUT_DG, CDC_7632BA_HPPD_OUT_DG );
            VCDC_WriteReg ( CDC_REG763X_NMILLER_DG, CDC_7632BA_NMILLER_DG );
            VCDC_WriteReg ( CDC_REG763X_CBOOST_DG, CDC_7632BA_CBOOST_DG );
            VCDC_WriteReg ( CDC_REG763X_VGATE_SEL_DG, CDC_7632BA_VGATE_SEL_DG );
            VCDC_WriteReg ( CDC_REG763X_VSUB_HIGH_DG, CDC_7632BA_VSUB_HIGH_DG );
            VCDC_WriteReg ( CDC_REG763X_HPA_NATIVE_DG, CDC_7632BA_HPA_NATIVE_DG ); //0x97
            VCDC_WriteReg ( CDC_REG763X_LINEIN_AMP_VREF_DG, CDC_7632BA_LINEIN_AMP_VREF_DG ); //0x99
            VCDC_WriteReg ( CDC_REG763X_HPAGAIN_MODE_DG, CDC_7632BA_HPAGAIN_MODE_DG ); //0x9B

            //at system power on need do audio power down;
            {
                UINT8 mode =0;
			#if defined (HW_CDC_DEBUG)
                if(VCODEC_Check_PopBetter_Enable()){
                    mode|=0x01;
                }
			#endif
                VCDC_ConfigOutput(CDC_OUTPUT_NONE,mode);
            }
            VCDC_ConfigInput ( CDC_INPUT_NONE );
		} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
			;//only PMU not audio;
        } else {

            VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
        }
		#if VDEV_CODEC_IO_INITIALIZE_ONCE 
			VI2S_InitClk_Once();
		#endif
    } else if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S) {
		VCDC_I2S_Clock_En(TRUE);

    } else {
        //7621 and others not need do config.
        ;
    }
}

//20220410xy, close IIS for RF test at ATE/ATS mode beeter;
void VCDC_ConfigHW_CloseIIS(void)
{
	//the IIS had open at VCDC_ConfigHW when power on for output the IIS bclk for ex-IC;
	if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
		//VCODEC_MultiChannel_Delay_Sync();
		#if VDEV_CODEC_IO_INITIALIZE_ONCE 
			VI2S_UninitClk_Once();
		#endif	
    } else if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S) {
		VCDC_I2S_Clock_En(FALSE);
    } else {
        //7621 and others not need do config.
        ;
    }
}

#if 1
// 2022-11-9 cjh, close IIS clock for deep sleep mode
void VCDC_HW_CloseIIS(void)
{
	VI2S_SetTx(I2S_CH0, FALSE);
	VI2S_finit(I2S_CH0);
}
void VCDC_HW_ReinitIIS(void)
{
	if(VI2S_GetTx(I2S_CH0)){
		return;
	}
	VI2S_Init(0, 1, 0, 48000, FALSE, 0);
	VI2S_SetTx(I2S_CH0, TRUE);
}
// end 2022-11-9 cjh
#endif

UINT8 VCDC_GetDACChan ( void )
{
    return g_VCDC_Config.dac_chn;
}

void VCDC_ConfigInput ( UINT8 Input )
{
	if(VCDC_GetVerNo() == CDC_VERSION_PLUS ){
		
	    UINT8 cnt = 0;
	    //for future 7621 ver.
		for ( cnt = 0; cnt < CDC_PLUS_INPUT_REGNUM; cnt++ ) {
		    if ( Input != CDC_INPUT_NONE ) { //open
		        if ( g_VCDC_Plus_Config.CDC_InPut[cnt].cdc_reg_addr ) {
		            VCDC_WriteReg ( g_VCDC_Plus_Config.CDC_InPut[cnt].cdc_reg_addr, g_VCDC_Plus_Config.CDC_InPut[cnt].cdc_on_value );
		        }
		    } else {//close
		        if ( g_VCDC_Plus_Config.CDC_InPut[CDC_PLUS_INPUT_REGNUM - 1 - cnt].cdc_reg_addr ) {
		            VCDC_WriteReg ( g_VCDC_Plus_Config.CDC_InPut[CDC_PLUS_INPUT_REGNUM - 1 - cnt].cdc_reg_addr, g_VCDC_Plus_Config.CDC_InPut[CDC_PLUS_INPUT_REGNUM - 1 - cnt].cdc_off_value );
		        }
		    }
		}
		
	}else if(VCDC_GetVerNo() == CDC_VERSION_7621S_A ){
	
	    if ( Input != CDC_INPUT_NONE ) {
	        if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	            /* power on REF */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 ); //if open dac&adc, open Mbias for adc
	           /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x00 | CDC_9611S_SCBUFFICTL ) );
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) );
	        }
	    }

	    switch ( Input ) {
	        case CDC_INPUT_MICL:
	            /* power on and unmute PGAL */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x10 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x31 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x30 );
	            /* enable ADCL clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x01 );
	            /* power on ADCL current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x02 );
	            /* power on ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x02 );
	            /* reset ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x01 );
	            break;
	        case CDC_INPUT_MICR:
	            /* power on and unmute PGAR */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x13 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );
	            /* enable ADCR clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x02 );
	            /* power on ADCR current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x01 );
	            /* power on ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x01 );
	            /* reset ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x02 );
	            break;
	        case CDC_INPUT_DUAL_MIC:
	            /* power on and unmute PGA L/R */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x11 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x00 );
	            /* enable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x03 );
	            /* power on ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x00 );
	            /* power on ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x00 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x03 );
	            break;
	        case CDC_INPUT_NONE:
	            //make the volume descending to 0
	            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
	                VCDC_VolShadow ( CDC_REGS_ADVOL, 0, g_VCDC_Config.adc_volset_delayus );
	            }
	            /* disable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x00 );
	            /* power down ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x03 );
	            /* power down ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x03 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x00 );
	            /* power down and mute PGAL/R */
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x33 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x11 );
	            if ( g_VCDC_Output == CDC_OUTPUT_NONE ) { // &&(g_VCDC_Input != CDC_INPUT_NONE)
	                /* power down CTBUF */
	                VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) );
	                /* power down SCBUF */
	                VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
	                /* power down REF */
		            VCDC_WriteReg ( CDC_REGS_REFPD, 0x1F );
	            } else {//for dac delay close
	                VCDC_WriteReg ( CDC_REGS_REFPD, 0x08 ); //the output is power on only for dac, close Mbias
	            }
	            break;
	    }
	}else if (VCDC_GetVerNo() == CDC_VERSION_7621SD_C0){
		
	    if ( Input != CDC_INPUT_NONE ) {
	        if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	            /* power on REF */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 ); //if open dac&adc, open Mbias for adc
	            /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x00 | CDC_9611S_SCBUFFICTL ) );
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) );
	        }
	    }

	    switch ( Input ) {
	        case CDC_INPUT_MICL:
	            /* power on and unmute PGAL */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x10 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x31 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x30 );

	            /* enable ADCL clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x01 );
	            /* power on ADCL current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x02 );
	            /* power on ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x02 );
	            /* reset ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x01 );
	            break;
	        case CDC_INPUT_MICR:
	            /* power on and unmute PGAR */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x13 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );

	            /* enable ADCR clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x02 );
	            /* power on ADCR current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x01 );
	            /* power on ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x01 );
	            /* reset ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x02 );
	            break;
	        case CDC_INPUT_DUAL_MIC:

	            /* power on and unmute PGA L/R */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x11 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x00 );
	            /* enable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x03 );
	            /* power on ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x00 );
	            /* power on ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x00 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x03 );
	            break;
	        case CDC_INPUT_NONE:
	            //make the volume descending to 0
	            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
	                VCDC_VolShadow ( CDC_REGS_ADVOL, 0, g_VCDC_Config.adc_volset_delayus );
	            }
	            /* disable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x00 );
	            /* power down ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x03 );
	            /* power down ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x03 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x00 );
	            /* power down and mute PGAL/R */
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x33 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x11 );

	            if ( g_VCDC_Output == CDC_OUTPUT_NONE ) { // &&(g_VCDC_Input != CDC_INPUT_NONE)
	                /* power down CTBUF */
	                VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) ); //VCDC_WriteReg(CDC_REGS_CTBUF, (0x04 | CDC_9611S_CTBUFFICTL));
	                /* power down SCBUF */
	                VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
	                /* power down REF */
	                VCDC_WriteReg ( CDC_REGS_REFPD, 0x1F );
	            } else {//for dac delay close
	                VCDC_WriteReg ( CDC_REGS_REFPD, 0x08 ); //the output is power on only for dac, close Mbias
	            }

	            break;
	    }
    }else if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S){
    
	    if ( Input != CDC_INPUT_NONE ) {
	        if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	            /* power on REF */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 ); //if open dac&adc, open Mbias for adc
	            /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x00 | CDC_9611S_SCBUFFICTL ) );
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) );
	        }
	    }
		
	    switch ( Input ) {
	        case CDC_INPUT_MICL:
	            /* power on and unmute PGAL */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x10 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x31 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x30 );
	            /* enable ADCL clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x01 );
	            /* power on ADCL current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x02 );
	            /* power on ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x02 );
	            /* reset ADCL channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x01 );
	            break;
	        case CDC_INPUT_MICR:
	            /* power on and unmute PGAR */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x13 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );

	            /* enable ADCR clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x02 );
	            /* power on ADCR current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x01 );
	            /* power on ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x01 );
	            /* reset ADCR channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x02 );
	            break;
	        case CDC_INPUT_DUAL_MIC:

	            /* power on and unmute PGA L/R */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x11 );
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x00 );

	            /* enable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x03 );
	            /* power on ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x00 );
	            /* power on ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x00 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x03 );
	            break;
	        case CDC_INPUT_NONE:
	            //make the volume descending to 0
	            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
	                VCDC_VolShadow ( CDC_REGS_ADVOL, 0, g_VCDC_Config.adc_volset_delayus );
	            }
	            /* disable ADCL/R clock */
	            VCDC_WriteReg ( CDC_REGS_ADCLKEN, 0x00 );
	            /* power down ADCL/R current reference */
	            VCDC_WriteReg ( CDC_REGS_ADPDIREF, 0x03 );
	            /* power down ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADPD, 0x03 );
	            /* reset ADCL/R channel */
	            VCDC_WriteReg ( CDC_REGS_ADRSTB, 0x00 );
	            /* power down and mute PGAL/R */
	            VCDC_WriteReg ( CDC_REGS_PGA, 0x33 );
	            VCDC_WriteReg ( CDC_REGS_PGA_PD_IREF, 0x11 );

	            if ( g_VCDC_Output == CDC_OUTPUT_NONE ) { // &&(g_VCDC_Input != CDC_INPUT_NONE)
	                /* power down CTBUF */
	                VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) ); //VCDC_WriteReg(CDC_REGS_CTBUF, (0x04 | CDC_9611S_CTBUFFICTL));
	                /* power down SCBUF */
	                VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
	                /* power down REF */
	                VCDC_WriteReg ( CDC_REGS_REFPD, 0x1F );
	            } else {//for dac delay close
	                VCDC_WriteReg ( CDC_REGS_REFPD, 0x08 ); //the output is power on only for dac, close Mbias
	            }

	            break;
	    }
	} else if (VCDC_GetVerNo() == CDC_VERSION_7632B_A0){
	
	    if ( Input != CDC_INPUT_NONE ) {
	        if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	            /* power on REF */
	            //a)    0x40=0x00 MIC /   0x08 LINEIN Bandgap Mic bias 
	            VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x00 ); //CDC_REFPD[4:0] =5'b00000  //if open dac&adc, open Mbias for adc

				//for 7632BA0, B0, C0, D0, E0, F0, C1, D1, E1, F1, E2, F2, E5, F5;
	            /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x00 ); //CDC_SCBUFPD = 0
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x04 ); //CDC_CTBUFPD = 1
	        }
	    }

	    switch ( Input ) {
	        case CDC_INPUT_MICL:
	            /* Power on ADC */
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x01 ); //CDC_ADCLKENL=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x02 ); //CDC_ADPDIREFL=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x02 ); //CDC_ADPDL=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x01 ); //CDC_ADRSTBL=1

	            /* Power on and unMute PGA */
	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x31); //CDC_PGAPDL=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x30 ); //CDC_PGAMUTEL=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x10 ); //CDC_PGA_PD_IREF_L=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELL=0
	            break;
	        case CDC_INPUT_MICR:
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x02 ); //CDC_ADCLKENR=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x01 ); //CDC_ADPDIREFR=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x01 ); //CDC_ADPDR=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x02 ); //CDC_ADRSTBR=1

	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x13); //CDC_PGAPDR=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x03 ); //CDC_PGAMUTER=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x01 ); //CDC_PGA_PD_IREF_R=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELR=0
	            break;
	        case CDC_INPUT_DUAL_MIC:
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x03 ); //CDC_ADCLKENL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x00 ); //CDC_ADPDIREFL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x00 ); //CDC_ADPDL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x03 ); //CDC_ADRSTBL&R=1

	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x11); //CDC_PGAPDL&R=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x00 ); //CDC_PGAMUTEL&R=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x00 ); //CDC_PGA_PD_IREF_L&R=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELL&R=0
	            break;
	        case CDC_INPUT_NONE:
	            if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	                /* Power down REF SCBUF and CTBUF */
	                // Bandgap Mic bias 
	                VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x1F ); //CDC_REFPD[4:0] =5'b11111
	                
	                //for 7632BA0, B0, C0, D0, E0, F0, C1, D1, E1, F1, E2, F2, E5, F5;
	                VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
	                VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
	            }

	            /* Power down ADC */
	            // ADC 
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x00 ); //CDC_ADCLKENL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x03 ); //CDC_ADPDIREFL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x03 ); //CDC_ADPDL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x00 ); //CDC_ADRSTBL&R=0

	            /* Power down and Mute PGA */
	            // PGA 
	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x22); //CDC_PGAPDL&R=1
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x33 ); //CDC_PGAMUTEL&R=1
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x11 ); //CDC_PGA_PD_IREF_L&R=1
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x11 ); //CDC_LINEIN_SELL&R=1
	            break;
	    }
    }else if (VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0){
	
	    if ( Input != CDC_INPUT_NONE ) {
	        if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	            /* power on REF */
	            //a)    0x40=0x00 MIC /   0x08 LINEIN Bandgap Mic bias 
	            VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x00 ); //CDC_REFPD[4:0] =5'b00000  //if open dac&adc, open Mbias for adc

				//20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6; 
			if(VCDC_GetVerNo() == CDC_VERSION_7632B_E3){		
				/* power on CTBUF */
				VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
				/* power on SCBUF */
				VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
			}else{
				/* power on CTBUF */
				VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x04 ); //CDC_CTBUFPD = 0
				/* power on SCBUF */
				VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x00); //CDC_SCBUFPD = 1
			}
		}
	    }

	    switch ( Input ) {
	        case CDC_INPUT_MICL:
	            /* Power on ADC */
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x01 ); //CDC_ADCLKENL=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x02 ); //CDC_ADPDIREFL=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x02 ); //CDC_ADPDL=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x01 ); //CDC_ADRSTBL=1

	            /* Power on and unMute PGA */
	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x31); //CDC_PGAPDL=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x30 ); //CDC_PGAMUTEL=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x10 ); //CDC_PGA_PD_IREF_L=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELL=0
	            break;
	        case CDC_INPUT_MICR:
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x02 ); //CDC_ADCLKENR=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x01 ); //CDC_ADPDIREFR=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x01 ); //CDC_ADPDR=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x02 ); //CDC_ADRSTBR=1

	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x13); //CDC_PGAPDR=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x03 ); //CDC_PGAMUTER=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x01 ); //CDC_PGA_PD_IREF_R=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELR=0
	            break;
	        case CDC_INPUT_DUAL_MIC:
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x03 ); //CDC_ADCLKENL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x00 ); //CDC_ADPDIREFL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x00 ); //CDC_ADPDL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x03 ); //CDC_ADRSTBL&R=1

	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x11); //CDC_PGAPDL&R=0
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x00 ); //CDC_PGAMUTEL&R=0
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x00 ); //CDC_PGA_PD_IREF_L&R=0
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x00 ); //CDC_LINEIN_SELL&R=0
	            break;
	        case CDC_INPUT_NONE:
	            if ( g_VCDC_Output == CDC_OUTPUT_NONE ) {
	                /* Power down REF SCBUF and CTBUF */
	                // Bandgap Mic bias 
	                VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x1F ); //CDC_REFPD[4:0] =5'b11111
	                
	                //20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6; 
	                VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
	                VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
	            }

	            /* Power down ADC */
	            // ADC 
	            VCDC_WriteReg ( CDC_REG763X_ADCLKEN, 0x00 ); //CDC_ADCLKENL&R=0
	            VCDC_WriteReg ( CDC_REG763X_ADPDIREF, 0x03 ); //CDC_ADPDIREFL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADPD, 0x03 ); //CDC_ADPDL&R=1
	            VCDC_WriteReg ( CDC_REG763X_ADRSTB, 0x00 ); //CDC_ADRSTBL&R=0

	            /* Power down and Mute PGA */
	            // PGA 
	            //VCDC_WriteReg(CDC_REG763X_PGA, 0x22); //CDC_PGAPDL&R=1
	            VCDC_WriteReg ( CDC_REG763X_PGA, 0x33 ); //CDC_PGAMUTEL&R=1
	            VCDC_WriteReg ( CDC_REG763X_PGA_PD_IREF, 0x11 ); //CDC_PGA_PD_IREF_L&R=1
	            VCDC_WriteReg ( CDC_REG763X_LINEIN_SEL, 0x11 ); //CDC_LINEIN_SELL&R=1
	            break;
	    }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    }else{
    	VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
    g_VCDC_Input = Input;
}
/******************************************************************************
Description:
 	Delays for specified units of 1 ms(not for CDC_TIMERDELAY_DEALNOISE mode).

Parameters:
 	NumOfMilliSec - the interval in ms to delay.
	
Return Value:
	None.

Remarks:
	The maximum interval to delay is 65535 ms.for VCDC_CTST_ICRTL64_9611SC.

*****************************************************************************/
void VCDC_Delayms(UINT16 NumOfMilliSec)
{
	if ((g_VCDC_Config.deal_popupnois&CDC_DEALNOISE_DELAYMS) != 0) {
		while (NumOfMilliSec--)
			VCOMMON_Delay(1000);//delay 1ms
	} else {
#ifndef DRIVER_TEST
		VOSTimeDly(NumOfMilliSec);	//10ms?,	(OS_TICKS_PER_SEC*NumOfMilliSec/1000)
#endif
		VCOMMON_Delay((NumOfMilliSec%10)*1000);	//us
	}
}

UINT8 VCDC_ModeCheck_Output ( UINT8 DacChn )
{
    UINT8 Ret = CDC_OUTPUTMODE_SING;    //default

    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
        VRET_VALUE ret = VRET_ERROR;
        UINT8 PD_FD_Val;

		if((VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC) || (VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC)){
			VCDC_WPRINTF ( "%s: cdc_ver = %d only PMU not audio.\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
			return Ret;
		}
		
        /* read I2C register */
        //maybe the VA7621S can (left diff&right sing), but only need one mode by low bit check, now
        if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
            ret = VCDC_ReadReg ( CDC_REGS_PD_FD, &PD_FD_Val );
        } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
            ret = VCDC_ReadReg ( CDC_REG763X_PD_FD_DG, &PD_FD_Val );
        } else {
            ret = VRET_ERROR;
        }

        if ( ret == VRET_NO_ERROR ) {
            if ( CDC_7621A_PD_FD != PD_FD_Val ) {
                VCDC_EPRINTF ( "%s: CDC_REGS_PD_FD val[%d]!=config[%d].\r\n", FUNCTION_NAME, PD_FD_Val, CDC_7621A_PD_FD );

                if ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) {
                    ret = VCDC_WriteReg ( CDC_REGS_PD_FD, CDC_7621A_PD_FD );
                } else if ( VCDC_GetVerNo() >= CDC_VERSION_7632B_A0 ) {
                    ret = VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD );
                }

                if ( ret != VRET_NO_ERROR ) {
                    VCDC_FPRINTF ( "%s: retry write CDC_REGS_PD_FD, but fail[0x%x]; maybe the 9611 err, not 7621A!\r\n", FUNCTION_NAME, ret );
                    //return Ret;   //return CDC_OUTPUTMODE_DIFF;   //7621A default
                    return ( ( PD_FD_Val > 0 ) ? CDC_OUTPUTMODE_SING : CDC_OUTPUTMODE_DIFF );
                } else {
                    PD_FD_Val = CDC_7621A_PD_FD;
                }
            }

            VCDC_NPRINTF ( "%s: CDC_REGS_PD_FD read val=%d, DacChn%d.\r\n", FUNCTION_NAME, PD_FD_Val, DacChn );
            //ASSERT(PD_FD_Val<=7);

            if ( PD_FD_Val > 0 ) {
                Ret = CDC_OUTPUTMODE_SING;
            } else { //PD_FD_Val == 0
                Ret = CDC_OUTPUTMODE_DIFF;
            }

        } else {
            VCDC_EPRINTF ( "%s: CDC_REGS_PD_FD read fail[0x%x]!\r\n", FUNCTION_NAME, ret );
            Ret = ( CDC_7621A_PD_FD > 0 ) ? CDC_OUTPUTMODE_SING : CDC_OUTPUTMODE_DIFF;
        }
    } else {
        VCDC_FPRINTF ( "%s: cdc_ver = %d err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }

    return Ret;
}
/*
Mode: every bit can support one function 
bit[0]:control PA, 0 means it will control pa register in interface; 1 means it doesn't control but fade pop will control pa register
bit[1]:
*/
void VCDC_ConfigOutput ( UINT8 Output, UINT8 Mode )
{
	if (VCDC_GetVerNo() == CDC_VERSION_PLUS){
	
	    UINT8 cnt = 0;
	    //for future 7621 ver.
	    for ( cnt = 0; cnt < CDC_PLUS_OUTPUT_REGNUM; cnt++ ) {
	        if ( Output != CDC_OUTPUT_NONE ) { //open
	            if ( g_VCDC_Plus_Config.CDC_OutPut[cnt].cdc_reg_addr ) {
	                VCDC_WriteReg ( g_VCDC_Plus_Config.CDC_OutPut[cnt].cdc_reg_addr, g_VCDC_Plus_Config.CDC_OutPut[cnt].cdc_on_value );
	            }

	            if ( g_VCDC_Plus_Config.CDC_OutPut[cnt].cdc_delay ) {
	                VCDC_Delayms ( g_VCDC_Plus_Config.CDC_OutPut[cnt].cdc_delay );
	            }
	        } else {//close
	            if ( g_VCDC_Plus_Config.CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM - 1 - cnt].cdc_reg_addr ) {
	                VCDC_WriteReg ( g_VCDC_Plus_Config.CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM - 1 - cnt].cdc_reg_addr, g_VCDC_Plus_Config.CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM - 1 - cnt].cdc_off_value );
	            }

	            if ( g_VCDC_Plus_Config.CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM - 1 - cnt].cdc_delay ) {
	                VCDC_Delayms ( g_VCDC_Plus_Config.CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM - 1 - cnt].cdc_delay );
	            }
	        }
	    }
	}else if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S){
	
	    if ( Output == CDC_OUTPUT_HP ) {
	        //void VCDC_7621_I2S_Open(void)
	        //should enable aif clock before open I2S.

	        //v507 7621i2s aif DAC clock DAC  VAIF_EnableDAC(AIF_DAC_L | AIF_DAC_R); VCODEC_HW_DACIntEnable  
	        //7621i2s  3 ADC / DAC / I2c 0x5c[4 3] . akt7755  i2s   bit clock  aif   ADC  
	        //UINT8 i = 0;
	        Output = g_VCDC_Config.dac_chn;

	        if ( !g_VCDC_I2S_Config.i2s_ctrl ) {
	            return;
	        }

			if(g_VCDC_I2S_Config.i2s_clk_sel!=2) {//form 9641 to optimization IIS pop by cjh;
	        	VCDC_WriteReg ( CDC_REGS_SPORT_MODE, 0x00 ); // relase TX FIFO
	    	}
			
	        VCDC_WriteReg ( CDC_REGS_I2S_CTRL, g_VCDC_I2S_Config.i2s_ctrl );
	        #if 1//20191123xy, set this reg by eck config; 
	        VCDC_WriteReg ( CDC_REGS_DECIM_CTRL, g_VCDC_I2S_Config.decim_ctrl );
	        #else
	        //  VCDC_WriteReg(0x41, 0xb9); //ATK dsp mode
	        // 2019-4-9 decim_gain_sel[6:3] set 4 b0110. gain x1.25, and Aif reg set sdm_sel[3:0] =4 b0010   -------  4002_000c  bit [7:4]
	        //VCDC_WriteReg ( CDC_REGS_DECIM_CTRL, 0x30 ); // fix 20~1KHZ Frequency response test.4DH DECIM_CTRL, hpf_sel[2:0] must set 0. HPF coefficient selection. 3'b0xx:  by pass HPF
	        VCDC_WriteReg ( CDC_REGS_DECIM_CTRL, 0x10 );//20191113 change by cjh; harman make EQ param by 0x10 before but this will let adc input power samller, and 0x30 is better but harman not change;
	        // va7621i2s decimfilter (24,20) [-8,8)  va9625p bit-SDM [-7,7] 625p DAC  7621i2s /8 
	        //9638c    7621i2s   0x4D = 0x10   eck  eck   Codec Keys-> Debug -> Aif Cdc Ctrl = 00040930  0.8*1.25*(7/8)=0.875
	        //9638c    7621i2s   0x4D = 0x30   eck  eck   Codec Keys-> Debug -> Aif Cdc Ctrl = 00040920  0.65*1.75*(7/8)=0.995
	        #endif
			
	        // change pll frq up, to reduce jitter
	        if(g_VCDC_I2S_Config.i2s_clk_sel!=2) {
	        //for 6M clock
	        	VCDC_WriteReg ( CDC_REGS_PLL_CTRL_N_LOW, 0x00 );
	        	VCDC_WriteReg ( CDC_REGS_PLL_CTRL_N_HIGH, 0x02 );
	        } else {
	        	//for hd 16M clock; // change pll frq ref hds clock 16M
	        	VCDC_WriteReg(CDC_REGS_PLL_CTRL_N_LOW, 0xC0);	// N low
				VCDC_WriteReg(CDC_REGS_PLL_CTRL_N_HIGH, 0x00);	// N high
	        }
			VCDC_WriteReg ( CDC_REGS_PLL_CTRL_R, 0x05 );//use default value is ok;
			
	        if ( 0/*32000 == VCODEC_HW_GetDACSampleRate()*/) {
				//if choose dac as IIS output clock and DACSampleRate =32K then the dac clock is 4M(not 6M) so will config it; 
	            VCDC_WriteReg ( CDC_REGS_PLLOUT_DIV, 150 ); // for 32k test
	        } else {
	            VCDC_WriteReg ( CDC_REGS_PLLOUT_DIV, 100 );
	        }
	        // end change pll frq up

	        if ( g_VCDC_I2S_Config.i2s_clk_sel==1 ) {//dac out IIS clock 
	            VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x04 );
	            VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x10 );
	            VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x07 );// ref dac clock to PLL
	            //  VCDC_WriteReg(0x41, 0xb8);
	        } else if(g_VCDC_I2S_Config.i2s_clk_sel==0){//adc out IIS clock 
	            VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x0f ); // ref Adc clock to PLL
	        } else if(g_VCDC_I2S_Config.i2s_clk_sel==2){//hd out IIS clock 
	            VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x17 ); // ref hd clock to PLL
	        }

			//20191021 bb2, extern dsp&dac&adc;
			//VCDC_ExCodecConfig(1,1);
	    } else if ( Output == CDC_OUTPUT_NONE ) {
	        //VCDC_7621_I2S_Close
	        //UINT8 I2sCtrlVal = 0, i = 0;
	        if ( !g_VCDC_I2S_Config.i2s_ctrl ) {
	            return;
	        }
			// 2019-11-16 by chen jh, rm for fix no I2S date out.TODO 
	/*
	        //VCDC_WriteReg ( CDC_REGS_SPORT_MODE, 0x01 ); // reset TX FIFO

	        if ( g_VCDC_I2S_Config.i2s_clk_sel > 0) {
	            I2sCtrlVal = g_VCDC_I2S_Config.i2s_ctrl;
	            I2sCtrlVal &= ~ ( BIT5 | BIT6 ); // close i2s RX and TX enable
	            VCDC_WriteReg ( CDC_REGS_I2S_CTRL, I2sCtrlVal );
				
				if(g_VCDC_I2S_Config.i2s_clk_sel == 1){
	                // disable FOUT_EN: pll and decim off
	                VCDC_WriteReg ( CDC_REGS_FOUT_EN, 0x04 );
	                VCDC_WriteReg ( CDC_REGS_PLLDIV_OEN, 0x00 );
	                VCDC_WriteReg ( CDC_REGS_PLL_CTRL_OTHERS, 0x10 );
				} else if(g_VCDC_I2S_Config.i2s_clk_sel == 2){
					//select hd out IIS clock, not do disable PLL output;
					;
				}
	        }
			*/
			//20191021 bb2, extern dsp&dac&adc;
			//VCDC_ExCodecConfig(1,0);
	    }
	}else if (VCDC_GetVerNo() == CDC_VERSION_7621S_A){
		    /*enter normal work mode*/  //MEN5 [0] =1, MEN5 [1] =0;
		    UINT8 TimeOut = 0, HP1Reg = 0;
		    UINT8 OutputMode = VCDC_ModeCheck_Output ( g_VCDC_Config.dac_chn ); //CDC_OUTPUTMODE_SING;

		    if ( Output == CDC_OUTPUT_HP ) {
		        Output = g_VCDC_Config.dac_chn;
		        //set the volume to 0 at begin; minish popup noise when power on
		        if ( g_VCDC_Config.deal_popupnois != CDC_DEALNOISE_NONE ) {
		            VCDC_WriteReg ( CDC_REGS_HPVOL, 0 );
		        }
		        //Define delay time 1 == 10mS (default value),  3S.
		        VCDC_Delayms ( g_VCDC_Config.cdc_powerup1_delay1 );

		        /* Power on REF SCBUF and CTBUF */
		        if ( g_VCDC_Input == CDC_INPUT_NONE ) {
		            /* power on REF */
		            VCDC_WriteReg ( CDC_REGS_REFPD, 0x08 ); //the output is power on only for dac, close Mbias
		            /* power on time delay */
		            VCOMMON_Delay ( g_VCDC_Config.cdc_poweron_delayus );
		            /* power on SCBUF */
		            VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x00 | CDC_9611S_SCBUFFICTL ) );
		            /* power on CTBUF */
		            VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) );
		        }
		        //Define delay time 2 == 1mS (default value)
		        VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay2 );
		        if ( OutputMode == CDC_OUTPUTMODE_SING ) {
		            //CDC_PWD_STBF = 0;//not need, if CDC_OUTPUTMODE_DIFF
		            if ( Output == CDC_OUTPUT_HPL ) {
		                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x10 );
		            } else if ( Output == CDC_OUTPUT_HPR ) {
		                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x01 );
		            } else {//CDC_OUTPUT_HP
		                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
		            }
		            //CDC_PWD_HFOUT<2:0> is debug pin: CDC_PWD_HFOUT<0> is set 10uA current, CDC_PWD_HFOUT<0> is set 20uA current; CDC_PWD_HFOUT<0> is set 40uA current.
		        } else { //CDC_OUTPUTMODE_DIFF
		            //If is differential of BA0 and SA0, then CDC_PWD_HFOUT<2:0>=1;
		            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x77 );
		        }
		        //Define delay time 3 == 100mS (default value)  3S.
		        VCDC_Delayms ( g_VCDC_Config.cdc_powerup1_delay3 );
		    }

		    switch ( Output ) {
		        case CDC_OUTPUT_HPL://left channel
		            /* power on LPF, power on DAC digital, reset DAC digital */
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0xC8 | CDC_9611S_DACLPFICTL_L ) ); //CDC_DACPDDIG_L = 0
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0xC0 | CDC_9611S_DACLPFICTL_L ) ); //CDC_DACPDLPF_L = 0
		            /* power on dwa */
		            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x40 | CDC_9611S_DACMODE_L | CDC_9611S_DACCTL_L ) ); //CDC_DACDISDWA_L = 0
		            /* dac digital power on */
		            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x10 ); //CDC_DACRSTDIG_L = 0
		            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 ); //Define delay time 1 == 1mS (default value)
		            /* power on HPA */
		            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x10 ); //CDC_HP_PWD_IREF_L (1->0)
		            VCDC_WriteReg ( CDC_REGS_HP1, 0x34 ); //CDC_HPPD_L (1->0);
		            
		            if ( (Mode & 0x01) == 0 ) {
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, CDC_7621A_PD_OUT_L );
		            }
		            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x11 );
		            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x77 ); //CDC_PWD_HFOUT<2:0> = 111
		            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0xff ); //close //CDC_PD_CHARGE<3:0> = 1111
		            /* power on MIXER, unmute MIXER */
		            VCDC_WriteReg ( CDC_REGS_MIX, ( 0xC0 | CDC_9611S_MIXICTL_L ) );
		            /* unmute PA */
		            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x48 |  CDC_9611S_HPLP_L ) ); //VCDC_WriteReg(CDC_REGS_HP0, 0x48); //

		            break;
		        case CDC_OUTPUT_HPR://right channel
		            /* power on LPF, power on DAC digital, reset DAC digital */
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x8C | CDC_9611S_DACLPFICTL_R ) ); //CDC_DACPDDIG_R = 0
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x0C | CDC_9611S_DACLPFICTL_R ) ); //CDC_DACPDLPF_R = 0
		            /* power on dwa */
		            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x04 | CDC_9611S_DACMODE_R | CDC_9611S_DACCTL_R ) ); //CDC_DACDISDWA_R = 0
		            /* dac digital power on */
		            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x01 ); //CDC_DACRSTDIG_R = 0
		            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 ); //Define delay time 1 == 1mS (default value)
		            /* power on HPA */
		            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x01 ); //CDC_HP_PWD_IREF_R (1->0)
		            VCDC_WriteReg ( CDC_REGS_HP1, 0x43 ); //CDC_HPPD_R (1->0);
		            if ( (Mode & 0x01) == 0 ) {
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, CDC_7621A_PD_OUT_R );
		            }
		            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x11 );
		            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x77 ); //CDC_PWD_HFOUT<2:0> = 111
		            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0xff ); //close //CDC_PD_CHARGE<3:0> = 1111
		            /* power on MIXER, unmute MIXER */
		            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x0C | CDC_9611S_MIXICTL_R ) ); 
		            /* unmute PA */
		            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x84 |  CDC_9611S_HPLP_R ) ); 

		            break;
		        case CDC_OUTPUT_HP://dual channel
		            /* power on LPF, power on DAC digital, reset DAC digital */
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x88 | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x00 | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
		            /* power on dwa */
		            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x00 | CDC_9611S_DACMODE_L | CDC_9611S_DACMODE_R | CDC_9611S_DACCTL_L | CDC_9611S_DACCTL_R ) ); //VCDC_WriteReg(CDC_REGS_DACCTL1, 0x00); //
		            /* dac digital power on */
		            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x00 );
		            //Define delay time 1 == 1mS (default value)
		            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 );
		            /* power on HPA */
		            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x00 );
		            VCDC_WriteReg ( CDC_REGS_HP1, 0x44 );

		            if ( (Mode & 0x01) == 0 ) {
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, CDC_7621A_PD_OUT_R );
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, CDC_7621A_PD_OUT_L );
		            }
		            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x11 );
		            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x77 );
		            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0xff ); //close //CDC_PD_CHARGE<3:0> = 1111
		            /* power on MIXER, unmute MIXER */
		            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x00 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) ); 
		            /* unmute PA */
		            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x88 |  CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) ); //VCDC_WriteReg(CDC_REGS_HP0, 0x88); //

		            break;
		        case CDC_OUTPUT_NONE:
		            //make the volume descending to 0
		            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
		                VCDC_VolShadow ( CDC_REGS_HPVOL, 0, g_VCDC_Config.dac_volset_delayus );
		            }
		            //HPPDSEL=1 power down
		            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_HPPDSEL ) != 0 ) { //#define CDC_DEALNOISE_HPPDSEL     2
		                /* update when CDC_HPVCH_L/R is switched */
		                VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x11 );
		                /* Headphone Popup Noise Suppression enable */
		                VCDC_WriteReg ( CDC_REGS_HP1, 0x2D ); // dac channel 
		                while ( ( TimeOut++ ) < 200 ) { // timeout I2C 
		                    VCDC_ReadReg ( CDC_REGS_HP1, &HP1Reg );
		                    if ( HP1Reg == 0x2D ) {
		                        break;
		                    }
		                }
		                /* connected directly to register */
		                VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x00 );
		            }

		            /* mute PA */
		            VCDC_WriteReg ( CDC_REGS_HP0, 0x44 ); 
		            if ( CDC_OUTPUT_HPL == g_VCDC_Output ) { //g_VCDC_Config.dac_chn
		                VCDC_WriteReg ( CDC_REGS_MIX, ( 0xC4 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
		            } else if ( CDC_OUTPUT_HPR == g_VCDC_Output ) {
		                VCDC_WriteReg ( CDC_REGS_MIX, ( 0x4C | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
		            } else {//CDC_OUTPUT_HP
		                VCDC_WriteReg ( CDC_REGS_MIX, 0x44 );
		            }
		            /* power down LPF, power down DAC digital, reset DAC digital,*/
		            VCDC_WriteReg ( CDC_REGS_DACCTL2, 0xCC );   
		            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x11 ); 
		            /* dis dwa */
		            VCDC_WriteReg ( CDC_REGS_DACCTL1, 0x44 ); 
		            /* power down MIXER */
		            VCDC_WriteReg ( CDC_REGS_MIX, 0xCC ); 
		            /*power up charge circuit, power down HPA*/
		            //CDC_PD_CHARGE<3:0> is debug pin; only for single end, if differential, set CDC_PD_CHARGE<3:0>=1
		            if ( OutputMode == CDC_OUTPUTMODE_SING ) {
		                VCDC_WriteReg ( CDC_REGS_PD_CHARGE, CDC_7621A_PD_CHARGE );
		            } else { //diff
		                VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0xff ); //close //CDC_PD_CHARGE<3:0> = 1111
		            }
		            VCDC_WriteReg ( CDC_REGS_HP1, 0x33 );

		            if ( (Mode & 0x01) == 0 ) {
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x77 );
		                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x77 );
		            }
		            VCDC_Delayms ( g_VCDC_Config.cdc_powerdown1_delay1 ); //Define delay time 1
		            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x11 );

		            if ( g_VCDC_Input == CDC_INPUT_NONE ) {
		                /* power down CTBUF */
		                VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) );
		                /* power down SCBUF */
		                VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
		                /* power down REF */
		                //VCDC_WriteReg(CDC_REGS_REFPD, 0x1F);
		                VCDC_WriteReg ( CDC_REGS_REFPD_HIGH, 0x0f ); //CDC_REFPD[8:5]=0xF
		                VCDC_WriteReg ( CDC_REGS_REFPD_LOW, 0x1f ); //CDC_REFPD[4:0]=0x1F
		            }
					
		            break;
		    }
		}else if (VCDC_GetVerNo() == CDC_VERSION_7621SD_C0){
	    /*enter normal work mode*/  //MEN5 [0] =1, MEN5 [1] =0;
	    UINT8 TimeOut = 0, HP1Reg = 0;
	    UINT8 OutputMode = VCDC_ModeCheck_Output ( g_VCDC_Config.dac_chn ); //CDC_OUTPUTMODE_SING;

	    if ( Output == CDC_OUTPUT_HP ) {
	        Output = g_VCDC_Config.dac_chn;
	        //set the volume to 0 at begin; minish popup noise when power on
	        if ( g_VCDC_Config.deal_popupnois != CDC_DEALNOISE_NONE ) {
	            VCDC_WriteReg ( CDC_REGS_HPVOL, 0 );
	        }
	        //Define delay time 1 == 10mS (default value),  3S.
	        VCDC_Delayms ( g_VCDC_Config.cdc_powerup1_delay1 );
	        /* Power on REF SCBUF and CTBUF */
	        if ( g_VCDC_Input == CDC_INPUT_NONE ) {
	            /* power on REF */
	            VCDC_WriteReg ( CDC_REGS_REFPD, 0x08 ); //the output is power on only for dac, close Mbias
	            /* power on time delay */
	            VCOMMON_Delay ( g_VCDC_Config.cdc_poweron_delayus );
	            /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x00 | CDC_9611S_SCBUFFICTL ) );
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x04 | CDC_9611S_CTBUFFICTL ) ); //VCDC_WriteReg(CDC_REGS_CTBUF, (0x00 | CDC_9611S_CTBUFFICTL));//VCDC_WriteReg(CDC_REGS_CTBUF, 0x04);
	        }
	        //Define delay time 2 == 1mS (default value)
	        VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay2 );

	        if ( OutputMode == CDC_OUTPUTMODE_SING ) {
	            if ( Output == CDC_OUTPUT_HPL ) {
	                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x01 );
	            } else if ( Output == CDC_OUTPUT_HPR ) {
	                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x10 );
	            } else {
	                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x11 ); 
	            }
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT ); 
	        } else { 
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x00 );
                VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
	        }
	        //Define delay time 3 == 100mS (default value)  3S.
	        VCDC_Delayms ( g_VCDC_Config.cdc_powerup1_delay3 );
	    }

	    switch ( Output ) {
	        case CDC_OUTPUT_HPL://left channel
	            /* power on LPF, power on DAC digital, reset DAC digital */
	            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x0C | CDC_9611S_DACLPFICTL_L ) );
	            /* power on dwa */
	            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x40 | CDC_9611S_DACMODE_L | CDC_9611S_DACCTL_L ) );
	            /* dac digital power on */
	            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x10 ); 
	            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 ); //Define delay time 1 == 1mS (default value)
	            /* power on HPA */
	            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x01 );
	            VCDC_WriteReg ( CDC_REGS_HP1, 0x25 );
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, CDC_7621A_PD_OUT_L );
	            }
	            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT );
	            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0x00 ); //close //CDC_PD_CHARGE<3:0> = 0000
	            /* power on MIXER, unmute MIXER */
	            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
	            /* unmute PA */
	            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x48 |  CDC_9611S_HPLP_L ) ); 

	            break;
	        case CDC_OUTPUT_HPR://right channel
	            /* power on LPF, power on DAC digital, reset DAC digital */
	            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0xC0 | CDC_9611S_DACLPFICTL_R ) );
	            /* power on dwa */
	            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x04 | CDC_9611S_DACMODE_R | CDC_9611S_DACCTL_R ) ); //CDC_DACDISDWA_R = 0
	            /* dac digital power on */
	            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x01 ); //CDC_DACRSTDIG_R = 0

	            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 ); //Define delay time 1 == 1mS (default value)
	            /* power on HPA */
	            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x10 );
	            VCDC_WriteReg ( CDC_REGS_HP1, 0x52 );
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, CDC_7621A_PD_OUT_R );
	            }
	            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT );
	            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0x00 ); //close //CDC_PD_CHARGE<3:0> = 0000
	            /* power on MIXER, unmute MIXER */
	            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
	            /* unmute PA */
	            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x84 |  CDC_9611S_HPLP_R ) );

	            break;
	        case CDC_OUTPUT_HP://dual channel
	            /* power on LPF, power on DAC digital, reset DAC digital */
	            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0xCC | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
	            /* power on dwa */
	            VCDC_WriteReg ( CDC_REGS_DACCTL1, ( 0x00 | CDC_9611S_DACMODE_L | CDC_9611S_DACMODE_R | CDC_9611S_DACCTL_L | CDC_9611S_DACCTL_R ) ); //VCDC_WriteReg(CDC_REGS_DACCTL1, 0x00); //
	            /* dac digital power on */
	            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x00 );
	            //Define delay time 1 == 1mS (default value)
	            VCOMMON_Delay ( g_VCDC_Config.cdc_powerup1_delay4 );
	            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x11 );
	            VCDC_WriteReg ( CDC_REGS_HP1, 0x55 );
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, CDC_7621A_PD_OUT_R );
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, CDC_7621A_PD_OUT_L );
	            }
	            VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0x00 ); 
	            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, CDC_7621A_PWD_HFOUT );
	            /* power on MIXER, unmute MIXER */
	            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
	            /* unmute PA */
	            VCDC_WriteReg ( CDC_REGS_HP0, ( 0x88 |  CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );

	            break;
	        case CDC_OUTPUT_NONE:
	            //make the volume descending to 0
	            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { 
	                VCDC_VolShadow ( CDC_REGS_HPVOL, 0, g_VCDC_Config.dac_volset_delayus );
	            }
	            //HPPDSEL=1 power down
	            if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_HPPDSEL ) != 0 ) {
	                /* update when CDC_HPVCH_L/R is switched */
	                VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x11 );
	                /* Headphone Popup Noise Suppression enable */
	                VCDC_WriteReg ( CDC_REGS_HP1, 0x2D ); // dac channel 
	                while ( ( TimeOut++ ) < 200 ) { // timeout I2C 
	                    VCDC_ReadReg ( CDC_REGS_HP1, &HP1Reg );
	                    if ( HP1Reg == 0x2D ) {
	                        break;
	                    }
	                }
	                /* connected directly to register */
	                VCDC_WriteReg ( CDC_REGS_HPPDSEL, 0x00 );
	            }

	            /* mute PA */
	            VCDC_WriteReg ( CDC_REGS_HP0, 0x44 );
	                /* mute MIXER */
	            if ( CDC_OUTPUT_HPL == g_VCDC_Output ) { //g_VCDC_Config.dac_chn
	                VCDC_WriteReg ( CDC_REGS_MIX, ( 0x4C | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) ); 
	            } else if ( CDC_OUTPUT_HPR == g_VCDC_Output ) {
	                VCDC_WriteReg ( CDC_REGS_MIX, ( 0xC4 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) ); 
	            } else {//CDC_OUTPUT_HP
	                VCDC_WriteReg ( CDC_REGS_MIX, ( 0xCC | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
	            }

	            /* power down LPF, power down DAC digital, reset DAC digital,*/
	            VCDC_WriteReg ( CDC_REGS_DACCTL2, ( 0x00 | CDC_9611S_DACLPFICTL_L | CDC_9611S_DACLPFICTL_R ) );
	            VCDC_WriteReg ( CDC_REGS_DACCTL3, 0x11 );
	            /* dis dwa */
	            VCDC_WriteReg ( CDC_REGS_DACCTL1, 0x44 );
	            /* power down MIXER */
	            VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
	            /*power up charge circuit, power down HPA*/
	            //CDC_PD_CHARGE<3:0> is debug pin; only for single end, if differential, set CDC_PD_CHARGE<3:0>=1
	            if ( OutputMode == CDC_OUTPUTMODE_SING ) {
	                VCDC_WriteReg ( CDC_REGS_PD_CHARGE, CDC_7621A_PD_CHARGE );
	            } else { //diff
	                if ( VCDC_ModeCheck_Output ( g_VCDC_Config.dac_chn ) == CDC_OUTPUTMODE_DIFF ) {
	                    VCDC_WriteReg ( CDC_REGS_PD_CHARGE, 0x00 );
	                } else {
	                    VCDC_WriteReg ( CDC_REGS_PD_CHARGE, CDC_7621A_PD_CHARGE );
	                }
	            }

	            VCDC_WriteReg ( CDC_REGS_PWD_STBF, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_PWD_HFOUT, 0x00 );
	            VCDC_WriteReg ( CDC_REGS_HP1, 0x22 );

	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x00 );
	                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x00 );
	            }
	            VCDC_Delayms ( g_VCDC_Config.cdc_powerdown1_delay1 ); //Define delay time 1
	            VCDC_WriteReg ( CDC_REGS_PGA_HP_PWD_IREF, 0x00 );

	            if ( g_VCDC_Input == CDC_INPUT_NONE ) { 
	                /* power down CTBUF */
	                VCDC_WriteReg ( CDC_REGS_CTBUF, ( 0x00 | CDC_9611S_CTBUFFICTL ) ); 
	                /* power down SCBUF */
	                VCDC_WriteReg ( CDC_REGS_SCBUF, ( 0x04 | CDC_9611S_SCBUFFICTL ) );
	                /* power down REF */
	                //VCDC_WriteReg(CDC_REGS_REFPD, 0x1F);
	                VCDC_WriteReg ( CDC_REGS_REFPD_HIGH, 0x0f ); //CDC_REFPD[8:5]=0xF
	                VCDC_WriteReg ( CDC_REGS_REFPD_LOW, 0x1f ); //CDC_REFPD[4:0]=0x1F
	            }

	            break;
	    }
	} else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 ) {

	    if ( Output == CDC_OUTPUT_HP ) {
	        Output = g_VCDC_Config.dac_chn;
	        //set the volume to 0 at begin; minish popup noise when power on
	        if ( g_VCDC_Config.deal_popupnois != CDC_DEALNOISE_NONE ) {
	            VCDC_WriteReg ( CDC_REGS_HPVOL, 0 );
	        }

	        //if (Output != CDC_OUTPUT_NONE) {
	        if ( g_VCDC_Input == CDC_INPUT_NONE ) {
	            /* power on REF */
	            // Bandgap 
	            //VCDC_WriteReg(CDC_REG763X_REFPD_LOW, 0x00); //CDC_REFPD[4:0] =5'b00000    //if open dac&adc, open Mbias for adc
	            VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x08 ); //the output is power on only for dac, close Mbias
				
				//for 7632BA0, B0, C0, D0, E0, F0, C1, D1, E1, F1, E2, F2, E5, F5;
	            /* power on SCBUF */
	            VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x00 ); //CDC_SCBUFPD = 0
	            /* power on CTBUF */
	            VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x04 ); //CDC_CTBUFPD = 1
	        }
	        //}
	    }

	    switch ( Output ) {
	        case CDC_OUTPUT_HPL://left channel
	            /* Power on DAC and Mixer */
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x04); // CDC_DACPDDIG_L = 1
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0x0C ); //CDC_DACPDLPF_L = 1

	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x10 ); //CDC_DACRSTDIG_L = 0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x01 ); //CDC_HP_PWD_IREF_L = 1
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x23 ); //CDC_HPPD_L = 1

	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x4C); //CDC_MIXPD_L = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x48); //CDC_MIXMUTE_L = 0
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG ); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
	            }
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_L=0
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_L=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x40 ); //CDC_HPMUTE_L=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x23); //CDC_HPPD_L=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x23); //CDC_HPPOP_L=0
	            break;
	        case CDC_OUTPUT_HPR://right channel
	            /* Power on DAC and Mixer */
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x40); // CDC_DACPDDIG_R = 1
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0xC0 ); // CDC_DACPDLPF_R = 1

	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x01 ); // CDC_DACRSTDIG_R = 0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_R=0
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x10 ); //CDC_HP_PWD_IREF_R = 1
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x32 ); //CDC_HPPD_R = 1

	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0xC4); //CDC_MIXPD_R = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x84); //CDC_MIXMUTE_R = 0
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG ); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
	            }
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_R=0
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_R=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x04 ); //CDC_HPMUTE_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x32); //CDC_HPPD_R=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x32); //CDC_HPPOP_R=0
	            break;
	        case CDC_OUTPUT_HP://dual channel
	            /* Power on DAC and Mixer */
	            // DAC 
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x44); // CDC_DACPDDIG_L = 1, CDC_DACPDDIG_R = 1,
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0xCC ); //CDC_DACPDLPF_L = 1, CDC_DACPDLPF_R = 1,
	            // DAC digital reset 
	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x00 ); //CDC_DACRSTDIG_L = 0,  CDC_DACRSTDIG_R = 0,
	            // PA IREF 
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x11 ); //CDC_HP_PWD_IREF_L = 1, CDC_HP_PWD_IREF_R = 1
	            // PA 
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x33 ); //CDC_HPPD_L = 1, CDC_HPPD_R = 1
	            // 0x00 0x77 
	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0xCC); //CDC_MIXPD_L = 1,      CDC_MIXPD_R = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x88); //CDC_MIXMUTE_L = 0,    CDC_MIXMUTE_R = 0

	            // PA ECK 0x11,0x22 ,0x77 ECK 0x01,0x02 ,0x07 
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG ); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG ); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
	            }
	            // POP buffer 
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
	            // POP charge 
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_L=0, CDC_PD_CHARGE_R=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x00 ); //CDC_HPMUTE_L=0,       CDC_HPMUTE_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x33); //CDC_HPPD_L=1,     CDC_HPPD_R=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x33); //CDC_HPPOP_L=0,        CDC_HPPOP_R=0
	            break;
	        case CDC_OUTPUT_NONE://close dac
	            //a)    0x8E=0x00 ECK          POP charge 
	            if ( VCDC_ModeCheck_Output ( Output ) == CDC_OUTPUTMODE_DIFF ) {
	                //diff:
	                VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CMFB_L=0,  CDC_PD_FDNCS_L=0,   CDC_PD_FDPCS_L=0
	            } else {//CDC_OUTPUTMODE_SING
	                //sing:
	                VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, CDC_7632BA_PD_CHARGE_DG ); //eck
	            }
	            /* Power down HPPA */
	            if ( (Mode & 0x01) == 0 ) {
	                //b)    0x8B=0x00,  0x8C=0x00                       PA 
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, 0x00 ); //CDC_PD_OUT0I_R=0, CDC_PD_OUT1I_R=0, CDC_PD_OUT2I_R=0
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, 0x00 ); //CDC_PD_OUT0I_L=0, CDC_PD_OUT1I_L=0, CDC_PD_OUT2I_L=0
	            }
	            //Mute PA 
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x44 ); //CDC_HPMUTE_L=1,       CDC_HPMUTE_R=1
	            // PA 
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x22 ); //CDC_HPPD_L=0,     CDC_HPPD_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x22); //CDC_HPPOP_L=1,        CDC_HPPOP_R=1
	            // PA IREF 
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x00 ); //CDC_HP_PWD_IREF_L=0,          CDC_HP_PWD_IREF_R=0

	            /* Power down DAC and Mixer */
	            //DAC digital reset 
	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x11 ); //CDC_DACRSTDIG_L = 1,  CDC_DACRSTDIG_R = 1,
	            // DAC 
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x88); // CDC_DACPDDIG_L = 0, CDC_DACPDDIG_R = 0,
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0x00 ); //CDC_DACPDLPF_L = 0, CDC_DACPDLPF_R = 0,

	            //VCDC_WriteReg(CDC_REG763X_PWD_STBF, 0x00);    //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0, CDC_HP_PULLDOWN_EN_R=0

	            //VCDC_WriteReg(CDC_REG763X_PD_FD_DG, 0x00);    //CDC_PD_CMFB_L=0,      CDC_PD_FDNCS_L=0,       CDC_PD_FDPCS_L=0
	            //VCDC_WriteReg(CDC_REG763X_PD_FD_DG, 0x00);    //CDC_PD_CMFB_R=0,      CDC_PD_FDNCS_R=0,   CDC_PD_FDPCS_R=0

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x00); //CDC_MIXPD_L = 0,      CDC_MIXPD_R = 0
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x44); //CDC_MIXMUTE_L = 1,    CDC_MIXMUTE_R = 1

	            if ( g_VCDC_Input == CDC_INPUT_NONE ) {
	                /* Power down REF SCBUF and CTBUF */
	                // Bandgap 
	                VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x1F ); //CDC_REFPD[4:0] =5'b11111
	                VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
	                VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
	            }
	            break;
	    }
	}else if (VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {

	    if ( Output == CDC_OUTPUT_HP ) {
	        Output = g_VCDC_Config.dac_chn;
	        //set the volume to 0 at begin; minish popup noise when power on
	        if ( g_VCDC_Config.deal_popupnois != CDC_DEALNOISE_NONE ) {
	            VCDC_WriteReg ( CDC_REGS_HPVOL, 0 );
	        }

	        //if (Output != CDC_OUTPUT_NONE) {
	        if ( g_VCDC_Input == CDC_INPUT_NONE ) {
	            /* power on REF */
	            // Bandgap 
	            //VCDC_WriteReg(CDC_REG763X_REFPD_LOW, 0x00); //CDC_REFPD[4:0] =5'b00000    //if open dac&adc, open Mbias for adc
	            VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x08 ); //the output is power on only for dac, close Mbias
				
			//20191107xy, change from 7632BE3, F3; for wujie by VA7632B_A0_AudioCODEC_DesignSpec_v0.6; 
			//E3 version need set reg 0x41 to 0x00,reg 0x42 to 0x04
		     if(VCDC_GetVerNo() == CDC_VERSION_7632B_E3){
			     /* power on CTBUF */
		            VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
					/* power on SCBUF */
		            VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1
		     }else{
		            //F5 version need set reg 0x41 to 0x04,reg 0x42 to 0x00
		            /* power on CTBUF */
		            VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x04 ); //CDC_CTBUFPD = 1
		             /* power on SCBUF */
		            VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x00 ); //CDC_SCBUFPD = 0
		     }
	        }
	        //}
	    }

	    switch ( Output ) {
	        case CDC_OUTPUT_HPL://left channel
	            /* Power on DAC and Mixer */
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x04); // CDC_DACPDDIG_L = 1
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0x0C ); //CDC_DACPDLPF_L = 1

	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x10 ); //CDC_DACRSTDIG_L = 0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x01 ); //CDC_HP_PWD_IREF_L = 1
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x23 ); //CDC_HPPD_L = 1

	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x4C); //CDC_MIXPD_L = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x48); //CDC_MIXMUTE_L = 0
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG ); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
	            }
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_L=0
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_L=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x40 ); //CDC_HPMUTE_L=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x23); //CDC_HPPD_L=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x23); //CDC_HPPOP_L=0
	            break;
	        case CDC_OUTPUT_HPR://right channel
	            /* Power on DAC and Mixer */
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x40); // CDC_DACPDDIG_R = 1
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0xC0 ); // CDC_DACPDLPF_R = 1

	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x01 ); // CDC_DACRSTDIG_R = 0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_R=0
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x10 ); //CDC_HP_PWD_IREF_R = 1
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x32 ); //CDC_HPPD_R = 1

	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0xC4); //CDC_MIXPD_R = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x84); //CDC_MIXMUTE_R = 0
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG ); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
	            }
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_R=0
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_R=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x04 ); //CDC_HPMUTE_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x32); //CDC_HPPD_R=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x32); //CDC_HPPOP_R=0
	            break;
	        case CDC_OUTPUT_HP://dual channel
	            /* Power on DAC and Mixer */
	            // DAC 
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x44); // CDC_DACPDDIG_L = 1, CDC_DACPDDIG_R = 1,
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0xCC ); //CDC_DACPDLPF_L = 1, CDC_DACPDLPF_R = 1,
	            // DAC digital reset 
	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x00 ); //CDC_DACRSTDIG_L = 0,  CDC_DACRSTDIG_R = 0,
	            // PA IREF 
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x11 ); //CDC_HP_PWD_IREF_L = 1, CDC_HP_PWD_IREF_R = 1
	            // PA 
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x33 ); //CDC_HPPD_L = 1, CDC_HPPD_R = 1
	            // 0x00 0x77 
	            VCDC_WriteReg ( CDC_REG763X_PD_FD_DG, CDC_7621A_PD_FD ); //set diff or sing mode.

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0xCC); //CDC_MIXPD_L = 1,      CDC_MIXPD_R = 1
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x88); //CDC_MIXMUTE_L = 0,    CDC_MIXMUTE_R = 0

	            // PA ECK 0x11,0x22 ,0x77 ECK 0x01,0x02 ,0x07 
	            if ( (Mode & 0x01) == 0 ) {
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG ); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG ); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
	            }
	            // POP buffer 
	            VCDC_WriteReg ( CDC_REG763X_PWD_STBF, 0x00 ); //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
	            // POP charge 
	            VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CHARGE_L=0, CDC_PD_CHARGE_R=0

	            /* Power on HPPA */
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x00 ); //CDC_HPMUTE_L=0,       CDC_HPMUTE_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x33); //CDC_HPPD_L=1,     CDC_HPPD_R=1
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x33); //CDC_HPPOP_L=0,        CDC_HPPOP_R=0
	            break;
	        case CDC_OUTPUT_NONE://close dac
	            //a)    0x8E=0x00 ECK          POP charge 
	            if ( VCDC_ModeCheck_Output ( Output ) == CDC_OUTPUTMODE_DIFF ) {
	                //diff:
	                VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, 0x00 ); //CDC_PD_CMFB_L=0,  CDC_PD_FDNCS_L=0,   CDC_PD_FDPCS_L=0
	            } else {//CDC_OUTPUTMODE_SING
	                //sing:
	                VCDC_WriteReg ( CDC_REG763X_PD_CHARGE_DG, CDC_7632BA_PD_CHARGE_DG ); //eck
	            }
	            /* Power down HPPA */
	            if ( (Mode & 0x01) == 0 ) {
	                //b)    0x8B=0x00,  0x8C=0x00                       PA 
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, 0x00 ); //CDC_PD_OUT0I_R=0, CDC_PD_OUT1I_R=0, CDC_PD_OUT2I_R=0
	                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, 0x00 ); //CDC_PD_OUT0I_L=0, CDC_PD_OUT1I_L=0, CDC_PD_OUT2I_L=0
	            }
	            //Mute PA 
	            VCDC_WriteReg ( CDC_REG763X_HP0, 0x44 ); //CDC_HPMUTE_L=1,       CDC_HPMUTE_R=1
	            // PA 
	            VCDC_WriteReg ( CDC_REG763X_HP1, 0x22 ); //CDC_HPPD_L=0,     CDC_HPPD_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP1, 0x22); //CDC_HPPOP_L=1,        CDC_HPPOP_R=1
	            // PA IREF 
	            VCDC_WriteReg ( CDC_REG763X_HP_PWD_IREF, 0x00 ); //CDC_HP_PWD_IREF_L=0,          CDC_HP_PWD_IREF_R=0

	            /* Power down DAC and Mixer */
	            //DAC digital reset 
	            VCDC_WriteReg ( CDC_REG763X_DACCTL3, 0x11 ); //CDC_DACRSTDIG_L = 1,  CDC_DACRSTDIG_R = 1,
	            // DAC 
	            //VCDC_WriteReg(CDC_REG763X_DACCTL2, 0x88); // CDC_DACPDDIG_L = 0, CDC_DACPDDIG_R = 0,
	            VCDC_WriteReg ( CDC_REG763X_DACCTL2, 0x00 ); //CDC_DACPDLPF_L = 0, CDC_DACPDLPF_R = 0,

	            //VCDC_WriteReg(CDC_REG763X_PWD_STBF, 0x00);    //CDC_PWD_STBF_L=0, CDC_PWD_STBF_R=0
	            //VCDC_WriteReg(CDC_REG763X_HP_PULLDOWN_EN_DG, 0x00);   //CDC_HP_PULLDOWN_EN_L=0, CDC_HP_PULLDOWN_EN_R=0

	            //VCDC_WriteReg(CDC_REG763X_PD_FD_DG, 0x00);    //CDC_PD_CMFB_L=0,      CDC_PD_FDNCS_L=0,       CDC_PD_FDPCS_L=0
	            //VCDC_WriteReg(CDC_REG763X_PD_FD_DG, 0x00);    //CDC_PD_CMFB_R=0,      CDC_PD_FDNCS_R=0,   CDC_PD_FDPCS_R=0

	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x00); //CDC_MIXPD_L = 0,      CDC_MIXPD_R = 0
	            //VCDC_WriteReg(CDC_REG763X_MIX, 0x44); //CDC_MIXMUTE_L = 1,    CDC_MIXMUTE_R = 1

	            if ( g_VCDC_Input == CDC_INPUT_NONE ) {
	                /* Power down REF SCBUF and CTBUF */
	                // Bandgap 
	                VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x1F ); //CDC_REFPD[4:0] =5'b11111
	                VCDC_WriteReg ( CDC_REG763X_CTBUF, 0x00 ); //CDC_CTBUFPD = 0
	                VCDC_WriteReg ( CDC_REG763X_SCBUF, 0x04 ); //CDC_SCBUFPD = 1						
	            }
	            break;
	    }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
	}else{
    	VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
	}
    g_VCDC_Output = Output;
}

void VCDC_SetInputGain ( UINT8 GainVal )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
            VCDC_VolShadow ( CDC_REGS_ADVOL, ( ( GainVal << 4 ) | ( GainVal ) ), g_VCDC_Config.adc_volset_delayus );
        } else {
            VCDC_WriteReg ( CDC_REGS_ADVOL, ( ( GainVal << 4 ) | ( GainVal ) ) );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD  1
            VCDC_VolShadow ( CDC_REG763X_ADVOLL_INIT, GainVal, g_VCDC_Config.adc_volset_delayus );
            VCDC_VolShadow ( CDC_REG763X_ADVOLR_INIT, GainVal, g_VCDC_Config.adc_volset_delayus ); //the adc R at 7632B is invalid.
        } else {
            VCDC_WriteReg ( CDC_REG763X_ADVOLL_INIT, GainVal );
            VCDC_WriteReg ( CDC_REG763X_ADVOLR_INIT, GainVal ); //the adc R at 7632B is invalid.
        }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

void VCDC_SetADCVol ( UINT8 AdcChn, UINT8 LVol, UINT8 RVol )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
        UINT8 CurVol = 0;
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD  1
            if ( AdcChn == CDC_INPUT_DUAL_MIC ) {
                VCDC_VolShadow ( CDC_REGS_ADVOL, ( ( RVol << 4 ) | ( LVol ) ), g_VCDC_Config.adc_volset_delayus );
            } else if ( AdcChn == CDC_INPUT_MICL ) {
                VCDC_ReadReg ( CDC_REGS_ADVOL, &CurVol );
                CurVol = ( CurVol & 0xf0 ) | LVol;
                VCDC_VolShadow ( CDC_REGS_ADVOL, CurVol, g_VCDC_Config.adc_volset_delayus );
            } else if ( AdcChn == CDC_INPUT_MICR ) {
                VCDC_ReadReg ( CDC_REGS_ADVOL, &CurVol );
                CurVol = ( CurVol & 0x0f ) | ( RVol << 4 );
                VCDC_VolShadow ( CDC_REGS_ADVOL, CurVol, g_VCDC_Config.adc_volset_delayus );
            } else {
                ;
            }
        } else {
            if ( AdcChn == CDC_INPUT_DUAL_MIC ) {
                VCDC_WriteReg ( CDC_REGS_ADVOL, ( ( RVol << 4 ) | ( LVol ) ) );
            } else if ( AdcChn == CDC_INPUT_MICL ) {
                VCDC_ReadReg ( CDC_REGS_ADVOL, &CurVol );
                VCDC_WriteReg ( CDC_REGS_ADVOL, ( CurVol & 0xf0 ) | LVol );
            } else if ( AdcChn == CDC_INPUT_MICR ) {
                VCDC_ReadReg ( CDC_REGS_ADVOL, &CurVol );
                VCDC_WriteReg ( CDC_REGS_ADVOL, ( CurVol & 0x0f ) | ( RVol << 4 ) );
            } else {
                ;
            }
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD  1
            if ( AdcChn & CDC_INPUT_MICL ) {
                VCDC_VolShadow ( CDC_REG763X_ADVOLL_INIT, LVol, g_VCDC_Config.adc_volset_delayus );
            }
            if ( AdcChn & CDC_INPUT_MICR ) {
                VCDC_VolShadow ( CDC_REG763X_ADVOLR_INIT, RVol, g_VCDC_Config.adc_volset_delayus );    //the adc R at 7632B is invalid.
            }
        } else {
            if ( AdcChn & CDC_INPUT_MICL ) {
                VCDC_WriteReg ( CDC_REG763X_ADVOLL_INIT, LVol );
            }
            if ( AdcChn & CDC_INPUT_MICR ) {
                VCDC_WriteReg ( CDC_REG763X_ADVOLR_INIT, RVol );    //the adc R at 7632B is invalid.
            }
        }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}



typedef struct {
    SINT8  gain_db;
    UINT8  HP_val;
} VCDC_7621SDC0_GAINTABLE;

#define VCDC_7621SDC0_GAIN_NUM  64
const VCDC_7621SDC0_GAINTABLE g_7621SDC0_GainTable[VCDC_7621SDC0_GAIN_NUM] = {
    /*  dB  --- value   */
    {-36,   0x00},      //-18.0db
    {-35,   0x01},      //-17.5db
    {-34,   0x02},      //-17.0db
    {-33,   0x03},      //-16.5db
    {-32,   0x10},      //-16.0db
    {-31,   0x11},      //-15.5db
    {-30,   0x12},      //-15.0db
    {-29,   0x13},      //-14.5db
    {-28,   0x20},      //-14.0db
    {-27,   0x21},      //-13.5db
    {-26,   0x22},      //-13.0db
    {-25,   0x23},      //-12.5db
    {-24,   0x30},      //-12.0db
    {-23,   0x31},      //-11.5db
    {-22,   0x32},      //-11.0db
    {-21,   0x33},      //-10.5db
    {-20,   0x40},      //-10.0db
    {-19,   0x41},      //-9.5db
    {-18,   0x42},      //-9.0db
    {-17,   0x43},      //-8.5db
    {-16,   0x50},      //-8.0db
    {-15,   0x51},      //-7.5db
    {-14,   0x52},      //-7.0db
    {-13,   0x53},      //-6.5db
    {-12,   0x60},      //-6.0db
    {-11,   0x61},      //-5.5db
    {-10,   0x62},      //-5.0db
    {-9,    0x63},      //-4.5db
    {-8,    0x70},      //-4.0db
    {-7,    0x71},      //-3.5db
    {-6,    0x72},      //-3.0db
    {-5,    0x73},      //-2.2db
    {-4,    0x80},      //-2.0db
    {-3,    0x81},      //-1.5db
    {-2,    0x82},      //-1.0db
    {-1,    0x83},      //-0.5db
    { 0,    0x90},      //0db
    { 1,    0x91},      //0.5db
    { 2,    0x92},      //1.0db
    { 3,    0x93},      //1.5db
    { 4,    0xA0},      //2.0db
    { 5,    0xA1},      //2.5db
    { 6,    0xA2},      //3.0db
    { 7,    0xA3},      //3.5db
    { 8,    0xB0},      //4.0db
    { 9,    0xB1},      //4.5db
    {10,    0xB2},      //5.0db
    {11,    0xB3},      //5.5db { 6,    8173},      //0.5db
    {12,    0xC0},      //6.0db
    {13,    0xC1},      //6.5db
    {14,    0xC2},      //7.0db
    {15,    0xC3},      //7.5db
    {16,    0xD0},      //8.0db
    {17,    0xD1},      //8.5db
    {18,    0xD2},      //9.0db
    {19,    0xD3},      //9.5db
    {20,    0xE0},      //10.0db
    {21,    0xE1},      //10.5db
    {22,    0xE2},      //11.0db
    {23,    0xE3},      //11.5db
    {24,    0xF0},      //12.0db
    {24,    0xF1},      //12.0db
    {24,    0xF2},      //12.0db
    {24,    0xF3},      //12.0db
};
/*****************************************************************
    get HP change volume gain step:
    0----2db----other 7621 version
    1----0.5db---7621SD_C0/7632B_A0
******************************************************************/
//UINT8 VCDC_GetHPVOL_GainStepDB ( void )
UINT8 VCDC_GetGainMode ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 || VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        if ( CDC_7621SD_HPAGAIN_MODE & 0x01 ) { //0.5db; //some control as CDC_REG763X_HPAGAIN_MODE_DG for 7632B; 
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;    //other 7621 2.0db
    }
}

UINT8 VCDC_GetHPVOL_7621SDC0 ( SINT16 Gain ) //0.1db
{
    if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
        UINT8 i = 0;
        SINT8 AnalogGain = Gain / 5;
        if ( CDC_7621SD_HPAGAIN_MODE & 0x01 ) { //0.5db
            for ( i = 0; i < VCDC_7621SDC0_GAIN_NUM; i++ ) {
                if ( AnalogGain <= g_7621SDC0_GainTable[i].gain_db ) {
                    return g_7621SDC0_GainTable[i].HP_val;
                }
            }
        } else { // 2.0db, CDC_HPVOL_R[3:0] control both.
            UINT8 HP_val = ( AnalogGain - ( -18 ) * 2 ) / 4;
            return ( HP_val << 4 );
        }
    } /*else {//other 7621
        //return ( Gain - ( -18 ) * 10 ) / 20; //2.0db
        return 0;
    }*/
		
		return 0;
}

SINT8 VCDC_GetGainDB_7621SDC0 ( UINT8 HPVOL )
{
	if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
	    UINT8 i = 0;
	    for ( i = 0; i < VCDC_7621SDC0_GAIN_NUM; i++ ) {
	        if ( HPVOL == g_7621SDC0_GainTable[i].HP_val ) {
	            return g_7621SDC0_GainTable[i].gain_db;
	        }
	    }
    }/* else {
        return 0;
    }*/
		
		return 0;
}

UINT8 VCDC_GetHPVOL_7632B ( SINT16 Gain ) //0.1db
{
    if (VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        //UINT8 i = 0;
        //SINT8 AnalogGain = Gain / 5;
			
        if ( CDC_7621SD_HPAGAIN_MODE & 0x01 ) { //0.5db; -18db(Gain== -180)-->0(reg HPVOL_6bit),  -17.5db(Gain== -180)-->1(reg HPVOL_6bit), 12db(Gain== 120)-->60(reg HPVOL_6bit)
            return ( (Gain/5) + 36);
        } else { // 2.0db; -18db(Gain== -180)-->0(reg HPVOL_4bit), -16db(Gain== -160)-->1(reg HPVOL_4bit), 12db(Gain== 120)-->15(reg HPVOL_4bit)
            return ((Gain/20) + 9);
        }
    } else {
        return 0;
    }
}

SINT8 VCDC_GetGainDB_7632B ( UINT8 HPVOL )
{
    if (VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
		if ( CDC_7621SD_HPAGAIN_MODE & 0x01 ) { //0.5db; 0(reg HPVOL_6bit) --> -18db(Gain== -180), 1(reg HPVOL_6bit) --> -17.5db(Gain== -180), 60/61/62/63(reg HPVOL_6bit) --> 12db(Gain== 120);
			if((HPVOL*5) <= 300) {
            	return ((HPVOL*5) - 180);
			} else {
				return 120;//when 61/62/63(reg HPVOL_6bit) --> 12db(Gain== 120);
			}
        } else { // 2.0db; 0(reg HPVOL_4bit)--> -18db(Gain== -180), 1(reg HPVOL_4bit) --> -16db(Gain== -160), 15(reg HPVOL_4bit) --> 12db(Gain== 120);
            return ((HPVOL*20) - 180);
        }
    } else {
        return 0;
    }
}

//for 0.5db get reg value by input 0.1db Gain;
UINT8 VCDC_GetHPVOL_05DB ( SINT16 Gain ) //0.1db
{
    if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ){
		return (VCDC_GetHPVOL_7621SDC0(Gain));
    } else if (VCDC_GetVerNo() >= CDC_VERSION_7632B_A0){	
    	return (VCDC_GetHPVOL_7632B(Gain));
    } else {//other 7621
        return ( Gain - ( -18 ) * 10 ) / 20; //2.0db
    }
}

//for 0.5db get db by reg value; the return is 0.1db unit;
SINT16 VCDC_GetGainDB_05DB ( UINT8 HPVOL )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ){
		return ((SINT16)VCDC_GetGainDB_7621SDC0(HPVOL)*5);
    } else if (VCDC_GetVerNo() >= CDC_VERSION_7632B_A0){	
    	return (VCDC_GetGainDB_7632B(HPVOL));
    } else {//other 7621
        return 0;
    }
}


//Gain: input gain 0.1db; return g_codec_hw.dac.vol for 0.5db 7621SD or 7632B and 0.1db others.
UINT8 VCDC_GetHPVOL ( SINT16 Gain )
{
   AT_NPRINTF("VCDC_GetHPVOL mode=%d\r\n",VCDC_GetGainMode());
    if ((VCDC_GetGainMode()&0x01) > 0) {
        return ( Gain - ( -18 ) * 10 ) / 5; //0.5db
    } else {//other 7621
        return ( Gain - ( -18 ) * 10 ) / 20; //2.0db
    }
}

void VCDC_SetOutputGain ( UINT8 GainVal )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if (( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) && ((VCDC_GetGainMode()&0x01) > 0)) {
        VCDC_WriteReg ( CDC_REGS_HPVOL, GainVal );
    } else if (( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 )) { //stereo
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD   1
            VCDC_VolShadow ( CDC_REGS_HPVOL, ( ( GainVal << 4 ) | ( GainVal ) ), g_VCDC_Config.dac_volset_delayus );
        } else {
            VCDC_WriteReg ( CDC_REGS_HPVOL, ( ( GainVal << 4 ) | ( GainVal ) ) );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        if ((VCDC_GetGainMode()&0x01) > 0) {
            VCDC_WriteReg ( CDC_REG763X_HPVOL_6BIT_L, GainVal );
            VCDC_WriteReg ( CDC_REG763X_HPVOL_6BIT_R, GainVal );
        } else {
            VCDC_WriteReg ( CDC_REG763X_HPVOL_4BIT, ( ( GainVal << 4 ) | ( GainVal ) ) );
        }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

void VCDC_MuteOutput ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else {
        switch ( g_VCDC_Output ) {
            case CDC_OUTPUT_HP:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
                    /* mute PA */
                    VCDC_WriteReg ( CDC_REGS_HP0, ( 0x44 |  CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );
                    /* mute MIXER */
                    if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
                        VCDC_WriteReg ( CDC_REGS_MIX, ( 0xCC | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                    }
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
                    ;
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
					;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
        }
    }
}

void VCDC_UnmuteOutput ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else {
        switch ( g_VCDC_Output ) {
            case CDC_OUTPUT_HP:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
                    /* unmute PA */
                    VCDC_WriteReg ( CDC_REGS_HP0, ( 0x88 |  CDC_9611S_HPLP_L | CDC_9611S_HPLP_R ) );
                    /* unmute MIXER */
                    VCDC_WriteReg ( CDC_REGS_MIX, ( 0x00 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                    if ( VCDC_GetVerNo() == CDC_VERSION_7621EA0 ) {
                        VCDC_WriteReg ( CDC_REGS_MIX, 0x44 );
                    } else if ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) {
                        //VCDC_WriteReg(CDC_REGS_MIX, (0x88 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R));//for VA7621SD
                        VCDC_WriteReg ( CDC_REGS_MIX, ( 0x44 | CDC_9611S_MIXICTL_L | CDC_9611S_MIXICTL_R ) );
                    }
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
                    ;
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
					;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
        }
    }
}

void VCDC_MuteInput ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else {
#if CDC_NEWVERSION_DEFINE 
        switch ( g_VCDC_Input ) {
            case CDC_INPUT_MICL:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E, 7621A
                    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x31 );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x0D );
                    }
                } else {
                    ;
                }
                break;
            case CDC_INPUT_MICR:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E, 7621A
                    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x13 );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x07 );
                    }
                } else {
                    ;
                }
                break;
            case CDC_INPUT_DUAL_MIC:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E, 7621A
                    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x11 );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x05 );
                    }
                } else {
                    ;
                }
                break;
        }

#else
        switch ( g_VCDC_Input ) {
            case CDC_INPUT_MICL:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x31 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x31 );
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
					;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
            case CDC_INPUT_MICR:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x13 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x13 );
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
					;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
            case CDC_INPUT_DUAL_MIC:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x11 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x11 );
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
                	;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
        }

#endif
    }
}

void VCDC_UnmuteInput ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else {
#if CDC_NEWVERSION_DEFINE
        switch ( g_VCDC_Input ) {
            case CDC_INPUT_MICL:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E
                    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x30 );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x0C );
                    }
                } else {
                    ;
                }
                break;
            case CDC_INPUT_MICR:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E
                    if ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );
                    } else {
                        VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );    //??0x05
                    }
                } else {
                    ;
                }
                break;
            case CDC_INPUT_DUAL_MIC:
                if ( VCDC_GetVerNo() >= CDC_VERSION_9611S ) { //9611S, 9611S_B, 9611S_C, 9611S_D, 9611S_E, 7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x00 );
                } else {
                    ;
                }
                break;
        }

#else

        switch ( g_VCDC_Input ) {
            case CDC_INPUT_MICL:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x30 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x30 );          
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
					;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
            case CDC_INPUT_MICR:
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x03 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x03 );
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x03 );
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
            case CDC_INPUT_DUAL_MIC:
                //if ((g_VCDC_Config.cdc_ver == CDC_VERSION_9611S)||(g_VCDC_Config.cdc_ver == CDC_VERSION_9611S_B)||(g_VCDC_Config.cdc_ver == CDC_VERSION_9611S_C)||(g_VCDC_Config.cdc_ver == CDC_VERSION_9611S_D)) {
                if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo
                    VCDC_WriteReg ( CDC_REGS_PGA, 0x00 );
                } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
                    VCDC_WriteReg ( CDC_REG763X_PGA, 0x00 );
				} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
                    ;//only PMU not audio;
                } else {
                    VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
                }
                break;
        }
#endif
    }
}

void VCDC_CloseMBias ( void )
{
    UINT8 GainVal = 0;

    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( ( VCDC_GetVerNo() == CDC_VERSION_7621_A ) || ( VCDC_GetVerNo() == CDC_VERSION_7621G_A ) ) { //BA0 and GA0
        //bit[2]==CDC_REFPD[7]==mbias "=1" to close,CDC_REFPD[5]==debug pin
        if ( VCDC_ReadReg ( CDC_REGS_REFPD_HIGH, &GainVal ) == VRET_NO_ERROR ) {
            //CDC_REG_REFPD //9611F
            //CDC_REGS_REFPD    //9611S~
            VCDC_WriteReg ( CDC_REGS_REFPD_HIGH, ( GainVal | 0x04 ) ); //set bit[2]"1" to close mbias
        }
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //VA7621A
        //9611+7621S
        if ( VCDC_ReadReg ( CDC_REGS_REFPD, &GainVal ) == VRET_NO_ERROR ) {
            VCDC_WriteReg ( CDC_REGS_REFPD, ( GainVal | 0x08 ) ); // close Mbias
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        VCDC_WriteReg ( CDC_REG763X_REFPD_LOW, 0x08 ); //the output is power on only for dac, close Mbias
    } else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

#if VCDC_DBG_EN
UINT8 g_7621Reg_Value[256] = {0};
#endif
VRET_VALUE VCDC_WriteReg ( UINT8 Reg, UINT8 Value )
{
    VRET_VALUE ret;
	ret = VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,CDC_I2C_SLAVE_ADDR,Reg,&Value,1);
    if ( ret ) {
        VCDC_FPRINTF ( "%s: CDC config failed!\r\n", FUNCTION_NAME );
    } else {
    	#if VCDC_DBG_EN
        if ( Reg < 256 ) {
            g_7621Reg_Value[Reg] = Value;
        }
	#endif
    }
    return ret;
}

VRET_VALUE VCDC_ReadReg ( UINT8 Reg, UINT8 *p_Value )
{
    VRET_VALUE ret;
	ret = VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,CDC_I2C_SLAVE_ADDR,Reg,p_Value,1);
    if ( ret ) {
        VCDC_FPRINTF ( "%s: CDC reg read failed!\r\n", FUNCTION_NAME );
    }
    return ret;
}

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
void VCDC_VolShadow ( UINT8 DestReg, UINT8 DestVol, UINT16 DelayTime )
{
    UINT8 CurVol = 0;
    UINT8 CurVolL = 0, CurVolR = 0;
    UINT8 DestVolL = 0, DestVolR = 0;

    if ( VCDC_GetVerNo() < CDC_VERSION_7621_A ) { //9611
        VCDC_WriteReg ( DestReg, DestVol );
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) { //stereo, 7621
        DestVolL = DestVol & 0x0f;
        DestVolR = ( DestVol >> 4 ) & 0x0f;
        if ( DestVolL != DestVolR ) {
            VCDC_WPRINTF ( "The DestVol[reg=0x%x], left[%d] != right[%d].\r\n", DestReg, DestVolL, DestVolR );
        }

        VCDC_ReadReg ( DestReg, &CurVol );
        CurVolL = CurVol & 0x0f;
        CurVolR = ( CurVol >> 4 ) & 0x0f;
        if ( CurVolL != CurVolR ) {
            VCDC_WPRINTF ( "The CurVol[reg=0x%x], left[%d] != right[%d].\r\n", DestReg, DestVolL, DestVolR );
        }

        while ( ( CurVolL != DestVolL ) || ( CurVolR != DestVolR ) ) {
            if ( CurVolL != DestVolL ) { //left channel, [3:0] CDC_HPVOL_L or CDC_ADVOLL
                if ( CurVolL < DestVolL ) {
                    CurVolL++;
                } else { //CurVolL > DestVolL
                    CurVolL--;
                }
            }
            if ( CurVolR != DestVolR ) { //right channel,[7:4] CDC_HPVOL_R or CDC_ADVOLR
                if ( CurVolR < DestVolR ) {
                    CurVolR++;
                } else { //CurVolR > DestVolR
                    CurVolR--;
                }
            }

            CurVol = CurVolL | ( CurVolR << 4 );
            VCDC_WriteReg ( DestReg, CurVol );
            VCOMMON_Delay ( DelayTime );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        VCDC_ReadReg ( DestReg, &CurVol );

        while ( CurVol != DestVol ) {
            if ( CurVol < DestVol ) {
                CurVol++;
            } else { //CurVolL > DestVolL
                CurVol--;
            }

            VCDC_WriteReg ( DestReg, CurVol );
            VCOMMON_Delay ( DelayTime );
        }
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

void VCDC_7621_PA_Opne ( void )
{
   if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) {
        if ( VCDC_GetVerNo() == CDC_VERSION_7621_A ) {
            VCDC_WriteReg ( CDC_REGS_PD_OUT, CDC_7621A_PD_OUT );
        } else if ( VCDC_GetVerNo() == CDC_VERSION_7621EA0 ) {
            if ( g_VCDC_Output & CDC_OUTPUT_HPL ) {
                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x77 | CDC_7621A_PD_OUT_L );
            }
            if ( g_VCDC_Output & CDC_OUTPUT_HPR ) {
                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x77 | CDC_7621A_PD_OUT_R );
            }
        } else {
            if ( g_VCDC_Output & CDC_OUTPUT_HPL ) {
                VCDC_WriteReg ( CDC_REGS_PD_OUT_L, CDC_7621A_PD_OUT_L );
            }
            if ( g_VCDC_Output & CDC_OUTPUT_HPR ) {
                VCDC_WriteReg ( CDC_REGS_PD_OUT_R, CDC_7621A_PD_OUT_R );
            }
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        if ( g_VCDC_Output & CDC_OUTPUT_HPL ) {
            VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG );    //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
        }
        if ( g_VCDC_Output & CDC_OUTPUT_HPR ) {
            VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG );    //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
        }
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}

void VCDC_7621_PA_Close ( void )
{
   if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( ( VCDC_GetVerNo() >= CDC_VERSION_7621_A ) && ( VCDC_GetVerNo() < CDC_VERSION_7632B_A0 ) ) {
        if ( VCDC_GetVerNo() == CDC_VERSION_7621_A ) {
            VCDC_WriteReg ( CDC_REGS_PD_OUT, 0x77 );
        } else if ( ( VCDC_GetVerNo() == CDC_VERSION_7621EA0 ) || ( VCDC_GetVerNo() == CDC_VERSION_7621SD_C0 ) ) {
            VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x00 );
            VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x00 );
        } else {
            VCDC_WriteReg ( CDC_REGS_PD_OUT_R, 0x77 );
            VCDC_WriteReg ( CDC_REGS_PD_OUT_L, 0x77 );
        }
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //VA7632B
        VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, 0x00 ); //CDC_PD_OUT0I_R=0, CDC_PD_OUT1I_R=0, CDC_PD_OUT2I_R=0
        VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, 0x00 ); //CDC_PD_OUT0I_L=0, CDC_PD_OUT1I_L=0, CDC_PD_OUT2I_L=0
    } else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
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
UINT8 VCDC_GetDACStat ( void )
{
    return g_VCDC_Output;
}

UINT8 VCDC_GetADCStat ( void )
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
UINT8 VCDC_GetVerNo ( void )
{
	static UINT8 g_VCDC_Ver = CDC_VERSION_NONE;
	if(g_VCDC_Ver != CDC_VERSION_NONE)
		return g_VCDC_Ver;
	#if 1//20240123xy, support automatic recognition by Reg chip ID, add VA7638_A0(just PMU DCDC had different to VA7632B_F5) ; 
	if(CDC_VERSION_DEFAULT == CDC_VERSION_CHECK){
		VRET_VALUE ret=0;
		UINT8 ver=0;

		//ret = VPMU_GetMCVer(&ver);
		ret = VCDC_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
		//VCOMMON_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
		
        if(VRET_NO_ERROR == ret) {
			if((ver==0x11)||(ver==0x12)) { //VA7632B_E5==0x11, VA7632B_F5==0x12, and audio just one adc ;
            	(g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_F5);
			} else if(ver==0x01) { 
				//20240201xy, when CHIP_ID3=0x01 then should distinguish the had done chip is 7652PMU_CSMC or 7638_A0, then should check reg CHIP_ID2; 
 				UINT8 chip_ID2=0; 
				if(VRET_NO_ERROR ==VCDC_ReadReg ( 0xF4/*PMU_REG763X_CHIP_ID2*/, &chip_ID2)){
					if(chip_ID2==0xba) {
		            	//VA7638_A0(for 9648C/9647X)==0x01 have audio and two adc, just PMU DCDC had different to VA7632B_F5;
            			return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7638_A0);
					} else if(chip_ID2==0xbb) {
						//CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
						return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
					}else {
						VCOMMON_EPRINTF("GetVerNo, but CHIP_ID2=%x, not define!\r\n", chip_ID2);
						return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
					}
				} else {
					VCOMMON_EPRINTF("GetVerNo, but IIC read CHIP_ID2 fai!\r\n");
					return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
				}
			} else if((ver==0x0C)||(ver==0x0D)) { //VA7632B_E3==0x0C, VA7632B_F3==0x0D;
            	(g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_E3);
			//} else if((ver>=0x01)&&(ver<=0x04)) { //CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
			} else if((ver>0x01)&&(ver<=0x04)) { //CSMC 7652PMU==0x02~0x04 maybe used for future reserved;CSMC 7652PMU(for 9652, remove audio)==0x01 had done; 
            	return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
			} else if((ver>=0xa0)&&(ver<=0xcf)) { //TSMC 7652PMU(for 9652, remove audio) ==0xa0~0xcf will been done, the pmu driver most some as 7652PMU_CSMC, different at 02H MEN5[1:0];
            	(g_VCDC_Config.cdc_ver = CDC_VERSION_7652PMU_TSMC);
			} else {
				VCOMMON_EPRINTF("GetVerNo, but CHIP_ID3=%x, not define!\r\n", ver);
				return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
			}
			g_VCDC_Ver = g_VCDC_Config.cdc_ver;
			return g_VCDC_Ver;
        } else {
        	VCOMMON_EPRINTF("GetVerNo, but IIC read fail[ret=%x]!\r\n", ret);
            return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
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
		ret = VCDC_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
		//VCOMMON_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
		
        if(VRET_NO_ERROR == ret) {
			if((ver==0x11)||(ver==0x12)||(ver==0x01)) { //VA7632B_E5==0x11, VA7632B_F5==0x12,  VA7638_A0(for 9648C)==0x01 have audio and two adc ;
            	return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_F5);
			} else if((ver==0x0C)||(ver==0x0D)) { //VA7632B_E3==0x0C, VA7632B_F3==0x0D;
            	return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_E3);
			} else if((ver>=0x01)&&(ver<=0x04)) { //CSMC 7652PMU(for 9652, remove audio)==0x01 had done; CSMC 7652PMU==0x02~0x04 maybe used for future reserved;
            	return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7652PMU_CSMC);
			} else if((ver>=0xa0)&&(ver<=0xcf)) { //TSMC 7652PMU(for 9652, remove audio) ==0xa0~0xcf will been done, the pmu driver most some as 7652PMU_CSMC, different at 02H MEN5[1:0];
            	return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7652PMU_TSMC);
			} else {
				VCOMMON_EPRINTF("GetVerNo, but CHIP_ID3=%x, not define!\r\n", ver);
				return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
			}
        } else {
        	VCOMMON_EPRINTF("GetVerNo, but IIC read fail[ret=%x]!\r\n", ret);
            return  (g_VCDC_Config.cdc_ver = CDC_VERSION_NONE);
        }
	} else {
		return CDC_VERSION_DEFAULT; //may optimization for code size down;
	}
	
	#else
	#if 1//20210105xy, temporary special check F3 or F5; if F3F5 the bt bin more 23K size (from 728K to 751K);
	if(CDC_VERSION_DEFAULT == CDC_VERSION_7632B_F3F5){
		VRET_VALUE ret=0;
		UINT8 ver=0;

		if((g_VCDC_Config.cdc_ver == CDC_VERSION_7632B_E3) || (g_VCDC_Config.cdc_ver == CDC_VERSION_7632B_F5)) 
			  return g_VCDC_Config.cdc_ver;
		else {
				//ret = VPMU_GetMCVer(&ver);
				ret = VCDC_ReadReg ( 0xF5/*PMU_REG763X_CHIP_ID3*/, &ver);
				//VCOMMON_NPRINTF("GetVerNo, MC=%x[ret=%x]!\r\n", ver, ret);
				
				// if((VRET_NO_ERROR == ret)&&((ver==0x11)||(ver==0x12)))
                if (1) {    //  … … –chip_id ” F5 
                    return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_F5);
                }
                else {
                    return  (g_VCDC_Config.cdc_ver = CDC_VERSION_7632B_E3);
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

#if (VDEV_CHIP_VERSION < CHIP_9638P)
//0==not I2S, 1==7621I2S-Dac,
UINT8 VCDC_I2S_GetMode ( void )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_7621_I2S ) {
        return 1;
    } else {
        return 0;
    }
}
#endif

//FOUT_EN[4:3] pll_ref_src_sel[1:0], referecne clock selection: 0==dac clock, 1==adc clock, 2==hd_scl 
//return value: 0==adc clock, 1==dac clock, 2==hd_scl;
UINT8 VCDC_I2S_GetRefClkSel ( void )
{
    return g_VCDC_I2S_Config.i2s_clk_sel;
}

//for 7621I2S do extern codec(with I2S data) control; ctrl==0, close extern codec, ctrl==1, open extern codec;
void VCDC_I2S_ConfigExCodec ( UINT8 ctrl )
{
	#if (VDEV_CHIP_VERSION < CHIP_9638P)
	if (VCDC_I2S_GetMode() > 0)
	#else //9648, had IIS, all will use ex-IIS not by 7621IIS;
	if(1)
	#endif
    {
        #if 0//20191113xy, BB2 not had Ti2652, so not need;
        UINT8 i;
        if ( ctrl == 1 ) {
            //open extern codec for I2S dac Ti 2652.
            for ( i = 0; i < g_VCDC_I2S_Config.open_set_reg_num; i++ ) {
                if ( g_VCDC_I2S_Config.open_set_regs[i].delay_us ) {
                    VCOMMON_Delay ( g_VCDC_I2S_Config.open_set_regs[i].delay_us );
                }
                VCDC_VI2C_register_write ( g_VCDC_I2S_Config.open_set_regs[i].i2c_addr, g_VCDC_I2S_Config.open_set_regs[i].reg_addr, g_VCDC_I2S_Config.open_set_regs[i].reg_data );
            }
        } else if ( ctrl == 0 ) {
            for ( i = 0; i < g_VCDC_I2S_Config.close_set_reg_num; i++ ) {
                if ( g_VCDC_I2S_Config.close_set_regs[i].delay_us ) {
                    VCOMMON_Delay ( g_VCDC_I2S_Config.close_set_regs[i].delay_us );
                }
                VCDC_VI2C_register_write ( g_VCDC_I2S_Config.close_set_regs[i].i2c_addr, g_VCDC_I2S_Config.close_set_regs[i].reg_addr, g_VCDC_I2S_Config.close_set_regs[i].reg_data );
            }
        } else {
            ;
        }
        #endif
    } else {
        ;//not extern codec,
    }
}

#define CDC_7632BA_PDOUT_MAXGAIN_THRESHOLD      (g_VCDC_Config.cdc_powerup1_delay1)
void VCDC_PDOUTConfigByGain ( SINT16 Gain )
{
    /* by wujie 20190904
     0x8B 0x8C 
    1 ECK 
    2 0x8B 0x8C ECK 0x8B=0x11 0x8C=0x11 0x8B=0x01 0x8C=0x01 
    3 0x8B 0x8C ECK 0x8B=0x11 0x8C=0x11 0x8B=0x01 0x8C=0x01 
    */
    if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) { //only for VA7632B
        //the Gain = DigitGain+AnalogGain
        if ( Gain > CDC_7632BA_PDOUT_MAXGAIN_THRESHOLD ) {
            VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_DG ); //CDC_PD_OUT0I_L= ECK, CDC_PD_OUT1I_L= ECK, CDC_PD_OUT2I_L= ECK
            VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_DG ); //CDC_PD_OUT0I_R= ECK, CDC_PD_OUT1I_R= ECK, CDC_PD_OUT2I_R= ECK
        } else {
            if ( CDC_7621A_PD_FD == 0 ) { //CDC_OUTPUTMODE_DIFF
                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_0 ); //VCDC_WriteReg(CDC_REG763X_PD_OUT_L_DG, 0x11);
                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_0 ); //VCDC_WriteReg(CDC_REG763X_PD_OUT_R_DG, 0x11);
            } else {//CDC_OUTPUTMODE_SING
                VCDC_WriteReg ( CDC_REG763X_PD_OUT_L_DG, CDC_7632BA_PD_OUT_L_0 ); //VCDC_WriteReg(CDC_REG763X_PD_OUT_L_DG, 0x01);
                VCDC_WriteReg ( CDC_REG763X_PD_OUT_R_DG, CDC_7632BA_PD_OUT_R_0 ); //VCDC_WriteReg(CDC_REG763X_PD_OUT_R_DG, 0x01);
            }
        }
    } else {
        ;
    }
}

#if VCODEC_HWGAINSYC_SUPPORTED
//mode == 0 close the gain sync, ==1 only open dac gain sync, ==2 only open adc gain sync, ==3 open both dac&adc gain sync;
void VCDC_GainSyncSet ( UINT8	mode )
{
	if ((mode&0x01) != 0) {
		VCDC_WriteReg ( CDC_REG763X_LINEIN_AMP_PD_DG, 0x00 );//open the Linein_Amp_Det;
	} else {
		VCDC_WriteReg ( CDC_REG763X_LINEIN_AMP_PD_DG, 0x01 );//close the  Linein_Amp_Det;
	}
	
	if(mode == 0){//close gain sync, as default;
		// 
		VCDC_WriteReg ( CDC_REG763X_SERD_EN, 0x10 );
		VCDC_WriteReg ( CDC_REG763X_DATA_RSTN, 0x00 );
	} else if(mode == 1){//dac gain sync
		// 
		VCDC_WriteReg ( CDC_REG763X_SERD_EN, 0x15 );
		VCDC_WriteReg ( CDC_REG763X_DATA_RSTN, 0x01 );
	} else if(mode == 2){//adc gain sync
		VCDC_WriteReg ( CDC_REG763X_SERD_EN, 0x0A );
		VCDC_WriteReg ( CDC_REG763X_DATA_RSTN, 0x02 );
		VCDC_WriteReg ( CDC_REG763X_PGA_GAIN_CONF, 0x05 );
		//VCDC_WriteReg ( CDC_REG763X_W_SEL_ADC, 0x00 );
	} else if(mode == 3){//dac&adc gain sync
		VCDC_WriteReg ( CDC_REG763X_SERD_EN, 0x0F );
		VCDC_WriteReg ( CDC_REG763X_DATA_RSTN, 0x03 );
		VCDC_WriteReg ( CDC_REG763X_PGA_GAIN_CONF, 0x05 );
		VCDC_WriteReg ( CDC_REG763X_W_SEL_ADC, 0x00 );
	} else {
		;
	}
}

void VCDC_GainSync_SetADVOL( UINT8 AdcChn, UINT8 LVol, UINT8 RVol )
{
    if ( VCDC_GetVerNo() == CDC_VERSION_PLUS ) {
        //for future 7621 ver.
        ;
    } else if ( VCDC_GetVerNo() == CDC_VERSION_7632B_A0 || VCDC_GetVerNo() == CDC_VERSION_7632B_E3 || VCDC_GetVerNo() == CDC_VERSION_7632B_F5 || VCDC_GetVerNo() == CDC_VERSION_7638_A0) {
        #if 1//gainsyn not need volume shaow in/de.20200603 by zc
        if ( AdcChn & CDC_INPUT_MICL ) {
            VCDC_WriteReg ( CDC_REG763X_ADVOLL_CUR, LVol );
        }
        if ( AdcChn & CDC_INPUT_MICR ) {
            VCDC_WriteReg ( CDC_REG763X_ADVOLR_CUR, RVol );    //the adc R at 7632B is invalid.
        }
        #else
        if ( ( g_VCDC_Config.deal_popupnois & CDC_DEALNOISE_VOLSHAD ) != 0 ) { //#define CDC_DEALNOISE_VOLSHAD  1
            if ( AdcChn & CDC_INPUT_MICL ) {
                VCDC_VolShadow ( CDC_REG763X_ADVOLL_CUR, LVol, g_VCDC_Config.adc_volset_delayus );
            }
            if ( AdcChn & CDC_INPUT_MICR ) {
                VCDC_VolShadow ( CDC_REG763X_ADVOLR_CUR, RVol, g_VCDC_Config.adc_volset_delayus );    //the adc R at 7632B is invalid.
            }
        } else {
            if ( AdcChn & CDC_INPUT_MICL ) {
                VCDC_WriteReg ( CDC_REG763X_ADVOLL_CUR, LVol );
            }
            if ( AdcChn & CDC_INPUT_MICR ) {
                VCDC_WriteReg ( CDC_REG763X_ADVOLR_CUR, RVol );    //the adc R at 7632B is invalid.
            }
        }
        #endif
	} else if(VCDC_GetVerNo() == CDC_VERSION_7652PMU_CSMC || VCDC_GetVerNo() == CDC_VERSION_7652PMU_TSMC){
		;//only PMU not audio;
    } else {
        VCDC_WPRINTF ( "%s: cdc_ver = %d, the eck config err!\r\n", FUNCTION_NAME, VCDC_GetVerNo() );
    }
}
#endif


