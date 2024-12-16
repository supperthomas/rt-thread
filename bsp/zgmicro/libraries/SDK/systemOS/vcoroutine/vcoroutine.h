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
	croutine
Description:
	 test

Revision History:
	 Date                  Author                   Description
	2023.04.3            cxt   						create
------------------------------------------------------------------------------*/

#ifndef _VCROUTINE_H_
#define _VCROUTINE_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VCORPRIO_SUPPORT (1)
#define VCORPRIO_MEM_BASE (0x20000000)

#define VCORCPUCycleGet() __CPUCycleGet()
#define VCORMalloc(x) vmalloc(x)
#define VCORFree(x) vfree(x)
#define VCORMemcpy(x, y, z) vmemcpy(x, y, z)

	void vcorCriticalEnter(void);
	void vcorCriticalExit(void);
#define VCORCRITICAL_ENTER() vcorCriticalEnter()
#define VCORCRITICAL_EXIT() vcorCriticalExit()

#define vcorINITIAL_STATE (0)
#define vcorDELETE_STATE ((UINT32)0xFFFFFFFF)
	struct vcortcb_handle;
	typedef void (*VCORTaskFunc_t)(void *);
	typedef struct vcortcb_handle
	{
		struct vcortcb_handle *pxNext;
		VCORTaskFunc_t pxCoRoutineFunction;
		void *p_arg;
		UINT32 uxState;
		UINT32 timedelay;
		UINT32 runtime_max;
		UINT8 prio;
		UINT8 runcnt;
		BOOL StaticAlloc;
	} VCORTcb_Static, *VCORTcb_t;

	typedef struct
	{
		UINT16 length;
		UINT16 numb;
		UINT16 rd_p;
		UINT16 wr_p;
		BOOL StaticAlloc;
		UINT32 queList[1];
	} VCORQue_Static, *VCORQue_t;

#define VCORTMRSTOP_STATE (0)
#define VCORTMRRUN_STATE (1)
#define VCORTMREXE_STATE (2)
#define VCORTMRDEL_STATE (3)
#define VCORTMRRELOAD_STATE (4)
	struct vcortmr_handle;
	typedef void (*VCORTmrFunc_t)(void *);
	typedef struct vcortmr_handle
	{
		struct vcortmr_handle *pxNext;
		VCORTmrFunc_t pxCorTimerFunction;
		void *p_arg;
		UINT32 init_ms;
		UINT32 period_ms;
		UINT32 timeout_va;
		UINT8 uxState;
		BOOL StaticAlloc;
	} VCORTmr_Static, *VCORTmr_t;

	void vcortaskdel(VCORTcb_t p_cortcb);
	UINT32 vcortimegetms(void);
	UINT32 vcortimeinterval(UINT32 ms_start, UINT32 ms_end);
	VRET_VALUE vcorquereceive(VCORQue_t p_corque, UINT8 *id, UINT8 **p_void);
	VRET_VALUE vcorquepeek(VCORQue_t p_corque, UINT8 *id, UINT8 **p_void);
	VRET_VALUE vcorquesend(VCORQue_t p_corque, UINT8 id, UINT8 *p_void, size_t size);
	//----------------------------------------------
	extern VCORTcb_t pxCurrentCoRTCB;
	extern BOOL pxCurrentCoRSche;

#define VCORSTART()                   \
	switch (pxCurrentCoRTCB->uxState) \
	{                                 \
	case 0:

#define VCOREND()                                    \
	default:                                         \
		pxCurrentCoRTCB->uxState = vcorDELETE_STATE; \
		}

#define VCORSched()                            \
	do                                         \
	{                                          \
		pxCurrentCoRTCB->uxState = (__LINE__); \
		pxCurrentCoRSche = TRUE;               \
		return;                                \
	case (__LINE__):;                          \
	} while (0)

	void VCORScheduler(void);
	void VCORISRCoRSche(BOOL en);
	BOOL VCORISRCoRGetRun(void);
	VCORTcb_t VCORTaskCreate(VCORTaskFunc_t pxCoRoutineCode, void *p_arg, UINT8 uxPriority);
	VCORTcb_t VCORTaskCreateStatic(VCORTaskFunc_t pxCoRoutineCode, void *p_arg, UINT8 uxPriority, VCORTcb_Static *p_cortcb);

