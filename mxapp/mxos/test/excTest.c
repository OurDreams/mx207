/**
 ******************************************************************************
 * @file      excTest.c
 * @brief     C Source file of taskCmd.c.
 * @details   This file including all API functions's
 *            implement of taskCmd.c.
 *
 * @copyright
 ******************************************************************************
 */

/*------------------------------------------------------------------------------ 
Section: Includes 
------------------------------------------------------------------------------*/ 
#if 0

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <shell.h>
#include <taskLib.h>
#include <string.h>

/*------------------------------------------------------------------------------ 
Section: Macro Definitions 
------------------------------------------------------------------------------*/ 
/* NONE */ 
 
/*------------------------------------------------------------------------------ 
Section: Private Type Definitions 
------------------------------------------------------------------------------*/ 
/* NONE */ 
 
/*------------------------------------------------------------------------------ 
Section: Global Variables 
------------------------------------------------------------------------------*/ 
#define SHCSR 0xE000ED24
 
/*------------------------------------------------------------------------------ 
Section: Private Variables 
------------------------------------------------------------------------------*/ 
static SEM_ID semExcTst;
static int testid;
static unsigned int * pTst;
FUNCPTR     _func_excTest;
/*------------------------------------------------------------------------------ 
Section: Private Function Prototypes 
------------------------------------------------------------------------------*/ 
/*
 * 1、在shell中调用et -1建立tExcTest任务
 * 2、调用et 0 测试usage fault
 * 3、调用et 1 测试bus fault
 * 4、调用et 2 测试除零异常
 * 5、调用et 3 测试堆栈溢出
 * 6、调用et 4 测试malloc溢出
 * 7、调用et 5 测试heap数据头部被修改
 * 8、调用et 6 测试logMsg和logBuf
 * 9、调用et 7 测试手工产生异常
 *
 *
 */

void testHeapOver()
{
    for (int i=0;i<2000;i++)
    {

        void* p = malloc(100);
        printf("i=%d,p=0x%x \n",i,p);
    }

}

void testMemModify()
{
    uint8_t* p = malloc(1000);
    p+=1000;
    *p = 0xFF;
    *(p+1) = 0xEE;
    free(p);
}

void testStackOver()
{
    /*
     * tExcTest任务堆栈空间为512个字节,栈空间起始地址
     * 紧挨着TCB控制块，当栈空间异常时，容易覆盖TCB的
     * 内容，造成任务切换时死机，无法捕获该异常。
     * 由于printf将用到200字节栈空间，因此当BLEN设为
     * 300时，将更改tExcTest的TCB数据；当BLEN设为500
     * 时，将修改前一个任务的栈数据，造成前一个任务出现
     * 异常，次数堆栈溢出事件也能够捕获。
     */
    #define BLEN 500
    uint8_t a[BLEN];
    printf("a[BLEN] addr = 0x%x \n",(unsigned int)a);
    memset(a,0xff,BLEN);

}

void testLogBuf()
{
    TASK_ID tsk = taskIdSelf();
    uint8_t* pbuf = (uint8_t*)tsk;

    logmsg("taskid=0x%x\n",(uint32_t)tsk);
    logbuf(pbuf,40000);
}

#if 0
FUNCPTR     _func_excTest;
static void
timer5_isr(void)
{
    _func_excTest(0);

}

extern uint32_t SysCtlClockGet(void);
void test_timer_irq_exec(void)
{


    uint32_t timertick = (1000 / 1) -1;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /*重新将Timer设置为缺省值*/
    TIM_DeInit(TIM5);

    TIM_TimeBaseStructure.TIM_Prescaler = SysCtlClockGet() / 2000 - 1;
    /*设置时钟分频*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    /*设置计数器模式为向上计数模式*/
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    /* 设置计数溢出大小，每计x个数就产生一个更新事件 */
    TIM_TimeBaseStructure.TIM_Period =  timertick;
    /* 将配置应用到TIM5中 */
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    /* 清除溢出中断标志 */
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    /* TIM5 counter enable */
    TIM_Cmd(TIM5, ENABLE);

    /* 连接中断服务程序 */
    intConnect(TIM5_IRQn, timer5_isr, 0);


    //intPrioSet(TIM5_IRQn,0);

    /* 开启TIM5的中断 */
    intEnable(TIM5_IRQn);

}
#endif

void taskloop()
{
    while(1)
    {
        semTake(semExcTst, WAIT_FOREVER);
        switch (testid){
            case 0 : /* usage fault */
                _func_excTest(0);
                break;
            case 1 : /* bus fault */
                pTst = (unsigned int *)0xD00ED000;
                printf("pTst value: %08x \n\r",*pTst);
                break;
            case 2 :
                pTst = 456 / 0;
                printf("pTst value: %08x \n\r",*pTst);
                break;
            case 3 :

                testStackOver();
                break;
            case 4 :
                testHeapOver();
                break;
            case 5 :
                testMemModify();


#if 0
              __asm(
                   "MRS R0  MSP"
                   "LDR R1  =0x01"
                   "MSR R1  FAULTMASK  "
                    );
#endif
                 break;
            case 6 :
                testLogBuf();
                break;
            case 7 :
                *(volatile uint32_t*) (SHCSR) |= 1<<13; //1<<12 1<<13 1<<14 手工产生异常
                break;
            case 8 :
               //test_timer_irq_exec();
                break;
           default:
                printf(".\r\n");
                break;
            }
        
    }

}

void createTask()
{
    semExcTst = semBCreate(0);

    taskSpawn("tExcTest",4,512,(OSFUNCPTR)taskloop,0);
    
}


void exctest(int id)
{
    testid = id;
    semGive(semExcTst);
}
/*------------------------------------------------------------------------------ 
Section: Public Function  
------------------------------------------------------------------------------*/ 

uint32_t do_excTest(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{   
    int id;

    if (argc != 2) {
        printf ("Usage:\n\r");
        printf ("%s\n\r", cmdtp->usage);
        return 1;
    }

    id = atoi(argv[1]);


    if (id == -1)
        createTask();
    else
        exctest(id);

    return 0;
}
SHELL_CMD(
	et,	CFG_MAXARGS,		do_excTest,
 	"et \r\t id(-1,0,1,2) \r\t\t\t\t Test exception handler \n\r"
);

#endif
 
/*------------------------------End of excTest.c---------------------------------*/ 

