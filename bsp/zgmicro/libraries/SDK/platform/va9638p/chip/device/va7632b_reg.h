#ifndef __VREG_VA7632B_H__
#define __VREG_VA7632B_H__

#define PMU_I2C_SLAVE_ADDR      0x34

#define REG_VA7632B_BASE                                            0x0000

/*
MVDCA: None
[3:3]: RF DC-DC output voltage setting.MvdcA[3:0]  RF DC-DC output   Units 0011               1.225                          V1011               1.225                          V0010               1.25                            V1010               1.25                            V0001               1.3                              V1001               1.3                              V0000               1.35                            V1000               1.35                            V0111               1.4                              V1111               1.4                              V0110               1.5                              V1110               1.5                              V (default)0101               1.7                              V1101               1.7                              V0100               1.8                              V1100               1.8                              V
[2:2]: None
[1:1]: None
[0:0]: None
*/
#define REG_VA7632B_MVDCA                                           (REG_VA7632B_BASE + 0x0000)
#define REG_VA7632B_MVDCA_MVDCA_3                                   ((((REG_VA7632B_MVDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_MVDCA_MVDCA_3_BIT                               (3)
#define REG_VA7632B_MVDCA_MVDCA_3_MASK                              (0x00000008)
#define REG_VA7632B_MVDCA_MVDCA_2                                   ((((REG_VA7632B_MVDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_MVDCA_MVDCA_2_BIT                               (2)
#define REG_VA7632B_MVDCA_MVDCA_2_MASK                              (0x00000004)
#define REG_VA7632B_MVDCA_MVDCA_1                                   ((((REG_VA7632B_MVDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MVDCA_MVDCA_1_BIT                               (1)
#define REG_VA7632B_MVDCA_MVDCA_1_MASK                              (0x00000002)
#define REG_VA7632B_MVDCA_MVDCA_0                                   ((((REG_VA7632B_MVDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MVDCA_MVDCA_0_BIT                               (0)
#define REG_VA7632B_MVDCA_MVDCA_0_MASK                              (0x00000001)

/*
MCHV: None
[5:5]: 4-bit setting for CC mode charging current.Mchi[3:0]    CC charging current    Units 0000                     10                        mA0001                     20                        mA0010                     30                        mA0011                     40                        mA0100                     50                        mA0101                     60                        mA0110                     70                        mA0111                     80                        mA1000                     90                        mA1001                     100                      mA (default)1010                     130                      mA1011                     160                      mA1100                     180                      mA1101                     200                      mA1110                     250                      mA1111                     300                      mA
[4:4]: None
[3:3]: None
[2:2]: None
[1:1]: Charging termination voltage control register.[Mchv1, Mchv2] = 2b00: Vout_ch = 4.35V                              2b01: Vout_ch = 4.1V                              2b10: Vout_ch = 4.2V (default)                              2b11: Vout_ch = 4.25V
[0:0]: None
*/
#define REG_VA7632B_MCHV                                            (REG_VA7632B_BASE + 0x0001)
#define REG_VA7632B_MCHV_MCHI                                       ((((REG_VA7632B_MCHV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_MCHV_MCHI_BIT                                   (2)
#define REG_VA7632B_MCHV_MCHI_MASK                                  (0x0000003C)
#define REG_VA7632B_MCHV_MCHV                                       ((((REG_VA7632B_MCHV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MCHV_MCHV_BIT                                   (0)
#define REG_VA7632B_MCHV_MCHV_MASK                                  (0x00000003)

/*
MEN5: None
[7:7]: Setting PWM frequency for BUCKRF&DCDCA.Men5[7]     Men5[5]       PWM frequency0                   0                   2.5MHz0                   1          1                   0                   1.5MHz1                   1                   1 MHz
[6:6]: If Men5[6]="0" & Men5[2]="0", LDOB (LDOB's Error Amplifier is shared with LDO3/LDO6, High Current Power Input: VBAT)LDO1LDO5 work when charger is disconnected; LDO3/LDO6LDO4LDO5 work when charger is connected. If Men5[6]="1" &  Men5[2]="0", BuckLDO1LDO5 work when charger is disconnected; LDO3/LDO6LDO4LDO5 work when charger is connected.If Men5[6]="0" &  Men5[2]="1", LDOBLDO1LDO5 work when charger is disconnected; LDOBLDO1LDO5 work when charger is connected.If Men5[6]="1" &  Men5[2]="1", BuckLDO1LDO5 work when charger is disconnected; BuckLDO1LDO5 work when charger is connected.LDO3 replaces BUCKRF when charger is connected; and LDO6 replaces DCDCA when charger is connected.
[5:5]: Setting PWM frequency for BUCKRF&DCDCA. See Men5[7].
[4:4]: if Men5[4]=0, RF DC-DC is turned off; if Men5[4]=1, RF DC-DC is turned on.
[3:3]: Men5[3]="0", VOD Enable. Men5[3]="1", VOD Disable.
[2:2]: Men5[2]="1", charger bypass mode will be disabled.Men5[2]="0", charger bypass mode will be enabled.
[1:1]: LDO5 operation mode control:Men5[1]=1'b1 && Men5[0]=1'b1 : LDO5 enter low power mode;Men5[1]=1'b0 && Men5[0]=1'b1 : LDO5 stays at normal mode;
[0:0]: LDO5 enable control register.1b0: disable LDO51b1: enable LDO5
*/
#define REG_VA7632B_MEN5                                            (REG_VA7632B_BASE + 0x0002)
#define REG_VA7632B_MEN5_MEN5_7                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_VA7632B_MEN5_MEN5_7_BIT                                 (7)
#define REG_VA7632B_MEN5_MEN5_7_MASK                                (0x00000080)
#define REG_VA7632B_MEN5_MEN5_6                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_MEN5_MEN5_6_BIT                                 (6)
#define REG_VA7632B_MEN5_MEN5_6_MASK                                (0x00000040)
#define REG_VA7632B_MEN5_MEN5_5                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_MEN5_MEN5_5_BIT                                 (5)
#define REG_VA7632B_MEN5_MEN5_5_MASK                                (0x00000020)
#define REG_VA7632B_MEN5_MEN5_4                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_MEN5_MEN5_4_BIT                                 (4)
#define REG_VA7632B_MEN5_MEN5_4_MASK                                (0x00000010)
#define REG_VA7632B_MEN5_MEN5_3                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_MEN5_MEN5_3_BIT                                 (3)
#define REG_VA7632B_MEN5_MEN5_3_MASK                                (0x00000008)
#define REG_VA7632B_MEN5_MEN5_2                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_MEN5_MEN5_2_BIT                                 (2)
#define REG_VA7632B_MEN5_MEN5_2_MASK                                (0x00000004)
#define REG_VA7632B_MEN5_MEN5_1                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MEN5_MEN5_1_BIT                                 (1)
#define REG_VA7632B_MEN5_MEN5_1_MASK                                (0x00000002)
#define REG_VA7632B_MEN5_MEN5_0                                     ((((REG_VA7632B_MEN5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MEN5_MEN5_0_BIT                                 (0)
#define REG_VA7632B_MEN5_MEN5_0_MASK                                (0x00000001)

/*
MMDC: None
[2:2]: Reset_Mode: it is used to set reset mode or shutdown mode when PMU_EN  goes low. If Reset_Mode=1, VA7633 will generate a reset pulse (POR signal) to reset SOC chip when PMU_EN goes low. The low-level time of the reset pulse is same as the low-level time of power-on reset. If Reset_Mode=0, VA7633 will shut down the system and all power supplies when PMU_EN goes low.
[1:1]: 0 means PWM mode for RF&Dig DCDC. 1 means PFM mode for RF&Dig DCDC
[0:0]: 0 means RF&Dig DCDC decide PFM/PWM mode automatically. 1 means RF&Dig DCDC PFM/PWM mode will be decided by Mmdc2
*/
#define REG_VA7632B_MMDC                                            (REG_VA7632B_BASE + 0x0003)
#define REG_VA7632B_MMDC_RESET_MODE                                 ((((REG_VA7632B_MMDC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_MMDC_RESET_MODE_BIT                             (2)
#define REG_VA7632B_MMDC_RESET_MODE_MASK                            (0x00000004)
#define REG_VA7632B_MMDC_MMDC2                                      ((((REG_VA7632B_MMDC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MMDC_MMDC2_BIT                                  (1)
#define REG_VA7632B_MMDC_MMDC2_MASK                                 (0x00000002)
#define REG_VA7632B_MMDC_MMDC1                                      ((((REG_VA7632B_MMDC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MMDC_MMDC1_BIT                                  (0)
#define REG_VA7632B_MMDC_MMDC1_MASK                                 (0x00000001)

/*
MBV: None
[4:6]: setting for LDO2 output voltage000: 0.9v001: 0.95v010: 1v011: 1.1v100: 1.15v101: 1.2v110: NA
[3:3]: Set Battery Low Detection threshold. When Battery voltage is lower than this threshold, BAT_Low will become high.Mbv4/Mbv3/Mbv2/Mvb1    VBAT_LOW Threshold    Units0000                                        2.7                         V0001                                        2.75                       V0010                                        2.8                         V0011                                        2.85                       V0100                                        2.90                       V0101                                        2.95                       V0110                                        3.00                       V0111                                        3.05                       V1000                                        3.10                       V1001                                        3.15                       V1010                                        3.20                       V1011                                        3.25                       V1100                                        3.30                       V1101                                        3.35                       V1110                                        3.40                       V (default)1111                                        3.45                       V
[2:2]: None
[1:1]: None
[0:0]: None
*/
#define REG_VA7632B_MBV                                             (REG_VA7632B_BASE + 0x0004)
#define REG_VA7632B_MBV_MVOL2                                       ((((REG_VA7632B_MBV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_MBV_MVOL2_BIT                                   (4)
#define REG_VA7632B_MBV_MVOL2_MASK                                  (0x00000070)
#define REG_VA7632B_MBV_MBV                                         ((((REG_VA7632B_MBV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MBV_MBV_BIT                                     (0)
#define REG_VA7632B_MBV_MBV_MASK                                    (0x0000000F)

/*
MEN_CMETER: None
[0:0]: Coulomb Meter enable signal.1'b1: Coulomb Meter enter active mode.1'b0: Coulomb Meter enter sleep mode (low power consumption).
*/
#define REG_VA7632B_MEN_CMETER                                      (REG_VA7632B_BASE + 0x0005)
#define REG_VA7632B_MEN_CMETER_MEN_CMETER                           ((((REG_VA7632B_MEN_CMETER) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MEN_CMETER_MEN_CMETER_BIT                       (0)
#define REG_VA7632B_MEN_CMETER_MEN_CMETER_MASK                      (0x00000001)

/*
VD: None
[5:5]: Coulomb Meter output data, which demonstrate the battery voltage.
[4:4]: None
[3:3]: None
[2:2]: None
[1:1]: None
[0:0]: None
*/
#define REG_VA7632B_VD                                              (REG_VA7632B_BASE + 0x0006)
#define REG_VA7632B_VD_VD_5                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_VD_VD_5_BIT                                     (5)
#define REG_VA7632B_VD_VD_5_MASK                                    (0x00000020)
#define REG_VA7632B_VD_VD_4                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_VD_VD_4_BIT                                     (4)
#define REG_VA7632B_VD_VD_4_MASK                                    (0x00000010)
#define REG_VA7632B_VD_VD_3                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_VD_VD_3_BIT                                     (3)
#define REG_VA7632B_VD_VD_3_MASK                                    (0x00000008)
#define REG_VA7632B_VD_VD_2                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_VD_VD_2_BIT                                     (2)
#define REG_VA7632B_VD_VD_2_MASK                                    (0x00000004)
#define REG_VA7632B_VD_VD_1                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_VD_VD_1_BIT                                     (1)
#define REG_VA7632B_VD_VD_1_MASK                                    (0x00000002)
#define REG_VA7632B_VD_VD_0                                         ((((REG_VA7632B_VD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_VD_VD_0_BIT                                     (0)
#define REG_VA7632B_VD_VD_0_MASK                                    (0x00000001)

