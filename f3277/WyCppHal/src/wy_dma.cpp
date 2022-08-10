#include "wy_dma.hpp"
#include "wy_nvic.hpp"

#define DMA1_Channel_Num 7
#define DMA2_Channel_Num 5

void (*dma1TC_CallbackFun[DMA1_Channel_Num])(void) = {nullptr};
void (*dma2TC_CallbackFun[DMA2_Channel_Num])(void) = {nullptr};

void (*dma1GL_CallbackFun[DMA1_Channel_Num])(void) = {nullptr};
void (*dma2GL_CallbackFun[DMA2_Channel_Num])(void) = {nullptr};

void (*dma1HT_CallbackFun[DMA1_Channel_Num])(void) = {nullptr};
void (*dma2HT_CallbackFun[DMA2_Channel_Num])(void) = {nullptr};

void (*dma1TE_CallbackFun[DMA1_Channel_Num])(void) = {nullptr};
void (*dma2TE_CallbackFun[DMA2_Channel_Num])(void) = {nullptr};

// DMA_DeInit(DMA##num##_Channel##channel##);            \

#define DMA_CONFIG_FUNC(num, channel, tp)                                                 \
    dma##num##TC_CallbackFun[channel - 1] = callback;                                     \
    NVIC_Init(DMA##num##_Channel##channel##_IRQn, p, sp);                                 \
    RCC->AHBENR |= RCC_AHBENR_DMA##num##;                                                 \
    DMA_Init(DMA##num##_Channel##channel##, ##tp##);                                      \
    DMA##num##_Channel##channel##->CCR |= DMA_IT_TC;                                      \
    MODIFY_REG(DMA##num##_Channel##channel##->CCR, DMA_CCR_EN, ENABLE << DMA_CCR_EN_Pos); \
    return DMA##num##_Channel##channel##;

namespace dma
{
    DMA_Channel_TypeDef* configDMA(DMA_Source s, DMA_InitTypeDef *d, void (*callback)(void), uint8_t p, uint8_t sp)
    {
        //DMA_Channel_TypeDef *channel;
        switch (s)
        {
        case DMA_ADC1:
        case DMA_UART6_RX:
        case DMA_TIM2_CC3:
        case DMA_TIM4_CC1:
            DMA_CONFIG_FUNC(1, 1, d)


        case DMA_ADC2:
        case DMA_SPI1_RX:
        case DMA_UART3_TX:
        case DMA_TIM1_CC1:
        case DMA_TIM2_UP:
        case DMA_TIM3_CC3:
            DMA_CONFIG_FUNC(1, 2, d)

        case DMA_SPI1_TX:
        case DMA_UART3_RX:
        case DMA_TIM1_CC2:
        case DMA_TIM3_UP:
        case DMA_TIM3_CC4:
            DMA_CONFIG_FUNC(1, 3, d)

        case DMA_SPI2_RX:
        case DMA_UART1_TX:
        case DMA_I2C2_TX:
        case DMA_TIM1_CC4:
        case DMA_TIM1_TRIG:
        case DMA_TIM1_COM:
        case DMA_TIM4_CC2:
            DMA_CONFIG_FUNC(1, 4, d)

        case DMA_SPI2_TX:
        case DMA_UART1_RX:
        case DMA_I2C2_RX:
        case DMA_TIM1_UP:
        case DMA_TIM2_CC1:
        case DMA_TIM4_CC3:
            DMA_CONFIG_FUNC(1, 5, d)
		
        case DMA_UART2_RX:
        case DMA_I2C1_TX:
        case DMA_TIM1_CC3:
        case DMA_TIM3_CC1:
        case DMA_TIM3_TRIG:
            DMA_CONFIG_FUNC(1, 6, d)

        case DMA_UART2_TX:
        case DMA_I2C1_RX:
        case DMA_TIM2_CC2:
        case DMA_TIM2_CC4:
        case DMA_TIM4_UP:
            DMA_CONFIG_FUNC(1, 7, d)

        case DMA_SPI3_RX:
        case DMA_UART5_RX:
        case DMA_UART7_RX:
        case DMA_TIM5_CC4:
        case DMA_TIM5_TRIG:
        case DMA_TIM8_CC3:
        case DMA_TIM8_UP:
            DMA_CONFIG_FUNC(2, 1, d)

        case DMA_SPI3_TX:
        case DMA_UART5_TX:
        case DMA_UART7_TX:
        case DMA_TIM5_CC3:
        case DMA_TIM5_UP:
        case DMA_TIM8_CC4:
        case DMA_TIM8_TRIG:
        case DMA_TIM8_COM:
            DMA_CONFIG_FUNC(2, 2, d)

        case DMA_UART4_RX:
        case DMA_UART8_RX:
        case DMA_TIM8_CC1:
        case DMA_TIM6_UP:
        case DMA_DAC_CH1:
            DMA_CONFIG_FUNC(2, 3, d)

        case DMA_UART6_TX:
        case DMA_SDIO:
        case DMA_TIM5_CC2:
        case DMA_TIM7_UP:
        case DMA_DAC_CH2:
            DMA_CONFIG_FUNC(2, 4, d)

        case DMA_UART4_TX:
        case DMA_UART8_TX:
        case DMA_TIM5_CC1:
        case DMA_TIM8_CC2:
        case DMA_ADC3:
            DMA_CONFIG_FUNC(2, 5, d)
        default:
            break;
        }
    }

}

// void DMA1_Channel1_IRQHandler(void)
// {
//     if(DMA1->ISR & DMA1_IT_TC1)
//     {
//         DMA1->IFCR = DMA1_IT_TC1;
//         if (dma1TC_CallbackFun[0] != nullptr)
//             dma1TC_CallbackFun[0]();
//     }
//     if(DMA1->ISR & DMA1_IT_TE1)
//     {
//         DMA1->IFCR = DMA1_IT_TE1;
//         if (dma1TE_CallbackFun[0] != nullptr)
//             dma1TE_CallbackFun[0]();
//     }
//     if(DMA1->ISR & DMA1_IT_HT1)
//     {
//         DMA1->IFCR = DMA1_IT_HT1;
//         if (dma1HT_CallbackFun[0] != nullptr)
//             dma1HT_CallbackFun[0]();
//     }
//     if(DMA1->ISR & DMA1_IT_GL1)
//     {
//         DMA1->IFCR = DMA1_IT_GL1;
//         if (dma1GL_CallbackFun[0] != nullptr)
//             dma1GL_CallbackFun[0]();
//     }
// }
#define DMA_IT_FUNC(NUM, CHANNEL, NAME)                       \
    if (DMA##NUM##->ISR & DMA##NUM##_IT_##NAME##CHANNEL##)    \
    {                                                         \
        DMA##NUM##->IFCR = DMA##NUM##_IT_##NAME##CHANNEL##;   \
        if (dma##NUM##NAME##_CallbackFun[NUM - 1] != nullptr) \
            dma##NUM##NAME##_CallbackFun[NUM - 1]();          \
    }

#define DMA_IRQ_FUNCTION(NUM, CHANNEL)                  \
    void DMA##NUM##_Channel##CHANNEL##_IRQHandler(void) \
    {                                                   \
        DMA_IT_FUNC(NUM, CHANNEL, TC)                   \
        DMA_IT_FUNC(NUM, CHANNEL, HT)                   \
        DMA_IT_FUNC(NUM, CHANNEL, TE)                   \
        DMA_IT_FUNC(NUM, CHANNEL, GL)                   \
    }

DMA_IRQ_FUNCTION(1, 1)
// DMA_IRQ_FUNCTION(1, 2)
DMA_IRQ_FUNCTION(1, 3)
DMA_IRQ_FUNCTION(1, 4)
DMA_IRQ_FUNCTION(1, 5)
DMA_IRQ_FUNCTION(1, 6)
DMA_IRQ_FUNCTION(1, 7)

DMA_IRQ_FUNCTION(2, 1)
DMA_IRQ_FUNCTION(2, 2)
DMA_IRQ_FUNCTION(2, 3)
DMA_IRQ_FUNCTION(2, 4)
DMA_IRQ_FUNCTION(2, 5)
