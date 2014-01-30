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
#include <string.h>

const char *http_appkey = YEELINK_APPKEY_HDR;

enum {STATE_IDLE, STATE_DNS_SENT, STATE_DNS_OK, STATE_HTTP_SENT};
static int current_state;

static char dat_buf[128];

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

	if(NULL == resolv_lookup(YEELINK_HOSTNAME)){
	
		current_state = STATE_DNS_SENT;
		resolv_query(YEELINK_HOSTNAME);
	}else{
		current_state = STATE_DNS_OK;
	}

	return 0;
}

void Yeelink_Task(void)
{
	uint8_t ret;
	static int test_day = 17, test_temp = 2;
	switch(current_state)
	{
		case STATE_DNS_OK:
			current_state = STATE_HTTP_SENT;
			sprintf(dat_buf, "{\"timestamp\":\"2014-09-%dT16:13:14\",\"value\":%d}", test_day, test_temp);
			test_day++;
			test_temp++;
			// ret = webclient_get("192.168.1.30", 1234, YEELINK_TEMP_SENSOR, "{\"timestamp\":\"2014-01-15T16:13:14\",\"value\":21.34}");
			ret = webclient_get(YEELINK_HOSTNAME, 80, YEELINK_TEMP_SENSOR, dat_buf);
			DBG_MSG("webclient_get()=%d", (int)ret);
			break;
	}
}

void webclient_statehandler(u16_t code)
{
	DBG_MSG("response code: %d", (int)code);
	if(code != 200){
		webclient_close();
		current_state = STATE_IDLE;
	}
}

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
	if(current_state != STATE_HTTP_SENT)
		return;
	if(len == 0){
		webclient_close();
		current_state = STATE_IDLE;
	}
}

void resolv_found(char *name, u16_t *ipaddr)
{
	if(name && 0==strcmp(name, YEELINK_HOSTNAME)) {
		DBG_MSG("name:%s, ip:%d", name, (int)*ipaddr);
		if(current_state == STATE_DNS_SENT)
			current_state = STATE_DNS_OK;
	}
}

void webclient_aborted(void)
{
	DBG_MSG("aborted", 0);
	current_state = STATE_IDLE;
}

void webclient_closed(void)
{
	DBG_MSG("closed", 0);
	current_state = STATE_IDLE;
}

void webclient_timedout(void)
{
	DBG_MSG("timedout", 0);
	current_state = STATE_IDLE;
}

void webclient_connected(void)
{
	DBG_MSG("connected", 0);

}
