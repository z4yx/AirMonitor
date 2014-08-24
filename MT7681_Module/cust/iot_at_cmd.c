#include <stdio.h>
#include "string.h"
#include "types.h"
#include "uart.h"
#include "iot_custom.h"
#include "iot_api.h"
#include "eeprom.h"
#include "uart_sw.h"
#include "bmd.h"
#include "ate.h"

/******************************************************************************
* MODULE NAME:     iot_at_cmd.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2013
*
*最新版本程序我们会在 http://www.ai-thinker.com 发布下载链接
*
* LICENSE:
*     This source code is copyright (c) 2011 Ralink Tech. Inc.
*     All rights reserved.
*
* 深圳市安信可科技 MTK7681串口模块专业生产厂家 
*   V1.0.0     Jan 2012    - Initial Version V1.0
*
*
* 串口WIFI 价格大于500 30元   大于5K  28元   大于10K  20元
* ISSUES:
*    First Implementation.
* 淘宝店铺http://anxinke.taobao.com/?spm=2013.1.1000126.d21.FqkI2r
*
******************************************************************************/

/******************************************************************************
 * MACRO DEFINITION
 ******************************************************************************/
#define SYNC_STATE_0 0
#define SYNC_STATE_1 1
#define SYNC_STATE_2 2
#define SYNC_STATE_3 3

/******************************************************************************
 * GLOBAL PARAMTER
 ******************************************************************************/
INT8  state   = 0;

#if (UART_INTERRUPT != 1)
UCHAR command[AT_CMD_MAX_LEN];
INT16 cmd_len = 0;
#endif

#if (ATCMD_ATE_SUPPORT == 1)
BOOLEAN gCaliEnabled = FALSE; 
extern ATE_INFO gATEInfo;
#endif

/******************************************************************************
 * EXTERN PARAMTER
 ******************************************************************************/
extern 	char *optarg; 
extern  INT16 optind; 
extern  int iot_uart_rx_mode;

#if (EP_LOAD_SUPPORT == 1)
extern EEPROM_CFG eepcfg;
#endif

#if (UART_INTERRUPT == 1)
extern PKT_DESC      uartrx2_desc;    //descrypt packet length in uart rx ring2
extern BUFFER_INFO   uartrx2_info;    //uart rx ring2
#endif

/******************************************************************************
 * EXTERN FUNCTION
 ******************************************************************************/
#ifdef CONFIG_SOFTAP
extern VOID Config_SoftAP(UINT8 *SSID, UINT8 *Auth_Mode, UINT8 *Password, UINT8 Channel);
#endif


/******************************************************************************
 * FUNCTION 
 ******************************************************************************/
VOID IoT_AT_cmd_resp_header(
	IN OUT INT8 *pHeader, 
	IN OUT size_t* plen, 
	IN INT8* ATcmdPrefix,
	IN INT8* ATcmdType)
{
	size_t len2 =0;

	/*AT command prefix*/
	*plen = strlen(ATcmdPrefix);
	memcpy(pHeader, ATcmdPrefix, *plen);

	/*AT command Type*/
	len2 = strlen(ATcmdType);
	memcpy(pHeader + *plen, ATcmdType, len2);
	*plen += len2;

	len2 = strlen("=");
	memcpy(pHeader + *plen, "=", len2);
	*plen += len2;

	//*plen += 1;
	//*(pHeader+*plen) = '=';
	
	return;
}

#if (ATCMD_UART_SUPPORT == 1) && (UART_SUPPORT == 1)
/* Format:	AT#Uart -b57600 -w7 -p1 -s1 +enter*/
INT16 IoT_exec_AT_cmd_uart(PCHAR command_buffer, INT16 at_cmd_len)
{
	DBGPRINT(RT_DEBUG_INFO,("IoT_exec_ATcommand_uart: %s \n",command_buffer));
	IoT_uart_AT_config(command_buffer, at_cmd_len);
	return 0;

}
#endif

