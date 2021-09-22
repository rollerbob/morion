#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include "stm32l0xx.h"

typedef enum
{
    HELP_CMD = 0,
    BLINK_CMD,
    FADE_ON_CMD,
    FADE_OFF_CMD,
    STOP_CMD,
    NO_CMD,
    ERROR_CMD,
    ERROR_PARAM_CMD
} Command_e;

typedef struct
{
    Command_e cmd;
    uint32_t param0;
    uint32_t param1;
} Rcvd_cmd_t;

#endif