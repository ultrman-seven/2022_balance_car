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
        // 24 ---PLL_MUL_6---> 144 MHz
        RCC_PLLConfig(RCC_HSE_Div1, RCC_PLLMul_5);
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

#define CAM_RED_PIN_SOURCE GPIO_PinSource14
#define CAM_RED_PIN (0x01 << CAM_RED_PIN_SOURCE)
#define CAM_RED_PORT GPIOE
#define CAM_RED_RCC RCC_AHBPeriph_GPIOE
#define CAM_RED CAM_RED_PORT, CAM_RED_PIN

#define CAM_GREEN_PIN_SOURCE GPIO_PinSource13
#define CAM_GREEN_PIN (0x01 << CAM_GREEN_PIN_SOURCE)
#define CAM_GREEN_PORT GPIOE
#define CAM_GREEN_RCC RCC_AHBPeriph_GPIOE
#define CAM_GREEN CAM_GREEN_PORT, CAM_GREEN_PIN

void cam_LED_init(void)
{
    RCC_AHBPeriphClockCmd(CAM_RED_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(CAM_GREEN_RCC, ENABLE);

    GPIO_InitTypeDef g;
    g.GPIO_Mode = GPIO_Mode_Out_PP;
    g.GPIO_Pin = CAM_RED_PIN;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAM_RED_PORT, &g);

    g.GPIO_Pin = CAM_GREEN_PIN;
    GPIO_Init(CAM_GREEN_PORT, &g);

    GPIO_ResetBits(CAM_RED);
    GPIO_ResetBits(CAM_GREEN);
}

void communicateInit(void);
int main(void)
{
    // HSE_SysClock();
    delayInit();
    // computerUART_init();
    cameraInit();
    cameraOn();

    delayMs(5000);
    communicateInit();
    boardLED_init();
    cam_LED_init();

    while (1)
    {
        // LED_flip();
        cameraPicOption();
        if (camResult.x)
        {
            GPIO_SetBits(CAM_RED);
            GPIO_ResetBits(CAM_GREEN);
        }
        else
        {
            GPIO_SetBits(CAM_GREEN);
            GPIO_ResetBits(CAM_RED);
        }
        // delayMs(20);
    }
}
