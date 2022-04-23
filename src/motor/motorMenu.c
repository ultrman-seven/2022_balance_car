#include "menu.h"
#include "motor.h"
#include "encoder.h"
#include "stdlib.h"
#include "oledio.h"
#include "control.h"
#include "pid.h"
#include <stdio.h>

extern MenuTypedef pwmTestMenu[];
extern MenuTypedef motorTestMenu[];
extern MenuTypedef motorRelatedMenu[];
extern MenuTypedef speedPIDMenu[];

void showMotorSpeedInScreen(void) // encoder test callback function
{
    while (EXTI_GetITStatus(MID_KEY_EXTI_LINE) == RESET)
    {
        screenClear();
        OLED_printf("speed:\nl:%d/r:%d", getSpeed(LEFT), getSpeed(RIGHT));
        OLED_printf("\ncircles:\nl:%dr:%d", getCircleCount(LEFT), getCircleCount(RIGHT));
        delayMs(50);
    }
    showMenu(menuManager.getCurrentMenu());
}

// pwm test callback functions
uint16_t pwmTest[4] = {0};
void pwmMenuCaptionUpdata(void)
{
    uint8_t count = 4;
    while (count--)
        sprintf(pwmTestMenu[count].caption, "pwm%d: %d", count + 1, pwmTest[count]);
}
void pwmUp(void)
{
    valueAdjust_u16(RIGHT_KEY, pwmTest, PWM_PERIOD, 10, pwmMenuCaptionUpdata);
}
void pwmDown(void)
{
    valueAdjust_u16(LEFT_KEY, pwmTest, PWM_PERIOD, 10, pwmMenuCaptionUpdata);
}

void pwmReset(void)
{
    pwmTest[0] = pwmTest[1] = pwmTest[2] = pwmTest[3] = 0;
    pwmMenuCaptionUpdata();
    showMenu(menuManager.getCurrentMenu());
}

void motorPWM_Test(void)
{
    motorTest(pwmTest);
    while (EXTI_GetFlagStatus(EXTI_Line15) == RESET)
        printf("s=%d\r\n", getSpeed(LEFT));
}

void goToPwmTest(void)
{
    pwmMenuCaptionUpdata();
    gotoNextMenu(pwmTestMenu);
}

// motor test
int32_t menuVal_Speed[2];
void motorSpeedTest(void)
{
    setPower(menuVal_Speed[LEFT], LEFT);
    setPower(menuVal_Speed[RIGHT], RIGHT);
}
void motorMenuCaptionUpdate(void)
{
    sprintf(motorTestMenu[LEFT].caption, "left:   %d", menuVal_Speed[LEFT]);
    sprintf(motorTestMenu[RIGHT].caption, "right:  %d", menuVal_Speed[RIGHT]);
}

void motorInc(void)
{
    valueAdjust_32(RIGHT_KEY, menuVal_Speed, PWM_PERIOD, -PWM_PERIOD, 10, motorMenuCaptionUpdate);
}

void motorDec(void)
{
    valueAdjust_32(LEFT_KEY, menuVal_Speed, PWM_PERIOD, -PWM_PERIOD, 10, motorMenuCaptionUpdate);
}

void motorValReset(void)
{
    menuVal_Speed[0] = menuVal_Speed[1] = 0;
    motorMenuCaptionUpdate();
    showMenu(menuManager.getCurrentMenu());
}

void gotoMotorTest(void)
{
    motorMenuCaptionUpdate();
    gotoNextMenu(motorTestMenu);
}

int32_t speedTarget[2] = {0};
void speedPIDCaptionUpdate(void)
{
    // sprintf(speedPIDMenu[LEFT].caption, "left:%d,r:%d", speedTarget[LEFT],getSpeed(LEFT));
    // sprintf(speedPIDMenu[RIGHT].caption, "right:%d,r:%d", speedTarget[RIGHT],getSpeed(RIGHT));
    sprintf(speedPIDMenu[LEFT].caption, "left:    %d", speedTarget[LEFT]);
    sprintf(speedPIDMenu[RIGHT].caption, "right:  %d", speedTarget[RIGHT]);
}

void speedPidInc(void)
{
    valueAdjust_32(RIGHT_KEY, speedTarget, 500, -500, 10, speedPIDCaptionUpdate);
}

void speedPidDec(void)
{
    valueAdjust_32(LEFT_KEY, speedTarget, 500, -500, 10, speedPIDCaptionUpdate);
}

void speedPidSet(void)
{
    motorSetSpeed(LEFT, speedTarget[LEFT]);
    motorSetSpeed(RIGHT, speedTarget[RIGHT]);
    // pidCtrlTimeInit(5000);
    pidCtrlTimeInit(PIT_UPDATE_TIME);
}

void speedPidReset(void)
{
    pidOff();
    setPower(0, LEFT);
    setPower(0, RIGHT);
}

void gotoSpeedPidTest(void)
{
    speedPIDCaptionUpdate();
    gotoNextMenu(speedPIDMenu);
}

MenuTypedef motorRelatedMenu[] = {
    {.caption = "encoder test", .left = showMotorSpeedInScreen, .mid = NULL, .right = showMotorSpeedInScreen},
    {.caption = "PWM test", .left = goToPwmTest, .right = goToPwmTest, .mid = goToPwmTest},
    {.caption = "motor test", .left = gotoMotorTest, .right = gotoMotorTest, .mid = gotoMotorTest},
    {.caption = "speed test", .left = gotoSpeedPidTest, .right = gotoSpeedPidTest, .mid = gotoSpeedPidTest},
    GO_BACK_MENU,
    END_OF_MENU};

MenuTypedef pwmTestMenu[] = {
    {.caption = menuCaptionStr1, .left = pwmDown, .right = pwmUp, .mid = motorPWM_Test},
    {.caption = menuCaptionStr2, .left = pwmDown, .right = pwmUp, .mid = motorPWM_Test},
    {.caption = menuCaptionStr3, .left = pwmDown, .right = pwmUp, .mid = motorPWM_Test},
    {.caption = menuCaptionStr4, .left = pwmDown, .right = pwmUp, .mid = motorPWM_Test},
    {.caption = "value reset", .left = pwmReset, .right = pwmReset, .mid = pwmReset},
    GO_BACK_MENU,
    END_OF_MENU};

MenuTypedef motorTestMenu[] = {
    {.caption = menuCaptionStr1, .left = motorDec, .right = motorInc, .mid = motorSpeedTest},
    {.caption = menuCaptionStr2, .left = motorDec, .right = motorInc, .mid = motorSpeedTest},
    {.caption = "value reset", .left = motorValReset, .right = motorValReset, .mid = motorValReset},
    GO_BACK_MENU,
    END_OF_MENU};

MenuTypedef speedPIDMenu[] = {
    {.caption = menuCaptionStr1, .left = speedPidDec, .right = speedPidInc, .mid = NULL},
    {.caption = menuCaptionStr2, .left = speedPidDec, .right = speedPidInc, .mid = NULL},
    {.caption = "set speed", .left = speedPidSet, .right = speedPidSet, .mid = speedPidSet},
    {.caption = "reset speed", .left = speedPidReset, .right = speedPidReset, .mid = speedPidReset},
    GO_BACK_MENU,
    END_OF_MENU};
