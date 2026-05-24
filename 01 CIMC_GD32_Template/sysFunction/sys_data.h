#ifndef __SYS_DATA_H
#define __SYS_DATA_H

#include <stdint.h>

/*
 * sys_data
 *
 * 功能定位：
 *   数据计算和编码处理模块。
 *
 * 后续需要实现：
 *   1. ADC 实测电压乘以 ratio，得到 Ch0 电压值。
 *   2. 判断 Ch0 电压是否超过 limit。
 *   3. hide 指令：时间戳 + 电压值编码为 HEX 字符串。
 *   4. unhide 指令：HEX 字符串解码回时间和电压。
 *   5. Unix 时间戳转换。
 *
 * 注意：
 *   ADC 底层采样不写在这里；这里只负责数据处理。
 */

float Sys_CalcVoltage(float adc_voltage, float ratio);
uint8_t Sys_IsOverLimit(float voltage, float limit);

#endif
