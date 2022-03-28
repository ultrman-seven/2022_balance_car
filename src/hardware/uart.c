#include "common.h"
#include "stdio.h"
#include "oledio.h"

#define UART_RX_BUF_SIZE 100
uint8_t rxBuf[UART_RX_BUF_SIZE] = {0};

void uartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    nvic.NVIC_IRQChannel = UART1_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 2;
    NVIC_Init(&nvic);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_0);

    UART_InitStructure.UART_BaudRate = 115200;
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

void uart1SendByte(uint8_t dat)
{
    UART_SendData(UART1, dat);
    while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        ;
}

void uart1SendBytes(uint8_t *dat, uint16_t len)
{
    while (len--)
    {
        UART_SendData(UART1, *dat++);
        while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
            ;
    }
}

uint8_t *bufPointer = rxBuf;
void UART1_IRQHandler(void)
{
    if (UART_GetITStatus(UART1, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        // screenClear();
        // OLED_printf("receive:%x\n",UART_ReceiveData(UART1));
        // if (bufPointer - rxBuf < UART_RX_BUF_SIZE)
        //     *bufPointer++ = UART_ReceiveData(UART1);
        // if (*(bufPointer - 1) == '\n')
        // {
        //     bufPointer = rxBuf;
        //     printf("杰宝是:");
        //     while (*bufPointer != '\n')
        //     {
        //         UART_SendData(UART1, *bufPointer++);
        //         while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        //             ;
        //     }
        //     bufPointer = rxBuf;
        // }
        // UART_SendData(UART2, UART_ReceiveData(UART1));
        // while (!UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
        //     ;
        // UART_SendData(UART1, 0xff);
        // while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        //     ;
    }
}
