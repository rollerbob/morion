/**
 * @file defines.h
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Общие настройки программы
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef DEFINES_H
#define DEFINES_H

/// Рабочая частота SYSCLK
#define F_CPU                 32000000UL
/// Скорость передачи данных по последовательному порту
#define USART_BAUDRATE           56000UL

/// Макисмальное количество групп параметров в принимаемой команде
#define  MAX_CMD_PARAM_AMOUNT          2

#endif