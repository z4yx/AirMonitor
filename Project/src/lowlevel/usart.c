/*
 * =====================================================================================
 *
 *       Filename:  usart.c
 *
 *    Description:  USART1串口收发与调试输出
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
#include <stdarg.h>

/*
 * 初始化串口配置
 */
void USARTx_Config(USART_TypeDef* USARTx, u32 USART_BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    GPIO_TypeDef *USART_GPIO;
    u16 USART_Rx, USART_Tx;

    switch((u32)USARTx) {
        case (u32)USART1:
            USART_GPIO = GPIOA;
            USART_Tx = GPIO_Pin_9;
            USART_Rx = GPIO_Pin_10;
            break;
        case (u32)USART2:
            USART_GPIO = GPIOA;
            USART_Tx = GPIO_Pin_2;
            USART_Rx = GPIO_Pin_3;
            break;
        case (u32)USART3:
            USART_GPIO = GPIOB;
            USART_Tx = GPIO_Pin_10;
            USART_Rx = GPIO_Pin_11;
            break;

        default:
            return;
    }

    /* config USART clock */
    RCC_USARTClockCmd(USARTx, ENABLE);

    /* USART GPIO config */
    RCC_GPIOClockCmd(USART_GPIO, ENABLE);
    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_Tx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);
    /* Configure USART Rx as input*/
    GPIO_InitStructure.GPIO_Pin = USART_Rx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);

    /* USART mode config */
    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);
}

/*
 * 通过串口接收一个字节
 */
int USART_getchar()
{
    uint8_t ch;
    while (SET != USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
    ch = USART_ReceiveData(USART1);
    USART_ClearFlag(USART1, USART_FLAG_RXNE);
    return (ch);
}

/*
 * 读入一个整数值
 */
int USART_ReadInt()
{
    uint8_t neg = 0;
    int c = USART_getchar(), ret = 0;
    while(c<'0'||c>'9'){
        if(c=='-')
            neg = 1;
        c = USART_getchar();
    }
    while(c>='0'&&c<='9'){
        ret = ret*10+(c-'0');
        c = USART_getchar();
    }
    return neg ? -ret : ret;
}

/*
 * 数值转字符串函数
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 1000000000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * 通过串口发送格式化字符串
 */
void USARTx_printf(USART_TypeDef* USARTx, char *Data, ...)
{
    const char *s;
    int d;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)
    {
        if ( *Data == '\\' )
        {
            switch ( *++Data )
            {
            case 'r':
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

            case 'n':
                USART_SendData(USARTx, 0x0a);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            switch ( *++Data )
            {
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    USART_SendData(USARTx, *s);
                    while ( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;

            case 'd':
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    USART_SendData(USARTx, *s);
                    while ( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else USART_SendData(USARTx, *Data++);
        while ( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}
