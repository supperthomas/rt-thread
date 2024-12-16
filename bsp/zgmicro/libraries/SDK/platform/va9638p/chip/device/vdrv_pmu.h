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
#ifndef _VPMU_H_
#define _VPMU_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_I2C_SLAVE_ADDR      0x34

/*
 * R00: DC-DC Output Voltage Control
 * [7:2], reserved
 * [1:0] MVDC, Output voltage control register
 *          00 - 1.5V, 01 - 1.4V, 10 - 1.6V, 11 - 2.3V
 *          00 - 1.5V, 01 - 1.8V, 10 - 1.6V, 11 - 2.0V(9611SD)
 *          00 - 1.5V, 01 - 1.8V, 10 - 1.7V, 11 - 1.6V(9611SE)
 *          00 - 1.5V, 01 - 1.8V, 10 - 1.6V, 11 - 1.7V(7621A0)
 *          00 - 1.5V, 01 - 1.8V, 10 - 1.6V, 11 - 1.45V(7621S2A2)
 * Default: 0x00
 */
#define PMU_REG_MVDC            0x00

/*
 * R01: Charger Status (invalid at VA7621SA0)
 * [7:3], reserved
 * [2] BAT_LOW, When Battery voltage is lower than this threshold, BAT_Low will become high
 *          1 - battery voltage lower than threshold,
 *                  0 - battery voltage higher than threshold
 * [1] PG, Indicating if charger input voltage is higher than battery voltage:
 *      1 - charger input voltage is high enough for charging
 *          0 - charger input voltage is too low
 * [0] STAT, charging flag:
 *      1 - indicating charging, 0 - indicating no charging
 * Default: N/A
 */
#define PMU_REG_CHARGE_STATUS   0x01

/*
 * R02: Charging Termination Voltage Control
 * [7:2], reserved
 * [1:0] MCHV, Charging termination voltage control register.
 *          00 - 4.0V, 01 - 4.1V, 10 - 4.2V, 11 - 4.25V
 * Default: 0x02
 */
/*
 * R02: MCHV(2014-2-10,modify for 7621A)
 * [7:4], reserved
 * [3:2] [Mchi1, Mchi2], CC Charging mode  current control register.
 *          (Icc_chg)00 - 50mA, 01 - 200mA, 10 - 100mA(default), 11 - 300mA
 * [1:0] [Mchv1, Mchv2], Charging termination voltage control register.
 *          (Vout_ch)00 - 4.0V, 01 - 4.1V, 10 - 4.2V(default), 11 - 4.25V
 * Default: 0x0A
 */
/*
* R02: MCHV(2017-5-23,modify for 7621SDC0)
* [7:4], reserved
* [3:2] [Mchi1, Mchi2], CC Charging mode  current control register.
*      D_LPO[3]   D_LPO[2]   Mchi1  Mchi2       Icc_chg(mA)
*      0           0          0                0                  140
*      0           0          0                1                  120
*      0           0          1                0                  100 (default)
*      0           0          1                1                  90
*      0           1          0                0                  80
*      0           1          0                1                  70
*      0           1          1                0                  60
*      0           1          1                1                  50
*      1           0          0                0                  40
*      1           0          0                1                  30
*      1           0          1                0                  20
*      1           0          1                1                  300
*      1           1          0                0                  250
*      1           1          0                1                  200
*      1           1          1                0                  180
*      1           1          1                1                  160
* [1:0] [Mchv1, Mchv2], Charging termination voltage control register.
*          (Vout_ch)00 - 4.35V, 01 - 4.1V, 10 - 4.2V(default), 11 - 4.25V
* Default: 0x0A
*/
#define PMU_REG_MCHV            0x02

/*
 * R03: LDO5 Enable Control
 * [7:1], reserved
 * [0] MEN5, LDO5 enable controlling
 *          0 - disable, 1 - enable
 * Default: 0x00
 */
/*
* R03: MEN5(2014-2-10,modify for 7621A, 7621S)
* [7] Men5[7], DC-DC PWM frequency config register;
*      Men5[7]=1'b1: DC-DC PWM frequency is 1.5MHz;
*      Men5[7]=1'b0: DC-DC PWM frequency is 2.5MHz;
* [6] Men5[6], ;
*      Men5[6]&Men5[2]             meaning
*      Men5[6]="0" & Men5[2]="0"       LDOB (LDOB's Error Amplifier is shared with LDO3, High Current Power Input: VBAT)、LDO1、LDO5 work when charger is disconnected; LDO3、LDO4、LDO6 work when charger is connected.
*      Men5[6]="1" & Men5[2]="0"       Buck、LDO1、LDO5 work when charger is disconnected; LDO3、LDO4、LDO6 work when charger is connected.
*      Men5[6]="0" & Men5[2]="1"       LDOB、LDO1、LDO5 work when charger is disconnected; LDOB、LDO1、LDO5 work when charger is connected.
*      Men5[6]="1" & Men5[2]="1"       Buck、LDO1、LDO5 work when charger is disconnected; Buck、LDO1、LDO5 work when charger is connected.
* [5:4] Men5[5:4], dummy register, reserved.
* [3] Men5[3], dummy register, reserved(7621A)
* [3] Men5[3], Men5[3]="0", Charger Reset Inhibition. Men5[3]="1", Charger Reset Allowable. (7621SA)
* [2] Men5[2], charger bypass mode config register;
*      Men5[2]="1", charger bypass mode will be disabled;
*      Men5[2]="0", charger bypass mode will be enabled;
* [1] Men5[1], LDO5 operation mode control;
*      Men5[1]=1'b1 && Men5[0]=1'b1 : LDO5 enter low power mode;
*      Men5[1]=1'b0 && Men5[0]=1'b1 : LDO5 stays at normal mode;
* [0] Men5[0], LDO5 enable control register;
*       0 - disable, 1 - enable
* Default: 0xB0
*/
#define PMU_REG_MEN5            0x03

#define  BIT_PMU_REG_MEN5_DCDC   6

/*
 * R04: DC/DC PWM/PFM Mode Control
 * [7:2], reserved
 * [1] MMDC2, 0 - PWM mode for DCDC, 1 - PFM mode for DCDC
 * [0] MMDC1, 0 - DCDC decide PWM/PFM mode select automatically
 *               1 - DCDC PFM/PWM mode will be decided by Mmdc2
 * Default: 0x00
 */
#define PMU_REG_MMDC            0x04

/*
 * R05: Battery Low Detection Threshold
 * [7:4], reserved
 * [3:0] MBV- Mbv1[0], Mbv2[1], Mbv3[2], Mbv4[3] is the register signal from I2C. It is used to set Battery Low Detection threshold. When Battery voltage is lower than this threshold, BAT_Low will become high.
 *      MBV4/MBV3/MBV2/MBV1      Vbat_low threshold(unit V)
 *          0000                                        2.8
 *          0001                                        2.85
 *          0010                                        2.90
 *          0011                                        2.95
 *          0100                                        3.00
 *          0101                                        3.05
 *          0110                                        3.10
 *          0111                                        3.15
 *          1000                                        3.20
 *          1001                                        3.25
 *          1010                                        3.30
 *          1011                                        3.35
 *          1100                                        3.40(default)
 *          1101                                        3.45
 *          1110                                        3.50
 *          1111                                        3.55
 *
 *  0000 ~ 1111 - 2.8V ~ 3.55V, 0.05V per-step, 3.4V(1100) as default
 * Default: 0x0C
 */
#define PMU_REG_MBV             0x05

