#ifndef _TYPES_H_
#define _TYPES_H_
/******************************************************************************
* MODULE NAME:     types.h
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
 * RESERVED STRING
 ******************************************************************************/
/* Directing Symbol */
#ifdef IN
#error "MACRO IN has been defined. Please try to make it unique."
#else
#define IN
#endif

#ifdef OUT
#error "MACRO OUT has been defined. Please try to make it unique."
#else
#define OUT
#endif

#ifdef INOUT
#error "MACRO INOUT has been defined. Please try to make it unique."
#else
#define INOUT
#endif

#ifdef GLOBAL
#error "MACRO GLOBAL has been defined. Please try to make it unique."
#else
#define GLOBAL
#endif

/* Register characteristics */
#ifdef URW
#error "MACRO URW has been defined. Please try to make it unique."
#else
#define URW     /* UnReadable-and-UnWritable */
#endif

#ifdef RO
#error "MACRO RO has been defined. Please try to make it unique."
#else
#define RO      /* Read-Only */
#endif

#ifdef WO
#error "MACRO WO has been defined. Please try to make it unique."
#else
#define WO      /* Write-Only */
#endif

#ifdef RW
#error "MACRO RW has been defined. Please try to make it unique."
#else
#define RW      /* Readable-and-Writable */
#endif

#ifdef W1C
#error "MACRO W1C has been defined. Please try to make it unique."
#else
#define W1C     /* Readable-Write-1-Clear */
#endif

#ifdef WSC
#error "MACRO WSC has been defined. Please try to make it unique."
#else
#define WSC     /* Write-then-Self-Clear */
#endif

/* Default Symbol */
#ifdef EXTERN
#error "MACRO EXTERN has been defined. Please try to make it unique."
#else
#define EXTERN extern
#endif

#ifdef STATIC
#error "MACRO STATIC has been defined. Please try to make it unique."
#else
#define STATIC static
#endif

#ifdef VOLATILE
#error "MACRO VOLATILE has been defined. Please try to make it unique."
#else
#define VOLATILE volatile
#endif

#ifdef REGISTER
#error "MACRO REGISTER has been defined. Please try to make it unique."
#else
#define REGISTER register
#endif

#ifdef INLINE
#error "MACRO INLINE has been defined. Please try to make it unique."
#else
#define INLINE inline
#endif

#ifdef CONST
#error "MACRO CONST has been defined. Please try to make it unique."
#else
#define CONST const
#endif

/* */
/*  Some utility macros */
/* */
#ifndef min
#define min(_a, _b)     (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b)     (((_a) > (_b)) ? (_a) : (_b))
#endif

/******************************************************************************
 * SYSTEM DEPENDED TYPE DEFINITION
 ******************************************************************************/
/* Boolean Type */
typedef unsigned int        BOOL;

/* Sized Types */
typedef char                INT8;
typedef short               INT16;
typedef int                 INT32;
typedef long long           INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned short      UINT16LE;
typedef unsigned int        UINT32;
typedef unsigned int        UINT32LE;
typedef unsigned long long  UINT64;

/* Void Type */
typedef void                VOID;
typedef void*               PVOID;

/* NULL */
#ifndef NULL
#define NULL                ((VOID *)0)
#endif

/* Boolean Values */
#ifndef FALSE
#define FALSE               (0)
#endif
#ifndef TRUE
#define TRUE                (!FALSE)
#endif

/* Logical Values */
#ifndef LOW
#define LOW                 (0)
#endif
#ifndef HIGH
#define HIGH                (1)
#endif



/******************************************************************************
 * MACRO DEFINITION
 ******************************************************************************/
#define OFFSET_OF(type,member) \
    ((PVOID)(&((type*)0)->member))

#define CONTAINER_OF(ptr,type,member) \
    ((type*)((UINT8*)(ptr)-(UINT8*)OFFSET_OF(type,member)))

#define CASTING16(ptr)                        \
    (   ((UINT16)(*(UINT8*)((ptr))))        | \
        ((UINT16)(*(UINT8*)((ptr)+1))<<8)   )

#define CASTING24(ptr)                        \
    (   ((UINT32)(*(UINT8*)((ptr))))        | \
        ((UINT32)(*(UINT8*)((ptr)+1))<<8)   | \
        ((UINT32)(*(UINT8*)((ptr)+2))<<16)  )

#define CASTING32(ptr)                        \
    (   ((UINT32)(*(UINT8*)((ptr))))        | \
        ((UINT32)(*(UINT8*)((ptr)+1))<<8)   | \
        ((UINT32)(*(UINT8*)((ptr)+2))<<16)  | \
        ((UINT32)(*(UINT8*)((ptr)+3))<<24)  )

