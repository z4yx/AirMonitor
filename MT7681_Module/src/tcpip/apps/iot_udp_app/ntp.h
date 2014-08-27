#ifndef _NTP_H__
#define _NTP_H__

#include "uipopt.h"
#include "types.h"

typedef int64 int64_t;
typedef uint64 uint64_t;

#define NTPPort     123
#define TIMEZONE    (+8)

int ntp_query(const char *host);

void ntp_appcall(void);

void ntp_init(void);

#endif
