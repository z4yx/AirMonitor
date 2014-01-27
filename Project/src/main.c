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
#include "adc.h"
#include "sht1x.h"
#include "gp2y1010.h"
#include <stdlib.h>
#include <string.h>

// const Task_t SystemTasks[] = { Network_Task };


static void periphInit(void)
{
	SHT1x_Config();
	GP2Y1010_Init();
}

void useHSIClock(void)
{
	// RCC_HSEConfig(RCC_HSE_ON);
	// while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)//等待HSE使能成功
	// {
	// }
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);//等待HSI使能成功

	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_10);

	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
}

//核心组件初始化,包括串口(用于打印调试信息)
static void coreInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	SystemCoreClockUpdate();
	SysTick_Init();
	USART_Config();
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

	SysTick_t last_report = 0;
	while (1)
	{

		//运行系统中声明的任务
		// for(int i = 0; i < sizeof(SystemTasks)/sizeof(Task_t); i++)
		// 	(SystemTasks[i])();

		SysTick_t now = GetSystemTick();
		if(now - last_report > 1000){
			last_report = now;

			int air = GP2Y1010_Measure();
			DBG_MSG("Air: %d", air);
			
			float temp, humi;
			char str[16];
			if(SHT1x_MeasureTemp(&temp) == 0){
				sprintf(str, "%.2f℃", temp);
				DBG_MSG("temp: %s", str);

				if(SHT1x_MeasureHumi(&humi, temp) == 0){
					sprintf(str, "%.2f%%", humi);
					DBG_MSG("humi: %s", str);
				}
			}else{
				DBG_MSG("failed", 0);
			}
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
