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
    2014.02.10           xiongyan                add VA7621BA0
    2014.02.18           xiongyan                add 7621SA0 and 7621GA0
    2019.06.10           xiongyan                add VA7632B_A0
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VDRV_VA7632B_CDC_H__
#define __VDRV_VA7632B_CDC_H__

#define CDC_I2C_REG_BASE        0x10
#define CDC_I2C_REG_NUM         35      /* 0x00 ~ 0x22 */
#define CDC_I2C_RETRY           5
#define CDC_I2C_DELAY           2000//10000

#define CDC_INPUT_NONE          0
#define CDC_INPUT_MICL          1
#define CDC_INPUT_MICR          2
#define CDC_INPUT_DUAL_MIC      3
#define VA7632B_INPUT_NONE          0
#define VA7632B_INPUT_MICL          1
#define VA7632B_INPUT_MICR          2
#define VA7632B_INPUT_DUAL_MIC      3

#define CDC_OUTPUT_NONE         0
#define CDC_OUTPUT_HPL          1
#define CDC_OUTPUT_HPR          2
#define CDC_OUTPUT_HP           3//DUAL
#define VA7632B_OUTPUT_NONE         0
#define VA7632B_OUTPUT_HPL          1
#define VA7632B_OUTPUT_HPR          2
#define VA7632B_OUTPUT_HP           3//DUAL

#define CDC_DEALNOISE_NONE          0
#define CDC_DEALNOISE_VOLSHAD       1//call VCDC_VolShadow, deal_popupnois[0]
#define CDC_DEALNOISE_HPPDSEL       2//select HPPD,deal_popupnois[1]
#define CDC_DEALNOISE_ALL           3//VOLSHAD + HPPDSEL
#define CDC_DEALNOISE_TIMERDELAY    4//for 9611SB & 9611SC, some long delay by timer to let codec work before dac open completion, deal_popupnois[2]
#define CDC_DEALNOISE_ATE           5//for ATE, not deal pop for 9611SC
#define CDC_DEALNOISE_DELAYMS       8//use VCOMMON_Delay all for VCDC_Delayms, deal_popupnois[3]

#define CDC_DEALNOISE_CHECKTIMEOUT  0x10//use VDWT to count for check the I2C timeout, deal_popupnois[4]


#define CDC_STREAMSTART_AVRSP_DELAYTIME         (100)           /* 100 milliseconds, for 9611SD;(9611SC==200) is for AD*/
#define CDC_STREAMSUSPEND_AVRSP_DELAYTIME       (0)             /* set 0 for 9611SD;(9611SB==0x5F0) is for AD*/
//
#define CDC_NEWVERSION_DEFINE   0

#define CDC_OUTPUTMODE_DIFF             0   //fully differential mode, same as F0
#define CDC_OUTPUTMODE_SING             1   //single end mode, same as SD0(default)

#if CDC_NEWVERSION_DEFINE
    #define CDC_VERSION_9611        0x00//new 9611,9611D
    #define CDC_VERSION_9611B       0x01//old 9611,has a bug in register "PGA"
    #define CDC_VERSION_9611E       0x02//9611E, hw optimize the pop noise, 20111219
    #define CDC_VERSION_9611F       0x03//9611F, hw optimize the pop noise, 20121017
    #define CDC_VERSION_9611S       0x10//9611SA
    #define CDC_VERSION_9611S_B     0x11//plan1, for optimize the pop noise, 20111219
    #define CDC_VERSION_9611S_C     0x12//plan2, for optimize the pop noise, 20111219
    #define CDC_VERSION_9611S_D     0x13// for optimize the pop noise, 20120709
    #define CDC_VERSION_9611S_E     0x14//include both 9611SD and 9611F
    #define CDC_VERSION_7621_A      0x20//
    #define CDC_VERSION_7621S_A     0x21//
    #define CDC_VERSION_7621G_A     0x22//
    #define CDC_VERSION_7621SA_ECO  0x23
    #define CDC_VERSION_7621EA0     0x24
    #define CDC_VERSION_7621SD_C0   0x25

    #define CDC_VERSION_7632B_A0    0x30//201906
    #define CDC_VERSION_7632B_E3        0x31//for 7632BE3/F3;

        #define CDC_VERSION_7621_I2S    0x40//7621SDC0 ADC_L(lonly L not R) + DAC_I2S

    #define CDC_VERSION_PLUS        0xA0//for future new 76XX
