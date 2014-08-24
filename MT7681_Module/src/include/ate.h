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
#define ATE_TX_SPEED 	    	 (1)     /*unit: 1ms*/
#define ATE_TX_CNT_PER_SPEED 	 (1)     /*send how many Tx Packet during each ATE_TX_SPEED */
#define ATE_RX_CNT_DURATION		 (1000)  /*unit: 1ms*/

/* definition RSSI Number */
#define RSSI_0					 0
#define RSSI_1					 1
#define RSSI_2					 2


/* */
/*  Statistic counter structure */
/* */
typedef struct _COUNTER_802_3 {
	/* General Stats */
	ULONG GoodTransmits;
	ULONG GoodReceives;
	ULONG TxErrors;
	ULONG RxErrors;
	ULONG RxNoBuffer;

	/* Ethernet Stats */
	ULONG RcvAlignmentErrors;
	ULONG OneCollision;
	ULONG MoreCollisions;

} COUNTER_802_3, *PCOUNTER_802_3;

typedef struct _COUNTER_802_11 {
	ULONG Length;
/*	LARGE_INTEGER   LastTransmittedFragmentCount; */
	LARGE_INTEGER TransmittedFragmentCount;
	LARGE_INTEGER MulticastTransmittedFrameCount;
	LARGE_INTEGER FailedCount;
	LARGE_INTEGER RetryCount;
	LARGE_INTEGER MultipleRetryCount;
	LARGE_INTEGER RTSSuccessCount;
	LARGE_INTEGER RTSFailureCount;
	LARGE_INTEGER ACKFailureCount;
	LARGE_INTEGER FrameDuplicateCount;
	LARGE_INTEGER ReceivedFragmentCount;
	LARGE_INTEGER MulticastReceivedFrameCount;
	LARGE_INTEGER FCSErrorCount;
	LARGE_INTEGER TransmittedFrameCount;
	LARGE_INTEGER WEPUndecryptableCount;
	LARGE_INTEGER TransmitCountFrmOs;
} COUNTER_802_11, *PCOUNTER_802_11;

typedef struct _COUNTER_RALINK {
	UINT32 OneSecStart;			/* for one sec count clear use */
	UINT32 OneSecBeaconSentCnt;
	UINT32 OneSecFalseCCACnt;	/* CCA error count, for debug purpose, might move to global counter */
	UINT32 OneSecRxFcsErrCnt;	/* CRC error */
	UINT32 OneSecRxOkCnt;	    /* RX without error */
	UINT32 OneSecTxFailCount;
	UINT32 OneSecTxNoRetryOkCount;
	UINT32 OneSecTxRetryOkCount;
	UINT32 OneSecRxOkDataCnt;	/* unicast-to-me DATA frame count */
	UINT32 OneSecTransmittedByteCount;	/* both successful and failure, used to calculate TX throughput */

	ULONG OneSecOsTxCount[NUM_OF_TX_RING];
	ULONG OneSecDmaDoneCount[NUM_OF_TX_RING];
	UINT32 OneSecTxDoneCount;
	ULONG OneSecRxCount;
	UINT32 OneSecReceivedByteCount;
	UINT32 OneSecTxAggregationCount;
	UINT32 OneSecRxAggregationCount;
	UINT32 OneSecEnd;	/* for one sec count clear use */

	ULONG TransmittedByteCount;	/* both successful and failure, used to calculate TX throughput */
	ULONG ReceivedByteCount;	/* both CRC okay and CRC error, used to calculate RX throughput */
	ULONG BadCQIAutoRecoveryCount;
	ULONG PoorCQIRoamingCount;
	ULONG MgmtRingFullCount;
	ULONG RxCountSinceLastNULL;
	ULONG RxCount;
	ULONG KickTxCount;
	LARGE_INTEGER RealFcsErrCount;
	ULONG PendingNdisPacketCount;
	ULONG FalseCCACnt;              /* CCA error count */

	UINT32 LastOneSecTotalTxCount;	/* OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount */
	UINT32 LastOneSecRxOkDataCnt;	/* OneSecRxOkDataCnt */
	ULONG DuplicateRcv;
	ULONG TxAggCount;
	ULONG TxNonAggCount;
	ULONG TxAgg1MPDUCount;
	ULONG TxAgg2MPDUCount;
	ULONG TxAgg3MPDUCount;
	ULONG TxAgg4MPDUCount;
	ULONG TxAgg5MPDUCount;
	ULONG TxAgg6MPDUCount;
	ULONG TxAgg7MPDUCount;
	ULONG TxAgg8MPDUCount;
	ULONG TxAgg9MPDUCount;
	ULONG TxAgg10MPDUCount;
	ULONG TxAgg11MPDUCount;
	ULONG TxAgg12MPDUCount;
	ULONG TxAgg13MPDUCount;
	ULONG TxAgg14MPDUCount;
	ULONG TxAgg15MPDUCount;
	ULONG TxAgg16MPDUCount;

	LARGE_INTEGER TransmittedOctetsInAMSDU;
	LARGE_INTEGER TransmittedAMSDUCount;
	LARGE_INTEGER ReceivedOctesInAMSDUCount;
	LARGE_INTEGER ReceivedAMSDUCount;
	LARGE_INTEGER TransmittedAMPDUCount;
	LARGE_INTEGER TransmittedMPDUsInAMPDUCount;
	LARGE_INTEGER TransmittedOctetsInAMPDUCount;
	LARGE_INTEGER MPDUInReceivedAMPDUCount;
} COUNTER_RALINK, *PCOUNTER_RALINK;

typedef struct _COUNTER_TMP_1S
{
	UINT32 FalseCCACnt1S;
	UINT32 RxCntU2M1S;
	UINT32 RxCntBM1S;
	UINT32 RxCntMgmtCntl1S;
}COUNTER_TMP_1S,*PCOUNTER_TMP_1S;


typedef struct GNU_PACKED _ATE_INFO {
	UINT8  bATEMode;
	UINT8  ATECmdFmt;	 /*ATE cmd format : 0=AT#ATECAL ,   1=iwpriv ra0*/
	PUCHAR MacSA;
	PUCHAR MacBssid;
	PUCHAR MacDA;
	UINT8  Channel;
	
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
	UINT32 RxMgmtCntlTotalCnt;
	UINT32 RxCntU2MPerSec;
	UINT32 RxCntBMPerSec;
	UINT32 RxCntMgmtCntlPerSec;
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

	COUNTER_802_3 Counters8023; /* 802.3 counters */
	COUNTER_802_11 WlanCounters;	/* 802.11 MIB counters */
	COUNTER_RALINK RalinkCounters;	/* Ralink propriety counters */
	COUNTER_TMP_1S OneSecCountersTmp; 
} ATE_INFO, *PATE_INFO;


typedef enum {
	ATE_MODE_OFF = 0,
	ATE_MODE_TX,
	ATE_MODE_RX,
	ATE_MODE_MAX
}ATE_MODE;

typedef enum {
	ATE_CMD_TYPE_AT = 0,
	ATE_CMD_TYPE_IWPRIV,
	ATE_CMD_TYPE_MAX
}ATE_CMD_TYPE;


VOID SendATETxDataFrame(VOID);
BOOLEAN	Set_ResetStatCounter_Proc(VOID);
#endif  //__ATE_H__
