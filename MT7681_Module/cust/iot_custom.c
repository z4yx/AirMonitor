#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#include "flash_map.h"
#include "uip-conf.h"
#include "iot_custom.h"
#ifdef CONFIG_SOFTAP
#include "ap_pub.h"
#endif

/******************************************************************************
* MODULE NAME:     iot_customer.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2013
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2011 Mediatek Tech. Inc.
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

/************************************************/
/* Macros */
/************************************************/

/*Notice:  You must very carefully to OPEN this macro
  Any conflict between flash layout and  struct of IOT_COMMON_CFG / IOT_USR_CFG
  will cause mistake */
/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
  can use the following method to shrink code size */
#define FLASH_STRUCT_MAPPING       1

/* Calcurate PMK by 7681 software, it will spend 6sec */
//#define PMK_CAL_BY_SW

/*Default setting of User Config Block*/
#define DEFAULT_VENDOR_NEME			"Mediatek"
#define DEFAULT_PRODUCT_TYPE    	"IoT 1"
#define DEFAULT_PRODUCT_NAME        "MT7681"


/*Default setting of Common Config Block*/
#define DEFAULT_BOOT_FW_IDX            0	/*1: BootFromAP,   other: BootFromSTA*/
#define DEFAULT_RECOVERY_MODE_STATUS   0	/*not used*/
#define DEFAULT_IO_MODE                0    /*not used*/

#define DEFAULT_UART_BAUDRATE       UART_BAUD_115200
#define DEFAULT_UART_DATA_BITS      len_8
#define DEFAULT_UART_PARITY         pa_none
#define DEFAULT_UART_STOP_BITS      sb_1

#define DEFAULT_TCP_UDP_CS          1       /*0: UDP,  1:TCP  (Default 3*Client, 1*Server is Open)*/
#define DEFAULT_IOT_TCP_SRV_PORT    7681    /*The IoT Server TCP Port  in the internet */
#define DEFAULT_LOCAL_TCP_SRV_PORT  7681    /*The TCP Port  if 7681 as a TCP server */
#define DEFAULT_IOT_UDP_SRV_PORT    7681    /*The IoT Server UDP Port  in the internet */
#define DEFAULT_LOCAL_UDP_SRV_PORT  7681    /*The UDP  Port  if 7681 as a UDP server */

#define DEFAULT_USE_DHCP            1		/*0: Static IP,  1:Dynamic IP*/
#define DEFAULT_STATIC_IP           {192,168,0,99}
#define DEFAULT_SUBNET_MASK_IP      {255,255,255,0}
#define DEFAULT_DNS_IP              {192,168,0,1}
#define DEFAULT_GATEWAY_IP          {192,168,0,1}
#define DEFAULT_IOT_SERVER_IP       {182,148,123,91}

#define DEFAULT_IOT_CMD_PWD         {0xFF,0xFF,0xFF,0xFF}


/*this is the Common CFG region default table */
IOT_COM_CFG Com_Cfg = {
						DEFAULT_BOOT_FW_IDX,
 						DEFAULT_RECOVERY_MODE_STATUS,
 						DEFAULT_IO_MODE,
 						
 						DEFAULT_UART_BAUDRATE,
 						DEFAULT_UART_DATA_BITS,
 						DEFAULT_UART_PARITY,
 						DEFAULT_UART_STOP_BITS,
 						
 						DEFAULT_TCP_UDP_CS,
 						DEFAULT_IOT_TCP_SRV_PORT,
						DEFAULT_LOCAL_TCP_SRV_PORT,
 						DEFAULT_IOT_UDP_SRV_PORT,
						DEFAULT_LOCAL_UDP_SRV_PORT,
 						DEFAULT_USE_DHCP,
 						DEFAULT_STATIC_IP,
 						DEFAULT_SUBNET_MASK_IP,
 						DEFAULT_DNS_IP,
 						DEFAULT_GATEWAY_IP,
						DEFAULT_IOT_SERVER_IP,
 						DEFAULT_IOT_CMD_PWD
					 };

/*this is the User CFG region default table */
IOT_USR_CFG Usr_Cfg = {
						DEFAULT_VENDOR_NEME,
						DEFAULT_PRODUCT_TYPE,
						DEFAULT_PRODUCT_NAME
					 };


/*Default setting of STA Config Block*/

/*Default setting of AP Config Block*/


/*bit0-  read Calibration settings (TxPower/Tx Freq Offset) from [0:Flash,  1:Efuse]*/
UINT8 gCaliFrEfuse = 0x00;


