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
#if (UARTRX_TO_AIR_LEVEL == 2)
#define  INDICATE_GPIO  2

TIMER_T       uart2wifi_timer;
UINT_32       uart2wifi_interval          =   UART2WIFI_TIMER_INTERVAL;
UINT_32       uart2wifi_triger_count      =   UART2WIFI_LEN_THRESHOLD;

//UartRxMode    iot_uart_rx_mode            =   UARTRX_ATCMD_MODE;
UartRxMode    iot_uart_rx_mode            =   UARTRX_PUREDATA_MODE;


INT32         previous_input              = -1;


VOID IoT_Cust_uart2wifi_init(UINT_32 interval, UINT_32 triger_count)
{
	uart2wifi_interval     = interval;
	uart2wifi_triger_count = triger_count;
    
	if(uart2wifi_triger_count > UARTRX_TO_AIR_THRESHOLD)
    {
        uart2wifi_triger_count = UARTRX_TO_AIR_THRESHOLD;
    }   
		
	cnmTimerInitTimer(&uart2wifi_timer,  IoT_Cust_uart2wifi_timer_action, 0, 0);
}

#if (UART_INTERRUPT == 1)
        extern UARTStruct UARTPort;
#else
        extern UCHAR command[];
        extern INT16 cmd_len;
#endif

VOID IoT_Cust_uart2wifi_set_mode(UartRxMode mode)
{
	iot_uart_rx_mode = mode;
    
	//clear rx buffer
    #if (UART_INTERRUPT == 1)
        UARTRx_Buf_Init((UARTStruct*)(&UARTPort));
    #else
        cmd_len = 0;
    #endif 
    
	if(mode == UARTRX_PUREDATA_MODE)
	{   
        Printf_High("UARTRX_PUREDATA_MODE\n");
   		cnmTimerStartTimer(&uart2wifi_timer, uart2wifi_interval);
	}
	else if(mode == UARTRX_ATCMD_MODE)
	{
        Printf_High("UARTRX_ATCMD_MODE\n");
		cnmTimerStopTimer(&uart2wifi_timer);
	}
	return;
}


VOID IoT_Cust_uart2wifi_timer_action(UINT_32 param1, UINT_32 param2) 
{
#if (UART_INTERRUPT == 1)
        UINT16  i = 0;
        UCHAR * pCmdBuf;
        BUFFER_INFO *rx_ring = &(UARTPort.Rx_Buffer);
        UINT16 rx_len = 0;
    
        //if it is not in UARTRX_PUREDATA_MODE mode,return directly
        if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE)
        {
            return;
        }
        
        Buf_GetBytesAvail(rx_ring, rx_len);
    
        //Printf_High("uart_rb_send_handler:%d\n", uart_content_count);
    
        pCmdBuf = (UCHAR *)malloc(rx_len);
        
        for(i = 0;i < rx_len;i++)
        {
            Buf_Pop(rx_ring, pCmdBuf[i]);
        }    
        
        IoT_Cust_uart2wifi_data_handler(pCmdBuf, rx_len);
        
        free(pCmdBuf);
        
#else
         UINT16 i = 0;
         UCHAR * pCmdBuf;
         UINT16 rx_len = 0;
    
         //if it is not in UARTRX_PUREDATA_MODE mode,return directly
         if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE)
         {
             return;
         }
         rx_len = cmd_len;
    
         pCmdBuf = (UCHAR *)malloc(rx_len);
         for(i = 0;i < rx_len;i++)
         {
            pCmdBuf[i] = command[i];
         }   
         cmd_len = 0;
         IoT_Cust_uart2wifi_data_handler(pCmdBuf, rx_len);
         free(pCmdBuf);
#endif
	cnmTimerStartTimer(&uart2wifi_timer, uart2wifi_interval);

}


VOID IoT_Cust_uart2wifi_buffer_trigger_action(VOID) 
{
#if (UART_INTERRUPT == 1)
	UINT16	i = 0;
    UCHAR * pCmdBuf;
	BUFFER_INFO *rx_ring = &(UARTPort.Rx_Buffer);
	UINT16 rx_len = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE)
    {
        return;
    }
    
	Buf_GetBytesAvail(rx_ring, rx_len);

	if(rx_len < uart2wifi_triger_count)
    {   
		return;
    }
	//Printf_High("uart_rb_send_handler:%d\n", uart_content_count);

    pCmdBuf = (UCHAR *)malloc(rx_len);
    
	for(i = 0;i < rx_len;i++)
	{
		Buf_Pop(rx_ring, pCmdBuf[i]);
	}     
    IoT_Cust_uart2wifi_data_handler(pCmdBuf, rx_len);
    free(pCmdBuf);
    
#else
     UINT16	i = 0;
     UCHAR * pCmdBuf;
     UINT16 rx_len = 0;

     //if it is not in UARTRX_PUREDATA_MODE mode,return directly
     if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE)
     {
         return;
     }
     rx_len = cmd_len;
     if(rx_len < uart2wifi_triger_count)
     {   
         return;
     }
    
     pCmdBuf = (UCHAR *)malloc(rx_len);
     for(i = 0;i < rx_len;i++)
	 {
		pCmdBuf[i] = command[i];
	 }   
     cmd_len = 0;
     IoT_Cust_uart2wifi_data_handler(pCmdBuf, rx_len);
     free(pCmdBuf);
#endif



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

