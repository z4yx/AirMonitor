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
typedef void (*UART_TX_FUNC)(UART_PORT port);
typedef void (*UART_RX_FUNC)(UART_PORT port);
typedef void (*UART_RX_DMA_FUNC)(UART_PORT port);

typedef struct
{
      UART_PORT                      port_no;
      kal_bool                       initialized;
      //owner_id_enum                    ownerid;
      //owner_id_enum                    UART_id;
      kal_bool                       breakDet;
      kal_bool                       EscFound;
      UARTDCBStruct                  DCB;
      UART_RingBufferStruct          RingBuffers;
      UART_ESCDetectStruct           ESCDet;
      BUFFER_INFO                    Tx_Buffer_ISR; /* receive buffer */
      BUFFER_INFO                    Rx_Buffer;  /* receive buffer */
      BUFFER_INFO                    Tx_Buffer;  /* transmit buffer */
      //kal_hisrid                     hisr;
      kal_uint32                     hisr;
      IO_level                       DSR;
      /*detect Escape*/
      kal_uint8                      handle;    /*GPT handle*/
      kal_uint8                      EscCount;
      kal_uint8                      Rec_state; /**/
      UART_SLEEP_ON_TX               sleep_on_tx;
      kal_bool               		EnableTX;
      /*tx, rx dma call back function*/
      UART_TX_FUNC                  tx_cb;
      UART_RX_FUNC                  rx_cb;
      UART_RX_DMA_FUNC              rx_dma_cb;
      //#ifdef __DMA_UART_VIRTUAL_FIFO__
      kal_uint8 							Rx_DMA_Ch;
	  kal_uint8 							Tx_DMA_Ch;     
      //#endif
} UARTStruct;

// for uart dispatch table
typedef enum
{
	UART_TYPE = 0,
	IRDA_TYPE,
	USB_TYPE
}UartType_enum;

typedef struct _uartdriver
{
	kal_bool (*Open)(UART_PORT port);
	void (*Close)(UART_PORT port);
    kal_uint16 (*GetBytes)(UART_PORT port, volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length);
    kal_uint16 (*PutBytes)(UART_PORT port, volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length);
	kal_uint16 (*GetRxAvail)(UART_PORT port);
	void (*Purge)(UART_PORT port, UART_buffer dir);
	void (*SetDCBConfig)(UART_PORT port, UARTDCBStruct *UART_Config);
	void (*UART_Register_TX_cb)(UART_PORT port, UART_TX_FUNC func);
	void (*UART_Register_RX_cb)(UART_PORT port, UART_RX_FUNC func);
	kal_bool (*GetUARTByte)(UART_PORT port, kal_uint8 *data);
	void (*PutUARTBytes)(UART_PORT port, volatile kal_uint8 *data, volatile kal_uint16 len);
}UartDriver_strcut;

typedef struct _uarthandle
{
	UartType_enum type;
	UartDriver_strcut* drv;
	void* dev;
}UartHandle_struct;



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
extern void UART_Init(void);
extern UartDriver_strcut UartDriver;
extern kal_bool UART_GetUARTByte(UART_PORT port, kal_uint8 *data);
extern void PutUARTByte(UART_PORT port, kal_uint8 data);
extern void UART_SetBaudRate(UART_PORT port, UART_baudrate baud_rate);
extern void U_SetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config);

extern void UART_SetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config);
extern void UART_loopback(UART_PORT port);
extern void UART_HWInit(UART_PORT port);
extern kal_bool UART_Open(UART_PORT port);
extern void UART_Close(UART_PORT port);
//extern void UART_ReadHWStatus(UART_PORT port, IO_level *SDSR, IO_level *SCTS);
extern void UART_Purge(UART_PORT port, UART_buffer dir);
extern void UART_Register_RX_cb(UART_PORT port, UART_RX_FUNC func);
extern void UART_Register_TX_cb(UART_PORT port, UART_TX_FUNC func);
extern void UART_PutUARTBytes_Poll(UART_PORT port, volatile kal_uint8 *data, volatile kal_uint16 len);
extern kal_uint16 UART_GetBytes(UART_PORT port, volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length);
extern kal_uint16 UART_PutBytes(UART_PORT port, volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length);
extern kal_uint16 UART_GetBytesAvail(UART_PORT port);
extern void UART1_LISR(void);
extern void UART_enbale_RX(UART_PORT port, kal_bool enable);
extern void UART_Tx_Cb(UART_PORT port);
extern void UART_Rx_Cb(UART_PORT port);



// for Uart Dispatch
extern void UART_Register(UART_PORT port, UartType_enum type, UartDriver_strcut* drv);
/*UART customize*/
extern void uart_customize_init(void);
#endif   /*UART_SW_H*/


