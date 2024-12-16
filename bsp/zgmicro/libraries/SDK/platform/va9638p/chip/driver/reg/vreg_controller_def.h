#ifndef _VREG_CONTROLLER_DEF_H_
#define _VREG_CONTROLLER_DEF_H_

#define JAL_BASE_ADDR                   (0x40006000)
#define HAB_BASE_ADDR                   (0x40008000)

#define MODEM_BASE                      (0x40024000)
#define RFC_BASE                        (0x40028000)
#define BBLE_BASE                       (0x40034000)
#define BB_BASE                         (0x40034000)
#define AES_BASE                        (0x40034000)

/********************************************** 
 * MODEM Registers
 **********************************************/
#if MODEM_BASE

#define REG_MODEM_MODEM_CTRL                               (MODEM_BASE + 0x0)
#define REG_MODEM_MODEM_CTRL_BT4_enable                    ((((REG_MODEM_MODEM_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
MODEM_CTRL: 
[31:9]: 
[8]: 0: BT2.1 enable; 1: BT4.0 enable
[7:0]: 
*/

#define REG_MODEM_CIC_SHIFT_NUM                            (MODEM_BASE + 0xc)
/*
CIC_SHIFT_NUM: 
[31:4]: 
[3:0]: cic filter output scaling initial configure number.
*/

#define REG_MODEM_TRIGGER_THR                              (MODEM_BASE + 0x10)
/*
TRIGGER_THR: 
[31:20]: 
[19:16]: amplitude trigger calcultor scaling shift number when estimating the signal energy for trigger and AGC.
[15:9]: 
[8:0]: amplitude tirgger calcultor threshold ????RTL?????,???????;?????????????????,???????modem???????!!
*/

#define REG_MODEM_SYNC_FAIL_CTRL                           (MODEM_BASE + 0x14)
#define REG_MODEM_SYNC_FAIL_CTRL_sync_word_0_inv           ((((REG_MODEM_SYNC_FAIL_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_MODEM_SYNC_FAIL_CTRL_sync_fail_ctrl            ((((REG_MODEM_SYNC_FAIL_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SYNC_FAIL_CTRL: 
[31]: 1'b0: sync_word[0] from BB is used to compare in preamble_search and preamble_trailer_sync process; 1'b1: ~sync_word[0] from BB is used to compare in preamble_search and preamble_trailer_sync process;
[30:23]: 
[22:16]: The sync counter number to begin sync compare
[15:11]: 
[10:4]: The compare matched number of sync word.
[3:1]: 
[0]: 1'b0, go on with preamble timing point if sync word fail
1'b1, the state return to IDLE, if sync word fail
*/

#define REG_MODEM_SYNC_WINDOW_SIZE                         (MODEM_BASE + 0x28)
#define REG_MODEM_SYNC_WINDOW_SIZE_trailer_pos             ((((REG_MODEM_SYNC_WINDOW_SIZE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
SYNC_WINDOW_SIZE: 
[31:9]: 
[8]: trailer position using pre 4 sample(0) or post 4 sample(1)
[7]: 
[6:0]: The size of the sync window when preamble. ????RTL?????,???????;?????????????????,???????modem???????!!
*/

#define REG_MODEM_DAC_BIAS                                 (MODEM_BASE + 0x30)
#define REG_MODEM_DAC_BIAS_dac_bias_en                     ((((REG_MODEM_DAC_BIAS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
/*
DAC_BIAS: 
[31]: 
[30:28]: The tx amplitude scaling value, tx_re/im = tx_re/im - (tx_re/im>>tx_amp_scale); 0: 0db; 1: -6db; 2: -2.5db; 3: -1.6db; 4: -0.56db;5: ; 6: ;
[27:25]: 
[24]: dac bias add enable
[23:21]: 
[20:12]: dac bias imag level unsigned
[11:9]: 
[8:0]: dac bias real level unsigned
*/

#define REG_MODEM_TX_EXP                                   (MODEM_BASE + 0x34)
/*
TX_EXP: 
[31:16]: 
[15:8]: tx en expand 24m clk cycle to match gfsk end point.
[7:0]: tx en expand 24m clk cycle to match delay
*/

#define REG_MODEM_CORTRIG_THR01                            (MODEM_BASE + 0x38)
/*
CORTRIG_THR01: 
[31:26]: 
[25:16]: preamble seacrch cortrig threshold1
[15:10]: 
[9:0]: preamble seacrch cortrig threshold0
*/

#define REG_MODEM_CORTRIG_THR23                            (MODEM_BASE + 0x3c)
/*
CORTRIG_THR23: 
[31:26]: 
[25:16]: preamble seacrch cortrig threshold3
[15:10]: 
[9:0]: preamble seacrch cortrig threshold2
*/

#define REG_MODEM_GUARD_EDR_THR                            (MODEM_BASE + 0x40)
/*
GUARD_EDR_THR: 
[31:23]: 
[22:16]: approximate psk_sync time in demod
[15]: 
[14:8]: approximate guard time in demod
[7]: 
[6:0]: approximate edr time in demod
*/

#define REG_MODEM_TIMING_TRACK                             (MODEM_BASE + 0x44)
#define REG_MODEM_TIMING_TRACK_timing_track_en             ((((REG_MODEM_TIMING_TRACK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
TIMING_TRACK: 
[31:9]: 
[8]: timing offset track in EDR enable
[7:0]: timing offset track interval time.
*/

#define REG_MODEM_FREQ_OFFSET_TRA_PRE_W                    (MODEM_BASE + 0x48)
/*
FREQ_OFFSET_TRA_PRE_W: 
[31:27]: 
[26:16]: frequence offset preamble manual control value
[15:11]: 
[10:0]: frequence offset trailer manual control value
*/

#define REG_MODEM_FREQ_OFFSET_EDR_TRACK_W                  (MODEM_BASE + 0x4c)
/*
FREQ_OFFSET_EDR_TRACK_W: 
[31:27]: 
[26:16]: frequence offset track manual control value
[15:11]: 
[10:0]: frequence offset edr manual control value
*/

#define REG_MODEM_PLL_INTERVAL                             (MODEM_BASE + 0x50)
/*
PLL_INTERVAL: 
[31:13]: 
[12:8]: psk frequency offset track interval,edr_pll_interval+1
[7:5]: 
[4:0]: gfsk frequency offset track interval,bdr_pll_interval+1
*/

#define REG_MODEM_AGC_AMP_THR                              (MODEM_BASE + 0x54)
/*
AGC_AMP_THR: 
[31:28]: 
[27:16]: agc amplitude adjustment up threshold
[15:12]: 
[11:0]: agc amplitude adjustment low threshold
*/

#define REG_MODEM_AGC_STEP                                 (MODEM_BASE + 0x58)
/*
AGC_STEP: 
[31:12]: 
[11:8]: agc increase index step first long step,
[7:4]: agc increase index step 2nd step,
[3:0]: agc decrease index step step,
*/

#define REG_MODEM_AGC_ADJ_RANGE                            (MODEM_BASE + 0x5c)
/*
AGC_ADJ_RANGE: 
[31:30]: 
[29:28]: agc cnt initial value, which influence agc_adj_addr The value of 0, 1, 2 can be allowed
[27:24]: agc_adj_addr intervel
[23:20]: 
[19:12]: agc adjust start point
[11:8]: agc adjust times ????RTL?????,???????;?????????????????,???????modem???????!!
[7:4]: agc adjust interval between adjecent times
[3:0]: agc initial step times.
*/

#define REG_MODEM_TRIG_CAL_START                           (MODEM_BASE + 0x60)
/*
TRIG_CAL_START: 
[31:7]: 
[6:0]: energy triger start to avoid power up abnormal amp pulse ????RTL?????,???????;?????????????????,???????modem???????!!
*/

#define REG_MODEM_FOC_CTRL                                 (MODEM_BASE + 0x64)
#define REG_MODEM_FOC_CTRL_psk_foc_en                      ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_MODEM_FOC_CTRL_gfsk_foc_en                     ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_MODEM_FOC_CTRL_to_track_en                     ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_MODEM_FOC_CTRL_fo_cor_en                       ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_MODEM_FOC_CTRL_fo_preamble_en                  ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_MODEM_FOC_CTRL_fo_trailer_en                   ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_FOC_CTRL_fo_edr_en                       ((((REG_MODEM_FOC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FOC_CTRL: 
[31:25]: 
[24]: PSK frequence offset correct enable
[23:21]: 
[20]: GFSK frequence offset correct enable
[19:17]: 
[16]: EDR timing offset track enable
[15:13]: 
[12]: frequence offset correct enable
[11:9]: 
[8]: preamble bits frequency offset correct enable
[7:5]: 
[4]: trailer bits frequency offset correct enable
[3:1]: 
[0]: edr sync sequence frequency offset correct enable
*/

#define REG_MODEM_TIMING_POINT_CTRL                        (MODEM_BASE + 0x68)
#define REG_MODEM_TIMING_POINT_CTRL_trailer_timing_disable ((((REG_MODEM_TIMING_POINT_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
/*
TIMING_POINT_CTRL: 
[31:29]: 
[28]: trailer timing point calculation disable
[27:24]: trailer timing point calculation offset
[23:16]: preamble timing point calculation offset
[15:8]: edr timing point calculation offset
[7:0]: edr_ref addr correction if guard time changed.
*/

#define REG_MODEM_AGC_CTRL                                 (MODEM_BASE + 0x6c)
#define REG_MODEM_AGC_CTRL_cic_agc_cor_en                  ((((REG_MODEM_AGC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_MODEM_AGC_CTRL_agc_auto_en                     ((((REG_MODEM_AGC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_MODEM_AGC_CTRL_agc_init_en                     ((((REG_MODEM_AGC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
/*
AGC_CTRL: 
[31:29]: 
[28]: agc affect cic shift number enable
[27:25]: 
[24]: agc auto enable
[23:21]: 
[20]: agc init gain enable
[19:18]: 
[17:12]: agc initial index lut value
[11:0]: 
*/

#define REG_MODEM_INT_SETMASK                              (MODEM_BASE + 0x84)
#define REG_MODEM_INT_SETMASK_gfsk_dem_en_fall             ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((13) << 2))
#define REG_MODEM_INT_SETMASK_gfsk_dem_en_rise             ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_MODEM_INT_SETMASK_psk_dem_en_rise              ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_MODEM_INT_SETMASK_edr_sync_en_fall             ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_MODEM_INT_SETMASK_sync_word_pass_rise          ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_MODEM_INT_SETMASK_sync_word_finish_rise        ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_MODEM_INT_SETMASK_trailer_finish_rise          ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_MODEM_INT_SETMASK_preamble_finish_rise         ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_MODEM_INT_SETMASK_pre_search_en_fall           ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_MODEM_INT_SETMASK_pre_search_en_rise           ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_INT_SETMASK_trig_on_flag_rise            ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_INT_SETMASK_rx_en_rise                   ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_INT_SETMASK_rx_en_fall                   ((((REG_MODEM_INT_SETMASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INT_SETMASK: 
[31:14]: 
[13]: gfsk_dem_en_fall set mask
[12]: gfsk_dem_en_rise set mask
[11]: psk_dem_en_rise set mask
[10]: edr_sync_en_rise set mask
[9]: sync_word_pass_rise set mask
[8]: sync_word_finish_rise set mask
[7]: trailer_finish_rise set mask
[6]: preamble_finish_rise set mask
[5]: pre_search_en_fall set mask
[4]: pre_search_en_rise set mask
[3]: trig_on_flag_rise set mask
[2]: this bit is RW, but has no effect on the function
[1]: rx_en_rise set mask
[0]: rx_en_fall set mask
*/

#define REG_MODEM_INT_SRCPND                               (MODEM_BASE + 0x88)
#define REG_MODEM_INT_SRCPND_gfsk_dem_en_fall              ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((13) << 2))
#define REG_MODEM_INT_SRCPND_gfsk_dem_en_rise              ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_MODEM_INT_SRCPND_psk_dem_en_rise               ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_MODEM_INT_SRCPND_edr_sync_en_fall              ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_MODEM_INT_SRCPND_sync_word_pass_rise           ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_MODEM_INT_SRCPND_sync_word_finish_rise         ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_MODEM_INT_SRCPND_trailer_finish_rise           ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_MODEM_INT_SRCPND_preamble_finish_rise          ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_MODEM_INT_SRCPND_pre_search_en_fall            ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_MODEM_INT_SRCPND_pre_search_en_rise            ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_INT_SRCPND_trig_on_flag_rise             ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_INT_SRCPND_rx_en_rise                    ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_INT_SRCPND_rx_en_fall                    ((((REG_MODEM_INT_SRCPND) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INT_SRCPND: 
[31:14]: 
[13]: gfsk_dem_en_fall srcpnd
[12]: gfsk_dem_en_rise srcpnd
[11]: psk_dem_en_rise srcpnd
[10]: edr_sync_en_rise srcpnd
[9]: sync_word_pass_rise srcpnd
[8]: sync_word_finish_rise srcpnd
[7]: trailer_finish_rise srcpnd
[6]: preamble_finish_rise srcpnd
[5]: pre_search_en_fall srcpnd
[4]: pre_search_en_rise srcpnd
[3]: trig_on_flag_rise srcpnd
[2]: this bit is RW, but has no effect on the function
[1]: rx_en_rise srcpnd
[0]: rx_en_fall srcpnd
*/

#define REG_MODEM_INT_SRCPND_READ                          (MODEM_BASE + 0x9c)
#define REG_MODEM_INT_SRCPND_READ_gfsk_dem_en_fall         ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((13) << 2))
#define REG_MODEM_INT_SRCPND_READ_gfsk_dem_en_rise         ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_MODEM_INT_SRCPND_READ_psk_dem_en_rise          ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_MODEM_INT_SRCPND_READ_edr_sync_en_fall         ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_MODEM_INT_SRCPND_READ_sync_word_pass_rise      ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_MODEM_INT_SRCPND_READ_sync_word_finish_rise    ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_MODEM_INT_SRCPND_READ_trailer_finish_rise      ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_MODEM_INT_SRCPND_READ_preamble_finish_rise     ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_MODEM_INT_SRCPND_READ_pre_search_en_fall       ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_MODEM_INT_SRCPND_READ_pre_search_en_rise       ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_INT_SRCPND_READ_trig_on_flag_rise        ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_INT_SRCPND_READ_rx_en_rise               ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_INT_SRCPND_READ_rx_en_fall               ((((REG_MODEM_INT_SRCPND_READ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INT_SRCPND_READ: 
[31:14]: 
[13]: gfsk_dem_en_fall srcpndread
[12]: gfsk_dem_en_rise srcpnd read
[11]: psk_dem_en_rise srcpnd read
[10]: edr_sync_en_rise srcpndread
[9]: sync_word_pass_rise srcpnd read
[8]: sync_word_finish_rise srcpnd read
[7]: trailer_finish_rise srcpnd read
[6]: preamble_finish_rise srcpnd read
[5]: pre_search_en_fall srcpnd read
[4]: pre_search_en_rise srcpnd read
[3]: trig_on_flag_rise srcpnd read
[2]: this bit is RW, but has no effect on the function
[1]: rx_en_rise srcpnd read
[0]: rx_en_fall mask read
*/

