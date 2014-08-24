#ifndef _EEPROM_H_
#define _EEPROM_H_
/******************************************************************************
 * MODULE NAME:     eeprom.h
 * PROJECT CODE:    __RT7681__
 * DESCRIPTION:
 * DESIGNER:        Jinchuan Bao
 * DATE:            Jan 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 Mediatek Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Jan 2014    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/


/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/
/*------------------------------------------------------------------------- */
/* EEPROM Default */
/*------------------------------------------------------------------------- */
#define DEFAULT_TX_POWER	0x6




/*------------------------------------------------------------------------- */
/* EEPROM definition */
/*------------------------------------------------------------------------- */
#define EEDO                        	0x08
#define EEDI                        	0x04
#define EECS                        	0x02
#define EESK                        	0x01
#define EERL                        	0x80

#define EEPROM_WRITE_OPCODE         	0x05
#define EEPROM_READ_OPCODE          	0x06
#define EEPROM_EWDS_OPCODE          	0x10
#define EEPROM_EWEN_OPCODE          	0x13

#define NUM_EEPROM_BBP_PARMS			19	/* Include NIC Config 0, 1, CR, TX ALC step, BBPs */
#define NUM_EEPROM_TX_G_PARMS			7

#define VALID_EEPROM_VERSION        	1
#define EEPROM_VERSION_OFFSET       	0x02

#define EEPROM_MAC_12_OFFSET     		0x04
#define EEPROM_MAC_34_OFFSET			0x06
#define EEPROM_MAC_56_OFFSET       		0x08

#define EEPROM_NIC1_OFFSET          	0x34	/* The address is from NIC config 0, not BBP register ID */
#define EEPROM_NIC2_OFFSET          	0x36	/* The address is from NIC config 1, not BBP register ID */


#define EEPROM_COUNTRY_REGION			0x38

#define EEPROM_DEFINE_MAX_TXPWR			0x4e

#define EEPROM_FREQ_OFFSET				0x3a
#define EEPROM_LEDAG_CONF_OFFSET		0x3c
#define EEPROM_LEDACT_CONF_OFFSET		0x3e
#define EEPROM_LED_POLARITY_OFFSET		0x40

#define EEPROM_LNA_OFFSET				0x44

#define EEPROM_RSSI_BG_OFFSET			0x46
#define EEPROM_RSSI_A_OFFSET			0x4a
#define EEPROM_TXMIXER_GAIN_2_4G		0x48
#define EEPROM_TXMIXER_GAIN_5G			0x4c

#define EEPROM_TXPOWER_DELTA			0x50	/* 20MHZ AND 40 MHZ use different power. This is delta in 40MHZ. */

#define EEPROM_G_TX_PWR_OFFSET			0x52
#define EEPROM_G_TX2_PWR_OFFSET			0x60

#define EEPROM_G_TSSI_BOUND1			0x6e
#define EEPROM_G_TSSI_BOUND2			0x70
#define EEPROM_G_TSSI_BOUND3			0x72
#define EEPROM_G_TSSI_BOUND4			0x74
#define EEPROM_G_TSSI_BOUND5			0x76

#define EEPROM_TX0_TSSI_SLOPE			0x6e
#define EEPROM_TX0_TSSI_OFFSET_GROUP1	0x70
#define EEPROM_TX0_TSSI_OFFSET			0x76
#define EEPROM_G_TARGET_POWER			0xD0

#define EEPROM_A_TX_PWR_OFFSET      	0x78
#define EEPROM_A_TX2_PWR_OFFSET			0xa6

#define EEPROM_TXPOWER_BYRATE 				0xde	/* 20MHZ power. */
#define EEPROM_TXPOWER_BYRATE_20MHZ_2_4G	0xde	/* 20MHZ 2.4G tx power. */
#define EEPROM_TXPOWER_BYRATE_40MHZ_2_4G	0xee	/* 40MHZ 2.4G tx power. */










