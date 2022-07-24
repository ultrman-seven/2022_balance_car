#include "common.h"
#include "camera.h"

#define getUART(n) UART##n
#define getUART_NVIC(n) UART##n##_IRQn
#define getUART_IRQ_F(n) UART##n##_IRQHandler

#define CAM_UART getUART(7)
#define CAM_UART_NVIC getUART_NVIC(7)
#define CAM_UART_NVIC_IRQ_FUNCTION getUART_IRQ_F(7)
#define CAM_UART_RCC_FUN RCC_APB1PeriphClockCmd
#define CAM_UART_RCC RCC_APB1ENR_UART7

#define CAM_RX_PIN_SOURCE GPIO_PinSource7
#define CAM_RX_PIN (0x01 << CAM_RX_PIN_SOURCE)
#define CAM_RX_PORT GPIOE
#define CAM_RX_AF GPIO_AF_8
#define CAM_RX_RCC RCC_AHBPeriph_GPIOE

#define CAM_TX_PIN_SOURCE GPIO_PinSource8
#define CAM_TX_PIN (0x01 << CAM_TX_PIN_SOURCE)
#define CAM_TX_PORT GPIOE
#define CAM_TX_AF GPIO_AF_8
#define CAM_TX_RCC RCC_AHBPeriph_GPIOE

#define _DBG_ 1
void __cam_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef nvic;

    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannel = CAM_UART_NVIC;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);

    CAM_UART_RCC_FUN(CAM_UART_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(CAM_TX_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(CAM_RX_RCC, ENABLE);

    GPIO_PinAFConfig(CAM_TX_PORT, CAM_TX_PIN_SOURCE, CAM_TX_AF); // tx
    GPIO_PinAFConfig(CAM_RX_PORT, CAM_RX_PIN_SOURCE, CAM_RX_AF); // rx

    UART_InitStructure.UART_BaudRate = 9600;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(CAM_UART, &UART_InitStructure);

    UART_ITConfig(CAM_UART, UART_IT_RXIEN, ENABLE);

    UART_Cmd(CAM_UART, ENABLE);

    GPIO_InitStructure.GPIO_Pin = CAM_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(CAM_TX_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CAM_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(CAM_RX_PORT, &GPIO_InitStructure);
}

void CAM_UART_NVIC_IRQ_FUNCTION(void)
{
    if (UART_GetITStatus(CAM_UART, UART_IT_RXIEN) == SET)
    {
        UART_ClearITPendingBit(CAM_UART, UART_IT_RXIEN);
        UART_ReceiveData(CAM_UART);
    }
}

#define getDMA_NVIC(dma, ch) DMA##dma##_Channel##ch##_IRQn
#define getDMA_CHANNEL(dma, ch) DMA##dma##_ch##ch
#define getDMA_IRQ_F(dma, ch) DMA##dma##_Channel##ch##_IRQHandler
#define getDMA_IT(dma, ch) DMA##dma##_IT_TC##ch

#define CAM_DMA_RCC RCC_AHBPeriph_DMA1
#define CAM_DMA_CHANNEL getDMA_CHANNEL(1, 2)
#define CAM_DMA_NVIC getDMA_NVIC(1, 2)
#define CAM_DMA_NVIC_FUNCTION getDMA_IRQ_F(1, 2)
#define CAM_DMA_IT getDMA_IT(1, 2)

struct
{
    uint8_t state;
    uint8_t pic[PIC_LINE * PIC_COL];
} picReceive = {.state = 0, .pic = {0}};

#define CAM_DATA_PORT GPIOA
#define CAM_DATA_RCC RCC_AHBPeriph_GPIOA
void __cam_dma_init(void)
{
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    RCC_AHBPeriphClockCmd(CAM_DMA_RCC, ENABLE);

    nvic.NVIC_IRQChannel = CAM_DMA_NVIC;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic);

    dma.DMA_M2M = DMA_M2M_Disable;
    dma.DMA_Mode = DMA_Mode_Normal;
    // dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_BufferSize = PIC_COL * PIC_LINE;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_MemoryBaseAddr = (uint32_t) & (picReceive.pic);
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralBaseAddr = (uint32_t) & (CAM_DATA_PORT->IDR);
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_Auto_reload = DMA_Auto_Reload_Enable;
    DMA_Init(CAM_DMA_CHANNEL, &dma);

    DMA_ITConfig(CAM_DMA_CHANNEL, DMA_IT_TC, ENABLE);
    DMA_Cmd(CAM_DMA_CHANNEL, DISABLE);
}

#define getTIM(n) TIM##n
#define getTIM_Channel(n) TIM_Channel_##n
#define getTIM_DMA_Channel(n) TIM_DMA_CC##n

#define CAM_TIM getTIM(3)
#define CAM_TIM_CHANNEL getTIM_Channel(3)
#define CAM_TIM_DMA_CHANNEL getTIM_DMA_Channel(3)

#define CAM_TIM_PORT GPIOB
#define CAM_TIM_PIN_SOURCE GPIO_PinSource0
#define CAM_TIM_PIN (0x01 << CAM_TIM_PIN_SOURCE)
#define CAM_TIM_AF GPIO_AF_2
#define CAM_TIM_PIN_RCC RCC_AHBPeriph_GPIOB
#define CAM_TIM_RCC RCC_APB1Periph_TIM3
#define CAM_TIM_RCC_FUNCTION RCC_APB1PeriphClockCmd

void __cam_tim_init(void)
{
    TIM_ICInitTypeDef ic;
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(CAM_TIM_PIN_RCC, ENABLE);
    CAM_TIM_RCC_FUNCTION(CAM_TIM_RCC, ENABLE);

    GPIO_PinAFConfig(CAM_TIM_PORT, CAM_TIM_PIN_SOURCE, CAM_TIM_AF);

    gpio.GPIO_Pin = CAM_TIM_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAM_TIM_PORT, &gpio);

    ic.TIM_Channel = CAM_TIM_CHANNEL;
    ic.TIM_ICFilter = 0;
    ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(CAM_TIM, &ic);

    TIM_Cmd(CAM_TIM, ENABLE);
    TIM_DMACmd(CAM_TIM, CAM_TIM_DMA_CHANNEL, ENABLE);
}

