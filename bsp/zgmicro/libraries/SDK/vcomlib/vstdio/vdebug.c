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
        2018.02.13           miao.mingming           split functions to mass modules
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vstdcore.h"
#include "vpla_uart.h"

#if (VDBG_EN > 0)
#include "vdrv_uart.h"
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_COROUTINE)
#include "vcoroutine.h"
#define get_system_time_ms() vcortimegetms()
#elif (VOS_PLATFORM_SELECT_TYPE != VOS_PLATFORM_NONE)
#define get_system_time_ms() VOSTimeGet()
#else
#define get_system_time_ms() 0
#endif

#define TP_TAG          "TP "
#define BT_TAG          "BT "

#ifdef CORE_SEL_MCUTOP
#define PRINT_TAG TP_TAG
#else
#define PRINT_TAG BT_TAG
#endif

#pragma import(__use_no_semihosting)

volatile UINT32 logcount = 0;

extern void VDBG_DrvInit(void);
extern int vcom_vsprintf(char *buf, const char *fmt, va_list args);

void VDBG_Init(void) {
#if (CONFIG_DEBUG_TYPE == DEBUG_LOG_LEGACY_TYPE)
        VDBG_DrvInit();
#endif  
}


#ifdef PKGS_USING_MSH
#include "msh.h"
static volatile int output_enable = 1;      // 支持动态开关Debug信息的输出

static void logon(void)
{
    output_enable = 1;
}

static void logoff(void)
{
    output_enable = 0;
}

int log_control(int argc, char **argv)
{
    if (argc != 2) {
        goto usage;
    }

    if (strcmp(argv[1], "on") == 0) {
        logon();
    } else if (strcmp(argv[1], "off") == 0) {
        logoff();
    } else {
        goto usage;
    }

    return 0;

usage:
    MSH_PRINTF("Usage: log <on|off>\r\n");
    return -1;
}

MSH_FUNCTION_EXPORT_ALIAS(log_control, __cmd_log, enable/disable debug info output. Usage: log <on|off>);
#endif

int VDBG_IsOutputEnable(void)
{
    #ifdef PKGS_USING_MSH    
    return output_enable;
    #else
    return 1;
    #endif
}

UINT32 GET_LOGTIME(void)
{
        logcount++;
//      return OSTickCtr<<8|logcount++; 
        return logcount;
}

static SINT8 buf[512] = "\n";

/* 当开启新Logging 模块时，BT核禁用vcom_printf、vcom_dumphex_printf、v_printf。 因为BT可能不存在LOG后端 */
#if (CONFIG_DEBUG_TYPE == DEBUG_LOG_LEGACY_TYPE) || (CORE_SEL_MCUTOP == 1) 

void vcom_printf(const char *fmt, ...)
{
    if (VDBG_IsOutputEnable() == 0) {   // 支持动态开关Debug信息的输出
        return;
    }

    SINT8 time_buff[16];
	unsigned short len;
	va_list args;

	if ('\n' == buf[vstrlen(buf) - 1])
	{
		unsigned time = get_system_time_ms();
		vcom_sprintf(time_buff, "%s%d.%02d ", PRINT_TAG, time / 1000, time % 1000 / 10);
		vputs((char *)time_buff);
	}

	va_start(args, fmt);
	len = vcom_vsprintf((char *)buf, fmt, args);
	va_end(args);

	if (len > sizeof(buf))
		while (1);
	vputs((char *)buf);
}


void vcom_dumphex_printf(const unsigned char *p_data, unsigned short data_len,const char *fmt)
{
    if (VDBG_IsOutputEnable() == 0) {   // 支持动态开关Debug信息的输出
        return;
    }
    
    SINT8 time_buff[16];
	unsigned short len;
	va_list args;

	if ('\n' == buf[vstrlen(buf) - 1])
	{
		unsigned time = get_system_time_ms();
		vcom_sprintf(time_buff, "%s%d.%02d ", PRINT_TAG, time / 1000, time % 1000 / 10);
		vputs((char *)time_buff);
	}

    vmemcpy(buf, fmt, vstrlen((SINT8*)fmt));
    len = vstrlen((SINT8*)fmt);

    for (int i = 0; i < data_len; i++) {
        len += vcom_sprintf((SINT8*)&buf[len], "%02X ", p_data[i]);
        if(len >= (sizeof(buf) - 3))
        {
            break;
        }
    }
    len = MIN(len, sizeof(buf) - 3);
    buf[len] = '\r';
    buf[len+1] = '\n';
    buf[len+2] = 0;
    vputs((char *)buf);
}



