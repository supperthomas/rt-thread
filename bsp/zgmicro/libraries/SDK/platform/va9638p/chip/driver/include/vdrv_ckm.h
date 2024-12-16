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
    CKM

Description:
    CKM(ClocK Manager) driver.

Revision History:
    Date                  Author                   Description
    2010.05.05           chenjingen              initial version
    2012.02.22           jch                     add slow clock related
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VCKM_H_
#define _VCKM_H_
	
#include "vstdlib.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus 
extern "C" {
#endif
	
	/* crystal clock frequency definition */
#define CKM_XCLK_32000HZ 	    		(32000L)
#define CKM_XCLK_26000000HZ 			(26000000L)
#define CKM_XCLK_48000000HZ 			(48000000L)

/* system clock frequency definition */
#define CKM_SYSCLK_24MHZ 				(24000000L)  
#define CKM_SYSCLK_32MHZ 				(32000000L) 
#define CKM_SYSCLK_38MHZ 				(38000000L)  
#define CKM_SYSCLK_48MHZ 				(48000000L)  
#define CKM_SYSCLK_64MHZ 				(64000000L)
#define CKM_SYSCLK_77MHZ                (77000000L)
#define CKM_SYSCLK_96MHZ                (96000000L) 
#define CKM_SYSCLK_128MHZ               (128000000L)
#define CKM_SYSCLK_153MHZ 				(153600000L) // Just for test.
/*******************************************************************
VA9638P crystal clock frequency
	CKM_SYSCLK_24MHZ 	
	CKM_SYSCLK_39MHZ
	CKM_SYSCLK_48MHZ
	CKM_SYSCLK_64MHZ
	CKM_SYSCLK_78MHZ
	CKM_SYSCLK_96MHZ
	CKM_SYSCLK_128MHZ
	CKM_SYSCLK_156MHZ
	CKM_SYSCLK_192MHZ
********************************************************************/
#define CKM_SYSCLK_156MHZ               (156300000L)
#define CKM_SYSCLK_78MHZ                (78150000L)
#define CKM_SYSCLK_39MHZ                (39075000L)
#define CKM_SYSCLK_192MHZ               (192000000L)


#define CKM_SYSCTRL_DSP                 (0x01)
#define CKM_SYSCTRL_BT                  (0x02)
#define CKM_SYSCTRL_TOP                 (0x04)

/* peripheral clock frequency definition */
#define CKM_PERCLK_48MHZ 				(48000000L)
#define CKM_PERCLK_24MHZ 				(24000000L)
#define CKM_TIMER_CLK				  	(1000000L)
#define CKM_LPO_CLK					    (4000L)


/* clock selection */
#define CKM_ARM_CLK_SEL_OSC_PAD 		0 		/* CLK_SWT: from OSC pad */
#define CKM_ARM_CLK_SEL_PLL 			1 		/* CLK_SWT: from PLL1 or DPLL */
#define CKM_PLL_SEL_DPLL 				0 		/* PLL1_CLK_SEL: from DPLL */
#define CKM_PLL_SEL_PLL1 				1 		/* PLL1_CLK_SEL: from PLL1 */
#define CKM_32K_LPO_OUT 				0
#define CKM_32K_XTAL_IN                 1


/* clock selection */
#define CKM_ARM_CLK_SEL_OSC_PAD 		0 		/* CLK_SWT: from OSC pad */
#define CKM_ARM_CLK_SEL_PLL 			1 		/* CLK_SWT: from DPLL */

/* default clock configuration: for normal system frequency */
#if (VDEV_TYPE == TYPE_ASIC)
#define CKM_DEFAULT_SYSCLK_FREQ_HIGH 	CKM_SYSCLK_128MHZ
#define CKM_DEFAULT_APB_DIV 			4
#define CKM_DEFAULT_APB_DIV_HIGH 		2
#else
#define CKM_DEFAULT_APB_DIV 			2
#define CKM_DEFAULT_APB_DIV_HIGH 		2
#define CKM_DEFAULT_SYSCLK_FREQ_HIGH 	CKM_XCLK_48000000HZ
#endif


/* default PLL selection */
#if (VDEV_TYPE == TYPE_ASIC)
#define CKM_DEFAULT_XCLK_FREQ 			CKM_XCLK_26000000HZ
#else
#define CKM_DEFAULT_XCLK_FREQ 			CKM_XCLK_48000000HZ
#endif

/* default clock inverter selection */
#define CKM_DEFAULT_48M_INV_SEL 		0
#define CKM_DEFAULT_24M_INV_SEL 		0
/* default DPLL config */
#define CKM_DEFAULT_DPLL_R11 			0x00800733UL /* [28]: Test_mode_enable, [23:16]: Cap_array_trim_bits, [15:8]: Constant_gm_current_out_scaling, [7:4]: Constant_gm_resistor_scaling, [3:0]: Amp_control_out_scaling */
#if (VDEV_TYPE == TYPE_ASIC)
#define CKM_DEFAULT_DPLL_R15 			0x40415000UL /* [23:22]: 01 - non bypass, 10 - bypass, other bits by default */
#else
#define CKM_DEFAULT_DPLL_R15 			0x40815000UL // bypass DPLL on FPGA as XTAL is 48MHz already.
#endif

#define CKM_DEFAULT_DPLL_R16 			0x12100F00UL // To be determined.

#if (VDEV_TYPE == TYPE_ASIC)
#define CKM_DEFAULT_DPLL_REF_SEL 		0x02         /* 26MHz */
#else
#define CKM_DEFAULT_DPLL_REF_SEL 		0x05         /* 48MHz */
#endif


#define CKM_DSPCLK_192MHZ 				(192000000L)  
#define CKM_DSPCLK_128MHZ 				(128000000L) 
#define CKM_DSPCLK_384MHZ 				(384000000L)  
#define CKM_DSPCLK_256MHZ 				(256000000L)  
#define CKM_DSPCLK_26MHZ 					(26000000L)
#define CKM_DSPCLK_153_6MHZ       (153600000L)

/* 
 * RFFE DPLLH configuration
 * ref_sel[3:0]  Input Frequency
 * 0000          13
 * 0001          19.2
 * 0010          26
 * 0011          38.4
 * 0100          52
 * 0101          48
 * 0110          26
 * 0111          26
 * 1000          7.68
 * 1001          14.44
 * 1010          15.36
 * 1011          16.2
 * 1100          16.8
 * 1101          19.44
 * 1110          19.68
 * 1111          19.88
 */
typedef struct tagVCKM_DPLL_CONFIG {
	UINT32 r11;     /* R11_DCXO1 */
	UINT32 r15;     /* R15_DPLL_PFD */
	UINT32 r16;     /* R16_DPLL_overwrite */
	UINT8 ref_sel;  /* reference frequency selection in R18_DPLL_divider_ration */
}VCKM_DPLL_CONFIG;

/* Clock configuration */
typedef struct tagVCKM_CONFIG {
	UINT32 sys_clock_freq;
	UINT8 apb_divider;
	UINT8 timer_divider;
	UINT8 uart_divider;
	UINT8 clock_32k_src;
	UINT32 sys_clock_freq_high;           /* high system clock frequency, for algorithm working */
	UINT32 xtal_freq;                     /* crystal frequency */
	VCKM_DPLL_CONFIG dpll_cfg;            /* RFFE DPLLH config */
	UINT8 clk48m_inv_sel;                 /* 1: 48MHz clock selects the inverted clkin_48m, 0: 48MHz clock selects the clkin_48m */
	UINT8 clk24m_inv_sel;                 /* 1: 24MHz clock selects the inverted clkin_24m, 0: 24MHz clock selects the clkin_24m */
	UINT8 apb_divider_high;               /* apb clock divider for high system clock frequency, for algorithm working */
}VCKM_CONFIG;

/* Clock frequencies information */
typedef struct tagVCKM_FREQINFO {
	UINT32 sys_clock_freq;
	UINT32 apb_clock_freq;
	UINT32 i2c_clock_freq;
	UINT32 timer_clock_freq;
	UINT32 uart_clock_freq;
	UINT8   pll_stat; 					/* PLL status: 1-working; 0-bypass */
}VCKM_FREQINFO;

void VCKM_Init(UINT32 XtalFreq);
void VCKM_InitWithConfig(void);
void VCKM_SetSysClockFreq(UINT8 IsHighFreq, VCKM_CONFIG *p_Config);
void VCKM_SetSysClockForSleepDeep(void);
void VCKM_IncSysClockFreq(void);
void VCKM_ResSysClockFreq(void);
void VCKM_PwdDPLL(void);
void VCKM_PwonDPLL(void);
void VCKM_BypassPwOnDPLL(void);
BOOL VCKM_SetDPLL(VCKM_DPLL_CONFIG *p_Config);
void VCKM_ArmClockSelect(BOOL HighFreqClk);
void VCKM_Set48MInv(UINT8 Inv);
void VCKM_Set24MInv(UINT8 Inv);
void VCKM_SetDivClkCfg(UINT32 SysFreq);
void VCKM_SetAPBClockDivider(UINT8 Divider);
void VCKM_SetCorprocessor(UINT8 core, UINT8 enable);
void VCKM_SetProtect(UINT16 block_mask);
void VCKM_DSPClockSelect(UINT32 DSPFreqClk);

UINT32 VCKM_GetSysClockFreq(void);
UINT32 VCKM_GetAPBClockFreq(void);
UINT32 VCKM_GetI2CClockFreq(void);
UINT32 VCKM_GetTimerClockFreq(void);
UINT32 VCKM_GetUartClockFreq(void);
UINT32 VCKM_GetSdClockFreq(void);

void VCKM_SPIClockSelect(BOOL switchen,BOOL Sel_128msel);

#ifdef __cplusplus
}
#endif 

#endif  /* _VCKM_H_ */

