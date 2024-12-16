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
#include "vstddrv.h"
#include "vcoroutine.h"

#define Dbg_PRINT vcom_printf

UINT32 flag = 0;
VCORTcb_t corTask = NULL;
VCORQue_t corQueue = NULL;
VCORTmr_t corTmr = NULL;

static void timerfun(void *arg)
{
	static UINT8 num = 0;
	num++;
	Dbg_PRINT("%d %s\r\n", num, arg);
	if(num == 9)
	{
		VCORTmrStop(corTmr);
		VCORTmrDel(corTmr);
	}
}

static void APP_1_Task(void *p_arg)
{
	static UINT8 i = 5;
    VCORSTART();
    while (1)
    {
        Dbg_PRINT("%s %s\r\n", __FUNC__, p_arg);
        VCORWaitContent(i,VCORMAX_DELAY);
		i--;
    }
    VCOREND();
}

static void APP_2_Task(void *p_arg)
{
    VCORSTART();
    while (1)
    {
        Dbg_PRINT("%s %s\r\n", __FUNC__, p_arg);
        VCORWaitContent((flag&0xF) == 2,VCORMAX_DELAY);
		flag = 0;
		VCORTaskDel(corTask);
		corTmr = VCORTmrCreate(3000,1000,timerfun,"MyTimer");
		VCORTmrStart(corTmr);
    }
    VCOREND();
}

static void APP_3_Task(void *p_arg)
{
	static UINT8 *addr = 0;
    VCORSTART();
    while (1)
    {
		VCORQueReceive(corQueue,NULL,&addr,VCORMAX_DELAY);
        Dbg_PRINT("%s %p\r\n", __FUNC__, addr);
		flag++;
        
    }
    VCOREND();
}

static void APP_4_Task(void *p_arg)
{
	static UINT32 ret = 0;
    VCORSTART();
    while (1)
    {
        Dbg_PRINT("%s %s\r\n", __FUNC__, p_arg);
        VCORQueSend(corQueue,5,(UINT8*)0x6788,0,ret);
    }
    VCOREND();
}

int main(void)
{
    VCPU_CoreInit();
    VSYS_CKM_Init();

    Dbg_PRINT(" croutine test\r\n");
	VCORTimeCycles(VSYS_CKM_GetParam(VSYS_SysClockFreq),0xFFFFFFFF);
	
	corQueue = VCORQueCreate(5);

    VCORTaskCreate(APP_1_Task, "1", 1);
    VCORTaskCreate(APP_2_Task, "2", 1);
    VCORTaskCreate(APP_3_Task, "3", 0);
	corTask = VCORTaskCreate(APP_4_Task, "4", 1);

    for (;;)
    {
        VCORScheduler();
    }
}
