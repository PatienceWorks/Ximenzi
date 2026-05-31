#ifndef __SYS_COMMAND_H
#define __SYS_COMMAND_H

/*
 * sys_command：串口命令解析模块。
 *
 * 作用：
 *   1. 接收主循环传进来的串口命令字符串。
 *   2. 判断命令属于 start、stop、test、RTC now、RTC Config、ratio、limit 等哪一种。
 *   3. 对需要二次输入的命令保存临时状态，例如先输入 ratio，再输入新的比例系数。
 *   4. 根据命令调用 sys_sample、sys_config、RTC、ADC、Flash、TF 卡等模块完成实际功能。
 *
 * 注意：
 *   这里不直接负责 USART 底层收发，串口底层驱动仍然放在 HardWare/USART。
 */

/*
 * 处理一条完整串口命令。
 * 参数 cmd：串口接收到的一整条字符串，通常由 Function.c 在 recv_flag 置位后传入。
 */
void Sys_CommandProcess(const char *cmd);

#endif
