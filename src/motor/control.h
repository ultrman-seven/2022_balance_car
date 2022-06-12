#ifndef __WY_LIB_CTRL_H__
#define __WY_LIB_CTRL_H__
#include "motor.h"

#define pidOff()                 \
    {                            \
        TIM_Cmd(TIM16, DISABLE); \
    }

enum ctrlModes
{
    pwmMode,    // pwm测试
    speedMode,  //速度环测试
    angleMode,  //角度环测试
    rockerMode, //老头环测试
    picAngularSpeedTestMode,
    balanceModifyMode,
    accPidMode,
    accPhysicalMode,
    angleMode_accOutput,
    NullMode = 20
};
extern enum ctrlModes pidMode;

void motorSetSpeed(MotorChoose motor, int32_t speed);
void pidCtrlTimeInit(uint16_t us);
void setPidMode(enum ctrlModes m);
void adjustPara(uint8_t num, uint8_t *pidPara);
void setLinerSpeed(int8_t speed);
void setAngularVelocity(int8_t speed);
void setBalance(uint8_t *dat);
void sendPara(uint8_t p);
void setBaseSpeed(int32_t s);
extern int32_t balancePoint;
#endif /* __WY_LIB_CTRL_H__ */
