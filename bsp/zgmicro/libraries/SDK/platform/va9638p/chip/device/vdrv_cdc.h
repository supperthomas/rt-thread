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
#ifndef _VCDC_H_
#define _VCDC_H_

#define CDC_I2C_SLAVE_ADDR      0x34
#define CDC_I2C_REG_BASE        0x10
#define CDC_I2C_REG_NUM         35      /* 0x00 ~ 0x22 */
#define CDC_I2C_RETRY           5
#define CDC_I2C_DELAY           200*100//10000  fix by zdw

#define CDC_INPUT_NONE          0
#define CDC_INPUT_MICL          1
#define CDC_INPUT_MICR          2
#define CDC_INPUT_DUAL_MIC      3

#define CDC_OUTPUT_NONE         0
#define CDC_OUTPUT_HPL          1
#define CDC_OUTPUT_HPR          2
#define CDC_OUTPUT_HP           3//DUAL

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
    #define CDC_VERSION_7632B_E3	0x31//for 7632BE3/F3;
	#define CDC_VERSION_7632B_F5	0x32//for 7632BE3/F3;

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
    #define CDC_VERSION_7621SD_C0   12	//also VA7621S2A2 with 9638b chip;

    #define CDC_VERSION_7621_I2S    15//7621SDC0 ADC_L(lonly L not R) + DAC_I2S

	//20201130xy, add more 7632B[X] driver define by VA7632B_A0_AudioCODEC_DesignSpec_v0.9; the FPGA (VA9625_NewFPGA_QA == 1) used 7632BF3;
    #define CDC_VERSION_7632B_A0    16//for 7632BA0/B0/C0/D0/E0/F0/C1/D1/E1/F1/E2/F2;
	#define CDC_VERSION_7632B_E3	17//for 7632BE3/F3; audio and pmu driver had new change from 7632BA0, as temporary transition version, will be replaced by F5 ; 
	#define CDC_VERSION_7632B_F5	18//for 7632BE5/F5; audio driver same as 7632BA0; but pmu had some different from BA0, and some  connection for tmp F3, as mass-produced version; VA7638_A0(for 9648C)==0x01 have audio and two adc ;
	//#define CDC_VERSION_7632B_F3F5	19//just tmp for 7632BF3 or 7632BF5 check and automatic adaptation, then the customers will not distinguish the 9648 chip use F3 or F5 both use same bt bin;

	#define CDC_VERSION_7652PMU_CSMC	20//the CSMC 7652PMU, VA7652PMU_A0, chip_id3[7:0] 0x01(had done, remove audio, and some driver as 7632BF5), 0x02~0x04 for future; 7652PMU at CSMS version as 7632BXX;
	#define CDC_VERSION_7652PMU_TSMC	21//the TSMC 7652PMU, VA7652PMU_T2_A0, chip_id3[7:0] 0xa0~0xcf(will done for 9652, most some as 7652PMU_CSMC, different at 02H MEN5[1:0]); 7652PMU at TSMC version as New;
	#define CDC_VERSION_7638_A0			22//for 7638A0, chip_id3[7:0] version for metal change: VA7638_A0: 8'b0000_0001; have audio and two adc, just PMU DCDC had different to VA7632B_F5;

	#define CDC_VERSION_CHECK			28//system will check the chip ID by VCDC_GetVerNo().
    #define CDC_VERSION_NONE        	29//if select then CDC_VERSION_CHECK and read the chip ID not define then do nothing(use the default value) .
    
    #define CDC_VERSION_PLUS        	30//for future new 76XX.
#endif

#define CDC_VERSION_DEFAULT     	CDC_VERSION_CHECK //CDC_VERSION_7632B_E3//CDC_VERSION_7632B_A0//

#define HDS_SM_PDM_CLKDIV           (BIT7)
#define HDS_SM_I2S_CLKDIV           (BIT6)
#define HDS_SM_I2S_MODE             (BIT5)
#define HDS_SM_RXTEST               (BIT4)
#define HDS_SM_DELAY                (BIT3)
#define HDS_SM_MODE                 (BIT2)
#define HDS_SM_RXFIFO_RESET         (BIT1)
#define HDS_SM_TXFIFO_RESET         (BIT0)

//these registers define for 9611s
/*
 * R10: CDC_ADCLKEN
 * [7:2], reserved
 * [1] CDC_ADCLKENR, right channel ADC clock enable, 0 - disable, 1 - enable
 * [0] CDC_ADCLKENL, left channel ADC clock enable,  0 - disable, 1 - enable
 * Default: 0x00
 */
#define CDC_REGS_ADCLKEN        0x10

/*
 * R11: CDC_ADCUR
 * [7], reserved
 * [6:4] CDC_ADCURR, set reference current of ADC right channel
 * [3], reserved
 * [2:0] CDC_ADCURL, set reference current of ADC left channel
 * Default: 0x33
 */
/*
 * R11: CDC_ADCUR(2011-12-26,modify 9611S_B&9611S_C) + 9611S_D
 * [7:3] reserved
 * [2:0] CDC_ADCUR[2:0], Set reference current, 000--2.5u; 001--5u;011--10u;111--20u for low to high bit. 2 channels.(REF CUR = 10u)
 * Default: 0x03
 */
/*
 * R11: CDC_ADCUR(20140210, modify for 7621A)
 * [7] reserved
 * [6:4] CDC_ADCURR[2:0], right ADC current option:
 *      000--0.25I, 001--0.5I, 010--0.75I, 011--1I, 100--1.25I, 101--1.5I, 110--1.75I, 111--2I
 * [3] reserved
 * [2:0] CDC_ADCURL[2:0], left ADC current option,
 *      000--0.25I, 001--0.5I, 010--0.75I, 011--1I, 100--1.25I, 101--1.5I, 110--1.75I, 111--2I
 * Default: 0x33
 */
#define CDC_REGS_ADCUR          0x11

/*
 * R12: CDC_ADPDIREF
 * [7:2], reserved
 * [1] CDC_ADPDIREFR, power down current reference of right channel, 0 - power on, 1 - power down
 * [0] CDC_ADPDIREFL, power down current reference of left channel,  0 - power on, 1 - power down
 * Default: 0x03
 */
#define CDC_REGS_ADPDIREF       0x12

/*
 * R13: CDC_ADPD
 * [7:2], reserved
 * [1] CDC_ADPDR, power down ADC of right channel, 0 - power on, 1 - power down
 * [0] CDC_ADPDL, power down ADC of left channel,  0 - power on, 1 - power down
 * Default: 0x03
 */
#define CDC_REGS_ADPD           0x13

/*
 * R14: CDC_ADRSTB
 * [7:2], reserved
 * [1] CDC_ADRSTBR, reset ADC right channel;0--reset
 * [0] CDC_ADRSTBL, reset ADC left channel; 0--reset
 * Default: 0x00
 */
#define CDC_REGS_ADRSTB             0x14

/*
 * R15: CDC_ADVOL
 * [7:4] CDC_ADVOLR, ADC right channel PGA gain, -3dB to 42dB, 3dB/step;
 * [3:0] CDC_ADVOLL, ADC left channel PGA gain,  -3dB to 42dB, 3dB/step;
 * -3dB to 42dB, 3dB per step, default 0dB
 * Default: 0x11
 */
#define CDC_REGS_ADVOL          0x15

/*
 * R16: CDC_CTBUF
 * [7:3], reserved
 * [2] CDC_CTBUFFPD, Continous time buffer power down(active high), 0 - ct buffer power on, 1 - power down
 * [1:0] CDC_CTBUFFICTL[1:0], 00:default I set; 00--10u,01--12u;10--8u;11--5u; (REF CUR=10u)
 * [1:0] CDC_CTBUFFICTL[1:0], Continuous time buffer current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I(VA7621)
 * Default: 0x04
 */
#define CDC_REGS_CTBUF          0x16

/*
 * R17: CDC_DACCTL0
 * [7:4] CDC_DACDEBUG_R[3:0], right channel, 0000 - normal work;
 * [3:0] CDC_DACDEBUG_L[3:0], left channel,  0000 - normal work;
 * Default: 0x00
 */
#define CDC_REGS_DACCTL0        0x17

/*
 * R18: CDC_DACCTL1
 * [7] CDC_DACMODE_R, right channel, 0 - normal work
 * [6] CDC_DACDISDWA_R, right channel, 0 - normal work, 1 - disable DWA in DAC
 * [5:4] CDC_DACCTL_R[1:0], right channel, 00 - normal work
 *       CDC_DACCTL_R[1:0], DAC d2s current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I(VA7621)
 * [3] CDC_DACMODE_L, left channel,  0 - normal work
 * [2] CDC_DACDISDWA_L, left channel,  0 - normal work, 1 - disable DWA in DAC
 * [1:0] CDC_DACCTL_L[1:0], left channel,  00 - normal work
  *          CDC_DACCTL_L[1:0], DAC d2s current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I(VA7621)
 * Default: 0x44
 */
#define CDC_REGS_DACCTL1        0x18

/*
 * R19: CDC_DACCTL2
 * [7] CDC_DACPDLPF_R, power down dac lpf part(active high), right channel, 0 - normal work, 1 - power down dac lpf
 * [6] CDC_DACPDDIG_R, power down dac digital part(active high), right channel, 0 - normal work, 1 - power down dac digital
 * [5:4] CDC_DACLPFICTL_R[1:0], right channel, default I set; 00--10u,01--12u,10--8u,11--5u; (REF CUR=10u)
 *       CDC_DACLPFICTL_R[1:0], DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (VA7621)
 * [3] CDC_DACPDLPF_L, power down dac lpf part(active high), left channel, 0 - normal work, 1 - power down dac lpf
 * [2] CDC_DACPDDIG_L, power down dac lpf part(active high), left channel, 0 - normal work, 1 - power down dac digital
 * [1:0] CDC_DACLPFICTL_L[1:0], left channel,  default I set; 00--10u,01--12u,10--8u,11--5u; (REF CUR=10u)
 *       CDC_DACLPFICTL_L[1:0], DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (VA7621)
 * Default: 0xCC
 */
