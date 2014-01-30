/*
 * =====================================================================================
 *
 *       Filename:  ntp.c
 *
 *    Description:  通过ntp协议获取标准时间
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
#include "ntp.h"
#include "systick.h"
#include "common.h"

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

uint32_t ntohl(uint32_t __x)
{
    return  ((uint32_t)( (((uint32_t)(__x) & (uint32_t)0x000000ffUL) << 24) |
            (((uint32_t)(__x) & (uint32_t)0x0000ff00UL) <<  8) |
            (((uint32_t)(__x) & (uint32_t)0x00ff0000UL) >>  8) |
            (((uint32_t)(__x) & (uint32_t)0xff000000UL) >> 24) ));
}

#define htonl ntohl

struct NTPPacket
{
    uint8_t     li_vn_mode;
    uint8_t     stratum;
    uint8_t     poll;
    uint8_t     precision;
    uint32_t   root_delay;
    uint32_t   root_dispersion;
    uint32_t   reference_identifier;
    uint32_t   reftimestamphigh;
    uint32_t   reftimestamplow;
    uint32_t   oritimestamphigh;
    uint32_t   oritimestamplow;
    uint32_t   recvtimestamphigh;
    uint32_t   recvtimestamplow;
    uint32_t   trantimestamphigh;
    uint32_t   trantimestamplow;
};

struct NTPClientState
{
    uint32_t client_send_time;
    int8_t timer, retries;
};

#define NTPPort     123
#define From00to70  2208988800ll
#define MAX_RETRIES 8
#define TMR_INITIAL 70

static struct uip_udp_conn *ntp_conn;
static struct NTPClientState s;

static void ntp_packet_init(struct NTPPacket *p)
{
    memset(p, 0, sizeof(struct NTPPacket));
    p->li_vn_mode = 0x1b; //0|(3<<2)|(3<<5);
    p->stratum = 1;
}

int ntp_query(const char *host)
{
    uip_ipaddr_t *ipaddr;
    uip_ipaddr_t addr;

    /* First check if the host is an IP address. */
    ipaddr = &addr;
    if (uiplib_ipaddrconv(host, (unsigned char *)addr) == 0)
    {
        ipaddr = (uip_ipaddr_t *)resolv_lookup(host);

        if (ipaddr == NULL)
        {
            return 0;
        }
    }

    if(ntp_conn)
        uip_udp_remove(ntp_conn);

    ntp_conn = uip_udp_new(ipaddr, HTONS(NTPPort));

    s.retries = MAX_RETRIES;
    s.timer   = 0;

    DBG_MSG("ntp conn: %d", (int)ntp_conn);

    return 1;
}

static void senddata(void)
{
    struct NTPPacket *packet;
    packet = (struct NTPPacket *)uip_appdata;
    ntp_packet_init(packet);

    s.client_send_time = (uint32_t)(GetSystemTick()/1000);
    packet->oritimestamphigh = htonl(s.client_send_time);

    uip_udp_send(sizeof(struct NTPPacket));

    DBG_MSG("client_send_time: %d", (int)s.client_send_time);
}

static void newdata(void)
{
    struct NTPPacket *packet;
    uint32_t client_recv_time;
    int64_t  delta_time;

    client_recv_time = (uint32_t)(GetSystemTick()/1000);
    DBG_MSG("client_recv_time: %d", (int)client_recv_time);

    packet = (struct NTPPacket *)uip_appdata;

    packet->recvtimestamphigh = ntohl(packet->recvtimestamphigh);
    packet->trantimestamphigh = ntohl(packet->trantimestamphigh);

    delta_time = ((packet->recvtimestamphigh - (int64_t)s.client_send_time)
        + (packet->trantimestamphigh - (int64_t)client_recv_time)) >> 1;

    // convert from ntp epoch to unix epoch
    delta_time -= From00to70;

    ntp_deltatime(delta_time);

    s.retries = 0;
}

void ntp_appcall(void)
{
    if (uip_udp_conn->rport == HTONS(NTPPort))
    {
        if (uip_poll())
        {
            if (s.retries > 0) {
                if(--s.timer <= 0) {
                    senddata();

                    s.retries--;
                    s.timer = TMR_INITIAL;
                }
            }
        }
        if (uip_newdata())
        {
            newdata();
        }
    }
}


void ntp_init(void)
{
    s.retries = 0;
}
