/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
        i2c

Description:
        i2c Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vpla_i2c.h"
#include "vpla_gpio.h"
#include "vdrv_i2c.h"
#include "vdrv_hd_cport.h"
#include "vdrv_gpio.h"
#include "vdebug_errno.h"
#include "vdev_ipc.h"

#undef VSYS_I2C_DEBUG
#define VSYS_I2C_DEBUG(...)  DEBUG_PRINTF("[VPLA_IIC]", __VA_ARGS__)

#ifndef VSYS_I2C_SOFTWARE_SUPPORT
#define VSYS_I2C_SOFTWARE_SUPPORT 1
#endif

#define VIPC_IIC_MUTEX_LOCK(lock)    do{if(!VIPC_DRV_MUTEX_LOCK(i2c_ipc_lock, lock)){ERROR_PRINTF("[VPLA_IIC] lock%d failed~~~~~~~~\r\n", lock);}}while(0)
#define VIPC_IIC_MUTEX_UNLOCK(lock)  do{if(!VIPC_DRV_MUTEX_UNLOCK(i2c_ipc_lock, lock)){ERROR_PRINTF("[VPLA_IIC] unlock%d failed\r\n", lock);}}while(0)

VDRVI2CMode_t i2c_mode[VSYS_I2C_CH_LIMIT] = {VSYS_I2C_MODE_STANDARD};

/*********************************************** HDS iic **********************************************************************/
#define HDS_I2C_SLAVE_ADDR 0x34
static UINT8 HDS_I2C_slave_support[4] = {0};
VRET_VALUE Vcdc_I2C_SwitchToHDS(UINT8 clk_div, BOOL delay);
VRET_VALUE Vcdc_HDS_SwitchTOI2C(UINT8 clk_div);

/*********************************************** Software iic *****************************************************************/
typedef struct
{
    UINT8 scl_pin;
    UINT8 sda_pin;
	UINT8 mode;
    UINT8 state;
    UINT32 clock;
    UINT8 contmode;
    UINT8 err;
} g_I2C_Sim_inst_t;
#define I2C_MODE_NORMAL 0
#define I2C_MODE_WAIT_SCL 1

static void VI2C_SIM_Initialize(g_I2C_Sim_inst_t *inst, UINT8 SclPin, UINT8 SdaPin, VDRVI2CMode_t I2cClkmode, UINT8 mode);
static void VI2C_SIM_UnInitialize(g_I2C_Sim_inst_t *inst);
static UINT8 VI2C_SIM_Read(g_I2C_Sim_inst_t *inst, UINT8 SlaveAddr, UINT8 Reg, UINT8 *p_Buf, UINT32 num);
static UINT8 VI2C_SIM_Write(g_I2C_Sim_inst_t *inst, UINT8 SlaveAddr, UINT8 Reg, UINT8 *p_Buf, UINT32 num);
static UINT8 VI2C_SIM_Xfer(g_I2C_Sim_inst_t *inst, struct i2c_msg msgs[], UINT8 num);
static void VI2C_SIM_Set_Clkmode(g_I2C_Sim_inst_t *inst, VDRVI2CMode_t I2cClkmode);
static void VI2C_SIM_Set_Contmode(g_I2C_Sim_inst_t *inst, BOOL en);
static BOOL VI2C_SIM_Get_Contmode(g_I2C_Sim_inst_t *inst);

static g_I2C_Sim_inst_t g_I2C_Sim_inst[VSYS_I2C_CH_LIMIT - VSYS_I2C_SLAVE_SOFTWARE] = {0};
/***************************************************************************************************************************/

