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
    DEBUG

Description:
    Debug relative function definitions.

Revision History:
    Date                  Author                   Description
    2009.04.15           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDEBUG_H_
#define _VDEBUG_H_

#ifdef __cplusplus 
extern "C" {
#endif

#if (defined(CONFIG_DEBUG_TYPE) && CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE)
#include "log.h"
#else
#define LOG_PANIC() /* Empty */
#define LOG_PRINTK()
#endif 

/* enable/disable global debug */
#ifdef RELEASE_BUILD
#define VDBG_EN                 0   /* 0 - disable, 1 - enable */
#else
#define VDBG_EN                 1   /* 0 - disable, 1 - enable */
#endif

/* 0 - disable, 1 - enable */
#define RTT_HIGH_SPEED_SYNC_DBG_EN          (VDBG_EN && 0)
/* add zip message string before debug sring,include hashcode of filename\line\timestamp */
#define ZIPMSG_DBG_EN                       (VDBG_EN && 0)
#define ZIPMSG_TS_DBG_EN                    (ZIPMSG_DBG_EN && 0) /* timestamp value enable */
#define ZIPMSG_SHORTTS_DBG_EN               (ZIPMSG_TS_DBG_EN && 0) /* timestamp value enable */
#define ZIPMSG_CHKSUM_DBG_EN                (ZIPMSG_DBG_EN && 0)

/* enable real-mode, which offload OS and try to provide a Human-Friendly UI */
#define REAL_MODE_DBG_EN                    (VDBG_EN && 0)

/* enable/disable assert */
#ifndef VDBG_ASSERT_EN
#define VDBG_ASSERT_EN                      (VDBG_EN && 1) /* 0 - disable, 1 - enable */
#endif

/* enable/disable periodic print fault massage */
#define PERIODIC_PFM_EN                     (VDBG_EN && 1)
#define PERIODIC_PFM_SECONDS                (30)

/* enable/disable controller stack debug */
#define CONTROLLER_DBG_EN                   (VDBG_EN && 0)

/* enable/disable host stack debug */
#define HOST_DBG_EN                         (VDBG_EN && 0)

/* enable/disable individual module debug */
#define VCOMMON_DBG_EN                      (VDBG_EN && 0)
#define VSYS_SOC_DBG_EN                     (VDBG_EN && 1)
#define VSYS_PWR_DBG_EN                     (VDBG_EN && 0)
#define VSYS_MEM_DBG_EN                     (VDBG_EN && 1)
#define VSYS_CFG_DBG_EN                     (VDBG_EN && 0)
#define VSYS_TMR_DBG_EN                     (VDBG_EN && 0)
#define VMMI_DBG_EN                         (VDBG_EN && 0)
#define VAPP_INIT_DBG_EN                    (VDBG_EN && 0)
#define VSYS_ECK_DBG_EN                     (VDBG_EN && 0)
#define VAPP_COBRA_DBG_EN                   (VDBG_EN && 0)
#define VTOP_HLP_DBG_EN                     (VDBG_EN && 1)
#define VMMI_BTN_DBG_EN                     (VDBG_EN && 0)
#define VAPP_DM_DBG_EN                      (VDBG_EN && 0)
#define VAPP_DM_MULTI_SWIT_DBG_EN           (VDBG_EN && 0)
#define VAPP_DM_CONN_BACK_DBG_EN            (VDBG_EN && 0)
#define VAPP_IOP_FILTER_DBG_EN              (VDBG_EN && 0)
#define VAPP_DM_SNIFF_DBG_EN                (VDBG_EN && 0)
#define VAPP_HFP_DBG_EN                     (VDBG_EN && 0)
#define VAPP_MUSIC_DBG_EN                   (VDBG_EN && 0)
#define VAPP_HLP_DBG_EN                     (VDBG_EN && 0)
#define VAPP_OS_HOOK_DBG_EN                 (VDBG_EN && 0)
#define VCODEC_DBG_EN                       (VDBG_EN && 0)
#define VDSP_DBG_EN                         (VDBG_EN && 1)
#define VPMU_DBG_EN                         (VDBG_EN && 0)
#define VCDC_DBG_EN                         (VDBG_EN && 0)
#define VAPP_SPP_DBG_EN                     (VDBG_EN && 0) 
#define VSYS_DMA_DBG_EN                     (VDBG_EN && 0)
#define VSYS_SPI_DBG_EN                     (VDBG_EN && 0)
#define VAPP_VOICE_DBG_EN                   (VDBG_EN && 0)
#define VAPP_HID_DBG_EN                     (VDBG_EN && 0)
#ifndef VIO_TIMING_DEBUG_EN
#define VIO_TIMING_DEBUG_EN                 (VDBG_EN && 0)
#endif
#define VIO_TIMING_PIN_EN                   (VIO_TIMING_DEBUG_EN && 0)
#define VAPP_DURATION_DBG_EN                (VDBG_EN && 0)
#define VSYS_BB_DBG_EN                      (VDBG_EN && 0)
#define VAPP_AUDIOTRANS_DBG_EN              (VDBG_EN && 0)
#define VAPP_TWS_DBG_EN                     (VDBG_EN && 0)
#define VAPP_A2DPBROADCAST_DBG_EN           (VDBG_EN && 0)
#define LE_CONTROLLER_DBG_EN                (VDBG_EN && 0)
#define VSYS_OTG_DBG_EN                     (VDBG_EN && 0)
#define VSYS_SD_DBG_EN                      (VDBG_EN && 0)
#define VAPP_BLE_DBG_EN                     (VDBG_EN && 0)
#define VSYS_EVT_DEMO_TEST_EN               (0) //added by zd
#define VSYS_LOCK_DEMO_TEST_EN              (0) //added by zd
#define VSYS_MSG_DEMO_TEST_EN               (0) //added by zd
#define VAPP_ECSB_DBG_EN                    (VDBG_EN && 0)
#define VAPP_AUDIOBC_DBG_EN                 (VDBG_EN && 0)
#define VAPP_USBAUDIO_DBG_EN                (VDBG_EN && 0)
#define VDBG_DS_CACHE_EN                    (VDBG_EN && 0)//for VDEV_ECKEY_DS_CACHE_SUPPORTED
#define VSYS_IPC_DBG_EN                     (VDBG_EN && 0)//from vsys_ipc.h
#define VCMDBUS_DBG_EN                      (VDBG_EN && 0)
#define VCDC_ADA_EN                         (VDBG_EN && 0)
#define VTONE_DBG_EN                        (VDBG_EN && 0)
#define GFPS_DBG_EN                         (VDBG_EN && 0)  //modify 20191227 gao.frank

#define VDEBUG_BT_SW_EN                     (0)


#define LONG_IRQ_PRINT      0
#define LONG_IRQ_THRESHOLD  6400

#ifdef CORE_SEL_MCUTOP

#undef VAPP_BLE_DBG_EN
#define VAPP_BLE_DBG_EN     (VDBG_EN && 0)
#endif

// SOC Simulation testbench use value to probe and debug
#if (VIMC_TB_SOC_SIM==1)
#define VIMCTB_INT_ADDR                 0x400280c4 // RFC register, agc_map26[13:0], agc_map27[29:16]
#define VIMCTB_VISR_NMI                 26
#define VIMCTB_VISR_HardFault           25
#define VIMCTB_VISR_MMUFault            24
#define VIMCTB_VISR_BusFault            23
#define VIMCTB_VISR_UsageFault          22
#define VIMCTB_VISR_SVCall              21
#define VIMCTB_VISR_DebugMon            20
#define VIMCTB_OS_CPU_PendSVHandler     19
#define VIMCTB_OS_CPU_SysTickHandler    18

#define VIMCTB_VISR_VA7621              9
#define VIMCTB_VISR_DMA                 8
#define VIMCTB_VISR_Modem               7
#define VIMCTB_VISR_GPIOInput           6
#define VIMCTB_VISR_Timer               5
#define VIMCTB_VISR_I2c                 4
#define VIMCTB_VISR_Uart                3
#define VIMCTB_SYSirq_IRQ_Handler       2
#define VIMCTB_VISR_Dummy               1
#endif

/*
   VDBG_LOG_LEVEL specifies the log level, levels with larger value
   will not be printed.
*/
#define LEVEL_FATAL           2
#define LEVEL_CRITICAL        4
#define LEVEL_ERROR           6
#define LEVEL_WARNING         8
#define LEVEL_NOTICE          10
#define VDBG_LOG_LEVEL        LEVEL_NOTICE

/* if the terminal can not support ANSI color display, please comment it */
#define TERM_ANSI_COLOR_SUPPORT

/*
    VDBG_IO_RETARGET specifies the IO to retarget to, which will be used in stdio lib
*/
#define IO_ITM                  1
#define IO_UART                 2
#define IO_SIM                  3
#define IO_GPIO                 4
#define IO_MEM                  5
#define IO_FLASH                6
#define IO_USB_SERIAL           7
#define IO_RTT                  8
#define IO_USB_RTT              9
#define IO_UART_RTT             10
#define IO_UART_SIM             11
#define IO_UART_SIM_RTT         12

#ifndef VDBG_IO_RETARGET
#ifdef FPGA_QA_BUILD
#define VDBG_IO_RETARGET        IO_UART
#else
#define VDBG_IO_RETARGET        IO_UART_RTT
#endif
#endif

#if (VDBG_IO_RETARGET == IO_RTT)
#define VDBG_IO_SHARE_RETARGET              (1)/* TOP use RTT Terminal 1 , BT use RTT Terminal 0  */
#define VDBG_DATA_SHARE_RETARGET            (0)/* use vputs_rtt_data() function Upload data , does not affect log output ; <Data Logging...> */
#endif

/*  MCUTOP Debug config   */
// #ifndef FPGA_QA_BUILD
// #if (!VDBG_IO_SHARE_RETARGET)
// #if (CORE_SEL_MCUTOP && VDBG_IO_RETARGET != IO_USB_RTT && VDBG_IO_RETARGET != IO_UART_RTT)
// #undef VDBG_IO_RETARGET
// #define VDBG_IO_RETARGET        IO_UART //usbaudio mode use usb to transfer aux,so should change uart to catch log
// #endif
// #endif
// #endif

/*  BOOTLOADER Debug config   */
#ifdef BOOTLOADER_FOR_DEBUG
#undef VDBG_IO_RETARGET
#define VDBG_IO_RETARGET    IO_UART
#endif

//#ifdef RELEASE_BUILD
//#undef VDBG_IO_RETARGET
//#define VDBG_IO_RETARGET    IO_FLASH
//#endif

#if (VDBG_IO_RETARGET == IO_ITM)

#define VDBG_SWO_BT         0
#define VDBG_SWO_TOP        1

#undef TERM_ANSI_COLOR_SUPPORT

#elif (VDBG_IO_RETARGET == IO_RTT)

/* ANSI color display is not supported by ITM viewer */
#undef TERM_ANSI_COLOR_SUPPORT

#elif (VDBG_IO_RETARGET == IO_UART)

#elif (VDBG_IO_RETARGET == IO_SIM)  

/* ANSI color display is not supported by simulation */
#undef TERM_ANSI_COLOR_SUPPORT

#elif (VDBG_IO_RETARGET == IO_FLASH)  

#undef TERM_ANSI_COLOR_SUPPORT

#endif

#if (!CORE_SEL_MCUTOP && (VDBG_IO_RETARGET == IO_USB_RTT || VDBG_IO_RETARGET == IO_UART_RTT || VDBG_IO_RETARGET == IO_UART_SIM_RTT))
#undef  VDBG_IO_RETARGET
#define VDBG_IO_RETARGET IO_RTT
#endif

#if (VDBG_IO_RETARGET==IO_FLASH)
    extern char FLASH_system_log_buf[];
    extern void FLASH_system_log_userdata_store(char* buf);
    extern char* FLASH_system_log_add_time_stamp(char* buf);
#define FLASH_LOG_Record(...) \
do{\
    vcom_sprintf(FLASH_system_log_add_time_stamp(FLASH_system_log_buf),__VA_ARGS__);\
    FLASH_system_log_userdata_store(FLASH_system_log_buf);\
}while(0)
#else
#define FLASH_LOG_Record(...)
#endif

#if ZIPMSG_DBG_EN
#undef TERM_ANSI_COLOR_SUPPORT
#endif

#define NORMAL_LOG_FORMAT_TOP "\x1b[0;34m"
#define NORMAL_LOG_FORMAT_BT  "\x1b[0;30m"

#ifdef CORE_SEL_MCUTOP
#define NORMAL_LOG_FORMAT NORMAL_LOG_FORMAT_TOP
#else
#define NORMAL_LOG_FORMAT NORMAL_LOG_FORMAT_BT
#endif

#ifdef TERM_ANSI_COLOR_SUPPORT
#define ASSERT_PREFIX           "\x1b[5;1;31;40m[ASSERT]" NORMAL_LOG_FORMAT
#define FATAL_PREFIX            "\x1b[1;37;41m[FATAL]" NORMAL_LOG_FORMAT
#define CRITICAL_PREFIX         "\x1b[1;37;45m[CRITICAL]" NORMAL_LOG_FORMAT
#define ERROR_PREFIX            "\x1b[1;37;43m[ERROR]" NORMAL_LOG_FORMAT
#define WARNING_PREFIX          "\x1b[1;37;44m[WARNING]" NORMAL_LOG_FORMAT
#define NOTICE_PREFIX           
#else
#define ASSERT_PREFIX           "[ASSERT] "
#define FATAL_PREFIX            "[FATAL] "
#define CRITICAL_PREFIX         "[CRITICAL] "
#define ERROR_PREFIX            "[ERROR] "
#define WARNING_PREFIX          "[WARNING] "
#define NOTICE_PREFIX           ""
#endif


#if (VDBG_EN > 0)
    extern void vcom_printf(const char* fmt, ...);
    extern void vcom_dumphex_printf(const unsigned char *p_data, unsigned short data_len,const char *fmt);
    extern void vdbg_assert(const char *module_name,unsigned int line_num,const char *expr,unsigned int ret_addr);
#if (VDBG_IO_RETARGET==IO_RTT)
    extern void vputs_rtt(char* str, unsigned int len);
    extern void RTT_Crash_data_output(unsigned int return_addr, void* p_StkPtr);
#endif
    extern void vcom_log_printf(const char* fmt, ...);
    extern void vputs(char* s);
    extern void VDBG_Init(void);
    void VDBG_PrintTaskInfo(void);
#else
//#define vputs( a )
#endif

/*
    printf marco for each debug level
*/
#if (VDBG_EN > 0)

#if ZIPMSG_DBG_EN
#define DEBUG_HASH1(s,i,x)   (x*65599u+(UINT8)s[(i)<sizeof(s)-1?sizeof(s)-1-1-(i):sizeof(s)-1])
#define DEBUG_HASH4(s,i,x)   DEBUG_HASH1(s,i,DEBUG_HASH1(s,i+1,DEBUG_HASH1(s,i+2,DEBUG_HASH1(s,i+3,x))))
#define DEBUG_HASH16(s,i,x)  DEBUG_HASH4(s,i,DEBUG_HASH4(s,i+4,DEBUG_HASH4(s,i+8,DEBUG_HASH4(s,i+12,x))))
#define DEBUG_HASH64(s,i,x)  DEBUG_HASH16(s,i,DEBUG_HASH16(s,i+16,DEBUG_HASH16(s,i+32,DEBUG_HASH16(s,i+48,x))))
#define DEBUG_HASH256(s,i,x) DEBUG_HASH64(s,i,DEBUG_HASH64(s,i+64,DEBUG_HASH64(s,i+128,DEBUG_HASH64(s,i+192,x))))
#define DEBUG_HASH(s)        ((UINT16)(DEBUG_HASH64(s,0,0)^(DEBUG_HASH64(s,0,0)>>16)))

/* print zipped message */
#define DEBUG_PRINTF(level,fmat,...)      vcom_log_printf("%04X%04X" fmat,DEBUG_HASH(__FILE__),__LINE__,##__VA_ARGS__)
#define DEBUG_CPRINTF(...)                vcom_log_printf(__VA_ARGS__)

#else
#if (defined(CONFIG_DEBUG_TYPE) && CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE)    
#define DEBUG_ZPRINTF(level,fmat,...)     
#define DEBUG_PRINTF(level,...)           LOG_PRINTK(level __VA_ARGS__)
#define DEBUG_CPRINTF(...)                LOG_PRINTK(__VA_ARGS__)
#define DEBUG_ASSERT(...)                 vdbg_assert(__VA_ARGS__)
#else
#define DEBUG_ZPRINTF(level,fmat,...) 
#define DEBUG_PRINTF(level,...)           vcom_printf(level __VA_ARGS__)
#define DEBUG_CPRINTF(...)                vcom_printf(__VA_ARGS__)
#define DEBUG_ASSERT(...)                 vdbg_assert(__VA_ARGS__)
#endif

#endif //VDBG_ZIPMSG_EN


#if (VDBG_ASSERT_EN > 0)
#if (defined(CONFIG_DEBUG_TYPE) && CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE)
#define ASSERT(expr) \
    if (!(expr)) { \
        LOG_PANIC(); \
        DEBUG_ASSERT(ASSERT_PREFIX \
            MODULE_NAME, LINE_NUMBER, #expr, __GET_RETURN_ADDR());\
    }
#else
#define ASSERT(expr) \
    if (!(expr)) { \
        DEBUG_ASSERT(ASSERT_PREFIX \
            MODULE_NAME, LINE_NUMBER, #expr, __GET_RETURN_ADDR());\
    }
#endif
#else
#define ASSERT(expr)
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_FATAL)
#define FATAL_PRINTF(...)       DEBUG_PRINTF(FATAL_PREFIX,##__VA_ARGS__)
#else
#define FATAL_PRINTF(...)
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_CRITICAL)
#define CRITICAL_PRINTF(...)    DEBUG_PRINTF(CRITICAL_PREFIX,##__VA_ARGS__)
#else
#define CRITICAL_PRINTF(...)
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_ERROR)
#define ERROR_PRINTF(...)       DEBUG_PRINTF(ERROR_PREFIX,##__VA_ARGS__)
#else
#define ERROR_PRINTF(...)
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_WARNING)
#define WARNING_PRINTF(...)     DEBUG_PRINTF(WARNING_PREFIX,##__VA_ARGS__)
#else
#define WARNING_PRINTF(...)
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_NOTICE)
#define NOTICE_PRINTF(...)      DEBUG_PRINTF(NOTICE_PREFIX,##__VA_ARGS__)
#else
#define NOTICE_PRINTF(...)
#endif


