#include "common.h"
#include "camera.h"
#include "uart.h"
#include "stdio.h"
ErrorStatus HSE_SysClock(void)
{
    ErrorStatus HSE_StartUpState = ERROR;
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);

    HSE_StartUpState = RCC_WaitForHSEStartUp();
    if (HSE_StartUpState == SUCCESS)
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_HSE_Div1, RCC_PLLMul_15);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
            ;
        RCC_SYSCLKConfig(RCC_PLL);
        while (RCC_GetSYSCLKSource() != 0x08)
            ;
    }
    return HSE_StartUpState;
}

#define LED_RCC RCC_AHBENR_GPIOB
#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_1
void boardLED_init(void)
{
    GPIO_InitTypeDef g;

    RCC_AHBPeriphClockCmd(LED_RCC, ENABLE);

    g.GPIO_Mode = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    g.GPIO_Pin = LED_PIN;
    GPIO_Init(LED_PORT, &g);
}
void LED_flip(void)
{
    if (GPIO_ReadOutputDataBit(LED_PORT, LED_PIN))
        GPIO_ResetBits(LED_PORT, LED_PIN);
    else
        GPIO_SetBits(LED_PORT, LED_PIN);
}

void communicateInit(void);
int main(void)
{
    HSE_SysClock();
    delayInit();
    computerUART_init();
    cameraInit();
    cameraOn();

    delayMs(5000);
    communicateInit();
    boardLED_init();

    while (1)
    {
        cameraPicOption();
        // delayMs(20);
    }
}
