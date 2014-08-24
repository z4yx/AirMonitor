#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

#include "types.h"


/***************************************************************************
  *	Flash Mapping
  **************************************************************************/

#if 0

#define FLASH_OFFSET_BASE				(0x0000)
#define FLASH_OFFSET_LOADER_START		(0x0000)              //20KB
#define FLASH_OFFSET_RESERVE_1_START	(0x5000)              //4KB

#define FLASH_OFFSET_EEP_CFG_START		(0x6000)              //4KB
#define FLASH_OFFSET_COM_CFG_START		(0x7000)              //4KB
#define FLASH_OFFSET_STA_CFG_START		(0x8000)              //4KB
#define FLASH_OFFSET_AP_CFG_START		(0x9000)              //4KB
#define FLASH_OFFSET_USR_CFG_START		(0xA000)              //4KB
#define FLASH_OFFSET_RESERVE_2_START	(0xB000)              //12KB

#define FLASH_OFFSET_UPG_FW_START		(0xE000)              //64KB
#define FLASH_OFFSET_RESERVE_3_START	(0x1E000)             //4KB

#define FLASH_OFFSET_STA_FW_START		(0x1F000)             //64KB
#define FLASH_OFFSET_RESERVE_4_START	(0x2F000)             //4KB
#define FLASH_OFFSET_STA_XIP_FW_START	(0x30000)             //120KB
#define FLASH_OFFSET_RESERVE_5_START	(0x4E000)             //4KB

#define FLASH_OFFSET_AP_FW_START		(0x4F000)             //64KB
#define FLASH_OFFSET_RESERVE_6_START	(0x5F000)             //4KB
#define FLASH_OFFSET_AP_XIP_FW_START	(0x60000)             //120KB
#define FLASH_OFFSET_RESERVE_7_START	(0x7E000)             //4KB

#define FLASH_OFFSET_WRITE_BUF_START	(0x7F000)             //4KB
#define FLASH_OFFSET_RESERVE_8_START	(0x80000)             //0KB
#define FLASH_OFFSET_END				(0x80000)             //0KB

#else

#define FLASH_OFFSET_BASE				(0x0000)
#define FLASH_OFFSET_LOADER_START		(0x0000)              //20KB
#define FLASH_OFFSET_RESERVE_1_START	(0x5000)              //4KB

#define FLASH_OFFSET_UPG_FW_START		(0x6000)              //64KB
#define FLASH_OFFSET_RESERVE_2_START	(0x16000)             //4KB


#define FLASH_OFFSET_EEP_CFG_START		(0x17000)             //4KB
#define FLASH_OFFSET_COM_CFG_START		(0x18000)             //4KB
#define FLASH_OFFSET_STA_CFG_START		(0x19000)             //4KB
#define FLASH_OFFSET_AP_CFG_START		(0x1A000)             //4KB
#define FLASH_OFFSET_USR_CFG_START		(0x1B000)             //4KB
#define FLASH_OFFSET_RESERVE_3_START	(0x1C000)             //12KB


#define FLASH_OFFSET_STA_FW_START		(0x1F000)             //64KB
#define FLASH_OFFSET_RESERVE_4_START	(0x2F000)             //4KB
#define FLASH_OFFSET_STA_XIP_FW_START	(0x30000)             //120KB
#define FLASH_OFFSET_RESERVE_5_START	(0x4E000)             //4KB

#define FLASH_OFFSET_AP_FW_START		(0x4F000)             //64KB
#define FLASH_OFFSET_RESERVE_6_START	(0x5F000)             //4KB
#define FLASH_OFFSET_AP_XIP_FW_START	(0x60000)             //120KB
#define FLASH_OFFSET_RESERVE_7_START	(0x7E000)             //4KB

#define FLASH_OFFSET_WRITE_BUF_START	(0x7F000)             //4KB
#define FLASH_OFFSET_RESERVE_8_START	(0x80000)             //0KB
#define FLASH_OFFSET_END				(0x80000)             //0KB
#endif


/***************************************************************************
  *	Flash SECTOR or BLOCK
  **************************************************************************/
/*For flash model: EN25T16*/
#define FLASH_TOTAL_SIZE        (FLASH_OFFSET_END)
#define FLASH_SECTOR_SIZE		(0x1000)   //4KB
#define FLASH_BLOCK_SIZE		(0x10000)  //64KB
#define FLASH_RW_BUF_START		(FLASH_OFFSET_WRITE_BUF_START)

