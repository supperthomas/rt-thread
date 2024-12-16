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
    2009.04.08            chenjian                 initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VUART_H_
#define _VUART_H_

#include "vstdlib.h"

/* UART reserved error code from 0x401 ~ 0x40f */
#define VRET_DRV_UART_ERROR								0x401
#define VRET_DRV_UART_ERR_ERROCCUR						0x402
#define VRET_DRV_UART_ERR_RXFIFOEMPTY 					0x403
#define VRET_DRV_UART_ERR_TXFIFOFULL					0x404


#define _VPORT_UART_CBR_115200  (115200)
#define _VPORT_UART_CBR_512000  (512000)
#define _VPORT_UART_CBR_2000000  (2000000)
#define _VPORT_UART_CBR_3000000  (3000000)


#define VUART_PORT_REMAP_EN         (1)     
#define VUART_PORT_SEL_PRIMARY      (0)     // 0 = full UART port (for communication under function mode).(@GPIO[4:1])
#define VUART_PORT_SEL_AUX          (1)     // 1 = auxiliary port (for test and production, no CTS/RTS).(@GPIO[14:13])   

/******************************************************************************

                UART Recevier Data Register bits

*******************************************************************************/
#define RBIT_UART_URDR_BI           (12)  // Break Interrupt
#define RBIT_UART_URDR_FE           (11)  // Framing Error
#define RBIT_UART_URDR_PE           (10)  // Parity Error
#define RBIT_UART_URDR_OE           (9 )  // Overrun Error
#define RBIT_UART_URDR_DV           (8 )

/******************************************************************************

                UART Configuration Register bits

*******************************************************************************/
#define RBIT_UART_UCR_RPOL          (20)  // Receive Polarity
#define RBIT_UART_UCR_TPOL          (19)  // Transmit Polarity
#define RBIT_UART_UCR_LB            (18)  // Loopback
#define RBIT_UART_UCR_RTSEN         (17)  // Receiver hardware flow control
#define RBIT_UART_UCR_CTSEN         (16)  // Transmitter hardware flow control
#define RBIT_UART_UCR_SP            (4 )  // Stick Parity
#define RBIT_UART_UCR_EPS           (3 )  // Even Parity Select
#define RBIT_UART_UCR_PEN           (2 )  // Parity Enable
#define RBIT_UART_UCR_STP2          (1 )  // Enable 2 Stop
#define RBIT_UART_UCR_BIT8          (0 )  // Enable 8-bit mode

/******************************************************************************

                UART FIFO Reset Register bits

*******************************************************************************/
#define RBIT_UART_UFRST_TFRST       (1)  // Transmitter FIFO Reset
#define RBIT_UART_UFRST_RFRST       (0)  // Receiver FIFO Reset

/******************************************************************************

                UART RTS and CTS Data Register bits

*******************************************************************************/
#define RBIT_UART_URCTS_MODE        (2)
#define RBIT_UART_URCTS_RTS         (1)
#define RBIT_UART_URCTS_CTS         (0)

/******************************************************************************

                UART Testing Control Register bits

*******************************************************************************/
#define RBIT_UART_UTCR_BRK          (2)  // Break Control
#define RBIT_UART_UTCR_FFE          (1)  // Force Frame Error
#define RBIT_UART_UTCR_FPE          (0)  // Force Parity Error

/******************************************************************************

                UART Enable Register bits

*******************************************************************************/
#define RBIT_UART_UER_UPSEL         (3)  // UART pin select
#define RBIT_UART_UER_UEN           (2)  // UART Enable
#define RBIT_UART_UER_TEN           (1)  // Transmit Enable
#define RBIT_UART_UER_REN           (0)  // Receive Enable

/******************************************************************************

                UART FIFO and Line Status Register bits

*******************************************************************************/
#define RBIT_UART_UFLSR_ERR         (23) // uart error Interrupt indicator
#define RBIT_UART_UFLSR_TX          (22) // txfifo Interrupt indicator
#define RBIT_UART_UFLSR_RX          (21) // rxfifo Interrupt indicator
#define RBIT_UART_UFLSR_CC          (20) // uart_cts_b changed
#define RBIT_UART_UFLSR_BI          (19) // Break Interrupt indicator
#define RBIT_UART_UFLSR_FE          (18) // Framing Error indicator
#define RBIT_UART_UFLSR_PE          (17) // Parity Error indicator
#define RBIT_UART_UFLSR_OE          (16) // Overrun Error indicator
#define RBIT_UART_UFLSR_RFTO        (5 ) // Receiver FIFO Timeout
#define RBIT_UART_UFLSR_TFTRG       (4 ) // Transmit FIFO Reach Trigger Level
#define RBIT_UART_UFLSR_TFEPT       (3 ) // Transmit FIFO is empty
#define RBIT_UART_UFLSR_TREPT       (2 ) // Transmit Empty indicator
#define RBIT_UART_UFLSR_RFTRG       (1 ) // Receiver FIFO Reach Trigger Level
#define RBIT_UART_UFLSR_DR          (0 ) // Data Ready indicator

/******************************************************************************

                UART FIFO Depth

*******************************************************************************/
#define UART_RXFIFO_DEPTH           (32)
#define UART_TXFIFO_DEPTH           (32) 