#else
    //#define CDC_VERSION_9611      0//new 9611,9611D/E
    //#define CDC_VERSION_9611E         0//9611E ?
    //#define CDC_VERSION_9611F         0//9611F
    //#define CDC_VERSION_9611S         1//9611SA
    //#define CDC_VERSION_9611B         2//old 9611,has a bug in register "PGA"
    //#define CDC_VERSION_9611S_B   3//plan1, for optimize the pop noise, 20111219
    //#define CDC_VERSION_9611S_C   4//plan2, for optimize the pop noise, 20111219
    //#define CDC_VERSION_9611S_D   5
    //#define CDC_VERSION_9611S_E   6//include both 9611SD and 9611F
    #define CDC_VERSION_7621_A      7//7621BA
    #define CDC_VERSION_7621S_A     8//sing+diff; also CDC_VERSION_7621SC.
    #define CDC_VERSION_7621G_A     9//sing //most same as CDC_VERSION_7621_A,
    #define CDC_VERSION_7621SA_ECO  10//change from 7621SA about CDC_PD_OUT_R&CDC_PD_OUT_L, CDC_PD_CHARGE, CDC_PWD_STBF; change the Default control from 1"active high" to 0"active low".

    #define CDC_VERSION_7621EA0     11
    #define CDC_VERSION_7621SD_C0   12

    #define CDC_VERSION_7621_I2S    15//7621SDC0 ADC_L(lonly L not R) + DAC_I2S

        //20201130xy, add more 7632B[X] driver define by VA7632B_A0_AudioCODEC_DesignSpec_v0.9; the FPGA (VA9625_NewFPGA_QA == 1) used 7632BF3;
    #define CDC_VERSION_7632B_A0    16//for 7632BA0/B0/C0/D0/E0/F0/C1/D1/E1/F1/E2/F2/E5/F5; the F5 is current used;
        #define CDC_VERSION_7632B_E3    17//for 7632BE3/F3; the F3 is current used;
        #define CDC_VERSION_7632B_F5    18
    #define CDC_VERSION_PLUS        30//for future new 76XX.
#endif

#ifdef FPGA_QA_BUILD    
#define CDC_VERSION_DEFAULT     CDC_VERSION_7632B_E3//CDC_VERSION_7632B_A0//
#else
#define CDC_VERSION_DEFAULT     CDC_VERSION_7632B_E3//CDC_VERSION_7632B_A0//
#endif

/*
 * R20: CDC_HPVOL
 * [7:4] CDC_HPVOL_R[3:0], right channel, Headphone volume control, VOL_R<3> = MSB, (0000b = -18dB) (1111b = +12dB);2dB/step
 * [3:0] CDC_HPVOL_L[3:0], left channel,  Headphone volume control, VOL_L<3> = MSB, (0000b = -18dB) (1111b = +12dB);2dB/step
 * Default: 0x00
 */
#define CDC_REGS_HPVOL          0x20
//20181130 Chen jh add for VA7621I2S
/*
 * R40: SPORT_MODE
 * [7] pdm_clkdiv2_en, pdm clock div2 enable control
                0: bypass
                1: clock divided by 2
 * [6] i2s_clkdiv2_en, i2s bit clock div2 enable control
                0: clock bypass
                1: clock divided by 2
 * [5] i2s_mode, i2s mode selection
                0: master mode
                1: slave mode   ??7621iis only support Master Mode
 * [4] rxdata_test,  rx fifo test enable
                0: normal I2S parallel data input
                1: {24'h555555,24'haaaaaa} data input
 * [3]sport_delay,  Serial port delay control.
                0: no delay.     1: add half cycle? s delay
 * [2] sport_mode, Serial port mode selection.
                0: i2c mode.    1: high speed serial port.
  * [1] rxfifo_reset, Rx FIFO reset, high active
  * [0] txfifo_reset, TX FIFO reset, high active

 * Default: 0x00
 */
#define CDC_REGS_SPORT_MODE                  0x40

/*
 * R41: I2S_CTRL
 * [7] i2s_inf_mode, i2s interface mode selection
            0: pdm interface selected.
            1: i2s interface selected or analog ADC selected.
 * [6] rx_en, i2s rx enable control.     0: disable.    1: enable
 * [5] tx_en, i2s tx enable control.     0: disable      1: enable
 * [4:3] bitwidth[1:0],  select bit width of the sample.
            2'b00: 8bit  2'b01: 16bit    2'b1x: 24bit
 * [2] lsb_first,  select the serial data transfer mode
             0: msb output or input first.      1: lsb output or input first.
 * [1:0] amode[1:0], audio interface selection
            2'b00: I2S timing selected.
            2'b01: left justify timing selected.
            2'b10: right justify timing selected.
            2'b11: reserved

 * Default: 0x98
 */
