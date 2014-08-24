/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rtmp_data.c

	Abstract:
	Data path subroutines

	Revision History:
	Who 		When			What
	--------	----------		----------------------------------------------
*/
#include <stdio.h>
#include "types.h"
#include "config.h"
#include "queue.h"
#include "tcpip_main.h"
#include "iot_api.h"
#if (ATCMD_ATE_SUPPORT == 1)
#include "ate.h"
#endif

/*****************************************************************
  Defination
******************************************************************/


/*****************************************************************
  Extern Paramter
******************************************************************/
extern MLME_STRUCT 	 *pIoTMlme;
extern UCHAR EAPOL[2];	//  = {0x88, 0x8e};

#if (ATCMD_ATE_SUPPORT == 1)
extern ATE_INFO gATEInfo;
#endif

/*****************************************************************
  Extern Function
******************************************************************/



/*****************************************************************
  Functions
******************************************************************/
//
// All Rx routines use RX_BLK structure to hande rx events
// It is very important to build pRxBlk attributes
//  1. pHeader pointer to 802.11 Header
//  2. pData pointer to payload including LLC (just skip Header)
//  3. set payload size including LLC to DataSize
//  4. set some flags with RX_BLK_SET_FLAG()
// 
VOID STAHandleRxDataFrame(
	RX_BLK*			pRxBlk)
{
#if (CFG_SUPPORT_TCPIP == 0)
	PUCHAR rawpacket;
	UINT16 rawpacketlength;
#endif

/*************************************************
*data frame parsing only in SMTCN and CONNECTED
**************************************************/
#ifdef CONFIG_SOFTAP
	RTMPRawDataSanity(pRxBlk);
    pRxBlk->pData	-= 6;
    memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr3, 6);
    pRxBlk->DataSize += 6;

    pRxBlk->pData	-= 6;
    memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr1, 6);
    pRxBlk->DataSize += 6;
	
#if CFG_SUPPORT_TCPIP
    netif_rx(pRxBlk->pData, pRxBlk->DataSize);
#endif

#else
   switch (pIoTMlme->CurrentWifiState){
       case WIFI_STATE_SMTCNT:
			#if (CFG_SUPPORT_MTK_SMNT == 1)
				/*implement MTK smart connection if there is no customer callback for smart connection*/
				SMTCNRXFrameHandle(pRxBlk->pData, pRxBlk->pRxWI->MPDUtotalByteCount);
			#else
				{
					//customer smart connect process
					//collect smnt connection packet
				}
			#endif
		   break;
		
	   // [Arron Modify] : for 4-way handshake
	   case WIFI_STATE_4WAY:		
	   case WIFI_STATE_CONNED:
			//skip 802.11 and SNAP header
	   		RTMPRawDataSanity(pRxBlk);
#if CFG_SUPPORT_4WAY_HS
			//check is EAPOL package
			if (!memcmp(pRxBlk->pData, EAPOL, 2))
			{
				/* Confirm incoming is EAPOL-KEY frame */
				if( *(pRxBlk->pData + 3) == EAPOLKey)
				{	
					printf("EAPOL KEY +++\n");
					/* process EAPOL KEY 4-way handshake and 2-way group handshake */
					WpaEAPOLKeyAction(pRxBlk, pRxBlk->pData + 2, pRxBlk->DataSize - 2);
				}
				break;
			}
#endif
#if CFG_SUPPORT_TCPIP
			pRxBlk->pData    -= 6;
			memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr3, 6);
			pRxBlk->DataSize += 6;

			pRxBlk->pData    -= 6;
			memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr1, 6);
			pRxBlk->DataSize += 6;

			netif_rx(pRxBlk->pData, pRxBlk->DataSize);
#else
	   	   if (pRxBlk->pRxINFO->Bcast)
		   {
				DBGPRINT(RT_DEBUG_INFO,("UDP0\n"));
				rawpacket = ParsingBCData(pRxBlk, &rawpacketlength);

				if(rawpacket != NULL)
				{
					DBGPRINT(RT_DEBUG_INFO,("UDP2\n"));
					IoT_process_app_packet(rawpacket, rawpacketlength);
				}
	   	   	}
#endif
	   	    break;
	   default:
	   	    break;
   	}
#endif

	return;
}

