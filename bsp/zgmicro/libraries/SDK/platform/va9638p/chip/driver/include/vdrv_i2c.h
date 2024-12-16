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
    I2C

Description:
     I2C Driver

Revision History:
     Date                  Author                   Description
     2009.07.28              chenjian                   initial version
     2022.09.20              caoxuetao               9650
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VI2C_H_
#define _VI2C_H_
	
#include "vpla_i2c.h"

#define VRET_DRV_I2C_ERROR								0x410
#define VRET_DRV_I2C_ERR_PROCESSING						0x411
#define VRET_DRV_I2C_ERR_ARBIT_LOST						0x412
#define VRET_DRV_I2C_ERR_NACK_ADDR						0x413
#define VRET_DRV_I2C_ERR_NACK_DATA						0x414


#define I2C_ASSERT(expr)
#ifndef I2C_DEBUG
#define I2C_DEBUG(...)  //DEBUG_PRINTF("i2c", __VA_ARGS__)
#endif
/******************************************************************************

                         Macro Definition

*******************************************************************************/
#define I2C_RETRY_CNT           5
#define I2C_RETRY_DELAY         150  //2000//10000

#define I2C_CLK                            24000000

/* 延时时间 Delay_t */
#define I2C_AHB_Delay                     10

#define I2C_MODE_STANDARD                0
#define I2C_MODE_FAST                    1
#define I2C_MODE_SLOW                    2

#define I2C_WORK_MODE_INT                0
#define I2C_WORK_MODE_POLL                1
#define I2C_WORK_MODE                    I2C_WORK_MODE_POLL
/* I2C Read or Write Flag */
#define I2C_RWFLG_WRITE                    0        /* Write Mode    */
#define I2C_RWFLG_READ                    1        /* Read Mode    */

/* I2C slave device select */
#define I2C_SLAVE_PROM                     0
#define I2C_SLAVE_ANA                     1


/******************************************************************************

                         I2C Interrupt Clear Register Bits

*******************************************************************************/
#define RBIT_I2C_INTCLR_BYTEDONE        0
#define RBIT_I2C_INTCLR_ARBLOST            1
#define I2C_INTCLR_BYTEDONE_MASK        REG_BIT_MASK(RBIT_I2C_INTCLR_BYTEDONE)
#define I2C_INTCLR_ARBLOST_MASK            REG_BIT_MASK(RBIT_I2C_INTCLR_ARBLOST)


/******************************************************************************

                         Function Prototypes

*******************************************************************************/
void VI2C_Initialize(UINT32 I2cClk, UINT8 Mode, UINT8 SlaveDev);
UINT32 VI2C_Start(UINT8 SlaveAddr, UINT8 ReadOrWrite
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                , void (*p_CallBack)(UINT32 Result)
#endif
                );

UINT32 VI2C_Read(UINT8 *p_Buf, UINT32 Num
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                    , void (*p_CallBack)(UINT32 Result)
#endif
                    );

UINT32 VI2C_Write(UINT8 *p_Buf, UINT32 Num
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                    , void (*p_CallBack)(UINT32 Result)
#endif
                    );

#if I2C_WORK_MODE == I2C_WORK_MODE_INT
void VI2C_ByteTransfer(void);
#endif

void VI2C_ClearInterrupts(UINT8 IntFlag);
UINT32 VI2C_GetLastErrorCode(void);
void VI2C_Stop(void);
void VI2C_EnableI2CClk(void);
void VI2C_DisableI2CClk(void);


VRET_VALUE VI2C_register_read (UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value );
VRET_VALUE VI2C_register_write (UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value );
VRET_VALUE VI2C_Continue_read (UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value );
VRET_VALUE VI2C_Continue_write (UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value );

VRET_VALUE VI2C_Xfer(UINT8 i2c_sel, UINT8 i2c_mode, struct i2c_msg msgs[], UINT8 num);


typedef enum {
	i2c_rwflg_write_e = 0,
	i2c_rwflg_reade_e,
	i2c_rwflg_continue_write_e,
	i2c_rwflg_continue_reade_e,
	i2c_rwflg_set_e,
	i2c_rwflg_clr_e,
	i2c_rwflg_continue_set_e,
	i2c_rwflg_continue_clr_e,
}VI2C_RWFLG;

typedef struct{
    volatile enum {
        i2c_idle_e = 0,
        i2c_init_e,
        i2c_wr_start_e,
        i2c_wr_reg_e,
        i2c_wr_value_e,
        i2c_re_start_e,
        i2c_re_value_e,
    } work_stage;
	VI2C_RWFLG ReadOrWrite;
    UINT8 dev_addr;
    UINT8 Reg;
    UINT8 bytes;
    UINT8 bytes_cnt;
    enum {
        i2c_state_NOERR_e = 0,
        i2c_state_NACK_e,
        i2c_state_AL_e,
        i2c_state_CONTI_ERR_e,
        i2c_state_ARB_LOST_e,
        i2c_state_TIMEOUT_e,
        i2c_state_PARERR_e,
    } state;
    UINT8 is_set_or_clr;
    UINT8 reval_tmp;
    UINT8 wrval_tmp;
    UINT8 *p_Value;
}VI2C_IRQ_ST;

UINT32 i2c_irq_start(UINT8 SlaveDev, UINT8 i2c_mode, VI2C_RWFLG ReadOrWrite, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value, UINT8 len);
UINT32 i2c_irq_done(void);

UINT32 VI2C_register_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value);
UINT32 VI2C_register_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);
UINT32 VI2C_Continue_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value);
UINT32 VI2C_Continue_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);
UINT32 VI2C_register_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);
UINT32 VI2C_register_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);
UINT32 VI2C_Continue_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);
UINT32 VI2C_Continue_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value);

void VI2C_Interrupt_Routine_Service(void);
	
#define VI2C_Transfer_Done_irq()  i2c_irq_done()

#endif /*_VI2C_H_*/



