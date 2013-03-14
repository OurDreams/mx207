/**
 ******************************************************************************
 * @file      devlib.c
 * @brief     C Source file of devlib.c.
 * @details   �豸��������
 *
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <intLib.h>
#include <devLib.h>


#ifdef Dprintf
#undef Dprintf
#endif
#define Dprintf(x...)

static SEM_ID the_devlib_lock = NULL;
static struct ListNode the_dev_list;    /* ָ���豸���� */
static device_t* the_opend_devs[MAX_OPEN_NUM];  /* �Ѵ��豸�� */

/**
 ******************************************************************************
 * @brief �жϾ���Ƿ�Ϸ�
 * @param[in]  realfd
 * @retval     FALSE    : �Ƿ�
 * @retval     TRUE     : �Ϸ�
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static bool_e
is_fd_valid(int32_t realfd)
{
    if (realfd >= MAX_OPEN_NUM)
    {
        Dprintf("fd is out of range!\n");
        return FALSE;
    }

    if (the_opend_devs[realfd] == NULL)
    {
        Dprintf("fd is not opend!\n");
        return FALSE;
    }

    return TRUE;
}

/**
 ******************************************************************************
 * @brief Ѱ�ҿ��о���ռ�,������
 * @param[in]  *pnode    : �豸������
 * @param[out] None
 * @retval     -1   : �޿��пռ�
 * @retval     >0   : ����ľ��
 ******************************************************************************
 */
static int32_t
find_free_fd(device_t* pnode)
{
    for (int32_t i = 0; i < MAX_OPEN_NUM; i++)
    {
        if (the_opend_devs[i] == NULL)
        {
            the_opend_devs[i] = pnode;
            Dprintf("find free fd node\n");
            return i + 1;   /* ע�ⷵ�ؾ����1��ֹ��� */
        }
    }
    Dprintf("can not find free fd node\n");

    return -1;
}

/**
 ******************************************************************************
 * @brief      ���Ѵ��ļ��б��в�ѯ�ļ�
 * @param[in]  * pname  : ���ҵ��豸��
 *
 * @retval     -1   ���Ҳ���
 * @retval     >=0  : �Ѵ��ļ������±�(ʵ�ʵ��ļ����)
 ******************************************************************************
 */
static int32_t
find_opend_by_name(const char_t* pname)
{
    for (int32_t i = 0; i < MAX_OPEN_NUM; i++)
    {
        if (strncmp(the_opend_devs[i]->name, pname, MAX_DEVICE_NAME) == 0)
        {
            Dprintf("dev:%s is using\n", pname);
            return i;
        }
    }
    return -1;
}

/**
 ******************************************************************************
 * @brief Ѱ����ע����豸 (�����Ʋ���)
 * @param[in]  * pname  : ���ҵ��豸��
 *
 * @retval     NULL ���Ҳ���
 * @retval     �ǿ�    : �豸��������ַ
 ******************************************************************************
 */
static device_t*
find_dev_by_name(const char_t* pname)
{
    device_t* pnode = NULL;
    struct ListNode *iter;

    LIST_FOR_EACH(iter, &the_dev_list)
    {
        /* ȡ�ñ������Ķ��� */
        pnode = MemToObj(iter, struct device, list);
        if (strncmp(pnode->name, pname, sizeof(pnode->name)) == 0)
        {
            return pnode;
        }
    }
    return NULL;
}

/**
 ******************************************************************************
 * @brief      �豸���ĳ�ʼ��
 *
 * @retval     OK   : ��ʼ���ɹ�
 * @retval     ERROR: ��ʼ��ʧ��
 ******************************************************************************
 */
status_t
devlib_init(void)
{
    if (the_devlib_lock != NULL)
        return OK;
    if ((the_devlib_lock = semBCreate(1)) == NULL)
    {
        Dprintf("semBCreate err\n");
        return ERROR;
    }
    InitListHead(&the_dev_list);
    Dprintf("init OK\n");

    return OK;
}

