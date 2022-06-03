#include "common.h"
#include "pid.h"
#include "menu.h"
#include "stdio.h"
#include "flash.h"
#include "motor/control.h"

void cameraSetOn(void);
void cameraSetOff(void);

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
void generalParaCaptionUpdate(void)
{
    float value = 0;
    MenuTypedef *menu = menuManager.getCurrentMenu();
    sprintf(menu[0].caption, "Kp:  %d", menuVal_32_Buf[0]);
    value = menuVal_32_Buf[1] / 10.0;
    sprintf(menu[1].caption, "Ki:  %.1f", value);
    value = menuVal_32_Buf[2] / 10.0;
    sprintf(menu[2].caption, "Kd:  %.1f", value);
    sprintf(menu[3].caption, "值:  %d", menuVal_32_Buf[3]);
}

void generalParaSet(PID_paraTypdef *pid)
{
    pid->Kp = menuVal_32_Buf[0];
    pid->Ki = menuVal_32_Buf[1];
    pid->Kd = menuVal_32_Buf[2];
}

void generalInc(void)
{
    valueAdjust_32(RIGHT_KEY, menuVal_32_Buf, 300, -300, 1, generalParaCaptionUpdate);
}

void generalDec(void)
{
    valueAdjust_32(LEFT_KEY, menuVal_32_Buf, 300, -300, 1, generalParaCaptionUpdate);
}

#define GeneralPidParaAdjMenu                                                               \
    {.caption = menuCaptionStr1, .left = generalDec, .mid = NULL, .right = generalInc},     \
        {.caption = menuCaptionStr2, .left = generalDec, .mid = NULL, .right = generalInc}, \
    {                                                                                       \
        .caption = menuCaptionStr3, .left = generalDec, .mid = NULL, .right = generalInc    \
    }

void speedSet(void)
{
    generalParaSet(&speedPidLeft);
    generalParaSet(&speedPidRight);
}

