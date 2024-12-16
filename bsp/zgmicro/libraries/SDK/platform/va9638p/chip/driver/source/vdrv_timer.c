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
    TMR

Description:
    Timer driver.

Revision History:
    Date                  Author                   Description
    2009.07.08           chenjingen              initial version
    2022.05.30            cxt                       9650
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_ckm.h"
#include "vdrv_timer.h"
#include "vreg_def.h"

static VDRV_TMR_CALLBACK vdrv_timer_callback[TMR_END] = {0};
/******************************************************************************
Description:
    Configures specified timer.

Parameters:
    TimerX - which timer to configure.
    Value - initial/reload counter value.
    AutoReload - whether automatically reload the counter value or not

Return Value:
    None.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VTMR_Config ( VTMR_SEL TimerX, UINT32 Value, BOOL AutoReload )
{
    // if ( Value > TMR_VALUE_MAX ) {
    //     Value = TMR_VALUE_MAX;
    // }

    switch ( TimerX ) {
        case TMR_SCLK:
            if(Value > 5) Value -= 5;
            else Value = 0;
            WRITE_REG ( REG_TIMER_TSCLKCINI, Value );
            if ( AutoReload ) {
                SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TSCLK_RLD_EN_BIT );
            } else {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TSCLK_RLD_EN_BIT );
            }
            break;
        case TMR_FCLKS:
            WRITE_REG ( REG_TIMER_TFCLKSCINI, Value );
            if ( AutoReload ) {
                SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_RLD_EN_BIT );
            } else {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_RLD_EN_BIT );
            }
            break;
        case TMR_FCLKL:
            WRITE_REG ( REG_TIMER_TFCLKLCINI, Value );
            if ( AutoReload ) {
                SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_RLD_EN_BIT );
            } else {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_RLD_EN_BIT );
            }
            break;
        case TMR_FCLKH:
            WRITE_REG ( REG_TIMER_TFCLKHCINI, Value );
            if ( AutoReload ) {
                SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_RLD_EN_BIT );
            } else {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_RLD_EN_BIT );
            }
            break;
		default:
			break;
    }
}

/******************************************************************************
Description:
    Starts the specified timer.

Parameters:
    TimerX - which timer to start.

Return Value:
    None.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VTMR_Start ( VTMR_SEL TimerX )
{
    //UINT64 delay_us = 0;

    switch ( TimerX ) {
        case TMR_SCLK:
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_SCLK_EN_BIT ); /* enable SCLK */
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TSCLK_CEN_BIT ); /* enable counter */
			//delay_us = VTMR_BBTimeget();
            // while(!GET_REG_BIT2 ( REG_TIMER_TSTA2, REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK_BIT ));
			// delay_us = VTMR_BBTimediff(delay_us,VTMR_BBTimeget());
			// if(delay_us > 800){
			// 	VCOMMON_EPRINTF("TSCLK_CEN_SYN delay:%uus\r\n",(UINT32)delay_us);		
			// }
            break;
        case TMR_FCLKS:
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_CEN_BIT );
            break;
        case TMR_FCLKL:
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_CEN_BIT );
            break;
        case TMR_FCLKH:
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            SET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_CEN_BIT );
            break;
		default:
			break;
    }
}

