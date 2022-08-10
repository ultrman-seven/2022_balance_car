#include "ips.hpp"
using namespace ips;

IpsObject::IpsObject(const char *dc, const char *cs, const char *res,
                     void (*f)(uint8_t), void (*bk)(uint16_t))
{
    this->dc = new GPIO::Gpio_Object(dc);
    this->cs = new GPIO::Gpio_Object(cs);
    this->res = new GPIO::Gpio_Object(res);
    this->sd_dat = f;
    this->init();
}

void IpsObject::sendByte(uint8_t dat, bool isCMD)
{
    cs->reset();
    dc->setOnOff(!isCMD);
    if (this->sd_dat != nullptr)
        this->sd_dat(dat);
    cs->set();
}
void IpsObject::sendDoubleData(uint16_t dat)
{
    u16_split tmp;
    tmp.val = dat;
    cs->reset();
    dc->set();
    if (this->sd_dat != nullptr)
    {
        this->sd_dat(tmp.unit[1]);
        this->sd_dat(tmp.unit[0]);
    }
    cs->set();
}

void IpsObject::setRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    this->sendByte(0x2a, true); // 列地址设置
#if (IPS114_DISPLAY_DIR == 0)
    this->sendDoubleData(x1 + 52);
    this->sendDoubleData(x2 + 52);
#elif (IPS114_DISPLAY_DIR == 1)
    this->sendDoubleData(x1 + 53);
    this->sendDoubleData(x2 + 53);
#elif (IPS114_DISPLAY_DIR == 2)
    this->sendDoubleData(x1 + 40);
    this->sendDoubleData(x2 + 40);
#else
    this->sendDoubleData(x1 + 40);
    this->sendDoubleData(x2 + 40);
#endif
    this->sendByte(0x2b, true); // 行地址设置

#if (IPS114_DISPLAY_DIR == 0)
    this->sendDoubleData(y1 + 40);
    this->sendDoubleData(y2 + 40);
#elif (IPS114_DISPLAY_DIR == 1)
    this->sendDoubleData(y1 + 40);
    this->sendDoubleData(y2 + 40);
#elif (IPS114_DISPLAY_DIR == 2)
    this->sendDoubleData(y1 + 53);
    this->sendDoubleData(y2 + 53);
#else
    this->sendDoubleData(y1 + 52);
    this->sendDoubleData(y2 + 52);
#endif
    this->sendByte(0x2c, true);
}
void IpsObject::findPoint(uint16_t x, uint16_t y)
{
    this->setRegion(x, y, x, y);
    this->sendDoubleData(0x1234);
}

void IpsObject::showBitPic(const uint8_t *pic, uint16_t colStart, uint16_t pageStart, uint16_t line, uint16_t col)
{
    uint16_t i, j, k;
    uint16_t dat;
    for (i = 0; i < col; i++)
    {
        this->setRegion(i + colStart, pageStart * 8, colStart + i, pageStart * 8 + line);
        for (j = 0; j < line / 8; j++)
        {
            dat = pic[i + j * col];
            k = 8;
            while (k--)
            {
                this->sendDoubleData(dat & 0x01 ? 0xffff : 0x0000);
                dat >>= 1;
            }
        }
    }
}

void IpsObject::clear(uint16_t color)
{
    uint16_t i;
    setRegion(0, 0, IPS114_X_MAX - 1, IPS114_Y_MAX - 1);
    for (i = 0; i < IPS114_X_MAX * IPS114_Y_MAX; i++)
        // for (j = 0; j < IPS114_Y_MAX; j++)
        sendDoubleData(color);
}
void IpsObject::char_display(const uint8_t *ptr_pic, bool contrast, uint8_t l, uint8_t word, uint8_t h, uint8_t w)
{
    this->showBitPic(ptr_pic, word * this->asciiWide, l, h, w);
}
void IpsObject::char_display(uint8_t *ptr_pic, bool contrast, uint8_t l, uint8_t word, uint8_t h, uint8_t w)
{
    uint16_t i, j, k;
    uint16_t dat;
    for (i = 0; i < w; i++)
    {
        this->setRegion(word * this->asciiWide, l, word * this->asciiWide + w, l + h);
        for (j = 0; j < line / 8; j++)
        {
            dat = ptr_pic[i + j * w];
            k = 8;
            if (contrast)
                while (k--)
                {
                    this->sendDoubleData(dat & 0x01 ? 0xffff : 0x0000);
                    dat >>= 1;
                }
            else
                while (k--)
                {
                    this->sendDoubleData(dat & 0x01 ? 0x0000 : 0xffff);
                    dat >>= 1;
                }
        }
    }
}
void IpsObject::init(void)
{
    this->res->reset();
    delayMs(200);
    this->res->set();
    delayMs(100);

    this->sendByte(0x36, true);
    delayMs(100);
#if (IPS114_DISPLAY_DIR == 0)
    this->sendByte(0x00, false);
#elif (IPS114_DISPLAY_DIR == 1)
    this->sendByte(0xc0, false);
#elif (IPS114_DISPLAY_DIR == 2)
    this->sendByte(0x70, false);
#else
    this->sendByte(0xa0, false);
#endif
    this->sendByte(0x3a, true);
    this->sendByte(0x05, false);

    this->sendByte(0xb2, true);
    this->sendByte(0x0C, false);
    this->sendByte(0x0C, false);
    this->sendByte(0x00, false);
    this->sendByte(0x33, false);
    this->sendByte(0x33, false);

    this->sendByte(0xB7, true);
    this->sendByte(0x35, false);

    this->sendByte(0xBB, true);
    this->sendByte(0x37, false);

    this->sendByte(0xC0, true);
    this->sendByte(0x2C, false);

    this->sendByte(0xC2, true);
    this->sendByte(0x01, false);

    this->sendByte(0xC3, true);
    this->sendByte(0x12, false);

    this->sendByte(0xC4, true);
    this->sendByte(0x20, false);

    this->sendByte(0xC6, true);
    this->sendByte(0x0F, false);

    this->sendByte(0xD0, true);
    this->sendByte(0xA4, false);
    this->sendByte(0xA1, false);

    this->sendByte(0xE0, true);
    this->sendByte(0xD0, false);
    this->sendByte(0x04, false);
    this->sendByte(0x0D, false);
    this->sendByte(0x11, false);
    this->sendByte(0x13, false);
    this->sendByte(0x2B, false);
    this->sendByte(0x3F, false);
    this->sendByte(0x54, false);
    this->sendByte(0x4C, false);
    this->sendByte(0x18, false);
    this->sendByte(0x0D, false);
    this->sendByte(0x0B, false);
    this->sendByte(0x1F, false);
    this->sendByte(0x23, false);

    this->sendByte(0xE1, true);
    this->sendByte(0xD0, false);
    this->sendByte(0x04, false);
    this->sendByte(0x0C, false);
    this->sendByte(0x11, false);
    this->sendByte(0x13, false);
    this->sendByte(0x2C, false);
    this->sendByte(0x3F, false);
    this->sendByte(0x44, false);
    this->sendByte(0x51, false);
    this->sendByte(0x2F, false);
    this->sendByte(0x1F, false);
    this->sendByte(0x1F, false);
    this->sendByte(0x20, false);
    this->sendByte(0x23, false);

    this->sendByte(0x21, true);

    this->sendByte(0x11, true);
    delayMs(120);

    this->sendByte(0x29, true);
}