VRET_VALUE VSYS_I2C_Init(VDRVI2CSel_t i2c_sel, VDRVI2CConfig_t *i2c_config)
{
	VRET_VALUE ret = VRET_ERROR;
    if (i2c_config == NULL)
        return VRET_ERROR;

    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_PROM:
        VGPIO_SetMuxFunction(GPIO_PIN_7 | GPIO_PIN_8, GPIO_PIN_7 | GPIO_PIN_8);
        VGPIO_SetMux2Function(GPIO_PIN_7 | GPIO_PIN_8, GPIO_MUX_FUNC_NONE);
        if (i2c_config->mode >= VSYS_I2C_MODE_HDS)
        {
            i2c_mode[i2c_sel] = VSYS_I2C_MODE_FAST;
        }
        break;
    case VSYS_I2C_SLAVE_ANA:
        if (i2c_config->mode == VSYS_I2C_MODE_HDS)
        {
            ret = Vcdc_I2C_SwitchToHDS(1, FALSE);
            HDS_I2C_slave_support[0] = HDS_I2C_SLAVE_ADDR;
        }
        else
        {
            ret = Vcdc_HDS_SwitchTOI2C(1);
            HDS_I2C_slave_support[0] = 0;
            i2c_mode[i2c_sel] = i2c_config->mode;
        }
		if(ret != VRET_NO_ERROR)
		{
            __VERRNO_STORE(VERRNO_I2C_SWITCH);
			VSYS_I2C_DEBUG("%s: Switch%d failed!!!\r\n", FUNCTION_NAME, i2c_config->mode);
        }
        break;
    default:

#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            uint8_t scl_pin = VSYS_GPIO_GET_INDEX(i2c_config->scl_pin);
            if (scl_pin < 32) {
                VGPIO_SetMuxFunction(1u << scl_pin, GPIO_MUX_FUNC_NONE);
                VGPIO_SetMux2Function(1u << scl_pin, GPIO_MUX_FUNC_NONE);
            } else {
                scl_pin -= 32;
                VGPIO_SetLMuxFunction(1u << scl_pin, GPIO_MUX_FUNC_NONE);
                VGPIO_SetLMux2Function(1u << scl_pin, GPIO_MUX_FUNC_NONE);
                scl_pin |= 0x80;
            }

            uint8_t sda_pin = VSYS_GPIO_GET_INDEX(i2c_config->sda_pin);
            if (sda_pin < 32) {
                VGPIO_SetMuxFunction(1u << sda_pin, GPIO_MUX_FUNC_NONE);
                VGPIO_SetMux2Function(1u << sda_pin, GPIO_MUX_FUNC_NONE);
            } else {
                sda_pin -= 32;
                VGPIO_SetLMuxFunction(1u << sda_pin, GPIO_MUX_FUNC_NONE);
                VGPIO_SetLMux2Function(1u << sda_pin, GPIO_MUX_FUNC_NONE);
                sda_pin |= 0x80;
            }

            VI2C_SIM_Initialize(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], scl_pin, sda_pin, i2c_config->mode, I2C_MODE_WAIT_SCL);
            VI2C_SIM_Set_Contmode(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], i2c_config->contmode_en);
            break;
        } else 
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_I2C_Control(VDRVI2CSel_t i2c_sel, VDRVI2CControl_t control, UINT32 Value)
{
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_PROM:
    case VSYS_I2C_SLAVE_ANA:
        if (control == VSYS_I2C_Set_Clkmode)
        {
            i2c_mode[i2c_sel] = (VDRVI2CMode_t)Value;
        }
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            switch (control)
            {
            case VSYS_I2C_Set_Clkmode:
                VI2C_SIM_Set_Clkmode(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], (VDRVI2CMode_t)Value);
                break;
            case VSYS_I2C_Set_Contmode:   
                VI2C_SIM_Set_Contmode(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], (BOOL)Value);
                break;
            default:
                break;
            }
        } else
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_I2C_Uninit(VDRVI2CSel_t i2c_sel)
{
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_PROM:
        VGPIO_SetMuxFunction(GPIO_PIN_7 | GPIO_PIN_8, GPIO_MUX_FUNC_NONE);
        VGPIO_SetMux2Function(GPIO_PIN_7 | GPIO_PIN_8, GPIO_MUX_FUNC_NONE);
        break;
    case VSYS_I2C_SLAVE_ANA:
        Vcdc_HDS_SwitchTOI2C(1);
        HDS_I2C_slave_support[0] = 0;
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            VI2C_SIM_UnInitialize(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE]);
            break;
        } else
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_I2C_Write(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, const UINT8 *buf, size_t siz)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    size_t i = 0;
    if (buf == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_ANA:
        if (HDS_I2C_slave_support[0] == dev_addr)
        {
            for (i = 0; i < siz && ret == VRET_NO_ERROR; i++)
            {
                ret = VHDS_register_write(Reg + i, buf[i]);
            }
            break;
        }
    case VSYS_I2C_SLAVE_PROM:
        for (i = 0; i < siz && ret == VRET_NO_ERROR; i++)
        {
            ret = VI2C_register_write(i2c_sel, i2c_mode[i2c_sel], dev_addr, Reg + i, buf[i]);
        }
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            if(VI2C_SIM_Get_Contmode(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE]))
                i = siz;
            else
                i = 1;
            while(siz > 0)
            {   
                ret = VI2C_SIM_Write(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], dev_addr, Reg, (UINT8 *)buf, i);
                siz -= i;
                buf += i;
				Reg += i;
            }
            break;
        } else
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
	if (ret)
    {
        __VERRNO_STORE(VERRNO_I2C_WR);
        VSYS_I2C_DEBUG("%s: Writing D%02x R%02x %d %02x failed!!!\r\n", FUNCTION_NAME, dev_addr, Reg, siz, buf[0]);
    }
    return ret;
}

