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

/*
 * Code contributions:
 *
 * (Jul 28 2007)  Herman ten Brugge <hermantenbrugge@home.nl>:
 *
 * - Add 64 bit support. It now runs on x86_64 and solaris64.
 * - I also tested this on vxworks/32and solaris/32 and i386/32 processors.
 * - Remove assembly code. I could not measure any performance difference 
 *   on my core2 processor. This also makes the code more portable.
 * - Moved defines/typedefs from tlsf.h to tlsf.c
 * - Changed MIN_BLOCK_SIZE to sizeof (free_ptr_t) and BHDR_OVERHEAD to 
 *   (sizeof (bhdr_t) - MIN_BLOCK_SIZE). This does not change the fact 
 *    that the minumum size is still sizeof 
 *   (bhdr_t).
 * - Changed all C++ comment style to C style. (// -> /.* ... *./)
 * - Used ls_bit instead of ffs and ms_bit instead of fls. I did this to 
 *   avoid confusion with the standard ffs function which returns 
 *   different values.
 * - Created set_bit/clear_bit fuctions because they are not present 
 *   on x86_64.
 * - Added locking support + extra file target.h to show how to use it.
 * - Added get_used_size function (REMOVED in 2.4)
 * - Added rtl_realloc and rtl_calloc function
 * - Implemented realloc clever support.
 * - Added some test code in the example directory.
 * - Bug fixed (discovered by the rockbox project: www.rockbox.org).       
 *
 * (Oct 23 2006) Adam Scislowicz: 
 *
 * - Support for ARMv5 implemented
 *
 */

#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"



#include "vsys_mem_tlsf.h"

#if !defined(__GNUC__)
#ifndef __inline__
#define __inline__
#endif
#endif


 #undef ASSERT
 #define ASSERT(expr) if (!(expr)) {while(1);}   //cxt 2020.08.20      close ASSERT,reduce MIPS



/******************************************************************/
/**************     Helping functions    **************************/
/******************************************************************/
__INLINE__ void set_bit(int nr, u32_t * addr);
__INLINE__ void clear_bit(int nr, u32_t * addr);
__INLINE__ int ls_bit(int x);
__INLINE__ int ms_bit(int x);
__INLINE__ void MAPPING_SEARCH(size_t * _r, int *_fl, int *_sl);
__INLINE__ void MAPPING_INSERT(size_t _r, int *_fl, int *_sl);
__INLINE__ bhdr_t *FIND_SUITABLE_BLOCK(tlsf_t * _tlsf, int *_fl, int *_sl);
__INLINE__ bhdr_t *process_area(void *area, size_t size);
#if USE_SBRK || USE_MMAP
__INLINE__ void *get_new_area(size_t * size);
#endif
//static void tlsf_set_pool_size(size_t size);
static size_t tlsf_get_pool_size(void *mp);

//static size_t _tlsf_pool_size;
static const char table[] = {
    (char)-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4,
    4, 4,
    4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5,
    5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6,
    6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6,
    6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7,
    7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7,
    7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7,
    7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7,
    7, 7, 7, 7, 7, 7, 7
};

int is_addr_in_pool(void *pool, void *addr){
        tlsf_t *tlsf;
        area_info_t *area;
         
    tlsf = (tlsf_t *) pool;
        /* Check if already initialised */
    if (tlsf->tlsf_signature != TLSF_SIGNATURE)  return FALSE;
                
                area = tlsf->area_head;
          while(area != NULL) {
                        if(((UINT32)addr >=(UINT32)area) && 
                                ((UINT32)addr <(UINT32)(area->end)))
                          return TRUE;
                        area = area->next;
                }
    
        return FALSE;
}

/// return valid pool size
static int pool_size(void *pool){
    int sz = 0;
    tlsf_t *tlsf;
    area_info_t *area;
     
    tlsf = (tlsf_t *) pool;
    /* Check if already initialised */
    if (tlsf->tlsf_signature != TLSF_SIGNATURE)  return sz;
        
    area = tlsf->area_head;
    while(area != NULL) {
        sz += (int)((UINT32)(area->end) - (UINT32)area);
        area = area->next;
    }
    
    return sz;
}

