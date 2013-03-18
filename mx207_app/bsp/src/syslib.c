/**
 ******************************************************************************
 * @file      syslib.c
 * @brief     C Source file of syslib.c.
 * @details   This file including all API functions's implement of syslib.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <intLib.h>
#include <stm32f207.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#define UART_BASE             ((USART_TypeDef *) USART1_BASE)

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
void
sysHwInit0(void)
{
    SystemInit();
    //
    //ʹ������
    //
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
    RCC->APB2ENR |= RCC_APB2Periph_SYSCFG;
    RCC->APB1ENR |= RCC_APB1Periph_TIM5;

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

    RCC->APB1ENR |= RCC_APB1Periph_SPI2;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB1ENR |= RCC_APB1Periph_TIM3;
    RCC->APB1ENR |= RCC_APB1Periph_TIM2;
    RCC->APB2ENR |= RCC_APB2Periph_TIM1;
    RCC->APB1ENR |= RCC_APB1Periph_TIM4;
    RCC->APB2ENR |= RCC_APB2Periph_TIM8;
    RCC->APB1ENR |= RCC_APB1Periph_TIM7;
    RCC->APB1ENR |= RCC_APB1Periph_TIM6;
    RCC->APB2ENR |= RCC_APB2Periph_TIM9;
}


void
sysHwInit(void)
{
    intLock();

    intUnlock();


}



int32_t bsp_getchar(void)
{
    int32_t c;

    if (USART_GetFlagStatus(UART_BASE, USART_FLAG_RXNE) == RESET)
        return 0;
    c = (int32_t)USART_ReceiveData(UART_BASE);
    if (c < 0)
        c = 0;
    return c;
}

void bsp_putchar(char_t ch)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(UART_BASE, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(UART_BASE, USART_FLAG_TC) == RESET)
    {}
}

void bsp_reboot(void)
{
    NVIC_SystemReset();
}


void
timer_start(void)
{
// ���ݲ�����������ʱʱ��
    uint32_t baud_timertick = 0xffffffff;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC->APB1ENR |= RCC_APB1Periph_TIM2;
    //���½�Timer����Ϊȱʡֵ
    TIM_DeInit(TIM2);
    //�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
    //TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseStructure.TIM_Prescaler = 59;
    //����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //���ü�����ģʽΪ���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //���ü��������С��ÿ��x�����Ͳ���һ�������¼�
    TIM_TimeBaseStructure.TIM_Period = baud_timertick;
    //������Ӧ�õ�TIM2��
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //�������жϱ�־
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    //��ֹARRԤװ�ػ�����
    //TIM_ARRPreloadConfig(TIM2, DISABLE);
    //����TIM2���ж�
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    /* TIM2 counter enable */
    TIM_Cmd(TIM2, ENABLE);

    TIM_SetCounter(TIM2, 0);

}

uint32_t
timer_get(void)
{
    uint32_t starttime_ms = 0;
    starttime_ms = TIM_GetCounter(TIM2);
    return starttime_ms;
}

/*----------------------------syslib.c--------------------------------*/
