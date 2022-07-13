#include "common.h"
#include "beep.h"
#include "oledio.h"
#include "menu.h"

// up:D6
// mid:C12
// down:C11
// k:C10
// A:b4
// B:b3
void setExtiCallbackFunction(uint8_t line, void (*f)(void));
void key_A_Option(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))
        keyLeftOption();
    else
        keyRightOption();
}
void keyInterruptInit(void)
{
    GPIO_InitTypeDef gpioInit;
    EXTI_InitTypeDef extiInit;
    NVIC_InitTypeDef nvicInit;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    nvicInit.NVIC_IRQChannelPriority = 2;
    nvicInit.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_Init(&nvicInit);

    gpioInit.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;
    GPIO_Init(GPIOB, &gpioInit);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);
    setExtiCallbackFunction(6, keyUpOption);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
    setExtiCallbackFunction(4, key_A_Option);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
    setExtiCallbackFunction(10, NULL);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
    setExtiCallbackFunction(11, keyDownOption);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);
    setExtiCallbackFunction(12, keyMidOption);

    extiInit.EXTI_Line = EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line4 | EXTI_Line6;
    extiInit.EXTI_LineCmd = ENABLE;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&extiInit);
}
