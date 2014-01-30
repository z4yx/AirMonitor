#ifndef __YEELINK_H___
#define	__YEELINK_H___

void Yeelink_Init(void);
int Yeelink_Send(const char *datetime, int air, float temp, float humi);

#endif
