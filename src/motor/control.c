/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 *
 *
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *            佛祖保佑     永不宕机     永无BUG
 */

#include "common.h"
#include "motor.h"
#include "pid.h"
#include "mpu6050.h"
#include <stdio.h>

PID_paraTypdef speedPidLeft = {
    .Kp = 15, .Ki = 20, .Kd = 6, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef speedPidRight = {
    .Kp = 15, .Ki = 20, .Kd = 6, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef anglePid = {
    .Kp = 20, .Ki = 0, .Kd = 1, .integral = 0, .proportionLast = 0, .targetVal = -216};

void motorSetSpeed(MotorChoose motor, int32_t speed)
{
    if (motor == LEFT)
        speedPidLeft.targetVal = speed;
    else
        speedPidRight.targetVal = speed;
}

enum
{
    pwmMode,
    speedMode,
    angleMode
} pidMode = pwmMode;

int32_t pwmRight = 0, pwmLeft = 0;
// globalPidUpdate
float lastPitch = 0.0;
void TIM16_IRQHandler(void)
{
    float pitch, y, r;
    uint8_t waitErr = 8;
    Read_DMP(&pitch, &r, &y);
    while (pitch < 0.000001 && pitch > -0.000001 && waitErr)
    {
        Read_DMP(&pitch, &r, &y);
        waitErr--;
    }
    if (waitErr)
        lastPitch = pitch;
    else
        pitch = lastPitch;

    switch (pidMode)
    {
    case pwmMode:

        pwmLeft = speedPidLeft.targetVal;
        pwmRight = speedPidRight.targetVal;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        printf("r=%d,c=%d\r\n", speedPidLeft.targetVal, getSpeed(LEFT));
        break;
    case speedMode:
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        printf("r=%d,c=%d\r\n", speedPidLeft.targetVal, getSpeed(LEFT));
        break;
    case angleMode:
        speedPidLeft.targetVal = speedPidRight.targetVal = pidCtrlUpdate(pitch * 10, &anglePid) / 10;
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        float p = anglePid.targetVal / 10.0;
        printf("p=%f,a=%f\r\n", p, pitch);
        break;
    default:
        break;
    }

    TIM_ClearITPendingBit(TIM16, TIM_FLAG_Update);
}

void setPidMode(uint8_t m)
{
    pidMode = m;
}

PID_paraTypdef *pidList[] = {&anglePid, &speedPidLeft, &speedPidRight};
void adjustPara(uint8_t num, uint8_t *pidPara)
{
    pidList[num]->Kp = ((uint16_t)(*pidPara)) << 8 + *(pidPara + 1);
    pidPara += 2;
    pidList[num]->Ki = ((uint16_t)(*pidPara)) << 8 + *(pidPara + 1);
    pidPara += 2;
    pidList[num]->Kd = ((uint16_t)(*pidPara)) << 8 + *(pidPara + 1);
}

//定时器16用于更新pid控制
void time16Init(uint16_t period, uint16_t prescaler)
{
    TIM_TimeBaseInitTypeDef time;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
    time.TIM_ClockDivision = TIM_CKD_DIV1;
    time.TIM_CounterMode = TIM_CounterMode_Up;
    time.TIM_RepetitionCounter = 0;
    time.TIM_Period = period;
    time.TIM_Prescaler = prescaler;
    TIM_TimeBaseInit(TIM16, &time);

    nvic.NVIC_IRQChannel = TIM16_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 1;
    NVIC_Init(&nvic);

    TIM_ClearFlag(TIM16, TIM_FLAG_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM16, ENABLE);
}

void pidCtrlTimeInit(uint16_t us)
{
    time16Init(us, 95);
}
