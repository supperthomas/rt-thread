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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstdos.h"
#include "vdrv_dma.h"
#include "vdrv_mbox.h"
#include "vpla_box.h"
#include "vdev_ipc.h"
#ifdef TEST_BSP_MODE
#include "vpla_dsp.h"
#endif

#define VMBOX_SR_ALLOC	VCPU_SR_ALLOC
#define VMBOX_CRITICAL_ENTER VCPU_CRITICAL_ENTER
#define VMBOX_CRITICAL_EXIT VCPU_CRITICAL_EXIT

#if VBOX_CORE_IS_MASTER
static vbox_info_t selfbox; //__attribute__((at(VBOX_BOXREG_MASTER)));
static volatile vbox_info_t otherbox __attribute__((at(VBOX_BOXREG_SLAVE)));
#define VBOXUPDATE_SELFREG(x) (WRITE_REG(VBOX_BOXREG_MASTER + 4 * (x), *((UINT32 *)&selfbox + (x))))
#define VBOXUPDATE_OTHERREG(x)
static UINT32 Txcache[VBOX_TXCACHE_LEN / 4] = {0};
static UINT32 Rxcache[VBOX_RXCACHE_LEN / 4] = {0};
#else
static vbox_info_t selfbox; //__attribute__((at(VBOX_BOXREG_SLAVE)));
static vbox_info_t otherbox __attribute__((at(VBOX_BOXREG_MASTER)));
#define VBOXUPDATE_SELFREG(x) (WRITE_REG(VBOX_BOXREG_SLAVE + 4 * (x), *((UINT32 *)&selfbox + (x))))
#define VBOXUPDATE_OTHERREG(x)
#endif

VBOX_LOCK_HANDLE;

static vbox_msg_t boxTx = {0};
static vbox_msg_t boxRx = {0};
static UINT8 Tx_index = 0,Rx_index = 0, disable_flag = 0;

#define VBOXMSGFORMA_LEN VBOX_DATALEN_CONVERT(sizeof(vbox_msg_forma_t))

SINT32 VBOX_Init(void)
{
#if VBOX_CORE_IS_MASTER
	UINT16 msg[4] = {4, 255, VBOX_OUT_BUFF_SIZE, VBOX_IN_BUFF_SIZE};
	Tx_index = 0;
	Rx_index = 0;
	vmemset(&selfbox, 0, sizeof(vbox_info_t));
	boxTx.base = VBOX_SLAVE_BUFF_BASE;
	boxTx.depth = VBOX_DATALEN_CONVERT(VBOX_OUT_BUFF_SIZE);
	boxTx.cache = Txcache;
	boxTx.cache_len = VBOX_DATALEN_CONVERT(sizeof(Txcache));
	boxTx.cache_offset = 0;
	boxRx.base = boxTx.base + boxTx.depth;
	boxRx.depth = VBOX_DATALEN_CONVERT(VBOX_IN_BUFF_SIZE);
	boxRx.cache = Rxcache;
	boxRx.cache_len = VBOX_DATALEN_CONVERT(sizeof(Rxcache));
	boxRx.cache_offset = 0;
	VBOX_ASSERT(((UINT32)boxTx.cache & 3) == 0 && ((UINT32)boxRx.cache & 3) == 0);
	VBOX_SetData(0, &msg, sizeof(msg));
	selfbox.writep = 2;
	VBOX_DebugEn(TRUE);
#else
	UINT16 msg[4] = {0};
	VBOX_SetReset(TRUE);
	while(VBOX_GetReset()){};
	vmemset((void*)&selfbox, 0, sizeof(vbox_info_t));
	boxRx.base = (UINT32*)VBOX_SLAVE_BUFF_BASE;
	VBOX_ASSERT(((UINT32)boxRx.cache & 3) == 0);
	do
	{
		VBOXUPDATE_OTHERREG(1);
	}while (otherbox.writep < 2);
	VBOX_GetData(0, msg, sizeof(msg));
	if (msg[0] != 4 || msg[1] != 255)
	{
		return -1;
	}
	selfbox.readp = VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(sizeof(msg)));
	selfbox.rdreq = 1;

	boxRx.depth = VBOX_DATALEN_CONVERT(msg[2]);
	boxRx.cache = NULL;
	boxTx.base = boxRx.base + boxRx.depth;
	boxTx.depth = VBOX_DATALEN_CONVERT(msg[3]);
	boxTx.cache = NULL;
	vmemset(boxRx.base, 0 ,VBOX_OFFSET_CONVERT(boxRx.depth));
	vmemset(boxTx.base, 0 ,VBOX_OFFSET_CONVERT(boxTx.depth));
	Tx_index = 0;
	Rx_index = 0;
