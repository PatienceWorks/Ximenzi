#ifndef __SYS_SAMPLE_H
#define __SYS_SAMPLE_H

#include <stdint.h>

/*
 * sys_sample
 *
 * 功能定位：
 *   采样状态机模块。
 *
 * 后续需要实现：
 *   1. start 命令启动周期采样。
 *   2. stop 命令停止周期采样。
 *   3. KEY1 按下后切换采样启停状态。
 *   4. KEY2/KEY3/KEY4 切换采样周期 5s/10s/15s。
 *   5. 采样状态下 LED1 以 1s 周期闪烁。
 *   6. 停止采样时 LED1 常灭。
 *   7. 到达采样周期时读取 ADC、计算电压、刷新 OLED、串口输出。
 *   8. 电压超过 limit 时点亮 LED2 并输出 OverLimit 提示。
 */

void Sys_SampleInit(void);
void Sys_StartSample(void);
void Sys_StopSample(void);
void Sys_ToggleSample(void);
uint8_t Sys_IsSampling(void);
void Sys_SampleTask(void);

#endif
