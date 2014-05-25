/*
 * =====================================================================================
 *
 *       Filename:  alarm.c
 *
 *    Description:  基于RTC模块的闹钟功能
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

#include "stm32f10x.h"
#include "common.h"
#include "alarm.h"

void Alarm_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSI */
    RCC_LSICmd(ENABLE);
    /* Wait till LSI is ready */
    while(!RCC_GetFlagStatus(RCC_FLAG_LSIRDY));

    /* Select LSI as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    // RTC_EnterConfigMode();
    RTC_SetPrescaler(40000);
    // RTC_ExitConfigMode();
    RTC_WaitForLastTask();

    // RTC_EnterConfigMode();
    RTC_SetCounter(0);
    // RTC_ExitConfigMode();
    RTC_WaitForLastTask();
}

void Alarm_SetAlarm(uint32_t value)
{
    // RTC_EnterConfigMode();
    RTC_SetAlarm(value);
    // RTC_ExitConfigMode();
    RTC_WaitForLastTask();
}

void Alarm_SetInterrupt(FunctionalState bEnabled)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
/*
To wakeup from Stop mode with an RTC alarm event, it is necessary to:
● Configure the EXTI Line 17 to be sensitive to rising edge
● Configure the RTC to generate the RTC alarm
To wakeup from Standby mode, there is no need to configure the EXTI Line 17.
*/
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = bEnabled;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn; //指定中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   // 指定响应优先级别
    NVIC_InitStructure.NVIC_IRQChannelCmd = bEnabled;
    NVIC_Init(&NVIC_InitStructure);

    RTC_ITConfig(RTC_IT_ALR, bEnabled);
    RTC_WaitForLastTask();
}


uint32_t Alarm_ReadCounter()
{
    RTC_WaitForSynchro();
    return RTC_GetCounter();
}

void Alarm_Interrupt(void)
{
    DBG_MSG("Reseting...", 0);
    NVIC_SystemReset();
}
