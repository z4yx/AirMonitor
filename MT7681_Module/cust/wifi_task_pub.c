#include <config.h>
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <timer.h>
#include <queue.h>
#include "flash_map.h"
#include "tcpip_main.h"
#if (ATCMD_ATE_SUPPORT == 1)
#include "ate.h"
#endif
#include "iot_api.h"
#include "iot_custom.h"
#include "eeprom.h"

/*****************************************************************
  Defination
******************************************************************/
#define SYS_SIG_WIFI_IDLE_REQ    (UINT32)(1UL << 9)


/*****************************************************************
  Extern Paramter
******************************************************************/
#if (ADV_PWR_SAVING == 1)
extern TIMER_T AdvPwsTimer;
extern UCHAR AdvPwsWakeupFlag;
#endif

#if (ATCMD_ATE_SUPPORT == 1)
extern 	ATE_INFO gATEInfo;
extern  BOOLEAN  gCaliEnabled; 
#endif

/*unit:ms  indicated recovery mode duration*/
#if (ATCMD_RECOVERY_SUPPORT==1)
extern UINT16 gRecoveryModeTime; 
#endif

extern IOT_ADAPTER   IoTpAd;
extern IOT_CUST_OP   IoTCustOp;
extern IOT_SMNT_INFO IoTSmntInfo;

/*****************************************************************
  Global Paramter
******************************************************************/
MLME_STRUCT 	 *pIoTMlme;
STA_ADMIN_CONFIG *pIoTStaCfg;

#ifdef CONFIG_SOFTAP
UINT8 g_SoftAPStartFlag = 0;
#endif

/*to indicated wheter store the SMNT setting after Wifi Connected AP and Got IP*/
BOOLEAN gSmnted = FALSE;

/*gCurrentAddress shall be set in sys_init() as the value stored in flash sta cfg region if the value in flash is valid*/
UCHAR gCurrentAddress[MAC_ADDR_LEN]	=	{0x00, 0x0c, 0x43, 0x12, 0x34, 0xf3}; 


/*****************************************************************
  Extern Function
******************************************************************/



/*****************************************************************
  Functions
******************************************************************/

/*========================================================================
	Routine	Description:
		wifi_rx_proc -- Dequeue a buffer from RxPacketQueue which includes received packet (cmd/frame)
		After deal with the packet ,  release buffer to FreeQueue1/2 to recevieing new packet by 
		Enqueue operation.
		
	Arguments:
	Return Value:
	Note: 
========================================================================*/
VOID wifi_rx_proc(VOID)
{
	pBD_t pBufDesc;
	pQU_t pRxPktQueue;
	
	pRxPktQueue = GetQueueInfo();
	
	/* Check amount of PKT Queue */
	while (pRxPktQueue->Amount > 0)
	{
		pBufDesc = apiQU_Dequeue(pRxPktQueue);

		if (pBufDesc->SrcPort == 1) // from WIFI
		{
			STARxDoneInterruptHandle(pBufDesc);

			apiQU_Enqueue(&gFreeQueue1,pBufDesc);
		}
	}

	return;
}

/******************************************************************************
 * wifiTASK_LowPrioTask
 * Description :
 *  Low priority task of WIFI.
 ******************************************************************************/
