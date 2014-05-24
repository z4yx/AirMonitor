/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  程序入口
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
#include "usart.h"
#include "systick.h"
#include "pwmOutput.h"
#include "spi.h"
#include "adc.h"
#include "sht1x.h"
#include "gp2y1010.h"
#include "network.h"
#include "yeelink.h"
#include "calendar.h"
#include <stdlib.h>
#include <string.h>

const Task_t SystemTasks[] = { Network_Task, Yeelink_Task };

enum {STATE_INIT, STATE_ERR, STATE_SYNC, STATE_REPORT, STATE_REPORT_WAIT, STATE_POWEROFF, STATE_WAIT};

static void periphInit(void)
{
	SHT1x_Config();
	GP2Y1010_Init();
	Network_Init();
}

//核心组件初始化,包括串口(用于打印调试信息)
static void coreInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	SystemCoreClockUpdate();
	SysTick_Init();
	USART_Config();
}

static void measure_and_report()
{
	const char * datetime = Calendar_GetISO8601();

	int air = GP2Y1010_Measure();
	DBG_MSG("Air: %d", air);

	float temp = 0, humi = 0;
	char str[10];
	if(SHT1x_MeasureTemp(&temp) == 0){
		sprintf(str, "%.2f℃", temp);
		DBG_MSG("temp: %s", str);

		if(SHT1x_MeasureHumi(&humi, temp) == 0){
			sprintf(str, "%.2f%%", humi);
			DBG_MSG("humi: %s", str);
		}
	}

	Yeelink_Send(datetime, air, temp, humi);
}

int main(void)
{
	RCC_ClocksTypeDef clocks;
	// useHSIClock();
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_GetClocksFreq(&clocks);

	coreInit();

	Delay_ms(1000);

	DBG_MSG("\r\n\r\n", 0);
	DBG_MSG("Clock Source: %d", RCC_GetSYSCLKSource());
	DBG_MSG("SYSCLK: %d, H: %d, P1: %d, P2: %d",
		clocks.SYSCLK_Frequency,
		clocks.HCLK_Frequency,
		clocks.PCLK1_Frequency,
		clocks.PCLK2_Frequency);

	periphInit();

	Yeelink_Init();
	Calendar_Init();

	int state = STATE_INIT;
	SysTick_t timeout = 0;
	while (1)
	{

		//运行系统中声明的任务
		for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
			(SystemTasks[i])();

		switch(state)
		{
			case STATE_INIT:
				Calendar_Sync();
				state = STATE_SYNC;
				timeout = GetSystemTick();
				break;
			case STATE_SYNC:
				if(Calendar_DateTimeSet())
					state = STATE_REPORT;
				else if(GetSystemTick() - timeout > DATE_SYNC_TIMEOUT){
					ERR_MSG("Date sync timed out", 0);
					state = STATE_POWEROFF;
				}
				break;
			case STATE_REPORT:
				measure_and_report();
				timeout = GetSystemTick();
				state = STATE_REPORT_WAIT;
				break;
			case STATE_REPORT_WAIT:
				if(Yeelink_Idle())
					state = STATE_POWEROFF;
				else if(GetSystemTick() - timeout > REPORT_TIMEOUT){
					ERR_MSG("Report timed out", 0);
					state = STATE_POWEROFF;
				}
				break;
			case STATE_POWEROFF:
				DBG_MSG("Preparing to power off...", 0);
				GP2Y1010_Poweroff();

				state = STATE_WAIT;
				break;
			case STATE_WAIT:
				Delay_ms(REPORT_INTERVAL);
				NVIC_SystemReset();
				break;
		}

	}
}

#ifdef  USE_FULL_ASSERT

/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}

#endif
