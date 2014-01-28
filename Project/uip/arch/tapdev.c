#include "uip.h"
#include "enc28j60.h"
#include "configure.h"

extern struct uip_eth_addr uip_ethaddr;
static uint8_t mac_addr[6] = ETH_MAC_ADDR;

/*---------------------------------------------------------------------------*/
void
tapdev_init(void)
{
    int i;
    for (i = 0; i < 6; i++)
        uip_ethaddr.addr[i] = mac_addr[i];
    enc28j60Init(mac_addr);
}
/*---------------------------------------------------------------------------*/
unsigned int
tapdev_read(void)
{
    return enc28j60PacketReceive(UIP_CONF_BUFFER_SIZE, uip_buf);
}
/*---------------------------------------------------------------------------*/
void
tapdev_send(void)
{
    enc28j60PacketSend(uip_len, uip_buf);
}
/*---------------------------------------------------------------------------*/