/******************************************************************************

                UART Error Clear Bits

*******************************************************************************/
#define UART_INTCLR_OE              BIT_MASK(1)
#define UART_INTCLR_PE              BIT_MASK(2)
#define UART_INTCLR_FE              BIT_MASK(3)
#define UART_INTCLR_BI              BIT_MASK(4)
#define UART_INTCLR_CC              BIT_MASK(5)
#define UART_INTCLR_TX              BIT_MASK(6)
#define UART_INTCLR_RX              BIT_MASK(7)

/******************************************************************************

                UART Parameters

*******************************************************************************/
#define UART_CONF_PARITY_NONE       (0)
#define UART_CONF_PARITY_ODD        (1)  // odd number of '1'
#define UART_CONF_PARITY_EVEN       (2)  // even number of '1'
#define UART_CONF_STOP_BIT1         (0)  // 1 stop-bit
#define UART_CONF_STOP_BIT2         (1)  // 2 stop-bit
#define UART_CONF_CHAR_BIT7         (0)  // 7 bit per char
#define UART_CONF_CHAR_BIT8         (1)  // 8 bit per char

#define UART_FLOW_DISABLE           (0)  //disable rx/tx flow control
#define UART_FLOW_ENABLE            (1)  //enable rx/tx flow control

#define UART_RX_FIFO                (1)
#define UART_TX_FIFO                (2)
#define UART_BOTH_FIFO              (3)

#define UART_ERR_TO_MASK            BIT_MASK(0)
#define UART_ERR_OE_MASK            BIT_MASK(1)
#define UART_ERR_PE_MASK            BIT_MASK(2)
#define UART_ERR_FE_MASK            BIT_MASK(3)
#define UART_ERR_BI_MASK            BIT_MASK(4)
#define UART_ERR_CC_MASK            BIT_MASK(5)
#define UART_ERR_TX_MASK            BIT_MASK(6)
#define UART_ERR_RX_MASK            BIT_MASK(7)
#define UART_ERR_ALL_MASK				0xFF

#define UART_FLOW_GO                (0)
#define UART_FLOW_STOP              (1)



/******************************************************************************

                UART Function Prototypes

*******************************************************************************/
void VUART_SelectPort ( UINT8 port_sel );
void VUART_Enable ( void );
void VUART_Disable ( void );
void VUART_SetBaudRate ( UINT32 baudrate, UINT32 uartclk );
void VUART_Config ( UINT8 parity, UINT8 stopbit, UINT8 bit8 );
void VUART_FlowControl ( UINT8 rtsen, UINT8 ctsen );
void VUART_EnableInt ( void );
void VUART_DisableInt ( void );
UINT32 VUART_SetMask ( UINT32 flag );
void VUART_Unmask ( UINT32 flag );
UINT32 VUART_GetFifoAndLineStatus ( void );
void VUART_ClearInt ( UINT32 intflag );
UINT8 VUART_GetRxFifoTriggerLevel ( void );
void VUART_SetRxFifoTriggerLevel ( UINT8 level );
UINT8 VUART_GetTxFifoTriggerLevel ( void );
void VUART_SetTxFifoTriggerLevel ( UINT8 level );
void VUART_ResetFifo ( UINT8 Fifos );
VRET_VALUE VUART_CharRx ( UINT8 *p_ch );
VRET_VALUE VUART_CharRxWithBlocking ( UINT8 *p_ch );
VRET_VALUE VUART_CharTx ( UINT8 ch );
VRET_VALUE VUART_CharTxWithBlocking ( UINT8 ch );
VRET_VALUE VUART_StrTxWithBlocking ( UINT8 *str, UINT16 len );
UINT8 VUART_GetRxCounter ( void );
UINT8 VUART_GetTxCounter ( void );

UINT8 VUART_GetCTSStatus ( void );
void VUART_SetRTSStatus ( UINT8 Status );
UINT8 VUART_GetRTSStatus ( void );

void VUART2_Enable ( void );
void VUART2_Disable ( void );
void VUART2_SetBaudRate ( UINT32 baudrate, UINT32 uartclk );
void VUART2_Config ( UINT8 parity, UINT8 stopbit, UINT8 bit8 );
void VUART2_FlowControl ( UINT8 rtsen, UINT8 ctsen );
void VUART2_EnableInt ( UINT32 intnum );
void VUART2_DisableInt ( UINT32 intnum );
void VUART2_MaskError ( UINT32 Errors );
void VUART2_UnmaskError ( UINT32 Errors );
UINT32 VUART2_GetFifoAndLineStatus ( void );
void VUART2_ClearErrorInt ( UINT32 intflag );
UINT8 VUART2_GetRxFifoTriggerLevel ( void );
void VUART2_SetRxFifoTriggerLevel ( UINT8 level );
UINT8 VUART2_GetTxFifoTriggerLevel ( void );
void VUART2_SetTxFifoTriggerLevel ( UINT8 level );
void VUART2_ResetFifo ( UINT8 Fifos );
VRET_VALUE VUART2_CharRx ( UINT8 *p_ch );
VRET_VALUE VUART2_CharRxWithBlocking ( UINT8 *p_ch );
VRET_VALUE VUART2_CharTx ( UINT8 ch );
VRET_VALUE VUART2_CharTxWithBlocking ( UINT8 ch );
UINT8 VUART2_GetRxCounter ( void );
UINT8 VUART2_GetTxCounter ( void );
int tstc ( void );



#endif

