#include "wy_tim.hpp"
#include "wy_nvic.hpp"
#include "wy_gpio.hpp"
namespace TIM
{
#define getPWM_Channel(tim, c) (&(TIM##tim##->CCR1##c##))

    const uint32_t timBases[] = {TIM1_BASE, TIM2_BASE, TIM3_BASE, TIM4_BASE, TIM5_BASE, TIM6_BASE, TIM7_BASE, TIM8_BASE};
    const uint32_t timRccs[] = {RCC_APB2Periph_TIM1,
                                RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4, RCC_APB1ENR_TIM5, RCC_APB1ENR_TIM6, RCC_APB1ENR_TIM7,
                                RCC_APB2ENR_TIM8};
    void timBaseConfig(uint8_t timNum, uint32_t usPeriod)
    {
        uint8_t freq;
        if (timNum == 1 || timNum == 8)
        {
            RCC_APB2PeriphClockCmd(timRccs[timNum - 1], ENABLE);
            freq = APB2_FREQ_MHz;
        }
        else
        {
            RCC_APB1PeriphClockCmd(timRccs[timNum - 1], ENABLE);
            freq = APB1_FREQ_MHz;
        }
        TIM_TimeBaseInitTypeDef t;
        t.TIM_CounterMode = TIM_CounterMode_Up;
        t.TIM_RepetitionCounter = 0;
        t.TIM_ClockDivision = TIM_CKD_DIV1;
        t.TIM_Prescaler = (usPeriod <= 0xffff) ? freq - 1 : freq * 100 - 1;
        t.TIM_Period = (usPeriod <= 0xffff) ? usPeriod : usPeriod / 100;
        TIM_TimeBaseInit((TIM_TypeDef *)timBases[timNum - 1], &t);
        TIM_ARRPreloadConfig((TIM_TypeDef *)timBases[timNum - 1], ENABLE);
    }
    void timBaseConfig(TIM_TypeDef *tim, uint32_t usPeriod)
    {

        TIM_TimeBaseInitTypeDef t;
        t.TIM_CounterMode = TIM_CounterMode_Up;
        t.TIM_RepetitionCounter = 0;
        t.TIM_ClockDivision = TIM_CKD_DIV1;
        t.TIM_Prescaler = (usPeriod <= 0xffff) ? SYS_FREQ_MHz - 1 : SYS_FREQ * 100 - 1;
        t.TIM_Period = (usPeriod <= 0xffff) ? usPeriod : usPeriod / 100;
        TIM_TimeBaseInit(tim, &t);
    }
    // const TIMCHx_Typedef timChs[] = {TIM_Channel_1, TIM_Channel_2, TIM_Channel_3, TIM_Channel_4, TIM_Channel_5};

    TIMDMASRC_Typedef tim_d_s[4] = {TIM_DMA_CC1, TIM_DMA_CC2, TIM_DMA_CC3, TIM_DMA_CC4};
    void pClkInit(uint8_t tim, uint8_t ch, const char *p, uint8_t af)
    {
        timBaseConfig(tim, 100);
        GPIO::afConfig(p, af, GPIO_Mode_FLOATING);
        timIcInit(tim, ch);

        TIM_Cmd((TIM_TypeDef *)timBases[tim - 1], ENABLE);
        TIM_DMACmd((TIM_TypeDef *)timBases[tim - 1], tim_d_s[ch - 1], ENABLE);
    }
} // namespace TIM

using namespace TIM;
void TIM_Base_Object::setCountTime(uint16_t p, uint16_t s, bool e)
{
    this->tim->PSC = s;
    this->tim->ARR = p;
    if (e)
        this->tim->CR1 |= TIM_CR1_CEN;
    else
        this->tim->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
}

void TIM_Base_Object::setNVIC_Enable(bool e, uint8_t p, uint8_t sub_p)
{
    NVIC_Init(this->updateNVIC_Channel, p, sub_p, e);
    (e) ? SET_BIT(this->tim->DIER, TIM_IT_Update) : CLEAR_BIT(this->tim->DIER, TIM_IT_Update);
}

void (*timUpdateFunctions[8])(void);
const uint8_t timNVIC_Chs[] = {TIM1_UP_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM6_IRQn, TIM7_IRQn, TIM8_UP_IRQn};

TIM_Base_Object::TIM_Base_Object(uint8_t num, uint32_t us)
{
    this->updateFunc = timUpdateFunctions + num - 1;
    this->tim = (TIM_TypeDef *)(timBases[num - 1]);

    this->updateNVIC_Channel = timNVIC_Chs[num - 1];

    timBaseConfig(num, us);
}

// if (((TIM##num##->SR & TIM_IT_##name##) && (TIM##num##->DIER & TIM_IT_##name##))) \

#define TIM_IRQ_FUNC(num, name)                       \
    {                                                 \
        CLEAR_BIT(TIM##num##->SR, TIM_IT_##name##);   \
        if (tim##name##Functions[num - 1] != nullptr) \
            tim##name##Functions[num - 1]();          \
    }

void TIM1_UP_IRQHandler(void)
{
    TIM_IRQ_FUNC(1, Update);
}

void TIM8_UP_IRQHandler(void)
{
    TIM_IRQ_FUNC(8, Update);
}

#define TIM_IRQ_FUNCTION(num)        \
    void TIM##num##_IRQHandler(void) \
    {                                \
        TIM_IRQ_FUNC(num, Update)    \
    }

TIM_IRQ_FUNCTION(2);
TIM_IRQ_FUNCTION(3);
TIM_IRQ_FUNCTION(4);
TIM_IRQ_FUNCTION(5);
TIM_IRQ_FUNCTION(6);
TIM_IRQ_FUNCTION(7);
