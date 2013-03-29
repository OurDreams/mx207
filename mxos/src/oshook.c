/**
 ******************************************************************************
 * @file      oshook.c
 * @brief     os外提给提供的钩子接口库.
 * @details   This file including all API functions's  implement of mxhook.c.
 * @copyright
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

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
/**
 *  @brief 喂硬件狗函数钩子
 */
FUNCPTR     _func_feedDogHook = NULL;

/**
 *  @brief 芯片复位函数钩子
 */
FUNCPTR     _func_cpuRestHook = NULL;

/**
 *  @brief 任务超时未喂狗异常时函数钩子
 */
FUNCPTR     _func_dmnRestHook = NULL;

/**
 *  @brief 内核任务堆栈溢出发生时，调用的钩子函数，
 *  可用于在文件中保存现场信息。
 *  _func_evtLogOverStackHook(taskId,pcTaskName);
 */
VOIDFUNCPTR _func_evtLogOverStackHook;

int32_t _the_console_fd = -1;

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
/* NONE */

/*--------------------------------oshook.c-----------------------------------*/
