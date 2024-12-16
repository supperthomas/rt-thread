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
 	2018.05.3             tkm             		 initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"     
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_ipc.h"

#if (VDBG_EN > 0) && (VDBG_IO_RETARGET == IO_RTT)

#include "SEGGER_RTT.h"

void VDBG_DrvInit(void) {

	static UINT8 workon = 0;
	if(workon==0)
	{
		workon = 1;
#if (RTT_HIGH_SPEED_SYNC_DBG_EN >0)
	  // set the buffer run in block mode
	  SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#else
	// set the buffer run in nonblock mode
	  SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif //#if (RTT_HIGH_SPEED_SYNC_DBG_EN >0)

#ifdef CORE_SEL_MCUTOP
	VIPC_SetRTTInfoValue_TOP(&_SEGGER_RTT) ;
#else
	VIPC_SetRTTInfoValue_BT(&_SEGGER_RTT) ;
#endif

#if (VDBG_IO_SHARE_RETARGET)		
	#ifdef CORE_SEL_MCUTOP
		SEGGER_RTT_TerminalOut(1,RTT_CTRL_RESET);
		SEGGER_RTT_TerminalOut(1,RTT_CTRL_CLEAR);
		SEGGER_RTT_TerminalOut(1,RTT_CTRL_TEXT_BRIGHT_RED);
	#else
		SEGGER_RTT_TerminalOut(0,RTT_CTRL_RESET);
		SEGGER_RTT_TerminalOut(0,RTT_CTRL_CLEAR);
		SEGGER_RTT_TerminalOut(0,RTT_CTRL_TEXT_WHITE);
	#endif
#endif
		vputs("vdbg init\r\n");
	}
}

#if (VDBG_IO_SHARE_RETARGET)
#if (CORE_SEL_MCUTOP)
void vputs(char *str) {
	VIPC_SEGGER_RTT_mutex_lock();
  SEGGER_RTT_TerminalOut(1,str);
	VIPC_SEGGER_RTT_mutex_unlock();
}
void vputs_rtt(char *buf,unsigned int len) {
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(1);
  SEGGER_RTT_Write(0, buf,len);
	SEGGER_RTT_SetTerminal(0);
	VIPC_SEGGER_RTT_mutex_unlock();
}
 
int vfputc(int ch) 
{
	VCPU_SR_ALLOC();
  VCPU_CRITICAL_ENTER();  
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(1);	
	SEGGER_RTT_Write(0,&ch,1);
	SEGGER_RTT_SetTerminal(0);
	VIPC_SEGGER_RTT_mutex_unlock();
	VCPU_CRITICAL_EXIT();
	
	return(ch);
}

int vfgetc(void) 
{
	int ch = 0;
	ch = SEGGER_RTT_WaitKey();
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(1);
	SEGGER_RTT_Write(0,&ch,1);
	SEGGER_RTT_SetTerminal(0); 
	VIPC_SEGGER_RTT_mutex_unlock();
	return (ch);
}

void _ttywrch(int ch) 
{
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(1);
	SEGGER_RTT_Write(0,&ch,1);
	SEGGER_RTT_SetTerminal(0);
	VIPC_SEGGER_RTT_mutex_unlock();
}
#else
void vputs(char *str) {
	VIPC_SEGGER_RTT_mutex_lock();
  SEGGER_RTT_TerminalOut(0,str);
	VIPC_SEGGER_RTT_mutex_unlock();
}
void vputs_rtt(char *buf,unsigned int len) {
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(0);
  SEGGER_RTT_Write(0, buf,len);
	VIPC_SEGGER_RTT_mutex_unlock();
}
 
int vfputc(int ch) 
{
	VCPU_SR_ALLOC();
  VCPU_CRITICAL_ENTER();
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(0);	
	SEGGER_RTT_Write(0,&ch,1);
	VIPC_SEGGER_RTT_mutex_unlock();
	VCPU_CRITICAL_EXIT();
	
	return(ch);
}

int vfgetc(void) 
{
	int ch = 0;
	ch = SEGGER_RTT_WaitKey();
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_Write(0,&ch,1);
	VIPC_SEGGER_RTT_mutex_unlock();
	return (ch);
}

void _ttywrch(int ch) 
{
	VIPC_SEGGER_RTT_mutex_lock();
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_Write(0,&ch,1);
	VIPC_SEGGER_RTT_mutex_unlock();
}
#endif

#else

int vfputc(int ch) 
{
	VDBG_DrvInit();
	SEGGER_RTT_Write(0,&ch,1);
	return(ch);
}

int vfgetc(void) 
{
	int ch = 0;
	ch = SEGGER_RTT_WaitKey();
	SEGGER_RTT_Write(0,&ch,1);
	return (ch);
}

void vputs(char *str) {
	VDBG_DrvInit();
	SEGGER_RTT_WriteString(0, str);
}

void vputs_rtt(char *buf,unsigned int len) {
  SEGGER_RTT_Write(0, buf,len);
}  

void _ttywrch(int ch) 
{
	SEGGER_RTT_Write(0,&ch,1);
}

#endif


void vputs_rtt_data(char *buf,unsigned int len) {
  SEGGER_RTT_Write(1, buf,len);
}

void vputs_rtt_audio(char *buf,unsigned int len) {
  SEGGER_RTT_Write(3, buf,len);
}

