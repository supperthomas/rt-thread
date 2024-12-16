#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VCORTEX_M3_H_
#define _VCORTEX_M3_H_
		
#ifdef __cplusplus 
	extern "C" {
#endif
#include "vstdlib.h"
#include "vnvic_system.h"
#include "vnvic_interrupt.h"
#include "vnvic_fpb.h"
#include "vnvic_isr.h"

#define __DMB()          	__dmb(0xF)     						/* DMB: data memory barrier */
#define __DSB()          	__dsb(0xF)     						/* DSB: data sync. barrier */
#define __ISB() 		 	__isb(0xF)     						/* ISB: instruction sync. barrier */

#undef ASSERT
#define ASSERT(x)

#ifdef __cplusplus
}
#endif 

#endif  /* _VCORTEX_M3_H_ */
