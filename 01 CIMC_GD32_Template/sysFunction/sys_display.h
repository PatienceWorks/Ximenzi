#ifndef __SYS_DISPLAY_H
#define __SYS_DISPLAY_H

#include <stdint.h>

/*
 * sys_display：OLED 显示业务模块。
 *
 * 作用：
 *   1. 系统空闲时显示 idle 状态。
 *   2. 采样时显示 RTC 时间和当前 Ch0 电压。
 *
 * OLED 的底层 I2C/刷屏函数仍然放在 HardWare/OLED，
 * 这里负责决定“显示什么内容”。
 */

void Sys_DisplayIdle(void);
void Sys_DisplaySample(uint32_t voltage_mv);

#endif
