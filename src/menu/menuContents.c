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
void runStart(void)
{
}

void gotoTests(void)
{
    gotoNextMenu(testMenu);
}

extern ErrorStatus sysClkState;
extern uint8_t mpuDmpState;
void getSysState(void)
{
    screenClear();
    OLED_printf("sys clock: %s\nmpu dmp:%d\n", (sysClkState == SUCCESS) ? "ok" : "error", mpuDmpState);
    OLED_printf("mpu: 0x%x", I2C_ReadOneByte((0x69 << 1), 0x75));
}

// test menu callback functions

void beepTest(void)
{
    beepForHundredMs(5);
}

// mpu6050 callback functions
void gotoMPU6050Test(void)
{
    gotoNextMenu(mpuTestMenu);
}

extern uint32_t MPU_time;
extern uint8_t MPU_who;
extern PID_paraTypdef anglePid;
void mpu6050DmpTest(void)
{
    float y, p, r;
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        Read_DMP(&p, &r, &y);
        // y = MPU_yaw;
        // p = MPU_pitch;
        // r = MPU_roll;
        screenClear();
        OLED_printf("who:%d\np:%f\nr:%f\ny:%f", MPU_who, p, r, y);
        // printf("地址:%d\n俯仰角:%f\troll:%f\tyaw:%f\n", MPU_who, p, r, y);
        delayMs(50);
    }
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
        OLED_printf(
            "accel,p:%.2f\nx: %d\ny: %d\nz: %d", p,
            accel[0] + 500 - 168 * quickSin(p * 10),
            accel[1], accel[2] + 164 * quickCos(p * 10));
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
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0)
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
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0)
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
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0)
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
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0)
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
    {.caption = "调参", .left = gotoParaMenu, .right = gotoParaMenu, .right = gotoParaMenu},
    {.caption = "功能测试", .mid = gotoTests, .right = gotoTests, .left = gotoTests},
    {.caption = "run", .mid = runStart, .right = runStart, .left = runStart},
    {.caption = "系统状态", .left = getSysState, .right = getSysState, .mid = getSysState},
    END_OF_MENU};
void cameraSetOn(void);
void cameraSetOff(void);
MenuTypedef testMenu[] = {
    {.caption = "蜂鸣器", .left = beepTest, .mid = beepTest, .right = beepTest},
    {.caption = "mpu6050测试", .left = gotoMPU6050Test, .right = gotoMPU6050Test, .mid = gotoMPU6050Test},
    {.caption = "电机", .left = gotoMotorRelatedTest, .mid = gotoMotorRelatedTest, .right = gotoMotorRelatedTest},
    {.caption = "摄像头", .left = cameraSetOn, .right = cameraSetOn, .mid = cameraSetOff},
    GO_BACK_MENU,
    END_OF_MENU};

MenuTypedef mpuTestMenu[] = {
    {.caption = "mpu state:", .left = NULL, .mid = NULL, .right = NULL},
    {.caption = "DMP 测试", .left = mpu6050DmpTest, .right = mpu6050DmpTest, .mid = NULL},
    {.caption = "基础数据读写", .left = mpu6050DataTest, .right = mpu6050DataTest, .mid = NULL},
    GO_BACK_MENU,
    END_OF_MENU};
