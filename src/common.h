#ifndef B5075FB6_578A_4914_BA46_28864F2C431F
#define B5075FB6_578A_4914_BA46_28864F2C431F

#include "HAL_conf.h"
#include "MM32SPIN2xx_p.h"

void delay(__IO uint32_t time);
void delayInit(void);
void delayMs(uint32_t time);
int getTimeStamp(uint32_t *t);
#endif /* B5075FB6_578A_4914_BA46_28864F2C431F */