/******************************************************************************
Description:
    Stops the started timer.

Parameters:
    TimerX - which timer to stop.

Return Value:
    None.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VTMR_Stop ( VTMR_SEL TimerX )
{
    switch ( TimerX ) {
        case TMR_SCLK:
            CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TSCLK_CEN_BIT );
            CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_SCLK_EN_BIT );
            break;
        case TMR_FCLKS:
            CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_CEN_BIT );
            if ( ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_CEN_BIT ) ) && ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_CEN_BIT ) ) ) {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            }
            break;
        case TMR_FCLKL:
            CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_CEN_BIT );
            if ( ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_CEN_BIT ) ) && ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_CEN_BIT ) ) ) {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            }
            break;
        case TMR_FCLKH:
            CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_CEN_BIT );
            if ( ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_CEN_BIT ) ) && ( !GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_CEN_BIT ) ) ) {
                CLR_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_FCLK_EN_BIT );
            }
            break;
    }
}

/******************************************************************************
Description:
    Enables the interrupt generated by specified timer.

Parameters:
    TimerX - which timer to enable interrupt.

Return Value:
    None.

Remarks:
    The interrupt will be generated when counter of timer decreased to zero
    if enabled.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VTMR_EnableInt ( VTMR_SEL TimerX )
{
    #if 0
    VCPU_INT_SrcEnable ( IRQ_TIMER_INT );
    #else
    VCPU_INT_SrcEnable ( IRQ_TIMER_INT );
    switch (TimerX) {
      case TMR_SCLK:
      CLR_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_BIT ); /*open sclk Interrupt */
      break;
      case TMR_FCLKS:
      CLR_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_BIT ); /*open fclk_s Interrupt */
      break;
      case TMR_FCLKL:
      CLR_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_BIT ); /*open fclk_l Interrupt */
      break;
      case TMR_FCLKH:
      CLR_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_BIT ); /*open fclk_h Interrupt */
      break;
      default:
      ;
    }
    #endif
}

/******************************************************************************
Description:
    Disables the interrupt generated by specified timer.

Parameters:
    TimerX - which timer to disable interrupt.

Return Value:
    None.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VTMR_DisableInt ( VTMR_SEL TimerX )
{
    #if 0
    VCPU_INT_SrcDisable ( IRQ_TIMER_INT );
    #else
    //VCPU_INT_SrcDisable ( IRQ_TIMER_INT );
    switch (TimerX) {
      case TMR_SCLK:
      SET_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_BIT ); /*open sclk Interrupt */
      break;
      case TMR_FCLKS:
      SET_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_BIT ); /*open fclk_s Interrupt */
      break;
      case TMR_FCLKL:
      SET_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_BIT ); /*open fclk_l Interrupt */
      break;
      case TMR_FCLKH:
      SET_REG_BIT2 ( REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_BIT ); /*open fclk_h Interrupt */
      break;
      default:
      ;
    }
    #endif
}

void VTMR_ClearInt ( VTMR_SEL TimerX )
{
    switch ( TimerX ) {
        case TMR_SCLK:
            WRITE_REG ( REG_TIMER_TINTCLR, REG_TIMER_TINTCLR_S_CLR_MASK);
            break;
        case TMR_FCLKS:
            WRITE_REG ( REG_TIMER_TINTCLR, REG_TIMER_TINTCLR_FS_CLR_MASK);
            break;
        case TMR_FCLKL:
            WRITE_REG ( REG_TIMER_TINTCLR, REG_TIMER_TINTCLR_FL_CLR_MASK);
            break;
        case TMR_FCLKH:
            WRITE_REG ( REG_TIMER_TINTCLR, REG_TIMER_TINTCLR_FH_CLR_MASK);
            break;
    }
}


