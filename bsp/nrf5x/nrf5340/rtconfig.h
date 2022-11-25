#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* Hardware Drivers Config */

#define SOC_NRF5340
#define SOC_NORDIC
#define BSP_BOARD_PCA_10095

/* Onboard Peripheral Drivers */

#define RT_BSP_LED_PIN 28
#define BSP_USING_QSPI_FLASH
#define NRFX_QSPI_ENABLED 1
#define BSP_QSPI_SCK_PIN 17
#define BSP_QSPI_CSN_PIN 18
#define BSP_QSPI_IO0_PIN 13
#define BSP_QSPI_IO1_PIN 14
#define BSP_QSPI_IO2_PIN 15
#define BSP_QSPI_IO3_PIN 16
#define QSPI_FLASH_SIZE_KB 8192

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define NRFX_USING_UARTE
#define BSP_USING_UART0
#define BSP_UART0_RX_PIN 22
#define BSP_UART0_TX_PIN 20
#define NRFX_UARTE_ENABLED 1
#define NRFX_UARTE1_ENABLED 1
#define BSP_USING_UART1
#define NRFX_UARTE0_ENABLED 1
#define BSP_UART1_RX_PIN 32
#define BSP_UART1_TX_PIN 33
#define BSP_USING_UART3
#define NRFX_UARTE3_ENABLED 1
#define BSP_UART3_RX_PIN 7
#define BSP_UART3_TX_PIN 5
#define BSP_USING_SAADC
#define BSP_USING_IPC
#define BSP_USING_PWM
#define BSP_USING_PWM0
#define BSP_USING_PWM0_CH0 28
#define BSP_USING_PWM0_CH1 29
#define BSP_USING_PWM0_CH2 30
#define BSP_USING_PWM0_CH3 31
#define BSP_USING_WDT
#define BSP_USING_USB_CDC_ACM
#define BSP_USING_ONCHIP_RTC

/* MCU flash config */

#define MCU_FLASH_START_ADDRESS 0x00000000
#define MCU_FLASH_SIZE_KB 1024
#define MCU_SRAM_START_ADDRESS 0x20000000
#define MCU_SRAM_SIZE_KB 512
#define MCU_FLASH_PAGE_SIZE 0x1000
#define BSP_USING_TIM
#define NRFX_TIMER_ENABLED 1
#define BSP_USING_TIM1
#define NRFX_TIMER1_ENABLED 1
#define BSP_USING_TIM2
#define NRFX_TIMER2_ENABLED 1
#define BLE_STACK_USING_NULL
#define NRFX_CLOCK_ENABLED 1
#define NRFX_CLOCK_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_CLOCK_CONFIG_LF_SRC 1
#define NRFX_UART_ENABLED 1
#define NRFX_UART0_ENABLED 1
#define NRFX_GPIOTE_ENABLED 1
#define NRFX_SAADC_ENABLED 1
#define NRFX_IPC_ENABLED 1
#define NRFX_PWM_ENABLED 1
#define NRFX_PWM0_ENABLED 1
#define NRFX_WDT_ENABLED 1
#define NRFX_WDT0_ENABLED 1
#define NRFX_WDT_CONFIG_NO_IRQ 1
#define NRFX_USBREG_ENABLED 1
#define NRFX_USBREG_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_USBD_ENABLED 1
#define NRFX_USBD_CONFIG_IRQ_PRIORITY 6
#define NRFX_USBD_CONFIG_DMASCHEDULER_MODE 0
#define NRFX_USBD_CONFIG_DMASCHEDULER_ISO_BOOST 1
#define NRFX_USBD_CONFIG_ISO_IN_ZLP 0
#define NRFX_USBD_USE_WORKAROUND_FOR_ANOMALY_211 0
#define APP_USBD_ENABLED 1
#define APP_USBD_VID 0x1915
#define APP_USBD_PID 0x520F
#define APP_USBD_DEVICE_VER_MAJOR 1
#define APP_USBD_DEVICE_VER_MINOR 0
#define APP_USBD_DEVICE_VER_SUB 0
#define APP_USBD_CONFIG_SELF_POWERED 1
#define APP_USBD_CONFIG_MAX_POWER 100
#define APP_USBD_CONFIG_POWER_EVENTS_PROCESS 0
#define APP_USBD_CONFIG_EVENT_QUEUE_ENABLE 1
#define APP_USBD_CONFIG_EVENT_QUEUE_SIZE 32
#define APP_USBD_CONFIG_SOF_HANDLING_MODE 1
#define APP_USBD_CONFIG_SOF_TIMESTAMP_PROVIDE 32
#define APP_USBD_CONFIG_DESC_STRING_SIZE 31
#define APP_USBD_CONFIG_DESC_STRING_UTF_ENABLED 0
#define APP_USBD_STRING_ID_MANUFACTURER 1
#define APP_USBD_STRINGS_MANUFACTURER_EXTERN 0
#define APP_USBD_STRING_ID_PRODUCT 2
#define APP_USBD_STRINGS_PRODUCT_EXTERN 0
#define APP_USBD_STRING_ID_SERIAL 3
#define APP_USBD_STRING_SERIAL_EXTERN 1
#define APP_USBD_STRING_ID_CONFIGURATION 4
#define APP_USBD_STRING_CONFIGURATION_EXTERN 0
#define APP_USBD_CDC_ACM_ENABLED 1
#define APP_USBD_CDC_ACM_ZLP_ON_EPSIZE_WRITE 1
#define CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT 1
#define NRFX_RTC_ENABLED 1
#define NRFX_RTC1_ENABLED 1
#define NRFX_RTC2_ENABLED 0
#define RTC_INSTANCE_ID 1
#define NRFX_DPPI_ENABLED 1
#define NRFX_CLOCK_CONFIG_IRQ_PRIORITY 6
#define NRFX_RTC_MAXIMUM_LATENCY_US 2000
#define NRFX_RTC_DEFAULT_CONFIG_FREQUENCY 32768
#define NRFX_RTC_DEFAULT_CONFIG_RELIABLE 0
#define NRFX_TIMER0_ENABLED 1
#define NRFX_TIMER_DEFAULT_CONFIG_FREQUENCY 0
#define NRFX_TIMER_DEFAULT_CONFIG_MODE 0
#define NRFX_TIMER_DEFAULT_CONFIG_BIT_WIDTH 0
#define NRFX_RTC0_ENABLED 1
#define NRF_BALLOC_ENABLED 1
#define NRF_LIBUARTE_ASYNC_WITH_APP_TIMER 0
#define NRF_QUEUE_ENABLED 1
#define NRF_LIBUARTE_DRV_HWFC_ENABLED 0
#define NRF_LIBUARTE_DRV_UARTE0 1
#define NRF_LIBUARTE_DRV_UARTE1 0
#define NRF_LOG_ENABLED 0

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 100
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 512
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512

/* kservice optimization */


/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x40101

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_HWTIMER
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SPI
#define RT_USING_WDT

/* Using USB */

#define RT_USING_USB

/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */


/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */


/* Network */


/* Utilities */


/* RT-Thread Utestcases */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */


/* u8g2: a monochrome graphic library */


/* PainterEngine: A cross-platform graphics application framework written in C language */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* POSIX extension functions */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_FAL
#define PKG_USING_FAL_V10000
#define PKG_FAL_VER_NUM 0x10000

/* peripheral libraries and drivers */

#define PKG_USING_NRFX
#define PKG_USING_NRFX_LATEST_VERSION

/* AI packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */


#endif
