/**
 ******************************************************************************
 * @file      usrapp.c
 * @brief     本文实现应用程序的初始化.
 * @details   This file including all API functions's implement of usrapp.c.
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <taskLib.h>
#include <osLib.h>
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
#define  LEDSTACKSIZE  (512)
#define  KEYSTACKSIZE  (512)

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

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
extern void ledTask(void *p_arg);
extern void keyTask(void *p_arg);
/**
 ******************************************************************************
 * @brief   应用程序初始化
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void
usrapp_init(void)
{
	taskSpawn((const signed char*)"led", 3, LEDSTACKSIZE, (OSFUNCPTR)ledTask,0);
	taskSpawn((const signed char*)"key", 3, KEYSTACKSIZE, (OSFUNCPTR)keyTask,0);

    return;
}

/*--------------------------------usrapp.c-----------------------------------*/
