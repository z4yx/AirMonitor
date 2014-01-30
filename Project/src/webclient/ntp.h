#ifndef _NTP_H__
#define _NTP_H__

#include "uipopt.h"
#include <stdint.h>

int ntp_query(const char *host);

void ntp_appcall(void);

void ntp_init(void);

void ntp_deltatime(int64_t);

#endif
