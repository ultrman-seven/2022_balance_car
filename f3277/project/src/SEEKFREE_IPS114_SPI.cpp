#include "SEEKFREE_IPS114_SPI.h"
#include "wy_gpio.hpp"
#include "wy_spi.hpp"

// GPIO::Gpio_Object cs("d8");
// GPIO::Gpio_Object dc("d9");
// GPIO::Gpio_Object rst("d10");

// GPIO::Gpio_Object dc("b14");
// GPIO::Gpio_Object cs("b13");
// GPIO::Gpio_Object rst("b12");
GPIO::Gpio_Object dc("b7");
GPIO::Gpio_Object cs("b8");
GPIO::Gpio_Object rst("b9");
extern SPI::SPI_Object *spi;

#define IPS114_CS(n) cs = n
#define IPS114_DC(n) dc = n
#define IPS114_REST(n) rst = n
#define IPS114_SPIN 0

void spi_mosi(uint8_t a, uint8_t *dat, void *n, uint8_t l)
{
	while (l--)
		spi->writeRead(*dat++);
}
static void ips114_writeIndex(uint8_t dat)
{
	IPS114_CS(0);
	IPS114_DC(0);
	spi_mosi(IPS114_SPIN, &dat, NULL, 1);
	IPS114_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		写数据
// @param		dat				数据
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_writeData(uint8_t dat)
{
	IPS114_CS(0);
	IPS114_DC(1);
	spi_mosi(IPS114_SPIN, &dat, NULL, 1);
	IPS114_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		向液晶屏写一个16位数据
// @param		dat				数据
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_writedata_16bit(uint16_t dat)
{
	uint8_t dat1[2];
	IPS114_CS(0);
	dat1[0] = dat >> 8;
	dat1[1] = (uint8_t)dat;

	IPS114_DC(1);
	spi_mosi(IPS114_SPIN, dat1, NULL, 2); // 写入数据  高位在前  低位在后
	IPS114_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		设置显示区域
// @param		x1				起始x轴坐标
// @param		y1				起始y轴坐标
// @param		x2				结束x轴坐标
// @param		y2				结束y轴坐标
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips114_set_region(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	ips114_writeIndex(0x2a); // 列地址设置
#if (IPS114_DISPLAY_DIR == 0)
	ips114_writedata_16bit(x1 + 52);
	ips114_writedata_16bit(x2 + 52);
	ips114_writeIndex(0x2b); // 行地址设置
	ips114_writedata_16bit(y1 + 40);
	ips114_writedata_16bit(y2 + 40);

#elif (IPS114_DISPLAY_DIR == 1)
	ips114_writedata_16bit(x1 + 53);
	ips114_writedata_16bit(x2 + 53);
	ips114_writeIndex(0x2b); // 行地址设置
	ips114_writedata_16bit(y1 + 40);
	ips114_writedata_16bit(y2 + 40);
#elif (IPS114_DISPLAY_DIR == 2)
	ips114_writedata_16bit(x1 + 40);
	ips114_writedata_16bit(x2 + 40);
	ips114_writeIndex(0x2b); // 行地址设置
	ips114_writedata_16bit(y1 + 53);
	ips114_writedata_16bit(y2 + 53);
#else
	ips114_writedata_16bit(x1 + 40);
	ips114_writedata_16bit(x2 + 40);
	ips114_writeIndex(0x2b); // 行地址设置
	ips114_writedata_16bit(y1 + 52);
	ips114_writedata_16bit(y2 + 52);
#endif
	ips114_writeIndex(0x2c); // 储存器写
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		1.14寸 IPS液晶初始化
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
#define systick_delay_ms delayMs

#define IPS114_BGCOLOR 0x0000
void ips114_init(void)
{

	IPS114_REST(0);
	systick_delay_ms(200);

	IPS114_REST(1);
	systick_delay_ms(100);

	ips114_writeIndex(0x36);
	systick_delay_ms(100);
#if (IPS114_DISPLAY_DIR == 0)
	ips114_writeData(0x00);
#elif (IPS114_DISPLAY_DIR == 1)
	ips114_writeData(0xC0);
#elif (IPS114_DISPLAY_DIR == 2)
	ips114_writeData(0x70);
#else
	ips114_writeData(0xA0);
#endif
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
	ips114_clear(IPS114_BGCOLOR);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		液晶清屏函数
// @param		color			颜色设置
// @return		void
// @since		v1.0
// Sample usage:				ips114_clear(YELLOW);// 全屏设置为黄色
//-------------------------------------------------------------------------------------------------------------------
void ips114_clear(uint16_t color)
{
	uint16_t i, j;
	ips114_set_region(0, 0, IPS114_X_MAX - 1, IPS114_Y_MAX - 1);
	for (i = 0; i < IPS114_X_MAX; i++)
	{
		for (j = 0; j < IPS114_Y_MAX; j++)
		{
			ips114_writedata_16bit(color);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		液晶画点
// @param		x				坐标x方向的起点
// @param		y				坐标y方向的起点
// @param		dat				需要显示的颜色
// @return		void
// @since		v1.0
// Sample usage:				ips114_drawpoint(0,0,RED);  //坐标0,0画一个红色的点
//-------------------------------------------------------------------------------------------------------------------
void ips114_drawpoint(uint16_t x, uint16_t y, uint16_t color)
{
	ips114_set_region(x, y, x, y);
	ips114_writedata_16bit(color);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		液晶显示字符
// @param		x				坐标x方向的起点 参数范围 0 -（IPS114_X_MAX-1）
// @param		y				坐标y方向的起点 参数范围 0 -（IPS114_Y_MAX/16-1）
// @param		dat				需要显示的字符
// @return		void
// @since		v1.0
// Sample usage:				ips114_showchar(0,0,'x');//坐标0,0写一个字符x
//-------------------------------------------------------------------------------------------------------------------
void ips114_showchar(uint16_t x, uint16_t y, const int8_t dat)
{
	// uint8_t i, j;
	// uint8_t temp;

	// for (i = 0; i < 16; i++)
	// {
	// 	ips114_set_region(x, y + i, x + 7, y + i);
	// 	temp = tft_ascii[dat - 32][i]; // 减32因为是取模是从空格开始取得 空格在ascii中序号是32
	// 	for (j = 0; j < 8; j++)
	// 	{
	// 		if (temp & 0x01)
	// 			ips114_writedata_16bit(IPS114_PENCOLOR);
	// 		else
	// 			ips114_writedata_16bit(IPS114_BGCOLOR);
	// 		temp >>= 1;
	// 	}
	// }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		总钻风(灰度摄像头)液晶显示函数
// @param		*p				图像数组地址
// @param		width			图像宽度
// @param		height			图像高度
// @return		void
// @since		v1.0
// Sample usage:				ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
// @note		图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void ips114_displayimage032(uint8_t *p, uint16_t width, uint16_t height)
{
	uint32_t i, j;

	uint16_t color = 0;
	uint16_t temp = 0;

	uint16_t coord_x = 0;
	uint16_t coord_y = 0;

	coord_x = width > IPS114_X_MAX ? IPS114_X_MAX : width;
	coord_y = height > IPS114_Y_MAX ? IPS114_Y_MAX : height;
	ips114_set_region(0, 0, coord_x - 1, coord_y - 1);

	for (j = 0; j < coord_y; j++)
	{
		for (i = 0; i < coord_x; i++)
		{
			temp = *(p + j * width + i * width / coord_x); // 读取像素点
			color = (0x001f & ((temp) >> 3)) << 11;
			color = color | (((0x003f) & ((temp) >> 2)) << 5);
			color = color | (0x001f & ((temp) >> 3));
			ips114_writedata_16bit(color);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		总钻风(灰度摄像头)液晶缩放显示函数
// @param		*p				图像数组地址
// @param		width			图像宽度
// @param		height			图像高度
// @param		dis_width		图像显示宽度  1 -（IPS114_X_MAX）
// @param		dis_height		图像显示高度  1 -（IPS114_Y_MAX）
// @return		void
// @since		v1.0
// Sample usage:		ips114_displayimage032_zoom(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H, MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
// @note				图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void ips114_displayimage032_zoom(uint8_t *p, uint16_t width, uint16_t height, uint16_t dis_width, uint16_t dis_height)
{
	uint32_t i, j;

	uint16_t color = 0;
	uint16_t temp = 0;

	ips114_set_region(0, 0, dis_width - 1, dis_height - 1); //设置显示区域

	for (j = 0; j < dis_height; j++)
	{
		for (i = 0; i < dis_width; i++)
		{
			temp = *(p + (j * height / dis_height) * width + i * width / dis_width); // 读取像素点
			color = (0x001f & ((temp) >> 3)) << 11;
			color = color | (((0x003f) & ((temp) >> 2)) << 5);
			color = color | (0x001f & ((temp) >> 3));
			ips114_writedata_16bit(color);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		总钻风(灰度摄像头)液晶缩放显示函数
// @param		*p				图像数组地址
// @param		width			图像宽度
// @param		height			图像高度
// @param		start_x			设置显示起点的x轴坐标
// @param		start_y			设置显示起点的y轴坐标
// @param		dis_width		图像显示宽度  1 -（IPS114_X_MAX）
// @param		dis_height		图像显示高度  1 -（IPS114_Y_MAX）
// @return		void
// @since		v1.0
// Sample usage:				ips114_displayimage032_zoom1(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H, 0, 0, MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//-------------------------------------------------------------------------------------------------------------------
void ips114_displayimage032_zoom1(uint8_t *p, uint16_t width, uint16_t height, uint16_t start_x, uint16_t start_y, uint16_t dis_width, uint16_t dis_height)
{
	uint32_t i, j;

	uint16_t color = 0;
	uint16_t temp = 0;

	//检查设置的参数是否超过屏幕的分辨率
	//	if((start_x+dis_width)>IPS114_X_MAX)	assert_param(0);
	//	if((start_y+dis_height)>IPS114_Y_MAX)	assert_param(0);
	ips114_set_region(start_x, start_y, start_x + dis_width - 1, start_y + dis_height - 1); // 设置显示区域

	for (j = 0; j < dis_height; j++)
	{
		for (i = 0; i < dis_width; i++)
		{
			temp = *(p + (j * height / dis_height) * width + i * width / dis_width); // 读取像素点
			color = (0x001f & ((temp) >> 3)) << 11;
			color = color | (((0x003f) & ((temp) >> 2)) << 5);
			color = color | (0x001f & ((temp) >> 3));
			ips114_writedata_16bit(color);
		}
	}
}
