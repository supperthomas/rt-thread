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
    REGISTER DEFINITION

Description:
    Register address map and macro function definitions.

Revision History:
    Date                  Author                   Description
    2009.04.15           chenjingen              initial version
    2009.11.02           chenjingen              modify register map
    2014.03.20           yinxingjie              modify register map
    2020.11.30           chenyingchun            remove bitband definitions
    2022.05.30            cxt                       9650F
------------------------------------------------------------------------------*/
#ifndef _VREG_OPERATION_H_
#define _VREG_OPERATION_H_

#define REG(x)                          (*((volatile unsigned long *)(x)))
#define READ_REG(reg)                   (REG(reg))
#define WRITE_REG(reg, val)             (REG(reg) = (val))

#define REG_BIT_MASK(bit)               ((UINT32)0x1 << (bit))

#define GET_REG_BIT(reg_BIT_BAND)   		               (REG(reg_BIT_BAND))
#define SET_REG_BIT(reg_BIT_BAND)   		               (REG(reg_BIT_BAND) = 1)
#define CLR_REG_BIT(reg_BIT_BAND)   		               (REG(reg_BIT_BAND) = 0)
#define GET_REG_BIT2(reg, bit)          ((REG(reg) & REG_BIT_MASK(bit)) >> bit)
#define SET_REG_BIT2(reg, bit)          WRITE_REG(reg, (READ_REG(reg) | ((UINT32)1 << bit)))
#define CLR_REG_BIT2(reg, bit)          WRITE_REG(reg, (READ_REG(reg) & (~((UINT32)1 << bit))))

#define REG_BIT_GET(reg, bit)                  ((REG(reg) & REG_BIT_MASK(bit)) >> bit)
#define REG_BIT_SET(reg, bit)                  WRITE_REG(reg, (READ_REG(reg) | ((UINT32)1 << bit)))
#define REG_BIT_CLR(reg, bit)                  WRITE_REG(reg, (READ_REG(reg) & (~((UINT32)1 << bit))))


#define VAL_BIT_GET(val, bit)                  (((val) & REG_BIT_MASK(bit)) >> bit)             /**< 获取32位数据，指定bit位的值  */
#define VAL_BIT_SET(val, bit)                  ((val) |= REG_BIT_MASK(bit))                     /**< 将32位数据，指定bit位设置为1 */
#define VAL_BIT_CLR(val, bit)                  ((val) &= (~REG_BIT_MASK(bit)))                  /**< 将32位数据，指定bit位设置为0 */

#define GET_DATA_BIT(data, bit)         (((data) & REG_BIT_MASK(bit)) >> bit)

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
            
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))


#define SET_REG_BITS(reg, start, end, value)  (WRITE_REG(reg, ((REG(reg) & (~BITS(start, end))) | (((value) << start)&BITS(start, end)))))

#define SET_REG_MEMBER(reg, member, value)    (WRITE_REG(reg, ((REG(reg) & (~BITS(member, member+(member##_bit)-1))) | (((value)&BITS(0,(member##_bit)-1))<<member))))//cxt
#define GET_REG_MEMBER(reg, member)           ((REG(reg) & BITS(member, member+(member##_bit)-1))>>member)//cxt
#define REG_MEMBER_MASK(member)                     (BITS(member,member+(member##_bit)-1))


#define REG_SET_MEMBER(member, value)    (WRITE_REG(member##_ADDR, (REG(member##_ADDR) & ~member##_MASK) | (((value)<<member##_BIT)&member##_MASK)))
#define REG_GET_MEMBER(member)           ((REG(member##_ADDR) & member##_MASK)>>member##_BIT)

#define SET_REG_MEMBER_BIT(member)          ((member##_BITBAND)?(WRITE_REG(member##_BITBAND,(1))):\
                                                (WRITE_REG(member##_ADDR, (REG(member##_ADDR) & ~member##_MASK) | (((1)<<member##_BIT)&member##_MASK))))
#define CLR_REG_MEMBER_BIT(member)          ((member##_BITBAND)?(WRITE_REG(member##_BITBAND,(0))):\
                                                (WRITE_REG(member##_ADDR, (REG(member##_ADDR) & ~member##_MASK))))
#define GET_REG_MEMBER_BIT(member)           ((member##_BITBAND)?(REG(member##_BITBAND)):((REG(member##_ADDR) & member##_MASK)>>member##_BIT))

#endif /* _VREG_OPERATION_H_ */


