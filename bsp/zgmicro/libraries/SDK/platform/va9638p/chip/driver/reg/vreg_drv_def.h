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
    reg

Description:
    reg

Revision History:
    Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef _VREG38P_DEF_H_
#define _VREG38P_DEF_H_

#define CKM_BASE                        (0x40000000)
#define I2S_BASE                        (0x4000c000)
#define UART_BASE                       (0x40010000)
#define GPIO_BASE                       (0x40014000)
#define AIF_BASE                        (0x40020000)
#define VSD_REG_BASE                    (0x40040000)
#define TIMER_BASE                      (0x40018000)
#define I2C_BASE                        (0x4001c000)
#define DMA_BASE                        (0x4002c000)
#define SPI_BASE                        (0x40030000)
#define ASRC_BASE                       (0)
#define HD_CPORT_BASE                   (0x40038000)
#define PWM_BASE                        (0)
#define LCD_BASE                        (0x40044000)
#define UOTG_BASE                       (0x40048000)
#define MSG_BOX_BASE                    (0x4004C000)
#define R2Z_BASE                        (0)
#define R2Z1_BASE                       (0)
#define SPDIF_BASE                      (0x40050000)
#define SPI2_BASE                       (0x40054000)
#define TRNG_BASE                       (0x40058000)
#define DWT_BASE 						            (0xE0001000)
#define FPB_BASE                        (0xE0002000)
#define NVIC_BASE                       (0xE000E000)

#define PERI_BASE             			    (0x40000000UL)
#define PERI_BIT_BAND_BASE         		  (0x42000000UL)

#define PERI_BIT_BAND(base, bit)              ((((base) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((bit) << 2))

#include "reg_timer.h"
#include "reg_lcd.h"

/**********************************************
 * NVIC Register Definitions
 **********************************************/
#if NVIC_BASE
#define REG_NVIC_ICTR                     (NVIC_BASE + 0x004)  // Interrupt Controller Type Reg.
#define REG_NVIC_ST_CTRL                 (NVIC_BASE + 0x010)  // SysTick Control and Status Reg.
#define REG_NVIC_ST_RELOAD                (NVIC_BASE + 0x014)  // SysTick Reload Value Register
#define REG_NVIC_ST_CURRENT               (NVIC_BASE + 0x018)  // SysTick Current Value Register
#define REG_NVIC_ST_CALIB                 (NVIC_BASE + 0x01C)  // SysTick Calibration Value Reg.
#define REG_NVIC_SETENA0                 (NVIC_BASE + 0x100)  // IRQ 0 to 31 Set Enable Register
#define REG_NVIC_SETENA1                 (NVIC_BASE + 0x104)  // IRQ 32 to 63 Set Enable Register
#define REG_NVIC_CLRENA0                 (NVIC_BASE + 0x180)  // IRQ 0 to 31 Clear Enable Reg.
#define REG_NVIC_CLRENA1                 (NVIC_BASE + 0x184)  // IRQ 32 to 63 Clear Enable Reg.
#define REG_NVIC_SETPEND0                 (NVIC_BASE + 0x200)  // IRQ 0 to 31 Set Pending Register
#define REG_NVIC_SETPEND1                 (NVIC_BASE + 0x204)  // IRQ 32 to 63 Set Pending Reg.
#define REG_NVIC_CLRPEND0                 (NVIC_BASE + 0x280)  // IRQ 0 to 31 Clear Pending Reg.
#define REG_NVIC_CLRPEND1                 (NVIC_BASE + 0x284)  // IRQ 32 to 63 Clear Pending Reg.
#define REG_NVIC_ACTIVE0                 (NVIC_BASE + 0x300)  // IRQ 0 to 31 Active Register
#define REG_NVIC_ACTIVE1                 (NVIC_BASE + 0x304)  // IRQ 32 to 63 Active Register
#define REG_NVIC_PRI0                     (NVIC_BASE + 0x400)  // IRQ 0 to 3 Priority Register
#define REG_NVIC_PRI1                     (NVIC_BASE + 0x404)  // IRQ 4 to 7 Priority Register
#define REG_NVIC_PRI2                     (NVIC_BASE + 0x408)  // IRQ 8 to 11 Priority Register
#define REG_NVIC_PRI3                     (NVIC_BASE + 0x40C)  // IRQ 12 to 15 Priority Register
#define REG_NVIC_PRI4                     (NVIC_BASE + 0x410)  // IRQ 16 to 19 Priority Register
#define REG_NVIC_PRI5                     (NVIC_BASE + 0x414)  // IRQ 20 to 23 Priority Register
#define REG_NVIC_PRI6                     (NVIC_BASE + 0x418)  // IRQ 24 to 27 Priority Register
#define REG_NVIC_PRI7                     (NVIC_BASE + 0x41C)  // IRQ 28 to 31 Priority Register
#define REG_NVIC_PRI8                     (NVIC_BASE + 0x420)  // IRQ 32 to 35 Priority Register
#define REG_NVIC_PRI9                     (NVIC_BASE + 0x424)  // IRQ 36 to 39 Priority Register
#define REG_NVIC_PRI10                     (NVIC_BASE + 0x428)  // IRQ 40 to 43 Priority Register
#define REG_NVIC_PRI11                     (NVIC_BASE + 0x42C)  // IRQ 44 to 47 Priority Register
#define REG_NVIC_CPUID                     (NVIC_BASE + 0xD00)  // CPUID Base Register
#define REG_NVIC_ICSR                     (NVIC_BASE + 0xD04)  // Interrupt Control State Register
#define REG_NVIC_VTOR                     (NVIC_BASE + 0xD08)  // Vector Table Offset Register
#define REG_NVIC_AIRCR                     (NVIC_BASE + 0xD0C)  // App. Int & Reset Control Reg.
#define REG_NVIC_SCR                     (NVIC_BASE + 0xD10)  // System Control Register
#define REG_NVIC_CCR                     (NVIC_BASE + 0xD14)  // Configuration Control Register
#define REG_NVIC_SYS_PRI1                 (NVIC_BASE + 0xD18)  // Sys. Handlers 4 to 7 Priority
#define REG_NVIC_SYS_PRI2                 (NVIC_BASE + 0xD1C)  // Sys. Handlers 8 to 11 Priority
#define REG_NVIC_SYS_PRI3                 (NVIC_BASE + 0xD20)  // Sys. Handlers 12 to 15 Priority
#define REG_NVIC_SHCSR                     (NVIC_BASE + 0xD24)  // System Handler Control and State
#define REG_NVIC_FSR                     (NVIC_BASE + 0xD28)  // Configurable Fault Status Reg. (MFSR/BFSR/UFSR)
#define REG_NVIC_HFSR                     (NVIC_BASE + 0xD2C)  // Hard Fault Status Register
#define REG_NVIC_DFSR                     (NVIC_BASE + 0xD30)  // Debug Status Register
#define REG_NVIC_MMAR                     (NVIC_BASE + 0xD34)  // Mem Manage Address Register
#define REG_NVIC_BFAR                     (NVIC_BASE + 0xD38)  // Bus Fault Address Register
#define REG_NVIC_AFAR                     (NVIC_BASE + 0xD3C)  // Auxiliary Fault Status Register
#define REG_NVIC_MPUTR                     (NVIC_BASE + 0xD90)  // MPU Type Register
#define REG_NVIC_MPUCR                     (NVIC_BASE + 0xD94)  // MPU Control Register
#define REG_NVIC_MPURNR                 (NVIC_BASE + 0xD98)  // MPU Region Number Register
#define REG_NVIC_MPURBAR                 (NVIC_BASE + 0xD9C)  // MPU Region Base Address Register
#define REG_NVIC_MPURASR                 (NVIC_BASE + 0xDA0)  // MPU Region Attribute & Size Reg.
#define REG_NVIC_DHCSR                     (NVIC_BASE + 0xDF0)  // Debug Halting Control and Status Register
#define REG_NVIC_DCRSR                     (NVIC_BASE + 0xDF4)  // Debug Core Register Selector Register
#define REG_NVIC_DCRDR                     (NVIC_BASE + 0xDF8)  // Debug Core Register Data Register
#define REG_NVIC_DEMCR                     (NVIC_BASE + 0xDFC)  // Debug Exception and Monitor Control Register
#define REG_NVIC_STIR                     (NVIC_BASE + 0xF00)  // Software Trigger Interrupt Reg.
#endif

/********************************************** 
 * FPB Registers Definitions
 **********************************************/
#if FPB_BASE
#define REG_FPB_CTRL                       (FPB_BASE + 0x000)
#define REG_FPB_REMAP                     (FPB_BASE + 0x004)
#define REG_FPB_COMP0                     (FPB_BASE + 0x008)
#define REG_FPB_COMP1                     (FPB_BASE + 0x00C)
#define REG_FPB_COMP2                     (FPB_BASE + 0x010)
#define REG_FPB_COMP3                     (FPB_BASE + 0x014)
#define REG_FPB_COMP4                     (FPB_BASE + 0x018)
#define REG_FPB_COMP5                     (FPB_BASE + 0x01C)
#define REG_FPB_COMP6                     (FPB_BASE + 0x020)
#define REG_FPB_COMP7                     (FPB_BASE + 0x024)
#define REG_FPB_COMP8                     (FPB_BASE + 0x028)
#define REG_FPB_COMP9                     (FPB_BASE + 0x02C)
#define REG_FPB_COMP10                     (FPB_BASE + 0x030)
#define REG_FPB_COMP11                     (FPB_BASE + 0x034)
#define REG_FPB_COMP12                     (FPB_BASE + 0x038)
#define REG_FPB_COMP13                     (FPB_BASE + 0x03C)
#define REG_FPB_COMP14                     (FPB_BASE + 0x040)
#define REG_FPB_COMP15                     (FPB_BASE + 0x044)
#define REG_FPB_COMP16                     (FPB_BASE + 0x048)
#define REG_FPB_COMP17                     (FPB_BASE + 0x04C)
#define REG_FPB_COMP18                     (FPB_BASE + 0x050)
#define REG_FPB_COMP19                     (FPB_BASE + 0x054)
#define REG_FPB_COMP20                     (FPB_BASE + 0x058)
#define REG_FPB_COMP21                     (FPB_BASE + 0x05C)
#define REG_FPB_COMP22                     (FPB_BASE + 0x060)
#define REG_FPB_COMP23                     (FPB_BASE + 0x064)
#define REG_FPB_COMP24                     (FPB_BASE + 0x068)
#define REG_FPB_COMP25                     (FPB_BASE + 0x06C)
#define REG_FPB_COMP26                     (FPB_BASE + 0x070)
#define REG_FPB_COMP27                     (FPB_BASE + 0x074)
#define REG_FPB_COMP28                     (FPB_BASE + 0x078)
#define REG_FPB_COMP29                     (FPB_BASE + 0x07C)
#define REG_FPB_COMP30                     (FPB_BASE + 0x080)
#define REG_FPB_COMP31                     (FPB_BASE + 0x084)
#endif

/********************************************** 
 * DWT Registers Definitions
 **********************************************/
#if DWT_BASE
#define REG_DWT_CONTROL                   (DWT_BASE + 0x00)
#define REG_DWT_CYCCNT                    (DWT_BASE + 0x04)
#define REG_DWT_CPICNT                    (DWT_BASE + 0x08)
#define REG_DWT_EXCCNT                    (DWT_BASE + 0x0c)
#define REG_DWT_SLEEPCNT                  (DWT_BASE + 0x10)
#endif

/********************************************** 
 * CKM Registers
 **********************************************/
#if CKM_BASE
#define REG_CKM_PCLK_CFG 				(CKM_BASE + 0x10) // PCLK Frequency Configuration Register
/*
PCLK_CFG: 
[31:4]: reserved
[3:0]: Frequency of pclk_clk is divided from hclk Fpclk = 1/(pclk_cfg[3:0]+1) *Fhclk
*/

#define REG_CKM_DIV_CLK_CFG                                (CKM_BASE + 0x30)
#define REG_CKM_DIV_CLK_CFG_DIV_CLKDLY_EN                  ((((REG_CKM_DIV_CLK_CFG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_CKM_DIV_CLK_CFG_DIV_BARDLY_REG                 ((((REG_CKM_DIV_CLK_CFG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_CKM_DIV_CLK_CFG_DIV_DLY_REG                    ((((REG_CKM_DIV_CLK_CFG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
/*
DIV_CLK_CFG: 
[31:17]: reserved
[16]: Do not use in va9635.
[15:13]: reserved
[12]: Do not use in va9635.
[11:9]: reserved
[8]: Do not use in va9635.
[7:4]: Do not use in va9635.
[3:0]: Fclk_div = (1+ div_cnt_reg)/16 * 153.6M (or 128M). 
This clock is CM3’s fclk in normal working mode and if arm_clk_sel is 1.
*/

#define REG_CKM_SEL_CLK                                    (CKM_BASE + 0x40)
#define REG_CKM_SEL_CLK_DPLL_CLK_EN                        ((((REG_CKM_SEL_CLK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_CKM_SEL_CLK_ROMPG_ACK                          ((((REG_CKM_SEL_CLK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
#define REG_CKM_SEL_CLK_ARM_CLK_SEL                        ((((REG_CKM_SEL_CLK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_CKM_SEL_CLK_CLK48M_INV_SEL                     ((((REG_CKM_SEL_CLK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_CKM_SEL_CLK_CLK24M_INV_SEL                     ((((REG_CKM_SEL_CLK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_CKM_SEL_CLK_DSP_MAISK													 0x00003103

/*
SEL_CLK: 
[31]: The clocks output from RFFE DPLL (clkin_384m, clkin_48m, clkin_24m) work stable when 1
[30]: ROM power down when 1.
Do not use in va9635.
[29:10]: reserved
[9:8]: reserved
[7:5]: reserved
[4]: CM3 Clock Selection 0: from Osillator PAD 1: from high frequency clock, which is pll_clk or clock from ck_div
[3]: Select the 48MHz clock or the inverted 48MHz clock 0: Select the 48MHz clock 1: Select the inverted 48MHz clock
[2]: Select the 24MHz clock from the inverted 24MHz clock 0: Select the 24MHz clock 1: Select the inverted 24MHz clock
[1]: reserved
[0]: reserved
*/

#define REG_CKM_FASTCLK_SEL                                (CKM_BASE + 0x50)
#define REG_CKM_FASTCLK_SEL_fastclk_128m                   ((((REG_CKM_FASTCLK_SEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FASTCLK_SEL: 
[31:1]: reserved
[0]: the fast clock source select signal.
1'h1: the fast clock is 128MHz.the default value is 1 to select the 128MHz clock path.
1'h0: the fast clock is 153MHz
*/
//#define REG_CKM_BB48MCLK_OEN                               (CKM_BASE + 0x54)
//#define REG_CKM_BB48MCLK_OEN_bb48mclk_oen                  ((((REG_CKM_BB48MCLK_OEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))

#define REG_CKM_BB48MCLK_OEN                               (CKM_BASE + 0x54)
#define REG_CKM_BB48MCLK_OEN_bb48mclk_oen                  ((((REG_CKM_BB48MCLK_OEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB48MCLK_OEN: 
[31:1]: reserved
[0]: the bb48mclk output enable signal.
1'h1: bb48mclk is enabled to output at PAD.
1'h0: bb48mclk is disabled to output at PAD.
*/
//#define REG_CKM_CM3_STCALIB                                (CKM_BASE + 0x60)
//#define REG_CKM_CM3_STCALIB_stc_noref                      ((((REG_CKM_CM3_STCALIB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
//#define REG_CKM_CM3_STCALIB_stc_skew                       ((((REG_CKM_CM3_STCALIB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))

#define REG_CKM_CM3_STCALIB                                (CKM_BASE + 0x60)
#define REG_CKM_CM3_STCALIB_stc_noref                      ((((REG_CKM_CM3_STCALIB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
#define REG_CKM_CM3_STCALIB_stc_skew                       ((((REG_CKM_CM3_STCALIB) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((30) << 2))
//##+ str  (FPGA Release 2017080201 2017-08-10)
#define REG_CKM_SRAM_CPD_CTRL                              (CKM_BASE + 0x64) 
#define REG_CKM_SRAM_PPD_CTRL                              (CKM_BASE + 0x68)
#define REG_CKM_SRAM_PPD_CTRL_sram0_prph_po                ((((REG_CKM_SRAM_PPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2)) 
#define REG_CKM_SRAM_CPD_CTRL_sram_pg_enable               ((((REG_CKM_SRAM_CPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
#define REG_CKM_ROM_PD_CTRL                                (CKM_BASE + 0x6c)
#define REG_CKM_ROM_PD_CTRL_rom_pden_idle                  ((((REG_CKM_ROM_PD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*

CM3_STCALIB: 1 = the reference clock is not provided.
[31]: 1 = the reference clock is not provided.
[30]: 1 = the calibration value is not exactly 10ms because of clock frequency. This could affect its
suitability as a software real time clock.
[29:8]: reserved
[7:0]: This value is the Reload value to use for 10ms timing. Depending on the value of SKEW, this might
be exactly 10ms or might be the closest value. If this reads as 0, then the calibration value is not known. This is probably because the reference
clock is an unknown input from the system or scalable dynamically. The default value is used to count for 10ms timing when stclk is equal to 4KHz.
*/
//FPGA
#define REG_FPGA_CLK_CFG                                (CKM_BASE + 0x70)		//del by va9638b_ckm_v2.1   //fpga ?????

#define REG_CKM_SYS_CTRL                                   (CKM_BASE + 0x80)
#define REG_CKM_SYS_CTRL_cm3bt_clk_en                      ((((REG_CKM_SYS_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_CKM_SYS_CTRL_cm3bt_sw_rst_n                    ((((REG_CKM_SYS_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_CKM_SYS_CTRL_cm3_sw_en                         ((((REG_CKM_SYS_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_CKM_SYS_CTRL_mode35                            ((((REG_CKM_SYS_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))


#define REG_CKM_CRAM_RW_CTRL                               (CKM_BASE + 0x88)	//update by va9638b_ckm_v2.1

/* VA9638P regs */

#define REG_CKM_SMICPLL_CTRL                          (CKM_BASE + 0x8C)
#define REG_CKM_SMICPLL_CTRL_rgf_smicpll_rst_sw       ((((REG_CKM_SMICPLL_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/* SMICPLL_CTRL
[31:1]:reserved
[0]: Used to power down analog blocks and reset all digital D-flip flops;It should be set during normal operation;
     After power on PLL (90%*vdd) or change FXIN/M/N settings or BP(1->0) occurs, an ective is needed. Otherwise, the output clocks could never be stable.
    0: release reset of smic pll
    1: reset smic pll
*/

#define REG_CKM_SMICPLL_CFG                          (CKM_BASE + 0x90)
#define REG_CKM_SMICPLL_CFG_smicpll_lkdt             ((((REG_CKM_SMICPLL_CFG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((28) << 2))
#define REG_CKM_SMICPLL_CFG_smicpll_bp               ((((REG_CKM_SMICPLL_CFG) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/* SMICPLL_CFG
[31:29]:reserved
[28]: Goes "1" when PLL is locked
[27:24]: Input 4-bit divider control pins, N[0] is LSB. PLL output frequency (including FOUTPOSTDIV & FOUTVCO) is calculated from the following equations:
         a. FOUTPOSTDIV = XIN * smicpll_m/(smicpll_n * (2^smicpll_od))
         b. FOUTVCO =  XIN * smicpll_m/(smicpll_n )
         < Attention > Please keep these conditions during usage.
         a.  XIN/N  >= 1MHz and XIN/N  <= 50MHz
         b. FOUTVCO>=200MHz  and FOUTVCO<= 400MHz
         c. M=4;N=1
[23:22]:reserved
[21:8]: Feedback 14-bit divider control pins, M[0] is LSB
[7:3]:reserved
[2:1]:Output divider control pin
[0]:Bypass the PLL; Active high
*/

#define REG_CKM_BOOTSEL_LOCK                          (CKM_BASE + 0x94)
#define REG_CKM_BOOTSEL_LOCK_rgf_bootsel_lock         ((((REG_CKM_BOOTSEL_LOCK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*BOOTSEL_LOCK
[31:1]:Reserved
[0]: lock signal of step 2 latch, configured by SW.
0: boot_sel is not locked.
1: boot_sel is locked. "
*/

#define REG_CKM_MCUBT_RST_MAGIC                          (CKM_BASE + 0x98)
/*MCUBT_RST_MAGIC
[31:0]: magic word to protect the reset operation of mcubt.
32'h9638_1234: cm3bt_sw_rst_n(bit[2]@SYS_CTRL) is valid.cm3bt_sw_rst_n bit can reset mcubt core.
Others: cm3bt_sw_rst_n is not valid. cm3bt_sw_rst_n bit is disabled by rgf_cm3bt_rst_magic.
*/

#define REG_CKM_MCUTP_RST_MAGIC                          (CKM_BASE + 0x9C)
/*MCUTP_RST_MAGIC
[31:0]: magic word to protect the reset operation of mcutp.
32'h9638_5678: cm3tp_sw_rst_n(bit[8]@SYS_CTRL) is valid.cm3tp_sw_rst_n bit can reset mcutp core.
Others: cm3tp_sw_rst_n is not valid. cm3tp_sw_rst_n bit is disabled by rgf_cm3tp_rst_magic.
*/


#define REG_CKM_CHICKEN                               (CKM_BASE + 0xA0)
#define REG_CKM_CHICKEN_chicken                       ((((REG_CKM_CHICKEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/* CHICKEN
[31:1]:reserved
[0]: indicate that all registers are back to va9638b version.
 0: all registers are in new version.
 1: all registers are backward to va9638b version.
*/

#define REG_SPI_CLKSEL                              (CKM_BASE + 0xA4)
#define REG_SPICLKSEL_switchen                      ((((REG_SPI_CLKSEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_SPICLKSEL_128msel                       ((((REG_SPI_CLKSEL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/* REG_SPI_CLKSEL
[31:2]:reserved
[0]: spi clock path select. This signal is valid only when rgf_spiclk_switch_en= 1.
0: select clk_fast path
1: select 128M/96M path.
When rgf_spi_128msel =1, the frequency of spi_clk is configed by rfg_fastclk_sel[1:0]，
rfg_fastclk_sel[1:0]=2'h1: 128MHz
rfg_fastclk_sel[1:0]=other: 96MHz
[1]: spi clock path switch enable signal.
0: spi lcock path switch is disable. rgf_spi_128msel is invalid.
1:spi lcock path switch is enable. rgf_spi_128msel is valid.


*/

#endif

/********************************************** 
 * I2S Registers
 **********************************************/
#if I2S_BASE

#define REG_I2S_I2S_CTRL_0    (I2S_BASE + 0x00)
#define REG_I2S_I2S_CTRL_0_I2S0_USE_BB_ADC_EN              ((((REG_I2S_I2S_CTRL_0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_I2S_I2S_CTRL_0_I2S0_USE_BB_DAC_EN              ((((REG_I2S_I2S_CTRL_0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_I2S_I2S_CTRL_0_I2S0_RX_EN                      ((((REG_I2S_I2S_CTRL_0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_I2S_I2S_CTRL_0_I2S0_TX_EN                      ((((REG_I2S_I2S_CTRL_0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S_CTRL_0:     
Configuration register of I2S channel #0
[31:18]: (reserved)    14'h0
reserved
[17]: use_bb_adc_en    1'b0
1: Use the bb_adc_en sigal to sync I2S #0 audio Rx.
0: Do not use bb_adc_en. I2S #0 Rx starts right after setting TX_EN  1. 
[16]: use_bb_dac_en    1'b0
1: Use the bb_dac_en sigal to sync I2S #0 audio Tx.
0: Do not use bb_dac_en. I2S #0 Tx starts right after setting TX_EN  1. 
[15:12]: SAMP_RATE    4'h4
Sampling rate of I2S #0.
0: 8 KHz
1: 12 KHz 
2: 16 KHz
3: 24 KHz
4: 48 KHz 
5: 96 KHz 
6: 192 KHz
7: 384 KHz
8: 11.025 KHz
9: 22.05 KHz
10: 44.1 KHz
11: 88.2 KHz
[11]: reserved    1'h0

[10]: WS_INV    1'h0
0: WS '0' indicates left channel, '1' indicates right channel.
1: WS '1' indicates left channel, '0' indicates right channel.
[9:8]: DWIDTH    2'h1
Data word width.
00: 16-bit.
01: 24-bit.
10: 32 bit
[7:6]: WS_SEL    2'h2
WS frequency selection for I2S #0.
00: SCLK=32*Fs.
01: SCLK=48*Fs. Note: NOT support this when I2S #0 as a master.
10: SCLK=64*Fs.
[5:4]: FORMAT    2'h0
0: I2S format, serial data is active from the second sclk.
1: reserved.
2: Left-Justified.
3: Right-Justified.
[3]: i2s0_clk_src_sel    1'h0
Select clock source for I2S channel #0:
1: source from 768MHz.
0: source form 384MHz.
[2]: MASTER_MODE    1'h1
i2s mode select    0:slave mode /1:master mode
[1]: I2S0_RX_EN    1'h0
the i2s channel #0 rx function enable    1: enable     0:disable 
[0]: I2S0_TX_EN    1'h0
the i2s channel #0 tx function enable    1: enable     0:disable 
*/

#define REG_I2S_I2S_CTRL_1    (I2S_BASE + 0x10)
#define REG_I2S_I2S_CTRL_1_I2S1_USE_BB_ADC_EN              ((((REG_I2S_I2S_CTRL_1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_I2S_I2S_CTRL_1_I2S1_USE_BB_DAC_EN              ((((REG_I2S_I2S_CTRL_1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_I2S_I2S_CTRL_1_I2S1_RX_EN                      ((((REG_I2S_I2S_CTRL_1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_I2S_I2S_CTRL_1_I2S1_TX_EN                      ((((REG_I2S_I2S_CTRL_1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S_CTRL_1:     
Configuration register of I2S channel #1
[31:18]: (reserved)    14'h0
reserved
[17]: use_bb_adc_en    1'b0
1: Use the bb_adc_en sigal to sync I2S #1 audio Rx.
0: Do not use bb_adc_en. I2S #1 Rx starts right after setting TX_EN  1. 
[16]: use_bb_dac_en    1'b0
1: Use the bb_dac_en sigal to sync I2S #1 audio Tx.
0: Do not use bb_dac_en. I2S #1 Tx starts right after setting TX_EN  1. 
[15:12]: SAMP_RATE    4'h4
Sampling rate of I2S #1.
0: 8 KHz
1: 12 KHz 
2: 16 KHz
3: 24 KHz
4: 48 KHz 
5: 96 KHz 
6: 192 KHz
7: 384 KHz
8: 11.025 KHz
9: 22.05 KHz
10: 44.1 KHz
11: 88.2 KHz
[11]: reserved    1'h0

[10]: WS_INV    1'h0
0: WS '0' indicates left channel, '1' indicates right channel.
1: WS '1' indicates left channel, '0' indicates right channel.
[9:8]: DWIDTH    2'h1
Data word width.
00: 16-bit.
01: 24-bit.
10: 32 bit
[7:6]: WS_SEL    2'h2
WS frequency selection for I2S #1.
00: SCLK=32*Fs.
01: SCLK=48*Fs. Note: NOT support this when I2S #1 as a master.
10: SCLK=64*Fs.
[5:4]: FORMAT    2'h0
0: I2S format, serial data is active from the second sclk.
1: reserved.
2: Left-Justified.
3: Right-Justified.
[3]: i2s1_clk_src_sel    1'h0
Select clock source for I2S channel #1:
1: source from 768MHz.
0: source form 384MHz.
[2]: MASTER_MODE    1'h1
i2s mode select    0:slave mode /1:master mode
[1]: I2S1_RX_EN    1'h0
the i2s channel #1 rx function enable    1: enable     0:disable 
[0]: I2S1_TX_EN    1'h0
the i2s channel #1 tx function enable    1: enable     0:disable 
*/

#define REG_I2S_SCDIV0_CFG    (I2S_BASE + 0x20)
/*
SCDIV0_CFG[31:20]: cnt0_step    12'd180
Counter step length of SCDIV0, which used for SRC #0.
[19:18]: reserved    2'h0

[17:0]: cnt0_ceiling    18'd90000
Counter ceiling of SCDIV0 . 
*/

#define REG_I2S_SCDIV1_CFG    (I2S_BASE + 0x24)
/*
SCDIV1_CFG[31:20]: cnt1_step    12'd180
Counter step length of SCDIV1, which used for SRC #1.
[19:18]: reserved    2'h0

[17:0]: cnt1_ceiling    18'd90000
Counter ceiling of SCDIV1. 
*/

#define REG_I2S_ASRC0_CTRL    (I2S_BASE + 0x50)
/*
ASRC0_CTRL:     
Configuration register of ASRC0
[31:7]: reserved    25'h0
reserved
[6:4]: aud_ch_sel    3'h0
Apply ASRC0 to which audio channel:
3'h0:  None
3'h1:  I2S_0 Tx
3'h2:  I2S_0 Rx
3'h3:  I2S_1 Tx
3'h4:  Audio DAC
[3]: reserved    1'h0

[2]: locked_bypass    1'h0
locked mode bypass                                                              1'b0 : modified locked mode                                                    1'b1 : normal locked mode                                                        
[1]: resample_disable    1'h1
audio resample disbale                                                          1'b0: audio resample enable
1'b1: audio resample disable
[0]: softrst    1'h0
soft reset for ASRC module , High active
*/

#define REG_I2S_ASRC0_LOCK_0    (I2S_BASE + 0x54)
/*
ASRC0_LOCK_0[31:24]: lock_gain_log    8'h00
Lock Gain Log of audio resampler 
[23:16]: lock_deadzone    8'h02
Lock dead zone of audio resampler 
[15:8]: unlock_thresh    8'h60
unlock threshold of audio resampler
[7:0]: lock_thresh    8'h20
Lock threshold of audio resampler
*/

#define REG_I2S_ASRC0_LOCK_1    (I2S_BASE + 0x58)
/*
ASRC0_LOCK_1[31:28]: reserved    4'h0
reserved
[27:24]: lock_slew    4'h1
Lock Slew of audio resampler  
[23:16]: lock_slew_divider    8'h00
Lock Slew divider of audio resampler 
[15:8]: vari_deadzone    8'h00
Variety dead zone of audio resampler 
[7:0]: vari_gain_log    8'h01
Variety Gain Log of audio resampler 
*/

#define REG_I2S_ASRC0_FIFO_SET_POINT    (I2S_BASE + 0x5C)
/*
ASRC0_FIFO_SET_POINT[31:7]: reserved    25'h0
reserved
[6:0]: fifo_set_point    7'h10
FIFO level of ring buffer in audio resampler
*/

#define REG_I2S_ASRC0_MAX_COUNT    (I2S_BASE + 0x60)
/*
ASRC0_MAX_COUNT[31:14]: reserved    18'h0
reserved
[13:0]: max_count    14'h03ff
input clocks to count before resetting the counter
*/