/******************************************************************************
Description:
    Gets the current counter value of specified timer.

Parameters:
    TimerX - which timer to get counter value.

Return Value:
    Returns the counter value of timer.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VTMR_GetCntValue ( VTMR_SEL TimerX )
{
    UINT32 value = 0;

    switch ( TimerX ) {
        case TMR_SCLK: 
            if (VTMR_IsRunning(TimerX) && !VTMR_GetWorkStatus(TimerX)) {
                value = READ_REG(REG_TIMER_TSCLKCINI);
            }else {
                value = READ_REG(REG_TIMER_TSCLKCNT);
            }
            break;
        case TMR_FCLKS:
            value = READ_REG ( REG_TIMER_TFCLKSCNT );
            break;
        case TMR_FCLKL:
            value = READ_REG ( REG_TIMER_TFCLKLCNT );
            break;
        case TMR_FCLKH:
            value = READ_REG ( REG_TIMER_TFCLKHCNT );
            break;
    }
    
    return value;
}

BOOL VTMR_IsRunning ( VTMR_SEL TimerX )
{
    BOOL running = FALSE;

    switch ( TimerX ) {
        case TMR_SCLK:
            running = GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TSCLK_CEN_BIT );
            break;
        case TMR_FCLKS:
            running = GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKS_CEN_BIT );
            break;
        case TMR_FCLKL:
            running = GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKL_CEN_BIT );
            break;
        case TMR_FCLKH:
            running = GET_REG_BIT2 ( REG_TIMER_TCTRL, REG_TIMER_TCTRL_TFCLKH_CEN_BIT );
            break;
    }

    return running;
}

/******************************************************************************
Description:
    Gets the work status. Use it Combining with VTMR_IsRunning().

Parameters:
    TimerX - which timer to get counter value.

Return Value:
    Returns the work status.

Remarks:
    The reason to add work status in version 9613iDA, is that CEN can not indicate critical status
    of HW timer exactly especially when timer counting to 0xFFFFFFF~0xFFFFFFFA. Because of the
    process of metastable state when crossing clock domains access (between pclk used by register
    and sclk/fclk used by timer), action of writing CEN will be excuted in 4 pclk cycles + 2 sclk/fclk
    cycles in HW timer module. So during this critical preriod, the CEN can not be used to adjudge
    the actaul working status, then combining with CEN_SYNC_BACK is useful. The CEN_SYNC_BACK
    is a signal which is changed in 3 sclk/fclk cycles after CEN.

Example:
    None.

See Also:
    None.
*****************************************************************************/
BOOL VTMR_GetWorkStatus ( VTMR_SEL TimerX )
{
    switch ( TimerX ) {
        case TMR_SCLK:
            return ( BOOL ) GET_REG_BIT2 ( REG_TIMER_TSTA2, REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK_BIT );
        default:
            return TRUE;
    }
}

BOOL VTMR_GetReloadStatus ( VTMR_SEL TimerX )
{
    switch ( TimerX ) {
        case TMR_SCLK:
            return ( BOOL ) GET_REG_BIT2 ( REG_TIMER_TSTA2, REG_TIMER_TSTA2_TSCLK_RLD_EN_SYN_BACK_BIT );
        default:
            return TRUE;
    }
}


UINT8 VTMR_GetInterruptStatus ( VTMR_SEL TimerX )
{
    UINT8 status;
    switch ( TimerX ) {
        case TMR_SCLK:
            status = GET_REG_BIT2 ( REG_TIMER_TSTA, REG_TIMER_TSTA_S_STA_BIT );
            break;
        case TMR_FCLKS:
            status = GET_REG_BIT2 ( REG_TIMER_TSTA, REG_TIMER_TSTA_FS_STA_BIT);
            break;
        case TMR_FCLKL:
            status = GET_REG_BIT2 ( REG_TIMER_TSTA, REG_TIMER_TSTA_FL_STA_BIT );
            break;
        case TMR_FCLKH:
            status = GET_REG_BIT2 ( REG_TIMER_TSTA, REG_TIMER_TSTA_FH_STA_BIT );
            break;
    }
    return status;
}

UINT32 VTMR_GetCntValueRaw ( VTMR_SEL TimerX )
{
    UINT32 value0 = 0;

    switch ( TimerX ) {
        case TMR_SCLK:
            value0 = READ_REG ( REG_TIMER_TSCLKCNT );
            break;
        case TMR_FCLKS:
            value0 = READ_REG ( REG_TIMER_TFCLKSCNT );
            break;
        case TMR_FCLKL:
            value0 = READ_REG ( REG_TIMER_TFCLKLCNT );
            break;
        case TMR_FCLKH:
            value0 = READ_REG ( REG_TIMER_TFCLKHCNT );
            break;
    }
    return value0;
}

void VTMR_RegisterISRCallback(VTMR_SEL sel, VDRV_TMR_CALLBACK p_Callback)
{
    if(sel < TMR_END){
        vdrv_timer_callback[sel] = p_Callback;
    }
}