VRET_VALUE VSYS_I2C_Read(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, UINT8 *buf, size_t siz)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    size_t i = 0;
    if (buf == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_ANA:
        if (HDS_I2C_slave_support[0] == dev_addr)
        {
            for (i = 0; i < siz && ret == VRET_NO_ERROR; i++)
            {
                ret = VHDS_register_read(Reg + i, &buf[i]);
            }
            break;
        }
    case VSYS_I2C_SLAVE_PROM:
        for (i = 0; i < siz && ret == VRET_NO_ERROR; i++)
        {
            ret = VI2C_register_read(i2c_sel, i2c_mode[i2c_sel], dev_addr, Reg + i, &buf[i]);
        }
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            if(VI2C_SIM_Get_Contmode(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE]))
                i = siz;
            else
                i = 1;
            while(siz > 0)
            {   
                ret = VI2C_SIM_Read(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], dev_addr, Reg, (UINT8 *)buf, i);
                siz -= i;
                buf += i;
				Reg += i;
            }
            break;
        } else
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
	if (ret)
    {
        __VERRNO_STORE(VERRNO_I2C_RD);
        VSYS_I2C_DEBUG("%s: Reading D%02x R%02x %d failed!!!\r\n", FUNCTION_NAME, dev_addr, Reg, siz);
    }
    return ret;
}