/*unit:ms  indicated recovery mode duration*/
#if (ATCMD_RECOVERY_SUPPORT==1)
UINT16 gRecoveryModeTime = 4000; 
#endif

/*Indicated if use specified SSID to do scan  (TRUE/FALSE)  */
/*the SSID is got from Smart connection state or Flash        */
BOOLEAN bSpecAP = TRUE; 


/************************************************/
/* Extern  Paramenters */
/************************************************/
IOT_CUST_OP   	IoTCustOp;
IOT_CUST_TIMER  IoTCustTimer;
IOT_ADAPTER   	IoTpAd;

extern MLME_STRUCT *pIoTMlme;

#ifdef CONFIG_SOFTAP
extern AP_ADMIN_CONFIG  *pIoTApCfg;
#else
extern STA_ADMIN_CONFIG *pIoTStaCfg;
#endif

/*TRUE: Printf_High()/DBGPRINT_HIGH() is enabled,    FALSE: Printf_High/DBGPRINT_HIGH()  is disabled*/
BOOLEAN         PRINT_FLAG = TRUE;


#if (UART_INTERRUPT == 1)
/* 
 * We can use UART TX POLL scheme(such as debug/test),default is FALSE
 */
BOOLEAN UART_TX_POLL_ENABLE = FALSE;

extern void UART_Rx_Packet_Dispatch(void);
#endif

/************************************************/
/* Functions */
/************************************************/

/* Sample code for custom SubTask and Timer */
VOID IoT_Cust_Sub_Task(VOID)
{

#if (UARTRX_TO_AIR_LEVEL == 2)
	IoT_Cust_uart2wifi_buffer_trigger_action();
#endif

#if (ATCMD_SUPPORT == 1)
#if (UART_INTERRUPT == 1)
    UART_Rx_Packet_Dispatch();
#else
	/* Handle the AT Command */
	ATcommandHandler();
#endif
#endif

#if (UARTRX_TO_AIR_LEVEL == 2)
	static UINT32 i = 0;
	if (i++ == 0)
		cnmTimerStartTimer (&IoTCustTimer.custTimer0, 100);
#endif



#if 0 //timer sample
	static UINT32 i = 0;

	if (i++ == 0)
		cnmTimerStartTimer (&IoTCustTimer.custTimer0, 100);
	else if (i++ == 20000)
		cnmTimerStopTimer(&IoTCustTimer.custTimer0);
#endif
}


VOID IoT_Cust_Ops(VOID)
{
	IoTCustOp.IoTCustPreInit = Pre_init_cfg;  /*The callback in system Boot state, do not print msg as Uart is not ready*/
	IoTCustOp.IoTCustInit 	 = IoT_Cust_Init; /*The callback in system initial state*/

	/*The callback for each of Wifi State Machine,  
	   It is not suggested to do the heavy process here,  It is OK if only set GPIO status*/
	IoTCustOp.IoTCustWifiSMInit    = NULL;
	IoTCustOp.IoTCustWifiSMSmnt    = IoT_Cust_SM_Smnt;
	IoTCustOp.IoTCustWifiSMConnect = NULL;
	//IoTCustOp.IoTCustWifiRxHandler = IoT_Cust_Rx_Handler;  //use STARxDoneInterruptHandle()
	
	/*Callback function while do wifi_state_chg(WIFI_STATE_SMNT, 0);*/
	IoTCustOp.IoTCustSMNTStaChgInit= IoT_Cust_SMNT_Sta_Chg_Init;   

	/* The callback in  the wifi main task , it shall be called every cycle of the wifi main task*/
    /* we set the ATcommandHandler here to receive Uart Data and process AT command */
	IoTCustOp.IoTCustSubTask1 = IoT_Cust_Sub_Task;
}



VOID IoT_Cust_Init(VOID)
{
	/*Beacon lost timeout timer*/
	cnmTimerInitTimer(&pIoTMlme->BeaconTimer, BeaconTimeoutAction,0, 0);

	/*for customer initialization*/
	cnmTimerInitTimer(&IoTCustTimer.custTimer0,  CustTimer0TimeoutAction, 0, 0);

#if (UARTRX_TO_AIR_LEVEL == 2)	
	IoT_Cust_uart2wifi_init(UART2WIFI_TIMER_INTERVAL, UART2WIFI_LEN_THRESHOLD);
#endif

	//load_com_cfg();
	load_usr_cfg();

#if (MT7681_POWER_SAVING == 1)
    pIoTMlme->PMLevel = 1;
#endif
}



