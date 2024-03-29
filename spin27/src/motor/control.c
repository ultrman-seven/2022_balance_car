﻿/*
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
#include "motor/control.h"
#include "quickSin.h"
#include "mpu6050/filter.h"
#include "oledio.h"
#include "hardware/beep.h"
#include "menu.h"
#include "locate.h"
#include "stdlib.h"

// int32_t balancePoint = 95;
int32_t balancePoint = 55;
// int32_t balancePoint = 190;
// int32_t balancePoint = 110;
// int32_t balancePoint = 130;
int32_t baseSpeed = 0;
enum ctrlModes pidMode = NullMode; // pwmMode;

PID_paraTypdef speedPidLeft = {
    .Kp = 25, .Ki = 25, .Kd = 7, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
PID_paraTypdef speedPidRight = {
    .Kp = 25, .Ki = 25, .Kd = 7, .integral = 0, .proportionLast = 0, .proportionLastLast = 0, .targetVal = 0};
// .Kp = 60, .Ki = 5, .Kd = 24, .integral = 0, .proportionLast = 0, .targetVal = -286};
PID_paraTypdef anglePid = {
    .Kp = 105, .Ki = 35, .Kd = 40, .integral = 0, .proportionLast = 0, .targetVal = -286};
// 105,3.5,4.0
PID_paraTypdef picTurn = {
    .Kp = 3, .Ki = 2, .Kd = 6, .targetVal = 0, .integral = 0, .proportionLast = 0, .proportionLastLast = 0};
PID_paraTypdef findAnglePid = {
    .Kp = 3, .Ki = 1, .Kd = 1, .targetVal = 0, .integral = 0, .proportionLast = 0, .proportionLastLast = 0};
PID_paraTypdef speedCtrlPid = {
    .Kp = 3, .Ki = 1, .Kd = 1, .targetVal = 20, .integral = 0, .proportionLast = 0, .proportionLastLast = 0};

PID_paraTypdef accPidLeft = {
    .Kp = 3, .Ki = 7, .Kd = 3, .targetVal = 0, .integral = 0, .proportionLast = 0, .proportionLastLast = 0};
PID_paraTypdef accPidRight = {
    .Kp = 3, .Ki = 7, .Kd = 3, .targetVal = 0, .integral = 0, .proportionLast = 0, .proportionLastLast = 0};

void motorSetSpeed(MotorChoose motor, int32_t speed)
{
    baseSpeed = speed;
    if (motor == LEFT)
        speedPidLeft.targetVal = speed;
    else
        speedPidRight.targetVal = speed;
}

void setBaseSpeed(int32_t s)
{
    baseSpeed = s;
}
// PID_paraTypdef ph_car_home_anglePid = {
//     .Kp = 180, .Kd = 60, .Ki = 0, .targetVal = 0, .proportionLast = 0};
// PID_paraTypdef ph_car_home_anglePid = {
//     .Kp = 160, .Kd = 60, .Ki = 0, .targetVal = 0, .proportionLast = 0};
PID_paraTypdef ph_car_home_anglePid = {
    .Kp = 150, .Kd = 79, .Ki = 0, .targetVal = 0, .proportionLast = 0};
// PID_paraTypdef ph_car_home_anglePid = {
//     .Kp = 159, .Kd = 75, .Ki = 0, .targetVal = 0, .proportionLast = 0};
PID_paraTypdef ph_car_home_speedPid = {
    .Kp = 25, .Ki = 4, .Kd = 0, .targetVal = 0};

float gyro_y = 0.0;
int ph_car_home_balance(float Angle, float Gyro, PID_paraTypdef *p)
{
    float Bias; //, kp = 300, kd = 1;
    int balance;
    gyro_y = -Gyro;
    Bias = (float)(p->targetVal) / 10.0 - Angle; //===求出平衡的角度中值 和机械相关
    balance = (p->Kp) * Bias + (gyro_y * (p->Kd)) / 50.0;
    return balance;
}
float Target_Velocity;
float Encoder_Integral;
/**
 * @description:
 * @param {int} speed_left
 * @param {int} speed_right
 * @return pwm
 */