#define REG_MODEM_TIMING_POINT_R                           (MODEM_BASE + 0xa0)
/*
TIMING_POINT_R: 
[31:16]: 
[15:8]: edr sync timing point read
[7:0]: preamble trailer sync timing point read
*/

#define REG_MODEM_FREQ_OFFSET_R                            (MODEM_BASE + 0xa4)
/*
FREQ_OFFSET_R: 
[31:22]: freq offset edr read
[21:11]: freq offset preamble read
[10:0]: freq offset preamble read
*/

#define REG_MODEM_AGC_INDEX                                (MODEM_BASE + 0xa8)
/*
AGC_INDEX: 
[31:5]: 
[4:0]: agc index read after adjust 0-27
*/

#define REG_MODEM_TRIG_PSIG_AGC                            (MODEM_BASE + 0xac)
/*
TRIG_PSIG_AGC: 
[31:12]: 
[11:0]: amplitude of the received signal
*/

#define REG_MODEM_TRIG_ENG_THRD                            (MODEM_BASE + 0xb0)
/*
TRIG_ENG_THRD: 
[31:12]: 
[11:0]: energy trig threshold value
*/

#define REG_MODEM_RF_POLAR_CONF                            (MODEM_BASE + 0xb4)
#define REG_MODEM_RF_POLAR_CONF_envlp_sel                  ((((REG_MODEM_RF_POLAR_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((26) << 2))
#define REG_MODEM_RF_POLAR_CONF_order_sdm                  ((((REG_MODEM_RF_POLAR_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_MODEM_RF_POLAR_CONF_En_sdm                     ((((REG_MODEM_RF_POLAR_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_RF_POLAR_CONF_En_polar                   ((((REG_MODEM_RF_POLAR_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
RF_POLAR_CONF: 
[31:27]: 
[26]: 0: envlp from 8x interpolator; 1: envlp from reg
[25:16]: SBI test envelope signal
[15:14]: 
[13:8]: adjust envelope path delay
[7]: 
[6:4]: Pa gain control, used to control PA on/off
 3'b000: full scale
 3'b001: -6dB
 3'b010: -12dB
 3'b011: -18dB
 3'b100: -24dB
 3'b101: -30dB
 3'b110: -36dB
 3'b111: full scale
[3]: 
[2]: sdm order;
[1]: Enable sdm; when En_polar is 1, then sdm must 1, so we can just set it to 1
[0]: Enable envelope generation
*/

#define REG_MODEM_ADC_BIT_SEL                              (MODEM_BASE + 0xb8)
#define REG_MODEM_ADC_BIT_SEL_adc_bit_sel                  ((((REG_MODEM_ADC_BIT_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
ADC_BIT_SEL: 
[31:1]: 
[0]: 0: for the original third-oder 1 bit SD ADC; 1: for the new second-order 2 bits SD ADC
*/

#define REG_MODEM_TXENV_EXTRA                              (MODEM_BASE + 0xbc)
/*
TXENV_EXTRA: 
[31:16]: 
[15:0]: tx envelope extra register
*/

#define REG_MODEM_POLAR_ENV_CONTRL                         (MODEM_BASE + 0xc0)
/*
POLAR_ENV_CONTRL: 
[31:9]: 
[8:0]: tx envelope value for gfsk modulation(bb21 bdr and ble packet).unsigned.
*/

#define REG_MODEM_POLAR_PHASE_CONF                         (MODEM_BASE + 0xc4)
#define REG_MODEM_POLAR_PHASE_CONF_polar_phase_en          ((((REG_MODEM_POLAR_PHASE_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
POLAR_PHASE_CONF: 
[31:1]: 
[0]: Enable polar phase path:
0:polar phase path disable
1: polar phase path enable
*/

#define REG_MODEM_POLAR_PHASE_CONTRL                       (MODEM_BASE + 0xc8)
#define REG_MODEM_POLAR_PHASE_CONTRL_rgf_iqpath_sel        ((((REG_MODEM_POLAR_PHASE_CONTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
/*
POLAR_PHASE_CONTRL: 
[31:29]: Tx phase path delay control signal:
0: delay 0 24MHz cycles.
1: delay 1 24MHz cycles.
…
5: delay 5 24MHz cycles.
Others:delay 0 24MHz cycles.
[28]: the phse path selection signal:
0: I,q signals are came from modem hardware;
1: I,q signals are came from APB(for debug);
[27:25]: 
[24:16]: APB path q signal. Signed. For debug.
[15:9]: 
[8:0]: APB path I signsl. Singed. For debug.
*/

#define REG_MODEM_POLAR_ENV_SCALEBIAS                      (MODEM_BASE + 0xcc)
/*
POLAR_ENV_SCALEBIAS: 
[31:23]: 
[22:16]: envelope bias value. Add dc offset value for envelpoe after scale.
Range: 0-127.
[15:4]: 
[3:0]: scale envelope before dc offset. the value range:0-8 and related scale range: 8/8 -16/8.
4'd0: scaled value is 8/8 * original value
4'd1: scaled value is 9/8 * original value
4'd2: scaled value is 10/8 * original value
…
4'd8: scaled value is 16/8 * original value
other value: scaled value is 8/8 * original value



*/

#define REG_MODEM_FIR_LPF_COF01                            (MODEM_BASE + 0xe0)
/*
FIR_LPF_COF01: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_FIR_LPF_COF23                            (MODEM_BASE + 0xe4)
/*
FIR_LPF_COF23: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_FIR_LPF_COF45                            (MODEM_BASE + 0xe8)
/*
FIR_LPF_COF45: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_FIR_LPF_COF67                            (MODEM_BASE + 0xec)
/*
FIR_LPF_COF67: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_FIR_LPF_COF89                            (MODEM_BASE + 0xf0)
/*
FIR_LPF_COF89: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_FIR_LPF_COF1011                          (MODEM_BASE + 0xf4)
/*
FIR_LPF_COF1011: 
[31:26]: 
[25:16]: demoulator fir lpf coefficent
[15:10]: 
[9:0]: demoulator fir lpf coefficent
*/

#define REG_MODEM_NEW_MODEM_CONF                           (MODEM_BASE + 0x100)
#define REG_MODEM_NEW_MODEM_CONF_New_Preamble_Sync         ((((REG_MODEM_NEW_MODEM_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_NEW_MODEM_CONF_GFSK_Timing_Track         ((((REG_MODEM_NEW_MODEM_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_MODEM_NEW_MODEM_CONF_Improved_DPSK_Demod       ((((REG_MODEM_NEW_MODEM_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_NEW_MODEM_CONF_Improved_GFSK_Demod       ((((REG_MODEM_NEW_MODEM_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
NEW_MODEM_CONF: 
[31:4]: 
[3]: 
[2]: 
[1]: 
[0]: 
*/

#define REG_MODEM_GFSK_COMP_ANGLE                          (MODEM_BASE + 0x104)
/*
GFSK_COMP_ANGLE: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_PSK_MAPPER_INDEX                         (MODEM_BASE + 0x108)
/*
PSK_MAPPER_INDEX: 
[31:11]: 
[10:8]: 
[7]: 
[6:4]: 
[3]: 
[2:0]: 
*/

#define REG_MODEM_PREAMBLE_BIT_NUM                         (MODEM_BASE + 0x10c)
/*
PREAMBLE_BIT_NUM: 
[31:4]: 
[3:0]: 
*/

#define REG_MODEM_NOISE_SHIFT_BIT_NUM                      (MODEM_BASE + 0x110)
/*
NOISE_SHIFT_BIT_NUM: 
[31:4]: 
[3:0]: 
*/

#define REG_MODEM_PRB_TIM_AMPLEN_THRE                      (MODEM_BASE + 0x114)
/*
PRB_TIM_AMPLEN_THRE: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_PRB_TIM_AMPLEF_THRE                      (MODEM_BASE + 0x118)
/*
PRB_TIM_AMPLEF_THRE: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_PRB_TIM_AMPLEB_THRE                      (MODEM_BASE + 0x11c)
/*
PRB_TIM_AMPLEB_THRE: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_PRB_TIM_ANGLE_THRE                       (MODEM_BASE + 0x120)
/*
PRB_TIM_ANGLE_THRE: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_IQ_BALANCE_PRAMETER                      (MODEM_BASE + 0x144)
/*
IQ_BALANCE_PRAMETER: 
[31:16]: 
[15:0]: 
*/

#define REG_MODEM_AGC_INDEX_DEFAULT                        (MODEM_BASE + 0x148)
/*
AGC_INDEX_DEFAULT: 
[31:5]: 
[4:0]: agc index default value adjust 0-27
*/

#define REG_MODEM_TRIG_AGC_INDEX_DEFAULT                   (MODEM_BASE + 0x14c)
#define REG_MODEM_TRIG_AGC_INDEX_DEFAULT_trig_agc_index_default ((((REG_MODEM_TRIG_AGC_INDEX_DEFAULT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
TRIG_AGC_INDEX_DEFAULT: 
[31:1]: 
[0]: 
*/

#define REG_MODEM_OLD_PREA_OF_NEW_PREAMBLE                 (MODEM_BASE + 0x150)
#define REG_MODEM_OLD_PREA_OF_NEW_PREAMBLE_old_prea_of_new_preamble ((((REG_MODEM_OLD_PREA_OF_NEW_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
OLD_PREA_OF_NEW_PREAMBLE: 
[31:1]: 
[0]: old preamble algorithm in the new preamble algorithm
*/

#define REG_MODEM_ENERGE_TRIG_NUMBER                       (MODEM_BASE + 0x154)
/*
ENERGE_TRIG_NUMBER: 
[31:4]: 
[3:0]: old algorithm is 5, new algorithm is 8
*/

#define REG_MODEM_TIMING_OFFSET_ADDR_DEFAULT               (MODEM_BASE + 0x158)
#define REG_MODEM_TIMING_OFFSET_ADDR_DEFAULT_timing_offset_addr_default_sel ((((REG_MODEM_TIMING_OFFSET_ADDR_DEFAULT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
TIMING_OFFSET_ADDR_DEFAULT: 
[31:9]: 
[8]: 1: addr default is 31, 0: addr default is 30
[7:5]: 
[4:0]: old rtl is 30, new rtl is 31 (temporily not used)
*/

#define REG_MODEM_SYNC_WINDOW_EXTEND_INDEX                 (MODEM_BASE + 0x15c)
/*
SYNC_WINDOW_EXTEND_INDEX: 
[31:4]: 
[3:0]: sync_window_extend_index
*/

#define REG_MODEM_TRACK_CNT_START                          (MODEM_BASE + 0x160)
/*
TRACK_CNT_START: 
[31:5]: 
[4:0]: track_cnt_start
*/

#define REG_MODEM_COMPENSATED_ANGLE_GFSK_CFO_TRACK         (MODEM_BASE + 0x168)
/*
COMPENSATED_ANGLE_GFSK_CFO_TRACK: 
[31:12]: 
[11:0]: 
*/

#define REG_MODEM_PRBTRIG_NUM                              (MODEM_BASE + 0x16c)
/*
PRBTRIG_NUM: 
[31:4]: 
[3:0]: 
*/

#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE                   (MODEM_BASE + 0x170)
#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE_sel_point_find    ((((REG_MODEM_NEW_MODEM_SEL_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE_trailer_pos_disable ((((REG_MODEM_NEW_MODEM_SEL_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE_new_trailer_sync  ((((REG_MODEM_NEW_MODEM_SEL_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE_sel_freq_im_add   ((((REG_MODEM_NEW_MODEM_SEL_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PREAMBLE_trig_sync_sample_sel ((((REG_MODEM_NEW_MODEM_SEL_PREAMBLE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
/*
NEW_MODEM_SEL_PREAMBLE: 
[31:7]: 
[6]: 0: old rtl, not a pulse
1: new rtl, it is a pulse
[5]: 0: enable trailer pos
1: disable trailer pos
old rtl is 0
[4]: 0: old trailer sync
1: new trailer sync
[3]: 0: freq_offset_im - im
1: freq_offset_im + im(same as C code)
[2]: 0: trig_sync_sample + n*sample rate
1: trig_sync_sample - n*sample rate(same as c code)
[1:0]: 0: syn window
1: sync window * 2
2: sync window * 2 + 1
*/

#define REG_MODEM_NEW_MODEM_SEL_GFSK                       (MODEM_BASE + 0x174)
#define REG_MODEM_NEW_MODEM_SEL_GFSK_new_gfsk_foc_track_sel ((((REG_MODEM_NEW_MODEM_SEL_GFSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_NEW_MODEM_SEL_GFSK_gfsk_bit_equal0_sel   ((((REG_MODEM_NEW_MODEM_SEL_GFSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
NEW_MODEM_SEL_GFSK: 
[31:2]: 
[1]: 0:old rtl gfsk foc
1:new rtl gfsk foc
[0]: 0: gfsk bit equal0 don’t care
1: gfsk bit equal0 process as C code
*/

#define REG_MODEM_NEW_MODEM_SEL_PSK                        (MODEM_BASE + 0x178)
#define REG_MODEM_NEW_MODEM_SEL_PSK_psk_rotation_shift_bit_sel ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PSK_psk_new_rotation       ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PSK_psk_addr_addorminus8_foc ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PSK_psk_addr_addorminus8   ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PSK_new_edr_notadd_80      ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_NEW_MODEM_SEL_PSK_new_edr_addr           ((((REG_MODEM_NEW_MODEM_SEL_PSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
NEW_MODEM_SEL_PSK: 
[31:6]: 
[5]: 0: 10 bit(old rtl)
1: 8 bit(new rtl)
[4]: 0: old rtl rotation
1: new rtl rotation
[3]: 0: addr (for foc function)
1: addr +8
[2]: 0: addr (for conjuction mul function)
1: addr +8
[1]: 0: old rtl + 80 
1: new rtl
[0]: 0: old rtl
1: new rtl
*/

