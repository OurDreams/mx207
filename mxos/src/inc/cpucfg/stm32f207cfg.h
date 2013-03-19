/**
 ******************************************************************************
 * @file       stm32f103cfg.h
 * @brief      系统配置文件.
 * @details    This file including all API functions's declare of oscfg.h.
 *
 ******************************************************************************
 */


#ifndef __STM32F207CFG_H__
#define __STM32F207CFG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
/** 定义处理器的最大中断数量 */
#define MAX_INT_COUNT 103

#if MAX_INT_COUNT > 103
# error "file: stm32f207cfg.h MAX_INT_COUNT > 103"
#endif

/** 定义CPU的主频120MHZ */
#define SYS_CPU_CLOCK_HZ    (120000000u)

/* 定义时钟周期终端的频率 */
#define  SYS_TICKS_PER_SECOND     (100u)

/* 定义操作系统任务的最高优先级 */
#define  MAX_TASK_PRIORITIES        (8u)


/* shell任务配置 */
#define TASK_PRIORITY_SHELL         (2u)    /**< shell任务优先级 */
#define TASK_STK_SIZE_SHELL      (2048u)    /**< shell任务堆栈 */

/* dmn任务配置 */
#define TASK_PRIORITY_DMN           (1u)    /**< DMN任务优先级 */
#define TASK_STK_SIZE_DMN         (512u)    /**< DMN任务堆栈 */
#define DMN_MAX_CHECK_TIME          (6u)    /**< 默认喂狗超时时间（6*10秒） */

/* 异常任务 */
#define TASK_PRIORITY_EXC           (0u)    /**< 异常中断处理任务的优先 */
#define TASK_STK_SIZE_EXC         (512u)    /**< 异常中断处理任务的堆栈大小 */

/* logMsg任务配置 */
#define INCLUDE_LOGMSG_SUPPORT      (1u)    /**< 支持logMsg */
#define MAX_MSGS                   (10u)    /**< 消息队列中的最大消息个数 */
#define MAX_BYTES_IN_A_MSG        (200u)    /**< 1个logMsg最大打印的字节数 */
#define TASK_PRIORITY_LOGMSG        (1u)    /**< logMsg任务的优先 */
#define TASK_STK_SIZE_LOGMSG     (1024u)    /**< logMsg任务的堆栈大小 */


#endif /* __STM32F207CFG_H__ */
/*--------------------------End of stm32f103cfg.h----------------------------*/
