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
#include "uart.h"
#include <stdio.h>

int32_t balancePoint = -216;

PID_paraTypdef speedPidLeft = {
    .Kp = 25, .Ki = 25, .Kd = 7, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef speedPidRight = {
    .Kp = 25, .Ki = 25, .Kd = 7, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef anglePid = {
    .Kp = 60, .Ki = 5, .Kd = 24, .integral = 0, .proportionLast = 0, .targetVal = -286};

void motorSetSpeed(MotorChoose motor, int32_t speed)
{
    if (motor == LEFT)
        speedPidLeft.targetVal = speed;
    else
        speedPidRight.targetVal = speed;
}

enum
{
    pwmMode,    // pwm测试
    speedMode,  //速度环测试
    angleMode,  //角度环测试
    rockerMode  //老头环测试
} pidMode = 10; // pwmMode;

int32_t pidCtrlAngle(int32_t currentVal)
{
    int32_t proportion, diff;
    proportion = anglePid.targetVal - currentVal;

    anglePid.integral = anglePid.integral * 0.4 + proportion;
    // object->integral += proportion;
    if (anglePid.integral > PWM_PERIOD)
        anglePid.integral = PWM_PERIOD;
    if (anglePid.integral < -PWM_PERIOD)
        anglePid.integral = -PWM_PERIOD;

    diff = (gyro[1] + 40);
    anglePid.proportionLast = proportion;
    return (anglePid.Kp * proportion /* * 10*/ + anglePid.Ki * anglePid.integral + anglePid.Kd * diff / 10) / 10;
}

int32_t pwmRight = 0, pwmLeft = 0;
int32_t turnSpeed = 0;
int32_t linerSpeed = 0;
// globalPidUpdate
float lastPitch = 0.0;
// void TIM16_IRQHandler(void)
void pidUpdateFunction(void)
{
    float pitch, y, r;
    uint8_t waitErr = 8;
    // Read_DMP(&pitch, &r, &y);
    // while (pitch < 0.000001 && pitch > -0.000001 && waitErr)
    // {
    //     Read_DMP(&pitch, &r, &y);
    //     waitErr--;
    // }
    // if (waitErr)
    //     lastPitch = pitch;
    // else
    //     pitch = lastPitch;

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
        do
        {
            Read_DMP(&pitch, &r, &y);
        } while (pitch < 0.000001 && pitch > -0.000001);
        // speedPidLeft.targetVal = speedPidRight.targetVal = pidCtrlUpdate(pitch * 10, &anglePid) / 10;
        speedPidLeft.targetVal = speedPidRight.targetVal = pidCtrlAngle(pitch * 10) / 10;
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        float p = anglePid.targetVal / 10.0;
        // printf("p=%.1f,a=%.1f,r=%d\r\n", p, pitch,speedPidLeft.targetVal);
        break;
    case rockerMode:
        anglePid.targetVal = balancePoint + linerSpeed;
        speedPidLeft.targetVal = speedPidRight.targetVal = pidCtrlUpdate(pitch * 10, &anglePid) / 10;
        speedPidLeft.targetVal += turnSpeed;
        speedPidRight.targetVal -= turnSpeed;
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
    default:
        break;
    }

    TIM_ClearITPendingBit(TIM16, TIM_FLAG_Update);
}

void setPidMode(uint8_t m)
{
    pidMode = m;
}

void setLinerSpeed(int8_t speed)
{
    linerSpeed = speed;
}

void setAngularVelocity(int8_t speed)
{
    turnSpeed = speed;
}

void setBalance(uint8_t *dat)
{
    uint32_t tmp = 0;
    uint8_t i = 4;
    while (i--)
    {
        tmp <<= 8;
        tmp += *dat++;
    }
    balancePoint = (int32_t)tmp;
}

typedef enum
{
    g_balance,
    g_anglePid,
    g_leftPid,
    g_rightPid,
    g_turn,
    g_liner
} SendParaTypdef;
void sendPara(uint8_t p)
{
    switch (p)
    {
    case g_balance:
        uart1SendWord((uint32_t)balancePoint);
        break;
    case g_anglePid:
        uart1HalfWord((uint16_t)(anglePid.Kp));
        uart1HalfWord((uint16_t)(anglePid.Ki));
        uart1HalfWord((uint16_t)(anglePid.Kd));
        break;
    case g_leftPid:
        uart1HalfWord((uint16_t)(speedPidLeft.Kp));
        uart1HalfWord((uint16_t)(speedPidLeft.Ki));
        uart1HalfWord((uint16_t)(speedPidLeft.Kd));
        break;
    case g_rightPid:
        uart1HalfWord((uint16_t)(speedPidRight.Kp));
        uart1HalfWord((uint16_t)(speedPidRight.Ki));
        uart1HalfWord((uint16_t)(speedPidRight.Kd));
        break;
    case g_turn:
        uart1SendWord((uint32_t)turnSpeed);
        break;
    case g_liner:
        break;
    default:
        break;
    }
}

PID_paraTypdef *pidList[] = {&anglePid, &speedPidLeft, &speedPidRight};
void adjustPara(uint8_t num, uint8_t *pidPara)
{
    uint16_t tmp;

    tmp = *pidPara++;
    tmp <<= 8;
    tmp += *pidPara++;
    pidList[num]->Kp = (int16_t)tmp;

    tmp = *pidPara++;
    tmp <<= 8;
    tmp += *pidPara++;
    pidList[num]->Ki = (int16_t)tmp;

    tmp = *pidPara++;
    tmp <<= 8;
    tmp += *pidPara++;
    pidList[num]->Kd = (int16_t)tmp;
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
    // time16Init(us, 95);
    // TIM_Cmd(TIM17,ENABLE);
    pidMode = speedMode;
}