#define CDC_REGS_DACCTL2        0x19

/*
 * R1A: CDC_DACCTL3
 * [7:2], reserved
 * [1] CDC_DACRSTDIG_R, right channel, 0 - normal work, 1 - reset dac digital (can be controlled by i2c)
 * [0] CDC_DACRSTDIG_L, left channel,  0 - normal work, 1 - reset dac digital (can be controlled by i2c)
 * Default: 0x03
 */
/*
 * R1A: CDC_DACCTL3(20140210, modify for 7621A)
 * [7:5] reserved
 * [4] CDC_DACRSTDIG_R, Reset dac digital part, active high
 * [3:1] reserved
 * [0] CDC_DACRSTDIG_L, Reset dac digital part, active high
 * Default: 0x11
 */
#define CDC_REGS_DACCTL3        0x1A

/*
 * R1B: CDC_HPDOCN
 * [7:4] CDC_HPDOCN_R, right channel, Over current control of output NMOSFET. Normal value is 0x1100
 * [3:0] CDC_HPDOCN_L, left channel, Over current control of output NMOSFET. Normal value is 0x1100
 * Default: 0xCC
 */
/*
* R1B: CDC_HPDOCN(2011-12-26,modify 9611S_B&9611S_C) + 9611S_D, 7621A
* [7:4] RESERVED
* [3:0] CDC_HPDOCN[3:0], Over current control of output NMOSFET. Normal value is 0x1100; 2 channels
* Default: 0x0C
*/
#define CDC_REGS_HPDOCN             0x1B

/*
 * R1C: CDC_HPDOCP
 * [7:4] CDC_HPDOCP_R, right channel, Over current control of output PMOSFET. Normal value is 0x1100
 * [3:0] CDC_HPDOCP_L, left channel, Over current control of output PMOSFET. Normal value is 0x1100
 * Default: 0xCC
 */
/*
* R1C: CDC_HPDOCP(2011-12-26,modify 9611S_B&9611S_C) + 9611S_D, 7621A
* [7:4] RESERVED
* [3:0] CDC_HPDOCP[3:0], Over current control of output NMOSFET. Normal value is 0x1100; 2 channels
* Default: 0x0C
*/
#define CDC_REGS_HPDOCP             0x1C

/*
 * R1D: CDC_HP0
 * [7] CDC_HPOC_R, right channel, over current protection, 0 - power on, 1 - power down
 * [6] CDC_HPMUTE_R, right channel, mute HP PA, 0 - unmute, 1 - mute
 * [5:4] CDC_HPLP_R, right channel, set output current of PA, 00 - 800uA, 01 & 10 - 1.6mA, 11 - 2.4mA
 * [3] CDC_HPOC_L, left channel, over current protection, 0 - power on, 1 - power down
 * [2] CDC_HPMUTE_L, left channel, mute HP PA, 0 - unmute, 1 - mute
 * [1:0] CDC_HPLP_L, left channel, set output current of PA, 00 - 800uA, 01 & 10 - 1.6mA, 11 - 2.4mA
 * Default: 0x77
 */
/*
* R1D: CDC_HP0(2011-12-26,modify 9611S_B&9611S_C) + 9611S_D, 7621A
* [7] CDC_HPOC_R, right channel, over current protection, 0 - disable, 1 - enable
* [6] CDC_HPMUTE_R, right channel, mute HP PA, 0 - unmute PA, 1 - mute PA
* [5:4] RESERVED
* [3] CDC_HPOC_L, left channel,  over current protection, 0 - disable, 1 - enable
* [2] CDC_HPMUTE_L, left channel,  mute HP PA, 0 - unmute PA, 1 - mute PA
* [1:0] RESERVED
* Default: 0x44
*/
#define CDC_REGS_HP0                0x1D

/*
 * R1E: CDC_HP1
 * [7:6] RESERVED
 * [5] CDC_HPPOP_R, right channel, Headphone Popup Noise Suppression enable (active high, default low); 0--disable
 * [4] CDC_HPPDVCM_R, right channel, 1 - Power down vcom
 * [3] CDC_HPPD_R, right channel, Power down PA; when CDC_HPPDSEL_R=0:connected directly to register;when CDC_HPPDSEL_R=1:update when CDC_HPVCH_R is switched
 * [2] CDC_HPPOP_L, left channel,  Headphone Popup Noise Suppression enable (active high, default low); 0--disable
 * [1] CDC_HPPDVCM_L, left channel,  1 - Power down vcom
 * [0] CDC_HPPD_L, left channel,  Power down PA; when CDC_HPPDSEL_L=0:connected directly to register;when CDC_HPPDSEL_L=1:update when CDC_HPVCH_L is switched
 * Default: 0x1B
 */
/*
 * R1E: CDC_HP1(20140210, modify for 7621A)
 * [7] reserved
 * [6] CDC_HPPOP_R, enable cross zero of headphone PA, active high
 * [5] CDC_HPPDVCM_R, Power down vcom. (No use)
 * [4] CDC_HPPD_R, power down headphone PA
 * [3] reserved
 * [2] CDC_HPPOP_L, enable cross zero of headphone PA, active high
 * [1] CDC_HPPDVCM_L, Power down vcom. (No use)
 * [0] CDC_HPPD_L, power down headphone PA
 * Default: 0x33
 */
#define CDC_REGS_HP1                0x1E

/*
 * R1F: CDC_HP_STATUS
 * [7:4] reserved
 * [3] CDC_HPVCH_R, right channel, Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_R = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_R always High)
 * [2] CDC_HPSHORT_R, right channel, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low)
 * [1] CDC_HPVCH_L, left channel,  Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
 * [0] CDC_HPSHORT_L, left channel, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low)
 * Default: N/A
 */
/*
 * R1F: CDC_HP_STATUS(20140210, modify for 7621A)
 * [7:6] RESERVED
 * [5] CDC_HPVCH_R, Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
 * [4] CDC_HPSHORT_R, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
 * [3:2] RESERVED
 * [1] CDC_HPVCH_L, Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
 * [0] CDC_HPSHORT_L, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
 * Default: N/A
 */
#define CDC_REGS_HPSTAT             0x1F

/*
 * R20: CDC_HPVOL
 * [7:4] CDC_HPVOL_R[3:0], right channel, Headphone volume control, VOL_R<3> = MSB, (0000b = -18dB) (1111b = +12dB);2dB/step
 * [3:0] CDC_HPVOL_L[3:0], left channel,  Headphone volume control, VOL_L<3> = MSB, (0000b = -18dB) (1111b = +12dB);2dB/step
 * Default: 0x00
 */
#define CDC_REGS_HPVOL          0x20

/*
 * R21: CDC_MBSEL
 * [7:2], reserved
 * [1:0] CDC_MBSEL[1:0], Select MICBIAS output voltage.00 ->1.55V, 01 ->1.83V, 10 ->2.05V, 11 ->2.28V
 *       CDC_MBSEL[1:0], Mbias output voltage selection, 00--1.53V, 01--1.8V, 10--2V,11--2.2V(7621A)
 * Default: 0x02
 */
#define CDC_REGS_MBSEL          0x21

/*
 * R22: CDC_MIX
 * [7] CDC_MIXPD_R, right channel, power down MIXER, 0 -  normal work(power on), 1 - power down mixer
 * [6] CDC_MIXMUTE_R, right channel, mute MIXER, 0 - normal work, 1 - mute mixer
 * [5:4] CDC_MIXICTL_R[1:0], right channel, 0 - default I set; 00-10u,01--12u,10--8u,11--5u;
 * [3] CDC_MIXPD_L, left channel, power down MIXER, 0 -  normal work(power on), 1 - power down mixer
 * [2] CDC_MIXMUTE_L, left channel, mute MIXER, 0 - normal work, 1 - mute mixer
 * [1:0] CDC_MIXICTL_L[1:0], left channel,  0 - default I set; 00--10u,01--12u,10--8u,11--5u;
 * Default: 0xCC
 */
/*
* R22: CDC_MIX(VA7621S_DC0)
* [7] CDC_MIXPD_R, right channel, power down MIXER, 0 - power down mixer
* [6] CDC_MIXMUTE_R, right channel, mute MIXER, 0 - normal work, 1 - mute mixer
* [5:4] CDC_MIXICTL_R[1:0], right channel, 0 - default I set; 00-10u,01--12u,10--8u,11--5u;
* [3] CDC_MIXPD_L, left channel, power down MIXER, 0 - power down mixer
* [2] CDC_MIXMUTE_L, left channel, mute MIXER, 0 - normal work, 1 - mute mixer
* [1:0] CDC_MIXICTL_L[1:0], left channel,  0 - default I set; 00--10u,01--12u,10--8u,11--5u;
* Default: 0x44
*/
#define CDC_REGS_MIX            0x22

/*
 * R23: CDC_PGA
 * [7:4], reserved
 * [3] CDC_PGAPDR, power down PGA of ADC right channel, 0 - power on, 1 - power down
 * [2] CDC_PGAMUTER, mute PGA of ADC right channel, 0 - unmute, 1 - mute
 * [1] CDC_PGAPDL, power down PGA of ADC left channel, 0 - power on, 1 - power down
 * [0] CDC_PGAMUTEL, mute PGA of ADC left channel, 0 - unmute, 1 - mute
 * Default: 0x0F
 */
/*
 * R23: CDC_PGA(20140210, modify for 7621A)
 * [7:6] RESERVED
 * [5] CDC_PGAPDR, Power down right PGA, active high
 * [4] CDC_PGAMUTER, Mute right PGA, active high
 * [3:2] RESERVED
 * [1] CDC_PGAPDL, Power down left PGA, active high
 * [0] CDC_PGAMUTEL, Mute left PGA, active high
 * Default: 0x33
 */
