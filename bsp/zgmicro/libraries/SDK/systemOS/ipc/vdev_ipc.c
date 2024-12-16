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
	2023.04.3            cxt   						create
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_mbox.h"
#include "vdev_ipc.h"

static vipc_info_section_t *ipc_info_section_p = NULL;
#define ipc_info_section (*ipc_info_section_p)

static vipc_evt_hdle_t ipc_evt_hdle = NULL;

void ipc_spin_lock_init(spin_lock_t *lock)
{
    *lock = 0;
}

static inline int spin_lock_lowlevel_catch(int self, spin_lock_t *lock)
{
    int lock_raw;
    char *lock_member;

    ASSERT(self < sizeof(lock_member));

    lock_member = (char *)lock;

    lock_raw = *lock;
    if (lock_raw > 0)
        return 1;

    lock_member[self] = 0x01;

    lock_raw = *lock;
    lock_raw &= ~(0x000000FF << self);
    if (!lock_raw)
        return 0;

    lock_member[self] = 0x00;

    return 1;
}

static inline void spin_lock_lowlevel_release(int self, spin_lock_t *lock)
{
    char *lock_member = (char *)lock;

    ASSERT(self < sizeof(lock_member));

    lock_member[self] = 0x00;
}

static inline UINT32 spin_lock_lowlevel_check(int self, spin_lock_t *lock)
{
    char *lock_member = (char *)lock;

    ASSERT(self < sizeof(lock_member));

    return lock_member[self];
}

BOOL ipc_spin_lock_catch(spin_lock_t *lock, int timeout, unsigned int *ipc_sr)
{
    int core_id, ret;
    int dly_tm = timeout*IPC_TIMEOUT_MULT;
    core_id = VCORE_GetID();
    while (1)
    {
        *ipc_sr = VCPU_SR_Save();
        ret = spin_lock_lowlevel_catch(core_id, lock);
        if (ret == 0)
        {
            return TRUE;
        }
        else if (dly_tm > 0)
        {
            VCPU_SR_Restore(*ipc_sr);
            VCPU_Nop(5);
            dly_tm--;
        }
        else
        {   
            VCPU_SR_Restore(*ipc_sr);
            return FALSE;
        }
    }
}

BOOL ipc_spin_lowlock_catch(spin_lock_t *lock, int timeout)
{
    int core_id, ret;
    int dly_tm = timeout*IPC_TIMEOUT_MULT;
    core_id = VCORE_GetID();
    while (1)
    {
        ret = spin_lock_lowlevel_catch(core_id, lock);
        if (ret == 0)
        {
            return TRUE;
        }
        else if (dly_tm > 0)
        {
            VCPU_Nop(5);
            dly_tm--;
        }
        else
        {   
            return FALSE;
        }
    }
}

/// release the spin_lock
///
void ipc_spin_lock_release(spin_lock_t *lock, unsigned int ipc_sr)
{
    int core_id;

    core_id = VCORE_GetID();
    spin_lock_lowlevel_release(core_id, lock);
    VCPU_SR_Restore(ipc_sr);
}

void ipc_spin_lowlock_release(spin_lock_t *lock)
{
    int core_id;

    core_id = VCORE_GetID();
    spin_lock_lowlevel_release(core_id, lock);
}

UINT32 ipc_spin_lock_check(spin_lock_t *lock)
{
    int core_id;

    core_id = VCORE_GetID();
    return spin_lock_lowlevel_check(core_id, lock);
}

static int VIPCEvtInit(void)
{
    int ret = 0;
    int i = 0;

    vipc_evt_mtx_t *evt_mtx = (vipc_evt_mtx_t*)&ipc_info_section.ipc_info.evt_tbl;

    for (i = 0; i < sizeof(vipc_event_tbl_t) / sizeof(vipc_evt_mtx_t); i++)
    {
        ipc_spin_lock_init(&evt_mtx[i].lock);
        vmemset(&evt_mtx[i].reg, 0, sizeof(evt_mtx[i].reg));
    }

    return 0;
}


