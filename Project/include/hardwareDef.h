/*
 * =====================================================================================
 *
 *       Filename:  hardwareDef.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013/1/18 10:28:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangyuxiang
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __HARDWARE_DEF__HH__
#define __HARDWARE_DEF__HH__

#define AIR_SENSOR_LED_PORT GPIOA
#define AIR_SENSOR_LED_PIN GPIO_Pin_1
#define AIR_SENSOR_ADC_PORT GPIOB
#define AIR_SENSOR_ADC_PIN GPIO_Pin_1
#define AIR_SENSOR_ADC_CHAN ADC_Channel_9

#define VREF_PORT GPIOB
#define VREF_PIN GPIO_Pin_0
#define VREF_ADC_CHAN ADC_Channel_8
#define VREF_VOLT 1235

#define SHT_PORT GPIOB
#define SHT_SCK GPIO_Pin_8
#define SHT_DATA GPIO_Pin_9

#define ETH_PORT GPIOB
#define ETH_RST GPIO_Pin_10
#define ETH_INT GPIO_Pin_11
#define ETH_CS GPIO_Pin_12
#define ETH_SCK GPIO_Pin_13
#define ETH_MISO GPIO_Pin_14
#define ETH_MOSI GPIO_Pin_15


#endif
