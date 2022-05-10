#include "common.h"
#include "pid.h"
#include "menu.h"
#include "stdio.h"
#include "flash.h"
#include "motor/control.h"

extern PID_paraTypdef speedPidLeft;
extern PID_paraTypdef speedPidRight;
extern PID_paraTypdef anglePid;

extern MenuTypedef speedParaMenu[];

// void generalParaCaptionUpdate(MenuTypedef *menu, PID_paraTypdef *pid)
// {
//     sprintf(menu[0].caption, "Kp:  %d", pid->Kp);
//     sprintf(menu[1].caption, "Ki:  %d", pid->Ki);
//     sprintf(menu[2].caption, "Kd:  %d", pid->Kd);
// }
void generalParaCaptionUpdate(MenuTypedef *menu)
{
    float value = 0;
    sprintf(menu[0].caption, "Kp:  %d", menuVal_32_Buf[0]);
    value = menuVal_32_Buf[1] / 10.0;
    sprintf(menu[1].caption, "Ki:  %.1f", value);
    value = menuVal_32_Buf[2] / 10.0;
    sprintf(menu[2].caption, "Kd:  %.1f", value);
}

void generalParaSet(PID_paraTypdef *pid)
{
    pid->Kp = menuVal_32_Buf[0];
    pid->Ki = menuVal_32_Buf[1];
    pid->Kd = menuVal_32_Buf[2];
}

void speedParaCaptionUpdate(void)
{
    generalParaCaptionUpdate(speedParaMenu);
    sprintf(menuCaptionStr4, "speed:  %d", menuVal_32_Buf[3]);
}

void speedInc(void)
{
    // valueAdjust_u16(RIGHT_KEY, menuVal_u16_Buf, 200, 1, speedParaCaptionUpdate);
    valueAdjust_32(RIGHT_KEY, menuVal_32_Buf, 200, -200, 1, speedParaCaptionUpdate);
}

void speedDec(void)
{
    // valueAdjust_u16(LEFT_KEY, menuVal_u16_Buf, 200, 1, speedParaCaptionUpdate);
    valueAdjust_32(LEFT_KEY, menuVal_32_Buf, 200, -200, 1, speedParaCaptionUpdate);
}

void speedSet(void)
{
    generalParaSet(&speedPidLeft);
    generalParaSet(&speedPidRight);
}

void gotoSpeedPara(void)
{
    menuVal_32_Buf[0] = speedPidLeft.Kp;
    menuVal_32_Buf[1] = speedPidLeft.Ki;
    menuVal_32_Buf[2] = speedPidLeft.Kd;
    speedParaCaptionUpdate();
    gotoNextMenu(speedParaMenu);
}

void speed_inc(void)
{
    valueAdjust_32(RIGHT_KEY, menuVal_32_Buf, 300, -300, 10, speedParaCaptionUpdate);
}
void speed_dec(void)
{
    valueAdjust_32(LEFT_KEY, menuVal_32_Buf, 300, -300, 10, speedParaCaptionUpdate);
}
void setSpeed(void)
{
    motorSetSpeed(LEFT, menuVal_32_Buf[3]);
    // motorSetSpeed(RIGHT,menuVal_32_Buf[3]);
    setPidMode(1);
}
void testStop(void)
{
    setPidMode(10);
    setPower(0, LEFT);
    setPower(0, RIGHT);
}
extern const int8_t sinList[];
void gotoSine(void)
{
    uint32_t i = 0;
    setPidMode(1);
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        motorSetSpeed(LEFT, sinList[i++] * 3);
        delayMs(40);
        if (i >= 360)
            i = 0;
    }
    testStop();
}

void gotoRamp(void)
{
    int32_t i = 0;
    setPidMode(1);
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        motorSetSpeed(LEFT, i++ * 3);
        delayMs(40);
        if (i >= 130)
            i = -130;
    }
    testStop();
}
MenuTypedef speedParaMenu[] = {
    {.caption = menuCaptionStr1, .left = speedDec, .mid = NULL, .right = speedInc},
    {.caption = menuCaptionStr2, .left = speedDec, .mid = NULL, .right = speedInc},
    {.caption = menuCaptionStr3, .left = speedDec, .mid = NULL, .right = speedInc},
    {.caption = menuCaptionStr4, .mid = setSpeed, .left = speed_dec, .right = speed_inc},
    {.caption = "sine", .left = gotoSine, .right = gotoSine, .mid = NULL},
    {.caption = "ramp", .left = gotoRamp, .right = gotoRamp, .mid = NULL},
    {.caption = "stop test", .left = testStop, .right = testStop, .mid = testStop},
    {.caption = "apply", .left = speedSet, .mid = speedSet, .right = speedSet},
    GO_BACK_MENU,
    END_OF_MENU};