#endif
	VBOXUPDATE_SELFREG(0);
	VBOXUPDATE_SELFREG(1);
	VBOXUPDATE_SELFREG(2);
	VBOX_LOCK_INIT();
	return 0;
}

SINT32 VBOX_Uninit(void)
{
#if VBOX_CORE_IS_MASTER
	if(VBOX_GetTxBufEn() || disable_flag)
	{
		disable_flag = 0;
		VBOX_SetReset(TRUE);
		while(!VBOX_GetReset()){}
	}
	boxTx.cache_len = 0;
	boxRx.cache_len = 0;
	vmemset((void*)&selfbox, 0, sizeof(vbox_info_t));
	VBOXUPDATE_SELFREG(1);
	VBOXUPDATE_SELFREG(2);
	VBOXUPDATE_SELFREG(0);
#else
	vmemset((void*)&selfbox, 0, sizeof(vbox_info_t));
	vmemset(boxRx.base, 0 ,VBOX_OFFSET_CONVERT(boxRx.depth));
	vmemset(boxTx.base, 0 ,VBOX_OFFSET_CONVERT(boxTx.depth));
	VBOXUPDATE_SELFREG(0);
	VBOXUPDATE_SELFREG(1);
	VBOXUPDATE_SELFREG(2);
#endif

	
	VBOX_LOCK_UNINIT();
	return 0;
}

SINT32 VBOX_Disable(void)
{
#if VBOX_CORE_IS_MASTER
	if(VBOX_GetTxBufEn())
	{
		VBOX_SetReset(TRUE);
		disable_flag = 1;
	}
	boxTx.cache_len = 0;
	boxRx.cache_len = 0;
#else

#endif

	VBOX_LOCK_UNINIT();
	return 0;
}

void VBOX_HW_Signal(void)
{
#if VBOX_CORE_IS_MASTER
	VIPC_HW_Signal((vipc_dir_typ_t)VBOX_CORE_SLAVE);
#else
	VIPC_HW_Signal((vipc_dir_typ_t)VBOX_CORE_MASTER);
#endif
}

/*******************************event******************************************/
UINT16 VBOX_EvtSet(UINT16 evt)
{
	UINT16 event, othermask;
	VBOX_ASSERT(evt <= 0xFFFF);

	VMBOX_SR_ALLOC();
	VMBOX_CRITICAL_ENTER();
	VBOXUPDATE_OTHERREG(0);
	othermask = otherbox.mask;
	selfbox.event = (selfbox.event & ~evt) | ((othermask ^ evt) & evt);
	event = selfbox.event ^ othermask;
	VBOXUPDATE_SELFREG(0);
	VMBOX_CRITICAL_EXIT();

	VBOX_HW_Signal();

	return event;
}

UINT16 VBOX_EvtGet(UINT16 evt)
{
	UINT16 event, otherevent;

	VMBOX_SR_ALLOC();
	VMBOX_CRITICAL_ENTER();
	VBOXUPDATE_OTHERREG(0);
	otherevent = otherbox.event;
	event = otherevent ^ selfbox.mask;
	event &= evt;
	selfbox.mask = (selfbox.mask & ~event) | (otherevent & event);
	VBOXUPDATE_SELFREG(0);
	VMBOX_CRITICAL_EXIT();

	return event;
}

