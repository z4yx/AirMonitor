#ifndef _ASSERT_H_
#define _ASSERT_H_
/******************************************************************************
 * MODULE NAME:     assert.h
 * PROJECT CODE:    __RT7681__
 * DESCRIPTION:     The header defines ASSERT for debug.
 * DESIGNER:        Charles Su
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
#include "types.h"



/******************************************************************************
 * ASSERT
 * Description :
 *   ASSERT function is used to assert the expression is true.
 ******************************************************************************/
#if ((DBG_ENA==1) && (DBG_ASSERT==1))
STATIC INLINE VOID ASSERT(INT32 Expression) { if ((Expression) == 0) { while(1); } }
#else
#define ASSERT(expression)
#endif

#endif /* _ASSERT_H_ */

