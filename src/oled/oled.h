#ifndef __WY_LIB_OLED_H__
#define __WY_LIB_OLED_H__
#include "common.h"

void oledInit(void);
void oledReset(void);
void Screen_FillClear(uint8_t filler);
void Picture_display(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
void PictureContrastDisplay(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);

#endif /* __WY_LIB_OLED_H__ */
