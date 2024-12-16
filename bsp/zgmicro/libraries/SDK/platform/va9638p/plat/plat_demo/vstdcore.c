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
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdcore.h"
#include "vstdos.h"
#include "vcortex_m3.h"
#include "vdrv_dma.h"
#include "vdrv_timer.h"
#include "vdrv_i2c.h"
#include "vdrv_ckm.h"
#include "vreg_def.h"

unsigned int __CPUCycleGet(void)
{

    if (!GET_REG_BIT2(REG_DWT_CONTROL, 0))
    {
        SET_REG_BIT2(REG_NVIC_DEMCR, 24);
        WRITE_REG(REG_DWT_CYCCNT, 0);
        SET_REG_BIT2(REG_DWT_CONTROL, 0);
    }

    return READ_REG(REG_DWT_CYCCNT);
}

unsigned int __CPUMcauseGet(void)
{
    return (READ_REG(REG_NVIC_ICSR) & 0x1FF);
}

unsigned int __CPUSpGet(void)
{
    extern unsigned int CPU_CORE_GETPSP(void);
    return CPU_CORE_GETPSP() + 8;
}

unsigned int __CPUPcGet(void)
{
    return __GET_RETURN_ADDR();
}

__ASM void __SET_MSP(unsigned int val)
{
    msr msp, r0
                 bx lr
}

void VCPU_Nop(UINT32 cle)
{
    UINT32 cur_time = 0;
    if (cle > 10)
        cle -= 10;
    else
        cle = 0;
    cur_time = __CPUCycleGet();
    if (cur_time + cle >= cur_time)
    {
        while ((__CPUCycleGet() < (cur_time + cle)) && (__CPUCycleGet() >= cur_time))
            ;
    }
    else
    {
        while ((__CPUCycleGet() < (cur_time + cle)) || (__CPUCycleGet() >= cur_time))
            ;
    }
}

//unsigned int VCPU_SR_Save(void)
//{
//    extern unsigned int CPU_CORE_SR_Save(void);
//    return CPU_CORE_SR_Save();
//}

//void VCPU_SR_Restore(unsigned int val)
//{
//    extern void CPU_CORE_SR_Restore(unsigned int val);
//    CPU_CORE_SR_Restore(val);
//}

void VCPU_INT_Enable(void)
{
    extern void CPU_CORE_IntEn(void);
    CPU_CORE_IntEn();
}

void VCPU_INT_Disable(void)
{
    extern void CPU_CORE_IntDis(void);
    CPU_CORE_IntDis();
}

void VCPU_INT_SrcEnable(UINT16 Interrupt)
{
    VNVIC_INT_Enable(Interrupt);
}

void VCPU_INT_SrcDisable(UINT16 Interrupt)
{
    VNVIC_INT_Disable(Interrupt);
}

UINT32 VCPU_INT_IsExec(void)
{
    return (READ_REG(REG_NVIC_ICSR) & 0x1FF);
}

void VCPU_INT_SetPriorityGroup(UINT8 SplitBit)
{
    VNVIC_INT_SetPriorityGroup(SplitBit);
}

void VCPU_INT_SetPriority(UINT16 Interrupt, UINT8 PreemptPrio, UINT8 SubPrio)
{
    VNVIC_INT_SetPriority(Interrupt, PreemptPrio, SubPrio);
}

void VCPU_INT_SrcSetPending(UINT16 Interrupt)
{
    VNVIC_INT_SetPending(Interrupt);
}

void VCPU_INT_SrcClearPending(UINT16 Interrupt)
{
    VNVIC_INT_ClearPending(Interrupt);
}

BOOL VCPU_INT_SrcIsPending(UINT16 Interrupt)
{
    return VNVIC_INT_IsPending(Interrupt);
}

void VCPU_CoreInit(void)
{
    extern VISR_FUNCPTR VISR_VectorTable[];
	#if CORE_SEL_MCUTOP
	vmemset((void *)(VDEV_SHARERAM_BASE),0x00, VDEV_SHARERAM_SIZE); 
	#else
	extern void coderam_load(void);
	coderam_load();
	#endif
    VCPU_CoreUninit();
    VNVIC_INT_SetVectorTbl((UINT32)VISR_VectorTable);
}