/* MACRO IS_ONE_OF_THEM */
#define IS_ONE_OF_THEM(var,mask)        (((var)&(mask)) != 0)

/* MACRO IS_ALL_OF_THEM */
#define IS_ALL_OF_THEM(var,mask)        (((var)&(mask)) == (mask))

/* MACRO IS_EVEN */
#define IS_EVEN(value)                  ((((UINT32)(value))&(1UL)) == 0)

/* MACRO IS ODD */
#define IS_ODD(value)                   ((((UINT32)(value))&(1UL)) != 0)

/* MACRO UNUSED */
#define UNUSED(variable)                ((variable) = (variable))






/******************************************************************************
 * SYSTEM DEPENDED STRING
 ******************************************************************************/
/* move from rtmp_def.h */
#define GNU_PACKED  __attribute__ ((packed))

/* I DO NOT suggest to use this following types -- Charles Su */
typedef unsigned char			UCHAR;
typedef unsigned short			USHORT;
typedef unsigned int			UINT;
typedef unsigned long			ULONG;

typedef signed char         	CHAR;
typedef signed short			SHORT;
typedef signed int				INT;
typedef signed long				LONG;
typedef signed long long		LONGLONG;	

typedef UCHAR * 				PUCHAR;
typedef unsigned char *			PUINT8;
typedef unsigned short *		PUINT16;
typedef unsigned int *			PUINT32;
typedef unsigned long long *	PUINT64;
typedef int	*					PINT32;
typedef long long * 			PINT64;

typedef char					int8;
typedef short 					int16;
typedef int						int32;
typedef long long 				int64;
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned long long		uint64;

typedef int8                    bool;
typedef uint8                   BYTE;
typedef uint16                  WORD;
typedef uint32                  DWORD;
typedef unsigned char			BOOLEAN;

/* 
 * Unsigned types
 */
typedef unsigned char			u_int8;        /* unsigned integer,  8 bits long */
typedef unsigned short 			u_int16;  		/* unsigned integer, 16 bits long */
typedef unsigned int			u_int32;       /* unsigned integer, 32 bits long */

/*
 * Signed types 
 */
typedef signed char 			s_int8;
typedef signed short 			s_int16;
typedef signed int 				s_int32;

/******************************************************************************
 * SYSTEM DEPENDED TYPE DEFINITION -- For7681Driver  //jinchuan.bao add test
 ******************************************************************************/
typedef char*					PSTRING;
typedef CHAR*					PCHAR;
typedef USHORT*					PUSHORT;
typedef LONG*					PLONG;
typedef ULONG*					PULONG;
typedef UINT*					PUINT;

/******************************************************************************
 * SYSTEM USE TYPE DEFINITION
 ******************************************************************************/
typedef UINT32         			OS_SYSTIME, *POS_SYSTIME;
typedef USHORT          		UINT_16, *PUINT_16, **PPUINT_16; /* 16-bit unsigned value & pointer */
typedef ULONG           		UINT_32, *PUINT_32, **PPUINT_32; /* 32-bit unsigned value & pointer */


typedef CHAR	     			INT_8,  *PINT_8,  **PPINT_8;  	/*  8-bit signed value & pointer */
typedef LONG            		INT_32, *PINT_32, **PPINT_32;    /* 32-bit signed value & pointer */

/******************************************************************************
 *  KAL  USE TYPE DEFINITION
 ******************************************************************************/
typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef char                    kal_char;
typedef unsigned short          kal_wchar;

typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;

typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;

typedef unsigned long long      kal_uint64;
typedef signed long long        kal_int64;

typedef enum {
   KAL_FALSE,
   KAL_TRUE
} kal_bool;

typedef void (* VOID_FUNC)(void);

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000L
#define BIT24 0x01000000L
#define BIT25 0x02000000L
#define BIT26 0x04000000L
#define BIT27 0x08000000L
#define BIT28 0x10000000L
#define BIT29 0x20000000L
#define BIT30 0x40000000L
#define BIT31 0x80000000UL

#define CONTAINER_OF(ptr,type,member) \
    ((type*)((UINT8*)(ptr)-(UINT8*)OFFSET_OF(type,member)))

#if 0
#define CASTING16(ptr) \
    (   ((UINT16)(*(UINT8*)((ptr)))) | \
        ((UINT16)(*(UINT8*)((ptr)+1))<<8))

