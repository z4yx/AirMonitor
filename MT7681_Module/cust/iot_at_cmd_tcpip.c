#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iot_api.h"

/******************************************************************************
* MODULE NAME:     iot_at_cmd_tcpip.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            Jan 2013
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2011 Ralink Tech. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     Jan 2012    - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

#if (ATCMD_TCPIP_SUPPORT == 1) && (CFG_SUPPORT_TCPIP == 1)

extern INT16 optind; 
extern char *optarg; 

INT32 IoT_Tcpip_AT_Connect(PCHAR command_buffer, INT16 at_cmd_len)
{
	
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "a:p:?";
	char opt;
	char *endptr;
	
	UINT8  ip_addr[MAX_IP_STRING_LEN];
	UINT16 port=0;
	UINT16 ip_str_len;
	UINT8 content=0;
	UINT8 socket=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	memset(ip_addr,0,MAX_IP_STRING_LEN);
	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);
	
	
	//printf("argc14:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);
	
	/*move to split_string_cmd for code slim*/
	//optind = 1; 
	//optarg = NULL; 
	
	opt = getopt(argc, argv, opString);
	
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 'a':
			//Printf_High("2:%s\n", optarg);
			ip_str_len =  (UINT16)strlen(optarg);
			if(ip_str_len >= MAX_IP_STRING_LEN)
			{
				DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
				return -1;
			}
			memcpy(ip_addr, optarg, ip_str_len);
			content++;
			break;
		case 'p':
			port =  (UINT16)simple_strtoul(optarg,&endptr,10);
			//Printf_High("3:%s\n", optarg);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}
	
	// iot_connect(6, "192.168.1.131", 1234);
	if(content == 2)
	{
		socket = iot_connect(ip_addr, port);
		Printf_High("SOCKET NO:%d\n", socket);
	
	}
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}


INT32 IoT_Tcpip_AT_Send(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:d:?";
	char opt;
	char *endptr;
	
	UINT8 send_data[MAX_TCP_DATA];
	UINT8 socket=0;
	UINT16 data_len;
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	memset(send_data,0, MAX_TCP_DATA);
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
		case 's':
			
			socket =  (UINT8) simple_strtoul(optarg,&endptr,10);
			//Printf_High("1:%s,%d\n",optarg, socket);
			content++;
			break;
		case 'd':
			//Printf_High("2:%s\n", optarg);
			data_len =  (UINT16) strlen(optarg);
			if(data_len >= MAX_TCP_DATA)
			{
				DBGPRINT(RT_DEBUG_ERROR,("data_len error,%d\n",data_len));
				return -1;
			}
			memcpy(send_data, optarg, data_len);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}
	if(content == 2)
		iot_send(socket, send_data, strlen((char *)send_data));
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}


INT32 IoT_Tcpip_AT_Listen(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "p:?";
	char opt;
	char *endptr;
	
	UINT16 port=0;
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	
	//printf("IoT_Tcpip_AT_Listen\n");
	
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
		case 'p':
			port =  (UINT16)simple_strtoul(optarg,&endptr,10);
			//Printf_High("1:%s,%d\n",optarg, port);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}
	
	if(content == 1)
		iot_listen (port);
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}



INT32 IoT_Tcpip_AT_Disconnect(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:?";
	char opt;
	char *endptr;
	
	UINT8 content=0;
	UINT8 socket=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	
	//printf("IoT_Tcpip_AT_Listen\n");
	
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
		case 's':
			socket =  (UINT8)simple_strtoul(optarg,&endptr,10);
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
		iot_disconnect(socket);
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}



INT32 IoT_Tcpip_AT_Udp_Create(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "a:r:l:?";
	char opt = 0;
	char *endptr;
	
	UINT8	ip_addr[MAX_IP_STRING_LEN];
	UINT16 lport=0;
	UINT16 rport=0;
	UINT16 ip_str_len;
	
	UINT8 content=0;
	UINT8 socket=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	memset(ip_addr,0,MAX_IP_STRING_LEN);
	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);
	
	
	//Printf_High("argc14:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);

	/*move to split_string_cmd for code slim*/
	//optind = 1; 
	//optarg = NULL; 
	
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 'a':
			//Printf_High("1:%s\n", optarg);
			ip_str_len =	(UINT16)strlen(optarg);
			if(ip_str_len >= MAX_IP_STRING_LEN)
			{
				DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
				return -1;
			}
			memcpy(ip_addr, optarg, ip_str_len);
			content++;
			break;
		case 'r':
			rport =	simple_strtoul(optarg,&endptr,10);
			//Printf_High("2:%s,%d\n",optarg, rport);
			content++;
			break;
			
		case 'l':
			lport =  simple_strtoul(optarg,&endptr,10);
			//Printf_High("3:%s\n", optarg);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}
	
	if(content == 3)
	{
		socket = iot_udp_new(ip_addr, lport, rport);
		Printf_High("SOCKET NO:%d\n", socket);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));	 
	
	return 0;
	
}

INT32 IoT_Tcpip_AT_Udp_Del(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:?";
	char opt=0;
	char *endptr;
	
	UINT8 socket=0;
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
		case 's':
			socket =  (UINT8) simple_strtoul(optarg,&endptr,10);
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
		iot_udp_remove(socket);
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}



