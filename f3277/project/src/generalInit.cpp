#include "genObj.hpp"
#include "SEEKFREE_IPS114_SPI.h"
#include "camera.h"
#include "wy_commu.h"
#include "wy_key.hpp"

GPIO::Gpio_Object sysLED("b1");
ips::IpsObject *screen = nullptr;
SPI::SPI_Object *spi = nullptr;
WY_16Line_Communicate spin27('f', "c1");
void genInit(void)
{
    spi = new SPI::SPI_Object(1, "b5", "b4", "b3");
    spi = new SPI::SPI_Object(1, "b5", "b4", "b3");
    screen = new ips::IpsObject("b7", "b8", "b9", [](uint8_t d)
                                { spi->writeRead(d); });
    ips114_init();
    screen->clear(0xffff);
    cameraInit();
    cameraOn();
    KEY::KEY_Object request("c0");
    request.setOption([]()
                      { spin27.sendData(camResult.x);
                        delay(5);
                        spin27.sendData(camResult.y); });
    sys::throwFun2Main(cameraPicOption);
}