#define CASTING32(ptr) \
    (   ((UINT32)(*(UINT8*)((ptr)))) | \
        ((UINT32)(*(UINT8*)((ptr)+1))<<8) | \
        ((UINT32)(*(UINT8*)((ptr)+2))<<16) | \
        ((UINT32)(*(UINT8*)((ptr)+3))<<24))
#endif        

/* MACRO IS_ONE_OF_THEM */
#define IS_ONE_OF_THEM(var,mask)        (((var)&(mask)) != 0)

/* MACRO IS_ALL_OF_THEM */
#define IS_ALL_OF_THEM(var,mask)        (((var)&(mask)) == (mask))

/* MACRO IS_EVEN */
#define IS_EVEN(value)                  ((((UINT32)(value))&(1UL)) == 0)

/* MACRO IS ODD */
#define IS_ODD(value)                   ((((UINT32)(value))&(1UL)) != 0)

/* MACRO UNUSED */
#define UNUSED(variable)                ((variable) = (variable))

/******************************************************************************
 * SYSTEM DEPENDED STRING
 ******************************************************************************/

/* move from rtmp_def.h */
#define GNU_PACKED  __attribute__ ((packed))

#if ((ROM_EXISTED == 1) || (GENERATE_ROMCODE == 1))
/* ROM Section Declaratin */
#define __romtext	__attribute__ ((__section__(".dlmtextsection.text")))
#define __romdata	__attribute__ ((__section__(".dlmtextsection.data")))
#else
#define __romtext
#define __romdata
#endif

typedef enum {
    	io_low=0,
	   io_high
} IO_level;


/******************************************************************************
 * SYSTEM DEPENDED STRING     ----- MT7681 Driver   // Jinchuan.bao add test
 ******************************************************************************/
	/* Length define */
#define MAC_ADDR_LEN					6
#define MAX_LEN_OF_SSID                 32
#define MAX_LEN_OF_BSS_TABLE            1
#define MAX_LENGTH_OF_SUPPORT_RATES		12    // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_LEN_OF_SUPPORTED_RATES      MAX_LENGTH_OF_SUPPORT_RATES // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_VIE_LEN                     384   // New for WPA cipher suite variable IE sizes.
#define TIMESTAMP_LEN                   8
#define BEACON_PERIOD_LEN               2
#define CAPABILITYINFO_LEN              2
#define CIPHER_TEXT_LEN                 32



#define MAX_NUM_OF_CHS             		14     // 14 = 14 channels @2.4G
                                               // 54 = 14 channels @2.4G +  12@UNII(lower/middle) + 16@HiperLAN2 + 11@UNII(upper) + 0 @Japan + 1 as NULL termination
#define MAX_NUM_OF_CHANNELS             MAX_NUM_OF_CHS

/* ======================== Debug ====================== */ 
/* */
/*  Debug information verbosity: lower values indicate higher urgency */
/* */
#define RT_DEBUG_OFF        0
#define RT_DEBUG_ERROR      1
#define RT_DEBUG_TRACE      2
#define RT_DEBUG_INFO       3

/*For debug useage, please use  DBGPRINT(Level, Fmt) */
/*But do not use printf directly */
#define RT_DEBUG_SUPPORT

#ifdef RT_DEBUG_SUPPORT
extern UINT8 RTDebugLevel;

#define DBGPRINT(Level, Fmt)    	\
{                                   \
    if ((Level) <= RTDebugLevel)    \
    {                               \
        printf Fmt;					\
    }                               \
}
#else
#define DBGPRINT(Level, Fmt)
#endif


#ifdef RT_DEBUG_SUPPORT
extern UINT8 RTDebugLevel;

#define DBGPRINT_HIGH(Level, Fmt)   \
{                                   \
    if ((Level) <= RTDebugLevel)    \
    {                               \
        Printf_High Fmt;			\
    }                               \
}
#else
#define DBGPRINT_HIGH(Level, Fmt)
#endif



#define RTMPEqualMemory(Source1, Source2, Length)	(!memcmp(Source1, Source2, Length))
#define COPY_MAC_ADDR(Addr1, Addr2)             memcpy((Addr1), (Addr2), MAC_ADDR_LEN)
#define MAC_ADDR_EQUAL(pAddr1,pAddr2)           RTMPEqualMemory((PVOID)(pAddr1), (PVOID)(pAddr2), MAC_ADDR_LEN)
#define SSID_EQUAL(ssid1, len1, ssid2, len2)    ((len1==len2) && (RTMPEqualMemory(ssid1, ssid2, len1)))

#define PRINT_MAC(addr)	\
	addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
	

#endif /* _TYPES_H_ */

