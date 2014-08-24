
#ifndef __IOT_UDP_APP_H__
#define __IOT_UDP_APP_H__

#include "uipopt.h"

struct iot_udp_app_state {
	u8_t fd;
	u8_t new_data;
	u16_t len;
	u8_t *buf;
	u8_t *ripaddr;
	u16_t rport;
	u8_t state;
};

void iot_udp_app_init(void);
void iot_udp_appcall(void);
void handle_app_data(void);


typedef struct iot_udp_app_state uip_udp_appstate_t;
#define UIP_UDP_APPCALL iot_udp_appcall

#endif