VOID 
wifiTASK_LowPrioTask (VOID)
{
	static int	cnt = 0;
#if (ATCMD_RECOVERY_SUPPORT==1)
	static int	RecCnt = 0;
#endif
#if (ATCMD_ATE_SUPPORT == 1)
	static BOOLEAN	bCaliInited = FALSE;
#endif

	sysTASK_DeassertSignal(SYS_SIG_WIFI_IDLE_REQ);

#if 0//(ADV_PWR_SAVING == 1)
	if(AdvPwsWakeupFlag == 1)
	{
	  	//DBGPRINT(RT_DEBUG_INFO,("AdvPwsSwTimer\n"));
		AdvPwsWakeupFlag = 0;
		/* start SW timer for wakeup time */
		cnmTimerStartTimer(&AdvPwsTimer, AdvPSMode.wakeupTime);
	}
#endif

	SwTimerHandler();
	handle_FCE_interrupt();

	/*Do Period calibration*/
	CaliPeriodExec();

#if (ATCMD_RECOVERY_SUPPORT==1)
	if ((RecCnt++ > gRecoveryModeTime)
#if (ATCMD_ATE_SUPPORT == 1)  /*if both Recovey and Cali mode are enabled, need aslo detect Calib Command before reboot*/
		&& (bCaliInited == FALSE)
#endif
		)
	{
		RecCnt = 0;

		/*Set Flag to make loader startup STA/AP FW*/
		SetRec2LoaderFlag();
		Printf_High("<== RecoveryMode\n");

		/*if no AT#UpdateFW to trigger Xmodem , System Reboot to restart loader*/
		Sys_reboot();
	}
#endif

#if (ATCMD_ATE_SUPPORT == 1)
	if ((bCaliInited == FALSE) && (gCaliEnabled == TRUE))
	{
		Printf_High("Enter into Calibration Mode \n");
		bCaliInited = TRUE;  /*only call one time while gCaliEnabled=true*/
		wifi_state_chg(WIFI_STATE_INIT,0);
	}

	/*move from ATEPeriodicExec() to fix RX/Tx Dequeue fail */
	pIoTMlme->DataEnable = 1;
	wifi_rx_proc();
	
	if (gATEInfo.bATEMode != ATE_MODE_OFF)
	{
		ATEPeriodicExec();
	}
	else if (gCaliEnabled == TRUE)   /*only go wifi state machine while Calibration On, to initial MAC addres and so on*/
#endif
	{
	 #ifdef CONFIG_SOFTAP 
		 if (g_SoftAPStartFlag == 0)
		 {
			 APInitialize();
			 Printf_High("Start AP ...\n");
			 g_SoftAPStartFlag = 1; 
		 }	 
	 #endif

	 	/*wifi_rx_proc() before wifi_state_machine() to fix Dequeue fail*/
		wifi_rx_proc();
	 	
		wifi_state_machine();
	}
	
	/*  run cutsomer sub task */
	if (IoTCustOp.IoTCustSubTask1 != NULL)
		IoTCustOp.IoTCustSubTask1();
	

	/* stay alive message */
	if((cnt++) > 5000)
	{ 
#if (ATCMD_ATE_SUPPORT == 1)
		if	(gCaliEnabled == TRUE)
			Printf_High("[CalTask]%d \n", GetMsTimer());
		else
			Printf_High("[RTask]%d \n", GetMsTimer());
#else	
		Printf_High("[WTask]%d \n", GetMsTimer());
#endif
		cnt = 0;
	}

	if(GetFceIntStat() == 0)
		sysTASK_AssertSignal(SYS_SIG_WIFI_IDLE_REQ);

}

/*========================================================================
	Routine	Description:
		check_data_valid
		
	Arguments:	
	Return Value:	if all data is 0xFF or 0x00, we assume it is invalid
	Note: 
========================================================================*/
BOOLEAN check_data_valid(PUINT8 pdata, UINT16 len)
{
	UINT16 i=0, No0=0, NoF=0;
	
	for(i=0; i<len; i++)
	{
		if(pdata[i] == 0x0)
		{
			No0++;
		}
		else if (pdata[i] == 0xFF)
		{
			NoF++;
		}
	}

	/*if all data is 0xFF or 0x00, we assume it is invalid*/
	if ((No0 == len) || (NoF == len))
		return FALSE;

	return TRUE;
}


