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
    UART

Description:
    UART Driver

Revision History:
    Date                  Author                   Description
    2009.04.08      chenjian            initial version
    2022.05.30            cxt                       9650
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vreg_def.h"
#include "vdrv_uart.h"
#include "vdrv_dma.h"
#include "vstdcore.h"

/******************************************************************************
Description:
    From DA on, the UART pins can be remapped as:
    Primary: full UART port (for communication under function mode).(@GPIO[4:1])
    Aux:       auxiliary port (for debug tracing, test and production, no CTS/RTS).(@GPIO[14:13]).

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
void VUART_SelectPort ( UINT8 port_sel )
{
#if(VUART_PORT_REMAP_EN == 1)
    if ( port_sel == VUART_PORT_SEL_AUX ) {
        SET_REG_BIT2 ( REG_UART_UER, RBIT_UART_UER_UPSEL );
    } else {
        CLR_REG_BIT2 ( REG_UART_UER, RBIT_UART_UER_UPSEL );
    }
#endif
}

/******************************************************************************
Description:
    UART,Rx and Tx logic are enabled.
    Before UART,Rx and Tx are enabled, Rx and Tx FIFOs are cleared, and their logic are reset.
    But the shift registers are not cleared.

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
void VUART_Enable ( void )
{
    UINT32 reg;

    /*Reset UART RX and TX FIFOs*/
    VUART_ResetFifo(UART_BOTH_FIFO);

    /*Enable UART, UART Rx, UART Tx*/
    reg = READ_REG(REG_UART_UER);
    reg = reg | REG_BIT_MASK(RBIT_UART_UER_UEN) | REG_BIT_MASK(RBIT_UART_UER_REN) | REG_BIT_MASK(RBIT_UART_UER_TEN);
    WRITE_REG(REG_UART_UER, reg);

    // ??????????
    SET_REG_BIT2 ( REG_UART_UGENTXINT, 0 );
}

/******************************************************************************
Description:
    UART,Rx and Tx logic are disabled after Tx is over.

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
void VUART_Disable ( void )
{
    UINT32 reg;

    /*Wait until Tx over*/
    while ( !GET_REG_BIT2 ( REG_UART_UFLSR, RBIT_UART_UFLSR_TREPT ) )
        ;

    reg = READ_REG(REG_UART_UER);
    reg &= ~(REG_BIT_MASK(RBIT_UART_UER_UEN) | REG_BIT_MASK(RBIT_UART_UER_REN) | REG_BIT_MASK(RBIT_UART_UER_TEN));
    /*Disable UART, UART Rx, UART Tx*/
    WRITE_REG(REG_UART_UER, reg);

    CLR_REG_BIT2 ( REG_UART_UGENTXINT, 0 );
}

/******************************************************************************
Description:
    Set the baud rate of UART.


    UBCR register is used to specify the divisor for the baud rate generation block.
    The baud rate divisor contains two parts, integer part (DIV_integer) and fractional part (DIV_frac).
        DIVISOR = DIV_integer + (DIV_frac / 256)
    DIVISOR can be calculated as
        DIVISOR = F_UARTclk / (F_baud x Nsampclk)
    Here, F_ UARTclk is the frequency of uart_clk, F_baud is the required baud rate, Nsampclk is
    the number of the sample clock for one bit. When the baud rate is lower than 3Mbps, Nsampclk is
    16. When the baud rate is 12Mbps, Nsampclk is 4.


Parameters:
    baudrate - Expired baudrate of UART
    uartclk - The clock supplied to uart module

Return Value:
    void

Remarks:
    DIV_integer is at least 1.

Example:
    if the UART Clock is 24MHz and the required baud rate is 115.2 KHz. Then
        DIVISOR = 24000000 / (115200 x 16) = 13.020833
        DIV_integer = 13,
        DIV_frac = integer (0.020833 x 256) = 5
        DIVISOR_actual = 13 + 5/256 = 13.01953125
        F_baud_actual =
            24000000 / ( DIVISOR_actual * 16) = 24000000 / (13. 01953125 * 16 ) = 115211 Hz
        Error = (115211 - 115200) / 115200 * 100% = 0.01%

See Also:
    None.
*****************************************************************************/
void VUART_SetBaudRate ( UINT32 Baudrate, UINT32 UartClk )
{
    UINT32 div_int;
    UINT32 div_frac;
    UINT32 rclk_ini;
    UINT32 rclk_mid;

    if ( Baudrate == 0 ) {
        return;
    }

    if ( Baudrate <= 3000000 ) {
        div_int = ( UINT32 ) ( UartClk / ( Baudrate * 16 ) );
        div_frac = ( UINT32 ) ( UartClk / ( Baudrate / ( 256 / 16 ) ) - div_int * 256 );
        rclk_ini = 15;
        rclk_mid = 8;
    } else {
        div_int = 1;
        div_frac = 0;
        rclk_ini = ( UartClk / Baudrate ) - 1;
        rclk_mid = ( rclk_ini + ( ( rclk_ini & 0x1 ) ? 1 : 2 ) ) / 2;
    }
    WRITE_REG ( REG_UART_UBCR, ( ( div_frac << 16 ) + div_int ) );
    WRITE_REG ( REG_UART_UNSAMPCLK, ( rclk_mid << 4 ) + rclk_ini );
}

