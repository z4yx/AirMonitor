#ifndef __ATE_H__
#define __ATE_H__

#include "types.h"
#include "rtmp_general_pub.h"

/* All PHY rate summary in TXD */
/* Preamble MODE in TxD */
#define MODE_CCK                 0
#define MODE_OFDM                1
#define MODE_HTMIX               2
#define MODE_HTGREENFIELD        3
#define MODE_VHT                 4

#define ATE_TX_PAYLOAD_LEN		 (800)	 /*must  <=  1024 - 28 (24Bytes[MAC Header]+4Bytes[FCS])*/
#define ATE_TX_SPEED 	    	 (1000)   /*unit: 1ms*/
#define ATE_TX_CNT_PER_SPEED 	 (1)      /*send how many Tx Packet during each ATE_TX_SPEED */
#define ATE_RX_CNT_DURATION		 (1000)   /*unit: 1ms*/

/* definition RSSI Number */
#define RSSI_0					 0
#define RSSI_1					 1
#define RSSI_2					 2


typedef struct GNU_PACKED _ATE_INFO {
	UINT8  bATEMode;
	PUCHAR MacSA;
	PUCHAR MacBssid;
	PUCHAR MacDA;
	UINT8 Channel;
	
	/*Tx Test*/
	UINT8 RfFreqOffset;
	UINT8 TxPower0;
	TXWI_STRUC TxWI;	/* TXWI */
	UINT32 TxSpeed;       /*units 1ms - the frequency to do SendProcess*/
	UINT32 TxCntPerSpeed; /*How many packet be sent  within each of SendProcess*/
	UINT32 TxCount;		  /*How many SendProcess be called */
	UINT32 PayLoadLen;    /*The payLoad length for Tx packet, not contain Mac Header*/

	/*Rx Test*/
	UINT32 RxU2MTotalCnt;
	UINT32 RxBMTotalCnt;
	UINT32 RxCntU2MPerSec;
	UINT32 RxCntBMPerSec;
	UINT32 RxCntDuration;

	CHAR LastSnr0;		/* last received SNR */
	CHAR LastSnr1;		/* last received SNR for 2nd  antenna */
	CHAR LastSnr2;
	CHAR AvgSnr0;
	CHAR AvgSnr1;
	CHAR AvgSnr2;
	SHORT AvgSnr0X8;
	SHORT AvgSnr1X8;
	SHORT AvgSnr2X8;

	CHAR LastRssi0;		/* last received RSSI */
	CHAR LastRssi1;		/* last received RSSI for 2nd  antenna */
	CHAR LastRssi2;		/* last received RSSI for 3rd  antenna */
	CHAR AvgRssi0;		/* last 8 frames' average RSSI */
	CHAR AvgRssi1;		/* last 8 frames' average RSSI */
	CHAR AvgRssi2;		/* last 8 frames' average RSSI */
	SHORT AvgRssi0X8;	/* sum of last 8 frames' RSSI */
	SHORT AvgRssi1X8;	/* sum of last 8 frames' RSSI */
	SHORT AvgRssi2X8;	/* sum of last 8 frames' RSSI */
	UINT32 NumOfAvgRssiSample;
} ATE_INFO, *PATE_INFO;


typedef enum {
	ATE_MODE_OFF = 0,
	ATE_MODE_TX,
	ATE_MODE_RX,
	ATE_MODE_MAX
}ATE_MODE;

#if (ATCMD_ATE_SUPPORT == 1)
VOID SendATETxDataFrame(VOID);
#endif

#endif  //__ATE_H__
