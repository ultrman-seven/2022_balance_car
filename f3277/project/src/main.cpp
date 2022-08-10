#include "common.h"
#include "genObj.hpp"
#include "camera.h"
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
        RCC_PLLConfig(RCC_HSE_Div1, RCC_PLLMul_6);
        // RCC_PLLConfig(RCC_HSE_Div1, RCC_PLLMul_10);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
            ;
        RCC_SYSCLKConfig(RCC_PLL);
        while (RCC_GetSYSCLKSource() != 0x08)
            ;
    }
    return HSE_StartUpState;
}
GPIO::Gpio_Object red("e14");
GPIO::Gpio_Object green("e13");
void genInit(void);
int main(void)
{
    HSE_SysClock();
    delayInit();
    genInit();

    while (1)
    {
        /* code */
        sys::runFunList();
        red = !(camResult.x);
        green = camResult.x;
    }
    return 0;
}
