#ifndef __SYS_SAMPLE_H
#define __SYS_SAMPLE_H

#include <stdint.h>

/*
 * sys_sample：采样流程控制模块。
 *
 * 作用：
 *   1. 处理 KEY1~KEY4 的业务功能。
 *   2. 控制采样开始、停止和采样周期。
 *   3. 周期性读取 ADC，计算 Ch0 电压。
 *   4. 控制 LED1 采样闪烁、LED2 超限报警。
 *   5. 把采样结果输出到串口、OLED 和 TF 卡。
 *   6. 支持 hide/unhide 加密存储模式。
 */

void Sys_SampleInit(void);
void Sys_StartSample(void);
void Sys_StopSample(void);
void Sys_ToggleSample(void);
uint8_t Sys_IsSampling(void);
void Sys_SetHideMode(uint8_t enable);
uint8_t Sys_GetHideMode(void);
void Sys_SampleTask(void);

#endif
