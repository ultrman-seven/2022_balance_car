#include "common.h"

void batteryInit(void)
{
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef adc;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpio);

    ADC_StructInit(&adc);
    // adc.ADC_Mode=ADC_Mode_Single_Period;
    ADC_Init(ADC2, &adc);

    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 0, ADC_SampleTime_239_5Cycles);

    // ADC_DMACmd(ADC2,ENABLE);
    ADC_Cmd(ADC2, ENABLE);
}

float getVoltage(void)
{
    float result;
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
    while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
        ;
    // result = ((double)ADC_GetConversionValue(ADC2) * 14.85) / (double)4095;
    result = ADC_GetConversionValue(ADC2);
    ADC_SoftwareStartConvCmd(ADC2, DISABLE);
    ADC_ClearFlag(ADC2, ADC_FLAG_EOC);
    return result;
}

uint8_t getPercentElectricity(void)
{
    uint16_t adcResult;
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
    while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
        ;
    adcResult = ADC_GetConversionValue(ADC2);
    ADC_SoftwareStartConvCmd(ADC2, DISABLE);
    ADC_ClearFlag(ADC2, ADC_FLAG_EOC);

    return (1.1 / (double)adcResult);
}
