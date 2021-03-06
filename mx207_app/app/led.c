/**
 ******************************************************************************
 * @file      led.c
 * @brief     C Source file of led.c.
 * @details   This file including all API functions's 
 *            implement of led.c.	
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
/**
 ******************************************************************************
 * @brief   ��ˮ������
 * @param[in]  None
 * @param[out] None
 *
 * @retval     None
 ******************************************************************************
 */
void ledTask(void *p_arg)
{
	int32_t fd = -1;
	uint32_t ticks_per_second;

	ticks_per_second = osClkRateGet();
	DMN_ID dmnid = dmn_register();
	fd = dev_open("gpio", 0);
	if(-1 == fd)
	{
		printf("open gpio error\n");
		return ;
	}
	while(1)
	{
		dmn_sign(dmnid);
		dev_ioctl(fd, 0,IO_LED0);
		taskDelay(ticks_per_second);
		dev_ioctl(fd, 1,IO_LED0);
		taskDelay(ticks_per_second);
	}
	dev_close(fd);
	dev_release("gpio");
}



