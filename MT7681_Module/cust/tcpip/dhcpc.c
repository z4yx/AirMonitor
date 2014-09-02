/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * @(#)$Id: dhcpc.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 */
#include <stdio.h>
#include <string.h>
#include "uip.h"
#include "dhcpc.h"
#include "iot_api.h"

/*---------------------------------------------------------------------------*/
/*to indicated wheter store the SMNT setting after Wifi Connected AP and Got IP*/
/*it shalll be set to True after IoTCustOp.IoTCustSMNTStaChgInit()  */
extern BOOLEAN gSmnted;   

extern IOT_CUST_OP IoTCustOp;
extern MLME_STRUCT *pIoTMlme;
/*---------------------------------------------------------------------------*/

static struct dhcpc_state s;

struct dhcp_msg {
  u8_t op, htype, hlen, hops;
  u8_t xid[4];
  u16_t secs, flags;
  u8_t ciaddr[4];
  u8_t yiaddr[4];
  u8_t siaddr[4];
  u8_t giaddr[4];
  u8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  u8_t sname[64];
  u8_t file[128];
#endif
  u8_t options[312];
};

#define BOOTP_BROADCAST 0x8000
#define BOOTP_UNICAST 	0x0

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_HOST_NAME   12
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255

static const u8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
static const u8_t magic_cookie[4] = {99, 130, 83, 99};
static UCHAR hostname[8] = DEFAULT_DHCP_HOST_NAME;
/*---------------------------------------------------------------------------*/
static u8_t *
add_msg_type(u8_t *optptr, u8_t type) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_msg_type(u8_t *optptr, u8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_server_id(u8_t *optptr) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_server_id(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, s.serverid, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_ipaddr(u8_t *optptr) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_req_ipaddr(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_IPADDR;
  *optptr++ = 4;
  memcpy(optptr, s.ipaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_options(u8_t *optptr) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_req_options(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_LIST;
  *optptr++ = 3;
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = DHCP_OPTION_ROUTER;
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_other_options(u8_t *optptr, u8_t code, PCHAR data, u8_t len) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_other_options(u8_t *optptr, u8_t code, PCHAR data, u8_t len)
{
  *optptr++ = code;
  *optptr++ = len;
  memcpy(optptr, data, len);
  optptr += len;

  return optptr;
}

/*---------------------------------------------------------------------------*/
static u8_t *
add_end(u8_t *optptr) XIP_ATTRIBUTE(".xipsec1");

static u8_t *
add_end(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static void
create_msg(register struct dhcp_msg *m) XIP_ATTRIBUTE(".xipsec1");

static void
create_msg(register struct dhcp_msg *m)
{
  m->op = DHCP_REQUEST;
  m->htype = DHCP_HTYPE_ETHERNET;
  m->hlen = s.mac_len;
  m->hops = 0;
  memcpy(m->xid, xid, sizeof(m->xid));
  m->secs = 0;
  //m->flags = HTONS(BOOTP_BROADCAST); /*  Broadcast bit. */
  m->flags = HTONS(BOOTP_UNICAST); /*jinchuan 0512 , Fix: some AP may feedback DHCP OFFER/ACK in a  long time*/
  /*  uip_ipaddr_copy(m->ciaddr, uip_hostaddr);*/
  memcpy(m->ciaddr, uip_hostaddr, sizeof(m->ciaddr));
  memset(m->yiaddr, 0, sizeof(m->yiaddr));
  memset(m->siaddr, 0, sizeof(m->siaddr));
  memset(m->giaddr, 0, sizeof(m->giaddr));
  memcpy(m->chaddr, s.mac_addr, s.mac_len);
  memset(&m->chaddr[s.mac_len], 0, sizeof(m->chaddr) - s.mac_len);
#ifndef UIP_CONF_DHCP_LIGHT
  memset(m->sname, 0, sizeof(m->sname));
  memset(m->file, 0, sizeof(m->file));
#endif

  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
}
/*---------------------------------------------------------------------------*/
static void
send_discover(void) XIP_ATTRIBUTE(".xipsec1");

static void
send_discover(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  printf("dhcp TX DIS\n");
  create_msg(m);

  end = add_msg_type(&m->options[4], DHCPDISCOVER);
  end = add_req_options(end);
  end = add_other_options(end, DHCP_OPTION_HOST_NAME, hostname, strlen(hostname));
  end = add_end(end);

  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static void
send_request(void) XIP_ATTRIBUTE(".xipsec1");

static void
send_request(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  printf("dhcp TX REQ\n");
  create_msg(m);
  
  end = add_msg_type(&m->options[4], DHCPREQUEST);
  end = add_server_id(end);
  end = add_req_ipaddr(end);
  end = add_other_options(end, DHCP_OPTION_HOST_NAME, hostname, strlen(hostname));
  end = add_end(end);
  
  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static u8_t
parse_options(u8_t *optptr, int len)  XIP_ATTRIBUTE(".xipsec1");

static u8_t
parse_options(u8_t *optptr, int len)
{
  u8_t *end = optptr + len;
  u8_t type = 0;

  while(optptr < end) {
    switch(*optptr) {
    case DHCP_OPTION_SUBNET_MASK:
      memcpy(s.netmask, optptr + 2, 4);
      break;
    case DHCP_OPTION_ROUTER:
      memcpy(s.default_router, optptr + 2, 4);
      break;
    case DHCP_OPTION_DNS_SERVER:
      memcpy(s.dnsaddr, optptr + 2, 4);
      break;
    case DHCP_OPTION_MSG_TYPE:
      type = *(optptr + 2);
      break;
    case DHCP_OPTION_SERVER_ID:
      memcpy(s.serverid, optptr + 2, 4);
      break;
    case DHCP_OPTION_LEASE_TIME:
      memcpy(s.lease_time, optptr + 2, 4);
      break;
    case DHCP_OPTION_END:
      return type;
    }

    optptr += optptr[1] + 2;
  }
  return type;
}
/*---------------------------------------------------------------------------*/
static u8_t
parse_msg(void) XIP_ATTRIBUTE(".xipsec1");

static u8_t
parse_msg(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  
  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    memcpy(s.ipaddr, m->yiaddr, 4);
    return parse_options(&m->options[4], uip_datalen());
  }
  return 0;
}

/*---------------------------------------------------------------------------*/

void 
handle_dhcp(void)
{
    if (overlayload(10))
    {
        _handle_dhcp();
        overlayunload(10);
    }
}

void 
_handle_dhcp(void)
{
	switch (s.state) {
	case STATE_INITIAL:
		s.state = STATE_SEND_DIS;
		s.ticks = CLOCK_SECOND;
		send_discover();
		timer_set(&s.timer, s.ticks);
		break;
	case STATE_SEND_DIS:
		if(uip_newdata() && parse_msg() == DHCPOFFER) {
			printf("dhcp RX offer\n");
			s.state = STATE_OFFER_RECEIVED;
		} else if (timer_expired(&s.timer)) {
			send_discover();
			if(s.ticks < CLOCK_SECOND * 30) {
				s.ticks *= 2;
			} else {
				//IoT_Cmd_LinkDown(REASON_DEAUTH_STA_LEAVING);
				ws_got_ip_fail();
				break;
			}
			timer_set(&s.timer, s.ticks);
		}
		break;
	case STATE_OFFER_RECEIVED:
		s.state = STATE_SEND_REQ;
		s.ticks = CLOCK_SECOND;
		send_request();
		timer_set(&s.timer, s.ticks);
		break;
	case STATE_SEND_REQ:
		if(uip_newdata() && parse_msg() == DHCPACK) {
			printf("dhcp RX ACK\n");
			s.state = STATE_CONFIG_RECEIVED;
			Printf_High("Got IP:%d.%d.%d.%d\n",
				uip_ipaddr1(s.ipaddr), uip_ipaddr2(s.ipaddr),
				uip_ipaddr3(s.ipaddr), uip_ipaddr4(s.ipaddr));
			printf("Got netmask %d.%d.%d.%d\n",
				uip_ipaddr1(s.netmask), uip_ipaddr2(s.netmask),
				uip_ipaddr3(s.netmask), uip_ipaddr4(s.netmask));
			printf("Got DNS server %d.%d.%d.%d\n",
				uip_ipaddr1(s.dnsaddr), uip_ipaddr2(s.dnsaddr),
				uip_ipaddr3(s.dnsaddr), uip_ipaddr4(s.dnsaddr));
			printf("Got default router %d.%d.%d.%d\n",
				uip_ipaddr1(s.default_router), uip_ipaddr2(s.default_router),
				uip_ipaddr3(s.default_router), uip_ipaddr4(s.default_router));

      Printf_High("@$IP=%d.%d.%d.%d\r\n",
        uip_ipaddr1(s.ipaddr), uip_ipaddr2(s.ipaddr),
        uip_ipaddr3(s.ipaddr), uip_ipaddr4(s.ipaddr));

			dhcpc_configured(&s);
			
			ws_got_ip();
			
		} else if (timer_expired(&s.timer)) {
			send_request();
			if(s.ticks < CLOCK_SECOND * 10) {
				s.ticks += CLOCK_SECOND;
			} else {
				//IoT_Cmd_LinkDown(REASON_DEAUTH_STA_LEAVING);
				ws_got_ip_fail();
				break;
			}
			timer_set(&s.timer, s.ticks);
		}
		break;
	case STATE_CONFIG_RECEIVED:
		break;
	}
	return;
}

/*---------------------------------------------------------------------------*/
void
dhcpc_init(const void *mac_addr, int mac_len) 
{
	uip_ipaddr_t addr={0};

	s.mac_addr = mac_addr;
	s.mac_len  = mac_len;

	//uip_ipaddr(addr, 0,0,0,0);
#ifndef CONFIG_SOFTAP
  	uip_sethostaddr(addr);
#endif
  	//uip_setnetmask(addr);
  	//uip_setdraddr(addr);

	s.state = STATE_INITIAL;
	uip_ipaddr(addr, 255,255,255,255);
	s.conn = uip_udp_new(&addr, HTONS(DHCPC_SERVER_PORT));
	if(s.conn != NULL) {
		uip_udp_bind(s.conn, HTONS(DHCPC_CLIENT_PORT));
	}
}
/*---------------------------------------------------------------------------*/
void
dhcpc_appcall(void)
{
	handle_dhcp();
}
/*---------------------------------------------------------------------------*/
void
dhcpc_request(void)
{
  u16_t ipaddr[2];
  
  if(s.state == STATE_INITIAL) {
    uip_ipaddr(ipaddr, 0,0,0,0);
    uip_sethostaddr(ipaddr);
    /*    handle_dhcp(PROCESS_EVENT_NONE, NULL);*/
  }
}
/*---------------------------------------------------------------------------*/
void dhcpc_set_state(u8_t state)
{
	s.state = state;
}



/*========================================================================
	Routine	Description:
		wifi already got IP
		
	Arguments:
		
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_got_ip(VOID)
{
#ifndef CONFIG_SOFTAP
	/*if connect with smart connection,  store the setting to flash*/
	if (gSmnted){
		store_sta_cfg();
		gSmnted = FALSE;
	}
	
	/*set to default value,  whether go to smnt or not, it should be judged by Flash STA cfg content*/
	pIoTMlme->ATSetSmnt = FALSE;
	
	if (IoTCustOp.IoTCustWifiSMConnect != NULL)
		IoTCustOp.IoTCustWifiSMConnect();
#endif
}

/*========================================================================
	Routine	Description:
		wifi can not got IP
		
	Arguments:
		
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_got_ip_fail(VOID)
{
#ifndef CONFIG_SOFTAP
	printf("ws_got_ip_fail \n");
	
	setFlagVfyInit(TRUE);

	IoT_Cmd_LinkDown(REASON_DEAUTH_STA_LEAVING);
#endif
}

