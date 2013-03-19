/**
 ******************************************************************************
 * @file      udelay.c
 * @brief     本文实现微妙延时函数.
 * @details   This file including all API functions's implement of udelay.c.
 *
 * @copyright
 ******************************************************************************
 */
#ifndef __UDELAY_H__
#define __UDELAY_H__

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Globals
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern void
usleep(int32_t us);


#endif /* __UDELAY_H__ */
/*----------------------------End of udelay.h--------------------------------*/
