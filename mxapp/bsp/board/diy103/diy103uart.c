/**
 ******************************************************************************
 * @file      diy103uart.c
 * @brief     C Source file of diy103uart.c.
 * @details   This file including all API functions's 
 *            implement of mx207uart.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include "../../bspcfg.h"
#include <types.h>
#include <stdio.h>
#include <intLib.h>
#include <ttyLib.h>
#include <stm32f1xx.h>

#if (BOARD_BUILD_VER == BOARD_DIY103)
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
typedef struct
{
    uint32_t  base;
    uint16_t  ttyno;
    uint32_t  intid;
} uart_param_t;

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define MAX_UART    2
#define DEFAULT_BAUDRATE    115200
#define RING_BUF_SIZE       128
/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
static void
uart_int(tty_exparam_t *pexparam);

static void
uart_txenable(tty_exparam_t *pexparam, bool_e s);

static void
uart_rxenable(tty_exparam_t *pexparam, bool_e s);

static void
uart_trenable(tty_exparam_t *pexparam, bool_e s);

static int32_t
uart_set_param(tty_exparam_t *pexparam, tty_param_t* p);

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
// UART口的参数表
static const uart_param_t uartParas[] =
{
    { USART1_BASE, 0, USART1_IRQn},
    { USART2_BASE, 1, USART2_IRQn},
};

static const tty_opt uartopt =
{
    .tx_enable = uart_txenable,
    .rx_enable = uart_rxenable,
    .tr_enable = uart_trenable,
    .set_param = uart_set_param,
};

static tty_exparam_t the_tty_exparam[MAX_UART];

GPIO_TypeDef* COM_TX_PORT[MAX_UART] = {GPIOA, GPIOA};
GPIO_TypeDef* COM_RX_PORT[MAX_UART] = {GPIOA, GPIOA};

const uint8_t COM_TX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource9, GPIO_PinSource2};

const uint8_t COM_RX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource10, GPIO_PinSource3};

const uint32_t COM_USART_CLK[MAX_UART] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART2};

const uint32_t COM_TX_PORT_CLK[MAX_UART] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};

const uint32_t COM_RX_PORT_CLK[MAX_UART] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};


const uint16_t COM_TX_PIN[MAX_UART] = {GPIO_Pin_9, GPIO_Pin_2};

const uint16_t COM_RX_PIN[MAX_UART] = {GPIO_Pin_10, GPIO_Pin_3};
/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
void
tty_init(void)
{
    static bool_e is_inited = FALSE;
    if (is_inited == TRUE)
        return;
    is_inited = TRUE;
    RCC->APB2ENR |= RCC_APB2Periph_USART1;
    RCC->APB1ENR |= RCC_APB1Periph_USART2;
    RCC->APB1ENR |= RCC_APB1Periph_USART3;

    GPIO_InitTypeDef GPIO_InitStructure;
    //
    //UART口IO口线设置
    //
    //
    for (uint8_t i = 0u; i < MAX_UART; i++)
    {
        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

        GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[i];
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(COM_TX_PORT[i], &GPIO_InitStructure);

        /* Configure USART Rx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[i];
        GPIO_Init(COM_RX_PORT[i], &GPIO_InitStructure);
    }
    for (uint8_t i = 0u; i < MAX_UART; i++)
    {
        USART_InitTypeDef usart_inits;

        usart_inits.USART_BaudRate = DEFAULT_BAUDRATE;
        usart_inits.USART_WordLength = USART_WordLength_8b;
        usart_inits.USART_StopBits = USART_StopBits_1;
        usart_inits.USART_Parity = USART_Parity_No;
        usart_inits.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        usart_inits.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        USART_Init((USART_TypeDef *)uartParas[i].base, &usart_inits);
        ((USART_TypeDef *)uartParas[i].base)->CR1 |= USART_CR1_UE;
    }

    for (uint8_t i = 0u; i < ARRAY_SIZE(the_tty_exparam); i++)
    {
        the_tty_exparam[i].baseregs = uartParas[i].base;
        the_tty_exparam[i].popt = &uartopt;
        intConnect(uartParas[i].intid, uart_int, (uint32_t)&the_tty_exparam[i]);
        intEnable(uartParas[i].intid);
    }

    for (uint8_t i = 0u; i < MAX_UART; i++)
    {
        if (OK != tty_create(i, &the_tty_exparam[i], RING_BUF_SIZE, RING_BUF_SIZE))
        {
            printf("tty_create tty%d err\n", i);
        }
    }
}

static void
uart_int(tty_exparam_t *pexparam)
{
    if (USART_GetITStatus((USART_TypeDef*)(pexparam->baseregs), USART_IT_RXNE) != RESET)
    {
        ttylib_putchar(pexparam, (uint8_t)USART_ReceiveData((USART_TypeDef*)(pexparam->baseregs)));
    }

    if (USART_GetITStatus((USART_TypeDef*)(pexparam->baseregs), USART_IT_TC) != RESET)
    {
        uint8_t outchar;
        if (ttylib_getchar(pexparam, &outchar) != 0u)
        {
            USART_SendData((USART_TypeDef*)(pexparam->baseregs), (uint8_t) outchar);
        }
        else
        {
            /* Disable the USARTx transmit data register empty interrupt */
            USART_ITConfig((USART_TypeDef*)(pexparam->baseregs), USART_IT_TC, DISABLE);
        }
    }
}

static void
uart_txenable(tty_exparam_t *pexparam, bool_e s)
{
    FunctionalState NewState = (s == TRUE) ? ENABLE : DISABLE;
    USART_ITConfig((USART_TypeDef*)(pexparam->baseregs), USART_IT_TC, NewState);
}

static void
uart_rxenable(tty_exparam_t *pexparam, bool_e s)
{
    FunctionalState NewState = (s == TRUE) ? ENABLE : DISABLE;
    USART_ITConfig((USART_TypeDef*)(pexparam->baseregs), USART_IT_RXNE, NewState);
}

static void
uart_trenable(tty_exparam_t *pexparam, bool_e s)
{
    FunctionalState NewState = (s == TRUE) ? ENABLE : DISABLE;
    USART_Cmd((USART_TypeDef*) (pexparam->baseregs), NewState);
}

static int32_t
uart_set_param(tty_exparam_t *pexparam, tty_param_t* p)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = p->baudrate;
    USART_InitStructure.USART_WordLength = p->wordlength;
    USART_InitStructure.USART_StopBits = p->stopbits;
    USART_InitStructure.USART_Parity = p->parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


    USART_Init((USART_TypeDef*)(pexparam->baseregs), &USART_InitStructure);
    return 0;
}

#endif

/*-----------------------------diy103uart.c----------------------------------*/
