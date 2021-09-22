/**
 * @file shared_types.h
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Определение общих типов данных, используемых во всех модулях
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include "stm32l0xx.h"

/**
 * @brief Перечисление возможных команд, принимаемых от терминала.
 * 
 */
typedef enum
{
    HELP_CMD = 0,       ///< Вывод помощи в терминал
    BLINK_CMD,          ///< Мигание светодиода
    FADE_CMD,           ///< Плавное мигание светодиода
    FADE_ON_CMD,        ///< Плавное разгорание светодиода
    FADE_OFF_CMD,       ///< Плавное затухание светодиода
    STOP_CMD,           ///< Остановка программы, тест watchdog
    NO_CMD,             ///< Пустая команда
    ERROR_CMD,          ///< Нераспознаная команда
    ERROR_PARAM_CMD     ///< Ошибка в параметрах команды
} Command_e;

/**
 * @brief Структура с описанием объекта команды.
 * 
 * Включает в себя имя принятой команды и её параметры.
 * 
 */
typedef struct
{
    Command_e cmd;      ///< Принятая команда
    uint32_t param0;    ///< Первый параметр
    uint32_t param1;    ///< Второй параметр
} Rcvd_cmd_t;

#endif