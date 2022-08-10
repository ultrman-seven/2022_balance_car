#ifndef DEF663E2_5001_4043_8564_6AA832ECA550
#define DEF663E2_5001_4043_8564_6AA832ECA550
#include "wy_gpio.hpp"
#include "camera.h"

class WY_16Line_Communicate
{
private:
    GPIO_TypeDef *port = nullptr;
    GPIO::Gpio_Object *ok = nullptr;

public:
    WY_16Line_Communicate() = default;
    WY_16Line_Communicate(const char port, const char *ok);

    void sendData(uint16_t dat);
};

#endif /* DEF663E2_5001_4043_8564_6AA832ECA550 */
