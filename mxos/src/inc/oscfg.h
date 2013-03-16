/**
 ******************************************************************************
 * @file       oscfg.h
 * @brief      系统配置文件.
 * @details    This file including all API functions's declare of oscfg.h.
 *
 ******************************************************************************
 */
 

#ifndef __OSCFG_H__
#define __OSCFG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define OS_RELEASE  "0.0.1"

#define CPU_LM3S811     (0u)
#define CPU_STM32F103   (1u)
#define CPU_STM32F207   (2u)

#define OS_BUILD_VER    CPU_STM32F207

#if (OS_BUILD_VER == CPU_LM3S811)
# include"./cpucfg/lm3s811cfg.h"
#elif (OS_BUILD_VER == CPU_STM32F103)
# include"./cpucfg/stm32f103cfg.h"
#elif (OS_BUILD_VER == CPU_STM32F207)
# include"./cpucfg/stm32f207cfg.h"
#endif

#endif /* __OSCFG_H__ */
/*------------------------------End of oscfg.h-------------------------------*/
