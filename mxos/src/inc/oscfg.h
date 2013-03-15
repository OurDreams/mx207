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


/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define OS_RELEASE  "0.0.1"


#define MEM_BOUNDS_CHECK
#define MEM_CHECK_VALUE 2000


/**
 *  ���崦����������ж�����
 */
#define MAX_INT_COUNT 103

#if MAX_INT_COUNT > 103
#error "��������ж���"
#endif

/**
 *  ����CPU����Ƶ120MHZ
 */
#define SYS_CPU_CLOCK_HZ  120000000

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



#define TASK_PRIORITY_DMN           (1u)    /**< DMN�������ȼ� */
#define TASK_STK_SIZE_DMN        (1024u)    /**< DMN�����ջ */

#define TASK_PRIORITY_EXC
#define TASK_STK_SIZE_EXC


#endif /* __OSCFG_H__ */
/*------------------------------End of oscfg.h-------------------------------*/
