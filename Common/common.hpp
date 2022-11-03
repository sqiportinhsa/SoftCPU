#ifndef COMMON
#define COMMON

const int Verification_const = 0xDEED;

#define DEF_CMD(name, val, ...) CMD_##name = val,

typedef enum {
    MIN_CMD = -1,
    #include "commands.hpp"
    MAX_CMD   = 10,
    NOT_A_CMD =  11,
} Commands;

#undef DEF_CMD

typedef enum {
    CMD  =  1 | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    VAL  =  1 << 5,
    REG  =  1 << 6,
    RAM  =  1 << 7,
} Arg_bytes;

#endif