/*
 * R06: Coulomb Meter enable signal.
 * [7:1], reserved
 * [0] MEN_CMeter, Coulomb Meter enable signal.
 *              1: Coulomb Meter enter active mode.
 *              0: Coulomb Meter enter sleep mode (low power consumption).
 * Default: 0x00
 */
#define PMU_REG_MEN_CMeter      0x06

/*
 * R07: VD (VD0~VD4, Coulomb Meter digital output) ready signal, active low.(invalid at VA7621SA0)
 * [7:1], reserved
 * [0] termb, VD (VD0~VD4, Coulomb Meter digital output) ready signal, active low.
 *          1: VD is not ready;
 *             0: VD is ready to be read by 9613i.
 * Default: N/A
 */
#define PMU_REG_TERMB           0x07

/*
 * R08: Coulomb Meter output data.
 * [7:5], reserved
 * [4:0] VD, 00000 ~ 11111 - 2.9V ~ 4.3V,
 * Default: N/A
 */
/*
* R08: VD(2014-2-10,modify for 7621A)
* [7:4], reserved
* [3:0] Coulomb Meter output data, which demonstrate the battery voltage
*      [VD4, VD3, VD2, VD1, VD0]       VBAT (unit: V)
*          00000                    <3.0947
*          00001                    3.0947~3.4042
*          00010                    3.4042~3.4263
*          00011                    3.4263~3.4484
*          00100                    3.4484~3.4705
*          00101                    3.4705~3.4926
*          00110                    3.4926~3.5147
*          00111                    3.5147~3.5368
*          01000                    3.5368~3.5589
*          01001                    3.5589~3.5811
*          01010                    3.5811~3.6032
*          01011                    3.6032~3.6253
*          01100                    3.6253~3.6474
*          01101                    3.6474~3.6695
*          01110                    3.6695~3.6916
*          01111                    3.6916~3.7137
*          10000                    3.7137~3.7358
*          10001                    3.7358~3.7579
*          10010                    3.7579~3.7800
*          10011                    3.7800~3.8021
*          10100                    3.8021~3.8242
*          10101                    3.8242~3.8463
*          10110                    3.8463~3.8684
*          10111                    3.8684~3.8905
*          11000                    3.8905~3.9126
*          11001                    3.9126~3.9347
*          11010                    3.9347~3.9568
*          11011                    3.9568~3.9789
*          11100                    3.9789~4.0011
*          11101                    4.0011~4.0232
*          11110                    4.0232~4.0563
*          11111                    >4.0563
* Default: N/A
*/
#define PMU_REG_VD          0x08

//xy: 20131116 add registers for 9611S_E
/*
 * R09: MTRST_LOCK
 * [7:2] RESERVED
 * [1] MTrst_lock1, 1:single output, right channel
 * [0] MTrst_lock2, 1:single output, left channel
 *  Deadlock reset delay time control register.
 *  [MTrst_lock1, MTrst_lock2]           Deadlock Reset Delay Time      7621SDC0
 *      00                                        20s (default)                     7S(default)
 *      01                                        15s                               15S
 *      10                                        10s                               10S
 *      11                                        30s                               3S
 * Default: 0x00
 */
#define PMU_REG_MTRST_LOCK          0x09

/*
 * RFF: VER_SEL
 * [7:5] RESERVED
 * [4] VER_SEL_OldNew, 0: new chip (VA9611SE0), 1: old chip (VA9611SD0, VA9611F0).
 * [3:1] RESERVED
 * [0] VER_SEL_LowHigh, Version select: 0=low-end version, 1=high-end version.
 * Default: N/A
 */
#define VER_REG_SEL         0xFF
//xy: 20131116 end for 9611S_E add

//xy: 20140210 add registers for 7621A
/*
 * R0A: D_LPO1
 * [7:0] D_LPO[7:0], Dummy register.
 * Default: 0x00
 */
/*
* R0A: D_LPO1(2017-5-23,modify for 7621SDC0)
* [7] D_LPO[7], RESERVED
* [6] D_LPO[6], It is used charging enable when charger is connected. "0" enable charing; "1" disable charging.
* [5:4] D_LPO[5:4], Dummy register.
* [3:2] D_LPO[3:2], CC Charging mode  current control register.
*              This register is used together with "Mchi1" and "Mchi2".Please see the description of  "Mchi1" and "Mchi2".
* [1:0] D_LPO[1:0], 采用不同电感值时，以调节RCP_BUCK的反向电流阈值.
*                    00         电感为10uH
*                    01         电感为22uH
*                    10         电感为4.7uH
*                    11         电感为2.2uH
* Default: 0x00
*/
/*
* R0A: D_LPO1(2018-10-19,modify for 7621S2A2)
* [7] D_LPO[7], 0:  VOL2 output voltage is set to 1.2V; 1:  VOL2 output voltage is set to 1.1V.
* [6] D_LPO[6], It is used charging enable when charger is connected. "0" enable charing; "1" disable charging.
* [5] D_LPO[5], Register to set PMU ON/OFF.
*          D_LPO[5]=1, PMU will be set ON.
*          D_LPO[5]=0, PMU will be set according to PMU_EN PAD..
* [4] D_LPO[4], Dummy register.
* [3:2] D_LPO[3:2], CC Charging mode  current control register.
*              This register is used together with "Mchi1" and "Mchi2".Please see the description of  "Mchi1" and "Mchi2".
* [1:0] D_LPO[1:0], 采用不同电感值时，以调节RCP_BUCK的反向电流阈值.
*                    00         电感为10uH
*                    01         电感为22uH
*                    10         电感为4.7uH
*                    11         电感为2.2uH
* Default: 0x00
*/
#define PMU_REG_D_LPO1          0x0A

/*
 * R0B: D_LPO2
 * [7:0] D_LPO[15:8], Dummy register.
 * Default: 0x00
 */
#define PMU_REG_D_LPO2          0x0B

/*
 * R0C: LPO_EN
 * [7:1] RESERVED
 * [0] LPO_EN, Dummy register.
 * Default: 0x01
 */
#define PMU_REG_LPO_EN          0x0C

/*
 * R0D: M_OSC
 * [7:6] RESERVED
 * [5:2] M_OSC[5:2], Dummy register.
 * [1:0] M_OSC[1:0], Dummy register.
 * Default: 0x3D
 */
#define PMU_REG_M_OSC           0x0D

/*
 * R0E: MCP
 * [7:2] RESERVED
 * [1:0] Mcp[1:0], Set LDO9 and charge-pump output voltage.
 *      Mcp[1:0]    VOL9   VCP   Units
 *      00              0.9     -0.9   V (default)
 *      01              1        -1     V
 *      10              0.8     -0.8   V
 * Default: 0x00
 */
#define PMU_REG_MCP         0x0E

//20140218 add for 7621S
/*
 * R3B: INT_STATUS
 * [7:5], reserved
 * [4] BAT_LOW, When Battery voltage is lower than this threshold, BAT_Low will become high
 *          1 - battery voltage lower than threshold,
 *                  0 - battery voltage higher than threshold
 * [3] PG, Indicating if charger input voltage is higher than battery voltage:
 *      1 - charger input voltage is high enough for charging
 *          0 - charger input voltage is too low
 * [2] STAT, charging flag:
 *      1 - indicating charging, 0 - indicating no charging
 * [1] termb_inv, VD4~VD0 ready signal, active high.:
 *      1 - VD4~VD0 is ready to be read by 9613i, 0 - VD4~VD0 is not ready.
 * [0] CDC_LINE_DET, line in detect; 1=detect, 0=no detect
 * Default: N/A
 */
