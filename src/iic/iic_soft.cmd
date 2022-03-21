#include "common.h"
#include "oledio.h"

#define SCL GPIOB, GPIO_Pin_10
#define SDA GPIOB, GPIO_Pin_11

GPIO_InitTypeDef sda_IN={
    .GPIO_Mode=GPIO_Mode_IPU,
    .GPIO_Pin=GPIO_Pin_11,
    .GPIO_Speed=GPIO_Speed_50MHz
};
GPIO_InitTypeDef sda_OUT={
    .GPIO_Mode=GPIO_Mode_Out_OD,
    .GPIO_Pin=GPIO_Pin_11,
    .GPIO_Speed=GPIO_Speed_50MHz
};
#define MPU_SDA_IN()  {GPIO_Init(GPIOB, &sda_IN);}//{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=4<<12;}
#define MPU_SDA_OUT() {GPIO_Init(GPIOB, &sda_OUT);}//{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=5<<12;}

// 0110 1000
// #define ADDRESS 0x68
#define ADDRESS 0x69
#define MPU_READ ((ADDRESS << 1) | 0x01)
#define MPU_WRITE ((ADDRESS << 1) & 0xfe)

#define GapTime 5
typedef enum
{
    ACK_True = 1,
    ACK_False,
    ACK_Error
} ACK_State;

void iicSoftInit(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio.GPIO_Speed = GPIO_Speed_20MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio);
    GPIO_SetBits(SDA);
    GPIO_SetBits(SCL);
}

void iicStart(void)
{
    GPIO_SetBits(SDA);
    GPIO_SetBits(SCL);
    delay(GapTime);
    GPIO_ResetBits(SDA);
    delay(GapTime);
    GPIO_ResetBits(SCL);
}

void iicEnd(void)
{
    GPIO_ResetBits(SDA);
    GPIO_ResetBits(SCL);
    delay(GapTime);
    GPIO_SetBits(SCL);
    delay(GapTime);
    GPIO_SetBits(SDA);
    delay(GapTime);
}

ACK_State iicSendByte(uint8_t dat)
{
    uint8_t count = 8;
    GPIO_ResetBits(SCL);
    while (count--)
    {
        if (dat & 0x80)
            GPIO_SetBits(SDA);
        else
            GPIO_ResetBits(SDA);
        GPIO_SetBits(SCL);
        delay(GapTime);
        GPIO_ResetBits(SCL);
        delay(GapTime);
        dat <<= 1;
    }
    // ack
    GPIO_SetBits(SDA);
    MPU_SDA_IN();
    delay(GapTime);
    GPIO_SetBits(SCL);
    delay(GapTime);
    count = GPIO_ReadInputDataBit(SDA);
    MPU_SDA_OUT();
    if (count)
        return ACK_False;
    else
        return ACK_True;
}

uint8_t iicRecvByte(ACK_State ack)
{
    uint8_t count = 8;
    uint8_t result = 0x00;
    GPIO_ResetBits(SCL);
    delay(GapTime);
    GPIO_SetBits(SDA);
    MPU_SDA_IN();
    while (count--)
    {
        delay(GapTime);
        GPIO_ResetBits(SCL);
        delay(GapTime);
        GPIO_SetBits(SCL);
        delay(GapTime);
        result <<= 1;
        if (GPIO_ReadInputDataBit(SDA))
            result |= 0x01;
        else
            result &= 0xfe;
    }
    GPIO_ResetBits(SCL);
    delay(GapTime);
    MPU_SDA_OUT();
    if (ack == ACK_False)
        GPIO_SetBits(SDA); // nack
    else
        GPIO_ResetBits(SDA);
    delay(GapTime);
    GPIO_SetBits(SCL);
    delay(GapTime);
    return result;
}

int i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data)
{
    iicStart();
    iicSendByte((slave_addr << 1) & 0xfe);
    iicSendByte(reg_addr);
    while (length--)
        iicSendByte(*data++);
    iicEnd();
    return 0;
}
int i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data)
{
    iicStart();
    iicSendByte((slave_addr << 1) & 0xfe);
    iicSendByte(reg_addr);
    iicEnd();
    iicStart();
    iicSendByte((slave_addr << 1) | 0x01);
    while (--length)
        *data++ = iicRecvByte(ACK_True);
    *data = iicRecvByte(ACK_False);
    iicEnd();
    return 0;
}
void iicSendByte2Reg(uint8_t regAdd, uint8_t dat)
{
    iicStart();
    iicSendByte(MPU_WRITE);
    iicSendByte(regAdd);
    iicSendByte(dat);
    iicEnd();
}

uint8_t iicGetByteFromReg(uint8_t regAdd)
{
    uint8_t result;
    iicStart();
    iicSendByte(MPU_WRITE);
    iicSendByte(regAdd);
    iicEnd();
    iicStart();
    iicSendByte(MPU_READ);
    result = iicRecvByte(ACK_False);
    iicEnd();
    return result;
}

void iicGetBytes(uint8_t startAdd, uint8_t *dat, uint8_t len)
{
    uint8_t result;
    iicStart();
    iicSendByte(MPU_WRITE);
    iicSendByte(startAdd);
    iicEnd();
    iicStart();
    iicSendByte(MPU_READ);
    while (--len)
    {
        *dat = iicRecvByte(ACK_True);
        dat++;
    }
    *dat = iicRecvByte(ACK_False);
    iicEnd();
}