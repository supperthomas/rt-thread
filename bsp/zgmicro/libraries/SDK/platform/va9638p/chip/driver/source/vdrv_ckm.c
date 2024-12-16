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
 	2011.03.31           jch                     updated for 9613i
 	2011.05.05           jch                     update VCKM_SetDivClkCfg referring to VA9613i CK_MNG MAS_0.8.doc
 	2012.02.22           jch                     add slow clock related
 	2016.11.04           gzd                    update vckm driver code based on VA9635 CKM Mas
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vstdio.h"
#include "vreg_def.h"
#include "vdrv_ckm.h"
#include "vdrv_spi.h"
#include "vdrv_spiflash.h"

/* clock frequency for fclk timers: Hz */
#define CKM_TMR_CLK_FREQ 					(1000000L)

/* divider definition for powering-on process */
#define CKM_PWRON_APB_DIV 					2

/* high frequency clock divider configurations: [MAS 0.8]
   Reserved	        [31:17]	Reserved	15'h0
   DIV_CLKDLY_EN	[16]	See the Divider Configure Table	1'h0
   Reserved	        [15:13]	Reserved	3'h0
   DIV_BARDLY_REG	[12]	See the Divider Configure Table	1'h0
   Reserved	        [11:9]	Reserved	3'h0
   DIV_DLY_REG	    [8]	    See the Divider Configure Table	1'h0
   DIV_LCNT_REG	    [7:4]	See the Divider Configure Table	4'h3
   DIV_CNT_REG	    [3:0]	See the Divider Configure Table	4'h7
 */
#define GetDivClkCfgRegVal(cnt, lcnt, dly, bardly, clkdly_en) (((cnt)&0x0F) | \
				((UINT32)((lcnt)&0x0F)<<4) | ((UINT32)((dly)&0x01)<<8) | \
				((UINT32)((bardly)&0x01)<<12) | ((UINT32)((clkdly_en)&0x01)<<16))
				

/* DPLL lock status timeout: microsecond*7 */
#define CKM_DPLL_LOCK_TIMEOUT 				(1000*26)  /* for highest frequency, 52MHz: 52/8=6.5 */



/* bits definition: SEL_CLK */
// [2], 24MHz clock selects the inverted clkin_24m
#define RBIT_CKM_SEL_CLK_CLK24M_INV_SEL 	2
// [2], 48MHz clock selects the inverted clkin_48m
#define RBIT_CKM_SEL_CLK_CLK48M_INV_SEL 	3
// [4], CM3 clock selection
#define RBIT_CKM_SEL_CLK_ARM_CLK_SEL		4
// [7:5], reserved
// [8], CM3 clock selection
// [29:9], reserved
// [30], ROM power down when 1, READ ONLY
#define RBIT_CKM_SEL_ROMPG_ACK				30
// [31], The clocks output from RFFE DPLL (clkin_384m, clkin_48m, clkin_24m) work stable when 1, READ ONLY
#define RBIT_CKM_SEL_DPLL_CLK_EN			31


VCKM_CONFIG g_VCKM_Config;
VCKM_FREQINFO g_VCKM_FreqInfo;


/******************************************************************************
Description:
 	Initializes clock setting for powering-on process.

Parameters:
	XtalFreq: crystal frequency.
	PllPwOn: PLL power on flag. TRUE - power on, FALSE - power down
	
Return Value:
	None.
*****************************************************************************/
void VCKM_Init(UINT32 XtalFreq)
{
    VCOMMON_MemSet ( &g_VCKM_FreqInfo, 0, sizeof ( VCKM_FREQINFO ) );
    
    /* mcubt rst magic  */
    WRITE_REG ( REG_CKM_MCUBT_RST_MAGIC, 0x96381234 );
    WRITE_REG ( REG_CKM_CHICKEN, 0 );
	
	/* Configure SPI clock to prevent overclocking */
	VSPI_ClkSelect(4); 
	VCKM_SPIClockSelect( 0, 0);  //Restore default
    
	/* switch system clock to OSC pad clock */
	VCKM_ArmClockSelect(CKM_ARM_CLK_SEL_OSC_PAD);
	
	/*When the SOC clock is 26MHz, SPI selects clk_fast*/
	VSPI_ClkSelect(1);

	g_VCKM_FreqInfo.sys_clock_freq = XtalFreq;
	g_VCKM_FreqInfo.uart_clock_freq = XtalFreq;
	g_VCKM_FreqInfo.timer_clock_freq = CKM_TIMER_CLK;
	g_VCKM_FreqInfo.pll_stat = 0;

	/* set apb divider */
	VCKM_SetAPBClockDivider(CKM_PWRON_APB_DIV);
	
	/* bypass & power down DPLL */
	VCKM_PwdDPLL();

#if (VIMC_TB_SOC_SIM==1)	
	//select 128MHz PLL Clock default
	WRITE_REG(REG_CKM_FASTCLK_SEL,READ_REG(REG_CKM_FASTCLK_SEL)|1); // 128MHz
#else
	//select 96MHz PLL Clock default
	WRITE_REG(REG_CKM_FASTCLK_SEL,READ_REG(REG_CKM_FASTCLK_SEL)|2); 
#endif
}


