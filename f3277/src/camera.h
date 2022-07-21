#ifndef AEFBEEE5_E12F_4B1E_ABBA_864363E71034
#define AEFBEEE5_E12F_4B1E_ABBA_864363E71034
#include "common.h"
#define PIC_LINE 120
#define PIC_COL 188
// #define PIC_LINE 60
// #define PIC_COL 94

#define PIC_CUT_LINE 32
#define PIC_CUT (PIC_COL * PIC_CUT_LINE)

typedef struct
{
    uint16_t x;
    uint16_t y;
} point;
extern int16_t camResult;
void cameraInit(void);
void cameraOn(void);
void cameraOff(void);
void cameraPicOption(void);
#endif /* AEFBEEE5_E12F_4B1E_ABBA_864363E71034 */