#if (ATCMD_ATE_SUPPORT == 1)   //20140528 delete old ATE calibration cmd handler
/*AT#ATECAL -S0*/
/*AT#ATECAL -S1 -C1 -m1 -c7 -b0 -g0 -f65 -p30 -l800 -n100000 -r10+enter*/
/*AT#ATECAL -S2 -C1 -t5000+enter*/
INT16 IoT_exec_AT_cmd_ATE_Cal2(PCHAR command_buffer, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "S:m:c:b:g:f:p:l:n:r:t:C:u:?";
	char *endptr = NULL;
	long num = 0;
	INT16 argc = 0;
	char opt = 0;
	//int i=0;

	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);

	//Printf_High("argc1=%d , long=%d, int=%d, INT16=%d  char=%d\n",
	//	argc, sizeof(long), sizeof(int),sizeof(INT16), sizeof(char));

	//for(i=0; i<argc; i++)
	//	Printf_High(" argv[%d] = %s \n",i, argv[i]);


	/*move to split_string_cmd for code slim*/
	//optind = 1;
	//optarg = NULL;

	opt = getopt(argc, argv, opString);

	/*To indicate Calibration mode is enabled for Recovery/Calibration State Machine*/
	gCaliEnabled = TRUE;
	
	while (opt != -1)
	{
		switch (opt)
		{
			case 'S':
				/*ATE start,  ATE TxMode,  ATE RxMode (0:START,   1:TxMode   , 2:RxMode)*/
				num = simple_strtol(optarg,&endptr,10);
				ATESTART((UINT8)num);
				break;
			case 'C':
				/*ATE Set Channel   (0~14)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("1-SwitchCH:%d\n",num);
				Set_ATE_Switch_CH((UINT8)num);
				break;
			case 'm':
				/*ATE Tx Mode        (0:CCK, 1:OFDM, 2:HTMIX,  3:HT_GREEN_FIELD)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("2-TxMode:%d\n",num);
				Set_ATE_TX_MODE_Proc((UINT8)num);
				break;
			case 'c':
				/*ATE MCS rate       (MCS num is ralated with TxMode)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("3-MCS:%d\n",num);
				Set_ATE_TX_MCS_Proc((UINT8)num);
				break;
			case 'b':			
				/*ATE Bandwidth      (0:BW_20,   1:BW_40)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("4-BW:%d\n",num);
				MT7681_Set_ATE_TX_BW_Proc((UINT8)num);
			 	break;
			case 'g':
				/*ATE ShortGI Mode        (0:Full GI    1:Half GI)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("5-GI:%d\n",num);
				Set_ATE_TX_GI_Proc((UINT8)num);  
				//open this function with num=0,  no frame be detect by sniffer with this command
				//AT#ATECAL -S1 -C1 -m2 -c7 -b0 -g0 -l800 -f65 -p30 -n100000
				//if -c5, -c6 is OK,  -c7,-c8 NG
				break;
			case 'f':
				/*ATE Freq Offset              (0~FF)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("7-FreqOffset:%d\n",num);
				Set_ATE_TX_FREQ_OFFSET_Proc((UINT8)num);  //system halt if call this function, maybe stack issue
				break;
			case 'p':
				/*ATE Tx Power                 (0~47)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("8-TxPower:%d\n",num);
				Set_ATE_TX_POWER((UINT32)num);
				break;
			case 'l':   //default length : ATE_TX_PAYLOAD_LEN=800
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("6-PayloadLen:%d\n",num);
				Set_ATE_TX_PAYLOAD_LEN((UINT32)num);
				break;
			case 'n':
				/*ATE Tx Frame Sent Counter   (0~0xFFFFFFF)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("9-SentCounter:%d\n",num);
				Set_ATE_TX_COUNT_Proc((UINT32)num);
				break;
			case 'r':
				/*ATE Tx Frame Sent Speed   (0~0xFFFFFFF), unit:1ms*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("10-SentSpeed:%d ms\n",num);
				Set_ATE_TX_Speed_Proc((UINT32)num);
				break;
			case 'u':
				/*ATE Tx Count in every -r uint  (0~20)*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("11-SetTxCntPerSpeed: %d \n",num);
				Set_ATE_TX_Cnt_Per_Speed((UINT32)num);
				break;
			case 't':   //default time/duration : ATE_RX_CNT_DURATION=1000
				/*ATE Rx Count Duration  (0~0xFFFFFFF), unit:1ms*/
				num = simple_strtol(optarg,&endptr,10);
				//Printf_High("12-SetRxCntDuration:%d ms\n",num);
				Set_ATE_RX_DURATION((UINT32)num);
				break;
			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}

	return 0;
}

UINT8 RTMPIoctlE2PROM(BOOLEAN type, char *pBuf, INT16 Len)
{
	char  TmpBuf[16];
	char *opString   = "=";
	char *endptr     = NULL;
	char *Data	     = NULL;
	INT16 DataLen    = 0;
	UINT32 Value     = 0;
	UINT32 Offset    = 0;
	INT16  OffsetLen = 0;

	UINT8 eData      = 0;
	UINT8 fData[1]   = {0};

	memset(TmpBuf, 0, sizeof(TmpBuf));

	if ((pBuf == NULL) || (Len == 0))
		return 1;

	if (strchr(pBuf, ' ') != NULL)
		return 2;
	
	Data = strpbrk(pBuf, opString);

	/*for efuse/flash read*/
	if (Data == NULL)
	{
		Offset = simple_strtol(pBuf, &endptr, 16);
		//Printf_High("Offset=0x%x \n",Offset);

		if (type == 0)
		{
			if(Set_ATE_Efuse_Read((USHORT)Offset, &eData) == TRUE )
				Printf_High("[0x%x]=0x%02x\n",Offset, eData);
		}
		else
		{
			spi_flash_read((UINT32)Offset, fData, 1);
			Printf_High("[0x%x]=[0x%02x]\n",Offset,fData[0]);
		}
	}
	else /*for efuse/flash write*/
	{
		OffsetLen = Data - pBuf;
		memcpy(TmpBuf, pBuf, OffsetLen);
		Offset = simple_strtol(TmpBuf, &endptr, 16);
		
		
		Data++;   /*not include character "="  or "space"*/
		DataLen = Len-OffsetLen-1;
		memset(TmpBuf, 0, sizeof(TmpBuf));
		memcpy(TmpBuf, Data, DataLen);
		Value  = simple_strtol(TmpBuf, &endptr, 16);
		
		//Printf_High("Offset=0x%x , Value=0x%x \n",Offset, Value);

		if (type == 0)
		{
			if(Set_ATE_Efuse_Write((USHORT)Offset, (UINT8)Value) == FALSE)
				Printf_High("Offset must a even num\n");	
		}
		else
		{
			spi_flash_write((UINT32)Offset, (UINT8*)&Value, 1);
			Printf_High("[0x%x]=[0x%02x]\n",Offset,Value);
		}
	}
}