int ph_car_home_velocity(int speed_left, int speed_right, PID_paraTypdef *p)
{
    static float Velocity, Encoder_Least, Encoder; //, Movement = 0;
    // float kp = 80, ki = 0.4;
    //=============遥控前进后退部分=======================//
    // if (Bi_zhang == 1 && Flag_sudu == 1)
    // 	Target_Velocity = 45; //如果进入避障模式,自动进入低速模式
    // else
    // 	Target_Velocity = 90;
    // if (1 == Flag_Qian)
    // 	Movement = Target_Velocity / Flag_sudu; //===前进标志位置1
    // else if (1 == Flag_Hou)
    // 	Movement = -Target_Velocity / Flag_sudu; //===后退标志位置1
    // else
    // 	Movement = 0;
    // if (Bi_zhang == 1 && Distance < 500 && Flag_Left != 1 && Flag_Right != 1) //避障标志位置1且非遥控转弯的时候，进入避障模式
    // 	Movement = -Target_Velocity / Flag_sudu;
    //=============速度PI控制器=======================//
    Encoder_Least = (p->targetVal) - (speed_left + speed_right) / 2.0; //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零）
    Encoder *= 0.8;                                                    //===一阶低通滤波器
    Encoder += Encoder_Least * 0.2;                                    //===一阶低通滤波器
    Encoder_Integral += Encoder;                                       //===积分出位移 积分时间：10ms
    // Encoder_Integral = Encoder_Integral; //===接收遥控器数据，控制前进后退
    // Encoder_Integral *= 0.64;
    Encoder_Integral *= 0.7;
    if (Encoder_Integral > 10000)
        Encoder_Integral = 10000; //===积分限幅
    if (Encoder_Integral < -10000)
        Encoder_Integral = -10000;                                      //===积分限幅
    Velocity = Encoder * (p->Kp) + (Encoder_Integral * (p->Ki)) / 10.0; //===速度控制
    // if (Turn_Off(Angle_Balance, Voltage) == 1 || Flag_Stop == 1)
    // 	Encoder_Integral = 0; //===电机关闭后清除积分
    return Velocity / 10.0;
    // return Velocity;
}

PID_paraTypdef ph_car_home_speedPid_left = {
    .Kp = 120, .Ki = 7, .Kd = 0, .targetVal = 0, .integral = 0.0, .proportionLast = 0.0};
// PID_paraTypdef ph_car_home_speedPid_left = {
//     .Kp = 80, .Ki = 4, .Kd = 0, .targetVal = 0, .integral = 0.0, .proportionLast = 0.0};
PID_paraTypdef ph_car_home_speedPid_right = {
    .Kp = 25, .Ki = 4, .Kd = 0, .targetVal = 0, .integral = 0.0, .proportionLast = 0.0};

int ph_car_home_OneWheelVelocity(int speed_left, PID_paraTypdef *p)
{
    float Velocity, Encoder_Least;            //, Encoder; //, Movement = 0;
    Encoder_Least = 0 - speed_left;           //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零）
    p->proportionLast *= 0.8;                 //===一阶低通滤波器
    p->proportionLast += Encoder_Least * 0.2; //===一阶低通滤波器
    p->integral += p->proportionLast;         //===积分出位移 积分时间：10ms
    p->integral += (p->targetVal);            //===接收遥控器数据，控制前进后退
    if (p->integral > 10000)
        p->integral = 10000; //===积分限幅
    if (p->integral < -10000)
        p->integral = -10000;                                                  //===积分限幅
    Velocity = (p->proportionLast) * (p->Kp) + (p->integral * (p->Ki)) / 10.0; //===速度控制
    return Velocity / 10.0;
}
uint32_t myabs(int32_t val)
{
    if (val >= 0)
        return val;
    else
        return -val;
}

