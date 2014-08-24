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
#include "../../uip/uip.h"
#include "dhcpd.h"

#define UIP_DHCPD_CONNS	5

struct uip_dhcpd_conn uip_dhpcd_conns[UIP_DHCPD_CONNS];

static  UINT32 uip_softap_ipaddr = (192<<24)|(168<<16)|(81<<8)|(1);	//host sequence
static  UINT32 uip_softap_netmask = (255<<24)|(255<<16)|(255<<8)|(0);	//host sequence
static  UINT32 uip_softap_leasetime = 1800;	//30min


/*---------------------------------------------------------------------------*/
static u8_t *
add_msg_type(u8_t *optptr, u8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}

/*---------------------------------------------------------------------------*/

UINT32  
ntohl(UINT32 x)
{
    return (((x & 0x000000FF) << 24) |
            ((x & 0x0000FF00) <<  8) |
            ((x & 0x00FF0000) >>  8) |
            ((x & 0xFF000000) >> 24));
}


unsigned char *dhcpd_pickup_opt(struct dhcpd *packet, int code, int dest_len, void *dest)
{
	int i, length;
	unsigned char *ptr;
	int over = 0, done = 0, curr = OPTION_FIELD;
	unsigned char len;
	
	ptr = packet->options;
	i = 0;
	length = 308;
	while (!done) 
	{
		if (i >= length) 
		{
			printf( "Option fields too long.\n");
			return 0;
		}
		
		if (ptr[i + OPT_CODE] == code) 
		{
			if (i + 1 + ptr[i + OPT_LEN] >= length) 
				return 0;
			
			if (dest)
			{ 
				len = ptr[i + OPT_LEN];
				if (len > dest_len)
				{
					printf( "Option fields too long to fit in dest.\n");
					return 0;
				}
				
				memcpy(dest, &ptr[i + OPT_DATA], (int)len);
			}
			
			return &ptr[i + OPT_DATA];
		}
		
		switch (ptr[i + OPT_CODE]) 
		{
		case DHCP_PADDING:
			i++;
			break;
			
		case DHCP_OPTION_OVER:
			if (i + 1 + ptr[i + OPT_LEN] >= length)
				return 0;
			
			over = ptr[i + 3];
			i += ptr[OPT_LEN] + 2;
			break;
			
		case DHCP_OPTION_END:
			if (curr == OPTION_FIELD && over & FILE_FIELD) 
			{
				ptr = packet->file;
				i = 0;
				length = 128;
				curr = FILE_FIELD;
			} 
			else if (curr == FILE_FIELD && over & SNAME_FIELD) 
			{
				ptr = packet->sname;
				i = 0;
				length = 64;
				curr = SNAME_FIELD;
			}
			else
				done = 1;
			break;
			
		default:
			i += ptr[OPT_LEN + i] + 2;
		}
	}
	
	return 0;
}

static int discover()
{
	struct dhcpd *oldpacket = (struct dhcpd *)uip_appdata;
	struct dhcpd packet;
	
	in_addr addr;
	u8_t i,j=0;
	unsigned char* optptr = NULL;
	UINT32 temp;

	printf("recive discover.\n");
	/* block all 0 and all 0xff mac address */
	if (memcmp(oldpacket->chaddr, "\x00\x00\x00\x00\x00\x00", 6) == 0 ||
		memcmp(oldpacket->chaddr, "\xff\xff\xff\xff\xff\xff", 6) == 0)
	{
		printf("block all 0 and all 0xff mac address\n");
		return -1;
	}
	
 	memcpy(&packet,oldpacket,sizeof(struct dhcpd)-308);//308 is the size of options.
	memset(packet.options,0,308);
	
	packet.op = DHCP_REPLY;

	//find cached mac and ip first.
	for(i=0; i<UIP_DHCPD_CONNS; i++)
	{
		if(memcmp(uip_dhpcd_conns[i].chaddr,oldpacket->chaddr,6) == 0)
		{
			addr = uip_dhpcd_conns[i].yiaddr;
			break;
		}
	}
	//allocate ip.
	if(i==UIP_DHCPD_CONNS)
	{
		for(j=0; j<UIP_DHCPD_CONNS; j++)
		{
			if(uip_dhpcd_conns[j].flag == 0 )
			{
				addr = ntohl(uip_softap_ipaddr + j + 1);				
				break;
			}
		}
	}
	Printf_High("i = %d, j = %d\n", i, j);
	Printf_High("client ip addr: %d.%d.%d.%d\n",
		addr&0x000000ff,(addr&0x0000ff00)>>8,
		(addr&0x00ff0000)>>16,(addr&0xff000000)>>24);
	
	if(j==UIP_DHCPD_CONNS)
	{
		Printf_High("no more ipaddr free.\n");
		return	-1;
	}
	
	memcpy(&packet.yiaddr,&addr,sizeof(addr));
	
	optptr = packet.options;
	//add msg type
	optptr=add_msg_type(optptr,DHCPOFFER);
	//add server id
	*optptr++ = DHCP_OPTION_SERVER_ID;
	*optptr++ = 4;
	temp=ntohl(uip_softap_ipaddr);
	optptr = memcpy(optptr, &temp, 4);

	//add default router
	*optptr++ = DHCP_OPTION_ROUTER;
	*optptr++ = 4;
	temp=ntohl(uip_softap_ipaddr);
	optptr = memcpy(optptr, &temp, 4);

	//add dns server
	*optptr++ = DHCP_OPTION_DNS_SERVER;
	*optptr++ = 4;
	temp=ntohl(uip_softap_ipaddr);
	optptr = memcpy(optptr, &temp, 4);

	//add domain name
//	*optptr++ = DHCP_OPTION_DOMAIN_NAME;
//	*optptr++ = 8;
//	temp=ntohl(uip_softap_ipaddr);
//	optptr = memcpy(optptr, "mt7681", 8);
	
	//optptr = optptr + 4;
	//add lease time
	*optptr++ = DHCP_OPTION_LEASE_TIME;
	*optptr++ = 4;
	temp=ntohl(uip_softap_leasetime);
	optptr = memcpy(optptr, &temp, 4);
	//optptr = optptr + 4;	
	//add DHCP_SUBNET
	*optptr++ = DHCP_OPTION_SUBNET_MASK;
	*optptr++ = 4;
	temp=ntohl(uip_softap_netmask);
	optptr = memcpy(optptr, &temp, 4);
	//optptr = optptr + 4;
	//add end
	*optptr++ = DHCP_OPTION_END;
	
	printf("Send. [%d] data\n", sizeof(struct dhcpd)-308+(optptr-(unsigned char* )packet.options));
	uip_send(&packet,sizeof(struct dhcpd)-308+(optptr-(unsigned char* )packet.options));

	return 1;
	
}


