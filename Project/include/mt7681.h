#ifndef MT7681_H__
#define MT7681_H__

#include "common.h"

void MT7681_Init(void);
void MT7681_Yeelink_Send(const char *datetime, int air, float temp, float humi);
bool MT7681_Yeelink_Idle(void);
void MT7681_ntp_query(const char *host);
void MT7681_USART_Interrupt(void);

#endif
