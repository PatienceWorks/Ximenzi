#include "sys_sample.h"
#include "sys_config.h"
#include "sys_data.h"
#include "sys_display.h"
#include "sys_storage.h"
#include "ADC.h"
#include "KEY.h"
#include "LED.h"
#include "usart.h"
#include <stdio.h>

static uint8_t g_is_sampling = 0;
static uint8_t g_hide_mode = 0;
static uint8_t g_key1_last = 0;
static uint8_t g_key2_last = 0;
static uint8_t g_key3_last = 0;
static uint8_t g_key4_last = 0;
static uint8_t g_key_sync_done = 0;
static uint8_t g_key1_pressed_latch = 0;
static uint8_t g_led1_state = 0;
static uint16_t g_led_tick_10ms = 0;
static uint16_t g_sample_tick_10ms = 0;

static void Sys_PrintSample(uint16_t adc_raw, uint32_t adc_mv, uint32_t ch0_mv)
{
    char buf[96];

    sprintf(buf,
            "ADC: raw=%u adc=%lu.%03luV ch0=%lu.%02luV\r\n",
            adc_raw,
            (unsigned long)(adc_mv / 1000U),
            (unsigned long)(adc_mv % 1000U),
            (unsigned long)(ch0_mv / 1000U),
            (unsigned long)((ch0_mv % 1000U) / 10U));
    usart_send_string(buf);
}

void Sys_SampleInit(void)
{
    g_is_sampling = 0;
    g_hide_mode = 0;
    g_key1_last = 0;
    g_key2_last = 0;
    g_key3_last = 0;
    g_key4_last = 0;
    g_key_sync_done = 0;
    g_key1_pressed_latch = 0;
    g_led1_state = 0;
    g_led_tick_10ms = 0;
    g_sample_tick_10ms = 0;
    LED1_OFF();
    LED2_OFF();
    Sys_DisplayIdle();
}

void Sys_StartSample(void)
{
    g_is_sampling = 1;
    g_led_tick_10ms = 0;
    g_sample_tick_10ms = Sys_GetSamplePeriod() * 100U;
    usart_send_string("sample start\r\n");
    Sys_LogOperation("sample start");
}

void Sys_StopSample(void)
{
    g_is_sampling = 0;
    g_led1_state = 0;
    LED1_OFF();
    LED2_OFF();
    Sys_DisplayIdle();
    usart_send_string("sample stop\r\n");
    Sys_LogOperation("sample stop");
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

void Sys_SetHideMode(uint8_t enable)
{
    g_hide_mode = enable ? 1U : 0U;
}

uint8_t Sys_GetHideMode(void)
{
    return g_hide_mode;
}

static void Sys_SetPeriodByKey(uint16_t period_s, const char *msg, const char *log_msg)
{
    Sys_SetSamplePeriod(period_s);
    Sys_SaveConfigSilent();
    g_sample_tick_10ms = 0;
    usart_send_string(msg);
    Sys_LogOperation(log_msg);
}

void Sys_SampleTask(void)
{
    uint8_t key1_now;
    uint8_t key2_now;
    uint8_t key3_now;
    uint8_t key4_now;
    uint8_t over_limit;
    uint16_t adc_raw;
    uint32_t adc_mv;
    uint32_t ch0_mv;
    uint16_t sample_period_tick;

    key1_now = KEY_Stat(KEY_PORT, KEY1_PIN);
    key2_now = KEY_Stat(KEY_PORT, KEY2_PIN);
    key3_now = KEY_Stat(KEY_PORT, KEY3_PIN);
    key4_now = KEY_Stat(KEY_PORT, KEY4_PIN);

    if (!g_key_sync_done) {
        g_key1_last = key1_now;
        g_key2_last = key2_now;
        g_key3_last = key3_now;
        g_key4_last = key4_now;
        g_key_sync_done = 1U;
        return;
    }

    if ((key1_now == 1U) && (g_key1_last == 0U)) {
        g_key1_pressed_latch = 1U;
    }
    if ((key1_now == 0U) && (g_key1_last == 1U) && (g_key1_pressed_latch == 1U)) {
        g_key1_pressed_latch = 0U;
        Sys_ToggleSample();
    }

    if ((key2_now == 1U) && (g_key2_last == 0U)) {
        Sys_SetPeriodByKey(5U, "period: 5s\r\n", "KEY2 period 5s");
    }
    if ((key3_now == 1U) && (g_key3_last == 0U)) {
        Sys_SetPeriodByKey(10U, "period: 10s\r\n", "KEY3 period 10s");
    }
    if ((key4_now == 1U) && (g_key4_last == 0U)) {
        Sys_SetPeriodByKey(15U, "period: 15s\r\n", "KEY4 period 15s");
    }

    g_key1_last = key1_now;
    g_key2_last = key2_now;
    g_key3_last = key3_now;
    g_key4_last = key4_now;

    if (!g_is_sampling) {
        return;
    }

    g_led_tick_10ms++;
    g_sample_tick_10ms++;

    if (g_led_tick_10ms >= 100U) {
        g_led_tick_10ms = 0;
        if (g_led1_state) {
            LED1_OFF();
            g_led1_state = 0;
        } else {
            LED1_ON();
            g_led1_state = 1U;
        }
    }

    sample_period_tick = Sys_GetSamplePeriod() * 100U;
    if (g_sample_tick_10ms >= sample_period_tick) {
        g_sample_tick_10ms = 0;

        adc_raw = ADC_ReadRaw();
        adc_mv = ((uint32_t)adc_raw * 3300U) / 4095U;
        ch0_mv = Sys_CalcVoltageMillivolt(adc_mv, Sys_GetRatio());
        over_limit = Sys_IsOverLimitMillivolt(ch0_mv, Sys_GetLimit());

        if (over_limit) {
            LED2_ON();
            usart_send_string("OverLimit\r\n");
        } else {
            LED2_OFF();
        }

        Sys_DisplaySample(ch0_mv);
        Sys_PrintSample(adc_raw, adc_mv, ch0_mv);
        Sys_StorageWriteSample(adc_raw, adc_mv, ch0_mv, over_limit, g_hide_mode);
    }
}
