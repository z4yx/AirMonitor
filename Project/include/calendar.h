#ifndef __CALENDAR_H___
#define	__CALENDAR_H___

void Calendar_Init(void);
bool Calendar_DateTimeSet(void);
void Calendar_Sync(void);
const char *Calendar_GetISO8601(void);

#endif
