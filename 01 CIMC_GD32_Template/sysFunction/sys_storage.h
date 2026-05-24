#ifndef __SYS_STORAGE_H
#define __SYS_STORAGE_H

/*
 * sys_storage
 *
 * 功能定位：
 *   配置文件和掉电保存模块。
 *
 * 后续需要实现：
 *   1. 从 TF 卡读取 config.ini。
 *   2. 解析 Ch0 的 ratio 和 limit。
 *   3. 将 ratio、limit、采样周期保存到外部 Flash。
 *   4. 从外部 Flash 读取已保存配置。
 *   5. 处理文件不存在、TF 卡不存在、Flash 读写失败等异常。
 *
 * 注意：
 *   SD 卡、FatFs、SPI Flash 的底层驱动不写在这里。
 *   这里负责把底层读写组合成题目要求的业务功能。
 */

void Sys_LoadConfig(void);
void Sys_SaveConfig(void);

#endif
