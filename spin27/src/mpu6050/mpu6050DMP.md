#include "common.h"
#include "iicsoft.h"
#include "oledio.h"
#include "stdio.h"

#include "eMPL6/dmpKey.h"
#include "eMPL6/dmpmap.h"
#include "eMPL6/inv_mpu.h"
#include "eMPL6/inv_mpu_dmp_motion_driver.h"

#if 1
u8 run_self_test(void)
{
    int result;
    // char test_packet[4] = {0};
    long gyro[3] = {0}, accel[3] = {0};
    result = mpu_run_self_test(gyro, accel);
    if (result == 0x3)
    {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
        return 0;
    }
    else
        return result;
}
#else
static inline u8 run_self_test(void)
{
    char str[64] = {0};
    int result;
    long gyro[3], accel[3];

#if defined(MPU6500) || defined(MPU9250)
    result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined(MPU6050) || defined(MPU9150)
    result = mpu_run_self_test(gyro, accel);
#endif
    if (result == 0x7)
    {
        OLED_print("Passed!\n");
        sprintf(str, "accel: %7.4f %7.4f %7.4f\n",
                accel[0] / 65536.f,
                accel[1] / 65536.f,
                accel[2] / 65536.f);
        OLED_print(str);
        sprintf(str, "gyro: %7.4f %7.4f %7.4f\n",
                gyro[0] / 65536.f,
                gyro[1] / 65536.f,
                gyro[2] / 65536.f);
        OLED_print(str);
        /* Test passed. We can trust the gyro data here, so now we need to update calibrated data*/

#ifdef USE_CAL_HW_REGISTERS
        /*
         * This portion of the code uses the HW offset registers that are in the MPUxxxx devices
         * instead of pushing the cal data to the MPL software library
         */
        unsigned char i = 0;

        for (i = 0; i < 3; i++)
        {
            gyro[i] = (long)(gyro[i] * 32.8f); // convert to +-1000dps
            accel[i] *= 2048.f;                // convert to +-16G
            accel[i] = accel[i] >> 16;
            gyro[i] = (long)(gyro[i] >> 16);
        }

        mpu_set_gyro_bias_reg(gyro);

#if defined(MPU6500) || defined(MPU9250)
        mpu_set_accel_bias_6500_reg(accel);
#elif defined(MPU6050) || defined(MPU9150)
        mpu_set_accel_bias_6050_reg(accel);
#endif
#else
        /* Push the calibrated data to the MPL library.
         *
         * MPL expects biases in hardware units << 16, but self test returns
         * biases in g's << 16.
         */
        unsigned short accel_sens;
        float gyro_sens;

        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        // inv_set_accel_bias(accel, 3);
        mpu_get_gyro_sens(&gyro_sens);
        gyro[0] = (long)(gyro[0] * gyro_sens);
        gyro[1] = (long)(gyro[1] * gyro_sens);
        gyro[2] = (long)(gyro[2] * gyro_sens);
        // inv_set_gyro_bias(gyro, 3);
#endif
    }
    else
    {
        if (!(result & 0x1))
            OLED_print("Gyro failed.\n");
        if (!(result & 0x2))
            OLED_print("Accel failed.\n");
        if (!(result & 0x4))
            OLED_print("Compass failed.\n");
    }
    return 0;
}
#endif

int mpu6050eMPL_Init(void)
{
    int mpuErr;
    static signed char gyro_orientation[9] = {1, 0, 0,
                                              0, 1, 0,
                                              0, 0, 1};

    struct int_param_s int_param;

    NVIC_InitTypeDef nvic;
    EXTI_InitTypeDef exti;
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    nvic.NVIC_IRQChannel = EXTI0_1_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPriority = 2;
    //NVIC_Init(&nvic);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &gpio);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
    exti.EXTI_Line = EXTI_Line0;
    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    //EXTI_Init(&exti);

    iicSoftInit();

    mpuErr = mpu_init(&int_param);
    if (mpuErr)
    {
        OLED_print("Err: mpu init\n");
        return 1;
    }
    mpuErr = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL); //设置所需要的传感器
    if (mpuErr)
        return 1;
    mpuErr = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL); //设置FIFO
    if (mpuErr)
        return 2;
    mpuErr = mpu_set_sample_rate(100); //设置采样率
    if (mpuErr)
        return 3;
    mpuErr = dmp_load_motion_driver_firmware(); //加载dmp固件
    if (mpuErr)
        return 4;
    // mpuErr = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)); //设置陀螺仪方向
    mpuErr = dmp_set_orientation(0); //设置陀螺仪方向
    if (mpuErr)
        return 5;
    mpuErr = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | //设置dmp功能
                                DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
                                DMP_FEATURE_GYRO_CAL);
    if (mpuErr)
        return 6;
    mpuErr = dmp_set_fifo_rate(100); //设置DMP输出速率(最大不超过200Hz)
    if (mpuErr)
        return 7;
    // mpuErr = run_self_test(); //自检
    // if (mpuErr)
    //     return mpuErr + 11000;
    mpuErr = mpu_set_dmp_state(1); //使能DMP
    if (mpuErr)
        return 9;
    return 0;
}

#define q30 1073741824.0f
u8 mpu_dmp_get_data(float *pitch, float *roll, float *yaw)
{
    float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    unsigned long sensor_timestamp;
    short gyro[3], accel[3], sensors;
    unsigned char more;
    long quat[4];
    if (dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more))
        return 1;
    /* Gyro and accel data are written to the FIFO by the DMP in chip frame and hardware units.
     * This behavior is convenient because it keeps the gyro and accel outputs of dmp_read_fifo and mpu_read_fifo consistent.
     **/
    /*if (sensors & INV_XYZ_GYRO )
    send_packet(PACKET_TYPE_GYRO, gyro);
    if (sensors & INV_XYZ_ACCEL)
    send_packet(PACKET_TYPE_ACCEL, accel); */
    /* Unlike gyro and accel, quaternions are written to the FIFO in the body frame, q30.
     * The orientation is set by the scalar passed to dmp_set_orientation during initialization.
     **/
    if (sensors & INV_WXYZ_QUAT)
    {
        q0 = quat[0] / q30; // q30格式转换为浮点数
        q1 = quat[1] / q30;
        q2 = quat[2] / q30;
        q3 = quat[3] / q30;
        //计算得到俯仰角/横滚角/航向角
        *pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;                                    // pitch
        *roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3;     // roll
        *yaw = atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3; // yaw
    }
    else
        return 2;
    return 0;
}
