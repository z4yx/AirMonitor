#ifndef __DOT11_BASE_H__
#define __DOT11_BASE_H__

#include "types.h"

//#define MAC_ADDR_LEN                6

// 4-byte HTC field.  maybe included in any frame except non-QOS data frame.  The Order bit must set 1.
typedef struct GNU_PACKED {
    UINT32		rsv:1;	// Reserved
    UINT32		TRQ:1;	//sounding request
    UINT32		MRQ:1;	//MCS feedback. Request for a MCS feedback
    UINT32		MSI:3;	//MCS Request, MRQ Sequence identifier
    UINT32		MFSI:3;	//SET to the received value of MRS. 0x111 for unsolicited MFB.
    UINT32		MFBorASC:7;	//Link adaptation feedback containing recommended MCS. 0x7f for no feedback or not available
    UINT32		CalPos:2;	// calibration position
    UINT32		CalSeq:2;  //calibration sequence
    UINT32		rsv1:2;	// Reserved
    UINT32		CSISTEERING:2;	//CSI/ STEERING
    UINT32		NDPAnnouce:1;	// ZLF announcement
    UINT32		rsv2:5;  //calibration sequence
    UINT32		ACConstraint:1;	//feedback request
    UINT32		RDG:1;	//RDG / More PPDU
} HT_CONTROL, *PHT_CONTROL;

// 2-byte QOS CONTROL field
typedef struct GNU_PACKED {
    USHORT      TID:4;
    USHORT      EOSP:1;
    USHORT      AckPolicy:2;  //0: normal ACK 1:No ACK 2:scheduled under MTBA/PSMP  3: BA
    USHORT      AMsduPresent:1;
    USHORT      Txop_QueueSize:8;
} QOS_CONTROL, *PQOS_CONTROL;	


// 2-byte Frame control field
typedef	struct	GNU_PACKED {
	USHORT		Ver:2;				// Protocol version
	USHORT		Type:2;				// MSDU type
	USHORT		SubType:4;			// MSDU subtype
	USHORT		ToDs:1;				// To DS indication
	USHORT		FrDs:1;				// From DS indication
	USHORT		MoreFrag:1;			// More fragment bit
	USHORT		Retry:1;			// Retry status bit
	USHORT		PwrMgmt:1;			// Power management bit
	USHORT		MoreData:1;			// More data bit
	USHORT		Wep:1;				// Wep data
	USHORT		Order:1;			// Strict order expected
} FRAME_CONTROL, *PFRAME_CONTROL;

typedef	struct	GNU_PACKED _HEADER_802_11	{
    FRAME_CONTROL   FC;
    USHORT          Duration;
    UCHAR           Addr1[MAC_ADDR_LEN];
    UCHAR           Addr2[MAC_ADDR_LEN];
	UCHAR			Addr3[MAC_ADDR_LEN];
	USHORT			Frag:4;
	USHORT			Sequence:12;
	UCHAR			Octet[0];
}	HEADER_802_11, *PHEADER_802_11;

typedef	struct	GNU_PACKED _RTS_FRAME	{
    FRAME_CONTROL   FC;
    USHORT          Duration;
    UCHAR           Addr1[MAC_ADDR_LEN];
    UCHAR           Addr2[MAC_ADDR_LEN];
}RTS_FRAME, *PRTS_FRAME;

/* 2-byte BA Starting Seq CONTROL field */
typedef union GNU_PACKED _BASEQ_CONTROL{
    struct GNU_PACKED {
    USHORT      FragNum:4;	/* always set to 0 */
	USHORT      StartSeq:12;   /* sequence number of the 1st MSDU for which this BAR is sent */
    }   field;
    USHORT           word;
} BASEQ_CONTROL, *PBASEQ_CONTROL;

/* 2-byte BA Parameter Set field  in ADDBA frames to signal parm for setting up a BA */
typedef struct GNU_PACKED _BA_PARM{
    USHORT      AMSDUSupported:1;	/* 0: not permitted		1: permitted */
    USHORT      BAPolicy:1;	/* 1: immediately BA    0:delayed BA */
    USHORT      TID:4;	/* value of TC os TS */
    USHORT      BufSize:10;	/* number of buffe of size 2304 octetsr */
} BA_PARM, *PBA_PARM;

typedef struct GNU_PACKED _FRAME_ADDBA_REQ {
	HEADER_802_11   Hdr;
	UCHAR	Category;
	UCHAR	Action;
	UCHAR	Token;	/* 1 */
	BA_PARM		BaParm;	      /*  2 - 10 */
	USHORT		TimeOutValue;	/* 0 - 0 */
	BASEQ_CONTROL	BaStartSeq; /* 0-0 */
}   FRAME_ADDBA_REQ, *PFRAME_ADDBA_REQ;

typedef struct GNU_PACKED _FRAME_ADDBA_RSP {
	HEADER_802_11   Hdr;
	UCHAR	Category;
	UCHAR	Action;
	UCHAR	Token;
	USHORT	StatusCode;
	BA_PARM		BaParm; /*0 - 2 */
	USHORT		TimeOutValue;
}   FRAME_ADDBA_RSP, *PFRAME_ADDBA_RSP;

#if (ADV_PWR_SAVING == 1)  	
typedef struct GNU_PACKED _PSPOLL_FRAME {
	FRAME_CONTROL FC;
	USHORT Aid;
	UCHAR Bssid[MAC_ADDR_LEN];
	UCHAR Ta[MAC_ADDR_LEN];
} PSPOLL_FRAME, *PPSPOLL_FRAME;
#endif /* ADV_PWR_SAVING */

#endif // __DOT11_BASE_H__ //