UINT8 ATE_CMD_CALI_SET_HANDLE(char *pBuf, INT16 Len)
{
	char  Cmd[16];
	INT16 CmdLen   = 0;
	char *Data	   = NULL;
	INT16 DataLen  = 0;
	char *opString = "=";
	long  num      = 0;
	char *endptr   = NULL;

	memset(Cmd, 0, sizeof(Cmd));

	if ((pBuf == NULL) || (Len == 0))
		return 1;

	if (strchr(pBuf, ' ') != NULL)
	{
		Data = strpbrk(pBuf, " ");        /*for case:  set e2p d0=1E*/
	}
	else
	{
		Data = strpbrk(pBuf, opString);   /*for case:  set ATECHANNEL=1*/
	}
	
	if (Data == NULL)
		return 2;
	
	CmdLen = Data - pBuf;
	memcpy(Cmd, pBuf, CmdLen);
	
	Data++;   /*not include character "="  or "space"*/
	DataLen = Len-CmdLen-1;

	//Printf_High("LINE:%d, Cmd[%s], CmdLen[%d]\n",__LINE__, Cmd, CmdLen);
	//Printf_High("LINE:%d, Data[%s], Len[%d]\n",__LINE__, Data, DataLen);

	if (!memcmp(Cmd,"ATE",CmdLen))
	{
		if(!memcmp(Data,"ATESTART",strlen("ATESTART")))
		{
			ATE_INIT();
		}
		else if(!memcmp(Data,"TXFRAME",strlen("TXFRAME")))
		{
			ATE_TXFRAME();
		}
		else if(!memcmp(Data,"RXFRAME",strlen("RXFRAME")))
		{
			ATE_RXFRAME();
		}
		else if(!memcmp(Data,"ATESTOP",strlen("ATESTOP")))
		{
			ATE_STOP();
		}
	}
	else if (!memcmp(Cmd,"ATECHANNEL",CmdLen))
	{
		/*ATE Set Channel		(0~14)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("1-SwitchCH:%d\n",num);
		Set_ATE_Switch_CH((UINT8)num);
	}
	else if (!memcmp(Cmd,"ATETXMODE",CmdLen))
	{
		/*ATE Tx Mode		 (0:CCK, 1:OFDM, 2:HTMIX,  3:HT_GREEN_FIELD)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("2-TxMode:%d\n",num);
		Set_ATE_TX_MODE_Proc((UINT8)num);
	}
	else if (!memcmp(Cmd,"ATETXMCS",CmdLen))
	{
		/*ATE MCS rate		 (MCS num is ralated with TxMode)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("3-MCS:%d\n",num);
		Set_ATE_TX_MCS_Proc((UINT8)num);
	}
	else if (!memcmp(Cmd,"ATETXBW",CmdLen))
	{
		/*ATE Bandwidth 	 	(0:BW_20,	 1:BW_40)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("4-BW:%d\n",num);
		MT7681_Set_ATE_TX_BW_Proc((UINT8)num);
	}
	else if (!memcmp(Cmd,"ATETXGI",CmdLen))
	{
		/*ATE ShortGI Mode		  (0:Full GI	1:Half GI)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("5-GI:%d\n",num);
		Set_ATE_TX_GI_Proc((UINT8)num);  
	}
	else if (!memcmp(Cmd,"ATETXLEN",CmdLen))
	{
		//default length : ATE_TX_PAYLOAD_LEN=800
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("6-PayloadLen:%d\n",num);
		Set_ATE_TX_PAYLOAD_LEN((UINT32)num);
	}
	else if (!memcmp(Cmd,"ATETXFREQOFFSET",CmdLen))
	{
		/*ATE Freq Offset			   (0~FF)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("7-FreqOffset:%d\n",num);
		Set_ATE_TX_FREQ_OFFSET_Proc((UINT8)num);  //system halt if call this function, maybe stack issue
	}
	else if (!memcmp(Cmd,"ATETXPOW0",CmdLen))
	{
		/*ATE Tx Power			   (0~47)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("8-TxPower:%d\n",num);
		Set_ATE_TX_POWER((UINT32)num);
	}
	else if (!memcmp(Cmd,"ATETXCNT",CmdLen))
	{
		/*ATE Tx Frame Sent Counter   (0~0xFFFFFFF)*/
		num = simple_strtol(Data, &endptr,10);
		//Printf_High("9-SentCounter:%d\n",num);
		Set_ATE_TX_COUNT_Proc((UINT32)num);
	}
	else if (!memcmp(Cmd,"ATEDA",CmdLen))
	{
		/*not support*/
	}
	else if (!memcmp(Cmd,"ResetCounter",CmdLen))
	{
		Set_ResetStatCounter_Proc();
	}
	else if (!memcmp(Cmd,"e2p",CmdLen))
	{
		RTMPIoctlE2PROM(0, Data, DataLen);
	}
	else if (!memcmp(Cmd,"flash",CmdLen))
	{
		RTMPIoctlE2PROM(1, Data, DataLen);
	}
	else
	{
		/*not support*/
	}

	return 0;
}



/*========================================================================
	Routine	Description:
		IoT_exec_AT_cmd_ATE_Cal --  Do ATE calibration for iwpriv cmd format
		
	Arguments:
	Return Value: 0 is success
	Note:  
========================================================================*/
/*Case1:Tx Calibration*/
/*iwpriv ra0 set ATE=ATESTART+enter*/
/*iwpriv ra0 set ATE=ATECHANNEL=1+enter*/
/*iwpriv ra0 set ATE=ATETXMODE=2+enter*/
/*iwpriv ra0 set ATE=ATETXMCS=7+enter*/
/*iwpriv ra0 set ATE=ATETXBW=1+enter*/
/*iwpriv ra0 set ATE=ATETXGI=0+enter*/
/*iwpriv ra0 set ATE=ATETXLEN=800+enter*/
/*iwpriv ra0 set ATE=ATETXFREQOFFSET=33+enter*/
/*iwpriv ra0 set ATE=ATETXCNT=10000+enter*/
/*iwpriv ra0 set ATE=ATETXPOW0=16+enter*/
/*iwpriv ra0 set ATE=ATETXFRAME+enter*/

