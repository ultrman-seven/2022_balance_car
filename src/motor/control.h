#ifndef __WY_LIB_CTRL_H__
#define __WY_LIB_CTRL_H__
#include "motor.h"

#define pidOff()                 \
    {                            \
        TIM_Cmd(TIM16, DISABLE); \
    }
void motorSetSpeed(MotorChoose motor, int32_t speed);
void pidCtrlTimeInit(uint16_t us);
void setPidMode(uint8_t m);
void adjustPara(uint8_t num, uint8_t *pidPara);
void setLinerSpeed(int8_t speed);
void setAngularVelocity(int8_t speed);
void setBalance(uint8_t *dat);
void sendPara(uint8_t p);
extern int32_t balancePoint;
#endif /* __WY_LIB_CTRL_H__ */
