/**
 ******************************************************************************
 * @file      devnull.c
 * @brief     ����ʵ��NULL����.
 * @details   This file including all API functions's implement of devnull.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <types.h>
#include <devLib.h>

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
/* NONE */

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
/**
 ******************************************************************************
 * @brief   NULL�豸��ʼ������
 * @param[in]  None
 * @param[out] None
 *
 * @retval     OK   : ��ʼ���ɹ�
 ******************************************************************************
 */
static status_t
devnull_init(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   NULL�豸�򿪷���
 * @param[in]  None
 * @param[out] None
 *
 * @retval     OK   : �򿪳ɹ�
 ******************************************************************************
 */
static int32_t
devnull_open(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   NULL�豸�ͷŷ���
 * @param[in]  None
 * @param[out] None
 *
 * @retval     OK   : �ͷųɹ�
 ******************************************************************************
 */
static status_t
devnull_relesae(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   NULL�豸�رշ���
 * @param[in]  None
 * @param[out] None
 *
 * @retval     OK   : �رճɹ�
 ******************************************************************************
 */
static status_t
devnull_close(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   NULL�豸��ȡ����
 * @param[in]  None
 * @param[out] None
 *
 * @retval     size
 ******************************************************************************
 */
static size_t
devnull_read(struct device* dev, int32_t pos, void *buffer, size_t size)
{
    return size;
}

/**
 ******************************************************************************
 * @brief   NULL�豸д�뷽��
 * @param[in]  None
 * @param[out] None
 *
 * @retval     size
 ******************************************************************************
 */
static size_t
devnull_write(struct device* dev, int32_t pos, const void *buffer, size_t size)
{
    return size;
}


/**
 ******************************************************************************
 * @brief   NULL�豸���Ʒ���
 * @param[in]  None
 * @param[out] None
 *
 * @retval  0
 ******************************************************************************
 */
static int32_t
devnull_ioctl(struct device* dev, uint32_t cmd, void *args)
{
    return 0;
}

const static fileopt_t the_testopt =
{
    .init = devnull_init,
    .release = devnull_relesae,
    .open = devnull_open,
    .close = devnull_close,
    .read = devnull_read,
    .write = devnull_write,
    .ioctl = devnull_ioctl,
};


/**
 ******************************************************************************
 * @brief   ����NULL�豸
 * @param[in]  None
 * @param[out] None
 * @retval     OK   : �����ɹ�
 * @retval     ERROR: ����ʧ��
 ******************************************************************************
 */
status_t
devnull_create(void)
{
    if (OK != dev_create("NULL", &the_testopt, MKDEV(0, 0)))
    {
        printf("devnull create err\n");
        return ERROR;
    }
    printf("devnull create OK!\n");
    return OK;
}

/*-------------------------------devnull.c-----------------------------------*/
