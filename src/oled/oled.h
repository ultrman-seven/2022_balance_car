#ifndef A37459A3_0F53_4E31_9735_D570718EAE44
#define A37459A3_0F53_4E31_9735_D570718EAE44
#include "common.h"

void oledInit(void);
void oledReset(void);
void Screen_FillClear(uint8_t filler);
void Picture_display(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);
void PictureContrastDisplay(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col);

#endif /* A37459A3_0F53_4E31_9735_D570718EAE44 */
