#include "stm32l0xx.h"
#include "main.h"

// ====== Переменные ==========================================================
volatile uint32_t systick;

void Setup_MCU(void)
{
	// ====== Инцициализация переменных =======================================
	systick = 0;

	// ====== Настройка тактирования ==========================================

	// Установка задержки доступа к NVM
	FLASH->ACR |= FLASH_ACR_LATENCY;						// 1 цикл

	// Тактирование от внутреннего генератора
	RCC->CR |= RCC_CR_HSION;								// 16 MHz

	// Жду когда заведётся генератор
	while (!(READ_BIT(RCC->CR, RCC_CR_HSIRDY)))
	{
		;
	}

	// Устанавливаю делитель и множитель PLL
	RCC->CFGR |= RCC_CFGR_PLLMUL4 | RCC_CFGR_PLLDIV2;		// 16 * 4 / 2 = 32 MHz


}


int main(void)
{
	Setup_MCU();

	while(1)
	{
		;
	}

	return 0;
}

// Обработчик прерывания SysTick
void SysTick_handler(void)
{
	systick--;
}


