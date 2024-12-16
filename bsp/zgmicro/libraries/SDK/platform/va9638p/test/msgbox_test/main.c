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
#include "vpla_box.h"

#define Dbg_PRINT vcom_printf

#define APP_TASK_STK_SIZE 1024u
VOSTcb_t APP_1_TaskTCB;
VOSTcb_t APP_2_TaskTCB;

#define APP_1_TASK_PRIO 0u
#define APP_2_TASK_PRIO 1u

VOSEvent_t evt = NULL;
VOSMutex_t lock = NULL;
 
volatile UINT16 run_flag = 0;


__IRQ void VISR_IPC(void)
{
#ifdef CORE_SEL_MCUTOP
	VMBOX_INT_BT2TOP_CLR();
	if (VBOX_GetTxReq())
	{
		extern VOSEvent_t evt;
		VOSFlagSet(evt, 0x01); // vbox_queue_cacheupdate(); // TODO: execute in the task
	}
#else
	VMBOX_INT_TOP2BT_CLR();
#endif
}

#ifdef CORE_SEL_MCUTOP
static void APP_1_Task(void *p_arg)
{ 	
	while (1)
	{
		VOSFlagWait(evt,0xFFFFFFFF,VOSFLAG_ANY_CONSUME,VOSMAX_DELAY);
		VOSMutexLock(lock, VOSMAX_DELAY);
		VBOX_TxCacheUpdate();
		VOSMutexUnlock(lock);
	}
}

static void APP_2_Task(void *p_arg)
{ 	
	SINT8 *buff = NULL;
	SINT32 ret = 0;
	while (1)
	{
		VBOX_EvtSet(run_flag);
		VBOX_StateClr(0xFFFFFFFF);
		VBOX_StateSet(run_flag<<16|(run_flag+1));
		
		buff = vmalloc(17);
		vcom_sprintf(buff, "tt%d\r\n", run_flag);
		buff[12] = run_flag;
		do
		{
			VOSMutexLock(lock, VOSMAX_DELAY);
			ret = vbox_queue_send(0,buff, 17);
			VOSMutexUnlock(lock);
			if(ret != 0)
			{
				//Dbg_PRINT("send fail...\r\n");
				vdelayus(10);
			}
		}while(ret != 0);
//		while (NULL == (buff = vbox_queue_recv())){};
//		Dbg_PRINT(buff);
//		if(buff[12] != (UINT8)run_flag){
//			while(1);
//		}
//		vfree(buff);
			
//		if((UINT16)(run_flag + 1) != VBOX_EvtGet()){
//			while(1);
//		}
//		if((run_flag<<16|(run_flag+1)) + 2 != VBOX_StateGet()){
//			while(1);
//		}
//		
		run_flag++;
	}
}
#endif

int main(void)
{
#ifdef CORE_SEL_MCUTOP
	VSYS_DebugPort_Enable(TRUE);
	VSYS_CKM_SetParam(VSYS_BTClockEnable,FALSE);
	VSYS_CKM_SetParam(VSYS_BTClockEnable,TRUE);
#endif
	VCPU_CoreInit();
	VSYS_CKM_Init();
	
	VCPU_INT_Enable();
	VCPU_INT_SrcEnable(IRQ_MCUIPC_INT);

#ifdef CORE_SEL_MCUTOP
	VCPU_SysTick_Config(VOSTickToTime(1));
    VCPU_INT_SrcEnable(IRQ_SYSTICK_INT);

    if (VOSInit()) {
        while (1);
    }
	Dbg_PRINT("box test...\r\n");
	VBOX_Init();
	lock = VOSMutexCreate("box_lock");

	APP_1_TaskTCB = VOSTaskCreate("APP_1_Task",APP_1_Task,0u,APP_1_TASK_PRIO,APP_TASK_STK_SIZE);
    if (APP_1_TaskTCB == NULL) {
        while (1);
    }
	
	APP_2_TaskTCB = VOSTaskCreate("APP_2_Task",APP_2_Task,0u,APP_2_TASK_PRIO,APP_TASK_STK_SIZE);
    if (APP_2_TaskTCB == NULL) {
        while (1);
    }
	
	evt = VOSFlagCreate("ostest",0);
	
    VOSStart();
#else
	char *buff = NULL;
	VBOX_Init();
	Dbg_PRINT("box test...\r\n");
	while (1)
	{
		vdelayus(2);
		while (NULL == (buff = vbox_queue_recv())){};
		Dbg_PRINT(buff);
		if(buff[12] != (UINT8)run_flag){
			while(1);
		}
		vfree(buff);
		VBOX_EvtSet((UINT16)(VBOX_EvtGet() + 1));
		VBOX_StateClr(0xFFFFFFFF);
		VBOX_StateSet(VBOX_StateGet() + 2);
//		char *buff = vmalloc(13);
//		vcom_sprintf(buff, "tt%d\r\n", run_flag);
//		buff[12] = run_flag;
//		if(0 != vbox_queue_send(buff, 13)){
//			while(1);
//		}
		
		run_flag++;
	}
#endif
}
