#ifndef A51C7A8B_BEEE_424F_A8D8_477CC09E7978
#define A51C7A8B_BEEE_424F_A8D8_477CC09E7978
#include "motor.h"

#define pidOff()                 \
    {                            \
        TIM_Cmd(TIM16, DISABLE); \
    }
void motorSetSpeed(MotorChoose motor, int32_t speed);
void pidCtrlTimeInit(uint16_t us);
#endif /* A51C7A8B_BEEE_424F_A8D8_477CC09E7978 */
