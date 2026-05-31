#include "sys_storage.h"
#include "sys_config.h"
#include "usart.h"
#include "SPI_FLASH.h"
#include "RTC.h"
#include "ff.h"
#include <string.h>
#include <stdio.h>

#define CONFIG_FLASH_ADDR      0x000000UL
#define CONFIG_FLASH_MAGIC     "CFG1"
#define CONFIG_FLASH_BUF_SIZE  128U

#define META_FLASH_ADDR        0x001000UL
#define META_FLASH_MAGIC       "META"
#define META_FLASH_BUF_SIZE    128U

#define SAMPLE_PER_FILE        10U

static FATFS g_tf_fs;
static FIL g_tf_file;
static char g_tf_msg[128];
static uint8_t g_tf_ready = 0U;
static uint32_t g_log_id = 0U;
static char g_log_path[32];
static char g_sample_path[64];
static char g_over_path[64];
static char g_hide_path[64];
static uint8_t g_sample_count = 0U;
static uint8_t g_over_count = 0U;
static uint8_t g_hide_count = 0U;
static uint16_t g_sample_file_id = 0U;
static uint16_t g_over_file_id = 0U;
static uint16_t g_hide_file_id = 0U;

static void Sys_TrimLine(char *line)
{
    uint16_t len = (uint16_t)strlen(line);

    while (len > 0U) {
        if ((line[len - 1U] == '\r') || (line[len - 1U] == '\n') || (line[len - 1U] == ' ')) {
            line[len - 1U] = '\0';
            len--;
        } else {
            break;
        }
    }
}