/*******************************msg********************************************/
UINT32 VBOX_GetFreeTxBufLen(void)
{
	UINT32 off;
	UINT16 WrOff, RdOff;
	VBOXUPDATE_OTHERREG(1);
	WrOff = selfbox.writep;
	RdOff = otherbox.readp;
	if (RdOff > WrOff)
	{
		off = RdOff - WrOff - 1u;
	}
	else
	{
		off = boxTx.depth - (WrOff - RdOff + 1u);
	}
	return off;
}

UINT32 VBOX_GetValidRxBufLen(void)
{
	UINT32 off;
	UINT16 WrOff, RdOff;
	VBOXUPDATE_OTHERREG(1);
	RdOff = selfbox.readp;
	WrOff = otherbox.writep;
	if (WrOff >= RdOff)
	{
		off = WrOff - RdOff;
	}
	else
	{
		off = boxRx.depth - (RdOff - WrOff);
	}
	return off;
}

#if VBOX_CORE_IS_MASTER

UINT32 VBOX_GetFreeTxBufLen1(void)
{
	UINT32 off;
	UINT16 WrOff, RdOff;
	WrOff = selfbox.writep;
	RdOff = otherbox.readp;
	if (RdOff > WrOff)
	{
		off = RdOff - WrOff - 1u;
	}
	else
	{
		off = boxTx.depth - WrOff;
		if (RdOff == 0)
		{
			off -= 1;
		}
	}
	return off;
}

UINT32 VBOX_GetFreeTxBufLen2(void)
{
	UINT32 off = 0;
	UINT16 WrOff, RdOff;
	WrOff = selfbox.writep;
	RdOff = otherbox.readp;
	if (RdOff <= WrOff && RdOff != 0)
	{
		off = RdOff - 1;
	}
	return off;
}

UINT32 VBOX_GetValidRxBufLen1(void)
{
	UINT32 off;
	UINT16 WrOff, RdOff;
	RdOff = selfbox.readp;
	WrOff = otherbox.writep;
	if (WrOff >= RdOff)
	{
		off = WrOff - RdOff;
	}
	else
	{
		off = boxRx.depth - RdOff;
	}
	return off;
}

UINT32 VBOX_GetValidRxBufLen2(void)
{
	UINT32 off = 0;
	UINT16 WrOff, RdOff;
	RdOff = selfbox.readp;
	WrOff = otherbox.writep;
	if (WrOff < RdOff)
	{
		off = WrOff;
	}
	return off;
}

UINT32 VBOX_SetData(UINT32 offset, void *SrcAddr, UINT32 len)
{
	UINT32 size = VBOX_OFFSET_CONVERT(len);
#ifdef TEST_BSP_MODE
	VSYS_DSP_WriteDM(CODEC_DSP_XMEM, SrcAddr, boxTx.base + offset, size);
#else
	vmemcpy(boxTx.base + offset, SrcAddr, size);
#endif
	return 0;
}

UINT32 VBOX_GetData(UINT32 offset, void *DsetAddr, UINT32 len)
{
	UINT32 size = VBOX_OFFSET_CONVERT(len);
#ifdef TEST_BSP_MODE
	VSYS_DSP_ReadDM(CODEC_DSP_XMEM, DsetAddr, boxRx.base + offset, size);
#else
	vmemcpy(DsetAddr, boxRx.base + offset, size);
#endif
	return 0;
}

void VBOX_SetDataTxBuf(void *addr, UINT32 len)
{
	UINT32 lent;
	VBOX_KeepRun(TRUE);
	lent = MIN(boxTx.depth - selfbox.writep, len);
	if (lent != 0)
	{
		VBOX_SetData(selfbox.writep, addr, lent);
		if (lent < len)
		{
			VBOX_SetData(0, (UINT32 *)addr + lent, len - lent);
		}
	}
	VBOX_KeepRun(FALSE);
}

