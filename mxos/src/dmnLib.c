/**
 ******************************************************************************
 * @file      dmnLib.c
 * @brief     ����ʵ���������ι������Ļ���.
 * @details   This file including all API functions's implement of dmnLib.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>
#include <listLib.h>
#include <dmnLib.h>
#include <debug.h>
#include <oshook.h>
#include <oscfg.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#pragma pack(push, 1)
typedef struct
{
    struct ListNode nlist;
    TASK_ID task;
    int16_t count;
} dmn_t;
#pragma pack(pop)

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#ifndef DMN_MAX_CHECK_TIME
# define DMN_MAX_CHECK_TIME         (5u)    /**< Ĭ��ι����ʱʱ�䣨���ӣ� */
#endif

#ifndef TASK_PRIORITY_DMN
# define TASK_PRIORITY_DMN           (3u)    /**< Ĭ���������ȼ� */
#endif

#ifndef TASK_STK_SIZE_DMN
# define TASK_STK_SIZE_DMN         (512u)    /**< Ĭ�������ջ */
#endif

#if (DMN_MAX_CHECK_TIME > 10u)
# error "Plesase set DMN_MAX_CHECK_TIME <= (10u)"
#endif
/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
bool_e theRebootSignal = FALSE;

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
static struct ListNode the_registed_list;
static SEM_ID the_dmn_sem = NULL;
static TASK_ID the_dmn_id = NULL;

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
static bool_e
is_taskname_used(TASK_ID task);

static void
dmn_loop(void);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   dmnLib��ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval  OK      : ��ʼ���ɹ�
 * @retval  ERROR   : ��ʼ��ʧ��
 ******************************************************************************
 */
status_t
dmn_init(void)
{
    if (the_dmn_id != NULL)
    {
        return ERROR;
    }
    InitListHead(&the_registed_list);
    the_dmn_sem = semBCreate(1);
    D_ASSERT(the_dmn_sem != NULL);
    the_dmn_id = taskSpawn((const signed char * const )"DAEMON",
            TASK_PRIORITY_DMN, TASK_STK_SIZE_DMN, (OSFUNCPTR)dmn_loop, 0u);
    D_ASSERT(the_dmn_id != NULL);

    return OK;
}

/**
 ******************************************************************************
 * @brief      ���ػ�����������Ѱ���Ƿ��ظ�����
 * @param[in]  None
 * @param[out] None
 * @retval     TRUE : �Ѵ���
 * @retval     FALSE: ������
 ******************************************************************************
 */
static bool_e
is_taskname_used(TASK_ID task)
{
    dmn_t* pdmn = NULL;
    struct ListNode *piter = NULL;

    LIST_FOR_EACH(piter, &the_registed_list)
    {
        /* ȡ�ñ������Ķ��� */
        pdmn = MemToObj(piter, dmn_t, nlist);
        if (pdmn->task == task)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 ******************************************************************************
 * @brief   ���ػ�����ע��
 * @param[in]  None
 * @param[out] None
 *
 * @retval     NULL : ʧ��
 * @retval  !  NULL : ע��ID
 ******************************************************************************
 */
DMN_ID
dmn_register(void)
{
    TASK_ID task = taskIdSelf();
    if (TRUE == is_taskname_used(task))
    {
        printf("err %s already registered!\n", taskName(task));
        return NULL;
    }
    dmn_t *pnew = malloc(sizeof(dmn_t));
    if (NULL == pnew)
    {
        return NULL;
    }
    pnew->count = DMN_MAX_CHECK_TIME;
    pnew->task = task;
    semTake(the_dmn_sem, WAIT_FOREVER);
    ListAddTail(&pnew->nlist, &the_registed_list);
    semGive(the_dmn_sem);

    return (DMN_ID)pnew;
}

/**
 ******************************************************************************
 * @brief   ���ػ�����ι��
 * @param[in]  id   : ι��ע��ʱ���ص�id
 *
 * @retval     OK   : ι���ɹ�
 * @retval     ERROR: ι��ʧ��
 ******************************************************************************
 */
status_t
dmn_sign(DMN_ID id)
{
    dmn_t *pdmn = (dmn_t *)id;

    semTake(the_dmn_sem, WAIT_FOREVER);
    if (FALSE == is_taskname_used(id))
    {
        semGive(the_dmn_sem);
        return ERROR;
    }

    if (pdmn->count > 0u)
    {
        pdmn->count = DMN_MAX_CHECK_TIME;
    }
    semGive(the_dmn_sem);

    return OK;
}

/**
 ******************************************************************************
 * @brief     ���ػ�����ע��
 * @param[in]  id   : ι��ע��ʱ���ص�id
 *
 * @retval     None
 ******************************************************************************
 */
status_t
dmn_unregister(DMN_ID id)
{
    dmn_t *pdmn = (dmn_t *)id;

    semTake(the_dmn_sem, WAIT_FOREVER);
    if (FALSE == is_taskname_used(id))
    {
        semGive(the_dmn_sem);
        return ERROR;
    }

    ListDelNode(&pdmn->nlist);
    semGive(the_dmn_sem);
    free(pdmn);

    return OK;
}

/**
 ******************************************************************************
 * @brief   ���dmn��Ϣ
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
dmn_info(void)
{
    dmn_t *pdmn = NULL;
    struct ListNode *piter = NULL;

    printf("\nName");
    printf("\r\t\t\t");
    printf("RemainCounts\n");

    semTake(the_dmn_sem, WAIT_FOREVER);
    LIST_FOR_EACH(piter, &the_registed_list)
    {
        /* ȡ�ñ������Ķ��� */
        pdmn = MemToObj(piter, dmn_t, nlist);
        printf("%s", taskName(pdmn->task));
        printf("\r\t\t\t");
        printf("%d\n", pdmn->count);
    }
    semGive(the_dmn_sem);
}

/**
 ******************************************************************************
 * @brief   ��dmn����λ
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
dmn_reboot(void)
{
    theRebootSignal = TRUE;
}

/**
 ******************************************************************************
 * @brief   dmn����ִ����
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static void
dmn_loop(void)
{
    uint32_t tick = tickGet();
    dmn_t *pdmn = NULL;
    struct ListNode *piter = NULL;

    theRebootSignal = FALSE;

    FOREVER
    {
        taskDelay(SYS_TICKS_PER_SECOND);
        if (_func_feedDogHook != NULL)
        {
            _func_feedDogHook();    /* ιӲ���� */
        }
        if ((tickGet() - tick) < (SYS_TICKS_PER_SECOND * 60))
        {
            continue;
        }
        semTake(the_dmn_sem, WAIT_FOREVER);
        LIST_FOR_EACH(piter, &the_registed_list)
        {
            /* ȡ�ñ������Ķ��� */
            pdmn = MemToObj(piter, dmn_t, nlist);
            if (pdmn->count >= 0)
            {
                pdmn->count--;
            }
            if (pdmn->count == 0)
            {
                printf("dmn reboot system...\n");
                if (_func_dmnRestHook != NULL)
                {
                    _func_dmnRestHook();    /* �����ι���쳣 */
                }
                if (_func_cpuRestHook != NULL)
                {
                    _func_cpuRestHook();    /* reset CPU */
                }
            }

        }
        semGive(the_dmn_sem);
    }
    printf("dmn will reboot system after 10s...\n");
    /* wait a moment */
    taskDelay(SYS_TICKS_PER_SECOND * 10u);
    if (_func_cpuRestHook != NULL)
    {
        _func_cpuRestHook();    /* reset CPU */
    }
    taskDelete(NULL);
}

/*--------------------------------dmnLib.c-----------------------------------*/
