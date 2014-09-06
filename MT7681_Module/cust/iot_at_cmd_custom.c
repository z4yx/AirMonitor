#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iot_api.h"
#include "ntp.h"
#include "yeelink.h"
#include "common.h"

extern INT16 optind; 
extern char *optarg; 

static void fix_replaced_char(char *str)
{
	for(;*str; str++) {
		if(*str == '_')
			*str = '-';
	}
}

INT32 IoT_exec_AT_cmd_ntp_query(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "h:?";
	char *endptr = NULL;
	INT16 argc = 0;
	char opt = 0;

	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);

	opt = getopt(argc, argv, opString);

	while (opt != -1)
	{
		switch (opt)
		{
			case 'h':
				Printf_High("%s: host=[%s]\r\n", __func__, optarg);
				ntp_query(optarg);
				break;
			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}

	return 0;
}

INT32 IoT_exec_AT_cmd_yeelink_pub(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	char *argv[MAX_OPTION_COUNT];
	char *opString = "h:a:s:t:?";
	char *endptr = NULL;
	INT16 argc = 0;
	char opt = 0;

	const char *ts, *humi, *temp, *air;

	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);

	opt = getopt(argc, argv, opString);

	while (opt != -1)
	{
		switch (opt)
		{
			case 'h':
				humi = optarg;
				break;
			case 's':
				fix_replaced_char(optarg);
				ts = optarg;
				break;
			case 'a':
				air = optarg;
				break;
			case 't':
				temp = optarg;
				break;
			case '?':
			default:
				break;
		}
		opt = getopt(argc, argv, opString);
	}
	// LOG_D("ts=[%s], humi=[%s], temp=[%s], air=[%s]",
	// 	ts, humi, temp, air);
	Yeelink_Send(ts, air, temp, humi);

	return 0;
}

INT32 IoT_exec_AT_cmd_yeelink_stat(PCHAR pCmdBuf, INT16 at_cmd_len)
{
	Yeelink_ReportState();

	return 0;
}