VRET_VALUE VSYS_I2C_BitOperat(VDRVI2CSel_t i2c_sel, UINT8 dev_addr, UINT32 Reg, VDRVI2CBitOperat_t operat, UINT8 Value)
{
    VRET_VALUE ret = VRET_ERROR;
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_ANA:
        if (HDS_I2C_slave_support[0] == dev_addr)
        {
            UINT8 reg_val = 0;
            if (VRET_NO_ERROR == (ret = VHDS_register_read(Reg, &reg_val)))
            {
                switch (operat)
                {
                case VSYS_I2C_Bit_SET:
                    ret = VHDS_register_write(Reg, reg_val | Value);
                    break;
                case VSYS_I2C_Bit_CLR:
                    ret = VHDS_register_write(Reg, reg_val & (~Value));
                    break;
                default:
                    break;
                }
            }
            break;
        }
    case VSYS_I2C_SLAVE_PROM:
        switch (operat)
        {
        case VSYS_I2C_Bit_SET:
            ret = VI2C_Continue_set_irq(i2c_sel, i2c_mode[i2c_sel], dev_addr, Reg, Value);
            break;
        case VSYS_I2C_Bit_CLR:
            ret = VI2C_Continue_clr_irq(i2c_sel, i2c_mode[i2c_sel], dev_addr, Reg, Value);
            break;
        default:
            break;
        }
        VI2C_Transfer_Done_irq();
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            UINT8 reg_val = 0;
            if (VRET_NO_ERROR == (ret = VI2C_SIM_Read(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], dev_addr, Reg, &reg_val, 1)))
            {
                switch (operat)
                {
                case VSYS_I2C_Bit_SET:
                    reg_val |= Value;
                    break;
                case VSYS_I2C_Bit_CLR:
                    reg_val &= ~Value;
                    break;
                default:
                    break;
                }
                ret = VI2C_SIM_Write(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], dev_addr, Reg, &reg_val, 1);
            }
            break;
        } else
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            return VRET_ERROR;
        }
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
	if (ret)
    {
        __VERRNO_STORE(VERRNO_I2C_BIT);
        VSYS_I2C_DEBUG("%s: BitOperat D%02x R%02x %d %02x failed!!!\r\n", FUNCTION_NAME, dev_addr, Reg, operat, Value);
    }
    return ret;
}

VRET_VALUE VSYS_I2C_Transfer(VDRVI2CSel_t i2c_sel, struct i2c_msg msgs[], uint32_t num)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    if (msgs == NULL)
    {
        __VERRNO_STORE(VERRNO_PARA_NULL);
        return VRET_ERROR;
    }
    VIPC_IIC_MUTEX_LOCK(i2c_sel);
    switch (i2c_sel)
    {
    case VSYS_I2C_SLAVE_ANA:
        /* not suport */
        ret = VRET_ERROR;
        break;
    case VSYS_I2C_SLAVE_PROM:
        ret = VI2C_Xfer(i2c_sel, i2c_mode[i2c_sel], msgs, num);
        break;
    default:
#if VSYS_I2C_SOFTWARE_SUPPORT
        if (i2c_sel >= VSYS_I2C_SLAVE_SOFTWARE && i2c_sel < VSYS_I2C_CH_LIMIT) {
            ret = VI2C_SIM_Xfer(&g_I2C_Sim_inst[i2c_sel - VSYS_I2C_SLAVE_SOFTWARE], msgs, num);
        } else 
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */
        {
            __VERRNO_STORE(VERRNO_I2C_INVALID);
            ret = VRET_ERROR;
        }
        break;
    }
    VIPC_IIC_MUTEX_UNLOCK(i2c_sel);
	if (ret)
    {
        __VERRNO_STORE(VERRNO_I2C_WR);
        VSYS_I2C_DEBUG("%s: failed! i2c_sel=%d, ret=%d\r\n", FUNCTION_NAME, i2c_sel, ret);
    }
    return ret;
}

/*********************************************** Software iic **********************************************************************/
#if VSYS_I2C_SOFTWARE_SUPPORT
#include "vdrv_ckm.h"
#define I2C_RET_OK (0)
#define I2C_RET_NACK (1)
#define I2C_RET_ARBI (2)
#define I2C_RET_ERR (1)
#define I2C_ACK (0)
#define I2C_NACK (1)
#define I2C_NORMAL (0)
#define I2C_WAITSCL (1)
#define _I2C_RD_ADDR(addr) ((addr << 1) | (0x1))
#define _I2C_WR_ADDR(addr) ((addr << 1) & ~(0x1))

#define _I2C_GPIO_MODE(gpio, mode)                                                        \
    do                                                                                    \
    {                                                                                     \
        if (gpio & 0x80)                                                                  \
            VGPIO_SetLMode(1 << (gpio & 0x7F), mode, GPIO_Z_NORMAL | GPIO_INPUT_DISPULL); \
        else                                                                              \
            VGPIO_SetMode(1 << (gpio & 0x7F), mode, GPIO_Z_NORMAL | GPIO_INPUT_DISPULL);  \
    } while (0)

