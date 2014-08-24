#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#if 0 //CFG_SUPPORT_TCPIP
#include "uip.h"
#endif
#include "flash_map.h"
#include "uip-conf.h"
#include "eeprom.h"

/******************************************************************************
* MODULE NAME:     iot_parse.c
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

#define TCPUDP_SRC_PORT		(34932)
#define TCPUDP_DST_PORT		(8176)
UCHAR src_addr[]={192,168,1,10};
UCHAR dest_addr[]={255,255,255,255};

extern IOT_ADAPTER   IoTpAd;
UCHAR IoT_CmdPWD_Recv[PASSWORD_MAX_LEN] = {0xFF,0xFF,0xFF,0xFF};

UINT16 preSeq = 0;
UINT16 appLen = 0;

#if (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 1)
	/* {01011100B,00000000B,00001110B,7,1,4}; */
	const IoTHardwareRresource mt7681_HardwareRresource = {0x5C, 0x00, 0x0E, 0x7, 0x1, 0x4};
#elif (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 2)
	/* {00011111B,00000001B,00000000B,5,1,5}; */
	const IoTHardwareRresource mt7681_HardwareRresource = {0x1F, 0x01, 0x0E, 0x5, 0x1, 0x5};
#else
	/* {00011111B,00000001B,00000000B,7,1,4};*/
	const IoTHardwareRresource mt7681_HardwareRresource = {0x1F, 0x01, 0x00, 0x7, 0x1, 0x4};
#endif

IoTHardwareStatus mt7681_HardwareStatus = {0,0,0};

INT32 IoT_process_app_function_packet(
						DataHeader* Dataheader, 
						UINT8 FunctionType, 
						IoTPacketInfo  *pPacketInfo)
{

		GPIO_Information *GpioData;
		UINT_32 GPIO_Value;
		GPIO_Information *GPIO_Information_out;
		PWM_Information *LedData;
		UART_Information *UartData;
		PWM_Information *LedData_out;
		UART_Information *UartData_out;
		UINT32 gpio_input=0;
		//UINT8 gpio_read=0,  Polarity=0;
#if (UARTRX_TO_AIR_LEVEL == 1)
		UINT16 uart_content_count=0;
#endif
		UCHAR iot_buffer[IOT_BUFFER_LEN]={0};
		PUCHAR Data; 
		UCHAR * Data_out;
		UINT16 payload_len =0;
		INT8 i=0;
		bool Need_resp = TRUE;
		Status *Status_out;
		UINT16 uart_tx_length = 0;

		Data = (PUCHAR)(Dataheader) + CP_DATA_HDR_LEN; 
		Data_out = (UCHAR *)(iot_buffer+CP_HDR_LEN
							+CP_DATA_HDR_LEN);


		GpioData = (GPIO_Information *)Data;
		LedData = (PWM_Information *)Data;
		UartData = (UART_Information *)Data;
		LedData_out = (PWM_Information *)Data_out;
		UartData_out = (UART_Information *)Data_out;
		GPIO_Information_out = (GPIO_Information *) Data_out;
		Status_out = (Status *) Data_out;


		if(memcmp(&pPacketInfo->SessionID,  IoTpAd.ComCfg.CmdPWD, PASSWORD_MAX_LEN))
		{

			DBGPRINT(RT_DEBUG_ERROR, ("wrong iot device password\n"));
			return 0;

		}

		switch (FunctionType)
		{
			case GPIO_SET_REQUEST :
				DBGPRINT(RT_DEBUG_ERROR, ("GPIO_SET_REQUEST\n"));
				GPIO_Value = GpioData->GPIO_Value;
				if(Dataheader->Type == GPIO_INFORMATION)
				{
						
					if(pPacketInfo->Sequence != preSeq)
					{

						for(i=0; i < mt7681_HardwareRresource.GPIO_Count; i++)
						{
							if(((mt7681_HardwareRresource.GPIO_Rresource)&((UINT_32)1<<i))==0)
								continue;
							
							if((GPIO_Value)&((UINT_32)1<<i))
							{
								IoT_gpio_output(i,1);
								//Printf_High("iot gpio ok high: %d \n", i);
							}
							else
							{
								IoT_gpio_output(i,0);
								//Printf_High("iot gpio ok low: %d \n", i);
							}
						}
					}
					
					IoT_build_app_response_header(iot_buffer, FUNCTION, GPIO_SET_RESPONSE, STATUS, sizeof(Status), pPacketInfo); 
								
					Status_out->StatusCode = 0; 							
				}
				
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(Status));	
				break;
				
			case PWM_SET_REQUEST:
				DBGPRINT(RT_DEBUG_TRACE,("PWM_SET_REQUEST\n"));
#if (IOT_PWM_SUPPORT == 1)		
				if(Dataheader->Type == PWM_INFORMATION)
				{
					/*this is a sample,  customer maybe use other GPIO for PWM*/
					IoT_led_pwm(1, (INT32)LedData->RedLevel);
					IoT_led_pwm(2, (INT32)LedData->GreenLevel);
					IoT_led_pwm(3, (INT32)LedData->BlueLevel);
					//IoT_led_pwm(4, (INT32)LedData->RedLevel);
					//IoT_led_pwm(0, (INT32)LedData->RedLevel);

					mt7681_HardwareStatus.RedLevel = LedData->RedLevel;
					mt7681_HardwareStatus.GreenLevel = LedData->GreenLevel;
					mt7681_HardwareStatus.BlueLevel = LedData->BlueLevel;
					
					IoT_build_app_response_header(iot_buffer, FUNCTION, PWM_SET_RESPONSE, STATUS, sizeof(Status), pPacketInfo);	
								
					Status_out->StatusCode = 0; 			
				}
				
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(Status));	
#else
				return 0;