#define PMU_REG_INT_STATUS      0x3B

/*
 * R3C: INT_INV
 * [7:5], reserved
 * [4] INT_INV_BAT_Low, Inverse the interrupt input signal from register "BAT_Low", active high.
 * [3] INT_INV_Pg, Inverse the interrupt input signal from register "Pg", active high.
 * [2] INT_INV_Stat, Inverse the interrupt input signal from register "Stat", active high.
 * [1] INT_INV_termb_inv, Inverse the interrupt input signal from register "termb_inv", active high.
 * [0] INT_INV_LINE_DET, Inverse the interrupt input signal from register "CDC_LINE_DET", active high.
 * Default: 0x00
 */
#define PMU_REG_INT_INV         0x3C

/*
 * R3D: INT_MASK
 * [7:5], reserved
 * [4] INT_MASK_BAT_Low, Close the interrupt function from register "BAT_Low", active high.
 * [3] INT_MASK_Pg, Close the interrupt function from register "Pg", active high.
 * [2] INT_MASK_Stat, Close the interrupt function from register "Stat", active high.
 * [1] INT_MASK_termb_inv, Close the interrupt function from register "termb_inv", active high.
 * [0] INT_MASK_LINE_DET, Close the interrupt function from register "CDC_LINE_DET", active high.
 * Default: 0x1F
 */
#define PMU_REG_INT_MASK        0x3D

/*
 * RFF: VER_SEL(VA7621S_A0, but VA7621A not it)
 * [3:1] RESERVED
 * [0] VER_SEL_LowHigh, Version select: 0=low-end version, 1=high-end version.
 * Default: N/A
 */
#define VER_REG_SEL_7621S       0x54


#define PMU_MEN5_SET        0xB0    //Men5[7, 6, 2, 1] is valid bit
#define PMU_MEN5_DCDC       0xF0//if audio power by DCDC, then set to f0/70.
//xy: 20140210 end for 7621A add



//xy: 20190605 start for new VA7632B_A0

//I2CSLV  for  VA7632B_A0   v0.2;   note:I2C Register for "analog power part"：地址00H~3FH的范围，分配给analog power部分使用
/*
 * R00: Audio DC-DC output voltage setting.
 * [7:4], reserved
 * [3:0] MvdcA, Audio DC-DC output voltage setting.
 *          MvdcA[3:0]   Audio DC-DC output   Units
 *          0000               1.2                               V
 *          0001               1.225                           V
 *          0010               1.25                             V
 *          0011               1.263                           V
 *          0100               1.275                           V
 *          0101               1.288                           V
 *          0110               1.3                               V
 *          0111               1.316                           V
 *          1000               1.325                           V
 *          1001               1.35                             V
 *          1010               1.4                               V
 *          1011               1.45                             V
 *          1100               1.5                               V
 *          1101               1.6                               V
 *          1110               1.8                               V (default)
 *          1111               2.0                               V
 *
 ******new change for 7632BF3 **********************************************************************************
 * [3:0] MvdcA, Audio DC-DC output voltage setting.
 *           1000				1.1 							  V
 *           1001				1.125							V
 *           1010				1.15							 V
 *           1011				1.163							V
 *           1100				1.175							V
 *           1101				1.188							V
 *           1110				1.2 							  V (default)
 *           1111				1.216							V
 *           0000				1.225							V
 *           0001				1.25							 V
 *           0010				1.3 							  V
 *           0011				1.35							 V
 *           0100				1.4 							  V
 *           0101				1.5 							  V
 *           0110				1.7 							  V
 *           0111				1.9 							  V
 *
 ******new change for 7632BF5; VA7632B_A0_i2c_slave_v1p2_for_VA7632B_E5orF5_20200908 *******************
 * RF DC-DC output voltage setting.
 * MvdcA[3:0]  RF DC-DC output   Units 
 *           0011               1.1                                V
 *           0010               1.125                            V
 *           0001               1.15                              V
 *           0000               1.163                            V
 *           0111               1.175                            V
 *           0110               1.188                            V
 *           0101               1.2                                V
 *           0100               1.216                            V
 *           1011               1.225                            V
 *           1010               1.25                              V
 *           1001               1.3                                V
 *           1000               1.35                              V
 *           1111               1.4                                V
 *           1110               1.5                                V (default)
 *           1101               1.7                                V
 *           1100               1.9                                V
 *
  ******new change for 7632BF5; VA7632B_A0_i2c_slave_v1p3_for_VA7632B_E5orF5_20200918 *******************
  ******new change for 7632BF5; VA7632B_i2c_slave_v1p5_for_VA7632B_F5_20230928 *******************
  ******new change for 7632PMU; VA7652PMU_A0_i2c_slave_v0p2_20230928/VA7652PMU_T2_A0_i2c_slave_v1p2_20230928 *******************
 * RF DC-DC output voltage setting.
 * MvdcA[3:0]  RF DC-DC output   Units 
 *           0011				1.225						   V
 *           1011				1.225						   V
 *           0010				1.25							V
 *           1010				1.25							V
 *           0001				1.3 							 V
 *           1001				1.3 							 V
 *           0000				1.35							V
 *           1000				1.35							V
 *           0111				1.4 							 V
 *           1111				1.4 							 V
 *           0110				1.5 							 V
 *           1110				1.5 							 V (default)
 *           0101				1.7 							 V
 *           1101				1.7 							 V
 *           0100				1.8 							 V
 *           1100				1.8 							 V
 *
 * Default: 0x0E
 *
 *****for 7638A0(VA7638_A0_i2c_slave_v0p4) ***********************************************************************										 V
 *****  Digital DC-DC output voltage setting.
 *          MvdcA[3:0] 	  Digital DC-DC output		 Units 
 *          0000					  1.3									   V
 *          0001					  1.325 								   V
 *          0010					  1.35									   V
 *          0011					  1.4									   V
 *          0100					  1.45									   V
 *          0101					  1.5									   V
 *          0110					  1.3									   V
 *          0111					  1.4									   V
 *          1000					  0.85									   V
 *          1001					  0.9									   V
 *          1010					  0.95									   V
 *          1011					  1.0									   V
 *          1100					  1.1									   V
 *          1101					  1.2									   V (default)
 *          1110					  1.25									   V
 *          1111					  1.275 								   V
 *
 * Default: 0x0D
 */
#define PMU_REG763X_MVDCA           0x00

/*
 * R01: Setting CC mode charging current.
 * [7:6], reserved

 * [5:2]4-bit setting for CC mode charging current.
 * Mchi[3:0]      CC charging current    Units
 * 0000                   10                        mA
 * 0001                   20                        mA
 * 0010                   30                        mA
 * 0011                   40                        mA
 * 0100                   50                        mA
 * 0101                   60                        mA
 * 0110                   70                        mA
 * 0111                   80                        mA
 *1000                    90                        mA
 *1001                    100                      mA (default)
 *1010                    130                      mA
 *1011                    160                      mA
 *1100                    180                      mA
 *1101                    200                      mA
 *1110                    250                      mA
 *1111                    300                      mA

 * [1:0]Charging termination voltage control register.
 *[Mchv1, Mchv2] = 2'b00: Vout_ch = 4.35V
 *                               2'b01: Vout_ch = 4.1V
 *                               2'b10: Vout_ch = 4.2V (default)
 *                               2'b11: Vout_ch = 4.25V
 *
 * Default: 0x26
 */
#define PMU_REG763X_MCHV        0x01

