#include "usart.h"

uint8_t recv_buf[128] = {0};
uint8_t recv_len = 0;
uint8_t recv_real_buf[128] = {0};
uint8_t recv_real_len = 0;
uint8_t recv_flag = 0;

void usart_init(void)
{
    nvic_irq_enable(USART0_IRQn, 3, 2);

    rcu_periph_clock_enable(USART_RCU);
    rcu_periph_clock_enable(USART_PIN_RCU);

    gpio_af_set(USART_PORT, GPIO_AF_7, USART_TX_Pin | USART_RX_Pin);
    gpio_mode_set(USART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART_TX_Pin | USART_RX_Pin);
    gpio_output_options_set(USART_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART_TX_Pin | USART_RX_Pin);

    usart_deinit(USART);
    usart_baudrate_set(USART, 115200U);
    usart_transmit_config(USART, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART, USART_RECEIVE_ENABLE);
    usart_enable(USART);

    usart_interrupt_enable(USART, USART_INT_RBNE);
    usart_interrupt_enable(USART, USART_INT_IDLE);
}

void usart_recv_buf(void)
{
    if (recv_flag)
    {
        recv_flag = 0;
    }
}

void usart_send_byte(uint8_t data)
{
    usart_data_transmit(USART, data);
    while (RESET == usart_flag_get(USART, USART_FLAG_TBE))
    {
    }
}

void usart_send_string(const char *str)
{
    while (*str != '\0')
    {
        usart_send_byte((uint8_t)*str++);
    }
}

void USART0_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART, USART_INT_FLAG_RBNE) != RESET)
    {
        if (recv_len < sizeof(recv_buf))
        {
            recv_buf[recv_len++] = (uint8_t)usart_data_receive(USART);
        }
        else
        {
            (void)usart_data_receive(USART);
        }
    }

    if (usart_interrupt_flag_get(USART, USART_INT_FLAG_IDLE) != RESET)
    {
        (void)usart_data_receive(USART);
        if (recv_len != 0)
        {
            memcpy(recv_real_buf, recv_buf, recv_len);
            recv_real_len = recv_len;
            recv_len = 0;
            recv_flag = 1;
        }
    }
}
