/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
        irq

Description:
        irq Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef _VSTDCORE_H_
#define _VSTDCORE_H_
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#define ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDCORE_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES */
#include "vstdcfg.h"
#include "vstdlib.h"

#include "vpla_mem.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond doxygen不会解析这段代码
 */
/** @endcond */ 

#define IRQ_SYSSW_INT           14
#define IRQ_SYSTICK_INT         15
#define IRQ_TRNG_INT        28
#define IRQ_MCUIPC_INT		27
#define IRQ_DSP_INT         26
#define IRQ_UOTG_INT        25
#define IRQ_SDIO_INT     	24
#define IRQ_VA76XX_INT1     23
#define IRQ_DMA_INT         22
#define IRQ_GPIO_INT        21
#define IRQ_TIMER_INT       20
#define IRQ_I2C_INT         19
#define IRQ_UART_INT        18
#define IRQ_MODEM_INT       17
#define IRQ_BB_INT          16

#define IRQ_ATOM_TIMEOUT_INT 13
#define IRQ_EXI_INT			 12
#define IRQ_I2C1_INT		 11
#define IRQ_TIMER1_INT		 10
#define IRQ_ABB_INT			 9

#define IRQ_ALL_LIMIT          32

#define  NVIC_PRIO_BITS                                         4                               /* NVIC Priority Bits */

typedef enum{
    CORE_RESET_SYSTEM = 1,
    CORE_RESET_VECT = 2,
}VCPU_Rest_t;

typedef enum{
    CPU_ID_TOP = 0,
    CPU_ID_BT = 1,
    CPU_ID_DSP = 2,
    CPU_ID_UNKNOWN = 0xFF,
}VCoreId_t;

extern unsigned int __CPUCycleGet(void);
extern unsigned int __CPUMcauseGet(void);
extern unsigned int __CPUPcGet(void);
extern unsigned int __CPUSpGet(void);
extern void __SET_MSP(unsigned int val);

    
#if (defined __GNUC__)
#define __WFI()               __asm volatile("wfi")
#define __SOFT_WFI()          __asm volatile("wfi")
#define __NOP()               __asm volatile ("nop") 
#define __EBREAK()             __asm__ volatile("ebreak")  
#define __SOFT_BREAKPOINT()    __asm__ volatile("ebreak")
#define __GET_RETURN_ADDR()   ((UINT32)__builtin_return_address(0))  
#elif (defined __CC_ARM)
#define __WFI()            	  __wfi()    
#define __SOFT_WFI()          __wfi()   
#define __NOP()       		  __nop()    
#define __EBREAK()            __asm volatile ("BKPT #0")   
#define __SOFT_BREAKPOINT()   __breakpoint(0)
#define __GET_RETURN_ADDR()   (__return_address() & 0xFFFFFFFE)
#endif


extern unsigned int VCPU_SR_Save(void);
extern void VCPU_SR_Restore(unsigned int val);
#define  VCPU_SR_ALLOC()               unsigned  int cpu_sr = 0
#define  VCPU_CRITICAL_ENTER()         do { cpu_sr = VCPU_SR_Save(); } while (0)
#define  VCPU_CRITICAL_EXIT()          do { VCPU_SR_Restore(cpu_sr); } while (0)

 /**
 * @addtogroup Std_core
 * 
 * @{
 */

/// @brief CPU 空指令执行
/// @param cle 循环次数
/// @note 该函数用于消耗CPU资源，但不进行任何操作，适用于消耗CPU资源但不希望占用CPU时使用。
extern void VCPU_Nop(UINT32 cle);

/// @brief 更新虚拟 CPU 时钟频率
/// @param freq_hz 新的时钟频率（单位：赫兹）
/// @note 该函数用于更新虚拟 CPU 时钟频率，并重新计算系统时钟中断的周期。
extern void VCPU_ClockUpdate(uint32_t freq_hz);

/// @brief 延迟执行指定的微秒数。
/// @param usec 延迟的时间，以微秒为单位。
/// @note 该函数用于延迟执行指定的微秒数。
extern void VCPU_DelayUsec(uint32_t usec);

