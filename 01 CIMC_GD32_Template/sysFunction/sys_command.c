#include "sys_command.h"
#include "sys_sample.h"
#include "sys_config.h"
#include "sys_storage.h"
#include "usart.h"
#include "RTC.h"
#include "SPI_FLASH.h"
#include "sdcard.h"
#include "LED.h"
#include "KEY.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>

#define CMD_WAIT_NONE       0U
#define CMD_WAIT_RTC_CONFIG 1U
#define CMD_WAIT_RATIO      2U
#define CMD_WAIT_LIMIT      3U

static uint8_t g_wait_command = CMD_WAIT_NONE;

static void Sys_CommandTrim(char *cmd)
{
    uint8_t len;
    uint8_t start = 0U;
    uint8_t i;

    len = (uint8_t)strlen(cmd);

    while ((start < len) && ((uint8_t)cmd[start] <= ' ')) {
        start++;
    }

    while ((len > start) && ((uint8_t)cmd[len - 1U] <= ' ')) {
        len--;
    }

    if (start > 0U) {
        for (i = 0U; i < (uint8_t)(len - start); i++) {
            cmd[i] = cmd[start + i];
        }
        cmd[i] = '\0';
    } else {
        cmd[len] = '\0';
    }
}

static uint8_t Sys_ParseFloat(const char *cmd, float *value)
{
    uint32_t int_part = 0U;
    uint32_t frac_part = 0U;
    uint32_t frac_base = 1U;
    uint8_t i = 0U;
    uint8_t has_digit = 0U;
    uint8_t after_dot = 0U;

    if (cmd[0] == '-') {
        return 0U;
    }

    while (cmd[i] != '\0') {
        if ((cmd[i] >= '0') && (cmd[i] <= '9')) {
            has_digit = 1U;
            if (after_dot) {
                if (frac_base < 1000000U) {
                    frac_part = frac_part * 10U + (uint32_t)(cmd[i] - '0');
                    frac_base *= 10U;
                }
            } else {
                int_part = int_part * 10U + (uint32_t)(cmd[i] - '0');
            }
        } else if ((cmd[i] == '.') && (!after_dot)) {
            after_dot = 1U;
        } else {
            return 0U;
        }
        i++;
    }

    if (!has_digit) {
        return 0U;
    }

    *value = (float)int_part + ((float)frac_part / (float)frac_base);
    return 1U;
}

static void Sys_PrintFloat2(const char *name, float value)
{
    char buf[48];
    uint32_t scaled = (uint32_t)(value * 100.0f + 0.5f);

    sprintf(buf, "%s=%lu.%02lu\r\n", name,
            (unsigned long)(scaled / 100U),
            (unsigned long)(scaled % 100U));
    usart_send_string(buf);
}

static uint8_t Sys_ParseRtcDateTime(const char *cmd,
                                    uint16_t *year,
                                    uint8_t *month,
                                    uint8_t *date,
                                    uint8_t *hour,
                                    uint8_t *minute,
                                    uint8_t *second)
{
    uint8_t digits[14];
    uint8_t count = 0U;
    uint8_t i = 0U;

    while ((cmd[i] != '\0') && (count < 14U)) {
        if ((cmd[i] >= '0') && (cmd[i] <= '9')) {
            digits[count] = (uint8_t)(cmd[i] - '0');
            count++;
        }
        i++;
    }

    if (count != 14U) {
        return 0U;
    }

    *year = (uint16_t)(digits[0] * 1000U + digits[1] * 100U + digits[2] * 10U + digits[3]);
    *month = (uint8_t)(digits[4] * 10U + digits[5]);
    *date = (uint8_t)(digits[6] * 10U + digits[7]);
    *hour = (uint8_t)(digits[8] * 10U + digits[9]);
    *minute = (uint8_t)(digits[10] * 10U + digits[11]);
    *second = (uint8_t)(digits[12] * 10U + digits[13]);

    return 1U;
}

static void Sys_CommandTest(void)
{
    uint32_t flash_id;
    uint32_t sd_capacity;
    sd_error_enum sd_status;
    char buf[64];

    Sys_LogOperation("test");
    usart_send_string("=======system selftest=======\r\n");

    flash_id = spi_flash_read_id();
    if ((flash_id != 0x000000U) && (flash_id != 0xFFFFFFU)) {
        usart_send_string("flash...........ok\r\n");
    } else {
        usart_send_string("flash...........error\r\n");
    }

    sd_status = sd_init();
    if (sd_status == SD_OK) {
        usart_send_string("TF card.........ok\r\n");
    } else {
        usart_send_string("TF card.........error\r\n");
    }

    sprintf(buf, "flash ID: 0x%06lX\r\n", (unsigned long)flash_id);
    usart_send_string(buf);

    if (sd_status == SD_OK) {
        sd_capacity = sd_card_capacity_get();
        sprintf(buf, "TF card memory: %lu KB\r\n", (unsigned long)sd_capacity);
        usart_send_string(buf);
    } else {
        usart_send_string("can not find TF card\r\n");
    }

    RTC_PrintNowUart();
    usart_send_string("=======system selftest=======\r\n");
}

