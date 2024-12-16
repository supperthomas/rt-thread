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
	2023.04.3            cxt   						create
------------------------------------------------------------------------------*/

#ifndef _VDEV_IPC_H_
#define _VDEV_IPC_H_

#include "vstdlib.h"
#include "vstdcore.h"

#ifdef __cplusplus
extern "C"
{
#endif
    #define IPC_MEMPOOL_SIZE  (3*512)
    #define IPC_INFO_MAGIC MAKE_U32("IPCM")

    #define IPC_TIMEOUT_MULT    (300*30)//convert to us

#pragma pack (1)
    typedef volatile unsigned int spin_lock_t;
    
    typedef enum
    {
        VIPC_DIR_2TP = CPU_ID_TOP,
        VIPC_DIR_2BT = CPU_ID_BT,
        VIPC_DIR_2DP = CPU_ID_DSP,
        VIPC_DIR_ALL,
    } vipc_dir_typ_t;

    typedef enum
    {
        VIPC_EVT_TYP_MSG_TRANSMIT = VIPC_DIR_ALL,//0x08
        VIPC_EVT_TYP_MUTEX_UNLOCK,//0x10
        VIPC_EVT_TYP_SEMAPHORE_GIVE,//0x20
        VIPC_EVT_TYP_FLAGS_SET,//0x40
        VIPC_EVT_TYP_QUE_SEND,//0x80
        VIPC_EVT_TYP_RESET_CTR,//0x100
        VIPC_EVT_LIMIT,
    } vipc_evt_typ_t;

    typedef enum
    {
        VIPC_LOCK_TYP_MSG_QUE,
        VIPC_LOCK_TYP_MEM_POOL,
        VIPC_LOCK_LIST_LIMIT,
    } vipc_lock_typ_t;

    typedef enum
    {
        IPC_MUTEX_HANDLE,
        IPC_SEMAPHORE_HANDLE,
        IPC_FLAGS_HANDLE,
        IPC_QUE_HANDLE,
        IPC_HANDLE_LIST_LIMIT,
    } vipc_handle_t;

    typedef struct
    {
        spin_lock_t lock;
        UINT32      lflag;
        UINT32      reqlock;
    } vresspin_lock_t;

    typedef struct
    {
        spin_lock_t lock;
        UINT32 reg;
    } vipc_evt_mtx_t;
	
	#pragma anon_unions
    typedef struct
    {
        union
        {
            struct
            {
                vipc_evt_mtx_t tp2bt;
                vipc_evt_mtx_t tp2dp;
                vipc_evt_mtx_t bt2tp;
                vipc_evt_mtx_t bt2dp;
                vipc_evt_mtx_t dp2tp;
                vipc_evt_mtx_t dp2bt;
            };
            vipc_evt_mtx_t mtx[6];
        };
    } vipc_event_tbl_t;

    typedef struct ipc_list_node_s
    {
        struct ipc_list_node_s *prev;
        struct ipc_list_node_s *next;    
        UINT8 payload[1];                        
    } vipc_list_node_t;

    typedef struct
    {
        struct ipc_list_node_s *prev;
        struct ipc_list_node_s *next;                            
    } vipc_list_node_pt;
    
    typedef struct
    {
        vipc_list_node_t *head;
        vipc_list_node_t *tail;
        UINT32 cnt;
    } vipc_list_hd_t;

    typedef struct 
    {
        spin_lock_t lock;
        vipc_list_hd_t que;
    }vipc_msg_que_t;

    typedef union {
        struct
        {
            vipc_msg_que_t tp2bt;
            vipc_msg_que_t tp2dp;
            vipc_msg_que_t bt2tp;
            vipc_msg_que_t bt2dp;
            vipc_msg_que_t dp2tp;
            vipc_msg_que_t dp2bt;
        };
        vipc_msg_que_t que[6];
    } vipc_msg_tbl_t;

    typedef union {
        struct
        {
            vipc_msg_que_t mutex_handle;
            vipc_msg_que_t sem_handle;
            vipc_msg_que_t flags_handle;
            vipc_msg_que_t que_handle;
        };
        vipc_msg_que_t handle_list[4];
    } vipc_handle_tbl_t;

    typedef struct 
    {
        vresspin_lock_t lock;
        UINT32 block[(IPC_MEMPOOL_SIZE) / 4];
    }vipc_mem_pool_t;

    typedef struct
    {
        UINT32 magic;
        vipc_event_tbl_t evt_tbl;
        vipc_msg_tbl_t msg_tbl;
        vipc_handle_tbl_t handle_tbl;
        vipc_mem_pool_t pool;
    } vipc_info_t;

    #define VIPC_DRV_FLAG_TYPE  UINT8

    typedef enum  
    {
        i2c_ipc_lock,
        flash_ipc_lock,
        DRVS_LOCK_LIMIT,
        //....
    } vipc_drvs_lock_e;

    typedef struct  
    {
        spin_lock_t lock;
        VIPC_DRV_FLAG_TYPE drvs[DRVS_LOCK_LIMIT];
    } vipc_drv_mutex_t;

    typedef struct
    {
        vipc_info_t ipc_info;
        vipc_drv_mutex_t drv_mutex;
    } vipc_info_section_t;
    
#pragma pack () 

    typedef void (*vipc_evt_hdle_t)(vipc_dir_typ_t dir, UINT32 evt);
    typedef int (*vipc_list_check_t)(vipc_list_node_t *node, void *arg);

    void ipc_spin_lock_init(spin_lock_t *lock);
    BOOL ipc_spin_lock_catch(spin_lock_t *lock, int timeout, unsigned int *ipc_sr);
    BOOL ipc_spin_lowlock_catch(spin_lock_t *lock, int timeout);
    void ipc_spin_lock_release(spin_lock_t *lock, unsigned int ipc_sr);
    void ipc_spin_lowlock_release(spin_lock_t *lock);
    UINT32 ipc_spin_lock_check(spin_lock_t *lock);

    int VIPC_Init(void);
    void VIPC_DeInit(void);

    void VIPC_HW_Signal(vipc_dir_typ_t ipc_dir);
    int VIPC_EvtReg(vipc_evt_hdle_t hdle);
    int VIPC_GetExport(vipc_dir_typ_t dir);
    int VIPC_GetImport(vipc_dir_typ_t dir);
    int VIPC_EvtSet(vipc_dir_typ_t dir, unsigned int evt_id, int timeout);
    void VIPC_IntHandler(vipc_dir_typ_t dir);
	void VIPC_Interrupt_Routine_Service(void);
    vipc_info_t *VIPC_Info_Get(void);
    vipc_mem_pool_t *VIPC_Pool_Get(void);

    int ipc_drvlocks_init(vipc_drv_mutex_t *drv_mutex);
    BOOL ipc_drvlocks_check(UINT8 lock_numb, UINT8 offset);
    BOOL ipc_drvlocks_lock(UINT8 lock_numb, UINT8 offset);
    BOOL ipc_drvlocks_unlock(UINT8 lock_numb, UINT8 offset);

#if VIPC_DRV_MUTEX_LOCK_SUPPORT
    #define VIPC_DRV_MUTEX_INIT(mutex)          ipc_drvlocks_init(mutex)
    #define VIPC_DRV_MUTEX_LOCK(mutex, lock)    ipc_drvlocks_lock(mutex, lock)
    #define VIPC_DRV_MUTEX_UNLOCK(mutex, lock)  ipc_drvlocks_unlock(mutex, lock)
    #define VIPC_DRV_MUTEX_CHECK(mutex, lock)   ipc_drvlocks_check(mutex, lock)
#else
    #define VIPC_DRV_MUTEX_INIT(mutex)          (UNUSED(0))
    #define VIPC_DRV_MUTEX_LOCK(mutex, lock)    (TRUE)
    #define VIPC_DRV_MUTEX_UNLOCK(mutex, lock)  (TRUE)
    #define VIPC_DRV_MUTEX_CHECK(mutex, lock)   (FALSE)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _VDEV_IPC_H_ */
