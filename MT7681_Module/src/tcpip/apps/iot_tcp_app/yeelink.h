#ifndef __YEELINK_H___
#define	__YEELINK_H___

void Yeelink_Init(void);
bool Yeelink_Idle(void);
int Yeelink_GetState(void);
void Yeelink_ReportState(void);
int Yeelink_Send(const char *datetime, const char * air, const char * temp, const char * humi);
void Yeelink_Task(void);

#endif