void __cameraSenaBytes(uint8_t *dat, uint8_t len)
{
    while (len--)
    {
        UART_SendData(CAM_UART, *dat++);
        while (!UART_GetFlagStatus(CAM_UART, UART_FLAG_TXEPT))
            ;
    }
}

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
void __cameraConfig(cameraCMD name, uint16_t val)
{
    uint8_t cmds[4] = {0xa5, name, val >> 8, val & 0x00ff};
    __cameraSenaBytes(cmds, 4);
    delayMs(5);
}

#define CAM_V_INT_PORT GPIOC
#define CAM_V_INT_PIN_SOURCE GPIO_PinSource5
#define CAM_V_INT_NVIC EXTI9_5_IRQn
#define CAM_V_INT_RCC RCC_AHBPeriph_GPIOC
#define CAM_V_INT_EXTI_PORT EXTI_PortSourceGPIOC

#define CAM_V_INT_PIN (0x01 << CAM_V_INT_PIN_SOURCE)
#define CAM_V_INT_EXTI_PIN_SOURCE CAM_V_INT_PIN_SOURCE
#define CAM_V_INT_EXTI_LINE CAM_V_INT_PIN

void setExtiCallbackFunction(uint8_t line, void (*f)(void));
void v_int(void)
{
    if (picReceive.state)
        DMA_Cmd(CAM_DMA_CHANNEL, ENABLE);
}
void cameraInit(void)
{
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;
    EXTI_InitTypeDef exti;
    __cam_uart_init();
    __cameraConfig(SET_COL, PIC_COL);
    __cameraConfig(SET_ROW, PIC_LINE);
    __cameraConfig(EXP_TIME, 400);
    __cameraConfig(FPS, _DBG_ ? 30 : 25);
    __cameraConfig(INIT, 0);
    __cam_dma_init();
    __cam_tim_init();

    RCC_AHBPeriphClockCmd(CAM_DATA_RCC, ENABLE);
    RCC_AHBPeriphClockCmd(CAM_V_INT_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannel = CAM_V_INT_NVIC;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic);

    SYSCFG_EXTILineConfig(CAM_V_INT_EXTI_PORT, CAM_V_INT_EXTI_PIN_SOURCE);

    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Pin = CAM_V_INT_PIN;
    GPIO_Init(CAM_V_INT_PORT, &gpio);

    //接收数据
    gpio.GPIO_Pin = 0x00ff;
    GPIO_Init(CAM_DATA_PORT, &gpio);

    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_Line = CAM_V_INT_EXTI_LINE;
    EXTI_Init(&exti);
    setExtiCallbackFunction(CAM_V_INT_EXTI_PIN_SOURCE, v_int);
}
uint8_t cameraFlag = 0;
void cameraOn(void)
{

    cameraInit();
    cameraFlag = 1;
    picReceive.state = 1;
}
void cameraOff(void)
{
    cameraFlag = 0;
}

