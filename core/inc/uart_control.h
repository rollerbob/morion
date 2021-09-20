#ifndef UART_CONTROL_H
#define UART_CONTROL_H

#include "defines.h"

#define USART_BUFF_SIZE 64
#define USART_BUFF_MSK (USART_BUFF_SIZE - 1)

// Биты переменной _uart_flags
#define UART_MSG_RCV_FLG        (1 << 0) // 1 - Есть новое сообщение от пользователя.
#define UART_BUSY_FLG           (1 << 1) // 1 - идёт передача по UART. 0 - передатчик свободен.

void Uart_init(void);
void Uart_update(void);
void Uart_send_str_DMA(char *str, uint32_t size);

#endif