#include "uip.h"
#include "sample_udp_client.h"
#include "uip_timer.h"

static struct timer udp_client_timer;

void udp_client_init()
{
	UIP_UDP_CONN *udp_conn=NULL;
	uip_ipaddr_t raddr={0};

	uip_ipaddr(raddr, 255,255,255,255);

	/* Specify remote address and port here. */
	udp_conn = uip_udp_new(&raddr, HTONS(9999));
	if (udp_conn) {
		/* Specify local port here. */
		uip_udp_bind(udp_conn, HTONS(6666));
	}

	/* We start a timer and send "hello world" every 10 SEC. */
	timer_set(&udp_client_timer, 10*CLOCK_SECOND);
}

void udp_client_sample()
{
	if(uip_newdata()) {
		Printf_High("Client RX %d bytes\n", uip_datalen());
		/* below codes shows how to read data. */
	}

	if (uip_poll()) {
		/* below codes shows how to send data. */
		if(timer_expired(&udp_client_timer)) {
			uip_send("hello world.", 12);
		}
	}
}