/*
 * R02: LDO5 Enable Control
 * [7] Setting PWM frequency for BUCKRF&DCDCA.
 *  Men5[7]     Men5[5]       PWM frequency
 *  0                   0                   2.5MHz
 *  0                   1
 *  1                   0                   1.5MHz
 *  1                   1                   1 MHz
 *
 * [6] Men5[6];
 *   If Men5[6]="0" & Men5[2]="0", LDOB (LDOB's Error Amplifier is shared with LDO3, High Current Power Input: VBAT)、LDO1、LDO5 work when charger is disconnected; LDO3、LDO4、LDO6 work when charger is connected.
 *   If Men5[6]="1" &  Men5[2]="0", Buck、LDO1、LDO5 work when charger is disconnected; LDO3、LDO4、LDO6 work when charger is connected.
 *   If Men5[6]="0" &  Men5[2]="1", LDOB、LDO1、LDO5 work when charger is disconnected; LDOB、LDO1、LDO5 work when charger is connected.
 *   If Men5[6]="1" &  Men5[2]="1", Buck、LDO1、LDO5 work when charger is disconnected; Buck、LDO1、LDO5 work when charger is connected.
 *
 * 
 * LDO3 replaces BUCKRF when charger is connected; and LDO6 replaces DCDCA when charger is connected.(7632BF3)
 * 
 * 
 * [5] Men5[5], Setting PWM frequency for BUCKRF&DCDCA. See Men5[7].
 *
 * [4] Men5[4](7632BF5, VA7652PMU_A0)
 * if Men5[4]="0", RF DC-DC is turned off;
 * if Men5[4]="1", RF DC-DC is turned on.
 *
 * Men5[4](VA7652PMU_T2_A0)
 * if Men5[4]="0", BUCKRF(power supply for dig) is turned off; 
 * if Men5[4]="1", BUCKRF(power supply for dig) is turned on.
 * 
 * 
 * 
 * [3] Men5[3],
 * Men5[3]="0", VOD Enable.
 * Men5[3]="1", VOD Disable.
 *
 * [2]Men5[2],
 * Men5[2]="1", charger bypass mode will be disabled.
 * Men5[2]="0", charger bypass mode will be enabled.
 *
 *[1]Men5[1]
 *LDO5 operation mode control:
 *Men5[1]=1'b1 && Men5[0]=1'b1 : LDO5 enter low power mode;
 *Men5[1]=1'b0 && Men5[0]=1'b1 : LDO5 stays at normal mode;
  *
 *[1]Men5[1](VA7652PMU_T2_A0)
 *LDO5 operation mode control:
 *Men5[1]=1'b1 && Men5[0]=1'b0 : LDO5 enter low power mode;
 *Men5[1]=1'b0 && Men5[0]=1'b0 : LDO5 stays at normal mode;;
 *
 * [0] Men5[0], LDO5 enable control register.
 *1'b0: disable LDO5
 *1'b1: enable LDO5
 *
 *(VA7652PMU_T2_A0)
 * [0] Men5[0], LDO5 enable control register.
 *1'b0: enable LDO5
 *1'b1: disable LDO5
  *
 * Default: 0xB0
 */
#define PMU_REG7632X_MEN5           0x02

/*
 * R03: DC/DC PWM/PFM Mode Control
 * [7:3], reserved
 * [2] Reset_Mode: it is used to set reset mode or shutdown mode when PMU_EN  goes low.
 *If Reset_Mode=”1”, VA7633 will generate a reset pulse (POR signal) to reset SOC chip when PMU_EN goes low. The low-level time of the reset pulse is same as the low-level time of power-on reset.
 *If Reset_Mode=”0”, VA7633 will shut down the system and all power supplies when PMU_EN goes low.
 *
 * [1] Mmdc2,  "0" means PWM mode for RF&Dig DCDC.   "1" means PFM mode for RF&Dig DCDC
 * [0] Mmdc1, "0" means RF&Dig DCDC decide PFM/PWM mode automatically.  "1" means RF&Dig DCDC PFM/PWM mode will be decided by Mmdc2
 *
 * Default: 0x00
 */
#define PMU_REG763X_MMDC            0x03

/*
 * R04: Battery Low Detection Threshold
 * [7], reserved
 * [6:4], MVOL2; setting for LDO2 output voltage
 * 000: 0.9v
 * 001: 0.95v
 * 010: 1v
 * 011: 1.1v
 * 100: 1.15v
 * 101: 1.2v (default)
 * 110: NA
 *
 * [3:0] Mbv4[3], Mbv3[2], Mbv2[1], Mbv1[0]; Set Battery Low Detection threshold. When Battery voltage is lower than this threshold, BAT_Low will become high.
 *      Mbv4/Mbv3/Mbv2/Mvb1    VBAT_LOW Threshold    Units
 *      0000                                        2.7                         V
 *      0001                                        2.75                       V
 *      0010                                        2.8                         V
 *      0011                                        2.85                       V
 *      0100                                        2.90                       V
 *      0101                                        2.95                       V
 *      0110                                        3.00                       V
 *      0111                                        3.05                       V
 *      1000                                        3.10                       V
 *      1001                                        3.15                       V
 *      1010                                        3.20                       V
 *      1011                                        3.25                       V
 *      1100                                        3.30                       V
 *      1101                                        3.35                       V
 *      1110                                        3.40                       V (default)
 *      1111                                        3.45                       V
 *
 * Default: 0x5E
 */
#define PMU_REG763X_MBV             0x04

/*
 * R05:  Coulomb Meter enable signal.
  * [7:1], reserved
  * [0] MEN_CMeter, Coulomb Meter enable signal.
  *              1'b1: Coulomb Meter enter active mode.
  *              1'b0: Coulomb Meter enter sleep mode (low power consumption).
  * Default: 0x00
  */
#define PMU_REG763X_MEN_CMeter      0x05

/*
 * R06: Coulomb Meter output data.
 * [7:6], reserved
 * [5:0] Coulomb Meter output data, which demonstrate the battery voltage
 *      [VD5, VD4, VD3, VD2, VD1, VD0]       VBAT (unit: V)
 *          00000                    <3.0947
 *          00001                    3.0947~3.4042
 *          00010                    3.4042~3.4263
 *          00011                    3.4263~3.4484
 *          00100                    3.4484~3.4705
 *          00101                    3.4705~3.4926
 *          00110                    3.4926~3.5147
 *          00111                    3.5147~3.5368
 *          01000                    3.5368~3.5589
 *          01001                    3.5589~3.5811
 *          01010                    3.5811~3.6032
 *          01011                    3.6032~3.6253
 *          01100                    3.6253~3.6474
 *          01101                    3.6474~3.6695
 *          01110                    3.6695~3.6916
 *          01111                    3.6916~3.7137
 *          10000                    3.7137~3.7358
 *          10001                    3.7358~3.7579
 *          10010                    3.7579~3.7800
 *          10011                    3.7800~3.8021
 *          10100                    3.8021~3.8242
 *          10101                    3.8242~3.8463
 *          10110                    3.8463~3.8684
 *          10111                    3.8684~3.8905
 *          11000                    3.8905~3.9126
 *          11001                    3.9126~3.9347
 *          11010                    3.9347~3.9568
 *          11011                    3.9568~3.9789
 *          11100                    3.9789~4.0011
 *          11101                    4.0011~4.0232
 *          11110                    4.0232~4.0563
 *          11111                    >4.0563
 * Default: N/A
 */
#define PMU_REG763X_VD          0x06

