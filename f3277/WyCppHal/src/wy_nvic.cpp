#include "wy_nvic.hpp"

void NVIC_Init(uint8_t NVIC_IRQChannel, uint8_t p, uint8_t sub_p, bool en)
{
    if (en)
    {
        u32 pri = (SCB_AIRCR_PRIGROUP & ~(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)) >> SCB_AIRCR_PRIGROUP_Pos;

        pri = (((u32)p << (0x4 - pri)) | (sub_p & (0x0F >> pri))) << 0x04;

        NVIC->IP[NVIC_IRQChannel] = pri;
        NVIC->ISER[NVIC_IRQChannel >> 0x05] = 0x01 << (NVIC_IRQChannel & 0x1F);
    }
    else
    {
        NVIC->ICER[NVIC_IRQChannel >> 0x05] = 0x01 << (NVIC_IRQChannel & 0x1F);
    }
}
