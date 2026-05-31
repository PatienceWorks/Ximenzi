#ifndef __SYS_STORAGE_H
#define __SYS_STORAGE_H

#include <stdint.h>

/*
 * sys_storage：题目要求的存储业务模块。
 *
 * 负责：
 *   1. 从 TF 卡读取 config.ini。
 *   2. 把参数保存到外部 SPI Flash，并从 Flash 恢复。
 *   3. 在 TF 卡中创建 sample、overLimit、log、hideData 文件夹。
 *   4. 按 10 条数据一个文件的规则保存采样、超限和加密数据。
 *   5. 记录操作日志，日志编号由外部 Flash 保存，断电后继续递增。
 */

void Sys_StorageInit(void);

void Sys_LoadConfig(void);          /* 从 TF 卡读取 config.ini，命令 conf 使用。 */
void Sys_SaveConfig(void);          /* 串口命令 config save 使用，会打印结果。 */
void Sys_SaveConfigSilent(void);    /* 内部自动保存使用，不额外打印。 */
void Sys_LoadSavedConfig(void);     /* 从 Flash 读取配置，命令 config read 使用。 */
void Sys_LoadSavedConfigSilent(void); /* 上电自动恢复配置使用，不打印串口。 */

void Sys_LogOperation(const char *text);
void Sys_StorageWriteSample(uint16_t adc_raw, uint32_t adc_mv, uint32_t ch0_mv, uint8_t over_limit, uint8_t hide_mode);
void Sys_TfWriteTest(void);

#endif