#define CDC_REGS_PGA                0x23

/*
 * R24: CDC_REF
 * [7:5], reserved
 * [4] CDC_VCMSEL, 1:set vcom voltage, 0-1.45v, 1-1.65v
 * [3:2] CDC_REFVRFSEL[1:0], 00 - normal work;(disable in SC and SD version, 7621A)
 * [1:0] CDC_REFIOPT[1:0], 10 - default I set;00-2.5u,01-5u,10-10u,11-13u
 *       CDC_REFIOPT[1:0], current option of Reference block: 00 --0.25I ,01--0.5I ,10 --1I ,11--1.3I(7621A)
 * Default: 0x12
 */
#define CDC_REGS_REF                0x24

/*
 * R25: CDC_REFPD(20140220, 9611+7621SA)
 * [7:5], reserved
 * [4:0] CDC_REFPD[4:0], 00000 - normal work;
 *                                           00001 - power down self bias;
 *                                           00010 - power down bandgap;
 *                                           00100 - power down Iref;
 *                                           01000 - power down Mbias;
 *                                           10000 - power down VREF and VCOM
 * Default: 0x1F
 */
#define CDC_REGS_REFPD          0x25
/*
 * R25: CDC_REFPD_LOW(20140210, modify for 7621A+7621GA)
 * [7:5] RESERVED
 * [4:0] CDC_REFPD[4:0], bit3-bit0:vdd/2,current option. Bit4-5vdd/9. bit5-4vdd/9.
 *                       CDC_REFPD[4] --- vcom=5/9vdd,
 *                       CDC_REFPD[3] --- vcom=1/2vdd and 165uA,
 *                       CDC_REFPD[2] --- vcom=1/2vdd and 42uA,
 *                       CDC_REFPD[1] --- vcom=1/2vdd and 11uA,
 *                       CDC_REFPD[0] --- vcom=1/2vdd and 842nA.
 * Default: 0x1F
 */
#define CDC_REGS_REFPD_LOW          0x25

/*
 * R26: CDC_SCBUF
 * [7:3], reserved
 * [2] CDC_SCBUFFPD, SC BUFFER power down, 0 - power on(normal work), 1 - power down scbuffer; Power down switch cap buffer, active high
 * [1:0] CDC_SCBUFFICTL[1:0], 00 - normal I set; 00-10u,01-12u, 10-8u, 11-5u; (REF CUR=10u)
 *       CDC_SCBUFFICTL[1:0], current option of switch cap buffer: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I(7621A)
 * Default: 0x04
 */
#define CDC_REGS_SCBUF          0x26

/*
 * R27: CDC_HPPDSEL
 * [7:2], reserved
 * [1] CDC_HPPDSEL_R, select the source of CDC_HPPD_R. 0-connected directly to register;1-update when CDC_HPVCH_R is switched;
 * [0] CDC_HPPDSEL_L, select the source of CDC_HPPD_L. 0-connected directly to register,1-update when CDC_HPVCH_L is switched;
 * Default: 0x00
 */
/*
 * R27: CDC_HPPDSEL(20140210, modify for 7621A)
 * [7:5] RESERVED
 * [4] CDC_HPPDSEL_R, select the source of CDC_HPPD_R. 0:connected directly to register;1:update when CDC_HPVCH_R is switched;
 * [3:1] RESERVED
 * [0] CDC_HPPDSEL_L, select the source of CDC_HPPD_L. 0:connected directly to register;1:update when CDC_HPVCH_L is switched;
 * Default: 0x00
 */
#define CDC_REGS_HPPDSEL            0x27



//xy: 20140210 add registers for 7621A
/*
 * R28: CDC_HPLP
 * [7:6] RESERVED
 * [5:4] CDC_HPLP_R[1:0], Set output current of PA. 00->800uA, 01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
 * [3:2] RESERVED
 * [1:0] CDC_HPLP_L[1:0], Set output current of PA. 00->800uA, 01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
 * Default: 0x00
 */
#define CDC_REGS_HPLP           0x28

/*
 * R29: CDC_DF_RESERVE
 * [7:0] RESERVED
 * Default: 0x00
 */
#define CDC_REGS_DF_RESERVE         0x29

/*
 * R2A: CDC_POP_DEBUG
 * [7:0] RESERVED
 * Default: 0x00
 */
#define CDC_REGS_POP_DEBUG          0x2A

/*
 * R2B: CDC_PD_OUT_R
 * [7] RESERVED
 * [6]  CDC_PD_OUTN2I_R, power down output stage N terminal--2, active high, right channel
 * [5]  CDC_PD_OUTN1I_R, power down output stage N terminal--1, active high, right channel
 * [4]  CDC_PD_OUTN0I_R, power down output stage N terminal--0, active high, right channel
 * [3] RESERVED
 * [2]  CDC_PD_OUTP2I_R, power down output stage P terminal--2, active high, right channel
 * [1]  CDC_PD_OUTP1I_R, power down output stage P terminal--1, active high, right channel
 * [0]  CDC_PD_OUTP0I_R, power down output stage P terminal--0, active high, right channel
 * Default: 0x77
 */
#define CDC_REGS_PD_OUT_R           0x2B

/*
 * R2C: CDC_PD_OUT_L
 * [7] RESERVED
 * [6]  CDC_PD_OUTN2I_L, power down output stage N terminal--2, active high, left channel
 * [5]  CDC_PD_OUTN1I_L, power down output stage N terminal--1, active high, left channel
 * [4]  CDC_PD_OUTN0I_L, power down output stage N terminal--0, active high, left channel
 * [3] RESERVED
 * [2]  CDC_PD_OUTP2I_L, power down output stage P terminal--2, active high, left channel
 * [1]  CDC_PD_OUTP1I_L, power down output stage P terminal--1, active high, left channel
 * [0]  CDC_PD_OUTP0I_L, power down output stage P terminal--0, active high, left channel
 * Default: 0x77
 */
#define CDC_REGS_PD_OUT_L           0x2C

/*
 * R2D: CDC_ICTL_STBF(VA7621_A0)
 * [7:2] RESERVED
 * [1:0] CDC_ICTL_STBF[1:0], POP noise buffer current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I
 * Default: 0x00
 */
/*
 * R2D: CDC_ICTL_STBF(VA7621S_A0, VA7621G)
 * [7:6] RESERVED
 * [5:4] CDC_ICTL_STBF_R[1:0], POP noise buffer current option, right channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I
 * [3:2] RESERVED
 * [1:0] CDC_ICTL_STBF_L[1:0], POP noise buffer current option, left channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I
 * Default: 0x00
 */
#define CDC_REGS_ICTL_STBF          0x2D

/*
 * R2E: CDC_PD_CHARGE(VA7621_A0)
 * [7:4] RESERVED
 * [3:0] CDC_PD_CHARGE[3:0], power down resistor divider, active high
 * Default: 0x0E
 */
/*
 * R2E: CDC_PD_CHARGE(VA7621S_A0, VA7621G)
 * [7:4] CDC_PD_CHARGE_R[3:0], power down resistor divider, active high, right channel
 * [3:0] CDC_PD_CHARGE_L[3:0], power down resistor divider, active high, left channel
 * Default: 0xEE
 */
#define CDC_REGS_PD_CHARGE          0x2E

/*
 * R2F: CDC_PD_FD(VA7621_A0)
 * [7:3] RESERVED
 * [2] CDC_PD_CMFB, power down common mode feedback, change mode from full differential to single end;active high
 * [1] CDC_PD_FDNCS, power down NMOS current source, change mode from full differential to single end;active high
 * [0] CDC_PD_FDPCS, power down PMOS current source, change mode from full differential to single end;active high
 * Default: 0x00
 */
/*
 * R2F: CDC_PD_FD(VA7621S_A0, VA7621G)
 * [7] RESERVED
 * [6] CDC_PD_CMFB_R, power down common mode feedback, change mode from full differential to single end;active high, right channel
 * [5] CDC_PD_FDNCS_R, power down NMOS current source, change mode from full differential to single end;active high, right channel
 * [4] CDC_PD_FDPCS_R, power down PMOS current source, change mode from full differential to single end;active high, right channel
 * [3] RESERVED
 * [2] CDC_PD_CMFB_L, power down common mode feedback, change mode from full differential to single end;active high, left channel
 * [1] CDC_PD_FDNCS_L, power down NMOS current source, change mode from full differential to single end;active high, left channel
 * [0] CDC_PD_FDPCS_L, power down PMOS current source, change mode from full differential to single end;active high, left channel
 * Default: 0x00
 */
#define CDC_REGS_PD_FD              0x2F

/*
 * R30: CDC_PD_OUT(VA7621_A0)
 * [7] RESERVED
 * [6] CDC_PD_OUTN2I, power down output stage N terminal--2, active high
 * [5] CDC_PD_OUTN1I, power down output stage N terminal--1, active high
 * [4] CDC_PD_OUTN0I, power down output stage N terminal--0, active high
 * [3] RESERVED
 * [2] CDC_PD_OUTP2I, power down output stage P terminal--2, active high
 * [1] CDC_PD_OUTP1I, power down output stage P terminal--1, active high
 * [0] CDC_PD_OUTP0I, power down output stage P terminal--0, active high
 * Default: 0x77
 */
/*
 * R30: CDC_RESERVE(VA7621S_A0, VA7621G)
 * [7:0] RESERVED
 * Default: 0x00
 */
#define CDC_REGS_PD_OUT             0x30

/*
 * R31: CDC_PGA_CUR
 * [7] RESERVED
 * [6:4] CDC_PGA_CUR_R[2:0], right PGA current option:
 *      000--0.25I, 001--0.5I, 010--0.75I, 011--1I, 100--1.25I, 101--1.5I, 110--1.75I, 111--2I
 * [3] RESERVED
 * [2:0] CDC_PGA_CUR_L[2:0], left PGA current option:
 *      000--0.25I, 001--0.5I, 010--0.75I, 011--1I, 100--1.25I, 101--1.5I, 110--1.75I, 111--2I
 * Default: 0x033
 */