void VBOX_GetDataRxBuf(void *addr, UINT32 len)
{
	UINT32 lent;
	VBOX_KeepRun(TRUE);
	lent = MIN(boxRx.depth - selfbox.readp, len);
	if (lent != 0)
	{
		VBOX_GetData(selfbox.readp, addr, lent);
		if (lent < len)
		{
			VBOX_GetData(0, (UINT32 *)addr + lent, len - lent);
		}
	}
	VBOX_KeepRun(FALSE);
}

UINT32 VBOX_SetDataTxCache(void *addr, UINT32 size)
{
	UINT32 sizet;

	sizet = MIN(VBOX_OFFSET_CONVERT(boxTx.cache_len - boxTx.cache_offset), size);
	if (sizet > 0)
	{
		vmemcpy(boxTx.cache + boxTx.cache_offset, addr, sizet);
		boxTx.cache_offset += VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(sizet));
	}
	return sizet;
}

UINT32 VBOX_GetDataTxCache(void *addr, UINT32 size)
{
	UINT32 sizet, offset;

	sizet = MIN(VBOX_OFFSET_CONVERT(boxTx.cache_offset), size);
	offset = VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(sizet));
	boxTx.cache_offset -= offset;
	if (boxTx.cache_offset != 0)
	{
		vmemcpy(boxTx.cache, boxTx.cache + offset, VBOX_OFFSET_CONVERT(boxTx.cache_offset));
	}
	if (addr != NULL)
	{
		vmemcpy(addr, boxTx.cache, sizet);
	}
	return sizet;
}

UINT32 VBOX_SetDataRxCache(void *addr, UINT32 size)
{
	UINT32 sizet;

	sizet = MIN(VBOX_OFFSET_CONVERT(boxRx.cache_len - boxRx.cache_offset), size);
	if (sizet > 0)
	{
		if (addr != NULL)
		{
			vmemcpy(boxRx.cache + boxRx.cache_offset, addr, sizet);
		}
		boxRx.cache_offset += VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(sizet));
	}
	return sizet;
}

UINT32 VBOX_GetDataRxCache(void *addr, UINT32 size)
{
	UINT32 sizet, offset;

	sizet = MIN(VBOX_OFFSET_CONVERT(boxRx.cache_offset), size);
	offset = VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(sizet));
	boxRx.cache_offset -= offset;
	if (addr != NULL)
	{
		vmemcpy(addr, boxRx.cache, sizet);
	}
	if (boxRx.cache_offset != 0)
	{
		vmemcpy(boxRx.cache, boxRx.cache + offset, VBOX_OFFSET_CONVERT(boxRx.cache_offset));
	}
	
	return sizet;
}

void VBOX_TxCacheUpdate(void)
{
	VBOX_LOCK_LOCK();
	if (boxTx.cache_offset > 0 && VBOX_GetTxBufEn())
	{
		UINT16 freelen2 = VBOX_GetFreeTxBufLen2();
		UINT16 freelen1 = VBOX_GetFreeTxBufLen1();
		if (boxTx.cache_offset <= freelen1)
		{
			VBOX_SetDataTxBuf(boxTx.cache, boxTx.cache_offset);
			VBOX_SetTxBufPrt(boxTx.cache_offset);
			boxTx.cache_offset = 0;
		}
		else if (boxTx.cache_offset <= freelen2)
		{
			selfbox.writep = 0;
			VBOX_SetDataTxBuf(boxTx.cache, boxTx.cache_offset);
			VBOX_SetTxBufPrt(boxTx.cache_offset);
			boxTx.cache_offset = 0;
		}
		else
		{
			VBOX_SetDataTxBuf(boxTx.cache, boxTx.cache_offset);
			VBOX_SetTxBufPrt(boxTx.cache_offset);
			boxTx.cache_offset = 0;
		}
	}
	VBOX_LOCK_UNLOCK();
}

#else

UINT32 VBOX_SetData(UINT32 offset, void *SrcAddr, UINT32 len)
{
	vmemcpy(boxTx.base + offset, SrcAddr, len);

	return 0;
}

