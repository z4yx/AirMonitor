#include "iot_tcp_app.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
#include "string.h"
#include "webclient.h"

extern IOT_ADAPTER   	IoTpAd;

/*---------------------------------------------------------------------------*/
/*
 * The initialization function. We must explicitly call this function
 * from the system initialization code, some time after uip_init() is
 * called.
 */
void
iot_tcp_app_init(void)
{
  /* We start to listen for connections on TCP port 7681. */
  // uip_listen(HTONS(IoTpAd.ComCfg.Local_TCP_Srv_Port));
  webclient_init();
#if CFG_SUPPORT_TCPIP_ROBUST_TEST
  uip_listen(HTONS(7684));
#endif

}

/*---------------------------------------------------------------------------*/
/*
 * In mt76xx_tcp_app.h we have defined the UIP_APPCALL macro to
 * mt7681_tcp_appcall so that this funcion is uIP's application
 * function. This function is called whenever an uIP event occurs
 * (e.g. when a new connection is established, new data arrives, sent
 * data is acknowledged, data needs to be retransmitted, etc.).
 */
void
iot_tcp_appcall(void)
{
	/*
	* The uip_conn structure has a field called "appstate" that holds
	* the application state of the connection. We make a pointer to
	* this to access it easier.
	*/
	struct iot_tcp_app_state *s = &(uip_conn->appstate);
	u16_t lport = HTONS(uip_conn->lport);

#if UIP_HTTP_CLIENT_SUPPORT
	if(HTONS(uip_conn->rport) == HTTP_SERVER_DEFAULT_PORT){
	  webclient_appcall();
	  return;
	}	
#endif

	if (uip_aborted() || uip_timedout() || uip_closed()) {
		printf("fd %d uip_aborted.%d\n", uip_conn->fd, HTONS(uip_conn->lport));

		s->state = IOT_APP_S_CLOSED;
		s->buf = NULL;
		s->len = 0;
		uip_abort();
	}

	if (uip_connected()) {
	}

	if (uip_acked()) {
		printf("uip_acked.\n");
		s->state = IOT_APP_S_DATA_ACKED;
		s->buf = NULL;
		s->len = 0;
	}

	if (uip_newdata()) {
		printf("RX fd : %d\n", uip_conn->fd);
		if (lport == IoTpAd.ComCfg.Local_TCP_Srv_Port || lport==7682) {

#if CFG_SUPPORT_TCPIP_ROBUST_TEST
		} else if (lport==7684) {
			uip_send(uip_appdata, uip_datalen());
#endif
		} else {
#if ATCMD_TCPIP_SUPPORT
			IoT_uart_output(uip_appdata, uip_datalen());
#endif
		}
	}
#if 0
	if (uip_rexmit()) {
		printf("uip_rexmit. %d\n", uip_conn->fd);
		if (s->state == IOT_APP_S_DATA_SEND)
			uip_send(s->buf, s->len);
	}
#endif
	/* check if we have data to xmit for this connection.*/
	if (uip_poll()) {
#if ATCMD_TCPIP_SUPPORT
		if(s->state == IOT_APP_S_CLOSED) {
			uip_close();
		} else if(s->len > 0) {
			uip_send(s->buf, s->len);
			s->state = IOT_APP_S_DATA_SEND;
		}
#endif
	}
}


