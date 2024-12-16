/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    main

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstdos.h"
#include "vstddrv.h"
#include "vos_ipc.h"

#define Dbg_PRINT        vcom_printf

#define  APP_TASK_STK_SIZE                   1024u
VOSTcb_t APP_1_TaskTCB;
VOSTcb_t APP_2_TaskTCB;
VOSTcb_t APP_3_TaskTCB;
VOSTcb_t APP_4_TaskTCB;
#define  APP_1_TASK_PRIO                        4u
#define  APP_2_TASK_PRIO                        3u
#define  APP_3_TASK_PRIO                        5u
#define  APP_4_TASK_PRIO                        6u

VOSQueue_t queue;
VOSMutex_t mutex;
VOSSemap_t sem;
VOSEvent_t event;
VOSTimer_t timer;

ipc_que_t *ipc_que = NULL;
ipc_val_t *ipc_sem = NULL;
ipc_val_t *ipc_lock = NULL;
ipc_val_t *ipc_evt = NULL;

VOSQueue_t ipc_que1 = NULL;
VOSSemap_t ipc_sem1 = NULL;
VOSMutex_t ipc_lock1 = NULL;
VOSEvent_t ipc_evt1 = NULL;
VOSQueue_t ipc_que2 = NULL;

VOSMulti_t multi_handle = NULL;

volatile UINT32 run_flag = 0;

__IRQ void VISR_IPC(void)
{
	VIPC_Interrupt_Routine_Service();
}


void timer_callback_test(void *timer, void* arg)
{
    static UINT8 num = 0;
	SINT8 msg[20], *msgret = NULL;;
    vcom_sprintf(msg,"%d %s\r\n",num++,(char*)arg);
	VOSQueueSend(queue,msg,20);
	if(num%2 == 0){
		vcom_sprintf(msg,"ipc msg %p",msg);
		msgret = VOSQueueRequest(ipc_que1,msg,vstrlen(msg),10);
		if(msgret){
			Dbg_PRINT("c%d revipc:%s\r\n",VCORE_GetID(),msgret);
			vfree(msgret);
		}
	}
}


#define IPC_NAME "2demo"

static  void  APP_1_Task (void *p_arg)
{// top bt test
    (void)p_arg;
	SINT8 *msg = NULL;
	UINT32 value = 1, ret = 0;
	
	ipc_que1 = VOSQueueCreate(IPC_NAME,5);
	ipc_sem1 = VOSSemapCreate(IPC_NAME,0);
	ipc_lock1 = VOSMutexCreate(IPC_NAME);
	ipc_evt1 = VOSFlagCreate(IPC_NAME,0);
	ipc_que2 = VOSQueueCreate( "2demo2",5);
	
	multi_handle = VOSMultiCreate("Multi", 4, ipc_que1, ipc_que2, queue, ipc_evt1);
	
	if(VCORE_GetID() == CPU_ID_TOP){
		
	
	}
	
    while (1) 
    {
		UINT32 ret_id = 0;
		ret = VOSMultiWait(multi_handle, 5000);
		switch(ret)
		{
		case 3:
			value = VOSFlagWait(ipc_evt1,0xFFF,VOSFLAG_ANY_CONSUME,0);
			if(value == 0) break;
			VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
			Dbg_PRINT("c%d val:%d\r\n",VCORE_GetID(),value);
			vdelayus(2);
			VOSMutexUnlock(ipc_lock1);
			VOSSemapGive(ipc_sem1);
			break;
		case 0:
			msg = VOSQueueReplyReceive(ipc_que1,&ret_id,0);
			if(msg == NULL) break;
			VOSQueueResponse(ret_id,msg+2,18);
			VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
			Dbg_PRINT("c%d call rev:%x %s\r\n",VCORE_GetID(),*(UINT32*)msg, msg);
			vdelayus(2);
			VOSMutexUnlock(ipc_lock1);
			if(msg) vfree(msg);
			break;
		case 1:
			msg = VOSQueueReceive(ipc_que2,0);
			if(msg == NULL) break;
			VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
			Dbg_PRINT("c%d rev:%s\r\n",VCORE_GetID(),msg);
			vdelayus(2);
			VOSMutexUnlock(ipc_lock1);
			if(msg) vfree(msg);
			break;
		case 2:
			msg = VOSQueueReceive(queue,0);
			if(msg == NULL) break;
			VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
			Dbg_PRINT("os rev:%s\r\n",msg);
			vdelayus(2);
			VOSMutexUnlock(ipc_lock1);
			if(msg) vfree(msg);
			break;			
		default:
			VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
			Dbg_PRINT("c%d rev timeout\r\n",VCORE_GetID());
			vdelayus(2);
			VOSMutexUnlock(ipc_lock1);
			break;	
		}
    }
}

