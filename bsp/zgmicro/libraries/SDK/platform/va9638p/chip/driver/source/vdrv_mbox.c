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
    Message Box

Description:
    Message Box between CM3 & CoolFlux DSP

Revision History:
    Date                  Author                   Description
    2014.03.24      Deng Zhigao     initial version
    2017.11.10      Chanjiang       update for 9638b
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vstdio.h"
#include "vdrv_mbox.h"
#include "vreg_def.h"

VMBOX_DSP_CALLBACK g_vmbox_dsp_intCallback[3];

UINT8  g_vmbox_d2c_msg_info_inptr = 0;
UINT8  g_vmbox_d2c_msg_info_outptr = 0;

UINT32 g_interrupt_dbg[3];

void VMBOX_EnableInt(void)
{
    VCPU_INT_SrcEnable(IRQ_DSP_INT);
}
    
void VMBOX_DisableInt(void)
{
    VCPU_INT_SrcDisable(IRQ_DSP_INT);
}
    
BOOL VMBOX_IntPending(void)
{
    return VCPU_INT_SrcIsPending(IRQ_DSP_INT);
}

/******************************************************************************
Description:
    Message Box DSP Interrupt Callback
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Interrupt_Callback(UINT8 src, void *message)
{
    if (g_vmbox_dsp_intCallback[src]) {
        g_vmbox_dsp_intCallback[src](message);
    }
}

/******************************************************************************
Description:
    Message Box Register callback
Parameters:
    src:    source
    callback:   callback function
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_RegiserCallback(UINT8 src, VMBOX_DSP_CALLBACK callback)
{
    if (src < 3)
        g_vmbox_dsp_intCallback[src] = callback;

    g_vmbox_d2c_msg_info_inptr = 0;
    g_vmbox_d2c_msg_info_outptr = 0;
}

/******************************************************************************
Description:
    Message Box Get DSP State
Parameters:
    void
Return Value:
    DSP State
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
UINT8 VMBOX_DSP_GetState(void)
{
    UINT32 reg;
    UINT8 mode;

    reg = READ_REG(REG_MSG_CM3MBX_DSP_STAT);
    mode = (reg & 0x0f0);
    
    return mode;
}

UINT8 VMBOX_DSP_INT_Get(void)
{
    UINT32 reg;
    UINT8 mode;

    reg = READ_REG(REG_MSG_CM3MBX_DSP_STAT);
    mode = (reg & 0x0f);
    
    return mode;
}

void VMBOX_DSP_INT_CLR(void)
{
    SET_REG_BIT2(REG_MSG_CM3MBX_DSP_STAT, VCORE_GetID());
}


/******************************************************************************
Description:
    Message Box Reset DSP
Parameters:
    mode:       core/whole
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Reset(UINT8 mode)
{
    UINT32 reg;
    UINT32 timeout=100;
    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= ~(VMBOX_REG_CTRL2_RESET_MASK);
    //unhold
    reg &= ~(VMBOX_REG_CTRL2_HOLD_MASK);
    reg |= (1<< VMBOX_REG_CTRL2_BIT_RESTART);
    
    switch (mode) {
        case DSP_RESET_MODE_CORE:
            reg |= (1<<VMBOX_REG_CTRL2_BIT_RESET_DSP);
            break;
        case DSP_RESET_MODE_WHOLE:
            reg |= (1<<VMBOX_REG_CTRL2_BIT_RESET_DSS);
            break;
    }   
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);
    
    while (timeout) {
        timeout--;
        if (VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_STOP_ACK))
            break;
    }   
    
    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= (~(1<< VMBOX_REG_CTRL2_BIT_RESTART));
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);
}

/******************************************************************************
Description:
    Message Box DSP leave 
Parameters:
    mode:       core/whole
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Enable(void)
{
    UINT32 reg;

    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= ~(VMBOX_REG_CTRL2_RESET_MASK);
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);

    while (1) {
        if (!(VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_STOP_ACK)))
            return;
    }   
}


/******************************************************************************
Description:
    Message Box Hold DSP
Parameters:
    mode:       hold mode
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Hold(UINT8 mode)
{
    UINT32 reg;

    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= ~(VMBOX_REG_CTRL2_HOLD_MASK);
    reg |= ((mode<<VMBOX_REG_CTRL2_BIT_HOLD)&VMBOX_REG_CTRL2_HOLD_MASK);
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);

    while (1) {
        if ((VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_CORE_HOLD_ACK)) &&
            ((VMBOX_DSP_GetState()& (1 << VMBOX_REG_DSP_STAT_IO_HOLD_USER)) == 0))
            return;
    }   
}

/******************************************************************************
Description:
    Message Box DSP Run
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Unhold(void)
{
    UINT32 reg;

    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= ~(VMBOX_REG_CTRL2_HOLD_MASK);
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);

    while (1) {
        if (!(VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_CORE_HOLD_ACK)))
            return;
    }   
}


/******************************************************************************
Description:
    Message Box memory hold enable
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_MemHoldEnable(void)
{
    UINT32 reg;

    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg |= (1<<VMBOX_REG_CTRL2_BIT_DMS_HOLD_EN);
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);
}

/******************************************************************************
Description:
    Message Box memory hold enable
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_MemHoldDisable(void)
{
    UINT32 reg;
    
    reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
    reg &= ~(1<<VMBOX_REG_CTRL2_BIT_DMS_HOLD_EN);
    WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);   
}

/******************************************************************************
Description:
    Message Box memory hold enable
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_Restart(void)
{
    UINT32 reg,loop=0;
    if ((VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_HALT_ACK))){
        reg = READ_REG(REG_MSG_CM3MBX_CTRL2);
        reg |= (1<<VMBOX_REG_CTRL2_BIT_RESTART);
        WRITE_REG(REG_MSG_CM3MBX_CTRL2, reg);   

        while(VMBOX_DSP_GetState()&(1<<VMBOX_REG_DSP_STAT_HALT_ACK)){
            loop++;
            if(loop > 200)  break;
        }	

        //clear flag
        reg = READ_REG(REG_MSG_CM3MBX_CTRL2) & (~BIT5);
        WRITE_REG(REG_MSG_CM3MBX_CTRL2,reg);
    }	
}

void VMBOX_DSP_ClkEnable(UINT8 en)
{
	if(en) {
		REG_BIT_SET(REG_MSG_CM3MBX_CTRL2,VMBOX_REG_CTRL2_BIT_DSP_CLK_EN);
	}else {
		REG_BIT_CLR(REG_MSG_CM3MBX_CTRL2,VMBOX_REG_CTRL2_BIT_DSP_CLK_EN);
	}
}

UINT8 VMBOX_DSP_GetIntStat(int num)
{
    UINT32 reg;
    UINT8 ret;
    
#if (CM3_CORE_SEL == DSP_TO_CTP)
    reg = READ_REG(REG_MSG_CM3MBX_CTRL0) & BIT1;
      
#else
    reg = READ_REG(REG_MSG_CM3MBX_CTRL1) &  BIT1;
#endif  

    ret = reg ? TRUE:FALSE;

    return ret;
}


/******************************************************************************
Description:
    Message Box Setup Interrupt to DSP
Parameters:
    num:    divident
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_SetInterrupt(int num)
{
    
#if (CM3_CORE_SEL == DSP_TO_CTP)
    VMBOX_INT_TOP2DSP_SET(); 
#else
    VMBOX_INT_BT2DSP_SET(); 
#endif
    
    g_interrupt_dbg[1]++;
}

/******************************************************************************
Description:
    VMBOX_DSP_SetCacheUpload
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_SetCacheUpload(void)
{
    UINT32 reg;
   
    reg =  READ_REG(REG_MSG_DSP_CACH_UPD) ;
    reg |= (1<<VMBOX_REG_DSP_CACH_UPD_REQ);
    WRITE_REG(REG_MSG_DSP_CACH_UPD, reg);
    
    while(1){
        reg =  READ_REG(REG_MSG_DSP_CACH_UPD) ;
        if( reg & (1<<VMBOX_REG_DSP_CACH_UPD_RDY))
            break;
    }
}
/******************************************************************************
Description:
    VMBOX_DSP_ClrCacheUpload
Parameters:
    void
Return Value:
    void
Remarks:
    None.
Example:
    None.
See Also:
    None.
*****************************************************************************/
void VMBOX_DSP_ClrCacheUpload(void)
{
    UINT32 reg;
   
    reg =  READ_REG(REG_MSG_DSP_CACH_UPD) ;
    reg &= ~(1<<VMBOX_REG_DSP_CACH_UPD_REQ);
    WRITE_REG(REG_MSG_DSP_CACH_UPD, reg);
}