#define CDC_REGS_I2S_CTRL                               0x41// to enable rx_en,[4:3] bitwidth[1:0] must be 10
/*
 * R4F: PLL_CTRL_N_LOW
 * [7:0] n[7:0],PLL feedback divider control pins. Divider ratio configuration.
 * Default: 0x00
 */
#define CDC_REGS_PLL_CTRL_N_LOW             0x4F

/*
 * R50: PLL_CTRL_N_HIGH
 * [7:0] n[9:8],PLL feedback divider control pins. Divider ratio configuration.
 * Default: 0x00
 */
#define CDC_REGS_PLL_CTRL_N_HIGH                0x50

/*
 * R4E: PLL_CTRL_R
 * [7:6] RESERVED
 * [5:0] r[5:0],PLL input divider control pins.  Divider ratio configuration.
 * Default: 0x05
 */
#define CDC_REGS_PLL_CTRL_R             0x4E
/*
 * R5A: PLLOUT_DIV
 * [7:0] pllout_div[7:0], the divider ratio after PLL, for I2S_BCLK
  * Default: 100
 */
#define CDC_REGS_PLLOUT_DIV             0x5A

/*
 * R5C: FOUT_EN
 * [7:5] RESERVED
 * [4:3] pll_ref_src_sel[1:0],PLL referecne clock selection:
 *                                              2'b00 ---- dacki,       IIS used the dac(aif) clock, if open dac(should enable aif dac and give the data out) clock then output 6M clock to IIS ok;
 *                                              2'b01 ---- bclk_incore,  IIS used the adc(aif) clock, if open adc(enable aif adc) clock and the sample rate should >=16K(the adc 8K then out clock 1M to IIS may let it err, base clock should more then 2M==16K)
 *                                              2'b10 ---- hd_scl;      the 9625p not support, 9638&9635 had it;
 * [2] pd_mode,
        0: plldiv_oen controlled by REG1E[4], decim_en controlled by REG1A[4], fout_en controlled by REG1E[0], pll bp controlled by REG19[7].tx_en controlled by REG1A[0]
        1: plldiv_oen controlled by REG5B[0], decim_en controlled by REG5C[1], fout_en controlled by REG5C[0], pll bp controlled by REG51[6],tx_en controlled by Reg41[5]
 * [1] decim_en,
        1'b0: disable decimation filter
        1'b1: enbale decimation filter
 * [0] fout_en,
        1'b0:  PLL output signal FOUT is closed.
        1'b1:  PLL output signal FOUT is opened.
 * Default: 0x00
 */
#define CDC_REGS_FOUT_EN               0x5C
/*
 * R51: PLL_CTRL_OTHERS
 * [7] RESERVED
 * [6] bp, bp=0: PLL is normal opeation.    bp=1: the FOUT is XIN and the PLL is bypass.
 * [5] oe, oe=0: PLL is normal operation.   oe=1: the FOUT is disable.
 * [4] pd, PLL Power Down, active high.
 * [3:2] tst[1:0], M and N divider test mode.
            2'b00:  N divider test and normal operation.
            2'b11:  M divider test.
 * [1:0] od[1:0], 2'b00 is normal operation, 2'b01 is N / 2.
 * Default: 0x10
 */
#define CDC_REGS_PLL_CTRL_OTHERS                0x51
/*
 * R5B: PLLDIV_OEN
 * [7:2] RESERVED
 * [1] plldiv_oen_mclk, the divider output enable, active high, for I2S_MCLK
 * [0] plldiv_oen, the divider output enable, active high.
 * Default: 0x00
 */
#define CDC_REGS_PLLDIV_OEN             0x5B
/*
 * R15: CDC_ADVOL
 * [7:4] CDC_ADVOLR, ADC right channel PGA gain, -3dB to 42dB, 3dB/step;
 * [3:0] CDC_ADVOLL, ADC left channel PGA gain,  -3dB to 42dB, 3dB/step;
 * -3dB to 42dB, 3dB per step, default 0dB
 * Default: 0x11
 */
#define CDC_REGS_ADVOL          0x15

