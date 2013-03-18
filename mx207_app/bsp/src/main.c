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
#include <stdio.h>
#include <types.h>
#include <memLib.h>
#include <taskLib.h>
#include <shell.h>
#include <intLib.h>
#include <dmnLib.h>
#include <devLib.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define  ROOTSTACKSIZE  (1000)      /* 定义root task堆栈大小 */

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
extern char heap_low; /* Defined by the linker */
extern char cstack_top;
extern int32_t _the_console_fd;


extern void sysHwInit0(void);
extern void sysHwInit(void);
extern void vTaskStartScheduler( void );
extern status_t excInit(void);
extern void os_resource_init(void);
extern void os_print_banner(void);
extern void uart_init0(void);

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
static void
rootTask(void *p_arg)
{
    taskDelay(10);
    uart_init0();
    _the_console_fd = dev_open("tty0", O_RDWR);
    if (_the_console_fd <= 0)
    {
        return;
    }

    //printf BSP version
    excInit();
    dmn_init();
    shell_init();
    os_resource_init();
    os_print_banner();

    printf("....STM32F207 APP START...\n");

//    taskDelay(10);
//    char *pstr = "I like apple very much!\n";
//    dev_write(_the_console_fd, pstr, strlen(pstr));
}

int main (void)
{
    /* 初始化系统主频 */
    sysHwInit0();

    /* 初始化系统中断向量表 */
    intLibInit();

    /*挂TTY 等 中断起相关任务*/
    sysHwInit();

    if (OK != mem_init((unsigned long)&heap_low, (unsigned long)(&cstack_top - 0x200)))
    {
        printf("mem_init err!\n");
        while(1);
    }

    /*起根任务，做时钟节拍初始化*/
    (void)taskSpawn((const signed char*)"root", 1, ROOTSTACKSIZE, (OSFUNCPTR)rootTask,0);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for( ;; );

    return 0;
}

/*----------------------------main.c--------------------------------*/
