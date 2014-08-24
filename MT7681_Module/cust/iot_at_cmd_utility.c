#include <stdio.h>
#include "string.h"
#include "types.h"
#include "uart.h"
#include "iot_custom.h"

/******************************************************************************
* MODULE NAME:     iot_at_cmd_utility.c
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
	
char *optarg = NULL; /* pointer to the start of the option argument */ 
INT16 optind = 1; /* number of the next argv[] to be evaluated */ 
char opterr = 1; /* non-zero if a question mark should be returned 
when a non-valid option character is detected */ 
	
#define _next_char(string) (char)(*(string+1)) 


char * strchr(const char *str, int ch)
{
   while (*str && *str != (char)ch)
        str++;
   if (*str == (char)ch)
        return((char *)str);
   return(NULL);
}

void split_string_cmd(PCHAR string_cmd, INT16 cmdlen, INT16 *pargc, char **argv)
{
	//int cmdlen = (int)strlen(string_cmd);
	argv[0] = (char *)&string_cmd[0];
	INT16 argc = 1;
	INT16 i=0;
	//printf("split_string_cmd1:%s\n", string_cmd);

	/*move to split_string_cmd for code slim*/
	optind = 1;
	optarg = NULL;

	for(i=0; i< cmdlen-1;i++)
	{
		if(string_cmd[i] == '-' && 
	//		(string_cmd[i] == '-' || string_cmd[i] == '/') && 
		    (string_cmd[i+1]<='0' || string_cmd[i+1]>='9') )
		{
			string_cmd[i-1]='\0';
			argv[argc] = (char *)&string_cmd[i];
			argc++;
		}
	}
	*pargc=argc;
	
	//printf("split_string_cmd2:%s,%s,%s,%s\n", argv[0], argv[1],argv[2],argv[3]);
	return;
}

char getopt(INT16 argc, char *argv[], char *opstring) 
{ 
	static char *pIndexPosition = NULL; /* place inside current argv string */ 
	char *pArgString = NULL; /* where to start from next */ 
	char *pOptString = NULL; /* the string in our program */ 


	if (pIndexPosition != NULL) { 
		/* we last left off inside an argv string */ 
		if (*(++pIndexPosition)) { 
		/* there is more to come in the most recent argv */ 
		pArgString = pIndexPosition; 
		} 
	} 

	if (pArgString == NULL) { 
		/* we didn't leave off in the middle of an argv string */ 
		if (optind >= argc) { 
			/* more command-line arguments than the argument count */ 
			pIndexPosition = NULL; /* not in the middle of anything */ 
			return EOF; /* used up all command-line arguments */ 
		} 

		/*--------------------------------------------------------------------- 
		* If the next argv[] is not an option, there can be no more options. 
		*-------------------------------------------------------------------*/ 
		pArgString = argv[optind++]; /* set this to the next argument ptr */ 

		if (('/' != *pArgString) && /* doesn't start with a slash or a dash? */ 
			('-' != *pArgString)) { 
			--optind; /* point to current arg once we're done */ 
			optarg = NULL; /* no argument follows the option */ 
			pIndexPosition = NULL; /* not in the middle of anything */ 
			return EOF; /* used up all the command-line flags */ 
		} 

		/* check for special end-of-flags markers */ 
		if ((strcmp(pArgString, "-") == 0) || 
			(strcmp(pArgString, "--") == 0)) { 
			optarg = NULL; /* no argument follows the option */ 
			pIndexPosition = NULL; /* not in the middle of anything */ 
			return EOF; /* encountered the special flag */ 
		} 

		pArgString++; /* look past the / or - */ 
	} 

	if (':' == *pArgString) { /* is it a colon? */ 
		/*--------------------------------------------------------------------- 
		* Rare case: if opterr is non-zero, return a question mark; 
		* otherwise, just return the colon we're on. 
		*-------------------------------------------------------------------*/ 
		return (opterr ? '?' : ':'); 
	} 
	else if ((pOptString = strchr(opstring, *pArgString)) == 0) { 
		/*--------------------------------------------------------------------- 
		* The letter on the command-line wasn't any good. 
		*-------------------------------------------------------------------*/ 
		optarg = NULL; /* no argument follows the option */ 
		pIndexPosition = NULL; /* not in the middle of anything */ 
		return (opterr ? '?' : *pArgString); 
	} 
	else { 
		/*--------------------------------------------------------------------- 
		* The letter on the command-line matches one we expect to see 
		*-------------------------------------------------------------------*/ 
		if (':' == _next_char(pOptString)) { /* is the next letter a colon? */ 
			/* It is a colon. Look for an argument string. */ 
			if ('\0' != _next_char(pArgString)) { /* argument in this argv? */ 
				optarg = &pArgString[1]; /* Yes, it is */ 
			} 
			else { 
				/*------------------------------------------------------------- 
				* The argument string must be in the next argv. 
				* But, what if there is none (bad input from the user)? 
				* In that case, return the letter, and optarg as NULL. 
				*-----------------------------------------------------------*/ 
				if (optind < argc) 
					optarg = argv[optind++]; 
				else { 
					optarg = NULL; 
					return (opterr ? '?' : *pArgString); 
				} 
			} 
		pIndexPosition = NULL; /* not in the middle of anything */ 
		} 
		else { 
		/* it's not a colon, so just return the letter */ 
			optarg = NULL; /* no argument follows the option */ 
			pIndexPosition = pArgString; /* point to the letter we're on */ 
		} 
		return *pArgString; /* return the letter that matched */ 
	} 
} 

