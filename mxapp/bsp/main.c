/**
 ******************************************************************************
 * @file      main.c
 * @brief     C Source file of main.c.
 * @details   This file including all API functions's 
 *            implement of main.c.	
 *
 * @copyright
 ******************************************************************************
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include "./bspcfg.h"
#include <stdio.h>
#include <types.h>
#include <memLib.h>
#include <taskLib.h>
#include <shell.h>
#include <intLib.h>
#include <dmnLib.h>
#include <devLib.h>
#include <logLib.h>

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#ifndef ROOTSTACKSIZE
# define ROOT_STACK_SIZE     (1000u)    /**< 定义root task堆栈大小 */
#endif

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
extern char heap_low; /* Defined by the linker */
extern char cstack_top;
extern int32_t _the_console_fd;

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
extern void vTaskStartScheduler(void);
extern void os_resource_init(void);
extern void os_print_banner(void);
extern status_t excInit(int32_t);

extern void usrapp_init(void);
extern void cpuClkSetup(void);
extern void bspHwInit(void);
#if (USE_TTY == 1u)
extern void tty_init(void);
#endif

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
static void
rootTask(void *p_arg);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
static void
rootTask(void *p_arg)
{
    (void)p_arg;
#if (USE_TTY == 1u)
    tty_init();
    _the_console_fd = dev_open("tty0", O_RDWR);
    if (_the_console_fd <= 0)
    {
        return;
    }
#endif

#if (LOGMSG_STACK_SIZE != 0u)
    loglib_init(LOGMSG_STACK_SIZE);
#endif

#if (EXC_STACK_SIZE != 0u)
    excInit(EXC_STACK_SIZE);
#endif

#if (DMN_STACK_SIZE != 0u)
    dmn_init(DMN_STACK_SIZE);
#endif

#if (SHELL_STACK_SIZE != 0u)
    shell_init(SHELL_STACK_SIZE);
#endif

    os_resource_init();
    os_print_banner();

    puts("...."BOARD_BANNER" APP START...");
    usrapp_init();
}

int main(void)
{
    /* 初始化系统主频 */
    cpuClkSetup();

    /* 初始化系统中断向量表 */
    intLibInit();

    /* 执行os启动之前的初始化 */
    bspHwInit();

    if (OK != mem_init((unsigned long)&heap_low, (unsigned long)(&cstack_top - 0x200)))
    {
        puts("mem_init err!\n");
        while(1);
    }

    /*起根任务，做时钟节拍初始化*/
    (void)taskSpawn((const signed char*)"root", 1, ROOT_STACK_SIZE, (OSFUNCPTR)rootTask,0);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for( ;; );

    return 0;
}

/*---------------------------------main.c------------------------------------*/
