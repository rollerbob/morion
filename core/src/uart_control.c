#include "uart_control.h"

volatile static char _in_buff[USART_BUFF_SIZE];
volatile static char _out_buff[USART_BUFF_SIZE];

volatile static uint32_t _in_buff_head, _in_buff_tail;
volatile static uint32_t _out_buff_tail;
uint32_t static _out_buff_head;

void Uart_init(void)
{
    _in_buff_head = 0;
    _in_buff_tail = 0;

    _out_buff_head = 0;
    _out_buff_tail = 0;

    for (uint32_t i = 0; i < USART_BUFF_SIZE; i++)
    {
        _in_buff[i] = 0;
        _out_buff[i] = 0;
    }
}

void Uart_put_str(char *str)
{
    while(*str)
    {
        _out_buff[_out_buff_head] = *str;
        _out_buff_head++;
        _out_buff_head &= USART_BUFF_MSK;
        str++;
    }
    USART2->CR1 |= USART_CR1_TE | USART_CR1_TXEIE;
}

void USART2_IRQHandler(void)
{
    volatile uint32_t isr = USART2->ISR;

    // Если выходной регистр пуст.
    if (isr & USART_ISR_TXE)
    {
        if (_out_buff_tail != _out_buff_head)
        {
            USART2->TDR = _out_buff[_out_buff_tail++];
            _out_buff_tail &= USART_BUFF_MSK;
        } else
        {
            USART2->CR1 &= ~(USART_CR1_TE | USART_CR1_TXEIE);
        }
    }
}