/*
 * R07: MTRST_LOCK
 * [7:2] RESERVED
 * [1] MTrst_lock1, 1:single output, right channel
 * [0] MTrst_lock2, 1:single output, left channel
 *  Deadlock reset delay time control register.
 *  [MTrst_lock1, MTrst_lock2]           Deadlock Reset Delay Time
 *      2'b00                                                 7s (default)
 *      2'b01                                             15s
 *      2'b10                                             10s
 *      2'b11                                             3s
 * Default: 0x00
 */
#define PMU_REG763X_MTRST_LOCK          0x07

/*
* R08: D_LPO1
* [7] D_LPO[7], CLK_1P5M enable signal. Active low.
* [6] D_LPO[6], dummy register.
* [5] D_LPO[5], if D_LPO[8]=1,  D_LPO[5]="1", detect VBAT ;D_LPO[5]="0", detect VCHG.
*
* [4] D_LPO[4], dummy register.
* [3] D_LPO[3], 调节OCP_PFM阈值电流，具体如下：
*      BUCKRF:
*      D_LPO<3>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     67mA                      42mA
*      1                     115mA                    42mA
*      DCDCA:
*      D_LPO<3>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     77mA                      46mA
*      1                     109mA                    46mA
*
*****7632BF3***************************************************
* [4] D_LPO[4], 调节DCDCA的OCP_PFM阈值电流，具体如下：
*      D_LPO<4>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     77mA                      46mA
*      1                     109mA                    46mA.
* [3] D_LPO[3], 调节BUCKRF的OCP_PFM阈值电流，具体如下：
*      D_LPO<3>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     67mA                      42mA
*      1                     115mA                    42mA
*
* [2] D_LPO[2], charger OVP enable signal.  Active high.
* [1:0] D_LPO[1:0], charger OVP voltage setting.
*          D_LPO<1>  D_LPO<0>    charger OVP voltage
*           0                    0                     6.5V
*           0                    1                     7.2V
*           1                    0                     8V
*           1                    1                     7.2V
*
*****7632BF5***************************************************
* [7:6] D_LPO[7:6], dummy register.
* [5] D_LPO[5], if D_LPO[8]=1,  D_LPO[5]="1", detect VBAT ;
* 				D_LPO[5]="0", detect VCHG.
* [4] D_LPO[4], 调节DCDCA的OCP_PFM阈值电流，具体如下：
*      D_LPO<4>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     77mA                      46mA
*      1                     109mA                    46mA.
* [3] D_LPO[3], 调节BUCKRF的OCP_PFM阈值电流，具体如下：
*      D_LPO<3>     PFM_OCP(PFM)     PFM_OCP(PWM)
*      0                     67mA                      42mA
*      1                     115mA                    42mA
*
* [2] D_LPO[2], charger OVP enable signal.  Active high.
* [1:0] D_LPO[1:0], charger OVP voltage setting.
*          D_LPO<1>  D_LPO<0>    charger OVP voltage
*           0                    0                     6.5V
*           0                    1                     7.2V
*           1                    0                     8V
*           1                    1                     7.2V

* Default: 0x00
*/
#define PMU_REG763X_D_LPO1          0x08

/*
 * R09: D_LPO2
 * [7:5] D_LPO[15:13], setting for BUCKRF RCP current.
 *      D_LPO<15:13>   RCP current(L=4.7uH)   RCP current(L=2.2uH)   RCP current(L=1uH)
 *      000                       14.2mA
 *      100                       20.6 mA
 *      010                       26.6 mA
 *      110                                                                  21.1 mA
 *      001                                                                  26.4 mA
 *      101                                                                  31.5 mA
 *      011                                                                                                             28.9 mA
 *      111                                                                                                             35.7 mA
 * [4:2] D_LPO[12:10], setting for DCDCA RCP current.
 *      D_LPO<12:10>    RCP current(L=4.7uH)    RCP current(L=2.2uH)    RCP current(L=1uH)
 *      000                          14mA
 *      100                           20.5 mA
 *      010                           26.3 mA
 *      110                                                                         20.5 mA
 *      001                                                                         25.5 mA
 *      101                                                                         29.8 mA
 *      011                                                                                                                 --
 *      111                                                                                                                 --
 * [1] D_LPO[9], Dummy register.
 * [0] D_LPO[8], If D_LPO[8]="1", this ADC is used to detect VCHG or VBAT according to D_LPO[5];
 *          If D_LPO[8]="0", this ADC is used to detect AIO_ADC.
 * Default: 0x00
 */
#define PMU_REG763X_D_LPO2          0x09

/*
 * R0A:  RW_DUM1
 * [7:4]RW_DUM1[7:4], Dummy register.
 * [3:0]RW_DUM1[3:0], Dummy register.
 * Default: 0xF0
 */
#define PMU_REG763X_RW_DUM1         0x0A

/*
 * R0B: MCP
 * [7:2] RESERVED
 * [1] Mcp[1], Setting for charging time.
 *  Mcp[1]=0, charging time=1.8h.
 *  Mcp[1]=1, charging time=3.6h.
 * [0] Mcp[0], Enable signal for charging time.
 *  1'b0 enable charger timing.
 *  1'b1 disable charger timing.
 * Default: 0x00
 */
#define PMU_REG763X_MCP             0x0B

/*
 * R0C: CHG_STOP
 * [7:1] RESERVED
 * [0] CHG_Stop,
 *  CHG_Stop ="1" will stop charger charging current.
 *  CHG_Stop ="0" will not stop charger charging current.
 * Default: 0x00
 */
#define PMU_REG763X_CHG_STOP        0x0C

/*
 * R0D: DCA_EN
 * [7:1] RESERVED
 * 
 * [0] DCA_en,
 *  DCA_en="0", audio DC-DC will be turned off.
 *  DCA_en="1", audio DC-DC will be turned on.
 * 
 * [0] DCA_en(VA7652PMU_T2_A0),
 * DCA_en="0", DCDCA(power supply for RF) will be turned off.
 * DCA_en="1", DCDCA(power supply for RF) will be turned on.
 
 * Default: 0x01
 */
#define PMU_REG763X_DCA_EN          0x0D

/*
 * R0E: PMU_ON
 * [7:1] RESERVED
 * [0] pmu_on, Register to set PMU ON/OFF.
 *  pmu_on="1", PMU will be set ON.
 *  pmu_on="0", PMU will be set OFF.
 * Default: 0x01
 */
#define PMU_REG763X_PMU_ON          0x0E

/*
 * R0F: PMU_SEL
 * [7:1] RESERVED
 * [0] pmu_sel, Register to set mode of pmu_en_out.
 *  pmu_sel ="1", pmu_en_out operates according to the register (pmu_on).
 *  pmu_sel ="0", pmu_en_out operates according to the port signal (PMU_EN).
 * Default: 0x00
 */
#define PMU_REG763X_PMU_SEL         0x0F

/*
 * R10: MV5
 * [7:2] RESERVED
 * [1] Mv5[1],
 * [0] Mv5[0],
 * LDO5 output voltage setting.
 *  Mv5[1:0]      VOL5      Units
 *  00                2.0             V
 *  01                1.9             V
 *  10                1.8             V (default)
 *  11                2.1            V
 * Default: 0x02
 */
#define PMU_REG763X_MV5             0x10

/*
 * R11: MIT
 * [7:1] RESERVED
 * [0] Mit, Register to set mode of pmu_en_out.
 *  Mit="0", charging termination current is set to 10%*ICH;
 *  Mit="1", charging termination current is set to 5%*ICH.
 * Default: 0x00
 */
