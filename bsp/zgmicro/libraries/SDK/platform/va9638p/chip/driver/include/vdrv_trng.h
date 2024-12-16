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
    2020.04.26            ly                        initial version
------------------------------------------------------------------------------*/

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VTRNG_H_
#define _VTRNG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TRNG_NOP             0
#define TRNG_GENERATE_RANDOM 1
#define TRNG_RESEED_RANDOM   2
#define TRNG_RESEED_NONCE    3

#define TRNG_RANDOM_NUM_128BIT   0
#define TRNG_RANDOM_NUM_256BIT   1

#define TRNG_TEST   0
#if (TRNG_TEST == 1)
void TRNG_Test ( void );
#endif

#ifdef TRNG_GET_RANDOM_CACHE
void TRNG_Init ( UINT8 rand_num );
#endif
UINT32 TRNG_Get_Random ( void );

#ifdef __cplusplus
}
#endif

#endif  /* _VTRNG_H_ */


