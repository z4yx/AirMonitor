/******************************************************************************
 * MODULE NAME:	 main.c
 * PROJECT CODE:	__RT7681__
 * DESCRIPTION:
 * DESIGNER:		Charles Su
 * DATE:			Oct 2011
 *
 *最新版本程序我们会在 http://www.ai-thinker.com 发布下载链接
 *
 * LICENSE:
 *	 This source code is copyright (c) 2011 Ralink Tech. Inc.
 *	 All rights reserved.
 *
 * REVISION	 HISTORY:
 *   V1.0.0	 Oct 2011	- Initial Version V1.0
 *   V1.0.1	 Dec 2011	- Add Caution
 *
 *
 * 串口WIFI 价格大于500 30元   大于5K  28元   大于10K  20元
 * ISSUES:
 *	First Implementation.
 * 淘宝店铺http://anxinke.taobao.com/?spm=2013.1.1000126.d21.FqkI2r
 *
 ******************************************************************************/
#include "stdio.h"
#include "types.h"
#include "wifi_task.h"
#include "iot_api.h"
#include "iot_custom.h"

/*****************************************************************
  Extern Paramter
******************************************************************/
extern IOT_CUST_OP IoTCustOp;



/*****************************************************************
  Functions
******************************************************************/
/******************************************************************************
 * main
 * Description :
 *  The main function is used to initialize all the functionalities in C.
 ******************************************************************************/
OUT INT32
main (VOID)
{
	UINT8 i=0;
	
	/* customer hook function initial */
	IoT_Cust_Ops();

	/* The entry for customization */
	if (IoTCustOp.IoTCustPreInit != NULL)
		IoTCustOp.IoTCustPreInit();

	/* Initialize BSP */
	BSP_Init();

	/*GPIO initial*/   /*moved from BSP_Init() for customization*/
	for (i=0; i<=4; i++)
	{
		IoT_gpio_output(i, DEFAULT_GPIO04_OUTVAL);
	}

#if (WIFI_SUPPORT==1)
	/* Initialize APP */
	APP_Init();
#endif

	sys_Init();

	/* run customer initial function */
	if (IoTCustOp.IoTCustInit != NULL)
	{
		IoTCustOp.IoTCustInit();
	}

		/* Create Tasks */
	/*******************************************************************************
	 ****** CAUTION : SOFTWARE PROGRAMMER SHALL NOT MODIFY THE FOLLOWING CODES *****
	 ******************************************************************************/
	sysTASK_RegTask(wifiTASK_LowPrioTask);

#if (ATCMD_SLT_SUPPORT == 1)
	SLT_TEST();
#endif

	/* Start the Kernel process */
	sysKernelStart();

	return 0;
}

