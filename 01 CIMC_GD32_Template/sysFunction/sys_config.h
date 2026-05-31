#ifndef __SYS_CONFIG_H
#define __SYS_CONFIG_H

#include <stdint.h>

/*
 * sys_config：系统参数管理模块。
 *
 * 当前管理的参数：
 *   ratio：比例系数，用来把 ADC 实测电压换算成题目中的 Ch0 电压。
 *   limit：超限阈值，Ch0 电压超过该值时触发 OverLimit。
 *   sample period：采样周期，目前支持 5s、10s、15s。
 *
 * 这个模块只保存和提供参数，不直接操作 ADC、OLED、串口或 Flash。
 * 掉电保存和上电恢复后面由 sys_storage 调用本模块的接口完成。
 */

void Sys_ConfigInit(void);

void Sys_SetRatio(float ratio);
float Sys_GetRatio(void);

void Sys_SetLimit(float limit);
float Sys_GetLimit(void);

void Sys_SetSamplePeriod(uint16_t period_s);
uint16_t Sys_GetSamplePeriod(void);

#endif