/******************************************************************************
Description:
    Set parity mode of uart Tx and Rx.

Parameters:
    pairty - must be one of UART_CONF_PARITY_NONE, UART_CONF_PARITY_ODD or UART_CONF_PARITY_EVEN
    stopbit - must be one of UART_CONF_STOP_BIT1 or UART_CONF_STOP_BIT2
    bit8 - must be one of UART_CONF_CHAR_BIT7 or UART_CONF_CHAR_BIT8

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_Config ( UINT8 Parity, UINT8 StopBit, UINT8 Bit8 )
{
    //ASSERT(parity == UART_CONF_PARITY_NONE || parity == UART_CONF_PARITY_ODD || parity == UART_CONF_PARITY_EVEN);
    //ASSERT(stopbit == UART_CONF_STOP_BIT1 || stopbit == UART_CONF_STOP_BIT2);
    //ASSERT(bit8 == UART_CONF_CHAR_BIT7 || bit8 == UART_CONF_CHAR_BIT8);

    CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_SP );

    if ( Parity == UART_CONF_PARITY_NONE ) {
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_PEN );
    } else if ( Parity == UART_CONF_PARITY_ODD ) {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_PEN );
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_EPS );
    } else {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_PEN );
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_EPS );
    }

    if ( StopBit == UART_CONF_STOP_BIT1 ) {
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_STP2 );
    } else {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_STP2 );
    }

    if ( Bit8 == UART_CONF_CHAR_BIT7 ) {
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_BIT8 );
    } else {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_BIT8 );
    }
}

/******************************************************************************
Description:
    Set flow control of uart Tx and Rx.

Parameters:
    rtsen - enable/disable receiver hardware flow control
    ctsen - enable/disable transmitter hardware flow control

Return Value:
    void

Remarks:
    UART_FLOW_DISABLE - disable
    UART_FLOW_ENABLE - enable

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_FlowControl ( UINT8 Rtsen, UINT8 Ctsen )
{
    //ASSERT(Rtsen == UART_FLOW_DISABLE || Rtsen == UART_FLOW_ENABLE);
    //ASSERT(Ctsen == UART_FLOW_DISABLE || Ctsen == UART_FLOW_ENABLE);

    if ( Rtsen == UART_FLOW_DISABLE ) {
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_RTSEN );
    } else {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_RTSEN );
    }

    if ( Ctsen == UART_FLOW_DISABLE ) {
        CLR_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_CTSEN );
    } else {
        SET_REG_BIT2 ( REG_UART_UCR, RBIT_UART_UCR_CTSEN );
    }
}

/******************************************************************************
Description:
    Enable interrupts.

Parameters:
    no

Return Value:
    void

Remarks:
    Only needed in interrupt mode.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_EnableInt ( void )
{
	VCPU_INT_SrcClearPending(IRQ_UART_INT);
    VCPU_INT_SrcEnable ( IRQ_UART_INT );
}

/******************************************************************************
Description:
    Disable interrupts.

Parameters:
    no

Return Value:
    void

Remarks:
    Only needed in interrupt mode.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_DisableInt ( void )
{
    VCPU_INT_SrcDisable ( IRQ_UART_INT );
}

/******************************************************************************
Description:
    Mask errors.

Parameters:
    Errors - set of error masked bits
        UART_ERR_TO_MASK
        UART_ERR_OE_MASK
        UART_ERR_PE_MASK
        UART_ERR_FE_MASK
        UART_ERR_BI_MASK
        UART_ERR_CC_MASK
        UART_ERR_TX_MASK
        UART_ERR_RX_MASK

Return Value:
    void

Remarks:

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VUART_SetMask ( UINT32 Errors )
{
    UINT32 value;

    value = READ_REG ( REG_UART_UERR_MSK );
    value |= Errors;
    WRITE_REG ( REG_UART_UERR_MSK, value );
    return value;
}

/******************************************************************************
Description:
    Unmask errors.

Parameters:
    Errors - set of error masked bits
        UART_ERR_TO_MASK
        UART_ERR_OE_MASK
        UART_ERR_PE_MASK
        UART_ERR_FE_MASK
        UART_ERR_BI_MASK
        UART_ERR_CC_MASK
        UART_ERR_TX_MASK
        UART_ERR_RX_MASK
Return Value:
    void

Remarks:

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_Unmask ( UINT32 Errors )
{
    UINT32 value;

    value = READ_REG ( REG_UART_UERR_MSK );
    value &= ~Errors;
    WRITE_REG ( REG_UART_UERR_MSK, value );
}

/******************************************************************************
Description:
    Get interrupt and error status.

Parameters:
    void

Return Value:
    the current interrupt status, enumerated as a bit field of
        RBIT_UART_UFLSR_BI
        RBIT_UART_UFLSR_FE
        RBIT_UART_UFLSR_PE
        RBIT_UART_UFLSR_OE
        RBIT_UART_UFLSR_RFTO
        RBIT_UART_UFLSR_TFTRG
        RBIT_UART_UFLSR_TFEPT
        RBIT_UART_UFLSR_TREPT
        RBIT_UART_UFLSR_RFTRG
        RBIT_UART_UFLSR_DR
        RBIT_UART_UFLSR_RX
        RBIT_UART_UFLSR_TX
Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT32 VUART_GetFifoAndLineStatus ( void )
{
    return READ_REG ( REG_UART_UFLSR );
}

/******************************************************************************
Description:
    Clear error interrupt.

Parameters:
    ErrorFlag - interrupt flag waited to clear, this parameter must be enumerated as a bit field of
            RBIT_UART_UFLSR_CC
            RBIT_UART_UFLSR_BI
            RBIT_UART_UFLSR_FE
            RBIT_UART_UFLSR_PE
            RBIT_UART_UFLSR_OE
            RBIT_UART_UFLSR_RX
            RBIT_UART_UFLSR_TX
Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_ClearInt ( UINT32 intflag )
{
    /*VA9621_ChenJian_20091218: clear interrupt flag manually*/
    UINT32 reg = 0;

    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_CC ) ) ) {
        reg |= UART_INTCLR_CC;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_BI ) ) ) {
        reg |= UART_INTCLR_BI;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_FE ) ) ) {
        reg |= UART_INTCLR_FE;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_PE ) ) ) {
        reg |= UART_INTCLR_PE;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_OE ) ) ) {
        reg |= UART_INTCLR_OE;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_RX ) ) ) {
        reg |= UART_INTCLR_RX;
    }
    if ( intflag & ( REG_BIT_MASK ( RBIT_UART_UFLSR_TX ) ) ) {
        reg |= UART_INTCLR_TX;
    }
    WRITE_REG ( REG_UART_UINTCLR, reg );
}

