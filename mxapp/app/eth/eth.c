/**
 ******************************************************************************
 * @file       eth.c
 * @version    V0.0.1
 * @brief      ETHERNET应用层接口实现
 * @details
 *
 * @copy       Copyrigth(C), 2008-2012,Sanxing Electric Co.,Ltd.
 *
 ******************************************************************************
 */


/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
#include <net/lwip/mem.h>
#include <net/lwip/memp.h>
#include <net/lwip/tcp.h>
#include <net/lwip/udp.h>
#include <net/netif/etharp.h>
#include <net/lwip/dhcp.h>
#include "ethernetif.h"
#include "eth.h"
#include <net/lwip/tcp_impl.h>
#include <ethcfg.h>

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#define MAX_DHCP_TRIES        4
#define     __IO    volatile
#define OS_TASK_LN_STK_SIZE 4096
/*-----------------------------------------------------------------------------
Section: Private Type Definitions
-----------------------------------------------------------------------------*/
typedef enum
{
    DHCP_START = 0,
    DHCP_WAIT_ADDRESS,
    DHCP_ADDRESS_ASSIGNED,
    DHCP_TIMEOUT
} dhcp_state_e;


/*-----------------------------------------------------------------------------
Section: Global Variables
-----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
Section: Private Variables
-----------------------------------------------------------------------------*/
static struct netif the_netif;
static uint32_t the_arptimer = 0u;
static uint32_t the_eth_stack[OS_TASK_LN_STK_SIZE / 4];
static bool_e the_ethinit = FALSE;
static int32_t the_tid = -1;
static uint32_t the_needdhcp = 0u;
static uint32_t the_ipaddr ;
static uint32_t the_dhcpfinetimer = 0u;
static uint32_t the_dhcpcoarsetimer = 0u;
static dhcp_state_e the_dhcpstate = DHCP_START;


/*-----------------------------------------------------------------------------
Section: Private Function Prototypes
-----------------------------------------------------------------------------*/

void
eth_thread(void);
void
eth_dhcp_process_handle(void);

/*-----------------------------------------------------------------------------
Section: Public Function
-----------------------------------------------------------------------------*/
/**
 * @brief  eth thread
 * @param  None
 * @retval None
 */
void
eth_thread(void)
{
    while (1)
    {
        /* check if any packet received */
        if (ethernet_chk_receiveframe() > 0)
        {
            /* process received ethernet packet */
            /*logmsg("*");*/
            eth_pkt_handle();
        }
        /* handle periodic timers for LwIP */
        eth_periodic_handle(tickGet());
        taskDelay(1);
    }
}


/**
 * @brief  Initializes the eth
 * @param  None
 * @retval None
 */
void
eth_init(void)
{
    if (!the_ethinit)
    {
        struct ip_addr ipaddr;
        struct ip_addr netmask;
        struct ip_addr gw;

        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;

        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR0, GW_ADDR0, GW_ADDR0);

        if (ipaddr.addr != 0)
        {
            the_needdhcp = 0;
        }

        /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
         struct ip_addr *netmask, struct ip_addr *gw,
         void *state, err_t (* init)(struct netif *netif),
         err_t (* input)(struct pbuf *p, struct netif *netif))

         Adds your network interface to the netif_list. Allocate a struct
         netif and pass a pointer to this structure as the first argument.
         Give pointers to cleared ip_addr structures when using DHCP,
         or fill them with sane numbers otherwise. The state pointer may be NULL.

         The init function pointer must point to a initialization function for
         your ethernet netif interface. The following code illustrates it's use.*/
        netif_add(&the_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init,
                &ethernet_input);

        /*  Registers the default network interface.*/
        netif_set_default(&the_netif);

        /*  Set Mac */
        ethernet_setmac(&the_netif, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07);

        /*  When the netif is fully configured this function must be called.*/
        netif_set_up(&the_netif);

        the_tid = taskSpawn((uint8_t *) "eth", 3, the_eth_stack,
                OS_TASK_LN_STK_SIZE, (OSFUNCPTR) eth_thread, 0);
        the_ethinit = TRUE;
    }
}

/**
 * @brief  remove the eth
 * @param  None
 * @retval None
 */
void
eth_remove(void)
{
    if (the_ethinit == TRUE)
    {
        netif_remove(&the_netif);
        if (the_tid != -1)
        {
            (void) taskDelete((uint8_t) the_tid);
        }
        the_ethinit = FALSE;
    }
}

#if 1

#include <shell.h>
uint32_t
do_ethlowinit(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    low_level_init(&the_netif);
    return 0;
}

