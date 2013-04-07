/**
 ******************************************************************************
 * @file      bsptask.c
 * @brief     C Source file of bsptask.c.
 * @details   用来维护底层的的一些需要循环的程序，例如轮询按键等等，bsptask会每隔一
 *            段时间调用这些注册进来的函数。
 *
 *
 * @copyright
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stddef.h>
#include <udelay.h>
#include <listLib.h>
#include <memLib.h>
#include <taskLib.h>
#include <bsptask.h>
#include <stdio.h>
#include <stdlib.h>
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#define TASK_PRIORITY_BSP    4u
#define BSPSTACKSIZE         (512u)
typedef struct
{
	struct ListNode nlist;
	VOIDFUNCPTR pfunc;
}func_t;
static struct ListNode the_registed_list;
/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static SEM_ID the_bsp_sem = NULL;//操作链表时用
/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   任务执行体，遍历链表
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static void
bsp_task_loop(void)
{
    struct ListNode *piter = NULL;
    func_t *pfunc = NULL;
	while(1)
	{
		_usleep(100000);//100ms
		semTake(the_bsp_sem, WAIT_FOREVER);
		LIST_FOR_EACH(piter, &the_registed_list)
		{
			pfunc = MemToObj(piter, func_t, nlist);
			if(pfunc->pfunc != NULL)
			{
				pfunc->pfunc();
			}
		}
		semGive(the_bsp_sem);
	}
}
/**
 ******************************************************************************
 * @brief      创建信号量，初始化链表，创建任务
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
status_t bsp_task_init(void)
{
	the_bsp_sem = semBCreate(1);
	if(the_bsp_sem == NULL)
	{
		printf("Create bsp task semaphore error\n");
		return ERROR;
	}
	InitListHead(&the_registed_list);
	taskSpawn((const signed char * const )"bsp_task",
	            TASK_PRIORITY_BSP, BSPSTACKSIZE, (VOIDFUNCPTR)bsp_task_loop, 0u);
	return OK;
}

BSP_ID func_register(OSFUNCPTR func)
{
	func_t *pnew = malloc(sizeof(func_t));
	if(pnew == NULL)
	{
		return NULL;
	}
	pnew->pfunc = func;
    semTake(the_bsp_sem, WAIT_FOREVER);
    ListAddTail(&pnew->nlist, &the_registed_list);
    semGive(the_bsp_sem);

    return (BSP_ID)pnew;
}
status_t func_unregister(BSP_ID func_id)
{
	func_t *pfunc = (func_t *)func_id;
	semTake(the_bsp_sem, WAIT_FOREVER);
    ListDelNode(&pfunc->nlist);
    semGive(the_bsp_sem);
    free(pfunc);
	return OK;
}
/*---------------------------- bsptask.c --------------------------------*/


