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
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif

#ifndef _VPLA_BOX_H_
#define _VPLA_BOX_H_

#include "vstdlib.h"
#include "vstdcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VBOX_DSP_SLEEP_SUPPORT  (1)

#define VBOX_LOCK_HANDLE
#define VBOX_LOCK_INIT()
#define VBOX_LOCK_LOCK()
#define VBOX_LOCK_UNLOCK()
#define VBOX_LOCK_UNINIT()

#ifdef CORE_SEL_MCUTOP
#define VBOX_CORE_IS_MASTER (1)
#define VBOX_TXCACHE_LEN (1024 * 2)
#define VBOX_RXCACHE_LEN (1024 * 2)
#else
#define VBOX_CORE_IS_MASTER (0)
#endif

#define VBOX_CACHEDELAY_SUPPORT (1)

#ifdef TEST_BSP_MODE
#define VBOX_CORE_MASTER (CPU_ID_TOP)
#define VBOX_CORE_SLAVE (CPU_ID_DSP)
#define VBOX_BOXREG_MASTER REG_MSG_CM3TOP_MSG0
#define VBOX_BOXREG_SLAVE REG_MSG_DSP_MSG0
#define VBOX_SLAVE_BUFF_BASE ((void *)(12 * 1024 + 64 * 1024))
#else
#define VBOX_CORE_MASTER (CPU_ID_TOP)
#define VBOX_CORE_SLAVE (CPU_ID_BT)
#define VBOX_BOXREG_MASTER REG_MSG_CM3TOP_MSG0
#define VBOX_BOXREG_SLAVE REG_MSG_CM3BT_MSG0
#define VBOX_SLAVE_BUFF_BASE ((void *)VDEV_IPC_RAM_BASE)
#endif

#define VBOX_IN_BUFF_SIZE (4 * 1024)
#define VBOX_OUT_BUFF_SIZE (4 * 1024)

#define VBOX_ALIGN_SIZE(size) (((size) + 3) & ~0x3)
#define VBOX_OFFSET_CONVERT(offset) ((UINT32)(offset) << 2)
#define VBOX_DATALEN_CONVERT(size) ((UINT16)((size) >> 2))

