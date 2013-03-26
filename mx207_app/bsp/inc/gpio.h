/**
 ******************************************************************************
 * @file      gpio.h
 * @brief     C Source file of gpio.h.
 * @details   This file including all API functions's 
 *            implement of gpio.h.	
 *
 * @copyright
 ******************************************************************************
 */

#include <types.h>
#ifndef GPIO_H_
#define GPIO_H_

typedef enum
{
    IO_LED0 = 0,        /* LED0        */
    IO_LED1,            /* LED1        */
    IO_LED2,            /* LED2        */
    IO_LED3,            /* LED3        */
    IO_KEY0,            /* KEY0        */
    IO_KEY1,            /* KEY1        */
    IO_KEY2,            /* KEY2        */
    IO_KEY3,            /* KEY3        */
    IO_LCD_AK
} gpio_out_e;

extern status_t gpio_init(void);

#endif /* GPIO_H_ */
