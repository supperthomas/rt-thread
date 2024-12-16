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
    IPC

Description:
     IPC Driver

Revision History:
     Date                  Author                   Description
    2018.10.18            ming.m                        initial version
    ....
     2019.7.24                ming.m                     optimize IPC Lock
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_mbox.h"
#include "vdrv_ipc.h"
#include "vdev_ipc.h"

#define VSYS_IPC_EVENT_TEST 0

#if (VDEV_IPC_SUPPORTED > 0) 

#undef VDRV_IPC_DBG_EN
#define VDRV_IPC_DBG_EN         (VDBG_EN && 0)

#if (VDRV_IPC_DBG_EN > 0)
#define vdrv_ipc_dbg_info(fmt, ...)    NOTICE_PRINTF("%s(%05d):" fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define vdrv_ipc_dbg_info(...)
#endif


/* Global Infomation table */
#define VIPC_INFO_MAGIC     0x1234

typedef struct boot_info_s
{
    unsigned char  dsp_jtag_en:1;     /* 1-enable dsp jtag function, 0-disable dsp jtag function */
    unsigned char  mcu_jtag_sw_en:1;  /* 1-enable mcubt jtag-sw function, 0-disable mcubt jtag-sw function */
    unsigned char  mcu_jtag_etm_en:1; /* 1-enable mcubt etm trace function, 0-disable mcubt etm trace function */
    unsigned char  mcu_top_boot_en:1; /* 1-enable top bootloader , 0-disable top bootloader */
    unsigned char  mcu_code_state:2;  /* 0-no change, 1-success to load mcubt code image to code ram, 2-failed to decode, 3-failed to crc verify */
    unsigned char  dsp_pmem_state:2;  /* 0-no change, 1-success to load dsp pmem image to code ram, 2-failed to decode, 3-failed to crc verify */
    unsigned char  dsp_xmem_state:2;  /* 0-no change, 1-success to load dsp xmem image to code ram, 2-failed to decode, 3-failed to crc verify */
    unsigned char  dsp_ymem_state:2;  /* 0-no change, 1-success to load dsp ymem image to code ram, 2-failed to decode, 3-failed to crc verify */

    /* the flash address to stored mcubt and dsp code image, the highest bit indicate that is external or internal flash, 1 for external flash and 0 for internal flash */
    unsigned long  cbt_code_addr;     /* the flash address stored mcubt code image */
    unsigned long  cbt_code_len;      /* the length of the mcubt code image (encrypted)*/
    unsigned long  dsp_pmem_addr;     /* the flash address stored dsp pmem code image */
    unsigned long  dsp_pmem_len;      /* the length of the dsp pmem image (encrypted)*/
    unsigned long  dsp_xmem_addr;     /* the flash address stored dsp xmem const data */
    unsigned long  dsp_xmem_len;      /* the length of the dsp xmem image (encrypted)*/
    unsigned long  dsp_ymem_addr;     /* the flash address stored dsp ymem const data */
    unsigned long  dsp_ymem_len;      /* the length of the dsp ymem image (encrypted)*/
    unsigned long  per_load_addr;     /* per-loaded bt code address, bootloader will jump to this address without loading */
	
    /* boot up mode, boot up source,  for extension*/
    unsigned char boot_soft_reset_sign;     /* sign by bootloader to check whether soft reset */
    unsigned char boot_source:2;            /* boot source: 0-unknown, 1-power key, 2-soft reset */
    unsigned char boot_model;               /* 0-UserSet,1-EngineerMode */
    unsigned char bootloader_version[4]; 		/* Hardware ECO version identifier */
}boot_info_t;

typedef struct ipc_lock_s {
	spin_lock_t chip;
	spin_lock_t event;
	spin_lock_t flash;
	spin_lock_t msg_que;
	spin_lock_t mem_pool;
}ipc_locks_t;
//event matrix with a spin lock
typedef struct evt_mtx_s {
	spin_lock_t lock;
	unsigned int reg[((VIPC_EVT_LIST_LIMIT - 1) >> 5) + 1];//(IPC_EVT_LIST_SIZE - 1)/32 + 1]
}evt_mtx_t;