#if (defined(CONFIG_DEBUG_TYPE) && CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE)

#if (VDBG_LOG_LEVEL >= LEVEL_FATAL)
#define FATAL_HEXDUMP_PRINTF(data, length,...)       LOG_PRINTK_HEXDUMP(data, length, FATAL_PREFIX __VA_ARGS__)  
#else
#define FATAL_HEXDUMP_PRINTF(data, length,...)       
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_CRITICAL)
#define CRITICAL_HEXDUMP_PRINTF(data, length,...)    LOG_PRINTK_HEXDUMP(data, length, CRITICAL_PREFIX __VA_ARGS__)
#else
#define CRITICAL_HEXDUMP_PRINTF(data, length,...)    
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_ERROR)
#define ERROR_HEXDUMP_PRINTF(data, length,...)       LOG_PRINTK_HEXDUMP(data, length, ERROR_PREFIX __VA_ARGS__)
#else
#define ERROR_HEXDUMP_PRINTF(data, length,...)       
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_WARNING)
#define WARNING_HEXDUMP_PRINTF(data, length,...)     LOG_PRINTK_HEXDUMP(data, length, WARNING_PREFIX __VA_ARGS__)
#else
#define WARNING_HEXDUMP_PRINTF(data, length,...)     
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_NOTICE)
#define NOTICE_HEXDUMP_PRINTF(data, length,...)      LOG_PRINTK_HEXDUMP(data, length, NOTICE_PREFIX __VA_ARGS__)
#else
#define NOTICE_HEXDUMP_PRINTF(data, length,...)      
#endif

