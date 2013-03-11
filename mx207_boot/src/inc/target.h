/**
 ******************************************************************************
 * @file       target.c
 * @version    V1
 * @brief      API C source file of target.c
 *             Created on: 2013-3-10
 *             Author: daixuyi
 * @details    This file including all API functions's implement of dps.
 * @copy       Copyrigth(C), 2013-2015, daixuyi.
 *
 ******************************************************************************
 */

#ifndef TARGET_H_
#define TARGET_H_

extern void hw_init(void);
extern void iflash_init(void);
extern bool_e iflash_erase(uint32_t address,uint32_t size);
extern bool_e iflash_read(uint8_t *pdata,uint32_t address,uint32_t size);
extern bool_e iflash_write(const uint8_t *pdata,uint32_t address, uint32_t size);
extern void delay(const uint32_t usec);
extern void reset(void);
extern void uart_send(const uint8_t c);
extern bool_e uart_try_receive(uint8_t *pc);
extern void uart_receive(uint8_t *pdata, uint32_t size);
extern void print(const uint8_t* str);
extern void systick_open(void);
extern void systick_stop(void);


#endif /* TARGET_C_ */
