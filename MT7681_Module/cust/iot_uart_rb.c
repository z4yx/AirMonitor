#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_custom.h"
#include "iot_api.h"
#include "bmd.h"
#include "uart_sw.h"


/******************************************************************************
* MODULE NAME:     iot_uart_rb.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2013
*
*最新版本程序我们会在 http://www.ai-thinker.com 发布下载链接
*
* LICENSE:
*     This source code is copyright (c) 2011 Ralink Tech. Inc.
*     All rights reserved.
*
* 深圳市安信可科技 MTK7681串口模块专业生产厂家 
*   V1.0.0     Jan 2012    - Initial Version V1.0
*
*
* 串口WIFI 价格大于500 30元   大于5K  28元   大于10K  20元
* ISSUES:
*    First Implementation.
* 淘宝店铺http://anxinke.taobao.com/?spm=2013.1.1000126.d21.FqkI2r
*
******************************************************************************/
#if (UART_INTERRUPT == 1)

/*
 * 2014/05/13,terrence,define uart tx/rx ring buff
 * 
 */
kal_uint8     UARTRxBuf[UARTRX_RING_LEN];    //used in UARTPort.Rx_Buffer

kal_uint8     UARTTxBuf[UARTTX_RING_LEN];    //used in UARTPort.Tx_Buffer

UARTStruct    UARTPort;



/**************************************************************************/
/**************************************************************************/

void UARTRx_Buf_Init(UARTStruct *qp)
{
    PKT_DESC     *rx_desc = &(qp->Rx_desc);
    BUFFER_INFO  *rx_ring = &(qp->Rx_Buffer);

    rx_desc->pkt_num = 0;
    rx_desc->cur_num = 0;
    rx_desc->cur_type = PKT_UNKNOWN;
    Buf_init(rx_ring,(kal_uint8 *)(UARTRxBuf),(kal_uint16)UARTRX_RING_LEN);
}


/*
 *  task context
 *  write user data to UART tx buffer
 *  kal_uint8 *Buffaddr:pointer to user's data
 *  kal_uint16 Length:  user's data length
 *  return:real length write to tx buffer
 */
kal_uint16 UART_PutBytes(volatile kal_uint8 *Buffaddr, volatile kal_uint16 Length)
{
    kal_uint16 len, i;
    volatile kal_uint16 roomleft = 0;
    kal_uint8* ptr = Buffaddr;
    kal_uint16  size = Length;

    /*
     * when litter tx buff, should use polling
     */
    while (size)
    {  
        len = 0;
        Buf_GetRoomLeft(&(UARTPort.Tx_Buffer),roomleft);
        if(roomleft)
        {    
           if (size <= roomleft)
               len = size;
           else if (size > roomleft)
               len = roomleft;

           for(i = 0;i < len;i++ )
               Buf_Push(&(UARTPort.Tx_Buffer),*(ptr++));

            size -= len;  
        }
        UART_EnableTX(); 
    }
 
    return Length;
}


//2014/05/13,terrence add call back function in uart isr handler
/*
 * Notice: Must not use Printf_High in the RX INT Handler and here, the Rx FIFO,RxINT and ringbuf will mess
 */ 

/*
 * ISR context
 * UART_Tx_Cb() will be called when UART tx interrupt assert
 * fucntion: fetch data from user buffer to tx ring buffer
 *  
 */
void UART_Tx_Cb(void)
{
   //void currently 

}



/*
 *  ISR context
 *  Read byte from UART FIFO, we should use it in void UART_Rx_Cb(void)
 *  kal_uint8 *Byte:  a byte read from FIFO successful
 *  return: 1,read a byte successful;0,FIFO is empty
 */
extern kal_uint16 _UART_GetByte(volatile kal_uint8 *Byte);
kal_uint16 UART_GetByte(volatile kal_uint8 *Byte)
{
#if (UARTRX_TO_AIR_LEVEL == 1)
    //save bytes to query ring buff also
    if (_UART_GetByte(Byte))
    {
        uart_rb_push(*Byte); 
        return 1;
    }
    else
    {   
        UART_EnableRX();
        return 0;
    }
    
#else
    return _UART_GetByte(Byte);
#endif
}


