/*
 * =====================================================================================
 *
 *       Filename:  network.c
 *
 *    Description:  TCP/IP协议栈的初始化与运行时调用
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"

#include "common.h"
#include "enc28j60.h"
#include "configure.h"

static struct timer periodic_timer, arp_timer;

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void Network_Init()
{
	uip_ipaddr_t ipaddr;
	uint8_t ip[4] = ETH_IP_ADDR;
	uint8_t nm[4] = ETH_NETMASK;
	uint8_t gw[4] = ETH_GW_ADDR;

	tapdev_init();
	DBG_MSG("Chip Rev: %d", (int)enc28j60getrev());

	uip_init();

	uip_ipaddr(ipaddr, ip[0], ip[1], ip[2], ip[3]);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, gw[0], gw[1], gw[2], gw[3]);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, nm[0], nm[1], nm[2], nm[3]);
	uip_setnetmask(ipaddr);

    timer_set(&periodic_timer, CLOCK_SECOND / 10);
    timer_set(&arp_timer, CLOCK_SECOND / 2);

}

void Network_Task()
{
	uip_len = tapdev_read();
    if (uip_len > 0)
    {
        if (BUF->type == htons(UIP_ETHTYPE_IP))
        {
            uip_arp_ipin();
            uip_input();
            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                tapdev_send();
            }
        }
        else if (BUF->type == htons(UIP_ETHTYPE_ARP))
        {
            uip_arp_arpin();
            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                tapdev_send();
            }
        }

    }
    else if (timer_expired(&periodic_timer))
    {
    	int i;
        timer_reset(&periodic_timer);
        for (i = 0; i < UIP_CONNS; i++)
        {
            uip_periodic(i);
            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                tapdev_send();
            }
        }

#if UIP_UDP
        for (i = 0; i < UIP_UDP_CONNS; i++)
        {
            uip_udp_periodic(i);
            /* If the above function invocation resulted in data that
               should be sent out on the network, the global variable
               uip_len is set to a value > 0. */
            if (uip_len > 0)
            {
                uip_arp_out();
                tapdev_send();
            }
        }
#endif /* UIP_UDP */

        /* Call the ARP timer function every 10 seconds. */
        if (timer_expired(&arp_timer))
        {
            timer_reset(&arp_timer);
            uip_arp_timer();
        }
    }
}