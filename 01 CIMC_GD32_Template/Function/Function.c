/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：Function.c
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/


/************************* 头文件 *************************/

#include "Function.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/


/************************ 函数定义 ************************/



/************************************************************ 
 * Function :       System_Init
 * Comment  :       用于初始化MCU
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void System_Init(void)
{
	systick_config();     // 时钟配置
	usart_init();
	usart_send_string("====system init====\r\n");
	usart_send_string("Device_ID:2025-CIMC-0001\r\n");
	usart_send_string("====system ready====\r\n");
	LED_Init();
	KEY_Init();
	OLED_Init();
	OLED_Clear();
	RTC_TestInitFixed();
	ADC_port_init();
	spi_flash_init();
	Sys_ConfigInit();
	Sys_LoadSavedConfigSilent();
	Sys_StorageInit();
	Sys_SampleInit();
}
/************************************************************ 
 * Function :       Init_LED_Stat
 * Comment  :       系统初始化时用LED显示状态
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-10 V0.1 original
************************************************************/


/************************************************************ 
 * Function :       UsrFunction
 * Comment  :       用户程序功能: LED1闪烁
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void UsrFunction(void)
{
	while(1)
	{
		if(recv_flag)
		{
			if(recv_real_len >= sizeof(recv_real_buf))
			{
				recv_real_len = sizeof(recv_real_buf) - 1U;
			}
			recv_real_buf[recv_real_len] = '\0';
			Sys_CommandProcess((const char *)recv_real_buf);
			recv_real_len = 0;
			recv_flag = 0;
		}

		Sys_SampleTask();
		delay_1ms(10);
	}
}

/****************************End*****************************/