#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_ANTENNA_STRUC {
	struct {
		USHORT RssiIndicationMode:1; 	/* RSSI indication mode */
		USHORT Rsv:1;
		USHORT BoardType:2; 		/* 0: mini card; 1: USB pen */
		USHORT RfIcType:4;			/* see E2PROM document */
		USHORT TxPath:4;			/* 1: 1T, 2: 2T, 3: 3T */
		USHORT RxPath:4;			/* 1: 1R, 2: 2R, 3: 3R */
	} field;
	USHORT word;
} EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#else
typedef union _EEPROM_ANTENNA_STRUC {
	struct {
		USHORT RxPath:4;			/* 1: 1R, 2: 2R, 3: 3R */
		USHORT TxPath:4;			/* 1: 1T, 2: 2T, 3: 3T */
		USHORT RfIcType:4;			/* see E2PROM document */
		USHORT BoardType:2; 		/* 0: mini card; 1: USB pen */
		USHORT Rsv:1;
		USHORT RssiIndicationMode:1; 	/* RSSI indication mode */	
	} field;
	USHORT word;
} EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_NIC_CINFIG2_STRUC {
	struct {
		USHORT DACTestBit:1;	/* control if driver should patch the DAC issue */
#ifdef RELEASE_EXCLUDE
		/* Wi-Fi / Bluetooth coexistence */
#endif				/* RELEASE_EXCLUDE */
		USHORT CoexBit:1;
		USHORT bInternalTxALC:1;	/* Internal Tx ALC */
		USHORT AntOpt:1;	/* Fix Antenna Option: 0:Main; 1: Aux */
		USHORT AntDiversity:1;	/* Antenna diversity */
		USHORT Rsv1:1;	/* must be 0 */
		USHORT BW40MAvailForA:1;	/* 0:enable, 1:disable */
		USHORT BW40MAvailForG:1;	/* 0:enable, 1:disable */
		USHORT EnableWPSPBC:1;	/* WPS PBC Control bit */
		USHORT BW40MSidebandForA:1;
		USHORT BW40MSidebandForG:1;
		USHORT CardbusAcceleration:1;	/* !!! NOTE: 0 - enable, 1 - disable */
		USHORT ExternalLNAForA:1;	/* external LNA enable for 5G */
		USHORT ExternalLNAForG:1;	/* external LNA enable for 2.4G */
		USHORT DynamicTxAgcControl:1;	/* */
		USHORT HardwareRadioControl:1;	/* Whether RF is controlled by driver or HW. 1:enable hw control, 0:disable */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#else
typedef union _EEPROM_NIC_CINFIG2_STRUC {
	struct {
		USHORT HardwareRadioControl:1;	/* 1:enable, 0:disable */
		USHORT DynamicTxAgcControl:1;	/* */
		USHORT ExternalLNAForG:1;	/* */
		USHORT ExternalLNAForA:1;	/* external LNA enable for 2.4G */
		USHORT CardbusAcceleration:1;	/* !!! NOTE: 0 - enable, 1 - disable */
		USHORT BW40MSidebandForG:1;
		USHORT BW40MSidebandForA:1;
		USHORT EnableWPSPBC:1;	/* WPS PBC Control bit */
		USHORT BW40MAvailForG:1;	/* 0:enable, 1:disable */
		USHORT BW40MAvailForA:1;	/* 0:enable, 1:disable */
		USHORT Rsv1:1;	/* must be 0 */
		USHORT AntDiversity:1;	/* Antenna diversity */
		USHORT AntOpt:1;	/* Fix Antenna Option: 0:Main; 1: Aux */
		USHORT bInternalTxALC:1;	/* Internal Tx ALC */
#ifdef RELEASE_EXCLUDE
		/* Wi-Fi / Bluetooth coexistence */
#endif				/* RELEASE_EXCLUDE */
		USHORT CoexBit:1;
		USHORT DACTestBit:1;	/* control if driver should patch the DAC issue */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#endif


/*
	TX_PWR Value valid range 0xFA(-6) ~ 0x24(36)
*/
#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_TX_PWR_STRUC {
	struct {
		signed char Byte1;	/* High Byte */
		signed char Byte0;	/* Low Byte */
	} field;
	USHORT word;
} EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#else
typedef union _EEPROM_TX_PWR_STRUC {
	struct {
		signed char Byte0;	/* Low Byte */
		signed char Byte1;	/* High Byte */
	} field;
	USHORT word;
} EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#endif


/* Channel list subset */
typedef struct _CHANNEL_LIST_SUB {
	UCHAR	Channel;
	UCHAR	IdxMap; /* Index mapping to original channel list */
} CHANNEL_LIST_SUB, *PCHANNEL_LIST_SUB;


// structure to store channel TX power
typedef struct GNU_PACKED _CHANNEL_TX_POWER {
	//UCHAR     Channel;
	UCHAR      Power;
} CHANNEL_TX_POWER, *PCHANNEL_TX_POWER;


#define MAX_TXPOWER_ARRAY_SIZE	5


typedef struct GNU_PACKED _EEPROM_CONFIG {

	/* ---------------------------- */
	/* RFIC control */
	/* ---------------------------- */
	//UCHAR RfIcType;		/* RFIC_xxx */
	UCHAR RfFreqOffset;	/* Frequency offset for channel switching */
	//EEPROM_ANTENNA_STRUC Antenna;	/* Since ANtenna definition is different for a & g. We need to save it for future reference. */
	//EEPROM_NIC_CONFIG2_STRUC NicConfig2;

	CHANNEL_TX_POWER TxPower[MAX_NUM_OF_CHANNELS];	/* Store Tx power value for all channels. */
	//CHANNEL_TX_POWER ChannelList[MAX_NUM_OF_CHANNELS];	/* list all supported channels for site survey */
	
	//UCHAR CountryRegion;	/* Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel */
	//UCHAR CountryRegionForABand;	/* Enum of country region for A band */

	UCHAR TargetPower;
	
#if 1//(ATCMD_ATE_SUPPORT == 1)
	signed char BGRssiOffset[3]; /* Store B/G RSSI #0/1/2 Offset value on EEPROM 0x46h */
	//signed char ARssiOffset[3];  /* Store A RSSI 0/1/2 Offset value on EEPROM 0x4Ah */
#endif

	UCHAR BLNAGain;		/* Store B/G external LNA#0 value on EEPROM 0x44h */
	UCHAR ALNAGain0; 	/* Store A external LNA#0 value for ch36~64 */
	//CHAR ALNAGain1; 	/* Store A external LNA#1 value for ch100~128 */
	//CHAR ALNAGain2; 	/* Store A external LNA#2 value for ch132~165 */

	//ULONG Tx20MPwrCfgABand[MAX_TXPOWER_ARRAY_SIZE];
	ULONG Tx20MPwrCfgGBand[MAX_TXPOWER_ARRAY_SIZE];
	//ULONG Tx40MPwrCfgABand[MAX_TXPOWER_ARRAY_SIZE];
	ULONG Tx40MPwrCfgGBand[MAX_TXPOWER_ARRAY_SIZE];

	UINT32 TxCCKPwrCfg;

} EEPROM_CFG, *PEEPROM_CFG;

#endif /* _EFUSE_H_ */

