#ifndef __ALARM_H
#define	__ALARM_H

#include "stm32f10x.h"

void Alarm_Config(void);
void Alarm_SetInterrupt(FunctionalState bEnabled);
void Alarm_SetAlarm(uint32_t value);
uint32_t Alarm_ReadCounter(void);

#endif
