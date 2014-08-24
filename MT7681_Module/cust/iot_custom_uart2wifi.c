#include <stdio.h>
#include "string.h"
#include "types.h"
#include "uart.h"
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
#if (UARTRX_TO_AIR_AUTO == 1)

int iot_uart_rx_mode = UARTRX_ATCMD_MODE;

extern BUFFER_INFO uart_rb_info;

TIMER_T UartSendTimer;
UINT_32 uart2wifi_interval = 300; //ms
UINT_32  uart2wifi_triger_count = 30;

INT32 previous_input = -1;
#define INDICATE_GPIO 2

VOID IoT_Cust_uart2wifi_init(UINT_32 interval, UINT_32 triger_count)
{
	uart2wifi_interval = interval;
	uart2wifi_triger_count = triger_count;
	if(uart2wifi_triger_count>UART_RX_RING_BUFF_SIZE)
		uart2wifi_triger_count = UART_RX_RING_BUFF_SIZE;
	cnmTimerInitTimer(&UartSendTimer,  IoT_Cust_uart2wifi_timer_action, 0, 0);
}

VOID IoT_Cust_uart2wifi_set_mode(UartRxMode mode)

{
	iot_uart_rx_mode = mode;

	if(mode == UARTRX_PUREDATA_MODE)
		{
		Printf_High("UARTRX_PUREDATA_MODE\n");
		cnmTimerStartTimer(&UartSendTimer, uart2wifi_interval);
		}
	else if(mode == UARTRX_ATCMD_MODE)
		{
		Printf_High("UARTRX_ATCMD_MODE\n");
		cnmTimerStopTimer(&UartSendTimer);
		}

	return;
}


VOID IoT_Cust_uart2wifi_timer_action(UINT_32 param1, UINT_32 param2) 
{
	UINT16	i = 0;
	UCHAR uart_content[UART_RX_RING_BUFF_SIZE]={0};
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	UINT_32 uart_content_count = 0;
	
	Buf_GetBytesAvail(puart_rb_info, uart_content_count);
	
	//Printf_High("IoT_Cust_uart2wifi_timer_action:%d\n", uart_content_count);
	
	for(i=0; i<uart_content_count; i++)
	{
		Buf_Pop(puart_rb_info, uart_content[i]);
	}
	
    IoT_Cust_uart2wifi_data_handler(uart_content, uart_content_count);

	cnmTimerStartTimer(&UartSendTimer, uart2wifi_interval);

}


VOID IoT_Cust_uart2wifi_buffer_trigger_action(VOID) 
{
	UINT16	i = 0;
	UCHAR uart_content[UART_RX_RING_BUFF_SIZE]={0};
	BUFFER_INFO *puart_rb_info = &uart_rb_info;
	UINT_32 uart_content_count = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE)
    {
        return;
    }
    
	Buf_GetBytesAvail(puart_rb_info, uart_content_count);

    
	if(uart_content_count < uart2wifi_triger_count)
    {   
		return;
    }
	//Printf_High("uart_rb_send_handler:%d\n", uart_content_count);
	
	for(i=0; i<uart_content_count; i++)
	{
		Buf_Pop(puart_rb_info, uart_content[i]);
	}
	
    IoT_Cust_uart2wifi_data_handler(uart_content, uart_content_count);


}

void IoT_Cust_uart2wifi_data_handler(UCHAR *uart_content, UINT16 uart_content_count)

{
	IoT_uart_output(uart_content, uart_content_count);

	/*should not call uip_send here, all uip_send need to be implememted
	   in iot_udp_appcall() / iot_tcp_appcall(),  as the reason of the uIP app
	   management (Connection/Port...) is controlled in the iot_***_appcall()*/
	//uip_send(uart_content, uart_content_count);//mask
	
	/*here should allocate a buffer or flag,  
	   Let iot_***_appcall() detected it and call uip_send()*/

	return;
}



INT32 IoT_Cust_uart2wifi_detect_gpio_input_change(VOID)
{
#if 0
	UINT32 input, ret;
 	IoT_gpio_input(INDICATE_GPIO, &input);  /*shall change GPIO to input mode*/
#else
	UINT8 input, Polarity, ret;
	IoT_gpio_read(INDICATE_GPIO, &input, &Polarity);
#endif
	//Printf_High("IoT_Cust_uart2wifi_detect_gpio_input_change:%d\n",input);

	if((input==1&&previous_input==0)||
		 (input==0&&previous_input==1)||
				   previous_input==-1)
		ret = input;
	else
		ret = -1;

	previous_input = input;
	return ret;

}

VOID IoT_Cust_uart2wifi_change_mode_handler(VOID)
{

	int input_status=0;
	
	input_status = IoT_Cust_uart2wifi_detect_gpio_input_change();
	
	if(input_status == 0)
	{
		IoT_Cust_uart2wifi_set_mode(UARTRX_ATCMD_MODE);
	}
	else if(input_status ==1)
	{
		IoT_Cust_uart2wifi_set_mode(UARTRX_PUREDATA_MODE);
	}

}

#endif

