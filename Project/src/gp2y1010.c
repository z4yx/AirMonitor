/*
 * =====================================================================================
 *
 *       Filename:  gp2y1010.c
 *
 *    Description:  GP2Y1010AU0F粉尘传感器控制
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
#include "systick.h"
#include "adc.h"
#include "pwmOutput.h"
#include "gp2y1010.h"

static volatile uint8_t interruptOccur;
static __IO uint16_t adc_results[2];

void GP2Y1010_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_GPIOClockCmd(AIR_SENSOR_LED_PORT, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = AIR_SENSOR_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AIR_SENSOR_LED_PORT, &GPIO_InitStructure);

	TIMx_Config(TIM2, 1000, SystemCoreClock/100000);
	TIMx_OCx_Config(TIM2, 2, 32, ENABLE);
	TIM2_SetInterrupt(ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	ADC_Config(2);
	ADC_Channel_Config(VREF_PORT, VREF_PIN, VREF_ADC_CHAN, 1);
	ADC_Channel_Config(AIR_SENSOR_ADC_PORT, AIR_SENSOR_ADC_PIN, AIR_SENSOR_ADC_CHAN, 2);
	
	ADC_DMA_Init(adc_results, 2);

	ADC_Prepare();

	ADC_StartSingle();
	Delay_us(200);
	ADC_StartSingle();
}

void GP2Y1010_Poweroff()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = AIR_SENSOR_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(AIR_SENSOR_LED_PORT, &GPIO_InitStructure);

	TIM_Cmd(TIM2, DISABLE);
}

void GP2Y1010_IntHandler()
{
	interruptOccur = 1;
}

int GP2Y1010_Measure()
{
	uint16_t val;

	interruptOccur = 0;
	while(!interruptOccur); //等待下一个脉冲
	Delay_us(269);
	ADC_StartSingle();
	Delay_us(200);
	val = (uint16_t)((float)adc_results[1]/adc_results[0]*VREF_VOLT);

	return val;
}