#define CDC_REGS_PGA_CUR            0x31

/*
 * R32: CDC_PGA_PD_IREF
 * [7:5] RESERVED
 * [4] CDC_PGA_PD_IREF_R, power down current reference of right PGA
 * [3:1] RESERVED
 * [0] CDC_PGA_PD_IREF_L, power down current reference of left PGA
 * Default: 0x11
 */
#define CDC_REGS_PGA_PD_IREF            0x32

/*
 * R33: CDC_PWD_HFOUT(VA7621_A0)
 * [7:3] RESERVED
 * [2:0] CDC_PWD_HFOUT[2:0], power down pop noise buffer driver, active high
 * Default: 0x07
 */
/*
 * R33: CDC_PWD_HFOUT(VA7621S_A0, VA7621G)
 * [7] RESERVED
 * [6:4] CDC_PWD_HFOUT_R[2:0], power down pop noise buffer driver, active high, right channel
 * [3] RESERVED
 * [2:0] CDC_PWD_HFOUT_L[2:0], power down pop noise buffer driver, active high, left channel
 * Default: 0x77
 */
#define CDC_REGS_PWD_HFOUT              0x33

/*
 * R34: CDC_PWD_STBF(VA7621_A0)
 * [7:1] RESERVED
 * [0] CDC_PWD_STBF, power down pop noise buffer, active high
 * Default: 0x01
 */
/*
 * R34: CDC_PWD_STBF(VA7621S_A0, VA7621G)
 * [7:5] RESERVED
 * [4] CDC_PWD_STBF_R, power down pop noise buffer, active high, right channel
 * [3:1] RESERVED
 * [0] CDC_PWD_STBF_L, power down pop noise buffer, active high, left channel
 * Default: 0x11
 */
/*
* R34: CDC_PWD_STBF(VA7621S_DC0)
* [7:5] RESERVED
* [4] CDC_PWD_STBF_R, power down pop noise buffer, active low, right channel
* [3:1] RESERVED
* [0] CDC_PWD_STBF_L, power down pop noise buffer, active low, left channel
* Default: 0x00
*/
#define CDC_REGS_PWD_STBF               0x34

/*
 * R35: CDC_REFPD_HIGH
 * [7:4] RESERVED
 * [3:0] CDC_REFPD[8:5], bit3-bit0:vdd/2,current option. Bit4-5vdd/9. bit5-4vdd/9.
 *                      CDC_REFPD[8] --- iref,
 *                      CDC_REFPD[7] --- mbias,
 *                      CDC_REFPD[6] --- igen,
 *                      CDC_REFPD[5] --- vcom=4/9vdd
 * Default: 0x0F
 */
#define CDC_REGS_REFPD_HIGH             0x35

/*
 * R36: CDC_HP_PULLDOWN_EN(VA7621_A0)
 * [7:1] RESERVED
 * [0] CDC_HP_PULLDOWN_EN, pull down to ground of headphone PA output
 * Default: 0x00
 */
/*
 * R36: CDC_HP_PULLDOWN_EN(VA7621S_A0, VA7621G)
 * [7:5] RESERVED
 * [4] CDC_HP_PULLDOWN_EN_R, pull down to ground of headphone PA output, right channel
 * [3:1] RESERVED
 * [0] CDC_HP_PULLDOWN_EN_L, pull down to ground of headphone PA output, left channel
 * Default: 0x00
 */
#define CDC_REGS_HP_PULLDOWN_EN         0x36

/*
 * R37: CDC_HP_ICTL
 * [7:6] RESERVED
 * [5:4] CDC_HP_ICTL_R[1:0], current option of headphone PA: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I
 * [3:2] RESERVED
 * [1:0] CDC_HP_ICTL_L[1:0], current option of headphone PA: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I
 * Default: 0x00
 */
#define CDC_REGS_HP_ICTL                0x37

/*
 * R38: CDC_HP_PWD_IREF
 * [7:5] RESERVED
 * [4] CDC_HP_PWD_IREF_R, power down current reference of headphone PA
 * [3:1] RESERVED
 * [0] CDC_HP_PWD_IREF_L, power down current reference of headphone PA
 * Default: 0x11
 */
#define CDC_REGS_PGA_HP_PWD_IREF        0x38

/*
 * R39: CDC_HPPD_OUT
 * [7:6] RESERVED
 * [5] CDC_HPPD_OUT2I_R, power down headphone PA output driver,partly;active high
 * [4] CDC_HPPD_OUT1I_R, power down headphone PA output driver,partly;active high
 * [3:2] RESERVED
 * [1] CDC_HPPD_OUT2I_L, power down headphone PA output driver,partly;active high
 * [0] CDC_HPPD_OUT1I_L, power down headphone PA output driver,partly;active high
 * Default: 0x33
 */
#define CDC_REGS_HPPD_OUT               0x39

//20140218 add for VA7621S_A0
/*
 * R3A: CDC_NMILLER
 * [7:1] RESERVED
 * [0] CDC_NMILLER, control miller cap, 0=miller, 1=no miller
 * Default: 0x00
 */
#define CDC_REGS_NMILLER                0x3A

//xy: 20140210 end for 7621A add


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
#define CDC_REGS_I2S_CTRL				0x41// to enable rx_en,[4:3] bitwidth[1:0] must be 10

/*
 * R4D: DECIM_CTRL
 * [7] decim_clk_sel,    1'b0:  the main clock of decimation filter is the reverse signal of dack_incore.
                1'b1:  the main clock of decimation filter is dack_incore.
 *[6:3] decim_gain_sel[3:0], Decimation gain selection.
                        4'b0000~4'b1111 <==> 1~2.875
 * [2:0] hpf_sel[2:0], HPF coefficient selection.
                3'b0xx:  by pass HPF
                3'b100:  a=7/8
                3'b101:  a=15/16
                3'b110:  a=31/32
                3'b111:  a=2047/2048

 * Default: 0x04
 */
#define CDC_REGS_DECIM_CTRL             0x4D

/*
 * R4E: PLL_CTRL_R
 * [7:6] RESERVED
 * [5:0] r[5:0],PLL input divider control pins.  Divider ratio configuration.
 * Default: 0x05
 */
#define CDC_REGS_PLL_CTRL_R             0x4E

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
 * R5A: PLLOUT_DIV
 * [7:0] pllout_div[7:0], the divider ratio after PLL, for I2S_BCLK
  * Default: 100
 */
#define CDC_REGS_PLLOUT_DIV             0x5A

/*
 * R5B: PLLDIV_OEN
 * [7:2] RESERVED
 * [1] plldiv_oen_mclk, the divider output enable, active high, for I2S_MCLK
 * [0] plldiv_oen, the divider output enable, active high.
 * Default: 0x00
 */
#define CDC_REGS_PLLDIV_OEN             0x5B

/*
 * R5C: FOUT_EN
 * [7:5] RESERVED
 * [4:3] pll_ref_src_sel[1:0],PLL referecne clock selection:
 * 						2'b00 ---- dacki, 	IIS used the dac(aif) clock, if open dac(should enable aif dac and give the data out) clock then output 6M clock to IIS ok;
 * 						2'b01 ---- bclk_incore,	 IIS used the adc(aif) clock, if open adc(enable aif adc) clock and the sample rate should >=16K(the adc 8K then out clock 1M to IIS may let it err, base clock should more then 2M==16K)
 * 						2'b10 ---- hd_scl;	the 9625p not support, 9638&9635 had it;
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
 * R5D: PLLOUT_DIV_MCLK
 * [7:0] pllout_div_mclk[7:0], the divider ratio after PLL, for I2S_MCLK
  * Default: 25
 */
#define CDC_REGS_PLLOUT_DIV_MCLK                0x5D
//end chen jh add 20181130 add for VA7621I2S

//20170521 add for VA7621SD_C0
/*
 * R46: CDC_HPAGAIN_MODE
 * [7:1] RESERVED
 * [0] CDC_HPAGAIN_MODE , b0: Use {CDC_HPVOL_R[3:0], 2' b0} to control the headphone PA volume of left and right channels.
                          b1: Use {CDC_HPVOL_R[3:0], CDC_HPVOL_L[1:0]} to control the headphone PA volume of left and right channels.
 * Default: 0x00
 */
#define CDC_REGS_CDC_HPAGAIN_MODE           0x46

//xy: 20170521 end for 7621SD

/*
 * R55: I2C_TEST
 * [7:0] TEST_DATA, used for i2c r/w test
 * Default: 0x00
 */
#define I2C_REGS_TEST           0x55
//end 9611s registers define



//xy: 20190610 start for new VA7632B_A0

//I2CSLV  for  VA7632B_A0   v0.2;   note:I2C Register for "analog audio part":??40H~BFH???,???analog audio????
/*
 * R40: CDC_REFPD_LOW
 * [7:5] RESERVED
 * [4:0] CDC_REFPD[4:0], bit0:self bias; bit1:bandgap; bit2:current reference; bit3:Mic bias; bit4:Vref and Vcom; active high
 * Default: 0x1F
 */
#define CDC_REG763X_REFPD_LOW           0x40

/*
 * R41: CDC_CTBUF
 * [7:3] RESERVED
 * [2] CDC_CTBUFFPD, Continous time buffer power down, active low
 * [1:0] RESERVED, (?????????9H???debug pin??)
 * Default: 0x00
 */
#define CDC_REG763X_CTBUF               0x41

/*
 * R42: CDC_SCBUF
 * [7:3] RESERVED
 * [2] CDC_SCBUFFPD, Power down switch cap buffer, active high
 * [1:0] RESERVED, (?????????9H???debug pin??)
 * Default: 0x04
 */
#define CDC_REG763X_SCBUF               0x42

