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
#ifndef ROOT_STACK_SIZE
# define ROOT_STACK_SIZE     (1000u)    /**< 定义root task堆栈大小 */
#endif

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
extern char heap_low; /* Defined by the linker */
extern char cstack_top;
#if (USE_TTY == 1u)
extern int32_t _the_console_fd;
#endif

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
extern void vTaskStartScheduler(void);
extern void os_resource_init(void);
extern void os_print_banner(void);

extern uint32_t bsp_get_mcu_clk(void);
extern void usrapp_init(void);
extern void mcuClkSetup(void);
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
/**
 ******************************************************************************
 * @brief   根任务
 * @param[in]  None
 * @param[out] None
 * @retval     None
 ******************************************************************************
 */
static void
rootTask(void *p_arg)
{
    (void)p_arg;
    /* 1. 初始化tty设备 */
#if (USE_TTY == 1u)
    tty_init();
    _the_console_fd = dev_open("tty0", O_RDWR);
    if (_the_console_fd <= 0)
    {
        return;
    }
#endif

    /* 2. 初始化守护任务 */
#if (DMN_STACK_SIZE != 0u)
    dmn_init(DMN_STACK_SIZE);
#endif

    /* 3. 初始化log message模块 */
#if (LOGMSG_STACK_SIZE != 0u)
    loglib_init(LOGMSG_STACK_SIZE);
#endif

    /* 4. 初始化sell模块 */
#if (SHELL_STACK_SIZE != 0u)
    shell_init(SHELL_STACK_SIZE);
#endif

    /* 5. OS其他资源模块初始化 */
    os_resource_init();

    /* 6. 输出OS banner */
    os_print_banner();
    printf("  MCU is running at %d.%d MHz\n", bsp_get_mcu_clk() / 1000000,
            (bsp_get_mcu_clk() % 1000000) / 100000);
    puts("...."BOARD_BANNER" APP START...");

    /* 7. 进入应用程序 */
    usrapp_init();
}

/**
 ******************************************************************************
 * @brief   主函数
 * @param[in]  None
 * @param[out] None
 * @retval     None
 ******************************************************************************
 */
int main(void)
{
    /* 1. 初始化系统主频 */
    mcuClkSetup();

    /* 2. 初始化OS内存管理单元 */
    if (OK != memlib_init((uint32_t)&heap_low, (uint32_t)(&cstack_top - 0x200)))
    {
        puts("mem_init err!");
        while(1);
    }

    /* 3. 初始化OS中断向量表 */
    if (OK != intLibInit())
    {
        puts("intLibInit err!");
        while(1);
    }

    /* 4. 执行OS启动之前的初始化 */
    bspHwInit();

    /* 5. 起根任务，做时钟节拍初始化 */
    (void)taskSpawn((const signed char*)"root", 1u,
            ROOT_STACK_SIZE, rootTask, 0u);

    /* 6. 启动OS调度器 */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    while (1);

    return 0;
}

/*---------------------------------main.c------------------------------------*/
