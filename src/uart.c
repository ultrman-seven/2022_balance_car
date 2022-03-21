#include "common.h"
#include "stdio.h"

void uartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    nvic.NVIC_IRQChannel = UART1_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 2;
    NVIC_Init(&nvic);

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

#define UART_RX_BUF_SIZE 100
uint8_t rxBuf[UART_RX_BUF_SIZE] = {0};
uint8_t rxIndex = 0;
void UART1_IRQHandler(void)
{
    if (UART_GetITStatus(UART1, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        if (rxIndex < UART_RX_BUF_SIZE)
            rxBuf[rxIndex++] = UART_ReceiveData(UART1);
        if (rxBuf[rxIndex - 1] == '\n')
        {
            uint8_t i = 0;
            while (i < rxIndex)
            {
                UART_SendData(UART1, rxBuf[i++]);
                while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
                    ;
            }
            rxIndex = 0;
        }
        printf("杰宝\n");
    }
}