#if (!(CORE_SEL_MCUTOP)) 
void RTT_Crash_data_output(unsigned int return_addr , void *p_StkPtr)
{
#if (VDBG_DATA_SHARE_RETARGET)
	char ch = 0;
	unsigned int i = 0;
	char *cmd = "9648",*ack = "ok";
RTT_Crash_data_tag:
	while(SEGGER_RTT_HasData(0)) SEGGER_RTT_WaitKey();
	do{
		while(SEGGER_RTT_HasDataUp(0));
		vputs("Crash problem has been found, please enter \"9648\" to debug\n");
		while(((++i)&(0x03FFFFFF)) && (!SEGGER_RTT_HasData(0)));
	}while(!SEGGER_RTT_HasData(0));i = 0;ch = 0;	
	while(cmd[i] && (ch != 'q')){
		ch = SEGGER_RTT_WaitKey();
		if(ch == cmd[i]) i++;
		else i = 0;
	}i = 0;
	if(ch != 'q') ch = 'm';
	do{switch(ch){
			case '1':{
					vputs("\nPlease connect to RTT logger terminal to set channel 1 and save the file. Enter \"ok\" to start uploading\n");
					while(ack[i] && (ch != 'q')){
						ch = SEGGER_RTT_WaitKey();
						if(ch == ack[i]) i++;
						else i = 0;
					}i = 0;if(ch == 'q'){vputs("quit!\n");break;}
					vputs("Uploading data ...\n");
					vputs_rtt_data((char*)0,800*1024);
					vputs("Data upload completed, please save the file!\n");
				}break;
			case '2':{
					vputs("\n-----Output memory pool related information-----\n");
					VSYS_MEM_CheckMalloc_ERR(1);
				}break;
			case '3':{
					vcom_printf("Fault return[0x%08X]\np_StkPtr:0x%08X StkPtr_return:0x%08X\n",return_addr,p_StkPtr,*((UINT32 *)p_StkPtr+5));
				}break; 
			case '4':{
					vputs("\nRead data[r0xaddr number] - Write data[w0xaddr 0xdata] the data is 4 bytes\n");
					do{
						char mode = 0,j = 0;
						unsigned int addr = 0,data = 0;
						while(SEGGER_RTT_HasData(0)) SEGGER_RTT_WaitKey();
						mode = SEGGER_RTT_WaitKey(); if(mode == 'q') break;	
						for(j = 0;j < 2;j++){
							if((mode != 'r') || (mode != 'w')) break;
							if(!(j && (mode == 'r'))) if(SEGGER_RTT_WaitKey() != '0' || SEGGER_RTT_WaitKey() != 'x') break;
							for(i = 0;i < 8;i++){
								ch = SEGGER_RTT_WaitKey();
								if((ch == ' ') || (ch == '\n')){i = 10;break;}
								if(j && (mode == 'r')){
									if(!((ch >= '0') && (ch <= '9'))) break;
									data *= 10;
								}else{
									if(!(((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')) || ((ch >= 'a') && (ch <= 'f')))) break;
									data <<= 4;
								}
								if(ch <= '9')
									data += ch-'0';	
								else if(ch >= 'a')
									data += ch-'a';	
								else
									data += ch-'A';	
							}
							if(!j){addr = data;data = 0;
							if(((i == 8) && (SEGGER_RTT_WaitKey() != ' ')) || ((i != 8) && (i != 10)))break;}	
						}if(j != 2){vputs("Input error!\n");continue;}
						if(mode == 'r'){
							for(i = 0;i < data;i++){
								if(i%2) vputs("\n"); 
								vcom_printf("[0x%08X]0x%08X\t",addr+4*i,*((unsigned int *)addr + i));
							}vputs("\n"); 
						}else if(mode == 'w'){
							*(unsigned int *)addr = data;
							vputs("success!\n");
						}
					}while(1);i = 0;
					vputs("quit!\n");
				}break;
			case '5':{
					extern char _cacheUpBuffer[];
					unsigned short WrOff;	
					unsigned short RdOff;	
					unsigned SizeOfBuffer;
					unsigned NumBytesToWrite;
					vputs("\t----- cache buff log START-----\n");
					SizeOfBuffer = CACHE_BUFFER_SIZE_UP - sizeof(unsigned short);
					RdOff = WrOff  = *(unsigned short *)((unsigned)_cacheUpBuffer + SizeOfBuffer);
					NumBytesToWrite = SizeOfBuffer - RdOff;
					do{
						while(NumBytesToWrite > 1024){
							while(SEGGER_RTT_HasDataUp(0));
							vputs_rtt(_cacheUpBuffer + RdOff,1024);
							RdOff += 1024;
							NumBytesToWrite -= 1024;		
						}
						while(SEGGER_RTT_HasDataUp(0));
						vputs_rtt(_cacheUpBuffer + RdOff,NumBytesToWrite);
						RdOff += NumBytesToWrite;
						if(RdOff >= SizeOfBuffer) RdOff = 0;
						NumBytesToWrite = WrOff;
					}while(RdOff != WrOff);
					vputs("\t----- cache buff log END-----\n");
				}break;
			case 'c':{
					vputs(RTT_CTRL_CLEAR);	
				}break;
			case 'q':{
					vputs("Exit command line\n");
					goto RTT_Crash_data_tag;
				}break;
			case 'm':{
					vputs("Please enter the command to debug\n\t[1] Print data in cache\n\t[2] Print memory information\n\t[3] Print fault return address\n\
\t[4] Get setting address data\n\t[5] Print cache buff log\n\t[c] Clear screen\n\t[m] Print command information\n\t[q] Exit command line\n");
				}break;
			default :
				break;
		}
		while(SEGGER_RTT_HasDataUp(0));
		ch = SEGGER_RTT_WaitKey();
	}while(1);
	vputs("\r\n Press any key to quit!\n");
	SEGGER_RTT_WaitKey();
	goto RTT_Crash_data_tag;
#endif
}
#endif

#endif