#else

#if (VDBG_LOG_LEVEL >= LEVEL_FATAL)
#define FATAL_HEXDUMP_PRINTF(data, length,...)       vcom_dumphex_printf(data, length, FATAL_PREFIX __VA_ARGS__)  
#else
#define FATAL_HEXDUMP_PRINTF(data, length,...)       
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_CRITICAL)
#define CRITICAL_HEXDUMP_PRINTF(data, length,...)    vcom_dumphex_printf(data, length, CRITICAL_PREFIX __VA_ARGS__)
#else
#define CRITICAL_HEXDUMP_PRINTF(data, length,...)    
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_ERROR)
#define ERROR_HEXDUMP_PRINTF(data, length,...)       vcom_dumphex_printf(data, length, CRITICAL_PREFIX __VA_ARGS__)
#else
#define ERROR_HEXDUMP_PRINTF(data, length,...)       
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_WARNING)
#define WARNING_HEXDUMP_PRINTF(data, length,...)     vcom_dumphex_printf(data, length, WARNING_PREFIX __VA_ARGS__)
#else
#define WARNING_HEXDUMP_PRINTF(data, length,...)     
#endif

#if (VDBG_LOG_LEVEL >= LEVEL_NOTICE)
#define NOTICE_HEXDUMP_PRINTF(data, length,...)      vcom_dumphex_printf(data, length, NOTICE_PREFIX __VA_ARGS__)
#else
#define NOTICE_HEXDUMP_PRINTF(data, length,...)      
#endif

#endif //(CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE)


#else

//#define DEBUG_CPRINTF(...)
#define ASSERT(expr)

/* disable all level print */
#define FATAL_PRINTF(...)
#define CRITICAL_PRINTF(...)
#define ERROR_PRINTF(...)
#define WARNING_PRINTF(...)
#define NOTICE_PRINTF(...)

#define FATAL_HEXDUMP_PRINTF(...)
#define CRITICAL_HEXDUMP_PRINTF(...)
#define ERROR_HEXDUMP_PRINTF(...)
#define WARNING_HEXDUMP_PRINTF(...)
#define NOTICE_HEXDUMP_PRINTF(...)

#define DEBUG_ZPRINTF(level,fmat,...) 
#define DEBUG_PRINTF(level,...)
#define DEBUG_CPRINTF(...)
#define DEBUG_ASSERT(...)
#endif


#if (VDBG_EN > 0 && 0)
#define AT_NPRINTF(...)         NOTICE_PRINTF(" AT:"##__VA_ARGS__)
#else
#define AT_NPRINTF(...) 
#endif



