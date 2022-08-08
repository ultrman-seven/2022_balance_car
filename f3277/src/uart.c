#include "uart.h"

#define getUART(n) UART##n
#define getUART_NVIC(n) UART##n##_IRQn
#define getUART_IRQ_F(n) UART##n##_IRQHandler

#define COM_UART getUART(8)
#define COM_UART_NVIC getUART_NVIC(8)
#define COM_UART_NVIC_IRQ_FUNCTION getUART_IRQ_F(8)
#define COM_UART_RCC_FUN RCC_APB1PeriphClockCmd
#define COM_UART_RCC RCC_APB1ENR_UART8

#define COM_RX_PIN_SOURCE GPIO_PinSource0
#define COM_RX_PIN (0x01 << COM_RX_PIN_SOURCE)
#define COM_RX_PORT GPIOE
#define COM_RX_AF GPIO_AF_8
#define COM_RX_RCC RCC_AHBPeriph_GPIOE

#define COM_TX_PIN_SOURCE GPIO_PinSource1
#define COM_TX_PIN (0x01 << COM_TX_PIN_SOURCE)
#define COM_TX_PORT GPIOE
#define COM_TX_AF GPIO_AF_8
#define COM_TX_RCC RCC_AHBPeriph_GPIOE

void computerUART_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(COM_UART_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(COM_RX_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(COM_TX_RCC, ENABLE);

    nvic.NVIC_IRQChannel = COM_UART_NVIC;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic);

    GPIO_PinAFConfig(COM_RX_PORT, COM_RX_PIN_SOURCE, COM_RX_AF);
    GPIO_PinAFConfig(COM_TX_PORT, COM_TX_PIN_SOURCE, COM_TX_AF);

    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(COM_UART, &UART_InitStructure);
    UART_ITConfig(COM_UART, UART_IT_RXIEN, ENABLE);
    UART_Cmd(COM_UART, ENABLE);

    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(COM_TX_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);
}

void computerSendByte(uint8_t dat)
{
    UART_SendData(COM_UART, dat);
    while (!UART_GetFlagStatus(COM_UART, UART_FLAG_TXEPT))
        ;
}

void computerSendBytes(uint8_t *dat, uint16_t len)
{
    while (len--)
    {
        UART_SendData(COM_UART, *dat++);
        while (!UART_GetFlagStatus(COM_UART, UART_FLAG_TXEPT))
            ;
    }
}

void COM_UART_NVIC_IRQ_FUNCTION(void)
{
    if (UART_GetITStatus(COM_UART, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(COM_UART, UART_IT_RXIEN);
        UART_ReceiveData(COM_UART);
    }
}