VOID CustTimer0TimeoutAction(UINT_32 param, UINT_32 param2) 
{
	//Printf_High("CustTimer0TimeoutAction\n");
#if (UARTRX_TO_AIR_LEVEL == 2)
	IoT_Cust_uart2wifi_change_mode_handler();
#endif
	cnmTimerStartTimer (&IoTCustTimer.custTimer0, 100);
}


/*========================================================================
	Routine	Description:
		Pre_init_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
VOID Pre_init_cfg(VOID)
{
	memcpy(&IoTpAd.ComCfg , &Com_Cfg , sizeof(IOT_COM_CFG));
	memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

	/*read Uart setting in the flash, then config uart HW*/
	load_com_cfg();

#if (IOT_PWM_SUPPORT == 1)	
	IoTpAd.PWM_LVL = PWM_HIGHEST_LEVEL;
#endif
}


/*========================================================================
	Routine	Description:
		reset_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
VOID reset_cfg(VOID)
{	
	/* clean all content in the STA region of the flash */
	reset_sta_cfg();

	/* set default in the COMMON region of the flash */
	reset_com_cfg(TRUE);

	/* set default in the USER region of the flash */
	reset_usr_cfg(TRUE);
}

/*========================================================================
	Routine	Description:
		load_usr_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN load_usr_cfg(VOID)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));

	/* read settings stored on flash USER CONFIG BLOCK */
	//memset(IoTpAd.flash_rw_buf, 0, sizeof(IoTpAd.flash_rw_buf));
	spi_flash_read(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

#if 1  /*use stored flag to shrink code size*/
	if (IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] == PRODUCT_INFO_STORED)
	{
	#if FLASH_STRUCT_MAPPING
		/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
		  can use the following method to shrink code size */

		memcpy( IoTpAd.UsrCfg.VendorName,	
				&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],	
			    FLASH_USR_CFG_RESERVE_1 - FLASH_USR_CFG_VENDOR_NAME);
	#else
		/*if flash stored valid vendor name , product name and product type*/
		memcpy(IoTpAd.UsrCfg.VendorName,	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],	FLASH_USR_CFG_VENDOR_NAME_LEN);
		memcpy(IoTpAd.UsrCfg.ProductType,	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE],	FLASH_USR_CFG_PRODUCT_TYPE_LEN);
		memcpy(IoTpAd.UsrCfg.ProductName, 	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME],	FLASH_USR_CFG_PRODUCT_NAME_LEN);
	#endif
	}
#else
	if ((check_data_valid(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],  FLASH_USR_CFG_VENDOR_NAME_LEN)) &&
		(check_data_valid(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME], FLASH_USR_CFG_PRODUCT_NAME_LEN)) &&
		(check_data_valid(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE], FLASH_USR_CFG_PRODUCT_TYPE_LEN)))
	{
		/*if flash stored valid vendor name , product name and product type*/
		memcpy(IoTpAd.UsrCfg.VendorName, 	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],  	FLASH_USR_CFG_VENDOR_NAME_LEN);
		memcpy(IoTpAd.UsrCfg.ProductName, 	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME], 	FLASH_USR_CFG_PRODUCT_NAME_LEN);
		memcpy(IoTpAd.UsrCfg.ProductType, 	&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE], 	FLASH_USR_CFG_PRODUCT_TYPE_LEN);
	}
#endif
	else
	{
		reset_usr_cfg(TRUE);
	}


	return TRUE;
}

/*========================================================================
	Routine	Description:
		reset_usr_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/

BOOLEAN reset_usr_cfg(BOOLEAN bUpFlash)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));
	
	memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));
	
	memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

	if(bUpFlash == TRUE)
	{
		IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] = PRODUCT_INFO_STORED;
#if FLASH_STRUCT_MAPPING	
		/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
		  can use the following method to shrink code size */
		
		memcpy( &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],	
			    IoTpAd.UsrCfg.VendorName,
			    FLASH_USR_CFG_RESERVE_1 - FLASH_USR_CFG_VENDOR_NAME);
#else
		memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME] ,  Usr_Cfg.VendorName, FLASH_USR_CFG_VENDOR_NAME_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE] , Usr_Cfg.ProductType, FLASH_USR_CFG_PRODUCT_TYPE_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME] , Usr_Cfg.ProductName, FLASH_USR_CFG_PRODUCT_NAME_LEN);
