#include "vstdlib.h"
#define u_intHW                     UINT32

#define REG_MACRO
#ifndef REG_MACRO

extern  __ASM unsigned long REG(UINT32 x)
{
    MOV      r1,r0
    NOP
    NOP
    LDR      r0,[r1,#0]

    NOP
    NOP

    BX       lr  
    ALIGN
}


extern  __ASM unsigned long READ_REG(UINT32 x)
{
    MOV      r1,r0
    NOP
    NOP
		
    LDR      r0,[r1,#0]
    NOP
    NOP

    BX       lr     
    ALIGN
}

extern  __ASM void WRITE_REG(UINT32 x,unsigned long val)
{
    NOP
    NOP		
		
    STR      r1,[r0,#0]
    	
    NOP
    NOP		
		
    BX       lr
    MOVS     r0,r0
    ALIGN
}

extern  UINT8 GET_REG_BIT2(UINT32 reg, UINT8 bit)
{
    return (REG((((reg) - PERI_BASE) << 5) + PERI_BIT_BAND_BASE + ((bit) << 2)));
}

extern  __ASM void SET_REG_BIT2(UINT32 reg, UINT8 bit)
{
    PUSH     {r4,lr}
    MOV      r4,#0x42000000
    ADD      r3,r4,r0,LSL #5
    ADD      r2,r3,r1,LSL #2
 
    MOVS     r3,#1
	
    NOP
    NOP

    STR      r3,[r2,#0]
    
    NOP
    NOP		
  
    POP      {r4,pc}
    ALIGN
}

extern  __ASM void CLR_REG_BIT2(UINT32 reg, UINT8 bit)
{
    PUSH     {r4,lr}
    MOV      r4,#0x42000000
    ADD      r3,r4,r0,LSL #5
    ADD      r2,r3,r1,LSL #2
 	
    MOVS     r3,#0

	NOP
    NOP
		
    STR      r3,[r2,#0]
  
    NOP
    NOP		
		
    POP      {r4,pc}
    ALIGN
}

extern  UINT8 GET_REG_BIT(unsigned long bitband)
{
    return (REG(bitband));
}

extern  __ASM void SET_REG_BIT(unsigned long bitband)
{
    MOV      r1,r0
		
    MOVS     r2,#1

	NOP
    NOP
		
    STR      r2,[r1,#0]
   
    NOP
    NOP		
		
    BX       lr
    ALIGN
}

extern  __ASM void CLR_REG_BIT(unsigned long bitband)
{	
    MOV      r1,r0	
    MOVS     r2,#0

	NOP
    NOP
		
    STR      r2,[r1,#0]
  	
    NOP
    NOP		
		
    BX       lr
    ALIGN
}



extern  __ASM u_intHW GetHWEntry32(u_intHW Field_ADDR)
{

    MOV      r1,r0
    NOP
    NOP
	
    LDR      r0,[r1,#0]
    NOP
    NOP

    BX       lr 
    ALIGN
}

extern  __ASM void SetHWEntry32(u_intHW Field_ADDR,const u_intHW source)
{
    MOV      r2,r0
    NOP
    NOP
	
    STR      r1,[r2,#0]
    
    NOP
    NOP		
	
    BX       lr
    MOVS     r0,r0
    ALIGN
}
extern  __ASM void GetHWEntry64_Ex(u_intHW Field_ADDR,u_intHW *pDst)
{
    NOP
    NOP

    LDR      r2,[r0,#0]
    NOP
    NOP

    STR      r2,[r1,#0]
   	
    NOP
    NOP

    LDR      r0,[r0,#4]
    NOP
    NOP

    STR      r0,[r1,#4]
    	
    NOP
    NOP		

    BX       lr 
    ALIGN
}
extern  __ASM void SetHWEntry64(u_intHW Field_ADDR, const u_intHW *pSrc)
{
    NOP
    NOP

    LDR      r2,[r1,#0]
    NOP
    NOP

    STR      r2,[r0,#0]
   	
    NOP
    NOP

    LDR      r1,[r1,#4]
    NOP
    NOP

    STR      r1,[r0,#4]
    	
    NOP
    NOP		

    BX       lr           
    ALIGN
}

extern  __ASM void GetHWEntry128_Ex(u_intHW Field_ADDR,u_intHW *pDst)
{ 
    NOP
    NOP

    LDR      r2,[r0,#0]
    NOP
    NOP

    STR      r2,[r1,#0]
    	
    NOP
    NOP

    LDR      r2,[r0,#4]
    NOP
    NOP

    STR      r2,[r1,#4]
    	
    NOP
    NOP

    LDR      r2,[r0,#8]
    NOP
    NOP

    STR      r2,[r1,#8]
    	
    NOP
    NOP

    LDR      r0,[r0,#0xc]
    NOP
    NOP

    STR      r0,[r1,#0xc]
    	
    NOP
    NOP		

    BX       lr
    ALIGN
}

extern  __ASM void SetHWEntry128(u_intHW Field_ADDR, const u_intHW *pSrc)
{
    NOP
    NOP

    LDR      r2,[r1,#0]
    NOP
    NOP

    STR      r2,[r0,#0]
   	
    NOP
    NOP

    LDR      r2,[r1,#4]
    NOP
    NOP

    STR      r2,[r0,#4]
    	
    NOP
    NOP

    LDR      r2,[r1,#8]
    NOP
    NOP
 
    STR      r2,[r0,#8]
   	
    NOP
    NOP

    LDR      r1,[r1,#0xc]
    NOP
    NOP

    STR      r1,[r0,#0xc]
    	
    NOP
    NOP		

    BX       lr
    ALIGN
}

#else

//#define __INLINE__ __inline

 u_intHW GetHWEntry32(u_intHW Field_ADDR)
{
    u_intHW volatile *pdata =  (u_intHW volatile*)Field_ADDR;
    return *pdata;
}

 void SetHWEntry32(u_intHW Field_ADDR,const u_intHW source)
{
   u_intHW volatile *pdata =  (u_intHW volatile*)Field_ADDR;
   *pdata = source;
}
 void GetHWEntry64_Ex(u_intHW Field_ADDR,u_intHW *pDst)
{
    u_intHW volatile *pSrc = (u_intHW volatile*)Field_ADDR; 
    *(pDst) =  *pSrc; 
    *((pDst) + 1) =  *(++pSrc);             
}
 void SetHWEntry64(u_intHW Field_ADDR, const u_intHW *pSrc)
{
    u_intHW volatile *pDst = (u_intHW volatile*)Field_ADDR; 
    *pDst = *((pSrc)); 
    *(++pDst) = *((pSrc) + 1);     
}

 void GetHWEntry128_Ex(u_intHW Field_ADDR,u_intHW *pDst)
{ 
    u_intHW volatile *pSrc = (u_intHW volatile*)Field_ADDR; 
    *(pDst) =  *pSrc; 
    *((pDst) + 1) =  *(++pSrc); 
    *((pDst) + 2) =  *(++pSrc); 
    *((pDst) + 3) =  *(++pSrc); 
}

 void SetHWEntry128(u_intHW Field_ADDR, const u_intHW *pSrc)
{
    u_intHW volatile *pDst = (u_intHW volatile*)Field_ADDR; 
    *pDst = *((pSrc)); 
    *(++pDst) = *((pSrc) + 1); 
    *(++pDst) = *((pSrc) + 2); 
    *(++pDst) = *((pSrc) + 3);
}            

#endif
