#ifndef __WY_LIB_COMMON_H__
#define __WY_LIB_COMMON_H__

#include "HAL_conf.h"
#include "MM32SPIN2xx_p.h"

void delay(__IO uint32_t time);
void delayInit(void);
void delayMs(uint32_t time);
int getTimeStamp(uint32_t *t);
int getLongTimeStamp(unsigned long *t);
#endif /* __WY_LIB_COMMON_H__ */
