/**
 ******************************************************************************
 * @file      syscli.c
 * @brief     C Source file of syscli.c.
 * @details   This file including all API functions's 
 *            implement of syscli.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <types.h>
#include <shell.h>
#include <dmnLib.h>

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

extern void
bsp_reboot(void) __attribute__ (( weak ));

/* Shell cmd for task information */
extern uint32_t vTaskInfo();
uint32_t do_task_info (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    return vTaskInfo();
}

SHELL_CMD(
    i,  CFG_MAXARGS,        do_task_info,
    "Summary of tasks' TCBs\r\n"
);

/*SHELL CMD FOR NET*/
uint32_t do_reboot(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    bsp_reboot();
    return 1;
}

SHELL_CMD(
    reboot, CFG_MAXARGS,        do_reboot,
    "reboot \n"
);
/**
 *******************************************************************************
 * @brief      display the information of system memory pool.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *      This routine is a shell command for display system memory
 *      pool's usage information.
 * @note
 *******************************************************************************
 */
extern void showMenInfo(void);
uint32_t do_memshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    showMenInfo();
    return 0;
}

SHELL_CMD(
    memshow,   CFG_MAXARGS,        do_memshow,
    "Print heap memory infomation \n"
);

/**
 ******************************************************************************
 * @brief      Show devLib info
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void show_devlib_info(void);
uint32_t  do_devshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    show_devlib_info();
    return 0;
}

SHELL_CMD(
    devshow,   CFG_MAXARGS,        do_devshow,
    "Print devlib infomation \n"
);

uint32_t  do_dmnshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    dmn_info(TRUE);
    return 0;
}

SHELL_CMD(
    dmnshow,   CFG_MAXARGS,        do_dmnshow,
    "Print dmn infomation \n"
);

/*----------------------------syscli.c--------------------------------*/