extern MenuTypedef angleParaMenu[];
void angleParaCaptionUpdate(void)
{
    generalParaCaptionUpdate(angleParaMenu);
}

void angleInc(void)
{
    // valueAdjust_u16(RIGHT_KEY, menuVal_u16_Buf, 200, 1, speedParaCaptionUpdate);
    valueAdjust_32(RIGHT_KEY, menuVal_32_Buf, 200, -200, 1, speedParaCaptionUpdate);
}

void angleDec(void)
{
    // valueAdjust_u16(LEFT_KEY, menuVal_u16_Buf, 200, 1, speedParaCaptionUpdate);
    valueAdjust_32(LEFT_KEY, menuVal_32_Buf, 200, -200, 1, speedParaCaptionUpdate);
}

void angleSet(void)
{
    generalParaSet(&anglePid);
}

void angleStart(void)
{
    setPidMode(2);
}
void mpu6050DmpTest(void);
MenuTypedef angleParaMenu[] = {
    {.caption = menuCaptionStr1, .left = angleDec, .mid = NULL, .right = angleInc},
    {.caption = menuCaptionStr2, .left = angleDec, .mid = NULL, .right = angleInc},
    {.caption = menuCaptionStr3, .left = angleDec, .mid = NULL, .right = angleInc},
    {.caption = "apply", .left = angleSet, .mid = angleSet, .right = angleSet},
    {.caption = "balance angle", .left = mpu6050DmpTest, .mid = NULL, .right = mpu6050DmpTest},
    {.caption = "start", .left = angleStart, .mid = angleStart, .right = angleStart},
    {.caption = "stop", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

void gotoAnglePara(void)
{
    menuVal_32_Buf[0] = anglePid.Kp;
    menuVal_32_Buf[1] = anglePid.Ki;
    menuVal_32_Buf[2] = anglePid.Kd;
    angleParaCaptionUpdate();
    gotoNextMenu(angleParaMenu);
}

void loadBuf2PidPara(uint32_t *buf, PID_paraTypdef *pid)
{
    pid->Kp = (int16_t)(*buf++);
    pid->Ki = (int16_t)(*buf++);
    pid->Kd = (int16_t)(*buf++);
}

void loadPidPara2Buf(uint32_t *buf, PID_paraTypdef *pid)
{
    *buf++ = (uint32_t)(pid->Kp);
    *buf++ = (uint32_t)(pid->Ki);
    *buf++ = (uint32_t)(pid->Kd);
}

#define PID_NUM 2
void pidParaSave(void)
{
    uint32_t dat[PID_NUM * 3] = {0};
    loadPidPara2Buf(dat, &speedPidLeft);
    loadPidPara2Buf(dat + 3, &anglePid);
    Flash_saveData(dat, PID_NUM * 3);
}
void pidParaLoad(void)
{
    uint32_t buf[PID_NUM * 3] = {0};
    Flash_loadData(buf, PID_NUM * 3);
    loadBuf2PidPara(buf, &speedPidLeft);
    loadBuf2PidPara(buf, &speedPidRight);
    loadBuf2PidPara(buf + 3, &anglePid);
}

MenuTypedef paraAdjMenu[] = {
    {.caption = "speed", .left = gotoSpeedPara, .mid = gotoSpeedPara, .right = gotoSpeedPara},
    {.caption = "angle", .left = gotoAnglePara, .mid = gotoAnglePara, .right = gotoAnglePara},
    {.caption = "data save", .left = pidParaSave, .right = pidParaSave, .mid = pidParaSave},
    {.caption = "data load", .left = pidParaLoad, .right = pidParaLoad, .mid = pidParaLoad},
    GO_BACK_MENU,
    END_OF_MENU};
