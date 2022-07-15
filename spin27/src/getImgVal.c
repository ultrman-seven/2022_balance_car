#include "common.h"

#define DATA_PORT GPIOA
#define DATA_RCC RCC_AHBPeriph_GPIOA

#define REQUEST_PORT GPIOC
#define REQUEST_RCC RCC_AHBPeriph_GPIOC
#define REQUEST_PIN_SOURCE GPIO_PinSource13
#define REQUEST_PIN (0X01 << REQUEST_PIN_SOURCE)

#define OK_PORT GPIOC
#define OK_RCC RCC_AHBPeriph_GPIOC
#define OK_PIN_SOURCE GPIO_PinSource14
#define OK_PIN (0X01 << OK_PIN_SOURCE)


void communicateInit(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(REQUEST_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(DATA_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(OK_RCC, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = 0xffff;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DATA_PORT, &gpio);

    gpio.GPIO_Pin = OK_PIN;
    GPIO_Init(OK_PORT, &gpio);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Pin = REQUEST_PIN;
    GPIO_Init(REQUEST_PORT, &gpio);
    GPIO_SetBits(REQUEST_PORT, REQUEST_PIN);
}

typedef union 
{
    uint16_t us;
    int16_t s;
}u16to16;

int16_t getImgData(void)
{
    u16to16 dat;
    GPIO_ResetBits(REQUEST_PORT, REQUEST_PIN);
    while (!GPIO_ReadInputDataBit(OK_PORT,OK_PIN))
        ;
    dat.us = GPIO_ReadInputData(DATA_PORT);
    GPIO_SetBits(REQUEST_PORT, REQUEST_PIN);
    return dat.s;
}

#include "oledio.h"
void test3277(void)
{
    screenClear();
    OLED_printf("img:%d", getImgData());
}
