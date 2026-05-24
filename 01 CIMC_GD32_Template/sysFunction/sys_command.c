#include "sys_command.h"

/*
 * Sys_CommandProcess
 *
 * 输入：
 *   cmd：串口收到的一整条命令字符串。
 *
 * 后续需要实现：
 *   1. 判断 cmd 是否等于 test、start、stop 等命令。
 *   2. 根据不同命令调用对应业务函数。
 *   3. 对无效命令返回错误提示。
 *   4. 对带参数命令进行参数解析和范围检查。
 *
 * 示例：
 *   输入 "start"  -> 调用 Sys_StartSample()
 *   输入 "stop"   -> 调用 Sys_StopSample()
 *   输入 "ratio"  -> 打印当前 ratio，并等待用户输入新 ratio
 */
void Sys_CommandProcess(const char *cmd)
{
    (void)cmd;
}