#define REG_I2S_ASRC0_CONV_FACTOR    (I2S_BASE + 0x64)
/*
ASRC0_CONV_FACTOR[31:4]: reserved    28'h0
reserved
[3:0]: conv_factor    4'h6
Asrc0 Conversion factors
*/

#define REG_I2S_ASRC0_MCLK_DIVIDER    (I2S_BASE + 0x68)
/*
ASRC0_MCLK_DIVIDER[31:6]: reserved    26'h0
reserved
[5:0]: mclk_divider    6'h00
divide factor for period_counts
*/

#define REG_I2S_ASRC0_SIG_COUNT    (I2S_BASE + 0x6C)
/*
ASRC0_SIG_COUNT[31:16]: reserved    16'h0

[15:0]: sig_count    16'h3A98
ASRC0 clock number for no audio signal detection 
*/

#define REG_I2S_ASRC0_RATIO_NUM    (I2S_BASE + 0x70)
/*
ASRC0_RATIO_NUM[31:4]: reserved    28'h0

[3:0]: calc_ratio_num    4'h3
ASRC0 caculation ratio number for modified locked mode
*/

#define REG_I2S_ASRC0_STATUS    (I2S_BASE + 0x74)
/*
ASRC0_STATUS[31:2]: reserved    30'h0
reserved
[1]: fifo_overflow    
This active-High output indicates that the level
of the input FIFO is beyond the safe operating
range. Therefore, loss of input samples is likely.
[0]: locked    
This active-High output indicates that ratio
tracking is in a locked state, i.e., the input FIFO
level is within the prescribed thresholds and
stable.
*/

#define REG_I2S_ASRC1_CTRL    (I2S_BASE + 0x78)
/*
ASRC1_CTRL:     
Configuration register of I2S SRC1
[31:7]: reserved    25'h0
reserved
[6:4]: aud_ch_sel    3'h0
Apply ASRC1 to which audio channel:
3'h0:  None
3'h1:  I2S_1 Tx
3'h2:  I2S_1 Rx
3'h3:  I2S_0 Tx
3'h4:  SPDIF
[3]: reserved    1'h0

[2]: locked_bypass    1'h0
locked mode bypass                                                              1'b0 : modified locked mode                                                    1'b1 : normal locked mode                                                        
[1]: resample_disable    1'h1
audio resample disbale                                                          1'b0: audio resample enable
1'b1: audio resample disable
[0]: softrst    1'h0
soft reset for ASRC1 module , High active
*/

#define REG_I2S_ASRC1_LOCK_0    (I2S_BASE + 0x7C)
/*
ASRC1_LOCK_0[31:24]: lock_gain_log    8'h00
Lock Gain Log of audio resampler 
[23:16]: lock_deadzone    8'h02
Lock dead zone of audio resampler 
[15:8]: unlock_thresh    8'h60
unlock threshold of audio resampler
[7:0]: lock_thresh    8'h20
Lock threshold of audio resampler
*/

#define REG_I2S_ASRC1_LOCK_1    (I2S_BASE + 0x80)
/*
ASRC1_LOCK_1[31:28]: reserved    4'h0
reserved
[27:24]: lock_slew    4'h1
Lock Slew of audio resampler  
[23:16]: lock_slew_divider    8'h00
Lock Slew divider of audio resampler 
[15:8]: vari_deadzone    8'h00
Variety dead zone of audio resampler 
[7:0]: vari_gain_log    8'h01
Variety Gain Log of audio resampler 
*/

#define REG_I2S_ASRC1_FIFO_SET_POINT    (I2S_BASE + 0x84)
/*
ASRC1_FIFO_SET_POINT[31:7]: reserved    25'h0
reserved
[6:0]: fifo_set_point    7'h10
FIFO level of ring buffer in audio resampler
*/

#define REG_I2S_ASRC1_MAX_COUNT    (I2S_BASE + 0x88)
/*
ASRC1_MAX_COUNT[31:14]: reserved    18'h0
reserved
[13:0]: max_count    14'h03ff
input clocks to count before resetting the counter
*/

#define REG_I2S_ASRC1_CONV_FACTOR    (I2S_BASE + 0x8C)
/*
ASRC1_CONV_FACTOR[31:4]: reserved    28'h0
reserved
[3:0]: conv_factor    4'h6
ASRC1 Conversion factors
*/

#define REG_I2S_ASRC1_MCLK_DIVIDER    (I2S_BASE + 0x90)
/*
ASRC1_MCLK_DIVIDER[31:6]: reserved    26'h0
reserved
[5:0]: mclk_divider    6'h00
divide factor for period_counts
*/

#define REG_I2S_ASRC1_SIG_COUNT    (I2S_BASE + 0x94)
/*
ASRC1_SIG_COUNT[31:16]: reserved    16'h0

[15:0]: sig_count    16'h3A98
ASRC1 clock number for no audio signal detection 
*/

#define REG_I2S_ASRC1_RATIO_NUM    (I2S_BASE + 0x98)
/*
ASRC1_RATIO_NUM[31:4]: reserved    28'h0

[3:0]: calc_ratio_num    4'h3
ASRC1 caculation ratio number for modified locked mode
*/

#define REG_I2S_ASRC1_STATUS    (I2S_BASE + 0x9C)
/*
ASRC1_STATUS[31:2]: reserved    30'h0
reserved
[1]: fifo_overflow    
This active-High output indicates that the level
of the input FIFO is beyond the safe operating
range. Therefore, loss of input samples is likely.
[0]: locked    
This active-High output indicates that ratio
tracking is in a locked state, i.e., the input FIFO
level is within the prescribed thresholds and
stable.
*/

#endif

/********************************************** 
 * UART Registers
 **********************************************/
#if UART_BASE

#define REG_UART_URDR                                      (UART_BASE + 0x0)
#define REG_UART_URDR_BI                                   ((((REG_UART_URDR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((12) << 2))
#define REG_UART_URDR_FE                                   ((((REG_UART_URDR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((11) << 2))
#define REG_UART_URDR_PE                                   ((((REG_UART_URDR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_UART_URDR_OE                                   ((((REG_UART_URDR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
/*
URDR: 
[31:13]: reserved
[12]: Break Interrupt
1 = Break condition occurs in this character. 
 0 = No break condition occurs in this character
[11]: Framing Error
1 = Framing error occurs in this character
0 = No framing error occurs in this character
[10]: Parity Error
1 = Parity error occurs in this character.
0 = No parity error occurs in this character
[9]: Overrun in receiver FIFO
1 = Overrun occurred. Note that this error is not specific to this character.
0 = No overrun occurs
[8]: reserved
[7:0]: received data
*/

#define REG_UART_UTDR                                      (UART_BASE + 0x4)
/*
UTDR: 
[31:8]: reserved
[7:0]: transmitter data
*/

#define REG_UART_UFCR                                      (UART_BASE + 0x8)
/*
UFCR: 
[31:29]: reserved
[28:24]: Define the Receiver FIFO trigger level for interrupt. From 1 to 31. This register can not be set to 0. Writing 0 to it will result in 1 being written in the register.
[23:21]: reserved
[20:16]: Define the transmitter FIFO trigger level for interrupt. For interrupt, this register can be 0 to 31.
[15:0]: reserved
*/

#define REG_UART_UCR                                       (UART_BASE + 0xc)
#define REG_UART_UCR_RPOL                                  ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_UART_UCR_TPOL                                  ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_UART_UCR_LB                                    ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_UART_UCR_RTSEN                                 ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_UART_UCR_CTSEN                                 ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_UART_UCR_SP                                    ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_UART_UCR_EPS                                   ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_UART_UCR_PE                                    ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UCR_STP2                                  ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UCR_BIT8                                  ((((REG_UART_UCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UCR: 
[31:21]: reserved
[20]: Receive Polarity—This bit controls the polarity of the received data.
0 = Normal polarity (1 = idle).
1 = Inverted polarity (0 = idle).
[19]: Transmit Polarity—This bit controls the polarity of the transmitted data.
0 = Normal polarity (1 = idle).
1 = Inverted polarity (0 = idle).
[18]: Loopback—This bit controls loopback for system testing purposes. When this bit is high, the receiver input is internally connected to the transmitter and ignores the RXD pin. The TXD pin is unaffected by this bit. 
0 = Normal receiver operation.
1 = Internally connects the transmitter output to the receiver input.
[17]: RTSEN.
1 = enable receiver hardware flow control. The receiver set output UART_rts_b to 1’b1 when it detects pending overrun in receiver FIFO
0 = disable receiver hardware flow control. If the receiver FIFO overrun, the received data is discarded and an interrupt is generated.
[16]: CTSEN.
1 = enable transmitter hardware flow control. The transmitter waits until UART_cts_b == 1’b0 before transmitting characters.
0 = disable transmitter hardware flow control. The transmitter sends characters whenever data is available in transmitter FIFO.
[15:5]: reserved
[4]: Stick Parity bit
0 = Stick Parity disabled
1 = If EPS and PEN are logic ‘1’, the parity bit is transmitted and checked as logic ‘0’. If bit PEN is ‘1’ and EPS is ‘0’ then the parity bit is transmitted and checked as ‘1’.

[3]: Even Parity select
 0 = Odd number of ‘1’ is transmitted and checked in each word (data and parity combined). In other words, if the data has an even number of ‘1’ in it, then the parity bit is ‘1’.
 1 = Even number of ‘1’ is transmitted in each word.
[2]: Parity Enable
0 = No parity
1 = Parity bit is generated on each outgoing character and is checked on each incoming one.
[1]: Enable 2 stop bits
0 = 1 stop bit
1 = 2 stop bits 
Note that the receiver always checks the first stop bit only. When baud rate is >3Mbps, this bit must be set to 1.
[0]: Enable 8-bit mode
0 = Each character contains 7 bits
1 = Each character contains 8 bits
*/

#define REG_UART_UBCR                                      (UART_BASE + 0x10)
/*
UBCR: 
[31:24]: reserved
[23:16]: Fractional baud rate divisor (DIV_frac)
This is the fractional part of the baud rate divisor
[15:0]: Integer baud rate divisor (DIV_integer)
This is the integer part of the baud rate divisor.
*/

#define REG_UART_UFRST                                     (UART_BASE + 0x14)
#define REG_UART_UFRST_TFRST                               ((((REG_UART_UFRST) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UFRST_RFRST                               ((((REG_UART_UFRST) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UFRST: 
[31:2]: reserved
[1]: Transmitter FIFO Reset Register Writing a ‘1’ to this bit clears the transmitter FIFO and resets its logic. The shift register is not cleared, i.e. transmitting of the current character continues.
[0]: Receiver FIFO Reset Register Writing a ‘1’ to this bit clears the Receiver FIFO and resets its logic. But it doesn’t clear the shift register, i.e. receiving of the current character continues.
*/

#define REG_UART_URCTS                                     (UART_BASE + 0x18)
#define REG_UART_URCTS_RTS                                 ((((REG_UART_URCTS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_URCTS_CTS                                 ((((REG_UART_URCTS) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
URCTS: 
[31:2]: reserved
[1]: When RTSEN == 1’b0, this bit controls the output port UART_rts_b. (RTSEN is in register UCR)
[0]: Store the status of the pin UART_cts_b.
*/

#define REG_UART_UTCR                                      (UART_BASE + 0x1c)
#define REG_UART_UTCR_BRK                                  ((((REG_UART_UTCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UTCR_FFE                                  ((((REG_UART_UTCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UTCR_FPE                                  ((((REG_UART_UTCR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UTCR: 
[31:3]: reserved
[2]: Break Control bit (BRK)
1 = the serial out is forced into logic ‘0’ (break state).
0 = break is disabled
[1]: Force Frame Error (FFE)— When this bit=1, change the stop bit’s polarity (1’b0) for each bit transmission. 
0 = Normal operation.
1 = Generate the FERR error.
[0]: Force Parity Error (FPE)—When this bit is high, it forces the transmitter to generate parity errors if parity is enabled. This bit is for system debugging.
0 = Generate normal parity.
1 = Generate inverted parity (error).
*/

#define REG_UART_UER                                       (UART_BASE + 0x20)
#define REG_UART_UER_UPSEL                                 ((((REG_UART_UER) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_UART_UER_UEN                                   ((((REG_UART_UER) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UER_TEN                                   ((((REG_UART_UER) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UER_REN                                   ((((REG_UART_UER) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UER: 
[31:4]: reserved
[3]: UART port select (TEN)
1 = auxiliary port (for test and production, no CTS/RTS).(@GPIO[14:13])
0 = full UART port (for communication under function mode).(@GPIO[4:1])
[2]: UART enable. (UEN)
1 = the UART logic is enabled., and the three configuration registers can not be written.
0 – UART is disabled. The three configuration registers can be modified.
[1]: Transmit enable (TEN)
1 = the transmit logic is enabled. UART enable must also be ‘1’ for transmit logic to work.
0 = the transmit logic is disabled.
[0]: Receive enable (REN)
1 = the receive logic is enabled. UART enable must also be ‘1’ for receive logic to work.
0 = the receive logic is disabled.
*/

#define REG_UART_UFLSR                                     (UART_BASE + 0x24)
#define REG_UART_UFLSR_UART_ERR                            ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_UART_UFLSR_UART_TXFIFO_INT                     ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_UART_UFLSR_UART_RXFIFO_INT                     ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_UART_UFLSR_CC                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_UART_UFLSR_BI                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_UART_UFLSR_FE                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_UART_UFLSR_PE                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_UART_UFLSR_OE                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_UART_UFLSR_RFTO                                ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_UART_UFLSR_TFTRG                               ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_UART_UFLSR_TFEPT                               ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_UART_UFLSR_TREPT                               ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UFLSR_RFTRG                               ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UFLSR_DR                                  ((((REG_UART_UFLSR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UFLSR: 
[31:24]: reserved
[23]: the status of interrupt uart_err 
1 = the interrupt of uart_err is valid.
0 = the interrupt of uart_err is invalid
[22]: the status of interrupt uart_txfifo_int 
1 = the uart_txfifo_int is valid.
0 = the uart_txfifo_int is invalid
[21]: the status of interrupt uar_rxfifo_int 
1 = the interrupt uart_rxfifo_int is valid.
0 = the interrupt uart_rxfifo_in is invalid
[20]: uart_cts_b changed 1 = uart_cts_b edge changed 0 = no uart_cts_b change, write to clear the uart_cts_int
[19]: Break Interrupt (BI) indicator source pending
 ‘1’ – This bit is set when a break condition has been reached in the current character. The break occurs when the line is held in logic 0 for a time of one character (start bit + data + parity + stop bit). In that case, one zero character enters the FIFO and the UART waits for a valid start bit to receive next character. The bit is cleared upon writing to this register (value is not important). Generates Receiver Line Status interrupt.
 ‘0’ – No break condition occurs after last reset of this register.
[18]: Framing Error (FE) indicator source pending
 ‘1’ – This bit is set when the received character at the top of the FIFO did not have a valid stop bit. Of course, generally, it might be that all the following data is corrupt. The bit is cleared upon writing to this register. Generates Receiver Line Status interrupt.
 ‘0’ – No framing error occurs after last reset of this register.
[17]: Parity Error (PE) indicator source pending
 ‘1’ – This bit is set when the character that is just received comes with parity error. The bit is cleared upon writing to this register. Generates Receiver Line Status interrupt.
 ‘0’ – No parity error occurs after last reset of this register.
[6]: Overrun Error (OE) indicator source pending
 ‘1’ – Indicate that the buffer has data and another character has been received in the receiver shift register. If another character is starting to arrive, it will overwrite the data in the shift register but the buffer will remain intact. The bit is cleared upon writing to this register. Generates Receiver Line Status interrupt.
 ‘0’ – No overrun occurs after last reset of this register.
[15:6]: reserved
[5]: Receiver FIFO timeout 1 = There’s at least 1 character in the FIFO but no character has been written to the FIFO or read from it for the last 4 Char times. This bit is cleared when the FIFO is read or written. 0 – No timeout occurs.
[4]: Transmit FIFO reach trigger level 1 = The number of characters is less than or equal to the transmit FIFO trigger level. Generate interrupt through the pin UART_txfifo_int_o. This bit returns to 0 when characters are written to the transmit FIFO and the number of characters in the FIFO is more than the trigger level. 0 = The number of character is more than the transmit FIFO trigger level.
[3]: Transmit FIFO is empty. 1 = The transmitter FIFO is empty. The bit is cleared when data is being written to the transmitter FIFO 0 = Otherwise
[2]: Transmitter Empty indicator 1 = Both the transmitter FIFO and transmitter shift register are empty. The bit is cleared when data is being been written to the transmitter FIFO 0 = Otherwise
[1]: Receiver FIFO reach trigger level 1 = The number of characters is more than or equal to the receiver FIFO trigger level. Generate interrupt through the pin UART_rxfifo_int_o. This bit returns to 0 when characters are read from the receiver FIFO and the number of characters in the FIFO is less than the trigger level. 0 = The number of character is less than the transmit FIFO trigger level.
[0]: Data Ready indicator. 1 = At least one character has been received and is in the FIFO 0 = No characters in the FIFO
*/

#define REG_UART_UFCT                                      (UART_BASE + 0x2c)
/*
UFCT: 
[31:22]: reserved
[21:16]: Transmit FIFO counter : number of characters in the transmit FiFO.
[15:6]: Reserved
[5:0]: Receive FIFO counter : number of characters in the receive FiFO.
*/

#define REG_UART_UGENTXINT                                 (UART_BASE + 0x30)
#define REG_UART_UGENTXINT_BI                              ((((REG_UART_UGENTXINT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UGENTXINT: 
[31:1]: reserved
[0]: UART TX interrupt generation. When UART transmitting is enabled, writing ‘1’ to this register will generate a tx interrupt.
*/

#define REG_UART_UERR_MSK                                  (UART_BASE + 0x34)
#define REG_UART_UERR_MSK_RXFIFO_INT_MASK                  ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_UART_UERR_MSK_TXFIFO_INT_MASK                  ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_UART_UERR_MSK_CC_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_UART_UERR_MSK_BI_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_UART_UERR_MSK_FE_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_UART_UERR_MSK_PE_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UERR_MSK_OE_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_UART_UERR_MSK_TO_MSK                           ((((REG_UART_UERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
UERR_MSK: 
[31:8]: reserved
[7]: UART RXFIFO_INT_MASK Changed Interrupt Mask
1 – masked
0 – no masked

[6]: UART TXFIFO_INT_MASKChanged Interrupt Mask
1 – masked
0 – no masked

[5]: UART CTS Changed Interrupt Mask
1 – masked
0 – no masked

[4]: UART RX Break Interrupt Mask
1 – masked
0 – no masked

[3]: UART RX Frame Error Mask
1 – masked
0 – no masked

[2]: UART RX Parity Error Mask
1 – masked
0 – no masked

[1]: UART RX Overrun Error Mask
1 – masked
0 – no masked

[0]: UART RX Timeout Interrupt Mask
1 – masked
0 – no masked

*/

#define REG_UART_UINTCLR                                   (UART_BASE + 0x38)
#define REG_UART_UINTCLR_RXFIFO_INT_CLR                    ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_UART_UINTCLR_TXFIFO_INT_CLR                    ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_UART_UINTCLR_CC_CLR                            ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_UART_UINTCLR_BI_CLR                            ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_UART_UINTCLR_FE_CLR                            ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_UART_UINTCLR_PE_CLR                            ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_UART_UINTCLR_OE_CLR                            ((((REG_UART_UINTCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
/*
UINTCLR: 
[31:8]: reserved
[7]: UART_RXFIFO_INT Changed Interrupt Clear, high active


[6]: UART_TXFIFO_INT Changed Interrupt Clear, high active


[5]: UART CTS Changed Interrupt Clear, high active


[4]: UART RX Break Interrupt Clear, high active


[3]: UART RX Frame Error Clear, high active


[2]: UART RX Parity Error Clear, high active


[1]: UART RX Overrun Error Clear, high active


[0]: reserved
*/

#define REG_UART_UNSAMPCLK                                 (UART_BASE + 0x3c)
/*
UNSAMPCLK: 
[31:8]: reserved
[7:4]: The middle value of the rclk_ini. rclk_mid = (rclk_ini +1)/2
[3:0]: rclk_ini = Nsample_clock – 1. rclk_ini should be odd number.

When the baud rate > 3Mbps is wanted, then Fuart_clk=48MHz , and we configure UBCR=1 and adjust the number of sample clock to meet this request. 
For example, if 12Mbps baud rate is wanted, the number of the sample clock is 4, then rclk_ini = 3. If 6Mbps baud rate is wanted, the number of the sample clock is 8, then rclk_ini = 7.

If the communication is between the computer and the chip, the highest baud rate is 3Mbps. If the communication is between the chip and chip, the highest baud rate can be > 3Mbps.

*/

/* UART Registers */
//#define UART_BASE						(0x40010000)
//#define REG_UART_URDR					(UART_BASE + 0x00)
//#define REG_UART_UTDR					(UART_BASE + 0x04)
//#define REG_UART_UFCR					(UART_BASE + 0x08)
//#define REG_UART_UCR					(UART_BASE + 0x0c)
#define REG_UART_UBCR					(UART_BASE + 0x10)
#define REG_UART_UFRST					(UART_BASE + 0x14)
#define REG_UART_URCTS					(UART_BASE + 0x18)
#define REG_UART_UTCR					(UART_BASE + 0x1c)
#define REG_UART_UER					(UART_BASE + 0x20)
#define REG_UART_UFLSR					(UART_BASE + 0x24)
#define REG_UART_UFCT					(UART_BASE + 0x2c)
#define REG_UART_UGENTXINT				(UART_BASE + 0x30)
#define REG_UART_UERRMSK				(UART_BASE + 0x34)
#define REG_UART_UINTCLR				(UART_BASE + 0x38)
//#define REG_UART_UNSAMPCLK				(UART_BASE + 0x3C)

#endif

/********************************************** 
 * GPIO Registers
 **********************************************/
#if GPIO_BASE

#define REG_GPIO_GDOUT                                     (GPIO_BASE + 0x0)
/*
GDOUT: 
[31:23]: reserved
[22:0]: when DOEN=0, DOUT=output data;
when DOEN=1, DIE = input enable :1:input enable; 0:input disable
*/

#define REG_GPIO_GDOEN                                     (GPIO_BASE + 0x4)
/*
GDOEN: 
[31:23]: reserved
[22:0]: data output enable for corresponding bit 0 = output enable 1 = output disable
*/

#define REG_GPIO_GDINVEN                                   (GPIO_BASE + 0x8)
/*
GDINVEN: 
[31:23]: reserved
[22:0]: data input inverted enble for corresponding bit 0 = no inverted 1 = inverted
*/

#define REG_GPIO_GDDRIVE                                   (GPIO_BASE + 0xc)
/*
GDDRIVE: 
[31:23]: reserved
[22:0]: when DOEN=0, DS=drive select;0 = low drive;1 = high drive;
when DOEN=1, PE=pull enable; 0 = pull down/up disable; 1 = pull down/up enable
*/

#define REG_GPIO_GDIN                                      (GPIO_BASE + 0x10)
#define REG_GPIO_GDIN_analog_tst                           ((((REG_GPIO_GDIN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
GDIN: 
[31]: 1: Chip in analog test mode
[30:23]: reserved
[22:0]: data input after inverted or not
*/

#define REG_GPIO_GINTMASK                                  (GPIO_BASE + 0x14)
/*
GINTMASK: 
[31:23]: reserved
[22:0]: GPIO[n] interrupts mask control 1: the corresponding GPIOn can’t trigger the interrupt 0: the corresponding GPIOn can trigger the interrupt
*/

//#define REG_GPIO_GINTMODE                                  (GPIO_BASE + 0x18)
/*
GINTMODE: 
[31:23]: 
[22:0]: GPIO[n] interrupt mode selection
1: level change triggers the interrupt
0: single edge triggers the interrupt: DINVEN=0: rising edge interrupt; DINVEN=1: falling edge interrupt
*/
#define REG_GPIO_GINTSTATUS                                (GPIO_BASE + 0x1c)
/*
GINTSTATUS: 
[31:23]: reserved
[22:0]: GPIO[n] interrupt status register 1: the corresponding GPIOn in interrupt state 0: the corresponding GPIOn not in interrupt state
*/

//#define REG_GPIO_GINTCLR                                    (GPIO_BASE + 0x20) //del by va9638b_gpio_v1.7

//#define REG_GPIO_GCLKEN                                    (GPIO_BASE + 0x24)
//#define REG_GPIO_GCLKEN_gpio_clk_en                        ((((REG_GPIO_GCLKEN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
GCLKEN: 
[31:1]: reserved
[0]: 1: gpio_clk is enabled 0: gpio_clk is gated
*/
#define REG_GPIO_UMCTRL                                    (GPIO_BASE + 0x34)
/*
UMCTRL: 
[31:28]: UMONITOR module selection 0: TOP 
1: CODEC 2: AIF 3: TMR 4: I2C 5: UART 6: CKM 7: MODEM 8: RFC 9: RFFE 10: BB (CEVA Baseband) 11: DMA
12: SYS_MUX
13: SPI
14: BBLE
[27:22]: Umonitor signals selection in the selected module
[21:0]: reserved
*/

#define REG_GPIO_UMEN                                      (GPIO_BASE + 0x30)
/*
UMEN: 
[31:23]: reserved
[22:1]: Umonitor signals output enable to GPIO[n] 1: monitor signals output enable 0: monitor signals output disable
[0]: reserved
*/


#define REG_GPIO_GDMUX1                                     (GPIO_BASE + 0x38)
/*
GDMUX: 
[31:23]: reserved
[22:19]: GPIO[n] data path MUX register 
1: data is connected with I2S port 1.
0: data is connected with GPIO module
[18:15]: GPIO[n] data path MUX register 
1: data is connected with I2S port 0.
0: data is connected with GPIO module
[14:13]: GPIO[n] data path MUX register 
1: data is connected with UART2 or LED 3-4 
0: data is connected with GPIO module
[12:9]: GPIO[n] data path MUX register 
1: data is connected with SPI module 0: data is connected with GPIO module
[8:7]: GPIO[n] data path MUX register 
1: data is connected with I2C module 0: data is connected with GPIO module
[6:5]: GPIO[n] data path MUX register 
1: data is connected with SW ports or LED 1-2 0: data is connected with GPIO module
[4:1]: GPIO[n] data path MUX register 
1: data is connected with UART module 
0: data is connected with GPIO module
[0]: reserved
*/

#define REG_GPIO_GDMUX2                                    (GPIO_BASE + 0x3C)
/*
GDMUX2: 
[31:7]: 
[6:5]: GPIO[n] data path MUX register. This bit is valid when GDMUX[n]==1'b1.
1: data is connected with LED2, LED1. 
0: data is connected with SW module
[4:0]: 
*/

//#define REG_GPIO_GDMUX1 				(GPIO_BASE + 0x38)
#define REG_GPIO_GLDOUT 				(GPIO_BASE + 0x40) // GPIO Output Data Register
#define REG_GPIO_GLDOEN 				(GPIO_BASE + 0x44) // GPIO Data Output Enable Register
#define REG_GPIO_GLDINVEN 				(GPIO_BASE + 0x48) // GPIO Input Data Inverted Enable Register
#define REG_GPIO_GLDDRIVE 				(GPIO_BASE + 0x4C) // GPIO PAD Output Drive Strength Register
#define REG_GPIO_GLDIN 					(GPIO_BASE + 0x50) // GPIO Input Data Register
#define REG_GPIO_GLINTMASK 				(GPIO_BASE + 0x54) // GPIO Interrupts Mask Control Register
//#define REG_GPIO_GLINTMODE 			(GPIO_BASE + 0x58) // GPIO Interrupts Mode Selection Register
#define REG_GPIO_GLINTSTATUS 			(GPIO_BASE + 0x5C) // GPIO Interrupts Status Register
//#define REG_GPIO_GLINTCLR 				(GPIO_BASE + 0x60) // GPIO Interrupts Clear Register
#define REG_GPIO_GLDMUX1 				(GPIO_BASE + 0x60)
#define REG_GPIO_GLDMUX2 				(GPIO_BASE + 0x64)
#define REG_DBG_PORT_TOP				(GPIO_BASE + 0x84)
#define REG_DBG_PORT_BT				  (GPIO_BASE + 0x88)
#define REG_DBG_PORT_DSP				(GPIO_BASE + 0x8C)


 
//-----------------GPIO_OUT--GPIO_L_OUT----bitband-------------------------
#define BB_GPIO_BASE											(GPIO_BASE + 0x0100)
#define BB_GP0_OUT				                (GPIO_BASE + 0x0100)  /* PIN 0 out bitband*/
#define BB_GP1_OUT				                (GPIO_BASE + 0x0104) 
#define BB_GP2_OUT				                (GPIO_BASE + 0x0108)  
#define BB_GP3_OUT				                (GPIO_BASE + 0x010C)  
#define BB_GP4_OUT				                (GPIO_BASE + 0x0110) 
#define BB_GP5_OUT				                (GPIO_BASE + 0x0114)  
#define BB_GP6_OUT				                (GPIO_BASE + 0x0118)
#define BB_GP7_OUT				                (GPIO_BASE + 0x011C) 
#define BB_GP8_OUT				                (GPIO_BASE + 0x0120)  
#define BB_GP9_OUT				                (GPIO_BASE + 0x0124)
#define BB_GP10_OUT				                (GPIO_BASE + 0x0128) 
#define BB_GP11_OUT				                (GPIO_BASE + 0x012C)  
#define BB_GP12_OUT				                (GPIO_BASE + 0x0130)
#define BB_GP13_OUT				                (GPIO_BASE + 0x0134) 
#define BB_GP14_OUT				                (GPIO_BASE + 0x0138)  
#define BB_GP15_OUT				                (GPIO_BASE + 0x013C)
#define BB_GP16_OUT				                (GPIO_BASE + 0x0140) 
#define BB_GP17_OUT				                (GPIO_BASE + 0x0144)  
#define BB_GP18_OUT				                (GPIO_BASE + 0x0148)
#define BB_GP19_OUT				                (GPIO_BASE + 0x014C) 
#define BB_GP20_OUT				                (GPIO_BASE + 0x0150)  
#define BB_GP21_OUT				                (GPIO_BASE + 0x0154)
#define BB_GP22_OUT				                (GPIO_BASE + 0x0158)