#define REG_MODEM_TIMING_POINT_SHIFT                       (MODEM_BASE + 0x17c)
/*
TIMING_POINT_SHIFT: 
[31:8]: 
[7:0]: 
*/

#define REG_MODEM_AGC_ADDR_FINE_TUNE                       (MODEM_BASE + 0x190)
/*
AGC_ADDR_FINE_TUNE: 
[31:7]: 
[6:0]: 
*/

#define REG_MODEM_FIR_IQ_CONTROL                           (MODEM_BASE + 0x194)
#define REG_MODEM_FIR_IQ_CONTROL_fir_round_method          ((((REG_MODEM_FIR_IQ_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_FIR_IQ_CONTROL_IQ_balance_enable         ((((REG_MODEM_FIR_IQ_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FIR_IQ_CONTROL: 
[31:2]: 
[1]: 0: use old method
1: use c code method
[0]: 0: disable
1: enable
*/

#define REG_MODEM_AGC_PARA_CONTROL                         (MODEM_BASE + 0x198)
#define REG_MODEM_AGC_PARA_CONTROL_trig_en_start_sel       ((((REG_MODEM_AGC_PARA_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_MODEM_AGC_PARA_CONTROL_new_trig_psig_comp_sel  ((((REG_MODEM_AGC_PARA_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_MODEM_AGC_PARA_CONTROL_agc_start_sel           ((((REG_MODEM_AGC_PARA_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_MODEM_AGC_PARA_CONTROL_agc_addr_sel            ((((REG_MODEM_AGC_PARA_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_MODEM_AGC_PARA_CONTROL_agc_index_default_conf_sel ((((REG_MODEM_AGC_PARA_CONTROL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AGC_PARA_CONTROL: 
[31:5]: 
[4]: 0: not add 3 delay(old rtl code)
1: add 3 delay(same as c code)
[3]: 0: old rtl Trig_Eng>RxFE_Status.Trig_Threshold
1: new rtl Trig_Eng>=RxFE_Status.Trig_Threshold(same as C)
[2]: 0:old agc
1:new agc process
[1]: 0:old rtl addr
1:new rtl addr
[0]: 0:default is 7 (old algorithm)
1:default is 5 (new algorithm)
*/

#define REG_MODEM_OLD_PRE_IN_NEW_PRE                       (MODEM_BASE + 0x19c)
#define REG_MODEM_OLD_PRE_IN_NEW_PRE_old_pre_in_new_pre    ((((REG_MODEM_OLD_PRE_IN_NEW_PRE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
OLD_PRE_IN_NEW_PRE: 
[31:1]: 31'b0
[0]: 0: old pre
1: old pre in new pre
*/

#define REG_MODEM_EDR_SYNC_DELAY_US                        (MODEM_BASE + 0x1a0)
/*
EDR_SYNC_DELAY_US: 
[31:4]: 28'b0
[3:0]: delay 8 us
*/

#define REG_MODEM_IMPROVED_PSK_ACTIVE_SAMPLE               (MODEM_BASE + 0x1a4)
/*
IMPROVED_PSK_ACTIVE_SAMPLE: 
[31:4]: 28'b0
[3:0]: from number 9 sample, if from number 10 sample, then set it to 4'd1,….
*/

#define REG_MODEM_UPDATE_CNT_START_TGT                     (MODEM_BASE + 0x1a8)
/*
UPDATE_CNT_START_TGT: 
[31:8]: 
[7:0]: 
*/

#define REG_MODEM_PSK_PHASE_EN                             (MODEM_BASE + 0x1ac)
/*
PSK_PHASE_EN: 
[31:3]: 
[2:0]: 
*/

#define REG_MODEM_PSK_ROTATION_SHIFT_BIT                   (MODEM_BASE + 0x1b0)
/*
PSK_ROTATION_SHIFT_BIT: 
[31:8]: 
[7:0]: no use
*/

#define REG_MODEM_GFSK_PHASE_EN                            (MODEM_BASE + 0x1b4)
/*
GFSK_PHASE_EN: 
[31:2]: 
[1:0]: 
*/

#define REG_MODEM_TX_DIRECT_MDLT_EN                        (MODEM_BASE + 0x200)
#define REG_MODEM_TX_DIRECT_MDLT_EN_rgf_dirmdlt_en         ((((REG_MODEM_TX_DIRECT_MDLT_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
TX_DIRECT_MDLT_EN: 
[31:1]: 
[0]: direct modulator enable signal.
0: direct modulator disabled
1: idrec modulator enabled
*/

#define REG_MODEM_OSC_CLOCK_FRQ                            (MODEM_BASE + 0x204)
#define REG_MODEM_OSC_CLOCK_FRQ_rgf_osc_16m                ((((REG_MODEM_OSC_CLOCK_FRQ) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
OSC_CLOCK_FRQ: 
[31:1]: 
[0]: oscillator clock frequency indication signal.
0: 26MHz
1: 16MHz
*/

#define REG_MODEM_TX_FRQ_DLY_CTRL                          (MODEM_BASE + 0x208)
/*
TX_FRQ_DLY_CTRL: 
[31:20]: 
[19:16]: the fact "s" of expression "frq1 * 2^(s-3) * 1/cal".
The value range of s:0-15.
[15:11]: 
[10:8]: delay control for frq2 path.
0: delay 0 cycles of oscillator clock
1: delay 1 cycles of oscillator clock
2: delay 2 cycles of oscillator clock
3: delay 3 cycles of oscillator clock
[7:3]: 
[2:0]: delay control for frq1 path.
0: delay 0 cycles of oscillator clock
1: delay 1 cycles of oscillator clock
2: delay 2 cycles of oscillator clock
3: delay 3 cycles of oscillator clock
*/

#define REG_MODEM_CAL_CTRL                                 (MODEM_BASE + 0x20c)
#define REG_MODEM_CAL_CTRL_rgf_cal_apbsel                  ((((REG_MODEM_CAL_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
/*
CAL_CTRL: 
[31:17]: 
[16]: apb path select signal of "cal".
0: select the "cal" from pll_dig module
1: select the "cal' from APB path
[15:9]: 
[8:0]: the fact "cal" of expression "frq1 * 2^(s-3) * 1/cal".
The value range of cal:[1 2^9-1].
*/

#define REG_MODEM_GFSK32_11_WAVE01                         (MODEM_BASE + 0x210)
/*
GFSK32_11_WAVE01: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 1.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 0.
*/

#define REG_MODEM_GFSK32_11_WAVE23                         (MODEM_BASE + 0x214)
/*
GFSK32_11_WAVE23: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 3.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 2.
*/

#define REG_MODEM_GFSK32_11_WAVE45                         (MODEM_BASE + 0x218)
/*
GFSK32_11_WAVE45: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 5.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 4.
*/

#define REG_MODEM_GFSK32_11_WAVE67                         (MODEM_BASE + 0x21c)
/*
GFSK32_11_WAVE67: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 7.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 6.
*/

#define REG_MODEM_GFSK32_11_WAVE89                         (MODEM_BASE + 0x220)
/*
GFSK32_11_WAVE89: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 9.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 8.
*/

#define REG_MODEM_GFSK32_11_WAVE1011                       (MODEM_BASE + 0x224)
/*
GFSK32_11_WAVE1011: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 11.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 10.
*/

#define REG_MODEM_GFSK32_11_WAVE1213                       (MODEM_BASE + 0x228)
/*
GFSK32_11_WAVE1213: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 13.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 12.
*/

#define REG_MODEM_GFSK32_11_WAVE1415                       (MODEM_BASE + 0x22c)
/*
GFSK32_11_WAVE1415: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 15.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 14.
*/

#define REG_MODEM_GFSK32_11_WAVE1617                       (MODEM_BASE + 0x230)
/*
GFSK32_11_WAVE1617: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 17.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 16.
*/

#define REG_MODEM_GFSK32_11_WAVE1819                       (MODEM_BASE + 0x234)
/*
GFSK32_11_WAVE1819: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 19.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 18.
*/

#define REG_MODEM_GFSK32_11_WAVE2021                       (MODEM_BASE + 0x238)
/*
GFSK32_11_WAVE2021: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 21.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 20.
*/

#define REG_MODEM_GFSK32_11_WAVE2223                       (MODEM_BASE + 0x23c)
/*
GFSK32_11_WAVE2223: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 23.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 22.
*/

#define REG_MODEM_GFSK32_11_WAVE2425                       (MODEM_BASE + 0x240)
/*
GFSK32_11_WAVE2425: 
[31:25]: 
[24:16]: the wave for gfsk32_11. index is 25.
[15:9]: 
[8:0]: the wave for gfsk32_11. index is 24.
*/

#define REG_MODEM_GFSK32_10_WAVE01                         (MODEM_BASE + 0x250)
/*
GFSK32_10_WAVE01: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 1.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 0.
*/

#define REG_MODEM_GFSK32_10_WAVE23                         (MODEM_BASE + 0x254)
/*
GFSK32_10_WAVE23: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 3.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 2.
*/

#define REG_MODEM_GFSK32_10_WAVE45                         (MODEM_BASE + 0x258)
/*
GFSK32_10_WAVE45: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 5.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 4.
*/

#define REG_MODEM_GFSK32_10_WAVE67                         (MODEM_BASE + 0x25c)
/*
GFSK32_10_WAVE67: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 7.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 6.
*/

#define REG_MODEM_GFSK32_10_WAVE89                         (MODEM_BASE + 0x260)
/*
GFSK32_10_WAVE89: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 9.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 8.
*/

#define REG_MODEM_GFSK32_10_WAVE1011                       (MODEM_BASE + 0x264)
/*
GFSK32_10_WAVE1011: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 11.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 10.
*/

#define REG_MODEM_GFSK32_10_WAVE1213                       (MODEM_BASE + 0x268)
/*
GFSK32_10_WAVE1213: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 13.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 12.
*/

#define REG_MODEM_GFSK32_10_WAVE1415                       (MODEM_BASE + 0x26c)
/*
GFSK32_10_WAVE1415: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 15.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 14.
*/

#define REG_MODEM_GFSK32_10_WAVE1617                       (MODEM_BASE + 0x270)
/*
GFSK32_10_WAVE1617: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 17.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 16.
*/

#define REG_MODEM_GFSK32_10_WAVE1819                       (MODEM_BASE + 0x274)
/*
GFSK32_10_WAVE1819: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 19.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 18.
*/

#define REG_MODEM_GFSK32_10_WAVE2021                       (MODEM_BASE + 0x278)
/*
GFSK32_10_WAVE2021: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 21.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 20.
*/

#define REG_MODEM_GFSK32_10_WAVE2223                       (MODEM_BASE + 0x27c)
/*
GFSK32_10_WAVE2223: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 23.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 22.
*/

#define REG_MODEM_GFSK32_10_WAVE2425                       (MODEM_BASE + 0x280)
/*
GFSK32_10_WAVE2425: 
[31:25]: 
[24:16]: the wave for gfsk32_10. index is 25.
[15:9]: 
[8:0]: the wave for gfsk32_10. index is 24.
*/

#define REG_MODEM_GFSK50_11_WAVE01                         (MODEM_BASE + 0x290)
/*
GFSK50_11_WAVE01: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 1.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 0.
*/

#define REG_MODEM_GFSK50_11_WAVE23                         (MODEM_BASE + 0x294)
/*
GFSK50_11_WAVE23: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 3.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 2.
*/

#define REG_MODEM_GFSK50_11_WAVE45                         (MODEM_BASE + 0x298)
/*
GFSK50_11_WAVE45: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 5.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 4.
*/

#define REG_MODEM_GFSK50_11_WAVE67                         (MODEM_BASE + 0x29c)
/*
GFSK50_11_WAVE67: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 7.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 6.
*/

#define REG_MODEM_GFSK50_11_WAVE89                         (MODEM_BASE + 0x2a0)
/*
GFSK50_11_WAVE89: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 9.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 8.
*/

#define REG_MODEM_GFSK50_11_WAVE1011                       (MODEM_BASE + 0x2a4)
/*
GFSK50_11_WAVE1011: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 11.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 10.
*/

#define REG_MODEM_GFSK50_11_WAVE1213                       (MODEM_BASE + 0x2a8)
/*
GFSK50_11_WAVE1213: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 13.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 12.
*/

#define REG_MODEM_GFSK50_11_WAVE1415                       (MODEM_BASE + 0x2ac)
/*
GFSK50_11_WAVE1415: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 15.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 14.
*/

#define REG_MODEM_GFSK50_11_WAVE1617                       (MODEM_BASE + 0x2b0)
/*
GFSK50_11_WAVE1617: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 17.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 16.
*/

#define REG_MODEM_GFSK50_11_WAVE1819                       (MODEM_BASE + 0x2b4)
/*
GFSK50_11_WAVE1819: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 19.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 18.
*/

#define REG_MODEM_GFSK50_11_WAVE2021                       (MODEM_BASE + 0x2b8)
/*
GFSK50_11_WAVE2021: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 21.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 20.
*/

#define REG_MODEM_GFSK50_11_WAVE2223                       (MODEM_BASE + 0x2bc)
/*
GFSK50_11_WAVE2223: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 23.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 22.
*/

#define REG_MODEM_GFSK50_11_WAVE2425                       (MODEM_BASE + 0x2c0)
/*
GFSK50_11_WAVE2425: 
[31:25]: 
[24:16]: the wave for gfsk50_11. index is 25.
[15:9]: 
[8:0]: the wave for gfsk50_11. index is 24.
*/

#define REG_MODEM_GFSK50_10_WAVE01                         (MODEM_BASE + 0x2d0)
/*
GFSK50_10_WAVE01: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 1.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 0.
*/

#define REG_MODEM_GFSK50_10_WAVE23                         (MODEM_BASE + 0x2d4)
/*
GFSK50_10_WAVE23: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 3.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 2.
*/

#define REG_MODEM_GFSK50_10_WAVE45                         (MODEM_BASE + 0x2d8)
/*
GFSK50_10_WAVE45: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 5.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 4.
*/

#define REG_MODEM_GFSK50_10_WAVE67                         (MODEM_BASE + 0x2dc)
/*
GFSK50_10_WAVE67: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 7.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 6.
*/

