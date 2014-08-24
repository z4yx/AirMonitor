#ifndef _AP_PUB_H_
#define _AP_PUB_H_

#include "types.h"
#include "iot_api.h"

#define MAX_NUMBER_OF_MAC		32	/* if MAX_MBSSID_NUM is 8, this value can't be larger than 211 */
#define MAX_BEACON_SIZE		    512
#define MAX_SSID_LEN            32

#define TBTT_PRELOAD_TIME       384 /* usec. LomgPreamble + 24-byte at 1Mbps */
#define DEFAULT_DTIM_PERIOD     3

#define MAC_TABLE_AGEOUT_TIME			300			/* unit: sec */
#define MAC_TABLE_MIN_AGEOUT_TIME		60			/* unit: sec */
#define MAC_TABLE_ASSOC_TIMEOUT			5			/* unit: sec */
#define MAC_TABLE_FULL(Tab)				((Tab).size == MAX_LEN_OF_MAC_TABLE)

#define CAP_GENERATE(ess,ibss,priv,s_pre,s_slot,spectrum)  \
	(((ess) ? 0x0001 : 0x0000) | ((ibss) ? 0x0002 : 0x0000) | \
	((priv) ? 0x0010 : 0x0000) | ((s_pre) ? 0x0020 : 0x0000) | \
	((s_slot) ? 0x0400 : 0x0000) | ((spectrum) ? 0x0100 : 0x0000))

/***************************************************************************
  *	Multiple SSID related data structures
  **************************************************************************/
#define WLAN_MAX_NUM_OF_TIM			((MAX_LEN_OF_MAC_TABLE >> 3) + 1)	/* /8 + 1 */
#define WLAN_CT_TIM_BCMC_OFFSET		0	/* unit: 32B */


/*============================================================ */
/* ASIC WCID Table definition. */
/*============================================================ */
#define BSSID_WCID		1	/* in infra mode, always put bssid with this WCID */
#define MCAST_WCID	    0x0
#define BSS0Mcast_WCID	0x0
#define BSS1Mcast_WCID	0xf8
#define BSS2Mcast_WCID	0xf9
#define BSS3Mcast_WCID	0xfa
#define BSS4Mcast_WCID	0xfb
#define BSS5Mcast_WCID	0xfc
#define BSS6Mcast_WCID	0xfd
#define BSS7Mcast_WCID	0xfe
#define RESERVED_WCID	0xff

#define MAX_LEN_OF_MAC_TABLE            MAX_NUMBER_OF_MAC	/* if MAX_MBSSID_NUM is 8, this value can't be larger than 211 */

/*#if MAX_LEN_OF_MAC_TABLE>MAX_AVAILABLE_CLIENT_WCID */
/*#error MAX_LEN_OF_MAC_TABLE can not be larger than MAX_AVAILABLE_CLIENT_WCID!!!! */
/*#endif */

typedef enum _MAC_ENTRY_OP_MODE_ {
	ENTRY_OP_MODE_ERROR = 0x00,
	ENTRY_OP_MODE_CLI 	= 0x01,	/* Sta mode, set this TRUE after Linkup,too. */
	ENTRY_OP_MODE_WDS 	= 0x02,	/* This is WDS Entry. only for AP mode. */
	ENTRY_OP_MODE_APCLI = 0x04,	/* This is a AP-Client entry, only for AP mode which enable AP-Client functions. */
	ENTRY_OP_MODE_MESH 	= 0x08,	/* Peer conect with us via mesh. */
	ENTRY_OP_MODE_DLS 	= 0x10,	/* This is DLS Entry. only for STA mode. */
	ENTRY_OP_MODE_MAX 	= 0x20
} MAC_ENTRY_OP_MODE;

/* Values of LastSecTxRateChangeAction */
#define RATE_NO_CHANGE	0		/* No change in rate */
#define RATE_UP			1		/* Trying higher rate or same rate with different BF */
#define RATE_DOWN		2		/* Trying lower rate */

