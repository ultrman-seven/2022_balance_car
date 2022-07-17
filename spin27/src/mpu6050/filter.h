#ifndef __FILTER_H
#define __FILTER_H
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#include "common.h"

extern float angle, angle_dot;
extern float Gyro_Balance, Acceleration_Z, Gyro_Turn;
void Kalman_Filter(float Accel, float Gyro);
void Yijielvbo(float angle_m, float gyro_m);
void Get_Angle(uint8_t way);
#endif