/******************************************************************************
Description:
 	Initializes clock setting with config value from EEPROM.

Parameters:
	CfgFlag: TRUE - config is success, FALSE - config is failed and default keys in usage.
	
Return Value:
	None.

Remarks:
	This function should be called after config initial.

*****************************************************************************/
void VCKM_InitWithConfig(void)
{
	VCOMMON_MemSet(&g_VCKM_FreqInfo, 0, sizeof(VCKM_FREQINFO));
	VCKM_SetSysClockFreq(0, &g_VCKM_Config);
}


/******************************************************************************
Description:
 	Sets system clock frequency, including AHB & APB clock.

Parameters:
	IsHighFreq - flag of high frequency mode, 1-high frequency, 0-normal frequency
	p_Config - entry to config object
	
Return Value:
	None.

Remarks:
	This routine can only be called after OSInit.

*****************************************************************************/
void VCKM_SetSysClockFreq(UINT8 IsHighFreq, VCKM_CONFIG *p_Config)
{
#if (!defined FPGA_QA_BUILD) 
	UINT32 sys_freq;
	UINT8  apb_div;
	UINT8 pll_is_work = 0;
#endif
	UINT8 dev_comm_cfg, pclk_cfg;
	VCPU_SR_ALLOC();	
#if (!defined FPGA_QA_BUILD) 
	if (IsHighFreq) { /* high system clock frequency setting */
		sys_freq = p_Config->sys_clock_freq_high;
		apb_div = p_Config->apb_divider_high;
	} else { /* normal system clock frequency setting */
		sys_freq = p_Config->sys_clock_freq;
		apb_div = p_Config->apb_divider;
	}
#endif		
	VCPU_CRITICAL_ENTER();

#ifndef FPGA_QA_BUILD	
	/* Configure SPI clock to prevent overclocking */
	VSPI_ClkSelect(4); 
	VCKM_SPIClockSelect( 0, 0);  //Restore default
#endif	/* FPGA_QA_BUILD */
	
	/* switch system clock to OSC pad clock */
	VCKM_ArmClockSelect(CKM_ARM_CLK_SEL_OSC_PAD);
	
	/* set apb divider */
	VCKM_SetAPBClockDivider(CKM_PWRON_APB_DIV);
	
	/* set 48M/24M clock inverter */
	VCKM_Set48MInv(p_Config->clk48m_inv_sel);
	VCKM_Set24MInv(p_Config->clk24m_inv_sel);
	
	VCPU_CRITICAL_EXIT();
	
	/* PLL setting */
#if (defined FPGA_QA_BUILD) 
	g_VCKM_FreqInfo.sys_clock_freq = CKM_DEFAULT_XCLK_FREQ;
	g_VCKM_FreqInfo.uart_clock_freq = CKM_DEFAULT_XCLK_FREQ;
	g_VCKM_FreqInfo.timer_clock_freq = CKM_TIMER_CLK;
    dev_comm_cfg = ( READ_REG ( REG_CKM_PCLK_CFG ) >> 8 ) & 0x07;
    pclk_cfg = READ_REG ( REG_CKM_PCLK_CFG ) & 0x03;
    //  g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / ((READ_REG(REG_CKM_PCLK_CFG) & 0x0f) + 1);
    g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / ( ( dev_comm_cfg + 1 ) * ( pclk_cfg + 1 ) );
	g_VCKM_FreqInfo.i2c_clock_freq = g_VCKM_FreqInfo.apb_clock_freq;
#else 
	if (!g_VCKM_FreqInfo.pll_stat)  /* if DPLL is not working, configure and power on it */
	{
		if (VCKM_SetDPLL(&p_Config->dpll_cfg)) 
		{
			VCKM_SetDivClkCfg(sys_freq);
			g_VCKM_FreqInfo.pll_stat = 1;
			pll_is_work = 1;
		} 
		else //configure DPLL failed.
		{
			VCKM_PwdDPLL();
			g_VCKM_FreqInfo.pll_stat = 0;
			pll_is_work = 0;
		}
	} 
	else 
	{ /* if DPLL is working, set the divider */
		VCKM_SetDivClkCfg(sys_freq);
		pll_is_work = 1;
	}

	if (pll_is_work) 
	{
		
		VCPU_CRITICAL_ENTER();

		/* set APB divider by config */
		VCKM_SetAPBClockDivider(apb_div);
		
		/* switch ARM clock to PLL output */
		VCKM_ArmClockSelect(CKM_ARM_CLK_SEL_PLL);
		
		/* reconfigure SPI CLK */
		//TODO: 这个位置需要等到SPI driver同步后再做修改。
		VSPI_ReconfigureSpiClk(sys_freq);
		
		VCPU_CRITICAL_EXIT();
		
		g_VCKM_FreqInfo.sys_clock_freq = sys_freq;
		g_VCKM_FreqInfo.uart_clock_freq = CKM_PERCLK_48MHZ;
		g_VCKM_FreqInfo.timer_clock_freq = CKM_TIMER_CLK;				
		
	} 
	else 
	{
		g_VCKM_FreqInfo.uart_clock_freq = CKM_DEFAULT_XCLK_FREQ;
		g_VCKM_FreqInfo.sys_clock_freq = p_Config->xtal_freq;
		g_VCKM_FreqInfo.timer_clock_freq = CKM_TIMER_CLK;	
	}
	
    dev_comm_cfg = ( READ_REG ( REG_CKM_PCLK_CFG ) >> 8 ) & 0x07;
    pclk_cfg = READ_REG ( REG_CKM_PCLK_CFG ) & 0x03;

    //  g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / ((READ_REG(REG_CKM_PCLK_CFG) & 0x0f) + 1);
    g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / ( ( dev_comm_cfg + 1 ) * ( pclk_cfg + 1 ) );
    
g_VCKM_FreqInfo.i2c_clock_freq = g_VCKM_FreqInfo.apb_clock_freq;
#endif
#if (VIMC_TB_SOC_SIM==1)	
  VTB_CPU_trace(0x9638b001,g_VCKM_FreqInfo.sys_clock_freq);
	VTB_CPU_trace(0x9638b002,g_VCKM_FreqInfo.uart_clock_freq);
	VTB_CPU_trace(0x9638b003,g_VCKM_FreqInfo.timer_clock_freq);
	VTB_CPU_trace(0x9638b004,g_VCKM_FreqInfo.apb_clock_freq);
	VTB_CPU_trace(0x9638b005,g_VCKM_FreqInfo.i2c_clock_freq);
#endif
}
/* VDEV_CHIP_VERSION */