#define PMU_REG763X_MIT             0x11

/*
 * R12: MMDCA
 * [7:2] RESERVED
 * [1] MmdcA2,
 *  "0" means PWM mode for Audio DCDC.
 *  "1" means PFM mode for Audio DCDC.
 * [0] MmdcA1,
 *  "0" means Audio DCDC decide PFM/PWM mode automatically.
 *  "1" means Audio DCDC PFM/PWM mode will be decided by MmdcA2.
 * Default: 0x00
 */
#define PMU_REG763X_MMDCA           0x12

/*
 * R13: MV2
 * [7:1] RESERVED
 * [0] MV2, Set LDO2 bypass mode.
 *  MV2="1", LDO2 works in bypass mode (Vout=Vin);
 *  MV2="0", LDO2 works in normal mode.
 * Default: 0x00
 */
#define PMU_REG763X_MV2             0x13

/*
 * R14: MOTP
 * [7:2] RESERVED
 * [1] Motp[1],
 * [0] Motp[0],
 * Two bits for OTP (Over-Temperature Protection) threshold programming.
 *  Motp[1:0]   TH   TL   hysteresis
 *  00            140   100    40 (default)
 *  01            120   90      30
 *  10            100   80      20
 *  11             80     60      20
 * Default: 0x02
 */
#define PMU_REG763X_MOTP            0x14

/*
 * R15: RD_DUM1
 * [7:2] RD_DUM1[5:0], dummy register.
 * [1] CVCHG,
 *  CVCHG="1" means to enter the CVCHG mode.
 *  CVCHG="0" means to exit the CVCHG mode.
 * [0] PRECHG,
 *  PRECHG="1" means to enter the PRECHG mode.
 *  PRECHG="0" means to exit the PRECHG mode.
 * Default: N/A
 */
#define PMU_REG763X_RD_DUM1         0x15

/*
 * R16: RD_DUM2
 * [7:0] RD_DUM2[7:0], dummy register.
 * Default: N/A
 */
#define PMU_REG763X_RD_DUM2         0x16

/*
 * R17: MEN_AIOADC
 * [7:1] RESERVED
 * [0] MEN_AIOADC, Turn on AIO_ADC, active high.
 * Default: 0x00
 */
#define PMU_REG763X_MEN_AIOADC      0x17

/*
 * R18: TERM_AIOADC
 * [7:1] RESERVED
 * [0] term_AIOADC, "term_AIOADC" is AIO_ADC output to indicate data ready.
 *              1: data ready,   0: data not ready
 * Default: N/A
 */
#define PMU_REG763X_TERM_AIOADC     0x18

/*
 * R19: AIO_DATA1
 * [7:0] AIO_Data[7:0], AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
 * Default: N/A
 */
#define PMU_REG763X_AIO_DATA1       0x19

/*
 * R1A: AIO_DATA2
 * [7:2] RESERVED
 * [1:0] AIO_Data[9:8], AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
 * Default: N/A
 */
#define PMU_REG763X_AIO_DATA2       0x1A

/*
 * R1B: MVDC
 * [7:4], reserved
 * [3:0]Mvdc[3:0],  RF/Digital DC-DC output voltage setting.
 *          Mvdc[3:0]    RF/Digital DC-DC output    Units
 *          0000                        0.85                        V
 *          0001                        0.9                          V
 *          0010                        0.95                        V
 *          0011                        1.0                          V
 *          0100                        1.1                          V
 *          0101                        1.2                          V
 *          0110                        1.25                        V
 *          0111                        1.275                      V
 *          1000                        1.3                          V
 *          1001                        1.325                      V
 *          1010                        1.35                        V
 *          1011                        1.4                          V
 *          1100                        1.45                        V
 *          1101                        1.5                          V (default)
 *          1110                        1.6                          V
 *          1111                        1.7                          V
 *
 ******new change for 7632BF5; VA7632B_A0_i2c_slave_v1p2_for_VA7632B_E5orF5_20200908 *******************
 * [3:0]Digital DC-DC output voltage setting.
 *          Mvdc[3:0]    Digital DC-DC output    Units 
 *          1000                        0.85                       V
 *          1001                        0.9                         V
 *          1010                        0.95                       V
 *          1011                        1.0                         V
 *          1100                        1.1                         V
 *          1101                        1.2                         V (default)
 *          1110                        1.25                       V
 *          1111                        1.275                     V
 *          0000                        1.3                         V
 *          0001                        1.325                     V
 *          0010                        1.35                       V
 *          0011                        1.4                         V
 *          0100                        1.45                       V
 *          0101                        1.5                         V
 *          0110                        1.6                         V
 *          0111                        1.7                         V
 *
 ******new change for 7632BF5; VA7632B_A0_i2c_slave_v1p3_for_VA7632B_E5orF5_20200918 *******************
 ******new change for 7632BF5; VA7632B_i2c_slave_v1p5_for_VA7632B_F5_20230928 *******************
 ******new change for 7632PMU; VA7652PMU_A0_i2c_slave_v0p2_20230928/VA7652PMU_T2_A0_i2c_slave_v1p2_20230928 *******************
 * [3:0]Digital DC-DC output voltage setting.
 *          Mvdc[3:0]    Digital DC-DC output    Units 
 *          1000                        0.85                       V
 *          1001                        0.9                         V
 *          1010                        0.95                       V
 *          1011                        1.0                         V
 *          1100                        1.1                         V
 *          1101                        1.2                         V (default)
 *          1110                        1.25                       V
 *          1111                        1.275                     V
 *          0000                        1.3                         V
 *          0110                        1.3                        V                                                                                                      
 *          0001                        1.325                     V
 *          0010                        1.35                       V
 *          0011                        1.4                         V
 *          0100                        1.45                       V
 *          0101                        1.5                         V
 *
 * Default: 0x0D
 *
 *****for 7638A0(VA7638_A0_i2c_slave_v0p4) ***********************************************************************
 *****  RF/Audio DC-DC output voltage setting.
 *          Mvdc[3:0]		 RF/Audio DC-DC output			 Units 
 *          0000					   1.35 										 V
 *          0001					   1.3											 V
 *          0010					   1.25 										 V
 *          0011					   1.225										 V
 *          0100					   1.8											 V
 *          0101					   1.7											 V
 *          0110					   1.5											 V
 *          0111					   1.4											 V
 *          1000					   1.35 										 V
 *          1001					   1.3											 V
 *          1010					   1.25 										 V
 *          1011					   1.225										 V
 *          1100					   1.8											 V
 *          1101					   1.7											 V 
 *          1110					   1.5											 V(default)
 *          1111					   1.4											 V
 *
 * Default: 0x0E
 */
#define PMU_REG763X_MVDC            0x1B


//I2CSLV  for  VA7632B_A0   v0.2;   note:Others:??F0H~
/*
 * RF0: I2C_TEST
 * [7:0] i2c_test[7:0], used for i2c r/w test
 * Default: 0x00
 */
#define PMU_REG763X_I2C_TEST            0xF0

/*
 * RF1: VER_SEL
 * [7:2] RESERVED
 * [1] VER_SEL2,
 * [0] VER_SEL1,
 * [VER_SEL2, VER_SEL1] Version indicator: 0=low-end version, 1=high-end version.
 * Default: N/A
 */
#define PMU_REG763X_VER_SEL             0xF1

/*
 * RF2: CHIP_ID0
 * [7:0] chip_id0[7:0], chip_id0, 8'h76
 * Default: 0x00
 */
