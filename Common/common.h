#ifndef COMMON
#define COMMON

const int Verification_const = 0xDEED;

typedef enum {
    MIN_CMD   = -1,
    HLT_CMD   =  0,
    PUSH_CMD  =  1,
    ADD_CMD   =  2,
    MUL_CMD   =  3,
    SUB_CMD   =  4,
    DIV_CMD   =  5,
    OUT_CMD   =  6,
    IN_CMD    =  7,
    MAX_CMD   =  8,
    NOT_A_CMD =  9,
} Commands;

typedef enum {
    CMD = 1 | (1 << 1) | (1 << 2) | (1 << 3),
    VAL = 1 << 4,
    REG = 1 << 5,
    RAM = 1 << 6,
} Arg_bytes;

#endif
