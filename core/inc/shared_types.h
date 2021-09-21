#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include "stm32l0xx.h"

typedef enum
{
    NO_CMD,
    HELP_CMD,
    BLINK_CMD,
    FADE_ON_CMD,
    FADE_OFF_CMD,
    STOP_CMD,
    ERROR_CMD
} Command_e;

typedef struct
{
    Command_e cmd;
    uint32_t param0;
    uint32_t param1;
} Rcvd_cmd_t;

#endif