/**
 ******************************************************************************
 * @brief �����豸
 * @param[in]  *pname       : �豸��
 * @param[in]  *pfileopt    ���豸��������
 * @param[in]  serial       ���豸���кţ���С��ţ�

 * @retval  OK      : �ɹ�
 * @retval  ERROR   : ʧ��
 ******************************************************************************
 */
status_t
dev_create(const char_t* pname, const fileopt_t* pfileopt, int32_t serial)
{
    D_ASSERT(pname != NULL);
    D_ASSERT(pfileopt != NULL);

    /* ��ֹ�ظ�ע�� */
    if (find_dev_by_name(pname) != NULL)
    {
        return ERROR;
    }
    device_t* new = malloc(sizeof(struct device));
    if (new == NULL)
    {
        printf("dev_create out of mem! when creat name: %s.\n", pname);
        return ERROR;
    }

    memset(new, 0x00, sizeof(struct device));
    new->lock = semBCreate(1);
    if (new->lock == NULL)
    {
        printf("dev_create create sem err.\n");
        free(new);
        return ERROR;
    }
    strncpy(new->name, pname, sizeof(new->name));
    memcpy(&new->fileopt, pfileopt, sizeof(fileopt_t));
    new->serial = serial;
    if (new->fileopt.init != NULL)
    {
        if (new->fileopt.init(new) != OK)
        {
            semDelete(new->lock);
            free(new);
            return ERROR;
        }
    }
    /* �����豸�ڵ�������� */
    semTake(the_devlib_lock, WAIT_FOREVER);
    ListAddTail(&new->list, &the_dev_list);
    semGive(the_devlib_lock);

    return OK;
}

/**
 ******************************************************************************
 * @brief �豸�ͷ�
 * @param[in]  *pname   : �豸��
 *
 * @retval  OK      : �ɹ�
 * @retval  ERROR   : ʧ��
 ******************************************************************************
 */
status_t
dev_release(const char_t* pname)
{
    D_ASSERT(pname != NULL);

    /* �ж��豸�Ƿ���ʹ�� */
    if (0 < find_opend_by_name(pname))
    {
        Dprintf("dev:%s is using\n", pname);
        return ERROR;
    }

    device_t* pnode = find_dev_by_name(pname);
    if (pnode == NULL)
    {
        Dprintf("dev:%s is not found\n", pname);
        return ERROR;
    }

    if (pnode->fileopt.release != NULL)
    {
        if (OK != pnode->fileopt.release(pnode))
        {
            Dprintf("dev:%s release err!\n\n", pname);
            return ERROR;
        }
    }

    semTake(the_devlib_lock, WAIT_FOREVER);
    ListDelNode(&pnode->list);
    semGive(the_devlib_lock);
    semDelete(pnode->lock);
    free(pnode);

    return OK;
}

/**
 ******************************************************************************
 * @brief �豸��
 * @param[in]  *pname   : �豸��
 * @param[in]   flags   : ��ģʽO_RDONLY | OWRONLY | O_RDWR
 *
 * @retval  >=0   : �ɹ�
 * @retval  - 1   : ʧ��
 ******************************************************************************
 */
int32_t
dev_open(const char_t* pname, int32_t flags)
{
    int32_t fd;
    device_t* pnode;

    D_ASSERT(pname != NULL);

    /* ����豸�Ѵ�������ļ��򿪴����ۼ� */
    fd = find_opend_by_name(pname);
    if ((fd = find_opend_by_name(pname)) >= 0)
    {
        pnode = the_opend_devs[fd];
        (void)semTake(pnode->lock, WAIT_FOREVER);
        pnode->usrs++;
        (void)semGive(pnode->lock);
        return OK;
    }
    if ((pnode = find_dev_by_name(pname)) == NULL)
    {
        Dprintf("dev:%s is not found\n", pname);
        return -1;
    }
    if ((fd = find_free_fd(pnode)) < 0)
    {
        Dprintf("dev: opend max file.\n");
        return -1;
    }

    pnode->flags = flags;
    if (pnode->fileopt.open != NULL)
    {
        (void)semTake(pnode->lock, WAIT_FOREVER);
        if (OK != pnode->fileopt.open(pnode))
        {
            (void)semGive(pnode->lock);
            Dprintf("dev: opend err.\n");
            return -1;
        }
        (void)semGive(pnode->lock);
    }
    pnode->usrs++;

    return fd;
}

