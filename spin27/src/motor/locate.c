#include "locate.h"

#define PI 3.1415926535

#define Center_X 0
#define Center_Y -1000
// x:+-1500, y:-2100

float getDistance2Center(void)
{
    int32_t dx, dy;
    float result;
    dx = position_x - Center_X;
    dy = position_y - Center_Y;
    result = dx * dx + dy * dy;
    result = sqrt(result);
    return result;
}

float getLocationAngleErr(void)
{
    int32_t dx, dy;
    float theta;
    dx = position_x - Center_X;
    dy = position_y - Center_Y;
    theta = atan2(dy, dx);
    theta = theta * 180 / PI;
    theta = 90 + MPU_yaw - theta - yawErr;
    if (theta >= 360)
        theta -= 360;
    if (theta <= -360)
        theta += 360;
    return theta;
}

float getSubmissLocationAngleErr(uint8_t dir)
{
    int32_t dx, dy;
    float theta;
    dx = position_x - Center_X;
    dy = position_y - Center_Y;
    theta = atan2(dy, dx);
    theta = theta * 180 / PI;
    theta = 90 + MPU_yaw - theta - yawErr;
    if (theta >= 360)
        theta -= 360;
    if (theta <= -360)
        theta += 360;
    if (dir)
    {
        if (theta < 0)
            theta += 360;
    }
    else
    {
        if (theta > 0)
            theta -= 360;
    }
    return theta;
}