char ATCmdPrefixAT[] = AT_CMD_PREFIX;
char ATCmdPrefixIW[] = AT_CMD_PREFIX2;

/*
 * ISR context
 * UART_Rx_Cb() will be called when UART rx interrupt assert,
 * then we should featch data from HW FIFO quickly.
 * fucntion: fetch data from HW FIFO to rx ring buffer
 * we should use UART_GetByte(&ch) to fetch byte from HW FIFO as quickly as possible
 */
void UART_Rx_Cb(void)
{  
    kal_uint16   roomleft = 0;
    PKT_FIFO     *infor;
    PKT_FIFO     *temp_info;
    UCHAR        ch = 0;
    UINT8        i = 0;

    PKT_DESC     *rx_desc = &(UARTPort.Rx_desc);
    BUFFER_INFO  *rx_ring = &(UARTPort.Rx_Buffer); 

	static UINT8  ATMatchNum = 0;
	static UINT8  IWMatchNum = 0;
    
/*
 * MCU only forward uart rx data to air
 * here,copy to rx ring and return
 */    
#if (UARTRX_TO_AIR_LEVEL == 2)
    extern int iot_uart_rx_mode;
	if(iot_uart_rx_mode == UARTRX_PUREDATA_MODE)
	{
        while (UART_GetByte(&ch))
        {
            Buf_Push(rx_ring,ch);
        }   
        return;
	} 
#endif
 
/*
 * MCU should collect data to be packet
 */
    //normal case
    Buf_GetRoomLeft(rx_ring,roomleft);

    while (UART_GetByte(&ch))
    {   
        //new receive begin,detect packet header at first
        switch (rx_desc->cur_type)
        {
        
            case PKT_UNKNOWN:
            {  
                 
                /**************** detect packet type ***************/
    		   //support more ATcmd prefix analysis
    			/*case 1:AT#*/
    			if (ATCmdPrefixAT[ATMatchNum] == ch)
                {         
    				ATMatchNum++;
                }
    			else
                {         
    				ATMatchNum = 0;
                }
    			
    			/*case 2:iwpriv ra0*/
    			if (ATCmdPrefixIW[IWMatchNum] == ch)
                {         
    				IWMatchNum++;
                }
    			else
                {         
    				IWMatchNum = 0;
                }
    
    			if ((ATMatchNum == sizeof(ATCmdPrefixAT)-1) ||   //match case 1: AT#
    				(IWMatchNum == sizeof(ATCmdPrefixIW)-1))    //match case 2:iwpriv ra0
    			{   

    			    rx_desc->cur_num = rx_desc->pkt_num;                  
    				infor = &(rx_desc->infor[rx_desc->cur_num]);
    				infor->pkt_len = 0;
    
    				if(ATMatchNum == sizeof(ATCmdPrefixAT)-1)
                    {            
    					rx_desc->cur_type = PKT_ATCMD;			 //match case 1: AT#
                    }
    				else if (IWMatchNum == sizeof(ATCmdPrefixIW)-1)
                    {            
    					rx_desc->cur_type = PKT_IWCMD;			 //match case 2:iwpriv ra0
                    }
                    
    				ATMatchNum = 0;
    				IWMatchNum = 0;
    				continue;
    			}			
            }
            break;


    
            case PKT_ATCMD:
    	    case PKT_IWCMD:
            {   
                infor = &(rx_desc->infor[rx_desc->cur_num]);
     
                /*
                 * received one complete packet
                 */
                if(ch == '\n' || ch == '\r')
                {   
                    //if task has consumed some packets
                    if (rx_desc->cur_num != rx_desc->pkt_num)
                    {   
                        temp_info = infor;
                        infor     = &(rx_desc->infor[rx_desc->pkt_num]);
                        infor->pkt_len = temp_info->pkt_len;
                        temp_info->pkt_len = 0;
                        temp_info->pkt_type = PKT_UNKNOWN;
                    }
                    
                    infor->pkt_type = rx_desc->cur_type;  // PKT_ATCMD / PKT_IWCMD;
                    rx_desc->pkt_num++;
                    rx_desc->cur_type = PKT_UNKNOWN;
                }
                /*
                 * continue to receiving packet
                 */                
                else
                {
                    Buf_Push(rx_ring,ch);
                    roomleft--;
                    infor->pkt_len++;
                }
    
    
                /*
                 * if overflow,we discard the current packet
                 * example1:packet length > ring size
                 * example2:rx ring buff can no be freed by task as quickly as rx interrupt coming
                 */    
                if ((!roomleft) || (rx_desc->pkt_num >= NUM_DESCS))
                {   
                    //rollback
                    Buff_RollBack(rx_ring,infor->pkt_len);
         
                    roomleft += infor->pkt_len;
                    
                    infor->pkt_type = PKT_UNKNOWN;
                    infor->pkt_len = 0;
                    rx_desc->cur_type = PKT_UNKNOWN;

                    if (rx_desc->pkt_num >= NUM_DESCS)
                    {
                        rx_desc->pkt_num--;
                    }
                        
                }
            }  
            break;

            default:
               break;
        
        }
    }
           
 }

    
