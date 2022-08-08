#include "common.h"
#include "stdlib.h"

#define E_0_1_Priority 1
#define E_2_3_Priority 1
#define E_4_15_Priority 1

void (*EXTI_n_line_function[16])(void) = {NULL};

void setExtiCallbackFunction(uint8_t line, void (*f)(void))
{
    EXTI_n_line_function[line] = f;
}

const uint32_t gpioBases[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE};
const uint32_t gpioRccs[] = {RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOC, RCC_AHBPeriph_GPIOD};
const uint8_t extiIRQs[] = {
    EXTI0_1_IRQn,
    EXTI0_1_IRQn,
    EXTI2_3_IRQn,
    EXTI2_3_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
    EXTI4_15_IRQn,
};

const uint8_t extiPrioritys[] = {
    E_0_1_Priority,
    E_0_1_Priority,
    E_2_3_Priority,
    E_2_3_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority,
    E_4_15_Priority};

void WY_EXTI_Init(const char *k, void (*callback)(void))
{
    uint8_t n, pin_source = 0;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;
    EXTI_InitTypeDef exti;

    n = *k - ((*k >= 'a' && *k <= 'd') ? 'a' : 'A');
    while (*++k)
    {
        pin_source *= 10;
        pin_source += (*k - '0');
    }

    RCC_AHBPeriphClockCmd(gpioRccs[n], ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    EXTI_n_line_function[pin_source] = callback;

    SYSCFG_EXTILineConfig(n, pin_source);

    nvic.NVIC_IRQChannel = extiIRQs[pin_source];
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = extiPrioritys[n];
    NVIC_Init(&nvic);

    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Pin = (0x01 << pin_source);
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init((GPIO_TypeDef *)gpioBases[n], &gpio);

    exti.EXTI_Line = (0x01 << pin_source);
    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&exti);
}

#define EXTI_n_Func(n)                             \
    if (EXTI_GetITStatus(EXTI_Line##n##) != RESET) \
    {                                              \
        EXTI_ClearITPendingBit(EXTI_Line##n##);    \
        if (EXTI_n_line_function[n] != NULL)       \
            EXTI_n_line_function[n]();             \
    }

void EXTI0_1_IRQHandler(void)
{
    EXTI_n_Func(0);
    EXTI_n_Func(1);
}
void EXTI2_3_IRQHandler(void)
{
    EXTI_n_Func(2);
    EXTI_n_Func(3);
}
void EXTI4_15_IRQHandler(void)
{
    EXTI_n_Func(4);
    EXTI_n_Func(5);
    EXTI_n_Func(6);
    EXTI_n_Func(7);
    EXTI_n_Func(8);
    EXTI_n_Func(9);
    EXTI_n_Func(10);
    EXTI_n_Func(11);
    EXTI_n_Func(12);
    EXTI_n_Func(13);
    EXTI_n_Func(14);
    EXTI_n_Func(15);
}