/* default values of CDC configuration */
#if defined (HW_CDC_DEBUG)
    //both 9611 and 9611s
    #define CDC_DEFAULT_ADCURL          0x05//for 7621      0x03//CDC_ADCUR[2:0]
    #define CDC_DEFAULT_ADCURR          0x50//for 7621      0x30//CDC_ADCUR[6:4]
    #define CDC_DEFAULT_CTBUFFICTL      0x00//CDC_CTBUF[1:0]
    #define CDC_DEFAULT_SCBUFFICTL      0x00//CDC_SCBUF[1:0]
    #define CDC_DEFAULT_REFVRFSEL       0x00//CDC_REF[3:2]
    #define CDC_DEFAULT_REFIOPT         0x02//CDC_REF[1:0]
    //9611s
    #define CDC_DEFAULT_DACDEBUG_L      0x00//CDC_DACCTL0[3:0]  //#define CDC_DEFAULT_DACDEBUG      0x00
    #define CDC_DEFAULT_DACMODE_L       0x00//CDC_DACCTL1[3]    //#define CDC_DEFAULT_DACMODE       0x00
    #define CDC_DEFAULT_DACCTL_L        0x00//CDC_DACCTL1[1:0]  //#define CDC_DEFAULT_DACCTL        0x00
    #define CDC_DEFAULT_DACLPFICTL_L    0x00//CDC_DACCTL2[1:0]  //#define CDC_DEFAULT_DACLPFICTL    0x00
    #define CDC_DEFAULT_HPDOCN_L        0x0C//CDC_HPDOCN[3:0]   //#define CDC_DEFAULT_HPDOCN        0x0C
    #define CDC_DEFAULT_HPLP_L          0x03//CDC_HP0[1:0]      //#define CDC_DEFAULT_HPLP          0x03
    #define CDC_DEFAULT_MIXICTL_L       0x00//CDC_MIX[1:0]      //#define CDC_DEFAULT_MIXICTL       0x00
    #define CDC_DEFAULT_HPDOCP_R        0xC0//CDC_HPDOCP[7:4]   //#define CDC_DEFAULT_HPDOCP        0xC0

    #define CDC_DEFAULT_DACDEBUG_R      0x00//CDC_DACCTL0[7:4]  //#define CDC_DEFAULT_DACDISDWA     0x00
    #define CDC_DEFAULT_DACMODE_R       0x00//CDC_DACCTL1[7]    //#define CDC_DEFAULT_HPPOP         0x00
    #define CDC_DEFAULT_DACCTL_R        0x00//CDC_DACCTL1[5:4]  //#define CDC_DEFAULT_HPOC          0x00
    #define CDC_DEFAULT_DACLPFICTL_R    0x00//CDC_DACCTL2[5:4]  ////#define CDC_DEFAULT_MBSEL       0x02
    #define CDC_DEFAULT_HPDOCN_R        0xC0//CDC_HPDOCN[7:4]
    #define CDC_DEFAULT_HPLP_R          0x30//CDC_HP0[5:4]
    #define CDC_DEFAULT_MIXICTL_R       0x00//CDC_MIX[5:4]
    #define CDC_DEFAULT_HPDOCP_L        0x0C//CDC_HPDOCP[3:0]

    #define CDC_DEFAULT_VCMSEL          0x10//CDC_REF[4]
    //9611
    #define CDC_DEFAULT_MBSEL           0x02

    //9611S_B
    //#define CDC_DEFAULT_POP_DEBUG1            0x70
    //#define CDC_DEFAULT_POP_DEBUG2            0x03
    //#define CDC_DEFAULT_SB_INITEN             0x80
    //9611S_C
    //#define CDC_DEFAULT_SOFTSTART             0x70
    //#define CDC_DEFAULT_SC_INITEN             0x00

    //9611S_D
    #define CDC_DEFAULT_SD_HPPOP            0x00
    #define CDC_DEFAULT_SD_MBSEL            0x02
    #define CDC_DEFAULT_SD_POP_ICTL1        0x0F//0x00
    #define CDC_DEFAULT_SD_POP_ICTL2        0x00//0x80
    #define CDC_DEFAULT_SD_PD_OUTXI         0x70
    #define CDC_DEFAULT_SD_POP_SW_EN        0x00
    #define CDC_DEFAULT_SD_PD_COMP_L        0x01
    #define CDC_DEFAULT_SD_PD_COMP_R        0x02
    #define CDC_DEFAULT_SD_DFF_SET_L        0x01
    #define CDC_DEFAULT_SD_DFF_SET_R        0x02
    #define CDC_DEFAULT_SD_BYPASS_L         0x00
    #define CDC_DEFAULT_SD_BYPASS_R         0x00
    #define CDC_DEFAULT_SD_INIT_EN_L        0x00
    #define CDC_DEFAULT_SD_INIT_EN_R        0x00
    #define CDC_DEFAULT_SD_PD_CHG_L         0x01
    #define CDC_DEFAULT_SD_PD_CHG_R         0x02
    #define CDC_DEFAULT_SD_PD_STBF          0x03
    #define CDC_DEFAULT_SD_PREC_EN          0x00
    #define CDC_DEFAULT_SD_PD_LEVEL         0x03
    #define CDC_DEFAULT_SD_PD_BF_HPOUT      0x03

    //9611S_E
    #define CDC_DEFAULT_SE_D2S              0x03

    //7621_A
    #define CDC_DEFAULT_PD_FD               0x00   // 0==diff, 0x07>0 sing
    #define CDC_DEFAULT_HPPOP               0x00   //CDC_9611SD_HPPOP
    #define CDC_DEFAULT_HP_PULLDOWN_EN      0x00
    #define CDC_DEFAULT_PWD_HFOUT           0x77    //0x66--sing,  0x77--diff
    #define CDC_DEFAULT_PD_CHARGE           0xFF //0x0E
    #define CDC_DEFAULT_PD_OUT              0x00    //0x70--sing,  0x00--diff, only for 7621BA
    #define CDC_DEFAULT_REFPD_HIGH          0x01    //CDC_REFPD[5](vcom=4/9vdd)=1 to close
    #define CDC_DEFAULT_REFPD_LOW           0x10    //CDC_REFPD[3:0](vcom=1/2vdd)=0 to open; CDC_REFPD[4](vcom=5/9vdd)=1 to close
    //VA761S, VA7621G
    #define CDC_DEFAULT_7621A_PD_OUT_R      0x00    //0x70--sing,  0x00--diff, for 7621SA&7621GA
    #define CDC_DEFAULT_7621A_PD_OUT_L      0x00    //0x70--sing,  0x00--diff, for 7621SA&7621GA
    #define CDC_DEFAULT_7621A_CDC_NMILLER   0x00
    #define CDC_DEFAULT_7621A_RESERVE       0x00
    #define CDC_DEFAULT_7621A_PGA_CUR       0x33
    #define CDC_DEFAULT_7621A_HP_ICTL       0x00
    #define CDC_DEFAULT_7621A_ICTL_STBF     0x00
    #define CDC_DEFAULT_7621A_HPPD_OUT      0x33

    //VA7632BA0
    #define CDC_DEFAULT_7632BA_PD_OUT_L_DG      0x00
    #define CDC_DEFAULT_7632BA_PD_OUT_R_DG      0x00


