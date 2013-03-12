/**
 ******************************************************************************
 * @file       target.c
 * @version    V0.0.1
 * @brief      boot�йص�Ӳ������
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
/*----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
static void uart_init(void);
static void  led_init(void);
/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
#define BAUDRATE              115200u
#define UART_BASE             ((USART_TypeDef *) USART1_BASE)
/**
 ******************************************************************************
 * @brief      Ӳ����ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *  ��Ҫ�����������:
 *      1������ϵͳʱ��
 *      2����ʼ��uart��spi��ι��
 ******************************************************************************
 */
void
hw_init(void)
{
    SystemInit();


    /* ʹ������ */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    led_init();
    /* ����UART */
    uart_init();


}

/**
 ******************************************************************************
 * @brief      ����ϵͳ��ʱ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *  ע�⣺��ʱ��������Ϊ1ms�ж�1��
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
 * @brief      �ر�ϵͳ��ʱ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
void
systick_stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 ******************************************************************************
 * @brief      uart��ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
uart_init(void)
{
    GPIO_InitTypeDef gpio_inits;
    USART_InitTypeDef usart_inits;

    /* UART IO�������� */
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
 * @brief      LED��ʼ��Ϊȫ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
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
 * @brief      �����ڷ���һ���ֽ�
 * @param[in]  const uint8_t c: �������ֽ�
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
void
uart_send(const uint8_t c)
{
    /* �ȴ���ɷ��� */
    while (USART_GetFlagStatus(UART_BASE, USART_FLAG_TXE) == RESET)
    {

    }

    UART_BASE->DR = (uint16_t)c & 0x01FFu;
}
/**
 ******************************************************************************
 * @brief      ���ڳ��Խ���һ���ַ�
 * @param[in]  None
 * @param[out] uint8_t *pc: ���յ�������
 * @retval     bool_e: TRUE-������ FALSE-������
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

        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

/**
 ******************************************************************************
 * @brief      ���ڽ���ָ������������
 * @param[out] uint8_t *pdata: ���ջ���
 * @param[in]  uint32_t size: �����ճ���
 * @retval     None
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
void
uart_receive(uint8_t *pdata,
        uint32_t size)
{
    do
    {
        /* ���û�����ݣ���ȴ� */
        while (!uart_try_receive(pdata))
        {
        }
        pdata++;
        size--;
    }
    while (size > 0);
}


/**
 ******************************************************************************
 * @brief      ���������һ���ַ�
 * @param[in]  const uint8_t c: Ҫ������ַ�
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
put_char(const uint8_t c)
{
    uart_send(c);
}
/**
 ******************************************************************************
 * @brief      ��ӡһ���ַ���
 * @param[in]  const uint8_t *str: ����ӡ���ݣ�ע��:������'\0'��β
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
print(const uint8_t* str)
{
    while (0 != *str)
    {
        put_char(*str);
        str++;
    }
}
/**
 ******************************************************************************
 * @brief      ��λ
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details       reset
 *
 * @note
 ******************************************************************************
 */
void
reset(void)
{
    /* 0x05FA: VECTKEY����Կ�ף���ͬʱд��
     0x0004: ����оƬ����һ�θ�λ
     */
    *((uint32_t *) 0xE000ED0Cu) = 0x05FA0004u;
    return;
}
/**
 ******************************************************************************
 * @brief      Ƭ��flash��ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
void
iflash_init(void)
{
    /* ���� */
    FLASH_Unlock();
    /* ���״̬ */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}
/**
 ******************************************************************************
 * @brief      ����Ƭ��Flashָ�����������
 * @param[in]  uint32 address: ������ַ
 * @param[in]  uint32 size: ��������
 * @param[out] None
 * @retval     bool_e: TRUE-�ɹ� FALSE-ʧ��
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
bool_e
iflash_erase(uint32_t address,
        uint32_t size)
{
    uint32_t i;

    for (i = FLASH_Sector_2; i <= FLASH_Sector_7; i += 8u)
    {
        if (!FLASH_EraseSector(i, VoltageRange_3))
        {
            return (FALSE);
        }
    }
    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      ��ȡƬ��Flashָ�����������
 * @param[out] uint8 *pdata: ��ȡ����
 * @param[in]  uint32 address: Ŀ���ַ
 * @param[in]  uint32 size: ��ȡ����
 * @retval     bool_e: TRUE-�ɹ� FALSE-ʧ��
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
bool_e
iflash_read(uint8_t *pdata,
        uint32_t address,
        uint32_t size)
{
    uint32_t i;
    for (i = 0u; i < size; i++)
    {
        *pdata = *((uint8_t *) address);
        pdata++;
        address++;
    }
    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      �޸�Ƭ��Flashָ�����������
 * @param[in]  const uint8 * pdata: д������
 * @param[in]  uint32 address: Ŀ���ַ
 * @param[in]  uint32 size: д�볤��
 * @param[out] None
 * @retval     bool_e: TRUE-�ɹ� FALSE-ʧ��
 *
 * @details
 *
 * @note
 *
 ******************************************************************************
 */
bool_e
iflash_write(const uint8_t *pdata,
        uint32_t address,
        uint32_t size)
{
    uint32_t i;

    for (i = 0u; i < size; i += 4u)
    {
        /* д��4�ֽ� */
        if (!FLASH_ProgramWord(address + i, *(const uint32_t *)(pdata + i)))
        {
            return (FALSE);
        }
        /* У��д���Ƿ�ɹ� */
        if (*(uint32_t *)(address + i) != *(const uint32_t *)(pdata + i))
        {
            return (FALSE);
        }
    }

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      �ȴ�
 * @param[in]  const uint32_t usec: �ȴ�ʱ��
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
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
    }
    while (TRUE);
}

/*--------------------------------End of target.c----------------------------*/