void v_printf(const char *fmt, ...)
{
	unsigned short len;
	va_list args;

	va_start(args, fmt);
	len = vcom_vsprintf((char *)buf, fmt, args);
	va_end(args);

	if (len > sizeof(buf))
		while (1);
	vputs((char *)buf);
}
#endif

#if ((VDBG_IO_RETARGET != IO_UART_SIM) && VDBG_DIRECT_TO_UART_SIM)

extern void vdirect_uartinit(void);
extern int vdirect_vputc(int ch);
extern void vdirect_vputs(char *str);

#else
void vdirect_uartinit(void) {
	#if (VDBG_IO_RETARGET != IO_UART && VDBG_UART_BAUDRATE)
	static UINT8 init = 0;
	if(init == 0)
	{
		VDRVUartConfig_t uart_config = {0};
		uart_config.Baudrate = VDBG_UART_BAUDRATE;
		uart_config.port_sel = VSYS_UART_PORT_SEL_AUX;
		uart_config.Parity = VSYS_UART_PARITY_NONE;
		uart_config.StopBit = VSYS_UART_STOP_BIT1;
		uart_config.BitWidth = VSYS_UART_CHAR_BIT8;
		VSYS_UART_Init(&uart_config);
		init = 1;
	}
	#endif
}

void vdirect_vputs(char *str) 
{
    vdirect_uartinit();
	while (*str != '\0')
    {
        VUART_CharTxWithBlocking((UINT8)*str++);
    }
}

int vdirect_vputc(int ch) 
{
    vdirect_uartinit();
    VUART_CharTxWithBlocking((UINT8)ch);
	return ch;
}

#endif

void vdirect_printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vcom_vsprintf((char *)buf, fmt, args);
	va_end(args);

#if CONFIG_DEBUG_TYPE == DEBUG_LOG_NEW_TYPE
    NOTICE_PRINTF("%s",(char *)buf);
#else
    vdirect_vputs((char *)buf);
#endif
}

void _sys_exit(int return_code) {
	while (1);    /* endless loop */
} 
/************* stanard io ***********************/
#if defined __COMPILER_ARMCC__

//#ifndef __stdout
//struct __FILE { 
//      int handle; /* Add whatever you need here */ 
//};
//      
//FILE __stdout;
//FILE __stdin;
//#endif //#ifndef __stdout


/* stdio retarget to UART */
//int fputc(int ch, FILE *f) 
//{
//  return vfputc(ch);
//}

//int fgetc(FILE *f) 
//{
//  return vfgetc();
//}

void _ttywrch(int ch) 
{
  vfputc(ch);
}

int __backspace(FILE *f) {
  return (0);
}

int ferror(FILE *f) {
        /* Your implementation of ferror */
        return EOF;
}

void _sys_exit(int return_code) {
        while (1);    /* endless loop */
}
#elif (defined __COMPILER_GNUC__)

int getchar(void) {
  return (int) vfgetc();
}

int putchar(int ch) {
  return (int)vfputc(ch);
}

void _ttywrch(int ch) 
{
  vfputc(ch);
}

struct __FILE { 
        int handle; /* Add whatever you need here */ 
};
#endif // ARMCC

#else
void VDBG_Init(void) 
{
}

int vfputc(int ch)
{
        return 0;
}
void vputs(char *str)
{
}
int vfgetc(void)
{
        return 0;
}
void vcom_printf(const char *fmt, ...)
{
}
void v_printf(const char *fmt, ...)
{
}
void vdirect_vputs(char *str) 
{
}
int vdirect_vputc(int ch) 
{
	return ch;
}
void vdirect_printf(const char *fmt, ...)
{
}
#endif /* VDBG_EN */