#define _I2C_GPIO_SETOUT(gpio, val)                  \
    do                                               \
    {                                                \
        if (gpio & 0x80)                             \
            VGPIO_SetLPinOutput((gpio & 0x7F), val); \
        else                                         \
            VGPIO_SetPinOutput((gpio & 0x7F), val);  \
    } while (0)

static __INLINE__ UINT8 _I2C_GPIO_GETIN(UINT8 gpio)
{
    if (gpio & 0x80)
        return VGPIO_GetLPinInput(gpio & 0x7F);
    else
        return VGPIO_GetPinInput(gpio & 0x7F);
}

#define _I2C_SCL_OUT() _I2C_GPIO_MODE(inst->scl_pin, GPIO_MODE_OUTPUT)
#define _I2C_SCL_GIVENUP() _I2C_GPIO_MODE(inst->scl_pin, GPIO_MODE_Z)
#define _I2C_SCL_IN() _I2C_GPIO_MODE(inst->scl_pin, GPIO_MODE_INPUT)
#define _I2C_SDA_OUT() _I2C_GPIO_MODE(inst->sda_pin, GPIO_MODE_OUTPUT)
#define _I2C_SDA_IN() _I2C_GPIO_MODE(inst->sda_pin, GPIO_MODE_INPUT)
#define _I2C_SDA_GIVENUP() _I2C_GPIO_MODE(inst->sda_pin, GPIO_MODE_Z)

#define _I2C_SCL_H() _I2C_GPIO_SETOUT(inst->scl_pin, TRUE)
#define _I2C_SCL_L() _I2C_GPIO_SETOUT(inst->scl_pin, FALSE)
#define _I2C_SCL_READ() _I2C_GPIO_GETIN(inst->scl_pin)

#define _I2C_SDA_H() _I2C_GPIO_SETOUT(inst->sda_pin, TRUE)
#define _I2C_SDA_L() _I2C_GPIO_SETOUT(inst->sda_pin, FALSE)
#define _I2C_SDA_READ() _I2C_GPIO_GETIN(inst->sda_pin)

#define _I2C_DLY_DIV(n)                      \
    do                                       \
    {                                        \
        UINT16 cont = 0;                     \
        while (cont++ < inst->clock / 5 / n) \
            ;                                \
    } while (0)

#define _I2C_HDLY() _I2C_DLY_DIV(10650)
#define _I2C_HLDLY() _I2C_DLY_DIV(12500)
#define _I2C_LDLY() _I2C_DLY_DIV(21650)
#define _I2C_LLDLY() _I2C_DLY_DIV(25000)

#define _I2C_CHECK_ERR() \
    if (inst->err)       \
    goto i2c_stop

#define _I2C_Start() \
    I2C_Start(inst); \
    _I2C_CHECK_ERR()
#define _I2C_Stop() \
    i2c_stop:       \
    I2C_Stop(inst); \
    return inst->err;
#define _I2C_ACK(ack)   \
    I2C_Ack(inst, ack); \
    _I2C_CHECK_ERR()
#define _I2C_SendByte(byte)   \
    I2C_SendByte(inst, byte); \
    _I2C_CHECK_ERR()
#define _I2C_WaitAck() \
    I2C_WaitAck(inst); \
    _I2C_CHECK_ERR()
#define _I2C_ReadByte(byte)    \
    byte = I2C_ReadByte(inst); \
    _I2C_CHECK_ERR()

