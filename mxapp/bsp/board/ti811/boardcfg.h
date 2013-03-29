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
#define BOARD_BANNER        "TI811"

#define MAX_INT_COUNT         (59u)    /**< 定义中断数量 */
#define CPU_CLOCK_HZ    (50000000u)    /**< 定义CPU的主频50MHZ */

#define USE_TTY                (0u)    /**< 使用TTY设备 */
#define ROOT_STACK_SIZE      (512u)    /**< 定义root task堆栈大小 */
#define SHELL_STACK_SIZE    (1024u)    /**< shell任务堆栈 */
#define DMN_STACK_SIZE       (512u)    /**< DMN任务堆栈 */
#define EXC_STACK_SIZE       (512u)    /**< 异常中断处理任务的堆栈大小 */
#define LOGMSG_STACK_SIZE      (0u)    /**< logMsg任务的堆栈大小 */

//-----------------------------------------------------------------------------
//班级配置
/**
 *  System Clock(CCLK) Setting   系统时钟(CCLK) 设定
 *  CCLK must be less than or equal to  20MHz/50MHz(depend on the max. cclk of the CPU)
 *  CCLK必须小于或等于20MHz/50MHz(根据单片机的最大CCLK而定)
 *  If PLL_EN=0, CCLK=EXT_CLK/CCLK_DIV, CCLK must <=20MHZ/50MHz
 *  If PLL_EN>0, CCLK=200MHz/CCLK_DIV, CCLK must <=20MHZ/50MHz
 */
#define  EXT_CLK            SYSCTL_XTAL_6MHZ    /* external clock 外部时钟      */
#define  PLL_EN             1                   /* 1：Enable PLL  使能PLL   */
#define  CCLK_DIV           SYSCTL_SYSDIV_1     /* CCLK divider CCLK分频系数 */

#endif /* __BOARDCFG_H__ */
/*----------------------------End of boardcfg.h------------------------------*/
