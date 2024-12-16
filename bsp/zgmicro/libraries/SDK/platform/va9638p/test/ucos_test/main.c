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

#define Dbg_PRINT        vcom_printf


#define  APP_TASK_STK_SIZE                   1024u
VOSTcb_t APP_1_TaskTCB;
VOSTcb_t APP_2_TaskTCB;
VOSTcb_t APP_3_TaskTCB;
#define  APP_1_TASK_PRIO                        3u
#define  APP_2_TASK_PRIO                        10u
#define  APP_3_TASK_PRIO                        5u

VOSQueue_t queue;
VOSMutex_t mutex;
VOSSemap_t sem;
VOSEvent_t event;
VOSTimer_t timer;

void timer_callback_test(void *timer, void* arg)
{
    static UINT8 num = 0;
    Dbg_PRINT("%d %s\r\n",num++,(char*)arg);
}

static  void  APP_1_Task (void *p_arg)
{
   (void)p_arg;
	VSYS_SYSVIEW_MarkInit(1,"os delay");
	VSYS_SYSVIEW_MarkInit(3,"os send");
	VOSHooksRegister(VOSHOOK_TYPE_STATTASK, NULL);
    while (1) 
    {
        VOSMutexLock(mutex,VOSMAX_DELAY);
        Dbg_PRINT("I am in task1 %d errno%x %d\r\n",VOSTimerRemainGet(timer),__VERRNO, __VERRLINE);
        __VERRNO_STORE(__VERRNO+1);
        VOSMutexUnlock(mutex);
		VSYS_SYSVIEW_MarkStart(1);
		VSYS_SYSVIEW_MarkStart(2);
        VOSTimeDly(1000);
		VSYS_SYSVIEW_MarkStop(2);
		VSYS_SYSVIEW_MarkStop(1);
        VOSSemapGive(sem);
        VOSSemapGive(sem);
        VOSFlagSet(event,0x80000000);
        VOSQueueSend (queue,"uC/OS-III",0);   
		VSYS_SYSVIEW_Mark(3);		
    }
}

static  void  APP_2_Task (void *p_arg)
{
    (void)p_arg;
    char *pMsg = NULL;

    timer = VOSTimerCreate("test tmer",5000,2000,timer_callback_test,"good!");
    VOSTimerStart(timer);
    //VOSTimerDynTick(3000);
    while(1)
    {
        Dbg_PRINT("I am in task2\r\n");
        VOSMutexLock(mutex,VOSMAX_DELAY);
        pMsg = VOSQueueReceive (queue,VOSMAX_DELAY);
        if ( pMsg){
          VOSEventFlags_t flag = VOSFlagWait(event,0x80000000,VOSFLAG_ALL_CONSUME,VOSMAX_DELAY);
          VOSEventFlags_t flag_t = VOSFlagWait(event,0xFFFFFFFF,VOSFLAG_ANY_PEEK,VOSMAX_DELAY);
          VOSSemapTake(sem,VOSMAX_DELAY);
          VOSSemapCtr_t cnt = VOSSemapCtr(sem);
          Dbg_PRINT ("pMsg:%s flag:0x%x [0x%x] cnt:%d err%d\r\n", pMsg, flag,flag_t,cnt,__VERRNO);
        }
        VOSMutexUnlock(mutex);
    }
}

static  void  APP_3_Task (void *p_arg)
{
    (void)p_arg;
    char *pMsg = NULL;
    VOSMulti_t os_multi = NULL;
    VOSMultiNbr_t num = 0;

    os_multi = VOSMultiCreate("multi tset",2,queue,sem);
    while(1)
    {
        Dbg_PRINT("I am in task3\r\n");
        num = VOSMultiWait(os_multi,VOSMAX_DELAY);
		Dbg_PRINT("VOSMultiWait %d\r\n",num);
        if ((pMsg = VOSMultiParse(os_multi,queue)) != NULL){
          Dbg_PRINT ("pMsg:%s\r\n", pMsg);
        }
        if (VOSMultiParse(os_multi,sem)){
          Dbg_PRINT ("sem ok\r\n");
        }
    }
}

void mytimercallback(void* arg)
{
    //Dbg_PRINT("myttt %s\r\n",arg);
	VSYS_SYSVIEW_Mark(4);
}

int main(void)
{
    VCPU_CoreInit();
	VSYS_CKM_Init();

    if (VOSInit()) {
        while (1);
    }   
	
	Dbg_PRINT (" ucos test\r\n");

    queue = VOSQueueCreate("Queue For Test",10);
    mutex = VOSMutexCreate("Mutex For Test");
    sem = VOSSemapCreate("Sem For Test",7);
    event = VOSFlagCreate("Event For Test",0x5A5A5A5A);

    APP_1_TaskTCB = VOSTaskCreate("APP_1_Task",APP_1_Task,0u,APP_1_TASK_PRIO,APP_TASK_STK_SIZE);
    if (APP_1_TaskTCB == NULL) {
        while (1);
    }

    APP_2_TaskTCB = VOSTaskCreate("APP_2_Task",APP_2_Task,0u,APP_2_TASK_PRIO,APP_TASK_STK_SIZE);    
    if (APP_2_TaskTCB == NULL) {
        while (1);
    }

//    APP_3_TaskTCB = VOSTaskCreate("APP_3_Task",APP_3_Task,0u,APP_3_TASK_PRIO,APP_TASK_STK_SIZE);    
//    if (APP_3_TaskTCB == NULL) {
//        while (1);
//    }
    VSYS_SYSVIEW_MarkInit(4,"sys timer");
    VOSTimer_t timer = VOSTimerCreate("mytimer",12,12,(VOS_CALLBACK_TIMER)mytimercallback,"1222");
    VOSTimerStart(timer);
	
    VOSStart();  
}