static UINT8 I2c_WaitSCL_High(g_I2C_Sim_inst_t *inst)
{
    UINT8 ret = I2C_RET_OK;
    if (inst->mode == I2C_WAITSCL)
    {
        UINT8 count = 10000000 / inst->clock;
        _I2C_SCL_IN();
        while (count-- && !_I2C_SCL_READ())
        {
            _I2C_DLY_DIV(10);
            if (count == 0)
            {
                ret = I2C_RET_ARBI;
                inst->err |= 1 << I2C_RET_ARBI;
            }
        }
        _I2C_SCL_OUT();
    }
    return ret;
}

static UINT8 I2C_Start(g_I2C_Sim_inst_t *inst)
{
    if (I2c_WaitSCL_High(inst))
        return I2C_RET_ARBI;
    _I2C_SCL_H();
    _I2C_SCL_OUT();
    _I2C_LLDLY();
    _I2C_SDA_H();
    _I2C_SDA_OUT();
    _I2C_LDLY();
    _I2C_SDA_L();
    _I2C_LDLY();
    _I2C_SCL_L();
    _I2C_LDLY();
    return I2C_RET_OK;
}

static void I2C_Stop(g_I2C_Sim_inst_t *inst)
{
    _I2C_SDA_L();
    _I2C_SDA_OUT();
    _I2C_LDLY();
    I2c_WaitSCL_High(inst);
    _I2C_SCL_H();
    _I2C_LDLY();
    _I2C_SDA_H();
    _I2C_LDLY();
    _I2C_SCL_GIVENUP();
    _I2C_SDA_GIVENUP();
}

static UINT8 I2C_Ack(g_I2C_Sim_inst_t *inst, UINT8 ack)
{
    if (ack == I2C_ACK)
        _I2C_SDA_L();
    else
        _I2C_SDA_H();
    _I2C_SDA_OUT();
    _I2C_LDLY();
    if (I2c_WaitSCL_High(inst))
        return I2C_RET_ARBI;
    _I2C_SCL_H();
    _I2C_HDLY();
    _I2C_SCL_L();
    _I2C_LLDLY();
    return I2C_RET_OK;
}

static UINT8 I2C_SendByte(g_I2C_Sim_inst_t *inst, UINT8 byte)
{
    for (UINT8 i = 0; i < 8; i++)
    {
        if (byte & 0x80)
            _I2C_SDA_H();
        else
            _I2C_SDA_L();
        if (i == 0)
            _I2C_SDA_OUT();
        byte <<= 1;
        _I2C_LDLY();
        if (I2c_WaitSCL_High(inst))
            return I2C_RET_ARBI;
        _I2C_SCL_H();
        _I2C_HDLY();
        _I2C_SCL_L();
        _I2C_LLDLY();
    }
    return I2C_RET_OK;
}

static UINT8 I2C_WaitAck(g_I2C_Sim_inst_t *inst)
{
    UINT8 ret = I2C_RET_OK;

    _I2C_SDA_IN();
    _I2C_LDLY();
    if (I2c_WaitSCL_High(inst))
        return I2C_RET_ARBI;
    _I2C_SCL_H();
    _I2C_HLDLY();
    if (_I2C_SDA_READ())
    {
        ret = I2C_RET_NACK;
        inst->err |= 1 << I2C_RET_NACK;
    }
    _I2C_SCL_L();
    _I2C_LLDLY();

    return ret;
}

static UINT8 I2C_ReadByte(g_I2C_Sim_inst_t *inst)
{
    UINT8 value = 0;
    _I2C_SDA_IN();
    for (UINT8 i = 0; i < 8; i++)
    {
        _I2C_LDLY();
        if (I2c_WaitSCL_High(inst))
            return 0;
        _I2C_SCL_H();
        _I2C_HLDLY();
        value <<= 1;
        if (_I2C_SDA_READ())
        {
            value |= 0x01;
        }
        _I2C_SCL_L();
        _I2C_LLDLY();
    }

    return value;
}

