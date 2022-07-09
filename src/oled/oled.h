#ifndef __WY_LIB_OLED_H__
#define __WY_LIB_OLED_H__
#include "common.h"
// #define _IPS_114__
#if defined _IPS_114__
void showGrayPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
#else
// void showGrayPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col) {}
#endif
// void ips114_clear(uint16_t color);
void oledInit(void);
void oledReset(void);
void Screen_FillClear(uint8_t filler);
void Picture_display(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
void PictureContrastDisplay(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
void showPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
void showGrayPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
#endif /* __WY_LIB_OLED_H__ */
