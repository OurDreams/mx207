/**
 ******************************************************************************
 * @file      bsptask.h
 * @brief     C Source file of bsptask.h.
 * @details   This file including all API functions's 
 *            implement of bsptask.h.	
 *
 * @copyright
 ******************************************************************************
 */

#ifndef BSPTASK_H_
#define BSPTASK_H_

typedef void *BSP_ID;

extern BSP_ID func_register(OSFUNCPTR func);
extern status_t func_unregister(BSP_ID func_id);
extern status_t bsp_task_init(void);

#endif /* BSPTASK_H_ */