int VIPC_Init(void)
{
    if (ipc_info_section_p == NULL)
	{
		ipc_info_section_p = VShareMemInit((SINT8*)"ipci", sizeof(vipc_info_section_t), SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (ipc_info_section_p == NULL) return -1;
	}
    UINT32 *magic = &ipc_info_section.ipc_info.magic;

    if (*magic == IPC_INFO_MAGIC)
    {
        return 0;
    }

    VIPCEvtInit();

    VIPC_DRV_MUTEX_INIT(&ipc_info_section.drv_mutex);

    return 0;
}

void VIPC_DeInit(void)
{
    if (ipc_info_section_p == NULL)
	{
		ipc_info_section_p = VShareMemInit((SINT8*)"ipci", sizeof(vipc_info_section_t), SHAREMEM_TOP_BIT|SHAREMEM_BT_BIT);
		if (ipc_info_section_p == NULL) return;
	}
    vipc_info_t *info = &ipc_info_section.ipc_info;

    VCOMMON_MemSet(info, 0, sizeof(vipc_info_t));
}

void VIPC_HW_Signal(vipc_dir_typ_t ipc_dir)
{
    VCoreId_t core_id;
    UINT32 id = 0;

    core_id = VCORE_GetID();
    id = (ipc_dir | (core_id << 16));

    switch (id)
    {
    case ((CPU_ID_TOP << 16) | VIPC_DIR_2BT):
        VMBOX_INT_TOP2BT_SET();
        break;
    case ((CPU_ID_TOP << 16) | VIPC_DIR_2DP):
        VMBOX_INT_TOP2DSP_SET();
        break;
    case ((CPU_ID_BT << 16) | VIPC_DIR_2TP):
        VMBOX_INT_BT2TOP_SET();
        break;
    case ((CPU_ID_BT << 16) | VIPC_DIR_2DP):
        VMBOX_INT_BT2DSP_SET();
        break;
    case ((CPU_ID_DSP << 16) | VIPC_DIR_2TP):
        break;
    case ((CPU_ID_DSP << 16) | VIPC_DIR_2BT):
        break;
    default:
        break;
    }
}

int VIPC_EvtReg(vipc_evt_hdle_t hdle)
{
    ipc_evt_hdle = hdle;

    return 0;
}

int VIPC_GetExport(vipc_dir_typ_t dir)
{
    int ret = -1;
    VCoreId_t  core_id = VCORE_GetID();
    UINT32 id = ((core_id << 16) | dir);

    switch (id)
    {
    case ((CPU_ID_TOP << 16) | VIPC_DIR_2BT):
        ret = 0;
        break;
    case ((CPU_ID_TOP << 16) | VIPC_DIR_2DP):
        ret = 1;
        break;
    case ((CPU_ID_BT << 16) | VIPC_DIR_2TP):
        ret = 2;
        break;
    case ((CPU_ID_BT << 16) | VIPC_DIR_2DP):
        ret = 3;
        break;
    case ((CPU_ID_DSP << 16) | VIPC_DIR_2TP):
        ret = 4;
        break;
    case ((CPU_ID_DSP << 16) | VIPC_DIR_2BT):
        ret = 5;
        break;
    default:
        break;
    }
    return ret;
}

int VIPC_GetImport(vipc_dir_typ_t dir)
{
    int ret = -1;
    VCoreId_t  core_id = VCORE_GetID();
    UINT32 id = ((dir << 16) | core_id);

    switch (id)
    {
    case ((VIPC_DIR_2TP << 16) | CPU_ID_BT):
        ret = 0;
        break;
    case ((VIPC_DIR_2TP << 16) | CPU_ID_DSP):
        ret = 1;
        break;
    case ((VIPC_DIR_2BT << 16) | CPU_ID_TOP):
        ret = 2;
        break;
    case ((VIPC_DIR_2BT << 16) | CPU_ID_DSP):
        ret = 3;
        break;
    case ((VIPC_DIR_2DP << 16) | CPU_ID_TOP):
        ret = 4;
        break;
    case ((VIPC_DIR_2DP << 16) | CPU_ID_BT):
        ret = 5;
        break;
    default:
        break;
    }
    return ret;
}


int VIPC_EvtSet(vipc_dir_typ_t dir, unsigned int evt_id, int timeout)
{
    int ret;
    unsigned int ipc_sr = 0;
    vipc_evt_mtx_t *p_mtx = NULL;


    if (dir >= VIPC_DIR_ALL)
        return -1;

    ret = VIPC_GetExport(dir);
    if (ret < 0){
        return -1;
    }

    p_mtx = &ipc_info_section.ipc_info.evt_tbl.mtx[ret];

    if (p_mtx && ipc_spin_lock_catch(&p_mtx->lock, timeout,&ipc_sr))
    {
        p_mtx->reg |= evt_id;
        ipc_spin_lock_release(&p_mtx->lock,ipc_sr);
        VIPC_HW_Signal(dir);
        return 0;
    }
    return -1;
}

static UINT32 VIPCEvtGet(vipc_dir_typ_t dir, unsigned int timeout)
{
    int evt_id = 0,ret;
    unsigned int ipc_sr = 0;
    vipc_evt_mtx_t *p_mtx = NULL;


    if (dir >= VIPC_DIR_ALL)
        return 0;

    ret = VIPC_GetImport(dir);
    if (ret < 0){
        return 0;
    }
    p_mtx = &ipc_info_section.ipc_info.evt_tbl.mtx[ret];

    if (p_mtx && ipc_spin_lock_catch(&p_mtx->lock, timeout,&ipc_sr))
    {
        evt_id = p_mtx->reg;
        p_mtx->reg = 0;
        ipc_spin_lock_release(&p_mtx->lock,ipc_sr);
        return evt_id;
    }
    return 0;
}

/// ipc interrupt handler this function should be excute in finite time.
/// get event id from the event handle table and execute event handler function
/// @dir direction such as TOP2BT BT2DSP
void VIPC_IntHandler(vipc_dir_typ_t dir)
{
    UINT32 evt_reg = 0;

    ASSERT(dir < VIPC_DIR_ALL);

    evt_reg = VIPCEvtGet(dir, 0);
    if (evt_reg > 0 && ipc_evt_hdle)
    {
        ipc_evt_hdle(dir,evt_reg);
    }
}

void VIPC_Interrupt_Routine_Service(void)
{
	#if (defined __GNUC__)//TODO:cxt
	#elif (defined __CC_ARM)
	#ifdef CORE_SEL_MCUTOP
	UINT32 bx_ctrl = READ_REG(REG_MSG_CM3MBX_CTRL1);
	if(bx_ctrl&BIT0){
		VMBOX_INT_BT2TOP_CLR();
		VIPC_IntHandler(VIPC_DIR_2BT);
	}
	#else
	
	UINT32 bx_ctrl = READ_REG(REG_MSG_CM3MBX_CTRL0);
	if(bx_ctrl&BIT0){
		VMBOX_INT_TOP2BT_CLR();
		VIPC_IntHandler(VIPC_DIR_2TP);
	}
	#endif
	#endif
}
		
vipc_info_t *VIPC_Info_Get(void)
{
    return &ipc_info_section.ipc_info;
}

vipc_mem_pool_t *VIPC_Pool_Get(void)
{
    return &ipc_info_section.ipc_info.pool;
}

static vipc_drv_mutex_t *vipc_global_drv_mutex = NULL;
static unsigned int vipc_global_drv_sr = 0;
int ipc_drvlocks_init(vipc_drv_mutex_t *drv_mutex)
{
    if (drv_mutex != NULL)
	{
        ipc_spin_lock_init(&drv_mutex->lock);
        vmemset(drv_mutex->drvs, 0, DRVS_LOCK_LIMIT*sizeof(VIPC_DRV_FLAG_TYPE));
        vipc_global_drv_sr = 0;
        vipc_global_drv_mutex = drv_mutex;
        return 0;
    }
	return -1;
}

BOOL ipc_drvlocks_check(UINT8 lock_numb, UINT8 offset)
{
	if (vipc_global_drv_mutex != NULL)
	{
		return (vipc_global_drv_mutex->drvs[lock_numb]>>offset) & 0x01;
	}

	return FALSE;
}

BOOL ipc_drvlocks_lock(UINT8 lock_numb, UINT8 offset)
{
    BOOL ret = FALSE;
	if (vipc_global_drv_mutex != NULL && !ipc_drvlocks_check(lock_numb, offset))
	{
		if (ipc_spin_lock_catch(&vipc_global_drv_mutex->lock, 2, &vipc_global_drv_sr))
        {
            if (!ipc_drvlocks_check(lock_numb, offset))
            {
                vipc_global_drv_mutex->drvs[lock_numb] |= (1<<offset);
                ret = TRUE;
            }
            ipc_spin_lock_release(&vipc_global_drv_mutex->lock, vipc_global_drv_sr);
        }
	}
	return ret;
}

BOOL ipc_drvlocks_unlock(UINT8 lock_numb, UINT8 offset)
{
    BOOL ret = TRUE;
	if (vipc_global_drv_mutex != NULL && ipc_drvlocks_check(lock_numb, offset))
    {
        ret = FALSE;
        if (ipc_spin_lock_catch(&vipc_global_drv_mutex->lock, 2, &vipc_global_drv_sr))
        {
            vipc_global_drv_mutex->drvs[lock_numb] &= ~(1<<offset);
            ret = TRUE;
            ipc_spin_lock_release(&vipc_global_drv_mutex->lock, vipc_global_drv_sr);
        }
	}
    return ret;
}
