#include "common.h"
#include "wy_tim.hpp"
#include "wy_gpio.hpp"
#include "wy_nvic.hpp"
#include "wy_key.hpp"
#include "wy_uart.hpp"
#include "wy_dma.hpp"
namespace camera
{
    enum cameraCMD
    {
        INIT = 0,      // 摄像头初始化命令
        AUTO_EXP,      // 自动曝光命令
        EXP_TIME,      // 曝光时间命令
        FPS,           // 摄像头帧率命令
        SET_COL,       // 图像列命令
        SET_ROW,       // 图像行命令
        LR_OFFSET,     // 图像左右偏移命令
        UD_OFFSET,     // 图像上下偏移命令
        GAIN,          // 图像偏移命令
        CONFIG_FINISH, // 非命令位，主要用来占位计数

        COLOR_GET_WHO_AM_I = 0xEF,
        SET_EXP_TIME = 0XF0, // 单独设置曝光时间命令
        GET_STATUS,          // 获取摄像头配置命令
        GET_VERSION,         // 固件版本号命令

        SET_ADDR = 0XFE, // 寄存器地址命令
        SET_DATA         // 寄存器数据命令
    };
#define picHigh 200
#define picWidth 400

    struct cameraInitTypedef
    {
        uint8_t index = 0;
        char *pclk;
        uint8_t pclkAF;
        uint8_t tim, channel;
        char dataPort;
        bool isLowByte;
        char *vsync;
        UART::InitStruct &u;
    };
    class CameraObject
    {
    private:
        UART::UART_Object *cam = nullptr;
        uint8_t img[picHigh * picWidth];
        DMA_Channel_TypeDef *camCH = nullptr;

    public:
        bool enable = false;
        void config(cameraCMD name, uint16_t val);
        void v_int_DMA_enable(void)
        {
            if (this->enable)
                MODIFY_REG(camCH->CCR, DMA_CCR_EN, ENABLE << DMA_CCR_EN_Pos);
        }
        CameraObject() = default;
        CameraObject(cameraInitTypedef &c);
    };

} // namespace camera

using namespace camera;

void CameraObject::config(cameraCMD name, uint16_t val)
{
    u16_split dat;
    dat.val = val;
    uint8_t cmds[4] = {0xa5, name, dat.unit[1], dat.unit[0]};
    this->cam->sendByte(cmds, 4);
    delayMs(5);
}

#define MAX_CAM_NUM 5
CameraObject *camList[MAX_CAM_NUM] = {nullptr};
#define V_INT_FUNCTION(n)               \
    void v_int_callback##n##(void)      \
    {                                   \
        camList[n]->v_int_DMA_enable(); \
    }
V_INT_FUNCTION(0)
V_INT_FUNCTION(1)
V_INT_FUNCTION(2)
V_INT_FUNCTION(3)
V_INT_FUNCTION(4)

void (*v_int_callback_functions[MAX_CAM_NUM])(void) = {v_int_callback0, v_int_callback1, v_int_callback2, v_int_callback3, v_int_callback4};

const dma::DMA_Source tim_dma_source[8][4] = {
    {dma::DMA_TIM1_CC1, dma::DMA_TIM1_CC2, dma::DMA_TIM1_CC3, dma::DMA_TIM1_CC4},
    {dma::DMA_TIM2_CC1, dma::DMA_TIM2_CC2, dma::DMA_TIM2_CC3, dma::DMA_TIM2_CC4},
    {dma::DMA_TIM3_CC1, dma::DMA_TIM3_CC1, dma::DMA_TIM3_CC3, dma::DMA_TIM3_CC4},
    {dma::DMA_TIM4_CC1, dma::DMA_TIM4_CC2, dma::DMA_TIM4_CC3, dma::DMA_TIM4_CC3},
    {dma::DMA_TIM5_CC1, dma::DMA_TIM5_CC2, dma::DMA_TIM5_CC3, dma::DMA_TIM5_CC4},
    {dma::DMA_TIM5_CC1, dma::DMA_TIM5_CC2, dma::DMA_TIM5_CC3, dma::DMA_TIM5_CC4},
    {dma::DMA_TIM5_CC1, dma::DMA_TIM5_CC2, dma::DMA_TIM5_CC3, dma::DMA_TIM5_CC4},
    {dma::DMA_TIM8_CC1, dma::DMA_TIM8_CC2, dma::DMA_TIM8_CC3, dma::DMA_TIM8_CC4},
};
void dma_callback(void)
{
}
CameraObject::CameraObject(cameraInitTypedef &c)
{
    if (c.index >= MAX_CAM_NUM)
        sys::sysErr("camera index is too large!");
    c.isLowByte ? GPIO::lowPinConfig(c.dataPort, GPIO_Mode_IN_FLOATING) : GPIO::highPinConfig(c.dataPort, GPIO_Mode_IN_FLOATING);
    this->cam = new UART::UART_Object(c.u);
    TIM::pClkInit(c.tim, c.channel, c.pclk, c.pclkAF);

    DMA_InitTypeDef d;
    d.DMA_Auto_reload = DMA_Auto_Reload_Disable;
    d.DMA_BufferSize = picHigh * picHigh;
    d.DMA_DIR = DMA_DIR_PeripheralSRC;
    d.DMA_M2M = DMA_M2M_Disable;

    d.DMA_MemoryBaseAddr = (uint32_t)(this->img);
    d.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    d.DMA_MemoryInc = DMA_MemoryInc_Enable;

    d.DMA_Mode = DMA_Mode_Circular;

    d.DMA_PeripheralBaseAddr = (uint32_t)(&(((GPIO_TypeDef *)GPIO::gpios[GPIO::getGPIO_Index(c.dataPort)])->IDR) + (c.isLowByte ? 0 : 2));
    d.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    d.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    d.DMA_Priority = DMA_Priority_VeryHigh;
    camCH = dma::configDMA(tim_dma_source[c.tim - 1][c.channel - 1], &d, dma_callback, 0, 1);

    KEY::KEY_Object v_int(c.vsync);
    v_int.setOption(v_int_callback_functions[c.index]);

    camList[c.index] = this;
}
