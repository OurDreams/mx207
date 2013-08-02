/**
 ******************************************************************************
 * @file      oshook.h
 * @brief     os移植接口库.
 * @details   This file including all API functions's  implement of mxhook.c.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef __OSHOOK_H__
#define __OSHOOK_H__

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
/*--------------- 下面是os非必要方法，外部像os注册的钩子函数-------------------*/
/* 1. 喂硬件狗函数钩子 */
extern FUNCPTR     _func_feedDogHook;

/* 2. 芯片复位函数钩子 */
extern FUNCPTR     _func_cpuRestHook;

/* 3. 任务超时未喂狗异常时函数钩子 */
extern FUNCPTR     _func_dmnRestHook;

/* 4. shell操作句柄 */
extern int32_t _the_console_fd;

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
/*------------------- 下面是os移植在bsp中必须实现的方法 -----------------------*/
/* 0. 关闭看门狗方法 */
extern void WEAK bsp_close_watchdog(void);

/* 1. uart底层输出字节，非中断模式 */
extern void WEAK bsp_putchar(char_t c);

/* 2. uart底层读取字节，查询模式，若无数据返回0 （使用tty时非必要）*/
extern int32_t WEAK bsp_getchar(void);

/* 3. bsp底层实现复位函数，可以复位cpu也可以掉电复位 */
extern void WEAK bsp_reboot(void);

/* 4. bsp实现微秒定时器用于_usleep和任务统计功能 */

/* 4.1 启动定时器 */
extern void WEAK bsp_timer_start(void);

/* 4.2 获取计数器值 */
extern uint32_t WEAK bsp_timer_get(void);

/* 5. 获取MCU主频 */
extern uint32_t WEAK bsp_get_mcu_clk(void);

/* 6. 获取MCU中断数量 */
extern uint32_t WEAK bsp_get_max_int_count(void);

#endif /* __OSHOOK_H__ */
/*-----------------------------End of oshook.h-------------------------------*/
