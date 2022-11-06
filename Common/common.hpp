#ifndef COMMON
#define COMMON

const int Verification_const = 0xDEED;

#define DEF_CMD(name, val, ...) CMD_##name,

typedef enum {
    MIN_CMD = -1,
    #include "commands.hpp"
    MAX_CMD , //
    NOT_A_CMD,
} Commands;

#undef DEF_CMD

typedef enum {
    CMD1  =  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4), // cringe
    VAL  =  1 << 5,
    REG  =  1 << 6,
    RAM  =  1 << 7,
} Arg_bytes;

struct CMD {
    unsigned char cmd : 5 = 0;
    unsigned char val : 1 = 0;
    unsigned char reg : 1 = 0;
    unsigned char ram : 1 = 0;
};

#endif