/*
 * R43: CDC_ADCLKEN
 * [7:2] RESERVED
 * [1] CDC_ADCLKENR, adc clock enable, right channel, active high
 * [0] CDC_ADCLKENL, adc clock enable, left channel, active high
 * Default: 0x00
 */
#define CDC_REG763X_ADCLKEN             0x43

/*
 * R44: CDC_ADPDIREF
 * [7:2] RESERVED
 * [1] CDC_ADPDIREFR, power down current reference in right channel adc, active high
 * [0] CDC_ADPDIREFL, power down current reference in left channel adc, active high
 * Default: 0x03
 */
#define CDC_REG763X_ADPDIREF            0x44

/*
 * R45: CDC_ADPD
 * [7:2] RESERVED
 * [1] CDC_ADPDR, power down ADC right channel,active high
 * [0] CDC_ADPDL, power down ADC left channel,active high
 * Default: 0x03
 */
#define CDC_REG763X_ADPD                0x45

/*
 * R46: CDC_ADRSTB
 * [7:2] RESERVED
 * [1] CDC_ADRSTBR, Reset right ADC digital part, active low
 * [0] CDC_ADRSTBL, Reset left ADC digital part, active low
 * Default: 0x00
 */
#define CDC_REG763X_ADRSTB              0x46

/*
 * R47: CDC_ADVOLL_INIT
 * [7:6] RESERVED
 * [5:0] CDC_ADVOLL_INIT[5:0], PGA gain, -3dB to 42dB, left channel;
 *                      gain table:42dB,39,36,34.5,34...  (0.5dB/step)...  21dB, 18,15,12,10.5...  (0.5dB/step)...   -3dB;
 * Default: 0x06
 */
#define CDC_REG763X_ADVOLL_INIT         0x47

/*
 * R48: CDC_ADVOLR_INIT
 * [7:6] RESERVED
 * [5:0] CDC_ADVOLR_INIT[5:0], PGA gain, -3dB to 42dB, right channel;
 *                      gain table:42dB,39,36,34.5,34...  (0.5dB/step)...  21dB, 18,15,12,10.5...  (0.5dB/step)...   -3dB;
 * Default: 0x06
 */
#define CDC_REG763X_ADVOLR_INIT         0x48

/*
 * R49: CDC_LINEIN_AMP_DET
 * [7:5] RESERVED
 * [4] CDC_LINEIN_AMP_DET_R, CDC line_in amplitude detect for right channel.   0:small, 1: large.
 * [3:1] RESERVED
 * [0] CDC_LINEIN_AMP_DET_L, CDC line_in amplitude detect for left channel.    0:small, 1: large.
 * Default: N/A
 */
#define CDC_REG763X_LINEIN_AMP_DET      0x49

/*
 * R4A: CDC_ADVOLL_CUR
 * [7:6] RESERVED
 * [5:0] CDC_ADVOLL_CUR[5:0], PGA gain used with smalle amplitude signal (indicated by "CDC_LINEIN_AMP_DET_L"), left channel;
 *                       gain range: -3dB to 42dB
 *                       gain table: 42dB,39,36,34.5,34 (0.5dB/step)  21dB, 18,15,12,10.5  (0.5dB/step)  -3dB
 * Default: 0x06
 */
#define CDC_REG763X_ADVOLL_CUR          0x4A

/*
 * R4B: CDC_ADVOLR_CUR
 * [7:6] RESERVED
 * [5:0] CDC_ADVOLR_CUR[5:0], PGA gain used with smalle smplitude signal (indicated by "CDC_LINEIN_AMP_DET_R"), right channel;
 *                       gain range: -3dB to 42dB
 *                       gain table: 42dB,39,36,34.5,34 (0.5dB/step)  21dB, 18,15,12,10.5  (0.5dB/step)  -3dB
 * Default: 0x06
 */
#define CDC_REG763X_ADVOLR_CUR          0x4B

/*
 * R4C: CDC_PGA
 * [7:6] RESERVED
 * [5] CDC_PGAPDR, Power down right PGA, active high
 * [4] CDC_PGAMUTER, Mute right PGA, active high
 * [3:2] RESERVED
 * [1] CDC_PGAPDL, Power down left PGA, active high
 * [0] CDC_PGAMUTEL, Mute left PGA, active high
 * Default: 0x33
 */
#define CDC_REG763X_PGA                 0x4C

/*
 * R4D: CDC_PGA_PD_IREF
 * [7:5] RESERVED
 * [4] CDC_PGA_PD_IREF_R, power down current reference of right PGA
 * [3:1] RESERVED
 * [0] CDC_PGA_PD_IREF_L, power down current reference of left PGA
 * Default: 0x11
 */
#define CDC_REG763X_PGA_PD_IREF         0x4D

/*
 * R4E: CDC_LINEIN_SEL
 * [7:5] RESERVED
 * [4] CDC_LINEIN_SELR, 1:select right channel of line in; 0:select right channel of MIC.
 * [3:1] RESERVED
 * [0] CDC_LINEIN_SELL, 1:select left channel of line in; 0:select left channel of MIC.
 * Default: 0x00
 */
#define CDC_REG763X_LINEIN_SEL          0x4E

/*
 * R4F: CDC_DACCTL0
 * [7:4] CDC_DACDEBUG_R[3:0], 0000:normal work; right channel (No use)
 * [3:0] CDC_DACDEBUG_L[3:0], 0000:normal work; left channel (No use)
 * Default: 0x00
 */
#define CDC_REG763X_DACCTL0             0x4F

/*
 * R50: CDC_DACCTL2
 * [7] CDC_DACPDLPF_R, power down dac lpf part,active low
 * [6] CDC_DACPDDIG_R, power down dac digital part,active low
 * [5:4] RESERVED, (?????????9H???debug pin??)
 * [3] CDC_DACPDLPF_L, power down dac lpf part,active low
 * [2] CDC_DACPDDIG_L, power down dac digital part,active low
 * [1:0] RESERVED, (?????????9H???debug pin??)
 * Default: 0x00
 */
#define CDC_REG763X_DACCTL2             0x50

/*
 * R51: CDC_DACCTL3
 * [7:5] RESERVED
 * [4] CDC_DACRSTDIG_R, Reset dac digital part, active high
 * [3:1] RESERVED
 * [0] CDC_DACRSTDIG_L, Reset dac digital part, active high
 * Default: 0x11
 */
#define CDC_REG763X_DACCTL3             0x51

/*
* R52: CDC_HPDOCN
* [7:4] RESERVED
* [3:0] CDC_HPDOCN[3:0], Over current control of output NMOSFET. Normal value is 0x1100; 2 channels  (No use)
* Default: 0x0C
*/
#define CDC_REG763X_HPDOCN              0x52

/*
* R53: CDC_HPDOCP
* [7:4] RESERVED
* [3:0] CDC_HPDOCP[3:0], Over current control of output PMOSFET. Normal value is 0x1100; 2 channels (No use)
* Default: 0x0C
*/
#define CDC_REG763X_HPDOCP              0x53

/*
* R54: CDC_HP0
* [7] CDC_HPOC_R, Over current protection; right channel;0--disable (No use)
* [6] CDC_HPMUTE_R, Mute PA.1: mute PA, 0: unmute PA; right channel
* [5:4] RESERVED
* [3] CDC_HPOC_L, Over current protection; left channel;0--disable (No use)
* [2] CDC_HPMUTE_L, Mute PA.1: mute PA, 0: unmute PA; left channel
* [1:0] RESERVED
* Default: 0x44
*/
#define CDC_REG763X_HP0                 0x54

/*
 * R55: CDC_HP1
 * [7] RESERVED
 * [6] CDC_HPPOP_R, enable cross zero of headphone PA, active high
 * [5] CDC_HPPDVCM_R, Power down vcom. (No use)
 * [4] CDC_HPPD_R, power down headphone PA,active low
 * [3] RESERVED
 * [2] CDC_HPPOP_L, enable cross zero of headphone PA, active high
 * [1] CDC_HPPDVCM_L, Power down vcom. (No use)
 * [0] CDC_HPPD_L, power down headphone PA,active low
 * Default: 0x22
 */
#define CDC_REG763X_HP1                 0x55

/*
 * R56: CDC_HP_STATUS
 * [7:6] RESERVED
 * [5] CDC_HPVCH_R, Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
 * [4] CDC_HPSHORT_R, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
 * [3:2] RESERVED
 * [1] CDC_HPVCH_L, Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
 * [0] CDC_HPSHORT_L, Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
 * Default: N/A
 */
#define CDC_REG763X_HPSTAT              0x56

/*
* R57: CDC_HPVOL_4BIT
* [7:4] CDC_HPVOL_4BIT_R[3:0], headphone PA volume control, -18dB --12dB, 2dB/step, (0000b = -18dB) (1111b = +12dB)
* [3:0] CDC_HPVOL_4BIT_L[3:0], headphone PA volume control, -18dB --12dB, 2dB/step, (0000b = -18dB) (1111b = +12dB)
* Default: 0x0C
*/
#define CDC_REG763X_HPVOL_4BIT      0x57

/*
* R58: CDC_HPVOL_6BIT_L
* [7:6] RESERVED
* [5:0] CDC_HPVOL_6BIT_L[5:0], HPA gain adjusting for left/right channel;
*      gain table:
*                        Gain:  -18dB, -17.5dB, -17dB,.......11dB, 11.5dB, 12dB  (0.5dB gain step)
*  CDC_HPVOL_6BIT_L/R[5:0]:  0, 1, 2, ....... 58,  59, 60   (max gain(12dB) for either 60,61,62,63)
* Default: 0x00
*/
#define CDC_REG763X_HPVOL_6BIT_L        0x58

