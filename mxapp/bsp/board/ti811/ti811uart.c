/**
 ******************************************************************************
 * @file      ti811uart.c
 * @brief     C Source file of ti811uart.c.
 * @details   This file including all API functions's 
 *            implement of ti811uart.c.	
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
#include <lm3s811.h>

#if (BOARD_BUILD_VER == BOARD_TI811)
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
    { UART0_BASE, 0, INT_UART0},
    { UART1_BASE, 1, INT_UART1},
};

static const tty_opt uartopt =
{
    .tx_enable = uart_txenable,
    .rx_enable = uart_rxenable,
    .tr_enable = uart_trenable,
    .set_param = uart_set_param,
};

static tty_exparam_t the_tty_exparam[MAX_UART];

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

    /* 设置uart0、1模式 */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
//    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
//    UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

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
    uint8_t outchar;
    uint32_t status_t;

    UARTRxErrorClear(pexparam->baseregs);
    status_t = UARTIntStatus(pexparam->baseregs, true);
    if((status_t & UART_INT_RX) || (status_t & UART_INT_RT))
    {
        ttylib_putchar(pexparam, (uint8_t)HWREG(pexparam->baseregs + UART_O_DR));
        UARTIntClear(pexparam->baseregs, UART_INT_RT | UART_INT_RX);
    }

    if(status_t & (UART_INT_TX))
    {
        while((HWREG(pexparam->baseregs + UART_O_FR) & UART_FR_TXFF))
        {   //等待uart缓存空闲
        }

        while(!(HWREG(pexparam->baseregs + UART_O_FR) & UART_FR_TXFF))
        {
            if (ttylib_getchar(pexparam, &outchar) != 0u)
            {
                HWREG(pexparam->baseregs + UART_O_DR) = outchar;
            }
            else
            {
                HWREG(pexparam->baseregs   + UART_O_IM) &= ~UART_INT_TX;
                break;
            }
        }
        UARTIntClear(pexparam->baseregs, UART_INT_TX);
    }
}

static void
uart_txenable(tty_exparam_t *pexparam, bool_e s)
{
    if (s == TRUE)
    {
        uint8_t outchar;
        while((HWREG(pexparam->baseregs + UART_O_FR) & UART_FR_TXFF))
        {}
        while(!(HWREG(pexparam->baseregs + UART_O_FR) & UART_FR_TXFF))
        {
            if (ttylib_getchar(pexparam, &outchar) != 0u)
            {
                HWREG(pexparam->baseregs + UART_O_DR) = outchar;
            }
            else
            {
                break;
            }
        }
        UARTIntEnable(pexparam->baseregs, UART_INT_TX);
    }
    else
    {
        UARTIntDisable(pexparam->baseregs, UART_INT_TX);
    }
}

static void
uart_rxenable(tty_exparam_t *pexparam, bool_e s)
{
    if (s == TRUE)
    {
        UARTIntEnable(pexparam->baseregs, UART_INT_RX | UART_INT_RT);
    }
    else
    {
        UARTIntDisable(pexparam->baseregs, UART_INT_RX | UART_INT_RT);
    }
}

static void
uart_trenable(tty_exparam_t *pexparam, bool_e s)
{
    if (s == TRUE)
    {
        UARTEnable(pexparam->baseregs);
    }
    else
    {
        UARTDisable(pexparam->baseregs);
    }
}

static int32_t
uart_set_param(tty_exparam_t *pexparam, tty_param_t* p)
{
    uint32_t ulConfig = 0u;

    //todo: uart参数
    ulConfig = UART_CONFIG_WLEN_8 |
            UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE;
    UARTConfigSet(pexparam->baseregs, p->baudrate, ulConfig);
    return 0;
}
#endif
/*-------------------------------ti811uart.c---------------------------------*/