static  void  APP_4_Task (void *p_arg)
{// top bt test
    (void)p_arg;
	SINT8 *msg = NULL;
	UINT32 value = 1, ret = 0;
	
	ipc_que1 = VOSQueueCreate(IPC_NAME,5);
	ipc_sem1 = VOSSemapCreate(IPC_NAME,0);
	ipc_lock1 = VOSMutexCreate(IPC_NAME);
	ipc_evt1 = VOSFlagCreate(IPC_NAME,0);
	ipc_que2 = VOSQueueCreate( "2demo2",5);
	
	while(1){
        VOSFlagSet(ipc_evt1,value++);
		ret = VOSSemapTake(ipc_sem1, 5000);
        if(ret != VRET_NO_ERROR){
            Dbg_PRINT("c%d VOSSemapTake err:0x%x\r\n",VCORE_GetID(),ret);
			vdelayus(2);
			while(1){}
        }

		msg = vmalloc(20);
		vcom_sprintf(msg,"%s%d","calltest",value);
		UINT32 start_cycle = vtimeusget(),end_cycle = 0;
		msg = VOSQueueRequest(ipc_que1,msg,20,1);
		end_cycle = vtimeusget();
		VOSMutexLock(ipc_lock1, IPCMAX_DELAY);
		Dbg_PRINT("time %d us\r\n", vtimeusinterval(start_cycle,end_cycle));
		Dbg_PRINT("c%d cb:%s\r\n",VCORE_GetID(),msg);
		vdelayus(2);
		VOSMutexUnlock(ipc_lock1);
		if(msg) vfree(msg);
		
		msg = vmalloc(20);
        vcom_sprintf(msg,"c%d, ipc msg1 %d",VCORE_GetID(),value);
        ret = VOSQueueSend(ipc_que2,msg,20);
		if(ret){
			Dbg_PRINT("c%d err:0x%x\r\n",VCORE_GetID(),ret);
			vdelayus(2);
			if(msg) vfree(msg);
		}
		
		msg = vmalloc(20);
        vcom_sprintf(msg,"c%d, ipc msg2 %d",VCORE_GetID(),value);
        ret = VOSQueueSend(ipc_que2,msg,20);
		if(ret){
			Dbg_PRINT("c%d err:0x%x\r\n",VCORE_GetID(),ret);
			vdelayus(2);
			if(msg) vfree(msg);
		}
	}
}

static  void  APP_2_Task (void *p_arg)
{
	SINT8 retmsg[20];
	SINT8 *msg = NULL;
	while(1){
		VOSTimeDly(1000);
		vcom_sprintf(retmsg,"affdadfad\r\n");
		msg = VOSQueueRequest(ipc_que1,retmsg,vstrlen(retmsg),10);
		if(msg){
			Dbg_PRINT("c%d revipc:%s\r\n",VCORE_GetID(),msg);
			vfree(msg);
		}
	}
}



int main ( void )
{
#ifdef CORE_SEL_MCUTOP
	VIPC_Info_Get()->magic = 0;
	VSYS_DebugPort_Enable(TRUE);
	VSYS_CKM_SetParam(VSYS_BTClockEnable,FALSE);
	VSYS_CKM_SetParam(VSYS_BTClockEnable,TRUE);
#endif
    VCPU_CoreInit(); 
	VSYS_CKM_Init();

    if (VOSInit()) {
        while (1);
    }   
	Dbg_PRINT("OSipc test...\r\n");
	queue = VOSQueueCreate("2ostest",5);

#ifdef CORE_SEL_MCUTOP
    APP_1_TaskTCB = VOSTaskCreate("APP_1_Task",APP_1_Task,0u,APP_1_TASK_PRIO,APP_TASK_STK_SIZE);
    if (APP_1_TaskTCB == NULL) {
        while (1);
    }
//	timer = VOSTimerCreate("test tmer",5000,2000,timer_callback_test,"good!");
//    VOSTimerStart(timer);
#else
	APP_4_TaskTCB = VOSTaskCreate("APP_4_Task",APP_4_Task,0u,APP_4_TASK_PRIO,APP_TASK_STK_SIZE);    
    if (APP_4_TaskTCB == NULL) {
        while (1);
    }
#endif
	
    APP_2_TaskTCB = VOSTaskCreate("APP_2_Task",APP_2_Task,0u,APP_2_TASK_PRIO,APP_TASK_STK_SIZE);    
    if (APP_2_TaskTCB == NULL) {
        while (1);
    }

    VOSStart();  
}

