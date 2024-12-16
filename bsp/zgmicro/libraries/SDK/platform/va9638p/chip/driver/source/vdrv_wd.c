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
    WD

Description:
     Watchdog driver.

Revision History:
     Date                  Author                   Description
     2010.05.13         jch                        initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vdrv_wd.h"
#include "vreg_def.h"

/******************************************************************************
Description:
     Enable the watchdog timer

Parameters:
    Mode - watchdog timer working mode
        WD_RST: system reset
        WD_NMI: non-masked interrupt
     TimeoutMS - timeout of watchdog timer, unit is millisecond
    
Return Value:
    None.

Remarks:
    None.

Example:
     None.

See Also:
    None.
*****************************************************************************/
void VWD_Enable(UINT8 Mode, UINT32 TimeoutMS)
{
    UINT32 cnt;
    UINT32 retry = 5;
    UINT32 cnt_rd;

    cnt = (VWD_CLK_IN / 1000) * TimeoutMS;

    REG_BIT_CLR(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* unlock WD register */
    REG_BIT_CLR(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDCEN_BIT);         /* disable WD */
    VCOMMON_Delay(800);   // at least 0.5ms after enable and disable watch dog, modify by zhangcheng, from weihuaiqing
    if (Mode == WD_NMI)
        REG_BIT_CLR(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDMODE_BIT);     /* NMI mode */
    else
        REG_BIT_SET(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDMODE_BIT);     /* system reset mode */
    
    do {
        WRITE_REG(REG_TIMER_WDCINI, cnt);                         /* counter initial */
        cnt_rd = READ_REG(REG_TIMER_WDCINI);
        if (cnt == cnt_rd)
            break;
    } while (--retry);
    
    
    REG_BIT_SET(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDCEN_BIT);         /* enable WD */
    REG_BIT_CLR(REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_WD_MASK_BIT);         /*open WD TMR Interrupt*/
    VCOMMON_Delay(1200); /* delay a cycle of 32KHz to synchronize the register of REG_TIMER_WDCNT */
    
    REG_BIT_SET(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* lock WD register */

}

/******************************************************************************
Description:
     Disable the watchdog.

Parameters:
     None.
    
Return Value:
    None.

Remarks:
    None.

Example:
     None.

See Also:
    None.
*****************************************************************************/
void VWD_Disable(void)
{
    REG_BIT_CLR(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* unlock WD register */
    REG_BIT_SET(REG_TIMER_TINTMASK, REG_TIMER_TINTMASK_INT_WD_MASK_BIT);         /*mask WD TMR Interrupt*/
    REG_BIT_CLR(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDCEN_BIT);             /* disable WD */
    REG_BIT_SET(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* lock WD register */
}

/******************************************************************************
Description:
     Get the status of watchdog timer.

Parameters:
     None.
    
Return Value:
    Watchdog timer status, 1-enabled, 0-disabled.

Remarks:
    None.

Example:
     None.

See Also:
    None.
*****************************************************************************/
UINT8 VWD_GetStatus(void)
{
    UINT8 stat;
    
    REG_BIT_CLR(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* unlock WD register */
    stat = REG_BIT_GET(REG_TIMER_TCTRL, REG_TIMER_TCTRL_WDCEN_BIT);     /* get WD status */
    REG_BIT_SET(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* lock WD register */

    return stat;
}

/******************************************************************************
Description:
     Get the count of watchdog timer.

Parameters:
     None.
    
Return Value:
    Count of watchdog timer.

Remarks:
    None

Example:
     None.

See Also:
    None.
*****************************************************************************/
UINT32 VWD_GetCount(void)
{
    UINT32 value0 = 0;
    UINT32 value1 = 0;
    UINT32 value2 = 0;
    UINT32 cnt;
        
    REG_BIT_CLR(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* unlock WD register */
    value0 = READ_REG(REG_TIMER_WDCNT);
    value1 = READ_REG(REG_TIMER_WDCNT);
    value2 = READ_REG(REG_TIMER_WDCNT);
    if (value0 == value1 || value0 == value2)
        cnt = value0;
    else if (value1 == value2)
        cnt = value1;
    else
        cnt = value2;
    REG_BIT_SET(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* lock WD register */

    return cnt;
}

/******************************************************************************
Description:
     Kick watchdog timer.

Parameters:
     None.
    
Return Value:
    None

Remarks:
    None.

Example:
     None.

See Also:
    None.
*****************************************************************************/

void VWD_Feed(void)
{
    REG_BIT_CLR(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* unlock WD register */
	WRITE_REG(REG_TIMER_WDFEED, 0x01);									//arbitrary value
    REG_BIT_SET(REG_TIMER_WDLOCK, REG_TIMER_WDLOCK_WD_LOCK_BIT);         /* lock WD register */
}