#define BB_GPIOL_BASE											(GPIO_BASE + 0x0200)
#define BB_GPL0_OUT				                (GPIO_BASE + 0x0200)
#define BB_GPL1_OUT				                (GPIO_BASE + 0x0204)
#define BB_GPL2_OUT				                (GPIO_BASE + 0x0208)  
#define BB_GPL3_OUT				                (GPIO_BASE + 0x020C)
#define BB_GPL4_OUT				                (GPIO_BASE + 0x0210)  
#define BB_GPL5_OUT				                (GPIO_BASE + 0x0214)
#define BB_GPL6_OUT				                (GPIO_BASE + 0x0218)  
#define BB_GPL7_OUT				                (GPIO_BASE + 0x021C)
#define BB_GPL8_OUT				                (GPIO_BASE + 0x0220)  
#define BB_GPL9_OUT				                (GPIO_BASE + 0x0224)
#define BB_GPL10_OUT				              (GPIO_BASE + 0x0228)  
#define BB_GPL11_OUT				              (GPIO_BASE + 0x022C)
#define BB_GPL12_OUT				              (GPIO_BASE + 0x0230)  
#define BB_GPL13_OUT				              (GPIO_BASE + 0x0234)
#define BB_GPL14_OUT				              (GPIO_BASE + 0x0238)  
#define BB_GPL15_OUT				              (GPIO_BASE + 0x023C)
#define BB_GPL16_OUT				              (GPIO_BASE + 0x0240)

#endif

/********************************************** 
 * AIF Registers
 **********************************************/
#if AIF_BASE

#define REG_AIF_DACFS                                      (AIF_BASE + 0x0)
/*
DACFS: 
[31:13]: reserved
[12:0]: See document VA9613iDA_Audio_CODEC_DigitalPart_DesignSpec.doc
*/

#define REG_AIF_AIF_CTRL                                   (AIF_BASE + 0x4)
#define REG_AIF_AIF_CTRL_FS_ADC_96K                        ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_AIF_AIF_CTRL_FS_DAC_96K                        ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((10) << 2))
#define REG_AIF_AIF_CTRL_DWIDTH_16B                        ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AIF_AIF_CTRL_AWIDTH_16B                        ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AIF_AIF_CTRL_DAC_PATH_SEL                      ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_AIF_CTRL_ADC_PATH_SEL                      ((((REG_AIF_AIF_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_CTRL: 
[31:11]: Reserved
[10]: 96kHz sampling frequency enable signal for DAC channel.
1: enable 96kHz;
0: close 96kHz.
[9:4]: Samplling frequency selection signal for DAC channel. 
This bit can't be written when AIF_EN = 1.Please configure this bit based on the document of VA9613iDA_Audio_CODEC_DigitalPart_DesignSpec.doc
[3]: Selection of Audio Codec Data Width from APB. This bit can't be written when DAC_EN = 1.
0 – 24bit, dac_fwdata is from APB Write Data [23:0];
1 – 16bit, dac_fwdata[23:8] is from APB Write Data [15:0], dac_fwdata[7:0] is all zero.
[2]: Selection of Audio Codec Data Width to APB. This bit can’t be written when ADC_EN = 1.
0 – 24bit;
1 – 16bit.
[1]: DAC data path selection bit. This bit can’t be written when DAC_EN = 1. 
0 – DAC data is from CEVA VCI 
1 – DAC data is from DMA
[0]: ADC data path selection bit. This bit can’t be written when ADC_EN = 1. 
0 – ADC data is output to CEVA VCI. 
1 – ADC data is output to DMA
*/

#define REG_AIF_AIF_CDC_EN                                 (AIF_BASE + 0x8)

#define REG_AIF_AIF_CDC_EN_bb_syn_dac_sel                       ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_AIF_AIF_CDC_EN_bb_syn_adc_sel                        ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))    


#define REG_AIF_AIF_CDC_EN_ADC_CHANC                       ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_AIF_AIF_CDC_EN_DAC_R_EN                        ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_AIF_AIF_CDC_EN_DAC_L_EN                        ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AIF_AIF_CDC_EN_ADC_R_EN                        ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_AIF_CDC_EN_ADC_L_EN                        ((((REG_AIF_AIF_CDC_EN) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_CDC_EN: 
[31:5]: Reserved
[4]: ADC data left and Right Channel data combination mode. When cdc_hpf_vao_l and cdc_hpf_vao_r not change at the same time, this configure bit is usefull.0 – the 32bit ADC data combined due to cdc_hpf_vao_l1 –the 32bit ADC data combined due to cdc_hpf_vao_r ??CODEC???,cdc_hpf_vao_l?cdc_hpf_vao_r?????,??????,??????????????????cdc_hpf_vao_l?cdc_hpf_vao_r??????????????,?cdc_hpf_vao_l???????32???,???????????(????????????,???????????????),?????????1,????????????
[3]: DAC right channel path works or not:
0 – not work
1 – work
[2]: DAC left channel path works or not:
0 – not work
1 – work
[1]: ADC right channel path works or not:
0 – not work
1 – work
[0]: ADC left channel path works or not:
0 – not work
1 – work
*/

#define REG_AIF_CDC_CTRL                                   (AIF_BASE + 0xc)
#define REG_AIF_CDC_CTRL_DECIM2_96K_EN                     ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((29) << 2))
#define REG_AIF_CDC_CTRL_I2CCLK_OUT_EN                     ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((27) << 2))
#define REG_AIF_CDC_CTRL_I2S_EN_DAC                        ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((26) << 2))
#define REG_AIF_CDC_CTRL_I2S_EN_ADC                        ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_AIF_CDC_CTRL_LOOPBACK_EN                       ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_AIF_CDC_CTRL_CLKADC_SEL                        ((((REG_AIF_CDC_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
/*
CDC_CTRL: 
[31:28]: Reserved
[27]: I2C clock enable signal for 1bit ADC, active high
[26]: Select data transfer mode between VA7621 and VA9628, for DAC channel.
0 – direct transfer data;
1 – I2S interface enable.
[25:24]: Select the quantization width of analog Sigma-Delta ADC:
2’b00 – 1bit ADC,
2’b01 – 2bit ADC,
2’b10 – 3bit ADC,
2’b11 – 4bit ADC,
[23]: Select data transfer mode between VA7621 and VA9628, for ADC channel.
0 – direct transfer data, for 1bit ADC;
1 – I2S interface enable, for 2bit~4bit ADC.
[22]: loopback enable
[21:20]: Decimation Fs selection.
[19:16]: Decimation gain selection.
[15:8]: DAC Digital Channel Filter Configuration
[7:4]: “Sigma-Delta Modulator L/R” block configuration signal
[3]: The frequency of clkadc configuration signal
[2:0]: ADC Digital Channel Filter Configuration
*/

#define REG_AIF_CDC_CTRL_2                                 (AIF_BASE + 0x10)
#define REG_AIF_CDC_CTRL_2_cdc_src_sel                     ((((REG_AIF_CDC_CTRL_2) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
CDC_CTRL_2: 
[31:15]: 
[14:4]: Audio codec SRC argument R1[10:0].
[3:1]: 
[0]: Audio codec SRC selection.
*/

#define REG_AIF_FERR                                       (AIF_BASE + 0x14)
#define REG_AIF_FERR_DAC_UN                                ((((REG_AIF_FERR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_FERR_ADC_OV                                ((((REG_AIF_FERR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FERR: 
[31:2]: reserved
[1]: DMA to DAC channel underflow. Active high.
[0]: ADC to DMA channel overflow. Active high.
*/

#define REG_AIF_FERR_MSK                                   (AIF_BASE + 0x18)
#define REG_AIF_FERR_MSK_DAC_UN_M                          ((((REG_AIF_FERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_FERR_MSK_ADC_OV_M                          ((((REG_AIF_FERR_MSK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FERR_MSK: 
[31:2]: reserved
[1]: Mask the error of DAC underflow. 
1 – masked
0 – unmasked
[0]: Mask the error of ADC overflow. 
1 – masked
0 – unmasked
*/

#define REG_AIF_FERRCLR                                    (AIF_BASE + 0x1c)
#define REG_AIF_FERRCLR_DAC_UN_CLR                         ((((REG_AIF_FERRCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_FERRCLR_ADC_OV_CLR                         ((((REG_AIF_FERRCLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
FERRCLR: 
[31:2]: reserved
[1]: DAC underflow error is set by hardware and cleared by software. Write this bit “1” to clear the error.
[0]: ADC overflow error is set by hardware and cleared by software. Write this bit “1” to clear the error.
*/


#define REG_AIF_PDM_CTRL                                 (AIF_BASE + 0x24)
#define REG_AIF_PDM_CTRL_PDM_CLKDIV2_EN                      ((((REG_AIF_PDM_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_AIF_PDM_CTRL_PDM_RCH_SEL                        ((((REG_AIF_PDM_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_AIF_PDM_CTRL_PDM_MIC_EN                        ((((REG_AIF_PDM_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
PDM_CTRL:     
[31:3]: (reserved)    29'h0
[2]: pdm_clkdiv2_en    1'h0
PDM mic clock div2 enable, active high
[1]: pdm_rch_sel    1'h0
Right channel source selection. 
1: right channel data is from the same IO of left channel. 
0: right channel data is from the different IO of left channel.
[0]: pdm_mic_en    1'h0
PDM mic enable, active high
*/
#endif

/********************************************** 
 * SD Registers
 **********************************************/
#if VSD_REG_BASE
#define VSD_BLK_ATTR      				(VSD_REG_BASE + 0x00)
#define VSD_CMD_ARG      				(VSD_REG_BASE + 0x04)
#define VSD_CMD_IDX      				(VSD_REG_BASE + 0x08)
#define VSD_CMD_CONTROL       	 		(VSD_REG_BASE + 0x0c)
#define VSD_HOST_MODE     				(VSD_REG_BASE + 0x10)
#define VSD_RESPONSE3       			(VSD_REG_BASE + 0x14)
#define VSD_RESPONSE2   				(VSD_REG_BASE + 0x18)
#define VSD_RESPONSE1      				(VSD_REG_BASE + 0x1c)
#define VSD_RESPONSE0      				(VSD_REG_BASE + 0x20)
#define VSD_PRESENT_STATE      			(VSD_REG_BASE + 0x24)
#define VSD_INT_STATUS      			(VSD_REG_BASE + 0x28)
#define VSD_INT_MASK      				(VSD_REG_BASE + 0x2C)
#define VSD_INT_SETMASK      			(VSD_REG_BASE + 0x30)
#define VSD_INT_UNMASK      			(VSD_REG_BASE + 0x34)
#define VSD_HOST_CONTROL      			(VSD_REG_BASE + 0x38)
#define VSD_CLK_CONTROL        			(VSD_REG_BASE + 0x3c)
#define VSD_DMA_CONTROL     			(VSD_REG_BASE + 0x40)
#define VSD_DMA_ADDR0       			(VSD_REG_BASE + 0x44)
#define VSD_DMA_LEN0   					(VSD_REG_BASE + 0x48)
#define VSD_SOFT_RESET      			(VSD_REG_BASE + 0x64)
#define VSD_AUTO_CMD_ARG      			(VSD_REG_BASE + 0x68)
#define VSD_AUTO_CMD_CONTROL      		(VSD_REG_BASE + 0x6C)
#define VSD_AUTO_RESPONSE      			(VSD_REG_BASE + 0x70)
#define VSD_RESP_TIMEOUT      			(VSD_REG_BASE + 0x74)
#define VSD_DATA_TIMEOUT      			(VSD_REG_BASE + 0x78)
#define VSD_WAKEUP_CONTROL      		(VSD_REG_BASE + 0x7C)
#define VSD_AUTO_CLKGATE_DISABLE 		(VSD_REG_BASE + 0x80)
#define VSD_TIMING_PARA      			(VSD_REG_BASE + 0x84)
#define VSD_DMA_STATUS      			(VSD_REG_BASE + 0x90)
#define VSD_CLK_MASK        			(VSD_REG_BASE + 0x94)
#define VSD_CLK_SEL        				(VSD_REG_BASE + 0x98)
#define VSD_DRIVER_STRENGTH        		(VSD_REG_BASE + 0x9C)
#endif

/********************************************** 
 * TIMER Registers
 **********************************************/
#if 0 && TIMER_BASE
/*
TCTRL: 
[31]: TMR_SCLK enable 0 = clock gated 1 = clock enabled
[30]: TMR_FCLK enable 0 = clock gated 1 = clock enabled
[29:14]: reserved
[13]: The mode selection when the counter is zero. This bit can be written when WDLOCK = 0 0 = interrupt 1 = system reset
[12]: Watch Dog count enable, can be written only when WDLOCK = 0 0 = count disable 1 = count enable
[11:10]: reserved
[9]: TMR_SCLK counter auto reload enable 0 = disable 1 = enable, when the counter is counted down to zero, the value in TSCLKCINI will be load to this counter, then countdown again
[8]: TMR_SCLK count enable 0 = count disable 1 = count enable
[7:6]: reserved
[5]: TMR_FCLK counter auto reload enable 0 = disable 1 = enable, when the counter is counted down to zero, the value in TFCLKSCINI will be load to this counter, then countdown again
[4]: TMR_FCLKS count enable 0 = count disable 1 = count enable
[3:2]: reserved
[1]: TMR_FCLK counter auto reload enable 0 = disable 1 = enable, when the counter is counted down to zero, the value in TFCLKLCINI will be load to this counter, then countdown again
[0]: TMR_FCLKL count enable 0 = count disable 1 = count enable
*/
#define REG_TIMER_TCTRL                                    (TIMER_BASE + 0x0)
#define REG_TIMER_TCTRL_SCLK_EN                            (31)
#define REG_TIMER_TCTRL_FCLK_EN                            (30)
#define REG_TIMER_TCTRL_WDMODE                             (13)
#define REG_TIMER_TCTRL_WDCEN                              (12)
#define REG_TIMER_TCTRL_TSCLK_RLD_EN                       (9)
#define REG_TIMER_TCTRL_TSCLK_CEN                          (8)
#define REG_TIMER_TCTRL_TFCLKS_RLD_EN                      (5)
#define REG_TIMER_TCTRL_TFCLKS_CEN                         (4)
#define REG_TIMER_TCTRL_TFCLKL_RLD_EN                      (1)
#define REG_TIMER_TCTRL_TFCLKL_CEN                         (0)
#define REG_TIMER_TCTRL_TFCLKH_RLD_EN                      (7)
#define REG_TIMER_TCTRL_TFCLKH_CEN                         (6)

/*
TFCLKLCINI: 
[31:0]: Initial/reload value for TMR_FCLKL counter
*/
#define REG_TIMER_TFCLKLCINI                               (TIMER_BASE + 0x4)

/*
TFCLKLCNT: 
[31:0]: TMR_FCLKL counter value
*/
#define REG_TIMER_TFCLKLCNT                                (TIMER_BASE + 0x8)

/*
TFCLKSCINI: 
[31:0]: Initial/reload value for TMRS_FCLK counter
*/
#define REG_TIMER_TFCLKSCINI                               (TIMER_BASE + 0x14)

/*
TFCLKSCNT: 
[31:0]: TMR_FCLKS counter value
*/
#define REG_TIMER_TFCLKSCNT                                (TIMER_BASE + 0x18)

/*
TSCLKCINI: 
[31:0]: Initial/reload value for TMR_SCLK counter
*/
#define REG_TIMER_TSCLKCINI                                (TIMER_BASE + 0x24)

/*
TFCLKSCNT:
[31:0]: TMR_FCLKS counter value
*/
#define REG_TIMER_TFCLKHCNT                                    (TIMER_BASE + 0x20)

/*
TFCLKSCINI:
[31:0]: Initial/reload value for TMRS_FCLK counter
*/
#define REG_TIMER_TFCLKHCINI                                                                (TIMER_BASE + 0x1C)

/*
TSCLKCNT: 
[31:0]: TMR_SCLK counter value
*/
#define REG_TIMER_TSCLKCNT                                 (TIMER_BASE + 0x28)

/*
TSTA: 
[31:4]: reserved
[3]: WD counter interrupt status, which is set by hardware and cleared by software 0 = no interrupt 1 = interrupt
[2]: TMR_SCLK counter interrupt status, which is set by hardware and cleared by software 0 = no interrupt 1 = interrupt
[1]: TMR_FCLKS counter interrupt status, which is set by hardware and cleared by software 0 = no interrupt 1 = interrupt
[0]: TMR_FCLKL counter interrupt status, which is set by hardware and cleared by software 0 = no interrupt 1 = interrupt
*/
#define REG_TIMER_TSTA                                     (TIMER_BASE + 0x30)
#define REG_TIMER_TSTA_WD_STA                              (3)
#define REG_TIMER_TSTA_S_STA                               (2)
#define REG_TIMER_TSTA_FS_STA                              (1)
#define REG_TIMER_TSTA_FL_STA                              (0)

/*
TINTCLR: 
[31:4]: reserved
[3]: Write 1 to clear WD interrupt status
[2]: Write 1 to clear TMR_SCLK interrupt status
[1]: Write 1 to clear TMR_FCLKS interrupt status
[0]: Write 1 to clear TMR_FCLKL interrupt status
*/
#define REG_TIMER_TINTCLR                                  (TIMER_BASE + 0x34)
#define REG_TIMER_TINTCLR_WD_CLR                           (3)
#define REG_TIMER_TINTCLR_S_CLR                            (2)
#define REG_TIMER_TINTCLR_FS_CLR                           (1)
#define REG_TIMER_TINTCLR_FL_CLR                           (0)
#define REG_TIMER_TSTA_FH_STA                                                             (4)

/*
TSTA2: reserved
[31:2]: reserved
[1]: TMR_SCLK reload enable status.
[0]: TMR_SCLK working status.
*/
#define REG_TIMER_TSTA2                                    (TIMER_BASE + 0x38)
#define REG_TIMER_TSTA2_tsclk_rld_en_syn_back              (1)
#define REG_TIMER_TSTA2_tsclk_cen_syn_back                 (0)

/*
TINTMASK: 
[31:4]: 
[3]: interrupt mask of watchdog
[2]: interrupt mask of timer_sclk
[1]: interrupt mask of timer_fclks
[0]: interrupt mask of timer_fclkl
*/
#define REG_TIMER_TINTMASK                                 (TIMER_BASE + 0x3c)
#define REG_TIMER_TINTMASK_int_wd_mask                     (3)
#define REG_TIMER_TINTMASK_int_tmr_sclk_mask               (2)
#define REG_TIMER_TINTMASK_int_tmr_fclk_s_mask             (1)
#define REG_TIMER_TINTMASK_int_tmr_fclk_l_mask             (0)
#define REG_TIMER_TINTMASK_int_tmr_fclk_h_mask                         (4)

/*
WDLOCK: 
[31:1]: reserved
[0]: The register configuration of Watch Dog Lock or not 0 = unlock 1 = lock
*/
#define REG_TIMER_WDLOCK                                   (TIMER_BASE + 0x40)
#define REG_TIMER_WDLOCK_WD_LOCK                           (0)

/*
WDCINI: 
[31:0]: Initial/reload value for Watch Dog counter
*/
#define REG_TIMER_WDCINI                                   (TIMER_BASE + 0x44)
/*
WDCNT: 
[31:0]: Watch Dog counter value
*/
#define REG_TIMER_WDCNT                                    (TIMER_BASE + 0x48)

/*
WDFEED: 
[31:0]: Watch Dog feed register
*/
#define REG_TIMER_WDFEED                                   (TIMER_BASE + 0x4C)

/*
LTEN: 
[31:8]: reserved
[7:6]: LED-timer 4 Enable
00: always output low
01: always output high
1x: output as figure 4.1 in Timer MAS.
[5:4]: LED-timer 3 Enable
00: always output low
01: always output high
1x: output as figure 4.1 in Timer MAS.
[3:2]: LED-timer 2 Enable
00: always output low
01: always output high
1x: output as figure 4.1 in Timer MAS.
[1:0]: LED-timer 1 Enable
00: always output low
01: always output high
1x: output as figure 4.1 in Timer MAS.
*/
#define REG_TIMER_LTEN                                     (TIMER_BASE + 0x50)

/*
LT1_CTRL1: 
[31:28]: on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]: off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]: on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT1_CTRL1                                (TIMER_BASE + 0x54)

/*
LT1_CTRL2: 
[31:28]: 
[27:24]: Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[23:22]: 
[21]: Output inverse level in state ON/OFF.
0: output 1 in ON(state A), output 0 in OFF(state B).
1: output 0 in ON(state A), output 1 in OFF(state B).
[20]: Output high/low during interval.
[19:0]: Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT1_CTRL2                                (TIMER_BASE + 0x58)
#define REG_TIMER_LT1_CTRL2_lt1_level_inv                  (21)
#define REG_TIMER_LT1_CTRL2_lt1_onoff_intvl                (20)

/*
LT2_CTRL1: 
[31:28]: on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]: off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]: on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT2_CTRL1                                (TIMER_BASE + 0x5c)

/*
LT2_CTRL2: 
[31:28]: 
[27:24]: Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[23:22]: 
[21]: Output inverse level in state ON/OFF.
0: output 1 in ON(state A), output 0 in OFF(state B).
1: output 0 in ON(state A), output 1 in OFF(state B).
[20]: Output high/low during interval.
[19:0]: Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT2_CTRL2                                (TIMER_BASE + 0x60)
#define REG_TIMER_LT2_CTRL2_lt2_level_inv                  (21)
#define REG_TIMER_LT2_CTRL2_lt2_onoff_intvl                (20)

/*
LT3_CTRL1: 
[31:28]: on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]: off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]: on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT3_CTRL1                                (TIMER_BASE + 0x64)

/*
LT3_CTRL2: 
[31:28]: 
[27:24]: Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[23:22]: 
[21]: Output inverse level in state ON/OFF.
0: output 1 in ON(state A), output 0 in OFF(state B).
1: output 0 in ON(state A), output 1 in OFF(state B).
[20]: Output high/low during interval.
[19:0]: Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT3_CTRL2                                (TIMER_BASE + 0x68)
#define REG_TIMER_LT3_CTRL2_lt3_level_inv                  (21)
#define REG_TIMER_LT3_CTRL2_lt3_onoff_intvl                (20)

/*
LT4_CTRL1: 
[31:28]: on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]: off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]: on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT4_CTRL1                                (TIMER_BASE + 0x6c)

/*
LT4_CTRL2: 
[31:28]: 
[27:24]: Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[23:22]: 
[21]: Output inverse level in state ON/OFF.
0: output 1 in ON(state A), output 0 in OFF(state B).
1: output 0 in ON(state A), output 1 in OFF(state B).
[20]: Output high/low during interval.
[19:0]: Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT4_CTRL2                                (TIMER_BASE + 0x70)
#define REG_TIMER_LT4_CTRL2_lt4_level_inv                  (21)
#define REG_TIMER_LT4_CTRL2_lt4_onoff_intvl                (20)

#endif

/********************************************** 
 * I2C Registers
 **********************************************/
#if I2C_BASE
/*
I2C_CTRL: 
[31:28]: reserved
[27]: EN, I2C core enable bit, high active.
[26:24]: reserved
[23:0]: SCL5F, the value to divide pclk. (SCL calculation refer to 4.4)
*/
#define REG_I2C_I2C_CTRL                                   (I2C_BASE + 0x0)
#define REG_I2C_I2C_CTRL_EN                                (27)

/*
I2C_CMD: 
[31:6]: reserved
[5]: STA, generate (repeated) start condition
[4]: STO, generate stop condition
[3]: WR, write to slave
[2]: RD, read from slave
[1]: ACK, when a receiver, sent ACK (low) or NACK (high)
[0]: reserved
*/
#define REG_I2C_I2C_CMD                                    (I2C_BASE + 0x4)
#define REG_I2C_I2C_CMD_STA                                (5)
#define REG_I2C_I2C_CMD_STO                                (4)
#define REG_I2C_I2C_CMD_WR                                 (3)
#define REG_I2C_I2C_CMD_RD                                 (2)
#define REG_I2C_I2C_CMD_ACK                                (1)

/*
I2C_BYTE_TX: 
[31:8]: reserved
[7:0]: Next word to transmit via I2C
*/
#define REG_I2C_I2C_BYTE_TX                                (I2C_BASE + 0x8)
#define REG_I2C_I2C_BYTE_TX_TX                            (0)
#define REG_I2C_I2C_BYTE_TX_TX_bit                        (8)
/*
I2C_BYTE_RX: 
[31:8]: reserved
[7:0]: Last word received via I2C.
*/
#define REG_I2C_I2C_BYTE_RX                                (I2C_BASE + 0xc)
#define REG_I2C_I2C_BYTE_RX_RX                             (0)
#define REG_I2C_I2C_BYTE_RX_RX_bit                        (8)
/*
I2C_STATUS: 
[31:7]: reserved
[6]: status of i2c_byte_idle 1: i2c_byte is idle 0: i2c_byte is not idle
[5]: status of interrupt of i2c_byte_done 1: interrupt i2c_byte_done is valid 0: interrupt i2c_byte_done is invalid
[4]: status of interrupt of i2c_arb_lost 1: interrupt i2c_arb_lost is valid 0: interrupt i2c_arb_lost is invalid
[3]: AL, Arbitration lost
This bit is set when the core lost arbitration. Arbitration is lost when: a STOP signal is detected, but non requested;or, the master drives SDA high, but SDA is low.
[2]: RxACK, Received acknowledge from slave, low active
[1]: Busy, I2C bus busy, high active
[0]: TIP, Transfer in progress, high active
*/
#define REG_I2C_I2C_STATUS                                 (I2C_BASE + 0x10)
#define REG_I2C_I2C_STATUS_I2C_BYTE_IDLE                   (8)
#define REG_I2C_I2C_STATUS_I2C_BYTE_DONE                   (7)
#define REG_I2C_I2C_STATUS_I2C_ARB_LOST                    (6)
#define REG_I2C_I2C_STATUS_I2C_CONTI_ERR                   (5)
#define REG_I2C_I2C_STATUS_I2C_CONTI_BUSY                  (4)
#define REG_I2C_I2C_STATUS_AL                              (3)
#define REG_I2C_I2C_STATUS_RxACK                           (2)
#define REG_I2C_I2C_STATUS_Busy                            (1)
#define REG_I2C_I2C_STATUS_TIP                             (0)

/*
I2C_INTCLR: 
[31:2]: reserved
[1]: i2c_arb_lost interrupt is set by hardware and cleared by software. Write this bit to clear the interrupt.
[0]: i2c_byte_done interrupt is set by hardware and cleared by software. Write this bit to clear the interrupt
*/
#define REG_I2C_I2C_INTCLR                                 (I2C_BASE + 0x14)
#define REG_I2C_I2C_INTCLR_ARB_LST_CLR                     (1)
#define REG_I2C_I2C_INTCLR_BYTE_DONE_CLR                   (0)

/*

I2C_SLV_SEL: 
[31:1]: reserved
[0]: i2c slave device selection 1: ANA is selected 0: PROM is selected
*/
#define REG_I2C_I2C_SLV_SEL                                (I2C_BASE + 0x20)
#define REG_I2C_I2C_SLV_SEL_I2C_SEL                        (0)

#define REG_I2C_I2C_CONTIRW_ADDR                             (I2C_BASE + 0x28)
#define REG_I2C_I2C_CONTIRW_CMD                              (I2C_BASE + 0x2c)
#define REG_I2C0_I2C_CONTIRW_CMD_CONTI_WRITE              (1)
#define REG_I2C0_I2C_CONTIRW_CMD_CONTI_WRITE_bit          (1)
#define REG_I2C0_I2C_CONTIRW_CMD_CONTI_READ               (0)
#define REG_I2C0_I2C_CONTIRW_CMD_CONTI_READ_bit           (1)

#define REG_I2C_I2C_EN_9900                                (I2C_BASE + 0x30)

#endif

/********************************************** 
 * DMA Registers
 **********************************************/
#if DMA_BASE

#define REG_DMA_TRF_CTRL                                   (DMA_BASE + 0x0)
/*
TRF_CTRL: 
[31:0]: This register is changed to be all bit reserved in this version.
*/

#define REG_DMA_INTR_MASK                                  (DMA_BASE + 0x4)
#define REG_DMA_INTR_MASK_i2s1_wfifo_under_msk             ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_DMA_INTR_MASK_i2s1_rfifo_over_msk              ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_DMA_INTR_MASK_aes_wfifo_under_msk              ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_DMA_INTR_MASK_aes_rfifo_over_msk               ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_DMA_INTR_MASK_Bb_wfifo_under_msk               ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_DMA_INTR_MASK_Bb_rfifo_over_msk                ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_DMA_INTR_MASK_Spi_wfifo_under_msk              ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_DMA_INTR_MASK_Spi_rfifo_over_msk               ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_DMA_INTR_MASK_Aif_wfifo_under_msk              ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_DMA_INTR_MASK_Aif_rfifo_over_msk               ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_DMA_INTR_MASK_i2s1_wr_done_msk                 ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_DMA_INTR_MASK_i2s1_rd_done_msk                 ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_DMA_INTR_MASK_aes_wr_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_DMA_INTR_MASK_aes_rd_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_DMA_INTR_MASK_Bb_wr_done_msk                   ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_INTR_MASK_Bb_rd_done_msk                   ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_INTR_MASK_Spi_wr_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_DMA_INTR_MASK_Spi_rd_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_INTR_MASK_Aif_wr_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_INTR_MASK_Aif_rd_done_msk                  ((((REG_DMA_INTR_MASK) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INTR_MASK: 
[31:26]: 
[25]: I2S_1 write path fifo underflow interrupt mask register.
1’h1: i2s1_wfifo_under is muted. 
1’h0: i2s1_wfifo_under is unmasked.

[24]: I2S_ read path fifo overflow interrupt mask register.
1’h1: i2s1_rfifo_over is muted. 
1’h0: i2s1_rfifo_over is unmasked.

[23]: AES write path fifo underflow interrupt mask register.
1’h1: aes_wfifo_under is muted. 
1’h0: aes_wfifo_under is unmasked.

[22]: AES read path fifo overflow interrupt mask register.
1’h1: aes_rfifo_over is muted. 
1’h0: aes_rfifo_over is unmasked.

[21]: Bb write path fifo underflow interrupt mask register. 1’h1: bb_wfifo_under is muted. 1’h0: bb_wfifo_under is unmasked.
[20]: Bb read path fifo overflow interrupt mask register. 1’h1: bb_rfifo_over is muted. 1’h0: bb_rfifo_over is unmasked.
[19]: Spi write path fifo underflow interrupt mask register. 1’h1: spi_wfifo_under is muted. 1’h0: spi_wfifo_under is unmasked.
[18]: Spi read path fifo overflow interrupt mask register. 1’h1: spi_rfifo_over is muted. 1’h0: spi_rfifo_over is unmasked.
[17]: Aif write path fifo underflow interrupt mask register. 1’h1: aif_wfifo_under is muted. 1’h0: aif_wfifo_under is unmasked.
[16]: Aif read path fifo overflow interrupt mask register. 1’h1: aif_rfifo_over is muted. 1’h0: aif_rfifo_over is unmasked.
[15:10]: 
[9]: I2S_1 write transfer done interrupt mask register.
1’h1: i2s1_wr_done_intr is muted. 
1’h0: i2s1_wr_done_intr is unmasked.
[8]: I2S_1 read transfer done interrupt mask register.
1’h1: i2s1_rd_done_intr is muted. 
1’h0: i2s1_rd_done_intr is unmasked.
[7]: AES write transfer done interrupt mask register.
1’h1: aes_wr_done_intr is muted. 
1’h0: aes_wr_done_intr is unmasked.
[6]: AES read transfer done interrupt mask register.
1’h1: aes_rd_done_intr is muted. 
1’h0: aes_rd_done_intr is unmasked.
[5]: Bb write transfer done interrupt mask register. 1’h1: bb_wr_done_intr is muted.1’h0: bb_wr_done_intr is unmasked.
[4]: Bb read transfer done interrupt mask register. 1’h1: bb_rd_done_intr is muted.1’h0: bb_rd_done_intr is unmasked.
[3]: SPI write transfer done interrupt mask register.1’h1: spi_wr_done_intr is muted. 1’h0: spi_wr_done_intr is unmasked.
[2]: SPI read transfer done interrupt mask register.1’h1: spi_rd_done_intr is muted. 1’h0: spi_rd_done_intr is unmasked.
[1]: Aif write transfer done interrupt mask register.1’h1: aif_wr_done_intr is muted. 1’h0: aif_wr_done_intr is unmasked.
[0]: Aif read transfer done interrupt mask register.1’h1: aif_rd_done_intr is muted. 1’h0: aif_rd_done_intr is unmasked.
*/