/*
MTRST_LOCK: None
[1:1]: Deadlock reset delay time control register.[MTrst_lock1, MTrst_lock2]           Deadlock Reset Delay Time2'b00                                                 7s (default)2'b01                                                 15s2'b10                                                 10s2'b11                                                 3s
[0:0]: None
*/
#define REG_VA7632B_MTRST_LOCK                                      (REG_VA7632B_BASE + 0x0007)
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK1                          ((((REG_VA7632B_MTRST_LOCK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK1_BIT                      (1)
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK1_MASK                     (0x00000002)
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK2                          ((((REG_VA7632B_MTRST_LOCK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK2_BIT                      (0)
#define REG_VA7632B_MTRST_LOCK_MTRST_LOCK2_MASK                     (0x00000001)

/*
D_LPO1: None
[6:7]: dummy register.
[5:5]: if D_LPO[8]=1,  D_LPO[5]="1", detect VBAT ;D_LPO[5]="0", detect VCHG.
[4:4]: DCDCAOCP_PFMD_LPO<4>     PFM_OCP(PFM)     PFM_OCP(PWM)0                     77mA                      46mA1                     109mA                    46mA
[3:3]: BUCKRFOCP_PFMD_LPO<3>     PFM_OCP(PFM)     PFM_OCP(PWM)0                     67mA                      42mA1                     115mA                    42mA
[2:2]: charger OVP enable signal.  Active high.
[0:1]: charger OVP voltage setting.  D_LPO<1>  D_LPO<0>    charger OVP voltage0                    0                     6.5V0                    1                     7.2V1                    0                     8V1                    1                     7.2V
*/
#define REG_VA7632B_D_LPO1                                          (REG_VA7632B_BASE + 0x0008)
#define REG_VA7632B_D_LPO1_D_LPO_7_6                                ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_7_6_BIT                            (6)
#define REG_VA7632B_D_LPO1_D_LPO_7_6_MASK                           (0x000000C0)
#define REG_VA7632B_D_LPO1_D_LPO_5                                  ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_5_BIT                              (5)
#define REG_VA7632B_D_LPO1_D_LPO_5_MASK                             (0x00000020)
#define REG_VA7632B_D_LPO1_D_LPO_4                                  ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_4_BIT                              (4)
#define REG_VA7632B_D_LPO1_D_LPO_4_MASK                             (0x00000010)
#define REG_VA7632B_D_LPO1_D_LPO_3                                  ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_3_BIT                              (3)
#define REG_VA7632B_D_LPO1_D_LPO_3_MASK                             (0x00000008)
#define REG_VA7632B_D_LPO1_D_LPO_2                                  ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_2_BIT                              (2)
#define REG_VA7632B_D_LPO1_D_LPO_2_MASK                             (0x00000004)
#define REG_VA7632B_D_LPO1_D_LPO_1_0                                ((((REG_VA7632B_D_LPO1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_D_LPO1_D_LPO_1_0_BIT                            (0)
#define REG_VA7632B_D_LPO1_D_LPO_1_0_MASK                           (0x00000003)

/*
D_LPO2: setting for BUCKRF RCP current.D_LPO<15:13>   RCP current(L=4.7uH)   RCP current(L=2.2uH)   RCP current(L=1uH)000                       14.2mA 100                       20.6 mA  010                       26.6 mA  110                                                               21.1 mA 001                                                               26.4 mA 101                                                               31.5 mA 011                                                                                                        28.9 mA111                                                                                                        35.7 mA
[2:4]: setting for DCDCA RCP current.D_LPO<12:10>    RCP current(L=4.7uH)    RCP current(L=2.2uH)    RCP current(L=1uH)000                          14mA  100                           20.5 mA  010                           26.3 mA  110                                                                  20.5 mA 001                                                                  25.5 mA 101                                                                  29.8 mA 011                                                                                                           --111                                                                                                           --
[1:1]: Dummy register.
[0:0]: If D_LPO[8]=1, this ADC is used to detect VCHG or VBAT according to D_LPO[5]; If D_LPO[8]=0, this ADC is used to detect AIO_ADC.
*/
#define REG_VA7632B_D_LPO2                                          (REG_VA7632B_BASE + 0x0009)
#define REG_VA7632B_D_LPO2_D_LPO_15_13                              ((((REG_VA7632B_D_LPO2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_D_LPO2_D_LPO_15_13_BIT                          (5)
#define REG_VA7632B_D_LPO2_D_LPO_15_13_MASK                         (0x000000E0)
#define REG_VA7632B_D_LPO2_D_LPO_12_10                              ((((REG_VA7632B_D_LPO2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_D_LPO2_D_LPO_12_10_BIT                          (2)
#define REG_VA7632B_D_LPO2_D_LPO_12_10_MASK                         (0x0000001C)
#define REG_VA7632B_D_LPO2_D_LPO_9                                  ((((REG_VA7632B_D_LPO2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_D_LPO2_D_LPO_9_BIT                              (1)
#define REG_VA7632B_D_LPO2_D_LPO_9_MASK                             (0x00000002)
#define REG_VA7632B_D_LPO2_D_LPO_8                                  ((((REG_VA7632B_D_LPO2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_D_LPO2_D_LPO_8_BIT                              (0)
#define REG_VA7632B_D_LPO2_D_LPO_8_MASK                             (0x00000001)

/*
RW_DUM1: None
[4:7]: Dummy register.
[0:3]: Dummy register.
*/
#define REG_VA7632B_RW_DUM1                                         (REG_VA7632B_BASE + 0x000A)
#define REG_VA7632B_RW_DUM1_RW_DUM1_7_4                             ((((REG_VA7632B_RW_DUM1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_RW_DUM1_RW_DUM1_7_4_BIT                         (4)
#define REG_VA7632B_RW_DUM1_RW_DUM1_7_4_MASK                        (0x000000F0)
#define REG_VA7632B_RW_DUM1_RW_DUM1_3_0                             ((((REG_VA7632B_RW_DUM1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_RW_DUM1_RW_DUM1_3_0_BIT                         (0)
#define REG_VA7632B_RW_DUM1_RW_DUM1_3_0_MASK                        (0x0000000F)

/*
MCP: None
[1:1]: Setting for charging time.Mcp[1]=0, charging time=1.8h.Mcp[1]=1, charging time=3.6h.
[0:0]: Enable signal for charging time.1'b0 enable charger timing.1'b1 disable charger timing.
*/
#define REG_VA7632B_MCP                                             (REG_VA7632B_BASE + 0x000B)
#define REG_VA7632B_MCP_MCP_1                                       ((((REG_VA7632B_MCP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MCP_MCP_1_BIT                                   (1)
#define REG_VA7632B_MCP_MCP_1_MASK                                  (0x00000002)
#define REG_VA7632B_MCP_MCP_0                                       ((((REG_VA7632B_MCP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MCP_MCP_0_BIT                                   (0)
#define REG_VA7632B_MCP_MCP_0_MASK                                  (0x00000001)

/*
CHG_STOP: None
[0:0]: CHG_Stop =1  will stop charger charging current. CHG_Stop =0  will not stop charger charging current.
*/
#define REG_VA7632B_CHG_STOP                                        (REG_VA7632B_BASE + 0x000C)
#define REG_VA7632B_CHG_STOP_CHG_STOP                               ((((REG_VA7632B_CHG_STOP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CHG_STOP_CHG_STOP_BIT                           (0)
#define REG_VA7632B_CHG_STOP_CHG_STOP_MASK                          (0x00000001)

/*
DCA_EN: None
[0:0]: DCA_en=0, audio DC-DC will be turned off.DCA_en=1, audio DC-DC will be turned on.
*/
#define REG_VA7632B_DCA_EN                                          (REG_VA7632B_BASE + 0x000D)
#define REG_VA7632B_DCA_EN_DCA_EN                                   ((((REG_VA7632B_DCA_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_DCA_EN_DCA_EN_BIT                               (0)
#define REG_VA7632B_DCA_EN_DCA_EN_MASK                              (0x00000001)

/*
PMU_ON: None
[0:0]: Register to set PMU ON/OFF. pmu_on=1, PMU will be set ON.pmu_on=0, PMU will be set OFF.
*/
#define REG_VA7632B_PMU_ON                                          (REG_VA7632B_BASE + 0x000E)
#define REG_VA7632B_PMU_ON_PMU_ON                                   ((((REG_VA7632B_PMU_ON) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_PMU_ON_PMU_ON_BIT                               (0)
#define REG_VA7632B_PMU_ON_PMU_ON_MASK                              (0x00000001)

/*
PMU_SEL: None
[0:0]: Register to set mode of pmu_en_out. pmu_sel =1, pmu_en_out operates according to the register (pmu_on).pmu_sel =0, pmu_en_out operates according to the port signal (PMU_EN).
*/
#define REG_VA7632B_PMU_SEL                                         (REG_VA7632B_BASE + 0x000F)
#define REG_VA7632B_PMU_SEL_PMU_SEL                                 ((((REG_VA7632B_PMU_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_PMU_SEL_PMU_SEL_BIT                             (0)
#define REG_VA7632B_PMU_SEL_PMU_SEL_MASK                            (0x00000001)

/*
MV5: None
[1:1]: LDO5 output voltage setting.Mv5[1:0]      VOL5      Units 00                2.0             V01                1.9             V10                1.8             V (default)11                1.8             V
[0:0]: None
*/
#define REG_VA7632B_MV5                                             (REG_VA7632B_BASE + 0x0010)
#define REG_VA7632B_MV5_MV5_1                                       ((((REG_VA7632B_MV5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MV5_MV5_1_BIT                                   (1)
#define REG_VA7632B_MV5_MV5_1_MASK                                  (0x00000002)
#define REG_VA7632B_MV5_MV5_0                                       ((((REG_VA7632B_MV5) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MV5_MV5_0_BIT                                   (0)
#define REG_VA7632B_MV5_MV5_0_MASK                                  (0x00000001)

/*
MIT: None
[0:0]: Mit=0, charging termination current is set to 10%*ICH; Mit=1, charging termination current is set to 5%*ICH.
*/
#define REG_VA7632B_MIT                                             (REG_VA7632B_BASE + 0x0011)
#define REG_VA7632B_MIT_MIT                                         ((((REG_VA7632B_MIT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MIT_MIT_BIT                                     (0)
#define REG_VA7632B_MIT_MIT_MASK                                    (0x00000001)

/*
MMDCA: None
[1:1]: 0 means PWM mode for Audio DCDC. 1 means PFM mode for Audio DCDC.
[0:0]: 0 means Audio DCDC decide PFM/PWM mode automatically. 1 means Audio DCDC PFM/PWM mode will be decided by MmdcA2.
*/
#define REG_VA7632B_MMDCA                                           (REG_VA7632B_BASE + 0x0012)
#define REG_VA7632B_MMDCA_MMDCA2                                    ((((REG_VA7632B_MMDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MMDCA_MMDCA2_BIT                                (1)
#define REG_VA7632B_MMDCA_MMDCA2_MASK                               (0x00000002)
#define REG_VA7632B_MMDCA_MMDCA1                                    ((((REG_VA7632B_MMDCA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MMDCA_MMDCA1_BIT                                (0)
#define REG_VA7632B_MMDCA_MMDCA1_MASK                               (0x00000001)

/*
MV2: None
[0:0]: Set LDO2 bypass mode. MV2=1, LDO2 works in bypass mode (Vout=Vin); MV2=0, LDO2 works in normal mode.
*/
#define REG_VA7632B_MV2                                             (REG_VA7632B_BASE + 0x0013)
#define REG_VA7632B_MV2_MV2                                         ((((REG_VA7632B_MV2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MV2_MV2_BIT                                     (0)
#define REG_VA7632B_MV2_MV2_MASK                                    (0x00000001)

/*
MOTP: None
[1:1]: Two bits for OTP (Over-Temperature Protection) threshold programming.Motp[1:0]   TH   TL   hysteresis00            140   100    40 (default)01            120   90      3010            100   80      2011             80     60      20
[0:0]: None
*/
#define REG_VA7632B_MOTP                                            (REG_VA7632B_BASE + 0x0014)
#define REG_VA7632B_MOTP_MOTP_1                                     ((((REG_VA7632B_MOTP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_MOTP_MOTP_1_BIT                                 (1)
#define REG_VA7632B_MOTP_MOTP_1_MASK                                (0x00000002)
#define REG_VA7632B_MOTP_MOTP_0                                     ((((REG_VA7632B_MOTP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MOTP_MOTP_0_BIT                                 (0)
#define REG_VA7632B_MOTP_MOTP_0_MASK                                (0x00000001)

/*
RD_DUM1: dummy register.
[1:1]: CVCHG="1" means to enter the CVCHG modeCVCHG="0" means to exit the CVCHG mode
[0:0]: PRECHG="1" means to enter the PRECHG modePRECHG="0" means to exit the PRECHG mode
*/
#define REG_VA7632B_RD_DUM1                                         (REG_VA7632B_BASE + 0x0015)
#define REG_VA7632B_RD_DUM1_CVCHG                                   ((((REG_VA7632B_RD_DUM1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_RD_DUM1_CVCHG_BIT                               (1)
#define REG_VA7632B_RD_DUM1_CVCHG_MASK                              (0x00000002)
#define REG_VA7632B_RD_DUM1_PRECHG                                  ((((REG_VA7632B_RD_DUM1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_RD_DUM1_PRECHG_BIT                              (0)
#define REG_VA7632B_RD_DUM1_PRECHG_MASK                             (0x00000001)

/*
RD_DUM2: dummy register.
*/
#define REG_VA7632B_RD_DUM2                                         (REG_VA7632B_BASE + 0x0016)

/*
MEN_AIOADC: None
[0:0]: Turn on AIO_ADC, active high.
*/
#define REG_VA7632B_MEN_AIOADC                                      (REG_VA7632B_BASE + 0x0017)
#define REG_VA7632B_MEN_AIOADC_MEN_AIOADC                           ((((REG_VA7632B_MEN_AIOADC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MEN_AIOADC_MEN_AIOADC_BIT                       (0)
#define REG_VA7632B_MEN_AIOADC_MEN_AIOADC_MASK                      (0x00000001)

/*
TERM_AIOADC: None
[0:0]: "term_AIOADC" is AIO_ADC output to indicate data ready.1: data ready,   0: data not ready
*/
#define REG_VA7632B_TERM_AIOADC                                     (REG_VA7632B_BASE + 0x0018)
#define REG_VA7632B_TERM_AIOADC_TERM_AIOADC                         ((((REG_VA7632B_TERM_AIOADC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_TERM_AIOADC_TERM_AIOADC_BIT                     (0)
#define REG_VA7632B_TERM_AIOADC_TERM_AIOADC_MASK                    (0x00000001)

/*
AIO_DATA1: AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
*/
#define REG_VA7632B_AIO_DATA1                                       (REG_VA7632B_BASE + 0x0019)

/*
AIO_DATA2: None
[0:1]: AIO_Data[9:0] is AIO_ADC data output. AIO_ADC is 10-bit ADC.
*/
#define REG_VA7632B_AIO_DATA2                                       (REG_VA7632B_BASE + 0x001A)
#define REG_VA7632B_AIO_DATA2_AIO_DATA                              ((((REG_VA7632B_AIO_DATA2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_AIO_DATA2_AIO_DATA_BIT                          (0)
#define REG_VA7632B_AIO_DATA2_AIO_DATA_MASK                         (0x00000003)

/*
MVDC: None
[0:3]: Digital DC-DC output voltage setting.Mvdc[3:0]    Digital DC-DC output    Units 1000                        0.85                       V1001                        0.9                         V1010                        0.95                       V1011                        1.0                         V1100                        1.1                         V1101                        1.2                         V (default)1110                        1.25                       V1111                        1.275                     V0000                        1.3                         V0110                        1.3                        V                                                                                                      0001                        1.325                     V0010                        1.35                       V0011                        1.4                         V                                                                                                        0111                        1.4                         V                                                                                                      0100                        1.45                       V0101                        1.5                         V
*/
#define REG_VA7632B_MVDC                                            (REG_VA7632B_BASE + 0x001B)
#define REG_VA7632B_MVDC_MVDC                                       ((((REG_VA7632B_MVDC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_MVDC_MVDC_BIT                                   (0)
#define REG_VA7632B_MVDC_MVDC_MASK                                  (0x0000000F)

/*
CDC_REFPD_LOW: None
[0:4]: bit0:self bias; bit1:bandgap; bit2:current reference; bit3:Mic bias; bit4:Vref and Vcom; active high
*/
#define REG_VA7632B_CDC_REFPD_LOW                                   (REG_VA7632B_BASE + 0x0040)
#define REG_VA7632B_CDC_REFPD_LOW_VREF_VCOM                         ((((REG_VA7632B_CDC_REFPD_LOW) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_REFPD_LOW_VREF_VCOM_BIT                     (4)
#define REG_VA7632B_CDC_REFPD_LOW_VREF_VCOM_MASK                    (0x00000010)
#define REG_VA7632B_CDC_REFPD_LOW_MIC_BIAS                          ((((REG_VA7632B_CDC_REFPD_LOW) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_CDC_REFPD_LOW_MIC_BIAS_BIT                      (3)
#define REG_VA7632B_CDC_REFPD_LOW_MIC_BIAS_MASK                     (0x00000008)
#define REG_VA7632B_CDC_REFPD_LOW_CURRENT_REFERENCE                 ((((REG_VA7632B_CDC_REFPD_LOW) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_REFPD_LOW_CURRENT_REFERENCE_BIT             (2)
#define REG_VA7632B_CDC_REFPD_LOW_CURRENT_REFERENCE_MASK            (0x00000004)
#define REG_VA7632B_CDC_REFPD_LOW_BANDGAP                           ((((REG_VA7632B_CDC_REFPD_LOW) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_REFPD_LOW_BANDGAP_BIT                       (1)
#define REG_VA7632B_CDC_REFPD_LOW_BANDGAP_MASK                      (0x00000002)
#define REG_VA7632B_CDC_REFPD_LOW_SELF_BIAS                         ((((REG_VA7632B_CDC_REFPD_LOW) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_REFPD_LOW_SELF_BIAS_BIT                     (0)
#define REG_VA7632B_CDC_REFPD_LOW_SELF_BIAS_MASK                    (0x00000001)
#define REG_VA7632B_CDC_REFPD_LOW_CDC_REFPD_MASK                    (0x0000001F)
/*
CDC_CTBUF: None
[2:2]: Continous time buffer power down, active low
*/
#define REG_VA7632B_CDC_CTBUF                                       (REG_VA7632B_BASE + 0x0041)
#define REG_VA7632B_CDC_CTBUF_CDC_CTBUFFPD                          ((((REG_VA7632B_CDC_CTBUF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_CTBUF_CDC_CTBUFFPD_BIT                      (2)
#define REG_VA7632B_CDC_CTBUF_CDC_CTBUFFPD_MASK                     (0x00000004)

/*
CDC_SCBUF: None
[2:2]: Power down switch cap buffer, active high
*/
#define REG_VA7632B_CDC_SCBUF                                       (REG_VA7632B_BASE + 0x0042)
#define REG_VA7632B_CDC_SCBUF_CDC_SCBUFFPD                          ((((REG_VA7632B_CDC_SCBUF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_SCBUF_CDC_SCBUFFPD_BIT                      (2)
#define REG_VA7632B_CDC_SCBUF_CDC_SCBUFFPD_MASK                     (0x00000004)

/*
CDC_ADCLKEN: None
[1:1]: adc clock enable, right channel, active high
[0:0]: adc clock enable, left channel, active high
*/
#define REG_VA7632B_CDC_ADCLKEN                                     (REG_VA7632B_BASE + 0x0043)
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENR                        ((((REG_VA7632B_CDC_ADCLKEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENR_BIT                    (1)
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENR_MASK                   (0x00000002)
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENL                        ((((REG_VA7632B_CDC_ADCLKEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENL_BIT                    (0)
#define REG_VA7632B_CDC_ADCLKEN_CDC_ADCLKENL_MASK                   (0x00000001)

/*
CDC_ADPDIREF: None
[1:1]: power down current reference in right channel adc, active high
[0:0]: power down current reference in left channel adc, active high
*/
#define REG_VA7632B_CDC_ADPDIREF                                    (REG_VA7632B_BASE + 0x0044)
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFR                      ((((REG_VA7632B_CDC_ADPDIREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFR_BIT                  (1)
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFR_MASK                 (0x00000002)
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFL                      ((((REG_VA7632B_CDC_ADPDIREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFL_BIT                  (0)
#define REG_VA7632B_CDC_ADPDIREF_CDC_ADPDIREFL_MASK                 (0x00000001)

/*
CDC_ADPD: None
[1:1]: power down ADC right channel,active high
[0:0]: power down ADC left channel,active high
*/
#define REG_VA7632B_CDC_ADPD                                        (REG_VA7632B_BASE + 0x0045)
#define REG_VA7632B_CDC_ADPD_CDC_ADPDR                              ((((REG_VA7632B_CDC_ADPD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_ADPD_CDC_ADPDR_BIT                          (1)
#define REG_VA7632B_CDC_ADPD_CDC_ADPDR_MASK                         (0x00000002)
#define REG_VA7632B_CDC_ADPD_CDC_ADPDL                              ((((REG_VA7632B_CDC_ADPD) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADPD_CDC_ADPDL_BIT                          (0)
#define REG_VA7632B_CDC_ADPD_CDC_ADPDL_MASK                         (0x00000001)

/*
CDC_ADRSTB: None
[1:1]: Reset right ADC digital part, active low
[0:0]: Reset left ADC digital part, active low
*/
#define REG_VA7632B_CDC_ADRSTB                                      (REG_VA7632B_BASE + 0x0046)
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBR                          ((((REG_VA7632B_CDC_ADRSTB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBR_BIT                      (1)
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBR_MASK                     (0x00000002)
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBL                          ((((REG_VA7632B_CDC_ADRSTB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBL_BIT                      (0)
#define REG_VA7632B_CDC_ADRSTB_CDC_ADRSTBL_MASK                     (0x00000001)

/*
CDC_ADVOLL_INIT: None
[0:5]: PGA gain, -3dB to 42dB, left channel;gain table:42dB,39,36,34.5,34(0.5dB/step) 21dB,18,15,12,10.5 (0.5dB/step) -3dB
*/
#define REG_VA7632B_CDC_ADVOLL_INIT                                 (REG_VA7632B_BASE + 0x0047)
#define REG_VA7632B_CDC_ADVOLL_INIT_CDC_ADVOLL_INIT                 ((((REG_VA7632B_CDC_ADVOLL_INIT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADVOLL_INIT_CDC_ADVOLL_INIT_BIT             (0)
#define REG_VA7632B_CDC_ADVOLL_INIT_CDC_ADVOLL_INIT_MASK            (0x0000003F)

/*
CDC_ADVOLR_INIT: None
[0:5]: PGA gain, -3dB to 42dB, right channel;gain table:42dB,39,36,34.5,34(0.5dB/step) 21dB,18,15,12,10.5 (0.5dB/step) -3dB
*/
#define REG_VA7632B_CDC_ADVOLR_INIT                                 (REG_VA7632B_BASE + 0x0048)
#define REG_VA7632B_CDC_ADVOLR_INIT_CDC_ADVOLR_INIT                 ((((REG_VA7632B_CDC_ADVOLR_INIT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADVOLR_INIT_CDC_ADVOLR_INIT_BIT             (0)
#define REG_VA7632B_CDC_ADVOLR_INIT_CDC_ADVOLR_INIT_MASK            (0x0000003F)

/*
CDC_LINEIN_AMP_DET: None
[4:4]: CDC line_in amplitude detect for right channel.   0small, 1: large.
[0:0]: CDC line_in amplitude detect for left channel.    0small, 1: large.
*/
#define REG_VA7632B_CDC_LINEIN_AMP_DET                              (REG_VA7632B_BASE + 0x0049)
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_R         ((((REG_VA7632B_CDC_LINEIN_AMP_DET) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_R_BIT     (4)
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_R_MASK    (0x00000010)
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_L         ((((REG_VA7632B_CDC_LINEIN_AMP_DET) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_L_BIT     (0)
#define REG_VA7632B_CDC_LINEIN_AMP_DET_CDC_LINEIN_AMP_DET_L_MASK    (0x00000001)

/*
CDC_ADVOLL_CUR: None
[0:5]: PGA gain used with smalle amplitude signal (indicated by "CDC_LINEIN_AMP_DET_L"), left channel;gain range: -3dB to 42dBgain table: 42dB,39,36,34.5,34(0.5dB/step) 21dB,18,15,12,10.5 (0.5dB/step) -3dB
*/
#define REG_VA7632B_CDC_ADVOLL_CUR                                  (REG_VA7632B_BASE + 0x004A)
#define REG_VA7632B_CDC_ADVOLL_CUR_CDC_ADVOLL_CUR                   ((((REG_VA7632B_CDC_ADVOLL_CUR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADVOLL_CUR_CDC_ADVOLL_CUR_BIT               (0)
#define REG_VA7632B_CDC_ADVOLL_CUR_CDC_ADVOLL_CUR_MASK              (0x0000003F)

/*
CDC_ADVOLR_CUR: None
[0:5]: PGA gain used with smalle smplitude signal (indicated by "CDC_LINEIN_AMP_DET_R"), right channel;gain range: -3dB to 42dBgain table: 42dB,39,36,34.5,34(0.5dB/step) 21dB,18,15,12,10.5 (0.5dB/step) -3dB
*/
#define REG_VA7632B_CDC_ADVOLR_CUR                                  (REG_VA7632B_BASE + 0x004B)
#define REG_VA7632B_CDC_ADVOLR_CUR_CDC_ADVOLR_CUR                   ((((REG_VA7632B_CDC_ADVOLR_CUR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADVOLR_CUR_CDC_ADVOLR_CUR_BIT               (0)
#define REG_VA7632B_CDC_ADVOLR_CUR_CDC_ADVOLR_CUR_MASK              (0x0000003F)

/*
CDC_PGA: None
[5:5]: Power down right PGA, active high
[4:4]: Mute right PGA, active high
[1:1]: Power down left PGA, active high
[0:0]: Mute left PGA, active high
*/
#define REG_VA7632B_CDC_PGA                                         (REG_VA7632B_BASE + 0x004C)
#define REG_VA7632B_CDC_PGA_CDC_PGAPDR                              ((((REG_VA7632B_CDC_PGA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_PGA_CDC_PGAPDR_BIT                          (5)
#define REG_VA7632B_CDC_PGA_CDC_PGAPDR_MASK                         (0x00000020)
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTER                            ((((REG_VA7632B_CDC_PGA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTER_BIT                        (4)
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTER_MASK                       (0x00000010)
#define REG_VA7632B_CDC_PGA_CDC_PGAPDL                              ((((REG_VA7632B_CDC_PGA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_PGA_CDC_PGAPDL_BIT                          (1)
#define REG_VA7632B_CDC_PGA_CDC_PGAPDL_MASK                         (0x00000002)
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTEL                            ((((REG_VA7632B_CDC_PGA) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTEL_BIT                        (0)
#define REG_VA7632B_CDC_PGA_CDC_PGAMUTEL_MASK                       (0x00000001)

/*
CDC_PGA_PD_IREF: None
[4:4]: power down current reference of right PGA
[0:0]: power down current reference of left PGA
*/
#define REG_VA7632B_CDC_PGA_PD_IREF                                 (REG_VA7632B_BASE + 0x004D)
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_R               ((((REG_VA7632B_CDC_PGA_PD_IREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_R_BIT           (4)
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_R_MASK          (0x00000010)
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_L               ((((REG_VA7632B_CDC_PGA_PD_IREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_L_BIT           (0)
#define REG_VA7632B_CDC_PGA_PD_IREF_CDC_PGA_PD_IREF_L_MASK          (0x00000001)

/*
CDC_LINEIN_SEL: None
[4:4]: 1:select right channel of line in; 0:select right channel of MIC.
[0:0]: 1:select left channel of line in; 0:select left channel of MIC.
*/
#define REG_VA7632B_CDC_LINEIN_SEL                                  (REG_VA7632B_BASE + 0x004E)
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELR                  ((((REG_VA7632B_CDC_LINEIN_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELR_BIT              (4)
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELR_MASK             (0x00000010)
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELL                  ((((REG_VA7632B_CDC_LINEIN_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELL_BIT              (0)
#define REG_VA7632B_CDC_LINEIN_SEL_CDC_LINEIN_SELL_MASK             (0x00000001)

/*
CDC_DACCTL0: None
[4:7]: 0000:normal work; right channel (No use)
[0:3]: 0000:normal work; left channel (No use)
*/
#define REG_VA7632B_CDC_DACCTL0                                     (REG_VA7632B_BASE + 0x004F)
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_R                      ((((REG_VA7632B_CDC_DACCTL0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_R_BIT                  (4)
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_R_MASK                 (0x000000F0)
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_L                      ((((REG_VA7632B_CDC_DACCTL0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_L_BIT                  (0)
#define REG_VA7632B_CDC_DACCTL0_CDC_DACDEBUG_L_MASK                 (0x0000000F)

/*
CDC_DACCTL2: power down dac lpf part,active low
[7:7]: power down dac lpf part,active low
[6:6]: power down dac digital part,active low
[3:3]: power down dac lpf part,active low
[2:2]: power down dac digital part,active low
*/
#define REG_VA7632B_CDC_DACCTL2                                     (REG_VA7632B_BASE + 0x0050)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_R                      ((((REG_VA7632B_CDC_DACCTL2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_R_BIT                  (7)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_R_MASK                 (0x00000080)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_R                      ((((REG_VA7632B_CDC_DACCTL2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_R_BIT                  (6)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_R_MASK                 (0x00000040)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_L                      ((((REG_VA7632B_CDC_DACCTL2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_L_BIT                  (3)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDLPF_L_MASK                 (0x00000008)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_L                      ((((REG_VA7632B_CDC_DACCTL2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_L_BIT                  (2)
#define REG_VA7632B_CDC_DACCTL2_CDC_DACPDDIG_L_MASK                 (0x00000004)

/*
CDC_DACCTL3: None
[4:4]: Reset dac digital part, active high
[0:0]: Reset dac digital part, active high
*/
#define REG_VA7632B_CDC_DACCTL3                                     (REG_VA7632B_BASE + 0x0051)
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_R                     ((((REG_VA7632B_CDC_DACCTL3) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_R_BIT                 (4)
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_R_MASK                (0x00000010)
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_L                     ((((REG_VA7632B_CDC_DACCTL3) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_L_BIT                 (0)
#define REG_VA7632B_CDC_DACCTL3_CDC_DACRSTDIG_L_MASK                (0x00000001)

/*
CDC_HPDOCN: None
[0:3]: Over current control of output NMOSFET. Normal value is 0x1100; 2 channels  (No use)
*/
#define REG_VA7632B_CDC_HPDOCN                                      (REG_VA7632B_BASE + 0x0052)
#define REG_VA7632B_CDC_HPDOCN_CDC_HPDOCN                           ((((REG_VA7632B_CDC_HPDOCN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPDOCN_CDC_HPDOCN_BIT                       (0)
#define REG_VA7632B_CDC_HPDOCN_CDC_HPDOCN_MASK                      (0x0000000F)

/*
CDC_HPDOCP: None
[0:3]: Over current control of output PMOSFET. Normal value is 0x1100; 2 channels (No use)
*/
#define REG_VA7632B_CDC_HPDOCP                                      (REG_VA7632B_BASE + 0x0053)
#define REG_VA7632B_CDC_HPDOCP_CDC_HPDOCP                           ((((REG_VA7632B_CDC_HPDOCP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPDOCP_CDC_HPDOCP_BIT                       (0)
#define REG_VA7632B_CDC_HPDOCP_CDC_HPDOCP_MASK                      (0x0000000F)

/*
CDC_HP0: None
[7:7]: Over current protection; right channel;0--disable (No use)
[6:6]: Mute PA.1: mute PA, 0: unmute PA; right channel
[3:3]: Over current protection; left channel;0--disable (No use)
[2:2]: Mute PA.1: mute PA, 0: unmute PA; left channel
*/
#define REG_VA7632B_CDC_HP0                                         (REG_VA7632B_BASE + 0x0054)
#define REG_VA7632B_CDC_HP0_CDC_HPOC_R                              ((((REG_VA7632B_CDC_HP0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_VA7632B_CDC_HP0_CDC_HPOC_R_BIT                          (7)
#define REG_VA7632B_CDC_HP0_CDC_HPOC_R_MASK                         (0x00000080)
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_R                            ((((REG_VA7632B_CDC_HP0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_R_BIT                        (6)
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_R_MASK                       (0x00000040)
#define REG_VA7632B_CDC_HP0_CDC_HPOC_L                              ((((REG_VA7632B_CDC_HP0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_CDC_HP0_CDC_HPOC_L_BIT                          (3)
#define REG_VA7632B_CDC_HP0_CDC_HPOC_L_MASK                         (0x00000008)
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_L                            ((((REG_VA7632B_CDC_HP0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_L_BIT                        (2)
#define REG_VA7632B_CDC_HP0_CDC_HPMUTE_L_MASK                       (0x00000004)

/*
CDC_HP1: None
[6:6]: enable cross zero of headphone PA, active high
[5:5]: Power down vcom. (No use)
[4:4]: power down headphone PA,active low
[2:2]: enable cross zero of headphone PA, active high
[1:1]: Power down vcom. (No use)
[0:0]: power down headphone PA,active low
*/
#define REG_VA7632B_CDC_HP1                                         (REG_VA7632B_BASE + 0x0055)
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_R                             ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_R_BIT                         (6)
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_R_MASK                        (0x00000040)
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_R                           ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_R_BIT                       (5)
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_R_MASK                      (0x00000020)
#define REG_VA7632B_CDC_HP1_CDC_HPPD_R                              ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPD_R_BIT                          (4)
#define REG_VA7632B_CDC_HP1_CDC_HPPD_R_MASK                         (0x00000010)
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_L                             ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_L_BIT                         (2)
#define REG_VA7632B_CDC_HP1_CDC_HPPOP_L_MASK                        (0x00000004)
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_L                           ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_L_BIT                       (1)
#define REG_VA7632B_CDC_HP1_CDC_HPPDVCM_L_MASK                      (0x00000002)
#define REG_VA7632B_CDC_HP1_CDC_HPPD_L                              ((((REG_VA7632B_CDC_HP1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HP1_CDC_HPPD_L_BIT                          (0)
#define REG_VA7632B_CDC_HP1_CDC_HPPD_L_MASK                         (0x00000001)

/*
CDC_HP_STATUS: None
[5:5]: Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
[4:4]: Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
[1:1]: Headphone PA cross zero output. Headphone Volume Change Indication (Enabled by pop: When POP = High and Signal Cross Zero, VCHNG_L = High, otherwise, it is Low. When POP = Low or PWD = High, VCHNG_L always High)
[0:0]: Headphone Short Current Indication Bit, active high. (When PWD = High, SHORTI = Low). (No use)
*/
#define REG_VA7632B_CDC_HP_STATUS                                   (REG_VA7632B_BASE + 0x0056)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_R                       ((((REG_VA7632B_CDC_HP_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_R_BIT                   (5)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_R_MASK                  (0x00000020)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_R                     ((((REG_VA7632B_CDC_HP_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_R_BIT                 (4)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_R_MASK                (0x00000010)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_L                       ((((REG_VA7632B_CDC_HP_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_L_BIT                   (1)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPVCH_L_MASK                  (0x00000002)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_L                     ((((REG_VA7632B_CDC_HP_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_L_BIT                 (0)
#define REG_VA7632B_CDC_HP_STATUS_CDC_HPSHORT_L_MASK                (0x00000001)

/*
CDC_HPVOL_4BIT: None
[4:7]: headphone PA volume control, -18dB --12dB, 2dB/step, (0000b = -18dB) (1111b = +12dB)
[0:3]: headphone PA volume control, -18dB --12dB, 2dB/step, (0000b = -18dB) (1111b = +12dB)
*/
#define REG_VA7632B_CDC_HPVOL_4BIT                                  (REG_VA7632B_BASE + 0x0057)
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_R                 ((((REG_VA7632B_CDC_HPVOL_4BIT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_R_BIT             (4)
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_R_MASK            (0x000000F0)
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_L                 ((((REG_VA7632B_CDC_HPVOL_4BIT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_L_BIT             (0)
#define REG_VA7632B_CDC_HPVOL_4BIT_CDC_HPVOL_4BIT_L_MASK            (0x0000000F)

/*
CDC_HPVOL_6BIT_L: None
[0:5]: HPA gain adjusting for left/right channel; gain table: Gain:  -18dB, -17.5dB, -17dB, , 11dB, 11.5dB, 12dB  (0.5dB gain step)CDC_HPVOL_6BIT_L/R[5:0]:  0, 1, 2, , 58,  59, 60   (max gain(12dB) for either 60,61,62,63)
*/
#define REG_VA7632B_CDC_HPVOL_6BIT_L                                (REG_VA7632B_BASE + 0x0058)
#define REG_VA7632B_CDC_HPVOL_6BIT_L_CDC_HPVOL_6BIT_L               ((((REG_VA7632B_CDC_HPVOL_6BIT_L) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPVOL_6BIT_L_CDC_HPVOL_6BIT_L_BIT           (0)
#define REG_VA7632B_CDC_HPVOL_6BIT_L_CDC_HPVOL_6BIT_L_MASK          (0x0000003F)

/*
CDC_HPVOL_6BIT_R: None
[0:5]: HPA gain adjusting for left/right channel; gain table: Gain:  -18dB, -17.5dB, -17dB, , 11dB, 11.5dB, 12dB  (0.5dB gain step)CDC_HPVOL_6BIT_L/R[5:0]:  0, 1, 2, , 58,  59, 60   (max gain(12dB) for either 60,61,62,63)
*/
#define REG_VA7632B_CDC_HPVOL_6BIT_R                                (REG_VA7632B_BASE + 0x0059)
#define REG_VA7632B_CDC_HPVOL_6BIT_R_CDC_HPVOL_6BIT_R               ((((REG_VA7632B_CDC_HPVOL_6BIT_R) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPVOL_6BIT_R_CDC_HPVOL_6BIT_R_BIT           (0)
#define REG_VA7632B_CDC_HPVOL_6BIT_R_CDC_HPVOL_6BIT_R_MASK          (0x0000003F)

/*
CDC_MIX: None
[7:7]: power down mixer,active low
[6:6]: mute mixer, active high
[3:3]: power down mixer,active low
[2:2]: mute mixer, active high
*/
#define REG_VA7632B_CDC_MIX                                         (REG_VA7632B_BASE + 0x005A)
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_R                             ((((REG_VA7632B_CDC_MIX) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_R_BIT                         (7)
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_R_MASK                        (0x00000080)
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_R                           ((((REG_VA7632B_CDC_MIX) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_R_BIT                       (6)
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_R_MASK                      (0x00000040)
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_L                             ((((REG_VA7632B_CDC_MIX) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_L_BIT                         (3)
#define REG_VA7632B_CDC_MIX_CDC_MIXPD_L_MASK                        (0x00000008)
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_L                           ((((REG_VA7632B_CDC_MIX) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_L_BIT                       (2)
#define REG_VA7632B_CDC_MIX_CDC_MIXMUTE_L_MASK                      (0x00000004)

/*
CDC_HPLP: None
[4:5]: Set output current of PA. 00->800uA,01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
[0:1]: Set output current of PA. 00->800uA,01 and 10 -> 1.6mA, 11-> 2.4mA. (No use)
*/
#define REG_VA7632B_CDC_HPLP                                        (REG_VA7632B_BASE + 0x005B)
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_R                             ((((REG_VA7632B_CDC_HPLP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_R_BIT                         (4)
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_R_MASK                        (0x00000030)
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_L                             ((((REG_VA7632B_CDC_HPLP) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_L_BIT                         (0)
#define REG_VA7632B_CDC_HPLP_CDC_HPLP_L_MASK                        (0x00000003)

/*
CDC_PWD_STBF: None
[4:4]: power down pop noise buffer, active low, right channel
[0:0]: power down pop noise buffer, active low, left channel
*/
#define REG_VA7632B_CDC_PWD_STBF                                    (REG_VA7632B_BASE + 0x005C)
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_R                     ((((REG_VA7632B_CDC_PWD_STBF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_R_BIT                 (4)
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_R_MASK                (0x00000010)
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_L                     ((((REG_VA7632B_CDC_PWD_STBF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_L_BIT                 (0)
#define REG_VA7632B_CDC_PWD_STBF_CDC_PWD_STBF_L_MASK                (0x00000001)

/*
CDC_HP_PWD_IREF: None
[4:4]: power down current reference of headphone PA, right channel,active low
[0:0]: power down current reference of headphone PA, left channel,active low
*/
#define REG_VA7632B_CDC_HP_PWD_IREF                                 (REG_VA7632B_BASE + 0x005D)
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_R               ((((REG_VA7632B_CDC_HP_PWD_IREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_R_BIT           (4)
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_R_MASK          (0x00000010)
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_L               ((((REG_VA7632B_CDC_HP_PWD_IREF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_L_BIT           (0)
#define REG_VA7632B_CDC_HP_PWD_IREF_CDC_HP_PWD_IREF_L_MASK          (0x00000001)

/*
CDC_DF_RESERVE: reserverd for differential output. (No use)
*/
#define REG_VA7632B_CDC_DF_RESERVE                                  (REG_VA7632B_BASE + 0x005E)

/*
CDC_POP_DEBUG: reserverd for pop noise debug. (No use)
*/
#define REG_VA7632B_CDC_POP_DEBUG                                   (REG_VA7632B_BASE + 0x005F)

/*
CDC_RESERVE: bit0: CDC_CBOOST_L;  bit4:CDC_CBOOST_R  (SC4,5,6 version)
*/
#define REG_VA7632B_CDC_RESERVE                                     (REG_VA7632B_BASE + 0x0060)

/*
CDC_RESERVE1: CDC reserve.
*/
#define REG_VA7632B_CDC_RESERVE1                                    (REG_VA7632B_BASE + 0x0061)

/*
CDC_PGA_CUR_DG: None
[4:6]: right PGA current option:000--0.25I,001--0.5I,010--0.75I,011--1I,100--1.25I,101--1.5I,110--1.75I,111--2I debug pin
[0:2]: left PGA current option:000--0.25I,001--0.5I,010--0.75I,011--1I,100--1.25I,101--1.5I,110--1.75I,111--2I debug pin
*/
#define REG_VA7632B_CDC_PGA_CUR_DG                                  (REG_VA7632B_BASE + 0x0080)
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_R                    ((((REG_VA7632B_CDC_PGA_CUR_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_R_BIT                (4)
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_R_MASK               (0x00000070)
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_L                    ((((REG_VA7632B_CDC_PGA_CUR_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_L_BIT                (0)
#define REG_VA7632B_CDC_PGA_CUR_DG_CDC_PGA_CUR_L_MASK               (0x00000007)

/*
CDC_ADCUR_DG: None
[4:6]: right ADC current option:000--0.25I,001--0.5I,010--0.75I,011--1I,100--1.25I,101--1.5I,110--1.75I,111--2I  (debug pin)
[0:2]: left ADC current option:000--0.25I,001--0.5I,010--0.75I,011--1I,100--1.25I,101--1.5I,110--1.75I,111--2I  (debug pin)
*/
#define REG_VA7632B_CDC_ADCUR_DG                                    (REG_VA7632B_BASE + 0x0081)
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURR                         ((((REG_VA7632B_CDC_ADCUR_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURR_BIT                     (4)
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURR_MASK                    (0x00000070)
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURL                         ((((REG_VA7632B_CDC_ADCUR_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURL_BIT                     (0)
#define REG_VA7632B_CDC_ADCUR_DG_CDC_ADCURL_MASK                    (0x00000007)

/*
CDC_REF_DG: None
[4:4]: (debug pin)  (no use)
[2:3]: (debug pin)  (no use)
[0:1]: current option of Reference block:00 --0.25I ,01--0.5I ,10 --1I ,11--1.3I debug pin
*/
#define REG_VA7632B_CDC_REF_DG                                      (REG_VA7632B_BASE + 0x0082)
#define REG_VA7632B_CDC_REF_DG_CDC_VCMSEL                           ((((REG_VA7632B_CDC_REF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_REF_DG_CDC_VCMSEL_BIT                       (4)
#define REG_VA7632B_CDC_REF_DG_CDC_VCMSEL_MASK                      (0x00000010)
#define REG_VA7632B_CDC_REF_DG_CDC_REFVRFSEL                        ((((REG_VA7632B_CDC_REF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_REF_DG_CDC_REFVRFSEL_BIT                    (2)
#define REG_VA7632B_CDC_REF_DG_CDC_REFVRFSEL_MASK                   (0x0000000C)
#define REG_VA7632B_CDC_REF_DG_CDC_REFIOPT                          ((((REG_VA7632B_CDC_REF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_REF_DG_CDC_REFIOPT_BIT                      (0)
#define REG_VA7632B_CDC_REF_DG_CDC_REFIOPT_MASK                     (0x00000003)

/*
CDC_VREF_SEL_DG: None
[0:2]: CDC vref option. 000-1.2V, 001-1.3V, 010-1.4v, 011-1.5v, 100-1.6v, 101-1.7v, 110-1.8v, 111-2.0v. (debug pin)
*/
#define REG_VA7632B_CDC_VREF_SEL_DG                                 (REG_VA7632B_BASE + 0x0083)
#define REG_VA7632B_CDC_VREF_SEL_DG_CDC_VREF_SEL                    ((((REG_VA7632B_CDC_VREF_SEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_VREF_SEL_DG_CDC_VREF_SEL_BIT                (0)
#define REG_VA7632B_CDC_VREF_SEL_DG_CDC_VREF_SEL_MASK               (0x00000007)

/*
CDC_VCOM_SEL_DG: None
[0:2]: CDC vcom option. 100-0.4583vref, 101-0.475vref, 110-0.5vref, 111-0.5417vref. (debug pin)
*/
#define REG_VA7632B_CDC_VCOM_SEL_DG                                 (REG_VA7632B_BASE + 0x0084)
#define REG_VA7632B_CDC_VCOM_SEL_DG_CDC_VCOM_SEL                    ((((REG_VA7632B_CDC_VCOM_SEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_VCOM_SEL_DG_CDC_VCOM_SEL_BIT                (0)
#define REG_VA7632B_CDC_VCOM_SEL_DG_CDC_VCOM_SEL_MASK               (0x00000007)

/*
CDC_CTSC_DG: None
[2:3]: current option of switch cap buffer:00 -- 0.4I, 01 -- 0.5I, 10 -- 0.335I, 11 -- 0.2I debug pin
[0:1]: Continuous time buffer current option:00 -- 0.4I, 01 -- 0.5I, 10 -- 0.335I, 11 -- 0.2I debug pin
*/
#define REG_VA7632B_CDC_CTSC_DG                                     (REG_VA7632B_BASE + 0x0085)
#define REG_VA7632B_CDC_CTSC_DG_CDC_SCBUFFICTL                      ((((REG_VA7632B_CDC_CTSC_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_CTSC_DG_CDC_SCBUFFICTL_BIT                  (2)
#define REG_VA7632B_CDC_CTSC_DG_CDC_SCBUFFICTL_MASK                 (0x0000000C)
#define REG_VA7632B_CDC_CTSC_DG_CDC_CTBUFFICTL                      ((((REG_VA7632B_CDC_CTSC_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_CTSC_DG_CDC_CTBUFFICTL_BIT                  (0)
#define REG_VA7632B_CDC_CTSC_DG_CDC_CTBUFFICTL_MASK                 (0x00000003)

/*
CDC_MBSEL_DG: None
[0:1]: Mbias output voltage selection, 00:1.5V,01:1.83V,10:2V,11:2.2V (debug pin)
*/
#define REG_VA7632B_CDC_MBSEL_DG                                    (REG_VA7632B_BASE + 0x0086)
#define REG_VA7632B_CDC_MBSEL_DG_CDC_MBSEL                          ((((REG_VA7632B_CDC_MBSEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_MBSEL_DG_CDC_MBSEL_BIT                      (0)
#define REG_VA7632B_CDC_MBSEL_DG_CDC_MBSEL_MASK                     (0x00000003)

/*
CDC_REFPD_HIGH_DG: None
[0:3]: bit3-bit0:vdd/2,current option.Bit4-5vdd/9.bit5-4vdd/9. debug pin(no used in E version)
*/
#define REG_VA7632B_CDC_REFPD_HIGH_DG                               (REG_VA7632B_BASE + 0x0087)
#define REG_VA7632B_CDC_REFPD_HIGH_DG_CDC_REFPD                     ((((REG_VA7632B_CDC_REFPD_HIGH_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_REFPD_HIGH_DG_CDC_REFPD_BIT                 (0)
#define REG_VA7632B_CDC_REFPD_HIGH_DG_CDC_REFPD_MASK                (0x0000000F)

/*
CDC_DACCTL1_DG: 0: normal work; right channeldebug pin
[6:6]: disable DWA in DAC, active high debug pin
[4:5]: DAC control signal, only for debug.debug pin
[3:3]: 0: normal work; left channeldebug pin
[2:2]: disable DWA in DAC, active high debug pin
[0:1]: DAC control signal, only for debug.debug pin
*/
#define REG_VA7632B_CDC_DACCTL1_DG                                  (REG_VA7632B_BASE + 0x0088)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_R                  ((((REG_VA7632B_CDC_DACCTL1_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_R_BIT              (6)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_R_MASK             (0x00000040)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_R                     ((((REG_VA7632B_CDC_DACCTL1_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_R_BIT                 (4)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_R_MASK                (0x00000030)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACMODE_L                    ((((REG_VA7632B_CDC_DACCTL1_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACMODE_L_BIT                (3)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACMODE_L_MASK               (0x00000008)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_L                  ((((REG_VA7632B_CDC_DACCTL1_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_L_BIT              (2)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACDISDWA_L_MASK             (0x00000004)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_L                     ((((REG_VA7632B_CDC_DACCTL1_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_L_BIT                 (0)
#define REG_VA7632B_CDC_DACCTL1_DG_CDC_DACCTL_L_MASK                (0x00000003)

/*
CDC_DACCTL2_DG: None
[4:5]: DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
[0:1]: DAC lpf current option: 00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
*/
#define REG_VA7632B_CDC_DACCTL2_DG                                  (REG_VA7632B_BASE + 0x0089)
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_R                 ((((REG_VA7632B_CDC_DACCTL2_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_R_BIT             (4)
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_R_MASK            (0x00000030)
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_L                 ((((REG_VA7632B_CDC_DACCTL2_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_L_BIT             (0)
#define REG_VA7632B_CDC_DACCTL2_DG_CDC_DACLPFICTL_L_MASK            (0x00000003)

/*
CDC_MIX_DG: None
[4:5]: current option of Mixer:  00:1I ,01:1.2I ,10:0.8I ,11:0.5I debug pin
[0:1]: current option of Mixer:  00:1I ,01:1.2I ,10:0.8I ,11:0.5I debug pin
*/
#define REG_VA7632B_CDC_MIX_DG                                      (REG_VA7632B_BASE + 0x008A)
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_R                        ((((REG_VA7632B_CDC_MIX_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_R_BIT                    (4)
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_R_MASK                   (0x00000030)
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_L                        ((((REG_VA7632B_CDC_MIX_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_L_BIT                    (0)
#define REG_VA7632B_CDC_MIX_DG_CDC_MIXICTL_L_MASK                   (0x00000003)

/*
CDC_PD_OUT_R_DG: None
[6:6]: power down output stage N terminal--2 (64 fingers), active low, right channel debug pin640uA
[5:5]: power down output stage N terminal--1 (48 fingers), active low, right channel debug pin480uA
[4:4]: power down output stage N terminal--0 (48 fingers), active low, right channel debug pin480uA
[2:2]: power down output stage P terminal--2 (64 fingers), active low, right channel debug pin640uA
[1:1]: power down output stage P terminal--1 (48 fingers), active low, right channel debug pin480uA
[0:0]: power down output stage P terminal--0 (48 fingers), active low, right channel debug pin480uA
*/
#define REG_VA7632B_CDC_PD_OUT_R_DG                                 (REG_VA7632B_BASE + 0x008B)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN2I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN2I_R_BIT             (6)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN2I_R_MASK            (0x00000040)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN1I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN1I_R_BIT             (5)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN1I_R_MASK            (0x00000020)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN0I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN0I_R_BIT             (4)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTN0I_R_MASK            (0x00000010)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP2I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP2I_R_BIT             (2)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP2I_R_MASK            (0x00000004)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP1I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP1I_R_BIT             (1)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP1I_R_MASK            (0x00000002)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP0I_R                 ((((REG_VA7632B_CDC_PD_OUT_R_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP0I_R_BIT             (0)
#define REG_VA7632B_CDC_PD_OUT_R_DG_CDC_PD_OUTP0I_R_MASK            (0x00000001)

/*
CDC_PD_OUT_L_DG: None
[6:6]: power down output stage N terminal--2 (64 fingers), active low, left channel debug pin,640uA
[5:5]: power down output stage N terminal--1 (48 fingers), active low, left channel debug pin480uA
[4:4]: power down output stage N terminal--0 (48 fingers), active low, left channel debug pin480uA
[2:2]: power down output stage P terminal--2 (64 fingers), active low, left channel debug pin640uA
[1:1]: power down output stage P terminal--1 (48 fingers), active low, left channel debug pin480uA
[0:0]: power down output stage P terminal--0 (48 fingers), active low, left channel debug pin480uA
*/
#define REG_VA7632B_CDC_PD_OUT_L_DG                                 (REG_VA7632B_BASE + 0x008C)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN2I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN2I_L_BIT             (6)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN2I_L_MASK            (0x00000040)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN1I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN1I_L_BIT             (5)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN1I_L_MASK            (0x00000020)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN0I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN0I_L_BIT             (4)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTN0I_L_MASK            (0x00000010)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP2I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP2I_L_BIT             (2)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP2I_L_MASK            (0x00000004)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP1I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP1I_L_BIT             (1)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP1I_L_MASK            (0x00000002)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP0I_L                 ((((REG_VA7632B_CDC_PD_OUT_L_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP0I_L_BIT             (0)
#define REG_VA7632B_CDC_PD_OUT_L_DG_CDC_PD_OUTP0I_L_MASK            (0x00000001)

/*
CDC_ICTL_STBF_DG: None
[4:5]: POP noise buffer current option, right channel:00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
[0:1]: POP noise buffer current option, left channel:00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
*/
#define REG_VA7632B_CDC_ICTL_STBF_DG                                (REG_VA7632B_BASE + 0x008D)
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_R                ((((REG_VA7632B_CDC_ICTL_STBF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_R_BIT            (4)
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_R_MASK           (0x00000030)
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_L                ((((REG_VA7632B_CDC_ICTL_STBF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_L_BIT            (0)
#define REG_VA7632B_CDC_ICTL_STBF_DG_CDC_ICTL_STBF_L_MASK           (0x00000003)

/*
CDC_PD_CHARGE_DG: None
[4:7]: power down resistor divider, active low, right channel debug pin
[0:3]: power down resistor divider, active low, left channel debug pin
*/
#define REG_VA7632B_CDC_PD_CHARGE_DG                                (REG_VA7632B_BASE + 0x008E)
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_R                ((((REG_VA7632B_CDC_PD_CHARGE_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_R_BIT            (4)
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_R_MASK           (0x000000F0)
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_L                ((((REG_VA7632B_CDC_PD_CHARGE_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_L_BIT            (0)
#define REG_VA7632B_CDC_PD_CHARGE_DG_CDC_PD_CHARGE_L_MASK           (0x0000000F)

/*
CDC_PD_FD_DG: None
[6:6]: power down common mode feedback, change mode from full differential to single end;active high, right channel (debug pin)
[5:5]: power down NMOS current source, change mode from full differential to single end;active high, right channel (debug pin)
[4:4]: power down PMOS current source, change mode from full differential to single end;active high, right channel (debug pin)
[2:2]: power down common mode feedback, change mode from full differential to single end;active high, left channel (debug pin)
[1:1]: power down NMOS current source, change mode from full differential to single end;active high, left channel (debug pin)
[0:0]: power down PMOS current source, change mode from full differential to single end;active high, left channel (debug pin)
*/
#define REG_VA7632B_CDC_PD_FD_DG                                    (REG_VA7632B_BASE + 0x008F)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_R                      ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_R_BIT                  (6)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_R_MASK                 (0x00000040)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_R                     ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_R_BIT                 (5)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_R_MASK                (0x00000020)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_R                     ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_R_BIT                 (4)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_R_MASK                (0x00000010)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_L                      ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_L_BIT                  (2)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_CMFB_L_MASK                 (0x00000004)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_L                     ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_L_BIT                 (1)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDNCS_L_MASK                (0x00000002)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_L                     ((((REG_VA7632B_CDC_PD_FD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_L_BIT                 (0)
#define REG_VA7632B_CDC_PD_FD_DG_CDC_PD_FDPCS_L_MASK                (0x00000001)

/*
CDC_PWD_HFOUT_DG: None
[4:6]: power down pop noise buffer driver, active low, right channel debug pinbit0:10uA, bit1:20uA, bit2:40uA
[0:2]: power down pop noise buffer driver, active low, left channel debug pinbit0:10uA, bit1:20uA, bit2:40uA
*/
#define REG_VA7632B_CDC_PWD_HFOUT_DG                                (REG_VA7632B_BASE + 0x0090)
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_R                ((((REG_VA7632B_CDC_PWD_HFOUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_R_BIT            (4)
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_R_MASK           (0x00000070)
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_L                ((((REG_VA7632B_CDC_PWD_HFOUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_L_BIT            (0)
#define REG_VA7632B_CDC_PWD_HFOUT_DG_CDC_PWD_HFOUT_L_MASK           (0x00000007)

/*
CDC_HP_ICTL_DG: None
[4:5]: current option of headphone PA, right channel:00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
[0:1]: current option of headphone PA, left channel:00 -- 1I, 01 -- 1.2I, 10 -- 0.8I, 11 -- 0.5I debug pin
*/
#define REG_VA7632B_CDC_HP_ICTL_DG                                  (REG_VA7632B_BASE + 0x0091)
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_R                    ((((REG_VA7632B_CDC_HP_ICTL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_R_BIT                (4)
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_R_MASK               (0x00000030)
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_L                    ((((REG_VA7632B_CDC_HP_ICTL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_L_BIT                (0)
#define REG_VA7632B_CDC_HP_ICTL_DG_CDC_HP_ICTL_L_MASK               (0x00000003)

/*
CDC_HPPD_OUT_DG: None
[5:5]: power down headphone PA output driver,partly;active high debug pin
[4:4]: power down headphone PA output driver,partly;active high debug pin
[1:1]: power down headphone PA output driver,partly;active high debug pin
[0:0]: power down headphone PA output driver,partly;active high debug pin
*/
#define REG_VA7632B_CDC_HPPD_OUT_DG                                 (REG_VA7632B_BASE + 0x0092)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_R                ((((REG_VA7632B_CDC_HPPD_OUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_R_BIT            (5)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_R_MASK           (0x00000020)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_R                ((((REG_VA7632B_CDC_HPPD_OUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_R_BIT            (4)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_R_MASK           (0x00000010)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_L                ((((REG_VA7632B_CDC_HPPD_OUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_L_BIT            (1)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT2I_L_MASK           (0x00000002)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_L                ((((REG_VA7632B_CDC_HPPD_OUT_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_L_BIT            (0)
#define REG_VA7632B_CDC_HPPD_OUT_DG_CDC_HPPD_OUT1I_L_MASK           (0x00000001)

/*
CDC_NMILLER_DG: None
[0:0]: control miller cap, 0=miller, 1=no miller debug pin
*/
#define REG_VA7632B_CDC_NMILLER_DG                                  (REG_VA7632B_BASE + 0x0093)
#define REG_VA7632B_CDC_NMILLER_DG_CDC_NMILLER                      ((((REG_VA7632B_CDC_NMILLER_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_NMILLER_DG_CDC_NMILLER_BIT                  (0)
#define REG_VA7632B_CDC_NMILLER_DG_CDC_NMILLER_MASK                 (0x00000001)

/*
CDC_CBOOST_DG: None
[4:4]: Boost miller cap in HPA_R. Active high.debug pin
[0:0]: Boost miller cap in HPA_L. Active high.debug pin
*/
#define REG_VA7632B_CDC_CBOOST_DG                                   (REG_VA7632B_BASE + 0x0094)
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_R                      ((((REG_VA7632B_CDC_CBOOST_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_R_BIT                  (4)
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_R_MASK                 (0x00000010)
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_L                      ((((REG_VA7632B_CDC_CBOOST_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_L_BIT                  (0)
#define REG_VA7632B_CDC_CBOOST_DG_CDC_CBOOST_L_MASK                 (0x00000001)

/*
CDC_VGATE_SEL_DG: None
[0:1]: CDC gate voltage option for switches.11:2.25V, 10:2.0V, 01:1.8V, 00:1.5V.(debug pin)
*/
#define REG_VA7632B_CDC_VGATE_SEL_DG                                (REG_VA7632B_BASE + 0x0095)
#define REG_VA7632B_CDC_VGATE_SEL_DG_CDC_VGATE_SEL                  ((((REG_VA7632B_CDC_VGATE_SEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_VGATE_SEL_DG_CDC_VGATE_SEL_BIT              (0)
#define REG_VA7632B_CDC_VGATE_SEL_DG_CDC_VGATE_SEL_MASK             (0x00000003)

/*
CDC_VSUB_HIGH_DG: None
[4:4]: CDC substrate voltage option for HPA PMOS of right channel. 0: 7/12*avdd, 1: 8/12*avdd.(debug pin)
[0:0]: CDC substrate voltage option for HPA PMOS of left channel. 0: 7/12*avdd, 1: 8/12*avdd.(debug pin)
*/
#define REG_VA7632B_CDC_VSUB_HIGH_DG                                (REG_VA7632B_BASE + 0x0096)
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_R                ((((REG_VA7632B_CDC_VSUB_HIGH_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_R_BIT            (4)
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_R_MASK           (0x00000010)
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_L                ((((REG_VA7632B_CDC_VSUB_HIGH_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_L_BIT            (0)
#define REG_VA7632B_CDC_VSUB_HIGH_DG_CDC_VSUB_HIGH_L_MASK           (0x00000001)

/*
CDC_HPA_NATIVE_DG: None
[4:5]: CDC native/normal switch for HPA right channel.00:temp determined, 01:normal mode, 10: native mode, 11forbidden!!!  (debug pin)
[0:1]: CDC native/normal switch for HPA left channel.00:temp determined, 01:normal mode, 10: native mode, 11forbidden!!! (debug pin)
*/
#define REG_VA7632B_CDC_HPA_NATIVE_DG                               (REG_VA7632B_BASE + 0x0097)
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_R              ((((REG_VA7632B_CDC_HPA_NATIVE_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_R_BIT          (4)
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_R_MASK         (0x00000030)
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_L              ((((REG_VA7632B_CDC_HPA_NATIVE_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_L_BIT          (0)
#define REG_VA7632B_CDC_HPA_NATIVE_DG_CDC_HPA_NATIVE_L_MASK         (0x00000003)

/*
CDC_LINEIN_AMP_PD_DG: None
[0:0]: CDC line_in amplitude detect Power Down. 0on, 1: power down.
*/
#define REG_VA7632B_CDC_LINEIN_AMP_PD_DG                            (REG_VA7632B_BASE + 0x0098)
#define REG_VA7632B_CDC_LINEIN_AMP_PD_DG_CDC_LINEIN_AMP_PD          ((((REG_VA7632B_CDC_LINEIN_AMP_PD_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_LINEIN_AMP_PD_DG_CDC_LINEIN_AMP_PD_BIT      (0)
#define REG_VA7632B_CDC_LINEIN_AMP_PD_DG_CDC_LINEIN_AMP_PD_MASK     (0x00000001)

/*
CDC_LINEIN_AMP_VREF_DG: None
[0:1]: CDC VREF option for line_in amplitude detect.
*/
#define REG_VA7632B_CDC_LINEIN_AMP_VREF_DG                          (REG_VA7632B_BASE + 0x0099)
#define REG_VA7632B_CDC_LINEIN_AMP_VREF_DG_CDC_LINEIN_AMP_VREF      ((((REG_VA7632B_CDC_LINEIN_AMP_VREF_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_LINEIN_AMP_VREF_DG_CDC_LINEIN_AMP_VREF_BIT  (0)
#define REG_VA7632B_CDC_LINEIN_AMP_VREF_DG_CDC_LINEIN_AMP_VREF_MASK (0x00000003)

/*
CDC_HPAGAIN_SEP_DG: None
[0:0]: 1b0: Use the registers at "CDC_HPVOL_4BIT"  address to control the headphone PA volume of left and right channels.1b1: Use the registers at "CDC_HPVOL_6BIT_L" adress and "CDC_HPVOL_6BIT_R" address to control the headphone PA volume of left and right channels separately.
*/
#define REG_VA7632B_CDC_HPAGAIN_SEP_DG                              (REG_VA7632B_BASE + 0x009A)
#define REG_VA7632B_CDC_HPAGAIN_SEP_DG_CDC_HPAGAIN_SEP              ((((REG_VA7632B_CDC_HPAGAIN_SEP_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPAGAIN_SEP_DG_CDC_HPAGAIN_SEP_BIT          (0)
#define REG_VA7632B_CDC_HPAGAIN_SEP_DG_CDC_HPAGAIN_SEP_MASK         (0x00000001)

/*
CDC_HPAGAIN_MODE_DG: None
[0:0]: 1b0: Use {CDC_HPVOL_4BIT_R[3:0], 2b0} to control the headphone PA volume of left and right channels.1b1: Use {CDC_HPVOL_4BIT_R[3:0], CDC_HPVOL_4BIT_L[1:0]} to control the headphone PA volume of left and right channels.
*/
#define REG_VA7632B_CDC_HPAGAIN_MODE_DG                             (REG_VA7632B_BASE + 0x009B)
#define REG_VA7632B_CDC_HPAGAIN_MODE_DG_CDC_HPAGAIN_MODE            ((((REG_VA7632B_CDC_HPAGAIN_MODE_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPAGAIN_MODE_DG_CDC_HPAGAIN_MODE_BIT        (0)
#define REG_VA7632B_CDC_HPAGAIN_MODE_DG_CDC_HPAGAIN_MODE_MASK       (0x00000001)

/*
CDC_HPVOLSEL_DG: None
[4:4]: select the source of CDC_HPVOL_R[5:0]. 0:connected directly to register;1:update when CDC_HPVCH_R is switched;
[0:0]: select the source of CDC_HPVOL_L[5:0]. 0:connected directly to register;1:update when CDC_HPVCH_L is switched;
*/
#define REG_VA7632B_CDC_HPVOLSEL_DG                                 (REG_VA7632B_BASE + 0x009C)
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_R                  ((((REG_VA7632B_CDC_HPVOLSEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_R_BIT              (4)
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_R_MASK             (0x00000010)
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_L                  ((((REG_VA7632B_CDC_HPVOLSEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_L_BIT              (0)
#define REG_VA7632B_CDC_HPVOLSEL_DG_CDC_HPVOLSEL_L_MASK             (0x00000001)

/*
CDC_HPPDSEL_DG: None
[4:4]: select the source of CDC_HPPD_R. 0:connected directly to register;1:update when CDC_HPVCH_R is switched;
[0:0]: select the source of CDC_HPPD_L. 0:connected directly to register;1:update when CDC_HPVCH_L is switched;
*/
#define REG_VA7632B_CDC_HPPDSEL_DG                                  (REG_VA7632B_BASE + 0x009D)
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_R                    ((((REG_VA7632B_CDC_HPPDSEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_R_BIT                (4)
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_R_MASK               (0x00000010)
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_L                    ((((REG_VA7632B_CDC_HPPDSEL_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_L_BIT                (0)
#define REG_VA7632B_CDC_HPPDSEL_DG_CDC_HPPDSEL_L_MASK               (0x00000001)

/*
CDC_SETVCOM_DG: None
[4:4]: 1:set n terminal of MIC_R channel is VCOM; 0:disable.
[0:0]: 1:set n terminal of MIC_L channel is VCOM; 0:disable.
*/
#define REG_VA7632B_CDC_SETVCOM_DG                                  (REG_VA7632B_BASE + 0x009E)
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_R                    ((((REG_VA7632B_CDC_SETVCOM_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_R_BIT                (4)
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_R_MASK               (0x00000010)
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_L                    ((((REG_VA7632B_CDC_SETVCOM_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_L_BIT                (0)
#define REG_VA7632B_CDC_SETVCOM_DG_CDC_SETVCOM_L_MASK               (0x00000001)

/*
CDC_PULLDOWN_VCM_DG: None
[0:0]: pull down Vcom to 0, active low
*/
#define REG_VA7632B_CDC_PULLDOWN_VCM_DG                             (REG_VA7632B_BASE + 0x009F)
#define REG_VA7632B_CDC_PULLDOWN_VCM_DG_CDC_PULLDOWN_VCM            ((((REG_VA7632B_CDC_PULLDOWN_VCM_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_PULLDOWN_VCM_DG_CDC_PULLDOWN_VCM_BIT        (0)
#define REG_VA7632B_CDC_PULLDOWN_VCM_DG_CDC_PULLDOWN_VCM_MASK       (0x00000001)

/*
CDC_HP_PULLDOWN_EN_DG: None
[4:4]: pull down to ground of headphone PA output, right channel
[0:0]: pull down to ground of headphone PA output, left channel
*/
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG                           (REG_VA7632B_BASE + 0x00A0)
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_R      ((((REG_VA7632B_CDC_HP_PULLDOWN_EN_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_R_BIT  (4)
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_R_MASK (0x00000010)
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_L      ((((REG_VA7632B_CDC_HP_PULLDOWN_EN_DG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_L_BIT  (0)
#define REG_VA7632B_CDC_HP_PULLDOWN_EN_DG_CDC_HP_PULLDOWN_EN_L_MASK (0x00000001)

/*
INT_STATUS: None
[5:5]: ()KeyON 1b0
[4:4]: When Battery voltage is lower than this threshold, BAT_Low will become high
[3:3]: Indicating if charger input voltage is higher than battery voltage: 1 means charger input voltage is high enough for charging; 0 means charger input voltage is too low
[2:2]: Charging flag: 1 indicating charging; 0 indicating no charging
[1:1]: VD4~VD0 ready signal, active high.1'b0: VD4~VD0 is not ready; 1'b1: VD4~VD0 is ready to be read by 9613i.
[0:0]: line in detect; 0=detect, 1=no detect
*/
#define REG_VA7632B_INT_STATUS                                      (REG_VA7632B_BASE + 0x00C0)
#define REG_VA7632B_INT_STATUS_KEYON                                ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_INT_STATUS_KEYON_BIT                            (5)
#define REG_VA7632B_INT_STATUS_KEYON_MASK                           (0x00000020)
#define REG_VA7632B_INT_STATUS_BAT_LOW                              ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_INT_STATUS_BAT_LOW_BIT                          (4)
#define REG_VA7632B_INT_STATUS_BAT_LOW_MASK                         (0x00000010)
#define REG_VA7632B_INT_STATUS_PG                                   ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_INT_STATUS_PG_BIT                               (3)
#define REG_VA7632B_INT_STATUS_PG_MASK                              (0x00000008)
#define REG_VA7632B_INT_STATUS_STAT                                 ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_INT_STATUS_STAT_BIT                             (2)
#define REG_VA7632B_INT_STATUS_STAT_MASK                            (0x00000004)
#define REG_VA7632B_INT_STATUS_TERMB_INV                            ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_INT_STATUS_TERMB_INV_BIT                        (1)
#define REG_VA7632B_INT_STATUS_TERMB_INV_MASK                       (0x00000002)
#define REG_VA7632B_INT_STATUS_CDC_LINE_DET                         ((((REG_VA7632B_INT_STATUS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_INT_STATUS_CDC_LINE_DET_BIT                     (0)
#define REG_VA7632B_INT_STATUS_CDC_LINE_DET_MASK                    (0x00000001)

/*
INT_INV: None
[5:5]: Inverse the interrupt input signal from register "KeyON", active high.
[4:4]: Inverse the interrupt input signal from register "BAT_Low", active high.
[3:3]: Inverse the interrupt input signal from register "Pg", active high.
[2:2]: Inverse the interrupt input signal from register "Stat", active high.
[1:1]: Inverse the interrupt input signal from register "termb_inv", active high.
[0:0]: Inverse the interrupt input signal from register "CDC_LINE_DET", active high.
*/
#define REG_VA7632B_INT_INV                                         (REG_VA7632B_BASE + 0x00C1)
#define REG_VA7632B_INT_INV_INT_INV_KEYON                           ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_INT_INV_INT_INV_KEYON_BIT                       (5)
#define REG_VA7632B_INT_INV_INT_INV_KEYON_MASK                      (0x00000020)
#define REG_VA7632B_INT_INV_INT_INV_BAT_LOW                         ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_INT_INV_INT_INV_BAT_LOW_BIT                     (4)
#define REG_VA7632B_INT_INV_INT_INV_BAT_LOW_MASK                    (0x00000010)
#define REG_VA7632B_INT_INV_INT_INV_PG                              ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_INT_INV_INT_INV_PG_BIT                          (3)
#define REG_VA7632B_INT_INV_INT_INV_PG_MASK                         (0x00000008)
#define REG_VA7632B_INT_INV_INT_INV_STAT                            ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_INT_INV_INT_INV_STAT_BIT                        (2)
#define REG_VA7632B_INT_INV_INT_INV_STAT_MASK                       (0x00000004)
#define REG_VA7632B_INT_INV_INT_INV_TERMB_INV                       ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_INT_INV_INT_INV_TERMB_INV_BIT                   (1)
#define REG_VA7632B_INT_INV_INT_INV_TERMB_INV_MASK                  (0x00000002)
#define REG_VA7632B_INT_INV_INT_INV_LINE_DET                        ((((REG_VA7632B_INT_INV) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_INT_INV_INT_INV_LINE_DET_BIT                    (0)
#define REG_VA7632B_INT_INV_INT_INV_LINE_DET_MASK                   (0x00000001)

/*
INT_MASK: None
[5:5]: Close the interrupt function from register "KeyON", active high.
[4:4]: Close the interrupt function from register "BAT_Low", active high.
[3:3]: Close the interrupt function from register "Pg", active high.
[2:2]: Close the interrupt function from register "Stat", active high.
[1:1]: Close the interrupt function from register "termb_inv", active high.
[0:0]: Close the interrupt function from register "CDC_LINE_DET", active high.
*/
#define REG_VA7632B_INT_MASK                                        (REG_VA7632B_BASE + 0x00C2)
#define REG_VA7632B_INT_MASK_INT_MASK_KEYON                         ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_KEYON_BIT                     (5)
#define REG_VA7632B_INT_MASK_INT_MASK_KEYON_MASK                    (0x00000020)
#define REG_VA7632B_INT_MASK_INT_MASK_BAT_LOW                       ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_BAT_LOW_BIT                   (4)
#define REG_VA7632B_INT_MASK_INT_MASK_BAT_LOW_MASK                  (0x00000010)
#define REG_VA7632B_INT_MASK_INT_MASK_PG                            ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_PG_BIT                        (3)
#define REG_VA7632B_INT_MASK_INT_MASK_PG_MASK                       (0x00000008)
#define REG_VA7632B_INT_MASK_INT_MASK_STAT                          ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_STAT_BIT                      (2)
#define REG_VA7632B_INT_MASK_INT_MASK_STAT_MASK                     (0x00000004)
#define REG_VA7632B_INT_MASK_INT_MASK_TERMB_INV                     ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_TERMB_INV_BIT                 (1)
#define REG_VA7632B_INT_MASK_INT_MASK_TERMB_INV_MASK                (0x00000002)
#define REG_VA7632B_INT_MASK_INT_MASK_LINE_DET                      ((((REG_VA7632B_INT_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_INT_MASK_INT_MASK_LINE_DET_BIT                  (0)
#define REG_VA7632B_INT_MASK_INT_MASK_LINE_DET_MASK                 (0x00000001)

/*
W_SEL_ADC: None
[0:1]: Output signal width select of analog SDM ADC:2'b00-1bit ADC, 2'b01-2bit ADC, 2'b10-3bit ADC, 2'b11-4bit ADC
*/
#define REG_VA7632B_W_SEL_ADC                                       (REG_VA7632B_BASE + 0x00C3)
#define REG_VA7632B_W_SEL_ADC_W_SEL_ADC                             ((((REG_VA7632B_W_SEL_ADC) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_W_SEL_ADC_W_SEL_ADC_BIT                         (0)
#define REG_VA7632B_W_SEL_ADC_W_SEL_ADC_MASK                        (0x00000003)

/*
DATA_RSTN: None
[1:1]: Reset signal for serial-data interface of ADC path (serd_tx_7633 module), active low.
[0:0]: Reset signal for serial-data interface of DAC path (serd_rx_7633 module), active low.
*/
#define REG_VA7632B_DATA_RSTN                                       (REG_VA7632B_BASE + 0x00C4)
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_ADC                         ((((REG_VA7632B_DATA_RSTN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_ADC_BIT                     (1)
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_ADC_MASK                    (0x00000002)
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_DAC                         ((((REG_VA7632B_DATA_RSTN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_DAC_BIT                     (0)
#define REG_VA7632B_DATA_RSTN_SERD_RSTN_DAC_MASK                    (0x00000001)

/*
SERD_EN: None
[7:7]: dac sample rate selection.0: 48K,44.1K,24K,16k,8K1: 32K
[6:6]: pga gain synchronization mode0: pga gain sync from internal generation logic1: pga gain sync from external PAD  "ADOUTL"
[5:5]: output enable signal of bi-directional IO "ADOUTL", active low.
[4:4]: Output enable signal of bi-directional IO "RESETN", active low.
[3:3]: PGA gain synchronization for ADC path, active high.
[2:2]: PA gain synchronization for DAC path, active high.
[1:1]: Enable serial-data interface mode in ADC path, active high.
[0:0]: Enable serial-data interface mode in DAC path, active high.
*/
#define REG_VA7632B_SERD_EN                                         (REG_VA7632B_BASE + 0x00C5)
#define REG_VA7632B_SERD_EN_DAC_FS_MODE                             ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_VA7632B_SERD_EN_DAC_FS_MODE_BIT                         (7)
#define REG_VA7632B_SERD_EN_DAC_FS_MODE_MASK                        (0x00000080)
#define REG_VA7632B_SERD_EN_PGA_GAIN_SYN_MODE                       ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_VA7632B_SERD_EN_PGA_GAIN_SYN_MODE_BIT                   (6)
#define REG_VA7632B_SERD_EN_PGA_GAIN_SYN_MODE_MASK                  (0x00000040)
#define REG_VA7632B_SERD_EN_ADOUTL_OEN                              ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_VA7632B_SERD_EN_ADOUTL_OEN_BIT                          (5)
#define REG_VA7632B_SERD_EN_ADOUTL_OEN_MASK                         (0x00000020)
#define REG_VA7632B_SERD_EN_RESETN_OEN                              ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_VA7632B_SERD_EN_RESETN_OEN_BIT                          (4)
#define REG_VA7632B_SERD_EN_RESETN_OEN_MASK                         (0x00000010)
#define REG_VA7632B_SERD_EN_GAIN_SYN_ADC_EN                         ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_SERD_EN_GAIN_SYN_ADC_EN_BIT                     (3)
#define REG_VA7632B_SERD_EN_GAIN_SYN_ADC_EN_MASK                    (0x00000008)
#define REG_VA7632B_SERD_EN_GAIN_SYN_DAC_EN                         ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_SERD_EN_GAIN_SYN_DAC_EN_BIT                     (2)
#define REG_VA7632B_SERD_EN_GAIN_SYN_DAC_EN_MASK                    (0x00000004)
#define REG_VA7632B_SERD_EN_SERD_EN_ADC                             ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_SERD_EN_SERD_EN_ADC_BIT                         (1)
#define REG_VA7632B_SERD_EN_SERD_EN_ADC_MASK                        (0x00000002)
#define REG_VA7632B_SERD_EN_SERD_EN_DAC                             ((((REG_VA7632B_SERD_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_SERD_EN_SERD_EN_DAC_BIT                         (0)
#define REG_VA7632B_SERD_EN_SERD_EN_DAC_MASK                        (0x00000001)

/*
PGA_GAIN_CONF: None
[2:3]: PGA gain adjust rate for decrease mode:2'b00: 24kHz,    2'b01: 48kHz,   2'b10: 144kHz,   2'b11: 240kHz
[0:1]: PGA gain adjust rate for increase mode:2'b00: 24kHz,    2'b01: 48kHz,   2'b10: 144kHz,   2'b11: 240kHz
*/
#define REG_VA7632B_PGA_GAIN_CONF                                   (REG_VA7632B_BASE + 0x00C6)
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_DEC                      ((((REG_VA7632B_PGA_GAIN_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_DEC_BIT                  (2)
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_DEC_MASK                 (0x0000000C)
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_INC                      ((((REG_VA7632B_PGA_GAIN_CONF) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_INC_BIT                  (0)
#define REG_VA7632B_PGA_GAIN_CONF_PGA_GAIN_INC_MASK                 (0x00000003)

/*
SPORT_MODE: None
[3:3]: Serial port delay control.0: no delay.     1: add half cycles delay
[2:2]: Serial port mode selection.0: i2c mode.    1: high speed serial port.
*/
#define REG_VA7632B_SPORT_MODE                                      (REG_VA7632B_BASE + 0x00C7)
#define REG_VA7632B_SPORT_MODE_SPORT_DELAY                          ((((REG_VA7632B_SPORT_MODE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_VA7632B_SPORT_MODE_SPORT_DELAY_BIT                      (3)
#define REG_VA7632B_SPORT_MODE_SPORT_DELAY_MASK                     (0x00000008)
#define REG_VA7632B_SPORT_MODE_SPORT_MODE                           ((((REG_VA7632B_SPORT_MODE) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_VA7632B_SPORT_MODE_SPORT_MODE_BIT                       (2)
#define REG_VA7632B_SPORT_MODE_SPORT_MODE_MASK                      (0x00000004)

/*
I2C_TIMEOUT_EN: None
[0:0]: Enable I2C Timeout function, active high.
*/
#define REG_VA7632B_I2C_TIMEOUT_EN                                  (REG_VA7632B_BASE + 0x00C8)
#define REG_VA7632B_I2C_TIMEOUT_EN_I2C_TIMEOUT_EN                   ((((REG_VA7632B_I2C_TIMEOUT_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_I2C_TIMEOUT_EN_I2C_TIMEOUT_EN_BIT               (0)
#define REG_VA7632B_I2C_TIMEOUT_EN_I2C_TIMEOUT_EN_MASK              (0x00000001)

/*
I2C_TIMEOUT_MAX: None
[0:7]: The max value of the timeout counter is {i2c_timeout_max[7:0], 11'h7ff}.
*/
#define REG_VA7632B_I2C_TIMEOUT_MAX                                 (REG_VA7632B_BASE + 0x00C9)
#define REG_VA7632B_I2C_TIMEOUT_MAX_I2C_TIMEOUT_MAX                 ((((REG_VA7632B_I2C_TIMEOUT_MAX) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_I2C_TIMEOUT_MAX_I2C_TIMEOUT_MAX_BIT             (0)
#define REG_VA7632B_I2C_TIMEOUT_MAX_I2C_TIMEOUT_MAX_MASK            (0x000000FF)

/*
I2C_TEST: used for i2c r/w test
*/
#define REG_VA7632B_I2C_TEST                                        (REG_VA7632B_BASE + 0x00F0)

/*
VER_SEL: None
[1:1]: Version indicator: 0=low-end version, 1=high-end version.
[0:0]: None
*/
#define REG_VA7632B_VER_SEL                                         (REG_VA7632B_BASE + 0x00F1)
#define REG_VA7632B_VER_SEL_VER_SEL2                                ((((REG_VA7632B_VER_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_VA7632B_VER_SEL_VER_SEL2_BIT                            (1)
#define REG_VA7632B_VER_SEL_VER_SEL2_MASK                           (0x00000002)
#define REG_VA7632B_VER_SEL_VER_SEL1                                ((((REG_VA7632B_VER_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_VA7632B_VER_SEL_VER_SEL1_BIT                            (0)
#define REG_VA7632B_VER_SEL_VER_SEL1_MASK                           (0x00000001)

/*
CHIP_ID0: chip_id0
*/
#define REG_VA7632B_CHIP_ID0                                        (REG_VA7632B_BASE + 0x00F2)

/*
CHIP_ID1: chip_id1
*/
#define REG_VA7632B_CHIP_ID1                                        (REG_VA7632B_BASE + 0x00F3)

/*
CHIP_ID2: chip_id2
*/
#define REG_VA7632B_CHIP_ID2                                        (REG_VA7632B_BASE + 0x00F4)

/*
CHIP_ID3: version for metal change:VA7632B_E5: 8'b0001_0001VA7632B_F5: 8'b0001_0010
*/
#define REG_VA7632B_CHIP_ID3                                        (REG_VA7632B_BASE + 0x00F5)

#endif /* __VREG_VA7632B_H__ */
