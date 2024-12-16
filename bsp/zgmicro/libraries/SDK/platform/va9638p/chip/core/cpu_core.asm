

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        EXPORT  CPU_CORE_IntDis
        EXPORT  CPU_CORE_IntEn

        EXPORT  VCPU_SR_Save
        EXPORT  VCPU_SR_Restore

        EXPORT  CPU_CORE_CntLeadZeros
        EXPORT  CPU_CORE_CntTrailZeros
			
		EXPORT	CPU_CORE_CONTROL
		EXPORT	CPU_CORE_GETPSP


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8
		
CPU_CORE_IntDis
        CPSID   I
        BX      LR


CPU_CORE_IntEn
        CPSIE   I
        BX      LR



VCPU_SR_Save
        MRS     R0, PRIMASK                     ; Set prio int mask to mask all (except faults)
        CPSID   I
        BX      LR


VCPU_SR_Restore                                  ; See Note #2.
        MSR     PRIMASK, R0
        BX      LR


CPU_CORE_CntLeadZeros
        CLZ     R0, R0                          ; Count leading zeros
        BX      LR


CPU_CORE_CntTrailZeros
        RBIT    R0, R0                          ; Reverse bits
        CLZ     R0, R0                          ; Count trailing zeros
        BX      LR
		

CPU_CORE_CONTROL
		mrs r0, control
		bx lr

		
CPU_CORE_GETPSP
		mrs r0, control
		TST r0, #1
		ITE EQ
		MRSEQ R0, MSP
		MRSNE R0, PSP
		BX      LR
                
		END