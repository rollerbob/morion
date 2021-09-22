/**
 * @file parser.c
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Парсер входного буфера.
 * 
 * Разбирает принятые от терминала пользователя данные. Находит валдные команды
 * и параметры.
 * 
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "parser.h"

// Шаблоны команд
uint8_t help_rqst_str[] = "h\n";
uint8_t blink_rqst_str[] = "blink ";
uint8_t fade_rqst_str[] = "fade %";
uint8_t fade_od_rqst_str[] = "fade on ";
uint8_t fade_off_rqst_str[] = "fade off ";
uint8_t stop_rqst_str[] = "stop\n";

// Массив указателей на эти шаблоны
uint8_t *requests[] = {help_rqst_str, blink_rqst_str, fade_rqst_str, \
                        fade_od_rqst_str, fade_off_rqst_str, stop_rqst_str};

static uint8_t *_in_buff;

// ====== Локальные функции ====================================================

/**
 * @brief Сравнивает две строки и возвращает позицию с которой в первой строке
 * начинается вторая строка или 0xFFFFFFFF если в первой строке вторая строка не
 * найдена.
 *
 * @param str1 указатель на первую строку
 * @param size1 дилна первой строки
 * @param str2 указатель на вторую строку
 * @param size2 длина второй строки
 * @return uint32_t позиция с которой в первой строке начинается вторая
 */
uint32_t _Str_cmpr(uint8_t *str1, uint32_t size1, uint8_t *str2, uint32_t size2);

/**
 * @brief Поиск параметров команды во входящем буфере. Ищуться все вхождения
 * разделённые "%", затем эти вхождения проверяются на валидность и сохраняются
 * внутри объекта команды.
 *
 * @param buff буфер в котором происходит поиск.
 * @param buff_size размер буфера.
 * @param cmd указатель на команду, в которую сохраняются параметры
 * @param amount сколько групп параметров искать.
 * @return uint32_t статус поиска. 0 - ошибка во входных параметрах, 1 - входные
 * параметры успешно установлены в принятой команде.
 */
uint32_t _Str_search_param(uint8_t *buff, uint32_t buff_size, Rcvd_cmd_t *cmd, uint32_t amount);


// =============================================================================

void Parser_init(uint8_t *buff)
{
    _in_buff = buff;
}

void Parser_work(Rcvd_cmd_t *cmd,  uint32_t msg_size)
{
    cmd->cmd = ERROR_CMD;
    for (uint32_t rqsts_itr = 0; rqsts_itr < sizeof(requests) / sizeof(char*); rqsts_itr++)
    {
        uint8_t *rqst = requests[rqsts_itr];

        // Длина буфера приёма известна. нужно найти длину длину искомой подстроки
        uint32_t rqst_size = 0;
        while (rqst[rqst_size])
        {
            rqst_size++;
        }

        // Теперь ищу во входном буфере подстроку с куском предпологаемой команды
        uint32_t pos = _Str_cmpr(_in_buff, msg_size, rqst, rqst_size);

        // Если найдена любая команда на 0 позиции во входном буфере, то теперь
        // нужно уточнить её параметры.
        if (pos == 0)
        {
            cmd->cmd = (Command_e)rqsts_itr;
            break;
        }
    }

    // Есть три вида комманд:
    // 1) Команда без параметров - h, stop
    // 2) Команда с одним параметром - fade on, fade off
    // 3) Команада с двуми параметрами - blink
    // Искать параметры нужно только для вариантов 2 и 3
    switch (cmd->cmd)
    {
        case BLINK_CMD:
        case FADE_CMD:
            if (!_Str_search_param(_in_buff, msg_size, cmd, 2))
                cmd->cmd = ERROR_PARAM_CMD;
            break;

        case FADE_ON_CMD:
        case FADE_OFF_CMD:
            if (!_Str_search_param(_in_buff, msg_size, cmd, 1))
                cmd->cmd = ERROR_PARAM_CMD;
            break;

        default:
            break;
    }


}

uint32_t _Str_cmpr(uint8_t *str1, uint32_t size1, uint8_t *str2, uint32_t size2)
{
    uint32_t ret = ~0;

    if (size1 < size2)
        return ret;

    uint32_t i = 0;
    while (i <= (size1 - size2))
    {
        uint32_t j = 0;
        while (str1[i] == str2[j])
        {
            j++;
            i++;
            if (j == size2) return i - size2;
        }
        i++;
    }

    return ret;
}

uint32_t _Str_search_param(uint8_t *buff, uint32_t buff_size, Rcvd_cmd_t *cmd, uint32_t amount)
{
    if (amount > MAX_CMD_PARAM_AMOUNT) amount = MAX_CMD_PARAM_AMOUNT;

    uint32_t div_entrys[MAX_CMD_PARAM_AMOUNT * 2];
    uint32_t entrys_cntr = 0;

    // Нахожу все позиции с символом '%'
    for (uint32_t i = 0; i < buff_size; i++)
    {
        if (buff[i] == '%') div_entrys[entrys_cntr++] = i;
    }


    // прверяю сколько символов нашлось
    if (entrys_cntr !=  amount * 2)
    {
        // Если не совпало с расчётным, значит ошибка в передаче параметров.
       return 0;
    }

    // Проверяю что группы параметров стоят на своих местах.
    // Открывающий '%' точно стоит на своём месте, так как по этому шаблону
    // проверяется сама команда. теперь надо проверить что закрывающий '%' стоит
    // на предпоследней позциции в буфере

    if (div_entrys[amount * 2 - 1] != buff_size - 2)
        return 0;

    // Проверка что между вторым и третьим '%' ровно один пробел
    if (amount == 2)
    {
        // Шаблон
        uint8_t templt[] = "% %";
        uint32_t check = 0;
        uint32_t start_check = div_entrys[1];

        for (uint32_t i = 0; i < 3; i++)
            check += buff[start_check + i] ^ templt[i];

        if (check != 0)
            return 0;
    }

    // Теперь нужно проверить значения между '%' на валидность. Валидным
    // считается последовательность символов из дипазона ['0'(0x30) : '9'(0x39)]

    uint32_t params[] = {0, 0};
    for (uint32_t a = 0; a < amount; a++ )
    {
        uint32_t param = 0;
        uint32_t factor = 1;

        for (uint32_t i = div_entrys[MAX_CMD_PARAM_AMOUNT * a + 1] - 1; i > div_entrys[MAX_CMD_PARAM_AMOUNT * a]; i--)
        {
            if (buff[i] < 0x30 || buff[i] > 0x39)
                return 0;

            param += (buff[i] - 0x30) * factor;
            factor *= 10;
        }

        params[a] = param;
    }

    cmd->param0 = params[0];
    cmd->param1 = params[1];

    return 1;
}