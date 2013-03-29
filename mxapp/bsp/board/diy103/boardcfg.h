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

#define USE_TTY                (1u)    /**< ʹ��TTY�豸 */
#define ROOT_STACK_SIZE     (1000u)    /**< ����root task��ջ��С */
#define SHELL_STACK_SIZE    (2048u)    /**< shell�����ջ */
#define DMN_STACK_SIZE       (512u)    /**< DMN�����ջ */
#define EXC_STACK_SIZE       (512u)    /**< �쳣�жϴ�������Ķ�ջ��С */
#define LOGMSG_STACK_SIZE   (1024u)    /**< logMsg����Ķ�ջ��С */

#endif /* __BOARDCFG_H__ */
/*----------------------------End of boardcfg.h------------------------------*/