/******************************************************************************
Description:
 	
	Gets system clock frequency

Parameters:
	
Return Value:
	System clock frequrecy.

Remarks:
	

*****************************************************************************/
UINT32 VCKM_GetSysClockFreq(void)
{
    UINT32 fast_clk_sel;
    UINT32 fast_clk_freq;
    UINT8 fsel3, fsel2, div;

    // The DPLL clock table for VA9638P
    const UINT32 arm_dpll_clock_table[] = {
        CKM_SYSCLK_96MHZ,
        CKM_SYSCLK_128MHZ,
        CKM_SYSCLK_96MHZ,
        0,
        CKM_SYSCLK_48MHZ,
        CKM_SYSCLK_64MHZ,
        CKM_SYSCLK_48MHZ,
        0,
        CKM_SYSCLK_24MHZ,
    };
    const UINT32 arm_dpll_clock_table2[] = {
        CKM_SYSCLK_156MHZ,
        CKM_SYSCLK_78MHZ,
        CKM_SYSCLK_39MHZ,
    };
    const UINT32 arm_dpll_clock_table3[] = {
        CKM_SYSCLK_192MHZ,
        CKM_SYSCLK_96MHZ,
        CKM_SYSCLK_48MHZ,
    };


    // ToDo
    // 1. check dpll power on?
    // 2. check dpll bypass ?
    // 3.

    if ( GET_REG_BIT ( REG_CKM_SEL_CLK_ARM_CLK_SEL ) ) {
        fast_clk_sel = READ_REG ( REG_CKM_FASTCLK_SEL );
        fsel3 = ( fast_clk_sel >> 9 ) & 0x01;
        fsel2 = ( fast_clk_sel >> 8 ) & 0x01;
        div = fast_clk_sel & 0x0F;

        if ( ( fsel3 == 0 ) && ( fsel2 == 0 ) ) {
            fast_clk_freq = ( UINT32 ) arm_dpll_clock_table[div];
        } else if ( ( fsel3 == 0 ) && ( fsel2 == 1 ) ) {
            div >>= 2;
            fast_clk_freq = ( UINT32 ) arm_dpll_clock_table2[div];
        } else {
            div >>= 2;
            fast_clk_freq = ( UINT32 ) arm_dpll_clock_table3[div];
        }
    } else {
        fast_clk_freq = g_VCKM_FreqInfo.sys_clock_freq;
    }

    return fast_clk_freq;
}
/******************************************************************************
Description:
 	Set system clock for SLEEPDEEP.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_SetSysClockForSleepDeep(void)
{
	VCPU_SR_ALLOC();
	
	VCPU_CRITICAL_ENTER();
	
#ifndef FPGA_QA_BUILD	
	/* Configure SPI clock to prevent overclocking */
	VSPI_ClkSelect(4); 
	VCKM_SPIClockSelect( 0, 0);  //Restore default
#endif	/* FPGA_QA_BUILD */
	
	/* switch to OSC pad clock */
	VCKM_ArmClockSelect(CKM_ARM_CLK_SEL_OSC_PAD);

#ifndef FPGA_QA_BUILD		
	/*When the SOC clock is 26MHz, SPI selects clk_fast*/
	VSPI_ClkSelect(1);
#endif	/* FPGA_QA_BUILD */	
	
	/* set apb divider by default */
	VCKM_SetAPBClockDivider(CKM_PWRON_APB_DIV);
	
	VCPU_CRITICAL_EXIT();

	/* bypass & power down PLL */
	VCKM_PwdDPLL();
	
	g_VCKM_FreqInfo.pll_stat = 0;
	
	g_VCKM_FreqInfo.sys_clock_freq = g_VCKM_Config.xtal_freq;
	g_VCKM_FreqInfo.uart_clock_freq = g_VCKM_Config.xtal_freq;
	g_VCKM_FreqInfo.timer_clock_freq = CKM_TIMER_CLK;
}

