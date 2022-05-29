#include "common.h"
#include "beep.h"
#include "oledio.h"
#include "menu.h"

// up:D6
// left:C12
// right:C11
// mid:C10
// down:A15
void keyInterruptInit(void)
{
    GPIO_InitTypeDef gpioInit;
    EXTI_InitTypeDef extiInit;
    NVIC_InitTypeDef nvicInit;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    nvicInit.NVIC_IRQChannelPriority = 2;
    nvicInit.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_Init(&nvicInit);

    gpioInit.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &gpioInit);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource10);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

    extiInit.EXTI_Line = EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line15 | EXTI_Line6;
    extiInit.EXTI_LineCmd = ENABLE;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&extiInit);
}

void EXTI4_15_IRQHandler(void)
{
    void h_int(void);
    void v_int(void);
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        // screenClear();
        // OLED_print("\ndown:");
        keyDownOption();
        EXTI_ClearITPendingBit(EXTI_Line15);
    }

    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        // screenClear();
        // OLED_print("\nup:");
        keyUpOption();
        // printf("jb\n");
        EXTI_ClearITPendingBit(EXTI_Line6);
    }

    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        // OLED_print("left");
        keyLeftOption();
        EXTI_ClearITPendingBit(EXTI_Line12);
    }

    if (EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        // OLED_print("right");
        keyRightOption();
        EXTI_ClearITPendingBit(EXTI_Line11);
    }

    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        // OLED_print("\nmid:");
        keyMidOption();
        EXTI_ClearITPendingBit(EXTI_Line10);
    }

    if (EXTI_GetITStatus(EXTI_Line7) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        v_int();
    }
    if (EXTI_GetITStatus(EXTI_Line5) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
        h_int();
    }
}
