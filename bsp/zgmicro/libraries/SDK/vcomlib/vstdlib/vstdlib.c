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
        2009.04.08           chenjingen              initial version
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"

 /**
 * @addtogroup String
 * 
 * @{
 */

///
///@brief 计算字符串的长度。
///
///@param p_Str 指向以 nul 结尾的字符串的指针。
///@return UINT32 字符串的长度（以字节为单位）。
///@note 返回值不包括 NUL 终止字符。
///
UINT32 vstrlen(const SINT8 *p_Str)
{
    UINT32 len;
	char *ch = (char *)p_Str;

    len = 0;
    while (*ch != NUL) {
        ch++;
        len++;
    }
        
    return (len);
}


///
///@brief 将字符串从“源”字符串复制到“目标”字符串。
///
///@param p_Dest 指向目标字符串的指针。
///@param p_Src 指向源字符串的指针。
///
void vstrcpy(SINT8 *p_Dest, const SINT8 *p_Src)
{
        UINT8 *ps = (UINT8*)p_Src;
        UINT8 *pd = (UINT8*)p_Dest;
        while(*ps != NUL) {
                *pd = *ps;
                pd++;
                ps++;
        }
        
        *pd = NUL;
}


///
///@brief 从源字符串复制指定数量的字符到目标字符串。
///
///@param p_Dest 指向目标字符串的指针。
///@param p_Src 指向源字符串的指针。
///@param NumOfBytes 要复制的字节数。
///
void vstrncpy(SINT8 *p_Dest, const SINT8 *p_Src, UINT32 NumOfBytes)
{
        UINT8 *ps = (UINT8*)p_Src;
        UINT8 *pd = (UINT8*)p_Dest;
        while(*ps != NUL && NumOfBytes) {
                *pd = *ps;
                pd++;
                ps++;
				NumOfBytes--;
        }
        while(NumOfBytes--){
			*pd = NUL;
		}
}

///
/// @brief 比较两个字符串，判断它们是否相同。
///
/// @param p_Str1 指向第一个要比较的字符串的指针。
/// @param p_Src2 指向第二个要比较的字符串的指针。
/// @return SINT32
/// 如果两者相同，则返回零。 如果第一个字符串大于第二个字符串，则返回正值；
/// 如果第一个字符串小于第二个字符串，则返回负值。
///
SINT32 vstrcmp(const SINT8 *p_Str1, const SINT8 *p_Src2)
{
        const UINT8 *s1 = (const UINT8 *) p_Str1;
        const UINT8 *s2 = (const UINT8 *) p_Src2;
        UINT8 c1, c2;

        do {
                c1 = (UINT8)*s1++;
                c2 = (UINT8)*s2++;
                if (c1 == NUL)
                        return c1 - c2;
        }while (c1 == c2);

        return c1 - c2;
}

///
/// @brief 比较指定数量的字符，判断两个字符串是否相同。
///
/// @param p_Str1 指向第一个要比较的字符串的指针。
/// @param p_Src2 指向第二个要比较的字符串的指针。
/// @param NumOfBytes 要比较的字节数。
/// @return SINT32
/// 如果两者相同，则返回零。 如果第一个字符串大于第二个字符串，则返回正值；
/// 如果第一个字符串小于第二个字符串，则返回负值。
///
SINT32 vstrncmp(const SINT8 *p_Str1, const SINT8 *p_Src2, UINT32 NumOfBytes)
{
        const UINT8 *s1 = (const UINT8 *) p_Str1;
        const UINT8 *s2 = (const UINT8 *) p_Src2;
        UINT8 c1, c2;

        while(NumOfBytes--){
                c1 = (UINT8)*s1++;
                c2 = (UINT8)*s2++;
                if (c1 == NUL || c1 != c2)
                        return c1 - c2;
        }

        return 0;
}

/**@}*/


 /**
 * @addtogroup Memory
 * 
 * @{
 */
/**
 * @brief 使用指定的value值初始化缓冲区中指定长度数据的内存。
 * 
 * @param p_Buf 缓冲区指针
 * @param Value 初始化值
 * @param NumOfBytes 初始化长度, 单位是字节
 */
