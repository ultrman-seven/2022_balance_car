#include "wy_commu.h"

WY_16Line_Communicate::WY_16Line_Communicate(const char port, const char *ok)
{
    this->port = (GPIO_TypeDef *)GPIO::getGPIO_Base(port);
    this->ok = new GPIO::Gpio_Object(ok);
    GPIO::fullPinConfig(port);
}

void WY_16Line_Communicate::sendData(uint16_t dat)
{
    this->port->ODR = dat;
    delay(15);
    this->ok->set();
    delay(50);
    this->ok->reset();
}