#endif
		spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
	}
	
	return TRUE;
}

/*========================================================================
	Routine	Description:
		default_boot_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN default_boot_cfg(VOID)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));

	IoTpAd.ComCfg.BootFWIdx		    = DEFAULT_BOOT_FW_IDX;
	IoTpAd.ComCfg.RecovModeStatus	= DEFAULT_RECOVERY_MODE_STATUS;
	IoTpAd.ComCfg.IOMode		    = DEFAULT_IO_MODE;
}

BOOLEAN default_uart_cfg(VOID)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));

	IoTpAd.ComCfg.UART_Baudrate	= DEFAULT_UART_BAUDRATE;
	IoTpAd.ComCfg.UART_DataBits	= DEFAULT_UART_DATA_BITS;
	IoTpAd.ComCfg.UART_Parity 	= DEFAULT_UART_PARITY;
	IoTpAd.ComCfg.UART_StopBits	= DEFAULT_UART_STOP_BITS;
}

BOOLEAN default_ip_cfg(VOID)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));

#if FLASH_STRUCT_MAPPING	
    /*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
      can use the following method to shrink code size */
    
	memcpy( &IoTpAd.ComCfg.TCPUDP_CS,		
		    &Com_Cfg.TCPUDP_CS, 
		    (IoTpAd.ComCfg.IoT_ServeIP + MAC_IP_LEN - &IoTpAd.ComCfg.TCPUDP_CS));

            /*because IoTpAd.ComCfg.IoT_ServeDomain be canceled in IOT_COMMON_CFG, 
                        so the memcpy length is calculated by the position of IoTpAd.ComCfg.STATIC_IP*/
#else
	IoTpAd.ComCfg.TCPUDP_CS		= DEFAULT_TCP_UDP_CS; //Com_Cfg.TCPUDP_CS;
	
	IoTpAd.ComCfg.IoT_TCP_Srv_Port		= DEFAULT_IOT_TCP_SRV_PORT;   //Com_Cfg.IoT_TCP_Srv_Port;
	IoTpAd.ComCfg.Local_TCP_Srv_Port		= DEFAULT_LOCAL_TCP_SRV_PORT;   //Com_Cfg.Local_TCP_Srv_Port;
	IoTpAd.ComCfg.IoT_UDP_Srv_Port		= DEFAULT_IOT_UDP_SRV_PORT;   //Com_Cfg.IoT_UDP_Srv_Port;
	IoTpAd.ComCfg.Local_UDP_Srv_Port		= DEFAULT_LOCAL_UDP_SRV_PORT;   //Com_Cfg.Local_UDP_Srv_Port;
	
	IoTpAd.ComCfg.Use_DHCP = DEFAULT_USE_DHCP;    //Com_Cfg.Use_DHCP;
	memcpy(IoTpAd.ComCfg.STATIC_IP,		Com_Cfg.STATIC_IP, 	MAC_IP_LEN);
	memcpy(IoTpAd.ComCfg.SubnetMask_IP,	Com_Cfg.SubnetMask_IP, MAC_IP_LEN);
	memcpy(IoTpAd.ComCfg.DNS_IP,		Com_Cfg.DNS_IP,		MAC_IP_LEN);
	memcpy(IoTpAd.ComCfg.GateWay_IP,	Com_Cfg.GateWay_IP,	MAC_IP_LEN);
	memcpy(IoTpAd.ComCfg.IoT_ServeIP,	Com_Cfg.IoT_ServeIP,	MAC_IP_LEN);
#endif

	return TRUE;
}


