#ifndef __SYS_DISPLAY_H
#define __SYS_DISPLAY_H

/*
 * sys_display
 *
 * 功能定位：
 *   OLED 显示内容组织模块。
 *
 * 后续需要实现：
 *   1. 停止采样时显示 system idle。
 *   2. 采样时第一行显示 RTC 时间 hh:mm:ss。
 *   3. 采样时第二行显示电压 xx.xx V。
 *   4. OLED 显示内容要和串口采样输出保持一致。
 *
 * 注意：
 *   OLED 底层写命令、写数据、画字符不写在这里。
 *   这里只负责决定显示什么内容。
 */

void Sys_DisplayIdle(void);
void Sys_DisplaySample(float voltage);

#endif