#endif
				break;
				
			case UART_SET_REQUEST:
				DBGPRINT(RT_DEBUG_TRACE,("UART_SET_REQUEST\n"));
				
				if(Dataheader->Type == UART_INFORMATION)
				{

					if(pPacketInfo->Sequence != preSeq)
					{

						if(Dataheader->Length != appLen - CP_HDR_LEN - CP_DATA_HDR_LEN)
						{
							Status_out->StatusCode = 1; 
						}
						else
						{
							Status_out->StatusCode = 0; 
							uart_tx_length = Dataheader->Length;
							IoT_uart_output((CHAR *)UartData->Data, (size_t)uart_tx_length);
						}

					}
					
					IoT_build_app_response_header(iot_buffer, FUNCTION, UART_SET_RESPONSE, STATUS, sizeof(Status), pPacketInfo); 
						
					
				}
				
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(Status));					
				break;
				
			case GPIO_GET_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("GPIO_GET_REQUEST\n"));
				
				IoT_build_app_response_header(iot_buffer, FUNCTION, GPIO_GET_RESPONSE, GPIO_INFORMATION, sizeof(GPIO_Information), pPacketInfo);
				

				GPIO_Information_out->GPIO_List = 0;
				GPIO_Information_out->GPIO_Value = 0;			
			
				
				for(i=0; i < mt7681_HardwareRresource.GPIO_Count; i++)
				{
					if(!((mt7681_HardwareRresource.GPIO_Rresource)&((UINT_32)1<<i)))
						continue;
					
					IoT_gpio_input((INT32)i, &gpio_input);		 /*shall change GPIO to input mode*/
					//IoT_gpio_read((INT32)i, &gpio_read, &Polarity);  /*Only read GPIO val and mode, not change GPIO to input mode*/
	
					GPIO_Information_out->GPIO_List |= ((UINT_32)0x01<<i);
					
					if(gpio_input == 1)
					{
						GPIO_Information_out->GPIO_Value |= ((UINT_32)0x01<<i);
					}
					
					DBGPRINT(RT_DEBUG_TRACE,("gpio %d input: %d\n",i,gpio_input));
				}
				
				DBGPRINT(RT_DEBUG_TRACE,("gpio list&value %x,%x: %d\n",GPIO_Information_out->GPIO_List,GPIO_Information_out->GPIO_Value));				
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(GPIO_Information));					
				break;
				
			case PWM_GET_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("PWM_GET_REQUEST\n")); 
				
				IoT_build_app_response_header(iot_buffer, FUNCTION, PWM_GET_RESPONSE, PWM_INFORMATION, sizeof(PWM_Information), pPacketInfo);
				

				LedData_out->RedLevel = mt7681_HardwareStatus.RedLevel;
				LedData_out->GreenLevel = mt7681_HardwareStatus.GreenLevel;
				LedData_out->BlueLevel = mt7681_HardwareStatus.BlueLevel;
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(PWM_Information));			
				break;
			
			case UART_GET_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("UART_GET_REQUEST\n"));

