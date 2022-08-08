#include "common.h"
#include "stdlib.h"

void (*EXTI_n_line_function[16])(void) = {NULL};

void setExtiCallbackFunction(uint8_t line, void (*f)(void))
{
    EXTI_n_line_function[line] = f;
}

#define EXTI_n_Func(n)                             \
    if (EXTI_GetITStatus(EXTI_Line##n##) != RESET) \
    {                                              \
        EXTI_ClearITPendingBit(EXTI_Line##n##);    \
        if (EXTI_n_line_function[n] != NULL)       \
            EXTI_n_line_function[n]();             \
    }

#define EXTI_IRQ_FUNCTION(n) \
    void EXTI##n##_IRQHandler(void) { EXTI_n_Func(n) }

EXTI_IRQ_FUNCTION(0)
EXTI_IRQ_FUNCTION(1)
EXTI_IRQ_FUNCTION(2)
EXTI_IRQ_FUNCTION(3)
EXTI_IRQ_FUNCTION(4)

void EXTI9_5_IRQHandler(void)
{
    EXTI_n_Func(5);
    EXTI_n_Func(6);
    EXTI_n_Func(7);
    EXTI_n_Func(8);
    EXTI_n_Func(9);
}
void EXTI15_10_IRQHandler(void)
{
    EXTI_n_Func(10);
    EXTI_n_Func(11);
    EXTI_n_Func(12);
    EXTI_n_Func(13);
    EXTI_n_Func(14);
    EXTI_n_Func(15);
}
