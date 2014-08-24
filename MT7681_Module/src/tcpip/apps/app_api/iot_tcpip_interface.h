#if ATCMD_TCPIP_SUPPORT

#ifndef __IOT_TCPIP_INTERFACE_H__
#define __IOT_TCPIP_INTERFACE_H__
#include "uip.h"
#include "iot_tcp_app.h"
#include "iot_udp_app.h"

int iot_netif_cfg(u8_t *ip, u8_t *mask, u8_t * gw, u8_t *dns);
void iot_getipaddr(u8_t *ip, u8_t *mask, u8_t *gw, u8_t *dns);
int iot_connect(u8_t *dst, u16_t port);
void iot_disconnect(u8_t fd);
int iot_send(u8_t fd, u8_t *buf, u16_t len);
int iot_listen (u16_t port);
int iot_unlisten (u16_t port);
int iot_udp_new(u8_t *rip, u16_t lport, u16_t rport);
void iot_udp_remove(u8_t fd);
int iot_udp_send(u8_t fd, u8_t *buf, u16_t len, u8_t *rip, u16_t rport);

#endif
#endif
