/*
 * @Author: your name
 * @Date: 2022-03-11 09:59:35
 * @LastEditTime: 2022-03-19 11:39:36
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \car_pcb3.0V2\src\motor\pid.h
 */
#ifndef A92AA3F4_3F86_45B9_9B66_A4249A784C27
#define A92AA3F4_3F86_45B9_9B66_A4249A784C27
typedef struct
{
    int16_t Kp;
    int16_t Ki;
    int16_t Kd;
    // uint8_t Kp;
    // uint8_t Ki;
    // uint8_t Kd;
    // float Kp;
    // float Ki;
    // float Kd;

    int32_t targetVal;
    int32_t integral;
    int32_t proportionLast;
    int32_t proportionLastLast;
} PID_paraTypdef;

#define PIT_UPDATE_TIME 5000 //us
int32_t pidIncrementalCtrlUpdate(int32_t currentVal, PID_paraTypdef *pid);
int32_t pidCtrlUpdate(int32_t currentVal, PID_paraTypdef *object);
#endif /* A92AA3F4_3F86_45B9_9B66_A4249A784C27 */
