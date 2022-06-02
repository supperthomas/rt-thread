/*
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-02     supperthomas          first version
 */
#include <rtdevice.h>
#include <rthw.h>

#include <stdio.h>
#include "drv_uart.h"
#include "driver/uart.h"
#include "hal/uart_hal.h"

/* mcu config class */
struct mcu_uart_config
{
    const char *name;
  //  USART_TypeDef *Instance;
  //  IRQn_Type irq_type;

};


struct mcu_uart
{
   // UART_HandleTypeDef handle;
    void * handle;
    struct mcu_uart_config *config;
    struct rt_serial_device serial;
};

#ifdef RT_USING_SERIAL

#if !defined(BSP_USING_UART1) && !defined(BSP_USING_UART2) && !defined(BSP_USING_UART3) && \
    !defined(BSP_USING_UART4) && !defined(BSP_USING_UART5) && !defined(BSP_USING_UART6) && \
    !defined(BSP_USING_UART7) && !defined(BSP_USING_UART8) && !defined(BSP_USING_UART9) && \
    !defined(BSP_USING_UART10)
#error "Please define at least one BSP_USING_UARTx"
/* UART instance can be selected at menuconfig -> Hardware Drivers Config -> On-chip Peripheral Drivers -> Enable UART */
#endif

static struct mcu_uart_config uart_config[] =
{
#ifdef BSP_USING_UART1
    UART1_CONFIG,
#endif
};


static rt_err_t mcu_configure(struct rt_serial_device *serial,
                                struct serial_configure *cfg)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return RT_EOK;
}

static rt_err_t mcu_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    return RT_EOK;
}
static uart_hal_context_t s_send_uart = { .dev = &UART1 };

static int mcu_putc(struct rt_serial_device *serial, char c)
{
    int write_len = 0;
    char abc = c;
    uart_hal_write_txfifo(&s_send_uart, (const uint8_t *)&abc,1,&write_len);
    printf("\r\n=====%s===%d==%c== \r\n ",__func__,__LINE__,c);
    return 1;
}

static int mcu_getc(struct rt_serial_device *serial)
{
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    int ch= -1;
    return ch;
}



static const struct rt_uart_ops mcu_uart_ops =
{
    .configure = mcu_configure,
    .control = mcu_control,
    .putc = mcu_putc,
    .getc = mcu_getc,
    .dma_transmit = RT_NULL
};
static struct mcu_uart uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

int mcu_hw_uart_init(void)
{

#define CONFIG_EXAMPLE_UART_PORT_NUM 1
#define CONFIG_EXAMPLE_UART_BAUD_RATE 115200
#define CONFIG_EXAMPLE_UART_RXD 5
#define CONFIG_EXAMPLE_UART_TXD 4

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

#define BUF_SIZE (1024)

    uart_config_t uart_config0 = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config0));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    rt_size_t obj_num = sizeof(uart_obj) / sizeof(struct mcu_uart);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    rt_err_t result = 0;
    printf("\r\n=====%s===%d==== \r\n ",__func__,__LINE__);
    
    for (int i = 0; i < obj_num; i++)
    {
        /* init UART object */
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops    = &mcu_uart_ops;
        uart_obj[i].serial.config = config;

        /* register UART device */
        result = rt_hw_serial_register(&uart_obj[i].serial, uart_obj[i].config->name,
                                       RT_DEVICE_FLAG_RDWR
                                       | RT_DEVICE_FLAG_INT_RX
                                       | RT_DEVICE_FLAG_INT_TX
                                      // | uart_obj[i].uart_dma_flag
                                       , NULL);
        RT_ASSERT(result == RT_EOK);
    }
    return result;
}

INIT_BOARD_EXPORT(mcu_hw_uart_init);

#endif /* RT_USING_SERIAL */