/******************************************************************************
Description:
 	Increase the system clock frequency with the configuration from EEPROM.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_IncSysClockFreq(void)
{
	if (g_VCKM_Config.sys_clock_freq_high > g_VCKM_Config.sys_clock_freq) {
		VCKM_SetSysClockFreq(1, &g_VCKM_Config);
	}
}

/******************************************************************************
Description:
 	Resume the system clock frequency with the configuration from EEPROM.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_ResSysClockFreq(void)
{
	if (g_VCKM_Config.sys_clock_freq_high > g_VCKM_Config.sys_clock_freq) {
		VCKM_SetSysClockFreq(0, &g_VCKM_Config);
	}
}

/******************************************************************************
Description:
 	Bypass & power on DPLL, to supply peripheral clock.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_BypassPwOnDPLL(void)
{
	UINT32 reg;
	UINT32 timeout = 0;

	/* bypass DPLL */
	reg = READ_REG(REG_RFC_R15_DPLL_PFD);
	reg &= ~((UINT32)0x03 << 22);
	reg |= (UINT32)0x02 << 22;                 /* [23:22]: 01-non bypass, 10-bypass */
	WRITE_REG(REG_RFC_R15_DPLL_PFD, reg);
	/* enable SDM */
	SET_REG_BIT(REG_RFC_R16_DPLL_OVERWRITE_Sdm_enable);   /* [28]: 1-enable, 0-disable */
	/* power on PLL */
	SET_REG_BIT(REG_RFC_R0_RX_TX_SYN_CH_dpll_enable);     /* [10]: 1-enable, 0-disable */
	/* release DPLL reset */
	SET_REG_BIT(REG_RFC_R0_RX_TX_SYN_CH_dpll_counter_resetb);      /* [9]: 1-release reset, 0-reset */
	
	/* check lock status for stable output */
	while (!GET_REG_BIT(REG_CKM_SEL_CLK_DPLL_CLK_EN)) {
		if ((++timeout) >= CKM_DPLL_LOCK_TIMEOUT) {
			break;
		}
	}
}

/******************************************************************************
Description:
 	Bypass & power down DPLL.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_PwdDPLL(void)
{
	UINT32 reg;

	/* bypass DPLL */
	reg = READ_REG(REG_RFC_R15_DPLL_PFD);
	reg &= ~((UINT32)0x03 << 22);
	reg |= (UINT32)0x02 << 22;                 /* [23:22]: 01-non bypass, 10-bypass */
	WRITE_REG(REG_RFC_R15_DPLL_PFD, reg);
	/* disable SDM */
	CLR_REG_BIT(REG_RFC_R16_DPLL_OVERWRITE_Sdm_enable);   /* [28]: 1-enable, 0-disable */
	/* power down DPLL */
	reg = READ_REG(REG_RFC_R0_RX_TX_SYN_CH);
	reg &= ~((UINT32)0x03 << 9);               /* [10]: 1-enable, 0-disable; [9]: 1-release reset, 0-reset */
	WRITE_REG(REG_RFC_R0_RX_TX_SYN_CH, reg);
}

/******************************************************************************
Description:
 	Bypass & power on DPLL.

Parameters:
	None.
	
Return Value:
	None.
*****************************************************************************/
void VCKM_PwonDPLL(void)
{
	UINT32 reg;
	
	/* power on DPLL */
	reg = READ_REG(REG_RFC_R0_RX_TX_SYN_CH);
	reg |= ((UINT32)0x02 << 9);               /* [10]: 1-enable, 0-disable; [9]: 1-release reset, 0-reset */
	WRITE_REG(REG_RFC_R0_RX_TX_SYN_CH, reg);
}

/******************************************************************************
Description:
 	Set DPLL config and power on.

Parameters:
	None.
	
Return Value:
	TRUE - sucess
	FALSE - fail
	
*****************************************************************************/
BOOL VCKM_SetDPLL(VCKM_DPLL_CONFIG *p_Config)
{
	UINT32 timeout = 0;
	UINT32 reg;
	
	/* set DPLL configurations */
	WRITE_REG(REG_RFC_R11_DCXO1, p_Config->r11);
	WRITE_REG(REG_RFC_R15_DPLL_PFD, p_Config->r15);
	WRITE_REG(REG_RFC_R16_DPLL_OVERWRITE, p_Config->r16);
	WRITE_REG(REG_RFC_R18_DPLL_DIVIDE_RATION, ((UINT32)(p_Config->ref_sel & 0x0F) << 10)); /* [13:10]: reference frequency selection */
#if (VIMC_TB_SOC_SIM==1)
	WRITE_REG(REG_CKM_FASTCLK_SEL,1); // select 128Mhz as fast clock source. //K=6M
	//WRITE_REG(REG_CKM_DIV_CLK_CFG,15); // (N+1) * K = 96M
	WRITE_REG(REG_CKM_PCLK_CFG,(READ_REG(REG_CKM_PCLK_CFG) & 0x0f) | 3);    // 128M/4 = 32M APB clock
#else
    WRITE_REG ( REG_CKM_FASTCLK_SEL, 2 ); // select 96Mhz as fast clock source. K=6M
    //  WRITE_REG(REG_CKM_DIV_CLK_CFG,15); // (N+1) * K = 96M
    //  WRITE_REG(REG_CKM_PCLK_CFG,(READ_REG(REG_CKM_PCLK_CFG) & ~0x0f) | 3);    // 96M/3 = 24M
    WRITE_REG ( REG_CKM_PCLK_CFG, 0x0001 | 1 << 8 ); //fclk/(div+1)  96/((1+1)*(1+1))
#endif
	
	/* power on PLL */
	SET_REG_BIT(REG_RFC_R0_RX_TX_SYN_CH_dpll_enable);     /* [10]: 1-enable, 0-disable */
	/* release DPLL reset */
	SET_REG_BIT(REG_RFC_R0_RX_TX_SYN_CH_dpll_counter_resetb);      /* [9]: 1-release reset, 0-reset */
	/*not  bypass DPLL */
	reg = READ_REG(REG_RFC_R15_DPLL_PFD);
	reg &= ~((UINT32)0x03 << 22);
	reg |= (UINT32)0x01 << 22;                 /* [23:22]: 01-non bypass, 10-bypass */
	WRITE_REG(REG_RFC_R15_DPLL_PFD, reg);
	
	/* check lock status for stable output */
	while (!GET_REG_BIT(REG_CKM_SEL_CLK_DPLL_CLK_EN)) {
		if ((++timeout) >= CKM_DPLL_LOCK_TIMEOUT) {
			return FALSE;    /* timeout: FALSE */
		}
	}
	
	return TRUE;
}


