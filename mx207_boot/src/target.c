/**
 ******************************************************************************
 * @file       target.c
 * @version    V0.0.1
 * @brief      本文提供boot有关的硬件操作方法
 * @details
 * @copyright
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <stm32f207.h>
#include <cfg.h>
/*----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#ifndef BAUDRATE
#define BAUDRATE              115200u
#endif

#define UART_BASE             ((USART_TypeDef *) USART1_BASE)

/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
static void uart_init(void);
static void led_init(void);

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   硬件初始化
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 *
 * @note
 *  需要完成以下事项:
 *      1、设置系统时钟
 *      2、初始化uart、spi、喂狗
 ******************************************************************************
 */
void
hw_init(void)
{
    SystemInit();

    /* 使能外设 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    led_init();

    /* 配置UART */
    uart_init();
}

/**
 ******************************************************************************
 * @brief   开启系统定时器
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 *
 * @note
 *  注意：定时器的周期为1ms中断1次
 ******************************************************************************
 */
void
systick_open(void)
{
	RCC_ClocksTypeDef clocks_status;

	RCC_GetClocksFreq(&clocks_status);

    SysTick->LOAD = ((clocks_status.HCLK_Frequency / 1000)
            & SysTick_LOAD_RELOAD_Msk) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
            | SysTick_CTRL_ENABLE_Msk;
    return;
}

/**
 ******************************************************************************
 * @brief   关闭系统定时器
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
systick_stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 ******************************************************************************
 * @brief   uart初始化
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
uart_init(void)
{
    GPIO_InitTypeDef gpio_inits;
    USART_InitTypeDef usart_inits;

    /* UART IO口线设置 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    gpio_inits.GPIO_OType = GPIO_OType_PP;
    gpio_inits.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_inits.GPIO_Mode = GPIO_Mode_AF;
    gpio_inits.GPIO_Speed = GPIO_Speed_50MHz;

    /* Configure USART Tx as alternate function  */
    gpio_inits.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &gpio_inits);

    /* Configure USART Rx as alternate function  */
    gpio_inits.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &gpio_inits);

    usart_inits.USART_BaudRate = BAUDRATE;
    usart_inits.USART_WordLength = USART_WordLength_8b;
    usart_inits.USART_StopBits = USART_StopBits_1;
    usart_inits.USART_Parity = USART_Parity_No;
    usart_inits.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_inits.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(UART_BASE, &usart_inits);
    UART_BASE->CR1 |= USART_CR1_UE;
}

/**
 ******************************************************************************
 * @brief   LED初始化为全亮
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void led_init(void)
{
	GPIO_InitTypeDef led_inits;

	led_inits.GPIO_OType = GPIO_OType_PP;
	led_inits.GPIO_PuPd = GPIO_PuPd_DOWN;
	led_inits.GPIO_Mode = GPIO_Mode_OUT;
	led_inits.GPIO_Speed = GPIO_Speed_50MHz;
	led_inits.GPIO_Pin = GPIO_Pin_2;

    GPIO_Init(GPIOB, &led_inits);
}

/**
 ******************************************************************************
 * @brief   往串口发送一个字节
 * @param[in]  c    : 待发送字节
 * @param[out] None
 * @retval     None
 ******************************************************************************
 */
void
uart_send(const uint8_t c)
{
    /* 等待完成发送 */
    while (USART_GetFlagStatus(UART_BASE, USART_FLAG_TXE) == RESET)
    {
    }
    UART_BASE->DR = (uint16_t)c & 0x01FFu;
}

/**
 ******************************************************************************
 * @brief   串口尝试接收一个字符
 * @param[out] *pc  : 接收到的数据
 * @retval  TRUE    : 有数据
 * @retval  FALSE   : 无数据
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
bool_e
uart_try_receive(uint8_t *pc)
{
    if (USART_GetFlagStatus(UART_BASE, USART_FLAG_RXNE) != RESET)
    {
        *pc = (uint16_t) (UART_BASE->DR & (uint16_t) 0x01FF);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 ******************************************************************************
 * @brief   串口接收指定数量的数据
 * @param[out] *pdata   : 接收缓冲
 * @param[in]  size     : 待接收长度
 *
 * @retval     None
 ******************************************************************************
 */