/*Case2:Rx Calibration*/
/*iwpriv ra0 set ATE=ATESTART+enter*/
/*iwpriv ra0 set ATE=ATECHANNEL=1+enter*/
/*iwpriv ra0 set ATE=ATETXMODE=0+enter*/
/*iwpriv ra0 set ATE=ATETXMCS=0+enter*/
/*iwpriv ra0 set ATE=ATETXBW=0+enter*/
/*iwpriv ra0 set ATE=ATETXFREQOFFSET=33+enter*/
/*iwpriv ra0 set ATE=ATETXCNT=10000+enter*/
/*iwpriv ra0 set ATE=ATETXPOW0=16+enter*/
/*iwpriv ra0 set ATE=ATETXFRAME+enter*/

/*Case3:efuse/Flash r/w*/
/*iwpriv ra0 set e2p 0x52+enter*/
/*iwpriv ra0 set e2p 0x52=0x01+enter*/
/*iwpriv ra0 set flash 0x17052+enter*/
/*iwpriv ra0 set flash 0x17052=0x01+enter*/
INT16 IoT_exec_AT_cmd_ATE_Cal(PCHAR cmd_buf, INT16 Len)
{
	char  Cmd[16];
	INT16 CmdLen   = 0;
	char *Data	   = NULL;
	INT16 DataLen  = 0;
	char opString[] = {' ',','};

	/*To indicate Calibration mode is enabled for Recovery/Calibration State Machine*/
	//not set to FALSE, once input ATE Calibration cmd, should keep Calibration Mode
	gCaliEnabled = TRUE;
		
	memset(Cmd, 0, sizeof(Cmd));

	if ((cmd_buf == NULL) || (Len == 0))
		return 1;
	
	Data = strpbrk((char *)cmd_buf, opString);

	if (Data == NULL)
	{
		/* for the case:  only has a cmd name, no cmd val
		    e.g:  "iwpriv ra0 stat"  or "AT#ATECAL stat"*/
		CmdLen = Len;
		DataLen = 0;
	}
	else
	{
		/* for the case:  has a cmd name and cmd val with a sperator '='
		    e.g:  "iwpriv ra0 set ATE=ATESTART"  or "AT#ATECAL set ATE=ATESTART"*/
		CmdLen = Data - (char *)cmd_buf;
		Data++;   /*not include character "=" */
		DataLen = Len-CmdLen-1;
	}
	memcpy(Cmd, (char *)cmd_buf, CmdLen);

	//Printf_High("LINE:%d, Cmd[%s], CmdLen[%d]\n",__LINE__, Cmd, CmdLen);
	//Printf_High("LINE:%d, Data[%s], Len[%d]\n",__LINE__, Data, DataLen);	

	if (!memcmp(Cmd,"set",CmdLen))
	{
		ATE_CMD_CALI_SET_HANDLE(Data, DataLen); /*not include 1st Space*/
	}
	else if (!memcmp(Cmd,"stat",CmdLen))
	{
		ATE_CMD_CALI_STAT();
	}
	else
	{
		/*Do nothing*/
	}
	
	return 0;
}
#endif



