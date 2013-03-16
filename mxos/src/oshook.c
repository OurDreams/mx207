/**
 ******************************************************************************
 * @file      oshook.c
 * @brief     os������ṩ�Ĺ��ӽӿڿ�.
 * @details   This file including all API functions's  implement of mxhook.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/**
 *  @brief ιӲ������������
 */
FUNCPTR     _func_feedDogHook = NULL;

/**
 *  @brief оƬ��λ��������
 */
FUNCPTR     _func_cpuRestHook = NULL;

/**
 *  @brief ����ʱδι���쳣ʱ��������
 */
FUNCPTR     _func_dmnRestHook = NULL;

/**
 *  @brief �ں������ջ�������ʱ�����õĹ��Ӻ�����
 *  ���������ļ��б����ֳ���Ϣ��
 *  _func_evtLogOverStackHook(taskId,pcTaskName);
 */
VOIDFUNCPTR _func_evtLogOverStackHook;

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*--------------------------------oshook.c-----------------------------------*/