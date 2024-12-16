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
        

Description:
        

Revision History:
        Date                  Author                   Description
        2020.12.05           ming.m              initial version
------------------------------------------------------------------------------*/

#ifndef __VSTDLIB_H__
#define __VSTDLIB_H__
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDLIB_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES */
#include "vstdcfg.h"

#include "vtypes.h"
#include "vlist.h"
#include "vsys_sharemem.h"

#define PRIV_STDLIB_EN               0

#define VCOMMON_StrLen  vstrlen
#define VCOMMON_StrCpy  vstrcpy
#define VCOMMON_StrCmp  vstrcmp
#define VCOMMON_MemSet  vmemset
#define VCOMMON_MemCpy  vmemcpy
#define VCOMMON_MemMove vmemmove
#define VCOMMON_MemCmp  vmemcmp
#define VCOMMON_MemChr  vmemchr
#define VCOMMON_ItoA    vitoa
#define VCOMMON_Delay   vdelayus
#define VSYS_MEM_Malloc vmalloc
#define VSYS_MEM_Calloc vcalloc
#define VSYS_MEM_Free   vfree

extern UINT32  vstrlen(const SINT8 *p_Str);
extern void    vstrcpy(SINT8 *p_Dest, const SINT8 *p_Src);
extern void    vstrncpy(SINT8 *p_Dest, const SINT8 *p_Src, UINT32 NumOfBytes);
extern SINT32  vstrcmp(const SINT8 *p_Str1, const SINT8 *p_Src2);
extern SINT32  vstrncmp(const SINT8 *p_Str1, const SINT8 *p_Src2, UINT32 NumOfBytes);
extern void    vmemset(void *p_Buf, const UINT8 Value, UINT32 NumOfBytes);
extern void *  vmemcpy(void *p_Dest, const void *p_Src, UINT32 NumOfBytes);
extern void*   vmemmove(void* p_Dest, const void* p_Src, UINT32 NumOfBytes);
extern SINT32  vmemcmp(const void *p_Buf1, const void *p_Buf2, UINT32 NumOfBytes);
extern void *  vmemchr(const void * p_Buf, UINT8 Ch, UINT32 NumOfBytes);
extern void    vitoa(SINT64 Num, UINT8* p_Str, UINT8 Radix);
extern void    vdelayus(UINT32 NumOfMicroSec);
extern UINT32  vtimeusget(void);
extern UINT32  vtimeusinterval(UINT32 start, UINT32 end);
extern void *  vmalloc(UINT32 Size);
extern void    vfree(void *p_Addr);
extern void *  vrealloc(void *p_Addr,UINT32 Size);
extern void *  vcalloc(size_t nelem, size_t Size);
extern void    vmemsetowner(void *p_mem, UINT32 LR);
extern void    vmemsetarg(void *p_mem, UINT8 arg) ;
extern int     vcom_sprintf(SINT8 *buf, const char *fmt, ...);

extern void	   VSYS_MEM_SetType(void *p_mem, UINT8 type);
extern SINT32  VSYS_MEM_CheckAddr(void *p_Addr);
extern UINT32  VCOMMON_GetRand(UINT32 lower_limit, UINT32 upper_limit);
extern void    VCOMMON_GetRands(UINT8 *rand, UINT8 len);
extern UINT32  VCOMMON_GetHWVersion(void);
extern UINT8   VCOMMON_CRC7(const UINT8 *p_Data, UINT32 Len);
extern UINT16  VCOMMON_CRC16(const UINT8 *p_Data, UINT32 Len);
extern UINT16  VCOMMON_CRC16Ex(UINT16 crc, const UINT8 *p_Data, UINT32 Len);
extern UINT32  VCOMMON_CRC32(UINT32 crc, void* input, UINT32 len);
extern UINT8   VCOMMON_GetBitIdx(UINT32 BitMask);
extern UINT8   VCOMMON_CLZ(UINT32 wd);
extern UINT8   VCOMMON_MsBit(UINT32 BitMask);
extern UINT8   VCOMMON_LsBit(UINT32 BitMask);
extern UINT8   VCOMMON_CountOnes(UINT32 value);

enum BITDEFINEenum
{       BIT0=(1<<0),    BIT1=(1<<1),    BIT2=(1<<2),    BIT3=(1<<3),    BIT4=(1<<4),    BIT5=(1<<5),    BIT6=(1<<6),    BIT7=(1<<7),    BIT8=(1<<8),
        BIT9=(1<<9),    BIT10=(1<<10),  BIT11=(1<<11),  BIT12=(1<<12),  BIT13=(1<<13),  BIT14=(1<<14),  BIT15=(1<<15),  BIT16=(1<<16),  BIT17=(1<<17),
        BIT18=(1<<18),  BIT19=(1<<19),  BIT20=(1<<20),  BIT21=(1<<21),  BIT22=(1<<22),  BIT23=(1<<23),  BIT24=(1<<24),  BIT25=(1<<25),  BIT26=(1<<26),
        BIT27=(1<<27),  BIT28=(1<<28),  BIT29=(1<<29),  BIT30=(1<<30),  BIT31=(1<<30)
};

#ifndef ABS
#define ABS(a)                          ((a)>0?(a):-(a))
#endif
#define ABS_SUB(a, b)                   (((a) <(b))? ((b)-(a)) : ((a)-(b)))
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof(arr[0]))
#define BIT_MASK(x)                     ((UINT32)0x1 << (x))
#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                       (((x) < (y)) ? (x) : (y))
#define IS_DIGIT(x)                     (((x) >= '0') && ((x) <= '9'))

