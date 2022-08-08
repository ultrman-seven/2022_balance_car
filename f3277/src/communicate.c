#include "common.h"
#include "camera.h"

#define DATA_PORT GPIOF
#define DATA_RCC RCC_AHBENR_GPIOF

#define REQUEST_PORT GPIOC
#define REQUEST_RCC RCC_AHBPeriph_GPIOC
#define REQUEST_PIN_SOURCE GPIO_PinSource0
#define REQUEST_NVIC EXTI0_IRQn
#define REQUEST_EXTI_PORT EXTI_PortSourceGPIOC

#define REQUEST_PIN (0X01 << REQUEST_PIN_SOURCE)
#define REQUEST_EXTI_LINE REQUEST_PIN
#define REQUEST_EXTI_SOURCE REQUEST_PIN_SOURCE

#define OK_PORT GPIOC
#define OK_RCC RCC_AHBPeriph_GPIOC
#define OK_PIN_SOURCE GPIO_PinSource1
#define OK_PIN (0X01 << OK_PIN_SOURCE)

void setExtiCallbackFunction(uint8_t line, void (*f)(void));
void dat2spin27(void)
{
    GPIO_Write(DATA_PORT, camResult.x);
    delay(10);
    GPIO_SetBits(OK_PORT, OK_PIN);
    delay(50);
    GPIO_ResetBits(OK_PORT, OK_PIN);

    GPIO_Write(DATA_PORT, camResult.y);
    delay(15);
    GPIO_SetBits(OK_PORT, OK_PIN);
    delay(50);
    GPIO_ResetBits(OK_PORT, OK_PIN);
}
void communicateInit(void)
{
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;
    EXTI_InitTypeDef exti;

    RCC_AHBPeriphClockCmd(REQUEST_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(DATA_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(OK_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = 0xffff;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DATA_PORT, &gpio);

    gpio.GPIO_Pin = OK_PIN;
    GPIO_Init(OK_PORT, &gpio);
    GPIO_ResetBits(OK_PORT, OK_PIN);

    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = REQUEST_PIN;
    GPIO_Init(REQUEST_PORT, &gpio);

    nvic.NVIC_IRQChannel = REQUEST_NVIC;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);

    SYSCFG_EXTILineConfig(REQUEST_EXTI_PORT, REQUEST_EXTI_SOURCE);

    exti.EXTI_Line = REQUEST_EXTI_LINE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&exti);
    setExtiCallbackFunction(REQUEST_EXTI_SOURCE, dat2spin27);
}