#define REG_DMA_INTR_CLR                                   (DMA_BASE + 0x8)
#define REG_DMA_INTR_CLR_i2s1_wfifo_under_clr              ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_DMA_INTR_CLR_i2s1_rfifo_over_clr               ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_DMA_INTR_CLR_aes_wfifo_under_clr               ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_DMA_INTR_CLR_aes_rfifo_over_clr                ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_DMA_INTR_CLR_Bb_wfifo_under_clr                ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_DMA_INTR_CLR_Bb_rfifo_over_clr                 ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_DMA_INTR_CLR_Spi_wfifo_under_clr               ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_DMA_INTR_CLR_Spi_rfifo_over_clr                ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_DMA_INTR_CLR_Aif_wfifo_under_clr               ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_DMA_INTR_CLR_Aif_rfifo_over_clr                ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_DMA_INTR_CLR_i2s1_wr_done_clr                  ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_DMA_INTR_CLR_i2s1_rd_done_clr                  ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_DMA_INTR_CLR_aes_wr_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_DMA_INTR_CLR_aes_rd_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_DMA_INTR_CLR_Bb_wr_done_clr                    ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_INTR_CLR_Bb_rd_done_clr                    ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_INTR_CLR_Spi_wr_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_DMA_INTR_CLR_Spi_rd_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_INTR_CLR_Aif_wr_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_INTR_CLR_Aif_rd_done_clr                   ((((REG_DMA_INTR_CLR) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INTR_CLR: 
[31:26]: reserved
[25]: i2s1 write path fifo underflow interrupt clear register.
When this bit is set to 1, i2s1_wfifo_under is cleared to 0. It will automatically return to 0 at next clock cycle.
[24]: i2s1 read path fifo overflow interrupt clear register.
When this bit is set to 1, i2s1_rfifo_over is cleared to 0. It will automatically return to 0 at next clock cycle.
[23]: AES write path fifo underflow interrupt clear register.
When this bit is set to 1, aes_wfifo_under is cleared to 0. It will automatically return to 0 at next clock cycle.
[22]: AES read path fifo overflow interrupt clear register.
When this bit is set to 1, aes_rfifo_over is cleared to 0. It will automatically return to 0 at next clock cycle.
[21]: Bb write path fifo underflow interrupt clear register.When this bit is set to 1, bb_wfifo_under is cleared to 0. It will automatically return to 0 at next clock cycle.
[20]: Bb read path fifo overflow interrupt clear register.When this bit is set to 1, bb_rfifo_over is cleared to 0. It will automatically return to 0 at next clock cycle.
[19]: Spi write path fifo underflow interrupt clear register.When this bit is set to 1, spi_wfifo_under is cleared to 0. It will automatically return to 0 at next clock cycle.
[18]: Spi read path fifo overflow interrupt clear register.When this bit is set to 1, spi_rfifo_over is cleared to 0. It will automatically return to 0 at next clock cycle.
[17]: Aif write path fifo underflow interrupt clear register.When this bit is set to 1, aif_wfifo_under is cleared to 0. It will automatically return to 0 at next clock cycle.
[16]: Aif read path fifo overflow interrupt clear register.When this bit is set to 1, aif_rfifo_over is cleared to 0. It will automatically return to 0 at next clock cycle.
[15:10]: 
[9]: i2s1 write transfer done interrupt clear register.
When this bit is set to 1, i2s1_wr_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[8]: i2s1 read transfer done interrupt clear register.
When this bit is set to 1, i2s1_rd_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[7]: AES write transfer done interrupt clear register.
When this bit is set to 1, aes_wr_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[6]: AES read transfer done interrupt clear register.
When this bit is set to 1, aes_rd_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[5]: Bb write transfer done interrupt clear register.When this bit is set to 1, bb_wr_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[4]: Bb read transfer done interrupt clear register.When this bit is set to 1, bb_rd_done_intr is cleared to 0. It will automatically return to 0 at next clock cycle.
[3]: SPI write transfer done interrupt clear register.When this bit is set to 1, spi_wr_done_in is cleared to 0. It will automatically return to 0 at next clock cycle.
[2]: SPI read transfer done interrupt clear register.When this bit is set to 1, spi_rd_done_in is cleared to 0. It will automatically return to 0 at next clock cycle.
[1]: Aif write transfer done interrupt clear register.When this bit is set to 1, aif_wr_done_in is cleared to 0. It will automatically return to 0 at next clock cycle.
[0]: Aif read transfer done interrupt clear register.When this bit is set to 1, aif_rd_done_in is cleared to 0. It will automatically return to 0 at next clock cycle.
*/

#define REG_DMA_INTR_STAT                                  (DMA_BASE + 0xc)
#define REG_DMA_INTR_STAT_i2s1_wfifo_under_intr            ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((25) << 2))
#define REG_DMA_INTR_STAT_i2s1_rfifo_over_intr             ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((24) << 2))
#define REG_DMA_INTR_STAT_aes_wfifo_under_intr             ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((23) << 2))
#define REG_DMA_INTR_STAT_aes_rfifo_over_intr              ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((22) << 2))
#define REG_DMA_INTR_STAT_Bb_wfifo_under_intr              ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((21) << 2))
#define REG_DMA_INTR_STAT_Bb_rfifo_over_intr               ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((20) << 2))
#define REG_DMA_INTR_STAT_Spi_wfifo_under_intr             ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((19) << 2))
#define REG_DMA_INTR_STAT_Spi_rfifo_over_intr              ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((18) << 2))
#define REG_DMA_INTR_STAT_Aif_wfifo_under_intr             ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((17) << 2))
#define REG_DMA_INTR_STAT_Aif_rfifo_over_intr              ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((16) << 2))
#define REG_DMA_INTR_STAT_i2s1_wr_done_intr                ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((9) << 2))
#define REG_DMA_INTR_STAT_i2s1_rd_done_intr                ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((8) << 2))
#define REG_DMA_INTR_STAT_aes_wr_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_DMA_INTR_STAT_aes_rd_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_DMA_INTR_STAT_Bb_wr_done_intr                  ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_INTR_STAT_Bb_rd_done_intr                  ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_INTR_STAT_Spi_wr_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((3) << 2))
#define REG_DMA_INTR_STAT_Spi_rd_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_INTR_STAT_Aif_wr_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_INTR_STAT_Aif_rd_done_intr                 ((((REG_DMA_INTR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
INTR_STAT: 
[31:26]: reserved
[25]: i2s1 write path fifo underflow interrupt register.
1’h1: i2s1_wfifo_under is occurred. 
1’h0: i2s1_wfifo_under is not occurred.
[24]: i2s1 read path fifo overflow interrupt register.
1’h1: i2s1_rfifo_over is occurred. 
1’h0: i2s1_rfifo_over is not occurred.
[23]: AES write path fifo underflow interrupt register.
1’h1: aes_wfifo_under is occurred. 
1’h0: aes_wfifo_under is not occurred.
[22]: AES read path fifo overflow interrupt register.
1’h1: aes_rfifo_over is occurred. 
1’h0: aes_rfifo_over is not occurred.
[21]: Bb write path fifo underflow interrupt register.1’h1: bb_wfifo_under is occurred. 1’h0: bb_wfifo_under is not occurred.
[20]: Bb read path fifo overflow interrupt register.1’h1: bb_rfifo_over is occurred. 1’h0: bb_rfifo_over is not occurred.
[19]: Spi write path fifo underflow interrupt register.1’h1: spi_wfifo_under is occurred. 1’h0: spi_wfifo_under is not occurred.
[18]: Spi read path fifo overflow interrupt register.1’h1: spi_rfifo_over is occurred. 1’h0: spi_rfifo_over is not occurred.
[17]: Aif write path fifo underflow interrupt register.1’h1: aif_wfifo_under is occurred. 1’h0: aif_wfifo_under is not occurred.
[16]: Aif read path fifo overflow interrupt register.1’h1: aif_rfifo_over is occurred. 1’h0: aif_rfifo_over is not occurred.
[15:10]: reserved
[9]: I2S_1 write transfer done interrupt status.
1’h1: i2s1_wr_done_intr is occured. 
1’h0: i2s1_wr_done_intr is not occured.
[8]: I2S_1 read transfer done interrupt status.
1’h1: i2s1_rd_done_intr is occured. 
1’h0: i2s1_rd_done_intr is not occured.
[7]: AES write transfer done interrupt status.
1’h1: aes_wr_done_intr is occured. 
1’h0: aes_wr_done_intr is not occured.
[6]: AES read transfer done interrupt status.
1’h1: aes_rd_done_intr is occured. 
1’h0: aes_rd_done_intr is not occured.
[5]: Bb write transfer done interrupt status. 
1’h1: bb_wr_done_intr is occured.
1’h0: bb_wr_done_intr is not occured.
[4]: Bb read transfer done interrupt status. 
1’h1: bb_rd_done_intr is occured.
1’h0: bb_rd_done_intr is not occured.
[3]: SPI write transfer done interrupt status.
1’h1: spi_wr_done_intr is occured. 
1’h0: spi_wr_done_intr is not occured.
[2]: SPI read transfer done interrupt status.
1’h1: spi_rd_done_intr is occured. 
1’h0: spi_rd_done_intr is not occured.
[1]: Aif write transfer done interrupt status.
1’h1: aif_wr_done_intr is occured. 
1’h0: aif_wr_done_intr is not occured.
[0]: Aif read transfer done interrupt status.
1’h1: aif_rd_done_intr is occured. 
1’h0: aif_rd_done_intr is not occured.
*/

#define REG_DMA_AIF_CNT                                    (DMA_BASE + 0x1010)
#define REG_DMA_AIF_CNT_aif_cnt_flag                       ((((REG_DMA_AIF_CNT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
AIF_CNT: 
[31]: The flag indicates when the rfifo is empty and wfifo is full for AIF device.1h: the rfifo is empty and wfifo is full for AIF device.0h: the rfifo is not empty or wfifo is not full for AIF device. If Only DAC path is enabled or only ADC path is enabled, aec_cnt_flag = 1’b0.
[30:16]: The counter of data has wrriten from SRAM in DAC path of AIF when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1’b1.IF both left channel and right channel of DAC are enabled, the counter of left channel is selected as aec_wfifo_cnt; otherwise, the counter of channel enabled is used as aec_wfifo_cnt.
[15:0]: The counter of data has read to SRAM in ADC path of AIF when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1’b1.IF both left channel and right channel of ADC are enabled, the counter of left channel is selected as aec_rfifo_cnt; otherwise, the counter of channel enabled is used as aec_rfifo_cnt.
*/

#define REG_DMA_AIF_RD_CTRL                                (DMA_BASE + 0x1014)
#define REG_DMA_AIF_RD_CTRL_aif_rd_dev_sel                 ((((REG_DMA_AIF_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_AIF_RD_CTRL_aif_rd_bitmod                  ((((REG_DMA_AIF_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AIF_RD_CTRL_Aif_rd_abort                   ((((REG_DMA_AIF_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AIF_RD_CTRL_Aif_rd_start                   ((((REG_DMA_AIF_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_RD_CTRL: 
[31:6]: reserved
[5]: Read data from device. 0: audio_codec, 1: I2S_0.
[4]: Bit mode selecting for Aif ADC path.0h: 24bits mode.1h: 16bits mode
[3:2]: Aif read channel enable.0h: disable 1h: only Left ADC channel enable2h: only Right ADC channel enable3h: Left and Right ADC channels enable
[1]: Abort the AIF read path. When this bit is set to 1, the AIF read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the AIF read path transfer.When this bit is set to 1, AIF read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_AIF_RD_STAT                                (DMA_BASE + 0x1018)
#define REG_DMA_AIF_RD_STAT_Aif_rd_stop                    ((((REG_DMA_AIF_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AIF_RD_STAT_Aif_rd_fe1                     ((((REG_DMA_AIF_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AIF_RD_STAT_Aif_rd_fe0                     ((((REG_DMA_AIF_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (AIF) stops or system aborts current DMA transfer. It is valid during aif_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current AIF read path interrupts (aif_rd_done or aif_rd_error interrupt). It is valid during aif_rd_done interrupt or aif_rd_error interrupt.0h: indicates the source of current AIF read path is buf0.1h: indicates the source of current AIF read path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_AIF_RD_RADD0                               (DMA_BASE + 0x101c)
/*
AIF_RD_RADD0: 
[31]: Reserved
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for right ADC channel of AIF read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_RD_RADD1                               (DMA_BASE + 0x1020)
/*
AIF_RD_RADD1: 
[31]: Reserved
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for right ADC channel of AIF read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_RD_LADD0                               (DMA_BASE + 0x1024)
#define REG_DMA_AIF_RD_LADD0_End_buf0                      ((((REG_DMA_AIF_RD_LADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
AIF_RD_LADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for left ADC channel of AIF read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_RD_LADD1                               (DMA_BASE + 0x1028)
#define REG_DMA_AIF_RD_LADD1_End_buf1                      ((((REG_DMA_AIF_RD_LADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
AIF_RD_LADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for left ADC channel of AIF read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_RD_LENGTH0                             (DMA_BASE + 0x102c)
/*
AIF_RD_LENGTH0: 
[31:0]: Read length for buf0 of ping-pang buffer for read AIF DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_AIF_RD_LENGTH1                             (DMA_BASE + 0x1030)
/*
AIF_RD_LENGTH1: 
[31:0]: Read length for buf1 of ping-pang buffer for read AIF DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_AIF_RD_CNT                                 (DMA_BASE + 0x1034)
/*
AIF_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of aif_rd_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of aif_rd_cnt is equal to Aif_rd_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to Aif_rd_length0 (or 1).
*/

#define REG_DMA_AIF_WR_CTRL                                (DMA_BASE + 0x103c)
#define REG_DMA_AIF_WR_CTRL_aif_wr_dev_sel                 ((((REG_DMA_AIF_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_AIF_WR_CTRL_Aif_wr_bitmod                  ((((REG_DMA_AIF_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AIF_WR_CTRL_Aif_wr_abort                   ((((REG_DMA_AIF_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AIF_WR_CTRL_Aif_wr_start                   ((((REG_DMA_AIF_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_WR_CTRL: 
[31:6]: reserved
[5]: Write data to device. 0: audio_codec, 1: I2S_0.
[4]: Bit mode selecting for Aif DAC path.0h: 24bits mode.1h: 16bits mode
[3:2]: Aif write channel enable.0h: disable 1h: only Left DAC channel enable2h: only Right DAC channel enable3h: Left and Right DAC channels enable
[1]: Abort the AIF write path. When this bit is set to 1, the AIF write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the AIF write path transfer.When this bit is set to 1, AIF write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_AIF_WR_STAT                                (DMA_BASE + 0x1040)
#define REG_DMA_AIF_WR_STAT_Aif_wr_stop                    ((((REG_DMA_AIF_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AIF_WR_STAT_Aif_wr_fe1                     ((((REG_DMA_AIF_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AIF_WR_STAT_Aif_wr_fe0                     ((((REG_DMA_AIF_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AIF_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (AIF) stops or system aborts current DMA transfer. It is valid during aif_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current AIF write path interrupts (aif_wr_done or aif_wr_error interrupt). It is valid during aif_wr_done interrupt or aif_wr_error interrupt.0h: indicates the source of current AIF write path is buf0.1h: indicates the source of current AIF write path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_AIF_WR_ADD0                                (DMA_BASE + 0x1044)
#define REG_DMA_AIF_WR_ADD0_End_buf0                       ((((REG_DMA_AIF_WR_ADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
AIF_WR_ADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for DAC channel of AIF write path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_WR_ADD1                                (DMA_BASE + 0x1048)
#define REG_DMA_AIF_WR_ADD1_End_buf1                       ((((REG_DMA_AIF_WR_ADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
AIF_WR_ADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for DAC channel of AIF read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AIF_WR_LENGTH0                             (DMA_BASE + 0x104c)
/*
AIF_WR_LENGTH0: 
[31:0]: write length for buf0 of ping-pang buffer for write AIF DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_AIF_WR_LENGTH1                             (DMA_BASE + 0x1050)
/*
AIF_WR_LENGTH1: 
[31:0]: write length for buf1 of ping-pang buffer for write AIF DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_AIF_WR_CNT                                 (DMA_BASE + 0x1054)
/*
AIF_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of aif_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of aif_wr_cnt is equal to Aif_wr_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to Aif_wr_length0 (or 1).
*/

#define REG_DMA_SPI_RD_CTRL                                (DMA_BASE + 0x10ac)
#define REG_DMA_SPI_RD_CTRL_spi_rd_en                      ((((REG_DMA_SPI_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_SPI_RD_CTRL_spi_rd_abort                   ((((REG_DMA_SPI_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI_RD_CTRL_spi_rd_start                   ((((REG_DMA_SPI_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI_RD_CTRL: 
[31:3]: reserved
[2]: Spi read channel enable.0h: SPI read channel disable 1h: SPI read channel enable
[1]: Abort the spi read path. When this bit is set to 1, the spi read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the spi read path transfer.When this bit is set to 1, spi read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_SPI_RD_STAT                                (DMA_BASE + 0x10B0)
#define REG_DMA_SPI_RD_STAT_spi_rd_stop                    ((((REG_DMA_SPI_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_SPI_RD_STAT_spi_rd_fe1                     ((((REG_DMA_SPI_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI_RD_STAT_spi_rd_fe0                     ((((REG_DMA_SPI_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (spi) stops or system aborts current DMA transfer. It is valid during spi_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current spi read path interrupts (spi_rd_done or spi_rd_error interrupt). It is valid during spi_rd_done interrupt or spi_rd_error interrupt.0h: indicates the source of current spi read path is buf0.1h: indicates the source of current spi read path is buf1.2h: reserved.3h: reserved.It is always 0h for SPI device.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for SPI device.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for SPI device.
*/

#define REG_DMA_SPI_RD_ADD                                 (DMA_BASE + 0x10B4)
/*
SPI_RD_ADD: 
[31]: Reserved
[30:0]: Start address in SRAM for single buffer for SPI read channel. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPI_RD_LENGTH                              (DMA_BASE + 0x10B8)
/*
SPI_RD_LENGTH: 
[31:0]: Read length for single buffer for read SPI DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_SPI_RD_CNT                                 (DMA_BASE + 0x10Bc)
/*
SPI_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of spi_rd_cnt is updated at end of each DMA transfer. It can be read in interrupt service process or afterwards. The value of spi_rd_cnt is equal to spi_rd_length when DMA is finished normally, otherwise, it is less than or equal to spi_rd_length.
*/

#define REG_DMA_SPI_WR_CTRL                                (DMA_BASE + 0x10C4)
#define REG_DMA_SPI_WR_CTRL_spi_wr_en                      ((((REG_DMA_SPI_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_SPI_WR_CTRL_spi_wr_abort                   ((((REG_DMA_SPI_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI_WR_CTRL_spi_wr_start                   ((((REG_DMA_SPI_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI_WR_CTRL: 
[31:3]: reserved
[2]: spi write channel enable.0h: SPI write channel disable.1h: SPI write channel enable.
[1]: Abort the spi write path. When this bit is set to 1, the spi write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the spi write path transfer.When this bit is set to 1, spi write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_SPI_WR_STAT                                (DMA_BASE + 0x10C8)
#define REG_DMA_SPI_WR_STAT_spi_wr_stop                    ((((REG_DMA_SPI_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_SPI_WR_STAT_spi_wr_fe1                     ((((REG_DMA_SPI_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI_WR_STAT_spi_wr_fe0                     ((((REG_DMA_SPI_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (spi) stops or system aborts current DMA transfer. It is valid during spi_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current spi write path interrupts (spi_wr_done or spi_wr_error interrupt). It is valid during spi_wr_done interrupt or spi_wr_error interrupt.0h: indicates the source of current spi write path is buf0.1h: indicates the source of current spi write path is buf1.2h: reserved.3h: reserved.It is always 0h for SPI device.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for SPI device.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for SPI device.
*/

#define REG_DMA_SPI_WR_ADD                                 (DMA_BASE + 0x10Cc)
/*
SPI_WR_ADD: 
[31]: 
[30:0]: Start address in SRAM for single buffer for SPI write channel.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPI_WR_LENGTH                              (DMA_BASE + 0x10d0)
/*
SPI_WR_LENGTH: 
[31:0]: Write length for single buffer for write SPI DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_SPI_WR_CNT                                 (DMA_BASE + 0x10d4)
/*
SPI_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of spi_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of spi_wr_cnt is equal to spi_wr_length when DMA is finished normally, otherwise, it is less than or equal to spi_wr_length.
*/

#define REG_DMA_SPI1_RD_CTRL                                (DMA_BASE + 0x117c)
#define REG_DMA_SPI1_RD_CTRL_spi_rd_en                      ((((REG_DMA_SPI1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_SPI1_RD_CTRL_spi_rd_abort                   ((((REG_DMA_SPI1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI1_RD_CTRL_spi_rd_start                   ((((REG_DMA_SPI1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI1_RD_CTRL: 
[31:3]: reserved
[2]: Spi read channel enable.0h: SPI1 read channel disable 1h: SPI1 read channel enable
[1]: Abort the spi read path. When this bit is set to 1, the spi read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the spi read path transfer.When this bit is set to 1, spi read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_SPI1_RD_STAT                                (DMA_BASE + 0x1180)
#define REG_DMA_SPI1_RD_STAT_spi_rd_stop                    ((((REG_DMA_SPI1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_SPI1_RD_STAT_spi_rd_fe1                     ((((REG_DMA_SPI1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI1_RD_STAT_spi_rd_fe0                     ((((REG_DMA_SPI1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI1_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (spi) stops or system aborts current DMA transfer. It is valid during spi_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current spi read path interrupts (spi_rd_done or spi_rd_error interrupt). It is valid during spi_rd_done interrupt or spi_rd_error interrupt.0h: indicates the source of current spi read path is buf0.1h: indicates the source of current spi read path is buf1.2h: reserved.3h: reserved.It is always 0h for SPI1 device.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for SPI1 device.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for SPI1 device.
*/

#define REG_DMA_SPI1_RD_ADD                                 (DMA_BASE + 0x1184)
/*
SPI1_RD_ADD: 
[31]: Reserved
[30:0]: Start address in SRAM for single buffer for SPI1 read channel. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPI1_RD_LENGTH                              (DMA_BASE + 0x1188)
/*
SPI1_RD_LENGTH: 
[31:0]: Read length for single buffer for read SPI1 DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_SPI1_RD_CNT                                 (DMA_BASE + 0x118c)
/*
SPI1_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of spi_rd_cnt is updated at end of each DMA transfer. It can be read in interrupt service process or afterwards. The value of spi_rd_cnt is equal to spi_rd_length when DMA is finished normally, otherwise, it is less than or equal to spi_rd_length.
*/

#define REG_DMA_SPI1_WR_CTRL                                (DMA_BASE + 0x1194)
#define REG_DMA_SPI1_WR_CTRL_spi_wr_en                      ((((REG_DMA_SPI1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_SPI1_WR_CTRL_spi_wr_abort                   ((((REG_DMA_SPI1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI1_WR_CTRL_spi_wr_start                   ((((REG_DMA_SPI1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI1_WR_CTRL: 
[31:3]: reserved
[2]: spi write channel enable.0h: SPI1 write channel disable.1h: SPI1 write channel enable.
[1]: Abort the spi write path. When this bit is set to 1, the spi write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the spi write path transfer.When this bit is set to 1, spi write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_SPI1_WR_STAT                                (DMA_BASE + 0x1198)
#define REG_DMA_SPI1_WR_STAT_spi_wr_stop                    ((((REG_DMA_SPI1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_SPI1_WR_STAT_spi_wr_fe1                     ((((REG_DMA_SPI1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_SPI1_WR_STAT_spi_wr_fe0                     ((((REG_DMA_SPI1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
SPI1_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (spi) stops or system aborts current DMA transfer. It is valid during spi_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current spi write path interrupts (spi_wr_done or spi_wr_error interrupt). It is valid during spi_wr_done interrupt or spi_wr_error interrupt.0h: indicates the source of current spi write path is buf0.1h: indicates the source of current spi write path is buf1.2h: reserved.3h: reserved.It is always 0h for SPI1 device.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for SPI1 device.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for SPI1 device.
*/

#define REG_DMA_SPI1_WR_ADD                                 (DMA_BASE + 0x119c)
/*
SPI1_WR_ADD: 
[31]: 
[30:0]: Start address in SRAM for single buffer for SPI1 write channel.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPI1_WR_LENGTH                              (DMA_BASE + 0x11a0)
/*
SPI1_WR_LENGTH: 
[31:0]: Write length for single buffer for write SPI1 DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_SPI1_WR_CNT                                 (DMA_BASE + 0x11a4)
/*
SPI1_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of spi_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of spi_wr_cnt is equal to spi_wr_length when DMA is finished normally, otherwise, it is less than or equal to spi_wr_length.
*/

#define REG_DMA_BB_RD_CTRL                                 (DMA_BASE + 0x8c)
#define REG_DMA_BB_RD_CTRL_bb_rd_en                        ((((REG_DMA_BB_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_BB_RD_CTRL_bb_rd_abort                     ((((REG_DMA_BB_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_BB_RD_CTRL_bb_rd_start                     ((((REG_DMA_BB_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_RD_CTRL: 
[31:3]: reserved
[2]: bb read channel enable.0h: bb read channel disable 1h: bb read channel enable
[1]: Abort the bb read path. When this bit is set to 1, the bb read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the bb read path transfer.When this bit is set to 1, bb read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_BB_RD_STAT                                 (DMA_BASE + 0x90)
#define REG_DMA_BB_RD_STAT_bb_rd_stop                      ((((REG_DMA_BB_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_BB_RD_STAT_bb_rd_fe1                       ((((REG_DMA_BB_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_BB_RD_STAT_bb_rd_fe0                       ((((REG_DMA_BB_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (bb) stops or system aborts current DMA transfer. It is valid during bb_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current bb read path interrupts (bb_rd_done or bb_rd_error interrupt). It is valid during bb_rd_done interrupt or bb_rd_error interrupt.0h: indicates the source of current bb read path is buf0.1h: indicates the source of current bb read path is buf1.2h: reserved.3h: reserved.It is always 0h for bb device.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for bb device.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for bb device.
*/

#define REG_DMA_BB_RD_ADD                                  (DMA_BASE + 0x94)
/*
BB_RD_ADD: 
[31]: Reserved
[30:0]: Start address in SRAM for single buffer for bb read channel. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_BB_RD_LENGTH                               (DMA_BASE + 0x98)
/*
BB_RD_LENGTH: 
[31:0]: Read length for single buffer for read bb DMA transfer. The unit of read length is byte.Recommend: the read length is fixed 1088bytes (272X32 bits) for BB read channel.
*/

#define REG_DMA_BB_RD_CNT                                  (DMA_BASE + 0x9c)
/*
BB_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of bb_rd_cnt is updated at end of each DMA transfer. It can be read in interrupt service process or afterwards. The value of bb_rd_cnt is equal to bb_rd_length when DMA is finished normally, otherwise, it is less than or equal to bb_rd_length.
*/

#define REG_DMA_BB_CRRT_RD_CNT                             (DMA_BASE + 0xa0)
/*
BB_CRRT_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in current time.
*/

#define REG_DMA_BB_WR_CTRL                                 (DMA_BASE + 0xa4)
#define REG_DMA_BB_WR_CTRL_bb_wr_en                        ((((REG_DMA_BB_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_BB_WR_CTRL_bb_wr_abort                     ((((REG_DMA_BB_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_BB_WR_CTRL_bb_wr_start                     ((((REG_DMA_BB_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_WR_CTRL: 
[31:3]: reserved
[2]: bb write channel enable.0h: bb write channel disable.1h: bb write channel enable.
[1]: Abort the bb write path. When this bit is set to 1, the bb write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the bb write path transfer.When this bit is set to 1, bb write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_BB_WR_STAT                                 (DMA_BASE + 0xa8)
#define REG_DMA_BB_WR_STAT_bb_wr_stop                      ((((REG_DMA_BB_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_BB_WR_STAT_bb_wr_fe1                       ((((REG_DMA_BB_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_BB_WR_STAT_bb_wr_fe0                       ((((REG_DMA_BB_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
BB_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (bb) stops or system aborts current DMA transfer. It is valid during bb_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current bb write path interrupts (bb_wr_done or bb_wr_error interrupt). It is valid during bb_wr_done interrupt or bb_wr_error interrupt.0h: indicates the source of current bb write path is buf0.1h: indicates the source of current bb write path is buf1.2h: reserved.3h: reserved.It is always 0h for bb device.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for bb device.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for bb device.
*/

#define REG_DMA_BB_WR_ADD                                  (DMA_BASE + 0xac)
/*
BB_WR_ADD: 
[31]: 
[30:0]: Start address in SRAM for single buffer for bb write channel.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_BB_WR_LENGTH                               (DMA_BASE + 0xb0)
/*
BB_WR_LENGTH: 
[31:0]: Write length for single buffer for write bb DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_BB_WR_CNT                                  (DMA_BASE + 0xb4)
/*
BB_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of bb_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of bb_wr_cnt is equal to bb_wr_length when DMA is finished normally, otherwise, it is less than or equal to bb_wr_length.
*/

#define REG_DMA_AES_RD_CTRL                                (DMA_BASE + 0x240)
#define REG_DMA_AES_RD_CTRL_aes_rd_en                      ((((REG_DMA_AES_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_AES_RD_CTRL_aes_rd_abort                   ((((REG_DMA_AES_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AES_RD_CTRL_aes_rd_start                   ((((REG_DMA_AES_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AES_RD_CTRL: 
[31:3]: reserved
[2]: bb read channel enable.0h: bb read channel disable 1h: bb read channel enable
[1]: Abort the bb read path. When this bit is set to 1, the bb read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the bb read path transfer.When this bit is set to 1, bb read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_AES_RD_STAT                                (DMA_BASE + 0x244)
#define REG_DMA_AES_RD_STAT_aes_rd_stop                    ((((REG_DMA_AES_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AES_RD_STAT_aes_rd_fe1                     ((((REG_DMA_AES_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AES_RD_STAT_aes_rd_fe0                     ((((REG_DMA_AES_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AES_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (bb) stops or system aborts current DMA transfer. It is valid during aes_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current bb read path interrupts (aes_rd_done or aes_rd_error interrupt). It is valid during aes_rd_done interrupt or aes_rd_error interrupt.0h: indicates the source of current bb read path is buf0.1h: indicates the source of current bb read path is buf1.2h: reserved.3h: reserved.It is always 0h for bb device.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for bb device.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for bb device.
*/

