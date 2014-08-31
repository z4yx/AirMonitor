#ifndef _IOT_CUSTOM_H_
#define _IOT_CUSTOM_H_


/******************************************************************************
* MODULE NAME:     iot_custom.h
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V0.0.1     Jan 2014    - Initial Version V0.0.1
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

/******************************************************************************
 * MACRO DEFINITION
 ******************************************************************************/
#include "xip_ovly.h"

/*for record AT cmd Option count*/
#define		MAX_OPTION_COUNT 		(15)
#define		AT_CMD_MAX_LEN 			(150)


#define     FW_VERISON_CUST     	"IoT_V1.50"
#define 	DEFAULT_DHCP_HOST_NAME  "MT7681"

/******************************************************************************
 * MACRO DEFINITION  for AT Comamnd
 ******************************************************************************/
#define		AT_CMD_PREFIX_MAX_LEN	12
#define		AT_CMD_PREFIX			"AT#"
#define		AT_CMD_PREFIX2			"iwpriv ra0 "

/* AT command Type */
/* if one or more paramter following the command type*/
/* please reserve a space after command type*/
#define		AT_CMD_UART				"Uart "
#define		AT_CMD_NETMODE			"Netmode "
#define		AT_CMD_WIFI_CFG			"WifiConf "
#define		AT_CMD_CON_AP			"ConAP "
#define		AT_CMD_CHANNEL			"Channel "
#define		AT_CMD_SOFTAP_CFG		"SoftAPConf "
#define		AT_CMD_DHCP				"Dhcp "
#define		AT_CMD_STATIC_IP		"StaticIP "
#define		AT_CMD_UART2WIFI		"Uart2WiFi"
#define		AT_CMD_VER				"Ver"
#define		AT_CMD_REBOOT			"Reboot"
#define		AT_CMD_DEFAULT			"Default"
#define		AT_CMD_SET_SMNT			"Smnt"


#define		AT_CMD_TCPUDP			"TcpUdp "        /*set tcp/udp*/
#define		AT_CMD_TCPUDP_MODE		"TcpUdpMode "    /*set server/client*/
#define		AT_CMD_SLEEP			"Sleep "
#define		AT_CMD_STANDBY			"Standby "
#define		AT_CMD_WAKEUP			"WakeUp"
#define		AT_CMD_IFCONFIG			"ifconfig"
#define		AT_CMD_IWLIST			"iwlist"
#define		AT_CMD_SHOW				"Show"
#define		AT_CMD_TCPCONNECT		"Tcp_Connect "
#define		AT_CMD_TCPSEND			"Tcp_Send "
#define		AT_CMD_TCPDISCONNECT	"Tcp_Disconnect "
#define		AT_CMD_TCPLISTEN		"Tcp_Listen "

#define		AT_CMD_UDPCREATE		"Udp_Create "
#define		AT_CMD_UDPSEND			"Udp_Send "
#define		AT_CMD_UDPREMOVE		"Udp_Remove "
#define		AT_CMD_UPDATEFW			"UpdateFW"

#define		AT_CMD_ATE_CAL			"ATECAL "
#define		AT_CMD_EFUSE_SET		"EFUSE "
#define		AT_CMD_FLASH_SET		"FLASH "

#define		AT_CMD_STATIC_IP		"StaticIP "
#define		AT_CMD_JTAGMODE			"Jtag"
#define		AT_CMD_NETSTAT			"Netstat "

/*Only for Debug*/
#define		AT_CMD_MAC_SET			"MAC "
#define		AT_CMD_BBP_SET			"BBP "
#define		AT_CMD_RF_SET			"RF "
#define		AT_CMD_SLT_CTL			"SLT"
#define		AT_CMD_PS_SET			"PowerSaving "



#define		AT_CMD_YEELINK_PUB		"YeelinkPub "
#define		AT_CMD_NTP_QUERY		"NtpQuery "


/*Default output value for GPIO0~4*/
#define		DEFAULT_GPIO04_OUTVAL   (1)

/*Frequency for hardware timer 1 interrupt,  1 ~ 10*/
#define TICK_HZ_HWTIMER1	10 	/* T=1/TICK_HZ_HWTIMER1  */


typedef enum t_UartRxMode
{
	UARTRX_ATCMD_MODE = 1,
	UARTRX_PUREDATA_MODE
}UartRxMode;

#if 0
/*reset station config type*/
typedef enum t_ResetType
{
	RESET_CFG_ALL,		/*reset all config in sta cfg region of the flash*/
	RESET_CFG_1,		/*reset part of config in sta cfg region of the flash,  such as keep setting about CMD password*/
	RESET_CFG_MAX
}RESET_TYPE;
#endif


/******************************************************************************
 * FUNCTION  DEFINITION
 ******************************************************************************/
VOID IoT_Cust_Ops(VOID);
VOID IoT_Cust_Init(VOID);
VOID CustTimer0TimeoutAction(UINT_32 param, UINT_32 param2);


VOID Pre_init_cfg(VOID);
BOOLEAN load_com_cfg(VOID) XIP_ATTRIBUTE(".xipsec0");
BOOLEAN load_usr_cfg(VOID) XIP_ATTRIBUTE(".xipsec0");
BOOLEAN reset_usr_cfg(BOOLEAN bUpFlash) XIP_ATTRIBUTE(".xipsec0");
BOOLEAN reset_com_cfg(BOOLEAN bUpFlash) XIP_ATTRIBUTE(".xipsec0");

BOOLEAN default_boot_cfg(VOID);
BOOLEAN default_uart_cfg(VOID);
BOOLEAN default_ip_cfg(VOID);


VOID IoT_Cust_uart2wifi_init(UINT_32 interval, UINT_32 triger_count);
VOID IoT_Cust_uart2wifi_set_mode(UartRxMode mode);
VOID IoT_Cust_uart2wifi_timer_action(UINT_32 param1, UINT_32 param2);
VOID IoT_Cust_uart2wifi_buffer_trigger_action(VOID);
VOID IoT_Cust_uart2wifi_data_handler(UCHAR *uart_content, UINT16 uart_content_count);
VOID IoT_Cust_uart2wifi_change_mode_handler(VOID);
INT32 IoT_Cust_uart2wifi_detect_gpio_input_change(VOID);


#endif  /* _IOT_CUSTOM_H_ */
