/**
 ******************************************************************************
 * @file       target.h
 * @brief      本文为boot主程序提供芯片操作函数.
 * @details    This file including all API functions's declare of types.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef __TARGET_H__
#define __TARGET_H__

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
extern void hw_init(void);
extern void iflash_init(void);
extern bool_e iflash_erase(uint32_t address, uint32_t size);
extern bool_e iflash_read(uint8_t *pdata, uint32_t address, uint32_t size);
extern bool_e iflash_write(const uint8_t *pdata, uint32_t address, uint32_t size);
extern void delay(const uint32_t usec);
extern void reset(void);
extern void uart_send(const uint8_t c);
extern bool_e uart_try_receive(uint8_t *pc);
extern void uart_receive(uint8_t *pdata, uint32_t size);
extern void print(const char_t* str);
extern void systick_open(void);
extern void systick_stop(void);

#endif /* __TARGET_H__ */
/*------------------------------End of target.h------------------------------*/
