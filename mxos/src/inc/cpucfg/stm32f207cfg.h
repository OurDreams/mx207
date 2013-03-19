/**
 ******************************************************************************
 * @file       stm32f103cfg.h
 * @brief      ϵͳ�����ļ�.
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
/** ���崦����������ж����� */
#define MAX_INT_COUNT 103

#if MAX_INT_COUNT > 103
# error "file: stm32f207cfg.h MAX_INT_COUNT > 103"
#endif

/** ����CPU����Ƶ120MHZ */
#define SYS_CPU_CLOCK_HZ    (120000000u)

/* ����ʱ�������ն˵�Ƶ�� */
#define  SYS_TICKS_PER_SECOND     (100u)

/* �������ϵͳ�����������ȼ� */
#define  MAX_TASK_PRIORITIES        (8u)


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


#endif /* __STM32F207CFG_H__ */
/*--------------------------End of stm32f103cfg.h----------------------------*/