#if (ATCMD_FLASH_SUPPORT == 1)
/* Format:	AT#FLASH -r6 +enter*/
/* Format:	AT#FLASH -s6 -v56+enter*/
INT16 IoT_exec_AT_cmd_Flash_Set(PCHAR command_buffer, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "r:s:v:?";
	char *endptr = NULL;
	long num = 0;
	INT16 argc = 0;
	char opt = 0;
	UINT32 fOffset = 0;
	UINT8 fData[1] = {0};

	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);

	opt = getopt(argc, argv, opString);

	while (opt != -1)
	{	
		switch (opt)
		{
			case 'r':
				/*Flash Read Offset*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("0-Read Offset:%d\n",num);
				#if 0   //will ocupied 300Byte, thus disable this by default
				spi_flash_read((UINT32)num, fData, RAM_RW_BUF_SIZE);
				dump_spi_flash((UINT32)num, (UINT32)num+RAM_RW_BUF_SIZE);
				#else
				spi_flash_read((UINT32)num, fData, 1);
				Printf_High("[0x%x]=[0x%02x]\n",num,fData[0]);
				#endif
				
				break;
			case 's':
				/*Flash Write Offset*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("1-Write Offset:%d\n",num);
				fOffset = (UINT32)num;
				break;
			case 'v':
				/*Flash Write Value*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("2-Write Value:%d\n",num);
				spi_flash_write(fOffset, (UINT8*)&num, 1);
				Printf_High("[0x%x]=[0x%02x]\n",fOffset,num);
				break;
			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}
	
	return 0;
}
#endif



#if (ATCMD_EFUSE_SUPPORT == 1)
/* Format:	AT#EFUSE -r6 +enter*/
/* Format:	AT#EFUSE -s6 -v56+enter*/
INT16 IoT_exec_AT_cmd_Efuse_Set(PCHAR command_buffer, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "r:s:v:?";
	char *endptr = NULL;
	long num = 0;
	INT16 argc = 0;
	char opt = 0;
	USHORT eOffset = 0;
	UINT8 eData = 0;
	//int i=0;

	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);

	//Printf_High("argc1=%d , long=%d, int=%d, INT16=%d  char=%d\n",
	//	argc, sizeof(long), sizeof(int),sizeof(INT16), sizeof(char));

	//for(i=0; i<argc; i++)
	//	Printf_High(" argv[%d] = %s \n",i, argv[i]);


	/*move to split_string_cmd for code slim*/
	//optind = 1;
	//optarg = NULL;

	opt = getopt(argc, argv, opString);

	while (opt != -1)
	{	
		switch (opt)
		{
			case 'r':
				/*Efuse Read Offset*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("0-Read Offset:%d\n",num);
				if( Set_ATE_Efuse_Read((USHORT)num, &eData) == TRUE )
					Printf_High("[0x%x]=0x%02x\n",num, eData);
				break;
			case 's':
				/*Efuse Write Offset*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("1-Write Offset:%d\n",num);
				eOffset = (USHORT)num;
				break;
			case 'v':
				/*Efuse Write Value*/
				num = simple_strtol(optarg,&endptr,0);
				//Printf_High("2-Write Value:%d\n",num);
				if( Set_ATE_Efuse_Write(eOffset, (UINT8)num) == FALSE)
					Printf_High("Offset must a even num\n");			
				break;
			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}

	return 0;
}
#endif

#if (ATCMD_NET_MODE_SUPPORT == 1)
INT16 IoT_exec_AT_cmd_netmode(PCHAR command_buffer, INT16 at_cmd_len)
{
	DBGPRINT(RT_DEBUG_INFO,("IoT_exec_ATcommand_netmode: %s \n",command_buffer));
	return -1;
}
#endif

#if (ATCMD_CH_SWITCH_SUPPORT == 1)
/*========================================================================
	Routine	Description:
		IoT_exec_AT_cmd_ch_switch --  switch channel
		
	Arguments:
	Return Value: 0 is success
	Note:  In present , the channel range is channel 0~13, and Bandwidth is only support 20MHz
========================================================================*/
INT16 IoT_exec_AT_cmd_ch_switch(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	INT8 len, num = 0;

	len = 3;	//(INT8)strlen("-s ");
	if(!memcmp(pCmdBuf ,"-s ", len))
	{	
		pCmdBuf += len;
		at_cmd_len -= len;

		if(at_cmd_len < 0)
			return -1;

		num = (INT8)atoi((const int8 *)pCmdBuf);
		if (num > 14 || num < 0 )
			return -1;

		IoT_Cmd_Set_Channel((UINT8)num);
	}

	return 0;
}

#endif

#ifdef CONFIG_SOFTAP
/*========================================================================
	Routine	Description:
		IoT_exec_AT_cmd_ch_switch --  switch channel
		
	Arguments:
	Return Value: 0 is success
	Note:  In present , the channel range is channel 0~13, and Bandwidth is only support 20MHz
========================================================================*/
INT16 IoT_exec_AT_cmd_Conf_SoftAP(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	int argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:c:a:p:?";
	int opt;
	char *endptr;
	
	UINT8 SSID[MAX_SSID_PASS_LEN] = {0};
	UINT8 Auth_Mode[MAX_AUTH_MODE_LEN] = {0};
	UINT8 Password[MAX_SSID_PASS_LEN] = {0};
	UINT8 Channel = 0;
	UINT SSIDLen = 0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);
	
	//printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
	optind = 1; 
	optarg = NULL; 
	
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 's':
			SSIDLen = strlen(optarg);
			if (SSIDLen > MAX_SSID_PASS_LEN)
			{
				SSIDLen = MAX_SSID_PASS_LEN;
			}
			memcpy(SSID, optarg, SSIDLen);
			Printf_High("AT#SSID:[%s], Len = %d\n",SSID, strlen(optarg));
			break;
		case 'a':
			memcpy(Auth_Mode, optarg, strlen(optarg));
			Printf_High("AT#Auth_Mode:%s\n",Auth_Mode);
			break;
		case 'p':			
			memcpy(Password, optarg, strlen(optarg));
			Printf_High("AT#Password:%s\n",Password);
			break;
		case 'c':			
			Channel = atoi(optarg);
			Printf_High("AT#Channel:%d\n",Channel);
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}

	Config_SoftAP(SSID, Auth_Mode, Password, Channel);
	return 0;
}
#endif

#if (ATCMD_REBOOT_SUPPORT == 1)
/*========================================================================
	Routine	Description:
		IoT_exec_AT_cmd_reboot --  system reboot
		
	Arguments:
	Return Value:  NONE
	Note:
========================================================================*/
VOID IoT_exec_AT_cmd_reboot(VOID)
{
	Sys_reboot();
	return;
}
#endif

#if (ATCMD_GET_VER_SUPPORT == 1)
/*========================================================================
	Routine	Description:
		IoT_exec_AT_cmd_ver -- get FW version
		
	Arguments:
	Return Value: 0 is success,  -1 is out of memory
	Note:
========================================================================*/
INT16 IoT_exec_AT_cmd_ver(PCHAR pCmdBuf)
{
	size_t  len=0 , len2 =0;
	
	/* the response header format is:  "AT#CmdType=" */
	IoT_AT_cmd_resp_header((INT8 *)pCmdBuf, &len, AT_CMD_PREFIX, AT_CMD_VER);
	
	/*AT command Version*/
	len2 = strlen(FW_VERISON_CUST);
	memcpy(pCmdBuf + len, FW_VERISON_CUST, len2);
	len += len2;

	len2 = strlen("\n");
	if( (len + len2 + 3) >= AT_CMD_MAX_LEN)
	{
		/*pCmdBuf is mallocated on	ATcommandHandler(),  
		   but here it's start after "AT#"
		   thus it's length is	 (AT_CMD_MAX_LEN-3); */
		return -1;
	}
	memcpy(pCmdBuf + len, "\n", len2);
	len += len2;

	//len += 1;
	//*(pCmdBuf+len) = '\n';
	
	IoT_uart_output(pCmdBuf, len);

	return 0;
}
#endif