/*========================================================================
	Routine	Description:
		wifi state machine -- management the entry for each of wifi state
		because wifi_rx_proc() maybe no need to be called in some state or substate
		a bool param "b_doRx" is be used to declare it
		
	Arguments:
	Return Value:
	Note:
========================================================================*/
VOID wifi_state_machine(VOID)
{
#ifndef CONFIG_SOFTAP
	BOOLEAN b_doRx = FALSE;	 /*not use and move wifi_rx_proc() before wifi_state_machine() to fix Dequeue fail*/
	
	switch (pIoTMlme->CurrentWifiState)
	{
		case WIFI_STATE_INIT:
			ws_init(&b_doRx);
			break;
		
		/*Smart Connection SM*/
		case WIFI_STATE_SMTCNT:
			ws_smt_conn(&b_doRx);
			break;
		
		/*Scan SM*/
		case WIFI_STATE_SCAN:
			/*send Probe req frame at first , then listen Probe response in each channel*/
			ws_scan(&b_doRx);
			break;
			
		/*Auth SM*/
		case WIFI_STATE_AUTH:
			ws_auth(&b_doRx);
			break;

		/*Assoc SM*/
		case WIFI_STATE_ASSOC:
			ws_assoc(&b_doRx);
			break;
		
		/*4 Way handshake SM*/
		case WIFI_STATE_4WAY:
			ws_4way(&b_doRx);
			break;

		/*Connected SM*/
		case WIFI_STATE_CONNED:
			ws_connected(&b_doRx);
#if CFG_SUPPORT_TCPIP
			tcpip_periodic_timer();
#endif
			break;

		default:
			b_doRx = TRUE;
			break;
	}
#endif

	return;
}


/*========================================================================
	Routine	Description:
		wifi init state machine
		
	Arguments:
		[OUT]b_enable    need implement wifi_rx_proc() after this function be calling
		
	Return Value:
		No Return Value
	Note:
========================================================================*/

UCHAR	Vmac[MAC_ADDR_LEN]={0};
INT		Vcount = 0;

#ifndef CONFIG_SOFTAP
VOID ws_init(OUT BOOLEAN *pb_enable)
{
	*pb_enable = TRUE;

#if CFG_SUPPORT_4WAY_HS
	/*reset R_Counter for 4-way handshake*/
	memset(pIoTStaCfg->R_Counter, 0, LEN_KEY_DESC_REPLAY);
#endif

	/* if SMNT->Init ,	Connected->Init state,	not do reset sta cfg check*/
	/* if next smnt connection is not use sta cfg in flash, not do reset sta cfg check */
	/* if Dhcp fail->Init , and use flash sta cfg,	  need do reset sta cfg check*/
	if ((pIoTMlme->UseFlashStaCfg == TRUE) &&
		((pIoTMlme->VerfiyInit == TRUE) ||    /*for DHCP fail->Init*/
		 ((pIoTMlme->PreWifiState >= WIFI_STATE_SCAN) &&
		  (pIoTMlme->PreWifiState != WIFI_STATE_CONNED)))
		)
	{
		/* if do initial with same SSID extend "MAX_VALID_CHECK_COUNT" times,*/
		/* not load sta cfg from flash , but enter Smart connection state directly	  */
		/* to avoid 7681 aways failed in Auth, Assoc , 4Way or DHCP state with	   */
		/* the old Auth mode setting or  password	after user changed AP settings */
		if (!memcmp(Vmac, pIoTStaCfg->Ssid, MAC_ADDR_LEN))
		{
			Vcount++;
		}
		else
		{	/*now, a new ssid is scanning,	reset the count */
			memcpy(Vmac, pIoTStaCfg->Ssid, MAC_ADDR_LEN);
			Vcount = 0;
		}

		pIoTMlme->VerfiyInit = FALSE;
		
		printf("Vcount = %d \n", Vcount);
	}

	if(Vcount >= MAX_VALID_CHECK_COUNT)
	{
		/*Direct enter Smart Connection state*/
		pIoTMlme->UseFlashStaCfg = FALSE;
		Vcount = 0;
	}
	else
	{
		pIoTMlme->UseFlashStaCfg = load_sta_cfg();
	}

	/* The entry for customization */
	if (IoTCustOp.IoTCustWifiSMInit != NULL)
		IoTCustOp.IoTCustWifiSMInit();
	
	if (pIoTMlme->UseFlashStaCfg == TRUE)
		wifi_state_chg(WIFI_STATE_SCAN, SCAN_STA_IDLE);
	else
		ws_goto_smnt();
}

/*========================================================================
	Routine	Description:
		wifi smart connection state machine
		
	Arguments:
		[OUT]b_enable    need implement wifi_rx_proc() after this function be calling
		
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_smt_conn(OUT BOOLEAN *pb_enable)
{
	*pb_enable = TRUE;

	if (IoTCustOp.IoTCustWifiSMSmnt == NULL)
	{
		/*implement MTK smart connection*/
		SMTCN_process();
	}
	else
	{
		/*implement customer smart connection*/
		IoTCustOp.IoTCustWifiSMSmnt();
	}
}