/*
    add module debug print macro here
*/
#if (LE_CONTROLLER_DBG_EN > 0)
#define VCONTROLER_FPRINTF(...)         FATAL_PRINTF(" CONTROLER>> "##__VA_ARGS__)
#define VCONTROLER_CPRINTF(...)         CRITICAL_PRINTF(" CONTROLER>> "##__VA_ARGS__)
#define VCONTROLER_EPRINTF(...)         ERROR_PRINTF(" CONTROLER>> "##__VA_ARGS__)
#define VCONTROLER_WPRINTF(...)         WARNING_PRINTF(" CONTROLER>> "##__VA_ARGS__)
#define VCONTROLER_NPRINTF(...)         NOTICE_PRINTF(" CONTROLER>> "##__VA_ARGS__)
#else
#define VCONTROLER_FPRINTF(...)
#define VCONTROLER_CPRINTF(...)
#define VCONTROLER_EPRINTF(...)
#define VCONTROLER_WPRINTF(...)
#define VCONTROLER_NPRINTF(...)
#endif

#if (VCOMMON_DBG_EN > 0)
#define VCOMMON_FPRINTF         FATAL_PRINTF
#define VCOMMON_CPRINTF         CRITICAL_PRINTF
#define VCOMMON_EPRINTF         ERROR_PRINTF
#define VCOMMON_WPRINTF         WARNING_PRINTF
#define VCOMMON_NPRINTF         NOTICE_PRINTF
#else
#define VCOMMON_FPRINTF(...)
#define VCOMMON_CPRINTF(...)
#define VCOMMON_EPRINTF(...)
#define VCOMMON_WPRINTF(...)
#define VCOMMON_NPRINTF(...)
#endif

#if (VCDC_ADA_EN > 0)
#define CDCADA_FP(...)      FATAL_PRINTF(" CDCADA>> "##__VA_ARGS__)
#define CDCADA_CP(...)      CRITICAL_PRINTF(" CDCADA>> "##__VA_ARGS__)
#define CDCADA_EP(...)      ERROR_PRINTF(" CDCADA>> "##__VA_ARGS__)
#define CDCADA_WP(...)      WARNING_PRINTF(" CDCADA>> "##__VA_ARGS__)
#define CDCADA_NP(...)      NOTICE_PRINTF(" CDCADA>> "##__VA_ARGS__)
#else
#define CDCADA_FP(...)
#define CDCADA_CP(...)
#define CDCADA_EP(...)
#define CDCADA_WP(...)
#define CDCADA_NP(...)
#endif
#define VCDC_ADA_DBG 0 //print  data captured by adc&dac in cycles.

#define VTONE_EP(...)       ERROR_PRINTF(" VT>> " __VA_ARGS__)
#define VTONE_WP(...)       WARNING_PRINTF(" VT>> " __VA_ARGS__##)
#if (VTONE_DBG_EN > 0)
#define VTONE_NP(...)       NOTICE_PRINTF(" VT>> "##__VA_ARGS__)
#else
#define VTONE_NP(...)
#endif
#define VTONE_DBG    1


#if (VSYS_SOC_DBG_EN > 0)
#define VSYS_SOC_FPRINTF        FATAL_PRINTF
#define VSYS_SOC_CPRINTF        CRITICAL_PRINTF
#define VSYS_SOC_EPRINTF        ERROR_PRINTF
#define VSYS_SOC_WPRINTF        WARNING_PRINTF
#define VSYS_SOC_NPRINTF        NOTICE_PRINTF
#else
#define VSYS_SOC_FPRINTF(...)
#define VSYS_SOC_CPRINTF(...)
#define VSYS_SOC_EPRINTF(...)
#define VSYS_SOC_WPRINTF(...)
#define VSYS_SOC_NPRINTF(...)
#endif

#if (VSYS_USERDATA_DBG_EN > 0)
#define VSYS_USERDATA_FPRINTF       FATAL_PRINTF
#define VSYS_USERDATA_CPRINTF       CRITICAL_PRINTF
#define VSYS_USERDATA_EPRINTF       ERROR_PRINTF
#define VSYS_USERDATA_WPRINTF       WARNING_PRINTF
#define VSYS_USERDATA_NPRINTF       NOTICE_PRINTF
#else
#define VSYS_USERDATA_FPRINTF(...)
#define VSYS_USERDATA_CPRINTF(...)
#define VSYS_USERDATA_EPRINTF(...)
#define VSYS_USERDATA_WPRINTF(...)
#define VSYS_USERDATA_NPRINTF(...)
#endif

#if (VSYS_PWR_DBG_EN > 0)
#define VSYS_PWR_FPRINTF       FATAL_PRINTF
#define VSYS_PWR_CPRINTF       CRITICAL_PRINTF
#define VSYS_PWR_EPRINTF       ERROR_PRINTF
#define VSYS_PWR_WPRINTF       WARNING_PRINTF
#define VSYS_PWR_NPRINTF       NOTICE_PRINTF
#else
#define VSYS_PWR_FPRINTF(...)
#define VSYS_PWR_CPRINTF(...)
#define VSYS_PWR_EPRINTF(...)
#define VSYS_PWR_WPRINTF(...)
#define VSYS_PWR_NPRINTF(...)
#endif

#if (VSYS_MEM_DBG_EN > 0)
#define VSYS_MEM_FPRINTF        FATAL_PRINTF
#define VSYS_MEM_CPRINTF        CRITICAL_PRINTF
#define VSYS_MEM_EPRINTF        ERROR_PRINTF
#define VSYS_MEM_WPRINTF        WARNING_PRINTF
#define VSYS_MEM_NPRINTF        NOTICE_PRINTF
#else
#define VSYS_MEM_FPRINTF(...)
#define VSYS_MEM_CPRINTF(...)
#define VSYS_MEM_EPRINTF(...)
#define VSYS_MEM_WPRINTF(...)
#define VSYS_MEM_NPRINTF(...)
#endif

#if (VSYS_CFG_DBG_EN > 0)
#define VSYS_CFG_FPRINTF        FATAL_PRINTF
#define VSYS_CFG_CPRINTF        CRITICAL_PRINTF
#define VSYS_CFG_EPRINTF        ERROR_PRINTF
#define VSYS_CFG_WPRINTF        WARNING_PRINTF
#define VSYS_CFG_NPRINTF        NOTICE_PRINTF
#else
#define VSYS_CFG_FPRINTF(...)
#define VSYS_CFG_CPRINTF(...)
#define VSYS_CFG_EPRINTF(...)
#define VSYS_CFG_WPRINTF(...)
#define VSYS_CFG_NPRINTF(...)
#endif

#if (VSYS_TMR_DBG_EN > 0)
#define VSYS_TMR_FPRINTF        FATAL_PRINTF
#define VSYS_TMR_CPRINTF        CRITICAL_PRINTF
#define VSYS_TMR_EPRINTF        ERROR_PRINTF
#define VSYS_TMR_WPRINTF        WARNING_PRINTF
#define VSYS_TMR_NPRINTF        NOTICE_PRINTF
#else
#define VSYS_TMR_FPRINTF(...)
#define VSYS_TMR_CPRINTF(...)
#define VSYS_TMR_EPRINTF(...)
#define VSYS_TMR_WPRINTF(...)
#define VSYS_TMR_NPRINTF(...)
#endif

#if (VMMI_DBG_EN > 0)
#define VMMI_FPRINTF            FATAL_PRINTF
#define VMMI_CPRINTF            CRITICAL_PRINTF
#define VMMI_EPRINTF            ERROR_PRINTF
#define VMMI_WPRINTF            WARNING_PRINTF
#define VMMI_NPRINTF            NOTICE_PRINTF
#else
#define VMMI_FPRINTF(...)
#define VMMI_CPRINTF(...)
#define VMMI_EPRINTF(...)
#define VMMI_WPRINTF(...)
#define VMMI_NPRINTF(...)
#endif

