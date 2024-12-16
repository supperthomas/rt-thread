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
	slave die

Description:
	.

Revision History:
	Date                  Author                   Description
	2024.01.02				cxt					 initial version
------------------------------------------------------------------------------*/
#ifndef _VMODE_COMMON_H_
#define _VMODE_COMMON_H_

#include "vstdlib.h"
#include "vcoroutine.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AUDIO_PACK_LEN (1024)

#define VMODE_NAME_LEN 8

	typedef struct vmode_cor_que
	{
		UINT8 state;
		void *arg;
		void *handle;
		VCORTcb_t tcb;
		VCORQue_t send;
		VCORQue_t receive;
		struct vmode_cor_que *pre;
		struct vmode_cor_que *next;
	} VMODECorHandle;
	
	typedef UINT8 (*VMODECorQue_MountInit)(void);
	typedef UINT8 (*VMODECorQue_MountMode)(VMODECorHandle *self);
	typedef UINT8 (*VMODECorQue_UnmountMode)(VMODECorHandle *self);
	typedef UINT8 (*VMODECorQue_OperateMode)(VMODECorHandle *self, UINT8 cmd, void *arg);

	typedef struct
	{
		const SINT8 *name;
		const UINT8 handle_num;
		const VMODECorHandle *node;
		VMODECorQue_MountInit init;
		VMODECorQue_MountMode mount;
		VMODECorQue_UnmountMode unmount;
		VMODECorQue_OperateMode operate;
	} VMODEInitDesc;

	typedef struct
	{
		SINT8 *mode;
		SINT8 *pro_mode;
		void *arg;
	} VMODECorConfig;

	typedef struct
	{
		SINT8 m_name[VMODE_NAME_LEN];
		SINT8 p_name[VMODE_NAME_LEN];
		UINT8 depth;
	} VMODE_MountFormat_t;

	typedef struct
	{
		SINT8 p_name[VMODE_NAME_LEN];
		UINT8 depth;
	} VMODE_AddMountFormat_t;

#define INIT_MODE_EXPORT(str, handle, init, mount, unmount, oper, level)                                                       \
	const SINT8 __mode_##str##_name[] = #str;                                                                                  \
	__attribute__((used)) const VMODEInitDesc __mode_init_desc_##str __attribute__((section(".mode_desc." level))) =           \
		{__mode_##str##_name, sizeof(handle) / sizeof(VMODECorHandle), (VMODECorHandle *)&handle, (VMODECorQue_MountInit)init, \
		 (VMODECorQue_MountMode)mount, (VMODECorQue_UnmountMode)unmount, (VMODECorQue_OperateMode)oper};

#define INIT_MODE_MOUNT(name, handle, init, mount, unmount, oper) INIT_MODE_EXPORT(name, handle, init, mount, unmount, oper, "2")

	UINT8 VMODE_Mount_Handle(VMODECorHandle *m_node, VMODECorHandle *p_node, VCORQue_t que_list);
	UINT8 VMODE_Mount_AddMode(VMODECorHandle *m_node, SINT8 *pre_name, UINT8 que_depth);
	UINT8 VMODE_Mount_INVMode(SINT8 *m_name, VMODECorHandle *p_node, UINT8 que_depth);
	UINT8 VMODE_Clean_Handle(VMODECorHandle *m_node);

	UINT8 VMODE_Mount_Init(void);
	UINT8 VMODE_Mount_Mode(SINT8 *mode_name, SINT8 *pre_name, UINT8 que_depth);	 
	UINT8 VMODE_Unmount_Mode(SINT8 *mode_name);
	UINT8 VMODE_Mode_Operate(SINT8 *mode_name, UINT8 cmd, void *arg);
	UINT8 VMODE_Display_Mode(void);

	void vmode_quereceive_merge(VCORQue_t p_corque, UINT8 **buff, UINT8 len);
#define VMODE_QueReceive_Merge(receive, buff, len, timeout) \
	do                                                      \
	{                                                       \
		VCORWaitContent(receive->numb, timeout);            \
		vmode_quereceive_merge(receive, buff, len);         \
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* _VMODE_COMMON_H_ */
