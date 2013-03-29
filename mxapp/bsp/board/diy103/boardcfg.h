/**
 ******************************************************************************
 * @file       boardcfg.h
 * @brief      API include file of boardcfg.h.
 * @details    This file including all API functions's declare of boardcfg.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef __BOARDCFG_H__
#define __BOARDCFG_H__

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define BOARD_BANNER        "DIY103"

#define USE_TTY                (1u)    /**< 使用TTY设备 */
#define ROOT_STACK_SIZE     (1000u)    /**< 定义root task堆栈大小 */
#define SHELL_STACK_SIZE    (2048u)    /**< shell任务堆栈 */
#define DMN_STACK_SIZE       (512u)    /**< DMN任务堆栈 */
#define EXC_STACK_SIZE       (512u)    /**< 异常中断处理任务的堆栈大小 */
#define LOGMSG_STACK_SIZE   (1024u)    /**< logMsg任务的堆栈大小 */

#endif /* __BOARDCFG_H__ */
/*----------------------------End of boardcfg.h------------------------------*/