#if (VAPP_INIT_DBG_EN > 0)
#define VAPP_INIT_FPRINTF       FATAL_PRINTF
#define VAPP_INIT_CPRINTF       CRITICAL_PRINTF
#define VAPP_INIT_EPRINTF       ERROR_PRINTF
#define VAPP_INIT_WPRINTF       WARNING_PRINTF
#define VAPP_INIT_NPRINTF       NOTICE_PRINTF
#else
#define VAPP_INIT_FPRINTF(...)
#define VAPP_INIT_CPRINTF(...)
#define VAPP_INIT_EPRINTF(...)
#define VAPP_INIT_WPRINTF(...)
#define VAPP_INIT_NPRINTF(...)
#endif

#if (VSYS_ECK_DBG_EN > 0)
#define ECK_FPRINTF          FATAL_PRINTF
#define ECK_CPRINTF          CRITICAL_PRINTF
#define ECK_EPRINTF          ERROR_PRINTF
#define ECK_WPRINTF          WARNING_PRINTF
#define ECK_NPRINTF          NOTICE_PRINTF
//#define ECK_LPRINTF(fmt,...) NOTICE_PRINTF("%s(%05d):" fmt, __func__, __LINE__,##__VA_ARGS__) /* log display, for debug */
#else
#define ECK_FPRINTF(...)
#define ECK_CPRINTF(...)
#define ECK_EPRINTF(...)
#define ECK_WPRINTF(...)
#define ECK_NPRINTF(...)
//#define ECK_LPRINTF(fmt,...)
#endif

#if (VAPP_COBRA_DBG_EN > 0)
#define VAPP_COBRA_FPRINTF      FATAL_PRINTF
#define VAPP_COBRA_CPRINTF      CRITICAL_PRINTF
#define VAPP_COBRA_EPRINTF      ERROR_PRINTF
#define VAPP_COBRA_WPRINTF      WARNING_PRINTF
#define VAPP_COBRA_NPRINTF      NOTICE_PRINTF
#else
#define VAPP_COBRA_FPRINTF(...)
#define VAPP_COBRA_CPRINTF(...)
#define VAPP_COBRA_EPRINTF(...)
#define VAPP_COBRA_WPRINTF(...)
#define VAPP_COBRA_NPRINTF(...)
#endif

#if (VTOP_HLP_DBG_EN > 0)
#define TOP_FPRINTF(...)        FATAL_PRINTF(" TOP>> "##__VA_ARGS__)
#define TOP_CPRINTF(...)        CRITICAL_PRINTF(" TOP>> "##__VA_ARGS__)
#define TOP_EPRINTF(...)        ERROR_PRINTF(" TOP>> "##__VA_ARGS__)
#define TOP_WPRINTF(...)        WARNING_PRINTF(" TOP>> "##__VA_ARGS__)
#define TOP_NPRINTF(...)        NOTICE_PRINTF(" TOP>> " __VA_ARGS__)
#else
#define TOP_FPRINTF(...)
#define TOP_CPRINTF(...)
#define TOP_EPRINTF(...)
#define TOP_WPRINTF(...)
#define TOP_NPRINTF(...)
#endif

#if (VMMI_BTN_DBG_EN > 0)
#define BTN_FPRINTF(...)        FATAL_PRINTF(" BTN>> "##__VA_ARGS__)
#define BTN_CPRINTF(...)        CRITICAL_PRINTF(" BTN>> "##__VA_ARGS__)
#define BTN_EPRINTF(...)        ERROR_PRINTF(" BTN>> "##__VA_ARGS__)
#define BTN_WPRINTF(...)        WARNING_PRINTF(" BTN>> "##__VA_ARGS__)
#define BTN_NPRINTF(...)        NOTICE_PRINTF(" BTN>> "##__VA_ARGS__)
#else
#define BTN_FPRINTF(...)
#define BTN_CPRINTF(...)
#define BTN_EPRINTF(...)
#define BTN_WPRINTF(...)
#define BTN_NPRINTF(...)
#endif

#if (VAPP_DM_DBG_EN > 0)
#define DM_FPRINTF(...)         FATAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_CPRINTF(...)         CRITICAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_EPRINTF(...)         ERROR_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_WPRINTF(...)         WARNING_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_NPRINTF(...)         NOTICE_PRINTF(" DM>> "##__VA_ARGS__)
#else
#define DM_FPRINTF(...)
#define DM_CPRINTF(...)
#define DM_EPRINTF(...)
#define DM_WPRINTF(...)
#define DM_NPRINTF(...)
#endif

#if (VAPP_DM_MULTI_SWIT_DBG_EN > 0)
#define DM_FPRINTF_MS(...)      FATAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_CPRINTF_MS(...)      CRITICAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_EPRINTF_MS(...)      {ERROR_PRINTF(" DM>> "##__VA_ARGS__);while(1);}
#define DM_WPRINTF_MS(...)      WARNING_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_NPRINTF_MS(...)      NOTICE_PRINTF(" DM>> "##__VA_ARGS__)
#else
#define DM_FPRINTF_MS(...)
#define DM_CPRINTF_MS(...)
#define DM_EPRINTF_MS(...)
#define DM_WPRINTF_MS(...)
#define DM_NPRINTF_MS(...)
#endif

#if (VAPP_DM_CONN_BACK_DBG_EN > 0)
#define DM_FPRINTF_CB(...)      FATAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_CPRINTF_CB(...)      CRITICAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_EPRINTF_CB(...)      {ERROR_PRINTF(" DM>> "##__VA_ARGS__);while(1);}
#define DM_WPRINTF_CB(...)      WARNING_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_NPRINTF_CB(...)      NOTICE_PRINTF(" DM>> "##__VA_ARGS__)
#else
#define DM_FPRINTF_CB(...)
#define DM_CPRINTF_CB(...)
#define DM_EPRINTF_CB(...)
#define DM_WPRINTF_CB(...)
#define DM_NPRINTF_CB(...)
#endif

#if (VAPP_IOP_FILTER_DBG_EN > 0)
#define IOP_FILTER_FPRINTF(...)         FATAL_PRINTF(" IOP>> "##__VA_ARGS__)
#define IOP_FILTER_CPRINTF(...)         CRITICAL_PRINTF(" IOP>> "##__VA_ARGS__)
#define IOP_FILTER_EPRINTF(...)         {ERROR_PRINTF(" IOP>> "##__VA_ARGS__);while(1);}
#define IOP_FILTER_WPRINTF(...)         WARNING_PRINTF(" IOP>> "##__VA_ARGS__)
#define IOP_FILTER_NPRINTF(...)         NOTICE_PRINTF(" IOP>> "##__VA_ARGS__)
#else
#define IOP_FILTER_FPRINTF(...)
#define IOP_FILTER_CPRINTF(...)
#define IOP_FILTER_EPRINTF(...)
#define IOP_FILTER_WPRINTF(...)
#define IOP_FILTER_NPRINTF(...)
#endif


#if (VAPP_DM_SNIFF_DBG_EN > 0)
#define DM_FPRINTF_SNIFF(...)       FATAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_CPRINTF_SNIFF(...)       CRITICAL_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_EPRINTF_SNIFF(...)       ERROR_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_WPRINTF_SNIFF(...)       WARNING_PRINTF(" DM>> "##__VA_ARGS__)
#define DM_NPRINTF_SNIFF(...)       NOTICE_PRINTF(" DM>> "##__VA_ARGS__)
#else
#define DM_FPRINTF_SNIFF(...)
#define DM_CPRINTF_SNIFF(...)
#define DM_EPRINTF_SNIFF(...)
#define DM_WPRINTF_SNIFF(...)
#define DM_NPRINTF_SNIFF(...)
#endif