#define REG_DMA_AES_RD_ADD                                 (DMA_BASE + 0x248)
/*
AES_RD_ADD: 
[31]: Reserved
[30:0]: Start address in SRAM for single buffer for bb read channel. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AES_RD_LENGTH                              (DMA_BASE + 0x24c)
/*
AES_RD_LENGTH: 
[31:0]: Read length for single buffer for read bb DMA transfer. The unit of read length is byte.Recommend: the read length is fixed 1088bytes (272X32 bits) for BB read channel.
*/

#define REG_DMA_AES_RD_CNT                                 (DMA_BASE + 0x250)
/*
AES_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of aes_rd_cnt is updated at end of each DMA transfer. It can be read in interrupt service process or afterwards. The value of aes_rd_cnt is equal to aes_rd_length when DMA is finished normally, otherwise, it is less than or equal to aes_rd_length.
*/

#define REG_DMA_AES_CRRT_RD_CNT                            (DMA_BASE + 0x254)
/*
AES_CRRT_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in current time.
*/

#define REG_DMA_AES_WR_CTRL                                (DMA_BASE + 0x258)
#define REG_DMA_AES_WR_CTRL_aes_wr_en                      ((((REG_DMA_AES_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((2) << 2))
#define REG_DMA_AES_WR_CTRL_aes_wr_abort                   ((((REG_DMA_AES_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AES_WR_CTRL_aes_wr_start                   ((((REG_DMA_AES_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AES_WR_CTRL: 
[31:3]: reserved
[2]: bb write channel enable.0h: bb write channel disable.1h: bb write channel enable.
[1]: Abort the bb write path. When this bit is set to 1, the bb write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the bb write path transfer.When this bit is set to 1, bb write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_AES_WR_STAT                                (DMA_BASE + 0x25C)
#define REG_DMA_AES_WR_STAT_aes_wr_stop                    ((((REG_DMA_AES_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_AES_WR_STAT_aes_wr_fe1                     ((((REG_DMA_AES_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_AES_WR_STAT_aes_wr_fe0                     ((((REG_DMA_AES_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
AES_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (bb) stops or system aborts current DMA transfer. It is valid during aes_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current bb write path interrupts (aes_wr_done or aes_wr_error interrupt). It is valid during aes_wr_done interrupt or aes_wr_error interrupt.0h: indicates the source of current bb write path is buf0.1h: indicates the source of current bb write path is buf1.2h: reserved.3h: reserved.It is always 0h for bb device.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.It is always 0h for bb device.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.It is always 1h for bb device.
*/

#define REG_DMA_AES_WR_ADD                                 (DMA_BASE + 0x260)
/*
AES_WR_ADD: 
[31]: 
[30:0]: Start address in SRAM for single buffer for bb write channel.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_AES_WR_LENGTH                              (DMA_BASE + 0x264)
/*
AES_WR_LENGTH: 
[31:0]: Write length for single buffer for write bb DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_AES_WR_CNT                                 (DMA_BASE + 0x268)
/*
AES_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of aes_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of aes_wr_cnt is equal to aes_wr_length when DMA is finished normally, otherwise, it is less than or equal to aes_wr_length.
*/


#define REG_DMA_I2S0_CNT    (DMA_BASE + 0x1130)
#define REG_DMA_I2S0_CNT_i2s0_cnt_flag                     ((((REG_DMA_I2S0_CNT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
i2s0_cnt  :     i2s01 counter Register
  [31]:  i2s0_cnt_flag  1'h0 The flag indicates when the rfifo is empty and wfifo is full for i2s0 device.1h: the rfifo is empty and wfifo is full for i2s0 device.0h: the rfifo is not empty or wfifo is not full for i2s0 device. If Only DAC path is enabled or only ADC path is enabled
  [30:16]:  i2s0_wfifo_cnt  15'h0 The counter of data has wrriten from SRAM in DAC path of i2s0 when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1 b1.IF both left channel and right channel of DAC are enabled
  [15:0]:  i2s0_rfifo_cnt  16'h0 The counter of data has read to SRAM in ADC path of i2s0 when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1 b1.IF both left channel and right channel of ADC are enabled
*/

#define REG_DMA_I2S0_RD_CTRL    (DMA_BASE + 0x1134)
#define REG_DMA_I2S0_RD_CTRL_i2s0_rd_bitmod                ((((REG_DMA_I2S0_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S0_RD_CTRL_i2s0_rd_abort                 ((((REG_DMA_I2S0_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S0_RD_CTRL_i2s0_rd_start                 ((((REG_DMA_I2S0_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
i2s0_rd_ctrl  :     i2s0 read channel control register
  [31:8]:  (reserved)  24'h0 reserved
  [7]:  i2s0r_syncgain_en  1'h0 read data with sync gain mode enalbe.1:data with gain mode 0:data without gain mode
note: when i2s0r_syncgain_en=1 and i2s0_rd_bitmod=0
  [6]:  i2s0r_surc_dev_sel  1'h0 reserved
  [5]:  i2s0r_dest_mem_sel  1'h0 reserved. This bit is ingoned. The memory is CM3 SRAM
  [4]:  i2s0_rd_bitmod  1'h1 Bit mode selecting for i2s0 ADC path.0h: 24bits mode.1h: 16bits mode
NOTE:when i2s0r_syncgain_en=1&i2s0r_dest_mem_sel=1
  [3:2]:  i2s0_rd_en  2'h0 i2s0 read channel enable.0h: disable 1h: only Left ADC channel enable2h: only Right ADC channel enable3h: Left and Right ADC channels enable
  [1]:  i2s0_rd_abort  1'h0 Abort the i2s0 read path. When this bit is set to 1
  [0]:  i2s0_rd_start  1'h0 Start the i2s0 read path transfer.When this bit is set to 1
*/

#define REG_DMA_I2S0_RD_STAT    (DMA_BASE + 0x1138)
#define REG_DMA_I2S0_RD_STAT_i2s0_rd_stop                  ((((REG_DMA_I2S0_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S0_RD_STAT_i2s0_rd_fe1                   ((((REG_DMA_I2S0_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S0_RD_STAT_i2s0_rd_fe0                   ((((REG_DMA_I2S0_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
i2s0_rd_stat  :     i2s0 read channel state register
  [31:5]:  (reserved)  27'h0 reserved
  [4]:  i2s0_rd_stop  1'h0 Denotes if device (i2s0) stops or system aborts current DMA transfer. It is valid during i2s0_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA  is stopped by device or is aborted by system
  [3:2]:  i2s0_rdintr_src  2'h0 Source of current i2s0 read path interrupts (i2s0_rd_done or i2s0_rd_error interrupt). It is valid during i2s0_rd_done interrupt or i2s0_rd_error interrupt.0h: indicates the source of current i2s0 read path is buf0.1h: indicates the source of current i2s0 read path is buf1.2h: reserved.3h: reserved.
  [1]:  i2s0_rd_fe1  1'h0 Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.
  [0]:  i2s0_rd_fe0  1'h0 Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_I2S0_RD_RADD0    (DMA_BASE + 0x113C)
/*
i2s0_rd_radd0  :     Right ADC channel start address0 register
  [31]:  (reserved)  1'h0 Reserved
  [30:0]:  i2s0_rd_radd0  31'h0 Start address in SRAM or X/Y MEM for buf0 of ping-pang buffer for right ADC channel of i2s0 read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_RD_RADD1    (DMA_BASE + 0x1140)
/*
i2s0_rd_radd1  :     Right ADC channel start address1 register
  [31]:  (reserved)  1'h0 Reserved
  [30:0]:  i2s0_rd_radd1  31'h0 Start address in SRAM or X/Y MEM for buf1 of ping-pang buffer for right ADC channel of i2s0 read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_RD_LADD0    (DMA_BASE + 0x1144)
#define REG_DMA_I2S0_RD_LADD0_End_buf0                     ((((REG_DMA_I2S0_RD_LADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
i2s0_rd_ladd0  :     Left ADC channel start address0 in register
  [31]:  End_buf0  1'h0 indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
  [30:0]:  i2s0_rd_ladd0  31'h0 Start address in SRAM or X/Y MEM for buf0 of ping-pang buffer for left ADC channel of i2s0 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_RD_LADD1    (DMA_BASE + 0x1148)
#define REG_DMA_I2S0_RD_LADD1_End_buf1                     ((((REG_DMA_I2S0_RD_LADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
i2s0_rd_ladd1  :     i2s0 & CODEC Work Enable Register
  [31]:  End_buf1  1'h0 Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
  [30:0]:  i2s0_rd_ladd1  31'h0 Start address in SRAM or X/Y MEM for buf1 of ping-pang buffer for left ADC channel of i2s0 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_RD_LENGTH0    (DMA_BASE + 0x114C)
/*
i2s0_rd_length0  :     i2s0 read length0 register
  [31:0]:  i2s0_rd_length0  32'h0 Read length for buf0 of ping-pang buffer for read i2s0 DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_I2S0_RD_LENGTH1    (DMA_BASE + 0x1150)
/*
i2s0_rd_length1  :     i2s0 read length1 register
  [31:0]:  i2s0_rd_length1  32'h0 Read length for buf1 of ping-pang buffer for read i2s0 DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_I2S0_RD_CNT    (DMA_BASE + 0x1154)
/*
i2s0_rd_cnt  :     i2s0 read counter register
  [31:0]:  i2s0_rd_cnt  32'h0 Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of i2s0_rd_cnt is updated at end of each DMA transfer
*/

#define REG_DMA_I2S0_WR_CTRL    (DMA_BASE + 0x115c)
#define REG_DMA_I2S0_WR_CTRL_i2s0_wr_bitmod                ((((REG_DMA_I2S0_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S0_WR_CTRL_i2s0_wr_abort                 ((((REG_DMA_I2S0_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S0_WR_CTRL_i2s0_wr_start                 ((((REG_DMA_I2S0_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
i2s0_wr_ctrl  :     i2s0 write channel control register
  [31:8]:  (reserved)  24'h0 reserved
  [7]:  i2s0w_cntr_surc_sel  1'h0 reserved
  [6]:  i2s0w_surc_mem_sel  1'h0 reserved. This bit is ingoned. The memory is CM3 SRAM
  [5]:  i2s0w_dest_dev_sel  1'h0 reserved
  [4]:  i2s0_wr_bitmod  1'h1 Bit mode selecting for i2s0 DAC path.0h: 24bits  or 32bits mode.1h: 16bits mode
  [3:2]:  i2s0_wr_en  2'h0 i2s0 write channel enable.0h: disable 1h: only Left DAC channel enable2h: only Right DAC channel enable3h: Left and Right DAC channels enable
  [1]:  i2s0_wr_abort  1'h0 Abort the i2s0 write path. When this bit is set to 1
  [0]:  i2s0_wr_start  1'h0 Start the i2s0 write path transfer.When this bit is set to 1
*/

#define REG_DMA_I2S0_WR_STAT    (DMA_BASE + 0x1160)
#define REG_DMA_I2S0_WR_STAT_i2s0_wr_stop                  ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S0_WR_STAT_i2s0_wr_fe1                   ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S0_WR_STAT_i2s0_wr_fe0                   ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
i2s0_wr_stat  :     Right ADC channel start address0 register
  [31:5]:  (reserved)  27'h0 reserved
  [4]:  i2s0_wr_stop  1'h0 Denotes if device (i2s0) stops or system aborts current DMA transfer. It is valid during i2s0_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA  is stopped by device or is aborted by system
  [3:2]:  i2s0_wrintr_src  2'h0 Source of current i2s0 write path interrupts (i2s0_wr_done or i2s0_wr_error interrupt). It is valid during i2s0_wr_done interrupt or i2s0_wr_error interrupt.0h: indicates the source of current i2s0 write path is buf0.1h: indicates the source of current i2s0 write path is buf1.2h: reserved.3h: reserved.
  [1]:  i2s0_wr_fe1  1'h0 Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.
  [0]:  i2s0_wr_fe0  1'h0 Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_I2S0_WR_ADD0    (DMA_BASE + 0x1164)
#define REG_DMA_I2S0_WR_ADD0_End_buf0                      ((((REG_DMA_I2S0_WR_ADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
i2s0_wr_add0  :     i2s0 write channel start address0 register
  [31]:  End_buf0  1'h0 indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
  [30:0]:  i2s0_wr_add0  31'h0 Start address in SRAM for buf0 of ping-pang buffer for DAC channel of i2s0 write path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_WR_ADD1    (DMA_BASE + 0x1168)
#define REG_DMA_I2S0_WR_ADD1_End_buf1                      ((((REG_DMA_I2S0_WR_ADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
i2s0_wr_add1  :     i2s0 write channel start address1 register
  [31]:  End_buf1  1'h0 Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
  [30:0]:  i2s0_wr_add1  31'h0 Start address in SRAM for buf1 of ping-pang buffer for DAC channel of i2s0 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S0_WR_LENGTH0    (DMA_BASE + 0x116C)
/*
i2s0_wr_length0  :     i2s0 write length0 register
  [31:0]:  i2s0_wr_length0  32'h0 write length for buf0 of ping-pang buffer for write i2s0 DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_I2S0_WR_LENGTH1    (DMA_BASE + 0x1170)
/*
i2s0_wr_length1  :     i2s0 write length1 register
  [31:0]:  i2s0_wr_length1  32'h0 write length for buf1 of ping-pang buffer for write i2s0 DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_I2S0_WR_CNT    (DMA_BASE + 0x1174)
/*
i2s0_wr_cnt  :     i2s0 write Counter Register
  [31:0]:  i2s0_wr_cnt  32'h0 Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of i2s0_wr_cnt is updated at end of each DMA transfer
  :     
*/


#define REG_DMA_I2S1_CNT                                   (DMA_BASE + 0x1060)
#define REG_DMA_I2S1_CNT_i2s1_cnt_flag                     ((((REG_DMA_I2S1_CNT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
I2S1_CNT: 
[31]: The flag indicates when the rfifo is empty and wfifo is full for i2s_1 device.1h: the rfifo is empty and wfifo is full for i2s_1 device.0h: the rfifo is not empty or wfifo is not full for i2s_1 device. If Only TX path is enabled or only RX path is enabled, aec_cnt_flag = 1’b0.
[30:16]: The counter of data has wrriten from SRAM in TX path of i2s_1 when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1’b1.IF both left channel and right channel of TX are enabled, the counter of left channel is selected as aec_wfifo_cnt; otherwise, the counter of channel enabled is used as aec_wfifo_cnt.
[15:0]: The counter of data has read to SRAM in RX path of i2s_1 when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1’b1.IF both left channel and right channel of RX are enabled, the counter of left channel is selected as aec_rfifo_cnt; otherwise, the counter of channel enabled is used as aec_rfifo_cnt.
*/

#define REG_DMA_I2S1_RD_CTRL                               (DMA_BASE + 0x1064)
#define REG_DMA_I2S1_RD_CTRL_i2s1_rd_bitmod                ((((REG_DMA_I2S1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S1_RD_CTRL_i2s1_rd_abort                 ((((REG_DMA_I2S1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S1_RD_CTRL_i2s1_rd_start                 ((((REG_DMA_I2S1_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S1_RD_CTRL: 
[31:5]: reserved
[4]: Bit mode selecting for i2s_1 RX path.0h: 24bits mode.1h: 16bits mode
[3:2]: i2s_1 read channel enable.0h: disable 1h: only Left RX channel enable2h: only Right RX channel enable3h: Left and Right RX channels enable
[1]: Abort the i2s_1 read path. When this bit is set to 1, the i2s_1 read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the i2s_1 read path transfer.When this bit is set to 1, i2s_1 read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_I2S1_RD_STAT                               (DMA_BASE + 0x1068)
#define REG_DMA_I2S1_RD_STAT_i2s1_rd_stop                  ((((REG_DMA_I2S1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S1_RD_STAT_i2s1_rd_fe1                   ((((REG_DMA_I2S1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S1_RD_STAT_i2s1_rd_fe0                   ((((REG_DMA_I2S1_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S1_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (i2s_1) stops or system aborts current DMA transfer. It is valid during i2s_1_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current i2s_1 read path interrupts (i2s_1_rd_done or i2s_1_rd_error interrupt). It is valid during i2s_1_rd_done interrupt or i2s_1_rd_error interrupt.0h: indicates the source of current i2s_1 read path is buf0.1h: indicates the source of current i2s_1 read path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_I2S1_RD_RADD0                              (DMA_BASE + 0x106C)
/*
I2S1_RD_RADD0: 
[31]: Reserved
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for right RX channel of i2s_1 read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_RD_RADD1                              (DMA_BASE + 0x1070)
/*
I2S1_RD_RADD1: 
[31]: Reserved
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for right RX channel of i2s_1 read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_RD_LADD0                              (DMA_BASE + 0x1074)
#define REG_DMA_I2S1_RD_LADD0_End_buf0                     ((((REG_DMA_I2S1_RD_LADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
I2S1_RD_LADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for left RX channel of i2s_1 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_RD_LADD1                              (DMA_BASE + 0x1078)
#define REG_DMA_I2S1_RD_LADD1_End_buf1                     ((((REG_DMA_I2S1_RD_LADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
I2S1_RD_LADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for left RX channel of i2s_1 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_RD_LENGTH0                            (DMA_BASE + 0x107C)
/*
I2S1_RD_LENGTH0: 
[31:0]: Read length for buf0 of ping-pang buffer for read i2s_1 DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_I2S1_RD_LENGTH1                            (DMA_BASE + 0x1080)
/*
I2S1_RD_LENGTH1: 
[31:0]: Read length for buf1 of ping-pang buffer for read i2s_1 DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_I2S1_RD_CNT                                (DMA_BASE + 0x1084)
/*
I2S1_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of i2s_1_rd_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of i2s_1_rd_cnt is equal to i2s_1_rd_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to i2s_1_rd_length0 (or 1).
*/

#define REG_DMA_I2S1_WR_CTRL                               (DMA_BASE + 0x108C)
#define REG_DMA_I2S1_WR_CTRL_i2s1_wr_bitmod                ((((REG_DMA_I2S1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S1_WR_CTRL_i2s1_wr_abort                 ((((REG_DMA_I2S1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S1_WR_CTRL_i2s1_wr_start                 ((((REG_DMA_I2S1_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S1_WR_CTRL: 
[31:5]: reserved
[4]: Bit mode selecting for i2s_1 TX path.0h: 24bits mode.1h: 16bits mode
[3:2]: i2s_1 write channel enable.0h: disable 1h: only Left TX channel enable2h: only Right TX channel enable3h: Left and Right TX channels enable
[1]: Abort the i2s_1 write path. When this bit is set to 1, the i2s_1 write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the i2s_1 write path transfer.When this bit is set to 1, i2s_1 write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_I2S1_WR_STAT                               (DMA_BASE + 0x1090)
#define REG_DMA_I2S1_WR_STAT_i2s1_wr_stop                  ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_I2S1_WR_STAT_i2s1_wr_fe1                   ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_I2S1_WR_STAT_i2s1_wr_fe0                   ((((REG_DMA_I2S1_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
I2S1_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (i2s_1) stops or system aborts current DMA transfer. It is valid during i2s_1_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current i2s_1 write path interrupts (i2s_1_wr_done or i2s_1_wr_error interrupt). It is valid during i2s_1_wr_done interrupt or i2s_1_wr_error interrupt.0h: indicates the source of current i2s_1 write path is buf0.1h: indicates the source of current i2s_1 write path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_I2S1_WR_ADD0                               (DMA_BASE + 0x1094)
#define REG_DMA_I2S1_WR_ADD0_End_buf0                      ((((REG_DMA_I2S1_WR_ADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
I2S1_WR_ADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for TX channel of i2s_1 write path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_WR_ADD1                               (DMA_BASE + 0x1098)
#define REG_DMA_I2S1_WR_ADD1_End_buf1                      ((((REG_DMA_I2S1_WR_ADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
I2S1_WR_ADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for TX channel of i2s_1 read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_I2S1_WR_LENGTH0                            (DMA_BASE + 0x109C)
/*
I2S1_WR_LENGTH0: 
[31:0]: write length for buf0 of ping-pang buffer for write i2s_1 DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_I2S1_WR_LENGTH1                            (DMA_BASE + 0x10A0)
/*
I2S1_WR_LENGTH1: 
[31:0]: write length for buf1 of ping-pang buffer for write i2s_1 DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_I2S1_WR_CNT                                (DMA_BASE + 0x10A4)
/*
I2S1_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of i2s_1_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of i2s_1_wr_cnt is equal to i2s_1_wr_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to i2s_1_wr_length0 (or 1).
*/

//##+ str DMAC HDS REG BEGIN
#define REG_DMA_HDS_CNT                                   (DMA_BASE + 0x10e0)
#define REG_DMA_HDS_CNT_hds_cnt_flag                      ((((REG_DMA_HDS_CNT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
HDS_CNT: 
[31]: The flag indicates when the rfifo is empty and wfifo is full for hds device.1h: the rfifo is empty and wfifo is full for hds device.0h: the rfifo is not empty or wfifo is not full for hds device. If Only DAC path is enabled or only ADC path is enabled, aec_cnt_flag = 1?0.
[30:16]: The counter of data has wrriten from SRAM in DAC path of hds when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1?1.IF both left channel and right channel of DAC are enabled, the counter of left channel is selected as aec_wfifo_cnt; otherwise, the counter of channel enabled is used as aec_wfifo_cnt.
[15:0]: The counter of data has read to SRAM in ADC path of hds when rfifo is empty and wfifo is full.This counter is valid only when aec_cnt_flag = 1?1.IF both left channel and right channel of ADC are enabled, the counter of left channel is selected as aec_rfifo_cnt; otherwise, the counter of channel enabled is used as aec_rfifo_cnt.
*/

#define REG_DMA_HDS_RD_CTRL                               (DMA_BASE + 0x10e4)
#define REG_DMA_HDS_RD_CTRL_hdsr_cntr_surc_sel            ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_DMA_HDS_RD_CTRL_hdsr_surc_dev_sel             ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_DMA_HDS_RD_CTRL_hdsr_dest_mem_sel             ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_HDS_RD_CTRL_hds_rd_bitmod                 ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_HDS_RD_CTRL_hds_rd_abort                  ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_HDS_RD_CTRL_hds_rd_start                  ((((REG_DMA_HDS_RD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
HDS_RD_CTRL: 
[31:8]: reserved
[7]: reserved
[6]: reserved
[5]: destination memory selction.0: CM3 SRAM;1: DSP X/Y MEM
[4]: Bit mode selecting for hds ADC path.0h: 24bits mode.1h: 16bits mode
[3:2]: hds read channel enable.0h: disable 1h: only Left ADC channel enable2h: only Right ADC channel enable3h: Left and Right ADC channels enable
[1]: Abort the hds read path. When this bit is set to 1, the hds read process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the hds read path transfer.When this bit is set to 1, hds read path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_HDS_RD_STAT                               (DMA_BASE + 0x10e8)
#define REG_DMA_HDS_RD_STAT_hds_rd_stop                   ((((REG_DMA_HDS_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_HDS_RD_STAT_hds_rd_fe1                    ((((REG_DMA_HDS_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_HDS_RD_STAT_hds_rd_fe0                    ((((REG_DMA_HDS_RD_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
HDS_RD_STAT: 
[31:5]: reserved
[4]: Denotes if device (hds) stops or system aborts current DMA transfer. It is valid during hds_rd_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current hds read path interrupts (hds_rd_done or hds_rd_error interrupt). It is valid during hds_rd_done interrupt or hds_rd_error interrupt.0h: indicates the source of current hds read path is buf0.1h: indicates the source of current hds read path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current read DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current read DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_HDS_RD_RADD0                              (DMA_BASE + 0x10ec)
/*
HDS_RD_RADD0: 
[31]: Reserved
[30:0]: Start address in SRAM or X/Y MEM for buf0 of ping-pang buffer for right ADC channel of hds read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_RD_RADD1                              (DMA_BASE + 0x10f0)
/*
HDS_RD_RADD1: 
[31]: Reserved
[30:0]: Start address in SRAM or X/Y MEM for buf1 of ping-pang buffer for right ADC channel of hds read path. NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_RD_LADD0                              (DMA_BASE + 0x10f4)
#define REG_DMA_HDS_RD_LADD0_End_buf0                     ((((REG_DMA_HDS_RD_LADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
HDS_RD_LADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM or X/Y MEM for buf0 of ping-pang buffer for left ADC channel of hds read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_RD_LADD1                              (DMA_BASE + 0x10f8)
#define REG_DMA_HDS_RD_LADD1_End_buf1                     ((((REG_DMA_HDS_RD_LADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
HDS_RD_LADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM or X/Y MEM for buf1 of ping-pang buffer for left ADC channel of hds read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_RD_LENGTH0                            (DMA_BASE + 0x10fc)
/*
HDS_RD_LENGTH0: 
[31:0]: Read length for buf0 of ping-pang buffer for read hds DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_HDS_RD_LENGTH1                            (DMA_BASE + 0x1100)
/*
HDS_RD_LENGTH1: 
[31:0]: Read length for buf1 of ping-pang buffer for read hds DMA transfer. The unit of read length is byte.
*/

#define REG_DMA_HDS_RD_CNT                                (DMA_BASE + 0x1104)
/*
HDS_RD_CNT: 
[31:0]: Indicates the byte numbers of data has read to SRAM in just finished DMA transfer.The value of hds_rd_cnt is updated at end of each DMA transfer, and it is remained until next update or the path disable. It can be read in interrupt service process or afterwards. The value of hds_rd_cnt is equal to hds_rd_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to hds_rd_length0 (or 1).
*/

#define REG_DMA_HDS_WR_CTRL                               (DMA_BASE + 0x110c)
#define REG_DMA_HDS_WR_CTRL_hdsw_cntr_surc_sel            ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((7) << 2))
#define REG_DMA_HDS_WR_CTRL_hdsw_surc_mem_sel             ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((6) << 2))
#define REG_DMA_HDS_WR_CTRL_hdsw_dest_dev_sel             ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_DMA_HDS_WR_CTRL_hds_wr_bitmod                 ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_HDS_WR_CTRL_hds_wr_abort                  ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_HDS_WR_CTRL_hds_wr_start                  ((((REG_DMA_HDS_WR_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
HDS_WR_CTRL: 
[31:8]: reserved
[7]: reserved
[6]: source memory selction.0: CM3 SRAM;1: DSP X/Y MEM
[5]: reserved
[4]: Bit mode selecting for hds DAC path.0h: 24bits mode.1h: 16bits mode
[3:2]: hds write channel enable.0h: disable 1h: only Left DAC channel enable2h: only Right DAC channel enable3h: Left and Right DAC channels enable
[1]: Abort the hds write path. When this bit is set to 1, the hds write process should be aborted, and it will keep 1 until transfer process is aborted. It will return to 0 automatically when this transfer is finished.
[0]: Start the hds write path transfer.When this bit is set to 1, hds write path procedure start, and it will automatically return to 0 at next clock cycle. Before setting this bit, all the other corresponding transfer control registers must be configured already.
*/

#define REG_DMA_HDS_WR_STAT                               (DMA_BASE + 0x1110)
#define REG_DMA_HDS_WR_STAT_hds_wr_stop                   ((((REG_DMA_HDS_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_DMA_HDS_WR_STAT_hds_wr_fe1                    ((((REG_DMA_HDS_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_DMA_HDS_WR_STAT_hds_wr_fe0                    ((((REG_DMA_HDS_WR_STAT) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))
/*
HDS_WR_STAT: 
[31:5]: reserved
[4]: Denotes if device (hds) stops or system aborts current DMA transfer. It is valid during hds_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA is stopped by device or is aborted by system
[3:2]: Source of current hds write path interrupts (hds_wr_done or hds_wr_error interrupt). It is valid during hds_wr_done interrupt or hds_wr_error interrupt.0h: indicates the source of current hds write path is buf0.1h: indicates the source of current hds write path is buf1.2h: reserved.3h: reserved.
[1]: Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.
[0]: Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_HDS_WR_ADD0                               (DMA_BASE + 0x1114)
#define REG_DMA_HDS_WR_ADD0_End_buf0                      ((((REG_DMA_HDS_WR_ADD0) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
HDS_WR_ADD0: 
[31]: indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
[30:0]: Start address in SRAM for buf0 of ping-pang buffer for DAC channel of hds write path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_WR_ADD1                               (DMA_BASE + 0x1118)
#define REG_DMA_HDS_WR_ADD1_End_buf1                      ((((REG_DMA_HDS_WR_ADD1) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((31) << 2))
/*
HDS_WR_ADD1: 
[31]: Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
[30:0]: Start address in SRAM for buf1 of ping-pang buffer for DAC channel of hds read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_HDS_WR_LENGTH0                            (DMA_BASE + 0x111c)
/*
HDS_WR_LENGTH0: 
[31:0]: write length for buf0 of ping-pang buffer for write hds DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_HDS_WR_LENGTH1                            (DMA_BASE + 0x1120)
/*
HDS_WR_LENGTH1: 
[31:0]: write length for buf1 of ping-pang buffer for write hds DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_HDS_WR_CNT                                (DMA_BASE + 0x1124)
/*
HDS_WR_CNT: 
[31:0]: Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of hds_wr_cnt is updated at end of each DMA transfer, and it is remained until next update. It can be read in interrupt service process or afterwards. The value of hds_wr_cnt is equal to hds_wr_length0 (or 1) when DMA is finished normally, otherwise, it is less than or equal to hds_wr_length0 (or 1).
*/ 
//##+ str DMAC HDS REG end

#define REG_DMA_SPDIF_WR_CTRL    (DMA_BASE + 0x123c)
/*
spdif_wr_ctrl  :     spdif write channel control register
  [31:8]:  (reserved)  24'h0 reserved
  [7]:  spdifw_cntr_surc_sel  1'h0 reserved
  [6]:  reserved  1'h0 reserved. 
  [5]:  spdifw_dest_dev_sel  1'h0 reserved
  [4]:  spdif_wr_bitmod  1'h1 Bit mode selecting for spdif DAC path.0h: 24bits  or 32bits mode.1h: 16bits mode
  [3:2]:  spdif_wr_en  2'h0 spdif write channel enable.0h: disable 1h: only Left DAC channel enable2h: only Right DAC channel enable3h: Left and Right DAC channels enable
  [1]:  spdif_wr_abort  1'h0 Abort the spdif write path. When this bit is set to 1
  [0]:  spdif_wr_start  1'h0 Start the spdif write path transfer.When this bit is set to 1
*/