static int is_valid_addr(void *pool, void *addr) {
    tlsf_t *tlsf;     
    tlsf = (tlsf_t *) pool;


    if(addr == NULL) return 1;
    if(pool == NULL) return 1;
    /* Check if already initialised */
    if (tlsf->tlsf_signature != TLSF_SIGNATURE)  return 1;

    return is_addr_in_pool(pool, addr);
}

// least significant bit, from 0 to 31.
static __inline__ int ls_bit(int i)
{
    #if (defined __GNUC__)
    unsigned int a;
    unsigned int x = i & -i;

    a = x <= 0xffff ? (x <= 0xff ? 0 : 8) : (x <= 0xffffff ? 16 : 24);
    return table[x >> a] + a;
        #elif (defined __CC_ARM)
                return (31UL - ( u32_t ) __clz( (u32_t)(i & -i)));
        #endif
}

// most significant bit, from 0 to 31.
static __inline__ int ms_bit(int i)
{
    #if (defined __GNUC__)
    unsigned int a;
    unsigned int x = (unsigned int) i;

    a = x <= 0xffff ? (x <= 0xff ? 0 : 8) : (x <= 0xffffff ? 16 : 24);
    return table[x >> a] + a;
        #elif (defined __CC_ARM)
                return (31UL - ( u32_t ) __clz( (u32_t)i ) );
                //return (31UL - ( u32_t )__builtin_clz((u32_t)i ));
        #endif
        
}

static __inline__ void set_bit(int nr, u32_t * addr)
{
        #if 0
    addr[nr >> 5] |= 1 << (nr & 0x1f);
        #else
                *addr |= 1<<nr;
        #endif
}

static __inline__ void clear_bit(int nr, u32_t * addr)
{ 
        #if 0
    addr[nr >> 5] &= ~(1 << (nr & 0x1f));
        #else
                *addr &= ~(1<<nr);
        #endif
}

// example:
// When the user requests 130B, 
// the algorithm will search a free block from list {FLI=7, SLI=1} whose block sizes belongs to [136, 143],
// as if the request size is 136.
#if 1
STATIC __INLINE__ void MAPPING_SEARCH(size_t * _r, int *_fl, int *_sl)
{
    int _t;

    if (*_r < SMALL_BLOCK) {
        *_fl = 0;
        *_sl = *_r / (SMALL_BLOCK / MAX_SLI);
    } else {
        _t = (1 << (ms_bit(*_r) - MAX_LOG2_SLI)) - 1;
        *_r = *_r + _t;
        *_fl = ms_bit(*_r);
        *_sl = (*_r >> (*_fl - MAX_LOG2_SLI)) - MAX_SLI;
        *_fl -= FLI_OFFSET;
        /*if ((*_fl -= FLI_OFFSET) < 0) // FL wil be always >0!
         *_fl = *_sl = 0;
         */
        *_r &= ~_t;
    }
}
#else //cxt
__INLINE__ void MAPPING_SEARCH(size_t * _r, int *_fl, int *_sl)
{
    int _t,_sz = *_r;

    if (_sz < SMALL_BLOCK) {
        *_fl = 0;
        *_sl = _sz / (SMALL_BLOCK / MAX_SLI);
    } else {
        _t = (1 << (ms_bit(_sz) - MAX_LOG2_SLI)) - 1;
        _sz = _sz + _t;
        *_fl = ms_bit(_sz);
        *_sl = (_sz >> (*_fl - MAX_LOG2_SLI)) - MAX_SLI;
        *_fl -= FLI_OFFSET;
        /*if ((*_fl -= FLI_OFFSET) < 0) // FL wil be always >0!
         *_fl = *_sl = 0;
         */
        _sz &= ~_t;
    }
}
#endif
// example:
// When the user frees 130B, 
// the algorithm will insert it to list {FLI=7, SLI=0} whose block sizes belongs to [128, 135].
STATIC __INLINE__ void MAPPING_INSERT(size_t _r, int *_fl, int *_sl)
{
    if (_r < SMALL_BLOCK) {
        *_fl = 0;
        *_sl = _r / (SMALL_BLOCK / MAX_SLI);
    } else {
        *_fl = ms_bit(_r);
        *_sl = (_r >> (*_fl - MAX_LOG2_SLI)) - MAX_SLI;
        *_fl -= FLI_OFFSET;
    }
#if _DEBUG_TLSF_         
        if(*_fl >=REAL_FLI) {
                PRINT_MSG("tlsf error size.\r\n", _r);
                while(1);
        }
#endif  
}

