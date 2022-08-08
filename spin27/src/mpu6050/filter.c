#include "filter.h"
#include "mpu6050.h"
#include "iicsoft.h"

/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
float K1 = 0.02;
float angle, angle_dot;
float Q_angle = 0.001; // 过程噪声的协方差
float Q_gyro = 0.003;  // 0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵
float R_angle = 0.5;   // 测量噪声的协方差 既测量偏差
float dt = 0.005;	   //
char C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] = {0, 0, 0, 0};
float PP[2][2] = {{1, 0}, {0, 1}};
float Gyro_Balance = 0, Gyro_Turn = 0, Acceleration_Z = 0;

#define PI 3.141592654

void Get_Angle(uint8_t way)
{
	float Accel_Y, Accel_X, Accel_Z, Gyro_Y, Gyro_Z;
	if (way == 1)					  //===DMP的读取在数据采集中断提醒的时候，严格遵循时序要求
	{
		Read_DMP(&MPU_pitch,&MPU_roll,&MPU_yaw);				   //===读取加速度、角速度、倾角
	}
	else
	{
		Gyro_Y = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_YOUT_L);	//读取Y轴陀螺仪
		Gyro_Z = (I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_GYRO_ZOUT_L);	//读取Z轴陀螺仪
		Accel_X = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_XOUT_L); //读取X轴加速度计
		Accel_Z = (I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_ACCEL_ZOUT_L); //读取Z轴加速度计
		if (Gyro_Y > 32768)
			Gyro_Y -= 65536; //数据类型转换  也可通过short强制类型转换
		if (Gyro_Z > 32768)
			Gyro_Z -= 65536; //数据类型转换
		if (Accel_X > 32768)
			Accel_X -= 65536; //数据类型转换
		if (Accel_Z > 32768)
			Accel_Z -= 65536;						  //数据类型转换
		Gyro_Balance = -Gyro_Y;						  //更新平衡角速度
		Accel_Y = atan2(Accel_X, Accel_Z) * 180 / PI; //计算倾角
		Gyro_Y = Gyro_Y / 16.4;						  //陀螺仪量程转换
		if (way == 2)
			Kalman_Filter(Accel_Y, -Gyro_Y); //卡尔曼滤波
		else if (way == 3)
			Yijielvbo(Accel_Y, -Gyro_Y); //互补滤波
		MPU_pitch = angle;			 //更新平衡倾角
		Gyro_Turn = Gyro_Z;				 //更新转向角速度
		Acceleration_Z = Accel_Z;		 //===更新Z轴加速度计
	}
}

/**************************************************************************
函数功能：简易卡尔曼滤波
入口参数：加速度、角速度
返回  值：无
**************************************************************************/
void Kalman_Filter(float Accel, float Gyro)
{
	angle += (Gyro - Q_bias) * dt;			 //先验估计
	Pdot[0] = Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

	Pdot[1] = -PP[1][1];
	Pdot[2] = -PP[1][1];
	Pdot[3] = Q_gyro;
	PP[0][0] += Pdot[0] * dt; // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt; // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	Angle_err = Accel - angle; // zk-先验估计

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + C_0 * PCt_0;

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0; //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle += K_0 * Angle_err;  //后验估计
	Q_bias += K_1 * Angle_err; //后验估计
	angle_dot = Gyro - Q_bias; //输出值(后验估计)的微分=角速度
}

/**************************************************************************
函数功能：一阶互补滤波
入口参数：加速度、角速度
返回  值：无
**************************************************************************/
void Yijielvbo(float angle_m, float gyro_m)
{
	angle = K1 * angle_m + (1 - K1) * (angle + gyro_m * 0.005);
}
