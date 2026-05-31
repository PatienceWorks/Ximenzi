#include "sys_data.h"

/*
 * 根据比例系数计算 Ch0 电压。
 * adc_voltage：ADC 引脚实测电压，单位 V。
 * ratio：比例系数。
 * 返回值：换算后的 Ch0 电压，单位 V。
 */
float Sys_CalcVoltage(float adc_voltage, float ratio)
{
    return adc_voltage * ratio;
}

/*
 * 使用 mV 整数计算 Ch0 电压，减少串口打印时的浮点格式问题。
 * adc_mv：ADC 引脚实测电压，单位 mV。
 * ratio：比例系数。
 * 返回值：换算后的 Ch0 电压，单位 mV。
 */
uint32_t Sys_CalcVoltageMillivolt(uint32_t adc_mv, float ratio)
{
    return (uint32_t)((float)adc_mv * ratio);
}

/* 判断电压是否超过阈值，单位 V。 */
uint8_t Sys_IsOverLimit(float voltage, float limit)
{
    return voltage > limit;
}

/*
 * 判断电压是否超过阈值。
 * voltage_mv：当前 Ch0 电压，单位 mV。
 * limit：阈值，单位 V。
 */
uint8_t Sys_IsOverLimitMillivolt(uint32_t voltage_mv, float limit)
{
    return voltage_mv > (uint32_t)(limit * 1000.0f);
}
