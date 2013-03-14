/*;********************************************************************************************************
;                                               excALib.s
;                                         exception handle routine
;
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : excALib.s
; Version   : V1.0
; By        :
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : GCC
;********************************************************************************************************/

/*;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************/

.cpu cortex-m3
.fpu softvfp
.syntax unified
.thumb
.text
    .extern  pxCurrentTCB

    .extern  excExcHandle
    .extern  vTaskSwitchContext

    .global  faultRoutine                                     /*; Functions declared in this file*/
    .global  memFaultRoutine
    .global  busFaultRoutine
    .global  usageFaultRoutine



/*;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;*********************************************************************************************************

        RSEG CODE:CODE:NOROOT(2)
        THUMB

*/

/*;********************************************************************************************************
;                                         HANDLE HARD FAULT EXCEPTION
;                                     void EXC_ENTER_HARD_FAULT(void)
;
; Note(s) : 1) 本函数用于处理硬件FAULT异常，函数地址放置于中断向量表编号3的位置中。
; ********************************************************************************************************/
.thumb_func
faultRoutine:
    MOV	R1,#3
	B	excEnterCommon
.thumb_func
memFaultRoutine:
    MOV	R1,#4
	B	excEnterCommon
.thumb_func
busFaultRoutine:
    MOV	R1,#5
	B	excEnterCommon
.thumb_func
usageFaultRoutine:
    MOV	R1,#6

.thumb_func
excEnterCommon:
    CPSID   F
    TST     LR, #0x4
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP

    SUBS    R0, R0, #0x20
    STM     R0, {R4-R11}

    BL      excExcHandle

    BL      vTaskSwitchContext

    LDR     R2, =pxCurrentTCB
    LDR     R3, [R2]
    LDR     R0, [R3]                                            /*; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;*/
    LDM     R0, {R4-R11}                                        /*; Restore r4-11 from new process stack*/
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             /*; Load PSP with new process SP*/
   /*;ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack*/
    LDR     LR, =0xFFFFFFFD
    CPSIE   F
    BX      LR                                                  /*; Exception return will restore remaining context*/

    .END

