/**
 * @file led_control.h
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Заголовки функций модуля работы со светодиодом, основные параметры и
 * определения новых типов данных
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "defines.h"
#include "shared_types.h"

#define LED_FULL_DUTY   100.0F ///< Макисмальный коэффициент заполнения ШИМ светодиода
#define LED_ZERO_DUTY   0.0F   ///< Минимальный коэффициент заполнения ШИМ светодиода

/**
    @brief Перечисление возможных режимов работы светодиода
*/
typedef enum
{
    LED_IDLE,                           ///< режим простоя
    LED_BLINK,                          ///< режим мигания
    LED_FADE,                           ///< режим плавного мигания
    LED_FADE_ON,                        ///< режим плавного розжига
    LED_FADE_OFF                        ///< режим плавного затухания
} Led_mode_e;

/**
 * @brief Описание объекта светодиода.
 * 
 * Включает в себя режим работы светодиода, временные параметры для разных режимов.
 * 
 */
typedef struct led_t
{
    Led_mode_e mode;                    ///< режим работы светодиода
    uint32_t param0;                    ///< первый параметр (время включения, время розжига/время затухания)
    uint32_t param1;                    ///< второй параметр
} Led_t;

/**
 * @brief Инициализация светодиода.
 * 
 * @param Led_t* led - Указатель на экземпляр светодиода
 */
void Led_init(Led_t *led);

/**
 * @brief Обновление состояния светодиода
 * 
 * @param Led_t* led - Указатель на экземпляр светодиода
 */
void Led_update(Led_t *led);

#endif