#ifndef __SYS_CONFIG_H
#define __SYS_CONFIG_H

#include <stdint.h>

/*
 * sys_config
 *
 * 功能定位：
 *   系统参数管理模块。
 *
 * 后续需要管理的参数：
 *   1. ratio：变比，范围 0-100，float。
 *   2. limit：报警阈值，范围 0-500，float。
 *   3. sample period：采样周期，题目要求 5s/10s/15s。
 *
 * 后续需要实现：
 *   1. 参数合法性判断。
 *   2. 默认参数初始化。
 *   3. 和 sys_storage 配合，实现掉电保存和上电恢复。
 */

void Sys_ConfigInit(void);

void Sys_SetRatio(float ratio);
float Sys_GetRatio(void);

void Sys_SetLimit(float limit);
float Sys_GetLimit(void);

void Sys_SetSamplePeriod(uint16_t period_s);
uint16_t Sys_GetSamplePeriod(void);

#endif
