/**
    @file led_control.h
    @brief Заголовки функций модуля работы со светодиодом, основные параметры и
           определения новых типов данных
*/
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "defines.h"

#define LED_FULL_DUTY   100.0F //!< Макисмальный коэффициент заполнения ШИМ светодиода
#define LED_ZERO_DUTY   0.0F   //!< Минимальный коэффициент заполнения ШИМ светодиода

/**
    @brief Перечисление возможных режимов работы светодиода
*/
typedef enum
{
    LED_BLINK,                 ///< режим мигания
    LED_FADE                   ///< режим плавного изменения яркости
} Led_mode_e;

/**
    @brief Перечисление режимов плавного изменеия яркости свечения светодиода
*/
typedef enum
{
    FADE_ON,                   ///< Плавное разгорание светодиода
    FADE_OFF                   ///< Плавное затухание светодиода
} Fade_mode_e;

void Led_init(void);
void Led_update(void);
void Led_set_mode(Led_mode_e mode);
void Led_set_fade(Fade_mode_e mode, uint32_t time);
void Led_set_blink(uint32_t led_on_time, uint32_t led_off_time);

#endif