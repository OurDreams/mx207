/**
 ******************************************************************************
 * @file      excLib.c
 * @brief     ϵͳ�쳣����ģ��.
 * @details
 *      ϵͳ�쳣�������񣬵��쳣����ʱ��������ӹܣ���¼�쳣����ʱ�ļĴ�����Ϣ��
 *      ����������쳣������
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stdlib.h>
#include <taskLib.h>
#include <debug.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define MAX_EXC_MSG_SIZE	    2	  //< ϵͳ����������Ϣ���е����
#define EXC_MAX_ARGS	        6	  //< �쳣��Ϣ�Ĳ���������

#define OS_TASK_EXC_PRIO        0     //<�쳣�жϴ������������
#define EXC_TASK_STACK_SIZE     1024  //<�쳣�жϴ�������Ķ�ջ��С

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
/**
 *  EXC_MSG ϵͳ������Ϣ�Ľṹ����
 *
 */
typedef struct exc_msg
{
    VOIDFUNCPTR func;       //< pointer to function to invoke
    int arg[EXC_MAX_ARGS];  //< args for function
} EXC_MSG;

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
FUNCPTR     _func_excJobAdd;

/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static TASK_ID excTaskId;    /* ID of excTask */
static MSG_Q_ID excMsgQId;   /* ID of msgQ to excTask */



/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      handle task-level exceptions
 *
 * @details
 *        This routine is spawned as a task by excInit() to perform functions
 *        that cannot be performed at interrupt level.
 *
 * @note
 ******************************************************************************
 */
static void excTask()

{

	EXC_MSG *msg;
	while(1)
	{

		if (OK == msgQReceive(excMsgQId, 6000, (void **) &msg)) {

			(*msg->func)(msg->arg[0], msg->arg[1], msg->arg[2], msg->arg[3],msg->arg[4], msg->arg[5]);
			free(msg);
		}

	}
}

/*-----------------------------------------------------------------------------
 Section: Public Function
 ----------------------------------------------------------------------------*/

/**
 ******************************************************************************
 * @brief     request a task-level function call from interrupt level .
 * @param[in]   func: the function would be executed in excTask.
 * @param[in]   arg1,arg2,arg3,arg4,arg5,arg6 : the arguments of above function.
 * @retval
 *             OK  or ERROR
 *
 * @details
 *     This routine allows interrupt level code to request a function call
 *      to be made by excTask at task-level.
 * @note
 ******************************************************************************
 */
status_t excJobAdd(VOIDFUNCPTR func, int arg1, int arg2, int arg3, int arg4,
		int arg5, int arg6) {
	EXC_MSG *msg = malloc(sizeof(EXC_MSG));

	if (msg != NULL) {
		msg->func = func;
		msg->arg[0] = arg1;
		msg->arg[1] = arg2;
		msg->arg[2] = arg3;
		msg->arg[3] = arg4;
		msg->arg[4] = arg5;
		msg->arg[5] = arg6;
		return msgQSend(excMsgQId, (void *) msg);
	}
	printf("excJobAdd: FAILURE!\n");
	return ERROR;

}


/**
 ******************************************************************************
 * @brief      initialize the exception handling module.
 * @retval
 *      OK, or ERROR if a message queue could not be created
 *      or excTask() could not be spawned
 * @details
 *       This routine installs the exception handling facilities and spawns excTask(),
 *       which performs special exception handling functions that need to be done
 *       at task level. It also creates the message queue used to communicate with
 *       excTask().
 * @note
 ******************************************************************************
 */
status_t excInit()

{
    if (excTaskId != NULL)
        return (ERROR);
    excMsgQId = msgQCreate(MAX_EXC_MSG_SIZE);

    D_ASSERT(excMsgQId != NULL);

    excTaskId = taskSpawn((const signed char * const ) "tExcTask",
            OS_TASK_EXC_PRIO, EXC_TASK_STACK_SIZE, (OSFUNCPTR) excTask, 0);
    D_ASSERT(excTaskId != NULL);

    _func_excJobAdd = excJobAdd;

    printf("excInit: DONE! \n");
    return  (OK);
}


/*----------------------------End of excLib.c--------------------------------*/