typedef union _HTTRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct {
		USHORT MODE:2;	/* Use definition MODE_xxx. */
		USHORT iTxBF:1;
		USHORT rsv:1;
		USHORT eTxBF:1;
		USHORT STBC:2;	/*SPACE */
		USHORT ShortGI:1;
		USHORT BW:1;	/*channel bandwidth 20MHz or 40 MHz */
		USHORT MCS:7;	/* MCS */
	} field;
#else
	struct {
		USHORT MCS:7;	/* MCS */
		USHORT BW:1;	/*channel bandwidth 20MHz or 40 MHz */
		USHORT ShortGI:1;
		USHORT STBC:2;	/*SPACE */
		USHORT eTxBF:1;
		USHORT rsv:1;
		USHORT iTxBF:1;
		USHORT MODE:2;	/* Use definition MODE_xxx. */
	} field;
#endif
	USHORT word;
} HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;


#ifdef RT_BIG_ENDIAN
typedef	union _BCN_TIME_CFG_STRUC {
	struct {
		UINT32 TxTimestampCompensate:8;
		UINT32 :3;
		UINT32 bBeaconGen:1;		/* Enable beacon generator */
		UINT32 bTBTTEnable:1;
		UINT32 TsfSyncMode:2;		/* Enable TSF sync, 00: disable, 01: infra mode, 10: ad-hoc mode */
		UINT32 bTsfTicking:1;		/* Enable TSF auto counting */
		UINT32 BeaconInterval:16;  /* in unit of 1/16 TU */
	} field;
	UINT32 word;
} BCN_TIME_CFG_STRUC;
#else
typedef union _BCN_TIME_CFG_STRUC {
	struct {
		UINT32 BeaconInterval:16;
		UINT32 bTsfTicking:1;
		UINT32 TsfSyncMode:2;
		UINT32 bTBTTEnable:1;
		UINT32 bBeaconGen:1;
		UINT32 :3;
		UINT32 TxTimestampCompensate:8;
	} field;
	UINT32 word;
} BCN_TIME_CFG_STRUC;
#endif


typedef enum _RT_802_11_PREAMBLE {
	Rt802_11PreambleLong,
	Rt802_11PreambleShort,
	Rt802_11PreambleAuto
} RT_802_11_PREAMBLE, *PRT_802_11_PREAMBLE;


/*AP configure*/
typedef struct _COMMON_CONFIG {
		BOOLEAN bCountryFlag;
		UCHAR CountryCode[3];

		UCHAR CountryRegion;		/* Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel */
		UCHAR CountryRegionForABand;/* Enum of country region for A band */
		UCHAR PhyMode;
		UCHAR cfg_wmode;
		UCHAR SavedPhyMode;
		USHORT Dsifs;				/* in units of usec */

		USHORT BeaconPeriod;
		UCHAR Channel;
		UCHAR CentralChannel;		/* Central Channel when using 40MHz is indicating. not real channel. */
	
		UCHAR SupRate[MAX_LEN_OF_SUPPORTED_RATES];
		UCHAR SupRateLen;
		UCHAR ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
		UCHAR ExtRateLen;
		UCHAR DesireRate[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
		UCHAR MaxDesiredRate;
		UCHAR ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];
	
		ULONG BasicRateBitmap;		/* backup basic ratebitmap */
		ULONG BasicRateBitmapOld;	/* backup basic ratebitmap */

		
		UCHAR TxPower;				/* in unit of mW */
		ULONG TxPowerPercentage;	/* 0~100 % */
		ULONG TxPowerDefault;		/* keep for TxPowerPercentage */

		ULONG TxPreamble;			/* Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto */
		BOOLEAN bUseZeroToDisableFragment;	/* Microsoft use 0 as disable */
		ULONG UseBGProtection;		/* 0: auto, 1: always use, 2: always not use */
		BOOLEAN bUseShortSlotTime;	/* 0: disable, 1 - use short slot (9us) */
		BOOLEAN bEnableTxBurst;		/* 1: enble TX PACKET BURST (when BA is established or AP is not a legacy WMM AP), 0: disable TX PACKET BURST */
		BOOLEAN bAggregationCapable;/* 1: enable TX aggregation when the peer supports it */
		BOOLEAN bPiggyBackCapable;	/* 1: enable TX piggy-back according MAC's version */
		BOOLEAN bIEEE80211H;		/* 1: enable IEEE802.11h spec. */
		UCHAR RDDurRegion; 			/* Region of radar detection */
		ULONG DisableOLBCDetect;	/* 0: enable OLBC detect; 1 disable OLBC detect */
		UINT8 PwrConstraint;
} COMMON_CONFIG, *PCOMMON_CONFIG;


