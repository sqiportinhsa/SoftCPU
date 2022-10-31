#ifndef ASSMBLR
#define ASSMBLR

#define DEBUG_ASS

#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

const int Ass_version = 3;
const int Max_cmd_len = 5;

#ifdef DEBUG_ASS
#define ASS_DEBUG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define ASS_DEBUG(format, ...)
#endif

typedef struct {
    char*      cmd_ptr;
    char*      val_ptr;
    int        cmd_len;
    char       cmd = 0;
    int        val = 0;
    char       reg = 0;
    size_t jump_to = 0;
} Command;

typedef struct {
    char*  ptr                = nullptr;
    int    nstring            = 0;
    int    len                = 0;
    size_t index_in_assembled = 0;
} Marker;

typedef struct {
    Command* commands;
    Marker*  markers;
    char*    code;
    size_t   amount_of_code_symbols = 0;
    int      amount_of_code_strings = 0;
    int      amount_of_markers      = 0;
} Ass;

typedef enum {
    NOT_REG = -1,
    MIN_REG = 0,
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
    MAX_REG = 5,
} Register;

typedef enum {
    NO_ASS_ERR         = 0,
    INCORRECT_CMD      = 1 << 1,
    INCORRECT_ARG      = 1 << 2,
    INCORRECT_REG      = 1 << 3,
    INCORRECT_VAL      = 1 << 4,
    UNIDENTIF_SYM      = 1 << 5,
    UNEXP_NULLPTR      = 1 << 6,
    INCR_BR_USAGE      = 1 << 7,
    MISSING_ARG        = 1 << 8,
    UNEXPEC_ARG        = 1 << 9,
    CANT_OPEN_FILE     = 1 << 10,
    SAME_MARKERS       = 1 << 11,
    INCORR_INDEX       = 1 << 12,
    JMP_TO_NONEXS_MARK = 1 << 13,
} AssErrors;

int assemble(Ass *ass);

int place_pointers(Ass *ass);

int verify_markers(const Marker *markers, int amount_of_markers);

#endif
