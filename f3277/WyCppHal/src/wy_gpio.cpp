#include "wy_gpio.hpp"
using namespace GPIO;

const uint32_t gpio_rccs[] = {RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB,
                              RCC_AHBPeriph_GPIOC, RCC_AHBPeriph_GPIOD,
                              RCC_AHBPeriph_GPIOE, RCC_AHBENR_GPIOF,
                              RCC_AHBENR_GPIOG, RCC_AHBENR_GPIOH};

void Gpio_Object::familyInit(char port, uint8_t pin, bool rccOn)
{
    uint8_t tmp;
    this->pin_source = pin;
    this->pin = 0x0001 << pin;
    if (port >= 'a' && port <= 'h')
        tmp = port - 'a';
    else if (port >= 'A' && port <= 'H')
        tmp = port - 'A';

    if (rccOn)
        RCC_AHBPeriphClockCmd(gpio_rccs[tmp], ENABLE);

    this->port = (GPIO_TypeDef *)gpios[tmp];
}
void Gpio_Object::familyInit(const char *n, bool rccOn)
{
    uint8_t tmp;
    this->pin_source = getPinFromStr(n + 1);
    this->pin = 0x0001 << (this->pin_source);
    if (*n >= 'a' && *n <= 'h')
        tmp = *n - 'a';
    else if (*n >= 'A' && *n <= 'H')
        tmp = *n - 'A';

    if (rccOn)
        RCC_AHBPeriphClockCmd(gpio_rccs[tmp], ENABLE);
    this->port = (GPIO_TypeDef *)gpios[tmp];
}

void Gpio_Object::modeConfig(GPIOMode_TypeDef m, GPIOSpeed_TypeDef s)
{
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = m;
    gpio.GPIO_Pin = this->pin;
    gpio.GPIO_Speed = s;
    GPIO_Init(this->port, &gpio);
}

void Gpio_Object::setOnOff(bool on)
{
    if (on)
        this->port->BSRR = this->pin;
    else
        this->port->BRR = this->pin;
}

uint8_t Gpio_Object::read(void)
{
    if ((this->port->IDR & this->pin) != (uint32_t)Bit_RESET)
        return (uint8_t)Bit_SET;
    return (uint8_t)Bit_RESET;
}

namespace GPIO
{
    void turnOnRcc(uint8_t index)
    {
        RCC_AHBPeriphClockCmd(gpio_rccs[index], ENABLE);
    }
    void turnOnRcc(const char n)
    {
        RCC_AHBPeriphClockCmd(gpio_rccs[getGPIO_Index(n)], ENABLE);
    }
    uint8_t getPinFromStr(const char *s)
    {
        uint8_t result = 0;
        while (*s)
        {
            result *= 10;
            result += *s++ - '0';
        }
        return result;
    }
    uint8_t getGPIO_Index(const char n)
    {
        if (n >= 'a' && n <= 'h')
            return n - 'a';
        return n - 'A';
    }
    uint32_t getGPIO_Base(const char n)
    {
        if (n >= 'a' && n <= 'h')
            return gpios[n - 'a'];
        return gpios[n - 'A'];
    }
    void modeConfig(char port, uint8_t pin, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s, bool rcc)
    {
        GPIO_InitTypeDef gpio;

        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0x0001 << pin;
        gpio.GPIO_Speed = s;
        uint8_t idx = getGPIO_Index(port);
        if (rcc)
            RCC_AHBPeriphClockCmd(gpio_rccs[idx], ENABLE);
        GPIO_Init((GPIO_TypeDef *)gpios[idx], &gpio);
    }
    void modeConfig(const char *p, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s, bool rcc)
    {
        GPIO_InitTypeDef gpio;

        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0x0001 << getPinFromStr(p + 1);
        gpio.GPIO_Speed = s;
        uint8_t idx = getGPIO_Index(*p);
        if (rcc)
            RCC_AHBPeriphClockCmd(gpio_rccs[idx], ENABLE);
        GPIO_Init((GPIO_TypeDef *)gpios[idx], &gpio);
    }
    void fullPinConfig(char port, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s)
    {
        GPIO_InitTypeDef gpio;

        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0xffff;
        gpio.GPIO_Speed = s;
        uint8_t idx = getGPIO_Index(port);
        RCC->AHBENR |= gpio_rccs[idx];
        GPIO_Init((GPIO_TypeDef *)gpios[idx], &gpio);
    }
    void lowPinConfig(char p, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s)
    {
        GPIO_InitTypeDef gpio;

        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0x00ff;
        gpio.GPIO_Speed = s;
        uint8_t idx = getGPIO_Index(p);
        RCC->AHBENR |= gpio_rccs[idx];
        GPIO_Init((GPIO_TypeDef *)gpios[idx], &gpio);
    }
    void highPinConfig(char p, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s)
    {
        GPIO_InitTypeDef gpio;

        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0xff00;
        gpio.GPIO_Speed = s;
        uint8_t idx = getGPIO_Index(p);
        RCC->AHBENR |= gpio_rccs[idx];
        GPIO_Init((GPIO_TypeDef *)gpios[idx], &gpio);
    }

    void afConfig(char port, uint8_t pin, uint8_t af, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s, bool rcc)
    {
        GPIO_InitTypeDef gpio;
        uint8_t tmp = getGPIO_Index(port);
        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0x0001 << pin;
        gpio.GPIO_Speed = s;
        if (rcc)
            RCC_AHBPeriphClockCmd(gpio_rccs[tmp], ENABLE);
        GPIO_Init((GPIO_TypeDef *)gpios[tmp], &gpio);
        GPIO_PinAFConfig((GPIO_TypeDef *)gpios[tmp], pin, af);
    }
    void afConfig(const char *g, uint8_t af, GPIOMode_TypeDef m, GPIOSpeed_TypeDef s, bool rcc)
    {
        GPIO_InitTypeDef gpio;
        uint8_t tmp = getGPIO_Index(*g);
        uint8_t pin = getPinFromStr(g + 1);
        GPIO_PinAFConfig((GPIO_TypeDef *)gpios[tmp], pin, af);
        gpio.GPIO_Mode = m;
        gpio.GPIO_Pin = 0x0001 << pin;
        gpio.GPIO_Speed = s;
        if (rcc)
            RCC_AHBPeriphClockCmd(gpio_rccs[tmp], ENABLE);
        GPIO_Init((GPIO_TypeDef *)gpios[tmp], &gpio);
    }

    uint8_t findGPIO_AF(const char *g, const GPIO_AF_STRUCT *afs)
    {
        uint8_t g_code;
        uint8_t cnt = 0;
        g_code = ((getGPIO_Index(*g) << 4) & 0xf0) + (getPinFromStr(g + 1) & 0x0f);
        while (afs[cnt].gpio_code != 0xff)
        {
            if (afs[cnt].gpio_code == g_code)
                return afs[cnt].af;
            cnt++;
        }
        return 0xff;
    }
}
