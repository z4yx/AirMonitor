#ifndef __WIFI_TASK_H__
#define __WIFI_TASK_H__
#include "xip_ovly.h"
/******************************************************************************
 * MODULE NAME:     wifi_task.h
 * PROJECT CODE:    __RT7681__
 * DESCRIPTION:
 * DESIGNER:        ShangWei Huang 
 * DATE:            Oct 2011
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ralink Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Oct 2011    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/


/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/
VOID wifiTASK_LowPrioTask (VOID);
VOID wifi_rx_proc(VOID);
VOID wifi_state_machine(VOID);

VOID ws_goto_smnt(VOID)						XIP_ATTRIBUTE(".xipsec0");
VOID wifi_state_chg(UINT8 iStateMachine, UINT8 iSubState)  XIP_ATTRIBUTE(".xipsec0");
VOID ws_init(OUT BOOLEAN *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
VOID ws_smt_conn(OUT BOOLEAN *pb_enable)    XIP_ATTRIBUTE(".xipsec0");
VOID ws_scan(OUT BOOLEAN *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
VOID ws_auth(OUT BOOLEAN *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
VOID ws_assoc(OUT BOOLEAN *pb_enable)       XIP_ATTRIBUTE(".xipsec0");

VOID ws_4way(OUT BOOLEAN *pb_enable);
VOID ws_connected(OUT BOOLEAN *pb_enable);
VOID setFlagVfyInit(BOOLEAN flag);

#endif /* __WIFI_TASK_H__ */