/*
* R59: CDC_HPVOL_6BIT_R
* [7:6] RESERVED
* [5:0] CDC_HPVOL_6BIT_R[5:0], HPA gain adjusting for left/right channel;
*      gain table:
*                        Gain:  -18dB, -17.5dB, -17dB,.......11dB, 11.5dB, 12dB  (0.5dB gain step)
*  CDC_HPVOL_6BIT_L/R[5:0]:  0, 1, 2, ....... 58,  59, 60   (max gain(12dB) for either 60,61,62,63)
* Default: 0x00
*/
#define CDC_REG763X_HPVOL_6BIT_R        0x59

/*
* R5A: CDC_MIX
* [7] CDC_MIXPD_R, power down mixer,active low
* [6] CDC_MIXMUTE_R, mute mixer, active high
* [5:4] RESERVED, (?????????9H???debug pin??)
* [3] CDC_MIXPD_L, power down mixer,active low
* [2] CDC_MIXMUTE_L, mute mixer, active high
* [1:0] RESERVED, (?????????9H???debug pin??)
* Default: 0x44
*/
#define CDC_REG763X_MIX                 0x5A

/*
 * R5B: CDC_HPLP
 * [7:6] RESERVED
 * [5:4] CDC_HPLP_R[1:0], Set output current of PA. 00->800uA, 01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
 * [3:2] RESERVED
 * [1:0] CDC_HPLP_L[1:0], Set output current of PA. 00->800uA, 01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
 * Default: 0x00
 */
#define CDC_REG763X_HPLP                0x5B

/*
* R5C: CDC_PWD_STBF
* [7:5] RESERVED
* [4] CDC_PWD_STBF_R, power down pop noise buffer, active low, right channel
* [3:1] RESERVED
* [0] CDC_PWD_STBF_L, power down pop noise buffer, active low, left channel
* Default: 0x00
*/
#define CDC_REG763X_PWD_STBF            0x5C

/*
* R5D: CDC_HP_PWD_IREF
* [7:5] RESERVED
* [4] CDC_HP_PWD_IREF_R, power down current reference of headphone PA, right channel,active low
* [3:1] RESERVED
* [0] CDC_HP_PWD_IREF_L, power down current reference of headphone PA, left channel,active low
* Default: 0x00
*/
#define CDC_REG763X_HP_PWD_IREF         0x5D

/*
* R5E: CDC_DF_RESERVE
* [7:0] CDC_DF_RESERVE[7:0], reserverd for differential output. (No use)
* Default: 0x00
*/
#define CDC_REG763X_DF_RESERVE          0x5E

/*
* R5F: CDC_POP_DEBUG
* [7:0] CDC_POP_DEBUG[7:0], reserverd for pop noise debug. (No use)
* Default: 0x00
*/
#define CDC_REG763X_POP_DEBUG           0x5F

/*
* R60: CDC_RESERVE
* [7:0] CDC_RESERVE[7:0], bit0: CDC_CBOOST_L;  bit4:CDC_CBOOST_R  (SC4,5,6 version)
* Default: 0x00
*/
#define CDC_REG763X_RESERVE             0x60

/*
* R61: CDC_RESERVE1
* [7:0] CDC_RESERVE1[7:0], CDC reserve.
* Default: 0x00
*/
#define CDC_REG763X_RESERVE1            0x61


//I2CSLV  for  VA7632B_A0   v0.2;   note:80H~BF?????,??analog audio????debug pin?
/*
 * R80: CDC_PGA_CUR_DG
 * [7] RESERVED
 * [6:4] CDC_PGA_CUR_R[2:0], right PGA current option:000--0.25I,001--0.5I, 010--0.75I,011--1I,100--1.25I,101--1.5I, 110--1.75I,111--2I (debug pin)
 * [3] RESERVED
 * [2:0] CDC_PGA_CUR_L[2:0], left PGA current option:000--0.25I,001--0.5I, 010--0.75I,011--1I,100--1.25I,101--1.5I, 110--1.75I,111--2I (debug pin)
 * Default: 0x33
 */
#define CDC_REG763X_PGA_CUR_DG          0x80

/*
 * R81: CDC_ADCUR_DG
 * [7] RESERVED
 * [6:4] CDC_ADCURR[2:0], right ADC current option:000--0.25I,001--0.5I, 010--0.75I,011--1I,100--1.25I,101--1.5I, 110--1.75I,111--2I  (debug pin)
 * [3] RESERVED
 * [2:0] CDC_ADCURL[2:0], left ADC current option:000--0.25I,001--0.5I, 010--0.75I,011--1I,100--1.25I,101--1.5I, 110--1.75I,111--2I  (debug pin)
 * Default: 0x33
 */
#define CDC_REG763X_ADCUR_DG            0x81

/*
 * R82: CDC_REF_DG
 * [7:5] RESERVED
 * [4] CDC_VCMSEL, (debug pin)  (no use)
 * [3:2] CDC_REFVRFSEL[1:0], (debug pin)  (no use)
 * [1:0] CDC_REFIOPT[1:0], current option of Reference block: 00 --0.25I ,01--0.5I ,10 --1I ,11--1.3I (debug pin)
 * Default: 0x12
 */
#define CDC_REG763X_REF_DG              0x82

/*
 * R83: CDC_VREF_SEL_DG
 * [7:3] RESERVED
 * [2:0] CDC_VREF_SEL[2:0], CDC vref option. 000-1.2V, 001-1.3V, 010-1.4v, 011-1.5v, 100-1.6v, 101-1.7v, 110-1.8v, 111-2.0v. (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_VREF_SEL_DG         0x83

/*
 * R84: CDC_VCOM_SEL_DG
 * [7:3] RESERVED
 * [2:0] CDC_VCOM_SEL[2:0], Mbias output voltage selection, 00:1.5V,01:1.83V,10:2V,11:2.2V (debug pin)
 * Default: 0x06
 */
#define CDC_REG763X_VCOM_SEL_DG         0x84

/*
 * R85: CDC_CTSC_DG
 * [7:4] RESERVED
 * [3:2] CDC_SCBUFFICTL[1:0], current option of switch cap buffer: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin); 
 * 		(7632BF3)00 -- 0.4I, 01 -- 0.5I, 10 -- 0.335I, 11 -- 0.2I ;	
 * [1:0] CDC_CTBUFFICTL[1:0], Continuous time buffer current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * 		(7632BF3)00 -- 0.4I, 01 -- 0.5I, 10 -- 0.335I, 11 -- 0.2I ;
 * Default: 0x00
 */
#define CDC_REG763X_CTSC_DG             0x85

/*
 * R86: CDC_MBSEL_DG
 * [7:2] RESERVED
 * [1:0] CDC_MBSEL[1:0], Mbias output voltage selection, 00:1.5V,01:1.83V,10:2V,11:2.2V (debug pin)
 * Default: 0x02
 */
#define CDC_REG763X_MBSEL_DG            0x86

/*
 * R87: CDC_REFPD_HIGH_DG
 * [7:4] RESERVED
 * [3:0] CDC_REFPD[8:5], bit3-bit0:vdd/2,current option; Bit4-5vdd/9; .bit5-4vdd/9. (debug pin)(no used in E version)
 * Default: 0x0F
 */
#define CDC_REG763X_REFPD_HIGH_DG       0x87

/*
 * R88: CDC_DACCTL1_DG
 * [7] CDC_DACMODE_R, 0: normal work; right channel (debug pin)
 * [6] CDC_DACDISDWA_R, disable DWA in DAC, active high(debug pin)
 * [5:4] CDC_DACCTL_R[1:0], DAC control signal, only for debug.(debug pin)
 * [3] CDC_DACMODE_L, 0: normal work; left channel(debug pin)
 * [2] CDC_DACDISDWA_L, disable DWA in DAC, active high (debug pin)
 * [1:0] CDC_DACCTL_L[1:0], DAC control signal, only for debug.(debug pin)
 * Default: 0x44
 */
#define CDC_REG763X_DACCTL1_DG          0x88

/*
 * R89: CDC_DACCTL2_DG
 * [7:6] RESERVED
 * [5:4] CDC_DACLPFICTL_R[1:0], DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * [3:2] RESERVED
 * [1:0] CDC_DACLPFICTL_L[1:0], DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_DACCTL2_DG          0x89

/*
 * R8A: CDC_MIX_DG
 * [7:6] RESERVED
 * [5:4] CDC_MIXICTL_R[1:0], current option of Mixer:  00:1I ,01:1.2I ,10:0.8I ,11:0.5I (debug pin)
 * [3:2] RESERVED
 * [1:0] CDC_MIXICTL_L[1:0], current option of Mixer:  00:1I ,01:1.2I ,10:0.8I ,11:0.5I (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_MIX_DG              0x8A

/*
 * R8B: CDC_PD_OUT_R_DG
 * [7] RESERVED
 * [6] CDC_PD_OUTN2I_R, power down output stage N terminal--2 (64 fingers), active low, right channel (debug pin)640uA
 * [5] CDC_PD_OUTN1I_R, power down output stage N terminal--1 (48 fingers), active low, right channel (debug pin)480uA
 * [4] CDC_PD_OUTN0I_R, power down output stage N terminal--0 (48 fingers), active low, right channel (debug pin)480uA
 * [3] RESERVED
 * [2] CDC_PD_OUTP2I_R, power down output stage P terminal--2 (64 fingers), active low, right channel (debug pin)640uA
 * [1] CDC_PD_OUTP1I_R, power down output stage P terminal--1 (48 fingers), active low, right channel (debug pin)480uA
 * [0] CDC_PD_OUTP0I_R, power down output stage P terminal--0 (48 fingers), active low, right channel (debug pin)480uA
 * Default: 0x00
 */
#define CDC_REG763X_PD_OUT_R_DG         0x8B

