/**
 ******************************************************************************
 * @file      mx207uart.c
 * @brief     C Source file of mx207uart.c.
 * @details   This file including all API functions's 
 *            implement of mx207uart.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stdio.h>
#include <intLib.h>
#include <ttyLib.h>
#include <stm32f207.h>

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
#define MAX_UART    6
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
    { USART3_BASE, 2, USART3_IRQn},
    { UART4_BASE,  3, UART4_IRQn},
    { UART5_BASE,  4, UART5_IRQn},
    { USART6_BASE, 5, USART6_IRQn}
};

static const tty_opt uartopt =
{
    .tx_enable = uart_txenable,
    .rx_enable = uart_rxenable,
    .tr_enable = uart_trenable,
    .set_param = uart_set_param,
};

static tty_exparam_t the_tty_exparam[MAX_UART];

GPIO_TypeDef * COM_TX_PORT[MAX_UART] = {GPIOA,GPIOD,GPIOD,GPIOC,GPIOC,GPIOC};
GPIO_TypeDef * COM_RX_PORT[MAX_UART] = {GPIOA,GPIOD,GPIOD,GPIOC,GPIOD,GPIOC};

const uint8_t COM_TX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource9,GPIO_PinSource5,GPIO_PinSource8,
                                             GPIO_PinSource10,GPIO_PinSource12,GPIO_PinSource6};

const uint8_t COM_RX_PIN_SOURCE[MAX_UART] = {GPIO_PinSource10,GPIO_PinSource6,GPIO_PinSource9,
                                             GPIO_PinSource11,GPIO_PinSource2,GPIO_PinSource7};

const uint8_t COM_TX_AF[MAX_UART] = {GPIO_AF_USART1,GPIO_AF_USART2,GPIO_AF_USART3,
                                     GPIO_AF_UART4,GPIO_AF_UART5,GPIO_AF_USART6};

const uint8_t COM_RX_AF[MAX_UART] =  {GPIO_AF_USART1,GPIO_AF_USART2,GPIO_AF_USART3,
                                     GPIO_AF_UART4,GPIO_AF_UART5,GPIO_AF_USART6};

const uint16_t COM_TX_PIN[MAX_UART] = {GPIO_Pin_9,GPIO_Pin_5,GPIO_Pin_8,GPIO_Pin_10,GPIO_Pin_12,GPIO_Pin_6};

const uint16_t COM_RX_PIN[MAX_UART] = {GPIO_Pin_10,GPIO_Pin_6,GPIO_Pin_9,GPIO_Pin_11,GPIO_Pin_2,GPIO_Pin_7};


/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
void
uart_init0(void)
{
    static bool_e is_inited = FALSE;
    if (is_inited == TRUE)
        return;
    is_inited = TRUE;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

    GPIO_InitTypeDef GPIO_InitStructure;
    //
    //UART口IO口线设置
    //
    //
    for (uint8_t i = 0u; i < MAX_UART; i++)
    {
        /* Connect PXx to USARTx_Tx*/
        GPIO_PinAFConfig(COM_TX_PORT[i], COM_TX_PIN_SOURCE[i], COM_TX_AF[i]);

        /* Connect PXx to USARTx_Rx*/
        GPIO_PinAFConfig(COM_RX_PORT[i], COM_RX_PIN_SOURCE[i], COM_RX_AF[i]);


        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

        GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[i];
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(COM_TX_PORT[i], &GPIO_InitStructure);

        /* Configure USART Rx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
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

/*------------------------------mx207uart.c----------------------------------*/