// example:
// When the user requests 130B, 
// the algorithm will search lists {FLI=7, SLI>=1} at first, if there are no free blocks, it will then search lists {FLI >7}. 
// fl and sl will be changed to the list with free blocks.
STATIC __INLINE__ bhdr_t *FIND_SUITABLE_BLOCK(tlsf_t * _tlsf, int *_fl, int *_sl)
{
    u32_t _tmp = _tlsf->sl_bitmap[*_fl] & (~0 << *_sl);
    bhdr_t *_b = NULL;

    if (_tmp) {
        *_sl = ls_bit(_tmp);
        if (*_sl < MAX_SLI) //cxt 2020.08.20 Check the return value
            _b = _tlsf->matrix[*_fl][*_sl];
    } else {
        *_fl = ls_bit(_tlsf->fl_bitmap & (~0 << (*_fl + 1)));
        if (*_fl > 0 && *_fl < REAL_FLI) { /* likely */
            *_sl = ls_bit(_tlsf->sl_bitmap[*_fl]);
            _b = _tlsf->matrix[*_fl][*_sl];
        }
    }
    return _b;
}

// After the header of the list {fl, sl} is allocated, its header pointer shall be updated.
// If the list becomes empty, the sl bitmap shall be cleared.
// If the list {fl, sl = any} becomes empty, the fl bitmap shall be cleared.
#define _EXTRACT_BLOCK_HDR(_b, _tlsf, _fl, _sl) do {                                    \
                _tlsf -> matrix [_fl] [_sl] = _b -> ptr.free_ptr.next;          \
                if (_tlsf -> matrix[_fl][_sl])                                                          \
                        _tlsf -> matrix[_fl][_sl] -> ptr.free_ptr.prev = NULL;  \
                else {                                                                                                          \
                        clear_bit (_sl, &_tlsf -> sl_bitmap [_fl]);                             \
                        if (!_tlsf -> sl_bitmap [_fl])                                                  \
                                clear_bit (_fl, &_tlsf -> fl_bitmap);                           \
                }                                                                                                                       \
                _b -> ptr.free_ptr.prev =  NULL;                                \
                _b -> ptr.free_ptr.next =  NULL;                                \
        }while(0)

static inline void EXTRACT_BLOCK_HDR(bhdr_t *_b, tlsf_t *_tlsf, int _fl, int _sl) {
    ASSERT(is_valid_addr(_tlsf, _b));

    ASSERT(is_valid_addr(_tlsf, _b -> ptr.free_ptr.next));
    _tlsf -> matrix [_fl] [_sl] = _b -> ptr.free_ptr.next;      
    if (_tlsf -> matrix[_fl][_sl])                              
        _tlsf -> matrix[_fl][_sl] -> ptr.free_ptr.prev = NULL;  
    else {                                                      
        clear_bit (_sl, &_tlsf -> sl_bitmap [_fl]);             
        if (!_tlsf -> sl_bitmap [_fl])                          
            clear_bit (_fl, &_tlsf -> fl_bitmap);              
    }                                                           
    _b -> ptr.free_ptr.prev =  NULL;                
    _b -> ptr.free_ptr.next =  NULL;                
}
// Extract a blcok which may not be the header of list {fl, ls}.
// This routine is used for extracting an already-freed block which is the neighbor of the current-freed one, and merge them.

#define _EXTRACT_BLOCK(_b, _tlsf, _fl, _sl) do {                                                        \
                if (_b -> ptr.free_ptr.next)                                                                    \
                        _b -> ptr.free_ptr.next -> ptr.free_ptr.prev = _b -> ptr.free_ptr.prev; \
                if (_b -> ptr.free_ptr.prev)                                                                    \
                        _b -> ptr.free_ptr.prev -> ptr.free_ptr.next = _b -> ptr.free_ptr.next; \
                if (_tlsf -> matrix [_fl][_sl] == _b) {                                                 \
                        _tlsf -> matrix [_fl][_sl] = _b -> ptr.free_ptr.next;           \
                        if (!_tlsf -> matrix [_fl][_sl]) {                                                      \
                                clear_bit (_sl, &_tlsf -> sl_bitmap[_fl]);                              \
                                if (!_tlsf -> sl_bitmap [_fl])                                                  \
                                        clear_bit (_fl, &_tlsf -> fl_bitmap);                           \
                        }                                                                                                                       \
                }                                                                                                                               \
                _b -> ptr.free_ptr.prev = NULL;                                 \
                _b -> ptr.free_ptr.next = NULL;                                 \
        } while(0)