uint8_t Flag_Left = 0, Flag_Right = 0;
int ph_car_home_turn(int encoder_left, int encoder_right, float gyro) //转向控制
{
    static float Turn_Target, Turn, Encoder_temp, Turn_Convert = 0.49, Turn_Count;
    float Turn_Amplitude = 20, Kp = 10, Kd = 0;

    if (1 == Flag_Left || 1 == Flag_Right) //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
    {
        if (++Turn_Count == 1)
            Encoder_temp = myabs(encoder_left + encoder_right);
        Turn_Convert = 50 / Encoder_temp;

        if (Turn_Convert < 0.6)
            Turn_Convert = 0.6;
        if (Turn_Convert > 3)
            Turn_Convert = 3;
    }
    else
    {
        Turn_Convert = 0.9;
        Turn_Count = 0;
        Encoder_temp = 0;
        Turn_Target = 0;
        // return 0;
    }

    if (1 == Flag_Left)
        Turn_Target -= Turn_Convert;
    else if (1 == Flag_Right)
        Turn_Target += Turn_Convert;
    else
        Turn_Target = 0;

    if (Turn_Target > Turn_Amplitude)
        Turn_Target = Turn_Amplitude; //===转向速度限幅
    if (Turn_Target < -Turn_Amplitude)
        Turn_Target = -Turn_Amplitude;
    if (baseSpeed)
    {
        Kd = 0.5;
        Kp = 0;
    }
    else
    {
        Kd = 0;
        Kp = 10;
    }                                     //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
                                          //=============转向PD控制器=======================//
    Turn = -Turn_Target * Kp - gyro * Kd; //===结合Z轴陀螺仪进行PD控制
    return Turn / 50.0;
}

#define F_NUM 0.08
float getBellFunc(int val)
{
    if (val > -10 && val < 10)
        return 1;
    if (val > 0)
        val = -val;
    return (exp(F_NUM * val) - 1) / (exp(F_NUM * val) + 1) + 1;
}

// PID_paraTypdef turnPid = {.Kp = 10, .Kd = 2, .Ki = 0, .targetVal = 64};
PID_paraTypdef turnPid = {.Kp = 100, .Kd = 12, .Ki = 18, .targetVal = 0, .integral = 0, .proportionLast = 0};
// int imgPosition = 64;
point imgPosition = {0, 0};
point imgLastPosition = {0, 0};
int16_t img_x = 0;
int32_t turnMul = 8;
int cam_turn(int val, int16_t gyro, PID_paraTypdef *p)
{
    int re, diff, err = p->targetVal - val;
    // if (err > -3 && err < 3)
    //     return (err * p->Kp) / 10 + (gyro * p->Kd) / 20;
    // else
    p->integral = p->integral * turnMul / 10.0;
    p->integral += err;
    if (p->integral >= 10000)
        p->integral = 10000;
    if (p->integral <= -10000)
        p->integral = -10000;
    diff = err - p->proportionLast;
    p->proportionLast = err;
    re = (err * p->Kp + p->integral * p->Ki / 10 + diff * p->Kd / 10) / 10;
    // printf("x=%d,y=%d,s=%.1f\r\n", err, re, p->integral);
    return re;
    // float kg = getBellFunc(err);
    // return err * p->Kp + (-gyro * p->Kd * kg) / 10;
}

