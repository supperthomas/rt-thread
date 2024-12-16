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

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VWD_H_
#define _VWD_H_

#ifdef __cplusplus 
extern "C" {
#endif

/* WD TMR clock input */
#define VWD_CLK_IN 						4000

/* bits definition Watch Dog Lock Control Register */
#define RBIT_TIMER_WDLOCK_LOCK 		0

/* bits definition TMR Control Register */
#define RBIT_TIMER_TCTRL_WDMODE 		13
#define RBIT_TIMER_TCTRL_WDCEN 		12
#define RBIT_TIMER_MASK_WD			3

typedef enum tag_VWD_MOD {
	WD_RST = 0,
	WD_NMI = 1
}VWD_MOD;


typedef struct wd_info_s {
	UINT32 magic;
	UINT32 period;	
	UINT32 wd_count;
	UINT8 is_enable;	
}wd_info_t;

#define VWD_MAGIC_CODE 0x55AA6688

void VWD_Enable(UINT8 Mode, UINT32 TimeoutMS);
void VWD_Disable(void);
UINT8 VWD_GetStatus(void);
UINT32 VWD_GetCount(void);
void VWD_Feed(void);


#ifdef __cplusplus
}
#endif 

#endif  /* _VWD_H_ */


