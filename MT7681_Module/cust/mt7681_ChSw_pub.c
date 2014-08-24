/******************************************************************************
 * MODULE NAME:     ch_switch_pub.c
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:
 * DESIGNER:        Jinchuan
 * DATE:            April 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 mediatek Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0       April 2014   - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include "iot_api.h"

#ifdef __MT7681

#ifndef CONFIG_SOFTAP
extern STA_ADMIN_CONFIG *pIoTStaCfg;
#endif

/*========================================================================
	Routine	Description:
		AsicSetChannel --  switch channel , bandwidth and  40MHz above/below setting
	Arguments:
		ch    :     Channel number,     if 'bw' is BW_40,  'ch' is Center channel   [1~14]
		bw   :     Bandwidth    [BW_20, BW40]
		ext_ch:  11n bandwidth setting  [EXTCHA_NONE,  EXTCHA_ABOVE,   EXTCHA_BELOW]
		             if 'bw'=BW_20,  'ext_ch' should be EXTCHA_NONE  or EXTCHA_ABOVE, these two options have the same effect for BW_20.
			      if 'bw'=BW_20,  'ext_ch' should not be EXTCHA_BELOW
	Return Value: 0 is success
	Note: 
========================================================================*/
INT AsicSetChannel(UCHAR ch, UCHAR bw, UCHAR ext_ch)
{
	BOOLEAN bScan =  FALSE;

	rtmp_bbp_set_bw(bw);

	/*  Tx/RX : control channel setting */
	/*set band as 40MHz above or Below*/
	rtmp_bbp_set_ctrlch(ext_ch);
	rtmp_mac_set_ctrlch(ext_ch);

	/* Let BBP register at 20MHz to do scan */
	AsicSwitchChannel(ch, bScan);

	return 0;
}


/*========================================================================
	Routine	Description:
		IoT_Cmd_Set_Channel --  switch channel
		
	Arguments:
	Return Value: 0 is success
	Note:  In present , default is switch to channel 1~14, when Bandwidth 20MHz. 
	          if BW_40,   the ext_ch should be adjusted while central channel >= 12, this is reserved for customization
========================================================================*/
VOID IoT_Cmd_Set_Channel(UINT8 Channel)
{
#ifdef CONFIG_SOFTAP
	AsicSetChannel(Channel, BW_20, EXTCHA_NONE);
#else
	AsicSetChannel(Channel, pIoTStaCfg->Cfg_BW, EXTCHA_ABOVE);
#endif

	return;
}

#endif // __MT7681
