/**
 ******************************************************************************
 * @file      bsplib.c
 * @brief     C Source file of bsplib.c.
 * @details   This file including all API functions's 
 *            implement of bsplib.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include "../../bspcfg.h"
#include <types.h>
#include <intLib.h>
#include <stm32f1xx.h>

#if (BOARD_BUILD_VER == BOARD_DIY103)

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#define UART_BASE             USART1

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
mcuClkSetup(void)
{
    SystemInit();
}

void
bspHwInit(void)
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
bsp_timer_start(void)
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
//    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    /* TIM2 counter enable */
    TIM_Cmd(TIM2, ENABLE);

    TIM_SetCounter(TIM2, 0);
}

uint32_t
bsp_timer_get(void)
{
    //return (uint32_t)TIM_GetCounter(TIM2);
    static uint32_t i = 0;
    return i++;
}

/* 5. ��ȡMCU��Ƶ */
uint32_t
bsp_get_mcu_clk(void)
{
    return CPU_CLOCK_HZ;
}

/* 6. ��ȡMCU�ж����� */
uint32_t
bsp_get_max_int_count(void)
{
    return MAX_INT_COUNT;
}

#endif
/*--------------------------------bsplib.c-----------------------------------*/
