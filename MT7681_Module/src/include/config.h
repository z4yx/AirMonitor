
#ifndef   __CONFIG_H__
#define   __CONFIG_H__
#include "xip_ovly.h"

#define OS_ENABLE			    0
#define IRQ_STACK_SIZE			256
#define	CFG_NONE_OS             1


#define MHz(x)				((x)*1000000)
#define SYS_CLK				MHz(40) /* 40 MHz */
#define TICK_HZ				1000 	/* tick: 1ms */
#define KAL_HZ                  	TICK_HZ 

#if ((ILM_ENA == 1) && (DLM_ENA == 1))
#define ILM_SIZE			64	/* KBytes */
#define DLM_SIZE			64	/* KBytes */
#define USE_SRAM_SIZE			(ILM_SIZE + DLM_SIZE)
#endif

#define UART_BAUD_RATE			115200

#define CLI_EN				1u 	/* Enable/Disable CLI */
#define CLI_BUFF_SIZE			256	/* CLI Buffer size */

/* GPT timer list */
#define GLOBAL_TIMER_SIZE   1				/* must be power of 2 */

#endif
