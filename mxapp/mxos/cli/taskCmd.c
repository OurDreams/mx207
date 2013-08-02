/**
 ******************************************************************************
 * @file      taskCmd.c
 * @brief     C Source file of taskCmd.c.
 * @details   This file including all API functions's 
 *            implement of taskCmd.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <maths.h>
#include <shell.h>
#if 0
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
uint32_t do_task_delete(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    uint32_t taskId;
    if (argc == 1)
    {
        printf("please input taskId!\n");
        return 0;
    }
    else
    {
        sscanf((int8 *)argv[1], "%i", &taskId);
        if (taskIdVerify((TASK_ID)taskId) == OK)
        {
            taskDelete((TASK_ID)taskId);
        }
        else
        {
            printf("taskId is error!\n");
        }
    }
    return 1;
}
SHELL_CMD(
    td,  CFG_MAXARGS,        do_task_delete,
    "adr \r\t\t\t\t Delete a tasks\r\n"
);

uint32_t do_task_suspend(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    uint32_t taskId;
    if (argc == 1)
    {
        printf("please input taskId!\n");
        return 0;
    }
    else
    {
        sscanf((int8 *)argv[1], "%i", &taskId);
        if (taskIdVerify((TASK_ID)taskId) == OK)
        {
            taskSuspend((TASK_ID)taskId);
        }
        else
        {
            printf("taskId is error!\n");
        }
    }
    return 1;
}
SHELL_CMD(
    ts,  CFG_MAXARGS,        do_task_suspend,
    "adr \r\t\t\t\t Suspend a tasks\r\n"
);

uint32_t do_task_resume(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    uint32_t taskId;
    if (argc == 1)
    {
        printf("please input taskId!\n");
        return 0;
    }
    else
    {
        sscanf((char_t *)argv[1], "%i", &taskId);
        if (taskIdVerify((TASK_ID)taskId) == OK)
        {
            taskResume((TASK_ID)taskId);
        }
        else
        {
            printf("taskId is error!\n");
        }
    }
    return 1;
}
SHELL_CMD(
    tr,  CFG_MAXARGS,        do_task_resume,
    "adr \r\t\t\t\t Resume a tasks\r\n"
);
#endif
/*----------------------------taskCmd.c--------------------------------*/
