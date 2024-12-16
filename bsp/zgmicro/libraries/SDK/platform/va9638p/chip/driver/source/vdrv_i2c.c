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
    2009.07.28        chenjian            initial version, ported from NU820
    2009.07.30        chenjian            interrupt mode
    2022.05.30         caoxuetao                       9650
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_ckm.h"
#include "vdrv_i2c.h"
#include "vpla_i2c.h"


/******************************************************************************

                        I2C Control Register Bits

*******************************************************************************/
#define RBIT_I2C_CTRL_EN 27

/******************************************************************************

                        I2C Command Register Bits

*******************************************************************************/
#define RBIT_I2C_CMD_STA 5
#define RBIT_I2C_CMD_STO 4
#define RBIT_I2C_CMD_WR 3
#define RBIT_I2C_CMD_RD 2
#define RBIT_I2C_CMD_ACK 1

/******************************************************************************

                        I2C Status Register Bits

*******************************************************************************/
#define RBIT_I2C_STATUS_AL 3
#define RBIT_I2C_STATUS_RXACK 2
#define RBIT_I2C_STATUS_BUSY 1
#define RBIT_I2C_STATUS_TIP 0

/******************************************************************************

                        Macro Definition

*******************************************************************************/
/* I2C Process Status */
#define I2C_STATUS_IDLE 0       /* I2C Idle                               */
#define I2C_STATUS_SENDING 1   /* I2C Sending State                */
#define I2C_STATUS_AWAIT_ACK 2 /* I2C Awaiting ACK for Addr     */
#define I2C_STATUS_RECEIVING 3 /* I2C Receiving State              */
#define I2C_STATUS_RECV_LAST 4 /* I2C Receive Last                   */

#define I2C_STACON_OFF 0       /* Start Condition None              */
#define I2C_STACON_ON 1           /* Start Condition Occur             */
#define I2C_BYTE_CNT_DEFAULT 0 /* Start Condition Occur             */

/*I2C Max Time Count for Status Monitor */
#define I2C_MAX_TIME_COUNT 10000 /* Max Wait Time */
#define RBIT_AIF_CDCCTRL_I2CCLK_OUT_EN 27

/******************************************************************************

                        Data Type Definition

*******************************************************************************/
/* I2C Transmission data management structure object */
typedef struct
{
    UINT8 sladdr; /* Device Slave Address*/
    UINT8 *buf;      /* Transmission Data */
    UINT32 bytes; /* Transmission Data Size*/
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    void (*p_CallBack)(UINT32 Result); /*Callback function when transfer completes*/
#endif
} VI2C_TRANSINFO;

/* I2C Processing management structure object */
static struct
{
    UINT8 stacon_cnt;            /* Start condition occur*/
    UINT8 state;                /* I2C Processing Status*/
    UINT32 result;                /* I2C Processing Result*/
    UINT32 bytes_cnt;            /* */
    VI2C_TRANSINFO *trans_info; /* */
} g_I2C_Status;

static VI2C_TRANSINFO g_TraInfo;
/******************************************************************************

                        Internal Function Prototypes

*******************************************************************************/
static void _VI2C_StateManager(void);
#if I2C_WORK_MODE == I2C_WORK_MODE_POLL
static void _VI2C_ByteTransfer(void);
#endif

///
/// @brief [1:1]: Busy, I2C bus busy, high active
/// 
/// @return uint32_t Status of I2C bus busy.
///   1: 表示总线忙， 0:表示总线空闲
///
static uint32_t VI2C_Is_Bus_Busy(void)
{
    return REG_BIT_GET(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_Busy);
}

///
/// @brief [8:8]: Status of i2c_byte_idle 1:  i2c_byte is idle.
/// 
/// @return uint32_t Status of i2c_byte_idle.
///  1:  i2c_byte is idle
///  0:  i2c_byte is not idle
///
static uint32_t VI2C_Is_Byte_Idle(void)
{
    return REG_BIT_GET(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_I2C_BYTE_IDLE);
}

static int VI2C_Wait_Byte_Idle(void)
{
    int timeout = 500;
    while (timeout-- > 0)
    {
        if (VI2C_Is_Byte_Idle())
        {
            return 0;
        }
        vdelayus(1);
    }
    return -1;
}

///
/// @brief Wait for I2C Bus Idle.
/// 
/// @return int 0: successfully, -1: timeout
///
static int VI2C_Wait_Bus_Idle(void)
{
    int timeout = 500;
    while (timeout-- > 0)
    {
        if (!VI2C_Is_Bus_Busy())
        {
            return 0;
        }
        vdelayus(1);
    }
    return -1;
}

/******************************************************************************
Description:
    Enable AIF IIC Clock for VA9628

Parameters:
    void
Return Value:
    None.
*****************************************************************************/
void VI2C_EnableI2CClk(void)
{
    SET_REG_BIT(REG_AIF_CDC_CTRL_I2CCLK_OUT_EN);
}

/******************************************************************************
Description:
    Disable AIF IIC Clock for VA9628

Parameters:
    void
Return Value:
    None.
*****************************************************************************/
void VI2C_DisableI2CClk(void)
{
    CLR_REG_BIT(REG_AIF_CDC_CTRL_I2CCLK_OUT_EN);
}