void gotoSpeedPara(void)
{
    gotoNextMenu(speedParaMenu);
    menuVal_32_Buf[0] = speedPidLeft.Kp;
    menuVal_32_Buf[1] = speedPidLeft.Ki;
    menuVal_32_Buf[2] = speedPidLeft.Kd;
    generalParaCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

void setSpeed(void)
{
    motorSetSpeed(LEFT, menuVal_32_Buf[3]);
    // motorSetSpeed(RIGHT,menuVal_32_Buf[3]);
    setPidMode(speedMode);
}
void testStop(void)
{
    setPidMode(NullMode);
    setPower(0, LEFT);
    setPower(0, RIGHT);
    speedPidLeft.targetVal = speedPidRight.targetVal = 0;
    cameraSetOff();
}
extern const int8_t sinList[];
void gotoSine(void)
{
    uint32_t i = 0;
    setPidMode(speedMode);
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
    setPidMode(speedMode);
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
    GeneralPidParaAdjMenu,
    {.caption = menuCaptionStr4, .mid = setSpeed, .left = generalDec, .right = generalInc},
    {.caption = "参数应用", .left = speedSet, .mid = speedSet, .right = speedSet},
    {.caption = "正弦输入", .left = gotoSine, .right = gotoSine, .mid = NULL},
    {.caption = "斜坡输入", .left = gotoRamp, .right = gotoRamp, .mid = NULL},
    {.caption = "停止测试", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

extern MenuTypedef angleParaMenu[];

void angleSet(void)
{
    generalParaSet(&anglePid);
}

void angleStart(void)
{
    setPidMode(angleMode);
}

void b_s_set(void)
{
    setBaseSpeed(menuVal_32_Buf[3]);
}
void mpu6050DmpTest(void);
MenuTypedef angleParaMenu[] = {
   GeneralPidParaAdjMenu,
    {.caption = menuCaptionStr4, .left = generalDec, .mid = b_s_set, .right = generalInc},
    {.caption = "参数应用", .left = angleSet, .mid = angleSet, .right = angleSet},
    {.caption = "平衡点调整", .left = mpu6050DmpTest, .mid = NULL, .right = mpu6050DmpTest},
    {.caption = "开始", .left = angleStart, .mid = angleStart, .right = angleStart},
    {.caption = "结束", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

void gotoAnglePara(void)
{
    gotoNextMenu(angleParaMenu);
    menuVal_32_Buf[0] = anglePid.Kp;
    menuVal_32_Buf[1] = anglePid.Ki;
    menuVal_32_Buf[2] = anglePid.Kd;
    generalParaCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

extern PID_paraTypdef picTurn;
extern MenuTypedef picMenu[];

void picSet(void)
{
    generalParaSet(&picTurn);
}

void picStart(void)
{
    setPidMode(picAngularSpeedTestMode);
    cameraSetOn();
}

MenuTypedef picMenu[] = {
   GeneralPidParaAdjMenu,
    {.caption = menuCaptionStr4, .left = NULL, .mid = NULL, .right = NULL},
    {.caption = "参数应用", .left = picSet, .mid = picSet, .right = picSet},
    {.caption = "开始测试", .left = picStart, .mid = testStop, .right = picStart},
    {.caption = "停止测试", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

void gotoPicPara(void)
{
    gotoNextMenu(picMenu);
    menuVal_32_Buf[0] = picTurn.Kp;
    menuVal_32_Buf[1] = picTurn.Ki;
    menuVal_32_Buf[2] = picTurn.Kd;
    generalParaCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

extern PID_paraTypdef speedCtrlPid;
extern MenuTypedef speedHoldMenu[];


void s_h_Set(void)
{
    generalParaSet(&speedCtrlPid);
}

void s_h_Start(void)
{
    setPidMode(balanceModifyMode);
}

void s_h_set_speed(void)
{
    speedCtrlPid.targetVal = menuVal_32_Buf[3];
}

MenuTypedef speedHoldMenu[] = {
    GeneralPidParaAdjMenu,
    {.caption = menuCaptionStr4, .left = generalDec, .right = generalInc, .mid = s_h_set_speed},
    {.caption = "参数应用", .left = s_h_Set, .mid = s_h_Set, .right = s_h_Set},
    {.caption = "平衡点调整", .left = mpu6050DmpTest, .mid = NULL, .right = mpu6050DmpTest},
    {.caption = "开始", .left = s_h_Start, .mid = s_h_Start, .right = s_h_Start},
    {.caption = "停止", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

void gotoSpeedHold(void)
{
    gotoNextMenu(speedHoldMenu);
    menuVal_32_Buf[0] = speedCtrlPid.Kp;
    menuVal_32_Buf[1] = speedCtrlPid.Ki;
    menuVal_32_Buf[2] = speedCtrlPid.Kd;
    menuVal_32_Buf[3] = speedCtrlPid.targetVal;
    generalParaCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

extern MenuTypedef accPidMenu[];
extern PID_paraTypdef accPidLeft;
extern PID_paraTypdef accPidRight;

void accSet(void)
{
    generalParaSet(&accPidLeft);
    generalParaSet(&accPidRight);
    accPidLeft.targetVal = menuVal_32_Buf[3];
    accPidRight.targetVal = menuVal_32_Buf[3];
}
void accTest(void)
{
    setPidMode(accPidMode);
}

MenuTypedef accPidMenu[] = {
   GeneralPidParaAdjMenu,
    {.caption = menuCaptionStr4, .left = generalDec, .right = generalInc, .mid = accSet},
    {.caption = "参数应用", .left = accSet, .mid = accSet, .right = accSet},
    // {.caption = "balance angle", .left = mpu6050DmpTest, .mid = NULL, .right = mpu6050DmpTest},
    {.caption = "开始", .left = accTest, .mid = accTest, .right = accTest},
    {.caption = "停止", .left = testStop, .right = testStop, .mid = testStop},
    GO_BACK_MENU,
    END_OF_MENU};

void gotoAccMenu(void)
{
    gotoNextMenu(accPidMenu);
    menuVal_32_Buf[0] = accPidLeft.Kp;
    menuVal_32_Buf[1] = accPidLeft.Ki;
    menuVal_32_Buf[2] = accPidLeft.Kd;
    menuVal_32_Buf[3] = accPidLeft.targetVal;
    generalParaCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

// belows are about data loading and saving

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

#define PID_NUM 4
void pidParaSave(void)
{
    uint32_t dat[PID_NUM * 3] = {0};
    loadPidPara2Buf(dat, &speedPidLeft);
    loadPidPara2Buf(dat + 3, &anglePid);
    loadPidPara2Buf(dat + 6, &picTurn);
    loadPidPara2Buf(dat + 9, &speedCtrlPid);
    Flash_saveData(dat, PID_NUM * 3);
}
void pidParaLoad(void)
{
    uint32_t buf[PID_NUM * 3] = {0};
    Flash_loadData(buf, PID_NUM * 3);
    loadBuf2PidPara(buf, &speedPidLeft);
    loadBuf2PidPara(buf, &speedPidRight);
    loadBuf2PidPara(buf + 3, &anglePid);
    loadBuf2PidPara(buf + 6, &picTurn);
    loadBuf2PidPara(buf + 9, &speedCtrlPid);
}

MenuTypedef paraAdjMenu[] = {
    {.caption = "速度环", .left = gotoSpeedPara, .mid = gotoSpeedPara, .right = gotoSpeedPara},
    {.caption = "加速度环", .left = gotoAccMenu, .mid = gotoAccMenu, .right = gotoAccMenu},
    {.caption = "角度环", .left = gotoAnglePara, .mid = gotoAnglePara, .right = gotoAnglePara},
    {.caption = "摄像头", .left = gotoPicPara, .mid = gotoPicPara, .right = gotoPicPara},
    {.caption = "平衡调速环", .left = gotoSpeedHold, .mid = gotoSpeedHold, .right = gotoSpeedHold},
    {.caption = "保存", .left = pidParaSave, .right = pidParaSave, .mid = pidParaSave},
    {.caption = "载入", .left = pidParaLoad, .right = pidParaLoad, .mid = pidParaLoad},
    GO_BACK_MENU,
    END_OF_MENU};