/******************************************************************************
Description:
 	The clocks output from RFFE DPLL (clkin_384m, clkin_48m, clkin_24m) work stable when TRUE

Parameters:
	None.
	
Return Value:
	None.
	
*****************************************************************************/
BOOL VCKM_CheckDPLLStable(void) {
	if(!GET_REG_BIT(REG_CKM_SEL_CLK_DPLL_CLK_EN)) return FALSE;

	return TRUE;
}

UINT32 VCKM_Get_DPLL_Divisor(void)
{
    UINT32 val_part_0, val_part_1;
    val_part_0 = READ_REG(REG_RFC_R16_DPLL_OVERWRITE);
    val_part_0 &= 0x0000FFFF;
    val_part_0 <<= 16;
    val_part_1 = READ_REG(REG_RFC_R17_DPLL_DIVIDE_RATIO2);
    val_part_1 &= 0xFFFF0000;
    val_part_1 >>= 16;
    return (val_part_0 | val_part_1);
}

void VCKM_Set_DPLL_Divisor(UINT32 num)
{
    UINT32 val;
    val = READ_REG(REG_RFC_R16_DPLL_OVERWRITE);
    val &= 0xFFFF0000;
    val |= (num >> 16);
    WRITE_REG(REG_RFC_R16_DPLL_OVERWRITE, val);
    val = READ_REG(REG_RFC_R17_DPLL_DIVIDE_RATIO2);
    val &= 0x0000FFFF;
    val |= (num << 16);
    WRITE_REG(REG_RFC_R17_DPLL_DIVIDE_RATIO2, val);
}

#define VCKM_DPLL_DIV_DEFAULT    (0x0EC4E400)//default value.
#define VCKM_DPLL_DIV_MAX        (0x0EC5D5FB)//250ppm faster than the default value.
#define VCKM_DPLL_DIV_MIN        (0x0EC3F205)//250ppm slower than the default value.
#define VCKM_DPLL_DIV_STEP       (0x00000005)//(0x00000020)//roughly 0.13 ppm; 0x000100 for 1.033147 ppm.
void VCKM_Set_DPLL_Adjust_Enable(BOOL enable)
{
    UINT32 val = READ_REG(REG_RFC_R16_DPLL_OVERWRITE);
    if (enable) {
        if (!(val & 0x20000000)) {//previously disabled.
            VCKM_Set_DPLL_Divisor(VCKM_DPLL_DIV_DEFAULT);//initial value for divisor.
            val &= 0xFFFF0000;//reset the 31:16 bits of DPLL divisor.
            val |= VCKM_DPLL_DIV_DEFAULT >> 16;//initial value of 31:16 bits of DPLL divisor.
            val |= 0x20000000;//enable DPLL adjustment.
            WRITE_REG(REG_RFC_R16_DPLL_OVERWRITE, val);
        }
    }
    else {
        WRITE_REG(REG_RFC_R16_DPLL_OVERWRITE, val & 0xDFFFFFFF);
    }
}

UINT32 VCKM_Speed_Up_DPLL(UINT8 step)
{
    UINT32 div = VCKM_Get_DPLL_Divisor();
    div += VCKM_DPLL_DIV_STEP * step;
    if (div > VCKM_DPLL_DIV_MAX) {//divisor overflow.
        //no adjustment shall be applied.
        return 0;
    }
    VCKM_Set_DPLL_Divisor(div);
    return div;
}

UINT32  VCKM_Slow_Down_DPLL(UINT8 step)
{
    /*
     * slow down DPLL by DPLL_DIV_STEP.
     * DPLL clock = 26MHz (crystal freq) * divisor.
     * decrease the divisor to slow down the DPLL clock.
     */
    UINT32 div = VCKM_Get_DPLL_Divisor();
    div -= VCKM_DPLL_DIV_STEP * step;
    if (div < VCKM_DPLL_DIV_MIN) {//divisor overflow.
        //no adjustment shall be applied.
        return 0;
    }
    VCKM_Set_DPLL_Divisor(div);
    return div;
}

UINT32  VCKM_Slow_Adjust_DPLL(SINT8 step)
{
    UINT32 div = VCKM_Get_DPLL_Divisor();
    div += VCKM_DPLL_DIV_STEP * step;
    if (div < VCKM_DPLL_DIV_MIN) {//divisor overflow.
        //no adjustment shall be applied.
        return 0;
    }
    VCKM_Set_DPLL_Divisor(div);
    return div;
}

