#ifndef PARSER_H
#define PARSER_H

#include "defines.h"
#include "shared_types.h"

void Parser_init(uint8_t *buff);
void Parser_work(Rcvd_cmd_t *cmd_ptr, uint32_t msg_size);

#endif