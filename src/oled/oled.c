#include "common.h"
#include "oledio.h"

#define DC_PORT GPIOB
#define DC_PIN GPIO_Pin_15
#define DC DC_PORT, DC_PIN
#define RCC_DC RCC_AHBPeriph_GPIOB

#define RES_PORT GPIOA
#define RES_PIN GPIO_Pin_9
#define RES RES_PORT, RES_PIN
#define RCC_RES RCC_AHBPeriph_GPIOA

#ifndef __NSS_HARD__
#define CS_PORT GPIOB
#define CS_PIN GPIO_Pin_12
#define CS CS_PORT, CS_PIN
#endif
#define RCC_CS RCC_AHBPeriph_GPIOB

#define Flash_CS_PORT GPIOC
#define Flash_CS_PIN GPIO_Pin_3
#define Flash_CS Flash_CS_PORT, Flash_CS_PIN

void dc_res_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHBPeriphClockCmd(RCC_DC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_CS, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_RES, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    gpio.GPIO_Pin = RES_PIN;
    GPIO_Init(RES_PORT, &gpio);

#ifndef __NSS_HARD__
    gpio.GPIO_Pin = CS_PIN;
    GPIO_Init(CS_PORT, &gpio);
#endif

    gpio.GPIO_Pin = DC_PIN;
    GPIO_Init(DC_PORT, &gpio);
    gpio.GPIO_Pin = Flash_CS_PIN;
    GPIO_Init(Flash_CS_PORT, &gpio);
}

#ifdef __SPI_HARD__

void oledInit(void)
{
    GPIO_InitTypeDef init_gpio;
    SPI_InitTypeDef init_spi;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    dc_res_Init();

    // clk
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_3);
    // mosi
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_4);
    // miso
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_1);
#ifdef __NSS_HARD__
    // nss
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_0);
    init_gpio.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
#else
    init_gpio.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13;
#endif
    init_gpio.GPIO_Speed = GPIO_Speed_50MHz;
    init_gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &init_gpio);
    init_gpio.GPIO_Pin = GPIO_Pin_2;
    init_gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &init_gpio);

    init_spi.SPI_Mode = SPI_Mode_Master;
    init_spi.SPI_DataSize = SPI_DataSize_8b;
    init_spi.SPI_DataWidth = SPI_DataWidth_8b;
#ifndef __NSS_HARD__
    init_spi.SPI_NSS = SPI_NSS_Soft;
#else
    init_spi.SPI_NSS = SPI_NSS_Hard;
#endif
    init_spi.SPI_CPOL = SPI_CPOL_Low;
    init_spi.SPI_CPHA = SPI_CPHA_1Edge;
    init_spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    init_spi.SPI_FirstBit = SPI_FirstBit_MSB;

    SPI_Init(SPI2, &init_spi);
    SPI_Cmd(SPI2, ENABLE);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);
}

uint32_t __readWrite(uint8_t dat)
{
    SPI2->TXREG = dat;
    while (!(SPI2->CSTAT & SPI_FLAG_TXEPT))
        ;
    while (!(SPI2->CSTAT & SPI_CSTAT_RXAVL))
        ;
    return (u32)SPI2->RXREG;
}

void sendCMD(uint8_t cmd)
{
#ifndef __NSS_HARD__
    GPIO_ResetBits(CS);
#endif
    GPIO_ResetBits(DC);

    // SPI_SendData(SPI2, cmd);
    __readWrite(cmd);

#ifndef __NSS_HARD__
    GPIO_SetBits(CS);
#endif
}

void sendData(uint8_t dat)
{
#ifndef __NSS_HARD__
    GPIO_ResetBits(CS);
#endif
    GPIO_SetBits(DC);

    // SPI_SendData(SPI2, dat);
    __readWrite(dat);

#ifndef __NSS_HARD__
    GPIO_SetBits(CS);
#endif
}

#else
#define SCK_PORT GPIOB
#define SDA_PORT GPIOB

#define SCK_PIN GPIO_Pin_14
#define SDA_PIN GPIO_Pin_13

#define SCK SCK_PORT, SCK_PIN
#define SDA SDA_PORT, SDA_PIN

