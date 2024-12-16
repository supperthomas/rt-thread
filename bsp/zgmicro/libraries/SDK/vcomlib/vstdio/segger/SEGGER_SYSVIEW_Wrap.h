
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __SEGGER_SYSVIEW_WRAP_H__
#define __SEGGER_SYSVIEW_WRAP_H__

#include "vstdlib.h"
#if VSYS_SYSVIEW_SUPPORTED
    #define SYSVIEW_CFG_TRACE_HEAP_EN   0
    #define SYSVIEW_CFG_TRACE_MARK_EN   0
    #if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
        #define UCOS_CFG_TRACE_TASK_EN 1
		#define UCOS_CFG_TRACE_QESM_EN 0
		#define UCOS_CFG_TRACE_API_ENTER_EN 0
		#define UCOS_CFG_TRACE_API_EXIT_EN 0
		#define UCOS_CFG_TRACE_ISR_EN 1
    #elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
		#define FREERTOS_CFG_TRACE_TASK_EN 1
		#define FREERTOS_CFG_TRACE_QUEUE_EN 0
		#define FREERTOS_CFG_TRACE_STREAM_EN 0
		#define FREERTOS_CFG_TRACE_NOTIFY_EN 0
		#define FREERTOS_CFG_TRACE_ISR_EN 1
    #endif
#endif

#include "SEGGER_SYSVIEW_Conf.h"
#include "SEGGER_SYSVIEW.h"

#if VSYS_SYSVIEW_SUPPORTED
#define  SYSVIEW_TRACE_INIT()                             SEGGER_SYSVIEW_Conf()
#define  SYSVIEW_TRACE_START()                            SEGGER_SYSVIEW_Start()
#define  SYSVIEW_TRACE_STOP()                             SEGGER_SYSVIEW_Stop()
#else
#define  SYSVIEW_TRACE_INIT()
#define  SYSVIEW_TRACE_START()
#define  SYSVIEW_TRACE_STOP()
#endif


#if SYSVIEW_CFG_TRACE_HEAP_EN
#define SYSVIEW_HeapDefine(pHeap, pBase,HeapSize, MetadataSize)		SEGGER_SYSVIEW_HeapDefine(pHeap, pBase,HeapSize, MetadataSize)
#define SYSVIEW_HeapAlloc(pHeap, pUserData, UserDataLen)	 		SEGGER_SYSVIEW_HeapAlloc(pHeap, pUserData, UserDataLen)
#define SYSVIEW_HeapAllocEx(pHeap, pUserData, UserDataLen, Tag) 	SEGGER_SYSVIEW_HeapAllocEx(pHeap, pUserData, UserDataLen, Tag)
#define SYSVIEW_HeapFree(pHeap, pUserData)							SEGGER_SYSVIEW_HeapFree(pHeap, pUserData)
#else
#define SYSVIEW_HeapDefine(pHeap, pBase,HeapSize, MetadataSize)
#define SYSVIEW_HeapAlloc(pHeap, pUserData, UserDataLen)	 	
#define SYSVIEW_HeapAllocEx(pHeap, pUserData, UserDataLen, Tag) 	
#define SYSVIEW_HeapFree(pHeap, pUserData)					
#endif

#endif
