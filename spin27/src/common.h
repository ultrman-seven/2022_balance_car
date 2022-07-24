#ifndef __WY_LIB_SPIN27_COMMON_H__
#define __WY_LIB_SPIN27_COMMON_H__

#include "HAL_conf.h"
#include "MM32SPIN2xx_p.h"
typedef struct
{
    uint16_t x;
    uint16_t y;
} point;
typedef union
{
    uint64_t val;
    uint8_t unit[8];
} u64_split;

typedef union
{
    uint32_t val;
    uint8_t unit[4];
} u32_split;

typedef union
{
    uint16_t val;
    uint8_t unit[2];
} u16_split;

typedef union
{
    float val;
    uint8_t unit[4];
} float_split;

typedef union
{
    double val;
    uint8_t unit[8];
} double_split;

void delay(__IO uint32_t time);
void delayInit(void);
void delayMs(uint32_t time);
int getTimeStamp(uint32_t *t);
int getLongTimeStamp(unsigned long *t);
#endif /* __WY_LIB_SPIN27_COMMON_H__ */
