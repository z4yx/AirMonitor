#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"

void USARTx_Config(USART_TypeDef* USARTx, u32 USART_BaudRate);
int USART_getchar();
int USART_ReadInt();
void USARTx_printf(USART_TypeDef* USARTx, char *Data, ...);

#endif