/******************************************************************************
Description:
    Get the receiver FIFO trigger level for interrupt.

Parameters:
    void

Return Value:
    The receiver FIFO trigger level. This value must be from 1 to 31.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT8 VUART_GetRxFifoTriggerLevel ( void )
{
    return ( READ_REG ( REG_UART_UFCR ) & 0x1F000000 ) >> 24;
}

/******************************************************************************
Description:
    Set the receiver FIFO trigger level for interrupt.

Parameters:
    level - the new level to be set. This value must be from 1 to 31.

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_SetRxFifoTriggerLevel ( UINT8 Level )
{
    //ASSERT(Level > 0 && Level < 32);

    WRITE_REG ( REG_UART_UFCR, READ_REG ( REG_UART_UFCR ) & 0xE0FFFFFF | ( Level << 24 ) );
}

/******************************************************************************
Description:
    Get the transmitter FIFO trigger level for interrupt.

Parameters:
    void

Return Value:
    The transmitter FIFO trigger level. This value must be from 0 to 31.

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
UINT8 VUART_GetTxFifoTriggerLevel ( void )
{
    return ( READ_REG ( REG_UART_UFCR ) & 0x001F0000 ) >> 16;
}

/******************************************************************************
Description:
    Set the transmitter FIFO trigger level for interrupt.

Parameters:
    level - the new level to be set. This value must be from 0 to 31.

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_SetTxFifoTriggerLevel ( UINT8 Level )
{
    //ASSERT(Level >= 0 && Level < 32);

    WRITE_REG ( REG_UART_UFCR, READ_REG ( REG_UART_UFCR ) & 0xFFE0FFFF | ( Level << 16 ) );
}

/******************************************************************************
Description:
    Reset tx and/or rx fifo.

Parameters:
    Fifos -  UART_RX_FIFO, UART_TX_FIFO or UART_BOTH_FIFO

Return Value:
    void

Remarks:
    None.

Example:
    None.

See Also:
    None.
*****************************************************************************/
void VUART_ResetFifo ( UINT8 Fifos )
{
    if ( Fifos & UART_RX_FIFO ) {
        WRITE_REG ( REG_UART_UFRST, BIT_MASK ( RBIT_UART_UFRST_RFRST ) );
    }
    if ( Fifos & UART_TX_FIFO ) {
        WRITE_REG ( REG_UART_UFRST, BIT_MASK ( RBIT_UART_UFRST_TFRST ) );
    }
}

