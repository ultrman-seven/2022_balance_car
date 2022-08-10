#ifndef _SEEKFREE_IPS114_SPI_H
#define _SEEKFREE_IPS114_SPI_H

#include "common.h"

//-----------------引脚定义------------------------------
#define IPS114_SPIN 0            // 定义使用的SPI号
#define IPS114_SCL_PIN SPI2_SCK_C11  // 定义SPI_SCK引脚
#define IPS114_SDA_PIN SPI2_MOSI_C12 // 定义SPI_MOSI引脚

#define IPS114_REST_PIN C13 // 液晶复位引脚定义 由于实际通讯未使用B2因此 这里复用为GPIO
#define IPS114_DC_PIN C14   // 液晶命令位引脚定义
#define IPS114_CS_PIN C15   // 定义SPI_CS引脚
#define IPS114_BL_PIN C10   // 液晶背光引脚定义

//#define IPS114_DC(x) ((x == 0) ? (GPIO_PIN_RESET(IPS114_DC_PIN)) : (GPIO_PIN_SET(IPS114_DC_PIN)))
//#define IPS114_REST(x) ((x == 0) ? (GPIO_PIN_RESET(IPS114_REST_PIN)) : (GPIO_PIN_SET(IPS114_REST_PIN)))
//#define IPS114_CS(x) ((x == 0) ? (GPIO_PIN_RESET(IPS114_CS_PIN)) : (GPIO_PIN_SET(IPS114_CS_PIN)))
#define IPS114_BL(x) ((x == 0) ? (GPIO_PIN_RESET(IPS114_BL_PIN)) : (GPIO_PIN_SET(IPS114_BL_PIN)))

//定义写字笔的颜色
#define IPS114_PENCOLOR RED

//定义背景颜色
#define WHITE 0x00
#define IPS114_BGCOLOR WHITE

#define IPS114_W 135
#define IPS114_H 240

//定义显示方向
// 0 竖屏模式
// 1 竖屏模式  旋转180
// 2 横屏模式
// 3 横屏模式  旋转180
#define IPS114_DISPLAY_DIR 3

#if (0 == IPS114_DISPLAY_DIR || 1 == IPS114_DISPLAY_DIR)
#define IPS114_X_MAX IPS114_W //液晶X方宽度
#define IPS114_Y_MAX IPS114_H //液晶Y方宽度

#elif (2 == IPS114_DISPLAY_DIR || 3 == IPS114_DISPLAY_DIR)
#define IPS114_X_MAX IPS114_H //液晶X方宽度
#define IPS114_Y_MAX IPS114_W //液晶Y方宽度

#else
#error "IPS114_DISPLAY_DIR 定义错误"
#endif

void ips114_init(void);
void ips114_clear(uint16_t color);
void ips114_drawpoint(uint16_t x, uint16_t y, uint16_t color);
void ips114_showchar(uint16_t x, uint16_t y, const int8_t dat);
void ips114_displayimage032(uint8_t *p, uint16_t width, uint16_t height);
void ips114_displayimage032_zoom(uint8_t *p, uint16_t width, uint16_t height, uint16_t dis_width, uint16_t dis_height);

#endif
