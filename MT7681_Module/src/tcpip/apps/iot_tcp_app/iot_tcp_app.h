#ifndef __IOT_TCP_APP__
#define __IOT_TCP_APP__

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"

//#include "psock.h"

#define IOT_APP_S_CLOSED 0
#define IOT_APP_S_DATA_SEND 1
#define IOT_APP_S_DATA_ACKED 2
#define IOT_APP_S_CONNECTED 3
#define IOT_APP_S_WAIT_SEND 4


/* Next, we define the uip_tcp_appstate_t datatype. This is the state
   of our application, and the memory required for this state is
   allocated together with each TCP connection. One application state
   for each TCP connection. */
typedef struct iot_tcp_app_state {
  u8_t state;
  u8_t *buf;
  u16_t len;
} uip_tcp_appstate_t;

/* Finally we define the application function to be called by uIP. */
void iot_tcp_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL iot_tcp_appcall
#endif /* UIP_APPCALL */

void iot_tcp_app_init(void);

#endif
