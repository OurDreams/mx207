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
#include <logLib.h>
#include <devices.h>
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


extern void mcuClkSetup(void);
extern void bspHwInit(void);
extern void vTaskStartScheduler( void );
extern void os_resource_init(void);
extern void os_print_banner(void);
extern status_t excInit(int32_t);

extern void uart_init0(void);
extern void usrapp_init(void);

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
    uart_init0();
    _the_console_fd = dev_open("tty0", O_RDWR);
    if (_the_console_fd <= 0)
    {
        return;
    }

    //printf BSP version
    loglib_init(1024);
    excInit(1024);
    dmn_init(512);
    shell_init(2048);
    os_resource_init();
    os_print_banner();
    devices_init();

    puts("....STM32F207 APP START...");
    usrapp_init();
}

int main (void)
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

    /*起根任务，做时钟节拍初始化*/
    (void)taskSpawn((const signed char*)"root", 1, ROOTSTACKSIZE, (OSFUNCPTR)rootTask,0);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for( ;; );

    return 0;
}

/*----------------------------main.c--------------------------------*/