/******************************************************************************
Description:
    I2C module initialize

Parameters:
    I2cClk - work clock of I2C core
    Mode - I2C transfer speed selection, one of below value must be chose
             I2C_MODE_STANDARD - 100 kb/s
             I2C_MODE_FAST - 400 kb/s
    SlaveDev - slave device selection, can be one of following constants:
               I2C_SLAVE_PROM, I2C_SLAVE_ANA

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VI2C_Initialize(UINT32 I2cClk, UINT8 Mode, UINT8 SlaveDev)
{
    UINT32 prescaling;
    UINT32 reg = 0;

    if (Mode == I2C_MODE_FAST)
        prescaling = I2cClk / (8 * 400 * 1000); // Fix_hzc when iic be used 400k, iic clk line must be set to 6M clock, but this case is Unstable, A small chance will occur NACK situation
    else if (Mode == I2C_MODE_SLOW)
        prescaling = I2cClk / (8 * 25 * 1000);
    else
        prescaling = I2cClk / (8 * 100 * 1000);

    if (prescaling > 2)
        prescaling -= 1;
    else
        prescaling = 2; /* 1 & 0 are unallowed */

    reg |= prescaling;
    reg |= BIT_MASK(RBIT_I2C_CTRL_EN);

    WRITE_REG(REG_I2C_I2C_CTRL, reg);

    WRITE_REG(REG_I2C_I2C_SLV_SEL, SlaveDev);

    /* State setting */
    g_I2C_Status.stacon_cnt = I2C_STACON_OFF;
    g_I2C_Status.state = I2C_STATUS_IDLE;
    g_I2C_Status.result = VRET_NO_ERROR;

#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    VCPU_INT_SrcEnable(IRQ_I2C_BYTE_DONE);
#endif
}

void VI2C_UnInitialize(void)
{
    CLR_REG_BIT2(REG_I2C_I2C_CTRL, REG_I2C_I2C_CTRL_EN);
    ;
}

/******************************************************************************
Description:
    Send start condition

Parameters:
    SlaveAddr - slave device address
    ReadOrWrite - used to indicate this operation, so if this operation is write, I2C_RWFLG_WRITE
                shall be used;if read, I2C_RWFLG_READ shall be used.
    p_CallBack - callback function pointer, this function will be called in head of bytedone-interrupt,
                CAN NOT be NULL

Return Value:
    void

Remarks:
    For interrupt mode, programer should use the following calling flow:

        1. VI2C_Start()
        2. VI2C_Write() or VI2C_Read()
        >>>>>>>>>>>>>>OPTIONAL>>>>>>>>>>>>>>
        3. Operation(Start, Write or Read) function if needed
            .........
        <<<<<<<<<<<<<<OPTIONAL<<<<<<<<<<<<<<
        n. VI2C_Stop()


Example:
    None.

See Also:
    None.
*****************************************************************************/

UINT32 VI2C_Start(UINT8 SlaveAddr, UINT8 ReadOrWrite
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                  ,
                  void (*p_CallBack)(UINT32 Result)
#endif
)
{
    g_I2C_Status.trans_info = &g_TraInfo;
    g_I2C_Status.trans_info->sladdr = (UINT8)((SlaveAddr & 0xFE) | ReadOrWrite);
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    if (p_CallBack == NULL)
    {
        g_I2C_Status.result = VRET_DRV_I2C_ERROR;
        return g_I2C_Status.result;
    }
    g_I2C_Status.trans_info->p_CallBack = p_CallBack;
#endif
    g_I2C_Status.stacon_cnt = I2C_STACON_ON;
    g_I2C_Status.state = I2C_STATUS_IDLE;
    g_I2C_Status.result = VRET_NO_ERROR;

    WRITE_REG(REG_I2C_I2C_BYTE_TX, g_I2C_Status.trans_info->sladdr);
    //VCOMMON_Delay(I2C_AHB_Delay);

    WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_STA) | BIT_MASK(RBIT_I2C_CMD_WR))); // start to transfer, salve address first
    return g_I2C_Status.result;
}

/******************************************************************************
Description:
    Read bytes from slave device

Parameters:
    p_Buf - used to store read bytes
    Num - number to be read
    p_CallBack - callback function pointer

Return Value:
    VRET_NO_ERROR - OK
    VRET_DRV_I2C_ERR_NACK_ADDR - No ACK for address
    VRET_DRV_I2C_ERR_NACK_DATA - No ACK for data
    VRET_DRV_I2C_ERR_ARBIT_LOST - Arbtration occurs

Remarks:
    Return value does not make sense in interrupt mode, call VI2C_GetLastErrorCode() if needed.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VI2C_Read(UINT8 *p_Buf, UINT32 Num
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                 ,
                 void (*p_CallBack)(UINT32 Result)
#endif
)
{
    /* Receive state set */
    g_I2C_Status.state = I2C_STATUS_AWAIT_ACK;
    g_I2C_Status.trans_info->buf = p_Buf;
    g_I2C_Status.trans_info->bytes = Num;
    g_I2C_Status.bytes_cnt = I2C_BYTE_CNT_DEFAULT;
    g_I2C_Status.result = VRET_DRV_I2C_ERROR;

#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    g_I2C_Status.trans_info->p_CallBack = p_CallBack;
    _VI2C_StateManager();

    return VRET_NO_ERROR;
#else
    _VI2C_StateManager();

    return g_I2C_Status.result;
#endif
}

