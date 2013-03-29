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
#include <lm3s811.h>

#if (BOARD_BUILD_VER == BOARD_TI811)

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

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
#if 1
#if PLL_EN == 0
    /*  Not use PLL  ��ʹ��PLL      */
    SysCtlClockSet(CCLK_DIV
            | SYSCTL_USE_OSC
            | SYSCTL_OSC_MAIN
            | EXT_CLK);
 #else
    /*  Use PLL  ʹ��PLL            */
    SysCtlClockSet(CCLK_DIV
            | SYSCTL_USE_PLL
            | SYSCTL_OSC_MAIN
            | EXT_CLK);
#endif
#endif
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    UARTConfigSet(UART0_BASE, 115200,
            UART_CONFIG_WLEN_8 |
            UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
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

    c = (int32_t)UARTCharGetNonBlocking(UART0_BASE);
    if (c < 0)
        c = 0;
    return c;
}

void bsp_putchar(char_t ch)
{
    UARTCharPut(UART0_BASE, ch);
}

void bsp_reboot(void)
{
    SysCtlReset();
}

static uint32_t the_usticks = 0u;
static void
timer0_isr(void)
{
    the_usticks++;
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
void
bsp_timer_start(void)
{
    //��ʹ������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //��������ģʽ
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
    //����װ��ֵ
    TimerLoadSet(TIMER0_BASE, TIMER_A, 1000);
    //ʹ���ж�Դ
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //GPTMģ��ʹ��
    TimerEnable(TIMER0_BASE, TIMER_A);
    intConnect(INT_TIMER0A, timer0_isr, 0u);
    intEnable(INT_TIMER0A);
}

uint32_t
bsp_timer_get(void)
{
    return the_usticks;
}

/* 5. ��ȡMCU��Ƶ */
uint32_t
bsp_get_mcu_clk(void)
{
    return SysCtlClockGet();
}

/* 6. ��ȡMCU�ж����� */
uint32_t
bsp_get_max_int_count(void)
{
    return MAX_INT_COUNT;
}

#endif
/*--------------------------------bsplib.c-----------------------------------*/
