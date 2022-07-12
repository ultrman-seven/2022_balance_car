#include "common.h"
#include "oledio.h"

#define DC_PORT GPIOB
#define DC_PIN GPIO_Pin_15
#define DC DC_PORT, DC_PIN
#define RCC_DC RCC_AHBPeriph_GPIOB
// #define DC_PORT GPIOA
// #define DC_PIN GPIO_Pin_12
// #define DC DC_PORT, DC_PIN
// #define RCC_DC RCC_AHBPeriph_GPIOA

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
#ifndef _IPS_114__
{
    const uint8_t rstCmd[] = {0xAE, 0x02, 0x10, 0x40,
                              0xB0, 0x81, 0xFF, 0xA1, 0xA4, 0xA6, 0xC8, 0xA8,
                              0x3F, 0xD5, 0x80, 0xD3, 0x00, 0xAD, 0x8B, 0xDA,
                              0x12, 0xDB, 0x40, 0xD9, 0xF1, 0xAF};
    // const uint8_t rstCmd[] = {0xae, 0x00, 0x10, 0x40, 0x81,
    //                           0x7f, 0xa1, 0xc8, 0xa6, 0xa8, 0x3f, 0xd3,
    //                           0x00, 0xd5, 0x80, 0xd9, 0xf1, 0xda, 0x12, 0xdb,
    //                           0x40, 0x20, 0x02, 0x8d, 0x14, 0xa4, 0xa6, 0xaf};
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
void showGrayPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint8_t page, column;
    uint8_t dat;
    int8_t i;
    for (page = pageStart; page < pageStart + (line / 8); page++) // page loop
    {
        setCol(colStart);
        setPage(page);
        for (column = 0; column < col; column++) // column loop
        // sendData(*ptr_pic++);
        {
            dat = 0;
            for (i = page * 8 + 7; i >= page * 8; i--)
            {
                dat <<= 1;
                dat += ptr_pic[col * i + column] ? 1 : 0;
            }
            sendData(dat);
        }
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

#else
{
#define ips114_writeIndex sendCMD
#define ips114_writeData sendData
#define systick_delay_ms delayMs
    GPIO_ResetBits(RES);
    delayMs(200);
    GPIO_SetBits(RES);

    delayMs(100);

    ips114_writeIndex(0x36);
    systick_delay_ms(100);
    ips114_writeData(0x00);

    ips114_writeIndex(0x3A);
    ips114_writeData(0x05);

    ips114_writeIndex(0xB2);
    ips114_writeData(0x0C);
    ips114_writeData(0x0C);
    ips114_writeData(0x00);
    ips114_writeData(0x33);
    ips114_writeData(0x33);

    ips114_writeIndex(0xB7);
    ips114_writeData(0x35);

    ips114_writeIndex(0xBB);
    ips114_writeData(0x37);

    ips114_writeIndex(0xC0);
    ips114_writeData(0x2C);

    ips114_writeIndex(0xC2);
    ips114_writeData(0x01);

    ips114_writeIndex(0xC3);
    ips114_writeData(0x12);

    ips114_writeIndex(0xC4);
    ips114_writeData(0x20);

    ips114_writeIndex(0xC6);
    ips114_writeData(0x0F);

    ips114_writeIndex(0xD0);
    ips114_writeData(0xA4);
    ips114_writeData(0xA1);

    ips114_writeIndex(0xE0);
    ips114_writeData(0xD0);
    ips114_writeData(0x04);
    ips114_writeData(0x0D);
    ips114_writeData(0x11);
    ips114_writeData(0x13);
    ips114_writeData(0x2B);
    ips114_writeData(0x3F);
    ips114_writeData(0x54);
    ips114_writeData(0x4C);
    ips114_writeData(0x18);
    ips114_writeData(0x0D);
    ips114_writeData(0x0B);
    ips114_writeData(0x1F);
    ips114_writeData(0x23);

    ips114_writeIndex(0xE1);
    ips114_writeData(0xD0);
    ips114_writeData(0x04);
    ips114_writeData(0x0C);
    ips114_writeData(0x11);
    ips114_writeData(0x13);
    ips114_writeData(0x2C);
    ips114_writeData(0x3F);
    ips114_writeData(0x44);
    ips114_writeData(0x51);
    ips114_writeData(0x2F);
    ips114_writeData(0x1F);
    ips114_writeData(0x1F);
    ips114_writeData(0x20);
    ips114_writeData(0x23);

    ips114_writeIndex(0x21);

    ips114_writeIndex(0x11);
    systick_delay_ms(120);

    ips114_writeIndex(0x29);
}
void ips114_writedata_16bit(uint16_t dat)
{
    u16_split tmp;
    tmp.val = dat;
    GPIO_ResetBits(CS);
    GPIO_SetBits(DC);

    __readWrite(tmp.unit[1]);
    __readWrite(tmp.unit[0]);

    GPIO_SetBits(CS);
}
void ips114_set_region(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{

    ips114_writeIndex(0x2a); // 列地址设置
    ips114_writedata_16bit(x1 + 52);
    ips114_writedata_16bit(x2 + 52);
    ips114_writeIndex(0x2b); // 行地址设置
    ips114_writedata_16bit(y1 + 40);
    ips114_writedata_16bit(y2 + 40);
    ips114_writeIndex(0x2c); // 储存器写
    // ips114_writeIndex(0x2a); // 列地址设置
    // ips114_writedata_16bit(x1 + 40);
    // ips114_writedata_16bit(x2 + 40);
    // ips114_writeIndex(0x2b); // 行地址设置
    // ips114_writedata_16bit(y1 + 52);
    // ips114_writedata_16bit(y2 + 52);
    // ips114_writeIndex(0x2c);
}

// #define IPS114_W 240
// #define IPS114_H 135
#define IPS114_W 135
#define IPS114_H 240
void ips114_clear(uint16_t color)
{
    uint16_t i, j;
    ips114_set_region(0, 0, IPS114_W - 1, IPS114_H - 1);
    for (i = 0; i < IPS114_H; i++)
    {
        for (j = 0; j < IPS114_W; j++)
        {
            ips114_writedata_16bit(color);
        }
    }
}

void Screen_FillClear(uint8_t filler)
{
    uint16_t i, j;
    ips114_set_region(0, 0, IPS114_W - 1, IPS114_H - 1);
    for (i = 0; i < IPS114_H; i++)
        for (j = 0; j < IPS114_W; j++)
            ips114_writedata_16bit(0x0000 & filler);
}

void PictureContrastDisplay(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint16_t i, j, k;
    uint16_t dat;
    for (i = 0; i < col; i++)
    {
        ips114_set_region(i + colStart, pageStart * 8, colStart + i, pageStart * 8 + line);
        for (j = 0; j < line / 8; j++)
        {
            dat = ptr_pic[i + j * col];
            k = 8;
            while (k--)
            {
                ips114_writedata_16bit(dat & 0x01 ? 0x0000 : 0xffff);
                dat >>= 1;
            }
        }
    }
}
void Picture_display(const uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint16_t i, j, k;
    uint16_t dat;
    for (i = 0; i < col; i++)
    {
        ips114_set_region(i + colStart, pageStart * 8, colStart + i, pageStart * 8 + line);
        for (j = 0; j < line / 8; j++)
        {
            dat = ptr_pic[i + j * col];
            k = 8;
            while (k--)
            {
                ips114_writedata_16bit(dat & 0x01 ? 0xffff : 0x0000);
                dat >>= 1;
            }
        }
    }
}
void showPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint16_t i, j, k;
    uint16_t dat;
    for (i = 0; i < col; i++)
    {
        ips114_set_region(i + colStart, pageStart * 8, colStart + i, pageStart * 8 + line);
        for (j = 0; j < line / 8; j++)
        {
            dat = ptr_pic[i + j * col];
            k = 8;
            while (k--)
            {
                ips114_writedata_16bit(dat & 0x01 ? 0xffff : 0x0000);
                dat >>= 1;
            }
        }
    }
}

uint16_t gray2rgb16(uint8_t c)
{
    uint16_t temp = c;
    uint16_t color = 0;
    color = (0x001f & ((temp) >> 3)) << 11;
    color = color | (((0x003f) & ((temp) >> 2)) << 5);
    color = color | (0x001f & ((temp) >> 3));
    // uint16_t tmp = c * 255 / 31;
    // rgb16 = tmp;
    // rgb16 |= ((tmp << 11) & 0xf800);
    // //tmp = c * 255 / 63;
    // rgb16 |= ((tmp << 5) & 0x07e0);
    return color;
}

void showGrayPic(uint8_t *ptr_pic, uint8_t colStart, uint8_t pageStart, uint8_t line, uint8_t col)
{
    uint16_t i = col * line;
    ips114_set_region(0, 0, col, line);
    while (i--)
        ips114_writedata_16bit(gray2rgb16(*ptr_pic++));
}

#endif

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