/*
 * R8C: CDC_PD_OUT_L_DG
  * [7] RESERVED
 * [6] CDC_PD_OUTN2I_R, power down output stage N terminal--2 (64 fingers), active low, left channel (debug pin)640uA
 * [5] CDC_PD_OUTN1I_R, power down output stage N terminal--1 (48 fingers), active low, left channel (debug pin)480uA
 * [4] CDC_PD_OUTN0I_R, power down output stage N terminal--0 (48 fingers), active low, left channel (debug pin)480uA
 * [3] RESERVED
 * [2] CDC_PD_OUTP2I_R, power down output stage P terminal--2 (64 fingers), active low, left channel (debug pin)640uA
 * [1] CDC_PD_OUTP1I_R, power down output stage P terminal--1 (48 fingers), active low, left channel (debug pin)480uA
 * [0] CDC_PD_OUTP0I_R, power down output stage P terminal--0 (48 fingers), active low, left channel (debug pin)480uA
 * Default: 0x00
 */
#define CDC_REG763X_PD_OUT_L_DG         0x8C

/*
 * R8D: CDC_ICTL_STBF_DG
 * [7:6] RESERVED
 * [5:4] CDC_ICTL_STBF_R[1:0], POP noise buffer current option, right channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * [3:2] RESERVED
 * [1:0] CDC_ICTL_STBF_L[1:0], POP noise buffer current option, left channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I(debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_ICTL_STBF_DG        0x8D

/*
 * R8E: CDC_PD_CHARGE_DG
 * [7:4] CDC_PD_CHARGE_R[3:0], power down resistor divider, active low, right channel (debug pin)
 * [3:0] CDC_PD_CHARGE_L[3:0], power down resistor divider, active low, left channel (debug pin)
 * Default: 0xEE
 */
#define CDC_REG763X_PD_CHARGE_DG        0x8E

/*
 * R8F: CDC_PD_FD_DG
 * [7] RESERVED
 * [6] CDC_PD_CMFB_R, power down common mode feedback, change mode from full differential to single end;active high, right channel (debug pin)
 * [5] CDC_PD_FDNCS_R, power down NMOS current source, change mode from full differential to single end;active high, right channel (debug pin)
 * [4] CDC_PD_FDPCS_R, power down PMOS current source, change mode from full differential to single end;active high, right channel (debug pin)
 * [3] RESERVED
 * [2] CDC_PD_CMFB_L, power down common mode feedback, change mode from full differential to single end;active high, left channel (debug pin)
 * [1] CDC_PD_FDNCS_L, power down NMOS current source, change mode from full differential to single end;active high, left channel (debug pin)
 * [0] CDC_PD_FDPCS_L, power down PMOS current source, change mode from full differential to single end;active high, left channel (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_PD_FD_DG            0x8F

/*
 * R90: CDC_PWD_HFOUT_DG
 * [7] RESERVED
 * [6:4] CDC_PWD_HFOUT_R[2:0], power down pop noise buffer driver, active low, right channel (debug pin)bit0:10uA, bit1:20uA, bit2:40uA
 * [3] RESERVED
 * [2:0] CDC_PWD_HFOUT_L[2:0], power down pop noise buffer driver, active low, left channel (debug pin)bit0:10uA, bit1:20uA, bit2:40uA
 * Default: 0x00
 */
#define CDC_REG763X_PWD_HFOUT_DG        0x90

/*
 * R91: CDC_HP_ICTL_DG
 * [7:6] RESERVED
 * [5:4] CDC_HP_ICTL_R[1:0], current option of headphone PA, right channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * [3:2] RESERVED
 * [1:0] CDC_HP_ICTL_L[1:0], current option of headphone PA, left channel: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_HP_ICTL_DG          0x91

/*
 * R92: CDC_HPPD_OUT_DG
 * [7:6] RESERVED
 * [5] CDC_HPPD_OUT2I_R, power down headphone PA output driver,partly;active high (debug pin)
 * [4] CDC_HPPD_OUT1I_R, power down headphone PA output driver,partly;active high (debug pin)
 * [3:2] RESERVED
 * [1] CDC_HPPD_OUT2I_L, power down headphone PA output driver,partly;active high (debug pin)
 * [0] CDC_HPPD_OUT1I_L, power down headphone PA output driver,partly;active high (debug pin)
 * Default: 0x33
 */
#define CDC_REG763X_HPPD_OUT_DG         0x92

/*
 * R93: CDC_NMILLER_DG
 * [7:1] RESERVED
 * [0] CDC_NMILLER, control miller cap, 0=miller, 1=no miller (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_NMILLER_DG          0x93

/*
 * R94: CDC_CBOOST_DG
 * [7:5] RESERVED
 * [4] CDC_CBOOST_R , Boost miller cap in HPA_R. Active high.(debug pin)
 * [3:1] RESERVED
 * [0] CDC_CBOOST_L , Boost miller cap in HPA_L. Active high.(debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_CBOOST_DG           0x94

/*
 * R95: CDC_VGATE_SEL_DG
 * [7:2] RESERVED
 * [1:0] CDC_VGATE_SEL[1:0], CDC gate voltage option for switches.11:2.25V, 10:2.0V, 01:1.8V, 00:1.5V.(debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_VGATE_SEL_DG        0x95

/*
 * R96: CDC_VSUB_HIGH_DG
 * [7:5] RESERVED
 * [4] CDC_VSUB_HIGH_R , CDC substrate voltage option for HPA PMOS of right channel. 0: 7/12*avdd, 1: 8/12*avdd.(debug pin)
 * [3:1] RESERVED
 * [0] CDC_VSUB_HIGH_L , CDC substrate voltage option for HPA PMOS of left channel. 0: 7/12*avdd, 1: 8/12*avdd.(debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_VSUB_HIGH_DG        0x96

/*
 * R97: CDC_HPA_NATIVE_DG
 * [7:6] RESERVED
 * [5:4] CDC_HPA_NATIVE_R[1:0], CDC native/normal switch for HPA right channel.00:temp determined, 01:normal mode, 10: native mode, 11:forbidden!!!  (debug pin)
 * [3:2] RESERVED
 * [1:0] CDC_HPA_NATIVE_L[1:0], CDC native/normal switch for HPA left channel.00:temp determined, 01:normal mode, 10: native mode, 11:forbidden!!! (debug pin)
 * Default: 0x00
 */
#define CDC_REG763X_HPA_NATIVE_DG       0x97

/*
 * R98: CDC_LINEIN_AMP_PD_DG
 * [7:1] RESERVED
 * [0] CDC_LINEIN_AMP_PD, CDC line_in amplitude detect Power Down. 0:on, 1: power down.
 * Default: 0x01
 */
#define CDC_REG763X_LINEIN_AMP_PD_DG    0x98

/*
 * R99: CDC_LINEIN_AMP_VREF_DG
 * [7:2] RESERVED
 * [1:0] CDC_LINEIN_AMP_VREF[1:0], CDC VREF option for line_in amplitude detect.
 * Default: 0x00
 */
#define CDC_REG763X_LINEIN_AMP_VREF_DG  0x99

/*
 * R9A: CDC_HPAGAIN_SEP_DG
 * [7:1] RESERVED
 * [0] CDC_HPAGAIN_SEP, 1`b0: Use the registers at "CDC_HPVOL_4BIT"  address to control the headphone PA volume of left and right channels.
 *                     1`b1: Use the registers at "CDC_HPVOL_6BIT_L" adress and "CDC_HPVOL_6BIT_R" address to control the headphone PA volume of left and right channels separately.
 * Default: 0x00
 */
#define CDC_REG763X_HPAGAIN_SEP_DG      0x9A

/*
 * R9B: CDC_HPAGAIN_MODE_DG
 * [7:1] RESERVED
 * [0] CDC_HPAGAIN_MODE, 1`b0: Use {CDC_HPVOL_4BIT_R[3:0], 2`b0} to control the headphone PA volume of left and right channels.
 *                         1`b1: Use {CDC_HPVOL_4BIT_R[3:0], CDC_HPVOL_4BIT_L[1:0]} to control the headphone PA volume of left and right channels.
 * Default: 0x00
 */
#define CDC_REG763X_HPAGAIN_MODE_DG     0x9B

/*
 * R9C: CDC_HPVOLSEL_DG
 * [7:5] RESERVED
 * [4] CDC_HPVOLSEL_R , select the source of CDC_HPVOL_R[5:0].
 *                  0:connected directly to register;
 *                  1:update when CDC_HPVCH_R is switched;
 * [3:1] RESERVED
 * [0] CDC_HPVOLSEL_L , select the source of CDC_HPPD_L[5:0].
 *                  0:connected directly to register;
 *                  1:update when CDC_HPVCH_L is switched;
 * Default: 0x00
 */
#define CDC_REG763X_HPVOLSEL_DG         0x9C

/*
 * R9D: CDC_HPPDSEL_DG
 * [7:5] RESERVED
 * [4] CDC_HPPDSEL_R , select the source of CDC_HPPD_R.
 *                  0:connected directly to register;
 *                  1:update when CDC_HPVCH_R is switched;
 * [3:1] RESERVED
 * [0] CDC_HPPDSEL_L , select the source of CDC_HPPD_L[5:0].
 *                  0:connected directly to register;
 *                  1:update when CDC_HPVCH_L is switched;
 * Default: 0x00
 */
#define CDC_REG763X_HPPDSEL_DG          0x9D

/*
 * R9E: CDC_SETVCOM_DG
 * [7:5] RESERVED
 * [4] CDC_SETVCOM_R , 1:set n terminal of MIC_R channel is VCOM; 0:disable.
 * [3:1] RESERVED
 * [0] CDC_SETVCOM_L , 1:set n terminal of MIC_L channel is VCOM; 0:disable.
 * Default: 0x00
 */
#define CDC_REG763X_SETVCOM_DG          0x9E

/*
 * R9F: CDC_PULLDOWN_VCM_DG
 * [7:1] RESERVED
 * [0] CDC_PULLDOWN_VCM , pull down Vcom to 0, active low.
 * Default: 0x00
 */
#define CDC_REG763X_PULLDOWN_VCM_DG     0x9E

