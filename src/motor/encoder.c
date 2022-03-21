#include "common.h"
#include "encoder.h"
#include "oledio.h"

//毫秒
#define SPEED_COMPUT_PERIOD 10

//定时器17用于计算速度
void time17Init(uint16_t period, uint16_t prescaler)
{
    TIM_TimeBaseInitTypeDef time;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
    time.TIM_ClockDivision = TIM_CKD_DIV1;
    time.TIM_CounterMode = TIM_CounterMode_Up;
    time.TIM_RepetitionCounter = 0;
    time.TIM_Period = period;
    time.TIM_Prescaler = prescaler;
    TIM_TimeBaseInit(TIM17, &time);

    nvic.NVIC_IRQChannel = TIM17_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&nvic);

    TIM_ClearFlag(TIM17, TIM_FLAG_Update);
    TIM_ITConfig(TIM17, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM17, ENABLE);
}

void encoderInit(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef time;
    TIM_ICInitTypeDef ic;
    NVIC_InitTypeDef nvic;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOC, &gpio);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_6);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF_6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_6);

    time.TIM_ClockDivision = TIM_CKD_DIV1;
    time.TIM_CounterMode = TIM_CounterMode_Up;
    time.TIM_RepetitionCounter = 0;
    time.TIM_Prescaler = 0;
    time.TIM_Period = 1024 * 4 - 1;
    TIM_TimeBaseInit(TIM1, &time);
    TIM_TimeBaseInit(TIM8, &time);

    ic.TIM_Channel = TIM_Channel_1;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    ic.TIM_ICFilter = 0;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInit(TIM1, &ic);
    TIM_ICInit(TIM8, &ic);
    ic.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM1, &ic);
    TIM_ICInit(TIM8, &ic);

    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);

    nvic.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 1;
    NVIC_Init(&nvic);
    nvic.NVIC_IRQChannel = TIM8_BRK_UP_TRG_COM_IRQn;
    NVIC_Init(&nvic);

    TIM_SetCounter(TIM1, 0);
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
    time17Init(100 * SPEED_COMPUT_PERIOD, 960 - 1);
}

int circleCountLeft = 0;
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        if ((TIM1->CR1 >> 4 & 0x01) == 0) // DIR==0
            circleCountLeft++;
        else if ((TIM1->CR1 >> 4 & 0x01) == 1) // DIR==1
            circleCountLeft--;
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

int circleCountRight = 0;
void TIM8_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM8, TIM_IT_Update) == SET)
    {
        if ((TIM8->CR1 >> 4 & 0x01) == 0) // DIR==0
            circleCountRight++;
        else if ((TIM8->CR1 >> 4 & 0x01) == 1) // DIR==1
            circleCountRight--;
    }
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
}

int leftSpeed;
int rightSpeed;
#define GearAndDiameter 3
void TIM17_IRQHandler(void)
{
    int currentCNT;
    int speed;
    static int lastLeftCNT;
    static int lastRightCNT;
    static int lastLeftCir;
    static int lastRightCir;
    static uint8_t initFlag = 0;
    if (initFlag == 0)
    {
        initFlag++;
        lastLeftCNT = 0;
        lastRightCNT = 0;
        lastLeftCir = circleCountLeft;
        lastRightCir = circleCountRight;
    }

    //计算左轮速度
    currentCNT = (circleCountLeft - lastLeftCir) * (1024 * 4) + TIM1->CNT;
    // leftSpeed = (currentCNT - lastLeftCNT) / SPEED_COMPUT_PERIOD;
    speed = (currentCNT - lastLeftCNT) * GearAndDiameter;
    leftSpeed = (leftSpeed * 3 + speed * 7) / 10;
    lastLeftCNT = currentCNT;
    //计算右轮速度
    currentCNT = (circleCountRight - lastRightCir) * (1024 * 4) + TIM8->CNT;
    // rightSpeed = (currentCNT - lastRightCNT) / SPEED_COMPUT_PERIOD;
    speed = (currentCNT - lastRightCNT) * GearAndDiameter;
    rightSpeed = (rightSpeed * 3 + speed * 7) / 10;
    lastRightCNT = currentCNT;

    TIM_ClearITPendingBit(TIM17, TIM_FLAG_Update);
}

int getCircleCount(MotorChoose side)
{
    if (side == LEFT)
        return circleCountLeft;
    else
        return -circleCountRight;
}

int getSpeed(MotorChoose side)
{
    if (side == LEFT)
        return leftSpeed / SPEED_COMPUT_PERIOD;
    else
        return -rightSpeed / SPEED_COMPUT_PERIOD;
}