#if (VAPP_HLP_DBG_EN > 0)
#define HLP_FPRINTF(...)        FATAL_PRINTF(" HLP>> "##__VA_ARGS__)
#define HLP_CPRINTF(...)        CRITICAL_PRINTF(" HLP>> "##__VA_ARGS__)
#define HLP_EPRINTF(...)        ERROR_PRINTF(" HLP>> "##__VA_ARGS__)
#define HLP_WPRINTF(...)        WARNING_PRINTF(" HLP>> "##__VA_ARGS__)
#define HLP_NPRINTF(...)        NOTICE_PRINTF(" HLP>> "##__VA_ARGS__)
#else
#define HLP_FPRINTF(...)
#define HLP_CPRINTF(...)
#define HLP_EPRINTF(...)
#define HLP_WPRINTF(...)
#define HLP_NPRINTF(...)
#endif

#if (VAPP_HFP_DBG_EN > 0)
#define HFP_FPRINTF(...)        FATAL_PRINTF(" HFP>> "##__VA_ARGS__)
#define HFP_CPRINTF(...)        CRITICAL_PRINTF(" HFP>> "##__VA_ARGS__)
#define HFP_EPRINTF(...)        ERROR_PRINTF(" HFP>> "##__VA_ARGS__)
#define HFP_WPRINTF(...)        WARNING_PRINTF(" HFP>> "##__VA_ARGS__)
#define HFP_NPRINTF(...)        NOTICE_PRINTF(" HFP>> "##__VA_ARGS__)
#else
#define HFP_FPRINTF(...)
#define HFP_CPRINTF(...)
#define HFP_EPRINTF(...)
#define HFP_WPRINTF(...)
#define HFP_NPRINTF(...)
#endif

#if (VAPP_MUSIC_DBG_EN > 0)
#define MUSIC_FPRINTF(...)      FATAL_PRINTF(" MUSIC>> "##__VA_ARGS__)
#define MUSIC_CPRINTF(...)      CRITICAL_PRINTF(" MUSIC>> "##__VA_ARGS__)
#define MUSIC_EPRINTF(...)      ERROR_PRINTF(" MUSIC>> "##__VA_ARGS__)
#define MUSIC_WPRINTF(...)      WARNING_PRINTF(" MUSIC>> "##__VA_ARGS__)
#define MUSIC_NPRINTF(...)      NOTICE_PRINTF(" MUSIC>> "##__VA_ARGS__)
#else
#define MUSIC_FPRINTF(...)
#define MUSIC_CPRINTF(...)
#define MUSIC_EPRINTF(...)
#define MUSIC_WPRINTF(...)
#define MUSIC_NPRINTF(...)
#endif

#if (VAPP_SPP_DBG_EN > 0)
#define SPP_FPRINTF(...)        FATAL_PRINTF(" SPP>> "##__VA_ARGS__)
#define SPP_CPRINTF(...)        CRITICAL_PRINTF(" SPP>> "##__VA_ARGS__)
#define SPP_EPRINTF(...)        ERROR_PRINTF(" SPP>> "##__VA_ARGS__)
#define SPP_WPRINTF(...)        WARNING_PRINTF(" SPP>> "##__VA_ARGS__)
#define SPP_NPRINTF(...)        NOTICE_PRINTF(" SPP>> "##__VA_ARGS__)
#else
#define SPP_FPRINTF(...)
#define SPP_CPRINTF(...)
#define SPP_EPRINTF(...)
#define SPP_WPRINTF(...)
#define SPP_NPRINTF(...)
#endif

#if (VAPP_OS_HOOK_DBG_EN > 0)
#define VAPP_OS_HOOK_FPRINTF        FATAL_PRINTF
#define VAPP_OS_HOOK_CPRINTF        CRITICAL_PRINTF
#define VAPP_OS_HOOK_EPRINTF        ERROR_PRINTF
#define VAPP_OS_HOOK_WPRINTF        WARNING_PRINTF
#define VAPP_OS_HOOK_NPRINTF        NOTICE_PRINTF
#else
#define VAPP_OS_HOOK_FPRINTF(...)
#define VAPP_OS_HOOK_CPRINTF(...)
#define VAPP_OS_HOOK_EPRINTF(...)
#define VAPP_OS_HOOK_WPRINTF(...)
#define VAPP_OS_HOOK_NPRINTF(...)
#endif

#if (VCODEC_DBG_EN > 0)
#define CODEC_FPRINTF(...)      FATAL_PRINTF(" CODEC>> "##__VA_ARGS__)
#define CODEC_CPRINTF(...)      CRITICAL_PRINTF(" CODEC>> "##__VA_ARGS__)
#define CODEC_EPRINTF(...)      ERROR_PRINTF(" CODEC>> "##__VA_ARGS__)
#define CODEC_WPRINTF(...)      WARNING_PRINTF(" CODEC>> "##__VA_ARGS__)
#define CODEC_NPRINTF(...)      NOTICE_PRINTF(" CODEC>> "##__VA_ARGS__)
#else
#define CODEC_FPRINTF(...)
#define CODEC_CPRINTF(...)
#define CODEC_EPRINTF(...)
#define CODEC_WPRINTF(...)
#define CODEC_NPRINTF(...)
#endif

#if (VPMU_DBG_EN > 0)
#define VPMU_FPRINTF        FATAL_PRINTF
#define VPMU_CPRINTF        CRITICAL_PRINTF
#define VPMU_EPRINTF        ERROR_PRINTF
#define VPMU_WPRINTF        WARNING_PRINTF
#define VPMU_NPRINTF        NOTICE_PRINTF
#else
#define VPMU_FPRINTF(...)
#define VPMU_CPRINTF(...)
#define VPMU_EPRINTF(...)
#define VPMU_WPRINTF(...)
#define VPMU_NPRINTF(...)
#endif

#if (VCDC_DBG_EN > 0)
#define VCDC_FPRINTF        FATAL_PRINTF
#define VCDC_CPRINTF        CRITICAL_PRINTF
#define VCDC_EPRINTF        ERROR_PRINTF
#define VCDC_WPRINTF        WARNING_PRINTF
#define VCDC_NPRINTF        NOTICE_PRINTF
#else
#define VCDC_FPRINTF(...)
#define VCDC_CPRINTF(...)
#define VCDC_EPRINTF(...)
#define VCDC_WPRINTF(...)
#define VCDC_NPRINTF(...)
#endif

#if (VCMDBUS_DBG_EN > 0)
#define VCMDBUS_FPRINTF         FATAL_PRINTF
#define VCMDBUS_CPRINTF         CRITICAL_PRINTF
#define VCMDBUS_EPRINTF         ERROR_PRINTF
#define VCMDBUS_WPRINTF         WARNING_PRINTF
#define VCMDBUS_NPRINTF         NOTICE_PRINTF
#else
#define VCMDBUS_FPRINTF(...)
#define VCMDBUS_CPRINTF(...)
#define VCMDBUS_EPRINTF(...)
#define VCMDBUS_WPRINTF(...)
#define VCMDBUS_NPRINTF(...)
#endif



