#include "common.h"

#define CS_PIN GPIO_Pin_13
#define DI_PIN GPIO_Pin_15
#define DO_PIN GPIO_Pin_2
#define CLK_PIN GPIO_Pin_14
#define ORG_PIN GPIO_Pin_3

#define CS_PORT GPIOC
#define DO_PORT GPIOC
#define DI_PORT GPIOC
#define CLK_PORT GPIOC
#define ORG_PORT GPIOC

#define CS CS_PORT, CS_PIN
#define DO DO_PORT, DO_PIN
#define DI DI_PORT, DI_PIN
#define CLK CLK_PORT, CLK_PIN
#define ORG ORG_PORT, ORG_PIN

void AT93C46Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    gpio.GPIO_Pin = CS_PIN;
    GPIO_Init(CS_PORT, &gpio);
    gpio.GPIO_Pin = DO_PIN;
    GPIO_Init(DO_PORT, &gpio);
    gpio.GPIO_Pin = DI_PIN;
    GPIO_Init(DI_PORT, &gpio);
    gpio.GPIO_Pin = ORG_PIN;
    GPIO_Init(ORG_PORT, &gpio);
    gpio.GPIO_Pin = CLK_PIN;
    GPIO_Init(CLK_PORT, &gpio);

    GPIO_SetBits(ORG); // 16位存储
}

void ATsendData(uint16_t cmd, uint8_t len)
{
    GPIO_SetBits(CS);

    while (len--)
    {
        GPIO_ResetBits(CLK);
        if ((cmd & 0x80) >> 7)
            GPIO_SetBits(DI);
        else
            GPIO_ResetBits(DI);
        cmd = cmd << 1;
        __nop();
        GPIO_SetBits(CLK);
        __nop();
        __nop();
        GPIO_ResetBits(CLK);
    }
    GPIO_ResetBits(CS);
}

uint16_t readData(uint8_t len)
{
    uint16_t result = 0;
    GPIO_SetBits(CS);

    while (len--)
    {
        GPIO_SetBits(CLK);
        __nop();
        __nop();
        GPIO_ResetBits(CLK);
        __nop();
        result <<= 1;
        result += GPIO_ReadInputDataBit(DO);
    }
    GPIO_ResetBits(CS);
    return result;
}

#define READ 0xc0
#define WRITE 0xa0
#define ERASE 0xe0
#define EW_EN 0x98 //擦/写允许指令
#define EW_DS 0x80 //擦/写禁止指令

void ATClearPage(uint8_t add)
{
}

void ATWriteByte(uint16_t dat)
{
}