#define ASCII2INT(x)            ((x) - '0')
#define MAKE_U16(ptr)           (*((UINT8*)(ptr)) | (*((UINT8*)(ptr)+1) <<8))
#define MAKE_U24(ptr)           (*((UINT8*)(ptr)) | (*((UINT8*)(ptr)+1) <<8) | (*((UINT8*)(ptr)+2)<<16))
#define MAKE_U32(ptr)           (*((UINT8*)(ptr)) | (*((UINT8*)(ptr)+1) <<8) | (*((UINT8*)(ptr)+2)<<16) | (*((UINT8*)(ptr)+3) <<24))
#define SPLIT_U16(ptr, val)     {*((UINT8*)(ptr)) = (val) & 0xFF; *((UINT8*)(ptr)+1) = ((val) >>8)&0xFF;}
#define SPLIT_U24(ptr, val)     {*((UINT8*)(ptr)) = (val) & 0xFF; *((UINT8*)(ptr)+1) = ((val) >>8)&0xFF; *((UINT8*)(ptr)+2) = ((val) >>16)&0xFF;}
#define SPLIT_U32(ptr, val)     {*((UINT8*)(ptr)) = (val) & 0xFF; *((UINT8*)(ptr)+1) = ((val) >>8)&0xFF; *((UINT8*)(ptr)+2) = ((val) >>16)&0xFF; *((UINT8*)(ptr)+3) = ((val) >>24)&0xFF;}

#define SCATTER_U16(a)          ((a)&0xFF), (((a)>>8)&0xFF)
#define SCATTER_U24(a)          ((a)&0xFF), (((a)>>8)&0xFF), (((a)>>16)&0xFF)
#define SCATTER_U32(a)          ((a)&0xFF), (((a)>>8)&0xFF), (((a)>>16)&0xFF), (((a)>>24)&0xFF)
#define SCATTER_U16_BE(a)       (((a)>>8)&0xFF), ((a)&0xFF)
#define SCATTER_U24_BE(a)       (((a)>>16)&0xFF), (((a)>>8)&0xFF), ((a)&0xFF)
#define SCATTER_U32_BE(a)       (((a)>>24)&0xFF), (((a)>>16)&0xFF), (((a)>>8)&0xFF), ((a)&0xFF)

#define SWAP_ENDIAN24(Value)    ((((Value) & 0x0000FF) << 16) | (((Value) & 0x00FF00) << 0) | (((Value) & 0xFF0000) >> 16))

#define ALIGN_SIZE(x, n)        ((((x) + (n) -1) / n) * n)
#define ALIGN_256(x)            ALIGN_SIZE(x, 256)
#define ALIGN_32(x)       ALIGN_SIZE(x, 32)
#define ALIGN_16(x)       ALIGN_SIZE(x, 16)

#define voffsetof(type,member)           ((UINT32)(&(((type*)(NULL))->member)))
#define vcontainer_of(ptr, type, member) (type *)( (char *)ptr - voffsetof(type,member) )
#define membersizeof(type,member)       ((UINT32)(sizeof(((type*)(NULL))->member)))
#define GET_CRC_OFFSET(p)               ((UINT32)(&(((p*)(NULL))->crc)))


#if (PRIV_STDLIB_EN > 0)
/* std: <stdio.h> */
/*   std: <stdlib.h> */
int     abs (int);

double  atof (const char *__nptr);

int     atoi (const char *__nptr);
long    atol (const char *__nptr);
void    *calloc(size_t, size_t);
void    free (void *) ;
long    labs (long);
void    *malloc(size_t);

int     rand (void);
void    *realloc(void *, size_t);

void    srand (unsigned __seed);
double  strtod (const char * __n, char ** __end_PTR);
long    strtol (const char * __n, char ** __end_PTR, int __base);
unsigned long strtoul (const char * __n, char ** __end_PTR, int __base);

/*** std  <string.h> */
void *   memchr (const void *, int, size_t);
int      memcmp (const void *, const void *, size_t);
void *   memcpy (void *dst, const void *src, size_t sz);
void *   memmove (void *, const void *, size_t);
void *   memset (void *, int, size_t);
char    *strcat (char *, const char *);
char    *strchr (const char *, int);
int      strcmp (const char *, const char *);
int      strcoll (const char *, const char *);
char    *strcpy (char *, const char *);
size_t   strcspn (const char *, const char *);
char    *strerror (int);
size_t   strlen (const char *);
char    *strncat (char *, const char *, size_t);
int      strncmp (const char *, const char *, size_t);
char    *strncpy (char *, const char *, size_t);
char    *strpbrk (const char *, const char *);
char    *strrchr (const char *, int);
size_t   strspn (const char *, const char *);
char    *strstr (const char *, const char *);

#endif //#if (PRIV_STDLIB_EN > 0)

/*
 * va_list reading seems hard to do in a cross-platform way. Each platforms has its own implementation and I'm not sure it would work.
 * USE platform defined as default.
 */
#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
#include <stdarg.h>
#endif

#ifndef va_arg
typedef char *va_list;
#define _INTSIZEOF(n)    ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(ap, v)  (ap = (va_list) &v + _INTSIZEOF(v))
#define va_arg(ap, t)   (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap)       (ap = (va_list) 0)
#endif //#ifndef va_arg

#ifdef ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDLIB_H_
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDLIB_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDLIB_H_ */
#endif //#ifndef __VSTDLIB_H__
