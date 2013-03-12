/**
 ******************************************************************************
 * @file       main.c
 * @version    V0.0.1
 * @brief      mx207的boot程序，本程序实现引导应用程序以及串口升级应用程序功能
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
#define STACK_SIZE            (8 * 1024)    /* 栈大小(单位为字) */
#define APP_START_ADDRESS     0x08008000u   /* 应用程序起始地址，32K */
static volatile uint32_t the_run_time = 0u; /* 升级程序运行累计时间(秒) */
__attribute__((section(".stackarea")))
static uint32_t the_cstack[STACK_SIZE];
__attribute__((section(".isr_vector")))
const p_routine vector_table[] =
{
        (p_routine)((uint32_t) the_cstack + sizeof(the_cstack)),/* 栈顶指针 */
        reset_routine, /* 复位中断  */
        nmi_routine, /* NMI中断 */
        fault_routine, /* 硬件错误 */
        mem_fault_routine, /* 存储器管理错误 */
        bus_fault_routine, /* 总线错误 */
        usage_fault_routine, /* 用法错误 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        systick_routine, /* 15系统定时器 */

        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 21空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 31空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 41空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 51空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 61空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 71空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */
        dummy, /* 空闲 */

        dummy, /* 81空闲 */
        dummy, /* 82空闲 */
        dummy, /* 83 */
};

/**
 ******************************************************************************
 * @brief      复位中断入口
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

    /* 从闪存复制初始化数据段到SRAM中. */

    for(pul_dest = &_data, pul_src = &_etext; pul_dest < &_edata; )
    {
        *pul_dest++ = *pul_src++;
    }

    /* BSS数据段清零. */
    for(pul_dest = &_bss; pul_dest < &_ebss; )
    {
        *pul_dest++ = 0;
    }
    /* 跳转到主程序. */
    main();
}

/**
 ******************************************************************************
 * @brief      NMI中断入口
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
 * @brief      硬件错误处理
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
 * @brief      存储器管理错误处理
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
 * @brief      总线错误处理
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
 * @brief      用法错误处理
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
 * @brief      空闲中断入口
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
 * @brief      定时器中断处理
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note       注意：要求每毫秒中断一次
 ******************************************************************************
 */

static void systick_routine(void)
{
       static uint32_t ms = 0u; /* 毫秒数 */

        /* 每毫秒中断一次 */
        ms++;
        if (ms < 1000)
        {
            return;
        }
        ms = 0;

        /* 如果超过允许运行时间(10分钟)，则不再喂狗，使其自动复位 */
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
 * @brief      升级主函数
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
    /* 初始化设备 */
    hw_init();

    print("\r\n");
    print("*********************************************\r\n");
    print("*               BootLoader v1.1             *\r\n");
    print("*           Copyright 2013 mx207.           *\r\n");
    print("*********************************************\r\n");

    /* 开启定时器 */
    systick_open();

    /* 串口升级 */
    if (OK != uart_update())
    {
        return;
    }

    /* 关闭定时器 */
    systick_stop();
    /* 跳转至应用系统 */
    print("Enter application ...\r\n");
    /* 跳转 */
    jump_to_app(*(volatile uint32_t *) (APP_START_ADDRESS + 4u));

    while (1)
    {
    }

}
/**
 ******************************************************************************
 * @brief      获取当前系统运行时间
 * @param[in]  None
 * @param[out] None
 * @retval     uint32_t: 运行的时间
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
 * @brief      跳转到应用程序地址
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