/*========================================================================
	Routine	Description:
		load_com_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN load_com_cfg(VOID)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));

	/* read settings stored on flash Common CONFIG BLOCK */
	//memset(IoTpAd.flash_rw_buf, 0, sizeof(IoTpAd.flash_rw_buf));
	spi_flash_read(FLASH_COM_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
	
#if 1  /*use stored flag to shrink code size*/
	if (IoTpAd.flash_rw_buf[FLASH_COM_CFG_INFO_STORED] == COMMON_INFO_STORED)
	{
		IoTpAd.ComCfg.BootFWIdx		    =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_BOOT_IDX];
		IoTpAd.ComCfg.RecovModeStatus 	=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_RECOVERY_MODE_STATUS];
		IoTpAd.ComCfg.IOMode		    =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_IO_SET];
		
	#if FLASH_STRUCT_MAPPING
		/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
		  can use the following method to shrink code size */
	
		memcpy( &IoTpAd.ComCfg.UART_Baudrate,
				&IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],	
				FLASH_COM_CFG_RESERVE_2 - FLASH_COM_CFG_UART_BAUD);
	#else
		memcpy(&IoTpAd.ComCfg.UART_Baudrate, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],   FLASH_COM_CFG_UART_BAUD_LEN);
		IoTpAd.ComCfg.UART_DataBits	=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_DATA_BITS];
		IoTpAd.ComCfg.UART_Parity 	=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_PARITY];
		IoTpAd.ComCfg.UART_StopBits	=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_STOP_BITS];
	#endif
	
		if((IoTpAd.ComCfg.UART_Baudrate > UART_BAUD_3200000) ||
		   (IoTpAd.ComCfg.UART_Parity > pa_space) ||
		   ((IoTpAd.ComCfg.UART_DataBits < len_5) || (IoTpAd.ComCfg.UART_DataBits > len_8))  ||
		   ((IoTpAd.ComCfg.UART_StopBits < sb_1)  || (IoTpAd.ComCfg.UART_StopBits > sb_1_5)) )
		{
		    default_uart_cfg();
		}
		
	#if FLASH_STRUCT_MAPPING
		/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
		  can use the following method to shrink code size */
	
		memcpy( &IoTpAd.ComCfg.TCPUDP_CS,
				&IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET],	
				FLASH_COM_CFG_IOT_SERVER_IP + FLASH_COM_CFG_IOT_SERVER_IP_LEN - FLASH_COM_CFG_TCPUDP_CS_SET);
	#else
		
		IoTpAd.ComCfg.TCPUDP_CS		=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET];
		IoTpAd.ComCfg.Use_DHCP 		=  IoTpAd.flash_rw_buf[FLASH_COM_CFG_USE_DHCP];
		memcpy(&IoTpAd.ComCfg.IoT_TCP_Srv_Port,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_TCP_SRV_PORT],FLASH_COM_CFG_IOT_TCP_SRV_PORT_LEN);
		memcpy(&IoTpAd.ComCfg.Local_TCP_Srv_Port,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_TCP_SRV_PORT],FLASH_COM_CFG_LOCAL_TCP_SRV_PORT_LEN);
		memcpy(&IoTpAd.ComCfg.IoT_UDP_Srv_Port, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_UDP_SRV_PORT],FLASH_COM_CFG_IOT_UDP_SRV_PORT_LEN);
		memcpy(&IoTpAd.ComCfg.Local_UDP_Srv_Port,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_UDP_SRV_PORT],FLASH_COM_CFG_LOCAL_UDP_SRV_PORT_LEN);
		memcpy(IoTpAd.ComCfg.STATIC_IP,		&IoTpAd.flash_rw_buf[FLASH_COM_CFG_STATIC_IP], 	    FLASH_COM_CFG_STATIC_IP_LEN);
		memcpy(IoTpAd.ComCfg.SubnetMask_IP,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_SUBNET_MASK_IP], FLASH_COM_CFG_SUBNET_MASK_IP_LEN);
		memcpy(IoTpAd.ComCfg.DNS_IP,		&IoTpAd.flash_rw_buf[FLASH_COM_CFG_DNS_SERVER_IP],  FLASH_COM_CFG_DNS_SERVER_IP_LEN);
		memcpy(IoTpAd.ComCfg.GateWay_IP,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_GATEWAY_IP], 	FLASH_COM_CFG_GATEWAY_IP);
		memcpy(IoTpAd.ComCfg.IoT_ServeIP,	&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_SERVER_IP], 	FLASH_COM_CFG_IOT_SERVER_IP_LEN);
	#endif	
	
		if( ((IoTpAd.ComCfg.IoT_TCP_Srv_Port == 0) || (IoTpAd.ComCfg.IoT_TCP_Srv_Port == 0xFFFF)) ||
			((IoTpAd.ComCfg.Local_TCP_Srv_Port == 0) || (IoTpAd.ComCfg.Local_TCP_Srv_Port == 0xFFFF)) ||
			((IoTpAd.ComCfg.IoT_UDP_Srv_Port == 0) || (IoTpAd.ComCfg.IoT_UDP_Srv_Port == 0xFFFF)) ||
			((IoTpAd.ComCfg.Local_UDP_Srv_Port == 0) || (IoTpAd.ComCfg.Local_UDP_Srv_Port == 0xFFFF)) ||
			(check_data_valid(IoTpAd.ComCfg.STATIC_IP,    MAC_IP_LEN) == FALSE) ||
			(check_data_valid(IoTpAd.ComCfg.SubnetMask_IP,MAC_IP_LEN) == FALSE) ||
			(check_data_valid(IoTpAd.ComCfg.DNS_IP,       MAC_IP_LEN) == FALSE) ||
			(check_data_valid(IoTpAd.ComCfg.GateWay_IP,   MAC_IP_LEN) == FALSE) ||
			(check_data_valid(IoTpAd.ComCfg.IoT_ServeIP,  MAC_IP_LEN) == FALSE) )
		{
			default_ip_cfg();
		}
		
		memcpy(IoTpAd.ComCfg.CmdPWD, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_CMD_PWD],  FLASH_COM_CFG_CMD_PWD_LEN);
	}
	else
	{
		reset_com_cfg(TRUE);
	}
