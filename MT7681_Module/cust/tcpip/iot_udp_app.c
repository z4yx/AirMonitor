#include <stdio.h>
#include <string.h>

#include "iot_udp_app.h"
#include "dhcpc.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
#include "ntp.h"

#if CFG_SUPPORT_DNS
#include "resolv.h"
#endif

extern UCHAR gCurrentAddress[];
extern IOT_ADAPTER   	IoTpAd;
struct iot_udp_app_state udp_app_state;

void
iot_udp_app_init(void)
{
#ifdef CONFIG_SOFTAP
	dhcpd_init();
#else
	dhcpc_init(gCurrentAddress, 6);
#endif

#if CFG_SUPPORT_DNS
	/* DNS client. */
	resolv_init();
#endif
	/* Customer APP start. */
	ntp_init();

	/* udp_client_init(); */
	// udp_server_init();
	
	/* Customer APP end. */
	return;
}

void
iot_udp_appcall(void)
{
	UIP_UDP_CONN *udp_conn = uip_udp_conn;
	u16_t lport, rport;

	lport=HTONS(udp_conn->lport);
	rport=HTONS(udp_conn->rport);

	if(lport == DHCPC_CLIENT_PORT) {
		handle_dhcp();
#if CFG_SUPPORT_DNS
	} else if (rport == DNS_SERVER_PORT) {
		handle_resolv();
#endif
	/* Customer APP start. */
	} else if(rport == NTPPort) {
		ntp_appcall();

	// } else if (lport == 7682) {
	// 	udp_server_sample();
	/* } else if (lport == 6666) {
		udp_client_sample(); 
	} else if (lport == 8888) {
		resolv_usage_sample(); */

	/* Customer APP end. */
	}
#ifdef CONFIG_SOFTAP
	else if (DHCPC_SERVER_PORT == lport)
	{
		handle_dhcpd();
	}
#endif

	return;
}

