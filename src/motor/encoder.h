#ifndef __WY_LIB_ENCODER_H__
#define __WY_LIB_ENCODER_H__

typedef enum
{
    LEFT,
    RIGHT
} MotorChoose;

void encoderInit(void);
int getCircleCount(MotorChoose side);
int getSpeed(MotorChoose side);
#endif /* __WY_LIB_ */ENCODER_H__
