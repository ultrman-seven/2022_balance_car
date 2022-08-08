#include "common.h"
// #include "spin27/src/common.h"

#define BEEP GPIOC, GPIO_Pin_5

void __time16Init(uint16_t period, uint16_t prescaler)
{
    TIM_TimeBaseInitTypeDef time;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
    time.TIM_ClockDivision = TIM_CKD_DIV1;
    time.TIM_CounterMode = TIM_CounterMode_Up;
    time.TIM_RepetitionCounter = 0;
    time.TIM_Period = period;
    time.TIM_Prescaler = prescaler;
    TIM_TimeBaseInit(TIM16, &time);

    nvic.NVIC_IRQChannel = TIM16_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&nvic);

    TIM_ClearFlag(TIM16, TIM_FLAG_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM16, DISABLE);
}
void TIM16_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM16, TIM_FLAG_Update);
    TIM_Cmd(TIM16, DISABLE);
    GPIO_ResetBits(BEEP);
}
void beepInit(void)
{
    GPIO_InitTypeDef bep;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    bep.GPIO_Mode = GPIO_Mode_Out_PP;
    bep.GPIO_Pin = GPIO_Pin_5;
    bep.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &bep);
    GPIO_ResetBits(BEEP);
    __time16Init(1000, 9599);
}

void beep100Ms(void)
{
    GPIO_SetBits(BEEP);
    TIM_Cmd(TIM16, ENABLE);
}

void beepSet(FunctionalState state)
{
    if (state == ENABLE)
        GPIO_SetBits(BEEP);
    else
        GPIO_ResetBits(BEEP);
}
void beepFlip(void)
{
    if (GPIO_ReadInputDataBit(BEEP))
        GPIO_ResetBits(BEEP);
    else
        GPIO_SetBits(BEEP);
}

void beepForHundredMs(uint16_t time_100Ms)
{
    beepSet(ENABLE);
    while (time_100Ms--)
        delayMs(100);
    beepSet(DISABLE);
}