/******************************************************************************
Description:
 	Set SPIClock to select clk_fast path or 128M/96M path.

Parameters:
	switchen -  spi clock path switch enable signal.
						0: spi lcock path switch is disable. rgf_spi_128msel is invalid.
						1:spi lcock path switch is enable. rgf_spi_128msel is valid.
  Sel_128msel - spi clock path select. This signal is valid only when rgf_spiclk_switch_en= 1.
	           0: select clk_fast path
						 1: select 128M/96M path.
								When rgf_spi_128msel =1, the frequency of spi_clk is configed by rfg_fastclk_sel[1:0],
									rfg_fastclk_sel[1:0]=2'h1: 128MHz
									rfg_fastclk_sel[1:0]=other: 96MHz

Return Value:
	None.
	
*****************************************************************************/
void VCKM_SPIClockSelect(BOOL switchen,BOOL Sel_128msel)
{
	WRITE_REG ( REG_SPICLKSEL_128msel, Sel_128msel);
	
	WRITE_REG ( REG_SPICLKSEL_switchen, switchen );    
	
	return;
}

/******************************************************************************
Description:
 	Set CLK_SWT to select PLL output or OSC pad input for system clock.

Parameters:
	None.
	
Return Value:
	None.
	
*****************************************************************************/
void VCKM_ArmClockSelect(BOOL HighFreqClk)
{
	if (HighFreqClk)  /* from high frequency clock, which is from PLL1 or DPLL output */
	{	
		SET_REG_BIT(REG_CKM_SEL_CLK_ARM_CLK_SEL); 
	} else { /* from OSC pad */
		CLR_REG_BIT(REG_CKM_SEL_CLK_ARM_CLK_SEL);
	}
}

/******************************************************************************
Description:
 	Set 48M clock inverter.

Parameters:
	None.
	
Return Value:
	None.
	
*****************************************************************************/
void VCKM_Set48MInv(UINT8 Inv)
{
	if (Inv)
		SET_REG_BIT(REG_CKM_SEL_CLK_CLK48M_INV_SEL);
	else
		CLR_REG_BIT(REG_CKM_SEL_CLK_CLK48M_INV_SEL);
}

/******************************************************************************
Description:
 	Set 24M clock inverter.

Parameters:
	None.
	
Return Value:
	None.
	
*****************************************************************************/
void VCKM_Set24MInv(UINT8 Inv)
{
	if (Inv)
		SET_REG_BIT(REG_CKM_SEL_CLK_CLK24M_INV_SEL);
	else
		CLR_REG_BIT(REG_CKM_SEL_CLK_CLK24M_INV_SEL);
}