#endif

	return TRUE;
}

/*========================================================================
	Routine	Description:
		reset_com_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN reset_com_cfg(BOOLEAN bUpFlash)
{
	//DBGPRINT_HIGH(RT_DEBUG_INFO,("%s \n",__FUNCTION__));
	
	memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));
	
	default_boot_cfg();
	default_uart_cfg();
	default_ip_cfg();
	
	if(bUpFlash == TRUE)
	{
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_INFO_STORED]    = COMMON_INFO_STORED;
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_BOOT_IDX]       = Com_Cfg.BootFWIdx;
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_RECOVERY_MODE_STATUS] = Com_Cfg.RecovModeStatus;
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_IO_SET]         = Com_Cfg.IOMode;

#if FLASH_STRUCT_MAPPING
		/*if flash layout mapping  is same as IOT_COMMON_CFG  or IOT_USR_CFG,  
		  can use the following method to shrink code size */
		
		memcpy( &IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],
			    &Com_Cfg.UART_Baudrate, 
			    FLASH_COM_CFG_RESERVE_2 - FLASH_COM_CFG_UART_BAUD);
		
		memcpy( &IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET],
				&Com_Cfg.TCPUDP_CS,
				FLASH_COM_CFG_IOT_SERVER_DOMAIN - FLASH_COM_CFG_TCPUDP_CS_SET);
		
		memcpy( &IoTpAd.flash_rw_buf[FLASH_COM_CFG_CMD_PWD],	
			    Com_Cfg.CmdPWD,	
			    FLASH_COM_CFG_CMD_PWD_LEN);
#else
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],&Com_Cfg.UART_Baudrate, FLASH_COM_CFG_UART_BAUD_LEN);
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_DATA_BITS]   = Com_Cfg.UART_DataBits;
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_PARITY]	    = Com_Cfg.UART_Parity;
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_STOP_BITS]   = Com_Cfg.UART_StopBits;

		IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET]    = Com_Cfg.TCPUDP_CS;
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_TCP_SRV_PORT], &Com_Cfg.IoT_TCP_Srv_Port,	FLASH_COM_CFG_IOT_TCP_SRV_PORT_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_TCP_SRV_PORT], &Com_Cfg.Local_TCP_Srv_Port,	FLASH_COM_CFG_LOCAL_TCP_SRV_PORT_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_UDP_SRV_PORT], &Com_Cfg.IoT_UDP_Srv_Port,	FLASH_COM_CFG_IOT_UDP_SRV_PORT_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_UDP_SRV_PORT], &Com_Cfg.Local_UDP_Srv_Port,	FLASH_COM_CFG_LOCAL_UDP_SRV_PORT_LEN);
		IoTpAd.flash_rw_buf[FLASH_COM_CFG_USE_DHCP]          = Com_Cfg.Use_DHCP;

		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_STATIC_IP],     Com_Cfg.STATIC_IP,	 MAC_IP_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_SUBNET_MASK_IP],Com_Cfg.SubnetMask_IP, MAC_IP_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_DNS_SERVER_IP], Com_Cfg.DNS_IP,	     MAC_IP_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_GATEWAY_IP],    Com_Cfg.GateWay_IP,	 MAC_IP_LEN);
		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_SERVER_IP], Com_Cfg.IoT_ServeIP,	 MAC_IP_LEN);

		memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_CMD_PWD],	    Com_Cfg.CmdPWD ,	     MAC_IP_LEN);
#endif

		spi_flash_write(FLASH_COM_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

	}
	
	return TRUE;
}

