#ifndef USART_CONTROL_H
#define USART_CONTROL_H

#include "defines.h"
#include "shared_types.h"

#define USART_BUFF_SIZE              64 ///< Размер буфера приёма. Должен быть степенью 2.
#define USART_BUFF_MSK (USART_BUFF_SIZE - 1)

// Биты переменной _uart_flags
#define USART_MSG_RCV_FLG       (1 << 0) // 1 - Есть новое сообщение от пользователя.
#define USART_BUSY_FLG          (1 << 1) // 1 - идёт передача по UART. 0 - передатчик свободен.

void Usart_init(void);
void Usart_update(void);
uint8_t *Usart_get_inbuff(void);
void Usart_send_str_DMA(char *str, uint32_t size);

#endif