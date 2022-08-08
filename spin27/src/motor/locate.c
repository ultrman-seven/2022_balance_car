#include "common.h"
#include "mpu6050.h"
#include "encoder.h"
#include "math.h"

float position_x = 0;
float position_y = 0;

#define yawErr 110
#define PI 3.1415926535

void positionUpdate(void)
{
    float dx = (getSpeed(LEFT) + getSpeed(RIGHT)) / 2.0;
    float yaw = yawErr - MPU_yaw;

    if (yaw > 180)
        yaw -= 360;
    if (yaw < -180)
        yaw += 360;

    position_x += dx * sin(yaw * PI / 180);
    position_y += dx * cos(yaw * PI / 180);
}