VOID STAHandleRxMgmtFrame(
	RX_BLK*			pRxBlk)
{
#ifdef CONFIG_SOFTAP
	//Printf_High("BTYPE_MGMT subtype : %d\n", pRxBlk->pHeader->FC.SubType);
	switch (pRxBlk->pHeader->FC.SubType)
	{
		case SUBTYPE_ASSOC_REQ:
			APAssocReqActionAndSend(pRxBlk, 0);
			break;

		case SUBTYPE_REASSOC_REQ:
			APAssocReqActionAndSend(pRxBlk, 1);
			break;
			break;

		case SUBTYPE_PROBE_REQ:
			APSendProbeAction(pRxBlk);
			break;

		case SUBTYPE_DISASSOC:
			break;

		case SUBTYPE_AUTH:
			APPeerAuthReqAction(pRxBlk);		
			break;

		case SUBTYPE_DEAUTH:
			break;
		default :
			break;
	}

#else
	switch (pIoTMlme->CurrentWifiState)
	{
		/*Scan SM*/
		case WIFI_STATE_SCAN:
			ScanProcessMgmt(pRxBlk);
			break;
			
		/*Auth SM*/
		case WIFI_STATE_AUTH:
			MlmeAuthProcess(pRxBlk);
			break;
			
		/*Assoc SM*/
		case WIFI_STATE_ASSOC:
			MlmeAssocProcess(pRxBlk);
			break;
		
		/*Connected SM*/
		case WIFI_STATE_CONNED:
			if (pRxBlk->pRxINFO->U2M)
			{
				if (pRxBlk->pHeader->FC.SubType == SUBTYPE_DEAUTH)
				{
					wifi_state_chg(WIFI_STATE_INIT, 0);	 //PeerDeauthAction();
				}
				if (pRxBlk->pHeader->FC.SubType == SUBTYPE_DISASSOC)
				{
					PeerDisassocAction(pRxBlk, 
								pRxBlk->pData,
								pRxBlk->DataSize, 
								pRxBlk->pHeader->FC.SubType);
				}
			}
			
			if (pRxBlk->pRxINFO->MyBss){
				if(pRxBlk->pHeader->FC.SubType == SUBTYPE_BEACON){
					//DBGPRINT(RT_DEBUG_INFO, ("BCN FR: %02x:%02x:%02x:%02x:%02x:%02x \n",
					//							PRINT_MAC(pRxBlk->pHeader->Addr3)));
					RestartBCNTimer();
				}
			}
			break;

		default:
			break;
	}
#endif

}

/*
	========================================================================

	Routine Description:
		Process RxDone interrupt, running in DPC level

	Arguments:
		pAd Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	Note:
		This routine has to maintain Rx ring read pointer.
		Need to consider QOS DATA format when converting to 802.3
	========================================================================
*/
BOOLEAN STARxDoneInterruptHandle(
	pBD_t pBufDesc)
{
	PHEADER_802_11	pHeader;
	RX_BLK			RxCell;
	PRXWI_STRUC     pRxWI;
	NDIS_STATUS     Status;
	PRXINFO_STRUC   pRxINFO;
	PUCHAR          pBuff;
	
	if (pBufDesc->Length <= sizeof(HEADER_802_11) +RXINFO_SIZE +RXWI_SIZE){ // invalid packet length
		DBGPRINT(RT_DEBUG_TRACE,("len=%d\n", pBufDesc->Length));
		return FALSE;
	}	
	/* RX_FCEINFO  */
	/* RXINFO      */
	/* RXWI        */
	/* Payload     */
	
	/* check MIC error by pRxD attribute */
	/* single frame MIC is calculated by HW */
	pBuff   = (PUCHAR)pBufDesc->pBuf;
	pRxINFO = (PRXINFO_STRUC)(pBuff);
	pRxWI   = (PRXWI_STRUC) (pBuff +RXINFO_SIZE);
	pHeader = (PHEADER_802_11) (pBuff +RXINFO_SIZE +RXWI_SIZE);
	StorePhyMode(pRxWI->PHYMode);

	Status = RTMPCheckRxError(pHeader, pRxWI, pRxINFO);
	if(Status == NDIS_STATUS_FAILURE)
	{
		//printf("RTMPCheckRxError\n");

		/* free packet */
		return FALSE;
	}


	/* fill RxBLK */
	RxCell.pRxINFO = pRxINFO;
	RxCell.pRxWI   = pRxWI;
	RxCell.pHeader = pHeader;
	RxCell.pRxPacket = pBufDesc;
	RxCell.pData = (UCHAR *) pHeader;
	RxCell.DataSize = pRxWI->MPDUtotalByteCount;
	RxCell.Flags = 0;

	/* if remote wakeup is triggered, all the packets should be ignored */
	//if(pAd.remoteWakeupFlag ==1)
	//	return FALSE;

#if (RX_CLASS123 == 1)	
	if(!STARxFrameClassVfy(&RxCell, pHeader->FC.Type))
	{
		return FALSE;
	}
#endif


#if (ATCMD_ATE_SUPPORT == 1)
	if (gATEInfo.bATEMode == ATE_MODE_RX)
	{
		if ((RxCell.pRxINFO->Bcast) || (RxCell.pRxINFO->Mcast))
		{
			gATEInfo.RxCntBMPerSec++;
		}
		else
		{
			gATEInfo.RxCntU2MPerSec++;

			/*only summary RSSI for u2m packet*/
			//ATESampleRssi(RxCell.pRxWI);	 /*The old method for ATE, no exactly*/
			Update_Rssi_Sample(&gATEInfo, RxCell.pRxWI);
		}
	}
#endif

#if 0
	if (IoTCustOp.IoTCustWifiRxHandler != NULL)
	{
		IoTCustOp.IoTCustWifiRxHandler(RxCell.pHeader, RxCell.DataSize);
	}
#endif

	switch (pHeader->FC.Type)
	{	
		/* CASE I, receive a DATA frame */
		case BTYPE_DATA:
		{	
#ifndef CONFIG_SOFTAP
			//jinchuan  (Only deal with Data frame in scan or Connected State,	or we need response Deauth)
			if(pIoTMlme->DataEnable == 1) 
#endif
			STAHandleRxDataFrame(&RxCell);
		}
			break;

		case BTYPE_MGMT:
		{
			/* process Management frame */
			STAHandleRxMgmtFrame(&RxCell);
		}
			break;

		default:
			DBGPRINT(RT_DEBUG_INFO,("DF "));
			/* free control */
			break;
	}
 
	return TRUE;
}

