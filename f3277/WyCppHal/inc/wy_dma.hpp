#ifndef __WY_LIB_CPP_DMA_H__
#define __WY_LIB_CPP_DMA_H__
#include "common.h"

namespace dma
{
    enum DMA_Source
    {
        DMA_ADC1,
        DMA_ADC2,
        DMA_ADC3,

        DMA_SPI1_RX,
        DMA_SPI1_TX,
        DMA_SPI2_RX,
        DMA_SPI2_TX,
        DMA_SPI3_RX,
        DMA_SPI3_TX,

        DMA_UART1_TX,
        DMA_UART1_RX,
        DMA_UART2_TX,
        DMA_UART2_RX,
        DMA_UART3_TX,
        DMA_UART3_RX,
        DMA_UART4_TX,
        DMA_UART4_RX,
        DMA_UART5_TX,
        DMA_UART5_RX,
        DMA_UART6_TX,
        DMA_UART6_RX,
        DMA_UART7_TX,
        DMA_UART7_RX,
        DMA_UART8_TX,
        DMA_UART8_RX,

        DMA_I2C1_TX,
        DMA_I2C1_RX,
        DMA_I2C2_TX,
        DMA_I2C2_RX,

        DMA_SDIO,

        DMA_TIM1_CC1,
        DMA_TIM1_CC2,
        DMA_TIM1_CC3,
        DMA_TIM1_CC4,
        DMA_TIM1_TRIG,
        DMA_TIM1_COM,
        DMA_TIM1_UP,

        DMA_TIM2_CC1,
        DMA_TIM2_CC2,
        DMA_TIM2_CC3,
        DMA_TIM2_CC4,
        DMA_TIM2_UP,

        DMA_TIM3_CC1,
        DMA_TIM3_CC3,
        DMA_TIM3_CC4,
        DMA_TIM3_TRIG,
        DMA_TIM3_UP,

        DMA_TIM4_CC1,
        DMA_TIM4_CC2,
        DMA_TIM4_CC3,
        DMA_TIM4_UP,

        DMA_TIM5_CC1,
        DMA_TIM5_CC2,
        DMA_TIM5_CC3,
        DMA_TIM5_CC4,
        DMA_TIM5_TRIG,
        DMA_TIM5_UP,

        DMA_TIM6_UP,
        DMA_TIM7_UP,
        DMA_TIM8_CC1,
        DMA_TIM8_CC2,
        DMA_TIM8_CC3,
        DMA_TIM8_CC4,
        DMA_TIM8_TRIG,
        DMA_TIM8_COM,
        DMA_TIM8_UP,

        DMA_DAC_CH1,
        DMA_DAC_CH2
    };

    DMA_Channel_TypeDef* configDMA(DMA_Source s, DMA_InitTypeDef *d, void (*callback)(void) = nullptr, uint8_t p = 0, uint8_t sp = 0);
}
#endif /* __WY_LIB_CPP_DMA_H__ */
