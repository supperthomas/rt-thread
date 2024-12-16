/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
	HD Serial Port

Description:
 	HD_CPort Driver

Revision History:
 	Date                  Author                   Description
 	2017.10.08          guanguojin                 initial version
    2022.05.30            cxt                       
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vdrv_gpio.h"
#include "vstdcore.h"
#include "vdrv_hd_cport.h"
#include "vreg_def.h"

/******************************************************************************
Description:
 	Enable AIF IIC Clock for VA9640

Parameters:
	void
Return Value:
	None.
*****************************************************************************/
void VHDS_EnableBClk(void)
{
	SET_REG_BIT(REG_AIF_CDC_CTRL_I2CCLK_OUT_EN);
}

/******************************************************************************
Description:
 	Disable AIF IIC Clock for VA9640

Parameters:
	void
Return Value:
	None.
*****************************************************************************/
void VHDS_DisableBClk(void)
{
	CLR_REG_BIT(REG_AIF_CDC_CTRL_I2CCLK_OUT_EN);
}

void VHDS_HW_Bclk_Enable(void)
{
		UINT8 time = 17;
#if 1
		//2020114xy, set ADC sampleRate 48K for the HDS default clock 6M at Vcdc_CmdBus_Initialize(); not do here SetADCSampleRate each IIC process, is wrong;
		VHDS_EnClock();
		VHDS_EnableBClk();

		SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_HD_I2C_SEL);
#else
	 UINT32 setting_sample_rate = 16000;
	 switch (ADC_CLK_MODE) {
			 case PDM_1M:
					 setting_sample_rate = 8000;
					 break;
			 case PDM_2M:
					 setting_sample_rate = 16000;
					 break;
			 case PDM_4M:
					 setting_sample_rate = 32000;
					 break;
			 case PDM_6M:
					 setting_sample_rate = 48000;
					 break;
	 }
	 VAIF_SetADCSampleRate(setting_sample_rate); // ADC
	 VHDS_EnableBClk();
	 //VCOMMON_Delay(1); // when set 1M Delay 1us for slave dev working
#endif
	 while (time--);       // when set 1M Delay 1us for slave dev working
}
void VHDS_HW_Bclk_Disable(void)
{
	VHDS_DisableBClk();
	VHDS_DisClock();
	
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_HD_I2C_SEL);//20220225xy, avoid the mode not change maybe the RF EDR Phase&Floor Fail; fix&test ok by weilin;
}

/******************************************************************************
Description:
	write command.

Parameters:
	addr - 8-bit register address .
	val  - 8-bit value to write into the specific register. 
Return Value:
	VRET_NO_ERROR - sucess
	VRET_DRV_HDS_TIMEOUT - fail
 *****************************************************************************/
VRET_VALUE VHDS_WriteCmd(UINT8 addr, UINT8 val)
{
//	UINT32 old_val;
	UINT32 retry = VHDS_DONE_RETRY;
	
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	
	//    CLR_REG_BIT(REG_HD_CTRL, RBIT_HD_CTRL_DMA_MODE_EN);
	WRITE_REG(REG_HD_CMD_WDATA, val);
	WRITE_REG(REG_HD_ADDRESS, addr);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CMD_WR);

	do {
		if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_DONE)) {
			break;
		}
		retry--;
	} while (retry);

	if(retry){
		retry = VHDS_DONE_RETRY; //wait done status be cleared
		do {
			if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_DONE) == 0) {
				break;
			}
			retry--;
		} while (retry);
	}

	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CMD_WR);

	VCPU_CRITICAL_EXIT();
	
	if (retry == 0) {
		return VRET_DRV_HDS_TIMEOUT;
	} else if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_CMDERR)) {
		return VRET_DRV_HDS_CMD_ERROR;
	}

	return VRET_NO_ERROR;
}

/******************************************************************************
Description:
	read command.

Parameters:
	addr - 8-bit register address .
	val  - A pointer to receive respond value. 
Return Value:
	VRET_NO_ERROR - sucess
	VRET_DRV_HDS_TIMEOUT - fail
 *****************************************************************************/
VRET_VALUE VHDS_ReadCmd(UINT8 addr, UINT8 *p_val)
{
//	UINT32 old_val;
	UINT32 retry = VHDS_DONE_RETRY;
	VRET_VALUE ret = VRET_NO_ERROR;

	if (p_val == NULL) {
		return VRET_DRV_HDS_PARAM_ERROR;
	}
	
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();

	//    CLR_REG_BIT(REG_HD_CTRL, RBIT_HD_CTRL_DMA_MODE_EN);
	WRITE_REG(REG_HD_ADDRESS, addr);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CMD_RD);

	do {
		if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_DONE)) {
			break;
		}
		retry--;
	} while (retry);

	if (retry == 0) {
		ret = VRET_DRV_HDS_TIMEOUT;
	} else {
		*p_val = READ_REG(REG_HD_CMD_RDATA);
	}

	retry = VHDS_DONE_RETRY; //wait done status be cleared
	do {
		if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_DONE) == 0) {
			break;
		}
		retry--;
	} while (retry);

	if (retry == 0) {
		ret = VRET_DRV_HDS_TIMEOUT;
	} 
	else if (GET_REG_BIT2(REG_HD_STATUS, RBIT_HD_STATUS_CMDERR)) {
		ret = VRET_DRV_HDS_CMD_ERROR;
	}

	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CMD_RD);
	
	VCPU_CRITICAL_EXIT();
	

	return ret;
}

/******************************************************************************
Description:
	Write AudioData.

Parameters:
	VHD_DMA_SIZE - VHD_DMA_SIZE_16BIT/VHD_DMA_SIZE_24BIT.
Return Value:
	None.
 *****************************************************************************/
