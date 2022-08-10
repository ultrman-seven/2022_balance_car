#ifndef FBB603F1_460A_4998_87FD_8B5DFF17B784
#define FBB603F1_460A_4998_87FD_8B5DFF17B784
#include "common.h"
namespace TIM
{
    class TIM_Base_Object
    {
    private:
        TIM_TypeDef *tim;
        void (**updateFunc)(void) = nullptr;
        uint8_t updateNVIC_Channel;

    public:
        TIM_Base_Object() = default;
        TIM_Base_Object(uint8_t num, uint32_t us = 10);
        void enable(bool e = true) { TIM_Cmd(this->tim, e ? ENABLE : DISABLE); }
        void setUsCountTime(uint16_t us) { setCountTime(us, SYS_FREQ_MHz - 1, true); };
        void setMsCountTime(uint16_t ms) { setCountTime(ms * 100, SYS_FREQ_MHz * 100 - 1, true); };
        void setCountTime(uint16_t p, uint16_t s, bool e);
        void setNVIC_Enable(bool e = true, uint8_t p = 0, uint8_t sub_p = 0);
        void setUpdateFunc(void (*f)(void)) { *(this->updateFunc) = f; }
        FlagStatus getFlagStatus(TIMFLAG_Typedef flag) { return TIM_GetFlagStatus(this->tim, flag); }
    };

    void timBaseConfig(uint8_t timNum, uint32_t usPeriod);
    void timBaseConfig(TIM_TypeDef *tim, uint32_t usPeriod);
    void timIcInit(uint8_t tim, uint8_t ch);
    void pClkInit(uint8_t tim, uint8_t ch, const char *p, uint8_t af);
} // TIM

#endif /* FBB603F1_460A_4998_87FD_8B5DFF17B784 */
