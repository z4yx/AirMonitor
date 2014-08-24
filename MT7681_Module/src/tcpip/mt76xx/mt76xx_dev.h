
#ifndef __MT76XX_DEV_H__
#define __MT76XX_DEV_H__
#include "uip-conf.h"

void mt76xx_dev_init(void);
unsigned int mt76xx_dev_read(u8_t *p, u16_t len);
void mt76xx_dev_send(void);

#endif