void
uart_receive(uint8_t *pdata,
        uint32_t size)
{
    do
    {
        /* 如果没有数据，则等待 */
        while (!uart_try_receive(pdata))
        {
        }
        pdata++;
        size--;
    } while (size > 0);
}

/**
 ******************************************************************************
 * @brief   打印一个字符串
 * @param[in]  *str: 待打印数据，注意:必须以'\0'结尾
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
print(const char_t* str)
{
    while ('\0' != *str)
    {
        uart_send(*str);
        str++;
    }
}

/**
 ******************************************************************************
 * @brief   复位
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
reset(void)
{
#if 0
    /* 0x05FA: VECTKEY访问钥匙，需同时写入
     0x0004: 请求芯片产生一次复位
     */
    *((uint32_t *) 0xE000ED0Cu) = 0x05FA0004u;
#endif
    NVIC_SystemReset();
}
/**
 ******************************************************************************
 * @brief   片内flash初始化
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
iflash_init(void)
{
    /* 解锁 */
    FLASH_Unlock();
    /* 清除状态 */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/**
 ******************************************************************************
 * @brief   擦除片内Flash指定区域的内容
 * @param[in]  address  : 擦除地址
 * @param[in]  size     : 擦除长度
 *
 * @retval  TRUE    : 成功
 * @retval  FALSE   : 失败
 ******************************************************************************
 */
bool_e
iflash_erase(uint32_t address,
        uint32_t size)
{
    uint32_t i;

    for (i = FLASH_Sector_2; i <= FLASH_Sector_7; i += 8u)
    {
        if (!FLASH_EraseSector(i, VoltageRange_3))  //TODO !的用法需确认
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 ******************************************************************************
 * @brief   读取片内Flash指定区域的内容
 * @param[out] *pdata   : 读取缓冲
 * @param[in]  address  : 目标地址
 * @param[in]  size     : 读取长度
 *
 * @retval  TRUE    : 成功
 * @retval  FALSE   : 失败
 ******************************************************************************
 */
bool_e
iflash_read(uint8_t *pdata,
        uint32_t address,
        uint32_t size)
{
    for (uint32_t i = 0u; i < size; i++)
    {
        *pdata = *((uint8_t *)address);
        pdata++;
        address++;
    }
    return TRUE;
}

/**
 ******************************************************************************
 * @brief   修改片内Flash指定区域的内容
 * @param[in]  *pdata   : 写入内容
 * @param[in]  address  : 目标地址
 * @param[in]  size     : 写入长度
 *
 * @retval  TRUE    : 成功
 * @retval  FALSE   : 失败
 ******************************************************************************
 */
bool_e
iflash_write(const uint8_t *pdata,
        uint32_t address,
        uint32_t size)
{
    for (uint32_t i = 0u; i < size; i += 4u)
    {
        /* 写入4字节 */
        if (!FLASH_ProgramWord(address + i, *(const uint32_t *)(pdata + i)))
        {
            return (FALSE);
        }
        /* 校验写入是否成功 */
        if (*(uint32_t *)(address + i) != *(const uint32_t *)(pdata + i))
        {
            return (FALSE);
        }
    }

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief   等待
 * @param[in]  usec     : 等待时间
 *
 * @retval     None
 ******************************************************************************
 */
void
delay(const uint32_t usec)
{
    volatile uint32_t count = 0;
    const uint32_t utime = 120 * usec;
    do
    {
        if (++count > utime)
        {
            return;
        }
    } while (TRUE);
}

/*--------------------------------End of target.c----------------------------*/
