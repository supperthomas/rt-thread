/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
    I2S

Description:
     I2S Driver
Revision History:
     Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vreg_def.h"
#include "vstdcore.h"
#include "vdrv_asrc.h" 

/******************************************************************************
Description:
 	soft reset for ASRC module.

Parameters:
 	ASRC Number.
	
Return Value:
	Returns Null
*****************************************************************************/
void VASRC_SoftReset(UINT8 ASRC_Num)
{
	if (ASRC_Num == 0)
	{
		SET_REG_BIT2(REG_I2S_ASRC0_CTRL, 0);
		VCOMMON_Delay(10);
		CLR_REG_BIT2(REG_I2S_ASRC0_CTRL, 0);
	}
	else if (ASRC_Num == 1)
	{
		SET_REG_BIT2(REG_I2S_ASRC1_CTRL, 0);
		VCOMMON_Delay(10);
		CLR_REG_BIT2(REG_I2S_ASRC1_CTRL, 0);
	}
}

/******************************************************************************
Description:
 	Start work for ASRC module.

Parameters:
 	ASRC Number, run or sotp.
	
Return Value:
	Returns Null
*****************************************************************************/
void VASRC_ReSampleEn(UINT8 ASRC_Num, BOOL enable)
{
	if (ASRC_Num == 0)
	{
		if (enable)
		{
			CLR_REG_BIT2(REG_I2S_ASRC0_CTRL, 1);
		}
		else
		{
			SET_REG_BIT2(REG_I2S_ASRC0_CTRL, 1);
		}
	}
	else if (ASRC_Num == 1)
	{
		if (enable)
		{
			CLR_REG_BIT2(REG_I2S_ASRC1_CTRL, 1);
		}
		else
		{
			SET_REG_BIT2(REG_I2S_ASRC1_CTRL, 1);
		}
	}
}

/******************************************************************************
Description:
 	Set for ASRC module audio channel.

Parameters:
 	ASRC Number,  audio channel
ASRC0 to which audio channel:
3'h0:  None
3'h1:  I2S_0 Tx
3'h2:  I2S_0 Rx
3'h3:  I2S_1 Tx
3'h4:  Audio DAC
ASRC1 to which audio channel:
3'h0:  None
3'h1:  I2S_1 Tx
3'h2:  I2S_1 Rx
3'h3:  I2S_0 Tx
3'h4:  SPDIF

Return Value:
	Returns Null
*****************************************************************************/
void VASRC_SetAudioChannel(UINT8 ASRC_Num,UINT8 Channel)
{
	if (ASRC_Num == 0)
	{
		WRITE_REG(REG_I2S_ASRC0_CTRL, (READ_REG(REG_I2S_ASRC0_CTRL) & (~0x00000070)) | (Channel << 4));
	}
	else if (ASRC_Num == 1)
	{
		WRITE_REG(REG_I2S_ASRC1_CTRL, (READ_REG(REG_I2S_ASRC1_CTRL) & (~0x00000070)) | (Channel << 4));
	}
}

/******************************************************************************
Description:
 	Set step of ASRC module's  Software Controlling clock Divider .
	Fix ceiling, add step increase in_samp_rate
Parameters:
 	ASRC Number, step(12bit).
	
Return Value:
	Returns Null
*****************************************************************************/
void VASRC_SetSCDIV_step(UINT8 ASRC_Num,UINT32 step)
{
	if (ASRC_Num == 0)
	{
		WRITE_REG(REG_I2S_SCDIV0_CFG, (READ_REG(REG_I2S_SCDIV0_CFG) & (~0xFFF00000)) | (step << 20));
	}
	else if (ASRC_Num == 1)
	{
		WRITE_REG(REG_I2S_SCDIV1_CFG, (READ_REG(REG_I2S_SCDIV1_CFG) & (~0x7FF00000)) | (step << 20));
	}
}

