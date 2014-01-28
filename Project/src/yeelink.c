/*
 * =====================================================================================
 *
 *       Filename:  yeelink.c
 *
 *    Description:  向yeelink上报数据的接口
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
#include "webclient.h"
#include "common.h"

const char *http_appkey = YEELINK_APPKEY_HDR;

enum {STATE_IDLE, STATE_DNS_SENT, STATE_DNS_OK, STATE_HTTP_SENT};
static int current_state;

void Yeelink_Init()
{
	uip_ipaddr_t ipaddr;
	uint8_t ip[4] = ETH_DNS_ADDR;

	uip_ipaddr(ipaddr, ip[0], ip[1], ip[2], ip[3]);

	resolv_init();
	resolv_conf(ipaddr);
	webclient_init();

	current_state = STATE_IDLE;
}

int Yeelink_Send(void)
{
	if(current_state != STATE_IDLE)
		return -1;

	resolv_query(YEELINK_HOSTNAME);

	current_state = STATE_DNS_SENT;

	return 0;
}

void Yeelink_Task(void)
{
	uint8_t ret;
	switch(current_state)
	{
		case STATE_DNS_OK:
			current_state = STATE_HTTP_SENT;
			ret = webclient_get(YEELINK_HOSTNAME, 80, YEELINK_GET_SENSORS);
			DBG_MSG("webclient_get()=%d", (int)ret);
			break;
	}
}

static char dat_buf[128];
void webclient_datahandler(char *data, u16_t len)
{
	DBG_MSG("len=%d",(int)len);
	u16_t i = 0;
	while(i<len){
		if(len - i > 127){
			u16_t j;
			for(j=0; j<127; j++)
				dat_buf[j] = data[i+j];
			dat_buf[j] = 0;
			i+=127;
		}else{
			u16_t j;
			for(j=0; i+j<len; j++)
				dat_buf[j] = data[i+j];
			dat_buf[j] = 0;
			i=len;
		}
		DBG_MSG("%s", dat_buf);
	}
}

void resolv_found(char *name, u16_t *ipaddr)
{
	DBG_MSG("name:%s, ip:%d", name, (int)*ipaddr);
	if(name && 0==strcmp(name, YEELINK_HOSTNAME)) {
		if(current_state == STATE_DNS_SENT)
			current_state = STATE_DNS_OK;
	}
}

void webclient_aborted(void)
{
	current_state = STATE_IDLE;
}

void webclient_closed(void)
{
	current_state = STATE_IDLE;
}

void webclient_timedout(void)
{
	current_state = STATE_IDLE;
}

void webclient_connected(void)
{

}