int32_t pidCtrlAngle(int32_t currentVal)
{
    int32_t proportion, diff;
    proportion = anglePid.targetVal / 10.0 - currentVal;

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
int32_t balanceModify = 0;
#define MAX_Modify 80

int16_t acc = 0;
int Balance_Pwm, Velocity_Pwm, Moto1, Moto2, Turn_Pwm;
extern uint8_t timerFlag;
point getImgData(void);
// #define PIC_LINE 120
// #define PIC_COL 188
#define PIC_LINE 60
#define PIC_COL 94
float pitch = 0;
const uint8_t whiteLine[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t picCnt = 0;
uint8_t picLossCnt;
uint8_t turnDir = 0;

// const uint8_t distortionList[49]
const uint8_t distortionList[] = {
    0, 0, 1, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 30,
    32, 34, 36, 38, 41, 44, 48, 53, 57,
    62, 67, 73, 80, 87, 94, 102, 110, 130, 160};

int8_t img_sign = 1;
uint8_t lampDieFlag = 0;
int16_t jb_tmp;

uint16_t pictureFlags = 0;
uint8_t pictureCnts[16] = {0};
enum pic_flags
{
    pic_flag_lamp_die,
    pic_flag_lamp_miss,
    pic_flag_reGet,
    pic_miss_plan1,
    pic_miss_plan2,
    pic_miss_plan3,
    pic_miss_plan4,
    pic_running_end
};

#define GET_PIC_FLAG(flag) ((pictureFlags >> (flag)) & 0x0001)
#define PIC_FLAG_SET(flag) pictureFlags |= (0x0001 << (flag))
#define PIC_FLAG_RESET(flag)                 \
    {                                        \
        pictureFlags &= ~(0x0001 << (flag)); \
        pictureCnts[flag] = 0;               \
    }

#define IF_CONTINUE_RUNNING(flag, times)   \
    if ((pictureFlags >> (flag)) & 0x0001) \
        if (pictureCnts[flag]++ <= times)  \
        {

#define END_CONTINUE_RUNNING(flag)           \
    }                                        \
    else                                     \
    {                                        \
        pictureCnts[flag] = 0;               \
        pictureFlags &= ~(0x0001 << (flag)); \
    }

#define __vs_code_tmp_15355135151 {

#define END_AND_GO_TO_NEXT_CONTINUE_RUNNING(flag, next) \
    }                                                   \
    else                                                \
    {                                                   \
        pictureFlags |= (0x0001 << (next));             \
        pictureCnts[flag] = 0;                          \
        pictureFlags &= ~(0x0001 << (flag));            \
    }

int32_t y_position2killLamp = 125; // 94;
// int32_t y_position2changePara = 80;
int32_t y_position2changePara = 66;  // 81;
int32_t y_positionLampDie = 105;     // 90;
int32_t y_position2ChangeSpeed = 80; // 80;
int32_t lampDieSpeed = 95;           // 90;// 86;// 79;
int32_t killSpeed = 120;
// int32_t speedMul1 = 520;
int32_t speedMul1 = 500;
int32_t speedMul2 = 120;
int32_t basSpeedMul = 16; // 15;

int32_t paraMul = 26; // 24;

// int32_t die_waitTime = 160;
int32_t die_waitTime = 42; // 50;
int32_t miss_waitTime = 100;
int32_t reGet_waitTime = 20;
int32_t miss_speed = -160;
int32_t toCenter_mul = 10;
int32_t reGet_TurnSpeed = 100;

int32_t x_modify = 10;

#include "fuzzy.h"
#define fuzzy_lamp_position_size 7
int32_t fuzzy_lamp_position[7] = {20, 60, 72, 75, 84, 95, 140};
int32_t lamp_speed_fuzzy_rule[7] = {-300, -270, -255, -220, -190, -170, -150}; // {-300, -260, -240, -200, -180, -160, -150};
int32_t lamp_turnPara_fuzzy_rule[7] = {90, 95, 120, 135, 145, 145, 145};       // {85, 96, 110, 125, 142, 145, 145};
int32_t *arrayList[] = {fuzzy_lamp_position, lamp_speed_fuzzy_rule, lamp_turnPara_fuzzy_rule};
#include "flash.h"
void fuzzyArrayLoad(void)
{
    uint16_t tmp, cnt = 3;
    uint32_t *buf = (uint32_t *)calloc(fuzzy_lamp_position_size * 3, sizeof(uint32_t));
    uint32_t *buf_cp = buf;
    Flash_loadData(FlashPage_Fuzzy_BLEuart, buf, fuzzy_lamp_position_size * 3);
    while (cnt--)
    {
        tmp = fuzzy_lamp_position_size;
        while (tmp--)
            arrayList[cnt][tmp] = *buf_cp++;
    }
    free(buf);
}
void fuzzyArraySave(void)
{
    uint16_t tmp, cnt = 3;
    uint32_t *dat = (uint32_t *)calloc(fuzzy_lamp_position_size * 3, sizeof(uint32_t));
    uint32_t *dat_cp = dat;
    while (cnt--)
    {
        tmp = fuzzy_lamp_position_size;
        while (tmp--)
            *dat_cp++ = arrayList[cnt][tmp];
    }
    Flash_saveData(FlashPage_Fuzzy_BLEuart, dat, fuzzy_lamp_position_size * 3);
    free(dat);
}
void fuzzyArraySet(uint8_t idx, uint8_t len, uint8_t *dat)
{
    u32_split tmp;
    if (idx == 0xff)
    {
        fuzzyArraySave();
    }
    else if (idx == 0xfe)
    {
        fuzzyArrayLoad();
        screenClear();
        idx = 7;
        while (idx--)
        {
            OLED_printf("%d,", arrayList[0][idx]);
        }
    }
    else
    {
        int32_t *ary = arrayList[idx];
        screenClear();
        while (len--)
        {
            tmp.unit[3] = *dat++;
            tmp.unit[2] = *dat++;
            tmp.unit[1] = *dat++;
            tmp.unit[0] = *dat++;
            *ary++ = tmp.sign_val;
            OLED_printf("%d,", tmp.sign_val);
        }
    }
}

void variableListInit(void)
{
    pushVariable("平衡点", &balancePoint);
    pushVariable("速度", &baseSpeed);

    pushVariable("变参倍率", &paraMul);

    pushVariable("y:变参", &y_position2changePara);
    pushVariable("y:转弯", &y_position2killLamp);
    pushVariable("y:灭灯", &y_positionLampDie);
    pushVariable("y:加减速", &y_position2ChangeSpeed);
    pushVariable("灭后转速", &lampDieSpeed);

    pushVariable("速度倍数", &basSpeedMul);
    pushVariable("减速衰减", &speedMul1);
    pushVariable("加速衰减", &speedMul2);

    pushVariable("灭后持续", &die_waitTime);
    pushVariable("丢灯持续", &miss_waitTime);
    pushVariable("重见持续", &reGet_waitTime);

    pushVariable("重见转速", &reGet_TurnSpeed);
    pushVariable("丢灯转速", &miss_speed);
    pushVariable("回中心速度", &toCenter_mul);
    pushVariable("提前灭速", &killSpeed);

    pushVariable("微分衰减", &turnMul);
    pushVariable("图像修正", &x_modify);

    pushArray("模糊论域:y", fuzzy_lamp_position, fuzzy_lamp_position_size);
    pushArray("规则表-速度", lamp_speed_fuzzy_rule, fuzzy_lamp_position_size);
    pushArray("规则表-参数", lamp_turnPara_fuzzy_rule, fuzzy_lamp_position_size);
}

void paraPlan1(void)
{
    y_position2killLamp = 116;
    y_positionLampDie = 105;
    lampDieSpeed = 105;

    die_waitTime = 42;
    miss_waitTime = 200;
    reGet_waitTime = 20;
    reGet_TurnSpeed = 100;
    miss_speed = -160;
    toCenter_mul = 15;
    killSpeed = 80;

    turnPid.Ki = 18;
    turnPid.Kd = 12;

    ph_car_home_anglePid.Kp = 160;
    ph_car_home_anglePid.Kd = 60;
    // int32_t fuzzy_lamp_position[7] = {20, 60, 72, 75, 84, 95, 140};
    // int32_t lamp_speed_fuzzy_rule[7] = {-300, -270, -255, -220, -190, -170, -150};
    // int32_t lamp_turnPara_fuzzy_rule[7] = {90, 95, 120, 135, 145, 145, 145};
}

#define K_th 1.262485
#define K_th2 0.6531915
int32_t getRealDistance_lamp2car(uint16_t y)
{
    float tmp_y; //, db_tmp_y, result;
    tmp_y = y + K_th * MPU_pitch;
    // if (tmp_y >= 102)
    //     tmp_y = y + K_th2 * MPU_pitch;
    // db_tmp_y = tmp_y * tmp_y;

    // result = 5380.0 - 1172317.0 / tmp_y + 67179442.0 / db_tmp_y;
    // if (result > 3500)
    //     result = 3500;
    // return 350 - result / 10;
    return tmp_y;
    // return result / 10;
}
void lampDistanceTest(void)
{
    screenClear();
    imgPosition = getImgData();
    OLED_printf("d:%d", getRealDistance_lamp2car(imgPosition.y));
}

void pidUpdateFunction(void)
{
    float y, r;

    switch (pidMode)
    {
    case balanceCarHomeMode:
        pitch = MPU_pitch;
        ph_car_home_anglePid.targetVal = balancePoint;

        if (++picCnt == 3)
        {
            imgPosition = getImgData();
            if (imgPosition.x == 0xffff)
                return;
            turnDir = (imgPosition.x < (PIC_COL / 2));
            imgPosition.y = getRealDistance_lamp2car(imgPosition.y);

            jb_tmp = imgPosition.x - (PIC_COL / 2);
            img_sign = (jb_tmp >= 0) ? 1 : -1;
            jb_tmp *= img_sign;
            jb_tmp = distortionList[jb_tmp];
            jb_tmp *= img_sign;
            picCnt = 0;
            jb_tmp += x_modify;

            // if (imgPosition.y >= y_position2changePara)
            //     jb_tmp = jb_tmp * paraMul / 10;
            turnPid.Kp = fuzzy_1_dimensional(imgPosition.y, fuzzy_lamp_position_size, lamp_turnPara_fuzzy_rule, fuzzy_lamp_position);
        }

        // imgPosition.x = PIC_COL / 2;
        // imgPosition.y = PIC_LINE / 2;
        // jb_tmp = getLocationAngleErr();
        // if (getDistance2Center() <= 100)
        //     setPidMode(NullMode);

        img_x = jb_tmp;

        if (imgPosition.x == 0) //找不到灯
        {
            turnPid.Kp = 90;
            if (imgLastPosition.x && imgLastPosition.y >= y_positionLampDie) //刚灭完灯
            {
                PIC_FLAG_SET(pic_flag_lamp_die);
                // ph_car_home_speedPid_left.targetVal = (baseSpeed / 2);
                turnPid.integral = 0;
                beep100Ms();
            }
            else if (!GET_PIC_FLAG(pic_flag_reGet))
            //纯找不到灯
            {
                PIC_FLAG_SET(pic_flag_lamp_miss);
            }

            //顺势转
            if ((getSpeed(RIGHT) - getSpeed(LEFT)) > 5)
                img_x = distortionList[40];
            else if ((getSpeed(LEFT) - getSpeed(RIGHT)) > 5)
                img_x = -distortionList[40];
            // beepSet(DISABLE);
        }
        else //有灯
        {
            // ph_car_home_speedPid_left.targetVal = baseSpeed - (imgPosition.y * baseSpeed / 350);
            // if (imgPosition.y >= y_position2ChangeSpeed)
            //     ph_car_home_speedPid_left.targetVal = baseSpeed + (imgPosition.y * baseSpeed / speedMul1);
            // else
            //     ph_car_home_speedPid_left.targetVal = baseSpeed * basSpeedMul / 10 + (imgPosition.y) * baseSpeed / speedMul2;
            ph_car_home_speedPid_left.targetVal = fuzzy_1_dimensional(imgPosition.y, fuzzy_lamp_position_size, lamp_speed_fuzzy_rule, fuzzy_lamp_position);
            // printf("s=%d\r\n", ph_car_home_speedPid_left.targetVal);
            // lampDieFlag = 0;
            // ph_car_home_speedPid_left.targetVal = baseSpeed * basSpeedMul / 10;
            PIC_FLAG_RESET(pic_flag_lamp_die);
            PIC_FLAG_RESET(pic_flag_reGet);
            PIC_FLAG_RESET(pic_flag_lamp_miss);
            if (imgLastPosition.x == 0) //转的时候刚找到灯
            {
                // img_reGet_flag = 1;
                PIC_FLAG_SET(pic_flag_reGet);
                // beep100Ms();
            }
            if (imgPosition.y >= y_position2killLamp)
            {
                if ((getSpeed(RIGHT) - getSpeed(LEFT)) > 2)
                    // img_x = distortionList[45];
                    img_x = killSpeed;
                else
                    img_x = -killSpeed;
            }
            // beepSet(ENABLE);
        }
        imgLastPosition = imgPosition;

        IF_CONTINUE_RUNNING(pic_flag_reGet, reGet_waitTime)
        img_x = (turnDir ? -1 : 1) * reGet_TurnSpeed;
        END_CONTINUE_RUNNING(pic_flag_reGet)

        IF_CONTINUE_RUNNING(pic_flag_lamp_die, die_waitTime)
        PIC_FLAG_RESET(pic_flag_lamp_miss);
        // ph_car_home_speedPid_left.targetVal = baseSpeed * 0.7;
        ph_car_home_speedPid_left.targetVal = baseSpeed;
        if ((getSpeed(RIGHT) - getSpeed(LEFT)) > 2)
            img_x = lampDieSpeed;
        else
            img_x = -lampDieSpeed;
        // END_CONTINUE_RUNNING(pic_flag_lamp_die)
        END_AND_GO_TO_NEXT_CONTINUE_RUNNING(pic_flag_lamp_die, pic_flag_lamp_miss);

        IF_CONTINUE_RUNNING(pic_flag_lamp_miss, miss_waitTime)
        // img_x = 0;
        // ph_car_home_speedPid_left.targetVal = baseSpeed * 0.8;
        ph_car_home_speedPid_left.targetVal = miss_speed;
        // img_x = toCenter_mul * sqrt(getLocationAngleErr()) / 10.0;
        img_x = toCenter_mul * sqrt(getSubmissLocationAngleErr((getSpeed(RIGHT) - getSpeed(LEFT)) > 2)) / 10.0;
        if (getDistance2Center() <= 100)
            PIC_FLAG_RESET(pic_flag_lamp_miss);
        // END_AND_GO_TO_NEXT_CONTINUE_RUNNING(pic_flag_lamp_miss, pic_miss_plan1);
        END_AND_GO_TO_NEXT_CONTINUE_RUNNING(pic_flag_lamp_miss, pic_flag_lamp_die);

        IF_CONTINUE_RUNNING(pic_miss_plan1, 30)
        ph_car_home_speedPid_left.targetVal = baseSpeed * 0.5;
        // if ((getSpeed(RIGHT) - getSpeed(LEFT)) > 2)
        //     img_x = distortionList[40];
        // else if ((getSpeed(LEFT) - getSpeed(RIGHT)) > 2)
        //     img_x = -distortionList[40];
        img_x = 0;
        turnPid.integral = 0.0;
        END_AND_GO_TO_NEXT_CONTINUE_RUNNING(pic_miss_plan1, pic_flag_lamp_miss);
        // END_CONTINUE_RUNNING(pic_miss_plan1)

        Balance_Pwm = ph_car_home_balance(
            pitch, (gyro[1]), &ph_car_home_anglePid); //===平衡PID控制

        turnPid.targetVal = 0;
        Turn_Pwm = cam_turn(img_x, gyro[2], &turnPid);
        Velocity_Pwm = ph_car_home_velocity(getSpeed(LEFT), getSpeed(RIGHT), &ph_car_home_speedPid_left);

        Moto1 = Balance_Pwm - Velocity_Pwm + Turn_Pwm; //===计算左轮电机最终PWM
        Moto2 = Balance_Pwm - Velocity_Pwm - Turn_Pwm;

        setPower(Moto1, LEFT);
        setPower(Moto2, RIGHT);
        break;
    case pwmMode:
        pwmLeft = speedPidLeft.targetVal;
        pwmRight = speedPidRight.targetVal;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        // printf("r=%d,c=%d\r\n", (int32_t)speedPidLeft.targetVal, getSpeed(LEFT));
        break;
    case speedMode:
        speedPidLeft.targetVal = baseSpeed + turnSpeed;
        speedPidRight.targetVal = baseSpeed - turnSpeed;
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        // printf("r=%d,c=%d\r\n", (int32_t)speedPidLeft.targetVal, getSpeed(LEFT));
        break;
    case angleMode:
        do
        {
            Read_DMP(&pitch, &r, &y);
        } while (pitch < 0.000001 && pitch > -0.000001);
        // speedPidLeft.targetVal = speedPidRight.targetVal = pidCtrlUpdate(pitch * 10, &anglePid) / 10;

        // balanceModify += pidIncrementalCtrlUpdate(-1 * (accel[0] - 164 * quickSin(pitch * 10)) / 1000, &findAnglePid) / 4;
        // balanceModify -= pidIncrementalCtrlUpdate((getSpeed(LEFT) + getSpeed(RIGHT)) / 2, &speedCtrlPid)/10;
        anglePid.targetVal = balancePoint;

        //角度 -pid-> 速度
        speedPidLeft.targetVal =
            speedPidRight.targetVal =
                baseSpeed + pidCtrlAngle(pitch * 10) / 10;

        speedPidLeft.targetVal += turnSpeed;
        speedPidRight.targetVal -= turnSpeed;
        if (speedPidLeft.targetVal > 500)
            speedPidLeft.targetVal = 500;
        if (speedPidLeft.targetVal < -500)
            speedPidLeft.targetVal = -500;
        if (speedPidRight.targetVal > 500)
            speedPidRight.targetVal = 500;
        if (speedPidRight.targetVal < -500)
            speedPidRight.targetVal = -500;
        pwmLeft = pwmLeft + pidIncrementalCtrlUpdate(getSpeed(LEFT), &speedPidLeft) / 10;
        pwmRight = pwmRight + pidIncrementalCtrlUpdate(getSpeed(RIGHT), &speedPidRight) / 10;
        setPower(pwmLeft, LEFT);
        setPower(pwmRight, RIGHT);
        // printf("p=%.1f,a=%.1f\r\n", p, pitch);
        // float p = anglePid.targetVal / 10.0;
        printf("a=%d,r=%d\r\n", acc, (int32_t)speedPidLeft.targetVal);
        break;
    case rockerMode:
        // 代码见历史
        break;
    case picAngularSpeedTestMode:
        // 代码见历史
        break;
    case balanceModifyMode:
        // 代码见历史
        break;
    case accPhysicalMode:
        // 代码见历史
        break;
    case accPidMode:
        // 代码见历史
        break;
    case angleMode_accOutput:
        // 代码见历史
        break;
    default:
        break;
    }

    // TIM_ClearITPendingBit(TIM16, TIM_FLAG_Update);
}

void setPidMode(enum ctrlModes m)
{
    pidMode = m;
    if (m == NullMode)
    {
        Encoder_Integral = 0;
        ph_car_home_speedPid_left.integral = 0.0;
        ph_car_home_speedPid_right.integral = 0.0;
        gyro_y = 0;
    }
}

void setLinerSpeed(int8_t speed)
{
    baseSpeed = speed / 2;
}

void setAngularVelocity(int8_t speed)
{
    if (speed > 50)
        speed = 50;
    if (speed < -50)
        speed = -50;
    imgPosition.x = turnPid.targetVal + speed;
    // turnSpeed = speed;
    // if (speed > 40)
    // {
    //     Flag_Left = 1;
    //     Flag_Right = 0;
    // }
    // else if (speed < -40)
    // {
    //     Flag_Left = 0;
    //     Flag_Right = 1;
    // }
    // else
    //     Flag_Left = Flag_Right = 0;
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

PID_paraTypdef *pidList[] = {&ph_car_home_anglePid, &ph_car_home_speedPid_left, &turnPid};
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

void pidCtrlTimeInit(uint16_t us)
{
    // time16Init(us, 95);
    // TIM_Cmd(TIM17,ENABLE);
    pidMode = speedMode;
}
