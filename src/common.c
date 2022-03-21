#include "common.h"
#include "stdio.h"
uint32_t sysTicDecTime = 0;
__IO uint32_t msTimeStamp = 0;
void delayInit(void)
{
    SysTick_Config(96000);
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

void delay(__IO uint32_t time)
{
    while (time--)
        __nop();
}

void delayMs(uint32_t time)
{
    sysTicDecTime = time;
    while (sysTicDecTime)
        ;
}

int getTimeStamp(uint32_t *t)
{
    *t = msTimeStamp;
    return 0;
}

void SysTick_Handler(void)
{
    msTimeStamp++;
    if (sysTicDecTime)
        sysTicDecTime--;
}

//重定义fputc函数
int fputc(int ch, FILE *f)
{
    UART_SendData(UART1, ch);
    while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        ;
    return ch;
}
