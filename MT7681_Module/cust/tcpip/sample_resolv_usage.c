#include "uip.h"
#include "sample_resolv_usage.h"
#include "resolv.h"
#include "uip_timer.h"

struct sample_state {
	u8_t state;
};

#define SAMPLE_STATE_INIT 1
#define SAMPLE_STATE_DNS_QUERYING 2
#define SAMPLE_STATE_DONE 3

static struct sample_state ss;
static struct timer dns_timer;

void resolv_found(char *name, u16_t *ipaddr)
{
	if (name == NULL || ipaddr == NULL) {
		return;
	}
	ss.state = SAMPLE_STATE_DONE;
	Printf_High("DNS %s, %d.%d.%d.%d\n", name,
					htons(ipaddr[0]) >>8,  
					htons(ipaddr[0]) & 0xff,  
					htons(ipaddr[1]) >>8,  
					htons(ipaddr[1]) & 0xff);  
	resolv_usage_init(ipaddr, 80);
	return;
}

void resolv_usage_init(u16_t *rip, u16_t rport)
{
	struct uip_conn *udp_conn=NULL;

	udp_conn = uip_udp_new(rip, HTONS(rport));
	if (udp_conn) {
		uip_udp_bind(udp_conn, HTONS(8888));
	}
}

void resolv_usage_sample()
{
	if(uip_newdata()) {

	}

	if(uip_poll()) {

	}

	return 0;
}