#define VBOX_ASSERT(expr)                                                  \
	do                                                                     \
	{                                                                      \
		if (!(expr))                                                       \
		{                                                                  \
			ERROR_PRINTF("%s %d %s\r\n", MODULE_NAME, LINE_NUMBER, #expr); \
			while (1)                                                      \
			{                                                              \
				extern void VBOX_Debug(void);                              \
				VBOX_Debug();                                              \
			}                                                              \
		}                                                                  \
	} while (0)

#pragma pack(1)
#pragma anon_unions
	/// @brief 定义一个数据结构，用于存储事件和状态信息
	typedef struct
	{
		/// @brief 事件标识
		UINT16 event;
		/// @brief 掩码，用于事件过滤
		UINT16 mask;
		/// @brief 写指针和控制标志位字段
		struct
		{
			/// @brief 写指针
			UINT16 writep : 14;
			/// @brief 写请求标志
			UINT16 wrreq : 1;
			/// @brief 写使能标志
			UINT16 wrnen : 1;
		};
		/// @brief 读指针和控制标志位字段
		struct
		{
			/// @brief 读指针
			UINT16 readp : 14;
			/// @brief 读请求标志
			UINT16 rdreq : 1;
			/// @brief 读使能标志
			UINT16 rdnen : 1;
		};
		/// @brief 状态信息
		UINT32 state:24;
		/// @brief 控制信息
		UINT32 control:8;
	} vbox_info_t;

	/// @brief 定义一个数据结构，用于消息的存储和处理
	typedef struct
	{
		/// @brief 消息数据存储的起始地址
		UINT32 *base;
		/// @brief 消息深度，即消息存储的单元数
		UINT16 depth; // unit word
		/// @brief 缓存长度
		UINT16 cache_len;
		/// @brief 缓存偏移量
		UINT16 cache_offset;
		/// @brief 缓存数据的存储地址
		UINT32 *cache;
	} vbox_msg_t;

	/// @brief 定义一个数据结构，用于消息格式化
	typedef struct
	{
		/// @brief 消息的大小
		UINT16 size;
		/// @brief 索引
		UINT8 index;
		/// @brief 校验值
		UINT8 magic;
		/// @brief 消息的唯一标识符
		UINT16 id;
		/// @brief 保留字段
		UINT16 reserved;
	} vbox_msg_forma_t;

 /**
 * @addtogroup Vpla_BOX
 * 
 * @{
 */	
#pragma pack()

	/// @brief VBOX 初始化
	/// @return 成功返回0，失败返回错误码
	SINT32 VBOX_Init(void);
	
	/// @brief VBOX 反初始化
	/// @return 成功返回0，失败返回错误码
	SINT32 VBOX_Uninit(void);
	
	/// @brief 禁用VBOX
	/// @return 成功返回0，失败返回错误码
	SINT32 VBOX_Disable(void);
	
	/// @brief 硬件信号处理函数
	void VBOX_HW_Signal(void);
	
	/// @brief 设置事件
	/// @param evt 需要设置的事件
	/// @return 设置后的事件值
	UINT16 VBOX_EvtSet(UINT16 evt);
	
	/// @brief 获取事件
	/// @param evt 需要获取的事件
	/// @return 获取到的事件值
	UINT16 VBOX_EvtGet(UINT16 evt);
	
	/// @brief 获取空闲的发送缓冲区长度
	/// @return 空闲的发送缓冲区长度
	UINT32 VBOX_GetFreeTxBufLen(void);
	
	/// @brief 获取有效的接收缓冲区长度
	/// @return 有效的接收缓冲区长度
	UINT32 VBOX_GetValidRxBufLen(void);
	
	/// @brief 硬件信号处理函数
	void VBOX_HW_Signal(void);
	
	/// @brief 设置事件
	/// @param evt 需要设置的事件
	/// @return 设置后的事件值
	UINT16 VBOX_EvtSet(UINT16 evt);
	
	/// @brief 获取事件
	/// @param evt 需要获取的事件
	/// @return 获取到的事件值
	UINT16 VBOX_EvtGet(UINT16 evt);
	
	/// @brief 获取空闲的发送缓冲区长度
	/// @return 空闲的发送缓冲区长度
	UINT32 VBOX_GetFreeTxBufLen(void);
	
	/// @brief 获取有效的接收缓冲区长度
	/// @return 有效的接收缓冲区长度
	UINT32 VBOX_GetValidRxBufLen(void);
	
	/// @brief 硬件信号处理函数
	void VBOX_HW_Signal(void);
	
	/// @brief 设置事件
	/// @param evt 需要设置的事件
	/// @return 设置后的事件值
	UINT16 VBOX_EvtSet(UINT16 evt);
	
	/// @brief 获取事件
	/// @param evt 需要获取的事件
	/// @return 获取到的事件值
	UINT16 VBOX_EvtGet(UINT16 evt);
	
	/// @brief 获取空闲的发送缓冲区长度
	/// @return 空闲的发送缓冲区长度
	UINT32 VBOX_GetFreeTxBufLen(void);
	
	/// @brief 获取有效的接收缓冲区长度
	/// @return 有效的接收缓冲区长度
	UINT32 VBOX_GetValidRxBufLen(void);

	/// @brief 设置数据
	/// @param offset 数据偏移量
	/// @param SrcAddr 数据源地址
	/// @param len 数据长度
	/// @return 操作结果
	UINT32 VBOX_SetData(UINT32 offset, void *SrcAddr, UINT32 len);

	/// @brief 获取数据
	/// @param offset 数据偏移量
	/// @param DsetAddr 数据目标地址
	/// @param len 数据长度
	/// @return 操作结果
	UINT32 VBOX_GetData(UINT32 offset, void *DsetAddr, UINT32 len);

	/// @brief 设置数据发送缓冲区
	/// @param addr 缓冲区地址
	/// @param len 数据长度
	void VBOX_SetDataTxBuf(void *addr, UINT32 len);

	/// @brief 获取数据接收缓冲区
	/// @param addr 缓冲区地址
	/// @param len 数据长度
	void VBOX_GetDataRxBuf(void *addr, UINT32 len);

	/// @brief 设置发送缓冲区指针偏移
	/// @param off 偏移量
	void VBOX_SetTxBufPrt(UINT16 off);

	/// @brief 设置接收缓冲区指针偏移
	/// @param off 偏移量
	void VBOX_SetRxBufPrt(UINT16 off);

	/// @brief 启用/禁用发送缓冲区
	/// @param en 布尔值，表示使能或禁用
	void VBOX_SetTxBufEn(BOOL en);

	/// @brief 获取发送缓冲区使能状态
	/// @return 布尔值，表示使能状态
	BOOL VBOX_GetTxBufEn(void);

	/// @brief 启用/禁用接收缓冲区
	/// @param en 布尔值，表示使能或禁用
	void VBOX_SetRxBufEn(BOOL en);

	/// @brief 获取接收缓冲区使能状态
	/// @return 布尔值，表示使能状态
	BOOL VBOX_GetRxBufEn(void);

	/// @brief 获取发送请求状态
	/// @return 布尔值，表示发送请求状态
	BOOL VBOX_GetTxReq(void);

	/// @brief 发送静态消息到队列
	/// @param id 消息ID
	/// @param msg 消息指针
	/// @param size 消息大小
	/// @return 操作结果
	SINT32 vbox_queue_send_static(UINT16 id, void *msg, UINT16 size);

	/// @brief 发送消息到队列
	/// @param id 消息ID
	/// @param msg 消息指针
	/// @param size 消息大小
	/// @return 操作结果
	SINT32 vbox_queue_send(UINT16 id, void *msg, UINT16 size);

	/// @brief 接收静态消息队列
	/// @param id 消息ID指针
	/// @param addr 缓冲区地址
	/// @param addr_len 缓冲区长度
	/// @return 消息指针
	UINT8 *vbox_queue_recv_static(UINT16 *id, UINT8 *addr, UINT16 addr_len);

	/// @brief 接收消息队列
	/// @param id 消息ID指针
	/// @return 消息指针
	UINT8 *vbox_queue_recv(UINT16 *id);

	/// @brief 设置状态
	/// @param state 状态值
	/// @return 操作结果
	UINT32 VBOX_StateSet(UINT32 state);

	/// @brief 写入状态
	/// @param state 状态值
	/// @return 操作结果
	UINT32 VBOX_StateWrite(UINT32 state);

	/// @brief 清除状态
	/// @param state 状态值
	/// @return 操作结果
	UINT32 VBOX_StateClr(UINT32 state);

	/// @brief 获取状态
	/// @return 当前状态值
	UINT32 VBOX_StateGet(void);
#if VBOX_CORE_IS_MASTER
	/// @brief 更新VBOX传输缓存
	void VBOX_TxCacheUpdate(void);

	/// @brief 保持VBOX运行状态
	/// @param en 使能标志，TRUE表示启用，FALSE表示禁用
	void VBOX_KeepRun(BOOL en);

	/// @brief 启用或禁用VBOX调试
	/// @param en 使能标志，TRUE表示启用，FALSE表示禁用
	void VBOX_DebugEn(BOOL en);
#else
	/// @brief 获取调试状态
	/// @return 返回BOOL类型的调试状态
	BOOL VBOX_GetDebug(void);
	
	/// @brief 获取保持运行状态
	/// @return 返回BOOL类型的保持运行状态
	BOOL VBOX_GetKeepRun(void);
#endif
 /// @brief 设置或重置VBOX 
 /// @param en 布尔值，指示是否重置
 void VBOX_SetReset(BOOL en);
 /// @brief 获取VBOX的重置状态
 /// @return 布尔值，指示VBOX是否被重置
 BOOL VBOX_GetReset(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_BOX_H_ */