void VCPU_CoreUninit(void)
{
    VCPU_INT_Disable();
    VCPU_INT_SrcDisable(IRQ_SYSSW_INT);
    VCPU_INT_SrcDisable(IRQ_SYSTICK_INT);
    for (UINT8 i = 18; i < 32; i++)
    {
        VCPU_INT_SrcDisable(i);
    }
}

void VCPU_SysTick_Config(uint32_t ms)
{
    UINT32 reload;
#if CORE_SEL_MCUTOP
    reload = ms * VCKM_GetSysClockFreq() / 1000;
    VNVIC_SYS_ConfigSysTick(NVIC_SYSTICK_SRC_FCLK, reload);
#else
#if SYS_LPO_SUPPOR
	extern UINT32 SYSpwr_LPO_GetTickNum(UINT32 Ms);
	reload = SYSpwr_LPO_GetTickNum(ms);
#else
    reload = ms * 4;
#endif
    VNVIC_SYS_ConfigSysTick(NVIC_SYSTICK_SRC_STCLK, reload);
#endif
}

void VCPU_SysTick_Reload(uint32_t ms)
{
    // SysTick_Reload(ms * SOC_TIMER_FREQ / 1000);
}

void VCPU_SysSW_Set(void)
{
    // SysTimer_SetSWIRQ();
}

void VCPU_SysSW_Clr(void)
{
    // SysTimer_ClearSWIRQ();
}

void VCPU_SleepNow(UINT8 Deep, UINT8 SleepOnExit)
{
    VNVIC_SYS_SleepNow(Deep, SleepOnExit);
}

void VCPU_Reset(VCPU_Rest_t mode)
{
    VNVIC_SYS_Reset(mode);
}

static int VarInRAM = -1;
VCoreId_t VCORE_GetID(void)
{
    if (VarInRAM == -1)
    {
        int *pVarInRAM = &VarInRAM;
        if ((unsigned int)pVarInRAM >= VDEV_BT_RAM_BASE && (unsigned int)pVarInRAM < (VDEV_BT_RAM_BASE + VDEV_BT_RAM_SIZE))
        {
            // Variable is locate in BT RAM
            VarInRAM = CPU_ID_BT;
        }
        else if ((unsigned int)pVarInRAM >= VDEV_TOP_RAM_BASE && (unsigned int)pVarInRAM < (VDEV_TOP_RAM_BASE + VDEV_TOP_RAM_SIZE))
        {
            // Variable is locate in TOP RAM
            VarInRAM = CPU_ID_TOP;
        }
        else
        {
            // unknown stack
            VarInRAM = CPU_ID_UNKNOWN;
            while (1)
            {
            }
        }
    }
    return (VCoreId_t)VarInRAM;
}


//----------------------------------------------------
__WEAK __IRQ void VISR_NMI(void){while (1);}
__IRQ __ASM void VISR_HardFault ( void )
{
    /* Go to infinite loop when Hard Fault exception occurs */
    IMPORT HardFault_Handler
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B HardFault_Handler
}
__irq __ASM void VISR_MMUFault(void) 
{ 
	IMPORT HardFault_Handler
	TST LR, #4 
	ITE EQ 
	MRSEQ R0, MSP 
	MRSNE R0, PSP 
	B HardFault_Handler
}
__irq __ASM void VISR_BusFault(void) 
{ 
	IMPORT HardFault_Handler
	TST LR, #4 
	ITE EQ 
	MRSEQ R0, MSP 
	MRSNE R0, PSP 
	B HardFault_Handler
}
__irq __ASM void VISR_UsageFault(void) 
{ 
	IMPORT HardFault_Handler
	TST LR, #4 
	ITE EQ 
	MRSEQ R0, MSP 
	MRSNE R0, PSP 
	B HardFault_Handler
}
__WEAK __IRQ void VISR_SVCall ( void ){while (1);}
__WEAK __IRQ void VISR_DebugMon ( void ){while (1);}
__WEAK __IRQ void PendSV_Handler ( void ){while (1);}     //THOMAS
__WEAK __IRQ void SysTick_Handler ( void ){while (1);}   //THOMAS
__WEAK __IRQ void VISR_BB(void){while (1);}
__WEAK __IRQ void VISR_Modem(void){while (1);}
__WEAK __IRQ void VISR_Uart(void){while (1);}
__WEAK __IRQ void VISR_I2c(void){while (1);}  //THOMAS
__WEAK __IRQ void VISR_Timer(void){VTMR_IntHandler();}
__WEAK __IRQ void VISR_GPIO(void){while (1);}
__WEAK __IRQ void VISR_DMA(void){VDMA_Interrupt_Routine_Service(); VDMA_Interrupt_Routine_Service_1();}
__WEAK __IRQ void VISR_VA76xx(void){while (1);}
__WEAK __IRQ void VISR_SDIO(void){while (1);}
__WEAK __IRQ void VISR_Usb(void){while (1);}
__WEAK __IRQ void VISR_Mbox(void){while (1);}
__WEAK __IRQ void VISR_IPC(void){while (1);}
__WEAK __IRQ void VISR_Dummy(void){while (1);}
/*
 * Exception Table, in seperate section so it can be correctly placed at 0x0
 */
