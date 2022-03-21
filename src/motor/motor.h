/*
 * @Author: your name
 * @Date: 2022-03-08 14:45:40
 * @LastEditTime: 2022-03-17 20:15:18
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \car_pcb3.0V2\src\motor\motor.h
 */
#ifndef AB2416DA_3648_43B0_AC45_30CE0B6B3A9E
#define AB2416DA_3648_43B0_AC45_30CE0B6B3A9E
#include "encoder.h"

#define PWM_PERIOD 1000

void motorInit(void);
void setPower(int32_t power, MotorChoose side);
void motorTest(uint16_t *pwms);
#endif /* AB2416DA_3648_43B0_AC45_30CE0B6B3A9E */
