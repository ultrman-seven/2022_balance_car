#include "common.h"

void cameraInit(void)
{
    // GPIO_InitTypeDef gpio;
    // UART_InitTypeDef uart;

    // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    // RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);

    // GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_3); // tx
    // GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_3); // rx

    // gpio.GPIO_Pin = GPIO_Pin_4;
    // gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    // gpio.GPIO_Mode = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOC, &gpio);

    // gpio.GPIO_Pin = GPIO_Pin_5;
    // gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_Init(GPIOC, &gpio);

    // uart.UART_WordLength = UART_WordLength_8b;
    // uart.UART_BaudRate = 9600;
    // uart.UART_StopBits = UART_StopBits_1;
    // uart.UART_Parity = UART_Parity_No;
    // uart.UART_Mode = UART_Mode_Tx;
    // uart.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    // UART_Init(UART2, &uart);
    // UART_Cmd(UART2, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_0);

    UART_InitStructure.UART_BaudRate = 9600;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(UART1, &UART_InitStructure);

    UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);

    UART_Cmd(UART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
