#ifndef __OLEDIO_H__
#define __OLEDIO_H__
#include "oled.h"
#include "stdio.h"

#define __RECURSION__ //是否使用递归输出数字
#define __SPI_HARD__  //是否使用硬件spi

#ifdef __SPI_HARD__
//#define __NSS_HARD__ //硬件片选还是软件片选
#endif

extern int16_t chooseLine;
extern char oled_str[64];
#define OLED_printf(...)              \
    sprintf(oled_str, ##__VA_ARGS__); \
    OLED_print(oled_str)

void OLED_print(char *str);
void OLED_putchar(char ch);
void OLED_putNumber(__IO int num);
void OLED_putFractionalIntNumber(int num, int bit);
void screenClear(void);
#endif // !__OLEDIO_H__
