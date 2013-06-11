/**
 ******************************************************************************
 * @file      gpio.c
 * @brief     C Source file of gpio.c.
 * @details   This file including all API functions's 
 *            implement of gpio.c.	
 *
 * @copyright
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stm32f2xx.h>
#include <gpio.h>
#include <types.h>
#include <devLib.h>
#include <stdio.h>

#ifdef Dprintf
#undef Dprintf
#endif
#define Dprintf(x...)

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/*IO_MAP�Ķ���*/
typedef struct
{
    uint8_t  iono;
    uint8_t  mode;    //����or���
    uint8_t  type;    //����or��©
    uint8_t  pupd;    //����or����or��
    GPIO_TypeDef * gpiobase;//GPIOA~GPIOG
    uint32_t pinno;   //GPIO_Pin_0~GPIO_Pin_15
} io_map_t;
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
/*����IO�Ķ�Ӧ��ϵ��*/
static const io_map_t the_inout_iomap[] =
{
    {IO_LED0,   GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP,     GPIOB, GPIO_Pin_1},
    {IO_LED1,   GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP,     GPIOB, GPIO_Pin_2},
    {IO_LED2,   GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP,     GPIOF, GPIO_Pin_11},
    {IO_LED3,   GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP,     GPIOF, GPIO_Pin_12},
    {IO_KEY0,   GPIO_Mode_IN,  GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIOE, GPIO_Pin_2},
    {IO_KEY1,   GPIO_Mode_IN,  GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIOE, GPIO_Pin_3},
    {IO_KEY2,   GPIO_Mode_IN,  GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIOE, GPIO_Pin_4},
    {IO_KEY3,   GPIO_Mode_IN,  GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIOE, GPIO_Pin_5},
    {IO_LCD_AK, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_DOWN,   GPIOE, GPIO_Pin_11}
};
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
 * @brief      �豸��ʼ��
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t dev_gpio_init(struct device* dev)
{
	uint8_t i;
	GPIO_InitTypeDef gpio_inits;
	for(i = 0; i < ARRAY_SIZE(the_inout_iomap); i++)
	{
		gpio_inits.GPIO_Mode = the_inout_iomap[i].mode;
		gpio_inits.GPIO_OType = the_inout_iomap[i].type;
		gpio_inits.GPIO_PuPd = the_inout_iomap[i].pupd;
		gpio_inits.GPIO_Pin = the_inout_iomap[i].pinno;
		gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(the_inout_iomap[i].gpiobase, &gpio_inits);
	}
	return OK;
}
/**
 ******************************************************************************
 * @brief      �豸�ͷ�
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t dev_gpio_release(struct device* dev)
{
	printf("gpio release\n");
	return OK;
}
/**
 ******************************************************************************
 * @brief      ���豸
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t  dev_gpio_open(struct device* dev)
{
	Dprintf("gpio open\n");
	return OK;
}
/**
 ******************************************************************************
 * @brief      �ر��豸
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t  dev_gpio_close(struct device* dev)
{
	printf("gpio close\n");
	return OK;
}
/**
 ******************************************************************************
 * @brief      ��GPIO
 * @param[in]  device* dev : �豸����
 * @param[in]  *buffer     : д�����ַ
 * @param[in]  pinno       : ��ȡ������

 * @retval     OK          : �ɹ�
 * @retval     FALSE       : ʧ��
 ******************************************************************************
 */
static size_t dev_gpio_read (struct device* dev, int32_t pos, void *buffer, size_t pinno)
{
	int8_t val = -1;
	val = GPIO_ReadInputDataBit(the_inout_iomap[pinno].gpiobase, the_inout_iomap[pinno].pinno);
	if(val < 0)
	{
		return ERROR;
	}
	*(uint8_t *)buffer = val;
	return OK;
}

/**
 ******************************************************************************
 * @brief      GPIO�Ŀ���
 * @param[in]  device* dev : �豸����
 * @param[in]  cmd         ��0���õͣ�1���ø�
 * @param[in]  *arg        ��io����
 * @retval     OK          : �ɹ�
 * @retval     FALSE       : ʧ��
 ******************************************************************************
 */
static int32_t dev_gpio_ioctl (struct device* dev, uint32_t cmd, void* arg)
{
	uint32_t pinno = (uint32_t)arg;
	if(pinno < 0)
	{
		printf("Wrong pinno!\n");
		return ERROR;
	}
	if(cmd == 1)
	{
		GPIO_SetBits(the_inout_iomap[pinno].gpiobase, the_inout_iomap[pinno].pinno);
	}
	else if(cmd == 0)
	{
		GPIO_ResetBits(the_inout_iomap[pinno].gpiobase, the_inout_iomap[pinno].pinno);
	}
	else
	{
		printf("Wrong command!\n");
		return ERROR;
	}
	return OK;
}

const static fileopt_t gpio_opt =
{
		.init = dev_gpio_init,
		.release = dev_gpio_release,
		.open = dev_gpio_open,
		.read = dev_gpio_read,
		.close = dev_gpio_close,
		.ioctl = dev_gpio_ioctl,
};

status_t gpio_init(void)
{
	if (OK != dev_create("gpio", &gpio_opt, MKDEV(1, 0), NULL))
    {
	    printf("dev_create err\n");
	    return ERROR;
    }
	return OK;
}
