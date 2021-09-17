#include "stm32l0xx.h"
#include "main.h"

// ====== Переменные ===========================================================
//! Переменная уменьшается каждое системное прерывание
volatile uint32_t systick;

/**
    @brief Инициализация микроконтроллера

    Инциализируется система тактирования, порты ввода вывода, UART, таймеры, DMA
*/
void Setup_MCU(void)
{
	// ====== Инцициализация переменных ========================================
	systick = 0;

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
    // Подключаю тактирование к timer2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    
    

}

/**
    @brief Точка входа в программу
*/
void main(void)
{
	Setup_MCU();



	while(1)
	{
		systick = 500;
        while(systick)
        {
            ;
        }
        GPIOB->BSRR |= GPIO_BSRR_BS_3;
        systick = 500;
        while(systick)
        {
            ;
        }
        GPIOB->BSRR |= GPIO_BSRR_BR_3;
	}
}

/**
    @brief Обработчик прерывания SysTic
*/
void SysTick_Handler(void)
{
	systick--;
}


