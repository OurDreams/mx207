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

#define MAX_INT_COUNT         (59u)    /**< �����ж����� */
#define CPU_CLOCK_HZ    (50000000u)    /**< ����CPU����Ƶ50MHZ */

#define USE_TTY                (0u)    /**< ʹ��TTY�豸 */
#define ROOT_STACK_SIZE      (512u)    /**< ����root task��ջ��С */
#define SHELL_STACK_SIZE    (1024u)    /**< shell�����ջ */
#define DMN_STACK_SIZE       (512u)    /**< DMN�����ջ */
#define EXC_STACK_SIZE       (512u)    /**< �쳣�жϴ�������Ķ�ջ��С */
#define LOGMSG_STACK_SIZE      (0u)    /**< logMsg����Ķ�ջ��С */

//-----------------------------------------------------------------------------
//�༶����
/**
 *  System Clock(CCLK) Setting   ϵͳʱ��(CCLK) �趨
 *  CCLK must be less than or equal to  20MHz/50MHz(depend on the max. cclk of the CPU)
 *  CCLK����С�ڻ����20MHz/50MHz(���ݵ�Ƭ�������CCLK����)
 *  If PLL_EN=0, CCLK=EXT_CLK/CCLK_DIV, CCLK must <=20MHZ/50MHz
 *  If PLL_EN>0, CCLK=200MHz/CCLK_DIV, CCLK must <=20MHZ/50MHz
 */
#define  EXT_CLK            SYSCTL_XTAL_6MHZ    /* external clock �ⲿʱ��      */
#define  PLL_EN             1                   /* 1��Enable PLL  ʹ��PLL   */
#define  CCLK_DIV           SYSCTL_SYSDIV_1     /* CCLK divider CCLK��Ƶϵ�� */

#endif /* __BOARDCFG_H__ */
/*----------------------------End of boardcfg.h------------------------------*/
