#include "sys_display.h"
#include "OLED.h"
#include "RTC.h"
#include <stdio.h>

/*
 * 显示系统空闲状态。
 * 采样停止后调用，用来告诉操作者当前没有在采样。
 */
void Sys_DisplayIdle(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, (u8 *)"system idle", 16);
    OLED_Refresh();
}

/*
 * 显示一次采样结果。
 * 第一行显示 RTC 时间，第二行显示换算后的 Ch0 电压。
 * voltage_mv：Ch0 电压，单位 mV。
 */
void Sys_DisplaySample(uint32_t voltage_mv)
{
    char time_buf[12];
    char voltage_buf[20];

    RTC_GetTimeString(time_buf);
    sprintf(voltage_buf, "%lu.%02lu V", (unsigned long)(voltage_mv / 1000U), (unsigned long)((voltage_mv % 1000U) / 10U));

    OLED_Clear();
    OLED_ShowString(0, 0, (u8 *)time_buf, 16);
    OLED_ShowString(0, 16, (u8 *)voltage_buf, 16);
    OLED_Refresh();
}
