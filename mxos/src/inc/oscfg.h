/**
 ******************************************************************************
 * @file       oscfg.h
 * @brief      系统配置文件.
 * @details    This file including all API functions's declare of oscfg.h.
 *
 ******************************************************************************
 */
 

#ifndef __OSCFG_H__
#define __OSCFG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define OS_RELEASE  "0.0.1"


#define MEM_BOUNDS_CHECK
#define MEM_CHECK_VALUE 2000


/**
 *  定义处理器的最大中断数量
 */
#define MAX_INT_COUNT 103

#if MAX_INT_COUNT > 103
#error "超出最大中断数"
#endif

/**
 *  定义CPU的主频120MHZ
 */
#define SYS_CPU_CLOCK_HZ  120000000

/**
 *  定义时钟周期终端的频率
 */
#define  SYS_TICKS_PER_SECOND       100

/**
 *  定义操作系统任务的最高优先级
 */
#define  MAX_TASK_PRIORITIES        8


/* shell任务配置 */
#define TASK_PRIORITY_SHELL         (1u)    /**< shell任务优先级 */
#define TASK_STK_SIZE_SHELL      (1024u)    /**< shell任务堆栈 */



#define TASK_PRIORITY_DMN           (1u)    /**< DMN任务优先级 */
#define TASK_STK_SIZE_DMN        (1024u)    /**< DMN任务堆栈 */

#define TASK_PRIORITY_EXC
#define TASK_STK_SIZE_EXC


#endif /* __OSCFG_H__ */
/*------------------------------End of oscfg.h-------------------------------*/