#define REG_MODEM_GFSK50_10_WAVE89                         (MODEM_BASE + 0x2e0)
/*
GFSK50_10_WAVE89: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 9.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 8.
*/

#define REG_MODEM_GFSK50_10_WAVE1011                       (MODEM_BASE + 0x2e4)
/*
GFSK50_10_WAVE1011: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 11.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 10.
*/

#define REG_MODEM_GFSK50_10_WAVE1213                       (MODEM_BASE + 0x2e8)
/*
GFSK50_10_WAVE1213: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 13.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 12.
*/

#define REG_MODEM_GFSK50_10_WAVE1415                       (MODEM_BASE + 0x2ec)
/*
GFSK50_10_WAVE1415: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 15.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 14.
*/

#define REG_MODEM_GFSK50_10_WAVE1617                       (MODEM_BASE + 0x2f0)
/*
GFSK50_10_WAVE1617: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 17.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 16.
*/

#define REG_MODEM_GFSK50_10_WAVE1819                       (MODEM_BASE + 0x2f4)
/*
GFSK50_10_WAVE1819: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 19.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 18.
*/

#define REG_MODEM_GFSK50_10_WAVE2021                       (MODEM_BASE + 0x2f8)
/*
GFSK50_10_WAVE2021: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 21.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 20.
*/

#define REG_MODEM_GFSK50_10_WAVE2223                       (MODEM_BASE + 0x2fc)
/*
GFSK50_10_WAVE2223: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 23.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 22.
*/

#define REG_MODEM_GFSK50_10_WAVE2425                       (MODEM_BASE + 0x300)
/*
GFSK50_10_WAVE2425: 
[31:25]: 
[24:16]: the wave for gfsk50_10. index is 25.
[15:9]: 
[8:0]: the wave for gfsk50_10. index is 24.
*/

#define REG_MODEM_SAR_CIC_CONFIG                           (MODEM_BASE + 0x320)
/*
SAR_CIC_CONFIG: 
[31:26]: 
[25:16]: offset data for input adc data.unsigned.0-1023.
after subtract rgf_saradc_din_offset, the saradc input data is converted singed signal.
[15:4]: reserved
[3:0]: IF down conversion multple shift number. The range is 4-20.
*/

#endif

/********************************************** 
 * RFC Registers
 **********************************************/
#if RFC_BASE

#define REG_RFC_R0_RX_TX_SYN_CH                            (RFC_BASE + 0x0)
#define REG_RFC_R0_RX_TX_SYN_CH_RF_bias_power_down         ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_TX_div2__power_down        ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_TX_LO_power_down           ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_UPC_power_down             ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_DA_power_down              ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Power_Down_TX_DAC          ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((26) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Enable_master_bias         ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RX_LO_power_down           ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RX_DIV2_power_down         ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Mixer_power_down           ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_LNA_power_down             ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RX_PGA_Enable              ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RX_Filter_Enable           ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RX_TIA_Enable              ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_ADC_Power_down             ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_RCCAL_Enable               ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Synthesizer_enable         ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((15) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Synthesizer_counter_resetb ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((14) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_Coarse_tuning_enable       ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((13) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_VCO_ctune_start            ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_VCO_enable                 ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_dpll_enable                ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_dpll_counter_resetb        ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_DCXO_Enable                ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_RFC_R0_RX_TX_SYN_CH_SEL_TXRX                   ((((REG_RFC_R0_RX_TX_SYN_CH) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
/*
R0_RX_TX_SYN_CH: 
[31]: 0: pwr_on RF master bias
1: default
[30]: 0: pwr on TX div2
1:default
[29]: 0: pwr_on TX LO 
1: default
[28]: 0: pwr on UPC 
1: default
[27]: 0: pwr_on DA 
1: default
[26]: 1: Power Down (default)
0: Enable
[25]: 1: Enable
0: Disable (default)
[24]: 0: power on LO 
1: default
[23]: 0: power on RX DIV2
1: default
[22]: 0: power on Mixer 
1: default
[21]: 0: power on LNA 
1: default
[20]: 1: Power On
0:Power down(Default)
[19]: 1: Power On
0:Power down(Default)
[18]: 1: Power On
0:Power down(Default)
[17]: 0: power on
1: default
[16]: 1: Enable 
0: Disable RCCAL (default)
[15]: 1: enable synthesizer
0: default
[14]: 0: reset (default)
[13]: 1: enable
0: disable (default)
[12]: 1: start
0: stop (default)
[11]: 0 = Power Down (default)
1 = Power Up
[10]: 1: enable
0: default
[9]: 0: reset (default)
[8]: 0 default (active high
1 enable, 0 disable)
[7]: 0: RX (default) 
1: TX
[6:0]: channel number 2 ~ 80
*/

#define REG_RFC_R1_RX_GAIN_DCOS                            (RFC_BASE + 0x4)
#define REG_RFC_R1_RX_GAIN_DCOS_BGR_Enable                 ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_RX_limiter_enable          ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_RXFEG_1                    ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((15) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_RXFEG_0                    ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((14) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_sleep_deep                 ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((13) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_rf_control_on              ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_LNA_gain                   ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_RFC_R1_RX_GAIN_DCOS_Mixer_low_gain             ((((REG_RFC_R1_RX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
/*
R1_RX_GAIN_DCOS: 
[31]: 1: Enable
0: Disable(default)
[30]: 1: Enable
0: Disable(default)
[29:23]: 
[22:16]: 
[15]: LNA Gain
10: High Gain
01: Mid Gain
00: Low Gain
[14]: 
[13]: 1: activate levelshifters 
0:disable levelshifters
[12]: 0: all channel_on, txrx_on and da_on is 0(gated)
1: normal.
[11]: 0: low gain
1: high gain( default)
[10]: 0: high gain ( default)
1: low gain
[9:8]: 00:72dB(Default)
10:70dB
01:68dB
[7:0]: 10100000:34dB
01100000:31dB
10000000:28dB(Default)
01000000:25dB
10010000:22dB
01010000:19dB
10001000:16dB
01001000:13dB
10000100:10dB
01000100:7dB
10000010:4dB
01000010:1dB
10000001:-2dB
01000001:-5dB
00000001:-8dB
*/

#define REG_RFC_R2_TX_GAIN_DCOS                            (RFC_BASE + 0x8)
#define REG_RFC_R2_TX_GAIN_DCOS_DA_low_gain                ((((REG_RFC_R2_TX_GAIN_DCOS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
/*
R2_TX_GAIN_DCOS: 
[31:25]: 1: enable
0: disable(default)
Bit<6>:RFPLL
Bit<5>:DPLL
Bit<4>: RF
Bit<3>: analog
Bit<2>: xtal
Bit<1>: Digital
Bit<0>: !Digital

[24]: 0: high gain( default)
1: low gain
[23:20]: 1000: Default
[19:16]: 1000: Default
[15:13]: 
[12:8]: 01100 ( default)
[7:5]: 
[4:0]: 01100 ( default)
*/