UINT32 VBOX_GetData(UINT32 offset, void *DsetAddr, UINT32 len)
{
	vmemcpy(DsetAddr, boxRx.base + offset, len);

	return 0;
}

void VBOX_SetDataTxBuf(void *addr, UINT32 len)
{
	UINT32 lent;

	lent = MIN(VBOX_OFFSET_CONVERT(boxTx.depth - selfbox.writep), len);
	if (lent != 0)
	{
		VBOX_SetData(selfbox.writep, addr, lent);
		if (lent < len)
		{
			VBOX_SetData(0, (UINT8 *)addr + lent, len - lent);
		}
	}
}

void VBOX_GetDataRxBuf(void *addr, UINT32 len)
{
	UINT32 lent;

	lent = MIN(VBOX_OFFSET_CONVERT(boxTx.depth - selfbox.readp), len);
	if (lent != 0)
	{
		VBOX_GetData(selfbox.readp, addr, lent);
		if (lent < len)
		{
			VBOX_GetData(0, (UINT8 *)addr + lent, len - lent);
		}
	}
}

UINT32 VBOX_CheckRxBufHead(UINT8 index)
{
	UINT16 WrOff, RdOff;
	VBOXUPDATE_OTHERREG(1);
	RdOff = selfbox.readp;
	WrOff = otherbox.writep;
	if (WrOff < RdOff && WrOff != 0)
	{
		vbox_msg_forma_t *boxmsg = (vbox_msg_forma_t *)(boxRx.base);
		if ((UINT8)~boxmsg->magic == boxmsg->index && boxmsg->index == index)
		{
			selfbox.readp = 0;
			return boxmsg->size | (boxmsg->id << 16);
		}
	}
	vbox_msg_forma_t boxmsg;
	VBOX_GetDataRxBuf(&boxmsg, sizeof(vbox_msg_forma_t));
	VBOX_ASSERT((UINT8)~boxmsg.magic == boxmsg.index && boxmsg.index == index);
	return boxmsg.size | (boxmsg.id << 16);
}

#endif

void VBOX_SetTxBufPrt(UINT16 off)
{
	if (selfbox.writep + off < boxTx.depth)
	{
		selfbox.writep += off;
	}
	else
	{
		selfbox.writep = selfbox.writep + off - boxTx.depth;
	}
	VBOXUPDATE_SELFREG(1);
}

void VBOX_SetRxBufPrt(UINT16 off)
{
	if (selfbox.readp + off < boxRx.depth)
	{
		selfbox.readp += off;
	}
	else
	{
		selfbox.readp = selfbox.readp + off - boxRx.depth;
	}
	VBOXUPDATE_SELFREG(1);
}

void VBOX_SetTxBufEn(BOOL en)
{
	if (en)
	{
		selfbox.wrnen = 0;
	}
	else
	{
		selfbox.wrnen = 1;
	}

	VBOXUPDATE_SELFREG(1);
}

BOOL VBOX_GetTxBufEn(void)
{
#if VBOX_CORE_IS_MASTER
	if(boxTx.cache_len == 0)
		return FALSE;
	VBOXUPDATE_OTHERREG(1);
	if (otherbox.rdnen != selfbox.wrnen)
	{
		selfbox.wrnen = otherbox.rdnen;
		VBOXUPDATE_SELFREG(1);
	}
	return otherbox.rdnen ? FALSE : TRUE;
#else
	return TRUE;
#endif
}

void VBOX_SetRxBufEn(BOOL en)
{
	if (en)
	{
		selfbox.rdnen = 0;
	}
	else
	{
		selfbox.rdnen = 1;
	}
	VBOXUPDATE_SELFREG(1);
}