/**
 ******************************************************************************
 * @brief �豸��
 * @param[in]  fd       : �豸���
 * @param[in] *buf      : д�����ַ
 * @param[in]  count    : д���ֽ���

 * @retval  >=0   : �ɹ�
 * @retval  - 1   : ʧ��
 ******************************************************************************
 */
int32_t
dev_read(int32_t fd, void* buf, int32_t count)
{
    int32_t size = -1;
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    device_t* pnode = the_opend_devs[realfd];

    if ((pnode->fileopt.read == NULL) || (pnode->flags & O_WRONLY))
    {
        return -1;
    }

    (void)semTake(pnode->lock, WAIT_FOREVER);
    size = pnode->fileopt.read(pnode, pnode->offset, buf, count);
    (void)semGive(pnode->lock);

    return size;
}

/**
 ******************************************************************************
 * @brief �豸д
 * @param[in]  fd       : �豸���
 * @param[in] *buf      : д�����ַ
 * @param[in]  count    : д���ֽ���

 * @retval  >=0   : �ɹ�
 * @retval  - 1   : ʧ��
 ******************************************************************************
 */
int32_t
dev_write(int32_t fd, const void* buf, int32_t count)
{
    int32_t size = -1;
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    device_t* pnode = the_opend_devs[realfd];

    if ((pnode->fileopt.write == NULL) || (pnode->flags & O_RDONLY))
    {
        return -1;
    }

    (void)semTake(pnode->lock, WAIT_FOREVER);
    size = pnode->fileopt.write(pnode, pnode->offset, buf, count);
    (void)semGive(pnode->lock);

    return size;
}

/**
 ******************************************************************************
 * @brief �豸ioctl
 * @param[in]  fd       : �豸���
 * @param[in]  cmd      : ����
 * @param[in]  *args    : ����

 * @retval  >=0   : �ɹ�
 * @retval  -1    : ʧ��
 ******************************************************************************
 */
int32_t
dev_ioctl(int32_t fd, uint32_t cmd, void *args)
{
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    device_t* pnode = the_opend_devs[realfd];

    if (pnode->fileopt.ioctl == NULL)
    {
        return -1;
    }

    (void)semTake(pnode->lock, WAIT_FOREVER);
    int32_t ret = pnode->fileopt.ioctl(pnode, cmd, args);
    (void)semGive(pnode->lock);

    return ret;
}

/**
 ******************************************************************************
 * @brief �豸�ر�
 * @param[in]  fd       : �豸���
 *
 * @retval  1   : �رճɹ�
 * @retval -1   : �ر�ʧ��
 ******************************************************************************
 */
int32_t
dev_close(int32_t fd)
{
    int32_t realfd = fd - 1;    /* ����ȡ����ʵ��fd */

    if (FALSE == is_fd_valid(realfd))
    {
        return -1;
    }
    device_t* pnode = the_opend_devs[realfd];
    pnode->usrs--;
    if (pnode->usrs <= 0)
    {
        if (pnode->fileopt.close != NULL)
        {
            if (OK != pnode->fileopt.close(pnode))
            {
                return -1;
            }
        }
        the_opend_devs[realfd] = NULL;  /* release fd */
    }

    return 1;
}

/**
 ******************************************************************************
 * @brief      ��ʾ�豸��Ϣ
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
show_devlib_info(void)
{
    printf("   list devices:\n");
    printf("  -- dev name -- \r\t\t-- opend num --\n");
    device_t* pnode = NULL;
    struct ListNode *iter;

    semTake(the_devlib_lock, WAIT_FOREVER);
    LIST_FOR_EACH(iter, &the_dev_list)
    {
        /* ȡ�ñ������Ķ��� */
        pnode = MemToObj(iter, struct device, list);
        printf("       %s\r\t\t      %d\n", pnode->name, pnode->usrs);
    }
    semGive(the_devlib_lock);
    //printf("  --- --- --- ---\r\t\t--- --- --- ---\n");
}
/*------------------------------- devlib.c ----------------------------------*/
