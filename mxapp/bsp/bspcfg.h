/**
 ******************************************************************************
 * @file       bspcfg.h
 * @brief      API include file of bspcfg.h.
 * @details    This file including all API functions's declare of bspcfg.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef __BSPCFG_H__
#define __BSPCFG_H__

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/

#define BOARD_MX207     (1u)
#define BOARD_DIY103    (2u)
#define BOARD_GY103     (3u)
#define BOARD_TI811     (4u)

#define BOARD_BUILD_VER BOARD_MX207

#if (BOARD_BUILD_VER == BOARD_MX207)
# include "./board/mx207/boardcfg.h"
#elif (BOARD_BUILD_VER == BOARD_DIY103)
# include "./board/diy103/boardcfg.h"
#elif (BOARD_BUILD_VER == BOARD_GY103)
# include "./board/gy103/boardcfg.h"
#elif (BOARD_BUILD_VER == BOARD_TI811)
# include "./board/ti811/boardcfg.h"
#else
# error "BOARD_BUILD_VER define error!"
#endif

#ifndef BOARD_BANNER
# error "Please define 'BOARD_BANNER' in 'boardcfg.h'!"
#endif

#ifndef MAX_INT_COUNT
# error "Please define 'MAX_INT_COUNT' in 'boardcfg.h'!"
#endif

#ifndef CPU_CLOCK_HZ
# error "Please define 'CPU_CLOCK_HZ' in 'boardcfg.h'!"
#endif

#ifndef SHELL_STACK_SIZE
# error "Please define 'SHELL_STACK_SIZE' in 'boardcfg.h'!"
#endif

#ifndef ROOT_STACK_SIZE
# error "Please define 'ROOT_STACK_SIZE' in 'boardcfg.h'!"
#endif

#ifndef DMN_STACK_SIZE
# error "Please define 'DMN_STACK_SIZE' in 'boardcfg.h'!"
#endif

#ifndef EXC_STACK_SIZE
# error "Please define 'EXC_STACK_SIZE' in 'boardcfg.h'!"
#endif

#ifndef LOGMSG_STACK_SIZE
# error "Please define 'LOGMSG_STACK_SIZE' in 'boardcfg.h'!"
#endif

#endif /* __BSPCFG_H__ */
/*-----------------------------End of bspcfg.h-------------------------------*/
