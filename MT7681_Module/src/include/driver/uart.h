
#ifndef _UART_H_
#define _UART_H_


void UART_PutChar(UCHAR ch);
int32 UART_GetChar(UCHAR* ch);
int UART_KbdHit(void);
int UART_LSROverErr(void);

#endif /* _UART_H_ */

