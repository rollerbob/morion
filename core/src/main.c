#include "defines.h"
#include "main.h"
#include "led_control.h"

/**
    @brief Инициализация микроконтроллера

    Инциализируется система тактирования, порты ввода вывода, UART, таймеры, DMA
*/
void Setup_MCU(void)
{
	// ====== Настройка тактирования ===========================================
	// Тактирование от внутреннего генератора
	RCC->CR |= RCC_CR_HSION;								// 16 MHz

	// Жду когда заведётся генератор
	while (!(READ_BIT(RCC->CR, RCC_CR_HSIRDY)))
	{
		;
	}
    
	// Устанавливаю делитель и множитель PLL
	RCC->CFGR |= RCC_CFGR_PLLMUL4 | RCC_CFGR_PLLDIV2;		// 16 * 4 / 2 = 32 MHz
  
    // Включаю PLL
    RCC->CR |= RCC_CR_PLLON;
    
    // Жду пока заведётся PLL
    while (!(READ_BIT(RCC->CR, RCC_CR_PLLRDY)))
    {
        ;
    }
    
    // Установка задержки доступа к NVM
	FLASH->ACR |= FLASH_ACR_LATENCY;						// 1 цикл

    // Установка источника SYSCLK
    RCC->CFGR |= RCC_CFGR_SW;                               // PLL

    // Жду когда PLL отрапортует о готовности
    while (!(RCC->CFGR & RCC_CFGR_SWS_0) && !(RCC->CFGR & RCC_CFGR_SWS_1))
    {
        ;
    }

    // Выключаю MSI
    RCC->CR &=~ RCC_CR_MSION;

    // Обновляю переменную SystemCoreClock
    SystemCoreClockUpdate();

    // Настраиваю системные прерывания 1000 раз в секунду
    SysTick_Config(SystemCoreClock / 1000);

    // Настройка timer2 на выход PWM на пине 3 порта B. 26-я нога
    // Подключаю тактирование к GPIOB
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    // Конфигурирую пин 3 порта B в alternative_mode
    GPIOB->MODER &= ~GPIO_MODER_MODE3_0;

    // Выбираю как альтернативную функцию выход второго канала второго таймера
    GPIOB->AFR[0] |= 0x0002000;                             // AF2

    // Подключаю тактирование к timer2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Настройка прескаллера
    TIM2->PSC = 3200;
    TIM2->ARR = 100;
    
    // Коэффицициент заполнения по умолчанию [0:100]
    TIM2->CCR2 = 0;

    // Режим работы выходного канала PWM - выравнивание по фронту
    TIM2->CCMR1 |= 0x6000;

    // Включаю выход второго канала второго таймера
    TIM2->CCER |= TIM_CCER_CC2E;

    // Запускаю счётчик
    TIM2->CR1 |= TIM_CR1_CEN;

    
    

}

/**
    @brief Точка входа в программу
*/
void main(void)
{
	Setup_MCU();
    Led_init();
    Led_set_mode(LED_FADE);
    Led_set_fade(FADE_OFF, 5000);

	while(1)
	{
        Led_update();
	}
}