void VHDS_WriteAudioData(VHD_DMA_SIZE size)
{
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_WRFIFO_RST);
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_WRFIFO_RST);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DMA_MODE_EN);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_TX_EN);
	if (size == VHD_DMA_SIZE_16BIT) {
		SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	} else {
		CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	}
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

/******************************************************************************
Description:
	Read AudioData.

Parameters:
	VHD_DMA_SIZE - VHD_DMA_SIZE_16BIT/VHD_DMA_SIZE_24BIT.
Return Value:
	None.
 *****************************************************************************/
void VHDS_ReadAudioData(VHD_DMA_SIZE size)
{
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_RDFIFO_RST);
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_RDFIFO_RST);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DMA_MODE_EN);
	//SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_RX_EN);
	if (size == VHD_DMA_SIZE_16BIT) {
		SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	} else {
		CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	}
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

/******************************************************************************
Description:
	Switch 9838B into HDS mode.

Parameters:
	None.
Return Value:
	None.
 *****************************************************************************/
void VHDS_Initialize(UINT8 div)
{
	//WRITE_REG(REG_HD_CTRL, 0x20);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_HD_I2C_SEL);	
	if (div < 2){
		WRITE_REG(REG_HD_CLK_DIV, div);//need to be confirmed
	}
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CLK_EN);
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

/******************************************************************************
Description:
	Switch 9838B into I2C mode.

Parameters:
	None.
Return Value:
	None.
 *****************************************************************************/
void VHDS_Finit(void)
{
	//WRITE_REG(REG_HD_CTRL, 0x00);
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_HD_I2C_SEL);
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CLK_EN);
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

/******************************************************************************
Description:
	Enable HDS clock in HDS mode.

Parameters:
	None.
Return Value:
	None.
 *****************************************************************************/
void VHDS_EnClock(void)
{
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CLK_EN);
}

/******************************************************************************
Description:
	Disable HDS clock in HDS mode.

Parameters:
	None.
Return Value:
	None.
 *****************************************************************************/
void VHDS_DisClock(void)
{
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_CLK_EN);
}

void VHDS_EnCtrl(void)
{
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

void VHDS_DisCtrl(void)
{
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_EN);
}

void VHDS_DataSizeConfig(VHD_DMA_SIZE dma_size)
{
	if (dma_size == VHD_DMA_SIZE_16BIT) {
		SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	} else {
		CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DATA_SIZE);
	}
}

UINT32 VHDS_GetCtrlConfig(void)
{
	return READ_REG(REG_HD_CTRL);
}

/******************************************************************************
Description:
	Set HDS clock div parameter.

Parameters:
	div - 0, 1.
Return Value:
	None.
 *****************************************************************************/
void VHDS_SetClock(UINT8 div)
{
	if (div > 1) {
		return;
	}
	WRITE_REG(REG_HD_CLK_DIV, div);//need to be confirmed
}

UINT8 VHDS_GetClock(void)
{
	return READ_REG(REG_HD_CLK_DIV);
}

void VHDS_SetRdAudio(void)
{
	SET_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_RX_EN);
}

void VHDS_DisRdAudio(void)
{
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_RX_EN);
}

void VHDS_DisWrAudio(void)
{
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_TX_EN);
}

void VHDS_DisDMAMode(void)
{
	CLR_REG_BIT2(REG_HD_CTRL, RBIT_HD_CTRL_DMA_MODE_EN);
}


/******************************************************************************
Description:
 	HDS read register.

Parameters:
	addr        - address.
    p_val       - a pointer to receive response data.
Return Value:
	VRET_VALUE.
*****************************************************************************/
VRET_VALUE VHDS_register_read(UINT8 addr, UINT8 *p_val)
{
    VRET_VALUE ret;

    // ASSERT(g_hds_hw.init == TRUE);
		VHDS_Initialize(1);
    VHDS_HW_Bclk_Enable();
	VCOMMON_Delay(1);
#ifdef HDS_BENCHMARK
    VDWT_Start();
#endif
    ret = VHDS_ReadCmd(addr, p_val);

#ifdef HDS_BENCHMARK
    VDWT_Stop();
    HDS_NPRINTF("VHDS_ReadCmd consume %dcycles/%dus @%dHz\r\n",
                VDWT_Cnt(),
                VDWT_Cnt()/(VCKM_GetSysClockFreq()/1000000),
                VCKM_GetSysClockFreq());
#endif
		VHDS_HW_Bclk_Disable();
    //VSYS_SOC_Unlock(g_I2CLock);   
    return ret;
}

/******************************************************************************
Description:
 	HDS write register.

Parameters:
	addr        - address.
    val         - value.
Return Value:
	VRET_VALUE.
*****************************************************************************/
VRET_VALUE VHDS_register_write(UINT8 addr, UINT8 val)
{
    VRET_VALUE ret;

    // ASSERT(g_hds_hw.init == TRUE);
   
	 VHDS_Initialize(1);
    //VSYS_SOC_Lock(g_I2CLock);
		VHDS_HW_Bclk_Enable();
		VCOMMON_Delay(1); // when set 1M Delay 1us for slave dev working

#ifdef HDS_BENCHMARK
    VDWT_Start();
#endif

    ret = VHDS_WriteCmd(addr, val);

#ifdef HDS_BENCHMARK
    VDWT_Stop();
    HDS_NPRINTF("VHDS_WriteCmd consume %dcycles/%dus @%dHz\r\n",
                VDWT_Cnt(),
                VDWT_Cnt()/(VCKM_GetSysClockFreq()/1000000),
                VCKM_GetSysClockFreq());
#endif
		VHDS_HW_Bclk_Disable();
    //VSYS_SOC_Unlock(g_I2CLock);  
    return ret;
}
