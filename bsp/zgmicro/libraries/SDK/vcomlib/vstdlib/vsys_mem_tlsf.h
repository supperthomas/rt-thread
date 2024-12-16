/*
 * Two Levels Segregate Fit memory allocator (TLSF)
 * Version 2.4.6
 *
 * Written by Miguel Masmano Tello <mimastel@doctor.upv.es>
 *
 * Thanks to Ismael Ripoll for his suggestions and reviews
 *
 * Copyright (C) 2008, 2007, 2006, 2005, 2004
 *
 * This code is released using a dual license strategy: GPL/LGPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of the GNU General Public License Version 2.0
 * Released under the terms of the GNU Lesser General Public License Version 2.1
 *
 */

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _TLSF_H_
#define _TLSF_H_

#ifndef TLSF_USE_LOCKS
#define	TLSF_USE_LOCKS 	(1)
#endif


#ifdef FPGA_QA_BUILD
#define	TLSF_STATISTIC 	(1)
#endif

#ifndef TLSF_STATISTIC
#define	TLSF_STATISTIC 	(1)
#endif

#ifndef USE_MMAP
#define	USE_MMAP 	(0)
#endif

#ifndef USE_SBRK
#define	USE_SBRK 	(0)
#endif

#if TLSF_USE_LOCKS
#define TLSF_MLOCK_T                UINT32
#define TLSF_CREATE_LOCK(_lock)     ( *(_lock) = (TLSF_MLOCK_T)0)
#define TLSF_DESTROY_LOCK(_lock)	( *(_lock) = (TLSF_MLOCK_T)0)
#define TLSF_ACQUIRE_LOCK(_lock)    do { *(_lock) = VCPU_SR_Save();} while (0)
#define TLSF_RELEASE_LOCK(_lock)    do { VCPU_SR_Restore(*(_lock));} while (0)
#endif

#if TLSF_STATISTIC
#define	TLSF_ADD_SIZE(tlsf, b) do {									\
		tlsf->used_size += (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;	\
		if (tlsf->used_size > tlsf->peak_size) 						\
			tlsf->peak_size = tlsf->used_size;						\
		} while(0)

#define	TLSF_REMOVE_SIZE(tlsf, b) do {								\
		tlsf->used_size -= (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;	\
	} while(0)
#else
#define	TLSF_ADD_SIZE(tlsf, b)	     do{}while(0)
#define	TLSF_REMOVE_SIZE(tlsf, b)    do{}while(0)
#endif

#if USE_MMAP || USE_SBRK
#include <unistd.h>
#endif

#if USE_MMAP
#include <sys/mman.h>
#endif

 	
/*************************************************************************/
/* Definition of the structures used by TLSF */


/* Some IMPORTANT TLSF parameters */
/* Unlike the preview TLSF versions, now they are statics */
#define BLOCK_ALIGN (sizeof(void *) * 2)

// FLI
// from 6 to 11, offset by 5. {FLI = 0} = [1,63], {FLI = 1} = [64,127]
#define MAX_FLI		(18) // manage pool of 256KB at the most.
#define FLI_OFFSET	(5) // tlsf structure just will manage blocks bigger than 64 bytes
#define REAL_FLI	(MAX_FLI - FLI_OFFSET)
#define SMALL_BLOCK	(1 <<(FLI_OFFSET+1))
// SLI
// from 0 to 7, divide each FLI into 8 ranges.
#define MAX_LOG2_SLI 	(3)
#define MAX_SLI		(1 << MAX_LOG2_SLI)     /* MAX_SLI = 2^MAX_LOG2_SLI */

#define MIN_BLOCK_SIZE	(sizeof (free_ptr_t))
#define BHDR_OVERHEAD	(sizeof (bhdr_t) - MIN_BLOCK_SIZE)
#define TLSF_SIGNATURE	(0x2A59FA59)

#define	PTR_MASK	(sizeof(void *) - 1)
#define BLOCK_SIZE	(0xFFFFFFFF - PTR_MASK)

#define GET_NEXT_BLOCK(_addr, _r) ((bhdr_t *) ((char *) (_addr) + (_r)))
#define	MEM_ALIGN		  ((BLOCK_ALIGN) - 1)
#define ROUNDUP_SIZE(_r)          (((_r) + MEM_ALIGN) & ~MEM_ALIGN)
#define ROUNDDOWN_SIZE(_r)        ((_r) & ~MEM_ALIGN)
#define ROUNDUP(_x, _v)           ((((~(_x)) + 1) & ((_v)-1)) + (_x))

#define BLOCK_STATE	(0x1)
#define PREV_STATE	(0x2)

/* bit 0 of the block size */
#define FREE_BLOCK	(0x1)
#define USED_BLOCK	(0x0)

/* bit 1 of the block size */
#define PREV_FREE	(0x2)
#define PREV_USED	(0x0)


#define DEFAULT_AREA_SIZE (1024*10)

#ifdef USE_MMAP
#define PAGE_SIZE (getpagesize())
#endif

typedef unsigned int u32_t;     /* NOTE: Make sure that this type is 4 bytes long on your computer */
typedef unsigned char u8_t;     /* NOTE: Make sure that this type is 1 byte on your computer */

typedef struct free_ptr_struct {
    struct bhdr_struct *prev;
    struct bhdr_struct *next;
} free_ptr_t;

typedef struct bhdr_struct {
    /* This pointer is just valid if the first bit of size is set */
    struct bhdr_struct *prev_hdr;
    /* The size is stored in bytes */
    size_t size;                /* bit 0 indicates whether the block is used and */
    /* bit 1 allows to know whether the previous block is free */
    union {
        struct free_ptr_struct free_ptr;
        u8_t buffer[1];         /*sizeof(struct free_ptr_struct)]; */
    } ptr;
} bhdr_t;

/* This structure is embedded at the beginning of each area, giving us
 * enough information to cope with a set of areas */

typedef struct area_info_struct {
    bhdr_t *end;
    struct area_info_struct *next;
} area_info_t;

typedef struct TLSF_struct {
    /* the TLSF's structure signature */
    u32_t tlsf_signature;

#if TLSF_USE_LOCKS
    TLSF_MLOCK_T lock;
#endif

#if TLSF_STATISTIC
    /* These can not be calculated outside tlsf because we
     * do not know the sizes when freeing/reallocing memory. */
    size_t used_size;
    size_t peak_size;
#endif

    /* A linked list holding all the existing areas */
    area_info_t *area_head;

    /* the first-level bitmap */
    /* This array should have a size of REAL_FLI bits */
    u32_t fl_bitmap;

    /* the second-level bitmap */
    u32_t sl_bitmap[REAL_FLI];

    bhdr_t *matrix[REAL_FLI][MAX_SLI];
} tlsf_t;



/* reentrant */
extern size_t init_memory_pool(size_t, void *);
extern size_t get_used_size(void *);
extern size_t get_max_size(void *);
extern void destroy_memory_pool(void *);
extern size_t add_new_area(void *, size_t, void *);
extern void *malloc_ex(size_t, void *);
extern void free_ex(void *, void *);
extern void *realloc_ex(void *, size_t, void *);
extern void *calloc_ex(size_t, size_t, void *);

/*** global use, non-reentrant ***/
extern size_t tlsf_init(size_t, void *);
extern void *tlsf_malloc(void *mp,size_t size);
extern void tlsf_free(void *mp,void *ptr);
extern void *tlsf_realloc(void *mp,void *ptr, size_t size);
extern void *tlsf_calloc(void *mp,size_t nelem, size_t elem_size);
int tlsf_is_addr_valid(void *mp,void *addr);

#endif
