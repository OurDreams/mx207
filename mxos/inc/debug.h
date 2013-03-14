/**
 ******************************************************************************
 * @file       debug.h
 * @brief      API include file of debug.h.
 * @details    This file including all API functions's declare of debug.h.
 *
 ******************************************************************************
 */
 
#ifndef __DEBUG_H__
#define __DEBUG_H__
/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <stdio.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
/** ���Ե��Կ��� */
#define ASSERT_DEBUG
/** ������Ϣ���� */
#define DEBUG_INFO

#ifdef ASSERT_DEBUG
#  define D_ASSERT(EX)  \
    if (!(EX))          \
    {                   \
        __assert_func(__FILE__, __LINE__, __FUNCTION__, #EX);\
    }

#else   /* DPROC_DEBUG */
#  define D_ASSERT(EX) (void)0
#endif  /* DPROC_DEBUG */

#ifdef DEBUG_INFO
# define Dprintf(x...) \
  {    \
    (void)printf("\'%s\'L[%d] %s()->", __FILE__, __LINE__, __FUNCTION__); \
    (void)printf(x);    \
  }
#else
# define Dprintf(x...)
#endif

/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern void
__assert_func(const char *, int, const char *, const char *);
extern void
printbuffer(char_t* format,
            uint8_t* buffer,
            int32_t len);

#endif /* __DEBUG_H__*/
/*-----------------------------End of debug.h--------------------------------*/
