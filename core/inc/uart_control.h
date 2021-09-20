#ifndef UART_CONTROL_H
#define UART_CONTROL_H

#include "defines.h"

#define USART_BUFF_SIZE 64
#define USART_BUFF_MSK (USART_BUFF_SIZE - 1)

void Uart_init(void);
void Uart_send_str_DMA(char *str, uint32_t size);

#endif