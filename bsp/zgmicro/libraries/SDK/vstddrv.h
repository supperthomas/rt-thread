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
        irq

Description:
        irq Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef _VSTDDRV_H_
#define _VSTDDRV_H_
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDDRV_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES */
#include "vstdcfg.h"
#include "vstdlib.h"

/*Check whether a driver is used and mask the driver interface*/
#ifdef _VGPIO_H_
#error  application layer error using _VGPIO_H_ driver
#endif 
#define _VGPIO_H_

#ifdef _VCKM_H_
#error  application layer error using _VCKM_H_ driver
#endif 
#define _VCKM_H_

#ifdef _VUART_H_
#error  application layer error using _VUART_H_ driver
#endif 
#define _VUART_H_

#ifdef _VTIMER_H_
#error  application layer error using _VTIMER_H_ driver
#endif 
#define _VTIMER_H_

#ifdef _VI2C_H_
#error  application layer error using _VI2C_H_ driver
#endif 
#define _VI2C_H_

#ifdef _VLED_H_
#error  application layer error using _VLED_H_ driver
#endif 
#define _VLED_H_



#ifdef _VSPI_FLASH_H_
#error  application layer error using _VSPI_FLASH_H_ device
#endif 
#define _VSPI_FLASH_H_

//-------------drv
#include "vpla_gpio.h"
#include "vpla_ckm.h"
#include "vpla_uart.h"
#include "vpla_timer.h"
#include "vpla_i2c.h"
#include "vpla_led.h"
#include "vpla_trace.h"
//-------------dve
#include "vpla_flash.h"
#include "vpla_dsp.h"
//-------------


//-------------
#include "vdrv_mbox.h"
#include "vdrv_ipc.h"
#include "vdrv_wd.h"

#if VOSIPCModule_SUPPORT
#include "vdev_ipc.h"
#include "vos_ipc.h"
#include "vos_rpc.h"
#include "vpla_ipc_control.h"
#endif

#ifdef CORE_SEL_MCUTOP
#include "vdrv_iis.h"
#ifndef VA9655_SUPPORT
#include "vdrv_pmu.h"
#endif
#include "vdrv_cdc.h"
#if BOOT_LOADER
#include "vdrv_trng.h"
#include "vdrv_otg.h"
#endif
#else
#ifndef VA9655_SUPPORT
#include "vdrv_pmu.h"
#include "vdrv_trng.h"
#endif
#include "vdrv_cdc.h"
#include "vdrv_aif.h"
#include "vdrv_9900.h"
#include "vdrv_iis.h"
#include "vdrv_asrc.h"
#include "vdrv_spdif.h"
#include "vdrv_espi.h"
#ifdef VA9655_SUPPORT
#include "va7632b_cdc.h"
#endif 
#endif

//-------------

#include "vpla_aif.h"
#include "vpla_iis.h"
#include "vpla_pmu.h"
#include "vpla_cdc.h"



#ifdef __cplusplus
extern "C" {
#endif

void VDRV_EnableDebugPort ( void );

#ifdef __cplusplus
}
#endif

#ifdef ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDDRV_H_
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDDRV_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDDRV_H_ */
#endif  /* _VSTDDRV_H_ */
