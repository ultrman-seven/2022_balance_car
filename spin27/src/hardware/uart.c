#include "common.h"
#include "stdio.h"
#include "oledio.h"
#include "motor/control.h"
#include "uart.h"

#define UART_RX_BUF_SIZE 15
uint8_t rxBuf[UART_RX_BUF_SIZE] = {0};

void uartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    nvic.NVIC_IRQChannel = UART2_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&nvic);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_3);

    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    // UART_InitStructure.UART_Mode = UART_Mode_Tx;

    UART_Init(UART2, &UART_InitStructure);
    UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void uart1SendWord(uint32_t dat)
{
    uint8_t i = 4;
    while (i--)
    {
        uart1SendByte((uint8_t)(dat >> 24));
        dat <<= 8;
    }
}
void uart1HalfWord(uint16_t dat)
{
    uart1SendByte((uint8_t)(dat >> 8));
    uart1SendByte((uint8_t)(dat & 0xff));
}

void uart1SendByte(uint8_t dat)
{
    UART_SendData(UART2, dat);
    while (!UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
        ;
}

void uart1SendBytes(uint8_t *dat, uint16_t len)
{
    while (len--)
    {
        UART_SendData(UART2, *dat++);
        while (!UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
            ;
    }
}

#define CMD_Start 0x97
#define CMD_END_0 0xf0
#define CMD_END_1 0xa5

typedef enum
{
    Hello,
    Get_Mpu6050,
    Get_Pic,
    Set_Mode,
    Para_Adj,
    Set_Angle,
    Set_Turn,
    Set_Balance,
    Get_Para
} UART_CMDs;

uint8_t *bufPointer = rxBuf;
uint8_t uartWaitCmdState = 1;
void cameraInit(void);
void cameraSetOn(void);
void UART2_IRQHandler(void)
{
    if (UART_GetITStatus(UART2, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
        if (bufPointer - rxBuf >= UART_RX_BUF_SIZE)
        {
            bufPointer = rxBuf;
            uartWaitCmdState = 1;
        }
        *bufPointer++ = UART_ReceiveData(UART2);

        if (uartWaitCmdState)
        {
            if (rxBuf[0] == CMD_Start)
                uartWaitCmdState = 0;
            else
                bufPointer = rxBuf;
        }
        else if (*(bufPointer - 1) == CMD_END_1 && *(bufPointer - 2) == CMD_END_0)
        {
            bufPointer = rxBuf;
            uartWaitCmdState = 1;
            switch (rxBuf[1])
            {
            case Hello:
                screenClear();
                OLED_print("Hello World!");
                printf("Hello World\n");
                break;
            case Get_Mpu6050:
                break;
            case Get_Pic:
                // cameraInit();
                cameraSetOn();
                break;
            case Set_Mode:
                setPidMode((enum ctrlModes)rxBuf[2]);
                break;
            case Para_Adj:
                adjustPara(rxBuf[2], rxBuf + 3);
                // printf("num:%d\n",rxBuf[2]);
                break;
            case Set_Angle:
                // screenClear();
                // OLED_printf("angle:%d", (int8_t)rxBuf[2]);
                setLinerSpeed((int8_t)rxBuf[2]);
                break;
            case Set_Turn:
                setAngularVelocity((int8_t)rxBuf[2]);
                break;
            case Set_Balance:
                setBalance(rxBuf + 2);
                break;
            case Get_Para:
                uart1SendByte(CMD_Start);
                uart1SendByte(Get_Para);
                uart1SendByte(rxBuf[2]);
                sendPara(rxBuf[2]);
                uart1SendByte(CMD_END_0);
                uart1SendByte(CMD_END_1);
                break;
            default:
                break;
            }
        }

        // UART_SendData(UART2, UART_ReceiveData(UART1));
        // while (!UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
        //     ;
        // UART_SendData(UART1, uartWaitCmdState);
        // while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        //     ;
    }
}
