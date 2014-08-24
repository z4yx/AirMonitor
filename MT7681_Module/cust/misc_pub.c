#include <types.h>

UINT32 gCPULowClok=0;

void DelayTick(BYTE Time) /* 4 clock cycle for each loop , 4 * 25ns = 100ns */
{
	for(;Time!=0; Time--) __asm__ __volatile__ ("nop");
}


/* 20121206 jamie suggestion */
/* CPU clock is change from 40MHZ=>1MHZ => 40 times */
/* 10m (mhz) vs 1/32*8 (1mhz) ==> 40 times */
void usecDelay(UINT32 micro)
{
	/* MCUCLK=40MHz, DelayTick(1)=1us */
	/* MCUCLK=1MHz, DelayTick(1)=40us */
	if(gCPULowClok==0)
	{
		for(;micro!=0; micro--) DelayTick(10);
	}
	else
	{
		micro = (micro+31)>>5;
		for(;micro!=0; micro--) DelayTick(8);
	}
}

void msecDelay(UINT32 milli)
{
	UINT32 jj=1000;

	if(gCPULowClok==0)
	{
		for(;milli!=0; milli--)
		for(;jj!=0; jj--) DelayTick(40);
	}
	else
	{
		jj = jj/8;
		for(;milli!=0; milli--)
		for(;jj!=0; jj--) DelayTick(1);
	}
}

