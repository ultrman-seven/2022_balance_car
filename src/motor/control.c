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

PID_paraTypdef speedPidLeft = {
    .Kp = 20, .Ki = 1, .Kd = 2, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef speedPidRight = {
    .Kp = 20, .Ki = 1, .Kd = 2, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef anglePid = {
    .Kp = 20, .Ki = 0, .Kd = 1, .integral = 0, .proportionLast = 0, .targetVal = -216};

void motorSetSpeed(MotorChoose motor, int32_t speed)
{
    if (motor == LEFT)
        speedPidLeft.targetVal = speed;
    else
        speedPidRight.targetVal = speed;
}

// int velocity(int encoder_left, int encoder_right)
// {
//     static float Velocity, Encoder_Least, Encoder, Movement;
//     static float Encoder_Integral;
//     //=============速度PI控制器=======================//
//     Encoder_Least = (encoder_left + encoder_right) - 0;
//     //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零）
//     Encoder *= 0.7;                 //===一阶低通滤波器
//     Encoder += Encoder_Least * 0.3; //===一阶低通滤波器
//     Encoder_Integral += Encoder;    //===积分出位移 积分时间：10ms
//     if (Encoder_Integral > 10000)
//         Encoder_Integral = 10000;
//     //===积分限幅
//     if (Encoder_Integral < -10000)
//         Encoder_Integral = -10000;
//     //===积分限幅
//     Velocity = Encoder * velocity_KP + Encoder_Integral * velocity_KI;
//     //===速度控制
//     if (pitch < -40 || pitch > 40)
//         Encoder_Integral = 0;
//     //===电机关闭后清除积分
//     return Velocity;
// }

// #define Motor_Speed_PWM_Proportion 1.2
int32_t pwmRight = 0, pwmLeft = 0;
// globalPidUpdate
void TIM16_IRQHandler(void)
{
    float pitch, y, r;
    Read_DMP(&pitch, &r, &y);
    speedPidLeft.targetVal = speedPidRight.targetVal = -pidCtrlUpdate(pitch * 10, &anglePid) / 10;
    // pwmLeft = pwmRight = -pidCtrlUpdate(pitch * 10, &anglePid) / 10;

    pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
    pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
    setPower(pwmLeft, LEFT);
    setPower(pwmRight, RIGHT);
    // setPower(pwmLeft + pidCtrlUpdate(getSpeed(LEFT), &speedPidLeft), LEFT);
    // setPower(pwmRight + pidCtrlUpdate(getSpeed(RIGHT), &speedPidRight), RIGHT);
    // setPower(Motor_Speed_PWM_Proportion * speedPidLeft.targetVal + (pidCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10), LEFT);
    // setPower(Motor_Speed_PWM_Proportion * speedPidRight.targetVal + (pidCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10), RIGHT);

    TIM_ClearITPendingBit(TIM16, TIM_FLAG_Update);
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
    nvic.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&nvic);

    TIM_ClearFlag(TIM16, TIM_FLAG_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM16, ENABLE);
}

void pidCtrlTimeInit(uint16_t us)
{
    time16Init(us, 95);
}