void vmemset(void *p_Buf, const UINT8 Value, UINT32 NumOfBytes)
{
	UINT8 *p = (UINT8 *)p_Buf;
	UINT32 *p32;
	UINT32 v32[2];

	while (((UINT32)p & 0x03) && NumOfBytes)
	{
		*p++ = Value;
		NumOfBytes--;
	}

	p32 = (UINT32 *)p;
	v32[0] = (Value << 24) | (Value << 16) | (Value << 8) | Value;
	v32[1] = v32[0];
	while (NumOfBytes >= 4*2*8)
	{
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		NumOfBytes -= 4*2*8;
	}
	while (NumOfBytes >= 4*2)
	{
		p32[0] = v32[0];
		p32[1] = v32[1];
		p32 += 2;
		NumOfBytes -= 4*2;
	}
	while (NumOfBytes >= 4)
	{
		p32[0] = v32[0];
		p32 += 1;
		NumOfBytes -= 4;
	}
	if (NumOfBytes == 0)
		return ;

	p = (UINT8 *)p32;
	while (NumOfBytes--)
	{
		*p++ = Value;
	}
}

/**
 * @cond doxygen不会解析这段代码
 */
UINT32 VCOMMON_GetRand(UINT32 lower_limit, UINT32 upper_limit)
{
        return (lower_limit + upper_limit)/2;
}

void VCOMMON_GetRands(UINT8 *rand, UINT8 len)
{
        UINT8 i;
        for(i=0;i<len;i++) {
                rand[i] = VCOMMON_GetRand(0, 255);
        }
}

typedef struct 
{
	UINT8 tmp0;
	UINT8 tmp1;
	UINT8 tmp2;
	UINT8 tmp3;
} TMP_t;

/** @endcond */

///
///@brief 将源缓冲区的指定字节数复制到目标缓冲区。
///
///@param p_Dest 指向目标缓冲区的指针。
///@param p_Src 指向源缓冲区的指针。
///@param NumOfBytes 要复制的字节数。
///@return void*
///
void *vmemcpy(void *p_Dest, const void *p_Src, UINT32 NumOfBytes)
{
	TMP_t tmp[8];
	UINT32 *tmp_32 = (UINT32*)&tmp;
	UINT8 *ps = (UINT8 *)p_Src;
	UINT8 *pd = (UINT8 *)p_Dest;
	
	if(NumOfBytes > 0 && (int)ps % sizeof(int) == (int)pd % sizeof(int))
	{	
		while (NumOfBytes != 0 && (int)pd % sizeof(int) != 0)
		{
			*pd = *ps;
			pd++;
			ps++;
			NumOfBytes -= 1;
		}
		UINT32 *ps32 = (UINT32 *)ps;
		UINT32 *pd32 = (UINT32 *)pd;
		while (NumOfBytes >= 4 * 7)
		{
			tmp_32[0] = *ps32;
			tmp_32[1] = *(ps32 + 1);
			tmp_32[2] = *(ps32 + 2);
			tmp_32[3] = *(ps32 + 3);
			tmp_32[4] = *(ps32 + 4);
			tmp_32[5] = *(ps32 + 5);
			tmp_32[6] = *(ps32 + 6);
			*pd32 = tmp_32[0];
			*(pd32 + 1) = tmp_32[1];
			*(pd32 + 2) = tmp_32[2];
			*(pd32 + 3) = tmp_32[3];
			*(pd32 + 4) = tmp_32[4];
			*(pd32 + 5) = tmp_32[5];
			*(pd32 + 6) = tmp_32[6];
			pd32 += 7;
			ps32 += 7;
			NumOfBytes -= 4 * 7;
		}
		if (NumOfBytes >= 4 * 4)
		{
			tmp_32[0] = *ps32;
			tmp_32[1] = *(ps32 + 1);
			tmp_32[2] = *(ps32 + 2);
			tmp_32[3] = *(ps32 + 3);
			*pd32 = tmp_32[0];
			*(pd32 + 1) = tmp_32[1];
			*(pd32 + 2) = tmp_32[2];
			*(pd32 + 3) = tmp_32[3];
			pd32 += 4;
			ps32 += 4;
			NumOfBytes -= 4 * 4;
		}
		while (NumOfBytes >= 4)
		{
			*pd32 = *ps32;
			pd32 += 1;
			ps32 += 1;
			NumOfBytes -= 4;
		}
		ps = (UINT8 *)ps32;
		pd = (UINT8 *)pd32;
		while (NumOfBytes != 0)
		{
			*pd = *ps;
			pd++;
			ps++;
			NumOfBytes--;
		}
		return p_Dest;
	}
	while (NumOfBytes >= 8*4)
	{
		tmp[0] = *(TMP_t*)ps;
		tmp[1] = *((TMP_t*)ps + 1);
		tmp[2] = *((TMP_t*)ps + 2);
		tmp[3] = *((TMP_t*)ps + 3);
		tmp[4] = *((TMP_t*)ps + 4);
		tmp[5] = *((TMP_t*)ps + 5);
		tmp[6] = *((TMP_t*)ps + 6);
		tmp[7] = *((TMP_t*)ps + 7);
		*(TMP_t*)pd = tmp[0];
		*((TMP_t*)pd + 1) = tmp[1];
		*((TMP_t*)pd + 2) = tmp[2];
		*((TMP_t*)pd + 3) = tmp[3];
		*((TMP_t*)pd + 4) = tmp[4];
		*((TMP_t*)pd + 5) = tmp[5];
		*((TMP_t*)pd + 6) = tmp[6];
		*((TMP_t*)pd + 7) = tmp[7];
		pd += 8*4;
		ps += 8*4;
		NumOfBytes -= 8*4;
	}
	while (NumOfBytes >= 4)
	{
		tmp[0] = *(TMP_t*)ps;
		*(TMP_t*)pd = tmp[0];
		pd += 4;
		ps += 4;
		NumOfBytes -= 4;
	}
	while (NumOfBytes != 0)
	{
		*pd = *ps;
		pd++;
		ps++;
		NumOfBytes--;
	}

	return p_Dest;
}