BOOL VBOX_GetRxBufEn(void)
{
#if VBOX_CORE_IS_MASTER
	if(boxRx.cache_len == 0)
		return FALSE;
	VBOXUPDATE_OTHERREG(1);
	if (otherbox.wrnen != selfbox.rdnen)
	{
		selfbox.rdnen = otherbox.wrnen;
		VBOXUPDATE_SELFREG(1);
	}
	return otherbox.wrnen ? FALSE : TRUE;
#else
	return TRUE;
#endif
}

BOOL VBOX_GetTxReq(void)
{
	return (otherbox.rdreq) ? TRUE : FALSE;
}

void VBOX_Interrupt_Routine_Service_demo(void)
{
#if VBOX_CORE_IS_MASTER
	VMBOX_INT_BT2TOP_CLR();
	if (selfbox.mask ^ otherbox.event)
	{
		// event
	}
	if (otherbox.rdreq)
	{
		// VOSFlagSet(evt, 0x01); // vbox_queue_cacheupdate(); // TODO: execute in the task
	}
#else
	VMBOX_INT_TOP2BT_CLR();
#endif
}

//-------------------------------------
SINT32 vbox_queue_send_static(UINT16 id, void *msg, UINT16 size)
{
	SINT32 ret = 0;
	static UINT8 reentry = 0;
	VBOX_LOCK_LOCK();
	if (!VBOX_GetTxBufEn() || size == 0)
	{
		VBOX_LOCK_UNLOCK();
		return -1;
	}
	UINT16 msg_len = VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(size));
	reentry++;
#if VBOX_CORE_IS_MASTER
	VBOX_ASSERT(msg_len + VBOXMSGFORMA_LEN <= boxTx.cache_len);
	UINT16 freelen = VBOX_GetFreeTxBufLen();
	UINT16 freelen2 = VBOX_GetFreeTxBufLen2();
	UINT16 freelen1 = VBOX_GetFreeTxBufLen1();
	if (freelen >= boxTx.cache_offset + msg_len + VBOXMSGFORMA_LEN)
	{
		vbox_msg_forma_t boxmsg;
		boxmsg.size = size;
		boxmsg.index = Tx_index++;
		boxmsg.magic = ~boxmsg.index;
		boxmsg.id = id;
#if VBOX_CACHEDELAY_SUPPORT
		if ((boxTx.cache_len - boxTx.cache_offset < msg_len + VBOXMSGFORMA_LEN) ||
			(freelen2 != 0 && freelen1 >= boxTx.cache_offset && freelen1 < boxTx.cache_offset + msg_len + VBOXMSGFORMA_LEN))
		{
			VBOX_TxCacheUpdate();
		}
#endif
		VBOX_SetDataTxCache(&boxmsg, sizeof(vbox_msg_forma_t));
		VBOX_SetDataTxCache((UINT8 *)msg, size);
#if VBOX_CACHEDELAY_SUPPORT
		if (otherbox.rdreq == 1)
#endif
		{
			VBOX_TxCacheUpdate();
			VBOX_HW_Signal();
		}
	}
	else
	{
#if VBOX_CACHEDELAY_SUPPORT
		// VBOX_TxCacheUpdate();
#endif
#else
	if (msg_len + VBOXMSGFORMA_LEN <= VBOX_GetFreeTxBufLen())
	{
		vbox_msg_forma_t boxmsg;
		boxmsg.size = size;
		boxmsg.index = Tx_index++;
		boxmsg.magic = ~boxmsg.index;
		boxmsg.id = id;
		VBOX_SetDataTxBuf(&boxmsg, sizeof(vbox_msg_forma_t));
		VBOX_SetTxBufPrt(VBOXMSGFORMA_LEN);
		VBOX_SetDataTxBuf(msg, size);
		VBOX_SetTxBufPrt(msg_len);
		if (otherbox.rdreq == 1)
		{
			VBOX_HW_Signal();
		}
	}
	else
	{
#endif
		ret = -1;
	}
	reentry--;
	VBOX_ASSERT(reentry == 0);
	VBOX_LOCK_UNLOCK();
	return ret;
}

