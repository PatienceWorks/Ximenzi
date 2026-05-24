#ifndef __USART_H__
#define __USART_H__

#include "HeaderFiles.h"

#define USART_PORT GPIOA
#define USART USART0
#define USART_TX_Pin GPIO_PIN_9
#define USART_RX_Pin GPIO_PIN_10
#define USART_RCU RCU_USART0
#define USART_PIN_RCU RCU_GPIOA

extern uint8_t recv_real_buf[128];
extern uint8_t recv_real_len;
extern uint8_t recv_flag;

void usart_init(void);
void usart_recv_buf(void);
void usart_send_byte(uint8_t data);
void usart_send_string(const char *str);

#endif