#if (VSYS_DMA_DBG_EN > 0)
#define VDMA_FPRINTF        FATAL_PRINTF
#define VDMA_CPRINTF        CRITICAL_PRINTF
#define VDMA_EPRINTF        ERROR_PRINTF
#define VDMA_WPRINTF        WARNING_PRINTF
#define VDMA_NPRINTF        NOTICE_PRINTF
#else
#define VDMA_FPRINTF(...)
#define VDMA_CPRINTF(...)
#define VDMA_EPRINTF(...)
#define VDMA_WPRINTF(...)
#define VDMA_NPRINTF(...)
#endif

#if (VSYS_SPI_DBG_EN > 0)
#define VSPI_FPRINTF        FATAL_PRINTF
#define VSPI_CPRINTF        CRITICAL_PRINTF
#define VSPI_EPRINTF        ERROR_PRINTF
#define VSPI_WPRINTF        WARNING_PRINTF
#define VSPI_NPRINTF        NOTICE_PRINTF
#else
#define VSPI_FPRINTF(...)
#define VSPI_CPRINTF(...)
#define VSPI_EPRINTF(...)
#define VSPI_WPRINTF(...)
#define VSPI_NPRINTF(...)
#endif

#if (VAPP_VOICE_DBG_EN > 0)
#define VAPP_VOICE_FPRINTF      FATAL_PRINTF
#define VAPP_VOICE_CPRINTF      CRITICAL_PRINTF
#define VAPP_VOICE_EPRINTF      ERROR_PRINTF
#define VAPP_VOICE_WPRINTF      WARNING_PRINTF
#define VAPP_VOICE_NPRINTF      NOTICE_PRINTF
#else
#define VAPP_VOICE_FPRINTF(...)
#define VAPP_VOICE_CPRINTF(...)
#define VAPP_VOICE_EPRINTF(...)
#define VAPP_VOICE_WPRINTF(...)
#define VAPP_VOICE_NPRINTF(...)
#endif

#if (VAPP_HID_DBG_EN > 0)
#define VAPP_HID_FPRINTF        FATAL_PRINTF
#define VAPP_HID_CPRINTF        CRITICAL_PRINTF
#define VAPP_HID_EPRINTF        ERROR_PRINTF
#define VAPP_HID_WPRINTF        WARNING_PRINTF
#define VAPP_HID_NPRINTF        NOTICE_PRINTF
#else
#define VAPP_HID_FPRINTF(...)
#define VAPP_HID_CPRINTF(...)
#define VAPP_HID_EPRINTF(...)
#define VAPP_HID_WPRINTF(...)
#define VAPP_HID_NPRINTF(...)
#endif

#if (VAPP_DURATION_DBG_EN > 0)
#define VAPP_DURATION_FPRINTF       FATAL_PRINTF
#define VAPP_DURATION_CPRINTF       CRITICAL_PRINTF
#define VAPP_DURATION_EPRINTF       ERROR_PRINTF
#define VAPP_DURATION_WPRINTF       WARNING_PRINTF
#define VAPP_DURATION_NPRINTF       NOTICE_PRINTF
#else
#define VAPP_DURATION_FPRINTF(...)
#define VAPP_DURATION_CPRINTF(...)
#define VAPP_DURATION_EPRINTF(...)
#define VAPP_DURATION_WPRINTF(...)
#define VAPP_DURATION_NPRINTF(...)
#endif

#if (VSYS_BB_DBG_EN > 0)
#define VSYS_BB_FPRINTF(...)    FATAL_PRINTF("BB>> "##__VA_ARGS__)
#define VSYS_BB_CPRINTF(...)    CRITICAL_PRINTF("BB>> "##__VA_ARGS__)
#define VSYS_BB_EPRINTF(...)    ERROR_PRINTF("BB>> "##__VA_ARGS__)
#define VSYS_BB_WPRINTF(...)    WARNING_PRINTF("BB>> "##__VA_ARGS__)
#define VSYS_BB_NPRINTF(...)    NOTICE_PRINTF("BB>> "##__VA_ARGS__)
#else
#define VSYS_BB_FPRINTF(...)
#define VSYS_BB_CPRINTF(...)
#define VSYS_BB_EPRINTF(...)
#define VSYS_BB_WPRINTF(...)
#define VSYS_BB_NPRINTF(...)
#endif

#if (VAPP_AUDIOTRANS_DBG_EN > 0)
#define AUDIOTRANS_FPRINTF(...)     FATAL_PRINTF("AUTRANS>> "##__VA_ARGS__)
#define AUDIOTRANS_CPRINTF(...)     CRITICAL_PRINTF("AUTRANS>> "##__VA_ARGS__)
#define AUDIOTRANS_EPRINTF(...)     ERROR_PRINTF("AUTRANS>> "##__VA_ARGS__)
#define AUDIOTRANS_WPRINTF(...)     WARNING_PRINTF("AUTRANS>> "##__VA_ARGS__)
#define AUDIOTRANS_NPRINTF(...)     NOTICE_PRINTF("AUTRANS>> "##__VA_ARGS__)
#else
#define AUDIOTRANS_FPRINTF(...)
#define AUDIOTRANS_CPRINTF(...)
#define AUDIOTRANS_EPRINTF(...)
#define AUDIOTRANS_WPRINTF(...)
#define AUDIOTRANS_NPRINTF(...)
#endif


#if (VAPP_TWS_DBG_EN > 0)
#define TWS_FPRINTF(...)        FATAL_PRINTF("TWS>> "##__VA_ARGS__)
#define TWS_CPRINTF(...)        CRITICAL_PRINTF("TWS>> "##__VA_ARGS__)
#define TWS_EPRINTF(...)        ERROR_PRINTF("TWS>> "##__VA_ARGS__)
#define TWS_WPRINTF(...)        WARNING_PRINTF("TWS>> "##__VA_ARGS__)
#define TWS_NPRINTF(...)        NOTICE_PRINTF("TWS>> "##__VA_ARGS__)
#else
#define TWS_FPRINTF(...)
#define TWS_CPRINTF(...)
#define TWS_EPRINTF(...)
#define TWS_WPRINTF(...)
#define TWS_NPRINTF(...)
#endif


#if (VAPP_A2DPBROADCAST_DBG_EN > 0)
#define A2DPBROADCAST_FPRINTF(...)      FATAL_PRINTF("BROAD>> "##__VA_ARGS__)
#define A2DPBROADCAST_CPRINTF(...)      CRITICAL_PRINTF("BROAD>> "##__VA_ARGS__)
#define A2DPBROADCAST_EPRINTF(...)      ERROR_PRINTF("BROAD>> "##__VA_ARGS__)
#define A2DPBROADCAST_WPRINTF(...)      WARNING_PRINTF("BROAD>> "##__VA_ARGS__)
#define A2DPBROADCAST_NPRINTF(...)      NOTICE_PRINTF("BROAD>> "##__VA_ARGS__)
#else
#define A2DPBROADCAST_FPRINTF(...)
#define A2DPBROADCAST_CPRINTF(...)
#define A2DPBROADCAST_EPRINTF(...)
#define A2DPBROADCAST_WPRINTF(...)
#define A2DPBROADCAST_NPRINTF(...)
#endif

#if (VAPP_BLE_DBG_EN > 0)
#define VAPP_BLE_FPRINTF(...)   FATAL_PRINTF("LEAPP>> "##__VA_ARGS__)
#define VAPP_BLE_CPRINTF(...)   CRITICAL_PRINTF("LEAPP>> "##__VA_ARGS__)
#define VAPP_BLE_EPRINTF(...)   ERROR_PRINTF("LEAPP>> "##__VA_ARGS__)
#define VAPP_BLE_WPRINTF(...)   WARNING_PRINTF("LEAPP>> "##__VA_ARGS__)
#define VAPP_BLE_NPRINTF(...)   NOTICE_PRINTF("LEAPP>> "##__VA_ARGS__)
#else
#define VAPP_BLE_FPRINTF(...)
#define VAPP_BLE_CPRINTF(...)
#define VAPP_BLE_EPRINTF(...)
#define VAPP_BLE_WPRINTF(...)
#define VAPP_BLE_NPRINTF(...)
#endif

