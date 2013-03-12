/**
 ******************************************************************************
 * @file       main.c
 * @version    V0.0.1
 * @brief      mx207��boot���򣬱�����ʵ������Ӧ�ó����Լ���������Ӧ�ó�����
 * @details
 * @copyright
 *
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>
#include <target.h>
#include <update.h>
/*----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
extern uint32_t _etext; //The end of text;
extern uint32_t _bss;   //The start of bss;
extern uint32_t _ebss;
extern uint32_t _data;
extern uint32_t _edata;
typedef void (* p_routine)(void);
/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
static void reset_routine(void);
static void nmi_routine(void);
static void fault_routine(void);
static void mem_fault_routine(void);
static void bus_fault_routine(void);
static void usage_fault_routine(void);
static void dummy(void);
static void systick_routine(void);
//static void set_msp(uint32_t main_stack);
static void jump_to_app(uint32_t address);
static void main(void);
/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
#define STACK_SIZE            (8 * 1024)    /* ջ��С(��λΪ��) */
#define APP_START_ADDRESS     0x08008000u   /* Ӧ�ó�����ʼ��ַ��32K */
static volatile uint32_t the_run_time = 0u; /* �������������ۼ�ʱ��(��) */
__attribute__((section(".stackarea")))
static uint32_t the_cstack[STACK_SIZE];
__attribute__((section(".isr_vector")))
const p_routine vector_table[] =
{
        (p_routine)((uint32_t) the_cstack + sizeof(the_cstack)),/* ջ��ָ�� */
        reset_routine, /* ��λ�ж�  */
        nmi_routine, /* NMI�ж� */
        fault_routine, /* Ӳ������ */
        mem_fault_routine, /* �洢��������� */
        bus_fault_routine, /* ���ߴ��� */
        usage_fault_routine, /* �÷����� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        systick_routine, /* 15ϵͳ��ʱ�� */

        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 21���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 31���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 41���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 51���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 61���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 71���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */
        dummy, /* ���� */

        dummy, /* 81���� */
        dummy, /* 82���� */
        dummy, /* 83 */
};

/**
 ******************************************************************************
 * @brief      ��λ�ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void reset_routine(void)
{
    uint32_t* pul_src;
    uint32_t* pul_dest;

    /* �����渴�Ƴ�ʼ�����ݶε�SRAM��. */

    for(pul_dest = &_data, pul_src = &_etext; pul_dest < &_edata; )
    {
        *pul_dest++ = *pul_src++;
    }

    /* BSS���ݶ�����. */
    for(pul_dest = &_bss; pul_dest < &_ebss; )
    {
        *pul_dest++ = 0;
    }
    /* ��ת��������. */
    main();
}

/**
 ******************************************************************************
 * @brief      NMI�ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void nmi_routine(void)
{
    while (TRUE)
    {
    }
}
/**
 ******************************************************************************
 * @brief      Ӳ��������
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void fault_routine(void)
{
    while (TRUE)
    {
    }
}
/**
 ******************************************************************************
 * @brief      �洢�����������
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void mem_fault_routine(void)
{
    while (TRUE)
    {
    }
}
/**
 ******************************************************************************
 * @brief      ���ߴ�����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void bus_fault_routine(void)
{
    while (TRUE)
    {
    }
}
/**
 ******************************************************************************
 * @brief      �÷�������
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void usage_fault_routine(void)
{
    while (TRUE)
    {
    }
}
/**
 ******************************************************************************
 * @brief      �����ж����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void dummy(void)
{
    return;
}
/**
 ******************************************************************************
 * @brief      ��ʱ���жϴ���
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note       ע�⣺Ҫ��ÿ�����ж�һ��
 ******************************************************************************
 */

static void systick_routine(void)
{
       static uint32_t ms = 0u; /* ������ */

        /* ÿ�����ж�һ�� */
        ms++;
        if (ms < 1000)
        {
            return;
        }
        ms = 0;

        /* ���������������ʱ��(10����)������ι����ʹ���Զ���λ */
        if (the_run_time >= (10u * 60u))
        {
            /* reboot */
            //TODO
//          print("reboot!\r\n");
//          reset();
            return;
        }
        the_run_time++;
}

/**
 ******************************************************************************
 * @brief      ����������
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */

void main(void)
{
    /* ��ʼ���豸 */
    hw_init();

    print("\r\n");
    print("*********************************************\r\n");
    print("*               BootLoader v1.1             *\r\n");
    print("*           Copyright 2013 mx207.           *\r\n");
    print("*********************************************\r\n");

    /* ������ʱ�� */
    systick_open();

    /* �������� */
    if (OK != uart_update())
    {
        return;
    }

    /* �رն�ʱ�� */
    systick_stop();
    /* ��ת��Ӧ��ϵͳ */
    print("Enter application ...\r\n");
    /* ��ת */
    jump_to_app(*(volatile uint32_t *) (APP_START_ADDRESS + 4u));

    while (1)
    {
    }

}
/**
 ******************************************************************************
 * @brief      ��ȡ��ǰϵͳ����ʱ��
 * @param[in]  None
 * @param[out] None
 * @retval     uint32_t: ���е�ʱ��
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern uint32_t get_systime(void)
{
    return the_run_time;
}
/**
 ******************************************************************************
 * @brief      ��ת��Ӧ�ó����ַ
 * @param[in]  uint32_t address
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void jump_to_app(uint32_t address)
{
    __asm(
            "movw    r3, #32768\n"
            "movt    r3, #2048\n"
            "ldr r3, [r3, #0]\n"
            "mov r0, r3\n"
            "msr   MSP, r0"
    );

    __asm(
            "movw    r3, #32772\n"
            "movt    r3, #2048\n"
            "ldr r3, [r3, #0]\n"
            "mov r0, r3\n"
            "bx r0"
    );
}

/*--------------------------------End of main.c------------------------------*/