typedef struct ipc_event_tbl_s {
	struct evt_mtx_s tp2bt;
	struct evt_mtx_s bt2tp;
//	struct evt_mtx_s tp2dp;
//	struct evt_mtx_s dp2tp;
//	struct evt_mtx_s bt2dp;
//	struct evt_mtx_s dp2bt;
}ipc_event_tbl_t;

//msg queue table shared in SRAM between top and bt core.
//typedef struct ipc_msg_que_tbl_s {
//	struct ipc_msg_que_hd_s tp2bt;
//	struct ipc_msg_que_hd_s tp2dp;
//	struct ipc_msg_que_hd_s bt2tp;
//	struct ipc_msg_que_hd_s bt2dp;
//	struct ipc_msg_que_hd_s dp2tp;
//	struct ipc_msg_que_hd_s dp2bt;
//}ipc_msg_que_tbl_t;

struct ipc_stat_s {
	unsigned int reg[2];
}ipc_stat_t;

struct ipc_info_s {
	unsigned int magic;
	struct ipc_event_tbl_s evt_tbl;
	struct ipc_lock_s locks;         //based on event
//	struct ipc_msg_que_tbl_s msg_tbl;    //based on event
	struct ipc_stat_s stats;
};

struct rtt_info_s{
	void *SEGGER_RTT_TOP;
	void *SEGGER_RTT_BT;
	int   locks;
}; 

struct global_info_section_s {
    boot_info_t boot_info;
    struct ipc_info_s ipc_info;
	struct rtt_info_s rtt_info;
	struct wd_info_s wd_info;
#ifndef VIPC_DRV_MUTEX_LOCK_UNSUPPORT_X4
	vipc_drv_mutex_t drv_mutex;
#endif
	unsigned char debug_flags[4];

}*global_info_section_p = NULL;//global_info_section  __attribute__((at(VDEV_INFO_RAM_BASE + 0),zero_init));
#define global_info_section (*global_info_section_p)
static ipc_evt_hdle_t ipc_evt_hdle_tbl[VIPC_EVT_LIST_LIMIT];


static int VIPCLockInit(void);

/// ipc event Init
/// reset the event block of IPC in SRAM
/// @return 0 for success,-1 for error
static int VIPCEvtInit(void);

/// ipc event id get function
/// get the event id from share memory in SRAM
/// @dir direction such as TOP2BT BT2DSP
/// @timeout limited time to get an event
/// @return -1 for error,others as the event id.
static int VIPCEvtGet(int dir, unsigned int timeout);

