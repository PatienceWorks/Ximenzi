#include "sys_config.h"

/*
 * 当前先使用 RAM 变量保存配置。
 *
 * 后续完善方向：
 *   1. 上电时优先从 Flash 读取已保存参数。
 *   2. 如果 Flash 无有效参数，则使用默认值。
 *   3. 修改 ratio、limit、sample_period 时进行范围检查。
 *   4. KEY2/KEY3/KEY4 修改采样周期后，需要保存到 Flash。
 */
static float g_ratio = 1.0f;
static float g_limit = 100.0f;
static uint16_t g_sample_period_s = 5;

void Sys_ConfigInit(void)
{
    g_ratio = 1.0f;
    g_limit = 100.0f;
    g_sample_period_s = 5;
}

void Sys_SetRatio(float ratio)
{
    g_ratio = ratio;
}

float Sys_GetRatio(void)
{
    return g_ratio;
}

void Sys_SetLimit(float limit)
{
    g_limit = limit;
}

float Sys_GetLimit(void)
{
    return g_limit;
}

void Sys_SetSamplePeriod(uint16_t period_s)
{
    g_sample_period_s = period_s;
}

uint16_t Sys_GetSamplePeriod(void)
{
    return g_sample_period_s;
}
