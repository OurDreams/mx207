/**
 ******************************************************************************
 * @file       stm32f103cfg.h
 * @brief      系统配置文件.
 * @details    This file including all API functions's declare of oscfg.h.
 *
 ******************************************************************************
 */


#ifndef __STM32F103CFG_H__
#define __STM32F103CFG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define MEM_BOUNDS_CHECK
#define MEM_CHECK_VALUE 2000


/**
 *  定义处理器的最大中断数量
 */
#define MAX_INT_COUNT 63

#if MAX_INT_COUNT > 63
#error "超出最大中断数"
#endif

/**
 *  定义CPU的主频72MHZ
 */
#define SYS_CPU_CLOCK_HZ  7200000

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

/* dmn任务配置 */
#define TASK_PRIORITY_DMN           (1u)    /**< DMN任务优先级 */
#define TASK_STK_SIZE_DMN         (512u)    /**< DMN任务堆栈 */
#define DMN_MAX_CHECK_TIME          (6u)    /**< 默认喂狗超时时间（6*10秒） */

#define TASK_PRIORITY_EXC           (0u)    /**< 异常中断处理任务的优先 */
#define TASK_STK_SIZE_EXC         (512u)    /**< 异常中断处理任务的堆栈大小 */


#endif /* __STM32F103CFG_H__ */
/*--------------------------End of stm32f103cfg.h----------------------------*/
