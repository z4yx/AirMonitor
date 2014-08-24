#include "uip.h"
#include "queue.h"
#include "iot_api.h"

EXTERN QU_t gFreeQueue1;

/*---------------------------------------------------------------------------*/
void
mt76xx_dev_init(void)
{

}
/*---------------------------------------------------------------------------*/
unsigned int
mt76xx_dev_read(u8_t *p, u16_t len)
{
	//if(p[0]==0x01&&p[1]==0&&p[2]==0x5e){
	//	uip_len=0;
	//	return 0;
	//}
#if 0  /*for debug*/
	u8_t i;
	Printf_High("read: %d ", len);
	for(i=0;i<14;i++)
		Printf_High("0x%02x ", uip_buf[i]);
	Printf_High("\n");
	for(i=0;i<20;i++)
		Printf_High("0x%02x ", uip_buf[i+14]);
	Printf_High("\n");
	for(i=0;i<40;i++)
		Printf_High("0x%02x ", uip_buf[i+34]);
	Printf_High("\n");
#endif
	
	memcpy(uip_buf, p, len);
	uip_len = len;
	
	return 0;
}
/*---------------------------------------------------------------------------*/
void
mt76xx_dev_send(void)
{
	pBD_t pBufDesc;

	//handle_FCE_TxTS_interrupt();
	pBufDesc = apiQU_Dequeue(&gFreeQueue1);

	if(pBufDesc ==NULL)
	{
		printf("dequeue fail\n");
		return;
	}
#if 0 /*for debug*/
	{
	u8_t i;
	Printf_High("write: %d ", uip_len);
	for(i=0;i<14;i++)
		Printf_High("0x%02x ", uip_buf[i]);
	Printf_High("\n");
	for(i=0;i<20;i++)
		Printf_High("0x%02x ", uip_buf[i+14]);
	Printf_High("\n");
	for(i=0;i<40;i++)
		Printf_High("0x%02x ", uip_buf[i+34]);
	Printf_High("\n");
	}
#endif
	memcpy(pBufDesc->pBuf, uip_buf, uip_len);

#ifndef CONFIG_SOFTAP
	STA_Legacy_Frame_Tx(pBufDesc, uip_len, GetClearFrameFlag());
#else
	/*in AP mode,  only support Open mode in present, 
	   if wpa2-psk supported in furture, TRUE will be replace with other value*/
	STA_Legacy_Frame_Tx(pBufDesc, uip_len, TRUE); 
#endif

}
/*---------------------------------------------------------------------------*/