#define REG_DMA_SPDIF_WR_STAT    (DMA_BASE + 0x1240)
/*
spdif_wr_stat  :     Right ADC channel start address0 register
  [31:5]:  (reserved)  27'h0 reserved
  [4]:  spdif_wr_stop  1'h0 Denotes if device (spdif) stops or system aborts current DMA transfer. It is valid during spdif_wr_done interrupt. 0h: current DMA is finished normally.1h: current DMA  is stopped by device or is aborted by system
  [3:2]:  spdif_wrintr_src  2'h0 Source of current spdif write path interrupts (spdif_wr_done or spdif_wr_error interrupt). It is valid during spdif_wr_done interrupt or spdif_wr_error interrupt.0h: indicates the source of current spdif write path is buf0.1h: indicates the source of current spdif write path is buf1.2h: reserved.3h: reserved.
  [1]:  spdif_wr_fe1  1'h0 Indicates if buf1 is active in current write DMA transfer.0h: buf1 is not active.1h: buf1 is active.
  [0]:  spdif_wr_fe0  1'h0 Indicates if buf0 is active in current write DMA transfer.0h: buf0 is not active.1h: buf0 is active.
*/

#define REG_DMA_SPDIF_WR_ADD0    (DMA_BASE + 0x1244)
/*
spdif_wr_add0  :     spdif write channel start address0 register
  [31]:  End_buf0  1'h0 indicate if DMAC will finish in current processing of buf0 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf0.0h: DMAC transfer will switch to buf1.
  [30:0]:  spdif_wr_add0  31'h0 Start address in SRAM for buf0 of ping-pang buffer for DAC channel of spdif write path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPDIF_WR_ADD1    (DMA_BASE + 0x1248)
/*
spdif_wr_add1  :     spdif write channel start address1 register
  [31]:  End_buf1  1'h0 Indicate if DMAC will finish in current processing of buf1 of ping-pang buffer.1h: DMAC transfer will finish in procedure of buf1.0h: DMAC transfer will switch to buf0.
  [30:0]:  spdif_wr_add1  31'h0 Start address in SRAM for buf1 of ping-pang buffer for DAC channel of spdif read path.NOTE: this address must be work boundary aligned.
*/

#define REG_DMA_SPDIF_WR_LENGTH0    (DMA_BASE + 0x124C)
/*
spdif_wr_length0  :     spdif write length0 register
  [31:0]:  spdif_wr_length0  32'h0 write length for buf0 of ping-pang buffer for write spdif DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_SPDIF_WR_LENGTH1    (DMA_BASE + 0x1250)
/*
spdif_wr_length1  :     spdif write length1 register
  [31:0]:  spdif_wr_length1  32'h0 write length for buf1 of ping-pang buffer for write spdif DMA transfer. The unit of write length is byte.
*/

#define REG_DMA_SPDIF_WR_CNT    (DMA_BASE + 0x1254)
/*
spdif_wr_cnt  :     spdif write Counter Register
  [31:0]:  spdif_wr_cnt  32'h0 Indicates the byte numbers of data has written from SRAM in just finished DMA transfer.The value of spdif_wr_cnt is updated at end of each DMA transfer
  :     
*/


#define REG_DMA_USB_RD_CTRL				(DMA_BASE + 0x20C0)
#define REG_DMA_USB_RD_STAT				(DMA_BASE + 0x20C4)
#define REG_DMA_USB_RD_RAM0_ADD			(DMA_BASE + 0x20C8)
#define REG_DMA_USB_RD_RAM0_LENGTH		(DMA_BASE + 0x20D0)
#define REG_DMA_USB_RD_RAM1_ADD			(DMA_BASE + 0x20D8)
#define REG_DMA_USB_RD_RAM1_LENGTH		(DMA_BASE + 0x20E0)
#define REG_DMA_USB_RD_RAM2_ADD			(DMA_BASE + 0x2100)
#define REG_DMA_USB_RD_RAM2_LENGTH		(DMA_BASE + 0x2104)
#define REG_DMA_USB_RD_RAM0_CNT		    (DMA_BASE + 0x2108)
#define REG_DMA_USB_RD_RAM1_CNT			(DMA_BASE + 0x210C)
#define REG_DMA_USB_RD_RAM2_CNT			(DMA_BASE + 0x2110)
#define REG_DMA_USB_RD_RAM0_CURTCNT 	(DMA_BASE + 0x2114)
#define REG_DMA_USB_RD_RAM1_CURTCNT		(DMA_BASE + 0x2118) 
#define REG_DMA_USB_RD_RAM2_CURTCNT 	(DMA_BASE + 0x211C)

// revmove 0x11cC
#define REG_DMA_USB_WR_CTRL				(DMA_BASE + 0x2120)
#define REG_DMA_USB_WR_STAT				(DMA_BASE + 0x2124)
#define REG_DMA_USB_WR_RAM0_ADD			(DMA_BASE + 0x2128)
#define REG_DMA_USB_WR_RAM0_LENGTH		(DMA_BASE + 0x2130)
#define REG_DMA_USB_WR_RAM1_ADD			(DMA_BASE + 0x2138)
#define REG_DMA_USB_WR_RAM1_LENGTH		(DMA_BASE + 0x2140)
#define REG_DMA_USB_WR_RAM2_ADD			(DMA_BASE + 0x2148)
#define REG_DMA_USB_WR_RAM2_LENGTH		(DMA_BASE + 0x2150)
#define REG_DMA_USB_WR_RAM0_CNT			(DMA_BASE + 0x2170)
#define REG_DMA_USB_WR_RAM1_CNT			(DMA_BASE + 0x2174)
#define REG_DMA_USB_WR_RAM2_CNT         (DMA_BASE + 0x2178)

//USB read/write registers external ,ep3-ep5
#define REG_DMA_USB_RD_CTRL1				(DMA_BASE + 0x21E0)
#define REG_DMA_USB_RD_STAT1				(DMA_BASE + 0x21E4)
#define REG_DMA_USB_RD_RAM3_ADD			(DMA_BASE + 0x21E8)
#define REG_DMA_USB_RD_RAM3_LENGTH		(DMA_BASE + 0x21F0)
#define REG_DMA_USB_RD_RAM4_ADD			(DMA_BASE + 0x21F8)
#define REG_DMA_USB_RD_RAM4_LENGTH		(DMA_BASE + 0x2200)
#define REG_DMA_USB_RD_RAM5_ADD			(DMA_BASE + 0x2210)
#define REG_DMA_USB_RD_RAM5_LENGTH		(DMA_BASE + 0x2214)
#define REG_DMA_USB_RD_RAM3_CNT		       (DMA_BASE + 0x2218)
#define REG_DMA_USB_RD_RAM4_CNT			(DMA_BASE + 0x221C)
#define REG_DMA_USB_RD_RAM5_CNT			(DMA_BASE + 0x2220)
#define REG_DMA_USB_RD_RAM3_CURTCNT 	(DMA_BASE + 0x2224)
#define REG_DMA_USB_RD_RAM4_CURTCNT		(DMA_BASE + 0x2228) 
#define REG_DMA_USB_RD_RAM5_CURTCNT 	(DMA_BASE + 0x222C)
//write
#define REG_DMA_USB_WR_CTRL1				(DMA_BASE + 0x2230)
#define REG_DMA_USB_WR_STAT1				(DMA_BASE + 0x2234)
#define REG_DMA_USB_WR_RAM3_ADD			(DMA_BASE + 0x2238)
#define REG_DMA_USB_WR_RAM3_LENGTH		(DMA_BASE + 0x2240)
#define REG_DMA_USB_WR_RAM4_ADD			(DMA_BASE + 0x2248)
#define REG_DMA_USB_WR_RAM4_LENGTH		(DMA_BASE + 0x2250)
#define REG_DMA_USB_WR_RAM5_ADD			(DMA_BASE + 0x2258)
#define REG_DMA_USB_WR_RAM5_LENGTH		(DMA_BASE + 0x2260)
#define REG_DMA_USB_WR_RAM3_CNT			(DMA_BASE + 0x2280)
#define REG_DMA_USB_WR_RAM4_CNT			(DMA_BASE + 0x2284)
#define REG_DMA_USB_WR_RAM5_CNT               (DMA_BASE + 0x2288)
//int
#define REG_DMA_INTR_FNSH_MASK1			(DMA_BASE + 0x3034)
#define REG_DMA_INTR_ERR_MASK1			(DMA_BASE + 0x3038)
#define REG_DMA_INTR_FNSH_CLR1			(DMA_BASE + 0x303C)
#define REG_DMA_INTR_ERR_CLR1			       (DMA_BASE + 0x3040)
#define REG_DMA_INTR_FNSH_STAT1			(DMA_BASE + 0x3044)
#define REG_DMA_INTR_ERR_STAT1			(DMA_BASE + 0x3048)

#define REG_DMA_CHICKEN                 (DMA_BASE + 0x3900)


#define REG_DMA_SD_RD_CTRL				(DMA_BASE + 0x2180)
#define REG_DMA_SD_RD_STAT				(DMA_BASE + 0x2184)
#define REG_DMA_SD_RD_ADD				(DMA_BASE + 0x2188)
#define REG_DMA_SD_RD_LENGTH			(DMA_BASE + 0x218C)
#define REG_DMA_SD_RD_CNT				(DMA_BASE + 0x2190)
#define REG_DMA_SD_RD_CURTCNT			(DMA_BASE + 0x2194) 
#define REG_DMA_SD_WR_CTRL				(DMA_BASE + 0x21A0)
#define REG_DMA_SD_WR_STAT				(DMA_BASE + 0x21A4)
#define REG_DMA_SD_WR_ADD				(DMA_BASE + 0x21A8)
#define REG_DMA_SD_WR_LENGTH			(DMA_BASE + 0x21AC)
#define REG_DMA_SD_WR_CNT				(DMA_BASE + 0x21B0)
#define REG_DMA_SD_WR_CURTCNT			(DMA_BASE + 0x21B4)

#define REG_DMA_LCD_WR_CTRL				(DMA_BASE + 0x21C0)
#define REG_DMA_LCD_WR_STAT				(DMA_BASE + 0x21C4)
#define REG_DMA_LCD_WR_ADD				(DMA_BASE + 0x21C8)
#define REG_DMA_LCD_WR_LENGTH			(DMA_BASE + 0x21CC)
#define REG_DMA_LCD_WR_CNT				(DMA_BASE + 0x21D0)

#define REG_DMA_RAM_DSPDM_CTRL			(DMA_BASE + 0x1E0)
#define REG_DMA_RAM_DSPDM_STAT			(DMA_BASE + 0x1E4)
#define REG_DMA_RAM_DSPDM_SRCADD		(DMA_BASE + 0x1E8)
#define REG_DMA_RAM_DSPDM_DESTADD		(DMA_BASE + 0x1EC)
#define REG_DMA_RAM_DSPDM_LENGTH		(DMA_BASE + 0x1F0)
#define REG_DMA_RAM_DSPDM_CNT			(DMA_BASE + 0x1F4) 
#define REG_DMA_DSPDM_RAM_CTRL			(DMA_BASE + 0x200)
#define REG_DMA_DSPDM_RAM_STAT			(DMA_BASE + 0x204)
#define REG_DMA_DSPDM_RAM_SRCADD		(DMA_BASE + 0x208)
#define REG_DMA_DSPDM_RAM_DESTADD		(DMA_BASE + 0x20C)
#define REG_DMA_DSPDM_RAM_LENGTH		(DMA_BASE + 0x210)
#define REG_DMA_DSPDM_RAM_CNT			(DMA_BASE + 0x214) 

#define REG_DMA_RAM_DSPPM_CTRL			(DMA_BASE + 0x220)
#define REG_DMA_RAM_DSPPM_STAT			(DMA_BASE + 0x224)
#define REG_DMA_RAM_DSPPM_SRCADD		(DMA_BASE + 0x228)
#define REG_DMA_RAM_DSPPM_DESTADD		(DMA_BASE + 0x22C)
#define REG_DMA_RAM_DSPPM_LENGTH		(DMA_BASE + 0x230)
#define REG_DMA_RAM_DSPPM_CNT			(DMA_BASE + 0x234) 

//#define REG_DMA_AES_RD_CTRL             (DMA_BASE + 0x240) 
//#define REG_DMA_AES_RD_STAT             (DMA_BASE + 0x244) 
//#define REG_DMA_AES_RD_ADD              (DMA_BASE + 0x248) 
//#define REG_DMA_AES_RD_LENGTH           (DMA_BASE + 0x24C) 
//#define REG_DMA_AES_RD_CNT              (DMA_BASE + 0x250)  
//#define REG_DMA_AES_CRRT_RD_CNT         (DMA_BASE + 0x254)   
//#define REG_DMA_AES_WR_CTRL             (DMA_BASE + 0x258)  
//#define REG_DMA_AES_WR_STAT             (DMA_BASE + 0x25C) 
//#define REG_DMA_AES_WR_ADD              (DMA_BASE + 0x260)  
//#define REG_DMA_AES_WR_LENGTH           (DMA_BASE + 0x264)  
//#define REG_DMA_AES_WR_CNT              (DMA_BASE + 0x268) 

#define REG_DMA_INTR_FNSH_MASK    (DMA_BASE + 0x3004)
/*
INTR_FNSH_MASK  :     path finish Interrupt mask Register
  [31]:  dspdm2hds_wr_done_msk  1'h0 dspdm2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [30]:  sram2hds_wr_done_msk  1'h0 sram2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [29]:  hds2dspdm_rd_done_msk  1'h0 hds2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [28]:  hds2sram_rd_done_msk  1'h0 hds2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [27]:  sram2i2s_wr_done_msk  1'h0 sram2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s_wr_done_msk  1'h0 dspdm2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  dspdm2aif_wr_done_msk  1'h0 dspdm2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [24]:  sram2aif_wr_done_msk  1'h0 sram2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [23]:  dspdm2sram_done_msk  1'h0 dspdm2sram transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [22]:  sram2dspdm_done_msk  1'h0 sram2dspdm transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [21]:  i2s2sram_rd_done_msk  1'h0 i2s2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s2dspdm_rd_done_msk  1'h0 i2s2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  aif2dspdm_rd_done_msk  1'h0 aif2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  aif2sram_rd_done_msk  1'h0 aif2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_done_msk  1'h0 sram2lcd write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [15]:  sram2spi_wr_done_msk  1'h0 sram2spi write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_done_msk  1'h0 spi2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  sram2sd_wr_done_msk  1'h0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [11]:  sram2dsppm_done_msk  1'h0 sram2dsppm transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [10]:  sd2sram_rd_done_msk  1'h0 sd2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [9]:  sram2usb_wr_done_msk2  1'h0 sram2usb2 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk1  1'h0 sram2usb1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk0  1'h0 sram2usb0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk2  1'h0 usb2sram2 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk1  1'h0 usb2sram1 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk0  1'h0 usb2sram0 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  sram2aes_wr_done_msk  1'h0 sram2aes write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [2]:  aes2sram_rd_done_msk  1'h0 aes2sram read transfer done interrupt mask register.1 h1: aes2sram_rd_done is muted. 1 h0: aes2sram_rd_done is unmasked.
  [1]:  sram2bb_wr_done_msk  1'h0 sram2bb write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [0]:  bb2sram_rd_done_msk  1'h0 bb2sram read transfer done interrupt mask register.1 h1: bb2sram_rd_done is muted. 1 h0: bb2sram_rd_done is unmasked.
*/

#define REG_DMA_INTR_ERR_MASK    (DMA_BASE + 0x3008)
/*
INTR_ERR_MASK  :     path err Interrupt mask Register
  [31]:  dspdm2hds_wr_undr_msk  1'h0 dspdm2hds write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [30]:  sram2hds_wr_undr_msk  1'h0 sram2hds write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [29]:  hds2dspdm_rd_over_msk  1'h0 hds2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [28]:  hds2sram_rd_over_msk  1'h0 hds2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [27]:  sram2i2s_wr_undr_msk  1'h0 sram2i2s write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [26]:  dspdm2i2s_wr_undr_msk  1'h0 dspdm2i2s write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [25]:  dspdm2aif_wr_undr_msk  1'h0 dspdm2aif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [24]:  sram2aif_wr_undr_msk  1'h0 sram2aif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [23]:  dspdm2sram_over_msk  1'h0 dspdm2sram transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [22]:  sram2dspdm_undr_msk  1'h0 sram2dspdm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [21]:  i2s2sram_rd_over_msk  1'h0 i2s2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [20]:  i2s2dspdm_rd_over_msk  1'h0 i2s2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [19]:  aif2dspdm_rd_over_msk  1'h0 aif2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [18]:  aif2sram_rd_over_msk  1'h0 aif2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_undr_msk  1'h0 sram2lcd write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [15]:  sram2spi_wr_undr_msk  1'h0 sram2spi write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_over_msk  1'h0 spi2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [12]:  sram2sd_wr_undr_msk  1'h0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [11]:  sram2dsppm_undr_msk  1'h0 sram2dsppm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [10]:  sd2sram_rd_over_msk  1'h0 sd2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [9]:  sram2usb_wr_undr_msk2  1'h0 sram2usb2 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [8]:  sram2usb_wr_undr_msk1  1'h0 sram2usb1 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [7]:  sram2usb_wr_undr_msk0  1'h0 sram2usb0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [6]:  usb2sram_rd_over_msk2  1'h0 usb2sram2 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [5]:  usb2sram_rd_over_msk1  1'h0 usb2sram1 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [4]:  usb2sram_rd_over_msk0  1'h0 usb2sram0 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [3]:  sram2aes_wr_undr_msk  1'h0 sram2aes write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [2]:  aes2sram_rd_over_msk  1'h0 aes2sram read overflow err interrupt mask register.1 h1: this path err is muted. 1 h0: this path err is unmasked.
  [1]:  sram2bb_wr_undr_msk  1'h0 sram2bb write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [0]:  bb2sram_rd_over_msk  1'h0 bb2sram read overflow err interrupt mask register.1 h1: this path err is muted. 1 h0: this path err is unmasked.
*/

#define REG_DMA_INTR_FNSH_CLR    (DMA_BASE + 0x300c)
/*
INTR_FNSH_CLR  :     path finish Interrupt clear Register
  [31]:  dspdm2hds_wr_done_clr  1'h0 dspdm2hds write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [30]:  sram2hds_wr_done_clr  1'h0 sram2hds write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [29]:  hds2dspdm_rd_done_clr  1'h0 hds2dspdm read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [28]:  hds2sram_rd_done_clr  1'h0 hds2sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [27]:  sram2i2s_wr_done_clr  1'h0 sram2i2s write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [26]:  dspdm2i2s_wr_done_clr  1'h0 dspdm2i2s write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [25]:  dspdm2aif_wr_done_clr  1'h0 dspdm2aif write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [24]:  sram2aif_wr_done_clr  1'h0 sram2aif write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [23]:  dspdm2sram_done_clr  1'h0 dspdm2sram transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [22]:  sram2dspdm_done_clr  1'h0 sram2dspdm transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [21]:  i2s2sram_rd_done_clr  1'h0 i2s2sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [20]:  i2s2dspdm_rd_done_clr  1'h0 i2s2dspdm read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [19]:  aif2dspdm_rd_done_clr  1'h0 aif2dspdm read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [18]:  aif2sram_rd_done_clr  1'h0 aif2sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_done_clr  1'h0 sram2lcd write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [15]:  sram2spi_wr_done_clr  1'h0 sram2spi write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_done_clr  1'h0 spi2sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [12]:  sram2sd_wr_done_clr  1'h0 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [11]:  sram2dsppm_done_clr  1'h0 sram2dsppm transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [10]:  sd2sram_rd_done_clr  1'h0 sd2sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [9]:  sram2usb_wr_done_clr2  1'h0 sram2usb2 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [8]:  sram2usb_wr_done_clr1  1'h0 sram2usb1 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [7]:  sram2usb_wr_done_clr0  1'h0 sram2usb0 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [6]:  usb2sram_rd_done_clr2  1'h0 usb2sram2 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [5]:  usb2sram_rd_done_clr1  1'h0 usb2sram1 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [4]:  usb2sram_rd_done_clr0  1'h0 usb2sram0 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [3]:  sram2aes_wr_done_clr  1'h0 sram2aes write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [2]:  aes2sram_rd_done_clr  1'h0 aes2sram read transfer done interrupt clear register.1 h1: aes2sram_rd_done is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [1]:  sram2bb_wr_done_clr  1'h0 sram2bb write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [0]:  bb2sram_rd_done_clr  1'h0 bb2sram read transfer done interrupt clear register.1 h1: bb2sram_rd_done is cleared to 0. This bit will automatically return to 0 at next clock cycle.
*/

#define REG_DMA_INTR_ERR_CLR    (DMA_BASE + 0x3010)
/*
INTR_ERR_CLR  :     path err Interrupt clear Register
  [31]:  dspdm2hds_wr_undr_clr  1'h0 dspdm2hds write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [30]:  sram2hds_wr_undr_clr  1'h0 sram2hds write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [29]:  hds2dspdm_rd_over_clr  1'h0 hds2dspdm read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [28]:  hds2sram_rd_over_clr  1'h0 hds2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [27]:  sram2i2s_wr_undr_clr  1'h0 sram2i2s write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [26]:  dspdm2i2s_wr_undr_clr  1'h0 dspdm2i2s write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [25]:  dspdm2aif_wr_undr_clr  1'h0 dspdm2aif write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [24]:  sram2aif_wr_undr_clr  1'h0 sram2aif write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [23]:  dspdm2sram_over_clr  1'h0 dspdm2sram transfer err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [22]:  sram2dspdm_undr_clr  1'h0 sram2dspdm transfer err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [21]:  i2s2sram_rd_over_clr  1'h0 i2s2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [20]:  i2s2dspdm_rd_over_clr  1'h0 i2s2dspdm read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [19]:  aif2dspdm_rd_over_clr  1'h0 aif2dspdm read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [18]:  aif2sram_rd_over_clr  1'h0 aif2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_undr_clr  1'h0 sram2lcd write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [15]:  sram2spi_wr_undr_clr  1'h0 sram2spi write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_over_clr  1'h0 spi2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [12]:  sram2sd_wr_undr_clr  1'h0 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [11]:  sram2dsppm_undr_clr  1'h0 sram2dsppm transfer err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [10]:  sd2sram_rd_over_clr  1'h0 sd2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [9]:  sram2usb_wr_undr_clr2  1'h0 sram2usb2 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [8]:  sram2usb_wr_undr_clr1  1'h0 sram2usb1 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [7]:  sram2usb_wr_undr_clr0  1'h0 sram2usb0 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [6]:  usb2sram_rd_over_clr2  1'h0 usb2sram2 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [5]:  usb2sram_rd_over_clr1  1'h0 usb2sram1 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [4]:  usb2sram_rd_over_clr0  1'h0 usb2sram0 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [3]:  sram2aes_wr_undr_clr  1'h0 sram2aes write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [2]:  aes2sram_rd_over_clr  1'h0 aes2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [1]:  sram2bb_wr_undr_clr  1'h0 sram2bb write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [0]:  bb2sram_rd_over_clr  1'h0 bb2sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
*/

#define REG_DMA_INTR_FNSH_STAT    (DMA_BASE + 0x3014)
/*
INTR_FNSH_STAT  :     path finish Interrupt state Register
  [31]:  dspdm2hds_wr_done  1'h0 dspdm2hds write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [30]:  sram2hds_wr_done  1'h0 sram2hds write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [29]:  hds2dspdm_rd_done  1'h0 hds2dspdm read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [28]:  hds2sram_rd_done  1'h0 hds2sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [27]:  sram2i2s_wr_done  1'h0 sram2i2s write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [26]:  dspdm2i2s_wr_done  1'h0 dspdm2i2s write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [25]:  dspdm2aif_wr_done  1'h0 dspdm2aif write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [24]:  sram2aif_wr_done  1'h0 sram2aif write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [23]:  dspdm2sram_done  1'h0 dspdm2sram transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [22]:  sram2dspdm_done  1'h0 sram2dspdm transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [21]:  i2s2sram_rd_done  1'h0 i2s2sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [20]:  i2s2dspdm_rd_done  1'h0 i2s2dspdm read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [19]:  aif2dspdm_rd_done  1'h0 aif2dspdm read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [18]:  aif2sram_rd_done  1'h0 aif2sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_done  1'h0 sram2lcd write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [15]:  sram2spi_wr_done  1'h0 sram2spi write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_done  1'h0 spi2sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [12]:  sram2sd_wr_done  1'h0 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [11]:  sram2dsppm_done  1'h0 sram2dsppm transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [10]:  sd2sram_rd_done  1'h0 sd2sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [9]:  sram2usb_wr_done2  1'h0 sram2usb2 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [8]:  sram2usb_wr_done1  1'h0 sram2usb1 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [7]:  sram2usb_wr_done0  1'h0 sram2usb0 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [6]:  usb2sram_rd_done2  1'h0 usb2sram2 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [5]:  usb2sram_rd_done1  1'h0 usb2sram1 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [4]:  usb2sram_rd_done0  1'h0 usb2sram0 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [3]:  sram2aes_wr_done  1'h0 sram2aes write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [2]:  aes2sram_rd_done  1'h0 aes2sram read transfer done interrupt state register.1 h1: aes2sram_rd_done is occurred. 1 h0: aes2sram_rd_done is not occurred.
  [1]:  sram2bb_wr_done  1'h0 sram2bb write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [0]:  bb2sram_rd_done  1'h0 bb2sram read transfer done interrupt state register.1 h1: bb2sram_rd_done is occurred. 1 h0: bb2sram_rd_done is not occurred.
*/

#define REG_DMA_INTR_ERR_STAT    (DMA_BASE + 0x3018)
/*
INTR_ERR_STAT  :     path err Interrupt state Register
  [31]:  dspdm2hds_wr_undr  1'h0 dspdm2hds write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [30]:  sram2hds_wr_undr  1'h0 sram2hds write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [29]:  hds2dspdm_rd_over  1'h0 hds2dspdm read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [28]:  hds2sram_rd_over  1'h0 hds2sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [27]:  sram2i2s_wr_undr  1'h0 sram2i2s write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [26]:  dspdm2i2s_wr_undr  1'h0 dspdm2i2s write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [25]:  dspdm2aif_wr_undr  1'h0 dspdm2aif write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [24]:  sram2aif_wr_undr  1'h0 sram2aif write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [23]:  dspdm2sram_over  1'h0 dspdm2sram transfer err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path erri interrupt is not occurred.
  [22]:  sram2dspdm_undr  1'h0 sram2dspdm transfer err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path erri interrupt is not occurred.
  [21]:  i2s2sram_rd_over  1'h0 i2s2sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [20]:  i2s2dspdm_rd_over  1'h0 i2s2dspdm read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [19]:  aif2dspdm_rd_over  1'h0 aif2dspdm read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [18]:  aif2sram_rd_over  1'h0 aif2sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_undr  1'h0 sram2lcd write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [15]:  sram2spi_wr_undr  1'h0 sram2spi write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_over  1'h0 spi2sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [12]:  sram2sd_wr_undr  1'h0 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [11]:  sram2dsppm_undr  1'h0 sram2dsppm transfer err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path erri interrupt is not occurred.
  [10]:  sd2sram_rd_over  1'h0 sd2sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [9]:  sram2usb_wr_undr2  1'h0 sram2usb2 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [8]:  sram2usb_wr_undr1  1'h0 sram2usb1 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [7]:  sram2usb_wr_undr0  1'h0 sram2usb0 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [6]:  usb2sram_rd_over2  1'h0 usb2sram2 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [5]:  usb2sram_rd_over1  1'h0 usb2sram1 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [4]:  usb2sram_rd_over0  1'h0 usb2sram0 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [3]:  sram2aes_wr_undr  1'h0 sram2aes write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [2]:  aes2sram_rd_over  1'h0 aes2sram read overflow err interrupt state register.1 h1: this path err is occurred. 1 h0: this path err is not occurred.
  [1]:  sram2bb_wr_undr  1'h0 sram2bb write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [0]:  bb2sram_rd_over  1'h0 bb2sram read overflow err interrupt state register.1 h1: this path err is occurred. 1 h0: this path err is not occurred.
*/

#define REG_DMA_MCU_ACCESS_LIMIT    (DMA_BASE + 0x3020)
/*
MCU_ACCESS_LIMIT  :     path finish Interrupt mask Register
  [31:17]:  (reserved)  15'h0 
  [16]:  limit_access_ocuure  1'h0 indicate that access of is ocurred. This bit can be write by SW.
  [15]:  mode35  1'h0 indicate chip is worked in 9635 mode or 9638 mode. 1'b0: in 9638 mode. 1'b1: in 9635 mode.
  [14]:  mcu  1'h0 indicate which MUC acssess inappropriately register. 1'b0: CM3TOP 1'b1: CM3BT.
  [13:0]:  addr[13:0]  14'h0 indicate last access register address to which inadmissible MCU accesses. 
*/

