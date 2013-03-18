/**
 ******************************************************************************
 * @file      ttyLib.c
 * @brief     C Source file of ttyLib.c.
 * @details   通用tty设备驱动.
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
 * @brief   tty设备初始化方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t
ttylib_init(struct device* dev)
{
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty设备释放方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
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
 * @brief   tty设备open方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static int32_t
ttylib_open(struct device* dev)
{
    if (TTY_EXPARAM.popt->tr_enable != NULL)
    {
        TTY_EXPARAM.popt->tr_enable(dev->param, TRUE); /* 使能硬件 */
    }
    if (TTY_EXPARAM.popt->rx_enable != NULL)
    {
        TTY_EXPARAM.popt->rx_enable(dev->param, TRUE); /* 使能接收中断 */
    }
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty设备close方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static int32_t
ttylib_close(struct device* dev)
{
    if (TTY_EXPARAM.popt->tr_enable != NULL)
    {
        TTY_EXPARAM.popt->tr_enable(dev->param, FALSE); /* 清除能硬件 */
    }
    if (TTY_EXPARAM.popt->rx_enable != NULL)
    {
        TTY_EXPARAM.popt->rx_enable(dev->param, FALSE); /* 清除接收中断 */
    }
    return OK;
}

/**
 ******************************************************************************
 * @brief   tty设备读取方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     size
 ******************************************************************************
 */
static size_t
ttylib_read(struct device* dev, int32_t pos, void *buffer, size_t size)
{
    return ring_read(&TTY_EXPARAM.ring.rd, buffer, (uint16_t)size);
}

/**
 ******************************************************************************
 * @brief   tty设备写入方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval     size
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
            TTY_EXPARAM.popt->tx_enable(dev->param, TRUE); /* 启动发送 */
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
 * @brief   tty设备控制方法
 * @param[in]  None
 * @param[out] None
 *
 * @retval  0
 ******************************************************************************
 */
static int32_t
ttylib_ioctl(struct device* dev, uint32_t cmd, void *args)
{
    switch (cmd)
    {
        case TTY_FIOFLUSH:  /* 清空当前接收缓冲区 */
            ring_flush(&TTY_EXPARAM.ring.rd);
            break;
        case TTY_FIONREAD:/* 获取当前接收缓冲区里的字符个数 */
            return ring_check(&TTY_EXPARAM.ring.rd);
            break;
    }
    //ioctl
    return 0;
}

/**
 ******************************************************************************
 * @brief   在中断从tty的ring部分中读取数据
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
__INLINE uint16_t
ttylib_getchar(tty_exparam_t *pexparam, uint8_t *pch)
{
    return ring_read(&pexparam->ring.wt, pch, 1u);
}

/**
 ******************************************************************************
 * @brief   在中断写入tty的ring部分中读取数据
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
__INLINE void
ttylib_putchar(tty_exparam_t *pexparam, uint8_t ch)
{
    (void)ring_write_force(&pexparam->ring.rd, &ch, 1u);
}

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
 * @brief   创建tty设备
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
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
    if (OK != dev_create((const char_t *)name, &the_ttylib_opt, MKDEV(6, ttyno), pexparam)) //todo : opt
    {
        printf("tty%d create err!\n", ttyno);
        return ERROR;
    }
    return OK;
}
/*-------------------------------ttyLib.c------------------------------------*/
