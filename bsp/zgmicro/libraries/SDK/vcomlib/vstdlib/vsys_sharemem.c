
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
     share mem


Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vstdlib.h"
#include "vstdcore.h"
#include "vdev_ipc.h"

#define SHAREMEM_NAME_LEN   (4)
#define SHAREMEM_SIGNATURE	(0x72616873)


typedef struct vshareblk_info
{
	struct vshareblk_info* next;
	UINT32 size;
	UINT8  limits;
	UINT8  use;
	UINT8  reserve[2];
	SINT8  name[SHAREMEM_NAME_LEN];
} VShareMemBlk_Info;

typedef struct vsharemem_head
{
    UINT32	sharemem_signature;
	UINT8   name_len;
	UINT8   reserve[3];
    UINT32  num;
	UINT32  allsize;
	spin_lock_t  lock;
	VShareMemBlk_Info info;
} VShareMemBlk_Head;

#if (defined __GNUC__)
#define SHAREMEM_SECTION_UNINITIALIZED_TO_ZERO  __attribute__((section(".vdev_share_ram")))
#elif (defined __CC_ARM)
#define SHAREMEM_SECTION_UNINITIALIZED_TO_ZERO __attribute__((at(VDEV_SHARERAM_BASE), zero_init))
#endif

static UINT32 VSYS_Share_MEM_Pool[VDEV_SHARERAM_SIZE / 4] SHAREMEM_SECTION_UNINITIALIZED_TO_ZERO; 
static VShareMemBlk_Head *VSYS_Share_MEM_Head = (VShareMemBlk_Head *)VSYS_Share_MEM_Pool;

/// @brief 申请获取共享内存块（为静态申请，无法进行释放操作）
/// @param name 内存块名称（多核共享时，以此为依据绑定共享块）
/// @param Size 内存块的大小
/// @param mode 内存块所属权限 （top、bt、dsp 或关系）
/// @return 返回内存块地址
void *VShareMemInit(SINT8 *name, UINT32 Size, UINT8 mode)
{
    static UINT16 id_default = 1;
    unsigned int ipc_sr = 0;
    VShareMemBlk_Info *info = NULL, *info_front = NULL;
    SINT8 name_default[sizeof(info->name) + 1];

    if (ipc_spin_lock_catch(&VSYS_Share_MEM_Head->lock, 3, &ipc_sr))
	{
        if (VSYS_Share_MEM_Head->num) {
            if (name != NULL) {
                for (info = &VSYS_Share_MEM_Head->info; info != NULL; info = info->next) {
                    if (!vstrncmp(info->name, name, sizeof(info->name)) &&
                        (mode == SHAREMEM_PRIVATE ? info->limits == BIT_MASK(VCORE_GetID()) : info->limits & BIT_MASK(VCORE_GetID())))
                        break;
                    info_front = info;
                }
            }
        } else {
            VSYS_Share_MEM_Head->sharemem_signature = SHAREMEM_SIGNATURE;
            VSYS_Share_MEM_Head->allsize = sizeof(VSYS_Share_MEM_Pool);
            VSYS_Share_MEM_Head->name_len = SHAREMEM_NAME_LEN;
        }
        if (mode == SHAREMEM_PRIVATE) {
            mode = BIT_MASK(VCORE_GetID());
            if (name == NULL) {
                vcom_sprintf(name_default, "%d", id_default++);
                name = name_default;
            }
        }
        if (name != NULL) {
            if (info == NULL) {
                if (info_front == NULL)
                    info = &VSYS_Share_MEM_Head->info;
                else
                    info = (VShareMemBlk_Info *)((UINT32)info_front + sizeof(VShareMemBlk_Info) + ALIGN_SIZE(info_front->size, 4));
                if ((UINT32)info + sizeof(VShareMemBlk_Info) + ALIGN_SIZE(Size, 4) <= (UINT32)VSYS_Share_MEM_Head + sizeof(VSYS_Share_MEM_Pool)) {
                    info->next = NULL;
                    info->size = Size;
                    vstrncpy(info->name, name, sizeof(info->name));
                    info->limits = mode;
                    info->use = BIT_MASK(VCORE_GetID());
                    if (info_front != NULL)
                        info_front->next = info;
                    VSYS_Share_MEM_Head->num++;
                } else
                    info = NULL;
            } else {
                info->use |= BIT_MASK(VCORE_GetID());
            }
        }
        ipc_spin_lock_release(&VSYS_Share_MEM_Head->lock, ipc_sr);
	}
    ASSERT(info != NULL);
    if (info != NULL){
        if (info->size != Size) {
            ERROR_PRINTF("%s %s 0x%08x exist size:%d malloc size:%d\r\n", __FUNC__, name, (UINT32)info + sizeof(VShareMemBlk_Info), info->size, Size);
            ASSERT(0);
        } else if (info->limits != mode) {
            ERROR_PRINTF("%s %s 0x%08x exist size:%d malloc size:%d\r\n", __FUNC__, name, (UINT32)info + sizeof(VShareMemBlk_Info), info->limits, mode);
            ASSERT(0);
        }
    }
    return (info ? (void *)((UINT32)info + sizeof(VShareMemBlk_Info)) : NULL);
}