#define PMU_REG763X_CHIP_ID0            0xF2

/*
 * RF3: CHIP_ID1
 * [7:0] chip_id1[7:0], chip_id1, 8'h32
 * Default: 0x00
 */
#define PMU_REG763X_CHIP_ID1            0xF3

/*
 * RF4: CHIP_ID2
 * VA7632  [7:0] chip_id2[7:0], chip_id2, 8'hba
 * VA7652  [7:0] chip_id2[7:0], chip_id2, 8'hcc; high "C" means"pmu", low "C" means"TSMC", 
 * Default: 0x00
 */
#define PMU_REG763X_CHIP_ID2            0xF4

/*
 * RF5: CHIP_ID3
 * [7:0] chip_id3[7:0], version for metal change, 8'hxx
 *  version for metal change:
 *VA7632B_E1: 8'b0000_0101
 *VA7632B_F1: 8'b0000_0110
 *
 *VA7632B_E2: 8'b0000_1010
 * VA7632B_F2: 8'b0000_1011
 * 
 * VA7632B_E3: 8'b0000_1100
 * VA7632B_F3: 8'b0000_1101
 *
 * VA7632B_E5: 8'b0001_0001
 * VA7632B_F5: 8'b0001_0010
 *
 * VA7638_A0: 8'b0000_0001(9648c)
 * VA7652PMU CSMC: 8'h01;
 * VA7652PMU TSMC: 8'ha0;
 * chip_id3: 高位"a"表示不同功能中的A版本，低位"0"表示非metal change版本注意: 此地址寄存器需要Layout team在版图中进行修改。
 *   
 * Default: 0x00
 */
#define PMU_REG763X_CHIP_ID3            0xF5


//xy: 20190605 end for new VA7632B_A0


//xy: 20191004 start for 7621I2S
#define PMU_REG_7621I2S_PO_DUM2                   0x0C
#define PMU_REG_7621I2S_MEN_AIOADC                0x0D
#define PMU_REG_7621I2S_TERM_AIOADC               0x0E
#define PMU_REG_7621I2S_AIO_DATA1                 0x70
#define PMU_REG_7621I2S_AIO_DATA2                 0x71
//xy: 20191004 end for 7621I2S



//xy: 20231003 start for VA7652PMU_T2_A0(CDC_VERSION_7652PMU_TSMC, not at CDC_VERSION_7652PMU_CSMC&VA7652PMU_A0)
/*
 * R25: OTP_DUMMY
 * [7:4] RESERVED
 * [3] OTP_dummy[3], final_trim_reg[3].
 * [2] OTP_dummy[2], final_trim_reg[2].
 * [1] OTP_dummy[1], final_trim_reg[1].
 * [0] OTP_dummy[0], final_trim_reg[0].
 *  OTP_dummy[] for some pum chip special function select. 
 * Default: 0x00
 */
#define PMU_REG7652_OTP_DUMMY            0x25

//xy: 20231003 end for VA7652PMU_T2_A0



#define PMU_DC_OUTVOL_1_5V  0x00
#define PMU_DC_OUTVOL_1_8V  0x01
#define PMU_DC_OUTVOL_1_6V  0x02
#define PMU_DC_OUTVOL_2_0V  0x03
#define VPMU_IsValidDCOutVol(Vol) (((Vol) == PMU_DC_OUTVOL_1_5V) || \
                                   ((Vol) == PMU_DC_OUTVOL_1_8V) || \
                                   ((Vol) == PMU_DC_OUTVOL_1_6V) || \
                                   ((Vol) == PMU_DC_OUTVOL_2_0V))

#define PMU_CHARGE_TERMVOL_4_0V     0x00
#define PMU_CHARGE_TERMVOL_4_1V     0x01
#define PMU_CHARGE_TERMVOL_4_2V     0x02
#define PMU_CHARGE_TERMVOL_4_25V    0x03
#define VPMU_IsValidChargeTerminateVol(Vol) (((Vol) == PMU_CHARGE_TERMVOL_4_0V) || \
                                             ((Vol) == PMU_CHARGE_TERMVOL_4_1V) || \
                                             ((Vol) == PMU_CHARGE_TERMVOL_4_2V) || \
                                             ((Vol) == PMU_CHARGE_TERMVOL_4_25V))

#define PMU_CHARGE_CURRENT_50_mA    0x00
#define PMU_CHARGE_CURRENT_200_mA   0x01
#define PMU_CHARGE_CURRENT_100_mA   0x02
#define PMU_CHARGE_CURRENT_300_mA   0x03
#define VPMU_IsValidChargeCurrent(Cur) (((Cur) == PMU_CHARGE_CURRENT_50_mA) || \
                                        ((Cur) == PMU_CHARGE_CURRENT_200_mA) || \
                                        ((Cur) == PMU_CHARGE_CURRENT_100_mA) || \
                                        ((Cur) == PMU_CHARGE_CURRENT_300_mA))

#define PMU_LDO9_OUTVOL_0_9V    0x00
#define PMU_LDO9_OUTVOL_1_0V    0x01
#define PMU_LDO9_OUTVOL_0_8V    0x02
#define VPMU_IsValidMcpOutVol(Vol) (((Vol) == PMU_LDO9_OUTVOL_0_9V) || \
                                    ((Vol) == PMU_LDO9_OUTVOL_1_0V) || \
                                    ((Vol) == PMU_LDO9_OUTVOL_0_8V))

#define PMU_DCDC_MODE_AUTO          0x00
#define PMU_DCDC_MODE_PWM           0x01
#define PMU_DCDC_MODE_PFM           0x03
#define VPMU_IsValidDCDCMode(Mode) (((Mode) == PMU_DCDC_MODE_AUTO) || \
                                    ((Mode) == PMU_DCDC_MODE_PWM) || \
                                    ((Mode) == PMU_DCDC_MODE_PFM))


#define PMU_BATT_LOWVOL_2_80V   0x00
#define PMU_BATT_LOWVOL_2_85V   0x01
#define PMU_BATT_LOWVOL_2_90V   0x02
#define PMU_BATT_LOWVOL_2_95V   0x03
#define PMU_BATT_LOWVOL_3_00V   0x04
#define PMU_BATT_LOWVOL_3_05V   0x05
#define PMU_BATT_LOWVOL_3_10V   0x06
#define PMU_BATT_LOWVOL_3_15V   0x07
#define PMU_BATT_LOWVOL_3_20V   0x08
#define PMU_BATT_LOWVOL_3_25V   0x09
#define PMU_BATT_LOWVOL_3_30V   0x0A
#define PMU_BATT_LOWVOL_3_35V   0x0B
#define PMU_BATT_LOWVOL_3_40V   0x0C
#define PMU_BATT_LOWVOL_3_45V   0x0D
#define PMU_BATT_LOWVOL_3_50V   0x0E
#define PMU_BATT_LOWVOL_3_55V   0x0F
#define VPMU_IsValidBattLowVol(Vol) (((Vol) == PMU_BATT_LOWVOL_2_80V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_2_85V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_2_90V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_2_95V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_00V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_05V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_10V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_15V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_20V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_25V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_30V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_35V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_40V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_45V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_50V) || \
                                     ((Vol) == PMU_BATT_LOWVOL_3_55V))


