/*;****************************************************************************
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
;*****************************************************************************/

/*;****************************************************************************
;                      PUBLIC FUNCTIONS
;*****************************************************************************/
	.cpu cortex-m3
	.fpu softvfp
	.syntax unified
	.thumb
	.text
    .extern  pxCurrentTCB
    .extern  excExcHandle
    .extern  vTaskSwitchContext

    .global  excEnterCommon		/*; Functions declared in this file*/


/*;****************************************************************************
;                                         HANDLE HARD FAULT EXCEPTION
;                                     void EXC_ENTER_HARD_FAULT(void)
;
; Note(s) : 1) 本函数用于处理硬件FAULT异常，函数地址放置于中断向量表编号3的位置中。
; ****************************************************************************/
.thumb_func
excEnterCommon:
    CPSID   F				/* Disable 异常中断 */
    MRS     R1, IPSR		/* 将异常号存入R1 */
    TST     LR, #0x4		/* 测试EXC_RETURN的bit2 */
    ITE     EQ				/* 如果为0,下面包括2条指令 (P76) */
    MRSEQ   R0, MSP			/* 如果EXC_RETURN的bit2==0 则使用MSP */
    MRSNE   R0, PSP			/* 否则使用PSP */

    SUBS    R0, R0, #0x20
    STM     R0, {R4-R11}	/* 保存R4-R11 */

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

