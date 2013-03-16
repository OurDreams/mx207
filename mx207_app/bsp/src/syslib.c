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
/*----------------------------syslib.c--------------------------------*/