__WEAK __IRQ void VSYS_SOC_FaultHandler(void){while(1);}

#pragma arm section rodata = "VECTOR_TABLE"
#pragma import(__use_two_region_memory)

/* Linker-generated Stack Base addresses */
extern unsigned int Image$$ARM_LIB_STACK$$ZI$$Limit;
/* ARM C library main entry */
extern int __main(void);

#ifdef CORE_SEL_MCUTOP
__WEAK VISR_FUNCPTR VISR_VectorTable[28] __FAST_ACCESS_VARIABLE = {
    /* Configure Initial Stack Pointer */
    (VISR_FUNCPTR)&Image$$ARM_LIB_STACK$$ZI$$Limit,
    /* Initial PC, set to entry point  */
    (VISR_FUNCPTR)__main,
    /* Cortex-M3 exceptions */
    VISR_NMI,
    VISR_HardFault,
    VISR_MMUFault,
    VISR_BusFault,
    VISR_UsageFault,
    0, 0, 0, 0, // Reserved
    VISR_SVCall,
    VISR_DebugMon,
    0, // Reserved
    (VISR_FUNCPTR)PendSV_Handler,
    (VISR_FUNCPTR)SysTick_Handler,
    /* Peripheral interrupts */
    VISR_Dummy, // IRQ_BB
    VISR_Dummy, // IRQ_MODEM
    VISR_Uart,  // IRQ_UART
    VISR_I2c,   // IRQ_I2C
    VISR_Timer, // IRQ_TIMER
    VISR_GPIO,  // IRQ_GPIO
    VISR_DMA,   // IRQ_DMA
    VISR_VA76xx, // IRQ_VA7633
    VISR_Dummy, // IRQ_SDIO
    VISR_Usb,   // IRQ_UOTG
    VISR_Mbox, // IRQ_DSP
    VISR_IPC,   // IRQ_MCUBT/IRQ_MCUTOP
};
#else
__WEAK VISR_FUNCPTR VISR_VectorTable[28] __FAST_ACCESS_VARIABLE = {
    /* Configure Initial Stack Pointer */
    (VISR_FUNCPTR)&Image$$ARM_LIB_STACK$$ZI$$Limit,
    /* Initial PC, set to entry point  */
    (VISR_FUNCPTR)__main,
    /* Cortex-M3 exceptions */
    VISR_NMI,
    VISR_HardFault,
    VISR_MMUFault,
    VISR_BusFault,
    VISR_UsageFault,
    0, 0, 0, 0, // Reserved
    VISR_SVCall,
    VISR_DebugMon,
    0, // Reserved
    (VISR_FUNCPTR)VISR_PendSV,
    (VISR_FUNCPTR)VISR_SysTick,
    /* Peripheral interrupts */
    VISR_BB,     // IRQ_BB
    VISR_Modem,  // IRQ_MODEM
    VISR_Uart,   // IRQ_UART
    VISR_I2c,    // IRQ_I2C
    VISR_Timer,  // IRQ_TIMER
    VISR_GPIO,   // IRQ_GPIO
    VISR_DMA,    // IRQ_DMA
    VISR_VA76xx, // IRQ_VA7633
    VISR_SDIO,   // IRQ_SDIO
    VISR_Usb,    // IRQ_UOTG
    VISR_Mbox,   // IRQ_DSP
    VISR_IPC,    // IRQ_MCUBT/IRQ_MCUTOP
};
#endif
#pragma arm section
