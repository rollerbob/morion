/**
 * @file usart_control.h
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Модуль работы с интерфейсом USART.
 * 
 * Принимает сообщения во входной буфер, отправляет сообщения в терминал
 * пользователя.
 * 
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef USART_CONTROL_H
#define USART_CONTROL_H

#include "defines.h"
#include "shared_types.h"

/// Размер буфера приёма. Должен быть степенью 2.
#define USART_BUFF_SIZE               64
/// Маска для для обработки переполнения буфера.
#define USART_BUFF_MSK (USART_BUFF_SIZE - 1)

// Биты переменной _uart_flags
#define USART_MSG_RCV_FLG       (1 << 0) // 1 - Есть новое сообщение от пользователя.
#define USART_BUSY_FLG          (1 << 1) // 1 - идёт передача по UART. 0 - передатчик свободен.

/**
 * @brief Инициализация модуля.
 * 
 */
void Usart_init(void);

/**
 * @brief Обновление модуля.
 * 
 * @param Rcvd_cmd_t* cmd_ptr - Указатель на экземпляр команды, который будет
 * передан в парсер если обнаружится символ конца передачи.
 */
void Usart_update(Rcvd_cmd_t *cmd_ptr);

/**
 * @brief Отправка строки текста на терминал пользователя.
 * 
 * Используется DMA.
 * 
 * @param const char* str - Указатель на начало строки.
 * @param uint32_t size - Длина этой строки.
 */
void Usart_send_str_DMA(const char *str, uint32_t size);

#endif