//audio power mode, by ldo5 or DCDC.
#define CDC_PWR_LDO5        0
#define CDC_PWR_DCDC        1

#define CDC_DEFAULT_DCDC_DAC            0x01
#define CDC_DEFAULT_DCDC_ADC            0x01
#define CDC_DEFAULT_RESERVED            0
#endif

// ggj20180118 for VA7633 , but 7621 has different reg addr,need to be confirmed begin
#define CDC_W_SEL_ADC                   0xB3//these regs addr are for 7633,7621 need to be confirmed
#define CDC_PGA_GAIN_CONF               0xB6
#define CDC_SPORT_MODE                  0xB7

#define CDC_I2S_CTRL                    0xB8//these regs addr are for 7633,7621 need to be confirmed
#define CDC_I2S_WLEN                    0xB9

#define CDC_KEYCODE_REG                 0xBA

#define CDC_WD_CTRL                     0xBB
#define CDC_WD_TG_WIDTH                 0xBC
#define CDC_WD_CLK_DIV0                 0xBD
#define CDC_WD_CLK_DIV1                 0xBE
#define CDC_WD_CNT_MAX0                 0xBF
#define CDC_WD_CNT_MAX1                 0xC0
#define CDC_WD_CNT_MAX2                 0xC1////these regs addr are for 7633,7621 need to be confirmed

/*
 * RD2 ~ RD5: CHIP_ID 
 */
#define CDC_REGS_CHIP_ID0               0xD2
#define CDC_REGS_CHIP_ID1               0xD3
#define CDC_REGS_CHIP_ID2               0xD4
#define CDC_REGS_CHIP_ID3               0xD5

//audio power mode, by ldo5 or DCDC. 
#define CDC_PWR_LDO5            0       
#define CDC_PWR_DCDC            1

#define CDC_DEFAULT_DCDC_DAC            0x01
#define CDC_DEFAULT_DCDC_ADC            0x01
#define CDC_DEFAULT_RESERVED            0

