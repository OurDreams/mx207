/**
 ******************************************************************************
 * @file      memLib.c
 * @brief     �ڴ����ģ��
 * @details   ���Ĺ���̬�ڴ���估�ͷ�(���Ĳ���ʹ���ź���)
 * @copyright
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <listLib.h>
#include <intLib.h>
#include <debug.h>
#include <memLib.h>

/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
#define WORD_SIZE               sizeof(uint32_t)
#define WORD_ALIGN_UP(addr)     (((addr) + WORD_SIZE - 1) & ~(WORD_SIZE - 1))
#define WORD_ALIGN_DOWN(addr)   ((addr) & ~(WORD_SIZE - 1))

#define DWORD_SIZE              (WORD_SIZE << 1)

#define LIST_NODE_SIZE          WORD_ALIGN_UP(sizeof(struct ListNode))
#define LIST_NODE_ALIGN(nSize)  (((nSize) + LIST_NODE_SIZE - 1) & ~(LIST_NODE_SIZE - 1))
#define MIN_HEAP_LEN            1024
/* �жϿ����ڴ�ڵ�����:���λ�Ƿ�Ϊ1 */
#define IS_FREE(nSize)          (((nSize) & (WORD_SIZE - 1)) == 0)
#define GET_SIZE(pRegion)       ((pRegion)->nCurrSize & ~(WORD_SIZE - 1))

struct MemRegion
{
    size_t   nPrevSize;    /**< ��һ���ڵ�Ĵ�С */
    size_t   nCurrSize;    /**< ��ǰ��Ĵ�С */
    struct ListNode lnMemRegion;    /**< ͨ������ڵ� */
};
/* ע�⣺���ǵ���һ���ڵ��ַ����ͨ��currsize����õ�����������Ľṹ��ֻ��
   ��һ���ڵ�Ĵ�С */

static struct ListNode g_listFreeRegion;    /**< ָ���ڴ����� */
static uint32_t totleMemSize = 0u;
static bool_e is_heap_init = FALSE;
/*-----------------------------------------------------------------------------
Section: Function Definitions
-----------------------------------------------------------------------------*/
/* ��ȡ��һ���ڵ��ַ */
static inline struct MemRegion *GetSuccessor(struct MemRegion *pRegion)
{
    return (struct MemRegion *)((uint8_t *)pRegion + DWORD_SIZE + GET_SIZE(pRegion));
}

/* ��ȡ��һ���ڵ��ַ */
static inline struct MemRegion *GetPredeccessor(struct MemRegion *pRegion)
{
    return (struct MemRegion *)((uint8_t *)pRegion - (pRegion->nPrevSize & ~(WORD_SIZE - 1)) - DWORD_SIZE);
}

/* ��������pRegion�ڵ�Ĵ�С */
static inline void RegionSetSize(struct MemRegion *pRegion, size_t nSize)
{
    struct MemRegion *pSuccRegion;

    pRegion->nCurrSize = nSize;     /* ��ǰ�ڵ�ߴ縳ֵ */

    pSuccRegion = GetSuccessor(pRegion);    /* ��ȡ��һ�ڵ� */
    pSuccRegion->nPrevSize = nSize; /* ��һ�ڵ����һ�ڵ㸳ֵ */
}

/**
 ******************************************************************************
 * @brief      ��ʼ����
 * @param[in]  start    : ����ʼ��ַ
 * @param[in]  end      : ��ĩ��ַ
 * @retval     OK
 * @retval     ERROR
 *
 * @details  ��ʼ��������ڴ��������������㣬ͷ����β�ڵ�
 *
 * @note
 ******************************************************************************
 */
