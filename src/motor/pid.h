#ifndef __WY_LIB_PID_H__
#define __WY_LIB_PID_H__
#include "common.h"
typedef struct
{
    int16_t Kp;
    int16_t Ki;
    int16_t Kd;
    uint8_t p_times;
    uint8_t i_times;
    uint8_t d_times;
    // uint8_t Kp;
    // uint8_t Ki;
    // uint8_t Kd;
    // float Kp;
    // float Ki;
    // float Kd;

    float targetVal;
    float integral;
    float proportionLast;
    float proportionLastLast;
    // int32_t targetVal;
    // int32_t integral;
    // int32_t proportionLast;
    // int32_t proportionLastLast;
} PID_paraTypdef;

#define PIT_UPDATE_TIME 10000 // us
int32_t pidIncrementalCtrlUpdate(int32_t currentVal, PID_paraTypdef *pid);
int32_t pidCtrlUpdate(int32_t currentVal, PID_paraTypdef *object);
void pidUpdateFunction(void);
#endif /* __WY_LIB_PID_H__ */
