#ifndef ASSMBLR
#define ASSMBLR

#include "../Stack/stack.h"
#include "../Stack/stack_logs.h"
#include "../Stack/stack_verification.h"

#include "../Common/file_reading.hpp"

const int Ass_version = 3;
const int Max_cmd_len = 5;

static const char *default_input_filename  = "input.asm";
static const char *default_output_filename = "output.bin";

#ifdef DEBUG_ASS
#define ASS_DEBUG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define ASS_DEBUG(format, ...)
#endif

#define RETURN_IF(errors)                                                              \
        if (errors != 0) {                                                             \
            fprintf(stderr, "Error: can't compile code. Stop. Errors: %d.\n", errors); \
            return -1;                                                                 \
        }

struct Command {
    char*      cmd_ptr = nullptr;
    char*      val_ptr = nullptr;
    int        cmd_len = 0;
    char       cmd = 0;
    int        val = 0;
    char       reg = 0;
};

struct Marker {
    char*                          ptr = nullptr;
    unsigned int                   len = 0;
    unsigned int    index_in_assembled = 0;
};

struct Ass {
    Command*     commands;
    Marker*      markers;
    char*        code;
    size_t       amount_of_code_symbols;
    int          amount_of_code_strings;
    int          amount_of_commands;
    int          amount_of_markers;
    const char*  input_name;
    const char*  output_name;
};

enum Register {
    NOT_REG = -1,
    MIN_REG = 0,
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
    MAX_REG = 5,
};

enum AssErrors {
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
};

CLArgs cmd_line_args(int argc, const char **argv);

int ass_constructor(Ass *ass, CLArgs *args);

int assemble(Ass *ass);

void free_ass(Ass *ass);

#endif