uint8_t DMA_ok_flag = 0;
void CAM_DMA_NVIC_FUNCTION(void)
{
    if (DMA_GetITStatus(CAM_DMA_IT) == SET)
    {
        DMA_Cmd(CAM_DMA_CHANNEL, DISABLE);
        DMA_ok_flag = 1;
        // cameraFlag = 0;
        picReceive.state = 0;
        DMA_ClearITPendingBit(CAM_DMA_IT);
    }
}

void imgGray2Bin(uint8_t *img, uint16_t l, uint16_t c);
uint16_t findMax(uint8_t *img, uint16_t height, uint16_t width);
uint8_t OTSU(uint8_t *image, uint16_t IMAGE_H, uint16_t IMAGE_W);
uint16_t BFS(uint8_t *image, uint16_t IMAGE_H, uint16_t IMAGE_W);
uint32_t findPointCenter(uint8_t *img, uint16_t l, uint16_t c);
uint8_t *gaussianFilter(uint8_t *img, uint16_t l, uint16_t c);
void LED_flip(void);
void fuck_zaoDian(uint8_t *img, uint16_t l, uint16_t c);
point findLamp(void);
void twoPass(uint8_t *img, uint16_t l, uint16_t c);
void gaussianFilterFast(uint8_t *img, uint16_t l, uint16_t c);
#include "stdio.h"
#include "string.h"

// int16_t camResult = PIC_COL / 2;
point camResult = {0, 0};

extern uint8_t pic_cp[PIC_COL * PIC_LINE];
uint8_t cnt;
#define Wait_Time 25
void cameraPicOption(void)
{
    if (DMA_ok_flag)
    {
        uint16_t tmp;
        uint16_t maxIdx, t;
        int16_t lamp_x;
        LED_flip();
        memset(picReceive.pic, 0x00, PIC_CUT);
        // fuck_zaoDian(picReceive.pic, PIC_LINE, PIC_COL);
        // for (tmp = PIC_CUT; tmp < PIC_COL * PIC_LINE; tmp++)
        //     pic_cp[tmp] = picReceive.pic[tmp];
        // gaussianFilter(picReceive.pic + PIC_CUT, PIC_LINE - PIC_CUT_LINE, PIC_COL);
        gaussianFilterFast(picReceive.pic + PIC_CUT, PIC_LINE - PIC_CUT_LINE, PIC_COL);
        // maxIdx = findMax(picReceive.pic, PIC_LINE, PIC_COL);
        t = OTSU(picReceive.pic + PIC_CUT, PIC_LINE - PIC_CUT_LINE, PIC_COL);
        // if (picReceive.pic[maxIdx] <= 2 && t <= 1)
        //     goto no_lamp;
        imgGray2Bin(picReceive.pic, PIC_LINE, PIC_COL);
        twoPass(picReceive.pic, PIC_LINE, PIC_COL);
        // camResult = (findPointCenter(picReceive.pic, PIC_LINE, PIC_COL) & 0x00ff) - PIC_COL / 2;
        // camResult = BFS(picReceive.pic, PIC_LINE, PIC_COL) - PIC_COL / 2;

#if (!_DBG_)
        lamp_x = findLamp();
        if (lamp_x)
        {
            cnt = Wait_Time;
            camResult = lamp_x - PIC_COL / 2;
        }
        else
        {
        no_lamp:
            if (camResult != PIC_COL / 2)
            {
                if (cnt == Wait_Time)
                    camResult = -(camResult * 2);
                if (!cnt--)
                    camResult = PIC_COL / 2;
            }
        }
#else
        camResult = findLamp();
        // if (camResult)
        //     camResult -= PIC_COL / 2;
        // else
        // {
        // no_lamp:
        //     camResult = PIC_COL / 2;
        // }
#endif

        if (cameraFlag)
            picReceive.state = 1;
        DMA_ok_flag = 0;
        return;
    no_lamp:
        camResult.x = camResult.y = 0;
        if (cameraFlag)
            picReceive.state = 1;
        DMA_ok_flag = 0;
    }
}