#if (ATCMD_JTAGMODE_SUPPORT == 1)
VOID IoT_exec_AT_cmd_jtag_mode_switch(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "m:?";
	char opt=0;
	char *endptr;
	
	UINT8 switch_on=0;
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	
	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);

	//Printf_High("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);

	/*move to split_string_cmd for code slim*/
	//optind = 1; 
	//optarg = NULL; 
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 'm':
			switch_on =  (UINT8) simple_strtoul(optarg,&endptr,10);
			//Printf_High("1:%s,%d\n",optarg, socket);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
	}
	
	if(content == 1)
		IoT_jtag_mode_switch(switch_on);
	else
		IoT_jtag_mode_get();
	
	return;

}

#endif


//extern UARTStruct UARTPort;    


INT16 IoT_parse_ATcommand(PCHAR cmd_buf, INT16 at_cmd_len)
{
	INT16 ret_code = 0;   
#if (ATCMD_CH_SWITCH_SUPPORT == 1)
    INT8 cTypeLen = 0;
#endif

    //BUFFER_INFO  *rx_ring = &(UARTPort.Rx_Buffer);
    //PKT_DESC     *rx_desc = &(UARTPort.Rx_desc);

	cmd_buf[at_cmd_len] = '\0';
	//DBGPRINT(RT_DEBUG_INFO,("AT command: %s , len=%d \n",cmd_buf, at_cmd_len));
	//Printf_High("AT command len=%d \n", at_cmd_len);
    //Printf_High("ring buf:pkt=%d,R=%d,W=%d\n", rx_desc->pkt_num,BRead(rx_ring),BWrite(rx_ring));
	
	/* The current process is not encough*/
	/* need improve for the command type and paramters parsing */

	/* Format:	AT#Default+enter*/
	if(!memcmp(cmd_buf,AT_CMD_DEFAULT,sizeof(AT_CMD_DEFAULT)-1))
	{
		reset_cfg();
		IoT_Cmd_LinkDown(0);   //disable Link Down to fix STA machine keeps SMNT and no reponse
	}	
#if (ATCMD_GET_VER_SUPPORT == 1)
	/* Format:  AT#Ver+enter*/
	else if(!memcmp(cmd_buf,AT_CMD_VER,sizeof(AT_CMD_VER)-1))
	{
		ret_code = IoT_exec_AT_cmd_ver(cmd_buf);
	}
#endif
#if (ATCMD_NET_MODE_SUPPORT == 1)
	else if(!memcmp(cmd_buf,AT_CMD_NETMODE,sizeof(AT_CMD_NETMODE)-1))
	{
		ret_code = IoT_exec_AT_cmd_netmode(cmd_buf, at_cmd_len);
	}
#endif
#if (ATCMD_REBOOT_SUPPORT == 1)
	/* Format:  AT#Reboot+enter*/
	else if(!memcmp(cmd_buf,AT_CMD_REBOOT,sizeof(AT_CMD_REBOOT)-1))
	{
		IoT_exec_AT_cmd_reboot();
	}
#endif	
#if (ATCMD_CH_SWITCH_SUPPORT == 1)
	/* Format:  AT#Channel -s 6+enter */
	else if(!memcmp(cmd_buf,AT_CMD_CHANNEL,sizeof(AT_CMD_CHANNEL)-1))
	{
		cTypeLen = strlen(AT_CMD_CHANNEL);
		cmd_buf += cTypeLen;
		
		at_cmd_len -= cTypeLen;
		if(at_cmd_len > 0)
		{
			ret_code = IoT_exec_AT_cmd_ch_switch(cmd_buf, at_cmd_len);
		}
	}
#endif
#if	(ATCMD_SOFTAP_SUPPORT == 1) && (ATCMD_SUPPORT == 1)	
/* Format:	AT#SoftAPConf -s[ssid] -c[channel] -a[auth_mode] -p[password]+enter*/	
else if(!memcmp(cmd_buf,AT_CMD_SOFTAP_CFG, sizeof(AT_CMD_SOFTAP_CFG)-1))	
	{		
		IoT_exec_AT_cmd_Conf_SoftAP(cmd_buf, at_cmd_len);			
		Printf_High("Config SoftAP\n");	
	}
#endif

#if (ATCMD_FLASH_SUPPORT == 1)
	/* Format:	AT#FLASH -r6 +enter*/
	/* Format:	AT#FLASH -s6 -v56+enter*/
	else if(!memcmp(cmd_buf,AT_CMD_FLASH_SET,sizeof(AT_CMD_FLASH_SET)-1))
	{
		//Printf_High("Flash Write/Read \n");
		ret_code = IoT_exec_AT_cmd_Flash_Set(cmd_buf, at_cmd_len);
	}
#endif
#if (ATCMD_EFUSE_SUPPORT == 1)
	/* Format:	AT#EFUSE -r6 +enter*/
	/* Format:	AT#EFUSE -s6 -v56 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_EFUSE_SET,sizeof(AT_CMD_EFUSE_SET)-1))
	{
		//Printf_High("Efuse Write/Read \n");
		ret_code = IoT_exec_AT_cmd_Efuse_Set(cmd_buf, at_cmd_len);
	}
#endif
#if (ATCMD_ATE_SUPPORT == 1)  //20140528 delete old ATE calibration cmd handler,  be instead of iwpriv cmd format handler
	/* Format:	AT#ATECAL -C1 -m1 -c7 -g0 -f65 -p30 -n100000+enter*/
	else if(!memcmp(cmd_buf,AT_CMD_ATE_CAL,sizeof(AT_CMD_ATE_CAL)-1))
	{
		//Printf_High("ATECAL \n");
		gATEInfo.ATECmdFmt = ATE_CMD_TYPE_AT;
		ret_code = IoT_exec_AT_cmd_ATE_Cal2(cmd_buf, at_cmd_len);
	}
#endif
#if (ATCMD_UART_SUPPORT == 1) && (UART_SUPPORT == 1)	
	/* Format:	AT#Uart -b57600 -w7 -p1 -s1 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_UART,sizeof(AT_CMD_UART)-1))
	{
		ret_code = IoT_exec_AT_cmd_uart(cmd_buf, at_cmd_len);
	}
#endif

/*Only for Debug*/
#if (ATCMD_ATE_MBR_CTL == 1)
	else if(!memcmp(cmd_buf,AT_CMD_MAC_SET,sizeof(AT_CMD_MAC_SET)-1))
	{
		ret_code = IoT_exec_AT_cmd_mac_rw(cmd_buf, at_cmd_len);
	}
	else if(!memcmp(cmd_buf,AT_CMD_BBP_SET,sizeof(AT_CMD_BBP_SET)-1))
	{
		ret_code = IoT_exec_AT_cmd_bbp_rw(cmd_buf, at_cmd_len);
	}
	else if(!memcmp(cmd_buf,AT_CMD_RF_SET,sizeof(AT_CMD_RF_SET)-1))
	{
		ret_code = IoT_exec_AT_cmd_rf_rw(cmd_buf, at_cmd_len);
	}
#endif

#if (ATCMD_SLT_SUPPORT == 1)
	else if(!memcmp(cmd_buf,AT_CMD_SLT_CTL,sizeof(AT_CMD_SLT_CTL)-1))
	{
		ret_code = SLT_TEST(cmd_buf, at_cmd_len);
	}
#endif

/*jinchuan  These functions developping is pending, no plan to use */
#if  0 //(ATCMD_TCPIP_SUPPORT == 1) && (CFG_SUPPORT_TCPIP == 1)

	/* Format:	AT#Tcp_Connect -a192.168.1.131 -p1234 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_TCPCONNECT,sizeof(AT_CMD_TCPCONNECT)-1))
	{

		IoT_Tcpip_AT_Connect(cmd_buf, at_cmd_len);
	}
	/* Format:	AT#Tcp_Send -s1 -dtest data +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_TCPSEND,sizeof(AT_CMD_TCPSEND)-1))
	{
		IoT_Tcpip_AT_Send(cmd_buf, at_cmd_len);
	}
	/* Format:	AT#Tcp_Listen -p7682 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_TCPLISTEN,sizeof(AT_CMD_TCPLISTEN)-1))
	{
		IoT_Tcpip_AT_Listen(cmd_buf, at_cmd_len);
	}
	/* Format:	AT#Tcp_Disconnect -s0 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_TCPDISCONNECT,sizeof(AT_CMD_TCPDISCONNECT)-1))
	{
		IoT_Tcpip_AT_Disconnect(cmd_buf, at_cmd_len);
	}
	
	/* Format:	AT#Udp_Create -a192.168.1.132 -r1234 -l4321 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_UDPCREATE,sizeof(AT_CMD_UDPCREATE)-1))
	{
		IoT_Tcpip_AT_Udp_Create(cmd_buf, at_cmd_len);
	}	
	/* Format:	AT#Udp_Remove -s130 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_UDPREMOVE,sizeof(AT_CMD_UDPREMOVE)-1))
	{
		IoT_Tcpip_AT_Udp_Del(cmd_buf, at_cmd_len);
	}

	/* Format:	AT#Udp_Send -s130 -d1234 -a192.168.1.132 -r1234 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_UDPSEND,sizeof(AT_CMD_UDPSEND)-1))
	{
		IoT_Tcpip_AT_Udp_Send(cmd_buf, at_cmd_len);
	}
	/* Format:	AT#StaticIP -i192.168.1.100 -m255.255.255.0 -g192.168.1.1 -d172.26.2.185 +enter*/
	else if(!memcmp(cmd_buf,AT_CMD_STATIC_IP,sizeof(AT_CMD_STATIC_IP)-1))
	{

		IoT_exec_AT_cmd_Set_Static_IP(cmd_buf, at_cmd_len);
	}
	else if(!memcmp(cmd_buf,AT_CMD_NETSTAT,sizeof(AT_CMD_NETSTAT)-1))
	{

		IoT_exec_AT_cmd_Netstat(cmd_buf, at_cmd_len);
	}
#endif
#if (ATCMD_UART_FW_SUPPORT == 1) && (UART_SUPPORT == 1)
	/* Format:	AT#UpdateFW+enter   (the range of type is [1~7] ) */
	else if(!memcmp(cmd_buf,AT_CMD_UPDATEFW,sizeof(AT_CMD_UPDATEFW)-1))
	{
		IoT_Xmodem_Update_FW_Start(); /*Disable Uart Rx Interrupt*/
		IoT_Xmodem_Update_FW();
		IoT_Xmodem_Update_FW_Stop();  /*Restore Uart Rx Interrupt*/
	}

#endif
#if (ATCMD_JTAGMODE_SUPPORT == 1)

	else if(!memcmp(cmd_buf,AT_CMD_JTAGMODE,sizeof(AT_CMD_JTAGMODE)-1))
	{
		IoT_exec_AT_cmd_jtag_mode_switch(cmd_buf, at_cmd_len);
	}
	
#endif

	return ret_code;
}