/******************************************************************************
Description:
    Receive data from receiver fifo if there is valid data and no error occurs.

Parameters:
    p_ch: pointer of the read character

Return Value:
    VRET_VALUE

Remarks:
    None.

Example:
    None.

See Also:
    vUART_CharRxWithBlock()
*****************************************************************************/
VRET_VALUE VUART_CharRx ( UINT8 *p_Ch )
{
    //ASSERT(p_Ch != NULL);
    if (( READ_REG ( REG_UART_UFCT ) & 0x3F ) > 0 ) {
        *p_Ch = ( UINT8 ) ( READ_REG ( REG_UART_URDR ) & 0xFF );
        return VRET_NO_ERROR;
    } else {
        return VRET_DRV_UART_ERR_RXFIFOEMPTY;
    }
}

/******************************************************************************
Description:
    Wait a data from receiver fifo. If there is no error occurs, return the right data.

Parameters:
    p_ch: pointer of the read character

Return Value:
    VRET_VALUE

Remarks:
    None.

Example:
    None.

See Also:
    vUART_CharRx()
*****************************************************************************/
VRET_VALUE VUART_CharRxWithBlocking ( UINT8 *p_Ch )
{
    //ASSERT(p_Ch != NULL);

    // ??RX FIFO??????,???0,????????????,???while?????????
    // ?????RX FIFO????????PC???????????,??????????????RX FIFO?
    while ( ( READ_REG ( REG_UART_UFCT ) & 0x3F ) == 0 )
        ;

    // RX FIFO???????0,???UART???????,????,?UART FIFO??????????
    *p_Ch = ( UINT8 ) ( READ_REG ( REG_UART_URDR ) & 0xFF );

    return VRET_NO_ERROR;
}
/******************************************************************************
Description:
    Put data into transmitter fifo if there is space.
    ????,??????PC???,??TX FIFO?,??TX FIFO????,???????
Parameters:
    ch - char to transmit

Return Value:
    VRET_VALUE

Remarks:
    None.

Example:
    None.

See Also:
    vUART_CharTxWithBlocking().
*****************************************************************************/
VRET_VALUE VUART_CharTx ( UINT8 Ch )
{
    /* UART_TXFIFO_DEPTH - 1 为了规避TX FIFO LEN读取滞后问题 */
    if ( ( ( READ_REG ( REG_UART_UFCT ) & 0x003F0000 ) >> 16 ) < UART_TXFIFO_DEPTH - 1) {
        WRITE_REG ( REG_UART_UTDR, Ch );
        return VRET_NO_ERROR;
    } else {
        return VRET_DRV_UART_ERR_TXFIFOFULL;
    }
}