/******************************************************************************
Description:
    Write bytes to slave device

Parameters:
    p_Buf - pointer to written bytes
    Num - number to be written
    p_CallBack - callback function pointer

Return Value:
    VRET_NO_ERROR - OK
    VRET_DRV_I2C_ERR_NACK_ADDR - No ACK for address
    VRET_DRV_I2C_ERR_NACK_DATA - No ACK for data
    VRET_DRV_I2C_ERR_ARBIT_LOST - Arbtration occurs

Remarks:
    Return value does not make sense in interrupt mode, call VI2C_GetLastErrorCode() if needed.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VI2C_Write(UINT8 *p_Buf, UINT32 Num
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                  ,
                  void (*p_CallBack)(UINT32 Result)
#endif
)
{
    g_I2C_Status.state = I2C_STATUS_SENDING;
    g_I2C_Status.trans_info->buf = p_Buf;
    g_I2C_Status.trans_info->bytes = Num;
    g_I2C_Status.bytes_cnt = I2C_BYTE_CNT_DEFAULT;
    g_I2C_Status.result = VRET_DRV_I2C_ERROR;

#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    g_I2C_Status.trans_info->p_CallBack = p_CallBack;
    _VI2C_StateManager();

    return VRET_NO_ERROR;
#else
    _VI2C_StateManager();

    return g_I2C_Status.result;
#endif
}

/******************************************************************************
Description:
    I2C bus status is returned to IDLE.

Parameters:
    void

Return Value:
    void

Remarks:
    Caller must delay a period after VI2C_Stop() called.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VI2C_Stop(void)
{
    SET_REG_BIT2(REG_I2C_I2C_CMD, REG_I2C_I2C_CMD_STO);
    g_I2C_Status.stacon_cnt = I2C_STACON_OFF;
    g_I2C_Status.state = I2C_STATUS_IDLE;
}

/******************************************************************************
Description:
    After the start condition is generated, the communication of every one byte is observed.

Parameters:
    void

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
static void _VI2C_StateManager(void)
{
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
    while (GET_REG_BIT(REG_I2C_I2C_STATUS_TIP))
        ;
    VI2C_ByteTransfer();
#else
    UINT8 tip;
    UINT16 retry = 0x3000; // 12288

    while (--retry)
    {
        if (g_I2C_Status.result != VRET_DRV_I2C_ERROR)
        {
            break;
        }
        vdelayus(10);
        tip = GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_TIP);
        if (tip != 1)
        {
            _VI2C_ByteTransfer();
        }
    }
#endif
}

static void VI2C_Re_7621err(void)
{ // VA7621 I2C sometimes maybe enter err status, so retry to let it recovery.
    UINT32 retry = 30;
    do
    {
        VI2C_Start(0, 0);
        retry--;
        VI2C_Stop();
    } while (retry);

#if 0
    //for test show LED
    VGPIO_SetMode(GPIO_PIN_8, GPIO_MODE_OUTPUT, GPIO_OUTPUT_LOW_STRENGTH);
    VGPIO_SetPinOutput(8, 1);
    
    //set the GPIO to do 7621 RST.
    if(g_err_cnt >= 3){
        VGPIO_SetMode(GPIO_PIN_1/*g_VCODEC_Plug_Config.io_mask*/, GPIO_MODE_OUTPUT, GPIO_OUTPUT_LOW_STRENGTH);
        VGPIO_SetPinOutput(1, 1);
        VGPIO_SetMode(GPIO_PIN_5, GPIO_MODE_OUTPUT, GPIO_OUTPUT_LOW_STRENGTH);
        VGPIO_SetPinOutput(5, 1);
        g_err_cnt = 0;
    }