SHELL_CMD( ethlowinit, CFG_MAXARGS, do_ethlowinit, "v \r\t\t\t\t ETH low level init\r\n"

);


/**
 * @brief  shell for eth init
 * @param  None
 * @retval None
 */
uint32_t
do_ethinit(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    eth_init();
    return 0;
}

SHELL_CMD( ethinit, CFG_MAXARGS, do_ethinit, "v \r\t\t\t\t ETH init\r\n"
);


/**
 * @brief  shell for delete eth interface
 * @param  None
 * @retval None
 */
uint32_t
do_ethdel(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{
    eth_remove();
    return 0;
}

SHELL_CMD( ethdel, CFG_MAXARGS, do_ethdel, "v \r\t\t\t\t ETH delete\r\n"

);


uint32_t
do_macreset(cmd_tbl_t * cmdtp, uint32_t argc, uint8_t *argv[])
{

    ETH_DeInit();
    ETH_Start();
    logmsg("MAC RESET !\n");
    return 0;
}

SHELL_CMD( macreset, CFG_MAXARGS, do_macreset, "v \r\t\t\t\t MAC RESET\r\n"

);
#endif
/**
 * @brief  set the eth's addr
 * @param  struct ip_addr *ipaddr   终端IP
 * @param struct ip_addr *netmask   子网掩码
 * @param struct ip_addr *gw        网关
 * @retval None
 */
void
eth_set_addr(struct ip_addr *ipaddr,
        struct ip_addr *netmask,
        struct ip_addr *gw)
{
    netif_set_addr(&the_netif, ipaddr, netmask, gw);
}

/**
 * @brief  Called when a frame is received
 * @param  None
 * @retval None
 */
void
eth_pkt_handle(void)
{
    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    (void)ethernetif_input(&the_netif);
}

/**
 * @brief  LwIP periodic tasks
 * @param  localtime the current LocalTime value
 * @retval None
 */
void
eth_periodic_handle(__IO uint32_t localtime)
{
    /* ARP periodic process every 5s */
    if (abs(localtime - the_arptimer) >= (ARP_TMR_INTERVAL / 10))
    {
        the_arptimer = localtime;
        etharp_tmr();
    }

#if 0
    /* Fine DHCP periodic process every 500ms */
    if ((localtime - the_dhcpfinetimer) >= DHCP_FINE_TIMER_MSECS)
    {
        the_dhcpfinetimer = localtime;
        dhcp_fine_tmr();
        if ((the_dhcpstate != DHCP_ADDRESS_ASSIGNED) && (the_dhcpstate != DHCP_TIMEOUT))
        {
            if (the_needdhcp > 0)
            {
                /* process DHCP state machine */
                eth_dhcp_process_handle();
            }
        }
    }

    /* DHCP Coarse periodic process every 60s */
    if ((localtime - the_dhcpcoarsetimer) >= DHCP_COARSE_TIMER_MSECS)
    {
        the_dhcpcoarsetimer = localtime;
        dhcp_coarse_tmr();
    }
#endif

}


/**
 * @brief  eth_dhcp_process_handle
 * @param  None
 * @retval None
 */
void
eth_dhcp_process_handle(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    switch (the_dhcpstate)
    {
        case DHCP_START:
            {
                (void) dhcp_start(&the_netif);
                the_ipaddr = 0;
                the_dhcpstate = DHCP_WAIT_ADDRESS;
            }
            break;
#if 0
        case DHCP_WAIT_ADDRESS:
            {
                /* Read the new IP address */
                the_ipaddr = the_netif.ip_addr.addr;

                if (the_ipaddr != 0)
                {
                    the_dhcpstate = DHCP_ADDRESS_ASSIGNED;

                    /* Stop DHCP */
                    dhcp_stop(&the_netif);
                }
                else
                {
                    /* DHCP timeout */
                    if (the_netif.dhcp->tries > MAX_DHCP_TRIES)
                    {
                        the_dhcpstate = DHCP_TIMEOUT;
                        /* Stop DHCP */
                        dhcp_stop(&the_netif);
                        /* Static address used */
                        IP4_ADDR(&ipaddr, IP_ADDR0 , IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                        /*netif_set_addr(&the_netif, &ipaddr , &netmask, &gw);*/
                        the_rnc.params.localip = ipaddr;
                        the_rnc.params.netmask = netmask;
                        the_rnc.params.netgate = gw;
                    }
                }
            }
            break;
#endif
        default:
            break;
    }
}

/************************END OF FILE**********************************************8****/