//ggj20180118 for VA7633,but 7621 has different reg addr,need to be confirmed  end
typedef struct _tagVCDC_CONFIG {
    UINT8 cdc_ver;//Select the type of current used Chip, 9611 or 9611S
    UINT8 cdc_poweron_sel;//Select Configuration sequence for power up(0, 1, 2)
    UINT8 cdc_powerdown_sel;//Select Configuration sequence for power down(0, 1, 2)
    UINT8 dac_chn;//Select adc channel for 9611s(1-CDC_OUTPUT_HPL,2-CDC_OUTPUT_HPR,3-CDC_OUTPUT_HP);invaild for 9611
    //Attention, all delays are 16bit, the value restriction
    UINT16 cdc_poweron_delayus;//the delay to reduce Popup Noise at power up sequence
    //UINT16 cdc_powerdown_delayus;//the delay to reduce Popup Noise at power down sequence
    UINT16 dac_volset_delayus;//the delay(us) between each level, in adjustment of DAC volume, need to open "CDC_DEALNOISE_VOLSHAD"
    UINT16 adc_volset_delayus;//the delay(us) between each level, in adjustment of ADC volume, need to open "CDC_DEALNOISE_VOLSHAD"
    UINT8 deal_popupnois;//Optimization methods for Popup Noise, at switching to DAC or ADC.

    //20111227 for 9611S_B(plan 1) and 9611S_C(plan 2); 20120712 for 9611SD
    UINT8 cdc_skip1;//Skip Option;1:skip,0:no skip(for 9611SB powerdown and 9611SC powerup, 9611SD powerup)
    UINT16 cdc_delayT1;//delay time 1;//Define delay time 1 == 1mS (default value)//us
    //for 9611S_B(plan 1)
    SINT16 cdc_powerup1_delay1;//delay -1mS ~ 1mS;//us(0S)
    UINT16 cdc_powerup1_delay2;//delay 500mS ~ 2S;//ms(1S) //alos for 9611S_D , after CDC_POP_PD_STBF(100ms)
    SINT16 cdc_powerup1_delay3;//delay -1mS ~ 1mS;//us(0S)
    UINT16 cdc_powerup1_delay4;//delay 0mS ~ 1mS;//us(0S)
    UINT16 cdc_powerdown1_delay1;// delay 0S~100uS;//us(0S) //also for 9611S_C , cdc_powerup2_delay1 = cdc_powerdown1_delay1
    UINT16 cdc_powerdown1_delay2;//delay 500mS~2S;/ms(1S)   //also for 9611S_C , CDC_CTST_PUPA time out(2S)=2*cdc_powerdown1_delay2; 9611S_D 300ms
    UINT16 cdc_powerdown1_delay3;//delay 0S~100uS;//us(0S)  //also for 9611F powerup(0us)
    SINT16 cdc_powerdown1_delay4;//delay -10uS~10uS;//us(0S)
    UINT16 cdc_powerdown1_delay5;////delay 0S~1mS;//us(100uS)
    //for 9611S_C(plan 2)
    UINT16 cdc_powerup2_Tdelay;//Delay 1mS~10mS (T_delay);//us(1mS); //alos for 9611S_D //also for 9611F powerdown(1ms)
    //UINT16 cdc_powerup2_delay1;//Delay 0S ~ 1mS; //us(0S)     ==cdc_powerdown1_delay1
    //for AV_StreamStart_Ind, in"Av_rsp.c".
    UINT16 cdc_StreamStartRsp_delayms;  //the StreamStartRsp delay time(ms) after send music start to APP, this delay depend on the cdc_ver(different 9611 has each init time)
    UINT16 cdc_StreamSuspendRsp_delayms;//the StreamSuspendRsp delay time(ms) after send Suspend start to APP, this delay for 9611SB(it power down need more time, deflaut value=0x5F0)
} VCDC_CONFIG;

typedef struct va7632b_debug
{
    uint8_t dg_pga_cur;
    uint8_t dg_adcur;
    uint8_t dg_ref;
    uint8_t dg_vref_sel;
    uint8_t dg_vcom_sel;
    uint8_t dg_ctsc;
    uint8_t dg_mbsel;
    uint8_t dg_refpd_high;
    uint8_t dg_dacctl1;
    uint8_t dg_dacctl2;
    uint8_t dg_mix;
    uint8_t dg_ictl_stbf;
    uint8_t dg_pwd_hfout;
    uint8_t dg_hp_ictl;
    uint8_t dg_hppd_out;
    uint8_t dg_nmiller;
    uint8_t dg_cboost;
    uint8_t dg_vgate_sel;
    uint8_t dg_vsub_high;
    uint8_t dg_hpa_native; 
    uint8_t dg_linein_amp_vref;
    uint8_t dg_hpagain_mode; 
}va7632b_debug_t;

