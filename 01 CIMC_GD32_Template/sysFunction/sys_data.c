#include "sys_data.h"

/*
 * Sys_CalcVoltage
 *
 * 当前实现：
 *   Ch0 电压 = ADC 实测电压 * ratio。
 *
 * 后续需要根据题目和硬件确认：
 *   1. ADC 原始值是否已经在驱动层转换成 0-3.3V。
 *   2. 是否需要分压系数。
 *   3. 输出是否统一保留两位小数。
 */
float Sys_CalcVoltage(float adc_voltage, float ratio)
{
    return adc_voltage * ratio;
}

/*
 * Sys_IsOverLimit
 *
 * 当前实现：
 *   voltage > limit 时认为超限。
 *
 * 后续需要配合采样模块实现：
 *   1. 超限时 LED2 点亮。
 *   2. 串口输出 OverLimit 和 limit 数值。
 *   3. hide 输出末尾根据题目要求添加标记。
 */
uint8_t Sys_IsOverLimit(float voltage, float limit)
{
    return voltage > limit;
}
