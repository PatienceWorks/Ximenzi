#ifndef __SYS_COMMAND_H
#define __SYS_COMMAND_H

/*
 * sys_command
 *
 * 功能定位：
 *   串口命令解析模块。
 *
 * 后续需要实现：
 *   1. 识别串口输入的命令字符串。
 *   2. 分发 test、RTC Config、RTC now、conf、ratio、limit。
 *   3. 分发 config save、config read、start、stop、hide、unhide。
 *   4. 对需要二次输入的命令保存临时状态，例如 ratio/limit 等待新参数。
 *   5. 调用 sys_config、sys_sample、sys_storage、sys_data 等模块完成实际功能。
 *
 * 注意：
 *   这里不要直接写 USART 底层收发逻辑，USART 底层代码仍放在 HardWare/USART。
 */

void Sys_CommandProcess(const char *cmd);

#endif