#define FLASH_LOADER_SIZE		(FLASH_OFFSET_RESERVE_1_START - FLASH_OFFSET_LOADER_START)
#define FLASH_UPG_FW_SIZE		(FLASH_OFFSET_RESERVE_2_START - FLASH_OFFSET_UPG_FW_START)
#define FLASH_EEP_SIZE			(FLASH_OFFSET_COM_CFG_START   - FLASH_OFFSET_EEP_CFG_START)
//#define FLASH_STA_FW_SIZE		(FLASH_OFFSET_RESERVE_4_START - FLASH_OFFSET_STA_FW_START)
#define FLASH_STA_FW_SIZE		(FLASH_OFFSET_RESERVE_5_START - FLASH_OFFSET_STA_FW_START)  /*include XIP/OVL region*/
#define FLASH_STA_XIP_FW_SIZE	(FLASH_OFFSET_RESERVE_5_START - FLASH_OFFSET_STA_XIP_FW_START)
//#define FLASH_AP_FW_SIZE		(FLASH_OFFSET_RESERVE_6_START - FLASH_OFFSET_AP_FW_START)
#define FLASH_AP_FW_SIZE		(FLASH_OFFSET_RESERVE_7_START - FLASH_OFFSET_AP_FW_START)   /*include XIP/OVL region*/
#define FLASH_AP_XIP_FW_SIZE	(FLASH_OFFSET_RESERVE_7_START - FLASH_OFFSET_AP_XIP_FW_START)
#define FLASH_RW_BUF_SIZE		(FLASH_OFFSET_RESERVE_8_START - FLASH_OFFSET_WRITE_BUF_START)

#define RAM_RW_BUF_SIZE			(256)  /* must (n*256) , Ram buffer used during flash writting */


/***************************************************************************
  *	The Detail layout for Common Config
  **************************************************************************/
#define FLASH_COM_CFG_BASE					(FLASH_OFFSET_COM_CFG_START)

#define FLASH_COM_CFG_INFO_STORED			(0x0)
#define FLASH_COM_CFG_INFO_STORED_LEN			(0x1)

#define FLASH_COM_CFG_BOOT_IDX				(0x1)
#define FLASH_COM_CFG_BOOT_IDX_LEN				(0x1)

#define FLASH_COM_CFG_RECOVERY_MODE_STATUS	(0x2)
#define FLASH_COM_CFG_RECOVERY_MODE_STATUS_LEN	(0x1)

#define FLASH_COM_CFG_IO_SET				(0x3)
#define FLASH_COM_CFG_IO_SET_LEN				(0x1)

#define FLASH_COM_CFG_RESERVE_1				(0x4)
#define FLASH_COM_CFG_RESERVE_1_LEN				(0x14)



#define FLASH_COM_CFG_UART_BASE				(0x18)

#define FLASH_COM_CFG_UART_BAUD				(0x18)
#define FLASH_COM_CFG_UART_BAUD_LEN				(0x4)

#define FLASH_COM_CFG_UART_DATA_BITS		(0x1C)
#define FLASH_COM_CFG_UART_DATA_BITS_LEN		(0x1)

#define FLASH_COM_CFG_UART_PARITY			(0x1D)
#define FLASH_COM_CFG_UART_PARITY_LEN			(0x1)

#define FLASH_COM_CFG_UART_STOP_BITS		(0x1E)
#define FLASH_COM_CFG_UART_STOP_BITS_LEN		(0x1)

#define FLASH_COM_CFG_RESERVE_2				(0x1F)
#define FLASH_COM_CFG_RESERVE_2_LEN				(0x14)


#define FLASH_COM_CFG_IP_BASE				(0x33)

#define FLASH_COM_CFG_TCPUDP_CS_SET			(0x33)
#define FLASH_COM_CFG_TCPUDP_CS_SET_LEN			(0x1)

#define FLASH_COM_CFG_IOT_TCP_SRV_PORT		(0x34)
#define FLASH_COM_CFG_IOT_TCP_SRV_PORT_LEN		(0x2)

#define FLASH_COM_CFG_LOCAL_TCP_SRV_PORT	(0x36)
#define FLASH_COM_CFG_LOCAL_TCP_SRV_PORT_LEN	(0x2)

#define FLASH_COM_CFG_IOT_UDP_SRV_PORT		(0x38)
#define FLASH_COM_CFG_IOT_UDP_SRV_PORT_LEN		(0x2)

#define FLASH_COM_CFG_LOCAL_UDP_SRV_PORT	(0x3A)
#define FLASH_COM_CFG_LOCAL_UDP_SRV_PORT_LEN	(0x2)

#define FLASH_COM_CFG_USE_DHCP				(0x3C)
#define FLASH_COM_CFG_USE_DHCP_LEN				(0x1)

#define FLASH_COM_CFG_STATIC_IP				(0x3D)
#define FLASH_COM_CFG_STATIC_IP_LEN				(MAC_IP_LEN)