///
/// @brief 将源缓冲区的指定字节数移动到目标缓冲区。
///
/// @param p_Dest 指向目标缓冲区的指针。
/// @param p_Src 指向源缓冲区的指针。
/// @param NumOfBytes 要移动的字节数。
/// @return void*
/// 返回目标缓冲区的指针。
///
void* vmemmove(void* p_Dest, const void* p_Src, UINT32 NumOfBytes)
{
        UINT8 *ret;    
        UINT8 *dst_t;    
        UINT8 *src_t;    
        ret = (UINT8 *)p_Dest; 
          
        if (p_Dest == NULL)  return 0; 
        if (p_Src == NULL)    return 0; 
        if (NumOfBytes == 0)    return 0; 

        if ((UINT8 *)p_Dest <= (UINT8 *)p_Src  || (UINT8 *)p_Dest >= ((UINT8 *)p_Src + NumOfBytes)) {   
                vmemcpy(p_Dest,p_Src, NumOfBytes);       
        } else {
                dst_t = (UINT8 *)p_Dest + NumOfBytes - 1;        
                src_t = (UINT8 *)p_Src + NumOfBytes - 1;        
                
                while (NumOfBytes--) {
                        *dst_t-- = *src_t--;        
                }    
        } 
        
        return(ret);    
}

////
///@brief 比较两个缓冲区中第一个指定的字节数。
///
///@param p_Buf1 指向要比较的第一个缓冲区的指针。
///@param p_Buf2 指向要比较的第二个缓冲区的指针。
///@param NumOfBytes 要比较的字节数。
///@return SINT32
/// 如果两者中的第一个指定字节数相同，则返回零。
/// 如果第一个缓冲区大于第二个缓冲区，则返回正值。
/// 如果第一个缓冲区小于第二个缓冲区，则返回负值。
///
SINT32 vmemcmp(const void *p_Buf1, const void *p_Buf2, UINT32 NumOfBytes)
{
    SINT32 res;
    UINT8 *p1 = (UINT8 *)p_Buf1;
    UINT8 *p2 = (UINT8 *)p_Buf2;

    if (p_Buf1 && p_Buf2) {
        if ((int)p1 % sizeof(int) == (int)p2 % sizeof(int)) {
            while (NumOfBytes != 0 && (int)p1 % sizeof(int) != 0) {
                res = *p1 - *p2;
                if (res != 0)
                    return res;
                p1 += 1;
                p2 += 1;
                NumOfBytes -= 1;
            }
            UINT32 *p3 = (UINT32 *)p1;
            UINT32 *p4 = (UINT32 *)p2;
            while (NumOfBytes >= 4) {
                res = *p3 - *p4;
                if (res != 0) {
                    return res;
                }
                p3 += 1;
                p4 += 1;
                NumOfBytes -= 4;
            }
            p1 = (UINT8 *)p3;
            p2 = (UINT8 *)p4;
        }
        while (NumOfBytes != 0) {
            res = *p1 - *p2;
            if (res != 0)
                return res;
            p1 += 1;
            p2 += 1;
            NumOfBytes -= 1;
        }
    } else {
        return 1;
    }

    return 0;
}