typedef struct _MULTISSID_STRUCT {

	UCHAR Bssid[MAC_ADDR_LEN];
	UCHAR SsidLen;
	CHAR Ssid[MAX_LEN_OF_SSID];
	USHORT CapabilityInfo;

	UCHAR MaxStaNum;				/* Limit the STA connection number per BSS */
	UCHAR StaCount;

	NDIS_802_11_AUTHENTICATION_MODE AuthMode;
	NDIS_802_11_WEP_STATUS WepStatus;
	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
	//WPA_MIX_PAIR_CIPHER WpaMixPairCipher;

	/*CIPHER_KEY SharedKey[SHARE_KEY_NUM]; // ref pAd->SharedKey[BSS][4] */
	UCHAR DefaultKeyId;

	UCHAR TxRate; 					/* RATE_1, RATE_2, RATE_5_5, RATE_11, ... */
	UCHAR DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
	UCHAR DesiredRatesIndex;
	UCHAR MaxTxRate; 				/* RATE_1, RATE_2, RATE_5_5, RATE_11 */

	/*ULONG TimBitmap; */      		// bit0 for broadcast, 1 for AID1, 2 for AID2, ...so on
	/*ULONG TimBitmap2; */     		// b0 for AID32, b1 for AID33, ... and so on */
	UCHAR TimBitmaps[WLAN_MAX_NUM_OF_TIM];

	/* outgoing BEACON frame buffer and corresponding TXWI */
	/* TXWI_STRUC *BeaconTxWI; */
	CHAR BeaconBuf[MAX_BEACON_SIZE];/* NOTE: BeaconBuf should be 4-byte aligned */

	BOOLEAN bHideSsid;
	UINT16 StationKeepAliveTime;	/* unit: second */

	/* used in if beacon send or stop */
	BOOLEAN bBcnSntReq;
	UCHAR BcnBufIdx;
	UCHAR PhyMode;
} MULTISSID_STRUCT, *PMULTISSID_STRUCT;



typedef struct GNU_PACKED _AP_ADMIN_CONFIG {
	/***********************************************/
	/*Reference to BSSID */
	/***********************************************/
	MULTISSID_STRUCT MBSSID;
	
	COMMON_CONFIG CommonCfg;

	TXWI_STRUC BeaconTxWI;
	UINT8 TXWISize;
	USHORT Aid;
	UCHAR DtimCount;	/* 0.. DtimPeriod-1 */
	UCHAR DtimPeriod;	/* default = 3 */

} AP_ADMIN_CONFIG, *PAP_ADMIN_CONFIG;




INT AsicSetPreTbttInt(BOOLEAN enable);
VOID AsicEnableBssSync(VOID);
VOID APMakeBssBeacon(VOID);
VOID APStartUp(VOID);
VOID PreApCfgInit(VOID);
INT  APInitialize(VOID);
VOID BeaconFrameSendAction(UINT_32 param, UINT_32 param2);

#endif //_AP_PUB_H_