#define FLASH_COM_CFG_SUBNET_MASK_IP		(0x41)
#define FLASH_COM_CFG_SUBNET_MASK_IP_LEN		(MAC_IP_LEN)

#define FLASH_COM_CFG_DNS_SERVER_IP			(0x45)
#define FLASH_COM_CFG_DNS_SERVER_IP_LEN			(MAC_IP_LEN)

#define FLASH_COM_CFG_GATEWAY_IP			(0x49)
#define FLASH_COM_CFG_GATEWAY_IP_LEN			(MAC_IP_LEN)

#define FLASH_COM_CFG_IOT_SERVER_IP			(0x4D)
#define FLASH_COM_CFG_IOT_SERVER_IP_LEN			(MAC_IP_LEN)

#define FLASH_COM_CFG_IOT_SERVER_DOMAIN		(0x51)
#define FLASH_COM_CFG_IOT_SERVER_DOMAIN_LEN		(0x80)   //128 Bytes

#define FLASH_COM_CFG_RESERVE_3				(0xD1)
#define FLASH_COM_CFG_RESERVE_3_LEN				(0x14)

#define FLASH_COM_CFG_CMD_PWD				(0xE5)
#define FLASH_COM_CFG_CMD_PWD_LEN				(PASSWORD_MAX_LEN)

#define FLASH_COM_CFG_RESERVE_4				(0xE9)
#define FLASH_COM_CFG_RESERVE_4_LEN				(0x0)



/***************************************************************************
  *	The Detail layout for Station Config
  **************************************************************************/
#define FLASH_STA_CFG_BASE				(FLASH_OFFSET_STA_CFG_START)

#define FLASH_STA_CFG_SMNT_INFO_STORED	(0x0)
#define FLASH_STA_CFG_SMNT_INFO_STORED_LEN	(0x1)

#define FLASH_STA_CFG_BSSID				(0x1)
#define FLASH_STA_CFG_BSSID_LEN				(MAC_ADDR_LEN)      //6 Bytes

#define FLASH_STA_CFG_SSID				(0x7)
#define FLASH_STA_CFG_SSID_LEN  			(MAX_LEN_OF_SSID)   //32 Bytes

#define FLASH_STA_CFG_SSIDLEN			(0x27)
#define FLASH_STA_CFG_SSIDLEN_LEN			(0x1)

#define FLASH_STA_CFG_PASSPHASE			(0x28)
#define FLASH_STA_CFG_PASSPHASE_LEN 		(CIPHER_TEXT_LEN)	//32Bytes

#define FLASH_STA_CFG_PASSPHASELEN		(0x48)
#define FLASH_STA_CFG_PASSPHASELEN_LEN 		(0x1)

#define FLASH_STA_CFG_AUTH_MODE			(0x49)
#define FLASH_STA_CFG_AUTH_MODE_LEN			(0x1)

#define FLASH_STA_CFG_4WAY_PMK			(0x4A)
#define FLASH_STA_CFG_4WAY_PMK_LEN			(0x32)

#define FLASH_STA_CFG_RESERVE_1			(0x6A)
#define FLASH_STA_CFG_RESERVE_1_LEN			(0x0)




/***************************************************************************
  *	The Detail layout for AP Config
  **************************************************************************/
#define FLASH_AP_CFG_BASE				(FLASH_OFFSET_AP_CFG_START)

#define FLASH_AP_CFG_INFO_STORED		(0x0)
#define FLASH_AP_CFG_INFO_STORED_LEN		(0x1)

#define FLASH_AP_CFG_BSSID				(0x1)
#define FLASH_AP_CFG_BSSID_LEN 				(MAC_ADDR_LEN)     //6 Bytes

#define FLASH_AP_CFG_SSID				(0x7)
#define FLASH_AP_CFG_SSID_LEN  				(MAX_LEN_OF_SSID)  //32 Bytes

#define FLASH_AP_CFG_SSIDLEN			(0x27)
#define FLASH_AP_CFG_SSIDLEN_LEN  			(0x1)

#define FLASH_AP_CFG_PHY_MODE			(0x28)
#define FLASH_AP_CFG_PHY_MODE_LEN  			(0x1)

#define FLASH_AP_CFG_AP_CH				(0x29)
#define FLASH_AP_CFG_AP_CH_LEN  			(0x1)

#define FLASH_AP_CFG_AUTH_MODE			(0x2A)
#define FLASH_AP_CFG_AUTH_MODE_LEN  		(0x1)

#define FLASH_AP_CFG_PASSPHASE			(0x2B)
#define FLASH_AP_CFG_PASSPHASE_LEN			(CIPHER_TEXT_LEN)  //32Bytes

