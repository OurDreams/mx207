/**
 ******************************************************************************
 * @file      startup_m3.c
 * @brief     内核中断向量表.
 * @details   This file including all API functions's 
 *            implement of startup_m3.c.
 *
 ******************************************************************************
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define ROUTINE(n) routine##n

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* 中断向量表项，存放栈顶地址及ISR地址 */
typedef union
{
    VOIDFUNCPTR routine;     /* 中断处理函数地址 */
    void *msp;               /* 栈顶地址         */
} INTVECT_ITEM;

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/**
 *  引用在链接脚本文件中定义的符号
 */
extern unsigned long _sidata;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long cstack_top;


extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );
extern int main();


extern  void faultRoutine(void) __attribute__ (( weak ));
extern  void memFaultRoutine(void) __attribute__ (( weak ));
extern  void busFaultRoutine(void) __attribute__ (( weak ));
extern  void usageFaultRoutine(void) __attribute__ (( weak ));

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

/**
 ******************************************************************************
 * @brief      复位中断入口.
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
resetRoutine(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_sidata;
    for (pulDest = &_data; pulDest < &_edata;)
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &_bss; pulDest < &_ebss; )
    {
        *pulDest++ = 0;
    }

    //
    // Call the application's entry point.
    //
    main();
}

/**
 ******************************************************************************
 * @brief   NMI中断入口
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
NMIRoutine(void)
{
    while (TRUE)
    {
        ;
    }
}

/**
 ******************************************************************************
 * @brief   空闲中断入口
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
dummy(void)
{
    return;
}

extern void intHandler(uint32_t irq_num);

/* 其它中断 */
void    routine16   (void)  { intHandler(   16  ); }
void    routine17   (void)  { intHandler(   17  ); }
void    routine18   (void)  { intHandler(   18  ); }
void    routine19   (void)  { intHandler(   19  ); }
void    routine20   (void)  { intHandler(   20  ); }
void    routine21   (void)  { intHandler(   21  ); }
void    routine22   (void)  { intHandler(   22  ); }
void    routine23   (void)  { intHandler(   23  ); }
void    routine24   (void)  { intHandler(   24  ); }
void    routine25   (void)  { intHandler(   25  ); }
void    routine26   (void)  { intHandler(   26  ); }
void    routine27   (void)  { intHandler(   27  ); }
void    routine28   (void)  { intHandler(   28  ); }
void    routine29   (void)  { intHandler(   29  ); }
void    routine30   (void)  { intHandler(   30  ); }
void    routine31   (void)  { intHandler(   31  ); }
void    routine32   (void)  { intHandler(   32  ); }
void    routine33   (void)  { intHandler(   33  ); }
void    routine34   (void)  { intHandler(   34  ); }
void    routine35   (void)  { intHandler(   35  ); }
void    routine36   (void)  { intHandler(   36  ); }
void    routine37   (void)  { intHandler(   37  ); }
void    routine38   (void)  { intHandler(   38  ); }
void    routine39   (void)  { intHandler(   39  ); }
void    routine40   (void)  { intHandler(   40  ); }
void    routine41   (void)  { intHandler(   41  ); }
void    routine42   (void)  { intHandler(   42  ); }
void    routine43   (void)  { intHandler(   43  ); }
void    routine44   (void)  { intHandler(   44  ); }
void    routine45   (void)  { intHandler(   45  ); }
void    routine46   (void)  { intHandler(   46  ); }
void    routine47   (void)  { intHandler(   47  ); }
void    routine48   (void)  { intHandler(   48  ); }
void    routine49   (void)  { intHandler(   49  ); }
void    routine50   (void)  { intHandler(   50  ); }
void    routine51   (void)  { intHandler(   51  ); }
void    routine52   (void)  { intHandler(   52  ); }
void    routine53   (void)  { intHandler(   53  ); }
void    routine54   (void)  { intHandler(   54  ); }
void    routine55   (void)  { intHandler(   55  ); }
void    routine56   (void)  { intHandler(   56  ); }
void    routine57   (void)  { intHandler(   57  ); }
void    routine58   (void)  { intHandler(   58  ); }
void    routine59   (void)  { intHandler(   59  ); }

/* 中断向量表 */
__attribute__((section(".isr_vector")))
const INTVECT_ITEM __vector_table[] =
{
    { (void *)&cstack_top},     // 栈顶指针
    { resetRoutine},            // 复位中断
    {NMIRoutine},               // NMI中断
#if 0
    {dummy},                    // 硬件错误中断
    {dummy},                    // 存储器管理错误中断
    {dummy},                    // 总线错误中断
    {dummy},                    // 用法错误中断
#else
    {faultRoutine},           // 硬件错误中断
    {memFaultRoutine},        // 存储器管理错误中断
    {busFaultRoutine},        // 总线错误中断
    {usageFaultRoutine},      // 用法错误中断
#endif
    {dummy},                  // 空闲中断
    {dummy}, {dummy}, {dummy},
    {vPortSVCHandler},        // FreeRtos SVC中断
    {dummy}, {dummy},
    {xPortPendSVHandler},     // FreeRtos Pend中断
    {xPortSysTickHandler},    // FreeRtos SysTick中断
    {ROUTINE(16 )}, {ROUTINE(17 )},{ROUTINE(18 )},{ROUTINE(19 )},
    {ROUTINE(20 )}, {ROUTINE(21 )},{ROUTINE(22 )},{ROUTINE(23 )},
    {ROUTINE(24 )}, {ROUTINE(25 )},{ROUTINE(26 )},{ROUTINE(27 )},
    {ROUTINE(28 )}, {ROUTINE(29 )},{ROUTINE(30 )},{ROUTINE(31 )},
    {ROUTINE(32 )}, {ROUTINE(33 )},{ROUTINE(34 )},{ROUTINE(35 )},
    {ROUTINE(36 )}, {ROUTINE(37 )},{ROUTINE(38 )},{ROUTINE(39 )},
    {ROUTINE(40 )}, {ROUTINE(41 )},{ROUTINE(42 )},{ROUTINE(43 )},
    {ROUTINE(44 )}, {ROUTINE(45 )},{ROUTINE(46 )},{ROUTINE(47 )},
    {ROUTINE(48 )}, {ROUTINE(49 )},{ROUTINE(50 )},{ROUTINE(51 )},
    {ROUTINE(52 )}, {ROUTINE(53 )},{ROUTINE(54 )},{ROUTINE(55 )},
    {ROUTINE(56 )}, {ROUTINE(57 )},{ROUTINE(58 )},{ROUTINE(59 )}
};

/*---------------------------- startup_m3.c ---------------------------------*/