#if (VIMC_TB_SOC_SIM==1)
/******************************************************************************
Description:
 	To set REG_CKM_DIV_CLK_CFG register.

Parameters:
 	None.
	
Return Value:
 	None.
	
Remarks:
	Refer to VA9613i CK_MNG MAS_0.8.doc
	Fini = 384MHz
	
	DIV_CLK_CFG		Bit		Description						Reset Value
    Reserved	    [31:17]	Reserved	                    15'h0
    DIV_CLKDLY_EN	[16]	See the Divider Configure Table	1'h0
    Reserved	    [15:13]	Reserved	                    3'h0
    DIV_BARDLY_REG	[12]	See the Divider Configure Table	1'h0
    Reserved	    [11:9]	Reserved	                    3'h0
    DIV_DLY_REG	    [8]	    See the Divider Configure Table	1'h0
    DIV_LCNT_REG	[7:4]	See the Divider Configure Table	4'h3
    DIV_CNT_REG	    [3:0]	See the Divider Configure Table	4'h7

	Divider Configure Table
	Fdiv 		CNT		LCNT 	DLY 	BARDLY 	CLKDLY_EN
	1/3*Fini	4'd2	4'd1	2'd0	2'd0	1'd0
	1/3.5*Fini	4'd6	4'd3	2'd1	2'd0	1'd1
	1/4*Fini	4'd3	4'd1	2'd0	2'd0	1'd0
	1/4.5*Fini	4'd8	4'd4	2'd1	2'd1	1'd1
	1/5*Fini	4'd4	4'd2	1'd0	1'd0	1'd0
	1/5.5*Fini	4'd10	4'd5	1'd1	1'd1	1'd1
	1/6*Fini	4'd5	4'd2	2'd0	2'd0	1'd0
	1/6.5*Fini	4'd12	4'd6	1'd1	1'd1	1'd1
	1/7*Fini	4'd6	4'd2	1'd0	1'd0	1'd0
	1/7.5*Fini	4'd14	4'd7	1'd1	1'd1	1'd1
	1/8*Fini	4'd7	4'd3	2'd0	2'd0	1'd0

*****************************************************************************/
void VCKM_SetDivClkCfg(UINT32 SysFreq)
{
	UINT32 reg = 0;
	UINT32 calfreq = 0;
	
	UINT32 fastclk = CKM_SYSCLK_128MHZ; // Fast clock default is 128MHz

	if (SysFreq > fastclk)
	{
		WRITE_REG(REG_CKM_FASTCLK_SEL, 0x00);
		fastclk = CKM_SYSCLK_153MHZ;

		if (SysFreq > fastclk)
			SysFreq = fastclk;
	}
	else
	{
		WRITE_REG(REG_CKM_FASTCLK_SEL, 0x01); // default is 128MHz
	}
	// VA9638B not use REG_CKM_DIV_CLK_CFG
  /*
	for (reg = 0; reg < 16; reg++)
	{
		calfreq = (reg+1)*fastclk/16;

		if (calfreq >= SysFreq)
			break;
	}
	if (reg == 16)
		reg = 15;

	WRITE_REG(REG_CKM_DIV_CLK_CFG, reg);
	*/
}
#else
/******************************************************************************
Description:
 	To set REG_CKM_DIV_CLK_CFG register.

Parameters:
 	None.
	
Return Value:
 	None.
	
Remarks:
	Refer to VA9613i CK_MNG MAS_0.8.doc
	Fini = 384MHz
	
	DIV_CLK_CFG		Bit		Description						Reset Value
    Reserved	    [31:17]	Reserved	                    15'h0
    DIV_CLKDLY_EN	[16]	See the Divider Configure Table	1'h0
    Reserved	    [15:13]	Reserved	                    3'h0
    DIV_BARDLY_REG	[12]	See the Divider Configure Table	1'h0
    Reserved	    [11:9]	Reserved	                    3'h0
    DIV_DLY_REG	    [8]	    See the Divider Configure Table	1'h0
    DIV_LCNT_REG	[7:4]	See the Divider Configure Table	4'h3
    DIV_CNT_REG	    [3:0]	See the Divider Configure Table	4'h7

	Divider Configure Table
	Fdiv 		CNT		LCNT 	DLY 	BARDLY 	CLKDLY_EN
	1/3*Fini	4'd2	4'd1	2'd0	2'd0	1'd0
	1/3.5*Fini	4'd6	4'd3	2'd1	2'd0	1'd1
	1/4*Fini	4'd3	4'd1	2'd0	2'd0	1'd0
	1/4.5*Fini	4'd8	4'd4	2'd1	2'd1	1'd1
	1/5*Fini	4'd4	4'd2	1'd0	1'd0	1'd0
	1/5.5*Fini	4'd10	4'd5	1'd1	1'd1	1'd1
	1/6*Fini	4'd5	4'd2	2'd0	2'd0	1'd0
	1/6.5*Fini	4'd12	4'd6	1'd1	1'd1	1'd1
	1/7*Fini	4'd6	4'd2	1'd0	1'd0	1'd0
	1/7.5*Fini	4'd14	4'd7	1'd1	1'd1	1'd1
	1/8*Fini	4'd7	4'd3	2'd0	2'd0	1'd0

*****************************************************************************/
void VCKM_SetDivClkCfg(UINT32 SysFreq)             
{ 
    UINT32 fclk_val = 0;
    UINT8 fsel3 = 0, fsel2 = 0, fsel = 0;

    UINT32 fastclk = CKM_SYSCLK_192MHZ;

    if ( SysFreq > fastclk ) {
        fsel3 = 1;
    } else {
        switch ( SysFreq ) {
				//when fastclk_sel2 = 0 &  fastclk_sel3 = 0  ,the RFFE's PLL 128MHz path is selected for genaration of fast clock.
						case CKM_SYSCLK_96MHZ:
                fsel = 0;
                break;
            case CKM_SYSCLK_128MHZ:
                fsel = 1;
                break;
						case CKM_SYSCLK_48MHZ:
                fsel = 4;
                break;
            case CKM_SYSCLK_64MHZ:
                fsel = 5;
                break;
            case CKM_SYSCLK_24MHZ:
                fsel = 8;
                break;	
				//when fastclk_sel2 = 1 & fastclk_sel3 = 0,the RFFE's PLL 156.3MHz path is selected, 
            case CKM_SYSCLK_156MHZ:
								fsel2 = 1;
                fsel = 0+1;
                break;
            case CKM_SYSCLK_78MHZ:
                fsel2 = 1;
                fsel = 4+1;
                break;
            case CKM_SYSCLK_39MHZ:
                fsel2 = 1;
                fsel = 8+1;
                break;
				//when fastclk_sel2 = x & fastclk_sel3 = 1,the RFFE's PLL 192MHz path is selected
            case CKM_SYSCLK_192MHZ:  
                fsel3 = 1;
                fsel = 0+1;
                break;
//            case CKM_SYSCLK_96MHZ:
//                fsel3 = 1;
//                fsel = 4;
//                break;
//            case CKM_SYSCLK_24MHZ:
//                fsel3 = 1;
//                fsel = 8;
//                break;
            default:    //128M
                fsel = 1;
                break;
        }
    }

    if ( fsel3 | fsel2 ) {
        fclk_val = fsel | fsel3 << 9 | fsel2 << 8;
    } else {
        fclk_val = fsel;
    }
    WRITE_REG ( REG_CKM_FASTCLK_SEL, fclk_val );
}
#endif

