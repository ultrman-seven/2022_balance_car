#include "common.h"

#define BEEP GPIOA, GPIO_Pin_11
void beepInit(void)
{
    GPIO_InitTypeDef bep;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    bep.GPIO_Mode = GPIO_Mode_Out_PP;
    bep.GPIO_Pin = GPIO_Pin_11;
    bep.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &bep);
    GPIO_ResetBits(BEEP);
}

void beepSet(FunctionalState state)
{
    if (state == ENABLE)
        GPIO_SetBits(BEEP);
    else
        GPIO_ResetBits(BEEP);
}

void beepForHundredMs(uint16_t time_100Ms)
{
    beepSet(ENABLE);
    while (time_100Ms--)
        delayMs(100);
    beepSet(DISABLE);
}