#if 1
#define TWS_PERFORMANCE_DEBUG 0 
#if (TWS_PERFORMANCE_DEBUG > 0)
#define TWS_PERFORMANCE_DEBUG_PRINTF NOTICE_PRINTF
#else
#define TWS_PERFORMANCE_DEBUG_PRINTF(...)
#endif
#endif



#if 1
#define TWS_BLE_DEBUG 1 
#if (TWS_BLE_DEBUG > 0)
#define TWS_BLE_DEBUG_PRINTF NOTICE_PRINTF
#else
#define TWS_BLE_DEBUG_PRINTF(...)
#endif
#endif

#if (VSYS_OTG_DBG_EN > 0)
#define VOTG_FPRINTF        FATAL_PRINTF
#define VOTG_CPRINTF        CRITICAL_PRINTF
#define VOTG_EPRINTF        ERROR_PRINTF
#define VOTG_WPRINTF        WARNING_PRINTF
#define VOTG_NPRINTF        NOTICE_PRINTF
#else
#define VOTG_FPRINTF(...)
#define VOTG_CPRINTF(...)
#define VOTG_EPRINTF(...)
#define VOTG_WPRINTF(...)
#define VOTG_NPRINTF(...)
#endif

#if (VSYS_SD_DBG_EN > 0)
#define VSD_FPRINTF         FATAL_PRINTF
#define VSD_CPRINTF         CRITICAL_PRINTF
#define VSD_EPRINTF         ERROR_PRINTF
#define VSD_WPRINTF         WARNING_PRINTF
#define VSD_NPRINTF         NOTICE_PRINTF
#else
#define VSD_FPRINTF(...)
#define VSD_CPRINTF(...)
#define VSD_EPRINTF(...)
#define VSD_WPRINTF(...)
#define VSD_NPRINTF(...)
#endif

#if (VAPP_ECSB_DBG_EN > 0)
#define VECSB_FPRINTF(...)      FATAL_PRINTF("ECSB>> "##__VA_ARGS__)
#define VECSB_CPRINTF(...)      CRITICAL_PRINTF("ECSB>> "##__VA_ARGS__)
#define VECSB_EPRINTF(...)      ERROR_PRINTF("ECSB>> "##__VA_ARGS__)
#define VECSB_WPRINTF(...)      WARNING_PRINTF("ECSB>> "##__VA_ARGS__)
#define VECSB_NPRINTF(...)      NOTICE_PRINTF("ECSB>> "##__VA_ARGS__)
#else
#define VECSB_FPRINTF(...)
#define VECSB_CPRINTF(...)
#define VECSB_EPRINTF(...)
#define VECSB_WPRINTF(...)
#define VECSB_NPRINTF(...)
#endif

#if (VAPP_AUDIOBC_DBG_EN>0)
#define VAUDIOBC_FPRINTF(...)       FATAL_PRINTF("AUDIOBC>> "##__VA_ARGS__)
#define VAUDIOBC_CPRINTF(...)       CRITICAL_PRINTF("AUDIOBC>> "##__VA_ARGS__)
#define VAUDIOBC_EPRINTF(...)       ERROR_PRINTF("AUDIOBC>> "##__VA_ARGS__)
#define VAUDIOBC_WPRINTF(...)       WARNING_PRINTF("AUDIOBC>> "##__VA_ARGS__)
#define VAUDIOBC_NPRINTF(...)       NOTICE_PRINTF("AUDIOBC>> "##__VA_ARGS__)
#else
#define VAUDIOBC_FPRINTF(...)
#define VAUDIOBC_CPRINTF(...)
#define VAUDIOBC_EPRINTF(...)
#define VAUDIOBC_WPRINTF(...)
#define VAUDIOBC_NPRINTF(...)
#endif

#if (VAPP_USBAUDIO_DBG_EN>0)
#define VUA_FPRINTF(...)        FATAL_PRINTF("UA>> "##__VA_ARGS__)
#define VUA_CPRINTF(...)        CRITICAL_PRINTF("UA>> "##__VA_ARGS__)
#define VUA_EPRINTF(...)        ERROR_PRINTF("UA>> "##__VA_ARGS__)
#define VUA_WPRINTF(...)        WARNING_PRINTF("UA>> "##__VA_ARGS__)
#define VUA_NPRINTF(...)        NOTICE_PRINTF("UA>> "##__VA_ARGS__)
#else
#define VUA_FPRINTF(...)
#define VUA_CPRINTF(...)
#define VUA_EPRINTF(...)
#define VUA_WPRINTF(...)
#define VUA_NPRINTF(...)
#endif


#if (VHDS_DBG_EN > 0)
#define HDS_FPRINTF(...)        FATAL_PRINTF("HDS>> "##__VA_ARGS__)
#define HDS_CPRINTF(...)        CRITICAL_PRINTF("HDS>> "##__VA_ARGS__)
#define HDS_EPRINTF(...)        ERROR_PRINTF("HDS>> "##__VA_ARGS__)
#define HDS_WPRINTF(...)        WARNING_PRINTF("HDS>> "##__VA_ARGS__)
#define HDS_NPRINTF(...)        NOTICE_PRINTF("HDS>> "##__VA_ARGS__)
#else
#define HDS_FPRINTF(...)
#define HDS_CPRINTF(...)
#define HDS_EPRINTF(...)
#define HDS_WPRINTF(...)
#define HDS_NPRINTF(...)
#endif

#if (VDBG_DS_CACHE_EN > 0)
#define DS_CACHE_NPRINTF        NOTICE_PRINTF
#else
#define DS_CACHE_NPRINTF(...)
#endif

#if (VSYS_IPC_DBG_EN > 0)
#define vsys_ipc_dbg_info(fmt, ...)    NOTICE_PRINTF("[IPC] %s(%05d):" fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define vsys_ipc_dbg_info(...)
#endif


#if (VCOS_ERR_DBG_EN > 0)
#define VCOS_ERR_EPRINTF(func,name,expr) \
    if(expr) { \
        vcom_printf("VCOS>> FUNC:%s, EVENT:%s, ERROR:(%d)\r\n",func,name,expr);\
    }
#else
#define VCOS_ERR_EPRINTF(...)
#endif

#if (VAPP_BLE_DBG_EN > 0)
#define vmcc_ble_dbg_info(fmt, ...)    NOTICE_PRINTF("BLE:BT%s(%05d):" fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define vmcc_ble_dbg_info(...)
#endif

#if 1
#define VMCC_NPRINTF            NOTICE_PRINTF

#else
#define vmcc_dsp_dbg(...)
#endif

#if 1
#define GFPS_RTT_DEBUG 1
#if (GFPS_RTT_DEBUG > 0)
#define GFPS_RTT_PRINTF(fmt, ...)    NOTICE_PRINTF("[GFPS] %s(%05d):" fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define GFPS_RTT_PRINTF(fmt, ...)
#endif
#endif

#ifdef FPGA_QA_BUILD
#if (VDBG_EN != 0)
//#error "vdebug.h, VDBG_EN should be set to 0 for FPGA QA Build!"
#endif
#endif

#ifdef RELEASE_BUILD
#if (VDBG_EN != 0)
//#error "vdebug.h, VDBG_EN should be set to 0 for Release Build!"
#endif
#endif


#ifdef __cplusplus
}
#endif 

#endif  /* _VDEBUG_H_ */

/* _VDEBUG_H_ */