static int request()
{
	struct dhcpd *oldpacket = (struct dhcpd *)uip_appdata;
	unsigned char *requested;
	unsigned char *server_id;
	
	in_addr req_ip = {0};
	in_addr server = {0};
	in_addr addr;
	struct dhcpd packet;
	u8_t i,j=0;
	unsigned char* optptr = NULL;
	UINT32 temp;
	
	printf("Recive REQUEST\n");
	
	requested = dhcpd_pickup_opt(oldpacket, DHCP_OPTION_REQ_IPADDR, sizeof(req_ip), &req_ip);
	server_id = dhcpd_pickup_opt(oldpacket, DHCP_OPTION_SERVER_ID, sizeof(server), &server);
	
	if (server_id && server != ntohl(uip_softap_ipaddr))
	{
		printf("Not my server ID\n");
		return -1;
	}

 	memcpy(&packet,oldpacket,sizeof(struct dhcpd)-308);//308 is the size of options.
	memset(packet.options,0,308);
	
	packet.op=DHCP_REPLY;

	//find cached mac and ip first.
	for(i=0; i<UIP_DHCPD_CONNS; i++)
	{
		if(memcmp(uip_dhpcd_conns[i].chaddr,oldpacket->chaddr,6)==0 )
		{
			addr = uip_dhpcd_conns[i].yiaddr;
			//uip_dhpcd_conns[i].flag =1;
			//memcpy(packet.yiaddr,addr,sizeof(addr));
			break;
		}
	}
	//allocate ip.
	if(i==UIP_DHCPD_CONNS)
	{
		for(j=0; j<UIP_DHCPD_CONNS; j++)
		{
			if(uip_dhpcd_conns[j].flag == 0 )
			{
				addr = ntohl(uip_softap_ipaddr + j + 1);
				//uip_dhpcd_conns[j].flag = 1;
				//uip_dhpcd_conns[j].yiaddr = addr;
				//uip_dhpcd_conns[j].chaddr = oldpacket->chaddr;
				//memcpy(packet.yiaddr,ntohl(addr),sizeof(addr));
				break;
			}
		}
	}
	//protect has been done in discover phase,so cancel here??
	
	if(j==UIP_DHCPD_CONNS)
	{
		printf("no more ipaddr free.\n");
		return -1;
	}

	optptr = packet.options;

	if(memcmp(&req_ip,&addr,sizeof(addr)) != 0)
	{
		//add msg type
		optptr=add_msg_type(optptr,DHCPNAK);
		//add server id
		*optptr++ = DHCP_OPTION_SERVER_ID;
		*optptr++ = 4;
		temp=ntohl(uip_softap_ipaddr);
		optptr=memcpy(optptr, &temp, 4);
		//optptr = optptr + 4;	
		memset(&packet.yiaddr, 0, 4);
	}
	else
	{
		//add msg type
		optptr=add_msg_type(optptr,DHCPACK);
		//add server id
		*optptr++ = DHCP_OPTION_SERVER_ID;
		*optptr++ = 4;
		temp=ntohl(uip_softap_ipaddr);
		optptr = memcpy(optptr, &temp, 4);
		//optptr = optptr + 4;
		//add lease time
		*optptr++ = DHCP_OPTION_LEASE_TIME;
		*optptr++ = 4;
		temp=ntohl(uip_softap_leasetime);
		optptr = memcpy(optptr, &temp, 4);
		//optptr = optptr + 4;	
		//add DHCP_SUBNET
		*optptr++ = DHCP_OPTION_SUBNET_MASK;
		*optptr++ = 4;
		temp=ntohl(uip_softap_netmask);
		optptr = memcpy(optptr, &temp, 4);
		//optptr = optptr + 4;
		
		//add default router
		*optptr++ = DHCP_OPTION_ROUTER;
		*optptr++ = 4;
		temp=ntohl(uip_softap_ipaddr);
		optptr = memcpy(optptr, &temp, 4);
	
		//add dns server
		*optptr++ = DHCP_OPTION_DNS_SERVER;
		*optptr++ = 4;
		temp=ntohl(uip_softap_ipaddr);
		optptr = memcpy(optptr, &temp, 4);
	
		//add domain name
//		*optptr++ = DHCP_OPTION_DOMAIN_NAME;
//		*optptr++ = 8;
	//	temp=ntohl(uip_softap_ipaddr);
//		optptr = memcpy(optptr, "mt7681", 8);

		//record cllocated ip addr.
		uip_dhpcd_conns[j].flag = 1;
		uip_dhpcd_conns[j].yiaddr = addr;
		//uip_dhpcd_conns[j].chaddr = oldpacket->chaddr;
		memcpy(uip_dhpcd_conns[j].chaddr,oldpacket->chaddr,sizeof(uip_dhpcd_conns[j].chaddr));
		
		timer_set(&uip_dhpcd_conns[j].timer, (uip_softap_leasetime + 300)*CLOCK_SECOND);//start lease time timer, add 300s margin.
	}


	//add end
	*optptr++ = DHCP_OPTION_END;

	memcpy(&packet.yiaddr,&addr,sizeof(addr));

	//send NAK or ACK
	uip_send(&packet,sizeof(struct dhcpd)-308+(optptr-(unsigned char* )packet.options));
	
	return 1;
}


