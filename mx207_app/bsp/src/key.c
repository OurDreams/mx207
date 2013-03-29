/**
 ******************************************************************************
 * @file      key.c
 * @brief     C Source file of key.c.
 * @details   �����ж϶�ȡ������д��ring_buf.
 *
 * @copyright
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stm32f207.h>
#include <types.h>
#include <ring.h>
#include <stddef.h>
#include <stdio.h>
#include <devLib.h>
#include <intLib.h>
#include <clkLib.h>
#include <time.h>

#ifdef Dprintf
#undef Dprintf
#endif
#define Dprintf(x...)
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#define KEY_BUF_LEN 8u
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
static struct ring_buf key_rbuf;
static uint8_t key_buf[KEY_BUF_LEN] = {0};
static uint8_t key_val = 0;

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
 * @brief      �����жϷ������
 * @param[in]  None
 * @param[in]  None
 * @retval     None
 ******************************************************************************
 */
static void key_isr(void)
{
	uint8_t temp;

    if ( EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
    	temp = 1;
        ring_write(&key_rbuf ,&temp, 1);
        Dprintf("Key%d is pressed\n", temp);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
    if ( EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
    	temp = 2;
        ring_write(&key_rbuf ,&temp, 1);
        Dprintf("Key%d is pressed\n", temp);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
    if ( EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
    	temp = 3;
    	ring_write(&key_rbuf ,&temp, 1);
        Dprintf("Key%d is pressed\n", temp);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
    if ( EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
    	temp = 4;
    	ring_write(&key_rbuf ,&temp, 1);
        Dprintf("Key%d is pressed\n", temp);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}

/**
 ******************************************************************************
 * @brief      �豸��ʼ��
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
static status_t dev_key_init(struct device* dev)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Connect EXTI Line2~5 to PE2~5 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
	/* Configure EXTI Line2~5 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2|EXTI_Line3|EXTI_Line4|EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	intConnect(EXTI2_IRQn, key_isr, 0);
	intConnect(EXTI3_IRQn, key_isr, 0);
	intConnect(EXTI4_IRQn, key_isr, 0);
	intConnect(EXTI9_5_IRQn, key_isr, 0);
	intEnable(EXTI2_IRQn);
	intEnable(EXTI3_IRQn);
	intEnable(EXTI4_IRQn);
	intEnable(EXTI9_5_IRQn);

	ring_init(&key_rbuf, key_buf, KEY_BUF_LEN);

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
static status_t  dev_key_open(struct device* dev)
{
	Dprintf("key open\n");
	return OK;
}
/**
 ******************************************************************************
 * @brief      ������
 * @param[in]  device* dev : �豸����
 * @param[in]  *buffer     : д�����ַ
 * @param[in]  pinno       : ��ȡ������

 * @retval     key_val
 * @retval     ERROR       : ʧ��
 ******************************************************************************
 */
static size_t dev_key_read (struct device* dev, int32_t pos, void *buffer, size_t pinno)
{
	uint8_t rlen;
	rlen = ring_read(&key_rbuf, &key_val, 1);
	if(key_val == 0 || rlen == 0)
	{
		return ERROR;
	}
	return key_val;
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
static status_t dev_key_release(struct device* dev)
{
	printf("key release\n");
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
static status_t  dev_key_close(struct device* dev)
{
	printf("key close\n");
	return OK;
}
const static fileopt_t key_opt =
{
		.init = dev_key_init,
		.release = dev_key_release,
		.open = dev_key_open,
		.read = dev_key_read,
		.close = dev_key_close,
};
status_t key_init(void)
{
	if (OK != dev_create("key", &key_opt, MKDEV(1, 1), NULL))
    {
	    printf("dev_create err\n");
	    return ERROR;
    }
	return OK;
}
/*----------------------------key.c--------------------------------*/
