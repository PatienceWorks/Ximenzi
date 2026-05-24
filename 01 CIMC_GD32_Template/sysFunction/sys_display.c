#include "sys_display.h"

/*
 * Sys_DisplayIdle
 *
 * 题目要求：
 *   未采样状态下，OLED 第一行显示 "system idle"，第二行为空。
 *
 * 后续需要调用 OLED 驱动实现具体显示。
 */
void Sys_DisplayIdle(void)
{
}

/*
 * Sys_DisplaySample
 *
 * 题目要求：
 *   第一行显示 RTC 时间，格式 hh:mm:ss。
 *   第二行显示电压值，格式 xx.xx V。
 *
 * 后续需要：
 *   1. 从 RTC 模块获取当前时间。
 *   2. 格式化时间字符串。
 *   3. 格式化电压字符串。
 *   4. 调用 OLED 驱动刷新显示。
 */
void Sys_DisplaySample(float voltage)
{
    (void)voltage;
}