#define REG_DMA_INTR_FNSH_MASK1    (DMA_BASE + 0x3034)
/*
INTR_FNSH_MASK1  :     path finish Interrupt mask Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_done_msk  1'h0 sram2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s0_wr_done_msk  1'h0 dspdm2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_done_msk  1'h0 i2s02sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s02dspdm_rd_done_msk  1'h0 i2s02dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  sram2spdif_wr_done_msk  1'h0 sram2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  reserved  1'h0 dspdm2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.In va9638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_done_msk  1'h0 sram2spi1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_done_msk  1'h0 spi12sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_done_msk5  1'h0 sram2usb5 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk4  1'h0 sram2usb4 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk3  1'h0 sram2usb3 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk5  1'h0 usb2sram5 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk4  1'h0 usb2sram4 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk3  1'h0 usb2sram3 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/

#define REG_DMA_INTR_ERR_MASK1    (DMA_BASE + 0x3038)
/*
INTR_ERR_MASK1  :     path err Interrupt mask Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_undr_msk  1'h0 sram2i2s0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [26]:  dspdm2i2s0_wr_undr_msk  1'h0 dspdm2i2s0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_over_msk  1'h0 i2s02sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [20]:  i2s02dspdm_rd_over_msk  1'h0 i2s02dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [19]:  sram2spdif_wr_undr_msk  1'h0 sram2spdif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [18]:  reserved  1'h0 dspdm2spdif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.In va9638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_undr_msk  1'h0 sram2spi1 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_over_msk  1'h0 spi12sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_undr_msk5  1'h0 sram2usb5 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [8]:  sram2usb_wr_undr_msk4  1'h0 sram2usb4 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [7]:  sram2usb_wr_undr_msk3  1'h0 sram2usb3 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [6]:  usb5sram_rd_over_msk5  1'h0 usb5sram5 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [5]:  usb5sram_rd_over_msk4  1'h0 usb5sram4 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [4]:  usb5sram_rd_over_msk3  1'h0 usb5sram3 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/

//#define REG_DMA_INTR_FNSH_CLR1    (DMA_BASE + 0x303c)
/*
INTR_FNSH_CLR1  :     path finish Interrupt clear Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_done_clr  1'h0 sram2i2s0 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [26]:  dspdm2i2s0_wr_done_clr  1'h0 dspdm2i2s0 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_done_clr  1'h0 i2s02sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [20]:  i2s02dspdm_rd_done_clr  1'h0 i2s02dspdm read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [19]:  sram2spdif_wr_done_clr  1'h0 sram2spdif write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [18]:  reserved  1'h0 dspdm2spdif write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.In va9638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_done_clr  1'h0 sram2spi1 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_done_clr  1'h0 spi12sram read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_done_clr5  1'h0 sram2usb5 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [8]:  sram2usb_wr_done_clr4  1'h0 sram2usb4 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [7]:  sram2usb_wr_done_clr3  1'h0 sram2usb3 write transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [6]:  usb5sram_rd_done_clr5  1'h0 usb5sram5 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [5]:  usb5sram_rd_done_clr4  1'h0 usb5sram4 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [4]:  usb5sram_rd_done_clr3  1'h0 usb5sram3 read transfer done interrupt clear register.1 h1: this path finish interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/

#define REG_DMA_INTR_ERR_CLR1    (DMA_BASE + 0x3040)
/*
INTR_ERR_CLR1  :     path err Interrupt clear Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_undr_clr  1'h0 sram2i2s0 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [26]:  dspdm2i2s0_wr_undr_clr  1'h0 dspdm2i2s0 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_over_clr  1'h0 i2s02sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [20]:  i2s02dspdm_rd_over_clr  1'h0 i2s02dspdm read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [19]:  sram2spdif_wr_undr_clr  1'h0 sram2spdif write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [18]:  reserved  1'h0 dspdm2spdif write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.In va9638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_undr_clr  1'h0 sram2spi1 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_over_clr  1'h0 spi12sram read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_undr_clr5  1'h0 sram2usb5 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [8]:  sram2usb_wr_undr_clr4  1'h0 sram2usb4 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [7]:  sram2usb_wr_undr_clr3  1'h0 sram2usb3 write underflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [6]:  usb5sram_rd_over_clr5  1'h0 usb5sram5 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [5]:  usb5sram_rd_over_clr4  1'h0 usb5sram4 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [4]:  usb5sram_rd_over_clr3  1'h0 usb5sram3 read overflow err interrupt clear register.1 h1: this path err interrupt is cleared to 0. This bit will automatically return to 0 at next clock cycle.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/

#define REG_DMA_INTR_FNSH_STAT1    (DMA_BASE + 0x3044)
/*
INTR_FNSH_STAT1  :     path finish Interrupt state Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_done  1'h0 sram2i2s0 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [26]:  dspdm2i2s0_wr_done  1'h0 dspdm2i2s0 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_done  1'h0 i2s02sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [20]:  i2s02dspdm_rd_done  1'h0 i2s02dspdm read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [19]:  sram2spdif_wr_done  1'h0 sram2spdif write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [18]:  reserved  1'h0 dspdm2spdif write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.In va0638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_done  1'h0 sram2spi1 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_done  1'h0 spi12sram read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_done5  1'h0 sram2usb5 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [8]:  sram2usb_wr_done4  1'h0 sram2usb4 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [7]:  sram2usb_wr_done3  1'h0 sram2usb3 write transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [6]:  usb5sram_rd_done5  1'h0 usb5sram5 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [5]:  usb5sram_rd_done4  1'h0 usb5sram4 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [4]:  usb5sram_rd_done3  1'h0 usb5sram3 read transfer done interrupt state register.1 h1: this path finish interrupt is occurred. 1 h0: this path finishi interrupt is not occurred.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/

#define REG_DMA_INTR_ERR_STAT1    (DMA_BASE + 0x3048)
/*
INTR_ERR_STAT1  :     path err Interrupt state Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_undr  1'h0 sram2i2s0 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [26]:  dspdm2i2s0_wr_undr  1'h0 dspdm2i2s0 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_over  1'h0 i2s02sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [20]:  i2s02dspdm_rd_over  1'h0 i2s02dspdm read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [19]:  sram2spdif_wr_undr  1'h0 sram2spdif write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [18]:  reserved  1'h0 dspdm2spdif write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.In va0638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_undr  1'h0 sram2spi1 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_over  1'h0 spi12sram read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_undr5  1'h0 sram2usb5 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [8]:  sram2usb_wr_undr4  1'h0 sram2usb4 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [7]:  sram2usb_wr_undr3  1'h0 sram2usb3 write underflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [6]:  usb5sram_rd_over5  1'h0 usb5sram5 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [5]:  usb5sram_rd_over4  1'h0 usb5sram4 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [4]:  usb5sram_rd_over3  1'h0 usb5sram3 read overflow err interrupt state register.1 h1: this path err interrupt is occurred. 1 h0: this path err interrupt is not occurred.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
  :     
*/

#define REG_DMA_INTR_FNSH_MASK_MCUBT    (DMA_BASE + 0x3804)
/*
INTR_FNSH_MASK_MCUBT  :     path finish Interrupt mask Register
  [31]:  dspdm2hds_wr_done_msk  1'h1 dspdm2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [30]:  sram2hds_wr_done_msk  1'h1 sram2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [29]:  hds2dspdm_rd_done_msk  1'h1 hds2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [28]:  hds2sram_rd_done_msk  1'h1 hds2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [27]:  sram2i2s_wr_done_msk  1'h1 sram2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s_wr_done_msk  1'h1 dspdm2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  dspdm2aif_wr_done_msk  1'h1 dspdm2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [24]:  sram2aif_wr_done_msk  1'h1 sram2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [23]:  dspdm2sram_over_msk  1'h1 dspdm2sram transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [22]:  sram2dspdm_undr_msk  1'h1 sram2dspdm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [21]:  i2s2sram_rd_done_msk  1'h1 i2s2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s2dspdm_rd_done_msk  1'h1 i2s2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  aif2dspdm_rd_done_msk  1'h1 aif2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  aif2sram_rd_done_msk  1'h1 aif2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [17]:  reserved  1'h1 reserved
  [16]:  sram2lcd_wr_done_msk  1'h1 sram2lcd write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [15]:  sram2spi_wr_done_msk  1'h1 sram2spi write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h1 reserved
  [13]:  spi2sram_rd_done_msk  1'h1 spi2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  sram2sd_wr_done_msk  1'h1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [11]:  sram2dsppm_undr_msk  1'h1 sram2dsppm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [10]:  sd2sram_rd_done_msk  1'h1 sd2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [9]:  sram2usb_wr_done_msk2  1'h1 sram2usb2 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk1  1'h1 sram2usb1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk0  1'h1 sram2usb0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk2  1'h1 usb2sram2 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk1  1'h1 usb2sram1 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk0  1'h1 usb2sram0 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  sram2aes_wr_done_msk  1'h0 sram2aes write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [2]:  aes2sram_rd_done_msk  1'h0 aes2sram read transfer done interrupt mask register.1 h1: aes2sram_rd_done is muted. 1 h0: aes2sram_rd_done is unmasked.
  [1]:  sram2bb_wr_done_msk  1'h0 sram2bb write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [0]:  bb2sram_rd_done_msk  1'h0 bb2sram read transfer done interrupt mask register.1 h1: bb2sram_rd_done is muted. 1 h0: bb2sram_rd_done is unmasked.
*/

#define REG_DMA_INTR_ERR_MASK_MCUBT    (DMA_BASE + 0x3808)
/*
INTR_ERR_MASK_MCUBT  :     path err Interrupt mask Register
  [31]:  dspdm2hds_wr_undr_msk  1'h1 dspdm2hds write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [30]:  sram2hds_wr_undr_msk  1'h1 sram2hds write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [29]:  hds2dspdm_rd_over_msk  1'h1 hds2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [28]:  hds2sram_rd_over_msk  1'h1 hds2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [27]:  sram2i2s_wr_undr_msk  1'h1 sram2i2s write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [26]:  dspdm2i2s_wr_undr_msk  1'h1 dspdm2i2s write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [25]:  dspdm2aif_wr_undr_msk  1'h1 dspdm2aif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [24]:  sram2aif_wr_undr_msk  1'h1 sram2aif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [23]:  dspdm2sram_over_msk  1'h1 dspdm2sram transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [22]:  sram2dspdm_undr_msk  1'h1 sram2dspdm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [21]:  i2s2sram_rd_over_msk  1'h1 i2s2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [20]:  i2s2dspdm_rd_over_msk  1'h1 i2s2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [19]:  aif2dspdm_rd_over_msk  1'h1 aif2dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [18]:  aif2sram_rd_over_msk  1'h1 aif2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [17]:  reserved  1'h1 reserved
  [16]:  sram2lcd_wr_undr_msk  1'h1 sram2lcd write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [15]:  sram2spi_wr_undr_msk  1'h1 sram2spi write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [14]:  reserved  1'h1 reserved
  [13]:  spi2sram_rd_over_msk  1'h1 spi2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [12]:  sram2sd_wr_undr_msk  1'h1 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [11]:  sram2dsppm_undr_msk  1'h1 sram2dsppm transfer err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path erri interrupt is unmasked.
  [10]:  sd2sram_rd_over_msk  1'h1 sd2sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [9]:  sram2usb_wr_undr_msk2  1'h1 sram2usb2 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [8]:  sram2usb_wr_undr_msk1  1'h1 sram2usb1 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [7]:  sram2usb_wr_undr_msk0  1'h1 sram2usb0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [6]:  usb2sram_rd_over_msk2  1'h1 usb2sram2 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [5]:  usb2sram_rd_over_msk1  1'h1 usb2sram1 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [4]:  usb2sram_rd_over_msk0  1'h1 usb2sram0 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [3]:  sram2aes_wr_undr_msk  1'h1 sram2aes write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [2]:  aes2sram_rd_over_msk  1'h1 aes2sram read overflow err interrupt mask register.1 h1: this path err is muted. 1 h0: this path err is unmasked.
  [1]:  sram2bb_wr_undr_msk  1'h1 sram2bb write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [0]:  bb2sram_rd_over_msk  1'h1 bb2sram read overflow err interrupt mask register.1 h1: this path err is muted. 1 h0: this path err is unmasked.
*/

#define REG_DMA_INTR_FNSH_MASK_MCUBT1    (DMA_BASE + 0x380c)
/*
INTR_FNSH_MASK_MCUBT1  :     path finish Interrupt mask Register
  [31]:  reserved  1'h1 reserved
  [30]:  reserved  1'h1 reserved
  [29]:  reserved  1'h1 reserved
  [28]:  reserved  1'h1 reserved
  [27]:  sram2i2s0_wr_done_msk  1'h1 sram2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s0_wr_done_msk  1'h1 dspdm2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  reserved  1'h1 reserved
  [24]:  reserved  1'h1 reserved
  [23]:  reserved  1'h1 reserved
  [22]:  reserved  1'h1 reserved
  [21]:  i2s02sram_rd_done_msk  1'h1 i2s02sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s02dspdm_rd_done_msk  1'h1 i2s02dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  sram2spdif_wr_done_msk  1'h1 sram2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  reserved  1'h1 dspdm2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.In va0638d, this bit is invalid.
  [17]:  reserved  1'h1 reserved
  [16]:  reserved  1'h1 reserved
  [15]:  sram2spi1_wr_done_msk  1'h1 sram2spi1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h1 reserved
  [13]:  spi12sram_rd_done_msk  1'h1 spi12sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  reserved  1'h1 reserved
  [11]:  reserved  1'h1 reserved
  [10]:  reserved  1'h1 reserved
  [9]:  sram2usb_wr_done_msk5  1'h1 sram2usb5 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk4  1'h1 sram2usb4 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk3  1'h1 sram2usb3 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk5  1'h1 usb2sram5 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk4  1'h1 usb2sram4 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk3  1'h1 usb2sram3 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  reserved  1'h1 reserved
  [2]:  reserved  1'h1 reserved
  [1]:  reserved  1'h1 reserved
  [0]:  reserved  1'h1 reserved
*/

#define REG_DMA_INTR_ERR_MASK_MCUBT1    (DMA_BASE + 0x3810)
/*
INTR_ERR_MASK_MCUBT1  :     path err Interrupt mask Register
  [31]:  reserved  1'h1 reserved
  [30]:  reserved  1'h1 reserved
  [29]:  reserved  1'h1 reserved
  [28]:  reserved  1'h1 reserved
  [27]:  sram2i2s0_wr_undr_msk  1'h1 sram2i2s0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [26]:  dspdm2i2s0_wr_undr_msk  1'h1 dspdm2i2s0 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [25]:  reserved  1'h1 reserved
  [24]:  reserved  1'h1 reserved
  [23]:  reserved  1'h1 reserved
  [22]:  reserved  1'h1 reserved
  [21]:  i2s02sram_rd_over_msk  1'h1 i2s02sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [20]:  i2s02dspdm_rd_over_msk  1'h1 i2s02dspdm read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [19]:  sram2spdif_wr_undr_msk  1'h1 sram2spdif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [18]:  reserved  1'h1 dspdm2spdif write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked. In va0638d, this bit is invalid.
  [17]:  reserved  1'h1 reserved
  [16]:  reserved  1'h1 reserved
  [15]:  sram2spi1_wr_undr_msk  1'h1 sram2spi1 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [14]:  reserved  1'h1 reserved
  [13]:  spi12sram_rd_over_msk  1'h1 spi12sram read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [12]:  reserved  1'h1 reserved
  [11]:  reserved  1'h1 reserved
  [10]:  reserved  1'h1 reserved
  [9]:  sram2usb_wr_undr_msk5  1'h1 sram2usb5 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [8]:  sram2usb_wr_undr_msk4  1'h1 sram2usb4 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [7]:  sram2usb_wr_undr_msk3  1'h1 sram2usb3 write underflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [6]:  usb5sram_rd_over_msk5  1'h1 usb5sram5 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [5]:  usb5sram_rd_over_msk4  1'h1 usb5sram4 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [4]:  usb5sram_rd_over_msk3  1'h1 usb5sram3 read overflow err interrupt mask register.1 h1: this path err interrupt is muted. 1 h0: this path err interrupt is unmasked.
  [3]:  reserved  1'h1 reserved
  [2]:  reserved  1'h1 reserved
  [1]:  reserved  1'h1 reserved
  [0]:  reserved  1'h1 reserved
*/

#define REG_DMA_CHICKEN    (DMA_BASE + 0x3900)
/*
CHICKEN  [31:7]:  Reserved  25'h0 reserved
  [6]:  aud_chicken  1'h1 indicate that all registers are back to va9638b version.
0: audio registers are in new version.
1: audio registers are backward to va9638b version.
  [5]:  usb_chicken  1'h1 indicate that all registers are back to va9638b version.
0: usb registers are in new version.
1: usb registers are backward to va9638b version.
  [4]:  spdif_chicken  1'h1 indicate that all registers are back to va9638b version.
0: spdif registers are in new version.
1: spdif registers are backward to va9638b version.
  [3]:  spi1_chicken  1'h1 indicate that all registers are back to va9638b version.
0: spi1 registers are in new version.
1: spi1 registers are backward to va9638b version.
  [2]:  hds_chicken  1'h1 indicate that all registers are back to va9638b version.
0: hds registers are in new version.
1: hds registers are backward to va9638b version.
  [1]:  i2s1_chicken  1'h1 indicate that all registers are back to va9638b version.
0: i2s1 registers are in new version.
1: i2s1 registers are backward to va9638b version.
  [0]:  i2s0_chicken  1'h1 indicate that all registers are back to va9638b version.
0: i2s0 related registers are in new version.
1: i2s0 related registers are backward to va9638b version.
*/


#endif

/**********************************************
 * SPI Register Definitions
 **********************************************/
#if SPI_BASE
#define VSPI_CTRLR0                      (SPI_BASE + 0x00)
#define VSPI_CTRLR1                      (SPI_BASE + 0x04)
#define VSPI_START                      (SPI_BASE + 0x08)
#define VSPI_CMD_ADDR                    (SPI_BASE + 0x0c)
#define VSPI_LENGTH                     (SPI_BASE + 0x10)
#define VSPI_RD_BUF                       (SPI_BASE + 0x14)
#define VSPI_WR_BUF                      (SPI_BASE + 0x18)
#define VSPI_CMDS                          (SPI_BASE + 0x1c)
#define VSPI_PROG_CYCL                  (SPI_BASE + 0x20)
#define VSPI_STATUS                     (SPI_BASE + 0x24)
#define VSPI_PAD_CTRL                   (SPI_BASE + 0x28)
#define VSPI_ENCRYPT_EN                 (SPI_BASE + 0x2C)
#define VSPI_ENCRYPT_KEY                (SPI_BASE + 0x30)

/* VA9638P SPI new reg */
#define VSPI_RO_CNT                        (SPI_BASE + 0x34)
#define VSPI_PROG_QUIT                    (SPI_BASE + 0x38)
#define VSPI_CACH_CTRL                    (SPI_BASE + 0x40)
#define VSPI_CACH_CMD                    (SPI_BASE + 0x44)

#define VSPI_SPI_CACHBT_ACC_CNT         (SPI_BASE + 0x98)
#define VSPI_SPI_CACHBT_LOD_CNT         (SPI_BASE + 0x9C)

#define SPI_ENCRYPT_KEY2                (SPI_BASE + 0x48)
/*
SPI_BT_BASE
[31:24]: reserved
[23:0]: Base address of MCUBT code space in flash. Note that the base address shall be 64K Byte aligned. This register can only be changed when below register bt_base_lock is 0.
*/
#define VSPI_BT_BASE                    (SPI_BASE + 0x80)

/*
SPI_BTBS_LOCK
[31:1]: reserved
[0]: Lock bit for SPI_BT_BASE. Only can change SPI_BT_BASE when this bit is 0.
*/
#define VSPI_BTBS_LOCK                (SPI_BASE + 0x84)

/*
SPI_TP_BASE
[31:24]: reserved
[23:0]: Base address of MCUTOP code space in flash. Note that the base address shall be 64K Byte aligned. This register can only be changed when below register bt_base_lock is 0.
*/
#define VSPI_TP_BASE                (SPI_BASE + 0x88)

/*
SPI_TPBS_LOCK
[31:1]: reserved
[0]: Lock bit for SPI_TP_BASE. Only can change SPI_TP_BASE when this bit is 0.
*/
#define VSPI_TPBS_LOCK                (SPI_BASE + 0x8C)

/*
SPI_DSP_BASE
[31:24]: reserved
[23:0]: Base address of DSP code space in flash. Note that the base address shall be 64K Byte aligned. This register can only be changed when below register dsp_base_lock is 0.
*/
#define VSPI_DSP_BASE                (SPI_BASE + 0x90)

/*
SPI_DSPBS_LOCK
[31:1]: reserved
[0]: Lock bit for SPI_DSP_BASE. Only can change SPI_DSP_BASE when this bit is 0.
*/
#define VSPI_DSPBS_LOCK                (SPI_BASE + 0x94)

#endif

/**********************************************
 * SPI2 Register Definitions
 **********************************************/ 
#if SPI2_BASE
/*
SPI_CTRL_1: 
[31:6]: 
[5]: SPI clock frequency selection:
1'b0: clk_fast/4
1'b1: clk_fast/2
clk_fast may be 153M or 128M
[4]: Write protection for in-package spi flash. Low active. Note that spi module has no need to care about external spi flash's WP.
[3]: 1'b0: In-package spi flash
1'b1: External spi flash
[2]: 1'b0: use APB to read or write data buffer in SPI. If the data length is no more than 4 bytes, this APB mode can be used.
1'b1: use DMA mode for data transferring between SRAM and SPI. DMA mode can be used for any data length.
[1]: read/write operation for flash
0: read
1: write/program/erase
[0]: spi enable
0: disable
1: enable
*/
#define REG_SPI2_SPI2_CTRL_1                                 (SPI2_BASE + 0x0)
#define REG_SPI2_SPI2_CTRL_1_spi_freq_sel                    (5)
#define REG_SPI2_SPI2_CTRL_1_wr_prot                         (4)
#define REG_SPI2_SPI2_CTRL_1_flash_sel                       (3)
#define REG_SPI2_SPI2_CTRL_1_dma_mode                        (2)
#define REG_SPI2_SPI2_CTRL_1_write                           (1)
#define REG_SPI2_SPI2_CTRL_1_spi_enable                      (0)

/*
SPI_CTRL_2: 
[31:16]: 
[15:12]: clock cycles from CSN rising edge to next falling edge.
[11:8]: clock cycles from SCLK inactive to CSN rising edge.
[7:4]: clock cycles from CSN falling edge to SCLK active.
[3]: simplified slave mode select.
[2]: In this mode, both SI and SO are parallel used for data output when write or for data input when read. Otherwise, the data is shifted out on SO when write and shifted in on SI when read.
[1]: In this mode, both SI and SO are parallel used for shifting out address. Otherwise, the address is shift out only on SO.
[0]: 0: spi mode 0
1: spi mode 3
*/
#define REG_SPI2_SPI2_CTRL_2                                 (SPI2_BASE + 0x4)
#define REG_SPI2_SPI2_CTRL_2_smp_slave                       (3)
#define REG_SPI2_SPI2_CTRL_2_dual_data_mode                  (2)
#define REG_SPI2_SPI2_CTRL_2_dual_addr_mode                  (1)
#define REG_SPI2_SPI2_CTRL_2_spi_mode                        (0)

/*
SPI_TRANS_START: 
[31:1]: 
[0]: Write 1 to this bit to lauch an operation, like read, write, erase, etc.
*/
#define REG_SPI2_SPI2_TRANS_START                            (SPI2_BASE + 0x8)
#define REG_SPI2_SPI2_TRANS_START_trans_start                (0)


/*
SPI_CMD_ADDR: Address of the flash for this operation.
[31:8]: Address of the flash for this operation.
[7:0]: command for this operation
*/
#define REG_SPI2_SPI2_CMD_ADDR                               (SPI2_BASE + 0xc)

/*
SPI_LENGTH: 
[31:20]: 
[19:4]: How many bytes of data in this operation.
[3:2]: Dummy clock cycles(dummy_bytes*8) to wait after address transmitted.
[1:0]: Address length in register SPI_ADDRESS.
*/
#define REG_SPI2_SPI2_LENGTH                                 (SPI2_BASE + 0x10)

/*
SPI_DATA_BUF_RD: Data buffer for spi flash read(Rx).
[31:0]: Data buffer for spi flash read(Rx).
*/
#define REG_SPI2_SPI2_DATA_BUF_RD                            (SPI2_BASE + 0x14)

/*
SPI_DATA_BUF_WR: Data buffer for spi flash write(Tx).
[31:0]: Data buffer for spi flash write(Tx).
*/
#define REG_SPI2_SPI2_DATA_BUF_WR                            (SPI2_BASE + 0x18)
/*
SPI_STATUS: 
[31:5]: 
[4]: Write(Tx) data buffer underflow. Write '1' to clear.
[3]: Write(Tx) data buffer is empty.
[2]: Read(Rx) data buffer overflow. Write '1' to clear.
[1]: Read(Rx) data buffer is full.
[0]: Current transaction finished.
*/
#define REG_SPI2_SPI2_STATUS                                 (SPI2_BASE + 0x24)
#define REG_SPI2_SPI2_STATUS_underflow                       (4)
#define REG_SPI2_SPI2_STATUS_empty                           (3)
#define REG_SPI2_SPI2_STATUS_overflow                        (2)
#define REG_SPI2_SPI2_STATUS_full                            (1)
#define REG_SPI2_SPI2_STATUS_trans_done                      (0)

/*
SPI_PAD_CTRL: 
[31:3]: 
[2]: Use GPIO[12:9] to connect with in-package flash.
GPIO[9]: SPI_CSN
GPIO[10]: SPI_SI
GPIO[11]: SPI_SO
GPIO[12]: SPI_SCLK
[1]: Drive strength of SO pad, which connect with in-package flash's SI.
[0]: Drive strength of SCLK pad, which connect with in-package flash's SCLK.
*/
#define REG_SPI2_SPI2_PAD_CTRL                               (SPI2_BASE + 0x20)
#define REG_SPI2_SPI2_PAD_CTRL_e2i_pad_sel                   (2)
#define REG_SPI2_SPI2_PAD_CTRL_i_so_ds                       (1)
#define REG_SPI2_SPI2_PAD_CTRL_i_sclk_ds                     (0)

#endif

/**********************************************
 * ASRC Register Definitions
 **********************************************/
#if ASRC_BASE
#define REG_ASRC_ASRC_CTRL                    (ASRC_BASE + 0x00)
#define REG_ASRC_ASRC_CTRL_ASRC_CLK_SEL                   (13)
#define REG_ASRC_ASRC_CTRL_DWIDTH                         (11)
#define REG_ASRC_ASRC_CTRL_SP_DATA_OUT_SEL                (9)
#define REG_ASRC_ASRC_CTRL_SP_CLK_OUT_SEL                 (7)
#define REG_ASRC_ASRC_CTRL_SP_DATA_IN_SEL                 (5)
#define REG_ASRC_ASRC_CTRL_SP_CLK_IN_SEL                  (3)
#define REG_ASRC_ASRC_CTRL_LOCKED_BYPASS                  (2)
#define REG_ASRC_ASRC_CTRL_RESAMPLE_DISABLE               (1)
#define REG_ASRC_ASRC_CTRL_SOFTRST                        (0)
#define REG_ASRC_ASRC_CTRL_ASRC_CLK_SEL_bit                     (3)
#define REG_ASRC_ASRC_CTRL_DWIDTH_bit                           (2)
#define REG_ASRC_ASRC_CTRL_SP_DATA_OUT_SEL_bit                  (2)
#define REG_ASRC_ASRC_CTRL_SP_CLK_OUT_SEL_bit                   (2)
#define REG_ASRC_ASRC_CTRL_SP_DATA_IN_SEL_bit                   (2)
#define REG_ASRC_ASRC_CTRL_SP_CLK_IN_SEL_bit                    (2)
#define REG_ASRC_ASRC_CTRL_LOCKED_BYPASS_bit                    (1)
#define REG_ASRC_ASRC_CTRL_RESAMPLE_DISABLE_bit                 (1)
#define REG_ASRC_ASRC_CTRL_SOFTRST_bit                          (1)

#define REG_ASRC_AHB_CTRL_CFG                    (ASRC_BASE + 0x04)
#define REG_ASRC_AHB_CTRL_CFG_READ_MODE_EN            (1)
#define REG_ASRC_AHB_CTRL_CFG_WRITE_MODE_EN            (0)
#define REG_ASRC_AHB_CTRL_CFG_READ_MODE_EN_bit            (1)
#define REG_ASRC_AHB_CTRL_CFG_WRITE_MODE_EN_bit           (1)

#define REG_ASRC_SCDIV0_CFG            (ASRC_BASE + 0x10)
#define REG_ASRC_SCDIV0_CFG_CNT0_STEP               (20)
#define REG_ASRC_SCDIV0_CFG_CNT0_CEILING            (0)
#define REG_ASRC_SCDIV0_CFG_CNT0_STEP_bit               (12)
#define REG_ASRC_SCDIV0_CFG_CNT0_CEILING_bit            (18)

#define REG_ASRC_SCDIV1_CFG             (ASRC_BASE + 0x14)
#define REG_ASRC_SCDIV1_CFG_CNT1_STEP            (20)
#define REG_ASRC_SCDIV1_CFG_CNT1_CEILING            (0)
#define REG_ASRC_SCDIV1_CFG_CNT1_STEP_bit            (12)
#define REG_ASRC_SCDIV1_CFG_CNT1_CEILING_bit            (18)


#define REG_ASRC_ASRC_BUF_DEPTH               (ASRC_BASE + 0x20)
#define REG_ASRC_HREADY_TIMEOUT_LIMIT         (ASRC_BASE + 0x24)
#define REG_ASRC_RD_BUF_DISTANCE              (ASRC_BASE + 0x28)
#define REG_ASRC_WR_BUF_DISTANCE              (ASRC_BASE + 0x30)
#define REG_ASRC_RD_BUF_RPTR                  (ASRC_BASE + 0x38)
#define REG_ASRC_RD_BUF_WPTR                  (ASRC_BASE + 0x3c)
#define REG_ASRC_WR_BUF_WPTR                  (ASRC_BASE + 0x40)
#define REG_ASRC_WR_BUF_RPTR                  (ASRC_BASE + 0x44)
#define REG_ASRC_WR_BUF_ADDR_START            (ASRC_BASE + 0x48)
#define REG_ASRC_RD_BUF_ADDR_START            (ASRC_BASE + 0x4c)

#define REG_ASRC_INTR_ASRC                    (ASRC_BASE + 0x50)
#define REG_ASRC_INTR_ASRC_WR_DISTANCE_INT            (6)
#define REG_ASRC_INTR_ASRC_WR_HREADY_TIMEOUT          (5)
#define REG_ASRC_INTR_ASRC_WR_BUF_ERROR               (4)
#define REG_ASRC_INTR_ASRC_RD_DISTANCE_INT            (2)
#define REG_ASRC_INTR_ASRC_RD_HREADY_TIMEOUT          (1)
#define REG_ASRC_INTR_ASRC_RD_BUF_ERROR               (0)
#define REG_ASRC_INTR_ASRC_WR_DISTANCE_INT_bit        (1)
#define REG_ASRC_INTR_ASRC_WR_HREADY_TIMEOUT_bit      (1)
#define REG_ASRC_INTR_ASRC_WR_BUF_ERROR_bit           (1)
#define REG_ASRC_INTR_ASRC_RD_DISTANCE_INT_bit        (1)
#define REG_ASRC_INTR_ASRC_RD_HREADY_TIMEOUT_bit      (1)
#define REG_ASRC_INTR_ASRC_RD_BUF_ERROR_bit           (1)


#define REG_ASRC_INTR_ASRC_EN (ASRC_BASE + 0x54)
#define REG_ASRC_INTR_ASRC_EN_INTR_EN                   (0)
#define REG_ASRC_INTR_ASRC_EN_INTR_EN_bit                   (8)


#define REG_ASRC_ASRC_LOCK_0    (ASRC_BASE + 0x60)
#define REG_ASRC_ASRC_LOCK_0_LOCK_GAIN_LOG                   (24)
#define REG_ASRC_ASRC_LOCK_0_LOCK_DEADZONE                   (16)
#define REG_ASRC_ASRC_LOCK_0_UNLOCK_THRESH                   (8)
#define REG_ASRC_ASRC_LOCK_0_LOCK_THRESH                     (0)
#define REG_ASRC_ASRC_LOCK_0_LOCK_GAIN_LOG_bit                   (8)
#define REG_ASRC_ASRC_LOCK_0_LOCK_DEADZONE_bit                   (8)
#define REG_ASRC_ASRC_LOCK_0_UNLOCK_THRESH_bit                   (8)
#define REG_ASRC_ASRC_LOCK_0_LOCK_THRESH_bit                     (8)


