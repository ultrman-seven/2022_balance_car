#include "common.h"
#include "encoder.h"
#include "oledio.h"
#include "motor/control.h"
#include "pid.h"

//毫秒
#define SPEED_COMPUT_PERIOD 5

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
    // TIM_Cmd(TIM17, DISABLE);
}
uint32_t timeLast;
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
    nvic.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&nvic);
    nvic.NVIC_IRQChannel = TIM8_BRK_UP_TRG_COM_IRQn;
    NVIC_Init(&nvic);

    TIM_SetCounter(TIM1, 0);
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
    time17Init(100 * SPEED_COMPUT_PERIOD, 960 - 1);
    getTimeStamp(&timeLast);
}

// 0:正向
// #define LeftDIR  0x80000000  //-
// #define RightDIR 0x00000000 //+
#define LeftDIR -1
#define RightDIR 1
int circleCount[2] = {0};
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        if ((TIM1->CR1 >> 4 & 0x01) == 0) // DIR==0
            circleCount[LEFT]++;
        else
            circleCount[LEFT]--;
    }
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

void TIM8_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM8, TIM_IT_Update) == SET)
    {
        if ((TIM8->CR1 >> 4 & 0x01) == 0) // DIR==0
            circleCount[RIGHT]++;
        else
            circleCount[RIGHT]--;
    }
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
}

int motorSpeed[2] = {0};
#define GearAndDiameter 3
#define TIM_Left TIM1
#define TIM_Right TIM8
int lastCNT[2] = {0};
// uint32_t ms = 0;
// int lastLCircle[2] = {0};
uint8_t timerFlag = 0;
void TIM17_IRQHandler(void)
{
    int32_t currintCnt;
    int speed;
    // uint32_t time;
    TIM_ClearITPendingBit(TIM17, TIM_FLAG_Update);
    timerFlag = 1;
    currintCnt = circleCount[LEFT] * 1024 * 4 + TIM_Left->CNT;
    // getTimeStamp(&time);
    // if (ms++ % 10 == 0)
        // printf("%d//////%d\n", time, timeLast);
        // printf("t1=%d,t2=%d,", time, timeLast);
        // printf("t1=%d\r\n", time- timeLast);
    // speed = (int32_t)HWDivider_Calc((uint32_t)(currintCnt - lastCNT[LEFT]) * GearAndDiameter, time - timeLast) * LeftDIR;
    // speed = (time - timeLast) * 10;
    speed = ((currintCnt - lastCNT[LEFT]) * GearAndDiameter) * LeftDIR;
    motorSpeed[LEFT] = (motorSpeed[LEFT] * 2 + speed * 8) / 10;
    // motorSpeed[LEFT] = speed;

    if (currintCnt > 4096)
    {
        circleCount[LEFT]--;
        lastCNT[LEFT] = currintCnt - 4096;
    }
    else if (currintCnt < -4096)
    {
        circleCount[LEFT]++;
        lastCNT[LEFT] = currintCnt + 4096;
    }
    else
    lastCNT[LEFT] = currintCnt;

    currintCnt = circleCount[RIGHT] * 1024 * 4 + TIM_Right->CNT;
    speed = ((currintCnt - lastCNT[RIGHT]) * GearAndDiameter) / RightDIR;
    motorSpeed[RIGHT] = (motorSpeed[RIGHT] * 2 + speed * 8) / 10;
    if (currintCnt > 4096)
    {
        circleCount[RIGHT]--;
        lastCNT[RIGHT] = currintCnt - 4096;
    }
    else if (currintCnt < -4096)
    {
        circleCount[RIGHT]++;
        lastCNT[RIGHT] = currintCnt + 4096;
    }
    else
        lastCNT[RIGHT] = currintCnt;
    // currentCNT = (lastLCircle[LEFT] - circleCount[LEFT]) * 1024 * 4 + TIM_Left->CNT;
    // lastLCircle[LEFT] = circleCount[LEFT];
    // motorSpeed[LEFT] = (currentCNT - lastCNT[LEFT]) * GearAndDiameter;
    // lastCNT[LEFT] = currentCNT;
    // motorSpeed[LEFT] = TIM_Left->CNT;
    // timeLast = time;

    pidUpdateFunction();
}

int getCircleCount(MotorChoose side)
{
    return circleCount[side];
}

int getSpeed(MotorChoose side)
{
    return motorSpeed[side] / SPEED_COMPUT_PERIOD;
}
