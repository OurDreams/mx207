/**
 ******************************************************************************
 * @file      excLib.c
 * @brief     ϵͳ�쳣����ģ��.
 * @details   �����쳣ʱ������Ĵ�����ֵ ���ڵ��ԡ�
 *
 * @copyright
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <taskLib.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define MEM_FALUT_FSR   (0xE000ED28)
#define BUS_FALUT_FSR   (0xE000ED29)
#define USGE_FALUT_FSR  (0xE000ED2A)
#define HARD_FALUT_FSR  (0xE000ED2C)

#define MEM_FALUT_FAR   (0xE000ED34)
#define BUS_FALUT_FAR   (0xE000ED38)

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
extern int32_t intCnt;

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      �쳣�ж����
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
excEnterCommon(void)
{
    __asm volatile (
        ".extern  pxCurrentTCB          \n"
        ".extern  excExcHandle          \n"
        ".extern  vTaskSwitchContext    \n"

        "   CPSID   F                   \n"
        "   CPSID   F                   \n" /* Disable �쳣�ж� */
        "   MRS     R1, IPSR            \n" /* ���쳣�Ŵ���R1 */
        "   TST     LR, #0x4            \n" /* ����EXC_RETURN��bit2 */
        "   ITE     EQ                  \n" /* ���Ϊ0,�������2��ָ�� (P76) */
        "   MRSEQ   R0, MSP             \n" /* ���EXC_RETURN��bit2==0 ��ʹ��MSP */
        "   MRSNE   R0, PSP             \n" /* ����ʹ��PSP */

        "   SUBS    R0, R0, #0x20       \n"
        "   STM     R0, {R4-R11}        \n" /* ����R4-R11 */

        "   BL      excExcHandle        \n"

        "   BL      vTaskSwitchContext  \n"

        "   LDR     R2, =pxCurrentTCB   \n"
        "   LDR     R3, [R2]\n"
        "   LDR     R0, [R3]            \n" /* R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;*/
        "   LDM     R0, {R4-R11}        \n" /* Restore r4-11 from new process stack*/
        "   ADDS    R0, R0, #0x20\n"
        "   MSR     PSP, R0             \n" /* Load PSP with new process SP*/
         /*;ORR     LR, LR, #0x04; Ensure exception return uses process stack*/
        "   LDR     LR, =0xFFFFFFFD     \n"
        "   CPSIE   F                   \n"
        "   BX      LR                  \n" /* Exception return will restore remaining context*/
                );
}

/**
 ******************************************************************************
 * @brief    interrupt level handling of exceptions
 * @param[in]  pRegs    : �����쳣ʱ�Ĵ���ֵ
 * @param[in]  excNo    : �쳣�жϺ�
 *
 * @retval None
 ******************************************************************************
 */
void
excExcHandle(void* pRegs, uint32_t excNo)
{
    uint32_t regs[16];
    uint32_t hfsr = *(uint32_t*)HARD_FALUT_FSR;
    uint8_t mfsr = *(uint8_t*)MEM_FALUT_FSR;
    uint8_t bfsr = *(uint8_t*)BUS_FALUT_FSR;
    uint16_t ufsr = *(uint16_t*)USGE_FALUT_FSR;

    memcpy(regs, pRegs, 64);
    uint32_t sp = (uint32_t)pRegs;

    intCnt++;
    switch (excNo)
    {
        case 3:
            /* ���hfsr��Ϣ */
            printf("=====Ӳfault=====\r\n");
            if (hfsr & 0x00000002) printf("ȡ����ʱ����Ӳfault\r\n");
            if (hfsr & 0x40000000) printf("�Ϸò���Ӳfault\r\n");
            if (hfsr & 0x80000000) printf("���Բ���Ӳfault\r\n");
            break;
        case 4:
            /* ���mfsr��Ϣ */
            printf("=====�洢��fault=====\r\n");
            if (mfsr & 0x01) printf("ȡָ�����Υ��\r\n");
            if (mfsr & 0x02) printf("���ݷ���Υ��\r\n");
            if (mfsr & 0x08) printf("��ջ����\r\n");
            if (mfsr & 0x10) printf("��ջ����\r\n");
            if (mfsr & 0x80) printf("��0x%08x����ָ������\r\n", *(uint32_t*)(MEM_FALUT_FAR));
            break;
        case 5:
            /* ���bsfr��Ϣ */
            printf("=====����fault=====\r\n");
            if (bfsr & 0x01) printf("ȡָ�����Υ��\r\n");
            if (bfsr & 0x02) printf("��ȷ�����ݷ���Υ��\r\n");
            if (bfsr & 0x04) printf("����ȷ�����ݷ���Υ��\r\n");
            if (bfsr & 0x08) printf("��ջ����\r\n");
            if (bfsr & 0x10) printf("��ջ����\r\n");
            if (bfsr & 0x80) printf("��0x%08x����ָ������\r\n", *(uint32_t*)(BUS_FALUT_FAR));
            else printf("�޷�׷��Υ��ָ��\r\n");
            break;
        case 6:
            /* ���ufsr��Ϣ */
            printf("=====�÷�fault=====\r\n");
            if (ufsr & 0x0001) printf("ָ��������\r\n");
            if (ufsr & 0x0002) printf("������ͼ����ARM״̬\r\n");
            if (ufsr & 0x0008) printf("�쳣����ʱ��ͼ�Ƿ��ļ���EXC_RETURN��PC\r\n");
            if (ufsr & 0x0010) printf("������ͼִ��Э���������ָ��\r\n");
            if (ufsr & 0x0100) printf("δ����\r\n");
            if (ufsr & 0x0200) printf("����Ϊ��\r\n");
            break;
        default:
            printf("=====��Ч�Ĵ����ж�:%d=====\r\n", excNo);
            intCnt--;
            return;
    }

    printf("exception occur regs : \r\n");
    printf("    r0  =%08x    r1  =%08x    r2  =%08x    r3  =%08x \r\n",
            regs[8], regs[9], regs[10], regs[11]);
    printf("    r4  =%08x    r5  =%08x    r6  =%08x    r7  =%08x \r\n",
            regs[0], regs[1], regs[2], regs[3]);
    printf("    r8  =%08x    r9  =%08x    r10 =%08x    r11 =%08x \r\n",
            regs[4], regs[5], regs[6], regs[7]);
    printf("    sp  =%08x    lr  =%08x    pc  =%08x    xpsr=%08x \r\n",
            sp, regs[13], regs[14], regs[15]);

    TASK_ID taskid = taskIdSelf();
    printf("exception occur in task: 0x%x \n\r", (uint32_t)taskid);
    printf("exception task name: %-16s \n\r", taskName(taskid));
    taskSuspend(taskid);    /* ��������쳣������ */

    time_t curtm = time(NULL);
    struct tm daytime;
    (void)localtime_r(&curtm, &daytime);
    printf("exception occur time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
            daytime.tm_year + 1900, daytime.tm_mon + 1, daytime.tm_mday,
            daytime.tm_hour, daytime.tm_min, daytime.tm_sec);
    intCnt--;
}

/*----------------------------End of excLib.c--------------------------------*/