#define RCC_SCK RCC_AHBPeriph_GPIOB
#define RCC_SDA RCC_AHBPeriph_GPIOB
void oledInit(void)
{
    GPIO_InitTypeDef init_gpio;

    dc_res_Init();

    RCC_AHBPeriphClockCmd(RCC_SCK, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_SDA, ENABLE);

    init_gpio.GPIO_Pin = SCK_PIN;
    init_gpio.GPIO_Speed = GPIO_Speed_50MHz;
    init_gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SCK_PORT, &init_gpio);

    init_gpio.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT, &init_gpio);
}

void sendCMD(uint8_t cmd)
{
    uint8_t cnt;
    GPIO_ResetBits(CS);
    GPIO_ResetBits(DC);

    for (cnt = 0; cnt < 8; cnt++)
    {
        GPIO_ResetBits(SCK);
        if ((cmd & 0x80) >> 7)
            GPIO_SetBits(SDA);
        else
            GPIO_ResetBits(SDA);
        cmd = cmd << 1;
        __nop();
        GPIO_SetBits(SCK);
        __nop();
        __nop();
        GPIO_ResetBits(SCK);
    }
    GPIO_SetBits(CS);
}

void sendData(uint8_t cmd)
{
    uint8_t cnt;
    GPIO_ResetBits(CS);
    GPIO_SetBits(DC);

    for (cnt = 0; cnt < 8; cnt++)
    {
        GPIO_ResetBits(SCK);
        if ((cmd & 0x80) >> 7)
            GPIO_SetBits(SDA);
        else
            GPIO_ResetBits(SDA);
        cmd = cmd << 1;
        __nop();
        GPIO_SetBits(SCK);
        __nop();
        __nop();
        GPIO_ResetBits(SCK);
    }

    GPIO_SetBits(CS);
}
#endif

void oledReset(void)
{
    uint8_t rstCmd[] = {0xAE, 0x02, 0x10, 0x40,
                        0xB0, 0x81, 0xFF, 0xA1, 0xA4, 0xA6, 0xC8, 0xA8,
                        0x3F, 0xD5, 0x80, 0xD3, 0x00, 0xAD, 0x8B, 0xDA,
                        0x12, 0xDB, 0x40, 0xD9, 0xF1, 0xAF};
    uint8_t cnt;
    GPIO_ResetBits(RES);
    delayMs(100);
    GPIO_SetBits(RES);
    for (cnt = 0; cnt < 26; cnt++)
        sendCMD(rstCmd[cnt]);
}

void setCol(uint8_t col)
{
    sendCMD(0x10 | (col >> 4));
    sendCMD(0x00 | (col & 0x0f));
}

void setPage(uint8_t page)
{
    sendCMD(0xb0 + page);
}

void Screen_FillClear(uint8_t filler)
{
    uint8_t page, col;
    for (page = 0; page < 8; page++)
    {
        setPage(page);
        setCol(2);
        for (col = 0; col < 128; col++)
            sendData(filler);
        // delay(0xff);
    }
}

void Picture_display(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint8_t page, column;

    for (page = pageStart; page < pageStart + (line / 8); page++) // page loop
    {
        setCol(colStart);
        setPage(page);
        for (column = 0; column < col; column++) // column loop
            sendData(*ptr_pic++);
    }
}

void showPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint8_t page, column;

    for (page = pageStart; page < pageStart + (line / 8); page++) // page loop
    {
        setCol(colStart);
        setPage(page);
        for (column = 0; column < col; column++) // column loop
            sendData(*ptr_pic++);
    }
}

void PictureContrastDisplay(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint8_t page, column;

    for (page = pageStart; page < pageStart + (line / 8); page++) // page loop
    {
        setCol(colStart);
        setPage(page);
        for (column = 0; column < col; column++) // column loop
            sendData(~*ptr_pic++);
    }
}

void w25qRead(uint32_t add, uint8_t *buf, uint16_t len)
{
    u32_split address;
    address.val = add;

    GPIO_ResetBits(Flash_CS);
    __readWrite(0x03);
    __readWrite(address.unit[2]);
    __readWrite(address.unit[1]);
    __readWrite(address.unit[0]);
    while (len--)
        *buf++ = __readWrite(0xff);
    GPIO_SetBits(Flash_CS);
}