status_t
mem_init(uint32_t start, uint32_t end)
{
    struct MemRegion *pFirst, *pTail;

    start = WORD_ALIGN_UP(start);   /* malloc�����׵�ַ��up�ֽڶ��� */
    end   = WORD_ALIGN_DOWN(end);   /* malloc����ĩ��ַ��down�ֽڶ��� */

    if (start + MIN_HEAP_LEN >= end)    /* ���ɷ���Ķѿռ�С����С�ѳߴ磬�򷵻� ���� */
        return ERROR;

    /* ��ʼ��ʱ��ͷ����β�ڵ㣬������Ϊ��ͷ�Ͷ�βλ�� */
    pFirst = (struct MemRegion *)start;
    pTail  = (struct MemRegion *)(end - DWORD_SIZE);    /* ��֪��Ϊ��-DWORD_SIZE */

    totleMemSize = (size_t)pTail - (size_t)pFirst;
    pFirst->nPrevSize = 1;  /* ����һ���ڵ�ߴ縳1��ʾ��һ�ڵ�Ϊʹ��״̬ */
    /* ����pFirst�ڵ�ɷ����ڴ� */
    pFirst->nCurrSize = (size_t)pTail - (size_t)pFirst - DWORD_SIZE;

    pTail->nPrevSize = pFirst->nCurrSize;   /* pTail��һ���ΪpTail */
    pTail->nCurrSize = 1;   /* pTail��һ���Ϊʹ��״̬ */

    InitListHead(&g_listFreeRegion);    /* ��ʼ���ڴ�free���� */

    /* ����ʼ����free�ڵ���ӵ��ڴ�free������ */
    ListAddTail(&pFirst->lnMemRegion, &g_listFreeRegion);
    is_heap_init = TRUE;

    return OK;
}


/**
 ******************************************************************************
 * @brief      malloc������ʵ��
 * @param[in]  nSize    : ��Ҫ����Ĵ�С
 * @retval     ����ɹ����ص�ַ��ʧ�ܷ���NULL
 ******************************************************************************
 */
void *
malloc(size_t nSize)
{
    void *p = NULL;
    struct ListNode *iter;
    size_t nAllocSize, nRestSize;
    struct MemRegion *pCurrRegion, *pSuccRegion;    // *pPrevRegion,

    if (is_heap_init != TRUE)
    {
        return NULL;
    }
    intLock();    /* �����ٽ��� */

    /* ����ʵ����Ҫ�Ĵ�С��4�ֽڶ��룩= Ҫ������ڴ��С + �ڵ���Ϣ��С */
    nAllocSize = LIST_NODE_ALIGN(nSize);

    /* ����free�ڴ����� */
    LIST_FOR_EACH(iter, &g_listFreeRegion)
    {
        /* ȡ�ñ������Ķ��� */
        pCurrRegion = MemToObj(iter, struct MemRegion, lnMemRegion);
        // printf("%d <--> %d\n", pCurrRegion->nCurrSize, nAllocSize);
        /* �ýڵ����㹻�ռ����nAllocSize,�����do_alloc */
        if (pCurrRegion->nCurrSize >= nAllocSize)
            goto do_alloc;
    }
    /* ��û�пռ�����ˣ��򷵻ؿ�ָ�� */
    intUnlock();  /* �˳��ٽ��� */

    return NULL;

do_alloc:
    ListDelNode(iter);  /* ����ɾ����ǰ�ڵ� */

    /* ��������ڴ��ʣ��ֵ */
    nRestSize = pCurrRegion->nCurrSize - nAllocSize;

    /* �������ʣ��ռ䲻�����ٷ���ڵ� */
    if (nRestSize < sizeof(struct MemRegion))
    {
        /* ������ʣ���ڴ�С�ڽ���Сʱ��ȫ���ڴ���䡣��־��ǰ�ڵ��ڴ�ʹ��λ*/
        RegionSetSize(pCurrRegion, pCurrRegion->nCurrSize | 1);
    }
    else
    {
        /* �ڵ�ǰ�ڵ����ռ�,����־ʹ��λ */
        RegionSetSize(pCurrRegion, nAllocSize | 1);

        /* �����һ�ڵ��ַ */
        pSuccRegion = GetSuccessor(pCurrRegion);
        /* ���¼�����һ�ڵ�ɷ����ڴ� */
        RegionSetSize(pSuccRegion, nRestSize - DWORD_SIZE);
        /* ������ڵ����ӵ��ڴ�free���������� */
        ListAddTail(&pSuccRegion->lnMemRegion, &g_listFreeRegion);
    }

    /* ����ڴ��ַ ��������,��ֱ��дlnMemRegion*/
    p = &pCurrRegion->lnMemRegion;

    intUnlock();  /* �˳��ٽ��� */

    return p;
}

