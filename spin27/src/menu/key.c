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
void key_A_Option(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))
        keyLeftOption();
    else
        keyRightOption();
}
void WY_EXTI_Init(const char *k, void (*callback)(void));

void keyInterruptInit(void)
{
    // GPIO_InitTypeDef g;
    WY_EXTI_Init("d6", keyUpOption);
    WY_EXTI_Init("c12", keyMidOption);
    WY_EXTI_Init("c11", keyDownOption);
    WY_EXTI_Init("b3", keyRightOption);
    WY_EXTI_Init("b4", keyLeftOption);

    // g.GPIO_Mode = GPIO_Mode_IPU;
    // g.GPIO_Speed = GPIO_Speed_50MHz;
    // g.GPIO_Pin = GPIO_Pin_3;
    // GPIO_Init(GPIOB, &g);
}