/******************************************************************************
Description:
    Wait to put data into transmitter fifo.
    ????,?????????FIFO?,?FIFO???????32????,????,?
    ???,???????32?,???????,??FIFO?
Parameters:
    ch - char to transmit

Return Value:
    VRET_VALUE

Remarks:
    None.

Example:
    None.

See Also:
    vUART_CharTx().
*****************************************************************************/
VRET_VALUE VUART_CharTxWithBlocking ( UINT8 Ch )
{
    /* UART_TXFIFO_DEPTH - 1 为了规避TX FIFO LEN读取滞后问题 */
	while (((READ_REG(REG_UART_UFCT) & 0x003F0000) >> 16) >= UART_TXFIFO_DEPTH - 1)
        ;
    WRITE_REG ( REG_UART_UTDR, Ch );
    return VRET_NO_ERROR;
}

/******************************************************************************
Description:
    Wait to put data into transmitter fifo.

Parameters:
    str - string to transmit

Return Value:
    VRET_VALUE

Remarks:
    None.

Example:
    None.

See Also:
    VUART_CharTxWithBlocking().
*****************************************************************************/
VRET_VALUE VUART_StrTxWithBlocking ( UINT8 *str, UINT16 len )
{
    int i;
    for ( i = 0; i < len; i++ ) {
        VUART_CharTxWithBlocking(str[i]);
    }
    return VRET_NO_ERROR;
}


/******************************************************************************
Description:
    Get rx fifo Count.

Parameters:
    void

Return Value:
    Rx Fifo count

Remarks:
    None.

Example:
    None.
*****************************************************************************/
UINT8 VUART_GetRxCounter ( void )
{
    return READ_REG ( REG_UART_UFCT ) & 0x3F;
}

/******************************************************************************
Description:
    Get tx fifo Count.

Parameters:
    void

Return Value:
    Tx Fifo count

Remarks:
    None.

Example:
    None.
*****************************************************************************/
UINT8 VUART_GetTxCounter ( void )
{
	return (UINT8)((READ_REG(REG_UART_UFCT) & 0x003F0000) >> 16);
}

UINT8 VUART_GetCTSStatus ( void )
{
    UINT8 cts;
    cts = ( READ_REG ( REG_UART_URCTS ) & BIT_MASK ( RBIT_UART_URCTS_CTS ) ) ? UART_FLOW_STOP : UART_FLOW_GO;
    return cts;
}

void VUART_SetRTSStatus ( UINT8 Status )
{
    if ( Status == UART_FLOW_GO ) {
        CLR_REG_BIT2 ( REG_UART_URCTS, RBIT_UART_URCTS_RTS );
    } else {
        SET_REG_BIT2 ( REG_UART_URCTS, RBIT_UART_URCTS_RTS );
    }
}

UINT8 VUART_GetRTSStatus ( void )
{
    UINT8 rts;
    rts = ( READ_REG ( REG_UART_URCTS ) & BIT_MASK ( RBIT_UART_URCTS_RTS ) ) ? UART_FLOW_STOP : UART_FLOW_GO;
    return rts;
}

int tstc ( void )
{
    if ( ( READ_REG ( REG_UART_UFCT ) & 0x3F ) == 0 ) {
        return 0;
    } else {
        return 1;
    }
}