///
///@brief给定字符的搜索内存。
///
///@param p_Buf 指向要搜索的缓冲区的指针。
///@param Ch 字符。
///@param NumOfBytes 要搜索的最大字节数。
///@return void*
/// 返回指向 buf 中首次出现的 chr 的指针。如果在第一个 cnt 字节中找不到 chr，则返回 NULL。
///
void * vmemchr(const void * p_Buf, UINT8 Ch, UINT32 NumOfBytes)
{
        UINT8 *pTmp = (UINT8 *)p_Buf;
        while ( NumOfBytes && (*(unsigned char *)pTmp != (unsigned char)Ch) ) {
                pTmp = (unsigned char *)pTmp + 1;
                NumOfBytes--;
   }

   return(NumOfBytes ? (void *)pTmp : NULL);
}

/**@}*/


 /**
 * @addtogroup Util
 * 
 * @{
 */

///
/// @brief 将整数转换为指定进制的字符串表示。
///
/// @param Num 要转换的整数。
/// @param p_Str 用于存储结果字符串的缓冲区指针。
/// @param Radix 指定转换的进制，范围为 2 到 16。
///
void vitoa(SINT64 Num, UINT8* p_Str, UINT8 Radix)
{
        UINT8 index[]="0123456789ABCDEF";
    UINT32 unum; 
    UINT32 i=0,j,k;
        UINT8 temp;

    if(Radix==10&&Num<0)
    {
        unum=(UINT32)-Num;
                p_Str[i++]='-';
    }
    else 
                unum=(UINT32)Num;
        
    do{
        p_Str[i++]=index[unum%(UINT32)Radix];
        unum/=Radix;
    }while(unum);
    p_Str[i]='\0';
    if(p_Str[0]=='-') 
                k=1;
    else 
                k=0;
    
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=p_Str[j];
        p_Str[j] = p_Str[i-1+k-j];
        p_Str[i-1+k-j] = temp;
    }
}

/**
 * @cond doxygen不会解析这段代码
 */
UINT32 VCOMMON_GetHWVersion(void)
{
        UINT32 ver = 0;
        //ver = READ_REG(REG_FPGA_VERSION);

        return ver;
}

/** @endcond */

///
///@brief 延迟指定的微秒数。
///
///@param NumOfMicroSec 要延迟的时间，以微秒为单位。
///@note 延迟的最大间隔为 65535 us。
/// 如果此函数被破坏，延迟间隔可能不准确
/// 通过 ISR 或 OS 调度程序。
///
void vdelayus(UINT32 NumOfMicroSec)
{
	extern void VTMR_CommDelay(UINT32 us);
    VTMR_CommDelay(NumOfMicroSec);

}

///
/// @brief 获取当前时间，以微秒为单位。
///
/// @return UINT32 当前时间，以微秒为单位。
///
UINT32 vtimeusget(void)
{
    extern UINT32 VTMR_CommTimeget(void);
    return ~VTMR_CommTimeget();
}

/// @brief 计算两个时间点之间的间隔，以微秒为单位。
///
/// @param start 起始时间，以微秒为单位。
/// @param end 结束时间，以微秒为单位。
/// @return UINT32 两个时间点之间的间隔，以微秒为单位。
UINT32 vtimeusinterval(UINT32 start, UINT32 end)
{
    return (end - start);
}



/**
 * @cond doxygen不会解析这段代码
 */