#define REG_ASRC_ASRC_LOCK_1    (ASRC_BASE + 0x64)
#define REG_ASRC_ASRC_LOCK_1_LOCK_SLEW                        (24)
#define REG_ASRC_ASRC_LOCK_1_LOCK_SLEW_DIVIDER                (16)
#define REG_ASRC_ASRC_LOCK_1_VARI_DEADZONE                    (8)
#define REG_ASRC_ASRC_LOCK_1_VARI_GAIN_LOG                    (0)
#define REG_ASRC_ASRC_LOCK_1_LOCK_SLEW_bit                        (4)
#define REG_ASRC_ASRC_LOCK_1_LOCK_SLEW_DIVIDER_bit                (8)
#define REG_ASRC_ASRC_LOCK_1_VARI_DEADZONE_bit                    (8)
#define REG_ASRC_ASRC_LOCK_1_VARI_GAIN_LOG_bit                    (8)


#define REG_ASRC_ASRC_FIFO_SET_POINT    (ASRC_BASE + 0x68)


#define REG_ASRC_ASRC_MAX_COUNT    (ASRC_BASE + 0x6c)

#define REG_ASRC_ASRC_CONV_FACTOR    (ASRC_BASE + 0x70)

/*
ASRC0_MCLK_DIVIDER[31:6]: reserved    26'h0
reserved
[5:0]: mclk_divider    6'h00
divide factor for period_counts
*/
#define REG_ASRC_ASRC_MCLK_DIVIDER    (ASRC_BASE + 0x74)

/*
ASRC0_SIG_COUNT[31:16]: reserved    16'h0

[15:0]: sig_count    16'h3A98
ASRC0 clock number for no audio signal detection 
*/
#define REG_ASRC_ASRC_SIG_COUNT    (ASRC_BASE + 0x78)

/*
ASRC0_RATIO_NUM[31:4]: reserved    28'h0

[3:0]: calc_ratio_num    4'h3
ASRC0 caculation ratio number for modified locked mode
*/
#define REG_ASRC_ASRC_RATIO_NUM    (ASRC_BASE + 0x7c)

/*
ASRC0_STATUS[31:2]: reserved    30'h0
reserved
[1]: fifo_overflow    
This active-High output indicates that the level
of the input FIFO is beyond the safe operating
range. Therefore, loss of input samples is likely.
[0]: locked    
This active-High output indicates that ratio
tracking is in a locked state, i.e., the input FIFO
level is within the prescribed thresholds and
stable.
*/
#define REG_ASRC_ASRC_STATUS    (ASRC_BASE + 0x80)
#define REG_ASRC_ASRC_STATUS_FIFO_OVERFLOW      (1)
#define REG_ASRC_ASRC_STATUS_LOCKED             (0)
#define REG_ASRC_ASRC_STATUS_FIFO_OVERFLOW_bit       (1)
#define REG_ASRC_ASRC_STATUS_LOCKED_bit              (1)

#endif

/**********************************************
 * HD Register Definitions
 **********************************************/
#if HD_CPORT_BASE
/*
HD_CTRL: 
hd_cport control register
[31:11]: reserved
[10]: dma mode data transmission 0:disalbe 1:enable
[9]: data size
0:high 24b is valid in 32b dma data
1:2 valid 16b data in 32b dma data
[8:7]: reserved
[6]: clock gate, high active
[5]: hd_cport & i2c select 1b0 : i2c 1b1 : hd_cport
[4]: DMA to HD serial 1b0 : disable 1b1 : enable
[3]: HD serial to DMA 1b0 : disable 1b1 : enable
[2]: Write command control signal 1b0 : disable 1b1 : enable
[1]: Read command control signal 1b0 : disable 1b1 : enable
[0]: HD serial enable signal. 1b0: disable 1b1: enable
*/
#define REG_HD_CTRL                     (HD_CPORT_BASE + 0x00)
#define RBIT_HD_CTRL_HD_I2C_SEL         (5)
#define REG_HD_CMD_WDATA                (HD_CPORT_BASE + 0x04)
#define REG_HD_CMD_RDATA                (HD_CPORT_BASE + 0x08)
#define REG_HD_STATUS                   (HD_CPORT_BASE + 0x0C)
#define REG_HD_CLK_DIV                  (HD_CPORT_BASE + 0x10)
#define REG_HD_ADDRESS                  (HD_CPORT_BASE + 0x14)
#define REG_HD_UM_CTRL                  (HD_CPORT_BASE + 0x18)

#endif

/**********************************************
 * USB Register Definitions
 **********************************************/
#if UOTG_BASE
//Common USB registers
#define REG_UOTG_FADDR              (UOTG_BASE+0x0000)   
#define REG_UOTG_POWER              (UOTG_BASE+0x0004)   
#define REG_UOTG_INTRTX             (UOTG_BASE+0x0008)   
#define REG_UOTG_INTRRX             (UOTG_BASE+0x000C)   
#define REG_UOTG_INTRTXE            (UOTG_BASE+0x0010)   
#define REG_UOTG_INTRRXE            (UOTG_BASE+0x0014)   
#define REG_UOTG_INTRUSB            (UOTG_BASE+0x0018)   
#define REG_UOTG_INTRUSBE           (UOTG_BASE+0x001C)   
#define REG_UOTG_FRAME              (UOTG_BASE+0x0020)   
#define REG_UOTG_INDEX              (UOTG_BASE+0x0024)   
#define REG_UOTG_TESTMODE           (UOTG_BASE+0x0028)   
#define REG_UOTG_DMAIF_CTRL         (UOTG_BASE+0x002C) 
//Indexed Endpoint Control/Status registers
#define REG_UOTG_EP_TXMAXP          (UOTG_BASE+0x0040)   
#define REG_UOTG_EP_TXCSR           (UOTG_BASE+0x0044)   
#define REG_UOTG_EP_RXMAXP          (UOTG_BASE+0x0048)   
#define REG_UOTG_EP_RXCSR           (UOTG_BASE+0x004C)   
#define REG_UOTG_EP_RXCOUNT         (UOTG_BASE+0x0050)   
#define REG_UOTG_EP_TXTYPE          (UOTG_BASE+0x0054)      // Host mode only
#define REG_UOTG_EP_TXINTV          (UOTG_BASE+0x0058)      // Host mode only
#define REG_UOTG_EP_RXTYPE          (UOTG_BASE+0x005C)      // Host mode only
#define REG_UOTG_EP_RXINTV          (UOTG_BASE+0x0060)      // Host mode only
#define REG_UOTG_EP_RXPKTNUM        (UOTG_BASE+0x0068)      // Host mode only
#define REG_UOTG_EP_PKTNUM          (UOTG_BASE+0x006C)      
#define REG_UOTG_EP_TXBYTECNT         (UOTG_BASE+0x0070)
//Additional Control registers  
#define REG_UOTG_DEVCTL              (UOTG_BASE+0x00A0)   
//Non-Indexed Endpoint Control/Status registers
#define REG_UOTG_EP0_CSR             (UOTG_BASE+0x0104)   
#define REG_UOTG_EP0_COUNT           (UOTG_BASE+0x0110)   
#define REG_UOTG_EP0_TXCNT           (UOTG_BASE+0x0114)   
#define REG_UOTG_EP0_NAKL            (UOTG_BASE+0x0118)      // Host Mode only
#define REG_UOTG_EP0_CFGDATA         (UOTG_BASE+0x0124) 
#define REG_UOTG_DMAIF_CTRL1          (UOTG_BASE+0x0130)
#define REG_UOTG_TXMAX_P1            (UOTG_BASE+0x0140)
#define REG_UOTG_TXCS_R1             (UOTG_BASE+0x0144)
#define REG_UOTG_RXMAX_P1            (UOTG_BASE+0x0148)
#define REG_UOTG_RXCS_R1             (UOTG_BASE+0x014C)
#define REG_UOTG_RXBYTECNT1          (UOTG_BASE+0x0150)
#define REG_UOTG_TXTYPE1             (UOTG_BASE+0x0154)   
#define REG_UOTG_TXINTERVAL1         (UOTG_BASE+0x0158) 
#define REG_UOTG_RXTYPE1             (UOTG_BASE+0x015C)   
#define REG_UOTG_RXINTERVAL1         (UOTG_BASE+0x0160)
#define REG_UOTG_ARXPKTNUM1          (UOTG_BASE+0x0068)
#define REG_UOTG_PKTNUM1             (UOTG_BASE+0x006C)
#define REG_UOTG_TXBYTECNT1          (UOTG_BASE+0x0170)

#define REG_UOTG_TXMAX_P2            (UOTG_BASE+0x0180)
#define REG_UOTG_TXCS_R2             (UOTG_BASE+0x0184)
#define REG_UOTG_RXMAX_P2            (UOTG_BASE+0x0188)
#define REG_UOTG_RXCS_R2             (UOTG_BASE+0x018C)
#define REG_UOTG_RXBYTECNT2          (UOTG_BASE+0x0190)
#define REG_UOTG_TXTYPE2             (UOTG_BASE+0x0194)   
#define REG_UOTG_TXINTERVAL2         (UOTG_BASE+0x0198) 
#define REG_UOTG_RXTYPE2             (UOTG_BASE+0x019C)   
#define REG_UOTG_RXINTERVAL2         (UOTG_BASE+0x01A0)
#define REG_UOTG_ARXPKTNUM2          (UOTG_BASE+0x00A8)
#define REG_UOTG_PKTNUM2             (UOTG_BASE+0x00AC)
#define REG_UOTG_TXBYTECNT2          (UOTG_BASE+0x01B0)
//User Defined Registers  
#define REG_UOTG_REG_BYTEMASK            (UOTG_BASE+0x0380)
#define REG_UOTG_CHARGER_CNT_T           (UOTG_BASE+0x0384)   
#define REG_UOTG_UOTGC_SRC_PND           (UOTG_BASE+0x0388)   
#define REG_UOTG_UOTGC_INTMASK           (UOTG_BASE+0x038C)   
#define REG_UOTG_UOTGC_SET_MASK          (UOTG_BASE+0x0390)   
#define REG_UOTG_UOTGC_UNSET_MASK        (UOTG_BASE+0x0394)   
#define REG_UOTG_UOTGPHY_STRAP           (UOTG_BASE+0x0400)
#define REG_UOTG_UOTGC_SIM_MODE          (UOTG_BASE+0x0404)
#define REG_UOTG_UOTGC_BUSRST_EN         (UOTG_BASE+0x0408)
#define REG_UOTG_UOTGC_CLK_GATE          (UOTG_BASE+0x040C)
#define REG_UOTG_UOTGC_RSTN                   (UOTG_BASE+0x0414)

#endif

/**********************************************
 * PWM Register Definitions
 **********************************************/
#if PWM_BASE
#define REG_PWM_PWM_CTRL0                      (PWM_BASE + 0x00)
#define REG_PWM_PWM_CTRL0_WSSET                   (7)
#define REG_PWM_PWM_CTRL0_DWIDTH                  (5)
#define REG_PWM_PWM_CTRL0_STARTSIGNAL             (4)
#define REG_PWM_PWM_CTRL0_CLKSELECT               (3)
#define REG_PWM_PWM_CTRL0_MODESELECT              (1)
#define REG_PWM_PWM_CTRL0_PWMEN                   (0)
#define REG_PWM_PWM_CTRL0_WSSET_bit                   (1)
#define REG_PWM_PWM_CTRL0_DWIDTH_bit                  (2)
#define REG_PWM_PWM_CTRL0_STARTSIGNAL_bit             (1)
#define REG_PWM_PWM_CTRL0_CLKSELECT_bit               (1)
#define REG_PWM_PWM_CTRL0_MODESELECT_bit              (2)
#define REG_PWM_PWM_CTRL0_PWMEN_bit                   (1)

#define REG_PWM_NORMALMODE_CTRL0                (PWM_BASE + 0x04)
#define REG_PWM_NORMALMODE_CTRL0_QUANTITY                   (0)
#define REG_PWM_NORMALMODE_CTRL0_QUANTITY_bit                   (7)

#define REG_PWM_BLCMODE_CTRL0                   (PWM_BASE + 0x08)
#define REG_PWM_BLCMODE_CTRL0_BLCLOOP                   (0)
#define REG_PWM_BLCMODE_CTRL0_BLCLOOP_bit                   (16)

#define REG_PWM_PWM_CTRL1                      (PWM_BASE + 0x10)
#define REG_PWM_PWM_CTRL1_WSSET                   (7)
#define REG_PWM_PWM_CTRL1_DWIDTH                  (5)
#define REG_PWM_PWM_CTRL1_STARTSIGNAL             (4)
#define REG_PWM_PWM_CTRL1_CLKSELECT               (3)
#define REG_PWM_PWM_CTRL1_MODESELECT              (1)
#define REG_PWM_PWM_CTRL1_PWMEN                   (0)
#define REG_PWM_PWM_CTRL1_WSSET_bit                   (1)
#define REG_PWM_PWM_CTRL1_DWIDTH_bit                  (2)
#define REG_PWM_PWM_CTRL1_STARTSIGNAL_bit             (1)
#define REG_PWM_PWM_CTRL1_CLKSELECT_bit               (1)
#define REG_PWM_PWM_CTRL1_MODESELECT_bit              (2)
#define REG_PWM_PWM_CTRL1_PWMEN_bit                   (1)

#define REG_PWM_NORMALMODE_CTRL1                (PWM_BASE + 0x14)
#define REG_PWM_NORMALMODE_CTRL1_QUANTITY                   (0)
#define REG_PWM_NORMALMODE_CTRL1_QUANTITY_bit                   (7)

#define REG_PWM_BLCMODE_CTRL1                   (PWM_BASE + 0x18)
#define REG_PWM_BLCMODE_CTRL1_BLCLOOP                   (0)
#define REG_PWM_BLCMODE_CTRL1_BLCLOOP_bit                   (16)

#endif

/**********************************************
 * MSG Register Definitions
 **********************************************/
#if MSG_BOX_BASE
#define REG_MSG_CM3MBX_CTRL0    (MSG_BOX_BASE + 0x00)
#define REG_MSG_CM3MBX_CTRL1    (MSG_BOX_BASE + 0x04)
#define REG_MSG_CM3MBX_CTRL2    (MSG_BOX_BASE + 0x08)
#define REG_MSG_CM3MBX_DSP_STAT (MSG_BOX_BASE + 0x0c)
#define REG_MSG_DSP_CACH_UPD    (MSG_BOX_BASE + 0x10)
#define REG_MSG_CM3TOP_MSG0     (MSG_BOX_BASE + 0x20)
#define REG_MSG_CM3TOP_MSG1     (MSG_BOX_BASE + 0x24)
#define REG_MSG_CM3TOP_MSG2     (MSG_BOX_BASE + 0x28)
#define REG_MSG_CM3BT_MSG0      (MSG_BOX_BASE + 0x30)
#define REG_MSG_CM3BT_MSG1      (MSG_BOX_BASE + 0x34)
#define REG_MSG_CM3BT_MSG2      (MSG_BOX_BASE + 0x38)
#define REG_MSG_DSP_MSG0        (MSG_BOX_BASE + 0x40)
#define REG_MSG_DSP_MSG1        (MSG_BOX_BASE + 0x44)
#define REG_MSG_DSP_MSG2        (MSG_BOX_BASE + 0x48)

/*bt preload register*/
#define REG_BTCACHE_UPD_CTRL MSG_BOX_BASE + 0x14
#define REG_BTCACHE_UPD_CTRL_BTCACHE_PAGE_ACK                  ((((REG_BTCACHE_UPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((5) << 2))
#define REG_BTCACHE_UPD_CTRL_BTCACHE_UP_RDY                    ((((REG_BTCACHE_UPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((4) << 2))
#define REG_BTCACHE_UPD_CTRL_BTCACHE_PAGE_LDEN                 ((((REG_BTCACHE_UPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((1) << 2))
#define REG_BTCACHE_UPD_CTRL_BTCACHE_UP_RDQ                    ((((REG_BTCACHE_UPD_CTRL) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((0) << 2))

#define REG_BTCACHE_PAGE_ADDR MSG_BOX_BASE + 0x18 
/*end of preload register*/

#endif


/**********************************************
 * R2Z Register Definitions
 **********************************************/
#if R2Z_BASE
#define REG_R2Z_R2Z_CTRL                      (R2Z_BASE + 0x00)
#define REG_R2Z_R2Z_CTRL_DWIDTH                  (3)
#define REG_R2Z_R2Z_CTRL_IDLE_LEVEL              (2)
#define REG_R2Z_R2Z_CTRL_REQUEST_SEL             (1)
#define REG_R2Z_R2Z_CTRL_R2Z_EN                  (0)
#define REG_R2Z_R2Z_CTRL_DWIDTH_bit                  (1)
#define REG_R2Z_R2Z_CTRL_IDLE_LEVEL_bit              (1)
#define REG_R2Z_R2Z_CTRL_REQUEST_SEL_bit             (1)
#define REG_R2Z_R2Z_CTRL_R2Z_EN_bit                  (1)

#define REG_R2Z_MODE0_RSTCODE_CTRL            (R2Z_BASE + 0x04)
#define REG_R2Z_CTRL_DATA_CNT_LIM                  (1)
#define REG_R2Z_CTRL_TRST_CODE_EN                  (0)
#define REG_R2Z_CTRL_DATA_CNT_LIM_bit                  (13)
#define REG_R2Z_CTRL_TRST_CODE_EN_bit                  (1)

#define REG_R2Z_MODE0_T0L_SET                 (R2Z_BASE + 0x10)
#define REG_R2Z_MODE0_T0L_SET_T0L_CYCLE_LIM         (0)
#define REG_R2Z_MODE0_T0L_SET_T0L_CYCLE_LIM_bit         (12)

#define REG_R2Z_MODE0_T0H_SET                 (R2Z_BASE + 0x14)
#define REG_R2Z_MODE0_T0H_SET_T0H_CYCLE_LIM         (0)
#define REG_R2Z_MODE0_T0H_SET_T0H_CYCLE_LIM_bit         (12)

#define REG_R2Z_MODE0_T1L_SET                 (R2Z_BASE + 0x18)
#define REG_R2Z_MODE0_T1L_SET_T1L_CYCLE_LIM         (0)
#define REG_R2Z_MODE0_T1L_SET_T1L_CYCLE_LIM_bit         (12)

#define REG_R2Z_MODE0_T1H_SET                 (R2Z_BASE + 0x1C)
#define REG_R2Z_MODE0_T1H_SET_T1H_CYCLE_LIM         (0)
#define REG_R2Z_MODE0_T1H_SET_T1H_CYCLE_LIM_bit         (12)

#define REG_R2Z_MODE0_TRST_SET                (R2Z_BASE + 0x20)
#define REG_R2Z_MODE0_TRST_SET_TRST_CYCLE_LIM         (0)
#define REG_R2Z_MODE0_TRST_SET_TRST_CYCLE_LIM_bit         (13)

#define REG_R2Z_R2Z_BUF_DEPTH                 (R2Z_BASE + 0x30)
#define REG_R2Z_R2Z_BUF_DEPTH_BUF_DEPTH         (0)
#define REG_R2Z_R2Z_BUF_DEPTH_BUF_DEPTH_bit         (12)

#define REG_R2Z_BUF_ADDR_START                 (R2Z_BASE + 0x34)
#define REG_R2Z_BUF_RPTR                       (R2Z_BASE + 0x38)
#define REG_R2Z_HREADY_TIMEOUT_LIMIT           (R2Z_BASE + 0x40)

#define REG_R2Z_R2Z_INTR                       (R2Z_BASE + 0x44)
#define REG_R2Z_R2Z_INTR_HREADY_TIMEOUT_INTR         (1)
#define REG_R2Z_R2Z_INTR_BUF_EMPTY_INTR              (0)
#define REG_R2Z_R2Z_INTR_HREADY_TIMEOUT_INTR_bit         (1)
#define REG_R2Z_R2Z_INTR_BUF_EMPTY_INTR_bit              (1)

#define REG_R2Z_R2Z_INTR_EN                       (R2Z_BASE + 0x48)
#define REG_R2Z_R2Z_INTR_EN_INTR_EN                (0)
#define REG_R2Z_R2Z_INTR_EN_INTR_EN_bit                 (2)

#define REG_R2Z_INTR_EMPTY_CLR                       (R2Z_BASE + 0x4C)
#define REG_R2Z_INTR_EMPTY_CLR_INTR_EMPTY_CLR                (0)
#define REG_R2Z_INTR_EMPTY_CLR_INTR_EMPTY_CLR_bit                 (1)

#define REG_R2Z_INTR_TIMEOUT_CLR                       (R2Z_BASE + 0x50)
#define REG_R2Z_INTR_TIMEOUT_CLR_INTR_TIMEOUT_CLR                (0)
#define REG_R2Z_INTR_TIMEOUT_CLR_INTR_TIMEOUT_CLR_bit                 (1)

#endif 

/**********************************************
 * R2Z1 Register Definitions
 **********************************************/
#if R2Z1_BASE
#define REG_R2Z1_R2Z1_CTRL                      (R2Z1_BASE + 0x00)
#define REG_R2Z1_R2Z1_CTRL_DWIDTH                  (3)
#define REG_R2Z1_R2Z1_CTRL_IDLE_LEVEL              (2)
#define REG_R2Z1_R2Z1_CTRL_REQUEST_SEL             (1)
#define REG_R2Z1_R2Z1_CTRL_R2Z_EN                  (0)
#define REG_R2Z1_R2Z1_CTRL_DWIDTH_bit                  (1)
#define REG_R2Z1_R2Z1_CTRL_IDLE_LEVEL_bit              (1)
#define REG_R2Z1_R2Z1_CTRL_REQUEST_SEL_bit             (1)
#define REG_R2Z1_R2Z1_CTRL_R2Z_EN_bit                  (1)

#define REG_R2Z1_MODE0_RSTCODE_CTRL            (R2Z1_BASE + 0x04)
#define REG_R2Z1_CTRL_DATA_CNT_LIM                  (1)
#define REG_R2Z1_CTRL_TRST_CODE_EN                  (0)
#define REG_R2Z1_CTRL_DATA_CNT_LIM_bit                  (13)
#define REG_R2Z1_CTRL_TRST_CODE_EN_bit                  (1)

#define REG_R2Z1_MODE0_T0L_SET                 (R2Z1_BASE + 0x10)
#define REG_R2Z1_MODE0_T0L_SET_T0L_CYCLE_LIM         (0)
#define REG_R2Z1_MODE0_T0L_SET_T0L_CYCLE_LIM_bit         (12)

#define REG_R2Z1_MODE0_T0H_SET                 (R2Z1_BASE + 0x14)
#define REG_R2Z1_MODE0_T0H_SET_T0H_CYCLE_LIM         (0)
#define REG_R2Z1_MODE0_T0H_SET_T0H_CYCLE_LIM_bit         (12)

#define REG_R2Z1_MODE0_T1L_SET                 (R2Z1_BASE + 0x18)
#define REG_R2Z1_MODE0_T1L_SET_T1L_CYCLE_LIM         (0)
#define REG_R2Z1_MODE0_T1L_SET_T1L_CYCLE_LIM_bit         (12)

#define REG_R2Z1_MODE0_T1H_SET                 (R2Z1_BASE + 0x1C)
#define REG_R2Z1_MODE0_T1H_SET_T1H_CYCLE_LIM         (0)
#define REG_R2Z1_MODE0_T1H_SET_T1H_CYCLE_LIM_bit         (12)

#define REG_R2Z1_MODE0_TRST_SET                (R2Z1_BASE + 0x20)
#define REG_R2Z1_MODE0_TRST_SET_TRST_CYCLE_LIM         (0)
#define REG_R2Z1_MODE0_TRST_SET_TRST_CYCLE_LIM_bit         (13)

#define REG_R2Z1_R2Z1_BUF_DEPTH                 (R2Z1_BASE + 0x30)
#define REG_R2Z1_R2Z1_BUF_DEPTH_BUF_DEPTH         (0)
#define REG_R2Z1_R2Z1_BUF_DEPTH_BUF_DEPTH_bit         (12)

#define REG_R2Z1_BUF_ADDR_START                 (R2Z1_BASE + 0x34)
#define REG_R2Z1_BUF_RPTR                       (R2Z1_BASE + 0x38)
#define REG_R2Z1_HREADY_TIMEOUT_LIMIT           (R2Z1_BASE + 0x40)

#define REG_R2Z1_R2Z1_INTR                       (R2Z1_BASE + 0x44)
#define REG_R2Z1_R2Z1_INTR_HREADY_TIMEOUT_INTR         (1)
#define REG_R2Z1_R2Z1_INTR_BUF_EMPTY_INTR              (0)
#define REG_R2Z1_R2Z1_INTR_HREADY_TIMEOUT_INTR_bit         (1)
#define REG_R2Z1_R2Z1_INTR_BUF_EMPTY_INTR_bit              (1)

#define REG_R2Z1_R2Z1_INTR_EN                       (R2Z1_BASE + 0x48)
#define REG_R2Z1_R2Z1_INTR_EN_INTR_EN                (0)
#define REG_R2Z1_R2Z1_INTR_EN_INTR_EN_bit                 (2)

#define REG_R2Z1_INTR_EMPTY_CLR                       (R2Z1_BASE + 0x4C)
#define REG_R2Z1_INTR_EMPTY_CLR_INTR_EMPTY_CLR                (0)
#define REG_R2Z1_INTR_EMPTY_CLR_INTR_EMPTY_CLR_bit                 (1)

#define REG_R2Z1_INTR_TIMEOUT_CLR                       (R2Z1_BASE + 0x50)
#define REG_R2Z1_INTR_TIMEOUT_CLR_INTR_TIMEOUT_CLR                (0)
#define REG_R2Z1_INTR_TIMEOUT_CLR_INTR_TIMEOUT_CLR_bit                 (1)

#endif

/**********************************************
 * TRNG Register Definitions
 **********************************************/
#if TRNG_BASE
#define TRNG_CTRL          (TRNG_BASE + 0x00)
#define TRNG_STAT          (TRNG_BASE + 0x04)
#define TRNG_MODE          (TRNG_BASE + 0x08)
#define TRNG_SMODE         (TRNG_BASE + 0x0c)
#define TRNG_IE            (TRNG_BASE + 0x10)
#define TRNG_ISTAT         (TRNG_BASE + 0x14)
#define TRNG_COREKIT_REL   (TRNG_BASE + 0x18)
#define TRNG_FEATURES      (TRNG_BASE + 0x1c)
#define TRNG_RAND0         (TRNG_BASE + 0x20)
#define TRNG_RAND1         (TRNG_BASE + 0x24)
#define TRNG_RAND3         (TRNG_BASE + 0x2c)
#define TRNG_RAND2         (TRNG_BASE + 0x28)
#define TRNG_RAND5         (TRNG_BASE + 0x34)
#define TRNG_RAND4         (TRNG_BASE + 0x30)
#define TRNG_RAND6         (TRNG_BASE + 0x38)
#define TRNG_RAND7         (TRNG_BASE + 0x3c)
#define TRNG_SEED0         (TRNG_BASE + 0x40)
#define TRNG_SEED1         (TRNG_BASE + 0x44)
#define TRNG_SEED2         (TRNG_BASE + 0x48)
#define TRNG_SEED3         (TRNG_BASE + 0x4c)
#define TRNG_SEED4         (TRNG_BASE + 0x50)
#define TRNG_SEED5         (TRNG_BASE + 0x54)
#define TRNG_SEED6         (TRNG_BASE + 0x58)
#define TRNG_SEED7         (TRNG_BASE + 0x5c)
#define TRNG_AUTO_RQSTS    (TRNG_BASE + 0x60)
#define TRNG_AUTO_AGE      (TRNG_BASE + 0x64)
#define TRNG_BUILD_CONFIG  (TRNG_BASE + 0x68)

#endif

/**********************************************
 * SPDIF Register Definitions
 **********************************************/
#if SPDIF_BASE
/*
SPDIF_CTRL:     
control register of SPDIF
[31]: CLK1X_EN_INV    1'h0
Invert 64*fs clock enable flag 
0:not invert(default) ;
1: invert
[30]: SPDENC_P_GENINT     1'h0
audio parity generate internal 
[29]: SPDENC_IVL0     1'h0
SPDIF encoder left channel invalid 
1: invalid 1: valid(default)
[28]: SPDENC_IVR0     1'h0
SPDIF encoder right channel invalid 
1: invalid 1: valid(default)
[27]: SPDENC_AC3_16     1'h0
audio channel status:
0: line PCM (default),
1: other purposes
[26:24]: SPDENC_CHNNUM     3'h1
(audio channel status, following IEC60958 spec.)
audio channel number
3'b000 1 channels
3'b001 2 channels (default)
others reserved
[23:17]: SPDENC_CC     7'h40
Category Code(audio channel status, following IEC60958 spec.)
[16]: SPDENC_L     1'h0
Generation Level
1'b0 no indication(default) 
[15]: SPDENC_CYP      1'h1
 copyright protection
1'b1  no copyright protection(default) 
[14:12]: SPDENC_LENGTH      3'b011
bits per sample, 
3'b010 20bit 
3'b011 24bits 
[11]: SPDENC_PCM_CH    1'b0
audio format change indicating, eg rate is changing
default 1'b0
[10]: SPDENC_FM_CH     1'b0
video mode change, (1: changing) 
[9]: SPDENC_ELD_UP     1'b0
get updated ELD, indicating video mode changing is finished active high
[8:6]: SPDENC_S_CODE     3'b001
Audio sample rate 
3'b010 32KHz
3'b000 44.1KHz
3'b011 88.2KHz
3'b100 176.4KHz
3'b001 48KHz ( default)
3'b101 96KHz
3'b110 192KHz
[5:1]: reserved    5'h0
[0]: SPDENC_EN       1'h0
 1: enable     0:disable 
*/
#define REG_SPDIF_CTRL    (SPDIF_BASE + 0x00)

/*
SPDIF_CLK_CTRL[31:19]: reserved    13'h0
[18:16]: spdif_samp_rate    3'h1
Sampling rate 
0: 32 KHz
1: 48 KHz
2: 96 KHz
3: 192 KHz
4: 44.1KHz
5: 88.2KHz
6: 176.4KHz
[15:4]: reserved    12'h0
[3]: use_bb_dac_en    1'b0
1: Use the bb_dac_en sigal to sync SPDIF Tx.
0: Do not use bb_dac_en. SPDIF Tx starts right after setting SPDENC_EN  1. 
[2:1]: spdif_data_width            2'b01
Data word width.
00: 16-bit.
01: 24-bit.
[0]: spdif_clk_sel      1'b0
spdif clock source select:
0: from 384MHz clock source
1: from 768MHz clock source
*/
#define REG_SPDIF_CLK_CTRL    (SPDIF_BASE + 0x0C)
     
#endif

#endif/*_VREG38P_DEF_H_*/