typedef struct _tagVCDC_DEBUG {
    //AIF test(CDC_CTRL)
    UINT32 aif_cdc_ctrl;//[2:0]hpf_sel,[3]clkadc_sel,[7:4]sdm_sel,[15:8]ditherpow,[19:16]decim_gain_sel
    // both 9611 and 9611s have
    UINT8 adc_cur_l;
    UINT8 adc_cur_r;
    UINT8 ctbuff_ictl;
    UINT8 scbuff_ictl;
    UINT8 ref_vrf_sel;
    UINT8 ref_iopt;
    //9611s have
    UINT8 dac_debug_l;      //UINT8 dac_debug;  //UINT8 pdout_dgl;(7632)
    UINT8 dac_mode_l;       //UINT8 dac_mode;
    UINT8 dac_ctl_l;        //UINT8 dac_ctl;
    UINT8 dac_lpfictl_l;    //UINT8 dac_lpf_ictl;
    UINT8 hp_docn_l;        //UINT8 hp_doc_n;
    UINT8 hp_lp_l;          //UINT8 hp_lp;
    UINT8 mix_ictl_l;       //UINT8 mix_ictl;
    UINT8 hp_docp_r;        //UINT8 hp_doc_p;

    UINT8 dac_debug_r;      //UINT8 dac_dis_dwa;    //UINT8 pdout_dgr;(7632)
    UINT8 dac_mode_r;       //UINT8 hp_pop;
    UINT8 dac_ctl_r;        //UINT8 hp_oc;
    UINT8 dac_lpfictl_r;    ////UINT8 mb_sel;
    UINT8 hp_docn_r;
    UINT8 hp_lp_r;
    UINT8 mix_ictl_r;
    UINT8 hp_docp_l;

    UINT8 vcm_sel;
    //9611S_D                                                   //7621A
    UINT8 pop_debug1;//(pd_outxI SD)    //UINT8 pop_debug1;     //UINT8 pwd_hfout;
    UINT8 pop_debug2;//(pop_ictl1 SD)   //UINT8 pop_debug2;     //UINT8 pd_out; //UINT8 nmiller(7621S);
    UINT8 pop_initen;//(pop_ictl2 SD)   //UINT8 pop_initen;         //UINT8 pd_charge;
    UINT8 hp_pop;                                               //UINT8 hp_pop;
    UINT8 mb_sel;                                               //UINT8 mb_sel
    UINT8 pop_sw_en;                                            //UINT8 hp_pulldown_en;
    UINT8 pd_comp_l;                                            //UINT8 refpd_low;
    UINT8 pd_comp_r;                                            //UINT8 refpd_high;
    UINT8 dff_set_l;                                            //UINT8 pd_out_l;   //add for 7621S/G
    UINT8 dff_set_r;                                            //UINT8 pd_out_r;
    UINT8 bypass_l;                                             //UINT8 reserve_s;(7621S, CDC_RESERVE)
    UINT8 bypass_r;                                             //UINT8 pga_cur;
    UINT8 init_en_l;                                            //UINT8 hp_ictl;
    UINT8 init_en_r;                                            //UINT8 ictl_stbf;
    UINT8 pd_chg_l;                                             //UINT8 hppd_out;
    UINT8 pd_chg_r;                                             //UINT8 pd_fd;(for mode sel)
    UINT8 pd_stbf;
    UINT8 prec_en;
    UINT8 pd_level;
    UINT8 pd_bf_hpout;
    //7621SD            remove//9611S_E
    UINT8 hpagain_mode;     //UINT8 d2s;
    //for LDO5 debug pin when close[0] & MEN5 set[7:2](VA7621)
    UINT8 pmu_ldo5;
    UINT8 pwr_mode; //for 7621, pmu by LDO5(0) or DCDC(1)
    UINT8 pmu_set_dac; //the DCDC set if adc not open; or the LDO5 mode set DCDC when codec open
    UINT8 pmu_set_adc; //the DCDC set when adc open

    //for 7632B
    UINT8 dg_pga_cur;
    UINT8 dg_adcur;
    UINT8 dg_ref;
    UINT8 dg_vref_sel;
    UINT8 dg_vcom_sel;
    UINT8 dg_ctsc;
    UINT8 dg_mbsel;
    UINT8 dg_refpd_high;
    UINT8 dg_dacctl1;
    UINT8 dg_dacctl2;
    UINT8 dg_mix;
    UINT8 dg_ictl_stbf;
    UINT8 dg_pwd_hfout;
    UINT8 dg_hp_ictl;
    UINT8 dg_hppd_out;
    UINT8 dg_nmiller;
    UINT8 dg_cboost;
    UINT8 dg_vgate_sel;
    UINT8 dg_vsub_high;
    UINT8 dg_hpa_native;
    UINT8 dg_linein_amp_vref;
} VCDC_DEBUG;



typedef struct _tagVCDC_REG_CONFIG {
    UINT8 cdc_reg_addr; //for cdc register address if == 0 invalid.
    UINT8 cdc_on_value; //open the reg set value
    UINT8 cdc_off_value;//close the reg set value
    UINT8 cdc_delay;//ms
} VCDC_REG_CONFIG;