SINT32 vbox_queue_send(UINT16 id, void *msg, UINT16 size)
{
	SINT32 ret = vbox_queue_send_static(id, msg, size);
	if (0 == ret)
	{
		vfree(msg);
	}
	return ret;
}

UINT8 *vbox_queue_recv_static(UINT16 *id, UINT8 *addr, UINT16 addr_len)
{
	static UINT8 reentry = 0;
	UINT8 *msg = NULL;
	VBOX_LOCK_LOCK();
	if (!VBOX_GetRxBufEn())
	{
		VBOX_LOCK_UNLOCK();
		return NULL;
	}
	reentry++;
#if VBOX_CORE_IS_MASTER
	UINT16 msg_sizeoff = 0, msg_size = 0, real_size = 0, rx_read;
	do
	{
		if (boxRx.cache_offset < 3)
		{
			UINT16 vallen2 = VBOX_GetValidRxBufLen2();
			UINT16 vallen1 = VBOX_GetValidRxBufLen1();
			if (vallen1 > 0)
			{
				rx_read = MIN(boxRx.cache_len - boxRx.cache_offset, vallen1);
				VBOX_GetDataRxBuf(boxRx.cache + boxRx.cache_offset, rx_read);
				boxRx.cache_offset += rx_read;
				VBOX_SetRxBufPrt(rx_read);
			}
			else if (vallen2 > 0)
			{
				rx_read = MIN(boxRx.cache_len - boxRx.cache_offset, vallen2);
				VBOX_GetDataRxBuf(boxRx.cache + boxRx.cache_offset, rx_read);
				boxRx.cache_offset += rx_read;
				VBOX_SetRxBufPrt(rx_read);
			}
		}
		if (boxRx.cache_offset > 2 || msg_size != 0)
		{
			if (real_size == 0)
			{
				UINT16 rev_size, offset;
				vbox_msg_forma_t *boxmsg = (vbox_msg_forma_t *)(boxRx.cache);
				
				selfbox.rdreq = 0;
				VBOX_ASSERT((UINT8)~boxmsg->magic == boxmsg->index && boxmsg->index == Rx_index);
				
				real_size = msg_size = boxmsg->size;
				VBOX_ASSERT(msg_size != 0 && VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(msg_size)) + VBOXMSGFORMA_LEN <= boxRx.depth);

				if (addr == NULL)
				{
					msg = vmalloc(boxmsg->size);
					if (msg == NULL)
					{
						break;
					}
				}
				else
				{
					msg = addr;
					msg_size = MIN(msg_size, addr_len);
				}
				if (id != NULL)
				{
					*id = boxmsg->id;
				}
				Rx_index++;
				VBOX_GetDataRxCache(NULL, VBOX_OFFSET_CONVERT(VBOXMSGFORMA_LEN));
			}
			if (msg_size > msg_sizeoff)
			{
				msg_sizeoff += VBOX_GetDataRxCache(msg + msg_sizeoff, msg_size - msg_sizeoff);
			}
			else
			{
				msg_sizeoff += VBOX_GetDataRxCache(NULL, real_size - msg_sizeoff);
			}
		}
	} while (msg_sizeoff < real_size);
	if (real_size == 0)
	{
		selfbox.rdreq = 1;
	}