INT32 IoT_Tcpip_AT_Udp_Send(PCHAR command_buffer, INT16 at_cmd_len)
{
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:d:a:r:?";
	char opt = 0;
	char *endptr;
	
	UINT8 send_data[MAX_TCP_DATA];
	UINT8 socket=0;
	UINT16 data_len;
	UINT8	ip_addr[MAX_IP_STRING_LEN];
	UINT16 rport=0;
	UINT16 ip_str_len;
	
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	memset(send_data,0, MAX_TCP_DATA);
	memset(ip_addr,0,MAX_IP_STRING_LEN);
	
	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);
	
    //Printf_High("argc:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);
    
	/*move to split_string_cmd for code slim*/
	//optind = 1; 
	//optarg = NULL; 
	
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
		case 's':
			socket =  (UINT8)simple_strtoul(optarg,&endptr,10);
			//Printf_High("1f:%s,%d\n",optarg, socket);
			content++;
			break;
		case 'd':
			//Printf_High("2:%s\n", optarg);
			data_len =  (UINT16)strlen(optarg);
			if(data_len >= MAX_TCP_DATA)
			{
				DBGPRINT(RT_DEBUG_ERROR,("data_len error,%d\n",data_len));
				return -1;
			}
			memcpy(send_data, optarg, data_len);
			content++;
			break;
		case 'a':
			//Printf_High("3:%s\n", optarg);
			ip_str_len =	(UINT16)strlen(optarg);
			if(ip_str_len >= MAX_IP_STRING_LEN)
			{
				DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
				return -1;
			}
			memcpy(ip_addr, optarg, ip_str_len);
			content++;
			break;
		case 'r':
			rport = (UINT16)simple_strtoul(optarg,&endptr,10);
			//Printf_High("4:%s,%d\n",optarg,rport);
			content++;
			break;
		case '?':
		default:
			break;
		}
		opt = getopt(argc, argv, opString);
		
	}
	if(content == 4)
		iot_udp_send(socket, send_data, strlen((char *)send_data), ip_addr, rport);
	else
		DBGPRINT(RT_DEBUG_ERROR,("please check content, %d\n", content));
	
	return 0;
	
}

INT16 IoT_exec_AT_cmd_Set_Static_IP(PCHAR command_buffer, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "i:m:g:d:?";
	//char *endptr = NULL;
	//long num = 0;
	INT16 argc = 0;
	char opt = 0;
	//int i=0;

	UINT16 ip_str_len;
	UCHAR local_ip[MAX_IP_STRING_LEN];
	UCHAR ip_mask[MAX_IP_STRING_LEN];
	UCHAR gateway_ip[MAX_IP_STRING_LEN];
	UCHAR dns_ip[MAX_IP_STRING_LEN];

	PUCHAR pLocal_ip = NULL;
	PUCHAR pIp_mask = NULL;
	PUCHAR pGateway_ip = NULL;
	PUCHAR pDns_ip = NULL;

	memset(local_ip,0,MAX_IP_STRING_LEN);
	memset(ip_mask,0,MAX_IP_STRING_LEN);
	memset(gateway_ip,0,MAX_IP_STRING_LEN);
	memset(dns_ip,0,MAX_IP_STRING_LEN);

	split_string_cmd(command_buffer, at_cmd_len, &argc, argv);

	//Printf_High("argc1=%d , long=%d, int=%d, INT16=%d  char=%d\n",
	//	argc, sizeof(long), sizeof(int),sizeof(INT16), sizeof(char));

	//for(i=0; i<argc; i++)
	//	Printf_High(" argv[%d] = %s \n",i, argv[i]);

	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
			case 'i':
				ip_str_len =  (UINT16)strlen(optarg);
				if(ip_str_len >= MAX_IP_STRING_LEN)
				{
					DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
					return -1;
				}
				memcpy(local_ip, optarg, ip_str_len);
				pLocal_ip = local_ip;
				break;
			case 'm':
				ip_str_len =  (UINT16)strlen(optarg);
				if(ip_str_len >= MAX_IP_STRING_LEN)
				{
					DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
					return -1;
				}
				memcpy(ip_mask, optarg, ip_str_len);
				pIp_mask = ip_mask;
				
				break;

			case 'g':
				ip_str_len =  (UINT16)strlen(optarg);
				if(ip_str_len >= MAX_IP_STRING_LEN)
				{
					DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
					return -1;
				}
				memcpy(gateway_ip, optarg, ip_str_len);
				pGateway_ip = gateway_ip;
				break;

			case 'd':
				ip_str_len =  (UINT16)strlen(optarg);
				if(ip_str_len >= MAX_IP_STRING_LEN)
				{
					DBGPRINT(RT_DEBUG_ERROR,("ip_str_len error,%d\n",ip_str_len));
					return -1;
				}
				memcpy(dns_ip, optarg, ip_str_len);
				pDns_ip = dns_ip;
				break;

			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}
    //Printf_High("%s,%s,%s,%s",pLocal_ip, pIp_mask, pGateway_ip, pDns_ip);
	iot_netif_cfg(pLocal_ip, pIp_mask, pGateway_ip, pDns_ip);

	return 0;
}


INT16 IoT_exec_AT_cmd_Netstat(PCHAR command_buffer, INT16 at_cmd_len)
{
	Printf_High("IoT_exec_AT_cmd_Netstat\n");
	return 0;
}
#endif

