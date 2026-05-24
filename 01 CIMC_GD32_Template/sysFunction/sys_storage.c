#include "sys_storage.h"

/*
 * Sys_LoadConfig
 *
 * 后续需要实现：
 *   1. 上电时从 Flash 读取 ratio、limit、sample_period。
 *   2. conf 命令时从 TF 卡读取 config.ini。
 *   3. 读取成功后更新 sys_config 中的参数。
 *   4. 读取失败时通过串口返回题目要求的提示。
 */
void Sys_LoadConfig(void)
{
}

/*
 * Sys_SaveConfig
 *
 * 后续需要实现：
 *   1. config save 命令调用本函数。
 *   2. 从 sys_config 获取当前 ratio、limit、sample_period。
 *   3. 写入外部 Flash。
 *   4. 写入后可以再次读取校验。
 */
void Sys_SaveConfig(void)
{
}
