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

}

uint32_t
bsp_timer_get(void)
{
    return 0;
}

/* 5. 获取MCU主频 */
uint32_t
bsp_get_mcu_clk(void)
{
    return CPU_CLOCK_HZ;
}

/* 6. 获取MCU中断数量 */
uint32_t
bsp_get_max_int_count(void)
{
    return MAX_INT_COUNT;
}

#endif
/*--------------------------------bsplib.c-----------------------------------*/