#if (UARTRX_TO_AIR_LEVEL == 1)

				if(pPacketInfo->Sequence == preSeq)
					return 0;
				
				uart_content_count = uart_get_avail();
				for(i=0; i<uart_content_count; i++)
					UartData_out->Data[(UINT8)i] = uart_rb_pop();

				DBGPRINT(RT_DEBUG_TRACE,("UART_GET_REQUEST:%d,%x,%x\n",uart_content_count,Data_out[0],Data_out[1]));
				IoT_build_app_response_header(iot_buffer, FUNCTION, UART_GET_RESPONSE, UART_INFORMATION, uart_content_count, pPacketInfo);
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+uart_content_count);
#else
				return 0;
#endif
				break;
									
		}
		
		if(Need_resp == TRUE)
		{
#if CFG_SUPPORT_TCPIP

#if (AES_DATAPARSING_SUPPORT == 1)
			AES_uip_send(iot_buffer, payload_len);
#else
			uip_send(iot_buffer, payload_len);
#endif

#else
			IoT_build_send_udp_packet(src_addr, dest_addr, TCPUDP_SRC_PORT, 
								TCPUDP_DST_PORT, iot_buffer, payload_len);
#endif
		}	
		return 0;

}


INT32 IoT_process_app_management_packet(
							DataHeader* Dataheader, 
							UINT16 ManagementType, 
							IoTPacketInfo  *pPacketInfo)

{

	UCHAR iot_buffer[IOT_BUFFER_LEN]={0};
	PUCHAR Data; 
	UCHAR * Data_out;
	UINT16 payload_len =0;
	bool Need_resp = TRUE;
	ClientCapability *ClientCapability_out;
	Status *Status_out;	
	PUCHAR PswdData;

	Data = (PUCHAR)(Dataheader) + CP_DATA_HDR_LEN; 
	Data_out = (UCHAR *)(iot_buffer + CP_HDR_LEN + CP_DATA_HDR_LEN);

	ClientCapability_out = (ClientCapability *) Data_out;
	Status_out = (Status *) Data_out;
	PswdData = (UCHAR *)Data;

	
	DBGPRINT(RT_DEBUG_INFO, ("IoT_CmdPWD: %x,%x,%x,%x,%x\n",   Dataheader->Length, 
		                    IoTpAd.ComCfg.CmdPWD[0],IoTpAd.ComCfg.CmdPWD[1], 
		                    IoTpAd.ComCfg.CmdPWD[2],IoTpAd.ComCfg.CmdPWD[3]));
	
	DBGPRINT(RT_DEBUG_INFO, ("PacketInfo.SessionID: %x\n",pPacketInfo->SessionID));
	
		if(ManagementType != QUERY_CAPAB_REQUEST && 
			 (memcmp(&pPacketInfo->SessionID, IoTpAd.ComCfg.CmdPWD, sizeof(IoTpAd.ComCfg.CmdPWD))))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("wrong iot device password\n"));
			return 0;
		}

		switch (ManagementType)
		{
			case QUERY_CAPAB_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("CAPAB_REQ\n"));
				IoT_build_app_response_header(iot_buffer, MANAGEMENT, QUERY_CAPAB_RESPONSE,
					CLIENT_CAPABILITY, sizeof(ClientCapability), pPacketInfo);														
	
				ClientCapability_out->VendorNameLen = sizeof(IoTpAd.UsrCfg.VendorName);
				memcpy(ClientCapability_out->VendorName,  IoTpAd.UsrCfg.VendorName,  ClientCapability_out->VendorNameLen);
				
				ClientCapability_out->ProductTypeLen = sizeof(IoTpAd.UsrCfg.ProductType);
				memcpy(ClientCapability_out->ProductType, IoTpAd.UsrCfg.ProductType, ClientCapability_out->ProductTypeLen);
				
				ClientCapability_out->ProductNameLen = sizeof(IoTpAd.UsrCfg.ProductName);
				memcpy(ClientCapability_out->ProductName, IoTpAd.UsrCfg.ProductName, ClientCapability_out->ProductNameLen);
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(ClientCapability));
	
				break;
			
			case CONTROL_CLIENT_OFFLINE_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("OFFLINE_REQUEST\n")); 
				
				IoT_build_app_response_header(iot_buffer, MANAGEMENT, CONTROL_CLIENT_OFFLINE_RESPONSE, STATUS, sizeof(Status), pPacketInfo);
				
				Status_out->StatusCode = 0;
				
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(Status));
#if CFG_SUPPORT_TCPIP