/*========================================================================
	Routine	Description:
		go to wifi smart connection state machine
		
	Arguments:
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_goto_smnt(VOID)
{
	wifi_state_chg(WIFI_STATE_SMTCNT, 0);

	if (IoTCustOp.IoTCustSMNTStaChgInit != NULL)
	{
		IoTCustOp.IoTCustSMNTStaChgInit();
	}
		
	gSmnted = TRUE;
}

/*========================================================================
	Routine	Description:
		wifi 4 way handshake state machine
		
	Arguments:
		[OUT]b_enable    need implement wifi_rx_proc() after this function be calling
		
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_4way(OUT BOOLEAN *pb_enable)
{
	*pb_enable = TRUE;
	return;
}

/*========================================================================
	Routine	Description:
		wifi connected state machine
		
	Arguments:
		[OUT]b_enable    need implement wifi_rx_proc() after this function be calling
		
	Return Value:
		No Return Value
	Note:
========================================================================*/
VOID ws_connected(OUT BOOLEAN *pb_enable)
{
	*pb_enable = TRUE;
}


/*========================================================================
	Routine	Description:
		store_sta_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
BOOLEAN load_sta_cfg(VOID)
{
	BOOLEAN bFlashInit = FALSE;
	
	/* read settings stored on flash STA CONFIG BLOCK */
	//memset(IoTpAd.flash_rw_buf, 0, sizeof(IoTpAd.flash_rw_buf));
	spi_flash_read(FLASH_OFFSET_STA_CFG_START, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
	//dump(IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

#if 1  /*use stored flag to shrink code size*/
	if (IoTpAd.flash_rw_buf[FLASH_STA_CFG_SMNT_INFO_STORED] == SMNT_INFO_STORED)
	{
		memcpy(pIoTStaCfg->Bssid,		&IoTpAd.flash_rw_buf[FLASH_STA_CFG_BSSID],		FLASH_STA_CFG_BSSID_LEN);
		memcpy(pIoTStaCfg->Ssid, 		&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSID],		FLASH_STA_CFG_SSID_LEN);
		memcpy(&pIoTStaCfg->SsidLen,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN],	FLASH_STA_CFG_SSIDLEN_LEN);

		if ((IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN] > 0x00) &&
		    (IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN] <= CIPHER_TEXT_LEN))
		{
			memcpy(pIoTStaCfg->Passphase,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASE],   FLASH_STA_CFG_PASSPHASE_LEN);
			memcpy(&pIoTStaCfg->PassphaseLen,&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN],FLASH_STA_CFG_PASSPHASELEN_LEN);
			memcpy(&pIoTStaCfg->AuthMode,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_AUTH_MODE],   FLASH_STA_CFG_AUTH_MODE_LEN);
#if (1 == CFG_SUPPORT_4WAY_HS)
			memcpy(pIoTStaCfg->PMK,			&IoTpAd.flash_rw_buf[FLASH_STA_CFG_4WAY_PMK],    FLASH_STA_CFG_4WAY_PMK_LEN);
#endif
		}
		
		bFlashInit = TRUE;    /*if has valid setting in flash,  direct go Scan state, but not do smart connect*/
	}
