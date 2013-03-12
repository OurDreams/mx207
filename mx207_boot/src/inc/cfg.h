/**
 ******************************************************************************
 * @file       cfg.h
 * @brief      本文提供mx207_boot相关配置.
 * @details    This file including all API functions's declare of cfg.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef __CFG_H__
#define __CFG_H__

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define APP_START_ADDRESS       (0x08008000u)   /**< 应用程序起始地址 */
#define STACK_SIZE              (8u * 1024u)    /**< boot栈大小(字节) */
#define MAX_BUF_SIZE            (252u)          /**< 数据缓冲区大小 <= 252 */
#define ESC_DELAY               (3u)            /**< 按ESC键延时时间 */
#define BAUDRATE                (115200u)       /**< 串口波特率 */

#endif /* __CFG_H__ */
/*-----------------------------End of cfg.h----------------------------------*/