/*---------------------------------------------------------------------------*/
void 
handle_dhcpd(void)
{
	struct dhcpd *oldpacket = (struct dhcpd *)uip_appdata;
	unsigned char *msg_type = NULL;
	unsigned char state;
	u8_t i=0;

	if(uip_newdata())
	{
		msg_type = dhcpd_pickup_opt(oldpacket, DHCP_OPTION_MSG_TYPE, sizeof(state), &state);

		if (msg_type)
		{
			switch (state) 
			{
			case DHCPDISCOVER:
				discover();
				break;
			case DHCPREQUEST:
				request();
				break;		
			default:
				break;
			}
		}
	}
	else
	{
		for(i=0; i<UIP_DHCPD_CONNS; i++)
		{
			if((uip_dhpcd_conns[i].flag ==1) && timer_expired(&uip_dhpcd_conns[i].timer))
			{
				uip_dhpcd_conns[i].flag = 0;
			}	
		}	
	}
	return;
}

/*---------------------------------------------------------------------------*/
void
dhcpd_init()
{
	UIP_UDP_CONN *udp_conn = NULL;
	uip_ipaddr_t Remoteaddr={0};
//	uip_ipaddr_t uip_hostaddr =
//  	{HTONS((UIP_IPADDR0 << 8) | UIP_IPADDR1),
//  	 HTONS((UIP_IPADDR2 << 8) | UIP_IPADDR3)};
	
	uip_softap_ipaddr = ntohl(uip_hostaddr[0]|(uip_hostaddr[1]<<16));
	uip_softap_netmask = ntohl(uip_netmask[0]|(uip_netmask[1]<<16));

//	Printf_High("dhcp server:%d.%d.%d.%d\n",
//		(uip_softap_ipaddr&0xff000000)>>24,(uip_softap_ipaddr&0x00ff0000)>>16,
//		(uip_softap_ipaddr&0x0000ff00)>>8,uip_softap_ipaddr&0x000000ff);
	
	memset(uip_dhpcd_conns,0,sizeof(uip_dhpcd_conns));

	uip_ipaddr(Remoteaddr, 255,255,255,255);
	udp_conn = uip_udp_new(Remoteaddr, HTONS(DHCPC_CLIENT_PORT));
	if (udp_conn)
		uip_udp_bind(udp_conn, HTONS(DHCPC_SERVER_PORT));

}
/*---------------------------------------------------------------------------*/
void
dhcpd_appcall(void)
{
	handle_dhcpd();
}
/*---------------------------------------------------------------------------*/

