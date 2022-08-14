#include "menu.h"
#include "stdlib.h"
#include "oledio.h"
#include "beep.h"
#include "mpu6050.h"
#include "encoder.h"
#include "motor.h"
#include "stdio.h"
#include "iicsoft.h"
#include "pid.h"
#include "motor/control.h"
#include "quickSin.h"
#include "battery.h"

char menuCaptionStr1[20];
char menuCaptionStr2[20];
char menuCaptionStr3[20];
char menuCaptionStr4[20];

uint16_t menuVal_u16_Buf[4] = {0};
int32_t menuVal_32_Buf[4] = {0};

extern MenuTypedef mainMenu[];
extern MenuTypedef testMenu[];
extern MenuTypedef mpuTestMenu[];
extern MenuTypedef motorRelatedMenu[];
extern MenuTypedef paraAdjMenu[];

void gotoNextMenu(MenuTypedef *menu)
{
    currentChoose = chooseLine = 0;
    // menuVal_u16_Buf[0] = menuVal_u16_Buf[1] = menuVal_u16_Buf[2] = menuVal_u16_Buf[3] = 0;
    // menuVal_32_Buf[0] = menuVal_32_Buf[1] = menuVal_32_Buf[2] = menuVal_32_Buf[3] = 0;
    menuManager.append(menu);
    showMenu(menuManager.getCurrentMenu());
}

void goBackToLastMenu(void)
{
    chooseLine = currentChoose = 0;
    showMenu(menuManager.getLastMenu());
}

// main menu callback functions
void ph_car_home_cam_start(void);
#define runStart ph_car_home_cam_start
// void runStart(void)
// {
//     ph_car_home_cam_start();
// }

void gotoTests(void)
{
    gotoNextMenu(testMenu);
}

extern ErrorStatus sysClkState;
extern uint8_t mpuDmpState;
void getSysState(void)
{
    screenClear();
    OLED_printf("外部时钟: %s\n", (sysClkState == SUCCESS) ? "ok/96MHz" : "error");
    OLED_printf("电池电量: NC\n"); // getVoltage());
    OLED_printf("mpu: 0x%x", I2C_ReadOneByte((0x68 << 1), 0x75));
}

// test menu callback functions

void beepTest(void)
{
    // beepForHundredMs(5);
    beep100Ms();
    // beepFlip();
}

// mpu6050 callback functions
void gotoMPU6050Test(void)
{
    gotoNextMenu(mpuTestMenu);
}

extern uint32_t MPU_time;
extern uint8_t MPU_who;
extern PID_paraTypdef anglePid;
#include "mpu6050/filter.h"
void mpu6050DmpTest(void)
{
    float y = 0, p, r = 0;
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        mpuIntCMD(DISABLE);
        Read_DMP(&p, &r, &y);
        // Get_Angle(3);
        // y = MPU_yaw;
        // p = MPU_pitch;
        // r = MPU_roll;
        screenClear();
        // OLED_printf("who:%d\np:%f\ngy:%f\ngz:%f", MPU_who, p, Gyro_Balance, Gyro_Turn);
        OLED_printf("mpu地址:0x%x\n俯仰角:%.2f\n横滚角:%.2f\n偏航角:%.2f\n", MPU_who, p, r, y);
        delayMs(50);
    }
    mpuIntCMD(ENABLE);
    balancePoint = anglePid.targetVal = 10 * p;
    showMenu(menuManager.getCurrentMenu());
}

void mpu6050DataTest(void)
{
    float y, p, r;
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        Read_DMP(&p, &r, &y);
        screenClear();
        // OLED_printf("t:%d", Read_Temperature());
        // OLED_printf("gy:%d", gyro[1]);
        // OLED_printf(
        //     "accel,p:%.2f\nx: %d\ny: %d\nz: %d", p,
        //     accel[0] + 500 - 168 * quickSin(p * 10),
        //     accel[1], accel[2] + 164 * quickCos(p * 10));
        OLED_printf("gyros:\nx:%d\ny:%d\nz:%d", gyro[0], gyro[1], gyro[2]);
        printf("r=%d\r\n", accel[0] + 500 - 168 * quickSin(p * 10));
        delayMs(5);
    }
    showMenu(menuManager.getCurrentMenu());
}

// value adjust operation
void valueAdjust_u16(KeyChoose key, uint16_t *val, uint64_t maxVal, uint8_t step, void (*menuUpdataCallbackFunction)(void))
{
    uint8_t waitTime = 100;
    if (key == RIGHT_KEY)
    {
        if (val[currentChoose] < maxVal)
            val[currentChoose] += step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0)
        {
            if (val[currentChoose] < maxVal)
                val[currentChoose] += step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
    else
    {
        if (val[currentChoose])
            val[currentChoose] -= step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0)
        {
            if (val[currentChoose])
                val[currentChoose] -= step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
}
void valueAdjust_32(KeyChoose key, int32_t *val, int64_t maxVal, int64_t minVal, uint8_t step, void (*menuUpdataCallbackFunction)(void))
{
    uint8_t waitTime = 100;
    if (key == RIGHT_KEY)
    {
        if (val[currentChoose] < maxVal)
            val[currentChoose] += step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0)
        {
            if (val[currentChoose] < maxVal)
                val[currentChoose] += step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
    else
    {
        if (val[currentChoose] > minVal)
            val[currentChoose] -= step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0)
        {
            if (val[currentChoose] > minVal)
                val[currentChoose] -= step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
}

void gotoMotorRelatedTest(void)
{
    gotoNextMenu(motorRelatedMenu);
}

void gotoParaMenu(void)
{
    gotoNextMenu(paraAdjMenu);
}

MenuTypedef mainMenu[] = {
    {.caption = "调参:pid", .mid = gotoParaMenu, .left = gotoParaMenu, .right = gotoParaMenu},
    {.caption = "调参:其他变量", .mid = gotoVariableMenu, .left = gotoVariableMenu, .right = gotoVariableMenu},
    {.caption = "功能测试", .mid = gotoTests, .right = gotoTests, .left = gotoTests},
    {.caption = "run", .mid = runStart, .right = runStart, .left = runStart},
    {.caption = "系统状态", .left = getSysState, .right = getSysState, .mid = getSysState},
    END_OF_MENU};
void cameraSetOn(void);
void cameraSetOff(void);
void test3277(void);
void lampDistanceTest(void);
MenuTypedef testMenu[] = {
    {.caption = "蜂鸣器", .left = beepTest, .mid = beepTest, .right = beepTest},
    {.caption = "mpu6050测试", .left = gotoMPU6050Test, .right = gotoMPU6050Test, .mid = gotoMPU6050Test},
    {.caption = "电机", .left = gotoMotorRelatedTest, .mid = gotoMotorRelatedTest, .right = gotoMotorRelatedTest},
    {.caption = "摄像头", .left = cameraSetOn, .right = cameraSetOn, .mid = cameraSetOff},
    {.caption = "3277", .left = test3277, .right = test3277, .mid = test3277},
    {.caption = "信标测距", .left = lampDistanceTest, .mid = lampDistanceTest, .right = lampDistanceTest},
    GO_BACK_MENU,
    END_OF_MENU};

MenuTypedef mpuTestMenu[] = {
    {.caption = "mpu state:", .left = NULL, .mid = NULL, .right = NULL},
    {.caption = "DMP 测试", .left = mpu6050DmpTest, .right = mpu6050DmpTest, .mid = NULL},
    {.caption = "基础数据读写", .left = mpu6050DataTest, .right = mpu6050DataTest, .mid = NULL},
    GO_BACK_MENU,
    END_OF_MENU};