#define PMU_MTRST_LOCK_20S  0x00
#define PMU_MTRST_LOCK_15S  0x01
#define PMU_MTRST_LOCK_10S  0x02
#define PMU_MTRST_LOCK_30S  0x03
#define VPMU_IsValidPowerRstDelay(DelaySel) (((DelaySel) == PMU_MTRST_LOCK_20S)|| \
                                             ((DelaySel) == PMU_MTRST_LOCK_15S) || \
                                             ((DelaySel) == PMU_MTRST_LOCK_10S) || \
                                             ((DelaySel) == PMU_MTRST_LOCK_30S))


#if 1
/* charger status bit mask, for SYS_PWR_SIMULATE_BATTERY and 9611*/
#define PMU_CHARGER_STAT_MASK           0x01
#define PMU_CHARGER_PG_MASK             0x02
#define PMU_CHARGER_BATLOW_MASK         0x04
#endif

/* charger status bit mask, for 7621S+  isr, set PMU_REG_INT_MASK, 1==disable */
#define PMU_INT_STATUS_LINE_DET_MASK        0x01    //bit[0], CDC_LINE_DET
#define PMU_INT_STATUS_TERMB_MASK           0x02    //bit[1], termb_inv
#define PMU_INT_STATUS_CHARGER_MASK         0x04    //bit[2], charging flag
#define PMU_INT_STATUS_PG_MASK              0x08    //bit[3], PG, Indicating if charger input voltage is higher than battery voltage
#define PMU_INT_STATUS_BAT_LOW_MASK         0x10    //bit[4], When Battery voltage is lower than this threshold, BAT_Low will become high

#define PMU_INT_STATUS_MASK_CLOSE       (PMU_INT_STATUS_LINE_DET_MASK|PMU_INT_STATUS_TERMB_MASK|PMU_INT_STATUS_CHARGER_MASK|PMU_INT_STATUS_PG_MASK|PMU_INT_STATUS_BAT_LOW_MASK)

//for 7632B,



/* power state definitions */
#define PMU_POWER_OFF               0
#define PMU_POWER_ON                1
#define PMU_POWER_FAILURE           2

typedef enum {
		DIG_DCDC_0_85V = 0,
		DIG_DCDC_0_9V,
		DIG_DCDC_0_95V,
		DIG_DCDC_1_0V,
		DIG_DCDC_1_1V,
		DIG_DCDC_1_2V,
		DIG_DCDC_1_25V,
		DIG_DCDC_1_257V,
		DIG_DCDC_1_3V,
		DIG_DCDC_1_325V,
		DIG_DCDC_1_35V,
		DIG_DCDC_1_4V,
		DIG_DCDC_1_45V,
		DIG_DCDC_1_5V,
		DIG_DCDC_1_6V,
		DIG_DCDC_1_7V,
}DIGITALDCDC_VOL;

#if AUDIO_9647X_I2S0_I2S1_TEST
#define SYSTEM_HIGH_FREQ_VOLTAGE        (DIG_DCDC_1_35V)   //默认值为DIG_DCDC_1_325V，当前为9647X专用； 系统超频时的VDD12数字电压值（当TOP/BT时钟大于128M，或者DSP时钟大于256M）则认为系统超频工作
#else
#define SYSTEM_HIGH_FREQ_VOLTAGE        (DIG_DCDC_1_325V)   // 系统超频时的VDD12数字电压值（当TOP/BT时钟大于128M，或者DSP时钟大于256M）则认为系统超频工作
#endif

#define SYSTEM_NORMAL_FREQ_VOLTAGE      (DIG_DCDC_1_2V)     // 系统正常工作时的VDD12数字电压值

#define SYSTEM_DEFAULT_DIGITAL_VOLTAGE  (SYSTEM_HIGH_FREQ_VOLTAGE)

//20230420xy, if the product not keep the battery connect always, then should need this 7632B HW fix enable; 
//when battery not connect and charger in will power on, then connect battery and plug out the charger quickly, maybe let power down abnormal; 
//debug with zhuhantao find the had hardware bug at 7632B, then should close then "BAT_LOW" void the hw bug; the MEN_CMeter default =0 Coulomb Meter enter sleep and open BAT_LOW, need set 1 enable Coulomb Meter also close BAT_LOW;
#define VPMU_7632BATLOW_HWBUGFIX		(1)


void VPMU_Init ( void );
UINT8 VPMU_GetPowerState ( void );
UINT8 VPMU_GetPowerOnReq ( void );
void VPMU_SetPMUEn ( BOOL IsEnable );
VRET_VALUE VPMU_GetChargerState ( UINT8 *p_State );
void VPMU_SetBatteryLowLevel ( UINT8 MBV );
//void VPMU_DCDC_Enable(BOOL IsEnable);
//void VPMU_VA7621_LDO5En ( void );
void VPMU_SetLDO5En ( BOOL IsEnable );
void VPMU_SetDCOutputVoltage ( UINT8 Voltage );
void VPMU_SetDigitalDCDC( DIGITALDCDC_VOL Voltage );
UINT8 VPMU_GetDigitalDCDC(void);
//void VPMU_SetChargeTerminateVoltage(UINT8 Voltage);
void VPMU_SetCharge ( UINT8 Voltage, UINT8 Current );
void VPMU_SetMCP ( UINT8 Mcp );
void VPMU_SetDCDCMode ( UINT8 Mode );
void VPMU_SetPowerRstDelay ( UINT8 DelaySel );
VRET_VALUE VPMU_GetCoulombVD ( UINT8 *p_VD );
VRET_VALUE VPMU_ReadReg ( UINT8 Reg, UINT8 *p_Value );
VRET_VALUE VPMU_WriteReg ( UINT8 Reg, UINT8 Value );
void VPMU_SetI2CLowSpeedFlag ( BOOL Flag );
VRET_VALUE VPMU_GetVerInfo ( UINT8 *p_VerInfo );
VRET_VALUE VPMU_LineInDetect ( UINT8 *p_LineIn );
VRET_VALUE VPMU_IntInverseConfig ( UINT8 *p_InvConfig, UINT8 mode );
VRET_VALUE VPMU_IntMaskConfig ( UINT8 *p_MaskConfig, UINT8 mode );

void VPMU_SetDCDC_CodecOn ( UINT8 Mode );
void VPMU_SetChargingDisable ( UINT8 Ctrl );
void VPMU_SetStateISR ( UINT8 State );
UINT8 VPMU_GetStateISR ( void );
BOOL VPMU_StatIsr_En ( void );

void VPMU_Set7621PMUEn ( UINT8 Enable );
VRET_VALUE VPMU_Get7621PMUEn ( UINT8 *p_State );

void VPMU_Set7632PMUSel ( UINT8 Sel );
void VPMU_Set7632ChargingTime ( UINT8 Mode );
void VPMU_Config7632AIO ( UINT8 Mode );
VRET_VALUE VPMU_Get7632AIO ( UINT16 *p_AIO );
void VPMU_Set7632MMDC ( UINT8 Mode );

void VPMU_Set_Bypass (UINT8 Enable);
#if VPMU_7632BATLOW_HWBUGFIX
VRET_VALUE VPMU_7632Close_BATLOW(void);
#endif
UINT8 VPMU_Get7652ChargerFix(void);

void VPMU_EnableInt(void);
void VPMU_DisableInt(void);
void VPMU_RestoreInt(BOOL enable);
UINT8 VPMU_GetIntStatus(void);
void VPMU_ClrIntStatus(UINT8 status);
UINT8 VPMU_GetIntMask(void);
void VPMU_SetIntMask(UINT8 mask);

void VPMU_Interrupt_Routine_Service ( void );

#ifdef __cplusplus
}
#endif

#endif  /* _VPMU_H_ */

