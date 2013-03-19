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

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define  ROOTSTACKSIZE  (1000)      /* ����root task��ջ��С */

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

    puts("....STM32F207 APP START...");
    usrapp_init();
}

int main (void)
{
    /* ��ʼ��ϵͳ��Ƶ */
    sysHwInit0();

    /* ��ʼ��ϵͳ�ж������� */
    intLibInit();

    /*��TTY �� �ж����������*/
    sysHwInit();

    if (OK != mem_init((unsigned long)&heap_low, (unsigned long)(&cstack_top - 0x200)))
    {
        puts("mem_init err!\n");
        while(1);
    }

    /*���������ʱ�ӽ��ĳ�ʼ��*/
    (void)taskSpawn((const signed char*)"root", 1, ROOTSTACKSIZE, (OSFUNCPTR)rootTask,0);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for( ;; );

    return 0;
}

/*----------------------------main.c--------------------------------*/
