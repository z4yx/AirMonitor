/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  项目的公共函数和参数定义
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
#ifndef __COMMON__H__
#define __COMMON__H__

#include "stm32f10x.h"
#include "hardwareDef.h"
#include "configure.h"
#include <stdint.h>

#define NULL ((void*)0)
#ifndef bool
#define bool uint8_t
#define true 1
#define false 0
#endif

extern void USARTx_printf(USART_TypeDef* USARTx, char *Data, ...);

#define DBG_MSG(format, ...) USARTx_printf(DEBUG_USART, "[Debug]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define ERR_MSG(format, ...) USARTx_printf(DEBUG_USART, "[Error]%s: " format "\r\n", __func__, ##__VA_ARGS__)

 
void RCC_GPIOClockCmd(GPIO_TypeDef* GPIOx, FunctionalState state);
void RCC_USARTClockCmd(USART_TypeDef* USARTx, FunctionalState state);
void Timer_16bit_Calc(int freq, uint16_t *period, uint16_t *prescaler);

typedef void (*Task_t)(void);


#endif /* __COMMON__H__ */