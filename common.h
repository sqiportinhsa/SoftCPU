#ifndef COMMON
#define COMMON

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

#endif
