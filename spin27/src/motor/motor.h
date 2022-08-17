/*
 * @Author: your name
 * @Date: 2022-03-08 14:45:40
 * @LastEditTime: 2022-08-15 11:11:45
 * @LastEditors: Leslie 1813180652@qq.com
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \car_pcb3.0V2\src\motor\motor.h
 */
#ifndef __WY_LIB_MOTOR_H__
#define __WY_LIB_MOTOR_H__
#include "motor/encoder.h"

// #define PWM_PERIOD 1000
#define PWM_PERIOD 7200

void motorInit(void);
void setPower(int32_t power, MotorChoose side);
void motorTest(uint16_t *pwms);
#endif /* __WY_LIB_MOTOR_H__ */