static void VI2C_SIM_Set_Clkmode(g_I2C_Sim_inst_t *inst, VDRVI2CMode_t I2cClkmode)
{
    UINT32 I2cClk = 100000;
    if (I2cClkmode == VSYS_I2C_MODE_FAST) {
        I2cClk = 400000;
    } else if (I2cClkmode == VSYS_I2C_MODE_SLOW) {
        I2cClk = 1500;
    }

    inst->clock = VCKM_GetSysClockFreq() / (I2cClk / 1000);
}

static void VI2C_SIM_Set_Contmode(g_I2C_Sim_inst_t *inst, BOOL en)
{
    inst->contmode = en;
}

static BOOL VI2C_SIM_Get_Contmode(g_I2C_Sim_inst_t *inst)
{
    return inst->contmode;
}

static void VI2C_SIM_Initialize(g_I2C_Sim_inst_t *inst, UINT8 SclPin, UINT8 SdaPin, VDRVI2CMode_t I2cClkmode, UINT8 mode)
{
    VI2C_SIM_Set_Clkmode(inst, I2cClkmode);
    inst->scl_pin = SclPin;
    inst->sda_pin = SdaPin;
    inst->mode = mode;
    inst->state = TRUE;
    inst->err = 0;
    _I2C_SCL_GIVENUP();
    _I2C_SDA_GIVENUP();
}

static void VI2C_SIM_UnInitialize(g_I2C_Sim_inst_t *inst)
{
    inst->err = 0;
    inst->state = FALSE;
    _I2C_SCL_GIVENUP();
    _I2C_SDA_GIVENUP();
}

static UINT8 VI2C_SIM_Read(g_I2C_Sim_inst_t *inst, UINT8 SlaveAddr, UINT8 Reg, UINT8 *p_Buf, UINT32 num)
{
    inst->err = 0;
    _I2C_Start();
    _I2C_SendByte(_I2C_WR_ADDR(SlaveAddr));
    _I2C_WaitAck();
    _I2C_SendByte(Reg);
    _I2C_WaitAck();
    _I2C_Start();
    _I2C_SendByte(_I2C_RD_ADDR(SlaveAddr));
    _I2C_WaitAck();
    while (num)
    {
        _I2C_ReadByte(*p_Buf);
        if (num != 1) {
            _I2C_ACK(I2C_ACK);
            _I2C_HDLY();
        }
        else
            _I2C_ACK(I2C_NACK);
        p_Buf++;
        num--;
    }
    _I2C_Stop();
}

static UINT8 VI2C_SIM_Write(g_I2C_Sim_inst_t *inst, UINT8 SlaveAddr, UINT8 Reg, UINT8 *p_Buf, UINT32 num)
{
    inst->err = 0;
    _I2C_Start();
    _I2C_SendByte(_I2C_WR_ADDR(SlaveAddr));
    _I2C_WaitAck();
    _I2C_SendByte(Reg);
    _I2C_WaitAck();
    while (num)
    {
        _I2C_SendByte(*p_Buf);
        _I2C_WaitAck();
        p_Buf++;
        num--;
    }
    _I2C_Stop();
}

static UINT8 VI2C_SIM_CheckSlave(g_I2C_Sim_inst_t *inst, UINT8 SlaveAddr)
{
    inst->err = 0;
    _I2C_Start();
    _I2C_SendByte(_I2C_WR_ADDR(SlaveAddr));
    _I2C_WaitAck();
    _I2C_Stop();
}

