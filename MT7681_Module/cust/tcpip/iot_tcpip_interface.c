#if ATCMD_TCPIP_SUPPORT

#include "mt76xx_dev.h"
#include "uiplib.h"
#include "iot_tcpip_interface.h"
#include "dhcpc.h"
#include "uip_arp.h"

extern struct iot_udp_app_state udp_app_state;
extern u8_t gCurrentAddress[];

int iot_netif_cfg(u8_t *ip, u8_t *mask, u8_t * gw, u8_t *dns)
{
	uip_ipaddr_t addr;

	if (ip && mask && gw && dns) { 
		if(uiplib_ipaddrconv((char *)ip,(unsigned char *)&addr) == 0) {
			return -1;
		}
		uip_sethostaddr(addr);
		
		if(uiplib_ipaddrconv((char *)mask,(unsigned char *)&addr) == 0) {
			return -2;
		}
		uip_setnetmask(addr);
		
		if(uiplib_ipaddrconv((char *)gw,(unsigned char *)&addr) == 0) {
			return -3;
		}
		uip_setdraddr(addr);
		
#if CFG_SUPPORT_DNS
		if(uiplib_ipaddrconv((char *)dns,(unsigned char *)&addr) == 0) {
			return -4;
		}

		resolv_conf(addr);
		resolv_query("www.baidu.com");
#endif
		dhcpc_set_state(STATE_CONFIG_DONE);
		return 0;
	}
	return -5;
}

void iot_getipaddr(u8_t *ip, u8_t *mask, u8_t *gw, u8_t *dns)
{
	uip_ipaddr_t addr;

	if (ip) {
		uip_gethostaddr(&addr);
		sprintf((char *)ip, "%d.%d.%d.%d", uip_ipaddr1(addr), uip_ipaddr2(addr), uip_ipaddr3(addr),uip_ipaddr4(addr));
	}

	if (mask) {
		uip_getnetmask(&addr);
		sprintf((char *)mask, "%d.%d.%d.%d", uip_ipaddr1(addr), uip_ipaddr2(addr), uip_ipaddr3(addr),uip_ipaddr4(addr));
	}

	if (gw) {
		uip_getdraddr(&addr);
		sprintf((char *)gw, "%d.%d.%d.%d", uip_ipaddr1(addr), uip_ipaddr2(addr), uip_ipaddr3(addr),uip_ipaddr4(addr));
	}

	if (dns) {
		uip_gethostaddr(&addr);
		sprintf((char *)dns, "%d.%d.%d.%d", uip_ipaddr1(addr), uip_ipaddr2(addr), uip_ipaddr3(addr),uip_ipaddr4(addr));
	}
	return;
}


int iot_connect(u8_t *dst, u16_t port)
{
	struct uip_conn *conn=NULL;
	uip_ipaddr_t ip;

	if(uiplib_ipaddrconv((char *)dst,(unsigned char *)&ip) == 0) {
		return -1;
	}
	
	conn = uip_connect(&ip, htons(port));
	if (conn != NULL) {
		return conn->fd;
	} else {
		return -1;
	}
}

void iot_disconnect(u8_t fd)
{
	if (fd < TCPUDP_FD_MAGIC) {
		uip_conns[fd].appstate.state = IOT_APP_S_CLOSED;
	} else {
		uip_udp_remove(&uip_udp_conns[fd-TCPUDP_FD_MAGIC]);
	}
	return;
}

int iot_send(u8_t fd, u8_t *buf, u16_t len)
{
	struct iot_tcp_app_state *s = &(uip_conns[fd].appstate);
	u8_t state = s->state;

	if (state == IOT_APP_S_CLOSED) {
		return -1;
	} else if (state == IOT_APP_S_DATA_SEND) {
		return -2;
	} else if (state == IOT_APP_S_WAIT_SEND){
		return -3;
	} else {
		s->buf = buf;
		s->len = len;
		s->state = IOT_APP_S_WAIT_SEND;
		uip_poll_conn(&uip_conns[fd]);		
		if(uip_len > 0) {
			uip_arp_out();
			mt76xx_dev_send();
		}
		
		return len;
	}
}
int iot_listen (u16_t port)
{
	uip_listen(HTONS(port));
	return 0;
}

int iot_unlisten (u16_t port)
{
	uip_unlisten(HTONS(port));
	return 0;
}

int iot_udp_new(u8_t *rip, u16_t lport, u16_t rport)
{
	uip_ipaddr_t addr;
	UIP_UDP_CONN *udp_conn = NULL;

	if (rip) {	
		if(uiplib_ipaddrconv((char *)rip,(unsigned char *)&addr) == 0) {
			return -1;
		}
		udp_conn = uip_udp_new(&addr, HTONS(rport));
	} else {
		udp_conn = uip_udp_new(NULL, HTONS(rport));
	}

	if(udp_conn != NULL) {
		uip_udp_bind(udp_conn, HTONS(lport));
		return udp_conn->fd;
	} else {
		return -2;
	}
}

void iot_udp_remove(u8_t fd)
{
	UIP_UDP_CONN *udp_conn = &uip_udp_conns[fd-TCPUDP_FD_MAGIC];

	uip_udp_remove(udp_conn);

	return; 
}

int iot_udp_send(u8_t fd, u8_t *buf, u16_t len, u8_t *rip, u16_t rport)
{
	UIP_UDP_CONN *udp_conn = &uip_udp_conns[fd-TCPUDP_FD_MAGIC];
	
	udp_app_state.ripaddr = rip;
	udp_app_state.fd = fd;
	udp_app_state.new_data = 1;
	udp_app_state.len = len;
	udp_app_state.buf = buf;
	udp_app_state.rport = rport;
	uip_udp_poll(udp_conn);
	
	if(uip_len > 0) {
		uip_arp_out();
		mt76xx_dev_send();
	}

	return len;
}
#endif
