/**
 ******************************************************************************
 * @file      ttyLib.c
 * @brief     C Source file of ttyLib.c.
 * @details   ͨ��tty�豸����.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <ttyLib.h>
#include <devLib.h>
#include <taskLib.h>
#include <ring.h>
#include <debug.h>
#include <oscfg.h>
/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
#define TTY_EXPARAM (*((tty_exparam_t *)dev->param))

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
 * @brief   tty�豸��ʼ������
 * @param[in]  dev  : �豸�ڵ�
 *
 * @retval     OK   : ��ʼ���ɹ�
 ******************************************************************************
 */
static status_t
ttylib_init(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty�豸�ͷŷ���
 * @param[in]  dev  : �豸�ڵ�
 *
 * @retval     OK   : �ͷųɹ�
 ******************************************************************************
 */
static status_t
ttylib_release(struct device* dev)
{
    free(TTY_EXPARAM.ring.rd.buf);
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty�豸open����
 * @param[in]  dev  : �豸�ڵ�
 *
 * @retval     OK   : �򿪳ɹ�
 ******************************************************************************
 */
static status_t
ttylib_open(struct device* dev)
{
    if (TTY_EXPARAM.popt->tr_enable != NULL)
    {
        TTY_EXPARAM.popt->tr_enable(dev->param, TRUE); /* ʹ��Ӳ�� */
    }
    if (TTY_EXPARAM.popt->rx_enable != NULL)
    {
        TTY_EXPARAM.popt->rx_enable(dev->param, TRUE); /* ʹ�ܽ����ж� */
    }
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty�豸close����
 * @param[in]  dev  : �豸�ڵ�
 *
 * @retval     OK   : �رճɹ�
 ******************************************************************************
 */
static int32_t
ttylib_close(struct device* dev)
{
    if (TTY_EXPARAM.popt->tr_enable != NULL)
    {
        TTY_EXPARAM.popt->tr_enable(dev->param, FALSE); /* �����Ӳ�� */
    }
    if (TTY_EXPARAM.popt->rx_enable != NULL)
    {
        TTY_EXPARAM.popt->rx_enable(dev->param, FALSE); /* ��������ж� */
    }
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty�豸��ȡ����
 * @param[in]  dev      : �豸�ڵ�
 * @param[in]  pos      : ��ȡƫ��
 * @param[out] buffer   : ��ȡ����
 * @param[in]  size     : ��Ҫ��ȡ���ֽ���
 *
 * @retval     ʵ�ʶ�ȡ�ֽ���
 ******************************************************************************
 */
static size_t
ttylib_read(struct device* dev, int32_t pos, void *buffer, size_t size)
{
    (void)pos;
    return ring_read(&TTY_EXPARAM.ring.rd, buffer, (uint16_t)size);
}

/**
 ******************************************************************************
 * @brief   tty�豸д�뷽��
 * @param[in]  dev      : �豸�ڵ�
 * @param[in]  pos      : д��ƫ��
 * @param[out] buffer   : д�뻺��
 * @param[in]  size     : ��Ҫд����ֽ���
 *
 * @retval     ʵ��д���ֽ���
 ******************************************************************************
 */
static size_t
ttylib_write(struct device* dev, int32_t pos, const void *buffer, size_t size)
{
    uint16_t nbytes = (uint16_t)size;
    while (nbytes != 0)
    {
        nbytes -= ring_write(&TTY_EXPARAM.ring.wt, buffer, size);
        if (TTY_EXPARAM.popt->tx_enable != NULL)
        {
            TTY_EXPARAM.popt->tx_enable(dev->param, TRUE); /* �������� */
        }
        if (nbytes != 0)
        {
            taskDelay(1);
        }
    }
    return size;
}

/**
 ******************************************************************************
 * @brief   tty�豸���Ʒ���
 * @param[in]  dev      : �豸�ڵ�
 * @param[in]  cmd      : ��������
 * @param[out] args     : �������
 *
 * @retval  0
 ******************************************************************************
 */
static int32_t
ttylib_ioctl(struct device* dev, uint32_t cmd, void *args)
{
    switch (cmd)
    {
        case TTY_FIOFLUSH:  /* ��յ�ǰ���ջ����� */
            ring_flush(&TTY_EXPARAM.ring.rd);
            break;
        case TTY_FIONREAD:  /* ��ȡ��ǰ���ջ���������ַ����� */
            return ring_check(&TTY_EXPARAM.ring.rd);
            break;
        case TTY_BAUD_SET:  /* ����ͨѶ���� */
            if (TTY_EXPARAM.popt->set_param != NULL)
            {
                return TTY_EXPARAM.popt->set_param(dev->param, args);
            }
            break;
    }
    return 0;
}

/**
 ******************************************************************************
 * @brief   ���жϴ�tty��ring�����ж�ȡ����
 * @param[in]  pexparam : tty��չ����
 * @param[out] pch      : ��ȡ������
 *
 * @retval     1    : ����
 * @retval     ����   : ������
 ******************************************************************************
 */
uint16_t
ttylib_getchar(tty_exparam_t *pexparam, uint8_t *pch)
{
    return ring_read(&pexparam->ring.wt, pch, 1u);
}

/**
 ******************************************************************************
 * @brief   ���ж�д��tty��ring�����ж�ȡ����
 * @param[in]  pexparam : tty��չ����
 * @param[out] pch      : д�������
 *
 * @retval     None
 ******************************************************************************
 */
void
ttylib_putchar(tty_exparam_t *pexparam, uint8_t ch)
{
    (void)ring_write_force(&pexparam->ring.rd, &ch, 1u);
}

/** tty�豸�������� */
const static fileopt_t the_ttylib_opt =
{
    .init = ttylib_init,
    .release = ttylib_release,
    .open = ttylib_open,
    .close = ttylib_close,
    .read = ttylib_read,
    .write = ttylib_write,
    .ioctl = ttylib_ioctl,
};


/**
 ******************************************************************************
 * @brief   ����tty�豸
 * @param[in]  ttyno    : tty�豸��(С���)
 * @param[in]  pexparam : tty��չ����
 * @param[in]  rdsz     : tty��ȡringbuf��С
 * @param[in]  wtsz     : ttyд��ringbuf��С
 *
 * @retval     OK       : �����ɹ�
 * @retval     ERROR    : ����ʧ��
 ******************************************************************************
 */
status_t
tty_create(uint8_t ttyno, tty_exparam_t *pexparam, uint16_t rdsz, uint16_t wtsz)
{
    uint8_t *pbuf = malloc(rdsz + wtsz);
    if (pbuf == NULL)
    {
        return ERROR;
    }
    ring_init(&pexparam->ring.rd, pbuf, rdsz);
    ring_init(&pexparam->ring.wt, pbuf + rdsz, wtsz);

    char_t name[5];
    (void)sprintf(name, "tty%d", ttyno);
    if (OK != dev_create((const char_t *)name, &the_ttylib_opt, MKDEV(TTY_MAJOR, ttyno), pexparam)) //todo : opt
    {
        printf("tty%d create err!\n", ttyno);
        return ERROR;
    }
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty�豸��Ϣ
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
tty_show_info(void)
{
    uint8_t ttyno = 0u;
    device_t *pdev = NULL;

    printf("  TTY DEVICE INFOMATION\n");
    while ((pdev = devlib_get_info_by_serial(MKDEV(TTY_MAJOR, ttyno++))) != NULL)
    {
        printf("tty:%s  rdsz:%d  wtsz:%d, read valid:%d write valid:%d\n",
                pdev->name,
                ((tty_exparam_t*)pdev->param)->ring.rd.max_len,
                ((tty_exparam_t*)pdev->param)->ring.wt.max_len,
                ((tty_exparam_t*)pdev->param)->ring.rd.len,
                ((tty_exparam_t*)pdev->param)->ring.wt.len
                );
    }
    printf("\n");
}

/*-------------------------------ttyLib.c------------------------------------*/
