#include "main.h"
#include "led_control.h"
#include "usart_control.h"


// ====== Глобальные перменные =================================================
// Светодиод
Led_t led;

// Принятая команда
Rcvd_cmd_t cmd;

// ====== Строки ===============================================================
const char hello_str[] = "LED control panel.\nEnter command or 'h' for help.\n>";

const char help_str[] = "\nHalp string!\nHalp me, please!\n>";

const char stop_str[] = "\nSystem stopped.\nWait for reset.\n";

const char unknow_cmd_str[] = "\nUnknown command. Send 'h' to list all aviable commands.\n>";

const char param_err_str[] =  "\nWrong parameters\n";

/**
    @brief Инициализация микроконтроллера

    Инциализируется система тактирования, порты ввода/вывода, UART, таймеры, DMA
*/
void Setup_MCU(void)
{
    // ====== Инициализация переменных =========================================
    cmd.cmd = NO_CMD;
    cmd.param0 = 0;
    cmd.param1 = 0;

    led.mode = LED_BLINK;
    led.param0 = 500;
    led.param1 = 500;

	// ====== Настройка тактирования ===========================================
	// Тактирование от внутреннего генератора
	RCC->CR |= RCC_CR_HSION;								// 16 MHz

	// Жду когда заведётся генератор. В документации этот бит обозначен как
    // RCC_CR_HSI16RDYF, а в файле stm32l011xx.h из стандартного репозитория
    // этот бит обозначен как RCC_CR_HSIRDY. И тот и другой разворачиваются в
    // (1 << 2).
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
    RCC->CR &= ~(RCC_CR_MSION);

    // Обновляю переменную SystemCoreClock
    SystemCoreClockUpdate();

    // Настраиваю системные прерывания 1000 раз в секунду
    SysTick_Config(SystemCoreClock / 1000);

    // ====== Настройка GPIO ===================================================
    // Подключаю тактирование к GPIOB и GPIOA
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOAEN;

    // Конфигурирую пин 3 порта B в alternative_mode
    GPIOB->MODER &= ~GPIO_MODER_MODE3_0;

    // Выбираю как альтернативную функцию выход второго канала второго таймера
    GPIOB->AFR[0] |= 0x0002000;         // AF2

    // Конфигурирую пины 2 и 15 порта A в alternative_mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_0 | GPIO_MODER_MODE15_0);

    // Пин 2 порта A - USART2-TX
    GPIOA->AFR[0] |= 0x00000400;        //AF4

    // Пин 15 порта A - USART2-RX
    GPIOA->AFR[1] |= 0x40000000;        //AF4

    // ====== Настройка TIMER2 для работы с PWM ================================
    // Подключаю тактирование к timer2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Настройка прескаллера
    TIM2->PSC = 3200;
    TIM2->ARR = 100;

    // Коэффицициент заполнения по умолчанию [0:100]
    TIM2->CCR2 = 0;

    // Режим работы выходного канала PWM - выравнивание по фронту
    TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

    // Включаю выход второго канала второго таймера
    TIM2->CCER |= TIM_CCER_CC2E;

    // Запускаю счётчик
    TIM2->CR1 |= TIM_CR1_CEN;

    // ====== Настройка USART2 =================================================
    // Передача по USART производится через DMA, а приём в кольцевой буфер с
    // контролем символа <CR>

    // Подлючаю тактирование к USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Установка BAUDRATE
    USART2->BRR = F_CPU / USART_BAUDRATE;

    // На всякий случай отключаю USART на время конфигурации
    USART2->CR1 &= ~(USART_CR1_UE);

    // По дефолту настроены 8 bit, 1 stop bit, parity control disable
    // Остаётся включить приёмник, передатчик  и разрешить прерывание
    // приёмника.
    USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_TE |USART_CR1_RE;

    // Включение передачи через DMA
    USART2->CR3 |= USART_CR3_DMAT;

    // Включаю USART
    USART2->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART2_IRQn);

    // ====== Настройка DMA на передачу в USART2 ===============================
    // Передача в USART2 - 4 канал DMA
    // Включаю тактирование DMA
    RCC->AHBENR |= RCC_AHBENR_DMAEN;

    // К 4-му каналу DMA подключаю USART2_TX
    DMA1_CSELR->CSELR |= 0x00004000;

    // Установка источника mem->prph, автоинкремент источника, разрешение
    // прерывания по окончанию передачи
    DMA1_Channel4->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;

    // Установка адреса приёма данных
    DMA1_Channel4->CPAR = (uint32_t)&(USART2->TDR); // регистр передатчика  USART2

    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
}

/**
    @brief Точка входа в программу
*/
int main(void)
{
	Setup_MCU();
    Led_init(&led);
    Usart_init();

    Usart_send_str_DMA(hello_str, sizeof(hello_str) - 1);

 	while(1)
	{
        Led_update(&led);
        Usart_update(&cmd);

        switch(cmd.cmd)
        {
        case NO_CMD:
            break;

        case HELP_CMD:
            Usart_send_str_DMA(help_str, sizeof(help_str) - 1);

        case BLINK_CMD:
            led.mode = LED_BLINK;
            led.param0 = cmd.param0;
            led.param1 = cmd.param1;
            break;

        case FADE_ON_CMD:
            led.mode = LED_FADE_ON;
            led.param0 = cmd.param0;
            led.param1 = cmd.param1;
            break;

        case FADE_OFF_CMD:
            led.mode = LED_FADE_OFF;
            led.param0 = cmd.param0;
            led.param1 = cmd.param1;
            break;

        case STOP_CMD:
            Usart_send_str_DMA(stop_str, sizeof(stop_str) - 1);
            break;

        case ERROR_CMD:
            Usart_send_str_DMA(unknow_cmd_str, sizeof(unknow_cmd_str) - 1);
            break;

        case ERROR_PARAM_CMD:
            Usart_send_str_DMA(param_err_str, sizeof(param_err_str) - 1);
            break;

        default:
            break;
        }

        cmd.cmd = NO_CMD;
	}

    return 0;
}


