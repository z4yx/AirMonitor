/******************************************************************************
* MODULE NAME:     iot_xmodem.c
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

#if 1//(ATCMD_UART_FW_SUPPORT == 1)
#include <stdio.h>
#include "string.h"
#include "flash_map.h"
#include "iot_api.h"
#include "eeprom.h"

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CTRLZ 0x1A
#define DLY_1S 1000
#define MAXRETRANS 25
static int last_error = 0;

#define PACKET_RETRY_LIMIT 25
#define TRYCHAR_RETRY_LIMIT 30
#define NAK_RETRY_LIMIT 3
#define LOOPCOUNT_1S 700

extern INT16 optind; 
extern char *optarg; 

unsigned short crc16_ccitt(const unsigned char *buf, int len)
{
	return crc_cal_by_bit(buf,len);

}


void port_outbyte(unsigned char trychar)
{
	UART_PutChar(trychar);
}

unsigned char port_inbyte(unsigned int time_out)
{	
	unsigned char ch = 0;
	int read = -1;
	int count = LOOPCOUNT_1S * time_out;
	last_error = 0;	
	while(count >0)
	{
   	    read = UART_GetChar((UCHAR*)&ch);
	    if(read != -1)
		{
		    return ch;	
		}
	    count--;
	}
    
	last_error = 1;	
	return ch;
}



static int check(int crc, const unsigned char *buf, int sz)
{	

	if (crc)
	{	
	
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
		if (crc == tcrc)
			return 1;
	}
	else		
	{		
		int i;
		unsigned char cks = 0;
		
		for (i = 0; i < sz; ++i)
		{
			cks += buf[i];
		}
		
		if (cks == buf[sz])
			return 1;		
	}	
	return 0;
}

static void flushinput(void)
{
	
	//while (port_inbyte(((DLY_1S)*3)>>1) >= 0) 
	;
}


int IoT_xmodemReceive(unsigned char *dest, int destsz)
{
	
	unsigned char xbuff[1030];
	unsigned char *p;
	int bufsz, crc = 0;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
    unsigned char c;
	int i,len = 0;
	int retry, retrans = MAXRETRANS;
	
	for(;;)
	{
		
		for( retry = 0; retry < 16; ++retry)
		{
			
			if (trychar)
				port_outbyte(trychar);
			c = port_inbyte((DLY_1S)<<1);
			
			if (last_error == 0)
			{
				switch (c)
				{
				case SOH:
					bufsz = 128;
					goto start_recv;
				case STX:
					bufsz = 1024;
					goto start_recv;
				case EOT:
					flushinput();
					port_outbyte(ACK);
					return len;
				case CAN:
					c = port_inbyte(DLY_1S);
					if (c == CAN)
					{
						flushinput();
						port_outbyte(ACK);
						return -1;
					}
					break;
				default:
					break;
					
				}
							
			}		
			
		}
		
		
		if (trychar == 'C')		
		{
			trychar = NAK;
			continue;		
		}
		
		flushinput();
		port_outbyte(CAN);
		port_outbyte(CAN);
		port_outbyte(CAN);
		return -2;
		
start_recv:
	
		if (trychar == 'C') 
			{crc = 1;
			}
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;i < (bufsz+(crc?1:0)+3); ++i)
		{

			c = port_inbyte(DLY_1S);
			if (last_error != 0)
				goto reject;
			*p++ = c;
			
		}

			
		
		if (xbuff[1] == (unsigned char)(~xbuff[2]) &&\
			(xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&\
			check(crc, &xbuff[3], bufsz))
		{
			
			if (xbuff[1] == packetno)
			{
				int count = destsz - len;
				if (count > bufsz)
					count = bufsz;
				if (count > 0)
				{
					memcpy (&dest[len], &xbuff[3], count);
					len += count;
				}
				
				++packetno;
				retrans = MAXRETRANS+1;
				
			}
			
			if (--retrans <= 0)
			{				
				flushinput();				
				port_outbyte(CAN);
				port_outbyte(CAN);
				port_outbyte(CAN);
				return -3;			
			}			
			port_outbyte(ACK);
			continue;
						
		}
		
reject:	
		flushinput();
		port_outbyte(NAK);
		
    }
}


int IoT_xmodemTransmit(unsigned char *src, int srcsz)
{
	
	unsigned char xbuff[1030];
	int bufsz, crc = -1;
	unsigned char packetno = 1;
    unsigned char c;
	int i,len = 0;
	int retry;
	
	for(;;) {
		
		for( retry = 0; retry < 16; ++retry)
		{
			c = port_inbyte((DLY_1S)<<1);
			if (last_error == 0)
			{
				
				switch (c)
				{
				case 'C':
					crc = 1;
					goto start_trans;
				case NAK:
					crc = 0;
					goto start_trans;
				case CAN:
					c = port_inbyte(DLY_1S);
					if (c == CAN)
					{
						port_outbyte(ACK);
						flushinput();
						return -1;
					}
					break;
				default:
					break;
				}
			}
		}
		
		
		port_outbyte(CAN);
		port_outbyte(CAN);
		port_outbyte(CAN);
		flushinput();
		return -2;
			
		for(;;)
		{
start_trans:
		
		xbuff[0] = SOH; bufsz = 128;
		xbuff[1] = packetno;
		xbuff[2] = ~packetno;
		
		c = srcsz - len;
		if (c > bufsz) c = bufsz;
		
		if (c >= 0)
		{
			
			memset (&xbuff[3], 0, bufsz);
			if (c == 0)
			{
				xbuff[3] = CTRLZ;
			}
			
			else
			{
				
				memcpy (&xbuff[3], &src[len], c);
				if (c < bufsz) xbuff[3+c] = CTRLZ;
			}
			if (crc)
			{
				unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
				xbuff[bufsz+3] = (ccrc>>8) & 0xFF;
				xbuff[bufsz+4] = ccrc & 0xFF;
			}
			else
			{
				unsigned char ccks = 0;
				for (i = 3; i < bufsz+3; ++i)
				{
					ccks += xbuff[i];
				}
				xbuff[bufsz+3] = ccks;
			}
			for (retry = 0; retry < MAXRETRANS; ++retry)
			{
				for (i = 0; i < bufsz+4+(crc?1:0); ++i)
				{
					port_outbyte(xbuff[i]);
				}
				c = port_inbyte(DLY_1S);
				if (last_error == 0 )
				{
					switch (c)
					{
					case ACK:
						++packetno;
						len += bufsz;
						goto start_trans;
						
					case CAN:
						c = port_inbyte(DLY_1S);
						if ( c == CAN)
						{
							port_outbyte(ACK);
							flushinput();
							return -1;
						}					
						break;	
						
					case NAK:						
						default:
						
						break;
						
					}
					
				}
				
			}
			port_outbyte(CAN);
			port_outbyte(CAN);
			port_outbyte(CAN);
			flushinput();
			return -4;
		}
		
		else
		{
			for (retry = 0; retry < 10; ++retry)
				
			{
				port_outbyte(EOT);
				c = port_inbyte((DLY_1S)<<1);
				if (c == ACK) break;
			}
			
			flushinput();
			return (c == ACK)?len:-5;
			
		}
		
		}
		
   }
}


int IoT_Xmodem_Update_FW(VOID)
{
	unsigned char xbuff[150];  /*only support 128Byte xmodem*/
	unsigned char *p;
	int bufsz, crc = 0;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
	unsigned char c;
	int i,len = 0;
	unsigned char retry, retrans = PACKET_RETRY_LIMIT;
	unsigned char retry_limit = 16;
	
	int flash_error_count = 0;
	unsigned char flash_write_result = 0;
	uint32 pktNum = 0;		  /*if packetno >255 , use this parameter*/
	uint8  Xcycle = 0;        /*if packetno >255 , use this parameter*/

	uint8  UartUpgHeader[UART_FW_HEADER_TOTAL_SIZE] = UART_FW_HEADER;
	int    UpgSize  = FLASH_UPG_FW_SIZE;
	uint8* pData	= NULL;
	uint8  type		= 0;

	//Printf_High("IoT_Xmodem_Update_FW\n");
	for(;;)
	{
		if(trychar == 'C')
			retry_limit = TRYCHAR_RETRY_LIMIT;
		else
			retry_limit = NAK_RETRY_LIMIT;
		
		for( retry = 0; retry < retry_limit; ++retry)
		{
			if (trychar)
				port_outbyte(trychar);
			c = port_inbyte((DLY_1S)<<1);
			
			if (last_error == 0)
			{
				switch (c)
				{
					case SOH:
						bufsz = 128;
						goto start_recv;
					case STX:
						bufsz = 1024;
						goto start_recv;
					case EOT:
						flushinput();
					    port_outbyte(ACK);
						if(flash_error_count == 0)
						{							
							spi_flash_update_fw_done(type);
						}
						else
							Printf_High("\nErr:%d\n", flash_error_count);
						
						return len;
					case CAN:
						c = port_inbyte(DLY_1S);
						if (c == CAN)
						{
							flushinput();
							port_outbyte(ACK);
							return -1;
						}
						break;
					default:
						break;
				}
							
			}		
			
		}
		
		
		if (trychar == 'C')		
		{
			trychar = NAK;
			continue;		
		}
		
		flushinput();
		port_outbyte(CAN);
		port_outbyte(CAN);
		port_outbyte(CAN);
		return -2;
		
start_recv:
	
		if (trychar == 'C') 
		{	
			crc = 1;
		}
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;i < (bufsz+(crc?1:0)+3); ++i)
		{
			c = port_inbyte(DLY_1S);
			if (last_error != 0)
				goto reject;
			*p++ = c;
		}

		if (xbuff[1] == (unsigned char)(~xbuff[2]) &&\
			(xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&\
			check(crc, &xbuff[3], bufsz))
		{
			if (xbuff[1] == packetno)
			{
				int count = UpgSize - len;
				pData = &xbuff[3];
				
				if (count > bufsz)
					count = bufsz;
				
				if (count > 0)
			  	{
					/*if packetno > 255 , it will overflow, thus , 
					we must use pktNum as the flash write offset */
					if((packetno == 0) && (pktNum % 256 == 255))
					{
						Xcycle++;
					}
					pktNum = ((uint32)Xcycle*256) + (uint32)packetno;


					/*packetno is start from 1, thus we check the header in the first packet*/
					/*the 1st packet of Upgrade FW is header,  include:  */
					
					/*|  3BYTE	 |	  4BYTE (Low-->High)   |   1 BYTE      |  120 BYTE |*/
					/*|  Header     |		 Length                  |      Type      |  PADDING |*/
					
					if (pktNum == 1)
					{	
						/*compare first 5 bytes */
						if(!memcmp(pData, UartUpgHeader, UART_FW_HEADER_DATA_SIZE))
						{
							//UpgSize  = *((uint32 *)(pData+UART_FW_HEADER_DATA_SIZE));
							UpgSize  =(((uint32)(*(pData + UART_FW_HEADER_DATA_SIZE))) 	         | 
										((uint32)(*(pData + UART_FW_HEADER_DATA_SIZE + 1)) << 8) |
										((uint32)(*(pData + UART_FW_HEADER_DATA_SIZE + 2)) << 16)|
										((uint32)(*(pData + UART_FW_HEADER_DATA_SIZE + 3)) << 24));

							/*The 8st Byte of UartUpgHeader[] is update Type */
							type     = pData[UART_FW_HEADER_DATA_SIZE + UART_FW_HEADER_LEN_SIZE];
						}
					}
					else
					{
						if ((type == 0) || (type >= UART_FlASH_UPG_ID_MAX))
						{
							/*invalid Uart Flash Upgrade ID*/
							port_outbyte(CAN);
							port_outbyte(CAN);
							port_outbyte(CAN);
							return -3;
						}
						
						flash_write_result = spi_flash_update_fw(type, (pktNum-2)*bufsz, pData, (uint16)count);

						if(flash_write_result != 0)
							flash_error_count++;

						len += count;
					}

				}
				
				++packetno;
				retrans = PACKET_RETRY_LIMIT + 1;
				
			}
			
			if (--retrans <= 0)
			{				
				flushinput();				
				port_outbyte(CAN);
				port_outbyte(CAN);
				port_outbyte(CAN);
				return -3;			
			}
			
			port_outbyte(ACK);
			continue;
						
		}
		
reject:	
		flushinput();
		port_outbyte(NAK);
	}
}

#if 0
IoT_Uart_AT_Xmodem_Update_FW(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "t:?";
	char opt=0;
	char *endptr;
	
	UINT8 type=0;
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	
	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);
	
	//printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);

	/*move to split_string_cmd for code slim*/
	//optind = 1; 
	//optarg = NULL; 
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 't':
			type =  (UINT8) simple_strtoul(optarg,&endptr,10);
			//Printf_High("1:%s,%d\n",optarg, type);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
	}
	
	if(content == 1)
		IoT_Xmodem_Update_FW(type);
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
		

}
#endif

#endif
