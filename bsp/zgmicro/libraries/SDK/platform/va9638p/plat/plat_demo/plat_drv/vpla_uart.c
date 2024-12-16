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
        uart

Description:
        uart Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_uart.h"
#include "vdrv_uart.h"
#include "vdrv_gpio.h"
#include "vdrv_ckm.h"

static UINT32 uart_UFLSR = 0;
static VDRVUartConfig_t uart_config_tmp = {115200, VSYS_UART_PORT_SEL_AUX, VSYS_UART_PARITY_NONE, VSYS_UART_STOP_BIT1, VSYS_UART_CHAR_BIT8, VSYS_UART_FIFO_MODE, FALSE, FALSE, 0, 0};

VRET_VALUE VSYS_UART_Init(VDRVUartConfig_t *uart_config)
{
    if (uart_config == NULL)
    {
        uart_config = &uart_config_tmp;
    }
    else
    {
        vmemcpy(&uart_config_tmp, uart_config, sizeof(uart_config_tmp));
    }
    VUART_Disable();
    VUART_ResetFifo(UART_BOTH_FIFO);
    if (uart_config->port_sel == VSYS_UART_PORT_SEL_AUX)
    {
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_UART_TRX_AUX, GPIO_MUX_FUNC_UART_TRX_AUX);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_UART_TRX_AUX, GPIO_MUX_FUNC_NONE);
    }
    else if (uart_config->port_sel == VSYS_UART_PORT_SEL_PRIMARY)
    {
        VGPIO_SetMuxFunction(GPIO_MUX_FUNC_UART_TRX, GPIO_MUX_FUNC_UART_TRX);
        VGPIO_SetMux2Function(GPIO_MUX_FUNC_UART_TRX, GPIO_MUX_FUNC_NONE);
        if (uart_config->Rtsen)
        {
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_UART_RTS, GPIO_MUX_FUNC_UART_RTS);
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_UART_RTS, GPIO_MUX_FUNC_NONE);
        }
        if (uart_config->Ctsen)
        {
            VGPIO_SetMuxFunction(GPIO_MUX_FUNC_UART_CTS, GPIO_MUX_FUNC_UART_CTS);
            VGPIO_SetMux2Function(GPIO_MUX_FUNC_UART_CTS, GPIO_MUX_FUNC_NONE);
        }
    }
    VUART_SetBaudRate(uart_config->Baudrate, VCKM_GetUartClockFreq());
    VUART_Config(uart_config->Parity, uart_config->StopBit, uart_config->BitWidth);
    VUART_FlowControl(uart_config->Rtsen, uart_config->Ctsen);
    VUART_SelectPort(uart_config->port_sel);
    VUART_SetMask(UART_ERR_ALL_MASK);
    VUART_Enable();
    VSYS_UART_ClearInt();
    if (uart_config->mode == VSYS_UART_INT_MODE)
    {
        if (uart_config->TxFifolevel != 0)
        {
            VUART_SetTxFifoTriggerLevel(uart_config->TxFifolevel);
            VUART_Unmask(UART_ERR_TX_MASK);
        }
        if (uart_config->RxFifolevel != 0)
        {
            VUART_SetRxFifoTriggerLevel(uart_config->RxFifolevel);
            VUART_Unmask(UART_ERR_TO_MASK | UART_ERR_RX_MASK | UART_ERR_OE_MASK | UART_ERR_PE_MASK | UART_ERR_FE_MASK | UART_ERR_BI_MASK);
        }
        if (uart_config->Ctsen != FALSE)
        {
            VUART_Unmask(UART_ERR_CC_MASK);
        }
        VUART_EnableInt();
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_UART_Uninit(void)
{
    VUART_Disable();
    VUART_DisableInt();
    VUART_SetMask(UART_ERR_ALL_MASK);
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_UART_EnableInt(VDRVUartInt_t Int, BOOL en)
{
    UINT32 flag = 0;
    if (Int & VSYS_UART_RX_INT)
    {
        flag |= UART_ERR_TO_MASK | UART_ERR_RX_MASK;
    }
    if (Int & VSYS_UART_TX_INT)
    {
        flag |= UART_ERR_TX_MASK;
    }
    if (Int & VSYS_UART_ERR_INT)
    {
        flag |= UART_ERR_OE_MASK | UART_ERR_PE_MASK | UART_ERR_FE_MASK | UART_ERR_BI_MASK | UART_ERR_CC_MASK;
    }
    if (en)
    {
        VUART_Unmask(flag);
        VUART_EnableInt();
    }
    else
    {
        flag = VUART_SetMask(flag);
        if (flag == UART_ERR_ALL_MASK)
        {
            VUART_DisableInt();
        }
    }
    return VRET_NO_ERROR;
}

VRET_VALUE VSYS_UART_ClearInt(void)
{
    if (uart_UFLSR == 0)
    {
        uart_UFLSR = VUART_GetFifoAndLineStatus();
    }
    VUART_ClearInt(uart_UFLSR);
    return VRET_NO_ERROR;
}

size_t VSYS_UART_CharTx(const UINT8 *const Chs, size_t size)
{
    VRET_VALUE ret = 0;
    size_t offset = 0;
    for (offset = 0; offset < size; offset++)
    {
        ret = VUART_CharTx(Chs[offset]);
        if (ret != VRET_NO_ERROR)
        {
            break;
        }
    }
    return offset;
}

size_t VSYS_UART_CharRx(UINT8 *const Chs, size_t size)
{
    VRET_VALUE ret = 0;
    size_t offset = 0;
    for (offset = 0; offset < size; offset++)
    {
        ret = VUART_CharRx(&Chs[offset]);
        if (ret != VRET_NO_ERROR)
        {
            break;
        }
    }
    return offset;
}

UINT32 VSYS_UART_GetStatus(VDRVUartStatus_t status)
{
    UINT32 ret = 0;
    switch (status)
    {
    case VSYS_UART_FreeTxCount_Status:
        ret = VUART_GetTxCounter();
        /* UART_TXFIFO_DEPTH - 1 为了规避TX FIFO LEN读取滞后问题 */
        if (ret > UART_TXFIFO_DEPTH - 1)
        {
            ret = 0;
        }
        else {
            ret = UART_TXFIFO_DEPTH - 1 - ret;
        }
        break;
    case VSYS_UART_ValidRxCount_Status:
        ret = VUART_GetRxCounter();
        break;
    case VSYS_UART_Interrupt_Status:
    {
        uart_UFLSR = VUART_GetFifoAndLineStatus();
        if (uart_UFLSR & (BIT_MASK(RBIT_UART_UFLSR_RFTO) | BIT_MASK(RBIT_UART_UFLSR_RX) | BIT_MASK(RBIT_UART_UFLSR_RFTRG)))
        {
            ret |= VSYS_UART_RX_INT;
        }
        if (uart_UFLSR & (BIT_MASK(RBIT_UART_UFLSR_TFTRG) | BIT_MASK(RBIT_UART_UFLSR_TX) | BIT_MASK(RBIT_UART_UFLSR_TFEPT)))
        {
            ret |= VSYS_UART_TX_INT;
        }
        if (uart_UFLSR & (BIT_MASK(RBIT_UART_UFLSR_CC)))
        {
            ret |= VSYS_UART_CTS_INT;
        }
        if (uart_UFLSR & (BIT_MASK(RBIT_UART_UFLSR_BI) | BIT_MASK(RBIT_UART_UFLSR_FE) | BIT_MASK(RBIT_UART_UFLSR_PE) |
                          BIT_MASK(RBIT_UART_UFLSR_OE)))
        {
            ret |= VSYS_UART_ERR_INT;
        }
    }
    break;
    case VSYS_UART_CTS_Status:
        ret = VUART_GetCTSStatus();
        break;
    case VSYS_UART_RecTra_Status:
        if (VUART_GetFifoAndLineStatus() & (BIT_MASK(RBIT_UART_UFLSR_DR) | BIT_MASK(RBIT_UART_UFLSR_TREPT)))
        {
            ret = TRUE;
        }
        break;
    default:
        break;
    }
    return ret;
}

VRET_VALUE VSYS_UART_Control(VDRVUartControl_t control)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    switch (control)
    {
    case VSYS_UART_ResetTxFifo:
        VUART_ResetFifo(UART_TX_FIFO);
        break;
    case VSYS_UART_ResetRxFifo:
        VUART_ResetFifo(UART_RX_FIFO);
        break;
    case VSYS_UART_FLOW_GO:
        VUART_SetRTSStatus(UART_FLOW_GO);
        break;
    case VSYS_UART_FLOW_STOP:
        VUART_SetRTSStatus(UART_FLOW_STOP);
        break;
    default:
        break;
    }
    return ret;
}