/******************************************************************************
Description:
        Caculate CRC7 of data

Parameters:
        p_Data - source data
        Len - length of source data
        
Return Value:
        caculated CRC7

Remarks:
        G(x) = x7 + x3 + 1                                                                                      1000 1001       0x89
       M(x) = (first bit)xn +拢篓second bit)xn-1 + ...+(last bit)x0
       CRC[6...0] = Remainder{(M(x) * x7)  / G(x)]
*****************************************************************************/
UINT8 VCOMMON_CRC7(const UINT8 *p_Data, UINT32 Len)
{ 
        UINT8 i; 
        UINT32 crc=0; 

        while (Len-- != 0) { 
                for (i=0x80; i!=0; i=i>>1) { 
                        if ((crc & 0x40) != 0) {
                                crc = crc << 1; 
                                crc ^= 0x09;
                        } else {
                                crc = crc << 1; 
                        }
                        if ((*p_Data & i) != 0) {
                                crc ^= 0x09;
                        }
                } 
                p_Data++; 
        }
        
        crc = crc & 0x7f;
        crc = crc << 1;
        crc = crc | 0x01;
        
        return(crc); 
} 

/******************************************************************************
Description:
        Caculate CRC16 of data

Parameters:
        p_Data - source data
        Len - length of source data
        
Return Value:
        caculated CRC16

Remarks:
*****************************************************************************/

static const UINT16 g_CRC_Ta16[16]={   /*   CRC脫脿脢陆卤铆   */ 
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7, 
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef, 
}; 

UINT16 VCOMMON_CRC16(const UINT8 *p_Data, UINT32 Len)
{
        UINT16 crc; 
        UINT8 da; 

        crc = 0; 
        while (Len-- != 0)  { 
                da = ((UINT8)(crc >> 8)) >> 4;
                crc <<= 4;
                crc ^= g_CRC_Ta16[da^(*p_Data>>4)];
                da = ((UINT8)(crc >> 8)) >> 4;
                crc <<= 4;
                crc ^= g_CRC_Ta16[da^(*p_Data&0x0f)];
                p_Data++; 
        } 
        
        return crc&0xffff;
} 

// for ECK file
UINT16 VCOMMON_CRC16Ex(UINT16 crc, const UINT8 *p_Data, UINT32 Len)
{
        UINT8 da; 

        while (Len-- != 0)  { 
                da = ((UINT8)(crc >> 8)) >> 4;
                crc <<= 4;
                crc ^= g_CRC_Ta16[da^(*p_Data>>4)];
                da = ((UINT8)(crc >> 8)) >> 4;
                crc <<= 4;
                crc ^= g_CRC_Ta16[da^(*p_Data&0x0f)];
                p_Data++; 
        } 
        
        return crc&0xffff;
}


