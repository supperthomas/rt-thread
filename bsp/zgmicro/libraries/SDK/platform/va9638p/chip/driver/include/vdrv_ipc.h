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
    IPC: Inter Processor Communication

Description:
     Tool set for Synchronization between Cores or Single Core tasks.

Revision History:
     Date                  Author                   Description
     2018.10.18            ming.m                        initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif

#ifndef _VIPC_H_
#define _VIPC_H_

#include "vstdlib.h"
#include "vstdcore.h"
#include "vdrv_wd.h"

#ifdef __cplusplus
extern "C"
{
#endif
	

typedef volatile unsigned int spin_lock_t;


static inline int spin_lock_lowlevel_catch(int self, spin_lock_t *lock) {
	int lock_raw;
	char *lock_member;

	lock_member = (char *)lock;
	// check if already locked
	lock_raw = *lock;
	if(lock_raw > 0) return 1;
	// set local lock
	lock_member[self] = 0x01;
	// check remote lock
	lock_raw = *lock;
	lock_raw &= ~(0x000000FF << self);
	if(!lock_raw) return 0;
	// catch lock fail bcoz of remote locked, clear local lock
	lock_member[self] = 0x00;
	return 1;
}

static inline void spin_lock_lowlevel_release(int self, spin_lock_t *lock) {
	char *lock_member = (char *)lock;

	lock_member[self] = 0x00;
}
//static int spin_lock_lowlevel_init(int self, spin_lock_t *lock);

/// get the spin_lock, and set it locked
/// @return 0 for success
///         1 for already locked
static inline int spin_lock_catch(spin_lock_t *lock) {
	int core_id;
	core_id = VCORE_GetID();
	return spin_lock_lowlevel_catch(core_id, lock);
}

/// release the spin_lock
/// 
static inline void spin_lock_release(spin_lock_t *lock) {
	int core_id;
	core_id = VCORE_GetID();
	spin_lock_lowlevel_release(core_id, lock);
}

static inline void spin_lock_init(spin_lock_t *lock) {
	*lock = 0;
	//spin_lock_release(lock);
}




#define BOOT_RESET_SIGN          0xAA

#define BOOT_SOURCE_UNKNOWN         0
#define BOOT_SOURCE_POWER_KEY       1
#define BOOT_SOURCE_SOFT_RESET      2

#define MSG_HEAD_SIZE(type)  (sizeof(type) - sizeof(UINT8))
#define IPC_MSG_SIZE( DATALEN ) (sizeof(ipc_msg_t) - 1 + (DATALEN))
//msg queue node head structure:sizeof (ipc_msg_que_node_t) - 1byte data[0]
#define MSG_QUE_NODE_HEAD_SIZE (sizeof(ipc_msg_que_node_t) - 1)
/* msg que node structure:
 * ______________________________________________________________
 * |                                                             |
 * | *prev(4bytes) | *next(4bytes) | len(2bytes) | data(n bytes)
 * |_____________________________________________________________|
 */
#define MSG_QUE_NODE_SIZE( DATALEN ) (sizeof(ipc_msg_que_node_t) + DATALEN - 1)

/* IPC lock, which shared between cores */
typedef enum  ipc_lock_typ_e{
	VIPC_LOCK_EVENT,
	VIPC_LOCK_FLASH,
	VIPC_LOCK_MSG_QUE,
	VIPC_LOCK_MEM_POOL,
	VIPC_LOCK_LIMIT
}ipc_lock_typ_t;

/// IPC event
typedef enum  ipc_evt_typ_e{
	VIPC_EVT_TYP_GPIO = 0,          // GPIO event
	VIPC_EVT_TYP_MSG,               // msg event
	VIPC_EVT_TYP_SYS_RST,
	VIPC_EVT_TYP_SYS_STATE_UPDATE,  // system state update event
	VIPC_EVT_TYP_RES_UNLOCK,        // resource un-lock event
	VIPC_EVT_TYP_OS_STARTED,         //after bt started, it will give this event to top. 
	//for testing ipc event 	
	VIPC_EVT_TYP_SYS_SOFT_RESET,
	VIPC_EVT_TYP_BT_DEV_POWER_ON,
	VIPC_EVT_TYP_BT_DEV_POWER_OFF,
	VIPC_EVT_TYP_BT_DEV_MUTE,
	VIPC_EVT_TYP_BT_DEV_UNMUTE,
	VIPC_EVT_TYP_PCMFORWARD,
	VIPC_EVT_TYP_USB_AUDIO_DATA_COMMING, //usb data is comming from top
	VIPC_EVT_TYP_USB_AUDIO_STATE_CTRL, //enter or exit usb audio mode
	VIPC_EVT_TYP_BT_DEV_WAKEUP, //enter or exit usb audio mode
	VIPC_EVT_TYP_DATA_DUMP,
	VIPC_EVT_LIST_LIMIT = 128        // ipc event total size
}ipc_evt_typ_t;

/* IPC direction based on hardware design*/
typedef enum  ipc_msg_dir_typ_e{
	VIPC_DIR_TP2BT = 0,
	VIPC_DIR_TP2DP,
	VIPC_DIR_BT2TP,
	VIPC_DIR_BT2DP,
	VIPC_DIR_DP2TP,
	VIPC_DIR_DP2BT,
	VIPC_DIR_LIMIT
}ipc_msg_dir_typ_t;

//ways to select of insert msg into a queue.
typedef enum ipc_msg_queue_insert_e {
	IPC_MSG_QUE_INSERT_HEAD = 0,	//insert to queue head
	IPC_MSG_QUE_INSERT_TAIL	    //insert to queue tail
}ipc_msg_queue_insert_t;

//msg queue node structure
typedef struct ipc_msg_que_node_s {
	struct ipc_msg_que_node_s *prev;
	struct ipc_msg_que_node_s *next;
	UINT16	len; 	// length of data
	UINT8 payload[1];//trans payload
}ipc_msg_que_node_t __attribute__( (aligned(1)) );//aligned to one byte

//linked msg queue head structure
typedef struct ipc_msg_que_hd_s {
	ipc_msg_que_node_t *head;
	ipc_msg_que_node_t *tail;
	UINT32 cnt;
}ipc_msg_que_hd_t;

typedef struct ipc_msg_hdr_s {
	UINT8	type;	// type of the message
	UINT16	len; 	// length of data
} ipc_msg_hdr_t  __attribute__( (aligned(1)) );

typedef struct ipc_msg_s {
	UINT16	type;	// type of the message,byte alignment to 2bytes.
	UINT16	len; 	// length of data
	UINT8	data[1];	//
}ipc_msg_t  __attribute__( (aligned(1)) );

typedef void (*ipc_evt_hdle_t)(int dir, unsigned int evt);	

/// Initialize IPC HW and data structure
int VIPCInit(void);

/// Reset the IPC static structure data to default
void VIPCDeInit(void);

// Get spin_lock substance from IPC Lock ID
spin_lock_t *VIPC_Lock_Get(int id);
/// get/release lock
///@lock_id   the lock id
///           VIPC_LOCK_FLASH
///@return 0 for success
int VIPC_Lock(int lock_id);
int VIPC_Unlock(int lock_id);
	
/// Invoke ipc hardware signal
/// @ipc_dir the target ipc direction
void VIPCHW_Signal(int ipc_dir);

int VIPC_BTCoreState(void);

/// set boot model stored in IPC
/// @model 
/// 		VIPC_BOOTMODEL_USER  		for user bootmodel
/// 		VIPC_BOOTMODEL_ENGINEER		for engineer bootmodel
void VIPC_SetBootMode(UINT8 model);

/// get boot model stored in IPC
/// @return 
int VIPC_GetBootMode(void);

/// register event handler function
/// @evt_id as index in event handler table
/// @hdle event handler
/// @return 0 for success,-1 for error
int VIPCEvtReg(unsigned int evt_id, ipc_evt_hdle_t hdle);

/// ipc event id set function
/// set the event id to share memory in SRAM
/// @dir direction such as TOP2BT BT2DSP
/// @evt_id  is the id to be written
/// @timeout the maximum time gived to send an event
/// @return 0 for success,-1 for error
int VIPCEvtSet(unsigned int dir,unsigned int evt_id, int timeout);


/// ipc interrupt handler this function should be excute in finite time.
/// get event id from the event handle table and execute event handler function
/// @dir direction such as TOP2BT BT2DSP
void VIPCIntHandler(int dir);

int VIPCMsgInit(ipc_msg_que_hd_t *msg_que);
int VIPCMsgEnQue(ipc_msg_que_hd_t *msg_que, ipc_msg_t *msg, ipc_msg_queue_insert_t where);
ipc_msg_t *VIPCMsgDeQue(ipc_msg_que_hd_t *msg_que);
int VIPCMsgQueEmpty(ipc_msg_que_hd_t *msg_que);

void VIPC_SetRTTInfoValue_BT(void *segger_rtt_bt);
void VIPC_SetRTTInfoValue_TOP(void *segger_rtt_top);

/// get wdt structure shared between bt and top
/// @return wdt structure address
wd_info_t *VIPC_Get_WD(void);

void * VIPC_GetRTTInfoValue_TOP(void);
void * VIPC_GetRTTInfoValue_BT(void); 
void VIPC_SEGGER_RTT_mutex_lock(void);
void VIPC_SEGGER_RTT_mutex_unlock(void);
	
void VIPC_SET_BTCoreState(UINT8 flag);
void Wait_top_run(void);

void VIPC_SetBootSource(UINT8 source);
int VIPC_GetBootSource(void);
void VIPC_SetBootVersion(void);
SINT32 VIPC_GetBootVersion(void);


#ifdef __cplusplus
}
#endif

#endif /* _VIPC_H_ */
