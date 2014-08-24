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

enum {STATE_IDLE, STATE_DNS, STATE_AIR, STATE_TEMP, STATE_HUMI};
static int current_state;

static bool net_waiting;

static struct report_values {
	const char *datetime;
	int air;
	float temp, humi;
}values;

static char dat_buf[128];

const char* build_json(float val)
{
	sprintf(dat_buf, "{\"timestamp\":\"%s\",\"value\":%f}", values.datetime, val);
	return dat_buf;
}

void Yeelink_Init()
{

	current_state = STATE_IDLE;
}

int Yeelink_Send(const char *datetime, int air, float temp, float humi)
{
	if(current_state != STATE_IDLE)
		return -1;

	values.datetime = datetime;
	values.air  = air;
	values.temp = temp;
	values.humi = humi;

	current_state = STATE_DNS;

	if(NULL == resolv_lookup(YEELINK_HOSTNAME)){
		net_waiting = true;
		resolv_query(YEELINK_HOSTNAME);
	}else{
		net_waiting = false;
	}

	return 0;
}

bool Yeelink_Idle(void)
{
	return STATE_IDLE == current_state;
}

void Yeelink_Task(void)
{
	if(net_waiting)
		return;

	switch(current_state)
	{
		case STATE_DNS:
			current_state = STATE_AIR;
			net_waiting = true;
			webclient_get(YEELINK_HOSTNAME, 80, YEELINK_AIR_SENSOR, build_json(values.air));
			break;
		case STATE_AIR:
			current_state = STATE_TEMP;
			net_waiting = true;
			webclient_get(YEELINK_HOSTNAME, 80, YEELINK_TEMP_SENSOR, build_json(values.temp));
			break;
		case STATE_TEMP:
			current_state = STATE_HUMI;
			net_waiting = true;
			webclient_get(YEELINK_HOSTNAME, 80, YEELINK_HUMI_SENSOR, build_json(values.humi));
			break;
		case STATE_HUMI:
			current_state = STATE_IDLE;
			break;
	}
}

void webclient_statehandler(u16_t code)
{
	LOG_D("response code: %d", (int)code);
	if(code != 200){
		webclient_close();
	}
}

void webclient_datahandler(char *data, u16_t len)
{
	LOG_D("len=%d",(int)len);
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
		LOG_D("%s", dat_buf);
	}
	if(len == 0)
		webclient_close();
}

void resolv_found(char *name, u16_t *ipaddr)
{
	if(name && 0==strcmp(name, YEELINK_HOSTNAME)) {
		LOG_D("name:%s, ip:%d", name, (int)*ipaddr);
		if(current_state == STATE_DNS)
			net_waiting = false;
	}
}

void webclient_aborted(void)
{
	LOG_D("aborted", 0);
	net_waiting = false;
}

void webclient_closed(void)
{
	LOG_D("closed", 0);
	net_waiting = false;
}

void webclient_timedout(void)
{
	LOG_D("timedout", 0);
	net_waiting = false;
}

void webclient_connected(void)
{
	LOG_D("connected", 0);

}