#define FLASH_AP_CFG_PASSPHASELEN		(0x4B)
#define FLASH_AP_CFG_PASSPHASELEN_LEN 		(0x1)

#define FLASH_AP_CFG_BCN_INTERVAL		(0x4C)
#define FLASH_AP_CFG_BCN_INTERVAL_LEN		(0x2)

#define FLASH_AP_CFG_DTIM_COUNT			(0x4E)
#define FLASH_AP_CFG_DTIM_COUNT_LEN			(0x1)

#define FLASH_AP_CFG_DTIM_INTERVAL		(0x4F)
#define FLASH_AP_CFG_DTIM_INTERVAL_LEN		(0x1)

#define FLASH_AP_CFG_CAP_INFO			(0x50)
#define FLASH_AP_CFG_CAP_INFO_LEN			(0x2)

#define FLASH_AP_CFG_FGIS_HID_SSID		(0x52)
#define FLASH_AP_CFG_FGIS_HID_SSID_LEN		(0x1)

#define FLASH_AP_CFG_RESERVE_1			(0x53)
#define FLASH_AP_CFG_RESERVE_1_LEN			(0x0)




/***************************************************************************
  *	The Detail layout for User Config
  **************************************************************************/
#define FLASH_USR_CFG_BASE				(FLASH_OFFSET_USR_CFG_START)

#define FLASH_USR_CFG_PRODUCT_INFO_STORED	(0x0)
#define FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN	(0x1)

#define FLASH_USR_CFG_VENDOR_NAME		(0x1)
#define FLASH_USR_CFG_VENDOR_NAME_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_PRODUCT_TYPE		(0x21)
#define FLASH_USR_CFG_PRODUCT_TYPE_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_PRODUCT_NAME		(0x41)
#define FLASH_USR_CFG_PRODUCT_NAME_LEN 		(0x20)  //32 Byte

#define FLASH_USR_CFG_RESERVE_1			(0x61)
#define FLASH_USR_CFG_RESERVE_1_LEN 		(0x0)




/***************************************************************************
  *	The Detail layout for EEPROM Config
  **************************************************************************/
#define FLASH_EEP_CFG_BASE				(FLASH_OFFSET_EEP_CFG_START)
/*detail layout   refers to eeprom.h */





/***************************************************************************
  *	The Value stored on flash defination
  **************************************************************************/
/*Indicate the smart connection info already be initialized/stored on flash.
   and no need do smart connection while IoT device power on,  
   but go to SCAN state with the infor stored on flash	 */
#define SMNT_INFO_STORED				(0x12)

/*Indicate the Production info already be initialized/stored on flash
   and no need do data check while IoT device power on*/
#define PRODUCT_INFO_STORED				(0x23)

/*Indicate the Common config info already be initialized/stored on flash. */
#define COMMON_INFO_STORED				(0x34)

/*Indicate the AP info already be initialized/stored on flash */
#define AP_INFO_STORED					(0x56)


/***************************************************************************
  *	enum for Uart FW upgrade
  **************************************************************************/
typedef enum _UART_FlASH_UPG_ID_{
	UART_FlASH_UPG_ID_LOADER = 1,
	UART_FlASH_UPG_ID_RECOVERY_FW,
	UART_FlASH_UPG_ID_EEPROM,
	UART_FlASH_UPG_ID_STA_FW,
	UART_FlASH_UPG_ID_STA_XIP_FW,
	UART_FlASH_UPG_ID_AP_FW,
	UART_FlASH_UPG_ID_AP_XIP_FW,
	UART_FlASH_UPG_ID_MAX
}UART_FlASH_UPG_ID; 


/***************************************************************************
  *	The Header of  FW Binary for Uart FW ugprade
  **************************************************************************/
#define UART_FW_HEADER_DATA_SIZE	(0x3)
#define UART_FW_HEADER_LEN_SIZE	    (0x4)
#define UART_FW_HEADER_TYPE_SIZE	(0x1)
#define UART_FW_HEADER_PADDING_SIZE	(0x78)   /*will ignore the padding*/
#define UART_FW_HEADER_TOTAL_SIZE	(UART_FW_HEADER_DATA_SIZE + UART_FW_HEADER_LEN_SIZE + UART_FW_HEADER_TYPE_SIZE)

/*|  3BYTE	 |	  4BYTE (Low-->High)   |   1 BYTE  |  120 BYTE |*/
/*|  Header 	 |		 Length 			|	Type  |  PADDING |*/
#define UART_FW_HEADER			{0x76,0x81,0xAA,0x00,0x00,0x00,0x00,0x00}

#endif /* _FLASH_MAP_H_ */

