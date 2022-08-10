#ifndef CAE6C501_2196_4267_A313_5C1A291C7335
#define CAE6C501_2196_4267_A313_5C1A291C7335

#include "common.h"
#include "generalO.hpp"
#include "wy_gpio.hpp"
#define IPS114_W 135
#define IPS114_H 240
#define IPS114_X_MAX IPS114_H
#define IPS114_Y_MAX IPS114_W
#define IPS114_DISPLAY_DIR 2
namespace ips
{
    class IpsObject : public genO::generalOutputBase
    {
    private:
        GPIO::Gpio_Object *cs, *dc, *res;
        void (*sd_dat)(uint8_t) = nullptr;
        virtual void char_display(const uint8_t *ptr_pic, bool contrast, uint8_t l, uint8_t word, uint8_t h, uint8_t w);
        virtual void char_display(uint8_t *ptr_pic, bool contrast, uint8_t l, uint8_t word, uint8_t h, uint8_t w);
        void sendByte(uint8_t dat, bool isCMD);
        void sendDoubleData(uint16_t dat);
        void setRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
        void init(void);
        void showBitPic(const uint8_t *pic, uint16_t colStart, uint16_t lineStart, uint16_t height, uint16_t width);

    public:
        IpsObject() = default;
        IpsObject(const char *dc, const char *cs, const char *res,
                  void (*f)(uint8_t), void (*bk)(uint16_t) = nullptr);
        void clear(uint16_t color);
        virtual void clear(void) { clear(0xffff); }
        void findPoint(uint16_t x, uint16_t y);
        void displayImage(uint8_t *p, uint16_t width, uint16_t height, uint16_t sx = 0, uint16_t sy = 0)
        {
            uint32_t i, j;

            uint16_t color = 0;
            uint16_t temp = 0;

            uint16_t coord_x = 0;
            uint16_t coord_y = 0;

            coord_x = width > IPS114_X_MAX ? IPS114_X_MAX : width;
            coord_y = height > IPS114_Y_MAX ? IPS114_Y_MAX : height;
            setRegion(sx, sy, sx + coord_x - 1, sy + coord_y - 1);

            for (j = 0; j < coord_y; j++)
            {
                for (i = 0; i < coord_x; i++)
                {
                    temp = *(p + j * width + i * width / coord_x); // 读取像素点
                    color = (0x001f & ((temp) >> 3)) << 11;
                    color = color | (((0x003f) & ((temp) >> 2)) << 5);
                    color = color | (0x001f & ((temp) >> 3));
                    this->sendDoubleData(color);
                }
            }
        }
    };
} // namespace ips

#endif /* CAE6C501_2196_4267_A313_5C1A291C7335 */