/*
 * RA0: CDC_HP_PULLDOWN_EN_DG
 * [7:5] RESERVED
 * [4] CDC_HP_PULLDOWN_EN_R , pull down to ground of headphone PA output, right channel.
 * [3:1] RESERVED
 * [0] CDC_HP_PULLDOWN_EN_L , pull down to ground of headphone PA output, left channel.
 * Default: 0x00
 */
#define CDC_REG763X_HP_PULLDOWN_EN_DG   0xA0

/*
 * RB7: SPORT_MODE
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
#define CDC_REG763X_SPORT_MODE                  0xC7


//I2CSLV  for  VA7632B_A0   v0.2;   note:I2C Register for "digital part":??C0H~EFH???,???digital????
/*
 * RC0: INT_STATUS
 * [7:6] RESERVED
 * [5] KeyON, ?????????(?????)?,KeyON??????,???????,???????????? ” ” ???1 b0,???????
 * ( ) KeyON   1 b0 
 *
 * [4] BAT_Low, When Battery voltage is lower than this threshold, BAT_Low will become high
 * [3] Pg, Indicating if charger input voltage is higher than battery voltage:
 *          "1" means charger input voltage is high enough for charging;
 *          "0" means charger input voltage is too low
 * [2] Stat, Charging flag: "1" indicating charging; "0" indicating no charging
 * [1] termb_inv, VD4~VD0 ready signal, active high.
 *          1'b0: VD4~VD0 is not ready;
 *          1'b1: VD4~VD0 is ready to be read by 9613i.
 * [0] CDC_LINE_DET, line in detect; 0=detect, 1=no detect
 * Default: N/A
 */
#define CDC_REG763X_INT_STATUS          0xC0

/*
 * RC1: INT_INV
 * [7:6] RESERVED
 * [5] INT_INV_KeyON, Inverse the interrupt input signal from register "KeyON", active high.
 * [4] INT_INV_BAT_Low, Inverse the interrupt input signal from register "BAT_Low", active high.
 * [3] INT_INV_Pg, Inverse the interrupt input signal from register "Pg", active high.
 * [2] INT_INV_Stat, Inverse the interrupt input signal from register "Stat", active high.
 * [1] INT_INV_termb_inv, Inverse the interrupt input signal from register "termb_inv", active high.
 * [0] INT_INV_LINE_DET, Inverse the interrupt input signal from register "CDC_LINE_DET", active high.
 * Default: 0x00
 */
#define CDC_REG763X_INT_INV             0xC1

/*
 * RC2: INT_MASK
 * [7:6] RESERVED
 * [5] INT_MASK_KeyON, Close the interrupt function from register "KeyON", active high.
 * [4] INT_MASK_BAT_Low, Close the interrupt input signal from register "BAT_Low", active high.
 * [3] INT_MASK_Pg, Close the interrupt input signal from register "Pg", active high.
 * [2] INT_MASK_Stat, Close the interrupt input signal from register "Stat", active high.
 * [1] INT_MASK_termb_inv, Close the interrupt input signal from register "termb_inv", active high.
 * [0] INT_MASK_LINE_DET, Close the interrupt input signal from register "CDC_LINE_DET", active high.
 * Default: 0x3F
 */
#define CDC_REG763X_INT_MASK                0xC2

/*
 * RC3: W_SEL_ADC
 * [7:2] RESERVED
 * [1:0] w_sel_adc[1:0] , Output signal width select of analog SDM ADC:
 *          2'b00-1bit ADC, 2'b01-2bit ADC,
 *          2'b10-3bit ADC, 2'b11-4bit ADC
 * Default: 0x00
 */
#define CDC_REG763X_W_SEL_ADC           0xC3

/*
 * RC4: DATA_RSTN
 * [7:2] RESERVED
 * [1] serd_rstn_adc, Reset signal for serial-data interface of ADC path (serd_tx_7633 module), active low.
 * [0] serd_rstn_dac, Reset signal for serial-data interface of DAC path (serd_rx_7633 module), active low.
 * Default: 0x00
 */
#define CDC_REG763X_DATA_RSTN           0xC4

/*
 * RC5: SERD_EN
 * [7] dac_fs_mode, dac sample rate selection.
 *          0: 48K,44.1K,24K,16k,8K
 *          1: 32K
 * [6] pga_gain_syn_mode, pga gain synchronization mode
 *          0: pga gain sync from internal generation logic
 *          1: pga gain sync from external PAD  "ADOUTL"
 * [5] adoutl_oen, output enable signal of bi-directional IO "ADOUTL", active low.
 * [4] resetn_oen, Output enable signal of bi-directional IO "RESETN", active low.
 * [3] gain_syn_adc_en, PGA gain synchronization for ADC path, active high.
 * [2] gain_syn_dac_en, PA gain synchronization for DAC path, active high.
 * [1] serd_en_adc, Enable serial-data interface mode in ADC path, active high.
 * [0] serd_en_dac, Enable serial-data interface mode in DAC path, active high.
 * Default: 0x10
 */
#define CDC_REG763X_SERD_EN             0xC5

/*
 * RC6: PGA_GAIN_CONF
 * [7:4] RESERVED
 * [3:2] pga_gain_dec[1:0], PGA gain adjust rate for decrease mode:
 *                  2'b00: 24kHz,    2'b01: 48kHz,   2'b10: 144kHz,   2'b11: 240kHz
 * [1:0] pga_gain_inc[1:0], PGA gain adjust rate for increase mode:
 *                  2'b00: 24kHz,    2'b01: 48kHz,   2'b10: 144kHz,   2'b11: 240kHz
 * Default: 0x03
 */
#define CDC_REG763X_PGA_GAIN_CONF       0xC6

/*
 * RC7: SPORT_MODE
 * [7:4] RESERVED
 * [3] sport_delay, Serial port delay control.
 *          0: no delay.     1: add half cycle's delay
 * [2] sport_mode, Serial port mode selection.
 *          0: i2c mode.    1: high speed serial port.
 * [1:0] RESERVED
 * Default: 0x00
 */
#define CDC_REG763X_PGA_SPORT_MODE      0xC7

/*
 * RB7: SPORT_MODE
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
                0: no delay.     1: add half cycle???s delay
 * [2] sport_mode, Serial port mode selection.
                0: i2c mode.    1: high speed serial port.
  * [1] rxfifo_reset, Rx FIFO reset, high active
  * [0] txfifo_reset, TX FIFO reset, high active

 * Default: 0x00
 */
#define CDC_REG763X_SPORT_MODE                  0xC7


/*
 * RC8: I2C_TIMEOUT_EN
 * [7:1] RESERVED
 * [0] i2c_timeout_en, Enable I2C Timeout function, active high.
 * Default: 0x00
 */
#define CDC_REG763X_I2C_TIMEOUT_EN      0xC8

/*
 * RC9: I2C_TIMEOUT_MAX
 * [7:0] i2c_timeout_max[7:0], The max value of the timeout counter is {i2c_timeout_max[7:0], 11'h7ff}.
 * Default: 0x04
 */
#define CDC_REG763X_I2C_TIMEOUT_MAX     0xC9



//xy: 20190610 end for new VA7632B_A0






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

#define CDC_DEFAULT_DCDC_DAC 		0x01
#define CDC_DEFAULT_DCDC_ADC 		0x01
#define CDC_DEFAULT_RESERVED		0
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
#define CDC_PWR_LDO5 		0	
#define CDC_PWR_DCDC 		1

#define CDC_DEFAULT_DCDC_DAC 		0x01
#define CDC_DEFAULT_DCDC_ADC 		0x01
#define CDC_DEFAULT_RESERVED		0
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

#if defined (HW_CDC_DEBUG)
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
#endif


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



void VCDC_Init ( void );
void VCDC_ConfigInput ( UINT8 Input );
void VCDC_ConfigOutput ( UINT8 Output, UINT8 Mode );
void VCDC_SetInputGain ( UINT8 GainVal );
void VCDC_SetOutputGain ( UINT8 GainVal );
void VCDC_MuteOutput ( void );
void VCDC_UnmuteOutput ( void );
void VCDC_MuteInput ( void );
void VCDC_UnmuteInput ( void );
VRET_VALUE VCDC_WriteReg ( UINT8 Reg, UINT8 Value );
VRET_VALUE VCDC_ReadReg ( UINT8 Reg, UINT8 *p_Value );
BOOL VCDC_I2CTest ( void );
void VCDC_VolShadow ( UINT8 DestReg, UINT8 DestVol, UINT16 DelayTime );
UINT8 VCDC_GetDACChan ( void );
//void VCDC_9611S_PopSuppression(UINT8 Output);
void VCDC_CloseMBias ( void );

void VCDC_7621_PA_Opne ( void );
void VCDC_7621_PA_Close ( void );

UINT8 VCDC_GetHPVOL_05DB ( SINT16 Gain );
UINT8 VCDC_GetHPVOL ( SINT16 Gain );

void VCDC_SetADCVol ( UINT8 AdcChn, UINT8 LVol, UINT8 RVol );

UINT8 VCDC_GetVerNo ( void );
UINT8 VCDC_GetGainMode ( void );

void VCDC_PDOUTConfigByGain ( SINT16 Gain );

#if VCODEC_HWGAINSYC_SUPPORTED
void VCDC_GainSyncSet ( UINT8	mode );
void VCDC_GainSync_SetADVOL( UINT8 AdcChn, UINT8 LVol, UINT8 RVol );
#endif

UINT8 VCDC_I2S_GetMode ( void );
//void VCDC_I2S_ConfigExCodec ( UINT8 ctrl );
UINT8 VCDC_ExCodecConfig ( UINT8 Mode, UINT8 Ctrl );
UINT8 VCDC_ExCodecCheck ( UINT8 Mode );

void VCDC_ConfigHW_CloseIIS(void);
void VCDC_HW_ReinitIIS(void);
void VCDC_HW_CloseIIS(void);

#endif /* _VCDC_H_ */


 /* _VCDC_H_ */



