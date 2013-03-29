/**
 ******************************************************************************
 * @file       oscfg.h
 * @brief      ϵͳ�����ļ�.
 * @details    This file including all API functions's declare of oscfg.h.
 *
 ******************************************************************************
 */
 

#ifndef __OSCFG_H__
#define __OSCFG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <oshook.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define OS_RELEASE  "0.1.0-rc1"

#define MCU_CLOCK      bsp_get_mcu_clk()        /**< ��ȡMCU����Ƶ */
#define MAX_INT_COUNT  bsp_get_max_int_count()  /**< ��ȡMCU����ж����� */

#define TICKS_PER_SECOND          (100u)    /**< ����ʱ�������ն˵�Ƶ�� */
#define MAX_TASK_PRIORITIES         (8u)    /**< �������ϵͳ�����������ȼ� */

/* shell�������� */
#define TASK_PRIORITY_SHELL         (2u)    /**< shell�������ȼ� */
#define TASK_STK_SIZE_SHELL      (2048u)    /**< shell�����ջ */

/* dmn�������� */
#define TASK_PRIORITY_DMN           (1u)    /**< DMN�������ȼ� */
#define TASK_STK_SIZE_DMN         (512u)    /**< DMN�����ջ */
#define DMN_MAX_CHECK_TIME          (6u)    /**< Ĭ��ι����ʱʱ�䣨6*10�룩 */

/* �쳣���� */
#define TASK_PRIORITY_EXC           (0u)    /**< �쳣�жϴ������������ */
#define TASK_STK_SIZE_EXC         (512u)    /**< �쳣�жϴ�������Ķ�ջ��С */

/* logMsg�������� */
#define INCLUDE_LOGMSG_SUPPORT      (1u)    /**< ֧��logMsg */
#define MAX_MSGS                   (10u)    /**< ��Ϣ�����е������Ϣ���� */
#define MAX_BYTES_IN_A_MSG        (200u)    /**< 1��logMsg����ӡ���ֽ��� */
#define TASK_PRIORITY_LOGMSG        (1u)    /**< logMsg��������� */
#define TASK_STK_SIZE_LOGMSG     (1024u)    /**< logMsg����Ķ�ջ��С */

#endif /* __OSCFG_H__ */
/*------------------------------End of oscfg.h-------------------------------*/
