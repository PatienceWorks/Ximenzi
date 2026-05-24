#include "sys_sample.h"

/*
 * g_is_sampling
 *
 * 0：当前停止采样。
 * 1：当前正在采样。
 *
 * 后续可以继续增加：
 *   1. 上一次采样时间。
 *   2. 上一次 LED1 翻转时间。
 *   3. 最近一次采样电压。
 *   4. 最近一次是否超限。
 */
static uint8_t g_is_sampling = 0;

void Sys_SampleInit(void)
{
    g_is_sampling = 0;
}

void Sys_StartSample(void)
{
    g_is_sampling = 1;
}

void Sys_StopSample(void)
{
    g_is_sampling = 0;
}

void Sys_ToggleSample(void)
{
    if (g_is_sampling) {
        Sys_StopSample();
    } else {
        Sys_StartSample();
    }
}

uint8_t Sys_IsSampling(void)
{
    return g_is_sampling;
}

void Sys_SampleTask(void)
{
    /*
     * 后续主循环应周期调用本函数。
     *
     * 需要实现的流程：
     *   1. 如果未启动采样，直接返回。
     *   2. 处理 LED1 1s 闪烁。
     *   3. 判断是否到达 Sys_GetSamplePeriod() 设置的采样周期。
     *   4. 到达周期后读取 ADC 电压。
     *   5. 乘以 ratio 得到 Ch0 电压。
     *   6. 判断是否超过 limit。
     *   7. 刷新 OLED。
     *   8. 串口打印本次采样结果。
     */
    if (!g_is_sampling) {
        return;
    }
}