#define VCORTaskDel(p_cortcb)  \
	do                         \
	{                          \
		vcortaskdel(p_cortcb); \
		VCORSched();           \
	} while (0)

#define VCORMAX_DELAY 0xffffffffUL
	void VCORTimeCycles(UINT32 cycle, UINT32 maxval);

#define VCORTaskDelay(timeout)                                                        \
	do                                                                                \
	{                                                                                 \
		pxCurrentCoRTCB->timedelay = vcortimegetms();                                 \
		pxCurrentCoRTCB->uxState = (__LINE__);                                        \
		pxCurrentCoRSche = TRUE;                                                      \
	case (__LINE__):                                                                  \
		if (vcortimeinterval(pxCurrentCoRTCB->timedelay, vcortimegetms()) <= timeout) \
		{                                                                             \
			return;                                                                   \
		}                                                                             \
	} while (0)

#define VCORWaitContent(flag, timeout)                                                    \
	do                                                                                    \
	{                                                                                     \
		pxCurrentCoRTCB->uxState = (__LINE__);                                            \
		pxCurrentCoRSche = TRUE;                                                          \
	case (__LINE__):                                                                      \
		if (!(flag) && timeout)                                                           \
		{                                                                                 \
			if (timeout == VCORMAX_DELAY ||                                               \
				vcortimeinterval(pxCurrentCoRTCB->timedelay, vcortimegetms()) <= timeout) \
			{                                                                             \
				return;                                                                   \
			}                                                                             \
		}                                                                                 \
	} while (0)

	//----------------------------------------------
	VCORQue_t VCORQueCreateStatic(VCORQue_Static *handle, UINT16 numb);
	VCORQue_t VCORQueCreate(UINT16 numb);
	void VCORQueDel(VCORQue_t p_corque);

#define VCORQueReceive(p_corque, id, p_void, timeout)                                     \
	do                                                                                    \
	{                                                                                     \
		pxCurrentCoRTCB->timedelay = vcortimegetms();                                     \
		pxCurrentCoRTCB->uxState = (__LINE__);                                            \
		pxCurrentCoRSche = TRUE;                                                          \
	case (__LINE__):                                                                      \
		if (VRET_NO_ERROR != vcorquereceive(p_corque, id, p_void) && timeout)             \
		{                                                                                 \
			if (timeout == VCORMAX_DELAY ||                                               \
				vcortimeinterval(pxCurrentCoRTCB->timedelay, vcortimegetms()) <= timeout) \
			{                                                                             \
				return;                                                                   \
			}                                                                             \
		}                                                                                 \
	} while (0)

#define VCORQueSend(p_corque, id, p_void, size, ret)   \
	do                                                 \
	{                                                  \
		pxCurrentCoRTCB->uxState = (__LINE__);         \
		pxCurrentCoRSche = TRUE;                       \
		ret = vcorquesend(p_corque, id, p_void, size); \
		if (ret == VRET_NO_ERROR)                      \
		{                                              \
			return;                                    \
		case (__LINE__):                               \
			ret = VRET_NO_ERROR;                       \
		}                                              \
	} while (0)

	//----------------------------------------------
	VCORTmr_t VCORTmrCreateStatic(UINT32 init_ms, UINT32 period_ms, VCORTmrFunc_t fun, void *p_arg, VCORTmr_Static *handle);
	VCORTmr_t VCORTmrCreate(UINT32 init_ms, UINT32 period_ms, VCORTmrFunc_t fun, void *p_arg);
	void VCORTmrDel(VCORTmr_t p_cortmr);
	UINT32 VCORTmrStart(VCORTmr_t p_cortmr);
	UINT8 VCORTmrStop(VCORTmr_t p_cortmr);

#ifdef __cplusplus
}
#endif

#endif /* _VCROUTINE_H_ */