static uint8_t Sys_ParseFloatValue(const char *str, float *value)
{
    uint32_t int_part = 0U;
    uint32_t frac_part = 0U;
    uint32_t frac_base = 1U;
    uint8_t i = 0U;
    uint8_t has_digit = 0U;
    uint8_t after_dot = 0U;

    if (str[0] == '-') {
        return 0U;
    }

    while (str[i] != '\0') {
        if ((str[i] >= '0') && (str[i] <= '9')) {
            has_digit = 1U;
            if (after_dot) {
                if (frac_base < 1000000U) {
                    frac_part = frac_part * 10U + (uint32_t)(str[i] - '0');
                    frac_base *= 10U;
                }
            } else {
                int_part = int_part * 10U + (uint32_t)(str[i] - '0');
            }
        } else if ((str[i] == '.') && (!after_dot)) {
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

static uint8_t Sys_ParseUint16Value(const char *str, uint16_t *value)
{
    uint32_t result = 0U;
    uint8_t i = 0U;
    uint8_t has_digit = 0U;

    while (str[i] != '\0') {
        if ((str[i] < '0') || (str[i] > '9')) {
            return 0U;
        }
        has_digit = 1U;
        result = result * 10U + (uint32_t)(str[i] - '0');
        if (result > 65535U) {
            return 0U;
        }
        i++;
    }

    if (!has_digit) {
        return 0U;
    }

    *value = (uint16_t)result;
    return 1U;
}

static void Sys_PrintConfig(void)
{
    char buf[64];
    uint32_t ratio_scaled = (uint32_t)(Sys_GetRatio() * 100.0f + 0.5f);
    uint32_t limit_scaled = (uint32_t)(Sys_GetLimit() * 100.0f + 0.5f);

    sprintf(buf, "ratio=%lu.%02lu\r\n", (unsigned long)(ratio_scaled / 100U), (unsigned long)(ratio_scaled % 100U));
    usart_send_string(buf);
    sprintf(buf, "limit=%lu.%02lu\r\n", (unsigned long)(limit_scaled / 100U), (unsigned long)(limit_scaled % 100U));
    usart_send_string(buf);
    sprintf(buf, "period=%u\r\n", Sys_GetSamplePeriod());
    usart_send_string(buf);
}

static uint8_t Sys_TfMount(void)
{
    FRESULT result;

    if (g_tf_ready) {
        return 1U;
    }

    result = f_mount(0, &g_tf_fs);
    if (result == FR_OK) {
        g_tf_ready = 1U;
        return 1U;
    }

    g_tf_ready = 0U;
    return 0U;
}

static void Sys_MkdirIgnoreExist(const char *path)
{
    FRESULT result;

    result = f_mkdir(path);
    (void)result;
}

static void Sys_WriteTextFile(const char *path, const char *text)
{
    UINT written;
    FRESULT result;
    UINT len;

    if ((path == 0) || (path[0] == '\0')) {
        usart_send_string("TF write failed: empty path\r\n");
        return;
    }

    if (!Sys_TfMount()) {
        usart_send_string("TF mount failed\r\n");
        return;
    }

    usart_send_string("TF open try: ");
    usart_send_string(path);
    usart_send_string("\r\n");

    result = f_open(&g_tf_file, path, FA_OPEN_ALWAYS | FA_WRITE);
    if (result == FR_OK) {
        result = f_lseek(&g_tf_file, g_tf_file.fsize);
        if (result != FR_OK) {
            f_close(&g_tf_file);
            sprintf(g_tf_msg, "TF seek failed:%d\r\n", result);
            usart_send_string(g_tf_msg);
            return;
        }
    } else {
        sprintf(g_tf_msg, "TF open failed:%d ", result);
        usart_send_string(g_tf_msg);
        usart_send_string(path);
        usart_send_string("\r\n");
        return;
    }

    len = (UINT)strlen(text);
    result = f_write(&g_tf_file, text, len, &written);
    if ((result != FR_OK) || (written != len)) {
        sprintf(g_tf_msg, "TF write failed:%d bytes:%u\r\n", result, written);
        usart_send_string(g_tf_msg);
    } else {
        usart_send_string("TF saved: ");
        usart_send_string(path);
        usart_send_string("\r\n");
    }

    f_sync(&g_tf_file);
    f_close(&g_tf_file);
}
static uint32_t Sys_LoadBootCount(void)
{
    char read_buf[META_FLASH_BUF_SIZE + 1U];
    char *pos;
    uint32_t value = 0U;

    memset(read_buf, 0, sizeof(read_buf));
    spi_flash_buffer_read((uint8_t *)read_buf, META_FLASH_ADDR, META_FLASH_BUF_SIZE);
    read_buf[META_FLASH_BUF_SIZE] = '\0';

    if (strncmp(read_buf, META_FLASH_MAGIC, 4U) != 0) {
        return 0U;
    }

    pos = strstr(read_buf, "boot=");
    if (pos == 0) {
        return 0U;
    }

    pos += 5;
    while ((*pos >= '0') && (*pos <= '9')) {
        value = value * 10U + (uint32_t)(*pos - '0');
        pos++;
    }

    return value;
}

static void Sys_SaveBootCount(uint32_t value)
{
    char save_buf[META_FLASH_BUF_SIZE];

    memset(save_buf, 0xFF, sizeof(save_buf));
    sprintf(save_buf, META_FLASH_MAGIC "\nboot=%lu\n", (unsigned long)value);
    spi_flash_sector_erase(META_FLASH_ADDR);
    spi_flash_buffer_write((uint8_t *)save_buf, META_FLASH_ADDR, META_FLASH_BUF_SIZE);
}

static void Sys_ParseConfigLine(char *line)
{
    float float_value;
    uint16_t period_value;
    char *value;

    Sys_TrimLine(line);

    if ((line[0] == '\0') || (line[0] == '#') || (line[0] == ';')) {
        return;
    }

    value = strchr(line, '=');
    if (value == 0) {
        return;
    }

    *value = '\0';
    value++;

    if (strcmp(line, "ratio") == 0) {
        if (Sys_ParseFloatValue(value, &float_value) && (float_value >= 0.0f) && (float_value <= 100.0f)) {
            Sys_SetRatio(float_value);
        }
    } else if (strcmp(line, "limit") == 0) {
        if (Sys_ParseFloatValue(value, &float_value) && (float_value >= 0.0f) && (float_value <= 200.0f)) {
            Sys_SetLimit(float_value);
        }
    } else if (strcmp(line, "period") == 0) {
        if (Sys_ParseUint16Value(value, &period_value) &&
            ((period_value == 5U) || (period_value == 10U) || (period_value == 15U))) {
            Sys_SetSamplePeriod(period_value);
        }
    }
}

void Sys_StorageInit(void)
{
    FIL file;
    UINT written;
    char line[64];

    if (!Sys_TfMount()) {
        g_tf_ready = 0U;
        return;
    }

    Sys_MkdirIgnoreExist("0:/sample");
    Sys_MkdirIgnoreExist("0:/overLimit");
    Sys_MkdirIgnoreExist("0:/log");
    Sys_MkdirIgnoreExist("0:/hideData");

    g_log_id = Sys_LoadBootCount();
    sprintf(g_log_path, "0:/L%04lu.TXT", (unsigned long)(g_log_id % 10000UL));
    Sys_SaveBootCount(g_log_id + 1U);

    if (f_open(&file, g_log_path, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
        sprintf(line, "log start: %lu\r\n", (unsigned long)g_log_id);
        f_write(&file, line, (UINT)strlen(line), &written);
        f_close(&file);
    } else {
        usart_send_string("log open failed: ");
        usart_send_string(g_log_path);
        usart_send_string("\r\n");
    }
}

void Sys_LogOperation(const char *text)
{
    (void)text;
}

void Sys_LoadConfig(void)
{
    FIL file;
    FRESULT result;
    char line[64];
    char buf[64];

    if (!Sys_TfMount()) {
        usart_send_string("TF card not found\r\n");
        return;
    }

    result = f_open(&file, "0:/config.ini", FA_READ);
    if (result != FR_OK) {
        result = f_open(&file, "0:/CONFIG.INI", FA_READ);
    }
    if (result != FR_OK) {
        sprintf(buf, "no config file: %d\r\n", result);
        usart_send_string(buf);
        Sys_LogOperation("conf failed: no config.ini");
        return;
    }

    while (f_gets(line, sizeof(line), &file) != 0) {
        Sys_ParseConfigLine(line);
    }

    f_close(&file);

    usart_send_string("config read success\r\n");
    Sys_PrintConfig();
    Sys_SaveConfigSilent();
    Sys_LogOperation("conf success");
}

void Sys_SaveConfigSilent(void)
{
    char save_buf[CONFIG_FLASH_BUF_SIZE];
    uint32_t ratio_scaled = (uint32_t)(Sys_GetRatio() * 100.0f + 0.5f);
    uint32_t limit_scaled = (uint32_t)(Sys_GetLimit() * 100.0f + 0.5f);

    memset(save_buf, 0xFF, sizeof(save_buf));
    sprintf(save_buf,
            CONFIG_FLASH_MAGIC "\nratio=%lu.%02lu\nlimit=%lu.%02lu\nperiod=%u\n",
            (unsigned long)(ratio_scaled / 100U),
            (unsigned long)(ratio_scaled % 100U),
            (unsigned long)(limit_scaled / 100U),
            (unsigned long)(limit_scaled % 100U),
            Sys_GetSamplePeriod());

    spi_flash_sector_erase(CONFIG_FLASH_ADDR);
    spi_flash_buffer_write((uint8_t *)save_buf, CONFIG_FLASH_ADDR, CONFIG_FLASH_BUF_SIZE);
}

void Sys_SaveConfig(void)
{
    char verify_buf[CONFIG_FLASH_BUF_SIZE];
    char expect_buf[CONFIG_FLASH_BUF_SIZE];
    uint32_t ratio_scaled = (uint32_t)(Sys_GetRatio() * 100.0f + 0.5f);
    uint32_t limit_scaled = (uint32_t)(Sys_GetLimit() * 100.0f + 0.5f);

    memset(expect_buf, 0xFF, sizeof(expect_buf));
    memset(verify_buf, 0x00, sizeof(verify_buf));
    sprintf(expect_buf,
            CONFIG_FLASH_MAGIC "\nratio=%lu.%02lu\nlimit=%lu.%02lu\nperiod=%u\n",
            (unsigned long)(ratio_scaled / 100U),
            (unsigned long)(ratio_scaled % 100U),
            (unsigned long)(limit_scaled / 100U),
            (unsigned long)(limit_scaled % 100U),
            Sys_GetSamplePeriod());

    Sys_SaveConfigSilent();
    spi_flash_buffer_read((uint8_t *)verify_buf, CONFIG_FLASH_ADDR, CONFIG_FLASH_BUF_SIZE);

    if (memcmp(expect_buf, verify_buf, CONFIG_FLASH_BUF_SIZE) == 0) {
        usart_send_string("config save success\r\n");
        Sys_PrintConfig();
        Sys_LogOperation("config save success");
    } else {
        usart_send_string("config save failed\r\n");
        Sys_LogOperation("config save failed");
    }
}

static uint8_t Sys_LoadSavedConfigInternal(uint8_t verbose)
{
    char read_buf[CONFIG_FLASH_BUF_SIZE + 1U];
    char *line;
    char *next;

    memset(read_buf, 0, sizeof(read_buf));
    spi_flash_buffer_read((uint8_t *)read_buf, CONFIG_FLASH_ADDR, CONFIG_FLASH_BUF_SIZE);
    read_buf[CONFIG_FLASH_BUF_SIZE] = '\0';

    if (strncmp(read_buf, CONFIG_FLASH_MAGIC, 4U) != 0) {
        if (verbose) {
            usart_send_string("config read failed\r\n");
            Sys_LogOperation("config read failed");
        }
        return 0U;
    }

    line = read_buf;
    while (line != 0) {
        next = strchr(line, '\n');
        if (next != 0) {
            *next = '\0';
            next++;
        }
        Sys_ParseConfigLine(line);
        line = next;
    }

    if (verbose) {
        usart_send_string("config read success\r\n");
        Sys_PrintConfig();
        Sys_LogOperation("config read success");
    }

    return 1U;
}

void Sys_LoadSavedConfig(void)
{
    (void)Sys_LoadSavedConfigInternal(1U);
}

void Sys_LoadSavedConfigSilent(void)
{
    (void)Sys_LoadSavedConfigInternal(0U);
}


static void Sys_BuildFreePath(char *path, const char *dir, char prefix, uint16_t *file_id)
{
    FILINFO info;
    uint16_t try_count = 0U;

    do {
        sprintf(path, "%s/%c%03u.TXT", dir, prefix, (unsigned int)(*file_id % 1000U));
        (*file_id)++;
        try_count++;
    } while ((f_stat(path, &info) == FR_OK) && (try_count < 1000U));
}
static void Sys_WriteDataLine(const char *path, uint16_t adc_raw, uint32_t adc_mv, uint32_t ch0_mv, uint8_t encrypted)
{
    char line[128];
    char time_buf[16];
    uint32_t show_mv = ch0_mv;

    RTC_GetTimeString(time_buf);

    if (encrypted) {
        show_mv = ch0_mv ^ 0x00005A5AU;
        sprintf(line,
                "%s,raw=%u,adc=%lu.%03luV,ch0=%lu.%02luV,hide=%08lX\r\n",
                time_buf,
                adc_raw,
                (unsigned long)(adc_mv / 1000U),
                (unsigned long)(adc_mv % 1000U),
                (unsigned long)(ch0_mv / 1000U),
                (unsigned long)((ch0_mv % 1000U) / 10U),
                (unsigned long)show_mv);
    } else {
        sprintf(line,
                "%s,raw=%u,adc=%lu.%03luV,ch0=%lu.%02luV\r\n",
                time_buf,
                adc_raw,
                (unsigned long)(adc_mv / 1000U),
                (unsigned long)(adc_mv % 1000U),
                (unsigned long)(ch0_mv / 1000U),
                (unsigned long)((ch0_mv % 1000U) / 10U));
    }

    Sys_WriteTextFile(path, line);
}

void Sys_StorageWriteSample(uint16_t adc_raw, uint32_t adc_mv, uint32_t ch0_mv, uint8_t over_limit, uint8_t hide_mode)
{
    char stamp[16];

    (void)stamp;

    if (!g_tf_ready) {
        Sys_TfMount();
    }
    if (!g_tf_ready) {
        return;
    }

    if ((!hide_mode) && ((g_sample_count == 0U) || (g_sample_count >= SAMPLE_PER_FILE))) {
        Sys_BuildFreePath(g_sample_path, "0:/sample", 'S', &g_sample_file_id);
        g_sample_count = 0U;
    }

    if (over_limit && ((g_over_count == 0U) || (g_over_count >= SAMPLE_PER_FILE))) {
        Sys_BuildFreePath(g_over_path, "0:/overLimit", 'O', &g_over_file_id);
        g_over_count = 0U;
    }

    if (hide_mode && ((g_hide_count == 0U) || (g_hide_count >= SAMPLE_PER_FILE))) {
        Sys_BuildFreePath(g_hide_path, "0:/hideData", 'H', &g_hide_file_id);
        g_hide_count = 0U;
    }

    if (!hide_mode) {
        Sys_WriteDataLine(g_sample_path, adc_raw, adc_mv, ch0_mv, 0U);
        g_sample_count++;
    } else {
        Sys_WriteDataLine(g_hide_path, adc_raw, adc_mv, ch0_mv, 1U);
        g_hide_count++;
    }

    if (over_limit) {
        Sys_WriteDataLine(g_over_path, adc_raw, adc_mv, ch0_mv, 0U);
        g_over_count++;
    }
}
void Sys_TfWriteTest(void)
{
    FIL file;
    FRESULT result;
    UINT written;
    char buf[64];

    result = f_mount(0, &g_tf_fs);
    sprintf(buf, "tf mount:%d\r\n", result);
    usart_send_string(buf);
    if (result != FR_OK) {
        return;
    }

    result = f_mkdir("0:/sample");
    sprintf(buf, "tf mkdir sample:%d\r\n", result);
    usart_send_string(buf);

    result = f_open(&file, "0:/sample/T.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    sprintf(buf, "tf open sample/T.TXT:%d\r\n", result);
    usart_send_string(buf);
    if (result == FR_OK) {
        f_close(&file);
    }

    result = f_open(&file, "0:/T.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    sprintf(buf, "tf open T.TXT:%d\r\n", result);
    usart_send_string(buf);
    if (result != FR_OK) {
        return;
    }

    result = f_write(&file, "tf ok\r\n", 7, &written);
    sprintf(buf, "tf write:%d bytes:%u\r\n", result, written);
    usart_send_string(buf);
    f_close(&file);
}
