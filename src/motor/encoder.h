#ifndef C26274E2_ABC6_4D8C_86DF_1D58B604AD5B
#define C26274E2_ABC6_4D8C_86DF_1D58B604AD5B

typedef enum
{
    LEFT,
    RIGHT
} MotorChoose;

void encoderInit(void);
int getCircleCount(MotorChoose side);
int getSpeed(MotorChoose side);
#endif /* C26274E2_ABC6_4D8C_86DF_1D58B604AD5B */