#endif
}
/******************************************************************************
Description:
    The data of one byte is transmitted, and state changes.

Parameters:
    Slave - select slave device

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
void VI2C_ByteTransfer(void)
#else
static void _VI2C_ByteTransfer(void)
#endif
{
    UINT8 status;

    if (g_I2C_Status.state == I2C_STATUS_IDLE)
    {
        if (g_I2C_Status.stacon_cnt == I2C_STACON_ON)
        {
            g_I2C_Status.result = VRET_DRV_I2C_ERR_PROCESSING;
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
            g_I2C_Status.trans_info->p_CallBack(g_I2C_Status.result);
#endif
        }
        else
        {
            g_I2C_Status.result = VRET_DRV_I2C_ERROR;
        }
        return;
    }

    status = GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_AL);
    if (status != 0)
    {
        // 20150722 VA7621 may enter err status, so retry to let it recovery.
        VI2C_Re_7621err();

        g_I2C_Status.result = VRET_DRV_I2C_ERR_ARBIT_LOST;
        return;
    }

    switch (g_I2C_Status.state)
    {
    case I2C_STATUS_SENDING:
        status = GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_RxACK);
        if (status != 0)
        {
            g_I2C_Status.result = VRET_DRV_I2C_ERR_NACK_DATA;
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
            g_I2C_Status.trans_info->p_CallBack(g_I2C_Status.result);
#endif
        }
        else
        {
            if (g_I2C_Status.bytes_cnt < g_I2C_Status.trans_info->bytes)
            {
                WRITE_REG(REG_I2C_I2C_BYTE_TX, g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt]);
                g_I2C_Status.bytes_cnt++;
                WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_WR));
                //VCOMMON_Delay(I2C_AHB_Delay);
            }
            else
            {
                g_I2C_Status.result = VRET_NO_ERROR;
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
                g_I2C_Status.trans_info->p_CallBack(g_I2C_Status.result);
#endif
            }
        }
        break;
    case I2C_STATUS_AWAIT_ACK:
        status = GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_RxACK);
        if (status != 0)
        {
            g_I2C_Status.result = VRET_DRV_I2C_ERR_NACK_ADDR;
        }
        else
        {
            if (g_I2C_Status.trans_info->bytes == 1)
            {
                WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_RD) | BIT_MASK(RBIT_I2C_CMD_ACK)));
                g_I2C_Status.state = I2C_STATUS_RECV_LAST;
            }
            else
            {
                WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_RD));
                g_I2C_Status.state = I2C_STATUS_RECEIVING;
            }
        }
        break;
    case I2C_STATUS_RECEIVING:
        g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt] = READ_REG(REG_I2C_I2C_BYTE_RX);
        g_I2C_Status.bytes_cnt++;
        if ((g_I2C_Status.bytes_cnt + 1) == g_I2C_Status.trans_info->bytes)
        {
            WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_RD) | BIT_MASK(RBIT_I2C_CMD_ACK)));
            g_I2C_Status.state = I2C_STATUS_RECV_LAST;
        }
        else
        {
            WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_RD));
            g_I2C_Status.state = I2C_STATUS_RECEIVING;
        }
        break;
    case I2C_STATUS_RECV_LAST:
        g_I2C_Status.trans_info->buf[g_I2C_Status.bytes_cnt] = READ_REG(REG_I2C_I2C_BYTE_RX);
        g_I2C_Status.bytes_cnt++;
        g_I2C_Status.result = VRET_NO_ERROR;
#if I2C_WORK_MODE == I2C_WORK_MODE_INT
        g_I2C_Status.trans_info->p_CallBack(g_I2C_Status.result);
#endif
        break;
    default:
        break;
    }
}

/******************************************************************************
Description:
    Clear interrupts

Parameters:
    IntFlag - interrupt flag waited to clear, this parameter must be enumerated as a bit field of
            I2C_INTCLR_BYTEDONE_MASK
            I2C_INTCLR_ARBLOST_MASK

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VI2C_ClearInterrupts(UINT8 IntFlag)
{
    if (IntFlag & I2C_INTCLR_BYTEDONE_MASK)
        SET_REG_BIT2(REG_I2C_I2C_INTCLR, REG_I2C_I2C_INTCLR_BYTE_DONE_CLR);
    if (IntFlag & I2C_INTCLR_ARBLOST_MASK)
        SET_REG_BIT2(REG_I2C_I2C_INTCLR, REG_I2C_I2C_INTCLR_ARB_LST_CLR);
}

/******************************************************************************
Description:
    Get the error code

Parameters:
    Slave - select slave device

Return Value:
    error code

Remarks:
    This function is useful in interrupt mode, becasue interrupt mode is asynchronous, user can't
    get error code from returned value of VI2C_Write() or VI2C_Read() .

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VI2C_GetLastErrorCode(void)
{
    return g_I2C_Status.result;
}

VRET_VALUE VI2C_register_read(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT32 retry = I2C_RETRY_CNT;
    UINT32 delay;
    UINT8 data = Reg;

    VI2C_EnableI2CClk();
    do
    {
        VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, SlaveDev);
        VI2C_Start((dev_addr << 1), I2C_RWFLG_WRITE);
        //VCOMMON_Delay(I2C_AHB_Delay);
        ret = VI2C_Write(&data, 1);
        //VCOMMON_Delay(I2C_AHB_Delay);
        if (ret == VRET_NO_ERROR)
        {
            VI2C_Start((dev_addr << 1), I2C_RWFLG_READ);
            //VCOMMON_Delay(I2C_AHB_Delay);
            ret = VI2C_Read(p_Value, 1);
            //VCOMMON_Delay(I2C_AHB_Delay);
        }
        VI2C_Stop();
        // 等待总线空闲+Byte IDLE
        if (VI2C_Wait_Bus_Idle()) {
            ret = -1;
        } else if (VI2C_Wait_Byte_Idle()) {
            ret = -1;
        }

        if (ret)
        {
            // I2C_DEBUG ( "%s: Reading R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret );
            retry--;
            delay = I2C_RETRY_DELAY * (I2C_RETRY_CNT - retry);
            if (i2c_mode == I2C_MODE_SLOW)
                vdelayus(delay*2);
            else
                vdelayus(delay);
        }
        VI2C_UnInitialize();
    } while (ret && retry);
    VI2C_DisableI2CClk();
    if (ret)
    {
        I2C_DEBUG("%s: Reading D0x%02x R0x%02x failed! (0x%X)\r\n", FUNCTION_NAME, dev_addr,Reg, ret);
    }

    return ret;
}

VRET_VALUE VI2C_register_write(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT32 retry = I2C_RETRY_CNT;
    UINT32 delay;
    UINT8 data[2] = {0, 0};
    data[0] = Reg;
    data[1] = Value;

    VI2C_EnableI2CClk();
    do
    {
        VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, SlaveDev);
        VI2C_Start((dev_addr << 1), I2C_RWFLG_WRITE);
        //VCOMMON_Delay(I2C_AHB_Delay);
        ret = VI2C_Write(&data[0], 2);
        //VCOMMON_Delay(I2C_AHB_Delay);
        if (ret)
        {
            // I2C_DEBUG ( "%s: Writing R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret );
            retry--;
            delay = I2C_RETRY_DELAY * (I2C_RETRY_CNT - retry);
            if (i2c_mode == I2C_MODE_SLOW)
                vdelayus(delay*2);
            else
                vdelayus(delay);
        }
        VI2C_Stop();
        // 等待总线空闲+Byte IDLE
        if (VI2C_Wait_Bus_Idle()) {
            ret = -1;
        } else if (VI2C_Wait_Byte_Idle()) {
            ret = -1;
        }
        VI2C_UnInitialize();
    } while (ret && retry);
    VI2C_DisableI2CClk();
    if (ret)
    {
        I2C_DEBUG("%s: Writing D0x%02x R0x%02x failed! (0x%X)\r\n", FUNCTION_NAME, dev_addr, Reg, ret);
    }
    return ret;
}

VRET_VALUE VI2C_Continue_read(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT32 retry = I2C_RETRY_CNT;
    UINT32 delay;
    UINT16 time_delay = 0x3000; // 12288

    VI2C_EnableI2CClk();
    do
    {
        VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, SlaveDev);
        WRITE_REG(REG_I2C_I2C_CONTIRW_ADDR, (((dev_addr & 0xFF) << 8)) | (Reg));
        SET_REG_BIT2(REG_I2C_I2C_CONTIRW_CMD, REG_I2C0_I2C_CONTIRW_CMD_CONTI_READ);
        while ((--time_delay) && (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_I2C_CONTI_BUSY)))
            ;
        if (time_delay == 0)
            ret = VRET_DRV_I2C_ERR_ARBIT_LOST;
        *p_Value = READ_REG(REG_I2C_I2C_BYTE_RX);
        if (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_I2C_CONTI_ERR))
            ret = VRET_DRV_I2C_ERR_NACK_DATA;
        if (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_AL))
            ret = VRET_DRV_I2C_ERR_ARBIT_LOST;
        VI2C_Stop();
        if (ret)
        {
            // I2C_DEBUG ( "%s: Reading R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret );
            retry--;
            delay = I2C_RETRY_DELAY * (I2C_RETRY_CNT - retry);
            if (i2c_mode == I2C_MODE_SLOW)
                vdelayus(delay*2);
            else
                vdelayus(delay);
        }
        VI2C_UnInitialize();
    } while (ret && retry);
    VI2C_DisableI2CClk();
    if (ret)
    {
        I2C_DEBUG("%s: Reading R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret);
    }

    return ret;
}

VRET_VALUE VI2C_Continue_write(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT32 retry = I2C_RETRY_CNT;
    UINT32 delay;
    UINT16 time_delay = 0x3000; // 12288

    VI2C_EnableI2CClk();
    do
    {
        VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, SlaveDev);
        WRITE_REG(REG_I2C_I2C_CONTIRW_ADDR, (((dev_addr & 0xFF) << 8)) | (Reg));
        WRITE_REG(REG_I2C_I2C_BYTE_TX, Value);
        SET_REG_BIT2(REG_I2C_I2C_CONTIRW_CMD, REG_I2C0_I2C_CONTIRW_CMD_CONTI_WRITE);
        while ((--time_delay) && (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_I2C_CONTI_BUSY)))
            ;
        if (time_delay == 0)
            ret = VRET_DRV_I2C_ERR_ARBIT_LOST;
        if (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_I2C_CONTI_ERR))
            ret = VRET_DRV_I2C_ERR_NACK_DATA;
        if (GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_AL))
            ret = VRET_DRV_I2C_ERR_ARBIT_LOST;
        if (ret)
        {
            // I2C_DEBUG ( "%s: Writing R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret );
            retry--;
            delay = I2C_RETRY_DELAY * (I2C_RETRY_CNT - retry);
            if (i2c_mode == I2C_MODE_SLOW)
                vdelayus(delay*2);
            else
                vdelayus(delay);
        }
        VI2C_Stop();
        VI2C_UnInitialize();
    } while (ret && retry);
    VI2C_DisableI2CClk();
    if (ret)
    {
        I2C_DEBUG("%s: Writing R%02d failed, retrying! (0x%X)\r\n", FUNCTION_NAME, Reg, ret);
    }
    return ret;
}

#if CONFIG_VKERNEL_I2C_INTERRUPT_ENABLE
VI2C_IRQ_ST g_i2c_irq_st = { i2c_idle_e,};
UINT32 i2c_irq_start(UINT8 SlaveDev, UINT8 i2c_mode, VI2C_RWFLG ReadOrWrite, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value, UINT8 len)
{
    UINT32 timeout = 10000000;
    if(len == 0 || p_Value == NULL) return i2c_state_PARERR_e;
    VCPU_SR_ALLOC();
    while (1)
    {
        while (g_i2c_irq_st.work_stage != i2c_idle_e)
        {
            if (timeout-- == 0){
                g_i2c_irq_st.work_stage = i2c_idle_e;
                CLR_REG_BIT2(REG_I2C_I2C_CTRL, REG_I2C_I2C_CTRL_EN);
                break;
            }
        }
        VCPU_CRITICAL_ENTER();
        if (g_i2c_irq_st.work_stage == i2c_idle_e)
        {
            g_i2c_irq_st.work_stage = i2c_init_e;
            VCPU_CRITICAL_EXIT();
            break;
        }
        VCPU_CRITICAL_EXIT();
    }
    g_i2c_irq_st.dev_addr = dev_addr;
    g_i2c_irq_st.Reg = Reg;
    g_i2c_irq_st.bytes_cnt = 0;
    g_i2c_irq_st.state = i2c_state_NOERR_e;
    g_i2c_irq_st.bytes = len;
    VI2C_EnableI2CClk();
    VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, SlaveDev);
    VCPU_INT_SrcEnable(IRQ_I2C_INT);

    switch (ReadOrWrite)
    {
    case i2c_rwflg_set_e:
        g_i2c_irq_st.ReadOrWrite = i2c_rwflg_reade_e;
        g_i2c_irq_st.p_Value = &g_i2c_irq_st.reval_tmp;
        g_i2c_irq_st.is_set_or_clr = ReadOrWrite;
        g_i2c_irq_st.wrval_tmp = *p_Value;
        break;
    case i2c_rwflg_clr_e:
        g_i2c_irq_st.ReadOrWrite = i2c_rwflg_reade_e;
        g_i2c_irq_st.p_Value = &g_i2c_irq_st.reval_tmp;
        g_i2c_irq_st.is_set_or_clr = ReadOrWrite;
        g_i2c_irq_st.wrval_tmp = *p_Value;
        break;
    case i2c_rwflg_continue_set_e:
        g_i2c_irq_st.ReadOrWrite = i2c_rwflg_continue_reade_e;
        g_i2c_irq_st.p_Value = &g_i2c_irq_st.reval_tmp;
        g_i2c_irq_st.is_set_or_clr = ReadOrWrite;
        g_i2c_irq_st.wrval_tmp = *p_Value;
        break;
    case i2c_rwflg_continue_clr_e:
        g_i2c_irq_st.ReadOrWrite = i2c_rwflg_continue_reade_e;
        g_i2c_irq_st.p_Value = &g_i2c_irq_st.reval_tmp;
        g_i2c_irq_st.is_set_or_clr = ReadOrWrite;
        g_i2c_irq_st.wrval_tmp = *p_Value;
        break;
    default:
        g_i2c_irq_st.ReadOrWrite = ReadOrWrite;
        g_i2c_irq_st.p_Value = p_Value;
        g_i2c_irq_st.is_set_or_clr = 0;
        break;    
    }
    switch (g_i2c_irq_st.ReadOrWrite)
    {
    case i2c_rwflg_write_e:
        if(len == 1){
            g_i2c_irq_st.wrval_tmp = *p_Value;
            g_i2c_irq_st.p_Value = &g_i2c_irq_st.wrval_tmp;
        }
    case i2c_rwflg_reade_e:
        g_i2c_irq_st.work_stage = i2c_wr_start_e;
        WRITE_REG(REG_I2C_I2C_BYTE_TX, (UINT8)((g_i2c_irq_st.dev_addr << 1) | I2C_RWFLG_WRITE));
        WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_STA) | BIT_MASK(RBIT_I2C_CMD_WR)));
        break;
    case i2c_rwflg_continue_write_e:
        if(len == 1){
            g_i2c_irq_st.wrval_tmp = *p_Value;
            g_i2c_irq_st.p_Value = &g_i2c_irq_st.wrval_tmp;
        }
        if (g_i2c_irq_st.p_Value == NULL) {
            g_i2c_irq_st.state = i2c_state_PARERR_e;
            return i2c_state_PARERR_e;
        }
        g_i2c_irq_st.work_stage = i2c_wr_value_e;
        WRITE_REG(REG_I2C_I2C_CONTIRW_ADDR, (((g_i2c_irq_st.dev_addr & 0xFF) << 8)) | (g_i2c_irq_st.Reg));
        WRITE_REG(REG_I2C_I2C_BYTE_TX, g_i2c_irq_st.p_Value[g_i2c_irq_st.bytes_cnt++]);
        SET_REG_BIT2(REG_I2C_I2C_CONTIRW_CMD, REG_I2C0_I2C_CONTIRW_CMD_CONTI_WRITE);
        break;
    case i2c_rwflg_continue_reade_e:
        g_i2c_irq_st.work_stage = i2c_re_value_e;
        WRITE_REG(REG_I2C_I2C_CONTIRW_ADDR, (((g_i2c_irq_st.dev_addr & 0xFF) << 8)) | (g_i2c_irq_st.Reg));
        SET_REG_BIT2(REG_I2C_I2C_CONTIRW_CMD, REG_I2C0_I2C_CONTIRW_CMD_CONTI_READ);
        break;
    case i2c_rwflg_set_e:
        break;
    case i2c_rwflg_clr_e:
        break;
    case i2c_rwflg_continue_set_e:
        break;
    case i2c_rwflg_continue_clr_e:
        break;
    default:
        break;    
    }
    return i2c_state_NOERR_e;
}

UINT32 i2c_irq_done(void)
{
    UINT32 timeout = 500000;
    while (g_i2c_irq_st.work_stage != i2c_idle_e)
    {
        if (timeout-- == 0)
            return i2c_state_TIMEOUT_e;
    }
    return g_i2c_irq_st.state;
}

UINT32 VI2C_register_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value)
{
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_reade_e, dev_addr, Reg, p_Value, 1);
}

UINT32 VI2C_register_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_write_e, dev_addr, Reg, &Value_v, 1);
}

UINT32 VI2C_Continue_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value) 
{
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_continue_reade_e, dev_addr, Reg, p_Value, 1);
}

UINT32 VI2C_Continue_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_continue_write_e, dev_addr, Reg, &Value_v, 1);
}

UINT32 VI2C_register_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_set_e, dev_addr, Reg, &Value_v, 1);
}

UINT32 VI2C_register_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_clr_e, dev_addr, Reg, &Value_v, 1);
}

UINT32 VI2C_Continue_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_continue_set_e, dev_addr, Reg, &Value_v, 1);
}

UINT32 VI2C_Continue_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value)
{
	UINT8 Value_v = Value; 
	return i2c_irq_start(SlaveDev, i2c_mode, i2c_rwflg_continue_clr_e, dev_addr, Reg, &Value_v, 1);
}

void VI2C_Interrupt_Routine_Service(void)
{
    UINT32 state = 0;
    state = READ_REG(REG_I2C_I2C_STATUS);

    if (state & BIT_MASK(REG_I2C_I2C_STATUS_I2C_ARB_LOST))
        g_i2c_irq_st.state = i2c_state_ARB_LOST_e;
    else if (state & BIT_MASK(REG_I2C_I2C_STATUS_I2C_CONTI_ERR))
        g_i2c_irq_st.state = i2c_state_CONTI_ERR_e;
    else if (state & BIT_MASK(REG_I2C_I2C_STATUS_AL))
        g_i2c_irq_st.state = i2c_state_AL_e;
    else if (g_i2c_irq_st.work_stage == i2c_wr_start_e)
    {
        if (!GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_RxACK))
        {
            g_i2c_irq_st.work_stage = i2c_wr_reg_e;
            WRITE_REG(REG_I2C_I2C_BYTE_TX, g_i2c_irq_st.Reg);
            WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_WR));
        }
        else
            g_i2c_irq_st.state = i2c_state_NACK_e;
    }
    else
    {
        switch (g_i2c_irq_st.ReadOrWrite)
        {
        case i2c_rwflg_continue_reade_e:
            if (state & BIT_MASK(REG_I2C_I2C_STATUS_I2C_CONTI_BUSY))
            {
                break;
            }
        case i2c_rwflg_reade_e:
            if (g_i2c_irq_st.work_stage == i2c_wr_reg_e)
            {
                g_i2c_irq_st.work_stage = i2c_re_start_e;
                WRITE_REG(REG_I2C_I2C_BYTE_TX, (UINT8)((g_i2c_irq_st.dev_addr << 1) | I2C_RWFLG_READ));
                WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_STA) | BIT_MASK(RBIT_I2C_CMD_WR)));
            }
            else if (g_i2c_irq_st.work_stage == i2c_re_start_e)
            {
                g_i2c_irq_st.work_stage = i2c_re_value_e;
                if (!GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_RxACK))
                {
                    if (g_i2c_irq_st.bytes == 1)
                        WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_RD) | BIT_MASK(RBIT_I2C_CMD_ACK)));
                    else
                        WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_RD));
                }
                else
                    g_i2c_irq_st.state = i2c_state_NACK_e;
            }
            else if (g_i2c_irq_st.work_stage == i2c_re_value_e)
            {
                if (g_i2c_irq_st.bytes_cnt < g_i2c_irq_st.bytes)
                {
                    g_i2c_irq_st.p_Value[g_i2c_irq_st.bytes_cnt++] = READ_REG(REG_I2C_I2C_BYTE_RX);
                    if (g_i2c_irq_st.bytes_cnt + 1 == g_i2c_irq_st.bytes)
                        WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_RD) | BIT_MASK(RBIT_I2C_CMD_ACK)));
                    else if (g_i2c_irq_st.bytes_cnt + 1 < g_i2c_irq_st.bytes)
                        WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_RD));
                }
                if (g_i2c_irq_st.bytes_cnt >= g_i2c_irq_st.bytes)
                    g_i2c_irq_st.work_stage = i2c_idle_e;
            }
            break;
        case i2c_rwflg_continue_write_e:
            if (state & BIT_MASK(REG_I2C_I2C_STATUS_I2C_CONTI_BUSY))
            {
                break;
            }
        case i2c_rwflg_write_e:
            if (g_i2c_irq_st.work_stage == i2c_wr_reg_e || g_i2c_irq_st.work_stage == i2c_wr_value_e)
            {
                if (g_i2c_irq_st.bytes_cnt < g_i2c_irq_st.bytes)
                {
                    if (!GET_REG_BIT2(REG_I2C_I2C_STATUS, REG_I2C_I2C_STATUS_RxACK))
                    {
                        g_i2c_irq_st.work_stage = i2c_wr_value_e;
                        WRITE_REG(REG_I2C_I2C_BYTE_TX, g_i2c_irq_st.p_Value[g_i2c_irq_st.bytes_cnt++]);
                        WRITE_REG(REG_I2C_I2C_CMD, BIT_MASK(RBIT_I2C_CMD_WR));
                    }
                    else
                        g_i2c_irq_st.state = i2c_state_NACK_e;
                }
                else
                    g_i2c_irq_st.work_stage = i2c_idle_e;
            }
            break;
        default:
            break;
        }
    }
    if (g_i2c_irq_st.work_stage == i2c_idle_e || g_i2c_irq_st.state != i2c_state_NOERR_e)
    {
        SET_REG_BIT2(REG_I2C_I2C_CMD, REG_I2C_I2C_CMD_STO);
        CLR_REG_BIT2(REG_I2C_I2C_CTRL, REG_I2C_I2C_CTRL_EN);
        if(g_i2c_irq_st.work_stage == i2c_idle_e && g_i2c_irq_st.is_set_or_clr != 0){
            SET_REG_BIT2(REG_I2C_I2C_CTRL, REG_I2C_I2C_CTRL_EN);
            if(g_i2c_irq_st.is_set_or_clr == i2c_rwflg_set_e || g_i2c_irq_st.is_set_or_clr == i2c_rwflg_continue_set_e)
                g_i2c_irq_st.wrval_tmp |= g_i2c_irq_st.reval_tmp;
            else
                g_i2c_irq_st.wrval_tmp = g_i2c_irq_st.reval_tmp & ~g_i2c_irq_st.wrval_tmp;
            g_i2c_irq_st.p_Value = &g_i2c_irq_st.wrval_tmp;
            if(g_i2c_irq_st.is_set_or_clr == i2c_rwflg_set_e || g_i2c_irq_st.is_set_or_clr == i2c_rwflg_clr_e){
                g_i2c_irq_st.ReadOrWrite = i2c_rwflg_write_e;
                g_i2c_irq_st.work_stage = i2c_wr_start_e;
                g_i2c_irq_st.bytes_cnt = 0;
                WRITE_REG(REG_I2C_I2C_BYTE_TX, (UINT8)((g_i2c_irq_st.dev_addr << 1) | I2C_RWFLG_WRITE));
                WRITE_REG(REG_I2C_I2C_CMD, (BIT_MASK(RBIT_I2C_CMD_STA) | BIT_MASK(RBIT_I2C_CMD_WR)));
            }else if(g_i2c_irq_st.is_set_or_clr == i2c_rwflg_continue_set_e || g_i2c_irq_st.is_set_or_clr == i2c_rwflg_continue_clr_e){
                g_i2c_irq_st.ReadOrWrite = i2c_rwflg_continue_write_e;
                g_i2c_irq_st.work_stage = i2c_wr_value_e;
                g_i2c_irq_st.bytes_cnt = 1;
                WRITE_REG(REG_I2C_I2C_CONTIRW_ADDR, (((g_i2c_irq_st.dev_addr & 0xFF) << 8)) | (g_i2c_irq_st.Reg));
                WRITE_REG(REG_I2C_I2C_BYTE_TX, g_i2c_irq_st.p_Value[0]);
                SET_REG_BIT2(REG_I2C_I2C_CONTIRW_CMD, REG_I2C0_I2C_CONTIRW_CMD_CONTI_WRITE);
            }
            g_i2c_irq_st.is_set_or_clr = 0;
        }else{
            VI2C_DisableI2CClk();
            VCPU_INT_SrcDisable(IRQ_I2C_INT);
            g_i2c_irq_st.work_stage = i2c_idle_e;
        }
    }
    VI2C_ClearInterrupts(I2C_INTCLR_BYTEDONE_MASK);
    // printf("i2c irq:0x%08x\r\n",state);
}

#else
UINT32 VI2C_Continue_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 VI2C_Continue_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 VI2C_register_set_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 VI2C_register_clr_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 VI2C_register_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value) { return 1; }
UINT32 VI2C_register_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 VI2C_Continue_read_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 *p_Value) { return 1; }
UINT32 VI2C_Continue_write_irq(UINT8 SlaveDev, UINT8 i2c_mode, UINT8 dev_addr, UINT8 Reg, UINT8 Value) { return 1; }
UINT32 i2c_irq_done(void) { return 1; }
void VI2C_Interrupt_Routine_Service(void) { return ; }
#endif
VRET_VALUE VI2C_Xfer(UINT8 i2c_sel, UINT8 i2c_mode, struct i2c_msg msgs[], UINT8 num)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT32 retry = I2C_RETRY_CNT;
    UINT32 delay;
    struct i2c_msg *msg;

    VI2C_EnableI2CClk();

    do
    {
        VI2C_Initialize(VCKM_GetI2CClockFreq(), i2c_mode, i2c_sel);

        for (int i = 0; i < num; i++)
        {
            msg = &msgs[i];
            if (!(msg->flags & I2C_NO_START))
            {
                // TODO: 增加对I2C_NO_START的处理
            }
            if (msg->flags & I2C_RD)
            {
                VI2C_Start((msg->addr << 1), I2C_RWFLG_READ);
                ret = VI2C_Read(msg->buf, msg->len);
            } else {
                VI2C_Start((msg->addr << 1), I2C_RWFLG_WRITE);
                ret = VI2C_Write(msg->buf, msg->len);
            }
        }
        VI2C_Stop();
        // 等待总线空闲+Byte IDLE
        if (VI2C_Wait_Bus_Idle()) {
            ret = -1;
        } else if (VI2C_Wait_Byte_Idle()) {
            ret = -1;
        }
        VI2C_UnInitialize();
        if (ret)
        {
            retry--;
            delay = I2C_RETRY_DELAY * (I2C_RETRY_CNT - retry);
            if (i2c_mode == I2C_MODE_SLOW) {
                vdelayus(delay*2);
            } else {
                vdelayus(delay);
            }
        }
    } while (ret && retry);

    VI2C_DisableI2CClk();
    if (ret)
    {
        I2C_DEBUG("%s: failed! ret=%d\r\n", FUNCTION_NAME, ret);
    }
    return ret;
}
