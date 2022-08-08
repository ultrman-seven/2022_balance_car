#include "common.h"
#include "encoder.h"
#include "motor.h"
#define HC244OE_PIN GPIO_Pin_3
#define HC244OE_PORT GPIOD
#define HC244OE HC244OE_PORT, HC244OE_PIN
#define RCC_HC244 RCC_AHBPeriph_GPIOD

#define v12_PIN GPIO_Pin_5
#define v12_PORT GPIOB
#define v12 v12_PORT, v12_PIN
#define RCC_v12 RCC_AHBPeriph_GPIOB

void motorInit(void)
{
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;
    GPIO_InitTypeDef io;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_HC244, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_v12, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_1);

    io.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    io.GPIO_Mode = GPIO_Mode_AF_PP;
    io.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &io);

    io.GPIO_Pin = HC244OE_PIN;
    io.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HC244OE_PORT, &io);
    // GPIO_SetBits(HC244OE);
    GPIO_ResetBits(HC244OE);

    io.GPIO_Pin = v12_PIN;
    GPIO_Init(v12_PORT, &io);
    GPIO_SetBits(v12);

    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = PWM_PERIOD;
    tim.TIM_Prescaler = 0;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &tim);

    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OutputNState = TIM_OutputNState_Disable;
    oc.TIM_Pulse = 300;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_OCNPolarity = TIM_OCNPolarity_High;
    oc.TIM_OCIdleState = TIM_OCIdleState_Set;
    oc.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM3, &oc);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM3, &oc);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OC3Init(TIM3, &oc);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OC4Init(TIM3, &oc);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void setPower(int32_t power, MotorChoose side)
{
    // power *= 5;
    if (power > PWM_PERIOD)
        power = PWM_PERIOD;
    if (power < -PWM_PERIOD)
        power = -PWM_PERIOD;
    if (side == LEFT)
    {
        if (power > 0)
        {
            TIM3->CCR3 = power;
            TIM3->CCR4 = 0;
        }
        else
        {
            TIM3->CCR4 = (-power);
            TIM3->CCR3 = 0;
        }
    }
    else if (power < 0)
    {
        TIM3->CCR1 = (-power);
        TIM3->CCR2 = 0;
    }
    else
    {
        TIM3->CCR2 = power;
        TIM3->CCR1 = 0;
    }
}

void motorTest(uint16_t *pwms)
{
    TIM3->CCR1 = *pwms++;
    TIM3->CCR2 = *pwms++;
    TIM3->CCR3 = *pwms++;
    TIM3->CCR4 = *pwms++;
}