#else
	if ((IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN] != 0) && 
		(IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN] <= MAX_LEN_OF_SSID) &&
		(check_data_valid(&IoTpAd.390flash_rw_buf[FLASH_STA_CFG_SSID],  FLASH_STA_CFG_SSID_LEN)) &&
		(check_data_valid(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_BSSID], FLASH_STA_CFG_BSSID_LEN)))
	{
		/*if flash stored valid Bssid, Ssid and Ssid_len*/
		memcpy(pIoTStaCfg->.Bssid,		&IoTpAd.flash_rw_buf[FLASH_STA_CFG_BSSID],		FLASH_STA_CFG_BSSID_LEN);
		memcpy(pIoTStaCfg->Ssid, 		&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSID],		FLASH_STA_CFG_SSID_LEN);
		memcpy(&pIoTStaCfg->SsidLen,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN],	FLASH_STA_CFG_SSIDLEN_LEN);
		
		if ((IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN] != 0) &&
			(IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN_LEN] <= CIPHER_TEXT_LEN) &&
			(check_data_valid(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASE], FLASH_STA_CFG_PASSPHASE)) &&
			(IoTpAd.flash_rw_buf[FLASH_STA_CFG_AUTH_MODE] <= Ndis802_11AuthModeMax))
		{	
			/*if flash stored valid password and authmode*/
			memcpy(pIoTStaCfg->Passphase,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASE],   FLASH_STA_CFG_PASSPHASE_LEN);
			memcpy(&pIoTStaCfg->PassphaseLen,&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN],FLASH_STA_CFG_PASSPHASELEN_LEN);
			memcpy(&pIoTStaCfg->AuthMode,	&IoTpAd.flash_rw_buf[FLASH_STA_CFG_AUTH_MODE],   FLASH_STA_CFG_AUTH_MODE_LEN);
		}
		
		/* not do smart connect, but enter scan directly with above valid data*/
		bFlashInit = TRUE;
	}
#endif	

	return bFlashInit;
}

/*========================================================================
	Routine	Description:
		store_sta_cfg
		
	Arguments:	
	Return Value:	
========================================================================*/
VOID store_sta_cfg(VOID)
{	
	DBGPRINT(RT_DEBUG_INFO,("store_sta_cfg \n"));
	
	/*for shrink code size ,  current we only use 256Byte for  STA_CFG sector */
	memset(IoTpAd.flash_rw_buf, 0xff, sizeof(IoTpAd.flash_rw_buf));
	
	memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_BSSID], 	     pIoTStaCfg->Bssid,			FLASH_STA_CFG_BSSID_LEN);
	memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSID],	     pIoTStaCfg->Ssid, 			FLASH_STA_CFG_SSID_LEN);
	memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASE],    pIoTStaCfg->Passphase,		FLASH_STA_CFG_PASSPHASE_LEN);
#if (1 == CFG_SUPPORT_4WAY_HS)
	memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_4WAY_PMK], 	 pIoTStaCfg->PMK,	 		FLASH_STA_CFG_4WAY_PMK_LEN);
#endif
	//memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN],     &pIoTStaCfg->SsidLen, 		FLASH_STA_CFG_SSIDLEN_LEN);
	//memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN],&pIoTStaCfg->PassphaseLen,	FLASH_STA_CFG_PASSPHASELEN_LEN);
	//memcpy(&IoTpAd.flash_rw_buf[FLASH_STA_CFG_AUTH_MODE],   &pIoTStaCfg->AuthMode, 		FLASH_STA_CFG_AUTH_MODE_LEN);
	IoTpAd.flash_rw_buf[FLASH_STA_CFG_SSIDLEN] 		= pIoTStaCfg->SsidLen;
	IoTpAd.flash_rw_buf[FLASH_STA_CFG_PASSPHASELEN] = pIoTStaCfg->PassphaseLen;
	IoTpAd.flash_rw_buf[FLASH_STA_CFG_AUTH_MODE] 	= pIoTStaCfg->AuthMode;
	
	
	/*Indicate the smart connection info already be stored on flash
	   and not do smart connection while IoT device power on, but go 
	   to SCAN state with the infor stored on flash	 */
	IoTpAd.flash_rw_buf[FLASH_STA_CFG_SMNT_INFO_STORED] = SMNT_INFO_STORED;
		
	spi_flash_write(FLASH_STA_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
}

#endif //#ifndef CONFIG_SOFTAP


/*========================================================================
	Routine	Description:
		setFlagVfyInit    to indicate 7681 DHCP fail and back to intial state
		
	Arguments:	 if Dhcp fail->Init , and use flash sta cfg,	  need do reset sta cfg check
	Return Value:	
========================================================================*/

VOID setFlagVfyInit(BOOLEAN flag)
{
	pIoTMlme->VerfiyInit = flag;

	return;
}

