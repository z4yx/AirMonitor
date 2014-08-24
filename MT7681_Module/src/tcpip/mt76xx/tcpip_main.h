
#ifndef __TCPIP_MAIN_H__
#define __TCPIP_MAIN_H__

#include "uip-conf.h"
#include "xip_ovly.h"


int tcpip_init(void);
int _tcpip_init(void) OVLY_ATTRIBUTE(".ovlysec9");

int netif_rx(u8_t *p, u16_t len);
void tcpip_periodic_timer(void);

#endif