#define CDC_PLUS_INIT_REGNUM        8
#define CDC_PLUS_OUTPUT_REGNUM      20
#define CDC_PLUS_INPUT_REGNUM       10
typedef struct _tagVCDC_PLUS_CONFIG {
    VCDC_REG_CONFIG CDC_Init[CDC_PLUS_INIT_REGNUM];
    VCDC_REG_CONFIG CDC_OutPut[CDC_PLUS_OUTPUT_REGNUM];
    VCDC_REG_CONFIG CDC_InPut[CDC_PLUS_INPUT_REGNUM];
} VCDC_PLUS_CONFIG;

//for 7621I2S
typedef struct _tagVCDC_I2S_SLAVE_I2C_REG_CONFIG {
    UINT8   i2c_addr;   // slave device's i2c address, it connect to 7621i2s
    UINT8   reg_addr;   // slave device's reg addres, it connect to 7621i2s
    UINT8   reg_data;   // slave device's reg data, it connect to 7621i2s
    UINT8   delay_us;   // setting i2c regs delay of slave device's connect to 7621i2s
} VCDC_I2S_SLAVE_I2C_REG_CONFIG;

#define CDC_I2S_SIMI2C_REGNUM       10
typedef struct _tagVCDC_I2S_CONFIG {
    UINT8   i2s_clk_sel; //0==adc clock, 1==dac clock//FOUT_EN[4:3] pll_ref_src_sel[1:0], referecne clock selection: 0==dac clock, 1==adc clock, 2==hd_scl(9625p not support hd_scl); 
    UINT8   i2s_ctrl;   // for set i2s bclk data size etc
    //UINT8   plldiv_oen; //for set MCLK
    //UINT8   open_set_reg_num;
    //UINT8   close_set_reg_num;
    //VCDC_I2S_SLAVE_I2C_REG_CONFIG   open_set_regs[CDC_I2S_SIMI2C_REGNUM];
    //VCDC_I2S_SLAVE_I2C_REG_CONFIG   close_set_regs[CDC_I2S_SIMI2C_REGNUM];
    UINT8   decim_ctrl; //for set DECIM_CTRL of 7621I2S about decim_gain_sel and so on at VCDC_ConfigOutput(); 
} VCDC_I2S_CONFIG;


void va7632b_output_gain_6bit_l(uint8_t gainvol);
void va7632b_output_gain_6bit_r(uint8_t gainvol);
void va7632b_output_gain_4bit_l(uint8_t gainvol);
void va7632b_output_gain_4bit_r(uint8_t gainvol);
int va7632b_get_output_gain_4bit(uint8_t *gainvol);
void va7632b_output_gain_4bit(uint8_t gainvol);
void VCDC_Init ( void );
void VCDC_ConfigInput ( UINT8 Input );
void VCDC_ConfigOutput(UINT8 output,UINT8 mode);
void VCDC_SetInputGain ( UINT8 GainVal );
void VCDC_SetOutputGain ( UINT8 GainVal );
void VCDC_MuteOutput ( void );
void VCDC_UnmuteOutput ( void );
void VCDC_MuteInput ( void );
void VCDC_UnmuteInput ( void );
VRET_VALUE VCDC_WriteReg ( UINT8 Reg, UINT8 Value );
VRET_VALUE VCDC_ReadReg ( UINT8 Reg, UINT8 *p_Value );
void VCDC_VolShadow ( UINT8 DestReg, UINT8 DestVol, UINT16 DelayTime );
UINT8 VCDC_GetDACChan ( void );
//void VCDC_9611S_PopSuppression(UINT8 Output);
void VCDC_CloseMBias ( void );

void VCDC_7621_PA_Opne ( void );
void VCDC_7621_PA_Close ( void );

UINT8 VCDC_GetHPVOL_7621SDC0 ( SINT16 Gain );
UINT8 VCDC_GetHPVOL ( SINT16 Gain );

void VCDC_SetADCVol ( UINT8 AdcChn, UINT8 LVol, UINT8 RVol );

UINT8 VCDC_GetVerNo ( void );
UINT8 VCDC_Get7621GainMode ( void );

void VCDC_PDOUTConfigByGain ( SINT16 Gain );

#if VCODEC_HWGAINSYC_SUPPORTED
void VCDC_GainSyncSet ( UINT8   mode );
void VCDC_GainSync_SetADVOL( UINT8 AdcChn, UINT8 LVol, UINT8 RVol );
#endif

UINT8 VCDC_I2S_GetMode ( void );
//void VCDC_I2S_ConfigExCodec ( UINT8 ctrl );
UINT8 VCDC_ExCodecConfig ( UINT8 Mode, UINT8 Ctrl );
UINT8 VCDC_ExCodecCheck ( UINT8 Mode );


#endif /* __VDRV_VA7632B_CDC_H__ */


