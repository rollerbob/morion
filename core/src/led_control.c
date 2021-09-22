/**
 * @file led_control.c
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Реализация функций работы со светодиодом.
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "led_control.h"

// Переменная уменьшается каждое системное прерывание
volatile static uint32_t _systick;

// Значение счётчика _now_systick при предыдущем вызове функции Led_update()
static uint32_t _last_systick;

// Мгновенное состояние коэффициента заполнения ШИМ светодиода
static float _duty;

// В режиме затухания светодиода шаг с которым уменьшается коэффициент
// заполнения ШИМ светодиода при каждом изменении счётчика системных прерываний
static float _duty_step;

static Led_mode_e _last_led_mode;

float static Calc_duty_step(uint32_t time)
{
    // Защита от передачи 0 в качестве параметра времени изменения. Так как
    if (!time) time++;
    // вот здесь происходит расчёт шага изменения коэффициента заполнения ШИМ,
    // при расчёте которого, происходит деление на time.
    return (LED_FULL_DUTY - LED_ZERO_DUTY) / time;
}

void Led_init(Led_t *led)
{
    _duty = 0;
    _duty_step = 0.0F;
    _last_led_mode = LED_IDLE;

    led->mode = LED_BLINK;
    led->param0 = 500;
    led->param1 = 500;
}

void Led_update(Led_t *led)
{
    // Сохраняется текущее значение системного счётчика, чтобы вдруг внезапно он
    // не изменился прямо во время обработки, сравнения, etc.
    uint32_t _now_systick = _systick;

    // В зависимости от режима работы светодиода обрабатывется его текущее
    //  состояние.
    switch (led->mode)
    {
    // Режим мигания. Реализовано тоже через ШИМ. Просто устанавливается либо
    // максимальный коэффициент заполнения, либо минимальный.
    case LED_BLINK:
        // Если это первый вызов после смены режима, то нужна инициализация
        if (_last_led_mode != LED_BLINK)
        {
            _last_led_mode = LED_BLINK;
            _duty = LED_FULL_DUTY;
            _systick = led->param0;
        }

        // Если системный счётчик дотикал до нуля, нужно сменить состояние
        // светодиода на противоположное и установить новое значение счётчика.
        if (!_now_systick)
        {
            if (_duty == LED_FULL_DUTY)
            {
                _duty = LED_ZERO_DUTY;
                _systick = led->param1;
            }
            else
            {
                _duty = LED_FULL_DUTY;
                _systick = led->param0;
            }

            // Установка нового коэффициента заполнения ШИМ.
            TIM2->CCR2 = (uint16_t)(_duty);
        }
        break;

    // Режим плавного затухания/разгорания
    case LED_FADE_ON:
        // Если это первый вызов после смены режима, то нужна инициализация
        if (_last_led_mode != LED_FADE_ON)
        {
            _last_led_mode = LED_FADE_ON;
            _duty = LED_ZERO_DUTY;
            _duty_step = Calc_duty_step(led->param0);
            _systick = led->param0;
        }

        if (_now_systick != _last_systick)
        {
            // Коэффициент заполнения увеличивается на расчитанный шаг.
            _duty += _duty_step;
            // Ограничивается максимальный коэффициент.
            if (_duty > LED_FULL_DUTY)
            { 
                _duty = LED_FULL_DUTY;
                _last_led_mode = LED_IDLE;
                led->mode = LED_IDLE;
            }

            // Установка нового коэффициента
            TIM2->CCR2 = (uint16_t)(_duty);
        }
        break;

    case LED_FADE_OFF:
        // Если это первый вызов после смены режима, то нужна инициализация
        if (_last_led_mode != LED_FADE_OFF)
        {
            _last_led_mode = LED_FADE_OFF;
            _duty = LED_FULL_DUTY;
            _duty_step = Calc_duty_step(led->param0);
            _systick = led->param0;
        }

        if (_now_systick != _last_systick)
        {
            // Коэффициент заполнения уменьшается на расчитанный шаг.
            _duty -= _duty_step;
            // Ограничивается минимальный коэффициент.
            if (_duty < LED_ZERO_DUTY)
            { 
                _duty = LED_ZERO_DUTY;
                _last_led_mode = LED_IDLE;
                led->mode = LED_IDLE;
            }

            // Установка нового коэффициента
            TIM2->CCR2 = (uint16_t)(_duty);
        }
        break;

    default:
        break;
    }

    _last_systick = _now_systick;
}

void SysTick_Handler(void)
{
	_systick--;
}