#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP                   (RFC_BASE + 0xc)
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_PFD_up_enable     ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_PFD_down_enable   ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_PFD_polarity      ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_Reference_non_bypass ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_Reference_polarity ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP_Syn_bias_res_enable ((((REG_RFC_R3_SYN_PFD_REFDIV_LDO_CP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
R3_SYN_PFD_REFDIV_LDO_CP: 
[31]: 
[30]: 
[29]: 
[28:23]: 
[22]: 
[21]: 
[20:16]: 
[15:14]: 
[13:12]: 
[11]: 
[10:8]: 
[7:4]: 
[3:0]: 
*/

#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV                   (RFC_BASE + 0x10)
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Vco_ctune_code_inv ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_VCO_tuning_code_overwriting_enable ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Loop_divide_ratio_overwriting_enable ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Sdm_enable        ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Sdm_architecture  ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Sdm_dither        ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((26) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Sdm_order         ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_VCO_coarse_tuning_start_selection ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((15) << 2))
#define REG_RFC_R4_SYN_OVERWRITE_SDM_DIV_Divider_output_polarity ((((REG_RFC_R4_SYN_OVERWRITE_SDM_DIV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
/*
R4_SYN_OVERWRITE_SDM_DIV: 
[31]: 
[30]: 
[29]: 
[28]: 
[27]: 
[26]: 
[25]: 
[24:20]: x10us
[19:16]: 
[15]: 
[14:13]: 
[12:11]: 
[10]: 
[9:8]: 
[7:0]: 
*/

#define REG_RFC_R5_SYN_VCO_BAND_DIV_RATIO                  (RFC_BASE + 0x14)
/*
R5_SYN_VCO_BAND_DIV_RATIO: 
[31:24]: 
[23:16]: 
[15:8]: 
[7:0]: 
*/

#define REG_RFC_R6_RFVCO_LO                                (RFC_BASE + 0x18)
#define REG_RFC_R6_RFVCO_LO_High_or_low_side_RX_mixer_injection ((((REG_RFC_R6_RFVCO_LO) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_RFC_R6_RFVCO_LO_Rescue_master_bias             ((((REG_RFC_R6_RFVCO_LO) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_RFC_R6_RFVCO_LO_Rescueb_VCO                    ((((REG_RFC_R6_RFVCO_LO) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
R6_RFVCO_LO: 
[31:29]: 
[28:26]: 
[25:21]: the delay between pwrdnb_vco rise edge and start_ct rese edge.unit is 1us.
[20:18]: 000: 10uA
001: 12.5uA
010: 15uA
011: 17.5uA
100: 20uA( default)
101: 22.5uA
110: 25uA
111: 27.5uA
[17:15]: 000 = 2.5uA
100 = default 4.5uA
111 = 9.5uA
[14:11]: 0000 = lowest bias
0100 = default
1111 = Highest bias
[10:7]: 0010 (26MHz)
[6]: 
[5:4]: 
[3:2]: 00: 750ohm
01: 3/4*750ohm (default)
10: 2/3*750ohm
11 6/11*750ohm

[1]: 1: Rescue 
0: Normal (default)
[0]: 1: Normal (default)
0: Output from Vdd/R
*/

#define REG_RFC_R7_TX_RF                                   (RFC_BASE + 0x1c)
#define REG_RFC_R7_TX_RF_Rescue_RFBias                     ((((REG_RFC_R7_TX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_RFC_R7_TX_RF_Bias_enhancement_for_UPC          ((((REG_RFC_R7_TX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_RFC_R7_TX_RF_Enhance_DA_bias                   ((((REG_RFC_R7_TX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
R7_TX_RF: 
[31:29]: 
[28:24]: 
[23:21]: 
[20:18]: 000: 2.5uA
001: 5uA ( default)
010: 7.5uA
011: 10uA
100 : 12.5u
101: 15u
110: 17.5u
111: 20uA
[17]: 0 default
1: rescue mode
[16]: 1: higher bias current for DA
0: lower bias current for DA (default)
[15:13]: 000: 5uA
001: 7.5uA
010: 10uA( default)
011:12.5uA
100:15uA
101: 17.5uA
110: 20uA
111: 22.5uA
[12:9]: 0100 ( default)
[8]: 0: default
1: extra current 50uA
[7:5]: 011(default)
0 4.5mA 8.0mA
1 5.0mA 9.5mA
2 5.3mA 10.6mA
3 5.6mA 11.9mA
4 6.0mA 13.5mA
5 6.4mA 15.3mA
6 6.8mA 17.0mA
7 7.4mA 18.0mA
[4:0]: 01001 (default)
*/

#define REG_RFC_R8_RX_RF                                   (RFC_BASE + 0x20)
#define REG_RFC_R8_RX_RF_TXDAC_clock_select                ((((REG_RFC_R8_RX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R8_RX_RF_Q_channel_im2_cal_enable          ((((REG_RFC_R8_RX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R8_RX_RF_I_channel_im2_cal_enable          ((((REG_RFC_R8_RX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_RFC_R8_RX_RF_LNA_tail_current                  ((((REG_RFC_R8_RX_RF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
/*
R8_RX_RF: 
[31]: 1: DPLL output (default)
0: DAC buffer clock
[30:29]: 00: Unit I = 125nA
01: Unit I = 187.5nA
10: Unit I=250nA (default)
11: Unit I = 500nA
[28]: 
[27]: 
[26:21]: 
[20:15]: 
[14:13]: 00: 0.1V
01: 0.2V
10: 0.3V(default)
11: 0.4V
[12:11]: 00: 0.3V
01: 0.4V (default)
10: 0.5V
11: 0.6V
[10:8]: 000
001
010
011: 1.4Kohm (default)
100
101
110
111
[7:5]: 000: 10uA
001: 30uA
010: 50uA(default)
011:70uA
100:90uA
101:110uA
110: 130uA
111: 150uA
[4]: 0: tail current(default)
1: w/o tail current
[3:0]: 
*/

#define REG_RFC_R9_RX_BB                                   (RFC_BASE + 0x24)
#define REG_RFC_R9_RX_BB_dcoc_iq_swap                      ((((REG_RFC_R9_RX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R9_RX_BB_txdac_iq_swap                     ((((REG_RFC_R9_RX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R9_RX_BB_ADC_reset                         ((((REG_RFC_R9_RX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_RFC_R9_RX_BB_CM_reference_voltage_rescue       ((((REG_RFC_R9_RX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_RFC_R9_RX_BB_Reference_buffer_IB_control       ((((REG_RFC_R9_RX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
/*
R9_RX_BB: 
[31]: 1:b1: swap dcoc I/Q
1'b0: no swap
[30]: 1'b1: swap txdac I/Q
1'b0: no swap
[29:27]: 
[26:25]: 00:7.5u
01:10u(Default)
10:15u
11:20u
[24:17]: 
[16]: 0:default
1: reset;
[15]: 
[14:13]: 10: Noswapping
01: Swapping(Default)
[12:11]: 10: Noswapping(Default)
01: Swapping
[10]: 
[9]: 
[8]: 
[7:6]: 00: 0.3V
01:0.4V (Default)
10: 0.5V
11: 0.6V
[5:4]: 00:7.5u
01:10u(Default)
10:15u
11:20u
[3:2]: 00:7.5u
01:10u(Default)
10:15u
11:20u
[1:0]: 00:7.5u
01:10u(Default)
10:15u
11:20u
*/

#define REG_RFC_R10_TX_BB                                  (RFC_BASE + 0x28)
#define REG_RFC_R10_TX_BB_TXDAC_restb                      ((((REG_RFC_R10_TX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R10_TX_BB_DAC_input_buffer_enable          ((((REG_RFC_R10_TX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_RFC_R10_TX_BB_DAC_buffer_IQ_indicator          ((((REG_RFC_R10_TX_BB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
/*
R10_TX_BB: 
[31]: 0: reset default
[30]: 
[29]: 
[28:27]: 00 DCXO CLK (default)
01 TXDAC clk 24M
10 ADC 48M Ckout
11 BB_CLK48M
[26:25]: 00: Modem input (default)
01: SBI input
10: DAC butter
[24]: 1'b0
[23]: 1, first data is I channel(default)
0, first data is Q channel
[22:21]: 
[20:12]: 011111111: Default
[11:9]: 3'b0
[8:0]: 011111111: Default
*/

#define REG_RFC_R11_DCXO1                                  (RFC_BASE + 0x2c)
#define REG_RFC_R11_DCXO1_Test_mode_enable                 ((((REG_RFC_R11_DCXO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R11_DCXO1_PWRDN_RSSI                       ((((REG_RFC_R11_DCXO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_RFC_R11_DCXO1_RXBB_WBEN                        ((((REG_RFC_R11_DCXO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
/*
R11_DCXO1: 
[31:29]: 
[28]: DCXO_TESTEN
[27:24]: previously crystal refrence setting
[23:16]: DCXO_CAPC
[15:8]: DCXO_MPS
[7:4]: DCXO_RBSEL
[3]: RSSI power down, 0 enable, 1 disable
[2]: RX BB wide band enable, 0 disable, 1 enable
[1:0]: RXBB Common mode adjustment
*/

#define REG_RFC_R12_DCXO2_RSSI1                            (RFC_BASE + 0x30)
#define REG_RFC_R12_DCXO2_RSSI1_dcoc_cal_disable           ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_Temperature_compensation_enable ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_PTAT_temperature_compensation_on ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_Cap_compensation_enable    ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_External_clock_enable      ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_EXTCLK_EN                  ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_RFC_R12_DCXO2_RSSI1_Constant_gm_rescue_mode_on ((((REG_RFC_R12_DCXO2_RSSI1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
/*
R12_DCXO2_RSSI1: 
[31:27]: DCXO_TEMPC
[26]: 
[25:23]: 
[22]: dcoc cal disable. 1 disable, 0 enable
[21]: DCXO_TEMPCEN
(active high, 1 enable, 0 disable)
[20]: DCXO_PTATEN
(active high, 1 enable, 0 disable)
[19]: DCXO_CAPCEN
[18]: 
[17]: Enable external clock, 1 enable, 0 disable
[16]: DCXO_CGMRSCEN
(active high, 1 enable, 0 disable)
[15:13]: 
[12:10]: 
[9:7]: 
[6:3]: 
[2:0]: 000: <-21dBm
001: (-21, -15) dBm
011: (-15, -9) dBm
111: >-9dBm
*/

#define REG_RFC_R13_RCCAL_RSSI2                            (RFC_BASE + 0x34)
#define REG_RFC_R13_RCCAL_RSSI2_RCCAL_rstn_select          ((((REG_RFC_R13_RCCAL_RSSI2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_R13_RCCAL_RSSI2_RCCAL_resetb               ((((REG_RFC_R13_RCCAL_RSSI2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R13_RCCAL_RSSI2_RSSI_DCOC_Automatic        ((((REG_RFC_R13_RCCAL_RSSI2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R13_RCCAL_RSSI2_RSSI_DCOC_enable           ((((REG_RFC_R13_RCCAL_RSSI2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R13_RCCAL_RSSI2_Auto_RCCAL_selection       ((((REG_RFC_R13_RCCAL_RSSI2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((14) << 2))
/*
R13_RCCAL_RSSI2: 
[31]: 0, using pd of misc_rccal_digital (default)
1, reg13[30]
[30]: 0 reset (default)
[29]: 1: Automatic (default)
0: Manuel
[28]: 1: enable (default)
0: disable
[27:22]: 
[21:16]: 
[15]: 
[14]: 0: Auto (default)
1: Manuel+Auto
[13:12]: 00: Unit I = 125nA
01: Unit I = 187.5nA
10: Unit I=250nA (default)
11: Unit I = 500nA
[11:6]: 
[5:0]: 
*/

#define REG_RFC_R14_LDO_CONTROL_REGISTER                   (RFC_BASE + 0x38)
/*
R14_LDO_CONTROL_REGISTER: 
[31:27]: 00000: Highest BW
11111: Lowest BW
00000: Default
[26:24]: 
[23:16]: ( digital,xtal, analog, RF)
00:1.225
01:1.25(default)
10:1.275
11:1.3
[15:12]: 
[11:8]: 1: 471mV
0:498mV (default)
Bit<11>: RF
Bit<10>: analog
Bit<9>: xtal
Bit<8>: dig
[7:4]: 1: debug mode
0: default
Bit<7>: RF
Bit<6>: analog
Bit<5>: xtal
Bit<4>: dig
[3:0]: 
*/

#define REG_RFC_R15_DPLL_PFD                               (RFC_BASE + 0x3c)
#define REG_RFC_R15_DPLL_PFD_PFD_polarity                  ((((REG_RFC_R15_DPLL_PFD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_RFC_R15_DPLL_PFD_Reference_bypass_to_output    ((((REG_RFC_R15_DPLL_PFD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_RFC_R15_DPLL_PFD_Reference_non_bypass          ((((REG_RFC_R15_DPLL_PFD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_RFC_R15_DPLL_PFD_Reference_polarity            ((((REG_RFC_R15_DPLL_PFD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_RFC_R15_DPLL_PFD_DPLL_bias_res_enable          ((((REG_RFC_R15_DPLL_PFD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
R15_DPLL_PFD: 
[31]: 
[30]: 
[29:24]: 
[23]: 
[22]: 
[21]: 
[20:16]: 
[15:14]: 
[13:12]: 
[11]: 
[10:8]: 
[7:4]: 
[3:0]: 
*/

#define REG_RFC_R16_DPLL_OVERWRITE                         (RFC_BASE + 0x40)
#define REG_RFC_R16_DPLL_OVERWRITE_Loop_divide_ratio_overwriting_enable ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R16_DPLL_OVERWRITE_Sdm_enable              ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R16_DPLL_OVERWRITE_Sdm_architecture        ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_RFC_R16_DPLL_OVERWRITE_Sdm_dither              ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((26) << 2))
#define REG_RFC_R16_DPLL_OVERWRITE_Sdm_order               ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_RFC_R16_DPLL_OVERWRITE_Divide_output_polarity  ((((REG_RFC_R16_DPLL_OVERWRITE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
/*
R16_DPLL_OVERWRITE: 
[31:30]: 00, no high frequency clock (default)
01, 96MHz
10, 109.7MHz
11, 128MHz
[29]: 
[28]: 
[27]: 
[26]: 
[25]: 
[24:23]: 
[22:21]: 
[20]: 
[19:15]: 
[14]: 
[13:8]: 
[7:0]: 
*/

#define REG_RFC_R17_DPLL_DIVIDE_RATIO2                     (RFC_BASE + 0x44)
/*
R17_DPLL_DIVIDE_RATIO2: 
[31:24]: 
[23:16]: 
[15:14]: 
[13:8]: 
[7:0]: (reading)/DPLL seting
*/

#define REG_RFC_R18_DPLL_DIVIDE_RATION                     (RFC_BASE + 0x48)
/*
R18_DPLL_DIVIDE_RATION: 
[31:24]: 
[23:16]: 
[15:14]: 
[13:10]: 0010 (26MHz)
[9:0]: 
*/

#define REG_RFC_R19_RX_TX_WAIT_TIME                        (RFC_BASE + 0x4c)
#define REG_RFC_R19_RX_TX_WAIT_TIME_UPC_pump_current       ((((REG_RFC_R19_RX_TX_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_RFC_R19_RX_TX_WAIT_TIME_LNA_bias_on_in_TX_mode ((((REG_RFC_R19_RX_TX_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
/*
R19_RX_TX_WAIT_TIME: 
[31:30]: 00 5uA 
'01 7.5uA 
'10 10uA
'11 12.5uA
[29:28]: 00 0uA 
'01 50uA 
'10 100uA 
'11 150uA
[27]: 0: 0uA 
1: 50uA
[26:25]: 
[24:23]: 00 3 
'01 4 
'10 6 
'11 8
[22]: 0: LNA bias off 
1: LNA bias on
[21:19]: 000: off
[18:16]: 000: off
[15:8]: (previously PA on delay)
00000010
[7:0]: (previously PA off delay)
00000010
*/

#define REG_RFC_R20_CALIBRATION_WAIT_TIME                  (RFC_BASE + 0x50)
#define REG_RFC_R20_CALIBRATION_WAIT_TIME_Modem_output_over_write ((((REG_RFC_R20_CALIBRATION_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_RFC_R20_CALIBRATION_WAIT_TIME_State_Machine_enable ((((REG_RFC_R20_CALIBRATION_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((15) << 2))
#define REG_RFC_R20_CALIBRATION_WAIT_TIME_Enable_Test      ((((REG_RFC_R20_CALIBRATION_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((14) << 2))
#define REG_RFC_R20_CALIBRATION_WAIT_TIME_dcoc_output_over_write ((((REG_RFC_R20_CALIBRATION_WAIT_TIME) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
R20_CALIBRATION_WAIT_TIME: 
[31:25]: DCXO settle time *500us
[24]: 0, AGC uses modem output
1, AGC uses APB output.
0 default
[23:16]: 110100 (x10us)
[15]: 0: manual control
1: state machine control (default)
[14]: 0 disable (default)
1 enable
[13:12]: 
[11]: 0: DCOC output frome dc_offset module 1:DCOC output from APB path
[10]: 
[9:3]: the delay between rxtx_on changed to "1" and ADC_RST fall edge,the unit is 1us.(adcrst_count*1us)
[2:0]: 000(default)
1 enable
Note: only one bit could set to 1
*/

#define REG_RFC_R21_TEST_BUFFER2                           (RFC_BASE + 0x54)
#define REG_RFC_R21_TEST_BUFFER2_Enahble_VCO_test          ((((REG_RFC_R21_TEST_BUFFER2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_RFC_R21_TEST_BUFFER2_Enhance_VCO_test_buffer_bias ((((REG_RFC_R21_TEST_BUFFER2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_R21_TEST_BUFFER2_ATEST_direction           ((((REG_RFC_R21_TEST_BUFFER2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
/*
R21_TEST_BUFFER2: 
[31:30]: 
[29]: 
[28]: 
[27:26]: 
[25]: 1 output
0 input (default)
[24:16]: 000000000(default)
1 enable
 Note: ONLY one bit could set to 1
[15:11]: 
[10:4]: 
[3:0]: OFF: 4'b0
RF_INIT: 4'b1
IDLE: 4'b10
RFPLL: 4'b11
RX: 4'b100
TX: 4'b101
*/

#define REG_RFC_R22_RSSI_DCOC                              (RFC_BASE + 0x58)
/*
R22_RSSI_DCOC: 
[31:28]: 
[27:22]: 
[21:16]: 
[15:12]: 
[11:6]: 
[5:0]: 
*/

#define REG_RFC_R23_SYN_DIV_RD1                            (RFC_BASE + 0x5c)
/*
R23_SYN_DIV_RD1: 
[31:26]: 
[25:24]: 
[23:16]: 
[15:8]: 
[7:0]: 
*/

#define REG_RFC_R24_SYN_DIV_RD3                            (RFC_BASE + 0x60)
/*
R24_SYN_DIV_RD3: 
[31:24]: 
[23:16]: 
[15:0]: 
*/

#define REG_RFC_R25_PASS_CHAN(FPGA_ONLY)                   (RFC_BASE + 0x64)
/*
R25_PASS_CHAN(FPGA_ONLY): noise coefficent
[31:23]: noise coefficent
[22:20]: 
[19:16]: noise right shift tune,
[15:14]: 
[13:8]: the amplitude of received signal
[7:5]: 
[4:0]: the frequency offset index
*/

#define REG_RFC_R26_RF_SEL                                 (RFC_BASE + 0x68)
#define REG_RFC_R26_RF_SEL_rf24m_invsel                    ((((REG_RFC_R26_RF_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_RFC_R26_RF_SEL_rf48m_invsel                    ((((REG_RFC_R26_RF_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_RFC_R26_RF_SEL_lpo_sel                         ((((REG_RFC_R26_RF_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_RFC_R26_RF_SEL_modem_iqtst_sel                 ((((REG_RFC_R26_RF_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_RFC_R26_RF_SEL_rf_sel                          ((((REG_RFC_R26_RF_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
R26_RF_SEL: 
[31:7]: 
[6]: 0:don’t invert clk_txdac;1:invert clk_txdac
[5]: 0:don’t invert clk_bb48m;1:invert clk_bb48m
[4]: 0:select lpo32;1:select lpo_32_2
[3:2]: 
[1]: output modem IQ data via BlueRF pad for test
[0]: 1'b0: select RFFE+MODEM 1'b1: select other RF chip
*/

#define REG_RFC_AGC_CTRL                                   (RFC_BASE + 0x6c)
#define REG_RFC_AGC_CTRL_agc_init_en                       ((((REG_RFC_AGC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
/*
AGC_CTRL: 
[31:29]: 
[28]: 
[27:25]: 
[24]: 
[23:21]: 
[20]: agc init gain enable
[19:18]: 
[17:14]: 
[13:0]: agc initial gain.
*/

#define REG_RFC_DCOC_INIT                                  (RFC_BASE + 0x70)
/*
DCOC_INIT: 
[31]: 
[30:24]: dc offset high gain Q initial value
[23]: 
[22:16]: dc offset high gain I initial value
[15]: 
[14:8]: dc offset low gain Q initial value
[7]: 
[6:0]: dc offset low gain I initial value
*/

#define REG_RFC_DCOC_AGC_GAIN                              (RFC_BASE + 0x74)
/*
DCOC_AGC_GAIN: 
[31:30]: 
[29:16]: dc offset calculation initial high gain
[15:14]: 
[13:0]: dc offset calculation initial low gain
*/

#define REG_RFC_DCOC_TIME                                  (RFC_BASE + 0x78)
/*
DCOC_TIME: 
[31:10]: 
[9:8]: dc offset integral clk cycle length.
For sdmadc RFFE version: 2'b00:512 2'b01:1024 2'b10: 2048 2'b11:4096 @48MHz.
For saradc RFFE version: 2'b00 256 2'b01:512 2'b10:1024 2'b11: 2048 @24MHz.
[7:4]: dc offset calculate repeat number, dcoc_repeat_num+1
[3:0]: dc offset wait time after gain ,dc ctrl code set, Unit:us
*/

#define REG_RFC_DCOC_SHIFT_BITS                            (RFC_BASE + 0x7c)
/*
DCOC_SHIFT_BITS: 
[31:5]: 
[4:0]: dc offset matrix caculte shift bits.
*/

#define REG_RFC_DC_OFFSET_EN                               (RFC_BASE + 0x80)
#define REG_RFC_DC_OFFSET_EN_dcoc_sw_rst                   ((((REG_RFC_DC_OFFSET_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_RFC_DC_OFFSET_EN_dc_offset_en                  ((((REG_RFC_DC_OFFSET_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
DC_OFFSET_EN: 
[31:2]: 
[1]: when write this bit to 1, reset the dcoc module. the reset signal drove by this bit will automatically release at next clock.
[0]: dc offset work enable
*/

#define REG_RFC_DCOC_SAVE                                  (RFC_BASE + 0x84)
/*
DCOC_SAVE: 
[31]: 
[30:24]: dc offset high gain Q after calculation
[23]: 
[22:16]: dc offset high gain I after calculation
[15]: 
[14:8]: dc offset low gain Q after calculation
[7]: 
[6:0]: dc offset low gain I after calculation
*/

#define REG_RFC_DCOC_MATRIX_0                              (RFC_BASE + 0x88)
/*
DCOC_MATRIX_0: 
[31:17]: 
[16:0]: dc offset calculte matrix
*/

#define REG_RFC_DCOC_MATRIX_1                              (RFC_BASE + 0x8c)
/*
DCOC_MATRIX_1: 
[31:17]: 
[16:0]: dc offset calculte matrix
*/

#define REG_RFC_AGC_MAP_1_0                                (RFC_BASE + 0x90)
/*
AGC_MAP_1_0: 
[31:30]: 
[29:16]: agc gain value register 1
[15:14]: 
[13:0]: agc gain value register 0
*/

#define REG_RFC_AGC_MAP_3_2                                (RFC_BASE + 0x94)
/*
AGC_MAP_3_2: 
[31:30]: 
[29:16]: agc gain value register 3
[15:14]: 
[13:0]: agc gain value register 2
*/

#define REG_RFC_AGC_MAP_5_4                                (RFC_BASE + 0x98)
/*
AGC_MAP_5_4: 
[31:30]: 
[29:16]: agc gain value register 5
[15:14]: 
[13:0]: agc gain value register 4
*/

#define REG_RFC_AGC_MAP_7_6                                (RFC_BASE + 0x9c)
/*
AGC_MAP_7_6: 
[31:30]: 
[29:16]: agc gain value register 7
[15:14]: 
[13:0]: agc gain value register 6
*/

#define REG_RFC_AGC_MAP_9_8                                (RFC_BASE + 0xa0)
/*
AGC_MAP_9_8: 
[31:30]: 
[29:16]: agc gain value register 9
[15:14]: 
[13:0]: agc gain value register 8
*/

#define REG_RFC_AGC_MAP_11_10                              (RFC_BASE + 0xa4)
/*
AGC_MAP_11_10: 
[31:30]: 
[29:16]: agc gain value register 11
[15:14]: 
[13:0]: agc gain value register 10
*/

#define REG_RFC_AGC_MAP_13_12                              (RFC_BASE + 0xa8)
/*
AGC_MAP_13_12: 
[31:30]: 
[29:16]: agc gain value register 13
[15:14]: 
[13:0]: agc gain value register 12
*/

#define REG_RFC_AGC_MAP_15_14                              (RFC_BASE + 0xac)
/*
AGC_MAP_15_14: 
[31:30]: 
[29:16]: agc gain value register 15
[15:14]: 
[13:0]: agc gain value register 14
*/

#define REG_RFC_AGC_MAP_17_16                              (RFC_BASE + 0xb0)
/*
AGC_MAP_17_16: 
[31:30]: 
[29:16]: agc gain value register 17
[15:14]: 
[13:0]: agc gain value register 16
*/

#define REG_RFC_AGC_MAP_19_18                              (RFC_BASE + 0xb4)
/*
AGC_MAP_19_18: 
[31:30]: 
[29:16]: agc gain value register 19
[15:14]: 
[13:0]: agc gain value register 18
*/

#define REG_RFC_AGC_MAP_21_20                              (RFC_BASE + 0xb8)
/*
AGC_MAP_21_20: 
[31:30]: 
[29:16]: agc gain value register 21
[15:14]: 
[13:0]: agc gain value register 20
*/

#define REG_RFC_AGC_MAP_23_22                              (RFC_BASE + 0xbc)
/*
AGC_MAP_23_22: 
[31:30]: 
[29:16]: agc gain value register 23
[15:14]: 
[13:0]: agc gain value register 22
*/

#define REG_RFC_AGC_MAP_25_24                              (RFC_BASE + 0xc0)
/*
AGC_MAP_25_24: 
[31:30]: 
[29:16]: agc gain value register 25
[15:14]: 
[13:0]: agc gain value register 24
*/

#define REG_RFC_AGC_MAP_27_26                              (RFC_BASE + 0xc4)
/*
AGC_MAP_27_26: 
[31:30]: 
[29:16]: agc gain value register 27
[15:14]: 
[13:0]: agc gain value register 26
*/

#define REG_RFC_RF_ADD1                                    (RFC_BASE + 0xc8)
/*
RF_ADD1: 
[31:18]: 
[17:16]: TX RF bias tuning bits
[15:12]: ADC rccal offset code
[11:0]: RFPLL and RF tuning bits
*/

#define REG_RFC_RF_ADD2                                    (RFC_BASE + 0xcc)
#define REG_RFC_RF_ADD2_dcxo_pwr_sel_lp                    ((((REG_RFC_RF_ADD2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_RFC_RF_ADD2_const_gm_resc_en_lp                ((((REG_RFC_RF_ADD2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_RFC_RF_ADD2_temp_comp_en_lp                    ((((REG_RFC_RF_ADD2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
/*
RF_ADD2: 
[31:30]: 
[29:27]: PA LDO setting during TX off
[26:23]: DCXO LP mode LDO mode control bit
[22]: DCXO LP mode supply selection bit
[21]: DCXO LP mode rescue bias enable bit
[20]: DCXO LP mode temperature compensation enable bit
[19:12]: DCXO LP mode cap tune control bits
[11:4]: DCXO LP mode constant gm current control bits
[3:0]: DCXO LP mode constant gm resistor control bits
*/

#define REG_RFC_RF_ADD3                                    (RFC_BASE + 0xd0)
#define REG_RFC_RF_ADD3_apb_dcxo_lpen                      ((((REG_RFC_RF_ADD3) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
RF_ADD3: 
[31:9]: 
[8]: dcxo_lpen when apb path is selected
[7]: 
[6:0]: ldo enable value when LP clock is come from RFFE and SLEEPN = 0.
*/

#define REG_RFC_RF_GTUNE                                   (RFC_BASE + 0xd4)
/*
RF_GTUNE: 
[31:9]: 
[8:0]: gtune value read out
*/

#define REG_RFC_RF_DAC_CRTRL1                              (RFC_BASE + 0xd8)
#define REG_RFC_RF_DAC_CRTRL1_en_gt                        ((((REG_RFC_RF_DAC_CRTRL1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_RFC_RF_DAC_CRTRL1_clk_num                      ((((REG_RFC_RF_DAC_CRTRL1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_RFC_RF_DAC_CRTRL1_dac_pol                      ((((REG_RFC_RF_DAC_CRTRL1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
/*
RF_DAC_CRTRL1: 
[31]: enable vco gain tuning
[30:29]: modem path2 data delay
[28]: gain tuning clock number
[27]: dac polarity
[26:18]: dac value2
[17:9]: dac value1
[8:0]: dac value0
*/

#define REG_RFC_RF_DAC_CRTRL2                              (RFC_BASE + 0xdc)
/*
RF_DAC_CRTRL2: 
[31:16]: dac_settle time, default=0x0008
[15:0]: gain tuning time, default=0x0400
*/

#define REG_RFC_RF_DAC_CRTRL3                              (RFC_BASE + 0xe0)
#define REG_RFC_RF_DAC_CRTRL3_spi_r0w1_gt                  ((((REG_RFC_RF_DAC_CRTRL3) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
/*
RF_DAC_CRTRL3: 
[31:10]: 
[9]: spi read/write(0/1) of gain tune value
[8:0]: spi write data of gain tune value
*/

#define REG_RFC_RF_VERSION                                 (RFC_BASE + 0xe8)
#define REG_RFC_RF_VERSION_rgf_ble_sdmadc                  ((((REG_RFC_RF_VERSION) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_RFC_RF_VERSION_rgf_ble_only                    ((((REG_RFC_RF_VERSION) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
RF_VERSION: 
[31:2]: reserved
[1]: version select signal.this signal is valid only when rgf_ble_only = 1.
1: RFFE is SDMADC BLE version .
0: RFFE is SARADC BLE version.
[0]: version select signal.
1: RFFE is BLE version (SARADC version or SDMADC version).
0: RFFE is 9638B version.
*/

#define REG_RFC_BLE_GFSK_MODE                              (RFC_BASE + 0xec)
#define REG_RFC_BLE_GFSK_MODE_rgf_gfsk2m                   ((((REG_RFC_BLE_GFSK_MODE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BLE_GFSK_MODE: 
[31:1]: reserved
[0]: indicate if the current modulate/demodulate mode is 2mpbs gfsk. 
0:is not 2mbps gfsk mode. 
1:is 2mbps gfsk mode.
*/

#define REG_RFC_SAR_DCOC_CONFIG                            (RFC_BASE + 0xf0)
/*
SAR_DCOC_CONFIG: 
[31:5]: reserved
[4:0]: dcoc offset_est shifter bit number.range: 0-19.
*/

#endif

/**********************************************
 * BBLE Register Definitions
 **********************************************/
#if BBLE_BASE

#define REG_BBLE_BBLE_CTRL                                 (BBLE_BASE + 0x0)
#define REG_BBLE_BBLE_CTRL_clk_gating_en                   ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_BBLE_BBLE_CTRL_bb_clk_en                       ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_BBLE_BBLE_CTRL_rx_dpt                          ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_BBLE_BBLE_CTRL_tx_ept                          ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_BBLE_BBLE_CTRL_addr_timer_en                   ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_BBLE_BBLE_CTRL_sync_timer_en                   ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_BBLE_BBLE_CTRL_loop_mode                       ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BBLE_BBLE_CTRL_slave_mode                      ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_BBLE_BBLE_CTRL_dwh_en                          ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_BBLE_BBLE_CTRL_rx_en                           ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BBLE_BBLE_CTRL_tx_en                           ((((REG_BBLE_BBLE_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BBLE_CTRL: 
[31:18]: 
[17]: Enable clock gating in BBLE. If set to  1 , hardware will dynamically gate off the clocks of logics which are not working.
[16]: Enable all clocks in BBLE.
[15:12]: 
[11]: Indicate the rx packet needs decryption by AES after receiving.
[10]: Indicate the tx packet is encrypted by AES before transmitting.
[9:8]: 2'b11 : direct test mode, 2'b00 : normal mode
[7]: 
[6]: Rx address check timer enable.
[5]: Rx sync check timer enable.
[4]: loop back mode,set 1 for internal loopback
[3]: slave mode
[2]: data whiten enable
[1]: rx transfer enable
[0]: tx transfer enable
*/

#define REG_BBLE_BBLE_CTRL_2                               (BBLE_BASE + 0x4)
#define REG_BBLE_BBLE_CTRL_2_err_cnt_rst                   ((((REG_BBLE_BBLE_CTRL_2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
/*
BBLE_CTRL_2: 
[31:6]: 
[5]: Write 1 to reset the error count in register BBLE_RX_ERR_CNT
[4]: 
[3:0]: BD address error bits could accept
*/

#define REG_BBLE_INT_STATUS                                (BBLE_BASE + 0x8)
#define REG_BBLE_INT_STATUS_tx_length_err                  ((((REG_BBLE_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_BBLE_INT_STATUS_rx_crc_ok                      ((((REG_BBLE_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_BBLE_INT_STATUS_sync_check_timeout             ((((REG_BBLE_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_BBLE_INT_STATUS_rx_done                        ((((REG_BBLE_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BBLE_INT_STATUS_tx_done                        ((((REG_BBLE_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INT_STATUS: 
[31:10]: 
[9]: tx_length error,assert in tx mode
[8]: Rx bits CRC checking OK.status bit
[7:3]: reserved
[2]: sync_check_timeout, no active packet found.Write 1 to clear this interrupt.
[1]: Rx bits all received. Write 1 to clear this interrupt.
[0]: Tx bits all transmitted. Write 1 to clear this interrupt.
*/

#define REG_BBLE_INT_MASK                                  (BBLE_BASE + 0xc)
#define REG_BBLE_INT_MASK_sync_check_timeout_mask          ((((REG_BBLE_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_BBLE_INT_MASK_rx_done_mask                     ((((REG_BBLE_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BBLE_INT_MASK_tx_done_mask                     ((((REG_BBLE_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INT_MASK: 
[31:3]: 
[2]: mask bits of interrupt, mask : 1'b1
[1]: mask bits of interrupt, mask : 1'b1
[0]: mask bits of interrupt, mask : 1'b1
*/

#define REG_BBLE_BBLE_CRC_INIT                             (BBLE_BASE + 0x10)
/*
BBLE_CRC_INIT: 
[31:30]: 
[29:24]: channel index
[23:0]: CRC initial value
*/

#define REG_BBLE_BBLE_RX_ERR_CNT                           (BBLE_BASE + 0x14)
/*
BBLE_RX_ERR_CNT: 
[31:16]: 
[15:0]: Rx CRC error count. Write 1 to BBLE_CTRL_2[5] to clear this register.
*/

#define REG_BBLE_SYNC_TIMER                                (BBLE_BASE + 0x18)
/*
SYNC_TIMER: search timer for RX windows
[31:0]: search timer for RX windows
*/

#define REG_BBLE_RX_ADDR_TIMER                             (BBLE_BASE + 0x1c)
/*
RX_ADDR_TIMER: 
[31:6]: 
[5:0]: Address checking time window. From preamble detected, if address does not match in this time window, reset modem.
*/

#define REG_BBLE_TX_CHNL_ON_PNT_0                          (BBLE_BASE + 0x20)
/*
TX_CHNL_ON_PNT_0: 
[31:11]: 
[10:0]: Tx channel on time, in us level.
*/

#define REG_BBLE_TX_CHNL_ON_PNT_1                          (BBLE_BASE + 0x24)
/*
TX_CHNL_ON_PNT_1: 
[31:28]: 
[27:0]: Tx channel on time, in half slot level.
*/

#define REG_BBLE_RX_GOTO_TX_CTRL                           (BBLE_BASE + 0x28)
#define REG_BBLE_RX_GOTO_TX_CTRL_rx_goto_tx_auto           ((((REG_BBLE_RX_GOTO_TX_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
/*
RX_GOTO_TX_CTRL: 
[31:17]: 
[16]: When finish receiving a packet, go to transmit next tx packet after delay  rx_goto_tx_dly  us.
[15:0]: Delay from rx ending point to next tx channel_on point in an event, in us level.
*/

#define REG_BBLE_TX_DELAYS                                 (BBLE_BASE + 0x2c)
/*
TX_DELAYS: 
[31:30]: 
[29:20]: Delay from da_on to transmit first tx bit, in us level.
[19:10]: Delay from rxtx_on to da_on in Tx procedure, in us level.
[9:0]: Delay from channel_on to rxtx_on in Tx procedure, in us level.
*/

#define REG_BBLE_TX_END_PNT_0                              (BBLE_BASE + 0x30)
/*
TX_END_PNT_0: 
[31:11]: 
[10:0]: Tx ending time, in us level. When a tx packet transmitting is over, BBLE records value of the ref_bt_cnt into this register.
*/

#define REG_BBLE_TX_END_PNT_1                              (BBLE_BASE + 0x34)
/*
TX_END_PNT_1: 
[31:28]: 
[27:0]: Tx ending time, in half slot level. When a tx packet transmitting is over, BBLE records value of the ref_bt_clk into this register.
*/

#define REG_BBLE_RX_CHNL_ON_PNT_0                          (BBLE_BASE + 0x38)
/*
RX_CHNL_ON_PNT_0: 
[31:11]: 
[10:0]: Rx channel on time, in us level.
*/

#define REG_BBLE_RX_CHNL_ON_PNT_1                          (BBLE_BASE + 0x3c)
/*
RX_CHNL_ON_PNT_1: 
[31:28]: 
[27:0]: Rx channel on time, in half slot level.
*/

#define REG_BBLE_TX_GOTO_RX_CTRL                           (BBLE_BASE + 0x40)
#define REG_BBLE_TX_GOTO_RX_CTRL_rx_goto_tx_auto           ((((REG_BBLE_TX_GOTO_RX_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
/*
TX_GOTO_RX_CTRL: 
[31:17]: 
[16]: When finish receiving a packet, go to transmit next tx packet after delay  rx_goto_tx_dly  us.
[15:10]: 
[9:0]: Delay from rx ending point to next tx channel_on point in an event, in us level.
*/

#define REG_BBLE_RX_DELAYS                                 (BBLE_BASE + 0x44)
/*
RX_DELAYS: 
[31:20]: 
[19:10]: Delay from rxtx_on to receive first rx bit, in us level.
[9:0]: Delay from channel_on to rxtx_on in Rx procedure, in us level.
*/

#define REG_BBLE_RX_END_PNT_0                              (BBLE_BASE + 0x48)
/*
RX_END_PNT_0: 
[31:11]: 
[10:0]: Rx ending time, in us level. When a rx packet receiving is over, BBLE records value of the ref_bt_cnt into this register.
*/

#define REG_BBLE_RX_END_PNT_1                              (BBLE_BASE + 0x4c)
/*
RX_END_PNT_1: 
[31:28]: 
[27:0]: Rx ending time, in half slot level. When a rx packet receiving is over, BBLE records value of the ref_bt_clk into this register.
*/

#define REG_BBLE_PAYLOAD_MAX_LEN                           (BBLE_BASE + 0x50) //The Maximum length of the payload in the receiving packet, in byte.[7:0].
/*
REG_BBLE_PAYLOAD_MAX_LEN:
[31:8]: 
[7:0]: Max length of receiving packet payload length, in byte.
*/

#define REG_BBLE_ACCESS_ADDR0                              (BBLE_BASE + 0x80)
/*
ACCESS_ADDR0: Access address could accept, total 8,for master mode
[31:0]: Access address could accept, total 8,for master mode
*/

#define REG_BBLE_ACCESS_ADDR1                              (BBLE_BASE + 0x84)
/*
ACCESS_ADDR1: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR2                              (BBLE_BASE + 0x88)
/*
ACCESS_ADDR2: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR3                              (BBLE_BASE + 0x8c)
/*
ACCESS_ADDR3: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR4                              (BBLE_BASE + 0x90)
/*
ACCESS_ADDR4: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR5                              (BBLE_BASE + 0x94)
/*
ACCESS_ADDR5: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR6                              (BBLE_BASE + 0x98)
/*
ACCESS_ADDR6: Access address could accept
[31:0]: Access address could accept
*/

#define REG_BBLE_ACCESS_ADDR7                              (BBLE_BASE + 0x9c)
/*
ACCESS_ADDR7: Access address could accept
[31:0]: Access address could accept
*/

#endif

/**********************************************
 * BB Register Definitions
 **********************************************/
#if BB_BASE

#define REG_BB_BB_SEL                                      (BB_BASE + 0x200)
#define REG_BB_BB_SEL_bble_sel                             ((((REG_BB_BB_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_SEL: 
[31:2]: 
[1]: ble 2M enable.this bit is valid only when bble_sel=1.
1: ble 2M rx/tx is enable
0: ble 1M rx/tx is enable.
[0]: bb2.1 baseband and bble baseband select signal:
0: bb2.1 baseband is selected
1: bble baseband is selected
*/

#define REG_BB_PICCLK_ADJ_EN                               (BB_BASE + 0x210)
#define REG_BB_PICCLK_ADJ_EN_Picclk_offset_adj_state       ((((REG_BB_PICCLK_ADJ_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BB_PICCLK_ADJ_EN_rgf_pic_switch_en             ((((REG_BB_PICCLK_ADJ_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_BB_PICCLK_ADJ_EN_rgf_lmp_offset_en             ((((REG_BB_PICCLK_ADJ_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_BB_PICCLK_ADJ_EN_rgf_drag_en                   ((((REG_BB_PICCLK_ADJ_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BB_PICCLK_ADJ_EN_Picclk_adj_en                 ((((REG_BB_PICCLK_ADJ_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
PICCLK_ADJ_EN: 
[31:5]: 
[4]: Indicates the current states of piconet offset adjust HW circuit
1 b0:HW is in no adjusting state
1 b1: HW is in adjusting sate


[3]: Piconet switch between two BT networks. This bit is set by SW, cleared by SW.when piconet offset adjusting is finished, this bit is cleared to 0.
1 b0: no switch
1 b1: picnet switch is occurred

[2]: LMP piconet clock adjust indication. This bit is set by SW, cleared by SW.when piconet offset adjusting is finished, this bit is cleared to 0.
1 b0:is not LMP adjust
1 b1:is LMP adjust

[1]: Software dragging clock offset enable. This bit is set by SW, cleared by SW.when piconet offset adjusting is finished, this bit is cleared to 0.
1 b0:software path disalbe
1 b1:software path enable

[0]: Enable piconet clock adjust engine enable
1 b0:disable
1 b1:enalbe

*/

#define REG_BB_CLK_DRAG_US                                 (BB_BASE + 0x214)
#define REG_BB_CLK_DRAG_US_Polarity                        ((((REG_BB_CLK_DRAG_US) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
CLK_DRAG_US: 
[31:12]: (reserved)
[11]: Sign bit:
0 positive offset value
1 negative offset value

[10:0]: Piconet offset adjust value, the unit is us:
Unsigned number 
Range: [0 5](NOTE:hardware can support range:[0 1249])

*/

#define REG_BB_CLK_ADJ_US                                  (BB_BASE + 0x218)
#define REG_BB_CLK_ADJ_US_Polarity                         ((((REG_BB_CLK_ADJ_US) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
CLK_ADJ_US: 
[31:12]: (reserved)
[11]: Sign bit:
0 positive offset value
1 negative offset value

[10:0]: Piconet offset adjust value, the unit is us:
Unsigned number 
Range: [0 1249]

*/

#define REG_BB_CLK_ADJ_SLOTS                               (BB_BASE + 0x21c)
#define REG_BB_CLK_ADJ_SLOTS_polarity                      ((((REG_BB_CLK_ADJ_SLOTS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
/*
CLK_ADJ_SLOTS: 
[31:29]: (reserved)
[28]: Sign bit:
0 positive offset value
1 negative offset value

[27:0]: Piconet offset adjust value, the unit is half slot:
Unsigned number 
NOTE: because of clk_adj_us big range, the bits[1:0] will be ignored by HW.
*/

#define REG_BB_CLK_ADJ_INSTENT                             (BB_BASE + 0x220)
/*
CLK_ADJ_INSTENT: 
[31:28]: (reserved)
[27:0]: Indicates The value of bt_clk for implementing LMP piconet offset adjust, the unit is half slot:
Unsigned number 
NOTE:the bits[1:0] must be 2'b00.
*/

#define REG_BB_CRRT_PICCLK_OFFSET                          (BB_BASE + 0x224)
/*
CRRT_PICCLK_OFFSET: 
[31:28]: (reserved)
[27:0]: Indicates The differnet value of pic_bt_clk - ref_bt_clk, the unit is half slot: Unsigned number.
This value is updated at end of each pic_bt_clk update procedre

*/

#define REG_BB_CRRT_PICUS_OFFSET                           (BB_BASE + 0x228)
#define REG_BB_CRRT_PICUS_OFFSET_crrt_picus_polarity       ((((REG_BB_CRRT_PICUS_OFFSET) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
CRRT_PICUS_OFFSET: 
[31:12]: (reserved)
[11]: Sign bit:
0 positive offset value
1 negative offset value
[10:0]: Indicates The differnet value of pic_bt_cnt - ref_bt_cnt, the unit is ust: Unsigned number.range:0-1249us.
This value is updated at end of each pic_bt_clk update procedre

*/

#define REG_BB_CRRT_NATIVECLK_OFFSET                       (BB_BASE + 0x22c)
/*
CRRT_NATIVECLK_OFFSET: 
[31:28]: (reserved)
[27:0]: Indicates The differnet value of native_bt_clk - ref_bt_clk, the unit is half slot: Unsigned number.
This value is updated at end of each native_bt_clk update procedre

*/

#define REG_BB_CRRT_NATIVEUS_OFFSET                        (BB_BASE + 0x230)
#define REG_BB_CRRT_NATIVEUS_OFFSET_crrt_nativeus_polarity ((((REG_BB_CRRT_NATIVEUS_OFFSET) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
/*
CRRT_NATIVEUS_OFFSET: 
[31:12]: (reserved)
[11]: Sign bit:
0 positive offset value
1 negative offset value
[10:0]: Indicates The differnet value of native_bt_cnt - ref_bt_cnt, the unit is ust: Unsigned number.range:0-1249us.
This value is updated at end of each native_bt_clk update procedre

*/

#define REG_BB_CRRT_REFCLK                                 (BB_BASE + 0x234)
/*
CRRT_REFCLK: 
[31:28]: (reserved)
[27:0]: Indicates The curret value ref_bt_clk, the unit is half slot: Unsigned number.


*/

#define REG_BB_CRRT_REFUS                                  (BB_BASE + 0x238)
/*
CRRT_REFUS: 
[31:11]: (reserved)
[10:0]: Indicates current value of ref_bt_cnt, the unit is us: Unsigned number.range:0-1249us.


*/

#define REG_BB_REF_TIM_CLK                                 (BB_BASE + 0x23c)
/*
REF_TIM_CLK: 
[31:28]: (reserved)
[27:0]: set The value of ref_bt_clk to which ref_bt_clk counter will reach, interrupt will be generated at this pointer. the unit is half slot: Unsigned number.

*/

#define REG_BB_REF_TIM_US                                  (BB_BASE + 0x240)
/*
REF_TIM_US: 
[31:11]: (reserved)
[10:0]: set The value of ref_bt_cnt to which ref_bt_cnt counter will reach, interrupt will be generated at this pointer. the unit is us: Unsigned number.range:0-1249us.

*/

#define REG_BB_REF_TIM_CTRL                                (BB_BASE + 0x244)
#define REG_BB_REF_TIM_CTRL_ref_time_en                    ((((REG_BB_REF_TIM_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
REF_TIM_CTRL: 
[31:1]: (reserved)
[0]: indicate that REF_TIM_CLK and REF_TIM_US are already configured.
NOTE: REF_TIM_CLK and REF_TIM_US must be configured before ref_time_en = 1. 
ref_time_en will be set to 0 by SW during ref_tim_intr ISR.


*/

#define REG_BB_AUX_TIM                                     (BB_BASE + 0x248)
/*
AUX_TIM: 
[31:14]: (reserved)
[13:0]: The AUX_TIMER will allow durations, in multiples of four slots (aligned with its own Native Bluetooth clock), to be timed.
*/


#define REG_BB_REF_TIM1_CLK                                 (BB_BASE + 0x25c)
/*
REF_TIM1_CLK: 
[31:28]: (reserved)
[27:0]: set The value of ref_bt_clk to which ref_bt_clk counter will reach, interrupt will be generated at this pointer. the unit is half slot: Unsigned number.

*/

#define REG_BB_REF_TIM1_US                                  (BB_BASE + 0x260)
/*
REF_TIM1_US: 
[31:11]: (reserved)
[10:0]: set The value of ref_bt_cnt to which ref_bt_cnt counter will reach, interrupt will be generated at this pointer. the unit is us: Unsigned number.range:0-1249us.

*/

#define REG_BB_REF_TIM1_CTRL                                (BB_BASE + 0x264)
#define REG_BB_REF_TIM1_CTRL_ref_time_en                    ((((REG_BB_REF_TIM1_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
REF_TIM1_CTRL: 
[31:1]: (reserved)
[0]: indicate that REF_TIM_CLK and REF_TIM_US are already configured.
NOTE: REF_TIM_CLK and REF_TIM_US must be configured before ref_time_en = 1. 
ref_time_en will be set to 0 by SW during ref_tim_intr ISR.

*/


#define REG_BB_REF_TIM2_CLK                                 (BB_BASE + 0x268)
/*
REF_TIM2_CLK: 
[31:28]: (reserved)
[27:0]: set The value of ref_bt_clk to which ref_bt_clk counter will reach, interrupt will be generated at this pointer. the unit is half slot: Unsigned number.

*/

#define REG_BB_REF_TIM2_US                                  (BB_BASE + 0x26C)
/*
REF_TIM2_US: 
[31:11]: (reserved)
[10:0]: set The value of ref_bt_cnt to which ref_bt_cnt counter will reach, interrupt will be generated at this pointer. the unit is us: Unsigned number.range:0-1249us.

*/

#define REG_BB_REF_TIM2_CTRL                                (BB_BASE + 0x270)
#define REG_BB_REF_TIM2_CTRL_ref_time_en                    ((((REG_BB_REF_TIM2_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
REF_TIM2_CTRL: 
[31:1]: (reserved)
[0]: indicate that REF_TIM_CLK and REF_TIM_US are already configured.
NOTE: REF_TIM_CLK and REF_TIM_US must be configured before ref_time_en = 1. 
ref_time_en will be set to 0 by SW during ref_tim_intr ISR.

*/


#define REG_BB_INTR_MASK                                   (BB_BASE + 0x274)
#define REG_BB_INTR_MASK_aux_tim_intr_mask                 ((((REG_BB_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BB_INTR_MASK_ref_tim_intr_mask                 ((((REG_BB_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_BB_INTR_MASK_ref_tim1_intr_mask                 ((((REG_BB_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_BB_INTR_MASK_ref_tim2_intr_mask                 ((((REG_BB_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))


/*
INTR_MASK: 
[31:5]: reserved
[4]: bb_tim module's aux timer reach interrupt mask signal.
1 h1: this interrupt is muted;
1 h0: this interrupt is unmasked.
[3]: bb_tim module's reference timer reach interrupt mask signal.
1 h1: this interrupt is muted;
1 h0: this interrupt is unmasked.
[2:0]: bb_tim module's adjust procedure finish interrupt mask signal.
Where, bit[0] is for drag_intr, bit[1] is used for lmp_intr, bit[2] is used forpic_intr.Each bit means that:
1 h1: this interrupt is muted;
1 h0: this interrupt is unmasked.
*/

#define REG_BB_INTR_CLR                                    (BB_BASE + 0x278)
#define REG_BB_INTR_CLR_aux_tim_intr_clr                   ((((REG_BB_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BB_INTR_CLR_ref_tim_intr_clr                   ((((REG_BB_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_BB_INTR_CLR_ref_tim1_intr_clr                   ((((REG_BB_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_BB_INTR_CLR_ref_tim2_intr_clr                   ((((REG_BB_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))


/*
INTR_CLR: 
[31:5]: reserved
[4]: bb_tim module's aux timer reach interrupt clr signal.
1 h1: this interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
[3]: bb_tim module's reference timer reach interrupt clr signal.
1 h1: this interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
[2:0]: bb_tim module's adjust procedure finish interrupt clr signal.
Where, bit[0] is for drag_intr, bit[1] is used for lmp_intr, bit[2] is used forpic_intr.Each bit means that:
1 h1: this interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
*/

#define REG_BB_INTR_STAT                                   (BB_BASE + 0x27c)
#define REG_BB_INTR_STAT_aux_tim_intr_stat                 ((((REG_BB_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BB_INTR_STAT_ref_tim_intr_stat                 ((((REG_BB_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_BB_INTR_STAT_ref_tim1_intr_stat                 ((((REG_BB_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_BB_INTR_STAT_ref_tim2_intr_stat                 ((((REG_BB_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))


/*
INTR_STAT: 
[31:5]: reserved
[4]: bb_tim module's aux timer reach interrupt stat signal.
1 h1: this interrupt is occurred;
1 h0: this intrrupt is not occurred.
[3]: bb_tim module's reference timer reach interrupt stat signal.
1 h1: this interrupt is occurred;
1 h0: this intrrupt is not occurred.
[2:0]: bb_tim module's adjust procedure finish interrupt stat signal.
Where, bit[0] is for drag_intr, bit[1] is used for lmp_intr, bit[2] is used forpic_intr.Each bit means that:
1 h1: this interrupt is occurred;
1 h0: this intrrupt is not occurred.
*/

#define REG_BB_BB_BB21_AES_SUPPT                           (BB_BASE + 0x280)
#define REG_BB_BB_BB21_AES_SUPPT_rgf_bb21_aes_support      ((((REG_BB_BB_BB21_AES_SUPPT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_BB21_AES_SUPPT: 
[31:1]: (reserved)
[0]: indicate if aes algorithm is supportted in bb21 module.
1: bb21 module supports aes algorithm
0: bb21 module do not support aes algorithm

*/

#define REG_BB_BB_BB21_AES_EN                              (BB_BASE + 0x284)
#define REG_BB_BB_BB21_AES_EN_rgf_bb21_lmp_aes_en          ((((REG_BB_BB_BB21_AES_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_BB21_AES_EN: 
[31:1]: (reserved)
[0]: indicate the packet to be rx or tx in bb21 module if use aes encription algorithm
1: use aes algorithm
0: do not use aes algorithm
*/

#define REG_BB_KXLNAPA_EN		(BB_BASE + 0x310)
#define REG_BB_KXLNAPA_EN_RGF_KXBPS_EN                     ((((REG_BB_KXLNAPA_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BB_KXLNAPA_EN_RGF_KXLNAPA_EN                   ((((REG_BB_KXLNAPA_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))

#define REG_BB_KXLNAPA_DLY		(BB_BASE + 0x314)

#define REG_BB_DAC_TIM_CTRL     (BB_BASE + 0x318) 
/*
REG_BB_DAC_TIM_CTRL: 
[31:1]: 
[0]: 1 - Enable the DAC auto enable timer
*/

#define REG_BB_DAC_TIM_CLK      (BB_BASE + 0x31C)
/*
REG_BB_DAC_TIM_CLK: 
[31:28]: 
[27:0]: DAC enable delay (tick partition)
*/

#define REG_BB_DAC_TIM_US       (BB_BASE + 0x320) 
/*
REG_BB_DAC_TIM_US: 
[31:28]: 
[10:0]: DAC enable delay (us partition)
*/
                                                                                     
#define REG_BB_ADC_TIM_CTRL      (BB_BASE + 0x324) 
/*
REG_BB_ADC_TIM_CTRL: 
[31:1]: 
[0]: 1 - Enable the ADC auto enable timer
*/

#define REG_BB_ADC_TIM_CLK       (BB_BASE + 0x328) 
/*
REG_BB_ADC_TIM_CLK: 
[31:28]: 
[27:0]: ADC enable delay (tick partition)
*/

#define REG_BB_ADC_TIM_US        (BB_BASE + 0x32C) 
/*
REG_BB_ADC_TIM_US: 
[31:28]: 
[10:0]: ADC enable delay (us partition)
*/

#endif

/**********************************************
 * AES Register Definitions
 **********************************************/
 #if AES_BASE
	
#define REG_AES_BB_AES_SKD_0                               (AES_BASE + 0x290)
/*
BB_AES_SKD_0: aes skd
[31:0]: aes skd
*/
	
#define REG_AES_BB_AES_SKD_1                               (AES_BASE + 0x294)
/*
BB_AES_SKD_1: aes skd
[31:0]: aes skd
*/
	
#define REG_AES_BB_AES_SKD_2                               (AES_BASE + 0x298)
/*
BB_AES_SKD_2: aes skd
[31:0]: aes skd
*/
	
#define REG_AES_BB_AES_SKD_3                               (AES_BASE + 0x29c)
/*
BB_AES_SKD_3: aes skd
[31:0]: aes skd
*/
	
#define REG_AES_BB_AES_LTK_0                               (AES_BASE + 0x2a0)
/*
BB_AES_LTK_0: aes ltk
[31:0]: aes ltk
*/
	
#define REG_AES_BB_AES_LTK_1                               (AES_BASE + 0x2a4)
/*
BB_AES_LTK_1: aes ltk
[31:0]: aes ltk
*/
	
#define REG_AES_BB_AES_LTK_2                               (AES_BASE + 0x2a8)
/*
BB_AES_LTK_2: aes ltk
[31:0]: aes ltk
*/
	
#define REG_AES_BB_AES_LTK_3                               (AES_BASE + 0x2ac)
/*
BB_AES_LTK_3: aes ltk
[31:0]: aes ltk
*/
	
#define REG_AES_BB_AES_CTRL                                (AES_BASE + 0x2c0)
#define REG_AES_BB_AES_CTRL_BB_DPT_HW_WR_MIC               ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_AES_BB_AES_CTRL_BB_DPT_HW_EN                   ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_AES_BB_AES_CTRL_BB_AES_KEY_INIT_EN             ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AES_BB_AES_CTRL_BB_AES_DPT_EN                  ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AES_BB_AES_CTRL_BB_AES_EPT_EN                  ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AES_BB_AES_CTRL_BB_AES_DMA_EN                  ((((REG_AES_BB_AES_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_AES_CTRL: 
[31:6]: 
[5]: 1'b1: hw automatically write the mic
[4]: 1'b1: hw automatically dpt start trig based on crc result
[3]: key init enable
[2]: decryption enable
[1]: encryption enable
[0]: aes dma enable
*/
	
#define REG_AES_BB_AES_KEY_0                               (AES_BASE + 0x2c4)
/*
BB_AES_KEY_0: bb aes key after encryption
[31:0]: bb aes key after encryption
*/
	
#define REG_AES_BB_AES_KEY_1                               (AES_BASE + 0x2c8)
/*
BB_AES_KEY_1: bb aes key after encryption
[31:0]: bb aes key after encryption
*/
	
#define REG_AES_BB_AES_KEY_2                               (AES_BASE + 0x2cc)
/*
BB_AES_KEY_2: bb aes key after encryption
[31:0]: bb aes key after encryption
*/
	
#define REG_AES_BB_AES_KEY_3                               (AES_BASE + 0x2d0)
/*
BB_AES_KEY_3: bb aes key after encryption
[31:0]: bb aes key after encryption
*/
	
#define REG_AES_BB_AES_NONCE_0                             (AES_BASE + 0x2d4)
/*
BB_AES_NONCE_0: aes nonce
[31:0]: aes nonce
*/
	
#define REG_AES_BB_AES_NONCE_1                             (AES_BASE + 0x2d8)
/*
BB_AES_NONCE_1: aes nonce
[31:0]: aes nonce
*/
	
#define REG_AES_BB_AES_NONCE_2                             (AES_BASE + 0x2dc)
/*
BB_AES_NONCE_2: aes nonce
[31:0]: aes nonce
*/
	
#define REG_AES_BB_AES_NONCE_3                             (AES_BASE + 0x2e0)
/*
BB_AES_NONCE_3: aes nonce
[31:0]: aes nonce
*/
	
#define REG_AES_BB_TX_PDU_MIC_EPT                          (AES_BASE + 0x2e4)
/*
BB_TX_PDU_MIC_EPT: tx pdu MIC ept
[31:0]: tx pdu MIC ept
*/
	
#define REG_AES_BB_RX_PDU_MIC_EPT                          (AES_BASE + 0x2e8)
/*
BB_RX_PDU_MIC_EPT: rx pdu MIC ept
[31:0]: rx pdu MIC ept
*/
	
#define REG_AES_BB_RX_PDU_MIC                              (AES_BASE + 0x2ec)
/*
BB_RX_PDU_MIC: rx pdu MIC,when BB_DPT_HW_WR_MIC=1,it can be write, or read only
[31:0]: rx pdu MIC,when BB_DPT_HW_WR_MIC=1,it can be write, or read only
*/
	
#define REG_AES_BB_LENGTH                                  (AES_BASE + 0x2f0)
/*
BB_LENGTH: 
[31:16]: 
[15:0]: payload length
*/
	
#define REG_AES_B1_BYTE                                    (AES_BASE + 0x2f4)
/*
B1_BYTE: 
[31:24]: 
[23:0]: B1 byte of aes
*/
	
#define REG_AES_BB_AES_INT_MASK                            (AES_BASE + 0x300)
#define REG_AES_BB_AES_INT_MASK_AES_INT_DPT_ERR_MASK       ((((REG_AES_BB_AES_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AES_BB_AES_INT_MASK_AES_INT_INIT_MASK          ((((REG_AES_BB_AES_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AES_BB_AES_INT_MASK_AES_INT_EPT_MASK           ((((REG_AES_BB_AES_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AES_BB_AES_INT_MASK_AES_INT_DPT_MASK           ((((REG_AES_BB_AES_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_AES_INT_MASK: 
[31:4]: 
[3]: 
[2]: 
[1]: 
[0]: 
*/
	
#define REG_AES_BB_AES_INT_CLR                             (AES_BASE + 0x304)
#define REG_AES_BB_AES_INT_CLR_AES_INT_DPT_ERR_CLR         ((((REG_AES_BB_AES_INT_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AES_BB_AES_INT_CLR_AES_INT_INIT_CLR            ((((REG_AES_BB_AES_INT_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AES_BB_AES_INT_CLR_AES_INT_EPT_CLR             ((((REG_AES_BB_AES_INT_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AES_BB_AES_INT_CLR_AES_INT_DPT_CLR             ((((REG_AES_BB_AES_INT_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_AES_INT_CLR: 
[31:4]: 
[3]: 
[2]: 
[1]: 
[0]: 
*/
	
#define REG_AES_BB_AES_INT_STATUS                          (AES_BASE + 0x308)
#define REG_AES_BB_AES_INT_STATUS_AES_INT_DPT_ERR_STATUS   ((((REG_AES_BB_AES_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AES_BB_AES_INT_STATUS_AES_INT_INIT_STATUS      ((((REG_AES_BB_AES_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AES_BB_AES_INT_STATUS_AES_INT_EPT_STATUS       ((((REG_AES_BB_AES_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AES_BB_AES_INT_STATUS_AES_INT_DPT_STATUS       ((((REG_AES_BB_AES_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_AES_INT_STATUS: debug use
[31:4]: debug use
[3]: 
[2]: 
[1]: 
[0]: 
*/

#endif

#endif/*_VREG_CONTROLLER_DEF_H_*/