static void Sys_CommandKeyTest(void)
{
    char buf[96];

    sprintf(buf, "KEY raw: K1=%u K2=%u K3=%u K4=%u\r\n",
            (unsigned int)gpio_input_bit_get(KEY_PORT, KEY1_PIN),
            (unsigned int)gpio_input_bit_get(KEY_PORT, KEY2_PIN),
            (unsigned int)gpio_input_bit_get(KEY_PORT, KEY3_PIN),
            (unsigned int)gpio_input_bit_get(KEY_PORT, KEY4_PIN));
    usart_send_string(buf);

    sprintf(buf, "KEY stat: K1=%u K2=%u K3=%u K4=%u\r\n",
            KEY_Stat(KEY_PORT, KEY1_PIN),
            KEY_Stat(KEY_PORT, KEY2_PIN),
            KEY_Stat(KEY_PORT, KEY3_PIN),
            KEY_Stat(KEY_PORT, KEY4_PIN));
    usart_send_string(buf);
}

static void Sys_CommandRtcConfig(const char *cmd)
{
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    if (!Sys_ParseRtcDateTime(cmd, &year, &month, &date, &hour, &minute, &second)) {
        usart_send_string("RTC Config failed\r\n");
        Sys_LogOperation("RTC Config failed");
        return;
    }

    if (RTC_SetDateTime(year, month, date, hour, minute, second)) {
        usart_send_string("RTC Config success\r\n");
        RTC_PrintNowUart();
        Sys_LogOperation("RTC Config success");
    } else {
        usart_send_string("RTC Config failed\r\n");
        Sys_LogOperation("RTC Config failed");
    }
}

static void Sys_CommandSetRatio(const char *cmd)
{
    float value;

    if ((!Sys_ParseFloat(cmd, &value)) || (value < 0.0f) || (value > 100.0f)) {
        usart_send_string("parameter invalid\r\n");
        Sys_LogOperation("ratio invalid");
        return;
    }

    Sys_SetRatio(value);
    usart_send_string("ratio set success\r\n");
    Sys_PrintFloat2("ratio", Sys_GetRatio());
    Sys_LogOperation("ratio set success");
}

static void Sys_CommandSetLimit(const char *cmd)
{
    float value;

    if ((!Sys_ParseFloat(cmd, &value)) || (value < 0.0f) || (value > 200.0f)) {
        usart_send_string("parameter invalid\r\n");
        Sys_LogOperation("limit invalid");
        return;
    }

    Sys_SetLimit(value);
    usart_send_string("limit set success\r\n");
    Sys_PrintFloat2("limit", Sys_GetLimit());
    Sys_LogOperation("limit set success");
}

void Sys_CommandProcess(const char *cmd)
{
    char cmd_buf[64];

    strncpy(cmd_buf, cmd, sizeof(cmd_buf) - 1U);
    cmd_buf[sizeof(cmd_buf) - 1U] = '\0';
    Sys_CommandTrim(cmd_buf);

    if (g_wait_command == CMD_WAIT_RTC_CONFIG) {
        g_wait_command = CMD_WAIT_NONE;
        Sys_CommandRtcConfig(cmd_buf);
        return;
    } else if (g_wait_command == CMD_WAIT_RATIO) {
        g_wait_command = CMD_WAIT_NONE;
        Sys_CommandSetRatio(cmd_buf);
        return;
    } else if (g_wait_command == CMD_WAIT_LIMIT) {
        g_wait_command = CMD_WAIT_NONE;
        Sys_CommandSetLimit(cmd_buf);
        return;
    }

    if (strcmp(cmd_buf, "start") == 0) {
        Sys_StartSample();
    } else if (strcmp(cmd_buf, "stop") == 0) {
        Sys_StopSample();
    } else if (strcmp(cmd_buf, "test") == 0) {
        Sys_CommandTest();
    } else if (strcmp(cmd_buf, "key test") == 0) {
        Sys_CommandKeyTest();    } else if (strcmp(cmd_buf, "tf test") == 0) {
        Sys_TfWriteTest();
    } else if (strcmp(cmd_buf, "conf") == 0) {
        Sys_LoadConfig();
    } else if (strcmp(cmd_buf, "config tf") == 0) {
        Sys_LoadConfig();
    } else if (strcmp(cmd_buf, "config save") == 0) {
        Sys_SaveConfig();
    } else if (strcmp(cmd_buf, "config read") == 0) {
        Sys_LoadSavedConfig();
    } else if (strcmp(cmd_buf, "config load") == 0) {
        Sys_LoadSavedConfig();
    } else if (strcmp(cmd_buf, "hide") == 0) {
        Sys_SetHideMode(1U);
        usart_send_string("hide mode on\r\n");
        Sys_LogOperation("hide");
    } else if (strcmp(cmd_buf, "unhide") == 0) {
        Sys_SetHideMode(0U);
        usart_send_string("hide mode off\r\n");
        Sys_LogOperation("unhide");
    } else if (strcmp(cmd_buf, "RTC now") == 0) {
        RTC_PrintNowUart();
        Sys_LogOperation("RTC now");
    } else if (strcmp(cmd_buf, "RTC Config") == 0) {
        g_wait_command = CMD_WAIT_RTC_CONFIG;
        usart_send_string("Input Datetime\r\n");
        Sys_LogOperation("RTC Config");
    } else if (strcmp(cmd_buf, "ratio") == 0) {
        Sys_PrintFloat2("current ratio", Sys_GetRatio());
        usart_send_string("Input ratio 0-100\r\n");
        g_wait_command = CMD_WAIT_RATIO;
        Sys_LogOperation("ratio");
    } else if (strcmp(cmd_buf, "limit") == 0) {
        Sys_PrintFloat2("current limit", Sys_GetLimit());
        usart_send_string("Input limit 0-200\r\n");
        g_wait_command = CMD_WAIT_LIMIT;
        Sys_LogOperation("limit");
    } else if (cmd_buf[0] != '\0') {
        usart_send_string("unknown command: [");
        usart_send_string(cmd_buf);
        usart_send_string("]\r\n");
    }
}
