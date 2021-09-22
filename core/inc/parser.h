/**
 * @file parser.h
 * @author Андрей Белов (gd.triebkraft@gmail.com)
 * @brief Парсер команд принятых с терминала.
 * 
 *  Проверяет синтаксис входящих команд, валидность параметров.
 * 
 * @version 0.1
 * @date 2021-09-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef PARSER_H
#define PARSER_H

#include "defines.h"
#include "shared_types.h"

/**
 * @brief Инициализация модуля.
 * 
 * @param buff 
 */
void Parser_init(uint8_t *buff);

/**
 * @brief Разбор входящего буфера.
 * 
 * Результат разбора входного буфера будет помещён в указатель на команду,
 * которая передана в качестве одного из параметров.
 * 
 * @param Rcvd_cmd_t* cmd_ptr - указатель на экземпляр команды.
 * @param uint32_t msg_size - размер приянтого сообщения во входном буфере.
 */
void Parser_work(Rcvd_cmd_t *cmd_ptr, uint32_t msg_size);

#endif