/// @brief 初始化中央处理单元核心
/// @note 该函数用于初始化中央处理单元核心，包括初始化系统时钟中断、初始化中断控制器、初始化系统定时器等。
extern void VCPU_CoreInit(void);

/// @brief 反初始化中央处理单元核心
/// @note 该函数用于反初始化中央处理单元核心，包括反初始化系统时钟中断、反初始化中断控制器、反初始化系统定时器等。
extern void VCPU_CoreUninit(void);

/// @brief 启用中央处理单元中断
/// @note 该函数用于启用中央处理单元中断。
extern void VCPU_INT_Enable(void);

/// @brief 禁用中央处理单元中断
/// @note 该函数用于禁用中央处理单元中断。
extern void VCPU_INT_Disable(void);

/// @brief 启用指定的中断源
/// @param Interrupt 表示中断号 
/// @note 该函数用于启用指定的中断源。
extern void VCPU_INT_SrcEnable ( UINT16 Interrupt );

/// @brief 禁用指定的中断源
/// @param Interrupt 表示中断号 
/// @note 该函数用于禁用指定的中断源。
extern void VCPU_INT_SrcDisable ( UINT16 Interrupt );

/// @brief 检查是否有中断正在执行
/// @return 正在执行的中断编号；   0表示没有中断正在执行。
/// @note 该函数用于检查是否有中断正在执行。
extern UINT32 VCPU_INT_IsExec(void);

/// @brief 设置中断优先级组
/// @param SplitBit 表示优先级组 
/// @note 该函数用于设置中断优先级组。
extern void VCPU_INT_SetPriorityGroup(UINT8 SplitBit);

/// @brief 设置指定中断的优先级
/// @param Interrupt 表示中断号   
/// @param PreemptPrio 表示抢占优先级 
/// @param SubPrio 表示子优先级 
/// @note 该函数用于设置指定中断的优先级。
extern void VCPU_INT_SetPriority(UINT16 Interrupt,UINT8 PreemptPrio, UINT8 SubPrio);

/// @brief 设置指定中断为挂起状态
/// @param Interrupt 表示中断号 
/// @note 该函数用于设置指定中断为挂起状态。
extern void VCPU_INT_SrcSetPending(UINT16 Interrupt);

/// @brief 清除指定中断的挂起状态
/// @param Interrupt 表示中断号  
/// @note 该函数用于清除指定中断的挂起状态。
extern void VCPU_INT_SrcClearPending ( UINT16 Interrupt );

/// @brief 检查指定中断是否为挂起状态
/// @param Interrupt 中断号 
/// @return TRUE：挂起；   FALSE：未挂起。
/// @note 该函数用于检查指定中断是否为挂起状态。
extern BOOL VCPU_INT_SrcIsPending ( UINT16 Interrupt );

/// @brief 配置系统定时器
/// @param ms 表示系统定时器的周期（单位：毫秒） 
/// @note 该函数用于配置系统定时器。
extern void VCPU_SysTick_Config(uint32_t ms);

/// @brief 重新加载系统定时器
/// @param ms 表示系统定时器的周期（单位：毫秒） 
/// @note 该函数用于重新加载系统定时器。
extern void VCPU_SysTick_Reload(uint32_t ms);

/// @brief 立即进入睡眠状态
/// @param Deep 代表深度睡眠  
/// @param SleepOnExit 代表在退出睡眠之前是否需要重新唤醒系统
/// @note 该函数用于立即进入睡眠状态。
extern void VCPU_SleepNow(UINT8 Deep, UINT8 SleepOnExit);

/// @brief 重置中央处理单元
/// @param mode 代表重置模式
/// @note 该函数用于重置中央处理单元。
extern void VCPU_Reset(VCPU_Rest_t mode);

/// @brief 获取核心ID
/// @return 返回核心ID
/// @note 该函数用于获取核心ID。
extern VCoreId_t VCORE_GetID(void);


/**@}*/

#ifdef __cplusplus
}
#endif

#ifdef ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDCORE_H_
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES
#undef  ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDCORE_H_
#endif /* ALLOW_VKERNEL_INTERNAL_INCLUDES_VSTDCORE_H_ */
#endif  /* _VSTDCORE_H_ */