/******************************************************************************
Description:
 	Set ceiling of ASRC module's  Software Controlling clock Divider.
	Fix step, add ceiling reduce in_samp_rate
Parameters:
 	ASRC Number, ceiling(18bit).
	
Return Value:
	Returns Null
*****************************************************************************/
void VASRC_SetSCDIV_ceiling (UINT8 ASRC_Num,UINT32 ceiling)
{
	if (ASRC_Num == 0)
	{
		WRITE_REG(REG_I2S_SCDIV0_CFG, (READ_REG(REG_I2S_SCDIV0_CFG) & (~0x0003ffff)) | (ceiling));
	}
	else if (ASRC_Num == 1)
	{
		WRITE_REG(REG_I2S_SCDIV1_CFG, (READ_REG(REG_I2S_SCDIV1_CFG) & (~0x0003ffff)) | (ceiling));
	}
}

/******************************************************************************
Description:
 	Get step of ASRC module's  Software Controlling clock Divider.
	
Parameters:
 	ASRC Number.
	
Return Value:
	Returns , step(12bit)
*****************************************************************************/
UINT32 VASRC_GetSCDIV_step(UINT8 ASRC_Num)
{
	if (ASRC_Num == 0)
	{
		return (READ_REG(REG_I2S_SCDIV0_CFG) & (0xFFF00000)) >> 20;
	}
	else if (ASRC_Num == 1)
	{
		return (READ_REG(REG_I2S_SCDIV1_CFG) & (0x7FF00000)) >> 20;
	}
	return 0;
}

/******************************************************************************
Description:
 	Get ceiling of ASRC module's  Software Controlling clock Divider.
	
Parameters:
 	ASRC Number.
	
Return Value:
	Returns , ceiling(18bit)
*****************************************************************************/
UINT32 VASRC_GetSCDIV_ceiling (UINT8 ASRC_Num)
{
	if (ASRC_Num == 0)
	{
		return (READ_REG(REG_I2S_SCDIV0_CFG) & (0x0003ffff));
	}
	else if (ASRC_Num == 1)
	{
		return (READ_REG(REG_I2S_SCDIV1_CFG) & (0x0003ffff));
	}
	return 0;
}


void VASRC_SetSampleRate(UINT8 ASRC_Num,UINT32 SR)
{
	UINT32 SCDIV_setp = 0, SCDIV_ceiling = 0;
#if (VDEV_TYPE == TYPE_ASIC)
	switch (SR) 
	{
		case 8000:
			SCDIV_setp = 24;
			SCDIV_ceiling = 96000;
			break;
		case 12000:
			SCDIV_setp = 36;
			SCDIV_ceiling = 96000;
			break;
		case 16000:
			SCDIV_setp = 48;
			SCDIV_ceiling = 96000;
			break;
		case 24000:
			SCDIV_setp = 72;
			SCDIV_ceiling = 96000;
			break;
		case 32000:
			SCDIV_setp = 96;
			SCDIV_ceiling = 96000;
			break;
		case 48000:
			SCDIV_setp = 144;
			SCDIV_ceiling = 96000;
			break;
		case 96000:
			SCDIV_setp = 288;
			SCDIV_ceiling = 96000;
			break;
		// 44.1K serial	
		case 11025:
			SCDIV_setp = 46;
			SCDIV_ceiling = 133515;		
			break;
		case 22050:
			SCDIV_setp = 92;
			SCDIV_ceiling = 133515; 	
			break;
		case 44100:
			SCDIV_setp = 184;
			SCDIV_ceiling = 133515; 	
			break;
		case 88200:
			SCDIV_setp = 368;
			SCDIV_ceiling = 133515; 	
			break;
	}

#else
	switch (SR) 
	{
		case 8000:
			SCDIV_setp = 24;
			SCDIV_ceiling = 72000;
			break;
		case 12000:
			SCDIV_setp = 36;
			SCDIV_ceiling = 72000;
			break;
		case 16000:
			SCDIV_setp = 48;
			SCDIV_ceiling = 72000;
			break;
		case 24000:
			SCDIV_setp = 72;
			SCDIV_ceiling = 72000;
			break;
		case 32000:
			SCDIV_setp = 96;
			SCDIV_ceiling = 72000;
			break;
		case 48000:
			SCDIV_setp = 144;
			SCDIV_ceiling = 72000;
			break;
		case 96000:
			SCDIV_setp = 288;
			SCDIV_ceiling = 72000;
			break;
		// 44.1K serial	
		case 11025:
			SCDIV_setp = 46;
			SCDIV_ceiling = 100136;		
			break;
		case 22050:
			SCDIV_setp = 92;
			SCDIV_ceiling = 100136; 	
			break;
		case 44100:
			SCDIV_setp = 184;
			SCDIV_ceiling = 100136; 	
			break;
		case 88200:
			SCDIV_setp = 368;
			SCDIV_ceiling = 100136; 	
			break;
	}


#endif

	//if (ASRC_Num == 0)
	//{
		VASRC_SetSCDIV_step(ASRC_Num, SCDIV_setp);
		VASRC_SetSCDIV_ceiling(ASRC_Num, SCDIV_ceiling);
	//}
	//else if (ASRC_Num == 1)
	//{
	//	VASRC_SetSCDIV_step(ASRC_Num, SCDIV_setp);
	//	VASRC_SetSCDIV_ceiling(ASRC_Num, SCDIV_ceiling);
	//}
	return;
}

