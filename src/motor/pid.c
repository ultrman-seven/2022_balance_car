#include "common.h"
#include "motor.h"
#include "pid.h"

int32_t pidCtrlUpdate(int32_t currentVal, PID_paraTypdef *object)
{
    int32_t proportion, diff;
    proportion = object->targetVal - currentVal;

    object->integral = object->integral * 0.4 + proportion;
    // object->integral += proportion;
    if (object->integral > PWM_PERIOD)
        object->integral = PWM_PERIOD;
    if (object->integral < -PWM_PERIOD)
        object->integral = -PWM_PERIOD;

    diff = proportion - object->proportionLast;
    object->proportionLast = proportion;
    return (object->Kp * proportion /* * 10*/ + object->Ki * object->integral + object->Kd * diff) / 10;
}

int32_t pidIncrementalCtrlUpdate(int32_t currentVal, PID_paraTypdef *pid)
{
    int32_t proportion, p, d;
    proportion = pid->targetVal - currentVal;
    p = proportion - pid->proportionLast;
    d = proportion + pid->proportionLastLast - 2 * pid->proportionLast;

    pid->proportionLastLast = pid->proportionLast;
    pid->proportionLast = proportion;
    return (pid->Kp * p * 10 + pid->Ki * proportion + pid->Kd * d) / 10;
}