#if (AES_DATAPARSING_SUPPORT == 1)
				AES_uip_send(iot_buffer, payload_len);
#else
				uip_send(iot_buffer, payload_len);
#endif

#else
				IoT_build_send_udp_packet(src_addr, dest_addr, TCPUDP_SRC_PORT, TCPUDP_DST_PORT, iot_buffer, payload_len);
#endif		
				/* reset sta config in the flash, and 7681 shall go into SMNT state again*/
				reset_sta_cfg();
				IoT_Cmd_LinkDown(0);
				
				Need_resp = FALSE;
				
				break;
			case CONTROL_PASSWORD_SET_REQUEST:
				
				DBGPRINT(RT_DEBUG_TRACE,("PASSWORD_SET_REQUEST\n")); 
								
				Status_out->StatusCode = 0;
	
				DBGPRINT(RT_DEBUG_TRACE, ("memcpy IoT_CmdPWD_Recv: %x,%x,%x,%x,%x\n", Dataheader->Length, PswdData[0],PswdData[1], PswdData[2],PswdData[3]));
	
				memcpy(IoT_CmdPWD_Recv, PswdData, PASSWORD_MAX_LEN);
				

				
				IoT_build_app_response_header(iot_buffer, MANAGEMENT, CONTROL_PASSWORD_SET_RESPONSE, STATUS, sizeof(Status), pPacketInfo);
	
				payload_len = (UINT16)(CP_HDR_LEN+CP_DATA_HDR_LEN+sizeof(Status));
				
				break;
	
			case CONTROL_PASSWORD_SET_CONFIRM:
				
				DBGPRINT(RT_DEBUG_TRACE,("PASSWORD_SET_CONFIRM\n")); 
				DBGPRINT(RT_DEBUG_TRACE, ("memcpy IoT_CmdPWD: %x,%x,%x,%x\n", 
										IoTpAd.ComCfg.CmdPWD[0], IoTpAd.ComCfg.CmdPWD[1], 
										IoTpAd.ComCfg.CmdPWD[2], IoTpAd.ComCfg.CmdPWD[3]));
				
				DBGPRINT(RT_DEBUG_TRACE, ("memcpy IoT_CmdPWD_Recv: %x,%x,%x,%x\n", 
										IoT_CmdPWD_Recv[0], IoT_CmdPWD_Recv[1], 
										IoT_CmdPWD_Recv[2], IoT_CmdPWD_Recv[3]));
				
				memcpy(IoTpAd.ComCfg.CmdPWD, IoT_CmdPWD_Recv, PASSWORD_MAX_LEN);

				/*store the Cmd Password*/
				spi_flash_write( FLASH_COM_CFG_BASE+FLASH_COM_CFG_CMD_PWD, 
					             IoTpAd.ComCfg.CmdPWD, 
					             sizeof(IoTpAd.ComCfg.CmdPWD));
				
				Need_resp = FALSE;
				
				break;
	
				
		}
		if(Need_resp == TRUE)
		{	
#if CFG_SUPPORT_TCPIP
#if 0
			{
				UCHAR i;
				printf("payload_len: %d\n", payload_len);
				for (i=0;i<payload_len;i++) {
					printf("0x%02x ", iot_buffer[i]);
				}
				printf("\n");
			}
#endif

#if (AES_DATAPARSING_SUPPORT == 1)
			AES_uip_send(iot_buffer, payload_len);
#else
			uip_send(iot_buffer, payload_len);
#endif

#else
			IoT_build_send_udp_packet(src_addr, dest_addr, TCPUDP_SRC_PORT, TCPUDP_DST_PORT, iot_buffer, payload_len);	
#endif
		}
		
		return 0;


}




