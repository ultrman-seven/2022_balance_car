#include "common.h"
#include "wy_uart.hpp"
#include "wy_gpio.hpp"
#include "wy_nvic.hpp"
#include "wy_dma.hpp"
using namespace UART;

void (*uartRxFux[8])(void);

const uint32_t uartBase[8] = {UART1_BASE, UART2_BASE, UART3_BASE, UART4_BASE, UART5_BASE, UART6_BASE, UART7_BASE, UART8_BASE};
const uint8_t uartIRQ[8] = {UART1_IRQn, UART2_IRQn, UART3_IRQn, UART4_IRQn, UART5_IRQn, UART6_IRQn, UART7_IRQn, UART8_IRQn};
const uint32_t uartRccs[8] = {RCC_APB2Periph_UART1,
                              RCC_APB1Periph_UART2, RCC_APB1Periph_UART3, RCC_APB1ENR_UART4, RCC_APB1ENR_UART5,
                              RCC_APB2ENR_UART6,
                              RCC_APB1ENR_UART7, RCC_APB1ENR_UART8};

UART_Object::UART_Object(InitStruct &s)
{
    UART_InitTypeDef uart;

    uart.UART_BaudRate = s.bode;
    uart.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    uart.UART_Parity = UART_Parity_No;
    uart.UART_StopBits = UART_StopBits_1;
    uart.UART_WordLength = UART_WordLength_8b;
    uart.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    this->rxFun = uartRxFux + s.uartIdx - 1;
    this->uart = (UART_TypeDef *)(uartBase[s.uartIdx - 1]);
    this->nvicChannel = uartIRQ[s.uartIdx - 1];
    this->idx = s.uartIdx;
    switch (s.uartIdx)
    {
    case 1:
    case 6:
        RCC_APB2PeriphClockCmd(uartRccs[s.uartIdx - 1], ENABLE);
        break;

    default:
        RCC_APB1PeriphClockCmd(uartRccs[s.uartIdx - 1], ENABLE);
        break;
    }
    UART_Init(this->uart, &uart);
    UART_Cmd(this->uart, ENABLE);
    GPIO::afConfig(s.rx, s.RxAF, GPIO_Mode_IN_FLOATING);
    GPIO::afConfig(s.tx, s.TxAF, GPIO_Mode_AF_PP);
}

void UART_Object::sendByte(uint8_t dat)
{
    UART_SendData(this->uart, dat);
    while (!UART_GetFlagStatus(this->uart, UART_FLAG_TXEPT))
        ;
}
void UART_Object::sendByte(uint8_t *dat, uint8_t len)
{
    while (len--)
    {
        UART_SendData(this->uart, *dat++);
        while (!UART_GetFlagStatus(this->uart, UART_FLAG_TXEPT))
            ;
    }
}

void UART_Object::setNVIC(uint8_t priority, uint8_t sub_priority, bool open, void (*f)(void))
{
    NVIC_Init(this->nvicChannel, priority, sub_priority, open);
    UART_ITConfig(this->uart, UART_IT_RXIEN, ENABLE);
    *(this->rxFun) = f;
}

const dma::DMA_Source uart_rx_dma[] = {dma::DMA_UART1_RX, dma::DMA_UART2_RX, dma::DMA_UART3_RX, dma::DMA_UART4_RX, dma::DMA_UART5_RX, dma::DMA_UART6_RX, dma::DMA_UART7_RX, dma::DMA_UART8_RX};

void UART_Object::setDMA(uint32_t add, uint16_t size, void (*f)(void))
{
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_Priority = DMA_Priority_High;
    dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t) & (this->uart->RDR); //(uint32_t)&(mode == 't' ? this->uart->TDR : this->uart->RDR);
    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    dmaInit.DMA_BufferSize = size;
    dmaInit.DMA_M2M = DMA_M2M_Disable;
    dmaInit.DMA_MemoryBaseAddr = add;
    dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInit.DMA_Mode = DMA_Mode_Circular;
    dmaInit.DMA_Auto_reload = DMA_Auto_Reload_Disable;
    UART_DMACmd(this->uart, UART_DMAReq_EN, ENABLE);

    dma::configDMA(uart_rx_dma[this->idx - 1], &dmaInit, f);
}

#define UART_IT_FUNC(num, name)                                 \
    if (UART_GetITStatus(UART##num##, UART_IT_##name##) == SET) \
    {                                                           \
        if (uartRxFux[num - 1] != nullptr)                      \
            uartRxFux[num - 1]();                               \
        UART##num##->ICR = UART_IT_##name##;                    \
    }
// UART_ClearITPendingBit(UART##num##, UART_IT_##name##);  \

#define UART_IRQ_FUNC(num) \
    void UART##num##_IRQHandler(void) { UART_IT_FUNC(num, RXIEN) }

UART_IRQ_FUNC(1)
UART_IRQ_FUNC(2)
UART_IRQ_FUNC(3)
UART_IRQ_FUNC(4)
UART_IRQ_FUNC(5)
UART_IRQ_FUNC(6)
// UART_IRQ_FUNC(7)
UART_IRQ_FUNC(8)