#else
	UINT32 head;
	UINT16 size, vallen, msg_size;
	while (1)
	{
		vallen = VBOX_GetValidRxBufLen();
		if (vallen > 2)
		{
			head = VBOX_CheckRxBufHead(Rx_index);
			msg_size = size = head & 0xFFFF;
			VBOX_ASSERT(size <= VBOX_OFFSET_CONVERT(boxRx.depth));

			if (vallen < VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(size)) + VBOXMSGFORMA_LEN)
			{
				if (selfbox.rdreq == 0)
				{
					selfbox.rdreq = 1;
					VBOXUPDATE_SELFREG(1);
					VBOX_HW_Signal();
				}
				break;
			}
			else if (msg == NULL)
			{
				if (addr == NULL)
				{
					msg = (UINT8 *)vmalloc(size);
					if (msg == NULL)
					{
						break;
					}
				}
				else
				{
					msg = addr;
					msg_size = MIN(msg_size, addr_len);
				}
				if (id != NULL)
				{
					*id = (head >> 16) & 0xFFFF;
				}
				Rx_index++;
				VBOX_SetRxBufPrt(VBOXMSGFORMA_LEN);
				VBOX_GetDataRxBuf(msg, msg_size);
				VBOX_SetRxBufPrt(VBOX_DATALEN_CONVERT(VBOX_ALIGN_SIZE(size)));
			}
			else
			{
				selfbox.rdreq = 0;
				VBOXUPDATE_SELFREG(1);
				break;
			}
		}
		else
		{
			if (selfbox.rdreq == 0)
			{
				selfbox.rdreq = 1;
				VBOXUPDATE_SELFREG(1);
				VBOX_HW_Signal();
			}
			break;
		}
	}
#endif
	reentry--;
	VBOX_ASSERT(reentry == 0);
	VBOX_LOCK_UNLOCK();
	return msg;
}

UINT8 *vbox_queue_recv(UINT16 *id)
{
	return vbox_queue_recv_static(id, NULL, 0);
}

/*******************************state******************************************/
UINT32 VBOX_StateSet(UINT32 state)
{

	selfbox.state |= state;
	VBOXUPDATE_SELFREG(2);
	VBOX_HW_Signal();
	return selfbox.state;
}

UINT32 VBOX_StateWrite(UINT32 state)
{

	selfbox.state = state;
	VBOXUPDATE_SELFREG(2);

	return selfbox.state;
}

UINT32 VBOX_StateClr(UINT32 state)
{

	selfbox.state &= ~state;
	VBOXUPDATE_SELFREG(2);

	return selfbox.state;
}

UINT32 VBOX_StateGet(void)
{	
	VBOXUPDATE_OTHERREG(2);
	return otherbox.state;
}

/*******************************state******************************************/
#if VBOX_CORE_IS_MASTER
void VBOX_KeepRun(BOOL en)
{
	if(en)
	{
		selfbox.control |= 0x01;
		VBOXUPDATE_SELFREG(2);
		VBOX_HW_Signal();
	}
#if VBOX_DSP_SLEEP_SUPPORT
	else
	{
		selfbox.control &= ~0x01;
		VBOXUPDATE_SELFREG(2);
	}
#endif
}

void VBOX_DebugEn(BOOL en)
{
	if(en)
	{
		selfbox.control |= 0x04;
		VBOXUPDATE_SELFREG(2);
	}
	else
	{
		selfbox.control &= ~0x04;
		VBOXUPDATE_SELFREG(2);
	}
}
#else
BOOL VBOX_GetKeepRun(void)
{
	VBOXUPDATE_OTHERREG(2);
	return otherbox.control&0x01?TRUE:FALSE;
}

BOOL VBOX_GetDebug(void)
{
	VBOXUPDATE_OTHERREG(2);
	return otherbox.control&0x04?TRUE:FALSE;;
}
#endif

void VBOX_SetReset(BOOL en)
{
	if(en)
	{
		selfbox.control |= 0x02;
		VBOXUPDATE_SELFREG(2);
		VBOX_HW_Signal();
	}
	else
	{
		selfbox.control &= ~0x02;
		VBOXUPDATE_SELFREG(2);
	}
}

BOOL VBOX_GetReset(void)
{
	VBOXUPDATE_OTHERREG(2);
	return otherbox.control&0x02?TRUE:FALSE;;
}

/*******************************debug******************************************/
void VBOX_Debug(void)
{
	static UINT8 read_buff[100] = {0};
	static volatile UINT32 read_addr;
	static volatile UINT8 read_flag = 0;
	VBOX_KeepRun(TRUE);
	while(read_flag)
		VBOX_GetData(read_addr, read_buff, sizeof(read_buff)); 
	VBOX_KeepRun(FALSE);
}
