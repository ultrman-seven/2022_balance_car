#include "common.h"
#include "stdio.h"
#include "uart.h"

typedef enum
{
    INIT = 0,      // 摄像头初始化命令
    AUTO_EXP,      // 自动曝光命令
    EXP_TIME,      // 曝光时间命令
    FPS,           // 摄像头帧率命令
    SET_COL,       // 图像列命令
    SET_ROW,       // 图像行命令
    LR_OFFSET,     // 图像左右偏移命令
    UD_OFFSET,     // 图像上下偏移命令
    GAIN,          // 图像偏移命令
    CONFIG_FINISH, // 非命令位，主要用来占位计数

    COLOR_GET_WHO_AM_I = 0xEF,
    SET_EXP_TIME = 0XF0, // 单独设置曝光时间命令
    GET_STATUS,          // 获取摄像头配置命令
    GET_VERSION,         // 固件版本号命令

    SET_ADDR = 0XFE, // 寄存器地址命令
    SET_DATA         // 寄存器数据命令
} cameraCMD;

#define PIC_LINE 60
#define PIC_COL 94
struct
{
    uint8_t state;
    uint8_t pic[PIC_LINE * PIC_COL];
} picReceive = {.state = 0, .pic = {0}};

void __uart1Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannel = UART2_IRQn;
    nvic.NVIC_IRQChannelPriority = 2;
    NVIC_Init(&nvic);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_3); // tx
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_3); // rx

    UART_InitStructure.UART_BaudRate = 9600;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

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

void __DMA_init(void)
{
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    nvic.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 1;
    NVIC_Init(&nvic);

    dma.DMA_M2M = DISABLE;
    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_BufferSize = PIC_COL * PIC_LINE;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_MemoryBaseAddr = (uint32_t) & (picReceive.pic);
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralBaseAddr = (uint32_t) & (GPIOA->IDR);
    dma.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel5, &dma);

    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
}

void __TIM2PclkInit(void)
{
    TIM_ICInitTypeDef ic;
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    ic.TIM_Channel = TIM_Channel_1;
    ic.TIM_ICFilter = 0;
    ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2, &ic);

    TIM_Cmd(TIM2, ENABLE);
    TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
}

void UART2_IRQHandler(void)
{
    if (UART_GetITStatus(UART2, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
        // UART_SendData(UART1, UART_ReceiveData(UART2));
        // while (!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT))
        //     ;
    }
}

void __cameraSenaBytes(uint8_t *dat, uint8_t len)
{
    while (len--)
    {
        UART_SendData(UART2, *dat++);
        while (!UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
            ;
    }
}

void __cameraConfig(cameraCMD name, uint16_t val)
{
    uint8_t cmds[4] = {0xa5, name, val >> 8, val & 0x00ff};
    __cameraSenaBytes(cmds, 4);
    delayMs(5);
}

void cameraInit(void)
{
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;
    EXTI_InitTypeDef exti;
    __uart1Init();
    __cameraConfig(SET_COL,PIC_COL);
    __cameraConfig(SET_ROW,PIC_LINE);
    __cameraConfig(FPS,2);
    __cameraConfig(INIT,0);
    __DMA_init();
    __TIM2PclkInit();

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 0;
    nvic.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_Init(&nvic);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource5);

    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOD, &gpio);

    //接收数据
    gpio.GPIO_Pin = 0x00ff;
    GPIO_Init(GPIOA, &gpio);

    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_Line = EXTI_Line4 | EXTI_Line5;
    EXTI_Init(&exti);
}

void h_int(void)
{
}

uint8_t pic_FLAG_BYTES[8] = {0xff, 0xf0, 0xf0, 0xf0, 0x00, 0xff, 0x00, 0xff};
void v_int(void)
{
    if (picReceive.state)
    {
        __DMA_init();
        picReceive.state = 0;
    }
    else
    {
        // DMA1_Channel5->CNDTR = PIC_COL * PIC_LINE;
        DMA_Cmd(DMA1_Channel5, ENABLE);
        uart1SendBytes(pic_FLAG_BYTES, 8);
        picReceive.state = 1;
    }
}

void DMA1_Channel4_5_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC5);
        DMA_Cmd(DMA1_Channel5, DISABLE);
        uart1SendBytes(picReceive.pic, PIC_COL * PIC_LINE);
    }
}