/// Initialize IPC HW and data structure
int VIPCInit(void) {
	int ret = 0;
	if (global_info_section_p == NULL)
	{
		global_info_section_p = VShareMemInit((SINT8*)"glob", sizeof(struct global_info_section_s), SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (global_info_section_p == NULL) return -1;
	}
	VCPU_INT_SrcDisable(IRQ_MCUIPC_INT);

	//lock init
	ret = VIPCLockInit();
	if (ret !=0) {
		return -1;
	}

	//event init
	ret = VIPCEvtInit();
	if (ret !=0) {
		return -1;
	}

	VIPC_DRV_MUTEX_INIT(&global_info_section.drv_mutex);

	VCPU_INT_SrcEnable(IRQ_MCUIPC_INT);

	return 0;
}

/// Reset the IPC static structure data to default
void VIPCDeInit(void) {
	void *p;
	int sz;
	if (global_info_section_p == NULL)
	{
		global_info_section_p = VShareMemInit((SINT8*)"glob", sizeof(struct global_info_section_s), SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (global_info_section_p == NULL) return;
	}
	p = (void *) &global_info_section.ipc_info;
	sz = sizeof(global_info_section) - ((int)p - (int)&global_info_section);
	VCOMMON_MemSet(p, 0, sz);
	
}

static int VIPCLockInit(void) {
	int i;
  struct ipc_info_s *ipc_info = &global_info_section.ipc_info;

	if(ipc_info->magic == ((~(unsigned int)VIPC_INFO_MAGIC  <<16) | (unsigned int)VIPC_INFO_MAGIC) ) {
		/// clear locks in my side
		for(i=0;i<(sizeof(struct ipc_lock_s)/sizeof(spin_lock_t));i++)
			spin_lock_release(&((spin_lock_t *)&ipc_info->locks)[i]);
		return 0;
	}

	// Set global locks as default
	ipc_info->magic = ~VIPC_INFO_MAGIC;
	ipc_info->magic <<= 16;
	ipc_info->magic |= VIPC_INFO_MAGIC;
	
	for(i=0;i<(sizeof(struct ipc_lock_s)/sizeof(spin_lock_t));i++)
		spin_lock_init(&((spin_lock_t *)&ipc_info->locks)[i]);
	
	return 0;
}
 
/// Invoke ipc hardware signal
/// @ipc_dir the target ipc direction
void VIPCHW_Signal(int ipc_dir) {

	vdrv_ipc_dbg_info("invoke hw %d\r\n", ipc_dir);

	switch(ipc_dir) {
		case VIPC_DIR_TP2BT:
			VMBOX_INT_TOP2BT_SET();
			break;
		case VIPC_DIR_TP2DP:
			//VMBOX_INT_TOP2DSP_SET();
			break;
		case VIPC_DIR_BT2TP:
			VMBOX_INT_BT2TOP_SET();
			break;
		case VIPC_DIR_BT2DP:
			//VMBOX_INT_BT2DSP_SET()
			break;
		case VIPC_DIR_DP2TP:break;
		case VIPC_DIR_DP2BT:break;
		default: break;
	}

	// OS_CRITICAL_EXIT();
}

/// select a lock
spin_lock_t *VIPC_Lock_Get(int id) {
	spin_lock_t *lock = NULL;
	struct ipc_info_s *ipc_info = &global_info_section.ipc_info;
    
	switch(id) {
		case VIPC_LOCK_FLASH:
			lock = &ipc_info->locks.flash;
			break;
		case VIPC_LOCK_EVENT:
			lock = &ipc_info->locks.event;
		  break;
		case VIPC_LOCK_MSG_QUE:
			lock = &ipc_info->locks.msg_que;
		  break;
		case VIPC_LOCK_MEM_POOL:
			lock = &ipc_info->locks.mem_pool;
		  break;
		
		default: break;
	}
	
	return lock;
}

/// get/release lock
///@lock_id   the lock id
///           VIPC_LOCK_FLASH
///@return 0 for success
int VIPC_Lock(int lock_id) {
	int ret;
	spin_lock_t *lock;
	lock = VIPC_Lock_Get(lock_id);
	
	if(lock == NULL) return -1;
	
	ret = spin_lock_catch(lock);
	
	return ret;
}

int VIPC_Unlock(int lock_id) {
	spin_lock_t *lock;
	lock = VIPC_Lock_Get(lock_id);
	
	if(lock == NULL) return -1;
	
	spin_lock_release(lock);
	
	return 0;
}


/// The HW state and the ROM state
/// return 0 = unknown
///        1 = jmp to bt binary
///        -1 = load code fail
int VIPC_BTCoreState(void){
    int state;

    /* 0-no change, 1-success to load mcubt code image to code ram, 2-failed to decode, 3-failed to crc verify */
    switch(global_info_section.boot_info.mcu_code_state){
        case 1:
            state = 1;
            break;
        case 2:
        case 3:
            state = -1;
            break;
        case 0:
        default:
            state = 0;
    }
    return state;
}

/// set boot model
/// @model 

void VIPC_SetBootMode(UINT8 model){

	global_info_section.boot_info.boot_model = model;
}

/// get boot model
/// @return 
int VIPC_GetBootMode(void){
	
	return (int)global_info_section.boot_info.boot_model;
}

/// ipc event Init
/// reset the event block of IPC in SRAM
/// @return 0 for success,-1 for error
static int VIPCEvtInit(void) {
	int ret = 0;
	int i = 0;
	int dly_tm = 500;//try 500us
	evt_mtx_t *evt_mtx = NULL;
	spin_lock_t *pLock = &global_info_section.ipc_info.locks.event;

	if (pLock == NULL) return -1;

	do {
		ret = spin_lock_catch(pLock);
		if (ret != 0) {
			VCOMMON_Delay(1);
			dly_tm--;		
		} else {
			break;
		}
	} while (dly_tm > 0);
	if (ret != 0) {
		return -1;
	}

	evt_mtx = (evt_mtx_t *)&global_info_section.ipc_info.evt_tbl;
	for(i = 0; i < sizeof(ipc_event_tbl_t) / sizeof(evt_mtx_t); i++) {
		//init the spin lock
		spin_lock_init(&evt_mtx[i].lock);
		//init the event matrix table.
		VCOMMON_MemSet(&evt_mtx[i].reg, 0, sizeof(evt_mtx[i].reg));
	}

	spin_lock_release(pLock);

	VCOMMON_MemSet(ipc_evt_hdle_tbl, 0, VIPC_EVT_LIST_LIMIT * sizeof(ipc_evt_hdle_t));

	return 0;
}

/// register event handler function
/// @evt_id as index in event handler table
/// @hdle event handler
/// @return 0 for success,-1 for error
int VIPCEvtReg(unsigned int evt_id, ipc_evt_hdle_t hdle) {
	if (evt_id >= VIPC_EVT_LIST_LIMIT)
		return -1;

	if (hdle == NULL)
		return -1;

	ipc_evt_hdle_tbl[evt_id] = hdle;

	return 0;
}

/// ipc event id set function
/// set the event id to share memory in SRAM
/// @dir direction such as TOP2BT BT2DSP
/// @evt_id  is the id to be written
/// @timeout the maximum time gived to send an event
/// @return 0 for success,-1 for error
int VIPCEvtSet(unsigned int dir,unsigned int evt_id, int timeout) {
	int grp = 0;
	int pos = 0;
	int ret = 0;
	int dly_tm = 0;
	struct evt_mtx_s *p_mtx = NULL;
	
	if ((dir >= VIPC_DIR_LIMIT) || (evt_id >= VIPC_EVT_LIST_LIMIT))
		return -1;

	switch (dir) {
		case VIPC_DIR_BT2TP:
		p_mtx = &global_info_section.ipc_info.evt_tbl.bt2tp;
		break;

		case VIPC_DIR_TP2BT:
		p_mtx = &global_info_section.ipc_info.evt_tbl.tp2bt;
		break;

		default:
		break;
	}

	if ((ret == -1) || (p_mtx == NULL))
		return -1;

	grp = (evt_id >> 5); //evt_id / 32
	pos = (evt_id & 31); //evt_id % 32

	dly_tm = timeout;
	
	VCPU_SR_ALLOC();
	VCPU_CRITICAL_ENTER();
	
	do {
		ret = spin_lock_catch(&p_mtx->lock);
		if (ret != 0) {
			VCOMMON_Delay(1);
			dly_tm--;		
		} else {
			break;
		}
	} while (dly_tm > 0);
	if (ret != 0) {
		VCPU_CRITICAL_EXIT();
		return -1;
	}
	
	p_mtx->reg[grp] |= ((1 << pos) & (0xFFFFFFFF));
	spin_lock_release(&p_mtx->lock);
	
	VCPU_CRITICAL_EXIT();
	
	VIPCHW_Signal(dir);
#if (VSYS_IPC_EVENT_TEST > 0)
	VGPIO_SetPinOutput(16,0);
#endif
	return 0;
}

/// ipc event id get function
/// get the event id from share memory in SRAM
/// @dir direction such as TOP2BT BT2DSP
/// @timeout limited time to get an event
/// @return -1 parameter illegal
/// @return -2 time out when get event 
/// @return -3 event table is empty 
static int VIPCEvtGet(int dir, unsigned int timeout) {
	int ret = 0;
	int i = 0;
	int pos = 0;
	unsigned int temp = 0;
	unsigned int dly_tm = 0;
	int evt_id = -1;
	struct evt_mtx_s *p_mtx = NULL;
	
	if ((dir < 0) || (dir >= VIPC_DIR_LIMIT))
		return -1;

	switch (dir) {
		case VIPC_DIR_BT2TP:
		p_mtx = &global_info_section.ipc_info.evt_tbl.bt2tp;
		break;

		case VIPC_DIR_TP2BT:
		p_mtx = &global_info_section.ipc_info.evt_tbl.tp2bt;
		break;

		default:
		break;
	}
	if (p_mtx == NULL)
		return -1;

	//get the res lock within limited time
	dly_tm = timeout;
	do {
		ret = spin_lock_catch(&p_mtx->lock);
		if (ret != 0) {
			VCOMMON_Delay(1);
			dly_tm--;		
		} else {
			break;
		}
	} while (dly_tm > 0);
	if (ret != 0) {
		return -2;
	}
	
	for (i = 0; i < ((VIPC_EVT_LIST_LIMIT - 1) >> 5) + 1; i++) {
		temp = p_mtx->reg[i];
		if (temp > 0) {
			while ((temp & 0x01) == 0) {
				temp = temp >> 1;
				pos++;
			}
			evt_id = 32 * i + pos;	
      //get the event and clear it immediately			
		  p_mtx->reg[i] &= (~((1 << pos) & (0xFFFFFFFF)));	
			break;
		}
	}
	//release the res lock whick was got before
	spin_lock_release(&p_mtx->lock);

	//event table is empty
	if (i == ((VIPC_EVT_LIST_LIMIT - 1) >> 5) + 1)
		return -3;

	return evt_id;
}


/// ipc interrupt handler this function should be excute in finite time.
/// get event id from the event handle table and execute event handler function
/// @dir direction such as TOP2BT BT2DSP
void VIPCIntHandler(int dir) {
	int i = 0;
	int evt_id = 0;
	int ts = 5;//default wait time.
	ipc_evt_hdle_t hdle = NULL;

	ASSERT ((dir >= 0) && (dir < VIPC_DIR_LIMIT))

	for (i = 0; i < VIPC_EVT_LIST_LIMIT; i++) {
		evt_id = VIPCEvtGet(dir, ts);
		
		if ((evt_id >= 0) && (evt_id < VIPC_EVT_LIST_LIMIT)) {
			hdle = ipc_evt_hdle_tbl[evt_id];
			if (hdle != NULL) {
				hdle(dir, evt_id);			
			}
		} else if (evt_id == -3) {
			break;//event is empty, jump out the cycle
		} else {
			;//wait time out,just continue
		}
	}
}

/// ipc msg Init
/// reset the msg queue head and tail pointer
/// @return 0 for success,-1 for error
int VIPCMsgInit(ipc_msg_que_hd_t *msg_que) {

	msg_que->head = msg_que->tail = NULL;
	msg_que->cnt = 0;

	return 0;
}

int VIPCMsgEnQue(ipc_msg_que_hd_t *msg_que, ipc_msg_t *msg, ipc_msg_queue_insert_t where) {
	ipc_msg_que_node_t *p_node = NULL;
//	UINT8 *p_char = NULL;
//	UINT16 *p_U16 = NULL;
//	UINT16 dly_tm = 0;
//	int ret = 0;
	
	ASSERT ((msg_que != NULL)&&(msg != NULL));
	ASSERT ((where == IPC_MSG_QUE_INSERT_HEAD) || (where == IPC_MSG_QUE_INSERT_TAIL));

	//from data find pool block address
	p_node = (ipc_msg_que_node_t *) vcontainer_of(msg, ipc_msg_que_node_t,payload);
	if (p_node == NULL) { //malloc failed
		return -1;
	}

	//insert the block into the linked queue
	if (VIPCMsgQueEmpty(msg_que)) { //this is the first node be insert.
		msg_que->head = msg_que->tail = p_node;
		p_node->prev =NULL;
		p_node->next =NULL;
	} else {
		//where to insert:head or tail?
		if (where == IPC_MSG_QUE_INSERT_TAIL) { //insert to the tail
			msg_que->tail->next = p_node;
			p_node->prev = msg_que->tail;
			msg_que->tail = p_node;
			p_node->next =NULL;
		} else { //insert to the head
			msg_que->head->prev = p_node;
			p_node->next = msg_que->head;
			msg_que->head = p_node;
			p_node->prev = NULL;
		}
	}
	msg_que->cnt++;
	
	return 0;
}

ipc_msg_t *VIPCMsgDeQue(ipc_msg_que_hd_t *msg_que) {
	ipc_msg_que_node_t *p_node = NULL;
	
	if ((msg_que == NULL) || (VIPCMsgQueEmpty(msg_que))) {
		return NULL;
	}

	//delete the node from the linked queue
	p_node = msg_que->head;
	msg_que->head = msg_que->head->next;
	msg_que->cnt--;
	
	if (msg_que->tail == p_node) {
		msg_que->tail = NULL;
	}
	
	return (ipc_msg_t *)&p_node->payload;
}

int VIPCMsgQueEmpty(ipc_msg_que_hd_t *msg_que) {
	int is_que_empty;

	ASSERT(msg_que != NULL);

    if (msg_que == NULL) {
        return TRUE;
    }

	is_que_empty = ((msg_que->head == NULL)||(msg_que->cnt == 0)) ? TRUE : FALSE;

	return is_que_empty;
}

void VIPC_SetRTTInfoValue_TOP(void *segger_rtt_top)
{
	global_info_section.rtt_info.SEGGER_RTT_TOP = segger_rtt_top;
}

void VIPC_SetRTTInfoValue_BT(void *segger_rtt_bt)
{
	global_info_section.rtt_info.SEGGER_RTT_BT = segger_rtt_bt;
}

wd_info_t *VIPC_Get_WD(void)
{
	return &global_info_section.wd_info;
}


void * VIPC_GetRTTInfoValue_TOP(void)
{
	return global_info_section.rtt_info.SEGGER_RTT_TOP;
}

void * VIPC_GetRTTInfoValue_BT(void)
{
	return global_info_section.rtt_info.SEGGER_RTT_BT;
}

void VIPC_SEGGER_RTT_mutex_lock(void)
{
	while(global_info_section.rtt_info.locks > 0){
		global_info_section.rtt_info.locks--;
	}
	global_info_section.rtt_info.locks = 0xD000;
}
	
void VIPC_SEGGER_RTT_mutex_unlock(void)
{
	global_info_section.rtt_info.locks = 0x00;
}

/// set mcu_code_state for top
void VIPC_SET_BTCoreState(UINT8 flag)
{
	if (global_info_section_p == NULL)
	{
		global_info_section_p = VShareMemInit((SINT8*)"glob", sizeof(struct global_info_section_s), SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (global_info_section_p == NULL) return;
	}
	global_info_section.boot_info.mcu_code_state = flag;
}

/// set mcu_code_state for top
void Wait_top_run(void)
{
   while(!global_info_section.boot_info.mcu_code_state);
}

/// set boot source
/// @model 
void VIPC_SetBootSource(UINT8 source){

	global_info_section.boot_info.boot_source = source;
}

/// get boot source
/// @return 
int VIPC_GetBootSource(void){
	
	return (int)global_info_section.boot_info.boot_source;
}

/******************************************************************************
Description:
  Obtain the ECO version information
Parameters:
	None.
Return Value:
	
******************************************************************************/
void VIPC_SetBootVersion(void)
{
	unsigned int reg = 0;
	unsigned int *version = (unsigned int*)global_info_section.boot_info.bootloader_version;
	
	reg = READ_REG((void*)0x7FF0);
	*version = reg;
}

SINT32 VIPC_GetBootVersion(void)
{
	unsigned char *version = global_info_section.boot_info.bootloader_version;
	
	if(version[0] == 'E' && version[1] == 'C' && version[2] == 'O'){
		return version[3] - '0';
	}
	return -1;
}

#endif //#if (VDEV_IPC_SUPPORTED > 0) 