/**
 ******************************************************************************
 * @brief      �ڴ��ͷ�
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
free(void *p)
{
    struct MemRegion *pCurrRegion, *pSuccRegion;

    if (is_heap_init != TRUE)
    {
        return ;
    }
    intLock();    /* �����ٽ��� */

    pCurrRegion = (struct MemRegion *)((size_t)p - DWORD_SIZE);
    pSuccRegion = GetSuccessor(pCurrRegion);

    /* ����һ���ڵ�Ϊfree״̬ */
    if (IS_FREE(pSuccRegion->nCurrSize))
    {
        /* ��ϲ���ǰ�ڵ����һ���ڵ㣬�����ܿ���ռ� */
        RegionSetSize(pCurrRegion, GET_SIZE(pCurrRegion) + pSuccRegion->nCurrSize + DWORD_SIZE);
        /* ɾ����һ���ڵ� */
        ListDelNode(&pSuccRegion->lnMemRegion);
    }
    else
    {
        /* ����λ��Ϊ0����ʾ����״̬ */
        RegionSetSize(pCurrRegion, GET_SIZE(pCurrRegion));
    }

    /* ����һ���ڵ��ǿ���״̬ */
    if (IS_FREE(pCurrRegion->nPrevSize))
    {
        struct MemRegion *pPrevRegion;

        /* ��ȡ��һ���ڵ��ַ */
        pPrevRegion = GetPredeccessor(pCurrRegion);
        /* �ϲ����ڵ�����һ���ڵ� */
        RegionSetSize(pPrevRegion, pPrevRegion->nCurrSize + pCurrRegion->nCurrSize + DWORD_SIZE);
    }
    else
    {
        /* �����ڵ���ӵ��ڴ�free������ */
        ListAddTail(&pCurrRegion->lnMemRegion, &g_listFreeRegion);
    }

    intUnlock();   /* �˳��ٽ��� */
}

/**
 ******************************************************************************
 * @brief      ����ڴ�ʹ����Ϣ
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
showMenInfo(void)
{
    struct ListNode *iter;
    struct MemRegion *pCurrRegion;
    uint32_t TotalFreeSize = 0u;
    uint32_t MaxSize = 0u;
    uint32_t MinSize = 0xffffffff;
    if (is_heap_init != TRUE)
    {
        printf(" Heap not initialized! Please call 'mem_init()'.\n");
        return ;
    }

    intLock();    /* �����ٽ��� */
    /* ����free�ڴ����� */
    LIST_FOR_EACH(iter, &g_listFreeRegion)
    {
        /* ȡ�ñ������Ķ��� */
        pCurrRegion = MemToObj(iter, struct MemRegion, lnMemRegion);
        if (pCurrRegion->nCurrSize > MaxSize)
            MaxSize = pCurrRegion->nCurrSize;
        if (pCurrRegion->nCurrSize < MinSize)
            MinSize = pCurrRegion->nCurrSize;
        TotalFreeSize += pCurrRegion->nCurrSize;
    }
    intUnlock();   /* �˳��ٽ��� */

    printf("********** Heap Monitor ***********\n");
    printf(" TotalHeapMem = %4d Kb  %4d Byte\n", totleMemSize / 1024, totleMemSize % 1024);
    printf(" TotalFreeMem = %4d Kb  %4d Byte\n", TotalFreeSize / 1024, TotalFreeSize % 1024);
    printf(" MaxFreeMem   = %4d Kb  %4d Byte\n", MaxSize / 1024, MaxSize % 1024);
    printf(" MinFreeMem   = %4d Kb  %4d Byte\n", MinSize / 1024, MinSize % 1024);
    //printf(" Fragindices  = %.2f\n", 1-(float)MaxSize / (float)TotalFreeSize);
    printf("***********************************\n");
}
/*------------------------------- memLib.c ----------------------------------*/