INT16 IoT_parse_IWCmd(PCHAR cmd_buf, INT16 at_cmd_len)
{
	INT16 ret_code = 0;   
	
#if (ATCMD_ATE_SUPPORT == 1)
	cmd_buf[at_cmd_len] = '\0';
	gATEInfo.ATECmdFmt = ATE_CMD_TYPE_IWPRIV;
	ret_code = IoT_exec_AT_cmd_ATE_Cal(cmd_buf, at_cmd_len);
#endif

	return ret_code;
}



#if (UART_INTERRUPT != 1)
INT16 IoT_detect_ATcommand(UINT8* pType)
{

	UCHAR ch = 0;
	INT32 read = -1;
	INT16 ret_len = 0;

	static UINT8  CmdType = PKT_UNKNOWN;
	static UINT8  ATMatchNum = 0;
	static UINT8  IWMatchNum = 0;
	char ATCmdPrefixAT[]=AT_CMD_PREFIX;
	char ATCmdPrefixIW[]=AT_CMD_PREFIX2;

	if (UART_LSROverErr() != 0)
			return -1;

	/*Notice: Must not use Printf_High in the while block,  the Rx FIFO,RxINT and ringbuf will mess*/
	while(1)
	{
		read = UART_GetChar((UCHAR*)&ch);

		if(read == -1)
			return -1;
		
#if (UARTRX_TO_AIR_LEVEL == 1)

		uart_rb_push(ch);

#elif (UARTRX_TO_AIR_LEVEL == 2)

		if(iot_uart_rx_mode == UARTRX_PUREDATA_MODE)
		{
			if(cmd_len >= AT_CMD_MAX_LEN)
            {
                cmd_len = 0;
            }  
            command[cmd_len] = ch;
            cmd_len++;
			continue;
		}

#endif

#if 1  //support more ATcmd prefix analysis
		if(CmdType == PKT_UNKNOWN)
		{
			/*case 1:AT#*/
			if (ATCmdPrefixAT[ATMatchNum] == ch)
				ATMatchNum++;
			else
				ATMatchNum = 0;
			
			/*case 2:iwpriv ra0*/
			if (ATCmdPrefixIW[IWMatchNum] == ch)
				IWMatchNum++;
			else
				IWMatchNum = 0;
			
			if (( ATMatchNum == sizeof(ATCmdPrefixAT)-1 ) ||   //match case 1: AT#
				( IWMatchNum == sizeof(ATCmdPrefixIW)-1 ) )    //match case 2:iwpriv ra0
			{
				if( ATMatchNum == sizeof(ATCmdPrefixAT)-1 )
					CmdType = PKT_ATCMD;			 //match case 1: AT#
				else if ( IWMatchNum == sizeof(ATCmdPrefixIW)-1 )
					CmdType = PKT_IWCMD;			 //match case 2:iwpriv ra0
			
				ATMatchNum = 0;
				IWMatchNum = 0;
				continue;
			}
		}
        else if ((PKT_ATCMD == CmdType) || (PKT_IWCMD == CmdType))
        {
			if(ch == '\n' || ch == '\r')
			{
				*pType = CmdType;
				CmdType = PKT_UNKNOWN;
				ret_len = cmd_len;   /*not include '\n'*/
				cmd_len = 0;
				return ret_len;
			}
			else
			{
				command[cmd_len] = ch;
				cmd_len++;
				if(cmd_len >= AT_CMD_MAX_LEN)
				{
					CmdType = PKT_UNKNOWN;
					cmd_len = 0;
					
					return -2;
				}
			}
		}
#else
		if(state != SYNC_STATE_3)
		{
			if(ch == 'A'|| ch == 'a')
			{
				state = SYNC_STATE_1;
			}
			else if(ch == 'T' || ch == 't')
			{
				if(state == SYNC_STATE_1)
					state = SYNC_STATE_2;
				else
					state = SYNC_STATE_0;
			}
			else if(ch == '#')
			{
				if(state == SYNC_STATE_2)
				{
					state = SYNC_STATE_3;
					cmd_len = 0;
					continue;
				}
				else
					state = SYNC_STATE_0;
			}
			else
			{
				state = SYNC_STATE_0;
			}
		}
			
		else if(state == SYNC_STATE_3)
		{
			if(ch == '\n' || ch == '\r')
			{
				state = SYNC_STATE_0;
				ret_len = cmd_len;   /*not include '\n'*/
				cmd_len = 0;
				return ret_len;
			}
			else
			{
				command[cmd_len] = ch;
				cmd_len++;
				if(cmd_len >= AT_CMD_MAX_LEN)
				{
					state = SYNC_STATE_0;
					cmd_len = 0;

					return -2;
				}
			}
		}
#endif
	}
	return 0;
		
}

VOID ATcommandHandler(VOID)
{  
    INT16 at_cmd_len = 0;
	INT16 ret_code = 0;
	UINT8 Type = PKT_UNKNOWN;
	
	at_cmd_len = IoT_detect_ATcommand(&Type);
	
	if(at_cmd_len > 0)
	{	
		Printf_High("at_cmd_len=%d, Type=%d \n",at_cmd_len,Type);

		if (Type == PKT_ATCMD)         //match case 1: AT#
			ret_code = IoT_parse_ATcommand(command, at_cmd_len);   
		else if (Type == PKT_IWCMD)    //match case 2:iwpriv ra0
			ret_code = IoT_parse_IWCmd(command, at_cmd_len);      
		else
			return;
		
		if(ret_code != 0)
			DBGPRINT(RT_DEBUG_ERROR,("exec error: %d\n",ret_code));
	}
	else if(at_cmd_len == -2)
	{
		DBGPRINT(RT_DEBUG_ERROR,("=>%s AT command is too long\n",__FUNCTION__));
	}
}
#endif