static inline void EXTRACT_BLOCK(bhdr_t *_b, tlsf_t *_tlsf, int _fl, int _sl) {
    ASSERT(is_valid_addr(_tlsf, _b));

    if (_b -> ptr.free_ptr.next) {
                        ASSERT(is_valid_addr(_tlsf, _b -> ptr.free_ptr.prev));
        _b -> ptr.free_ptr.next -> ptr.free_ptr.prev = _b -> ptr.free_ptr.prev;
                }
    if (_b -> ptr.free_ptr.prev) {
                        ASSERT(is_valid_addr(_tlsf, _b -> ptr.free_ptr.next));
        _b -> ptr.free_ptr.prev -> ptr.free_ptr.next = _b -> ptr.free_ptr.next;
                }
    if (_tlsf -> matrix [_fl][_sl] == _b) {
        ASSERT(is_valid_addr(_tlsf,  _b -> ptr.free_ptr.next));
        _tlsf -> matrix [_fl][_sl] = _b -> ptr.free_ptr.next;
        if (!_tlsf -> matrix [_fl][_sl]) {
            clear_bit (_sl, &_tlsf -> sl_bitmap[_fl]);
            if (!_tlsf -> sl_bitmap [_fl])
                clear_bit (_fl, &_tlsf -> fl_bitmap);
        }
    }
    _b -> ptr.free_ptr.prev = NULL;
    _b -> ptr.free_ptr.next = NULL;
}
// The block is inserted to the list head, sl and fl bitmaps shall be set accordingly.
#define _INSERT_BLOCK(_b, _tlsf, _fl, _sl) do {                                                 \
                _b -> ptr.free_ptr.prev = NULL; \
                _b -> ptr.free_ptr.next = _tlsf -> matrix [_fl][_sl]; \
                if (_tlsf -> matrix [_fl][_sl])                                                                 \
                        _tlsf -> matrix [_fl][_sl] -> ptr.free_ptr.prev = _b;           \
                _tlsf -> matrix [_fl][_sl] = _b;                                                                \
                set_bit (_sl, &_tlsf -> sl_bitmap [_fl]);                                               \
                set_bit (_fl, &_tlsf -> fl_bitmap);                                                             \
        } while(0)

static inline void INSERT_BLOCK(bhdr_t *_b, tlsf_t *_tlsf, int _fl, int _sl) {
    ASSERT(is_valid_addr(_tlsf, _b));
    _b -> ptr.free_ptr.prev = NULL;
    ASSERT(is_valid_addr(_tlsf, _tlsf -> matrix [_fl][_sl]));
    _b -> ptr.free_ptr.next = _tlsf -> matrix [_fl][_sl];
    if (_tlsf -> matrix [_fl][_sl]) 
        _tlsf -> matrix [_fl][_sl] -> ptr.free_ptr.prev = _b;
    _tlsf -> matrix [_fl][_sl] = _b;
    set_bit (_sl, &_tlsf -> sl_bitmap [_fl]);
    set_bit (_fl, &_tlsf -> fl_bitmap);
}

#if USE_SBRK || USE_MMAP
__INLINE__ void *get_new_area(size_t * size) 
{
    void *area;

#if USE_SBRK
    area = (void *)sbrk(0);
    if (((void *)sbrk(*size)) != ((void *) -1))
        return area;
#endif

#ifndef MAP_ANONYMOUS
/* https://dev.openwrt.org/ticket/322 */
# define MAP_ANONYMOUS MAP_ANON
#endif


#if USE_MMAP
    *size = ROUNDUP(*size, PAGE_SIZE);
    if ((area = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) != MAP_FAILED)
        return area;
#endif
    return ((void *) ~0);
}
#endif

