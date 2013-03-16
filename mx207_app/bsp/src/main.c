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
#include <dmnLib.h>
#include <devLib.h>

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

extern void sysHwInit0(void);
extern void vTaskStartScheduler( void );
extern status_t excInit(void);
extern void os_resource_init(void);
extern void os_print_banner(void);

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
    //printf BSP version
    excInit();
    dmn_init();
    shell_init();
    os_resource_init();
    os_print_banner();
    printf("....STM32F207 APP START...\n");
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
        printf("mem_init err!\n");
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