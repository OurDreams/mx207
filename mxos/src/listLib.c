/**
 ******************************************************************************
 * @file      listLib.c
 * @brief     ͨ��������.
 * @details   This file including all API functions's implement of list.c.
 * @copyright
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <listLib.h>

/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Constant Definitions
-----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
Section: Global Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Local Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Local Function Prototypes
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Function Definitions
-----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      �����ʼ��
 * @param[in]  *pHead   :
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
InitListHead(struct ListNode *pHead)
{
    pHead->pNextNode = pHead;
    pHead->pPrevNode = pHead;
}

/**
 ******************************************************************************
 * @brief      .
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static inline void
__list_add(struct ListNode *pNew,
            struct ListNode *pPrev,
            struct ListNode *pNext)
{
    pNext->pPrevNode = pNew;
    pNew->pNextNode  = pNext;
    pNew->pPrevNode  = pPrev;
    pPrev->pNextNode = pNew;
}

/**
 ******************************************************************************
 * @brief  ���ڵ��������ͷ
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
ListAddHead(struct ListNode *pNew, struct ListNode *pHead)
{
    __list_add(pNew, pHead, pHead->pNextNode);
}

/**
 ******************************************************************************
 * @brief   ���ڵ��������β
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
ListAddTail(struct ListNode *pNew, struct ListNode *pHead)
{
    __list_add(pNew, pHead->pPrevNode, pHead);
}

/**
 ******************************************************************************
 * @brief      .
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static inline void
__list_del(struct ListNode *pPrev, struct ListNode *pNext)
{
    pNext->pPrevNode = pPrev;
    pPrev->pNextNode = pNext;
}

/**
 ******************************************************************************
 * @brief      ɾ���ڵ�
 * @param[in]  *pHead   :
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
ListDelNode(struct ListNode *pNode)
{
    __list_del(pNode->pPrevNode, pNode->pNextNode);
}

/**
 ******************************************************************************
 * @brief      �ж������Ƿ�Ϊ��
 * @param[in]  *pHead   :
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int
ListIsEmpty(const struct ListNode *pHead)
{
    return pHead->pNextNode == pHead;
}
/*------------------------------ listLib.c ----------------------------------*/