/// @brief 根据名称获取共享内存块
/// @param name 想要获取内存块的名称
/// @param Size 返回获取到的内存块大小
/// @param mode 返回获取到的内存块所属权限
/// @return 返回获取到的内存块地址
void *VShareMemGet(SINT8 *name, UINT32 *Size, UINT8 *mode)
{
    unsigned int ipc_sr = 0;
    VShareMemBlk_Info *info = NULL;

    if (ipc_spin_lock_catch(&VSYS_Share_MEM_Head->lock, 3, &ipc_sr))
	{
        if (VSYS_Share_MEM_Head->num) {
            for (info = &VSYS_Share_MEM_Head->info; info != NULL; info = info->next) {
                if (name && !vstrncmp(info->name, name, sizeof(info->name)) && info->limits & BIT_MASK(VCORE_GetID()))
                    break;
            }
        }
        if (info != NULL) {
            info->use |= BIT_MASK(VCORE_GetID());
            if (Size != NULL)
                *Size = info->size;
            if (mode != NULL)
                *mode = info->limits;
        }
        ipc_spin_lock_release(&VSYS_Share_MEM_Head->lock, ipc_sr);
	}

    return (info ? (void *)((UINT32)info + sizeof(VShareMemBlk_Info)) : NULL);
}

/// @brief 获取内存块的名称
/// @param addr 内存块地址
/// @return 返回内存块名称
const SINT8 *VShareMemGetName(void * addr)
{
    if (addr == NULL) {
        return 0;
    }

    VShareMemBlk_Info *info = (VShareMemBlk_Info *)((UINT8 *)addr - sizeof(VShareMemBlk_Info));
    
    if(info != NULL)
    {
        return info->name;
    }
    return NULL;
}

/// @brief 获取内存块的大小
/// @param addr 内存块地址
/// @return 返回内存块的大小
UINT32 VShareMemGetSize(void * addr)
{
    if (addr == NULL) {
        return 0;
    }

    VShareMemBlk_Info *info = (VShareMemBlk_Info *)((UINT8 *)addr - sizeof(VShareMemBlk_Info));
    
    if(info != NULL)
    {
        return info->size;
    }
    return 0;
}

/// @brief 获取内存块的权限
/// @param addr 内存块地址
/// @return 返回内存块的权限
UINT32 VShareMemGetlimits(void * addr)
{
    if (addr == NULL) {
        return 0;
    }

    VShareMemBlk_Info *info = (VShareMemBlk_Info *)((UINT8 *)addr - sizeof(VShareMemBlk_Info));
    
    if(info != NULL)
    {
        return info->limits;
    }
    return 0;
}

/// @brief 获取内存块的使用者
/// @param addr 内存块地址
/// @return 返回内存块的使用者
UINT32 VShareMemGetUse(void * addr)
{
    if (addr == NULL) {
        return 0;
    }

    VShareMemBlk_Info *info = (VShareMemBlk_Info *)((UINT8 *)addr - sizeof(VShareMemBlk_Info));
    
    if(info != NULL)
    {
        return info->use;
    }
    return 0;
}