void VASRC_LeAudioSetSampleRate(UINT8 ASRC_Num,UINT32 SR)
{
	UINT32 SCDIV_setp = 0, SCDIV_ceiling = 0;
	switch (SR) 
	{
		case 8000:
			SCDIV_setp = 24;
			SCDIV_ceiling = 96000;
			break;
		case 12000:
			SCDIV_setp = 36;
			SCDIV_ceiling = 96000;
			break;
		case 16000:
			SCDIV_setp = 48;
			SCDIV_ceiling = 96000;
			break;
		case 24000:
			SCDIV_setp = 180;
			SCDIV_ceiling = 120000;
			break;
		case 32000:
			SCDIV_setp = 96;
			SCDIV_ceiling = 96000;
			break;
		case 48000:
			SCDIV_setp = 360;
			SCDIV_ceiling = 240000;
			break;
		case 96000:
			SCDIV_setp = 288;
			SCDIV_ceiling = 96000;
			break;
		case 11025:
			SCDIV_setp = 46;
			SCDIV_ceiling = 133515;		
			break;
		case 22050:
			SCDIV_setp = 92;
			SCDIV_ceiling = 133515; 	
			break;
		case 44100:
			SCDIV_setp = 184;
			SCDIV_ceiling = 133515; 	
			break;
		case 88200:
			SCDIV_setp = 368;
			SCDIV_ceiling = 133515; 	
			break;
	}
	VASRC_SetSCDIV_step(ASRC_Num, SCDIV_setp);
	VASRC_SetSCDIV_ceiling(ASRC_Num, SCDIV_ceiling);
	return;
}
/******************************************************************************
Description:
 	Get ASRC module's  STATUS_locked.
	This active-High output indicates that ratio
	tracking is in a locked state, i.e., the input FIFO
	level is within the prescribed thresholds and
	stable.
Parameters:
 	ASRC Number.
	
Return Value:
	Returns 
*****************************************************************************/
BOOL VASRC_Get_STATUS_locked(UINT8 ASRC_Num)
{
	if (ASRC_Num == 0)
	{
		return (BOOL)GET_REG_BIT2(REG_I2S_ASRC0_STATUS, 0);
	}
	else if (ASRC_Num == 1)
	{
		return (BOOL)GET_REG_BIT2(REG_I2S_ASRC1_STATUS, 0);
	}
	return FALSE;
}

/******************************************************************************
Description:
 	Get ASRC module's  STATUS_fifo_overflow.
	This active-High output indicates that the level
	of the input FIFO is beyond the safe operating
	range. Therefore, loss of input samples is likely.
Parameters:
 	ASRC Number.
	
Return Value:
	Returns 
*****************************************************************************/
BOOL VASRC_Get_STATUS_fifo_overflow(UINT8 ASRC_Num)
{
	if (ASRC_Num == 0)
	{
		return (BOOL)GET_REG_BIT2(REG_I2S_ASRC0_STATUS, 1);
	}
	else if (ASRC_Num == 1)
	{
		return (BOOL)GET_REG_BIT2(REG_I2S_ASRC1_STATUS, 1);
	}
	return FALSE;
}