/*========================================================================
	Routine	Description:
		reset_sta_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN reset_sta_cfg(VOID)
{
	/*in changlist98520, if do Offline from Data Cmd "CONTROL_CLIENT_OFFLINE_REQUEST",  system halt*/
	/*Only add Printf or other statement here, can fix it,   this is temparary solution, need anaylsis in furture*/
	usecDelay(10);
	
	/* Notice: erase sta config Flash region , default size is one sector [4KB] */
	spi_flash_erase_SE(FLASH_STA_CFG_BASE);
	
	return TRUE;
}

/*========================================================================
	Routine	Description:
		reset_ap_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN reset_ap_cfg(VOID)
{
	/*in changlist104708, if AT cmd  AT#SoftAPConf -d1+enter,  system halt*/
	/*Only add Printf or other statement here, can fix it,   this is temparary solution, need anaylsis in furture*/
	usecDelay(10);
	
	/* Notice: erase ap config Flash region , default size is one sector [4KB] */
	spi_flash_erase_SE(FLASH_AP_CFG_BASE);

	return TRUE;
}


/*========================================================================
	Routine	Description:
		IoT_Cust_SMNT_Sta_Chg_Init
		
	Arguments:	 the callback function while doing wifi_state_chg(WIFI_STATE_SMNT, 0);
				 once jump to SmartConnection State, maybe you need do SmartConnection Initialization here
	
	                      wifi_state_chg(WIFI_STATE_SMNT, 0); shall be implement  after WIFI_STATE_INIT done
	                      
	Return Value:	 None
	
========================================================================*/

VOID IoT_Cust_SMNT_Sta_Chg_Init(VOID)
{
#if (CONFIG_SOFTAP == 0)  /*Smnt is valid in STA mode, and should not available in SoftAP mode*/
#if (CFG_SUPPORT_MTK_SMNT == 1)   /*this is MTK smart connection sub state machine*/
	SMTCN_state_chg_init();

#else
	IoT_Set_RxFilter(0x7f93);   /*Enable Sniffer NotToMe unicast, not myBSS Packet*/
	pIoTMlme->DataEnable = 1;  /*Enable to handle Data frame in STARxDoneInterruptHandle*/

	/* once jump to SmartConnection State, maybe you need do SmartConnection Initialization here */
	{
	   //customer smart connect state initialization
	   
	}
#endif
#endif
}


/*========================================================================
	Routine	Description:
		IoT_Cust_SM_Smnt
		
	Arguments:	 This is smart connection entry for wifi state machine
	Return Value:	 None
	
========================================================================*/
VOID IoT_Cust_SM_Smnt(VOID)
{
#if (CONFIG_SOFTAP == 0)  /*Smnt is valid in STA mode, and should not available in SoftAP mode*/
#if (CFG_SUPPORT_MTK_SMNT == 1)   /*this is MTK smart connection sub state machine*/
	
	SMTCN_process();

#else
	/*this is Customer smart connection entry*/
	/*if set callback here, MTK smart connection will be disabled*/

	/*for example*/
	UCHAR  Ssid[MAX_LEN_OF_SSID+1]= "iot_ap";
	UCHAR  Passphase[CIPHER_TEXT_LEN];  //= "Passphase1234";
#ifndef PMK_CAL_BY_SW
	UCHAR  PMK[CIPHER_TEXT_LEN];        //= "PMK1234";
	memset(PMK, 0 ,CIPHER_TEXT_LEN);
#endif

	/* Must initialize Passphase and PMK  if no reference value be set on the above*/
	memset(Passphase, 0 ,CIPHER_TEXT_LEN);

	//Printf_High("%s: %d \n",__FUNCTION__,__LINE__);

	/* Smnt connection state machine start */
	{
	   //customer smart connect process
	   
	}
	/* Smnt connection done */

	
	/* After smnt connection done */
	/* need set Smnt connection information and start to scan*/
	pIoTStaCfg->AuthMode     = Ndis802_11AuthModeOpen;
	pIoTStaCfg->SsidLen	     = strlen(Ssid);
	pIoTStaCfg->PassphaseLen = strlen(Passphase); //sizeof(Passphase);
	memcpy(pIoTStaCfg->Ssid, 	  Ssid, 	 pIoTStaCfg->SsidLen);
	memcpy(pIoTStaCfg->Passphase, Passphase, pIoTStaCfg->PassphaseLen);

#ifdef PMK_CAL_BY_SW  //jinchuan  calcurate PMK by 7681 software, it will spend 6sec
	if (pIoTStaCfg->AuthMode >= Ndis802_11AuthModeWPA)
	{
		/*Deriver PMK by AP 's SSID and Password*/	
		UCHAR keyMaterial[40] = {0};
		
		RtmpPasswordHash(pIoTStaCfg->Passphase, pIoTStaCfg->Ssid, 
						 pIoTStaCfg->SsidLen,   keyMaterial);
		memcpy(pIoTStaCfg->PMK, keyMaterial, LEN_PMK);

		//Printf_High("keyMaterial:");
		//dump(keyMaterial, 40);
	}
#else
	memcpy(pIoTStaCfg->PMK,  PMK, strlen(PMK));
#endif

	/* change wifi state to SCAN*/
	wifi_state_chg(WIFI_STATE_SCAN, 0);
#endif
#endif
}


