#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_custom.h"
#include "iot_api.h"
#include "bmd.h"

/******************************************************************************
* MODULE NAME:     iot_uart_rb.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2013
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2011 Ralink Tech. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     Jan 2012    - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/
#if (UARTRX_TO_AIR_QUERY == 1 || UARTRX_TO_AIR_AUTO == 1)

BUFFER_INFO uart_rb_info;
UCHAR uart_rb_addr[UART_RX_RING_BUFF_SIZE];


void uart_rb_init(void)
{
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	UCHAR * puart_rb_addr = uart_rb_addr;

	Buf_init(puart_rb_info, puart_rb_addr, UART_RX_RING_BUFF_SIZE);
	

	return;
}


UCHAR uart_rb_pop(void)
{
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	UCHAR ch;

	Buf_Pop(puart_rb_info, ch);
	//Printf_High("uart_rb_pop:%c\n",ch);
	return ch;

}

void uart_rb_push(UCHAR ch)
{
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	Buf_Push(puart_rb_info,ch);

	//Printf_High("uart_rb_push:%c\n",ch);
	return;

}

UINT16 uart_get_avail(void)
{
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	UINT16 count;

	Buf_GetBytesAvail(puart_rb_info, count);
	//Printf_High("uart_get_avail:%d\n",count);

	return count;
}

#endif

