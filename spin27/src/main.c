#include "common.h"
#include "oledio.h"
#include "font.h"
#include "beep.h"
#include "motor.h"
#include "encoder.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "iicsoft.h"
#include <stdio.h>
#include "uart.h"
#include "battery.h"

ErrorStatus HSE_SysClock(void)
{
    ErrorStatus HSE_StartUpState = ERROR;
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);

    HSE_StartUpState = RCC_WaitForHSEStartUp();
    if (HSE_StartUpState == SUCCESS)
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        //外部晶振8MHz，12倍频成96MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
            ;
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08)
            ;
    }
    return HSE_StartUpState;
}

void boardLED_Init(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_4);

    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_Prescaler = 95;
    tim.TIM_Period = 10000;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim);

    oc.TIM_OCIdleState = TIM_OCIdleState_Set;
    oc.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_OCNPolarity = TIM_OCNPolarity_High;
    oc.TIM_OutputState = ENABLE;
    oc.TIM_OutputNState = DISABLE;
    oc.TIM_Pulse = 5000;
    TIM_OC3Init(TIM2, &oc);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

ErrorStatus sysClkState = ERROR;
uint8_t mpuDmpState;
void variableListInit(void);
void globalInit(void)
{
    void menuInit(void);
    void communicateInit(void);

    sysClkState = HSE_SysClock();
    delayInit();
    boardLED_Init();
    uartInit();
    beepInit();
    menuInit();
    variableListInit();
    encoderInit();
    motorInit();
    MPU6050_initialize();
    mpuDmpState = DMP_Init();
    MPU6050_INT_Ini();
    // batteryInit();
    delayMs(1000);
    communicateInit();
}
#define DangerSpeed 1200

void __time17Init(uint16_t period, uint16_t prescaler)
{
    TIM_TimeBaseInitTypeDef time;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
    time.TIM_ClockDivision = TIM_CKD_DIV4;
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
extern float yawErr;
void TIM17_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM17, TIM_FLAG_Update);
    TIM_Cmd(TIM17, DISABLE);
    yawErr = MPU_yaw;
    beep100Ms();
}
// #define DangerSpeed 1500
void picProcess(void);
void testStop(void);
int main(void)
{
    int32_t ledBright = 0;
    globalInit();

    __time17Init(30000, 0xffff);
    // delayMs(2000);
    while (1)
    {
        // picProcess();
        (ledBright % 2) ? (ledBright -= 2) : (ledBright += 2);
        if (ledBright > 10000 || ledBright == 1)
            ledBright--;
        TIM2->CCR3 = ledBright;

        if (getSpeed(LEFT) >= DangerSpeed)
            // NVIC_SystemReset();
            testStop();
        if (getSpeed(LEFT) <= -DangerSpeed)
            // NVIC_SystemReset();
            testStop();
        // if (getSpeed(RIGHT) >= DangerSpeed)
        //     NVIC_SystemReset();
        // if (getSpeed(RIGHT) <= -DangerSpeed)
        //     NVIC_SystemReset();
        // printf("x%.2fy%.2f\n", position_x, position_y);
        // printf("v=%.2f\r\n", getVoltage());
        // printf("p=%.2f,r=%.2f,y=%.2f\r\n", MPU_pitch, MPU_roll, MPU_yaw);
        // printf("x=%d,y=%d,z=%d\r\n", gyro[0], gyro[1], gyro[2]);
        // printf("l=%d,r=%d\r\n", getSpeed(LEFT),getSpeed(RIGHT));
    }
}
// x:+-1500, y:2100
// 1: 1047,-232.69
// 2: 1028,-887.87
// 3: 917.95,-1576.82
// 4: -868.211,-1191.82