#if 0  //disable this for use STARxDoneInterruptHandle()
/*========================================================================
	Routine	Description:
		IoT_Cust_Rx_Handler
		
	Arguments:	 This is RX handler for customization
	Return Value:	 None
	
========================================================================*/
VOID IoT_Cust_Rx_Handler(IN PHEADER_802_11 pHeader, IN USHORT DataSize)
{
   //Printf_High("Type=%d,   DataSize=%d,\n", pHeader->FC.Type, DataSize);
   //dump(pHeader, DataSize);

   switch (pHeader->FC.Type)
   {
		/* CASE I, receive a DATA frame */
		case IOT_BTYPE_DATA:
		{
			switch (pIoTMlme->CurrentWifiState)
			{
				case WIFI_STATE_SMTCNT:
				{
				  #if (CONFIG_SOFTAP == 0)  /*Smnt is valid in STA mode, and should not available in SoftAP mode*/
				  #if (CFG_SUPPORT_MTK_SMNT == 1)
					  SMTCNRXFrameHandle(pHeader, DataSize);
				  #else
				  {
				      //customer smart connect process
				      //collect smnt connection packet
				  }
				  #endif
				  #endif
				}
					break;
				default:
					break;
			}
		}
			break;
		
		/* CASE 2, receive a Management frame */
	    case IOT_BTYPE_MGMT:
	    {
		   /* process Management frame */
	    }
			break;
   
	   default:
		   /* free control */
		   break;
   }

}
#endif

/*========================================================================
	Routine	Description:
		IoT_Cust_GPIINT_Hdlr --  GPIO interrupt handler
		
	Arguments:
	[IN] GPI_STS  -  [0~6]:  GPIO0~6 Interrupt status
	
	Return Value:  NONE
	Note:
========================================================================*/
VOID IoT_Cust_GPIINT_Hdlr(IN UINT8 GPI_STS)
{
	if ((GPI_STS >> 0) & 0x01)
	{
		Printf_High("GPIO_1 interrupted\n");
	}
	else if ((GPI_STS >> 1) & 0x01)
	{
		Printf_High("GPIO_0 interrupted\n");
	}
	else if ((GPI_STS >> 2) & 0x01)
	{
		Printf_High("GPIO_2 interrupted\n");
	}
	else if ((GPI_STS >> 3) & 0x01)
	{
		Printf_High("GPIO_3 interrupted\n");
	}
	else if ((GPI_STS >> 4) & 0x01)
	{
		Printf_High("GPIO_4 interrupted\n");
	}
	else if ((GPI_STS >> 5) & 0x01)
	{
		Printf_High("GPIO_5 interrupted\n");
	}
	else if ((GPI_STS >> 6) & 0x01)
	{
		Printf_High("GPIO_6 interrupted\n");
	}
	else
	{
		Printf_High("Ignored\n");
	}
}

#if (HW_TIMER1_SUPPORT==1)

VOID IoT_Cust_HW_Timer1_Hdlr(VOID)
{
	/*Sample code for HW timer1 interrupt handle*/
	/*Notice:  Do not implement too much process here, as it is running in interrupt level*/

	#if 0
	UINT8 input, Polarity;
	
	/*Make GPIO 4 blinking by Timer1 HW EINT */
	IoT_gpio_read(4, &input, &Polarity);
	IoT_gpio_output(4, (input==0)?1:0 );
	#endif
}

UINT32 IoT_Cust_Get_HW_Timer1_TICK_HZ(VOID)
{
	return TICK_HZ_HWTIMER1;
}

#endif
