#include "wy_key.hpp"
#include "wy_nvic.hpp"
using namespace KEY;

void (*extiCallback[16])(void) = {nullptr};
// std::function<void(void)> extiCallback[16] = {nullptr};
// std::pointer_to_unary_function<void,void> extiCallback[16];
void KEY_Object::setOption(void (*f)(void))
// void KEY_Object::setOption(std::function<void(void)> f)
{
    extiCallback[this->pin_source] = f;
}

KEY_Object::KEY_Object(const char *n,EXTITrigger_TypeDef t, bool interrupt, uint8_t p, uint8_t sub_p)
{
    // this->familyInit(n, true);
    GPIO::turnOnRcc(*n);
    GPIO::modeConfig(n, GPIO_Mode_IPU);
    this->pin_source = GPIO::getPinFromStr(n + 1);
    if (interrupt)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        SYSCFG_EXTILineConfig(GPIO::getGPIO_Index(*n), this->pin_source);

        EXTI_InitTypeDef exti;
        switch (this->pin_source)
        {
        case 0:
            NVIC_Init(EXTI0_IRQn, p, sub_p, interrupt);
            break;
        case 1:
            NVIC_Init(EXTI1_IRQn, p, sub_p, interrupt);
            break;
        case 2:
            NVIC_Init(EXTI2_IRQn, p, sub_p, interrupt);
            break;
        case 3:
            NVIC_Init(EXTI3_IRQn, p, sub_p, interrupt);
            break;
        case 4:
            NVIC_Init(EXTI4_IRQn, p, sub_p, interrupt);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            NVIC_Init(EXTI9_5_IRQn, p, sub_p, interrupt);
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            NVIC_Init(EXTI15_10_IRQn, p, sub_p, interrupt);
            break;

        default:
            break;
        }
        exti.EXTI_Line = 0x0001 << (this->pin_source);
        exti.EXTI_LineCmd = ENABLE;
        exti.EXTI_Mode = EXTI_Mode_Interrupt;
        exti.EXTI_Trigger = t;
        EXTI_Init(&exti);
    }
}

#define EXTI_FUNC(num)                                                              \
    if (((EXTI->PR & EXTI_Line##num##) && (EXTI->IMR & EXTI_Line##num##)) != RESET) \
    {                                                                               \
        if (extiCallback[num] != nullptr)                                           \
            extiCallback[num]();                                                    \
        EXTI->PR = EXTI_Line##num##;                                                \
    }

#define EXTI_IRQ_FUNC(num) \
    void EXTI##num##_IRQHandler(void) { EXTI_FUNC(num) }

EXTI_IRQ_FUNC(0)
EXTI_IRQ_FUNC(1)
EXTI_IRQ_FUNC(2)
EXTI_IRQ_FUNC(3)
EXTI_IRQ_FUNC(4)

void EXTI9_5_IRQHandler(void)
{
    EXTI_FUNC(5)
    EXTI_FUNC(6)
    EXTI_FUNC(7)
    EXTI_FUNC(8)
    EXTI_FUNC(9)
}
void EXTI15_10_IRQHandler(void)
{
    EXTI_FUNC(10)
    EXTI_FUNC(11)
    EXTI_FUNC(12)
    EXTI_FUNC(13)
    EXTI_FUNC(14)
    EXTI_FUNC(15)
}
