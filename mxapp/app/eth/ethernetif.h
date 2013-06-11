#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include <types.h>
#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
void
ethernet_setmac(struct netif *netif,uint8_t mac_a0,uint8_t mac_a1,uint8_t mac_a2,uint8_t mac_a3,
                uint8_t mac_a4,uint8_t mac_a5);
uint32_t ethernet_chk_receiveframe();


#endif