static const unsigned long CRC32_table[256] =
{
0x00000000, 0x0E939C67, 0x1D2738CE, 0x13B4A4A9, 0x3A4E719C, 0x34DDEDFB, 0x27694952, 0x29FAD535,
0x749CE338, 0x7A0F7F5F, 0x69BBDBF6, 0x67284791, 0x4ED292A4, 0x40410EC3, 0x53F5AA6A, 0x5D66360D,
0x390EAC35, 0x379D3052, 0x242994FB, 0x2ABA089C, 0x0340DDA9, 0x0DD341CE, 0x1E67E567, 0x10F47900,
0x4D924F0D, 0x4301D36A, 0x50B577C3, 0x5E26EBA4, 0x77DC3E91, 0x794FA2F6, 0x6AFB065F, 0x64689A38,
0x721D586A, 0x7C8EC40D, 0x6F3A60A4, 0x61A9FCC3, 0x485329F6, 0x46C0B591, 0x55741138, 0x5BE78D5F,
0x0681BB52, 0x08122735, 0x1BA6839C, 0x15351FFB, 0x3CCFCACE, 0x325C56A9, 0x21E8F200, 0x2F7B6E67,
0x4B13F45F, 0x45806838, 0x5634CC91, 0x58A750F6, 0x715D85C3, 0x7FCE19A4, 0x6C7ABD0D, 0x62E9216A,
0x3F8F1767, 0x311C8B00, 0x22A82FA9, 0x2C3BB3CE, 0x05C166FB, 0x0B52FA9C, 0x18E65E35, 0x1675C252,
0x340DDA91, 0x3A9E46F6, 0x292AE25F, 0x27B97E38, 0x0E43AB0D, 0x00D0376A, 0x136493C3, 0x1DF70FA4,
0x409139A9, 0x4E02A5CE, 0x5DB60167, 0x53259D00, 0x7ADF4835, 0x744CD452, 0x67F870FB, 0x696BEC9C,
0x0D0376A4, 0x0390EAC3, 0x10244E6A, 0x1EB7D20D, 0x374D0738, 0x39DE9B5F, 0x2A6A3FF6, 0x24F9A391,
0x799F959C, 0x770C09FB, 0x64B8AD52, 0x6A2B3135, 0x43D1E400, 0x4D427867, 0x5EF6DCCE, 0x506540A9,
0x461082FB, 0x48831E9C, 0x5B37BA35, 0x55A42652, 0x7C5EF367, 0x72CD6F00, 0x6179CBA9, 0x6FEA57CE,
0x328C61C3, 0x3C1FFDA4, 0x2FAB590D, 0x2138C56A, 0x08C2105F, 0x06518C38, 0x15E52891, 0x1B76B4F6,
0x7F1E2ECE, 0x718DB2A9, 0x62391600, 0x6CAA8A67, 0x45505F52, 0x4BC3C335, 0x5877679C, 0x56E4FBFB,
0x0B82CDF6, 0x05115191, 0x16A5F538, 0x1836695F, 0x31CCBC6A, 0x3F5F200D, 0x2CEB84A4, 0x227818C3,
0x681BB522, 0x66882945, 0x753C8DEC, 0x7BAF118B, 0x5255C4BE, 0x5CC658D9, 0x4F72FC70, 0x41E16017,
0x1C87561A, 0x1214CA7D, 0x01A06ED4, 0x0F33F2B3, 0x26C92786, 0x285ABBE1, 0x3BEE1F48, 0x357D832F,
0x51151917, 0x5F868570, 0x4C3221D9, 0x42A1BDBE, 0x6B5B688B, 0x65C8F4EC, 0x767C5045, 0x78EFCC22,
0x2589FA2F, 0x2B1A6648, 0x38AEC2E1, 0x363D5E86, 0x1FC78BB3, 0x115417D4, 0x02E0B37D, 0x0C732F1A,
0x1A06ED48, 0x1495712F, 0x0721D586, 0x09B249E1, 0x20489CD4, 0x2EDB00B3, 0x3D6FA41A, 0x33FC387D,
0x6E9A0E70, 0x60099217, 0x73BD36BE, 0x7D2EAAD9, 0x54D47FEC, 0x5A47E38B, 0x49F34722, 0x4760DB45,
0x2308417D, 0x2D9BDD1A, 0x3E2F79B3, 0x30BCE5D4, 0x194630E1, 0x17D5AC86, 0x0461082F, 0x0AF29448,
0x5794A245, 0x59073E22, 0x4AB39A8B, 0x442006EC, 0x6DDAD3D9, 0x63494FBE, 0x70FDEB17, 0x7E6E7770,
0x5C166FB3, 0x5285F3D4, 0x4131577D, 0x4FA2CB1A, 0x66581E2F, 0x68CB8248, 0x7B7F26E1, 0x75ECBA86,
0x288A8C8B, 0x261910EC, 0x35ADB445, 0x3B3E2822, 0x12C4FD17, 0x1C576170, 0x0FE3C5D9, 0x017059BE,
0x6518C386, 0x6B8B5FE1, 0x783FFB48, 0x76AC672F, 0x5F56B21A, 0x51C52E7D, 0x42718AD4, 0x4CE216B3,
0x118420BE, 0x1F17BCD9, 0x0CA31870, 0x02308417, 0x2BCA5122, 0x2559CD45, 0x36ED69EC, 0x387EF58B,
0x2E0B37D9, 0x2098ABBE, 0x332C0F17, 0x3DBF9370, 0x14454645, 0x1AD6DA22, 0x09627E8B, 0x07F1E2EC,
0x5A97D4E1, 0x54044886, 0x47B0EC2F, 0x49237048, 0x60D9A57D, 0x6E4A391A, 0x7DFE9DB3, 0x736D01D4,
0x17059BEC, 0x1996078B, 0x0A22A322, 0x04B13F45, 0x2D4BEA70, 0x23D87617, 0x306CD2BE, 0x3EFF4ED9,
0x639978D4, 0x6D0AE4B3, 0x7EBE401A, 0x702DDC7D, 0x59D70948, 0x5744952F, 0x44F03186, 0x4A63ADE1
};

