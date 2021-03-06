/**
 ******************************************************************************
 * @file      key_test.c
 * @brief     C Source file of key_test.c.
 * @details   This file including all API functions's 
 *            implement of key_test.c.	
 *
 * @copyright
 ******************************************************************************
 */
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <devLib.h>
#include <gpio.h>
#include <taskLib.h>
#include <osLib.h>
#include <dmnLib.h>

#ifdef Dprintf
#undef Dprintf
#endif
#define Dprintf(x...)
/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */
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
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */
/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
void keyTask(void *p_arg)
{
    int32_t keyfd = -1;
	int8_t key_val = 0;
	uint32_t ticks_per_second;

	ticks_per_second = osClkRateGet();
	keyfd = dev_open("keys", 0);
	if(-1 == keyfd)
	{
		printf("open key error\n");
		return ;
	}
	while(1)
	{
		key_val = dev_read(keyfd, &key_val, 1);
		if(key_val > 0)
		{
			printf("Key%d is pressed\r\n", key_val);
		}
		taskDelay(ticks_per_second);
	}
    dev_close(keyfd);
    dev_release("keys");

}
/*----------------------------key_test.c-----------------------------------------*/


