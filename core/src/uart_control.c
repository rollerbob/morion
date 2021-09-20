#include "uart_control.h"

volatile static uint8_t _in_buff[USART_BUFF_SIZE];
volatile static uint32_t _in_head;
volatile static uint32_t _in_tail;
volatile static uint32_t _msg_rcvd;

void Uart_init(void)
{
    _in_head = 0;
    _in_tail = 0;
    _msg_rcvd = 0;
}

void Uart_send_str_DMA(char *str, uint32_t size)
{
    // Отключаю DMA
    DMA1_Channel4->CCR &= ~(DMA_CCR_EN);
    // Записываю в источник DMA начало строки
    DMA1_Channel4->CMAR = (uint32_t)str;
    DMA1_Channel4->CNDTR = size;
    // USART2->ICR &= ~(USART_ICR_TCCF);
    DMA1_Channel4->CCR |= DMA_CCR_EN;
}

void USART2_IRQHandler(void)
{
    volatile uint32_t isr = USART2->ISR;

    // Если сработало прерывание по заполнению входного регистра
    if (isr & USART_ISR_RXNE)
    {
        uint8_t in = (uint8_t)(USART2->RDR);
        if (in == 0x0D)
            _msg_rcvd = 1;
        _in_buff[_in_head] = in;
        _in_head++;
        _in_head &= USART_BUFF_MSK;
    }
}

void DMA1_Channel4_5_6_7_IRQHandler (void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4)
        DMA1->IFCR |= DMA_IFCR_CTCIF4;
    // if (DMA1->IFSR & DMA_ISR_TCIF4)
    //     DMA1->IFCR |= DMA_ISR_TCIF4;//очистить флаг окончания обмена.
}