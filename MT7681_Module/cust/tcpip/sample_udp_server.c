#include "uip.h"
#include "sample_udp_server.h"

void udp_server_init()
{
	//struct uip_conn *udp_conn=NULL;
	UIP_UDP_CONN *udp_conn=NULL;

	/* We don't specify a remote address and a remote port,
	   that means we can receive data from any address. */
	udp_conn = uip_udp_new(NULL, HTONS(0));
	if(udp_conn) {
		/* Bind to local 7777 port. */
		uip_udp_bind(udp_conn, HTONS(7682));
	}
}

void udp_server_sample()
{
	u8_t addr[16] = {0};
	uip_ipaddr_t ip;
	UIP_UDP_CONN *udp_conn = uip_udp_conn;

	if(uip_newdata()) {
		/* Here shows how to get the peer info of the received data. */
		sprintf((char *)addr, "%d.%d.%d.%d", 
				uip_ipaddr1(udp_conn->ripaddr), 
				uip_ipaddr2(udp_conn->ripaddr),
				uip_ipaddr3(udp_conn->ripaddr),
				uip_ipaddr4(udp_conn->ripaddr));
		
		printf("UDP Sample server RX: lp:%d,ra:%s,rp:%d,  got:%s\n",
			HTONS(udp_conn->lport), addr, HTONS(udp_conn->rport), uip_appdata);

		/* uip_appdata is a pointer pointed the received data. */
		if(!memcmp(uip_appdata, "ip", 2)) {
			uip_gethostaddr(&ip);
			sprintf((char *)addr, "%d.%d.%d.%d", 
					uip_ipaddr1(ip), uip_ipaddr2(ip), 
					uip_ipaddr3(ip), uip_ipaddr4(ip));
			uip_send(addr, 16);
		}
	}

	if (uip_poll()) {
	}

}

