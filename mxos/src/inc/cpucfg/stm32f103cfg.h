/**
 ******************************************************************************
 * @file       stm32f103cfg.h
 * @brief      ϵͳ�����ļ�.
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
 *  ���崦����������ж�����
 */
#define MAX_INT_COUNT 63

#if MAX_INT_COUNT > 63
#error "��������ж���"
#endif

/**
 *  ����CPU����Ƶ72MHZ
 */
#define SYS_CPU_CLOCK_HZ  7200000

/**
 *  ����ʱ�������ն˵�Ƶ��
 */
#define  SYS_TICKS_PER_SECOND       100

/**
 *  �������ϵͳ�����������ȼ�
 */
#define  MAX_TASK_PRIORITIES        8


/* shell�������� */
#define TASK_PRIORITY_SHELL         (1u)    /**< shell�������ȼ� */
#define TASK_STK_SIZE_SHELL      (1024u)    /**< shell�����ջ */

/* dmn�������� */
#define TASK_PRIORITY_DMN           (1u)    /**< DMN�������ȼ� */
#define TASK_STK_SIZE_DMN         (512u)    /**< DMN�����ջ */
#define DMN_MAX_CHECK_TIME          (6u)    /**< Ĭ��ι����ʱʱ�䣨6*10�룩 */

#define TASK_PRIORITY_EXC           (0u)    /**< �쳣�жϴ������������ */
#define TASK_STK_SIZE_EXC         (512u)    /**< �쳣�жϴ�������Ķ�ջ��С */


#endif /* __STM32F103CFG_H__ */
/*--------------------------End of stm32f103cfg.h----------------------------*/