static UINT8 VI2C_SIM_Xfer(g_I2C_Sim_inst_t *inst, struct i2c_msg msgs[], UINT8 num)
{
    UINT8 ret = I2C_RET_OK;
    struct i2c_msg *msg;
    UINT8 *p_Buf;
    UINT16 len = 0;
    inst->err = 0;

    for (int i = 0; i < num; i++)
    {
        msg = &msgs[i];
        p_Buf = msg->buf;
        len = msg->len;
        if (!(msg->flags & I2C_NO_START))
        {
            _I2C_Start();
        }
        if (msg->flags & I2C_RD)
        {
            _I2C_SendByte(_I2C_RD_ADDR(msg->addr));
            _I2C_WaitAck();
            while (len)
            {
                _I2C_ReadByte(*p_Buf);
                if (len != 1)
                    _I2C_ACK(I2C_ACK);
                else
                    _I2C_ACK(I2C_NACK);
                p_Buf++;
                len--;
            }
        } else {
            _I2C_SendByte(_I2C_WR_ADDR(msg->addr));
            _I2C_WaitAck();
            while (len)
            {
                _I2C_SendByte(*p_Buf);
                _I2C_WaitAck();
                p_Buf++;
                len--;
            }
        }
    }
    _I2C_Stop();
}
#endif /* VSYS_I2C_SOFTWARE_SUPPORT */

/***********************************************************************************************************************************/

// TODO 暂时放在这个位置，待76xx整理好后，移动位置
/* SPORT MODE */
#include "vdrv_cdc.h"
#define CDC_I2C_SLAVE_ADDR 0x34

#define HDS_SM_PDM_CLKDIV (BIT7)
#define HDS_SM_I2S_CLKDIV (BIT6)
#define HDS_SM_I2S_MODE (BIT5)
#define HDS_SM_RXTEST (BIT4)
#define HDS_SM_DELAY (BIT3)
#define HDS_SM_MODE (BIT2)
#define HDS_SM_RXFIFO_RESET (BIT1)
#define HDS_SM_TXFIFO_RESET (BIT0)
#define CDC_REG763X_SPORT_MODE 0xC7
VRET_VALUE Vcdc_I2C_SwitchToHDS(UINT8 clk_div, BOOL delay)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT8 value = 0;
    UINT8 cdc_mode_Regs = 0;

    VHDS_Finit();
    VHDS_SetClock(clk_div);

//    if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S)
//    {
//        cdc_mode_Regs = CDC_REGS_SPORT_MODE;
//    }
//    else
    {
        cdc_mode_Regs = CDC_REG763X_SPORT_MODE;
    }

    ret = VHDS_register_read(cdc_mode_Regs, &value);
    if (ret == VRET_NO_ERROR && value & HDS_SM_MODE)
    {
        return ret;
    }
    ret = VI2C_register_read(I2C_SLAVE_ANA, I2C_MODE_STANDARD, CDC_I2C_SLAVE_ADDR, cdc_mode_Regs, &value);
    if (ret != VRET_NO_ERROR)
    {
        return ret;
    }
    value |= HDS_SM_MODE;
    if (delay == TRUE)
    {
        value |= HDS_SM_DELAY;
    }
    else
    {
        value &= ~(HDS_SM_DELAY);
    }
    ret = VI2C_register_write(I2C_SLAVE_ANA, I2C_MODE_STANDARD, CDC_I2C_SLAVE_ADDR, cdc_mode_Regs, value);
    if (ret != VRET_NO_ERROR)
    {
        return ret;
    }
    return ret;
}

VRET_VALUE Vcdc_HDS_SwitchTOI2C(UINT8 clk_div)
{
    VRET_VALUE ret;
    UINT8 val;
    UINT8 cdc_mode_Regs = 0;

//    if (VCDC_GetVerNo() == CDC_VERSION_7621_I2S)
//    {
//        cdc_mode_Regs = CDC_REGS_SPORT_MODE;
//    }
//    else
    {
        cdc_mode_Regs = CDC_REG763X_SPORT_MODE;
    }

    VHDS_Initialize(1);
    VHDS_HW_Bclk_Enable();

    ret = VHDS_ReadCmd(cdc_mode_Regs, &val);
    if (ret == VRET_NO_ERROR)
    {
        val &= ~(HDS_SM_MODE);
        ret = VHDS_register_write(cdc_mode_Regs, val);
    }
    VHDS_HW_Bclk_Disable();
    VHDS_Finit();
    VHDS_SetClock(clk_div);
    return ret;
}
