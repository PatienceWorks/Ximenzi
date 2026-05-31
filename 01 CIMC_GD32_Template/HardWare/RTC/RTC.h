/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：RTC.h
 * 作者: Qiao Qin @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "HeaderFiles.h"

void RTC_Init(void);	// RTC初始化
void RTC_TestInitFixed(void);    // RTC固定时间测试初始化
void RTC_PrintTime(void);        // 串口打印当前RTC时间
void RTC_GetTimeString(char *buf); // 获取hh:mm:ss时间字符串
void RTC_GetDateTimeFileString(char *buf); // 获取YYYYMMDDhhmmss文件名时间字符串
void RTC_PrintNowUart(void);      // 串口打印当前RTC时间
uint8_t RTC_SetDateTime(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
void rtc_setup(void);	// RTC时钟设置
void rtc_show_time(void);	// RTC时间
void rtc_show_alarm(void);	// RTC闹钟
uint8_t usart_input_threshold(uint32_t value);  // 用作输入值有效校验
void rtc_pre_config(void);

#endif