/*
 * task context
 * fucntion: dispatch and handle packet from rx ring buffer
 */
void UART_Rx_Packet_Dispatch(void)
{

    PKT_FIFO     *infor;
    PKT_DESC     *rx_desc = &(UARTPort.Rx_desc);
    BUFFER_INFO  *rx_ring = &(UARTPort.Rx_Buffer); 

    
    PKT_TYPE rxpkt_type;
    UINT16   rxpkt_len;
    UINT16 i;
    //PCHAR pCmdBuf = NULL;
	UCHAR pCmdBuf[UARTRX_RING_LEN];
	
    while (rx_desc->pkt_num)
    {   
        //simulate FIFO,1st in,1st out
        infor = &(rx_desc->infor[0]);
        rxpkt_type = infor->pkt_type;
        rxpkt_len  = infor->pkt_len;
        
		memset(pCmdBuf, 0, UARTRX_RING_LEN);
        //pCmdBuf = (PCHAR)malloc(rxpkt_len);
    
        //copy from uart rx ring
        for(i = 0;i < rxpkt_len;i++)       //O(n)
        {
            Buf_Pop(rx_ring,pCmdBuf[i]);
            //Printf_High("Buf_Pop=%x \n",pCmdBuf[i]);
        }
        
        //reset value
        infor->pkt_type = PKT_UNKNOWN;
        infor->pkt_len = 0;
        
        //shift FIFO
        for (i=0; i<(rx_desc->pkt_num)-1; i++)  //O(n)
        {
           rx_desc->infor[i]= rx_desc->infor[i+1];
        }  
        rx_desc->pkt_num--;

        //handle previous packet
        switch (rxpkt_type)
        {
            case PKT_ATCMD:
                IoT_parse_ATcommand(pCmdBuf, rxpkt_len);
                break;
            case PKT_IWCMD:
                IoT_parse_IWCmd(pCmdBuf, rxpkt_len);
                break;
             // <----custom defined packet dispatch
             
             default:
                break;
        }    
        
        //free(pCmdBuf); 
    }   

}

#endif

void uart_customize_init(void)
{
#if (UARTRX_TO_AIR_LEVEL == 1)
	uart_rb_init();
#endif

#if (UART_INTERRUPT == 1)
	/*configure ringbuffer*/
    Buf_init(&(UARTPort.Tx_Buffer),(kal_uint8 *)(UARTTxBuf),(kal_uint16)UARTTX_RING_LEN); 
    UARTRx_Buf_Init((UARTStruct*)(&UARTPort));
#endif
}


#if (UARTRX_TO_AIR_LEVEL == 1)
//query ring buff
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

