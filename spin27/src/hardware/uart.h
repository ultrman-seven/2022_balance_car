#ifndef B73B2788_C01C_47C5_B18D_5F957003E36D
#define B73B2788_C01C_47C5_B18D_5F957003E36D
#include "common.h"
void uartInit(void);
void uart1SendByte(uint8_t dat);
void uart1SendBytes(uint8_t *dat, uint16_t len);
void uart1SendWord(uint32_t dat);
void uart1HalfWord(uint16_t dat);
#endif /* B73B2788_C01C_47C5_B18D_5F957003E36D */