STATIC __INLINE__ bhdr_t *process_area(void *area, size_t size)
{
    bhdr_t *b, *lb, *ib;
    area_info_t *ai;

    ib = (bhdr_t *) area;
    ib->size =
        (sizeof(area_info_t) <
         MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(sizeof(area_info_t)) | USED_BLOCK | PREV_USED;
    b = (bhdr_t *) GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
    b->size = ROUNDDOWN_SIZE(size - 3 * BHDR_OVERHEAD - (ib->size & BLOCK_SIZE)) | USED_BLOCK | PREV_USED;
    b->ptr.free_ptr.prev = b->ptr.free_ptr.next = 0;
    lb = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    lb->prev_hdr = b;
    lb->size = 0 | USED_BLOCK | PREV_FREE;
    ai = (area_info_t *) ib->ptr.buffer;
    ai->next = 0;
    ai->end = lb;
    return ib;
}

/******************************************************************/
/******************** Begin of the allocator code *****************/
/******************************************************************/
// #if TLSF_STATISTIC
// tlsf_t *mp = NULL;
// #else
// static char *mp = NULL;         /* Default memory pool. */
// #endif

/******************************************************************/
size_t init_memory_pool(size_t mem_pool_size, void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf;
    bhdr_t *b, *ib;

    if (!mem_pool || !mem_pool_size || mem_pool_size < (sizeof(tlsf_t) + BHDR_OVERHEAD * 8) ) {
        return ((size_t)-1);
    }

    if (((unsigned long) mem_pool & PTR_MASK)) {
        return ((size_t)-1);
    }
    
    tlsf = (tlsf_t *) mem_pool;
    /* Check if already initialised */
    if (tlsf->tlsf_signature == TLSF_SIGNATURE) {
        b = GET_NEXT_BLOCK(mem_pool, ROUNDUP_SIZE(sizeof(tlsf_t)));
        return b->size & BLOCK_SIZE;
    }


    /* Zeroing the memory pool */
    vmemset(mem_pool, 0, sizeof(tlsf_t));

    TLSF_CREATE_LOCK(&tlsf->lock);

    ib = process_area(GET_NEXT_BLOCK
                      (mem_pool, ROUNDUP_SIZE(sizeof(tlsf_t))), ROUNDDOWN_SIZE(mem_pool_size - ROUNDUP_SIZE(sizeof(tlsf_t))));
    b = GET_NEXT_BLOCK(ib->ptr.buffer, ib->size & BLOCK_SIZE);
    free_ex(b->ptr.buffer, tlsf);
    tlsf->area_head = (area_info_t *) ib->ptr.buffer;

#if TLSF_STATISTIC
    tlsf->used_size = mem_pool_size - (b->size & BLOCK_SIZE);
    tlsf->peak_size = tlsf->used_size;
#endif

    tlsf->tlsf_signature = TLSF_SIGNATURE;

    return (b->size & BLOCK_SIZE);
}

int tlsf_is_addr_valid(void *mp,void *addr){
        return is_addr_in_pool(mp, addr);
}

/******************************************************************/
size_t add_new_area(void *area, size_t area_size, void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    area_info_t *ptr, *ptr_prev, *ai;
    bhdr_t *ib0, *b0, *lb0, *ib1, *b1, *lb1, *next_b;

    vmemset(area, 0, area_size);
    ptr = tlsf->area_head;
    ptr_prev = 0;

    ib0 = process_area(area, area_size);
    b0 = GET_NEXT_BLOCK(ib0->ptr.buffer, ib0->size & BLOCK_SIZE);
    lb0 = GET_NEXT_BLOCK(b0->ptr.buffer, b0->size & BLOCK_SIZE);

    /* Before inserting the new area, we have to merge this area with the
       already existing ones */

    while (ptr) {
        ib1 = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
        b1 = GET_NEXT_BLOCK(ib1->ptr.buffer, ib1->size & BLOCK_SIZE);
        lb1 = ptr->end;

        /* Merging the new area with the next physically contigous one */
        if ((unsigned long) ib1 == (unsigned long) lb0 + BHDR_OVERHEAD) {
            if (tlsf->area_head == ptr) {
                tlsf->area_head = ptr->next;
                ptr = ptr->next;
            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;
            }

            b0->size =
                ROUNDDOWN_SIZE((b0->size & BLOCK_SIZE) +
                               (ib1->size & BLOCK_SIZE) + 2 * BHDR_OVERHEAD) | USED_BLOCK | PREV_USED;

            b1->prev_hdr = b0;
            lb0 = lb1;

            continue;
        }

        /* Merging the new area with the previous physically contigous
           one */
        if ((unsigned long) lb1->ptr.buffer == (unsigned long) ib0) {
            if (tlsf->area_head == ptr) {
                tlsf->area_head = ptr->next;
                ptr = ptr->next;
            } else {
                ptr_prev->next = ptr->next;
                ptr = ptr->next;
            }

            lb1->size =
                ROUNDDOWN_SIZE((b0->size & BLOCK_SIZE) +
                               (ib0->size & BLOCK_SIZE) + 2 * BHDR_OVERHEAD) | USED_BLOCK | (lb1->size & PREV_STATE);
            next_b = GET_NEXT_BLOCK(lb1->ptr.buffer, lb1->size & BLOCK_SIZE);
            next_b->prev_hdr = lb1;
            b0 = lb1;
            ib0 = ib1;

            continue;
        }
        ptr_prev = ptr;
        ptr = ptr->next;
    }

    /* Inserting the area in the list of linked areas */
    ai = (area_info_t *) ib0->ptr.buffer;
    ai->next = tlsf->area_head;
    ai->end = lb0;
    tlsf->area_head = ai;
    free_ex(b0->ptr.buffer, mem_pool);
        #if TLSF_STATISTIC
    tlsf->used_size += area_size + BHDR_OVERHEAD;       
        tlsf->peak_size = tlsf->used_size;
        #endif
    return (b0->size & BLOCK_SIZE);
}


/******************************************************************/
size_t get_used_size(void *mem_pool)
{
/******************************************************************/
#if TLSF_STATISTIC
    return ((tlsf_t *) mem_pool)->used_size;
#else
    return 0;
#endif
}

/******************************************************************/
size_t get_max_size(void *mem_pool)
{
/******************************************************************/
#if TLSF_STATISTIC
    return ((tlsf_t *) mem_pool)->peak_size;
#else
    return 0;
#endif
}

/******************************************************************/
void destroy_memory_pool(void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;

    tlsf->tlsf_signature = 0;

    TLSF_DESTROY_LOCK(&tlsf->lock);

}

size_t tlsf_init(size_t mem_pool_size, void *mem_pool) {
   return init_memory_pool(mem_pool_size, mem_pool);
}

/******************************************************************/
void *tlsf_malloc(void *mp,size_t size)
{
/******************************************************************/
    void *ret;

#if USE_MMAP || USE_SBRK
    if (!mp) {
        size_t area_size;
        void *area;

        area_size = sizeof(tlsf_t) + BHDR_OVERHEAD * 8; /* Just a safety constant */
        area_size = (area_size > DEFAULT_AREA_SIZE) ? area_size : DEFAULT_AREA_SIZE;
        area = get_new_area(&area_size);
        if (area == ((void *) ~0))
            return NULL;        /* Not enough system memory */
        init_memory_pool(area_size, area);
    }
#endif

    TLSF_ACQUIRE_LOCK(&((tlsf_t *)mp)->lock);

    ret = malloc_ex(size, mp);

    TLSF_RELEASE_LOCK(&((tlsf_t *)mp)->lock);

    return ret;
}

/******************************************************************/
void tlsf_free(void *mp,void *ptr)
{
/******************************************************************/
                //int bit=1;
    TLSF_ACQUIRE_LOCK(&((tlsf_t *)mp)->lock);
        ASSERT( is_valid_addr(mp, ptr));

    free_ex(ptr, mp);

    TLSF_RELEASE_LOCK(&((tlsf_t *)mp)->lock);

}

/******************************************************************/
void *tlsf_realloc(void *mp,void *ptr, size_t size)
{
/******************************************************************/
    void *ret;

#if USE_MMAP || USE_SBRK
        if (!mp) {
                return tlsf_malloc(size);
        }
#endif

    TLSF_ACQUIRE_LOCK(&((tlsf_t *)mp)->lock);

    ret = realloc_ex(ptr, size, mp);

    TLSF_RELEASE_LOCK(&((tlsf_t *)mp)->lock);

    return ret;
}

/******************************************************************/
void *tlsf_calloc(void *mp,size_t nelem, size_t elem_size)
{
/******************************************************************/
    void *ret;

    TLSF_ACQUIRE_LOCK(&((tlsf_t *)mp)->lock);

    ret = calloc_ex(nelem, elem_size, mp);

    TLSF_RELEASE_LOCK(&((tlsf_t *)mp)->lock);

    return ret;
}

/******************************************************************/
void *malloc_ex(size_t size, void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    bhdr_t *b, *b2, *next_b;
    int fl, sl;
    size_t tmp_size;

    size = (size < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(size);

    /* Rounding up the requested size and calculating fl and sl */
    MAPPING_SEARCH(&size, &fl, &sl);

    /* Searching a free block, recall that this function changes the values of fl and sl,
       so they are not longer valid when the function fails */
    b = FIND_SUITABLE_BLOCK(tlsf, &fl, &sl);
#if USE_MMAP || USE_SBRK
    if (!b) {
        size_t area_size;
        void *area;
        /* Growing the pool size when needed */
        area_size = size + BHDR_OVERHEAD * 8;   /* size plus enough room for the requered headers. */
        area_size = (area_size > DEFAULT_AREA_SIZE) ? area_size : DEFAULT_AREA_SIZE;
        area = get_new_area(&area_size);        /* Call sbrk or mmap */
        if (area == ((void *) ~0))
            return NULL;        /* Not enough system memory */
        add_new_area(area, area_size, mem_pool);
        /* Rounding up the requested size and calculating fl and sl */
        MAPPING_SEARCH(&size, &fl, &sl);
        /* Searching a free block */
        b = FIND_SUITABLE_BLOCK(tlsf, &fl, &sl);
    }
#endif
    //ASSERT(is_valid_addr(mem_pool, b));
                
    if (!b)
        return NULL;            /* Not found */
                ASSERT(is_valid_addr(mem_pool, b));

    EXTRACT_BLOCK_HDR(b, tlsf, fl, sl);

    /*-- found: */
    next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    /* Should the block be split? */
    tmp_size = (b->size & BLOCK_SIZE) - size;
    if (tmp_size >= sizeof(bhdr_t)) {
        tmp_size -= BHDR_OVERHEAD;
        b2 = GET_NEXT_BLOCK(b->ptr.buffer, size);
        b2->size = tmp_size | FREE_BLOCK | PREV_USED;
        next_b->prev_hdr = b2;
        MAPPING_INSERT(tmp_size, &fl, &sl);
        INSERT_BLOCK(b2, tlsf, fl, sl);

        b->size = size | (b->size & PREV_STATE);
    } else {
        next_b->size &= (~PREV_FREE);
        b->size &= (~FREE_BLOCK);       /* Now it's used */
    }

    TLSF_ADD_SIZE(tlsf, b);
        ASSERT(is_valid_addr(mem_pool, b->ptr.buffer)); 

    return (void *) b->ptr.buffer;
}

/******************************************************************/
void free_ex(void *ptr, void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    bhdr_t *b, *tmp_b;
    int fl = 0, sl = 0;

    if (!ptr) {
        return;
    }

    // backward find the container block head
    b = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
    if((b->size & BLOCK_STATE) == FREE_BLOCK) {
#ifdef RELEASE_BUILD        
        return; // zuolongjun: Neglect duplicate free errors currently.
#else
        while(1);        
#endif        
    }

    // flag the block as free block
    b->size |= FREE_BLOCK;

    TLSF_REMOVE_SIZE(tlsf, b);

    // reset the free block link list
    b->ptr.free_ptr.prev = NULL;
    b->ptr.free_ptr.next = NULL;
    tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    if (tmp_b->size & FREE_BLOCK) {
        MAPPING_INSERT(tmp_b->size & BLOCK_SIZE, &fl, &sl);
        EXTRACT_BLOCK(tmp_b, tlsf, fl, sl);
        b->size += (tmp_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
    }
    if (b->size & PREV_FREE) {
        tmp_b = b->prev_hdr;
        MAPPING_INSERT(tmp_b->size & BLOCK_SIZE, &fl, &sl);
        EXTRACT_BLOCK(tmp_b, tlsf, fl, sl);
        tmp_b->size += (b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
        b = tmp_b;
    }
    MAPPING_INSERT(b->size & BLOCK_SIZE, &fl, &sl);
    INSERT_BLOCK(b, tlsf, fl, sl);

    tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    tmp_b->size |= PREV_FREE;
    tmp_b->prev_hdr = b;
}

/******************************************************************/
void *realloc_ex(void *ptr, size_t new_size, void *mem_pool)
{
/******************************************************************/
    tlsf_t *tlsf = (tlsf_t *) mem_pool;
    void *ptr_aux;
    unsigned int cpsize;
    bhdr_t *b, *tmp_b, *next_b;
    int fl, sl;
    size_t tmp_size;

    if (!ptr) {
        if (new_size)
            return (void *) malloc_ex(new_size, mem_pool);
        if (!new_size)
            return NULL;
    } else if (!new_size) {
        free_ex(ptr, mem_pool);
        return NULL;
    }

    b = (bhdr_t *) ((char *) ptr - BHDR_OVERHEAD);
    next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
    new_size = (new_size < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : ROUNDUP_SIZE(new_size);
    tmp_size = (b->size & BLOCK_SIZE);
    if (new_size <= tmp_size) {
        TLSF_REMOVE_SIZE(tlsf, b);
        if (next_b->size & FREE_BLOCK) {
            MAPPING_INSERT(next_b->size & BLOCK_SIZE, &fl, &sl);
            EXTRACT_BLOCK(next_b, tlsf, fl, sl);
            tmp_size += (next_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
            next_b = GET_NEXT_BLOCK(next_b->ptr.buffer, next_b->size & BLOCK_SIZE);
            /* We allways reenter this free block because tmp_size will
               be greater then sizeof (bhdr_t) */
        }
        tmp_size -= new_size;
        if (tmp_size >= sizeof(bhdr_t)) {
            tmp_size -= BHDR_OVERHEAD;
            tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, new_size);
            tmp_b->size = tmp_size | FREE_BLOCK | PREV_USED;
            next_b->prev_hdr = tmp_b;
            next_b->size |= PREV_FREE;
            MAPPING_INSERT(tmp_size, &fl, &sl);
            INSERT_BLOCK(tmp_b, tlsf, fl, sl);
            b->size = new_size | (b->size & PREV_STATE);
        }
        TLSF_ADD_SIZE(tlsf, b);
        return (void *) b->ptr.buffer;
    }
    if ((next_b->size & FREE_BLOCK)) {
        if (new_size <= (tmp_size + (next_b->size & BLOCK_SIZE))) {
                        TLSF_REMOVE_SIZE(tlsf, b);
            MAPPING_INSERT(next_b->size & BLOCK_SIZE, &fl, &sl);
            EXTRACT_BLOCK(next_b, tlsf, fl, sl);
            b->size += (next_b->size & BLOCK_SIZE) + BHDR_OVERHEAD;
            next_b = GET_NEXT_BLOCK(b->ptr.buffer, b->size & BLOCK_SIZE);
            next_b->prev_hdr = b;
            next_b->size &= ~PREV_FREE;
            tmp_size = (b->size & BLOCK_SIZE) - new_size;
            if (tmp_size >= sizeof(bhdr_t)) {
                tmp_size -= BHDR_OVERHEAD;
                tmp_b = GET_NEXT_BLOCK(b->ptr.buffer, new_size);
                tmp_b->size = tmp_size | FREE_BLOCK | PREV_USED;
                next_b->prev_hdr = tmp_b;
                next_b->size |= PREV_FREE;
                MAPPING_INSERT(tmp_size, &fl, &sl);
                INSERT_BLOCK(tmp_b, tlsf, fl, sl);
                b->size = new_size | (b->size & PREV_STATE);
            }
                        TLSF_ADD_SIZE(tlsf, b);
            return (void *) b->ptr.buffer;
        }
    }
        ptr_aux = malloc_ex(new_size, mem_pool);
    if (!ptr_aux){
        return NULL;
    }      
    
    cpsize = ((b->size & BLOCK_SIZE) > new_size) ? new_size : (b->size & BLOCK_SIZE);

    vmemcpy(ptr_aux, ptr, cpsize);

    free_ex(ptr, mem_pool);
    return ptr_aux;
}


/******************************************************************/
void *calloc_ex(size_t nelem, size_t elem_size, void *mem_pool)
{
/******************************************************************/
    void *ptr;

    if (nelem <= 0 || elem_size <= 0)
        return NULL;
        ptr = malloc_ex(nelem * elem_size, mem_pool);
    if (!ptr)
        return NULL;
    vmemset(ptr, 0, nelem * elem_size);

    return ptr;
}


static size_t tlsf_get_pool_size(void *mp)
{
        
        return (size_t) pool_size(mp);
}


