#include "sys_config.h"

/*
 * 当前先用 RAM 变量保存参数。
 * 后续如果要实现掉电保存，可以在 Sys_ConfigInit() 里调用 sys_storage 从 Flash 读取参数。
 */
static float g_ratio = 1.0f;              /* Ch0 电压换算比例系数，默认 1.0，表示不放大也不缩小。 */
static float g_limit = 100.0f;            /* Ch0 超限阈值，单位 V，默认 100V。 */
static uint16_t g_sample_period_s = 5;    /* 采样周期，单位 s，默认 5s。 */

/*
 * 初始化系统参数。
 * 当前使用固定默认值，保证上电后即使没有配置文件也能运行。
 */
void Sys_ConfigInit(void)
{
    g_ratio = 1.0f;
    g_limit = 100.0f;
    g_sample_period_s = 5;
}

/* 设置 Ch0 电压换算比例系数。 */
void Sys_SetRatio(float ratio)
{
    g_ratio = ratio;
}

/* 读取当前 Ch0 电压换算比例系数。 */
float Sys_GetRatio(void)
{
    return g_ratio;
}

/* 设置超限阈值，单位 V。 */
void Sys_SetLimit(float limit)
{
    g_limit = limit;
}

/* 读取当前超限阈值，单位 V。 */
float Sys_GetLimit(void)
{
    return g_limit;
}

/* 设置采样周期，单位 s。 */
void Sys_SetSamplePeriod(uint16_t period_s)
{
    g_sample_period_s = period_s;
}

/* 读取当前采样周期，单位 s。 */
uint16_t Sys_GetSamplePeriod(void)
{
    return g_sample_period_s;
}
