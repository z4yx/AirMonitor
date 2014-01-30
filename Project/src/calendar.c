/*
 * =====================================================================================
 *
 *       Filename:  calendar.c
 *
 *    Description:  维护日期时间
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
#include "common.h"
#include "systick.h"

#include <time.h>

#define TIMEZONE         (+8)
#define ISO8601_BUF_SIZE 24

enum {STATE_NONE, STATE_SYNCING, STATE_SYNC_DONE};
static int state;
static int64_t offset;
static char iso8601[ISO8601_BUF_SIZE];

void Calendar_Init(void)
{
	state = STATE_NONE;
	ntp_init();
}

void Calendar_Sync(void)
{
	ntp_query(NTP_SERVER);
	state = STATE_SYNCING;
}

bool Calendar_DateTimeSet(void)
{
	return state == STATE_SYNC_DONE;
}

const char *Calendar_GetISO8601(void)
{
	struct tm *ptm;
	time_t now;

	if(!Calendar_DateTimeSet())
		return NULL;

	now = GetSystemTick()/1000 + offset + TIMEZONE*3600;
	ptm = gmtime(&now);

	if(strftime(iso8601, ISO8601_BUF_SIZE, "%Y-%m-%dT%H:%M:%S", ptm))
		return iso8601;
	else
		return NULL;
}

/* called by ntp module */
void ntp_deltatime(int64_t delte)
{
	offset = delte;
	state = STATE_SYNC_DONE;

	DBG_MSG("delta time: %d", (int)delte);
	DBG_MSG("current time is %s", Calendar_GetISO8601());
}