INT32 IoT_process_app_packet(
#if ENABLE_DATAPARSING_SEQUENCE_MGMT	
								UCHAR sock_num,
#endif
								PUCHAR packet ,
								UINT16 rawpacketlength
								)
{
	DataHeader * Dataheader;
	IoTPacketInfo  PacketInfo;
	IoTCtrlProtocolHeader * ProtocolHeader;
	UINT8 Subtype;
	UCHAR BROADCAST_ADDR[MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
#if (AES_DATAPARSING_SUPPORT == 1)

	UCHAR Plain[IOT_BUFFER_LEN];
	UINT16 PlainLen = 0;

	AES_receive(packet, &rawpacketlength, Plain, &PlainLen);

	packet = Plain;
	rawpacketlength = PlainLen;
	
#endif

	ProtocolHeader = (IoTCtrlProtocolHeader *)packet;
	Dataheader = (DataHeader *)(packet+CP_HDR_LEN);
	//printf("rawpacketlength:%d\n",rawpacketlength);
	appLen = rawpacketlength;
	//Printf_High("IoT_process_app_packet\n");
	if(rawpacketlength <= 4)
		return -1;


	
	DBGPRINT(RT_DEBUG_TRACE,("Mg:%x\n",ProtocolHeader->Magic));
	if(ProtocolHeader->Magic != IOT_MAGIC_NUMBER)
		return -2;
	
	if(MAC_ADDR_EQUAL(gCurrentAddress,ProtocolHeader->SendMAC))
	{
		DBGPRINT(RT_DEBUG_ERROR,("drop self's packet\n"));
		return -3;
	}
	

	if(!MAC_ADDR_EQUAL(gCurrentAddress, ProtocolHeader->ReceiveMAC) && 
		!MAC_ADDR_EQUAL(BROADCAST_ADDR, ProtocolHeader->ReceiveMAC))
	{
		DBGPRINT(RT_DEBUG_ERROR,("ReceiveMAC error\n"));
		return -4;
	}	
	PacketInfo.SessionID = ProtocolHeader->SessionID;
	PacketInfo.Sequence = ProtocolHeader->Sequence;
	
	COPY_MAC_ADDR(PacketInfo.ReceiveMAC, ProtocolHeader->ReceiveMAC);
	COPY_MAC_ADDR(PacketInfo.SendMAC, ProtocolHeader->SendMAC);
	
	DBGPRINT(RT_DEBUG_TRACE,("sendmac: %02x:%02x:%02x:%02x:%02x:%02x \n",PRINT_MAC(PacketInfo.SendMAC)));
	DBGPRINT(RT_DEBUG_TRACE,("receiveMAC: %02x:%02x:%02x:%02x:%02x:%02x \n",PRINT_MAC(PacketInfo.ReceiveMAC)));
	 

	/*no need handler the packet with the same seqence number as before*/
#if ENABLE_DATAPARSING_SEQUENCE_MGMT	
	preSeq = IoT_cp_app_search_seq(sock_num);

	if(!(PacketInfo.Sequence == 0 ||PacketInfo.Sequence > preSeq))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("wrong packet sequence: %x,%x\n", PacketInfo.Sequence, preSeq));
		return 0;
	}
#endif



	Subtype = ProtocolHeader->SubHdr.field.SubType;
	


	if(ProtocolHeader->SubHdr.field.Type == FUNCTION)
	{

		IoT_process_app_function_packet(Dataheader, Subtype, &PacketInfo);
	
	}
	
	else if(ProtocolHeader->SubHdr.field.Type == MANAGEMENT)
	{

	 	IoT_process_app_management_packet(Dataheader, Subtype, &PacketInfo);

	}
#if ENABLE_DATAPARSING_SEQUENCE_MGMT
	IoT_cp_app_set_seq(sock_num, PacketInfo.Sequence);
#else
	preSeq = PacketInfo.Sequence;
#endif


#if (AES_DATAPARSING_SUPPORT == 1)
	//free(Plain);
#endif

	return 0;
	
}



