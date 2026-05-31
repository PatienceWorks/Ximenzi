#ifndef __SYS_DATA_H
#define __SYS_DATA_H

#include <stdint.h>

/*
 * sys_data：数据计算和判断模块。
 *
 * 作用：
 *   1. 把 ADC 采到的电压乘以 ratio，换算成题目要求的 Ch0 电压。
 *   2. 判断 Ch0 电压是否超过 limit。
 *
 * 这里不读取硬件，也不显示数据，只做纯计算，方便后面测试和复用。
 */

float Sys_CalcVoltage(float adc_voltage, float ratio);
uint32_t Sys_CalcVoltageMillivolt(uint32_t adc_mv, float ratio);
uint8_t Sys_IsOverLimit(float voltage, float limit);
uint8_t Sys_IsOverLimitMillivolt(uint32_t voltage_mv, float limit);

#endif
