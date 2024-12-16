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
        COMMON

Description:
        Common data type & function definitions.

Revision History:
        Date                  Author                   Description
        2020.12.2              ming.m                   initilize
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VTYPES_H_
#define _VTYPES_H_

#ifdef __cplusplus 
extern "C" {
#endif

/* NULL pointer */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* NUL terminator of string */
#ifndef NUL
#define NUL ('\0')
#endif

#ifndef FALSE
#define FALSE ((unsigned char)0)
#endif

#ifndef TRUE
#define TRUE ((unsigned char)1)
#endif


#define DISABLE              0
#define ENABLE               1


#define STATIC           static
#define __INLINE          inline
#define __INLINE__        inline  
#define __inline__        inline 
#define __FUNC__         __func__
#define __ASM            __asm

#if (defined __GNUC__)
#define __IRQ  
#define FORCE_INLINE     __attribute__((always_inline))       
#define __EXTERN_INLINE__	static inline
#define __ALIGN(x)          __attribute__((aligned(x)))
#elif (defined __CC_ARM)
#define __IRQ            __irq
#define FORCE_INLINE     __forceinline
#define __EXTERN_INLINE__	extern inline
#define __ALIGN(x)             __align(x)
#endif

#define UNUSED(x)             (void)(x)     /* to avoid compiler warning */

#define __WEAK 			__attribute__((weak))

#define FUNCTION_NAME   __FUNC__        /* a text string of function name */
#define MODULE_NAME 	__FILE__ 		/* a text string of file (module) name */
#define LINE_NUMBER 	__LINE__ 		/* the numeric value of line number in source file */

/* common data type definitions */
typedef unsigned char                           BOOL;
typedef void                        			VOID;

typedef signed char                             SINT8;
typedef signed short                            SINT16;
typedef signed int                              SINT32;
typedef signed long long                        SINT64;

typedef unsigned char                           UINT8;
typedef unsigned short                          UINT16;
typedef unsigned int                            UINT32;
typedef unsigned long long                      UINT64;

typedef signed char                             int8_t;
typedef signed short                            int16_t;
typedef signed int                              int32_t;
typedef signed long long                        int64_t;

typedef unsigned char                           uint8_t;
typedef unsigned short                          uint16_t;
typedef unsigned int                            uint32_t;
typedef unsigned long long                      uint64_t;

typedef signed char                             INT8;
typedef signed short                            INT16;
typedef signed int                              INT32;
typedef signed long long                        INT64;

typedef UINT32                                  size_t;
typedef void *                                  HANDLE;

#if (defined __GNUC__)
typedef INT64	                               __int64;
#endif

#ifndef SINT8_MIN
#define SINT8_MIN          (-128)
#endif
#ifndef SINT8_MAX
#define SINT8_MAX          127
#endif
#ifndef UINT8_MIN
#define UINT8_MIN          0u
#endif
#ifndef UINT8_MAX
#define UINT8_MAX          255u
#endif

#ifndef SINT16_MIN
#define SINT16_MIN          (-32768)
#endif
#ifndef SINT16_MAX
#define SINT16_MAX          32767
#endif
#ifndef UINT16_MIN
#define UINT16_MIN          0u
#endif
#ifndef UINT16_MAX
#define UINT16_MAX          65535u
#endif

#ifndef SINT32_MIN
#define SINT32_MIN          (-2147483648)
#endif
#ifndef SINT32_MAX
#define SINT32_MAX          2147483647
#endif
#ifndef UINT32_MIN
#define UINT32_MIN          0u
#endif
#ifndef UINT32_MAX
#define UINT32_MAX          4294967295u
#endif

#ifndef SINT64_MIN
#define SINT64_MIN          (-9223372036854775808)
#endif
#ifndef SINT64_MAX
#define SINT64_MAX          9223372036854775807
#endif
#ifndef UINT64_MIN
#define UINT64_MIN          0u
#endif
#ifndef UINT64_MAX
#define UINT64_MAX          18446744073709551615u
#endif

/* Return value definitions */
typedef SINT32                                  		VRET_VALUE;
#define VRET_NO_ERROR                                   0x000
#define VRET_ERROR                                      0x001
#define VRET_NO_BUFFER                                  0x002
#define VRET_SUCCESS                                    VRET_NO_ERROR

#ifdef __cplusplus
}
#endif 

#endif  /* _VTYPES_H_ */

