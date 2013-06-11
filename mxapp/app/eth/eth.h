/**
 ******************************************************************************
 * @file       eth.h
 * @version    V0.0.1
 * @brief      API include file of eth.
 * @details    This file including all API functions's declare of rtucfg.
 * @copy       Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETH_H
#define __ETH_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void
eht_init(void);
void
eth_pkt_handle(void);
void
eth_periodic_handle(volatile uint32_t localtime);
void
eth_set_addr(struct ip_addr *ipaddr, struct ip_addr *netmask,
        struct ip_addr *gw);
void
eth_remove();


#ifdef __cplusplus
}
#endif

#endif /* __ETH_H */

/************************END OF FILE*********************************************/

