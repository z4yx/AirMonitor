/******************************************************************************
 * MODULE NAME:     uart_sw.h
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:     This file is intends for UART driver.
 * DESIGNER:        Th3 Huang
 * DATE:            04 23 2012
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2012 MediaTek. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     04 23 2012    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#ifndef UART_SW_H
#define UART_SW_H

#include "bmd.h"
#include "iot_api.h"
#include "xip_ovly.h"

#define  MAX_UART_PORT_NUM       1

typedef enum {
      uart_port1=0,
      uart_port2,
      uart_port3,
      uart_max_port,
      uart_port_null = 99	/* a dummy port for those who doesn't use physical port */
} UART_PORT;

typedef enum {
      RX_BUF,
      TX_BUF
} UART_buffer;

typedef enum {
      uart_sleep_on_tx_allow = 0,
      uart_sleep_on_tx_forbid
} UART_SLEEP_ON_TX;

/*
typedef enum {
      UART_MOD_BT = 1,
      UART_MOD_WF,
      UART_MOD_TST,
      UART_MOD_TEST
} owner_id_enum;
*/
typedef kal_uint32 UART_baudrate;


typedef enum {
      fc_none=1,
      fc_hw,
      fc_sw
} UART_flowCtrlMode;

typedef struct
{
      UART_baudrate                 baud;
      UART_bitsPerCharacter         dataBits;
      UART_stopBits                 stopBits;
      UART_parity                   parity;
      UART_flowCtrlMode             flowControl;
      kal_uint8                     xonChar;
      kal_uint8                     xoffChar;
      kal_bool                      DSRCheck;
} UARTDCBStruct;

typedef struct
{
      kal_uint8                      EscChar;
      kal_uint16                     GuardTime;
} UART_ESCDetectStruct;

typedef struct
{
      kal_uint8                      *rx_buffer;   /* data buffer */
      kal_uint8                      *tx_buffer;   /* data buffer */
} UART_RingBufferStruct;

typedef enum {
  uart_null_buffer=0,
  uart_isr_buffer,
  uart_normal_buffer
} UART_RingBuffer_Type;

typedef struct
{
     UART_RingBuffer_Type            type;
     kal_uint16                      index;
} UART_RingBuffer_SaveIndex;


/*TY adds these to expand flexibility 2004/10/15*/


// for uart dispatch table
typedef enum
{
	UART_TYPE = 0,
	IRDA_TYPE,
	USB_TYPE
}UartType_enum;



/*UART Customization */
typedef struct{
	kal_uint8*	rx_adrs;
	kal_uint16	rx_len;
}UART_ring_buffer_struct;

typedef struct {
	UART_ring_buffer_struct ring[MAX_UART_PORT_NUM];
	UART_PORT               not_support_VFIFO;
} UART_rings_struct;

typedef struct {
   const UART_rings_struct * (*UART_Get_Data)(void);
}UART_customize_function_struct;

/*Function Declaration*/

/*
 * UART tx&rx interrupt enable/disable
 */
void UART_DisableRX(void);
void UART_EnableRX(void);
void UART_DisableTX(void);
void UART_EnableTX(void);

void UART_Init(void) XIP_ATTRIBUTE(".xipsec0");

/*UART customize*/
void uart_customize_init(void) XIP_ATTRIBUTE(".xipsec0");
void UART_Configure_From_Flash(void) XIP_ATTRIBUTE(".xipsec0");

void UART_HWInit(void)  XIP_ATTRIBUTE(".xipsec0");

void UART_Close(void) XIP_ATTRIBUTE(".xipsec0");
void UART_SetBaudRate(UART_baudrate baudrate) XIP_ATTRIBUTE(".xipsec0");
void UART_SetDCBConfig(UARTDCBStruct *UART_Config) XIP_ATTRIBUTE(".xipsec0");
void UART_PutUARTByte(volatile kal_uint8 data);


#if (UART_INTERRUPT == 1)
void UART_Register_TX_cb(UART_TX_FUNC func) XIP_ATTRIBUTE(".xipsec0");
void UART_Register_RX_cb(UART_RX_FUNC func) XIP_ATTRIBUTE(".xipsec0");
void UART_LISR(void);
kal_bool UART_Open(void) XIP_ATTRIBUTE(".xipsec0");
kal_uint16 _UART_GetByte(volatile kal_uint8 *Byte);
kal_uint16 UART_PutBytes(volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length);
#endif

#endif   /*UART_SW_H*/