/******************************************************************************
Description:
 	To set APB clock divider.

Parameters:
 	Divider: divider value
	
Return Value:
 	None.
	
Remarks:
	The APB clock frequency should not be higher than 26MHz.

*****************************************************************************/
void VCKM_SetAPBClockDivider(UINT8 Divider)
{
    UINT32 reg;
    UINT32 dev_comm_cfg, pclk_cfg;

    //  if (Divider == 0)
    //      Divider = 1;
    //  if (Divider > 16)
    //      Divider = 16;

    //  reg = READ_REG(REG_CKM_PCLK_CFG);
    //  WRITE_REG(REG_CKM_PCLK_CFG, (reg & ~0x0f) | (Divider - 1)); /* [3:0] */

    if ( Divider == 0 ) {
        Divider = 2;
    }
    if ( Divider > 8 ) {
        Divider = 8;
    }

    dev_comm_cfg = 1;
    pclk_cfg = Divider / ( dev_comm_cfg + 1 ) - 1;
    reg = ( dev_comm_cfg << 8 ) | pclk_cfg;
    WRITE_REG ( REG_CKM_PCLK_CFG, reg ); /* [10:8] = 1, [1:0]= (Divider / 2 -1) */

    //  g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / Divider;
    g_VCKM_FreqInfo.apb_clock_freq = g_VCKM_FreqInfo.sys_clock_freq / ( ( dev_comm_cfg + 1 ) * ( pclk_cfg + 1 ) );
    g_VCKM_FreqInfo.i2c_clock_freq = g_VCKM_FreqInfo.apb_clock_freq;

}

/******************************************************************************
Description:
 	To set BT/DSP Core state by enable/disable clock.

Parameters:
 	core: CKM_SYSCTRL_DSP or CKM_SYSCTRL_BT
 	enable: TRUE or FALSE
	
Return Value:
 	None.
	
Remarks:
	

*****************************************************************************/
void VCKM_SetCorprocessor(UINT8 core, UINT8 enable)
{
	UINT32 regval;
	
#define REG_CKM_SYS_CTRL_DSP_CLK_EN_BIT  0x10
#define REG_CKM_SYS_CTRL_BT_CLK_EN_BIT   0x08
#define REG_CKM_SYS_CTRL_BT_SW_RST_BIT   0x04

	// hold reset signal
	regval = READ_REG(REG_CKM_SYS_CTRL);

	if(core & CKM_SYSCTRL_DSP ) {
		if(enable) regval |= REG_CKM_SYS_CTRL_DSP_CLK_EN_BIT;
		else regval &= ~REG_CKM_SYS_CTRL_DSP_CLK_EN_BIT;
	}
	if(core & CKM_SYSCTRL_BT) {
		if(enable) {
			regval |= REG_CKM_SYS_CTRL_BT_CLK_EN_BIT;
			regval |= REG_CKM_SYS_CTRL_BT_SW_RST_BIT;
		}
		else {
			regval &= ~REG_CKM_SYS_CTRL_BT_CLK_EN_BIT;
			regval &= ~REG_CKM_SYS_CTRL_BT_SW_RST_BIT;
		}
	}

	WRITE_REG(REG_CKM_SYS_CTRL, regval);
}


/******************************************************************************
Description:
 	To set RAM Protect.

Parameters:
	block_mask witch block to protect
	
Return Value:
 	None.
	
Remarks:
	

*****************************************************************************/
void VCKM_SetProtect(UINT16 block_mask)
{
	UINT32 regval;
	
	// hold reset signal
	regval = READ_REG(REG_CKM_CRAM_RW_CTRL);

	regval &= ~0x0FFF;
	regval |= (block_mask & 0x0FFF);

	WRITE_REG(REG_CKM_CRAM_RW_CTRL, regval);
}

//UINT32 VCKM_GetSysClockFreq(void)
//{
//	return g_VCKM_FreqInfo.sys_clock_freq;
//}


void VCKM_DSPClockSelect(UINT32 DSPFreqClk)
{
	UINT32 clk_sel_val = 0;
	clk_sel_val = READ_REG(REG_CKM_SEL_CLK);
	clk_sel_val &= (~REG_CKM_SEL_CLK_DSP_MAISK);
	switch(DSPFreqClk){
		case CKM_DSPCLK_192MHZ:
			clk_sel_val |= 0x0000;
			break;
    case CKM_DSPCLK_128MHZ:
			clk_sel_val |= 0x0001;
			break;
//		case CKM_DSPCLK_384MHZ:
//			clk_sel_val |= 0x0102;
//			break;
    case CKM_DSPCLK_256MHZ:
			clk_sel_val |= 0x0103;
			break;
		case CKM_DSPCLK_26MHZ:
			clk_sel_val |= 0x3000;
			break;
    case CKM_DSPCLK_384MHZ:
			clk_sel_val |= 0x2000;
			break;
		case CKM_DSPCLK_153_6MHZ:
			clk_sel_val |= 0x1000;
			break;
	}
	WRITE_REG(REG_CKM_SEL_CLK,clk_sel_val);
}

UINT32 VCKM_GetAPBClockFreq(void)
{
	return g_VCKM_FreqInfo.apb_clock_freq;
}

UINT32 VCKM_GetI2CClockFreq(void)
{
	return g_VCKM_FreqInfo.i2c_clock_freq;
}

UINT32 VCKM_GetTimerClockFreq(void)
{
	return g_VCKM_FreqInfo.timer_clock_freq;
}


UINT32 VCKM_GetUartClockFreq(void)
{
#if (VDEV_TYPE == TYPE_ASIC)
	return g_VCKM_FreqInfo.uart_clock_freq;	
#else
	return CKM_XCLK_48000000HZ;
#endif	
}

UINT32 VCKM_GetSdClockFreq(void)
{
#if (VDEV_TYPE == TYPE_ASIC)
	return CKM_SYSCLK_48MHZ * (READ_REG(VSD_CLK_SEL) + 1);	
#else
	return CKM_SYSCLK_48MHZ;
#endif	
}


