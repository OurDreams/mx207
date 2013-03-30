/**
 ******************************************************************************
 * @file      excArchLib.c
 * @brief     Cotex M3 异常处理模块.
 * @details
 * @copyright
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <string.h>
#include <stdio.h>
//#include <FreeRTOS.h>
//#include <task.h>
#include <taskLib.h>
#include <time.h>
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/

typedef struct
{
    uint32_t valid; /* indicators that following fields are valid */
    uint32_t vecAddr; /* exception vector address */
    uint32_t fsr; /* fault status register*/
    uint32_t faddr; /* fault address register */
    uint32_t sp;
    TASK_ID taskid;
    uint32_t regs[16];
    time_t time; /* occur time */
} EXC_INFO;

typedef struct
{
    uint32_t vecAddr; /* exception vector */
    char * excMsg; /* exception message */
} EXC_MSG_TBL;

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define HARD_FALUT_FSR  0xE000ED2C
#define MEM_FALUT_FSR   0xE000ED28
#define BUS_FALUT_FSR   0xE000ED29
#define USGE_FALUT_FSR  0xE000ED2A

#define MEM_FALUT_FAR   0xE000ED34
#define BUS_FALUT_FAR   0xE000ED38

/*-----------------------------------------------------------------------------
 Section: Global Variables
 -----------------------------------------------------------------------------*/
EXC_INFO excInfo;
//extern FUNCPTR _func_excPanicHook; /* 异常处理钩子函数，在异常中断中执行 */
//extern FUNCPTR _func_excTaskHook; /* 异常处理钩子函数，在异常任务中执行，可用于在文件中保存现场信息 */

extern FUNCPTR _func_excJobAdd;

extern void
bsp_reboot();
/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static EXC_MSG_TBL excMsgTbl[] =
    {
        { 1, "RESET" },
        { 2, "NMI" },
        { 3, "HARD FAULT" },
        { 4, "MEM MANAGE FAULT" },
        { 5, "BUS FAULT" },
        { 6, "USAGE FAULT" },
        { 0, NULL } };
/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/

/**
 ******************************************************************************
 * @brief     printf exception information to shell.
 * @param[in]  None
 * @param[out] None
 * @retval None
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
excInfoShow()
{
    int i;
    if (excInfo.valid != 1)
        return;
    for (i = 0; excMsgTbl[i].vecAddr != excInfo.vecAddr; i++)
    {
        if (excMsgTbl[i].excMsg == NULL)
            return;
    }
    if (excMsgTbl[i].excMsg != NULL)
        logmsg("EXCEPTION OCCURRED: %s\n\r", excMsgTbl[i].excMsg);
    else
    {
        logmsg("\n\rTrap to uninitialized vector no %d.\n\r", excInfo.vecAddr);
        return;
    }

    logmsg("exception fault state regs value: 0x%ux \n\r", excInfo.fsr);
    if (excInfo.faddr != 0)
        logmsg("exception occur address: 0x%08x \n\r", excInfo.faddr);

    logmsg("exception occur regs : \r\n");
    logmsg("    r0  =%08x    r1  =%08x    r2  =%08x    r3  =%08x \n\r",
            excInfo.regs[8], excInfo.regs[9], excInfo.regs[10],
            excInfo.regs[11]);
    logmsg("    r4  =%08x    r5  =%08x    r6  =%08x    r7  =%08x \n\r",
            excInfo.regs[0], excInfo.regs[1], excInfo.regs[2], excInfo.regs[3]);
    logmsg("    r8  =%08x    r9  =%08x    r10 =%08x    r11 =%08x \n\r",
            excInfo.regs[4], excInfo.regs[5], excInfo.regs[6], excInfo.regs[7]);
    logmsg("    sp  =%08x    lr  =%08x    pc  =%08x    xpsr=%08x \n\r",
            excInfo.sp, excInfo.regs[13], excInfo.regs[14], excInfo.regs[15]);

    logmsg("exception occur in task: 0x%x \n\r", (uint32_t) excInfo.taskid);
    logmsg("exception task name: %-16s \n\r", taskName(excInfo.taskid));

    struct tm daytime = *localtime(&excInfo.time);
    logmsg("exception occur time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
            daytime.tm_year + 1900, daytime.tm_mon + 1, daytime.tm_mday,
            daytime.tm_hour, daytime.tm_min, daytime.tm_sec);

}

/*
 *******************************************************************************
 * @brief     save exception information to flash.
 * @details
 *
 * @note
 *******************************************************************************
 */
static void
excInfoSave()
{

    if (excInfo.valid != 1)
        return;

    // 打印信息
    excInfoShow(excInfo);

    excInfo.valid = 0;
    excInfo.faddr = 0;
}

/**
 ******************************************************************************
 * @brief    interrupt level handling of exceptions
 * @param[in]  pRegs
 * @param[in] excNo
 * @retval None
 * @details
 *    This routine handles exception traps.  It is never to be called except
 *  from the special assembly language interrupt stub routine.
 * @note
 ******************************************************************************
 */
void
excExcHandle(void* pRegs, uint32_t excNo)
{
    excInfo.valid = 1;
    excInfo.vecAddr = excNo;
    memcpy(&excInfo.regs, pRegs, 64);
    excInfo.sp = (uint32_t) pRegs;
    excInfo.taskid = taskIdSelf();
    switch (excNo)
    {
        case 3:
            excInfo.fsr = *(uint32_t*) (HARD_FALUT_FSR);
            break;
        case 4:
            excInfo.fsr = (*(uint32_t*) (MEM_FALUT_FSR)) & 0xff;
            excInfo.faddr = *(uint32_t*) (MEM_FALUT_FAR);
            break;
        case 5:
            excInfo.fsr = ((*(uint32_t*) (MEM_FALUT_FSR)) >> 8) & 0xff;
            excInfo.faddr = *(uint32_t*) (BUS_FALUT_FAR);
            break;
        case 6:
            excInfo.fsr = ((*(uint32_t*) (MEM_FALUT_FSR)) >> 16) & 0xffff;
            break;
    }

    excInfo.time = time(NULL);

    /* 在任务未启动之前，直接复位系统  */
    extern int xTaskGetSchedulerState();
    if (xTaskGetSchedulerState() == 0)
    {

//        if (_func_excPanicHook != NULL) /* panic hook? */
//            (*_func_excPanicHook)(&excInfo);

        bsp_reboot();
        return;
    }

    /* 挂起产生异常的任务 */
    taskSuspend(excInfo.taskid);

    /* 输出到excTask 打印或存储信息*/
    if (_func_excJobAdd != NULL)
        _func_excJobAdd((VOIDFUNCPTR) excInfoSave, 0, 0, 0, 0, 0, 0);

}

/*----------------------------excArchLib.c--------------------------------*/