void VTMR_CommDelay(UINT32 us)
{
    UINT32 ticks_to_dly, cur_time, wait_time, tmp;
	if(us == 0) return; //Delay compensation
    if ( !VTMR_IsRunning ( TMR_FCLKL ) ){
        VTMR_Config ( TMR_FCLKL, 0xFFFFFFFF, TRUE ); //0xFFFFFFFF,FALSE
        VTMR_DisableInt ( TMR_FCLKL );
        VTMR_Start ( TMR_FCLKL );
    }
    cur_time = VTMR_GetCntValue ( TMR_FCLKL );
	if(us == 1) return; //Delay compensation
	ticks_to_dly = (VCKM_GetTimerClockFreq() / 1000000) * (us - 2); //Delay compensation
    if ( cur_time < ticks_to_dly ) {
        wait_time = 0xFFFFFFFF - ( ticks_to_dly - cur_time );
        while ( 1 ) {
            tmp = VTMR_GetCntValue ( TMR_FCLKL );
            if ( ( tmp > cur_time ) && ( tmp < wait_time ) ) {
                break;
            }
        }
    } else {
        wait_time = cur_time - ticks_to_dly;
        while ( 1 ) {
            tmp = VTMR_GetCntValue ( TMR_FCLKL );
            if ( ( tmp < wait_time ) || ( tmp > cur_time ) ) {
                break;
            }
        }
    }
}

UINT32 VTMR_CommTimeget(void)
{
    if (!VTMR_IsRunning ( TMR_FCLKL )){
        VTMR_Config ( TMR_FCLKL, 0xFFFFFFFF, TRUE ); //0xFFFFFFFF,FALSE
        VTMR_DisableInt ( TMR_FCLKL );
        VTMR_Start ( TMR_FCLKL );
    }
    return VTMR_GetCntValue ( TMR_FCLKL );
}

void VTMR_BBTimeStart(void)
{
	SET_REG_BITS(HAB_BASE_ADDR+0x28,0,4,24);
	SET_REG_BIT2(HAB_BASE_ADDR+0x28, 5);
}

UINT64 VTMR_BBTimeget(void)
{
	UINT64 start_us = 0;
    UINT32 refclk = 0;
    UINT16 refus = 0;
    do{
        refclk = READ_REG(REG_BB_CRRT_REFCLK)>>2;
        refus =  READ_REG(REG_BB_CRRT_REFUS) % 1250;
    }while(refclk != READ_REG(REG_BB_CRRT_REFCLK)>>2);
    start_us = refclk*1250 + refus;
	return start_us;
}

UINT64 VTMR_BBTimediff(UINT64 start_us, UINT64 end_us)
{
	UINT64 result_us = 0;

	if(end_us >= start_us) {
            result_us = end_us - start_us;
    }else {
            result_us = (UINT64)(0x10000000>>2)*1250 - start_us + end_us;
    }
	return result_us;
}

void VTMR_BBDelay(UINT32 us)
{
    UINT64 start_us = 0,tmp_us = 0,result_us = 0;

    start_us = VTMR_BBTimeget();
    do{
        tmp_us = VTMR_BBTimeget();
		result_us = VTMR_BBTimediff(start_us,tmp_us);
    }while(result_us <= us);
}

void VTMR_IntHandler ( void )
{
    UINT32 int_status;
    /* read timer interrupt status */
    int_status = READ_REG ( REG_TIMER_TSTA );
    /* clear timer interrupt hardware status */
    WRITE_REG ( REG_TIMER_TINTCLR, int_status & \
    (REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_MASK|REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_MASK| REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_MASK|\
    REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_MASK));

    for(UINT8 i = 0; i < TMR_END;i++){
        if((int_status&(1UL<<i)) && vdrv_timer_callback[i]){
            vdrv_timer_callback[i]((VTMR_SEL)i);
        }
    }
}