UINT32 VCOMMON_CRC32(UINT32 crc, void* input, UINT32 len)
{
    int i;
    unsigned char index;
    unsigned char* pch;
    pch = (unsigned char*)input;
    for(i=0;i<len;i++)
    {
        index = (unsigned char)(crc^*pch);
        crc = (crc>>8)^CRC32_table[index];
        pch++;
    }
    return crc;
}

/******************************************************************************
Description:
        Get the bit index of a bit-mask value.

Parameters:
        BitMask: bit-mask value, in which only one bit is unmasked, e.g. 0x00010000

Return Value:   
        Returns the bit index, e.g. 16

*****************************************************************************/
UINT8 VCOMMON_GetBitIdx(UINT32 BitMask)
{
        UINT8 i = 0;

        while (BitMask) {
                BitMask >>= 1;
                if (BitMask)
                        i++;
        };

        return i;
}

__INLINE__ UINT8 VCOMMON_CLZ(UINT32 wd)
{
        #if (defined __GNUC__)
        return ( UINT8 )__builtin_clz(wd);
        #elif (defined __CC_ARM)
        return ( UINT8 )__clz(wd);
        #endif
}

UINT8 VCOMMON_MsBit(UINT32 BitMask)
{
        return (31UL - VCOMMON_CLZ(BitMask));  
}
UINT8 VCOMMON_LsBit(UINT32 BitMask)
{
        return (31UL - VCOMMON_CLZ( BitMask & -BitMask));
}

/*
 * Count number of bits that are set to 1 in a uint32 value.
 */
UINT8 VCOMMON_CountOnes(UINT32 value)
{
    int i;
    UINT8 result = 0;
    for(i=0; i<32; i++) {
        if(value & ((UINT32)1<<i)) {
            result += 1;
        }
    }
    return result;
}

/** @endcond */

/**@}*/
#if (PRIV_STDLIB_EN > 0)

void *   memchr (const void *src, int chr, size_t sz){
        return (void *)VCOMMON_MemChr((const void *)src, (UINT8)chr, sz);
}

int      memcmp (const void *mem1, const void *mem2, size_t sz){
        return (int) VCOMMON_MemCmp(mem1, mem2, (UINT32)sz);
}

void *   memcpy (void *dst, const void *src, size_t sz) {

        return (void *)VCOMMON_MemCpy(dst, src, sz);

}

void *   memmove (void *dst, const void *src, size_t sz){
        return VCOMMON_MemMove(dst, src, sz);
}

void *   memset (void *dst, int val, size_t sz){
        
        VCOMMON_MemSet(dst, (UINT8)val, sz);

        return dst;
}

size_t strnlen(const char *s, size_t count) {
  const char *sc;
  for (sc = s; *sc != '\0' && count--; ++sc);
  return sc - s;
}

char    *strcpy (char *dst, const char *src){
        VCOMMON_StrCpy(dst, src);
        return dst;
}

size_t   strlen (const char *s){
        return (size_t) VCOMMON_StrLen((const SINT8 *)s);
}


int      strcmp (const char *s1, const char *s2){
        return (int) VCOMMON_StrCmp(s1, s2);
}


int strncmp(const char *cs, const char *ct, size_t nb)
{
  int result = 0;
  for (; nb > 0; nb--)
    {
      if ((result = (int)*cs - (int)*ct++) != 0 || !*cs++)
        {
          break;
        }
    }

  return result;
}


char *strncpy(char *dest, const char *src, size_t n)
{
  char *ret = dest;     /* Value to be returned */
  char *end = dest + n; /* End of dest buffer + 1 byte */

  /* Copy up n bytes, breaking out of the loop early if a NUL terminator is
   * encountered.
   */

  while ((dest != end) && (*dest++ = *src++) != '\0')
    {
    }

  /* Note that there may be no NUL terminator in 'dest' */

  /* Pad the remainder of the array pointer to 'dest' with NULs */

  while (dest != end)
    {
      *dest++ = '\0';
    }

  return ret;
}

#endif //#if (PRIV_STDLIB_EN > 0)

