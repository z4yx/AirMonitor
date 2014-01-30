#include "resolv.h"
#include "ntp.h"

void udp_appcall(void)
{
	resolv_appcall();
	ntp_appcall();
}