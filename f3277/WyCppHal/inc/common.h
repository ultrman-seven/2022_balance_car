#ifndef __WY_LIB_COMMON_H__
#define __WY_LIB_COMMON_H__

#include "hal_conf.h"
#define __weak __attribute__((weak))

union u64_split
{
    uint64_t val;
    uint8_t unit[8];
};
union u32_split
{
    uint32_t val;
    uint8_t unit[4];
};

union u16_split
{
    uint16_t val;
    uint8_t unit[2];
};
union float_split
{
    float val;
    uint8_t unit[4];
};
union double_split
{
    double val;
    uint8_t unit[8];
};

#define SYS_FREQ 120000000U
#define AHB_FREQ SYS_FREQ
#define APB1_FREQ (AHB_FREQ / 2)
#define APB2_FREQ (AHB_FREQ)

#define SYS_FREQ_MHz 120U
#define AHB_FREQ_MHz SYS_FREQ_MHz
#define APB1_FREQ_MHz (AHB_FREQ_MHz / 2)
#define APB2_FREQ_MHz (AHB_FREQ_MHz)

void delay(__IO uint32_t time);
void delayInit(void);
void delayMs(uint32_t time);
int getTimeStamp(uint32_t *t);
uint8_t pin2pinSource(uint16_t pin);
namespace sys
{
    void redirect_Printf(void (*f)(char));
    void runFunList(void);
    void throwFun2Main(void (*f)(void));
    void delFun(void (*f)(void));
    void sysErr(const char *s);
} // namespace sys

#endif /* __WY_LIB_COMMON_H__ */
