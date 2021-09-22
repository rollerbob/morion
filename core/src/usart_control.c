/**
 * @file usart_control.c
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Реализация модуля работы с интерфейсов USART.
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "usart_control.h"
#include "parser.h"

uint8_t _in_buff[USART_BUFF_SIZE]; ///< Входной буфер.
volatile uint32_t _in_head;        ///< Индекс входного буфера.
volatile uint32_t _uart_flags;     ///< Статусные флаги модуля.

void Usart_init(void)
{
    _in_head = 0;
    _uart_flags = 0;
    Parser_init(_in_buff);
}

void Usart_update(Rcvd_cmd_t *cmd_ptr)
{
    if (_uart_flags & USART_MSG_RCV_FLG)
    {
        _uart_flags &= ~USART_MSG_RCV_FLG;
        Parser_work(cmd_ptr, _in_head);
        _in_head = 0;
    }
}

void Usart_send_str_DMA(const char *str, uint32_t size)
{
    // Если модуль USART свободен
    if (!(_uart_flags & USART_BUSY_FLG))
    {
        // Выставляю флаг занятости передатчика USART
        _uart_flags |= USART_BUSY_FLG;

        // Отключаю DMA
        DMA1_Channel4->CCR &= ~(DMA_CCR_EN);
        // Записываю в источник DMA начало строки
        DMA1_Channel4->CMAR = (uint32_t)str;
        DMA1_Channel4->CNDTR = size;

        // Включаю обратно DMA
        DMA1_Channel4->CCR |= DMA_CCR_EN;
    }
}

/**
 * @brief Обработчик прерываний от USART
 * 
 */
void USART2_IRQHandler(void)
{
    volatile uint32_t isr = USART2->ISR;

    // Если сработало прерывание по заполнению входного регистра
    if (isr & USART_ISR_RXNE)
    {
        uint8_t in = (uint8_t)(USART2->RDR);

        _in_buff[_in_head++] = in;
        _in_head &= USART_BUFF_MSK;

        // Если обнаружен символ <LF>
        if (in == '\n')
        {
            // Выставляется флаг готовности буфера к разбору.
            _uart_flags |= USART_MSG_RCV_FLG;
        }
    }
}

/**
 * @brief Обработчик прерываний от DMA
 * 
 */
void DMA1_Channel4_5_IRQHandler (void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4)
        DMA1->IFCR |= DMA_IFCR_CTCIF4;

    _uart_flags &= ~USART_BUSY_FLG;
}